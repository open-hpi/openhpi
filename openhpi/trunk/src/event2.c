/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <epath_utils.h>
#include <uid_utils.h>
#include <oh_event.h>
#include <event_utils.h>
#include <glib.h>
#include <glib/gthread.h>

GAsyncQueue *oh_process_q;

/*
 *  The following is required to set up the thread state for 
 *  the use of async queues.  This is true even if we aren't
 *  using live threads.
 */

int oh_event_init() 
{
        if(!g_thread_supported()) {
                dbg("Initializing thread support");
                g_thread_init(NULL);
        } else {
                dbg("Already supporting threads");
        }
        dbg("Setting up event processing queue");
        oh_process_q = g_async_queue_new();
        return 1;
}

int oh_event_final()
{
        g_async_queue_unref(oh_process_q);
        return 1;
}

/*
 *  Event processing is split up into 2 stages
 *
 *
 */

static int harvest_events_for_handler(struct oh_handler *h) 
{
        struct oh_event event;
        struct oh_event *e2;
        struct timeval to = {0, 0};

        int rv;
        
        do {
                rv = h->abi->get_event(h->hnd, &event, &to);
                if(rv < 1) {
                        dbg("Handler is out of Events");
                        return rv;
                } else {
                        dbg("Found event for handler %p", h);
                        e2 = oh_dup_oh_event(&event);
                        e2->from = h;
                        g_async_queue_push(oh_process_q, e2);
                }
        } while(1);
}

SaErrorT harvest_events() 
{
        GSList *i;
        g_slist_for_each(i, global_handler_list) {
                harvest_events_for_handler(i->data);
        }
        return SA_OK;
}

static SaErrorT oh_add_event_to_dsel(SaHpiDomainIdT did, struct oh_hpi_event *e) 
{
        unsigned int log_severity;
        
        log_severity = get_log_severity(getenv("OPENHPI_LOG_SEV"));        
        if (e->event.Severity <= log_severity) {
                struct oh_domain *d;                
                /* yes, we need to add real domain support later here */
                d = get_domain_by_id(did);
                return oh_sel_add(d->sel, &(e->event));
        }
        return SA_OK;
}
                                

static int process_hpi_event(RPTable *rpt, struct oh_event *full_event)
{
        SaHpiRptEntryT *res;
        GSList *i;
        SaHpiRdrT *rdr;
        struct oh_session_event se;
        struct oh_hpi_event *e = NULL;
        
        e = &(full_event->u.hpi_event);

        res = oh_get_resource_by_id(rpt, e->parent);
        if (res == NULL) {
                dbg("No resource");
                return -1;
        }

        if (res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP 
            && e->event.EventType == SAHPI_ET_HOTSWAP) {
                hotswap_push_event(e);
        }
  
        oh_add_event_to_dsel(OH_DEFAULT_DOMAIN_ID, e);

        
        /* create a session event */
        memset(&se, 0, sizeof(struct oh_session_event ));
        
        oh_copy_event(&se.event, &e->event);
        
        /* copy rpt entry */
        memcpy(&se.rpt_entry, res, sizeof(SaHpiRptEntryT));

        /* find the rdr */
        if ((rdr = oh_get_rdr_by_id(rpt, e->parent, e->id)) != NULL) {
                memcpy(&se.rdr, rdr, sizeof(SaHpiRdrT));
        } else {
                se.rdr.RdrType = SAHPI_NO_RECORD;
        }

        /*
         * TODO: Here is where we need the SESSION MULTIPLEXING code
         */
        
        g_slist_for_each(i, global_session_list) {
                struct oh_session *s = i->data;
                /* yes, we need to add real domain support later here */
                if (s->domain_id == SAHPI_UNSPECIFIED_DOMAIN_ID &&
                    (s->event_state == OH_EVENT_SUBSCRIBE ||
                     (s->event_state == OH_EVENT_UNSUBSCRIBE &&
                      (e->event.Severity == SAHPI_MINOR ||
                       e->event.Severity == SAHPI_MAJOR || e->event.Severity == SAHPI_CRITICAL)))) {
                        /*
                          Push event if session is subscribed, or, if session
                          is unsubscribed, the event is an active alarm.
                        */
                        session_push_event(s, &se);
                }
        }
        
        return 0;
}



static int process_resource_event(RPTable *rpt, struct oh_event *e) 
{
        int rv;
        
        if (e->type == OH_ET_RESOURCE_DEL) {
                rv = oh_remove_resource(rpt,e->u.res_event.entry.ResourceId);
        } else {
                struct oh_resource_data *rd = g_malloc0(sizeof(struct oh_resource_data));

                if (!rd) {
                        dbg("Couldn't allocate resource data");
                        return SA_ERR_HPI_ERROR;
                }

                rd->handler = e->from;
                rd->controlled = 0;
                rd->auto_extract_timeout = get_default_hotswap_auto_extract_timeout();

                rv = oh_add_resource(rpt,&(e->u.res_event.entry),rd,0);
        }
        
        return rv;
}

static int process_rdr_event(RPTable *rpt, struct oh_event *e)
{
        int rv;
        SaHpiResourceIdT rid = e->u.rdr_event.parent;

        if (e->type == OH_ET_RDR_DEL) {
                rv = oh_remove_rdr(rpt,rid,e->u.rdr_event.rdr.RecordId);
        } else {
                rv = oh_add_rdr(rpt,rid,&(e->u.rdr_event.rdr),NULL,0);
        }

        if (rv) dbg("Could not process rdr event. Parent resource not found.");
                
        return rv;
}

SaErrorT process_events(RPTable *rpt)
{
        struct oh_event *e;
        
        while((e = g_async_queue_try_pop(oh_process_q)) != NULL) {
                switch(e->type) {
                case OH_ET_RESOURCE:
                        dbg("Resource");
                        process_resource_event(rpt, e);
                        break;
                case OH_ET_RESOURCE_DEL:
                        dbg("Resource Delete");
                        process_resource_event(rpt, e);
                        break;
                case OH_ET_RDR:
                        dbg("RDR");
                        process_rdr_event(rpt, e);
                        break;
                case OH_ET_RDR_DEL:
                        dbg("RDR Delete");
                        process_rdr_event(rpt, e);
                        break;
                case OH_ET_HPI:
                        dbg("HPI Event");
                        process_hpi_event(rpt, e);
                        break;
                default:
                        dbg("Unknown Event");
                }
        }
        g_free(e);
        return SA_OK;
}

SaErrorT get_events(RPTable *rpt) 
{
        // in a thread world this becomes a noop
        SaErrorT rv = SA_OK;
        rv = harvest_events();
        if(rv != SA_OK) {
                dbg("Error on harvest of events, aborting");
                return rv;
        }
        rv = process_events(rpt);
        if(rv != SA_OK) {
                dbg("Error on processing of events, aborting");
                return rv;
        }
        return rv;
}

unsigned int get_log_severity(char *severity) 
{
        if (!severity) return SAHPI_INFORMATIONAL;
        else if (!strcmp("CRITICAL", severity)) {
                return SAHPI_CRITICAL;
        } else if (!strcmp("MAJOR",severity)) {
                return SAHPI_MAJOR;
        } else if (!strcmp("MINOR",severity)) {
                return SAHPI_MINOR;
        } else if (!strcmp("OK",severity)) {
                return SAHPI_OK;        
        } else if (!strcmp("DEBUG",severity)) {
                return SAHPI_DEBUG;
        } else {
                return SAHPI_INFORMATIONAL;
        }
}

