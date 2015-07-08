/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005-2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renier@openhpi.org>
 *
 */

#include <oh_error.h>
#include <oh_plugin.h>

#include "event.h"
#include "threaded.h"
#include "sahpi_wrappers.h"


static const glong OH_DISCOVERY_THREAD_SLEEP_TIME = 180 * G_USEC_PER_SEC;
static const glong OH_EVTGET_THREAD_SLEEP_TIME    = 3 * G_USEC_PER_SEC;

static volatile int started = FALSE;
volatile int signal_stop    = FALSE;
int signal_service_thread    = FALSE; /* Used by the plugins */

GThread *discovery_thread = 0;
GMutex *discovery_lock    = 0;
GCond *discovery_cond     = 0;

GCond *evtget_cond     = 0;
GThread *evtget_thread = 0;
GMutex *evtget_lock    = 0;

GThread *evtpop_thread = 0;


static gpointer discovery_func(gpointer data)
{
        DBG("Begin discovery.");

        g_mutex_lock(discovery_lock);
        while (signal_stop == FALSE) {
                DBG("Discovery: Iteration.");
                SaErrorT error = oh_discovery();
                if (error != SA_OK) {
                        DBG("Got error on threaded discovery return.");
                }

                /* Let oh_wake_discovery_thread know this thread is done */
                g_cond_broadcast(discovery_cond);

		if(signal_stop == TRUE)
			break;

                DBG("Discovery: Going to sleep.");
                #if GLIB_CHECK_VERSION (2, 32, 0)
                gint64 time;
                time = g_get_monotonic_time();
                time = time + OH_DISCOVERY_THREAD_SLEEP_TIME;
                wrap_g_cond_timed_wait(discovery_cond, discovery_lock, time);
                #else
                GTimeVal time;
                g_get_current_time(&time);
                g_time_val_add(&time, OH_DISCOVERY_THREAD_SLEEP_TIME);
                wrap_g_cond_timed_wait(discovery_cond, discovery_lock, &time);
                #endif
        }
        /* Let oh_wake_discovery_thread know this thread is done */
        g_cond_broadcast(discovery_cond);
        g_mutex_unlock(discovery_lock);

        DBG("Done with discovery.");

        return 0;
}

static gpointer evtget_func(gpointer data)
{
        /* Give the discovery time to start first -> FIXME */
        g_usleep(G_USEC_PER_SEC / 2 );

        DBG("Begin event harvesting.");

        g_mutex_lock(evtget_lock);
        while (signal_stop == FALSE) {
                DBG("Event harvesting: Iteration.");
                SaErrorT error = oh_harvest_events();
                if (error != SA_OK) {
                        CRIT("Error on harvest of events.");
                }

		if(signal_stop == TRUE)
			break;

                DBG("Event harvesting: Going to sleep.");
                #if GLIB_CHECK_VERSION (2, 32, 0)
                gint64 time;
                time = g_get_monotonic_time();
                time = time + OH_EVTGET_THREAD_SLEEP_TIME;
                wrap_g_cond_timed_wait(evtget_cond, evtget_lock, time);
                #else
                GTimeVal time;
                g_get_current_time(&time);
                g_time_val_add(&time, OH_EVTGET_THREAD_SLEEP_TIME);
                wrap_g_cond_timed_wait(evtget_cond, evtget_lock, &time);
                #endif
        }
        g_mutex_unlock(evtget_lock);

        DBG("Done with event harvesting.");

        return 0;
}

static gpointer evtpop_func(gpointer data)
{
        SaErrorT error = SA_OK;

        DBG("Begin event processing.");
        while(1) {
                error = oh_process_events();
                if (error == SA_OK) {
                        // OpenHPI is about to quit
                        break;
                } else {
                        CRIT("Error on processing of events.");
                }
        }
        DBG("Done with event processing.");

        return 0;
}


int oh_threaded_start()
{
        if ( started != FALSE ) {
                return 0;
        }

        if (g_thread_supported() == FALSE) {
                wrap_g_thread_init(0);
        }

        signal_stop = FALSE;

        DBG("Starting discovery thread.");
        discovery_cond = wrap_g_cond_new_init();
        discovery_lock = wrap_g_mutex_new_init();
        discovery_thread = wrap_g_thread_create_new("DiscoveryThread",
                                            discovery_func, 0, TRUE, 0);

        DBG("Starting event threads.");
        evtget_cond = wrap_g_cond_new_init();
        evtget_lock = wrap_g_mutex_new_init();
        evtget_thread = wrap_g_thread_create_new("EventGet",evtget_func, 
                                                             0, TRUE, 0);

        evtpop_thread = wrap_g_thread_create_new("EventPop",evtpop_func, 
                                                             0, TRUE, 0);

        started = TRUE;

        return 0;
}

void oh_signal_service(void)
{
        /* Plugin may need to wait for a long time (ex: power cycle). This 
           variable could be used by service threads executing in the plugin 
           code to exit. Discovery, main and other plugin specific threads 
           need to use some other variable set by the close function */
        signal_service_thread = TRUE;
}

int oh_threaded_stop(void)
{
        if ( started == FALSE ) {
                return 0;
        }

        signal_stop = TRUE;

        g_thread_join(evtpop_thread);
        evtpop_thread = 0;

        g_mutex_lock(evtget_lock);
        g_cond_broadcast(evtget_cond);
        g_mutex_unlock(evtget_lock);
        g_thread_join(evtget_thread);
        wrap_g_mutex_free_clear(evtget_lock);
        wrap_g_cond_free(evtget_cond);
        evtget_cond   = 0;
        evtget_thread = 0;
        evtget_lock   = 0;

        g_mutex_lock(discovery_lock);
        g_cond_broadcast(discovery_cond);
        g_mutex_unlock(discovery_lock);
        g_thread_join(discovery_thread);
        wrap_g_mutex_free_clear(discovery_lock);
        wrap_g_cond_free(discovery_cond);
        discovery_cond   = 0;
        discovery_thread = 0;
        discovery_lock   = 0;

        started = FALSE;

        return 0;
}

/**
 * oh_wake_discovery_thread
 * The discovery thread is woken up
 * and we wait until it does a round throughout the
 * plugin instances. If the thread is already running,
 * we will wait for it until it completes the round.
 *
 * Returns: void
 **/
void oh_wake_discovery_thread()
{
        if ( started == FALSE ) {
                return;
        }

        g_mutex_lock(discovery_lock);
        DBG("Going to wait for discovery thread to loop once.");
        g_cond_broadcast(discovery_cond);
        g_cond_wait(discovery_cond, discovery_lock);
        DBG("Got signal from discovery thread being done. Giving lock back.");
        g_mutex_unlock(discovery_lock);
}

