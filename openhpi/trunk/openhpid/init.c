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
 *      Bryan Sutula <sutula@users.sourceforge.net>
 *
 */
#include <inttypes.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include <glib.h>

#include <oHpi.h>

#include <config.h>
#include <oh_domain.h>
#include <oh_error.h>
#ifndef _WIN32
#include <oh_ssl.h>
#endif /* _WIN32 */
#include <oh_plugin.h>
#include <oh_session.h>
#include <oh_utils.h>

#include "conf.h"
#include "event.h"
#include "init.h"
#include "lock.h"
#include "threaded.h"
#include "sahpi_wrappers.h"

/**
 * oh_init
 *
 * Returns: 0 on success otherwise an error code
 **/
int oh_init(void)
{
        static int initialized = 0;
        struct oh_global_param param;
        struct oh_parsed_config config = { NULL, 0, 0 };
        SaErrorT rval;

        if (g_thread_supported() == FALSE) {
            wrap_g_thread_init(0);
        }

        data_access_lock();
        if (initialized) { /* Don't initialize more than once */
        	data_access_unlock();
        	return 0;
        }

        /* Initialize event queue */
        oh_event_init();

#ifdef HAVE_OPENSSL
        INFO("Initializing SSL Library.");
	if (oh_ssl_init()) {
                CRIT("SSL library intialization failed.");
                data_access_unlock();
		return SA_ERR_HPI_OUT_OF_MEMORY; /* Most likely */
	}
#endif
        /* Set openhpi configuration file location */
        oh_get_global_param2(OPENHPI_CONF, &param);
#ifdef _WIN32
        char config_file[MAX_PATH];
        DWORD cc = ExpandEnvironmentStrings(param.u.conf, config_file, MAX_PATH);
        if ((cc != 0) && (cc < MAX_PATH)) {
            INFO("Loading config file %s.", config_file);
            rval = oh_load_config(config_file, &config);
        } else {
            CRIT("Failed to expand config file path: %s", param.u.conf);
            rval = SA_ERR_HPI_ERROR;
        }
#else
        INFO("Loading config file %s.", param.u.conf);
        rval = oh_load_config(param.u.conf, &config);
#endif /* _WIN32 */
        /* Don't error out if there is no conf file */
        if (rval < 0 && rval != -4) {
                CRIT("Can not load config.");
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

	/* One particular variable, OPENHPI_UNCONFIGURED, can cause us to exit
	 * immediately, without trying to run the daemon any further.
	 */
	oh_get_global_param2(OPENHPI_UNCONFIGURED, &param);
	if (param.u.unconfigured != SAHPI_FALSE) {
                CRIT("OpenHPI is not configured. See config file.");
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
	}

        /* Initialize uid_utils */
        INFO("Initializing UID.");
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                CRIT("Unique ID intialization failed.");
                data_access_unlock();
                return rval;
        }

        /* Initialize handler table */
        oh_handlers.table = g_hash_table_new(g_int_hash, g_int_equal);
        /* Initialize domain table */
        oh_domains.table = g_hash_table_new(g_int_hash, g_int_equal);
        /* Initialize session table */
        oh_sessions.table = g_hash_table_new(g_int_hash, g_int_equal);
        /* Load plugins, create handlers and domains */
        oh_process_config(&config);

        INFO("Creating default domain.");
        oh_get_global_param2(OPENHPI_AUTOINSERT_TIMEOUT, &param);
        SaHpiTimeoutT ai_timeout = param.u.ai_timeout;
        INFO("Auto-Insert Timeout is %" PRId64 " nsec.", (int64_t)ai_timeout);
        oh_get_global_param2(OPENHPI_AUTOINSERT_TIMEOUT_READONLY, &param);
        SaHpiDomainCapabilitiesT caps = 0;
        if ( param.u.ai_timeout_readonly != SAHPI_FALSE ) {
                INFO("Auto-Insert Timeout is READ-ONLY.");
                caps = SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY;
        }

	rval = oh_create_domain(OH_DEFAULT_DOMAIN_ID,
	                        "DEFAULT",
	                        SAHPI_UNSPECIFIED_DOMAIN_ID,
	                        SAHPI_UNSPECIFIED_DOMAIN_ID,
	                        caps,
	                        ai_timeout);
        if (rval != SA_OK) {
	        data_access_unlock();
		CRIT("Could not create first domain!");
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
                WARN("Warning: Handlers were defined, but none loaded.");
        } else if (config.handlers_defined > 0 &&
                   config.handlers_loaded < config.handlers_defined) {
                WARN("*Warning*: Not all handlers defined loaded."
                     " Check previous messages.");
        }

        /* Start discovery and event threads */
	oh_threaded_start();

        initialized = 1;
        data_access_unlock();
	INFO("OpenHPI has been initialized.");

        /* infrastructure initialization has completed at this point */

        /* Check if there are any handlers loaded */
        if (config.handlers_defined == 0) {
                WARN("*Warning*: No handler definitions found in config file."
                     " Check configuration file and previous messages" );
        }

        /*
         * HACK: wait a second before returning
         * to give the threads time to populate the RPT
         */
        g_usleep(G_USEC_PER_SEC);

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
        data_access_lock();
        oh_close_handlers();
        data_access_unlock();

        oh_threaded_stop();

        oh_destroy_domain(OH_DEFAULT_DOMAIN_ID);
        g_hash_table_destroy(oh_sessions.table);
        g_hash_table_destroy(oh_domains.table);
        g_hash_table_destroy(oh_handlers.table);

#ifdef HAVE_OPENSSL
        oh_ssl_finit();
#endif

        oh_event_finit();

	INFO("OpenHPI has been finalized.");

        return 0;
}

