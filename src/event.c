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

#include <openhpi.h>
#include <oh_event.h>
#include <glib.h>

GAsyncQueue *oh_process_q;

/*
 *  The following is required to set up the thread state for
 *  the use of async queues.  This is true even if we aren't
 *  using live threads.
 */

int oh_event_init()
{
        trace("Attempting to init event");
        if(!g_thread_supported()) {
                trace("Initializing thread support");
                g_thread_init(NULL);
        } else {
                trace("Already supporting threads");
        }
        trace("Setting up event processing queue");
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
                        trace("Handler is out of Events");
                        return rv;
                } else {
                        trace("Found event for handler %p", h);
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

static SaErrorT oh_add_event_to_del(SaHpiDomainIdT did, struct oh_hpi_event *e)
{
        unsigned int log_severity;
        struct oh_domain *d;
        SaErrorT rv = SA_OK;

        /* FIXME: this needs to be locked at boot time */
        log_severity = get_log_severity(getenv("OPENHPI_LOG_SEV"));
        
        if (e->event.Severity <= log_severity) { // less is more
                /* yes, we need to add real domain support later here */
                d = oh_get_domain(did);
                if(d) {
                        rv = oh_el_add(d->del, &e->event);
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

        if (e->res.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP
            && e->event.EventType == SAHPI_ET_HOTSWAP) {
                hotswap_push_event(e);
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
        sessions = oh_list_sessions(1);

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
        
        oh_release_domain(d);
        
        return 0;
}

static int process_resource_event(struct oh_event *e)
{
        int rv;
        RPTable *rpt = NULL;
        struct oh_domain *d = NULL;
        
        d = oh_get_domain(e->did);
        if(!d) {
                dbg("Domain %d doesn't exist", e->did);
                return -1;
        }
        rpt = &(d->rpt);
        
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
        oh_release_domain(d);

        return rv;
}

static int process_rdr_event(struct oh_event *e)
{
        int rv;
        SaHpiResourceIdT rid = e->u.rdr_event.parent;
        RPTable *rpt = NULL;
        struct oh_domain *d = NULL;

        d = oh_get_domain(e->did);
        if(!d) {
                dbg("Domain %d doesn't exist", e->did);
                return -1;
        }
        rpt = &(d->rpt);

        if (e->type == OH_ET_RDR_DEL) {
                rv = oh_remove_rdr(rpt,rid,e->u.rdr_event.rdr.RecordId);
        } else {
                rv = oh_add_rdr(rpt,rid,&(e->u.rdr_event.rdr),NULL,0);
        }

        if (rv) dbg("Could not process rdr event. Parent resource not found.");

        oh_release_domain(d);
        
        return rv;
}

SaErrorT process_events()
{
        struct oh_event *e;

        while((e = g_async_queue_try_pop(oh_process_q)) != NULL) {

                /* FIXME: add real check if handler is allowed to push event 
                   to the domain id in the event */
                if(e->did != 1) { // add use of real domain checker once renier provides
                        dbg("Domain Id %d not valid for event", e->did);
                        g_free(e);
                        return SA_ERR_HPI_INVALID_DATA;
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
        }
        g_free(e);
        return SA_OK;
}

SaErrorT get_events()
{
        // in a thread world this becomes a noop
        SaErrorT rv = SA_OK;
        rv = harvest_events();
        if(rv != SA_OK) {
                dbg("Error on harvest of events, aborting");
                return rv;
        }
        rv = process_events();
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

