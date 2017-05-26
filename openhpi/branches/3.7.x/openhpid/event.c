/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2006
 * (C) Copyright Pigeon Point Systems. 2010
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
 *     Renier Morales <renier@openhpi.org>
 *     Racing Guo <racing.guo@intel.com>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <errno.h>
#include <string.h>

#include <oHpi.h>

#include <oh_domain.h>
#include <oh_error.h>
#include <oh_handler.h>
#include <oh_plugin.h>
#include <oh_session.h>
#include <oh_utils.h>

#include "alarm.h"
#include "conf.h"
#include "event.h"


extern volatile int signal_stop;
oh_evt_queue * oh_process_q = 0;

/*
 *  The following is required to set up the thread state for
 *  the use of event async queues.  This is true even if we aren't
 *  using live threads.
 */
int oh_event_init()
{
        DBG("Setting up event processing queue.");
        if (!oh_process_q) oh_process_q = g_async_queue_new();
        if (oh_process_q) {
                DBG("Set up processing queue.");
                return 1;
        } else {
                CRIT("Failed to allocate processing queue.");
                return 0;
        }
}

int oh_event_finit(void)
{
        if (oh_process_q) {
                g_async_queue_unref(oh_process_q);
                DBG("Processing queue is disposed.");
        }
        return 0;
}

static struct oh_event * make_quit_event(void)
{
        static SaHpiUint8T signature[SAHPI_MAX_TEXT_BUFFER_LENGTH] =
        {
              0x01, 0xE8, 0xEC, 0x84, 0xFE, 0x23, 0x41, 0x62, 
              0x82, 0xF8, 0xDC, 0x1A, 0x32, 0xD9, 0x24, 0x14, 
              0x6A, 0x5B, 0x92, 0x41, 0xE1, 0xE4, 0x49, 0x34, 
              0x89, 0x36, 0x5C, 0x94, 0xA9, 0x26, 0xBE, 0x70, 
              0x95, 0x12, 0x74, 0xA7, 0xAE, 0x99, 0x49, 0x93, 
              0x87, 0x57, 0xAF, 0xA5, 0x0A, 0xF6, 0x6A, 0x3C, 
              0xC3, 0x02, 0x83, 0x48, 0xAF, 0xCB, 0x49, 0x90, 
              0xA6, 0x5A, 0x1C, 0xDA, 0x6E, 0x56, 0x09, 0x32, 
              0x38, 0x61, 0x87, 0x52, 0x76, 0x98, 0x4A, 0x6C, 
              0x94, 0xEC, 0x67, 0xD2, 0xAE, 0xAB, 0x56, 0xC4, 
              0xA1, 0xFA, 0x65, 0x89, 0xBA, 0x2A, 0x4F, 0x6B, 
              0x84, 0x8F, 0xE0, 0x53, 0xF4, 0xF7, 0xD2, 0x50, 
              0x22, 0xFE, 0x68, 0xB6, 0x38, 0xDD, 0x4C, 0xA7, 
              0xB8, 0x17, 0xF8, 0xC8, 0x5C, 0xE3, 0x27, 0x26, 
              0x8E, 0x76, 0x0F, 0x7C, 0x45, 0x50, 0x40, 0xA8, 
              0x83, 0xE7, 0xCA, 0x73, 0x0C, 0x64, 0x19, 0x74, 
              0xFE, 0x92, 0x39, 0x2B, 0x23, 0xBD, 0x43, 0x05, 
              0xB1, 0xE6, 0xA8, 0x9D, 0x81, 0x01, 0x16, 0x5A, 
              0x05, 0x27, 0xEE, 0xBB, 0x08, 0xE1, 0x48, 0x9F, 
              0xBB, 0xD4, 0x90, 0xA9, 0x46, 0x85, 0xF0, 0x09, 
              0x68, 0x93, 0xAA, 0x6A, 0x62, 0xA4, 0x43, 0xCD, 
              0x8D, 0x80, 0x3B, 0x6C, 0x1A, 0xFF, 0xB3, 0xCD, 
              0x91, 0x09, 0x01, 0x4B, 0x9E, 0xFB, 0x43, 0x1D, 
              0x83, 0xB0, 0x5C, 0x39, 0x6D, 0x6E, 0xB1, 0xDA, 
              0xE0, 0x5D, 0x8D, 0x87, 0xF3, 0x01, 0x40, 0xC0, 
              0xA0, 0x83, 0xEC, 0x5B, 0x71, 0x40, 0xE9, 0x2D, 
              0x54, 0xC1, 0x9B, 0x23, 0x29, 0xFC, 0x4E, 0x64, 
              0xAA, 0x8C, 0xD3, 0x77, 0x8B, 0x19, 0xEC, 0xB8, 
              0xE2, 0xBE, 0xB9, 0x15, 0x28, 0x69, 0x46, 0x73, 
              0x90, 0xFE, 0xE9, 0x24, 0xE6, 0x74, 0xAF, 0x4A, 
              0x78, 0x7D, 0xC7, 0x44, 0x87, 0x51, 0x4A, 0x2C, 
              0x9E, 0x1B, 0xB1, 0x6C, 0x31, 0xEA, 0x40
        };

        struct oh_event * e     = oh_new_event();
        SaHpiEventT * he        = &e->event;
        SaHpiHpiSwEventT * swe  = &he->EventDataUnion.HpiSwEvent;
        SaHpiTextBufferT * data = &swe->EventData;
        SaHpiRptEntryT * rpte   = &e->resource;

        e->hid                     = 0;
        he->Source                 = SAHPI_UNSPECIFIED_RESOURCE_ID;
        he->EventType              = SAHPI_ET_HPI_SW;
        oh_gettimeofday(&he->Timestamp);
        he->Severity               = SAHPI_CRITICAL;
        swe->MId                   = SAHPI_MANUFACTURER_ID_UNSPECIFIED;
        swe->Type                  = SAHPI_HPIE_OTHER;
        data->DataType             = SAHPI_TL_TYPE_BINARY;
        data->Language             = SAHPI_LANG_UNDEF;
        data->DataLength           = sizeof(signature);
        memcpy( &data->Data[0], &signature, sizeof(signature) );
        rpte->ResourceId           = SAHPI_UNSPECIFIED_RESOURCE_ID;
        rpte->ResourceCapabilities = 0;
        e->rdrs                    = 0;
        e->rdrs_to_remove          = 0;

        return e;
}

void oh_post_quit_event(void)
{
        if (oh_process_q) {
            oh_evt_queue_push(oh_process_q, make_quit_event());
        }
}

int oh_detect_quit_event(struct oh_event * e)
{
        if (!e) {
                return ENOENT;
        }
        if (e->event.EventType != SAHPI_ET_HPI_SW) {
                return ENOENT;
        }
        if (e->event.EventDataUnion.HpiSwEvent.MId != 0) {
                return ENOENT;
        }

        struct oh_event * qe = make_quit_event();
        qe->event.Timestamp = e->event.Timestamp;
        int cc = memcmp( e, qe, sizeof(struct oh_event) );
        oh_event_free(qe, FALSE);
        if ( cc != 0 ) {
                return ENOENT;
        }

        return 0;
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

	if (!h->hnd || !h->abi->get_event) return SA_OK;

        do {
                error = h->abi->get_event(h->hnd);
                if (error < 1) {
                        DBG("Handler is out of Events");
                }
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
                DBG("harvesting for %d", next_hid);
                hid = next_hid;

                if(signal_stop == TRUE){
                   error = SA_OK;
                   break;
                }

                h = oh_get_handler(hid);
                if (!h) {
                        CRIT("No such handler %d", hid);
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

static int oh_add_event_to_del(struct oh_domain *d, struct oh_event *e)
{
        struct oh_global_param param = { .type = OPENHPI_LOG_ON_SEV };
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];
        int error = 0;

        if (!d || !e) return -1;

        oh_get_global_param(&param);

        /* Events get logged in DEL if they are of high enough severity */
        if (e->event.EventType == SAHPI_ET_USER ||
            e->event.Severity <= param.u.log_on_sev) {
                param.type = OPENHPI_DEL_SAVE;
                oh_get_global_param(&param);
		SaHpiEventLogInfoT elinfo;

                SaHpiRdrT *rdr = (e->rdrs) ? (SaHpiRdrT *)e->rdrs->data : NULL;
                SaHpiRptEntryT *rpte =
                        (e->resource.ResourceCapabilities) ?
                                &e->resource : NULL;
		error = oh_el_info(d->del, &elinfo);
		if (error == SA_OK && elinfo.Enabled) {
                	error = oh_el_append(d->del, &e->event, rdr, rpte);
		}

                if (param.u.del_save) {
                        param.type = OPENHPI_VARPATH;
                        oh_get_global_param(&param);
                        snprintf(del_filepath,
                                 SAHPI_MAX_TEXT_BUFFER_LENGTH*2,
                                 "%s/del.%u", param.u.varpath, d->id);
                        oh_el_map_to_file(d->del, del_filepath);
                }
        }

        return error;
}

static int process_hpi_event(struct oh_domain *d, struct oh_event *e)
{
        int i;
        GArray *sessions = NULL;
        SaHpiSessionIdT sid;
        SaHpiEventT *event = NULL;
        SaHpiRptEntryT *resource = NULL;
        SaHpiRdrT *rdr = NULL;

        if (!d || !e) return -1;

        event = &e->event;
        resource = &e->resource;
        rdr = (e->rdrs) ? (SaHpiRdrT *)e->rdrs->data : NULL;

        if (event->EventType == SAHPI_ET_USER) {
                resource->ResourceCapabilities = 0;
                if (rdr) rdr->RdrType = SAHPI_NO_RECORD;
        }

        oh_add_event_to_del(d, e);
        DBG("Added event to EL");

        /*
         * Here is the SESSION MULTIPLEXING code
         */
        sessions = oh_list_sessions(d->id);
        if (!sessions) {
                CRIT("Error: Got an empty session list on domain id %u", d->id);
                return -2;
        }
        DBG("Got session list for domain %u", d->id);

        /* Drop events if there are no sessions open to receive them.
         */
        if (sessions->len < 1) {
                g_array_free(sessions, TRUE);
                DBG("No sessions open for event's domain %u. "
                    "Dropping hpi_event", d->id);
                return 0;
        }

        /* multiplex event to the appropriate sessions */
        for (i = 0; i < sessions->len; i++) {
                SaHpiBoolT is_subscribed = SAHPI_FALSE;
#if defined(__sparc) || defined(__sparc__)
                sid = ((SaHpiSessionIdT *)((void *)(sessions->data)))[i];
#else
                sid = g_array_index(sessions, SaHpiSessionIdT, i);
#endif
                oh_get_session_subscription(sid, &is_subscribed);
                if (is_subscribed) {
                        oh_queue_session_event(sid, e);
                }
        }
        g_array_free(sessions, TRUE);
        DBG("done multiplexing event into sessions");

        return 0;
}

static int process_resource_event(struct oh_domain *d, struct oh_event *e)
{
        RPTable *rpt = NULL;
        SaHpiRptEntryT *exists = NULL;
        unsigned int *hidp = NULL;
        SaErrorT error = SA_OK;
        SaHpiBoolT process = TRUE;
        SaHpiBoolT update  = FALSE;
        SaHpiBoolT remove  = FALSE;

        if (!d || !e) {
            return -1;
        }

        rpt = &(d->rpt);
        exists = oh_get_resource_by_id(rpt, e->resource.ResourceId);

        switch ( e->event.EventDataUnion.ResourceEvent.ResourceEventType ) {
            case SAHPI_RESE_RESOURCE_FAILURE:
            case SAHPI_RESE_RESOURCE_INACCESSIBLE:
                if ( exists && exists->ResourceFailed ) {
                    process = FALSE;
                } else {
                    e->resource.ResourceFailed = SAHPI_TRUE;
                }
                break;
            case SAHPI_RESE_RESOURCE_RESTORED:
                if ( exists && exists->ResourceFailed ) {
                    e->resource.ResourceFailed = SAHPI_FALSE;
                } else {
                    process = FALSE;
                }
                break;
            case SAHPI_RESE_RESOURCE_ADDED:
            case SAHPI_RESE_RESOURCE_UPDATED:
                update = TRUE;
                break;
            case SAHPI_RESE_RESOURCE_REMOVED:
                remove = TRUE;
                process = ( exists != 0 ) ? TRUE : FALSE;
                break;
            default:
                // unknown resource event
                // do nothing
                CRIT("Got unknown resource event.");
                return -1;
        }

        if ( remove ) {
            if ( exists ) {
                oh_remove_resource(rpt, e->resource.ResourceId);
            }
        } else {
            hidp = g_new0(unsigned int, 1);
            *hidp = e->hid;
            error = oh_add_resource(rpt, &e->resource, hidp, FREE_RPT_DATA);
            if (error != SA_OK) {
                g_free( hidp );
                CRIT("Cannot update resource.");
                return -1;
            }
            if ( update ) {
                GSList *node = NULL;
                for (node = e->rdrs_to_remove; node; node = node->next) {
                    SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                    SaHpiInstrumentIdT instr_id = oh_get_instrument_id(rdr);
                    SaHpiEntryIdT rdrid = oh_get_rdr_uid(rdr->RdrType, instr_id);
                    oh_remove_rdr(rpt, e->resource.ResourceId, rdrid);
                }
                for (node = e->rdrs; node; node = node->next) {
                    SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                    oh_add_rdr(rpt, e->resource.ResourceId, rdr, NULL, 0);
                }
            }
        }

        if ( process ) {
            process_hpi_event(d, e);
        }

        return 0;
}

static int process_hs_event(struct oh_domain *d, struct oh_event *e)
{
        RPTable *rpt = NULL;
        SaHpiRptEntryT *exists = NULL;
        SaHpiHotSwapEventT * hse = NULL;
        unsigned int *hidp = NULL;
        SaErrorT error = SA_OK;

        if (!d || !e) {
            return -1;
        }

        rpt = &(d->rpt);
        exists = oh_get_resource_by_id(rpt, e->resource.ResourceId);
        hse = &e->event.EventDataUnion.HotSwapEvent;
        if (hse->HotSwapState == SAHPI_HS_STATE_NOT_PRESENT) {
            oh_remove_resource(rpt, e->resource.ResourceId);
        } else {
            hidp = g_new0(unsigned int, 1);
            *hidp = e->hid;
            error = oh_add_resource(rpt, &e->resource, hidp, FREE_RPT_DATA);
            if (error == SA_OK && !exists) {
                GSList *node = NULL;
                for (node = e->rdrs; node; node = node->next) {
                    SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                    oh_add_rdr(rpt, e->resource.ResourceId, rdr, NULL, 0);
                }
            }
        }

        if (hse->HotSwapState != hse->PreviousHotSwapState) {
            process_hpi_event(d, e);
        }

        return 0;
}

static int process_event(SaHpiDomainIdT did,
                         struct oh_event *e)
{
        struct oh_domain *d = NULL;
        RPTable *rpt = NULL;

        if (!e) {
		CRIT("Got NULL event");
		return -1;
	}

        d = oh_get_domain(did);
        if (!d) return -2;
        rpt = &(d->rpt);

        DBG("Processing event for domain %u", d->id);

        switch (e->event.EventType) {
        case SAHPI_ET_RESOURCE:
                if (!e->hid) {
                        CRIT("Resource event with invalid handler id! Dropping.");
                        break;
                }
                if ( e->resource.ResourceCapabilities == 0 ) {
                        SaHpiRptEntryT * restored;
                        restored = oh_get_resource_by_id(rpt, e->event.Source);
                        if ( restored ) {
                                e->resource = *restored;
                        }
                }
                if (!(e->resource.ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE)) {
                        CRIT("Resource event with invalid capabilities. Dropping.");
                        break;
                } else if ((e->resource.ResourceCapabilities & SAHPI_CAPABILITY_FRU) &&
			   (e->event.EventDataUnion.ResourceEvent.ResourceEventType == SAHPI_RESE_RESOURCE_ADDED)) {
                        CRIT("Invalid event. Resource in resource added event "
                            "has FRU capability. Dropping.");
                } else {
                        process_resource_event(d, e);
                }
                break;
        case SAHPI_ET_HOTSWAP:
                if (!e->hid) {
                        CRIT("Hotswap event with invalid handler id! Dropping.");
                        break;
                }
                if ( e->resource.ResourceCapabilities == 0 ) {
                        SaHpiRptEntryT * restored;
                        restored = oh_get_resource_by_id(rpt, e->event.Source);
                        if ( restored ) {
                                e->resource = *restored;
                        }
                }
                if (!(e->resource.ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE)) {
                        CRIT("Hotswap event with invalid capabilities. Dropping.");
                        break;
                } else if (!(e->resource.ResourceCapabilities
                             & SAHPI_CAPABILITY_FRU)) {
                        CRIT("Invalid event. Resource in hotswap event "
                                "has no FRU capability. Dropping.");
                } else {
                        process_hs_event(d, e);
                }
                break;
        case SAHPI_ET_SENSOR:
        case SAHPI_ET_SENSOR_ENABLE_CHANGE:
        case SAHPI_ET_WATCHDOG:
        case SAHPI_ET_HPI_SW:
        case SAHPI_ET_OEM:
	case SAHPI_ET_DOMAIN:
	case SAHPI_ET_USER:
        case SAHPI_ET_DIMI:
        case SAHPI_ET_DIMI_UPDATE:
        case SAHPI_ET_FUMI:
                process_hpi_event(d, e);
                break;
        default:
		CRIT("Don't know what to do for event type  %d", e->event.EventType);
        }
        oh_detect_event_alarm(d, e);
        oh_release_domain(d);

        return 0;
}

SaErrorT oh_process_events()
{
        int cc;
        struct oh_event *e;

        while ((e = g_async_queue_pop(oh_process_q)) != NULL) {
                process_event(OH_DEFAULT_DOMAIN_ID, e);
                cc = oh_detect_quit_event(e);
                oh_event_free(e, FALSE);
                if (cc == 0) {
                        break;
                }
	}

        return SA_OK;
}

