/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
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
 *      David Judkovics <djudkovi@us.ibm.com>
 *
 */

#include <oh_init.h>
#include <oh_config.h>
#include <oh_plugin.h>
#include <oh_domain.h>
#include <oh_session.h>
#include <oh_threaded.h>
#include <oh_error.h>
#include <oh_lock.h>
#include <oh_utils.h>

#include <oh_config.h>


/**
 * _init
 *
 * Returns: 0 on success otherwise an error code
 **/
int _init(void)
{
//            struct oh_parsed_config config = {NULL, NULL, 0, 0, 0, 0};
//            struct oh_global_param config_param = { .type = OPENHPI_CONF };
            SaErrorT rval;
            SaHpiDomainCapabilitiesT capabilities = 0x00000000;
            SaHpiTextBufferT tag;
        
            data_access_lock();

            /* Initialize thread engine */
            oh_threaded_init();

#if 0
            /* Set openhpi configuration file location */
            oh_get_global_param(&config_param);

            rval = oh_load_config(config_param.u.conf, &config);
            /* Don't error out if there is no conf file */
            if (rval < 0 && rval != -4) {
                    dbg("Can not load config.");
                    data_access_unlock();
                    return SA_ERR_HPI_NOT_PRESENT;
            }
#endif


            /* Initialize uid_utils */
            rval = oh_uid_initialize();
            if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                    dbg("Unique ID intialization failed.");
                    data_access_unlock();
                    return rval;
            }
            trace("Initialized UID.");

            /* Initialize handler table */
            oh_handlers.table = g_hash_table_new(g_int_hash, g_int_equal);
            trace("Initialized handler table");

            /* Initialize domain table */
            oh_domains.table = g_hash_table_new(g_int_hash, g_int_equal);
            trace("Initialized domain table");

            /* Create first domain */
            oh_init_textbuffer(&tag);
            oh_append_textbuffer(&tag,"First Domain");
            if (!oh_create_domain(capabilities, SAHPI_TIMEOUT_IMMEDIATE, &tag)) {
                    data_access_unlock();
                    dbg("Could not create first domain!");
                    return SA_ERR_HPI_ERROR;
            }
            trace("Created first domain");

            /* Initialize session table */
            oh_sessions.table = g_hash_table_new(g_int_hash, g_int_equal);
            trace("Initialized session table");


#if 0
            /* Set openhpi configuration file location */
            oh_get_global_param(&config_param);

            rval = oh_load_config(config_param.u.conf, &config);
            /* Don't error out if there is no conf file */
            if (rval < 0 && rval != -4) {
                    dbg("Can not load config.");
                    data_access_unlock();
                    return SA_ERR_HPI_NOT_PRESENT;
            }

            /* Load plugins and create handlers*/
            oh_process_config(&config);
            
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
                    data_access_unlock();
                    dbg("Error: Handlers were defined, but none loaded.");
                    return SA_ERR_HPI_ERROR;
            } else if (config.handlers_defined > 0 &&
                       config.handlers_loaded < config.handlers_defined) {
                    dbg("*Warning*: Not all handlers defined loaded."
                        " Check previous messages.");
            }

            /* this only does something if the config says to */
            //struct oh_global_param my_global_param;
            //my_global_param.type = OPENHPI_DAEMON_MODE;
            //oh_get_global_param(&my_global_param);
            //if (!my_global_param.u.daemon_mode) {
                oh_threaded_start();
                dbg("### We ARE Starting infrastructure threads in _init() ###\n");
            //} else {
            //    dbg("### We ARE NOT Starting infrastructure threads in _init() ###\n");
            //}


            trace("Set init state");

            /* infrastructure initialization has completed at this point */

            /* Check if there are any handlers loaded */
            if (config.handlers_defined == 0) {
                    dbg("*Warning*: No handler definitions found in config file."
                        " Check configuration file %s and previous messages",
                        config_param.u.conf);
            }
#endif
            
            /* load plugins and start discovery threads if not a daemon */
            if ( _initialize(FALSE) == SA_ERR_HPI_ERROR) return SA_ERR_HPI_ERROR;

            data_access_unlock();

            /*
             * HACK: If threaded mode is on, wait a second before returning
            * to give the threads time to populate the RPT
            */
            if (oh_threaded_mode()) {
                    struct timespec waittime =
                            { .tv_sec = 1, .tv_nsec = 1000L};
                            nanosleep(&waittime, NULL);
            }
            
        /* Do not use SA_OK here in case it is ever changed to something
         * besides zero, The runtime stuff depends on zero being returned here
         * in order for the shared library to be completely initialized.
         */

        return 0;

}


/**
 * _init
 *
 * Returns: 0 on success otherwise an error code
 **/
int _initialize(int daemon)
{

            struct oh_parsed_config config = {NULL, NULL, 0, 0, 0, 0};
            struct oh_global_param config_param = { .type = OPENHPI_CONF };
            SaErrorT rval;
        
            data_access_lock();

            /* Set openhpi configuration file location */
            oh_get_global_param(&config_param);

            rval = oh_load_config(config_param.u.conf, &config);
            /* Don't error out if there is no conf file */
            if (rval < 0 && rval != -4) {
                    dbg("Can not load config.");
                    data_access_unlock();
                    return SA_ERR_HPI_NOT_PRESENT;
            }

            /* this only does something if the config says to */
            struct oh_global_param my_global_param;
            my_global_param.type = OPENHPI_DAEMON_MODE;
            oh_get_global_param(&my_global_param);
            if ((my_global_param.u.daemon_mode) && (daemon == TRUE)) {


                /* Load plugins and create handlers*/
                oh_process_config(&config);
            
                /*
                * Wipes away configuration lists (plugin_names and handler_configs).
                * global_params is not touched.
                */
                oh_clean_config(&config);


                oh_threaded_start();
                dbg("### We ARE Starting infrastructure threads in _initialize() my_global_param.u.daemon_mode[%d] daemon[%d]###\n",
                    my_global_param.u.daemon_mode, daemon);

                /*
                * If any handlers were defined in the config file AND
                * all of them failed to load, Then return with an error.
                */
                if (config.handlers_defined > 0 && config.handlers_loaded == 0) {
                        data_access_unlock();
                        dbg("Error: Handlers were defined, but none loaded.");
                        return SA_ERR_HPI_ERROR;
                } else if (config.handlers_defined > 0 &&
                           config.handlers_loaded < config.handlers_defined) {
                        dbg("*Warning*: Not all handlers defined loaded."
                            " Check previous messages.");
                }

                trace("Set init state");

                /* infrastructure initialization has completed at this point */

                /* Check if there are any handlers loaded */
                if (config.handlers_defined == 0) {
                        dbg("*Warning*: No handler definitions found in config file."
                            " Check configuration file %s and previous messages",
                            config_param.u.conf);
                }
            } else {
                dbg("### We ARE NOT Starting infrastructure threads in _initialize() my_global_param.u.daemon_mode[%d] daemon[%d]###\n",
                    my_global_param.u.daemon_mode, daemon);
            }
            
            /* Do not use SA_OK here in case it is ever changed to something
            * besides zero, The runtime stuff depends on zero being returned here
            * in order for the shared library to be completely initialized.
            */

            data_access_unlock();

            return 0;

}

/**
 * _fini
 *
 * Returns: always returns 0
 **/
int _fini(void)
{

        data_access_lock();

        oh_close_handlers();

        data_access_unlock();

        return 0;

}

