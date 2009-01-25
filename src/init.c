/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <config.h>
#include <oh_init.h>
#include <oh_ssl.h>
#include <oh_config.h>
#include <oh_plugin.h>
#include <oh_domain.h>
#include <oh_session.h>
#include <oh_threaded.h>
#include <oh_error.h>
#include <oh_lock.h>
#include <oh_utils.h>
#include <oh_event.h>

extern SaHpiBoolT stop_oh_threads;
extern GThread *oh_evtpop_thread;
extern GThread *oh_discovery_thread;
extern GThread *oh_evtget_thread;
extern oh_evt_queue oh_process_q;
extern GStaticMutex oh_wake_discovery_mutex;
extern GMutex *oh_discovery_thread_mutex;
extern GCond *oh_discovery_thread_wait;
extern GMutex *oh_evtget_thread_mutex;
extern GStaticMutex oh_wake_evtget_mutex;
extern GCond *oh_evtget_thread_wait;

/**
 * oh_init
 *
 * Returns: 0 on success otherwise an error code
 **/
int oh_init(void)
{
        static int initialized = 0;
        struct oh_parsed_config config = { NULL, 0, 0 };
        struct oh_global_param config_param = { .type = OPENHPI_CONF };
        SaErrorT rval;

        data_access_lock();
        if (initialized) { /* Don't initialize more than once */
        	data_access_unlock();
        	return 0;
        }

        /* Initialize thread engine */
        oh_threaded_init();
#ifdef HAVE_OPENSSL
	/* Initialize SSL library */
	if (oh_ssl_init()) {
                err("SSL library intialization failed.");
                data_access_unlock();
		return SA_ERR_HPI_OUT_OF_MEMORY; /* Most likely */
	}
#endif
        /* Set openhpi configuration file location */
        oh_get_global_param(&config_param);

        rval = oh_load_config(config_param.u.conf, &config);
        /* Don't error out if there is no conf file */
        if (rval < 0 && rval != -4) {
                err("Can not load config.");
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Initialize uid_utils */
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                err("Unique ID intialization failed.");
                data_access_unlock();
                return rval;
        }
        dbg("Initialized UID.");

        /* Initialize handler table */
        oh_handlers.table = g_hash_table_new(g_int_hash, g_int_equal);
        dbg("Initialized handler table");

        /* Initialize domain table */
        oh_domains.table = g_hash_table_new(g_int_hash, g_int_equal);
        dbg("Initialized domain table");

        /* Initialize session table */
        oh_sessions.table = g_hash_table_new(g_int_hash, g_int_equal);
        dbg("Initialized session table");

        /* Load plugins, create handlers and domains */
        oh_process_config(&config);

        /* Create default domain if it does not exist yet. */
	if (oh_create_domain(OH_DEFAULT_DOMAIN_ID,
	                     "DEFAULT",
	                      SAHPI_UNSPECIFIED_DOMAIN_ID,
	                      SAHPI_UNSPECIFIED_DOMAIN_ID,
	                      SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY,
	                      SAHPI_TIMEOUT_IMMEDIATE)) {
	        data_access_unlock();
		err("Could not create first domain!");
		return SA_ERR_HPI_ERROR;
       }

        /*
         * Wipes away configuration lists (plugin_names and handler_configs).
         * global_params is not touched.
         */
        oh_clean_config(&config);

        /*
         * If any handlers were defined in the config file AND
         * all of them failed to load, Then return with an error.
         */
        if (config.handlers_defined > 0 && config.handlers_loaded == 0) {
                warn("Warning: Handlers were defined, but none loaded.");
        } else if (config.handlers_defined > 0 &&
                   config.handlers_loaded < config.handlers_defined) {
                warn("*Warning*: Not all handlers defined loaded."
                     " Check previous messages.");
        }

        /* Start discovery and event threads */
	oh_threaded_start();

        initialized = 1;
        data_access_unlock();
	dbg("OpenHPI has been initialized");

        /* infrastructure initialization has completed at this point */

        /* Check if there are any handlers loaded */
        if (config.handlers_defined == 0) {
                warn("*Warning*: No handler definitions found in config file."
                     " Check configuration file %s and previous messages",
                     config_param.u.conf);
        }

        /*
         * HACK: wait a second before returning
         * to give the threads time to populate the RPT
         */
        struct timespec waittime = { .tv_sec = 1, .tv_nsec = 1000L};
        nanosleep(&waittime, NULL);

        /* Do not use SA_OK here in case it is ever changed to something
         * besides zero, The runtime stuff depends on zero being returned here
         * in order for the shared library to be completely initialized.
         */
        return 0;
}

/**
 * oh_finit
 *
 * Returns: always returns 0
 **/
int oh_finit(void)
{
//        unsigned int hid = 0, next_hid, i;
        unsigned int hid = 0, next_hid;
/*        GArray *domain_results = NULL;
        oh_domain_result dr; */

        data_access_lock();

        /* Set the stop_oh_threads to TRUE to stop the
         *  discovery and event threads 
         */
        stop_oh_threads = SAHPI_TRUE;

        /* Wake up the discovery thread, if it is waiting */
        g_static_mutex_lock(&oh_wake_discovery_mutex);
        if (g_mutex_trylock(oh_discovery_thread_mutex)) {
                g_cond_broadcast(oh_discovery_thread_wait);
                g_mutex_unlock(oh_discovery_thread_mutex);
        } else {
                g_mutex_lock(oh_discovery_thread_mutex);
                g_cond_broadcast(oh_discovery_thread_wait);
                g_mutex_unlock(oh_discovery_thread_mutex);
        }
        g_static_mutex_unlock(&oh_wake_discovery_mutex);

        /* Wait for the discovery thread to exit */
        g_thread_join(oh_discovery_thread);
        dbg("discovery thread stopped");

        /* Wake up the get event thread, if it is waiting */
        g_static_mutex_lock(&oh_wake_evtget_mutex);
        if (g_mutex_trylock(oh_evtget_thread_mutex)) {
                g_cond_broadcast(oh_evtget_thread_wait);
                g_mutex_unlock(oh_evtget_thread_mutex);
        } else {
                g_mutex_lock(oh_evtget_thread_mutex);
                g_cond_broadcast(oh_evtget_thread_wait);
                g_mutex_unlock(oh_evtget_thread_mutex);
        }
        g_static_mutex_unlock(&oh_wake_evtget_mutex);

        /* Wait for the event get thread to exit */
        g_thread_join(oh_evtget_thread);
        dbg("event get thread stopped");

        /* Wait for the event pop thread to exit */
        g_thread_join(oh_evtpop_thread);
        dbg("event pop thread stopped");

        /* Cleanup the instantiated plugins */
        oh_getnext_handler_id(hid, &next_hid);
        while (next_hid) {
                hid = next_hid;
                oh_destroy_handler(hid);
                oh_getnext_handler_id(hid, &next_hid);
        }
        dbg("all the instantiated plugins closed");

        /* Release the event queue */
        oh_event_queue_free();
        dbg("event queue released");

        /* Cleanup the thread related variables */
        oh_threaded_final();
        dbg("global variables released");

        /* Cleanup the SSL library */
        oh_ssl_finalize();
        dbg("ssl library cleaned");

        /* close the ltdl structures */
        oh_exit_ltdl();
        dbg("ltdl library cleaned");

        /* Destroy the sessions */
        oh_destroy_domain_sessions(OH_DEFAULT_DOMAIN_ID);
        /* Destroy the domain */
        oh_destroy_domain(OH_DEFAULT_DOMAIN_ID);
        dbg("default domain is destroyed");

        g_hash_table_destroy(oh_domains.table);
        dbg("domain table released");
        g_hash_table_destroy(oh_sessions.table);
        dbg("sessions table released");

        data_access_unlock();
        return 0;
}
