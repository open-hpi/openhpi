/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Sean Dague <http://dague.net/sean>
 *     Renier Morales <renierm@users.sourceforge.net>
 *     Racing Guo <racing.guo@intel.com>
 */

#include <string.h>

#include <oh_event.h>
#include <oh_config.h>
#include <oh_handler.h>
#include <oh_plugin.h>
#include <oh_domain.h>
#include <oh_session.h>
#include <oh_alarm.h>
#include <oh_utils.h>
#include <oh_error.h>

struct _oh_evt_queue {
        GAsyncQueue *q;
};
oh_evt_queue oh_process_q = { .q = NULL };

extern GMutex *oh_event_thread_mutex;

/*
 *  The following is required to set up the thread state for
 *  the use of event async queues.  This is true even if we aren't
 *  using live threads.
 */
int oh_event_init()
{
        trace("Setting up event processing queue");
        if (!oh_process_q.q) oh_process_q.q = g_async_queue_new();
        if (oh_process_q.q) {
                trace("Set up processing queue");
                return 1;
        } else {
                dbg("Failed to allocate processing queue");
                return 0;
        }
}

void oh_evt_queue_push(oh_evt_queue *equeue, gpointer data)
{
        g_async_queue_push(equeue->q, data);
        return;
}

void oh_event_free(struct oh_event *e, int only_rdrs)
{
	if (e) {
		if (e->rdrs) {
			GSList *node = NULL;
			for (node = e->rdrs; node; node = node->next) {
				g_free(node->data);
			}
			g_slist_free(e->rdrs);
		}
		if (!only_rdrs) g_free(e);
	}
}

struct oh_event *oh_dup_event(struct oh_event *old_event)
{
	GSList *node = NULL;
	struct oh_event *e = NULL;

	if (!old_event) return NULL;

	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	*e = *old_event;
	e->rdrs = NULL;
	for (node = old_event->rdrs; node; node = node->next) {
		e->rdrs = g_slist_append(e->rdrs, g_memdup(node->data,
							   sizeof(SaHpiRdrT)));
	}

	return e;
}

/*
 *  Event processing is split up into 2 stages
 *  1. Harvesting of the events
 *  2. Processing of the events into: Domain Event Log, Alarm Table,
 *  Session queues, Resource Precense Table.
 *
 */

static SaErrorT harvest_events_for_handler(struct oh_handler *h)
{
        SaErrorT error = SA_OK;

        do {
                error = h->abi->get_event(h->hnd);
                if (error < 1) {
                        trace("Handler is out of Events");
                }/* else if (!oh_domain_served_by_handler(h->id, event.did)) {
                        dbg("Handler %d sends event %s to wrong domain %d",
                            h->id,
                            oh_lookup_eventtype(event.event.EventType),
                            event.did);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                } else {
                        trace("Found event for handler %u", h->id);
                        e = oh_dup_event(&event);
			oh_event_free(&event, TRUE);
                        e->hid = h->id;
                        g_async_queue_push(oh_process_q.q, e);
                }*/
        } while (error > 0);

        return SA_OK;
}

SaErrorT oh_harvest_events()
{
        SaErrorT error = SA_ERR_HPI_ERROR;
        unsigned int hid = 0, next_hid;
        struct oh_handler *h = NULL;

        oh_getnext_handler_id(hid, &next_hid);
        while (next_hid) {
                trace("harvesting for %d", next_hid);
                hid = next_hid;

                h = oh_get_handler(hid);
                if (!h) {
                        dbg("No such handler %d", hid);
                        break;
                }
                /*
                 * Here we want to record an error unless there is
                 * at least one harvest_events_for_handler that
                 * finished with SA_OK. (RM 1/6/2005)
                 */
                if (harvest_events_for_handler(h) == SA_OK && error)
                        error = SA_OK;

                oh_release_handler(h);

                oh_getnext_handler_id(hid, &next_hid);
        }

        return error;
}

static SaErrorT oh_add_event_to_del(SaHpiDomainIdT did, struct oh_event *e)
{
        struct oh_global_param param = { .type = OPENHPI_LOG_ON_SEV };
        struct oh_domain *d = NULL;
        SaErrorT rv = SA_OK;
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];

        oh_get_global_param(&param);

        /* Events get logged in DEL if they are of high enough severity */
        if (e->event.EventType == SAHPI_ET_USER ||
            e->event.Severity <= param.u.log_on_sev) {
                param.type = OPENHPI_DEL_SAVE;
                oh_get_global_param(&param);
                d = oh_get_domain(did);

                if (d) {
                	SaHpiRdrT *rdr = (e->rdrs) ? e->rdrs->data : NULL;
                	SaHpiRptEntryT *rpte =
                		(e->resource.ResourceCapabilities) ?
                			&e->resource : NULL;
                        rv = oh_el_append(d->del, &e->event, rdr, rpte);
                        if (param.u.del_save) {
                                param.type = OPENHPI_VARPATH;
                                oh_get_global_param(&param);
                                snprintf(del_filepath,
                                         SAHPI_MAX_TEXT_BUFFER_LENGTH*2,
                                         "%s/del.%u", param.u.varpath, did);
                                oh_el_map_to_file(d->del, del_filepath);
                        }
                        oh_release_domain(d);
                } else {
                        rv = SA_ERR_HPI_ERROR;
                }
        }
        return rv;
}

static int process_hpi_event(struct oh_event *full_event)
{
        int i;
        GArray *sessions = NULL;
        SaHpiSessionIdT sid;
        struct oh_domain *d = NULL;
        SaHpiEventT *event = NULL;
        SaHpiRptEntryT *resource = NULL;
        SaHpiRdrT *rdr = NULL;

        if (!full_event) {
        	dbg("Event to process is NULL!");
        	return -1;
        }

        event = &full_event->event;
        resource = &full_event->resource;
        rdr = (full_event->rdrs) ? full_event->rdrs->data : NULL;

        /* We take the domain lock for the whole function here */
        d = oh_get_domain(full_event->did);
        if (!d) {
                dbg("Domain %d doesn't exist", full_event->did);
                return -1;
        }

        if (event->EventType == SAHPI_ET_USER) {
                resource->ResourceCapabilities = 0;
                if (rdr) rdr->RdrType = SAHPI_NO_RECORD;
        }

        oh_add_event_to_del(d->id, full_event);
        trace("Added event to EL");

        /*
         * Here is the SESSION MULTIPLEXING code
         */

        sessions = oh_list_sessions(full_event->did);
        trace("Got session list for domain %d", full_event->did);

        /* Drop events if there are no sessions open to receive them.
         */
        if (sessions->len < 1) {
                g_array_free(sessions, TRUE);
                oh_release_domain(d);
                dbg("No sessions open for event's domain %d", full_event->did);
                dbg("Dropping hpi_event.");
                return 0;
        }

        /* multiplex event to the appropriate sessions */
        for (i = 0; i < sessions->len; i++) {
                SaHpiBoolT is_subscribed = SAHPI_FALSE;
                sid = g_array_index(sessions, SaHpiSessionIdT, i);
                oh_get_session_subscription(sid, &is_subscribed);
                if (is_subscribed) {
                        oh_queue_session_event(sid, full_event);
                }
        }
        g_array_free(sessions, TRUE);
        trace("done multiplexing event into sessions");

        oh_release_domain(d);
        trace("done oh_release_domain");

        return 0;
}

static int process_resource_event(struct oh_event *e)
{
        RPTable *rpt = NULL;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *exists = NULL;
        unsigned int *hidp = NULL;
        SaErrorT error = SA_OK;
        SaHpiResourceEventTypeT *retype = NULL;
        SaHpiHsStateT state = SAHPI_HS_STATE_NOT_PRESENT;
        SaHpiBoolT process_hpi = TRUE;

        if (!e) { dbg("Got NULL event"); return -1; }

        d = oh_get_domain(e->did);
        if (!d) {
                dbg("Domain %d doesn't exist", e->did);
                return -1;
        }

	rpt = &(d->rpt);
	exists = oh_get_resource_by_id(rpt, e->resource.ResourceId);

	if (e->event.EventType == SAHPI_ET_RESOURCE) {
		retype = &e->event.EventDataUnion.ResourceEvent.ResourceEventType;
		if (*retype != SAHPI_RESE_RESOURCE_FAILURE) {
			/* If previously failed, set EventT to RESTORED */
			if (exists && exists->ResourceFailed) {
				*retype = SAHPI_RESE_RESOURCE_RESTORED;
			} else if (exists &&
				   !exists->ResourceFailed) {
				process_hpi = FALSE;
			}
			e->resource.ResourceFailed = SAHPI_FALSE;
		} else {
			e->resource.ResourceFailed = SAHPI_TRUE;
		}
	} else if (e->event.EventType == SAHPI_ET_HOTSWAP) {
		state = e->event.EventDataUnion.HotSwapEvent.HotSwapState;
		if (state == SAHPI_HS_STATE_NOT_PRESENT) {
			oh_remove_resource(rpt, e->resource.ResourceId);
		}
	} else {
		dbg("Expected a resource or hotswap event.");
		oh_release_domain(d);
		return -1;
	}

	if (e->event.EventType == SAHPI_ET_RESOURCE ||
	    (e->event.EventType == SAHPI_ET_HOTSWAP &&
	     state != SAHPI_HS_STATE_NOT_PRESENT)) {
        	hidp = g_malloc0(sizeof(unsigned int));
		*hidp = e->hid;
		error = oh_add_resource(rpt, &e->resource,
					hidp, FREE_RPT_DATA);
		if (error == SA_OK && !exists) {
			GSList *node = NULL;
			for (node = e->rdrs; node; node = node->next) {
				SaHpiRdrT *rdr = node->data;
				oh_add_rdr(rpt, e->resource.ResourceId,
					   rdr, NULL, 0);
			}
		}
	}

	if (process_hpi) process_hpi_event(e);
        oh_release_domain(d);

        return 0;
}

SaErrorT oh_process_events()
{
        struct oh_event *e;

        while ((e = g_async_queue_pop(oh_process_q.q)) != NULL) {

                switch (e->event.EventType) {
                case SAHPI_ET_RESOURCE:
                        trace("Event Type = Resource");
                        if (e->resource.ResourceCapabilities
                            & SAHPI_CAPABILITY_FRU) {
				dbg("Invalid event. Resource in resource event "
				    "has FRU capability. Dropping.");
			} else {
                        	process_resource_event(e);
			}
                        break;
                case SAHPI_ET_HOTSWAP:
                        trace("Event Type = Hotswap");
                        if (!(e->resource.ResourceCapabilities
                            & SAHPI_CAPABILITY_FRU)) {
				dbg("Invalid event. Resource in hotswap event "
				    "has no FRU capability. Dropping.");
			} else {
                        	process_resource_event(e);
			}
                        break;
                case SAHPI_ET_SENSOR:
                case SAHPI_ET_SENSOR_ENABLE_CHANGE:
                case SAHPI_ET_WATCHDOG:
                case SAHPI_ET_OEM:
                case SAHPI_ET_USER:
                case SAHPI_ET_HPI_SW:
                case SAHPI_ET_DOMAIN:
                        trace("Event Type = %s",
                              oh_lookup_eventtype(e->event.EventType));
                        process_hpi_event(e);
                        break;
                default:
                        dbg("Event Type = Unknown Event");
                }
                oh_detect_event_alarm(e);
                oh_event_free(e, FALSE);
	}
        /* Should never get here */
	return SA_ERR_HPI_ERROR;
}

