 /*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003, 2005
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oh_event.h>
#include <openhpi.h>

#define OH_THREAD_SLEEP_TIME 2 * G_USEC_PER_SEC

static gboolean oh_is_threaded = FALSE;
GAsyncQueue *oh_process_q = NULL;
GCond *oh_thread_wait = NULL;
GThread *oh_event_thread = NULL;
GError *oh_event_thread_error = NULL;
GMutex *oh_thread_mutex = NULL;

gboolean oh_run_threaded()
{
        return oh_is_threaded;
}

static gpointer oh_event_thread_loop(gpointer data)
{
        GTimeVal time;
        SaErrorT rv;

	g_mutex_lock(oh_thread_mutex);
        while(oh_run_threaded()) {
		trace("Thread Harvesting events");
                rv = oh_harvest_events();
                if(rv != SA_OK) {
                        trace("Error on harvest of events.");
                }
                
		trace("Thread processing events");
                rv = oh_process_events();
                if(rv != SA_OK) {
                        trace("Error on processing of events, aborting");
                }
                
                trace("Thread processing hotswap");
                process_hotswap_policy();
                
                g_get_current_time(&time);
                g_time_val_add(&time, OH_THREAD_SLEEP_TIME);

                trace("Going to sleep");
                if (g_cond_timed_wait(oh_thread_wait, oh_thread_mutex, &time))
                        trace("SIGNALED: Got signal from plugin");
                else
                        trace("TIMEDOUT: Woke up, am looping again");
        }
	g_mutex_unlock(oh_thread_mutex);
        g_thread_exit(0);
        return data;
}

/*
 *  The following is required to set up the thread state for
 *  the use of async queues.  This is true even if we aren't
 *  using live threads.
 */
int oh_event_init()
{
        trace("Attempting to init event");
        if (!g_thread_supported()) {
                trace("Initializing thread support");
                g_thread_init(NULL);
        } else {
                trace("Already supporting threads");
        }
        trace("Setting up event processing queue");
        oh_process_q = g_async_queue_new();
        if(oh_process_q) {
                return 1;
        } else {
                return 0;
        }
}

int oh_start_event_thread()
{
        struct oh_global_param threaded_param = { .type = OPENHPI_THREADED };

        oh_get_global_param(&threaded_param);
        if (threaded_param.u.threaded) {
                trace("Starting event thread");
                oh_is_threaded = TRUE;
                oh_thread_wait = g_cond_new();
                oh_thread_mutex = g_mutex_new();
                oh_event_thread = g_thread_create(oh_event_thread_loop,
                                                  NULL, FALSE, &oh_event_thread_error);
        }

        return 1;
}

int oh_event_final()
{
        g_async_queue_unref(oh_process_q);
        if(oh_run_threaded()) {
                g_mutex_free(oh_thread_mutex);
                g_cond_free(oh_thread_wait);
        }
        return 1;
}

/*
 *  Event processing is split up into 2 stages
 *
 *
 */

static SaErrorT harvest_events_for_handler(struct oh_handler *h)
{
        struct oh_event event;
        struct oh_event *e2;
        struct oh_global_param param = { .type = OPENHPI_EVT_QUEUE_LIMIT };

        SaErrorT error = SA_OK;

        if (oh_get_global_param(&param))
                param.u.evt_queue_limit = OH_MAX_EVT_QUEUE_LIMIT;

        do {
                error = h->abi->get_event(h->hnd, &event);
                if (error < 1) {
                        trace("Handler is out of Events");
                } else if (param.u.evt_queue_limit != OH_MAX_EVT_QUEUE_LIMIT &&
                           g_async_queue_length(oh_process_q) >= param.u.evt_queue_limit) {
                        dbg("Process queue is out of space");
                        return SA_ERR_HPI_OUT_OF_SPACE;
                } else {
                        trace("Found event for handler %p", h);
                        e2 = g_memdup(&event, sizeof(struct oh_event));
                        e2->hid = h->id;
                        g_async_queue_push(oh_process_q, e2);
                }
        } while(error > 0);

        return SA_OK;
}

SaErrorT oh_harvest_events()
{
        SaErrorT error = SA_ERR_HPI_ERROR;
        unsigned int hid = 0, next_hid;
        struct oh_handler *h = NULL;

        data_access_lock();

        oh_lookup_next_handler(hid, &next_hid);
        while (next_hid) {
                trace("harvesting for %d", next_hid);
                hid = next_hid;

                h = oh_lookup_handler(hid);
                if(!h) {
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

                oh_lookup_next_handler(hid, &next_hid);
        }
        data_access_unlock();

        return error;
}

static SaErrorT oh_add_event_to_del(SaHpiDomainIdT did, struct oh_hpi_event *e)
{
        struct oh_global_param param = { .type = OPENHPI_LOG_ON_SEV };
        struct oh_domain *d;
        SaErrorT rv = SA_OK;
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];

        oh_get_global_param(&param);

        if (e->event.Severity <= param.u.log_on_sev) { /* less is more */
                param.type = OPENHPI_DEL_SAVE;
                oh_get_global_param(&param);
                /* yes, we need to add real domain support later here */
                d = oh_get_domain(did);
                if (d) {
                        rv = oh_el_append(d->del, &e->event, &e->rdr, &e->res);
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
        struct oh_hpi_event *e = NULL;
        /* We take the domain lock for the whole function here */

        d = oh_get_domain(full_event->did);
        if(!d) {
                dbg("Domain %d doesn't exist", full_event->did);
                return -1; /* FIXME: should this be -1? */
        }

        e = &(full_event->u.hpi_event);
	if (e->event.EventType == SAHPI_ET_USER) {
		e->res.ResourceCapabilities = 0;
		e->rdr.RdrType = SAHPI_NO_RECORD;
	}

        if (e->res.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP
            && e->event.EventType == SAHPI_ET_HOTSWAP) {
                hotswap_push_event(&hs_eq, full_event);
                trace("Pushed hotswap event");
        }

        /* FIXME: Add event to DEL */
        trace("About to add to EL");
        oh_add_event_to_del(d->id, e);
        trace("Added event to EL");

        /*
         * TODO: Here is where we need the SESSION MULTIPLEXING code
         */

        /* FIXME: yes, we need to figure out the real domain at some point */
        trace("About to get session list");
        sessions = oh_list_sessions(oh_get_default_domain_id());
        trace("process_hpi_event, done oh_list_sessions");

        /* multiplex event to the appropriate sessions */
        for(i = 0; i < sessions->len; i++) {
                SaHpiBoolT is_subscribed = SAHPI_FALSE;
                sid = g_array_index(sessions, SaHpiSessionIdT, i);
                oh_get_session_subscription(sid, &is_subscribed);
                if(is_subscribed) {
                        oh_queue_session_event(sid, full_event);
                }
        }
        g_array_free(sessions, TRUE);
        trace("process_hpi_event, done multiplex event => sessions");

        oh_release_domain(d);
        trace("process_hpi_event, done oh_release_domain");

        return 0;
}

static int process_resource_event(struct oh_event *e)
{
        int rv;
        RPTable *rpt = NULL;
        struct oh_domain *d = NULL;
        struct oh_event hpie;

        d = oh_get_domain(e->did);
        if(!d) {
                dbg("Domain %d doesn't exist", e->did);
                return -1;
        }
        rpt = &(d->rpt);

        memset(&hpie, 0, sizeof(hpie));
        if (e->type == OH_ET_RESOURCE_DEL) {
                rv = oh_remove_resource(rpt,e->u.res_event.entry.ResourceId);
                trace("Resource %d in Domain %d has been REMOVED.",
                      e->u.res_event.entry.ResourceId,
                      e->did);

                hpie.did = e->did;
                hpie.u.hpi_event.event.Severity = e->u.res_event.entry.ResourceSeverity;
                hpie.u.hpi_event.event.Source = e->u.res_event.entry.ResourceId;
                hpie.u.hpi_event.event.EventType = SAHPI_ET_RESOURCE;
                hpie.u.hpi_event.event.EventDataUnion.ResourceEvent.ResourceEventType =
                        SAHPI_RESE_RESOURCE_FAILURE;
                if (oh_gettimeofday(&hpie.u.hpi_event.event.Timestamp) != SA_OK)
                    hpie.u.hpi_event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;

        } else {
                struct oh_resource_data *rd = g_malloc0(sizeof(struct oh_resource_data));

                if (!rd) {
                        oh_release_domain(d);
                        dbg("Couldn't allocate resource data");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }

                rd->hid = e->hid;
                rd->controlled = 0;
                rd->auto_extract_timeout = get_default_hotswap_auto_extract_timeout();

                rv = oh_add_resource(rpt,&(e->u.res_event.entry),rd,0);
                trace("Resource %d in Domain %d has been ADDED.",
                      e->u.res_event.entry.ResourceId,
                      e->did);

                hpie.did = e->did;
                hpie.u.hpi_event.event.Severity = e->u.res_event.entry.ResourceSeverity;
                hpie.u.hpi_event.event.Source = e->u.res_event.entry.ResourceId;
                hpie.u.hpi_event.event.EventType = SAHPI_ET_RESOURCE;
                hpie.u.hpi_event.event.EventDataUnion.ResourceEvent.ResourceEventType =
                        SAHPI_RESE_RESOURCE_ADDED;
                if (oh_gettimeofday(&hpie.u.hpi_event.event.Timestamp) != SA_OK)
                    hpie.u.hpi_event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
        }
        oh_release_domain(d);

        if (rv == SA_OK) {
                rv = process_hpi_event(&hpie);
        }

        return rv;
}

static int process_rdr_event(struct oh_event *e)
{
        int rv = -1;
        SaHpiResourceIdT rid = e->u.rdr_event.parent;
        RPTable *rpt = NULL;
        struct oh_domain *d = NULL;

        d = oh_get_domain(e->did);

        /* get the RPT for this domain */
        if(!d) {
                dbg("Domain %d doesn't exist", e->did);
                return -1;
        }

        rpt = &(d->rpt);

        if (e->type == OH_ET_RDR_DEL) {  /* Remove RDR */
                if (!(rv = oh_remove_rdr(rpt, rid, e->u.rdr_event.rdr.RecordId)) ) {
                        trace("SUCCESS: RDR %x in Resource %d in Domain %d has been REMOVED.",
                            e->u.rdr_event.rdr.RecordId, rid, e->did);
                } else {
                        dbg("FAILED: RDR %x in Resource %d in Domain %d has NOT been REMOVED.",
                            e->u.rdr_event.rdr.RecordId, rid, e->did);
                }
        } else if (e->type == OH_ET_RDR) { /* Add/Update RDR */
                if(!(rv = oh_add_rdr(rpt, rid, &(e->u.rdr_event.rdr), NULL, 0))) {
                        trace("SUCCESS: RDR %x in Resource %d in Domain %d has been ADDED.",
                            e->u.rdr_event.rdr.RecordId, rid, e->did);
                } else {
                        dbg("FAILED: RDR %x in Resource %d in Domain %d has NOT been ADDED.",
                            e->u.rdr_event.rdr.RecordId, rid, e->did);
                }
        }

        oh_release_domain(d);

        return rv;
}

SaErrorT oh_process_events()
{
        struct oh_event *e;

        while((e = g_async_queue_try_pop(oh_process_q)) != NULL) {

                /* FIXME: add real check if handler is allowed to push event
                   to the domain id in the event */
                if((e->did != oh_get_default_domain_id()) &&
                   (e->did != SAHPI_UNSPECIFIED_DOMAIN_ID)) {
                        dbg("Domain Id %d not valid for event", e->did);
                        g_free(e);
                        continue;
                }

                switch(e->type) {
                case OH_ET_RESOURCE:
                        trace("Event Type = Resource");
                        process_resource_event(e);
                        break;
                case OH_ET_RESOURCE_DEL:
                        trace("Event Type = Resource Delete");
                        process_resource_event(e);
                        break;
                case OH_ET_RDR:
                        trace("Event Type = RDR");
                        process_rdr_event(e);
                        break;
                case OH_ET_RDR_DEL:
                        trace("Event Type = RDR Delete");
                        process_rdr_event(e);
                        break;
                case OH_ET_HPI:
                        trace("Event Type = HPI Event");
                        process_hpi_event(e);
                        break;
                default:
                        trace("Event Type = Unknown Event");
                }
                oh_detect_event_alarm(e);
                g_free(e);
       }
        return SA_OK;
}

SaErrorT oh_get_events()
{
        SaErrorT rv = SA_OK;

        /* this waits for the event thread to sleep, then 
           runs to deal with sync issues */
        if(oh_run_threaded())
		g_mutex_lock(oh_thread_mutex);
	
        trace("Harvesting events synchronously");
        rv = oh_harvest_events();
        if(rv != SA_OK) {
                dbg("Error on harvest of events.");
        }
        
        rv = oh_process_events();
        if(rv != SA_OK) {
                dbg("Error on processing of events, aborting");
        }
        
        process_hotswap_policy();
        
	if(oh_run_threaded())
        	g_mutex_unlock(oh_thread_mutex);
        
        return rv;
}
