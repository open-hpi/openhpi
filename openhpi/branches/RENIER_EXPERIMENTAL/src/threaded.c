/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *
 */
 
#include <oh_threaded.h>

#define OH_DISCOVERY_THREAD_SLEEP_TIME 180 * G_USEC_PER_SEC
#define OH_EVENT_THREAD_SLEEP_TIME 3 * G_USEC_PER_SEC
 
static gboolean oh_is_threaded = FALSE;
GCond *oh_event_thread_wait = NULL;
GThread *oh_event_thread = NULL;
GError *oh_event_thread_error = NULL;
GMutex *oh_event_thread_mutex = NULL;
GThread *oh_discovery_thread = NULL;
GError *oh_discovery_thread_error = NULL;
GMutex *oh_discovery_thread_mutex = NULL;
GCond *oh_discovery_thread_wait = NULL;

static int oh_discovery_init()
{
        /* Nothing to do here...for now */
        return 0;
}

/*
 *  The following is required to set up the thread state for
 *  the use of event async queues.  This is true even if we aren't
 *  using live threads.
 */
static int oh_event_init()
{        
        trace("Setting up event processing queue");
        oh_process_q = g_async_queue_new();
        if(oh_process_q) {
                trace("Set up processing queue");
                return 1;
        } else {
                dbg("Failed to allocate processing queue");
                return 0;
        }
}

static int oh_discovery_final()
{
        if (oh_threaded_mode()) {
                g_mutex_free(oh_discovery_thread_mutex);
                g_cond_free(oh_discovery_thread_wait);
        }
        
        return 0;
}

static int oh_event_final()
{
        g_async_queue_unref(oh_process_q);
        if (oh_threaded_mode()) {
                g_mutex_free(oh_event_thread_mutex);
                g_cond_free(oh_event_thread_wait);
        }
        
        return 0;
}

static gpointer oh_discovery_thread_loop(gpointer data)
{
        GTimeVal time;
        SaErrorT error = SA_OK;
        	
        g_mutex_lock(oh_discovery_thread_mutex);
        while (oh_run_threaded()) {
                g_get_current_time(&time);
                g_time_val_add(&time, OH_EVENT_THREAD_SLEEP_TIME);
                trace("Going to sleep");
                if (g_cond_timed_wait(oh_discovery_thread_wait,
                                      oh_discovery_thread_mutex, &time))
                        trace("SIGNALED: Got signal from plugin");
                else
                        trace("TIMEDOUT: Woke up, am looping again");
                        
                /* FIXME: Add domain discovery code for all handlers */
        }
        g_mutex_unlock(oh_discovery_thread_mutex);
        g_thread_exit(0);
        
        return data;
}

static gpointer oh_event_thread_loop(gpointer data)
{
        GTimeVal time;
        SaErrorT error = SA_OK;
        
	g_mutex_lock(oh_event_thread_mutex);
        while(oh_run_threaded()) {
                g_get_current_time(&time);
                g_time_val_add(&time, OH_EVENT_THREAD_SLEEP_TIME);
                trace("Going to sleep");
                if (g_cond_timed_wait(oh_event_thread_wait, oh_event_thread_mutex, &time))
                        trace("SIGNALED: Got signal from plugin");
                else
                        trace("TIMEDOUT: Woke up, am looping again");
                        
		trace("Thread Harvesting events");
                error = oh_harvest_events();
                if(error != SA_OK) {
                        trace("Error on harvest of events.");
                }
                
		trace("Thread processing events");
                error = oh_process_events();
                if(error != SA_OK) {
                        trace("Error on processing of events, aborting");
                }
                
                trace("Thread processing hotswap");
                process_hotswap_policy();                                
        }
	g_mutex_unlock(oh_event_thread_mutex);
        g_thread_exit(0);
        
        return data;
}
 
gboolean oh_threaded_mode()
{
        return oh_is_threaded;
}

int oh_threaded_init()
{
        int error = 0;
        
        trace("Attempting to init event");
        if (!g_thread_supported()) {
                trace("Initializing thread support");
                g_thread_init(NULL);
        } else {
                trace("Already supporting threads");
        }
        
        error = oh_init_discovery();
        if (oh_init_event() || error) error = 1;
        
        return error;
}

int oh_threaded_start()
{
        struct oh_global_param threaded_param = { .type = OPENHPI_THREADED };

        oh_get_global_param(&threaded_param);
        if (threaded_param.u.threaded) {
                oh_is_threaded = TRUE;
                
                trace("Starting discovery thread");
                oh_discovery_thread_wait = g_cond_new();
                oh_discovery_thread_mutex = g_mutex_new();
                oh_discovery_thread = g_thread_create(oh_discovery_thread_loop,
                                                      NULL, FALSE,
                                                      &oh_discovery_thread_error);
                
                trace("Starting event thread");                
                oh_event_thread_wait = g_cond_new();
                oh_event_thread_mutex = g_mutex_new();
                oh_event_thread = g_thread_create(oh_event_thread_loop,
                                                  NULL, FALSE, &oh_event_thread_error);                
        }

        return 1;
}

int oh_threaded_final()
{
        oh_discovery_final();
        oh_event_final();
        
        return 0;
}