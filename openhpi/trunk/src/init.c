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

#include <oh_init.h>
#include <openhpi.h>
#include <uid_utils.h>
#include <sahpimacros.h>

/**
 * oh_initialize
 *
 * Returns:
 **/
SaErrorT oh_initialize()
{
        struct oh_plugin_config *tmpp;
        GHashTable *tmph;

        unsigned int i;
        char *openhpi_conf;

        int rval;

        data_access_lock();

        if (OH_STAT_UNINIT != oh_hpi_state) {
                dbg("Cannot initialize twice");
                data_access_unlock();
                return SA_ERR_HPI_DUPLICATE;
        }

        /* initialize mutex used for data locking */
        /* in the future may want to add seperate */
        /* mutexes, one for each hash list        */

        /* set up our global domain */
        if (add_domain(SAHPI_UNSPECIFIED_DOMAIN_ID)) {
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }

        /* setup our global rpt_table */
        default_rpt = g_malloc0(sizeof(RPTable));
        if(!default_rpt) {
                dbg("Couldn't allocate RPT for Default Domain");
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }
        default_rpt->rpt_info.UpdateTimestamp = SAHPI_TIME_UNSPECIFIED;

        /* initialize uid_utils, and load uid map file if present */
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                dbg("uid_intialization failed");
                data_access_unlock();
                return(rval);
        }

        openhpi_conf = getenv("OPENHPI_CONF");

        if (openhpi_conf == NULL) {
                openhpi_conf = OH_DEFAULT_CONF;
        }

        if (oh_load_config(openhpi_conf) < 0) {
                dbg("Can not load config");
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        for(i = 0; i < g_slist_length(global_plugin_list); i++) {
                tmpp = (struct oh_plugin_config *) g_slist_nth_data(
                        global_plugin_list, i);
                if(load_plugin(tmpp) == 0) {
                        dbg("Loaded plugin %s", tmpp->name);
                        tmpp->refcount++;
                } else {
                        dbg("Couldn't load plugin %s", tmpp->name);
                }
        }

        for(i = 0; i < g_slist_length(global_handler_configs); i++) {
                tmph = (GHashTable *) g_slist_nth_data(
                        global_handler_configs, i);
                if(plugin_refcount((char *)g_hash_table_lookup(tmph, "plugin")) > 0) {
                        if(load_handler(tmph) == 0) {
                                dbg("Loaded handler for plugin %s",
                                        (char *)g_hash_table_lookup(tmph, "plugin"));
                        } else {
                                dbg("Couldn't load handler for plugin %s",
                                        (char *)g_hash_table_lookup(tmph, "plugin"));
                        }
                } else {
                        dbg("load handler for unknown plugin %s",
                                (char *)g_hash_table_lookup(tmph, "plugin"));
                }
        }

        oh_hpi_state = OH_STAT_READY;

        /* check if we have at least one handler */
        if ( global_handler_list == 0 ) {
                /* there is no handler => this can not work */
                dbg("no handler found. please check %s!", openhpi_conf);

                data_access_unlock();
                oh_finalize();

                return SA_ERR_HPI_NOT_PRESENT;
        }

        data_access_unlock();

        return SA_OK;
}

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

        /* free domain list */
        oh_cleanup_domain();

        oh_hpi_state = OH_STAT_UNINIT;

        /* free mutex */
        data_access_unlock();

        return SA_OK;
}

