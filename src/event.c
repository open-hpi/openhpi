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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openhpi.h>
#include <oh_event.h>
#include <glib.h>

#define OH_THREAD_SLEEP_TIME 5 * G_USEC_PER_SEC

/* This is a short term hack to get this running properly.  Will 
   be changed to more robust mechanisms later */
// #define OPENHPI_RUN_THREADED
//#ifdef OPENHPI_RUN_THREADED
//#define oh_run_threaded() 1
//#else
//#define oh_run_threaded() 0
//#endif

GAsyncQueue *oh_process_q = NULL;
GCond *oh_thread_wait = NULL;
GThread *oh_event_thread = NULL;
GError *oh_event_thread_error = NULL;
GMutex *oh_thread_mutex = NULL;

/*
 *  The following is required to set up the thread state for
 *  the use of async queues.  This is true even if we aren't
 *  using live threads.
 */

gpointer oh_event_thread_loop(gpointer);

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
        if(oh_run_threaded()) {
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
        data_access_lock();
        g_slist_for_each(i, global_handler_list) {
                harvest_events_for_handler(i->data);
        }
        data_access_unlock();
        return SA_OK;
}

static SaErrorT oh_add_event_to_del(SaHpiDomainIdT did, struct oh_hpi_event *e)
{
        SaHpiSeverityT log_severity = SAHPI_MINOR;
        SaHpiTextBufferT buffer;
        struct oh_domain *d;
        SaErrorT rv = SA_OK;     
	char *openhpi_log_sev = NULL;
        
	/* FIXME: this needs to be locked at boot time */
	openhpi_log_sev = getenv("OPENHPI_LOG_SEV"); 
	if (openhpi_log_sev) {
		strncpy(buffer.Data, openhpi_log_sev, SAHPI_MAX_TEXT_BUFFER_LENGTH);
                oh_encode_severity(&buffer, &log_severity);                
        }

        if (e->event.Severity <= log_severity) { /* less is more */
                /* yes, we need to add real domain support later here */
                d = oh_get_domain(did);
                if(d) {
                        rv = oh_el_append(d->del, &e->event, &e->rdr, &e->res);
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
                
                hpie.did = e->did;
                hpie.u.hpi_event.event.Severity = e->u.res_event.entry.ResourceSeverity;
                hpie.u.hpi_event.event.Source = e->u.res_event.entry.ResourceId;
                hpie.u.hpi_event.event.EventType = SAHPI_ET_RESOURCE;
                hpie.u.hpi_event.event.EventDataUnion.ResourceEvent.ResourceEventType = 
                        SAHPI_RESE_RESOURCE_FAILURE;
		dbg("************************ process_resource_event REMOVE\n");                
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
                
                hpie.did = e->did;
                hpie.u.hpi_event.event.Severity = e->u.res_event.entry.ResourceSeverity;
                hpie.u.hpi_event.event.Source = e->u.res_event.entry.ResourceId;
                hpie.u.hpi_event.event.EventType = SAHPI_ET_RESOURCE;
                hpie.u.hpi_event.event.EventDataUnion.ResourceEvent.ResourceEventType = 
                        SAHPI_RESE_RESOURCE_ADDED;
		dbg("************************ process_resource_event ADD\n");
        }
        oh_release_domain(d);
        
        if(rv == SA_OK) {
                rv = process_hpi_event(&hpie);
        }

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
		dbg("************************ process_rdr_event REMOVE\n");
        } else {
                rv = oh_add_rdr(rpt,rid,&(e->u.rdr_event.rdr),NULL,0);
		dbg("************************ process_rdr_event ADD\n");
        }

        if (rv) dbg("Could not process rdr event. Parent resource not found.");

        oh_release_domain(d);

/*	need this after different type rdr events are processed above FIXME:DJ
	otherwise rdr events are never palced on eventq
        if(rv == SA_OK) {
                rv = process_hpi_event(&hpie);
        }
*/        
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
//        if(!oh_run_threaded()) {
                SaErrorT rv = SA_OK;
                dbg("About to harvest events in the loop");
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
        /*} else {
                dbg("Running threaded");
                g_cond_signal(oh_thread_wait);
                dbg("Signaled thread to process");
                sleep(1);
                return SA_OK;
        }
	*/	
	
	
}


gpointer oh_event_thread_loop(gpointer data) {
//        SaErrorT rv = SA_OK;
        GTimeVal time;
        
        while(oh_run_threaded()) {
                dbg("About to run through the event loop");

		get_events();
/*
                rv = harvest_events();
                if(rv != SA_OK) {
                        dbg("Error on harvest of events %s", oh_lookup_error(rv));
                }
                rv = process_events();
                if(rv != SA_OK) {
                        dbg("Error on processing of events %s", oh_lookup_error(rv));
                }
*/
                g_get_current_time(&time);
                g_time_val_add(&time, OH_THREAD_SLEEP_TIME);
                dbg("Going to sleep");

                if (g_cond_timed_wait(oh_thread_wait, oh_thread_mutex, &time))
			dbg("Got signal from plugin");
		else
			dbg("Timed Out: Woke up, am looping again");
        }
        g_thread_exit(0);
        return 0;
}
