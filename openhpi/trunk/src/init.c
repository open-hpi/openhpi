/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <openhpi.h>

static enum {
        UNINIT = 0,
        INIT
} oh_init_state = UNINIT;

SaErrorT oh_initialized()
{
        if(oh_init_state == UNINIT) {                
                return SA_ERR_HPI_ERROR;
        } else {
                return SA_OK;
        }
}

/**
 * oh_initialize
 *
 * Returns:
 **/
SaErrorT oh_initialize()
{
        GSList *node = NULL;
        struct oh_parsed_config config = {NULL, NULL};
        struct oh_global_param config_param = { .type = OPENHPI_CONF };
        int rval;
        unsigned int u;
        
        SaHpiDomainCapabilitiesT capabilities = 0x00000000;
        SaHpiTextBufferT tag;
        
        data_access_lock();
        
        /* Check ready state */
        if (oh_initialized() == SA_OK) {
                dbg("Cannot initialize twice");
                data_access_unlock();
                return SA_ERR_HPI_DUPLICATE;
        }        

        /* Initialize event process queue */
        oh_event_init();
        
        /* Set openhpi configuration file location */        
        oh_get_global_param(&config_param);
        
        rval = oh_load_config(config_param.u.conf, &config);
        /* Don't error out if there is no conf file */
        if (rval < 0 && rval != -4) {
                dbg("Can not load config");
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        /* Initialize uid_utils */
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                dbg("uid_intialization failed");
                data_access_unlock();
                return(rval);
        }
        trace("Initialized UID");
        
        /* Initialize plugins */
        for (node = config.plugin_names; node; node = node->next) {
                char *plugin_name = (char *)node->data;
                if (oh_load_plugin(plugin_name) == 0) {
                        trace("Loaded plugin %s", plugin_name);
                } else {
                        dbg("Couldn't load plugin %s", plugin_name);
                        g_free(plugin_name);
                }
        }

        /* Initialize handlers */        
        for (node = config.handler_configs; node; node = node->next) {
                GHashTable *handler_config = (GHashTable *)node->data;
                if(oh_load_handler(handler_config) > 0) {
                        trace("Loaded handler for plugin %s",
                              (char *)g_hash_table_lookup(handler_config, "plugin"));
                } else {
                        dbg("Couldn't load handler for plugin %s",
                            (char *)g_hash_table_lookup(handler_config, "plugin"));
                        g_hash_table_destroy(handler_config);
                }
        }

        /*
         * Wipes away configuration lists (plugin_names and handler_configs).
         * global_params is not touched.
         */
        oh_clean_config();

	/* Check if there are any handler loaded */
	oh_lookup_next_handler(0, &u);
        if (!u) {
                /* there is no handler => this can not work */
                dbg("No handlers loaded after initialization.");
                dbg("Check configuration file %s", config_param.u.conf);
                /*data_access_unlock();*/
                /*return SA_ERR_HPI_NOT_PRESENT;*/
        }

        /* Initialize domain table */
        oh_domains.table = g_hash_table_new(g_int_hash, g_int_equal);
        trace("Initialized domain table");

        /* Create first domain */
        tag.DataType = SAHPI_TL_TYPE_TEXT;
        tag.Language = SAHPI_LANG_ENGLISH;
        strcpy((char *)tag.Data, "First Domain");
        if (!oh_create_domain(capabilities, SAHPI_FALSE, &tag)) {
                data_access_unlock();
                dbg("Could not create first domain!");
                return SA_ERR_HPI_ERROR;
        }
        trace("Created first domain");

        /* Initialize session table */
        oh_sessions.table = g_hash_table_new(g_int_hash, g_int_equal);
        trace("Initialized session table");

        /* this only does something if the config says to */
        oh_start_event_thread();

        oh_init_state = INIT;
        trace("Set init state");

        data_access_unlock();

        return SA_OK;
}

#if 0
/**
 * oh_finalize
 *
 * Returns:
 **/
SaErrorT oh_finalize()
{
        OH_STATE_READY_CHECK;

        /* free mutex */
        /* TODO: this wasn't here in branch, need to resolve history */
        data_access_lock();

        /* TODO: realy should have a oh_uid_finalize() that */
        /* frees memory,                                    */
        if(oh_uid_map_to_file())
                dbg("error writing uid entity path mapping to file");

        /* check for open sessions */
        if ( global_session_list ) {
                dbg("cannot saHpiFinalize because of open sessions" );
                data_access_unlock();
                return SA_ERR_HPI_BUSY;
        }

        /* close all plugins */
        while(global_handler_list) {
                struct oh_handler *handler = (struct oh_handler *)global_handler_list->data;
                /* unload_handler will remove handler from global_handler_list */
                unload_handler(handler);
        }

        /* unload plugins */
        while(global_plugin_list) {
                struct oh_plugin_config *plugin = (struct oh_plugin_config *)global_plugin_list->data;
                unload_plugin(plugin);
        }

        /* free global rpt */
        if (default_rpt) {
                oh_flush_rpt(default_rpt);
                g_free(default_rpt);
                default_rpt = 0;
        }

        /* free global_handler_configs and uninit_plugin */
        oh_unload_config();
        oh_exit_ltdl(); /* Free ltdl structures after unloading plugins */

        /* free domain list */
        oh_cleanup_domain();

        oh_hpi_state = OH_STAT_UNINIT;

        /* free mutex */
        data_access_unlock();

        return SA_OK;
}
#endif
