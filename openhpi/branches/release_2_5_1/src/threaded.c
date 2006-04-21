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
#include <oh_config.h>
#include <oh_plugin.h>
#include <oh_hotswap.h>
#include <oh_error.h>

#define OH_DISCOVERY_THREAD_SLEEP_TIME 180 * G_USEC_PER_SEC
#define OH_EVENT_THREAD_SLEEP_TIME 3 * G_USEC_PER_SEC

GCond *oh_event_thread_wait = NULL;
GThread *oh_event_thread = NULL;
GError *oh_event_thread_error = NULL;
GMutex *oh_event_thread_mutex = NULL;
GStaticMutex oh_wake_event_mutex = G_STATIC_MUTEX_INIT;

GThread *oh_discovery_thread = NULL;
GError *oh_discovery_thread_error = NULL;
GMutex *oh_discovery_thread_mutex = NULL;
GCond *oh_discovery_thread_wait = NULL;
GStaticMutex oh_wake_discovery_mutex = G_STATIC_MUTEX_INIT;

static gboolean oh_is_threaded = FALSE;

static int oh_discovery_init(void)
{
        /* Nothing to do here...for now */
        return 0;
}

/*
 *  The following is required to set up the thread state for
 *  the use of event async queues.  This is true even if we aren't
 *  using live threads.
 */
static int oh_event_init(void)
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

static int oh_discovery_final(void)
{
        if (oh_threaded_mode()) {
                g_mutex_free(oh_discovery_thread_mutex);
                g_cond_free(oh_discovery_thread_wait);
        }

        return 0;
}

static int oh_event_final(void)
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
        while (oh_threaded_mode()) {
                trace("Doing threaded discovery on all handlers");
dbg("Doing threaded discovery on all handlers");
                error = oh_domain_resource_discovery(0);
                if (error) {
                        trace("Got error on threaded discovery return.");
                }

                /* Let oh_wake_discovery_thread know this thread is done */
                g_cond_broadcast(oh_discovery_thread_wait);
                g_get_current_time(&time);
                g_time_val_add(&time, OH_DISCOVERY_THREAD_SLEEP_TIME);
                /* Go to sleep; let oh_wake_discovery_thread take the mutex */
                trace("Going to sleep");
                if (g_cond_timed_wait(oh_discovery_thread_wait,
                                      oh_discovery_thread_mutex, &time))
                        trace("SIGNALED: Got signal from saHpiDiscover()");
                else
                        trace("TIMEDOUT: Woke up, am doing discovery again");
        }
        g_mutex_unlock(oh_discovery_thread_mutex);
        g_thread_exit(0);

        return data;
}

static gpointer oh_event_thread_loop(gpointer data)
{
        GTimeVal time;
        SaErrorT error = SA_OK;
        static int first_loop = 1;

        g_mutex_lock(oh_event_thread_mutex);
        while (oh_threaded_mode()) {
                /* Give the discovery time to start first -> FIXME */
                if (first_loop) {
                        struct timespec sleepytime =
                                { .tv_sec = 0, .tv_nsec = 500000000};
                        first_loop = 0;
                        nanosleep(&sleepytime, NULL);
                }

                trace("Thread Harvesting events");

dbg("Doing event harvesting on all handlers");

                error = oh_harvest_events();
                if (error != SA_OK) dbg("Error on harvest of events.");

                trace("Thread processing events");
                error = oh_process_events();
                if (error != SA_OK) dbg("Error on processing of events.");

                trace("Thread processing hotswap policy");
                process_hotswap_policy();

                /* Let oh_wake_event_thread know this thread is done */
                g_cond_broadcast(oh_event_thread_wait);
                g_get_current_time(&time);
                g_time_val_add(&time, OH_EVENT_THREAD_SLEEP_TIME);
                trace("Going to sleep");
                if (g_cond_timed_wait(oh_event_thread_wait, oh_event_thread_mutex, &time))
                        trace("SIGNALED: Got signal from plugin");
                else
                        trace("TIMEDOUT: Woke up, am looping again");
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

        error = oh_discovery_init();
        if (oh_event_init() || error) error = 1;

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

dbg("### discovery_thread, thrdid [%p] ###\n", (void *)oh_discovery_thread);

                trace("Starting event thread");
                oh_event_thread_wait = g_cond_new();
                oh_event_thread_mutex = g_mutex_new();
                oh_event_thread = g_thread_create(oh_event_thread_loop,
                                                  NULL, FALSE, &oh_event_thread_error);

dbg("### event_thread, thrdid [%p] ###\n", (void *)oh_discovery_thread);

        }

        return 0;
}

int oh_threaded_final()
{
        oh_discovery_final();
        oh_event_final();

        return 0;
}

/**
 * oh_wake_discovery_thread
 * @wait: Says whether we should wait for the discovery thread
 * to do one round through the plugin instances. Otherwise, we
 * just knock on the discovery thread's door and return quickly.
 *
 * If wait is true, the discovery thread is woken up
 * and we wait until it does a round throughout the
 * plugin instances. If the thread is already running,
 * we will wait for it until it completes the round.
 *
 * Returns: void
 **/
void oh_wake_discovery_thread(SaHpiBoolT wait)
{
        if (oh_threaded_mode()) {

                if (!wait) { /* If not waiting, just signal the thread and go. */
                        g_cond_broadcast(oh_discovery_thread_wait);
                        return;
                }

                g_static_mutex_lock(&oh_wake_discovery_mutex);
                if (g_mutex_trylock(oh_discovery_thread_mutex)) {
                        /* The thread was asleep; wake it up. */
                        trace("Going to wait for discovery thread to loop once.");
                        g_cond_broadcast(oh_discovery_thread_wait);
                        g_cond_wait(oh_discovery_thread_wait,
                                    oh_discovery_thread_mutex);
                        trace("Got signal from discovery"
                              " thread being done. Giving lock back");
                        g_mutex_unlock(oh_discovery_thread_mutex);
                } else {
                        /* Thread was already up. Wait until it completes */
                        trace("Waiting for discovery thread...");
                        g_mutex_lock(oh_discovery_thread_mutex);
                        trace("...Done waiting for discovery thread.");
                        g_mutex_unlock(oh_discovery_thread_mutex);
                }
                g_static_mutex_unlock(&oh_wake_discovery_mutex);
        }

        return;
}

/**
 * oh_wake_event_thread
 * @wait: Says whether we should wait for the event thread
 * to do one round through the plugin instances. Otherwise, we
 * just knock on the event thread's door and return quickly.
 *
 * If wait is true, the event thread is woken up
 * and we wait until it does a round throughout the
 * plugin instances. If the thread is already running,
 * we will wait for it until it completes the round.
 *
 * Returns: void
 **/
void oh_wake_event_thread(SaHpiBoolT wait)
{
        if (oh_threaded_mode()) {

                if (!wait) { /* If not waiting, just signal the thread and go. */
                        g_cond_broadcast(oh_event_thread_wait);
                        return;
                }

                g_static_mutex_lock(&oh_wake_event_mutex);
                if (g_mutex_trylock(oh_event_thread_mutex)) {
                        /* The thread was asleep; wake it up. */
                        trace("Going to wait for event thread to loop once.");
                        g_cond_broadcast(oh_event_thread_wait);
                        g_cond_wait(oh_event_thread_wait,
                                    oh_event_thread_mutex);
                        trace("Got signal from event"
                              " thread being done. Giving lock back");
                        g_mutex_unlock(oh_event_thread_mutex);
                } else {
                        /* Thread was already up. Wait until it completes */
                        trace("Waiting for event thread...");
                        g_mutex_lock(oh_event_thread_mutex);
                        trace("...Done waiting for event thread.");
                        g_mutex_unlock(oh_event_thread_mutex);
                }
                g_static_mutex_unlock(&oh_wake_event_mutex);
        }

        return;
}
