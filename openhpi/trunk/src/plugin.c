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
 *     Sean Dague <http://dague.net/sean>
 * Contributors:
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Renier Morales <renierm@users.sourceforge.net>
 */

#include <string.h>
#include <ltdl.h>

#include <glib.h>
#include <oh_plugin.h>
#include <oh_config.h>
#include <oh_error.h>
#include <oh_lock.h>
#include <config.h>

extern GCond *oh_thread_wait;

void oh_cond_signal(void)
{
        g_cond_signal(oh_thread_wait);
}

/**
 * oh_init_ltdl
 *
 * Does all the initialization needed for the ltdl process to
 * work.  It takes no arguments, and returns 0 on success, < 0 on error
 *
 * Returns: 0 on success.
 **/
int oh_init_ltdl()
{
        char * path = NULL;
        int err;

        err = lt_dlinit();
        if (err != 0) {
                dbg("Can not init ltdl");
                return -1;
        }

        path = getenv("OPENHPI_PLUGINS");
        if(path == NULL) {
                path = OH_PLUGIN_PATH;
        }

        err = lt_dlsetsearchpath(path);
        if (err != 0) {
                dbg("Can not set lt_dl search path");
                oh_exit_ltdl();
                return -1;
        }

        return 0;
}

/**
 * oh_exit_ltdl
 *
 * Does everything needed to close the ltdl structures.
 **/
void oh_exit_ltdl()
{
        int rv;

        rv = lt_dlexit();
        if (rv < 0)
                dbg("Could not exit ltdl!");
}

/*******************************************************************************
 * load_plugin - loads a plugin by name
 *
 *******************************************************************************/

/* list of static plugins. defined in plugin_static.c.in */
extern struct oh_static_plugin static_plugins[];

int load_plugin(struct oh_plugin_config *config)
{
        int (*get_interface) (struct oh_abi_v2 ** pp, const uuid_t uuid);
        int err;
        struct oh_static_plugin *p = static_plugins;

        if (!config) {
                dbg("ERROR loading plugin. NULL plugin configuration passed.");
                return -1;
        }

        /* first take search plugin in the array of static plugin */
        while( p->name ) {
                if ( !strcmp( config->name, p->name ) ) {
                        err = (*p->get_interface)( (void **)&config->abi, UUID_OH_ABI_V2);

                        if (err < 0 || !config->abi || !config->abi->open) {
                                dbg("Can not get ABI V1");
                                goto err1;
                        }

                        trace( "found static plugin %s", p->name );

                        config->dl_handle = 0;

                        return 0;
                }

                p++;
        }

        config->dl_handle = lt_dlopenext(config->name);
        if (config->dl_handle == NULL) {
                dbg("Can not open %s plugin: %s", config->name, lt_dlerror());
                goto err1;
        }

        get_interface = lt_dlsym(config->dl_handle, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }

        err = get_interface(&config->abi, UUID_OH_ABI_V2);
        if (err < 0 || !config->abi || !config->abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
        }

        return 0;
 err1:
        if (config->dl_handle) {
                lt_dlclose(config->dl_handle);
                config->dl_handle = 0;
        }

        return -1;
}


void unload_plugin(struct oh_plugin_config *config)
{
        if (!config) {
                dbg("ERROR unloading plugin. Invalid plugin configuration passed.");
                return;
        }

        if (config->dl_handle)
        {
                lt_dlclose(config->dl_handle);
                config->dl_handle = 0;
        }

        global_plugin_list = g_slist_remove(global_plugin_list, config);

        if (config->name)
                free(config->name);

        free(config);
}

/*
 * Load plugin by name and make a instance.
 * FIXME: the plugins with multi-instances should reuse 'lt_dlhandler'
 */

static struct oh_handler *new_handler(GHashTable *handler_config)
{
        struct oh_plugin_config *p_config;
        struct oh_handler *handler;
        static unsigned int handler_id = 1;

        if (!handler_config) {
                dbg("ERROR creating new handler");
                return NULL;
        }

        handler = g_malloc0(sizeof(struct oh_handler));
        if (!handler) {
                dbg("Out of Memory!");
                return NULL;
        }

        if(plugin_refcount((char *)g_hash_table_lookup(handler_config, "plugin")) < 1) {
                dbg("Attempt to create handler for unknown plugin %s",
                        (char *)g_hash_table_lookup(handler_config, "plugin"));
                goto err;
        }

        p_config = plugin_config((char *)g_hash_table_lookup(handler_config, "plugin"));
        if(p_config == NULL) {
                dbg("No such plugin config");
                goto err;
        }

        handler->abi = p_config->abi;
        handler->config = handler_config;

        /* FIXME: this should be done elsewhere.  if 0 it for now to make it
           easier to migrate */
        handler->hnd = handler->abi->open(handler->config);
        if (!handler->hnd) {
                dbg("A plugin instance could not be opened.");
                goto err;
        }
        handler->id = handler_id++;

        return handler;
err:
        g_free(handler);
        return NULL;
}

int load_handler (GHashTable *handler_config)
{
        struct oh_handler *handler;

        if (!handler_config) {
                dbg("ERROR loading handler. Invalid handler configuration passed.");
                return -1;
        }

        data_access_lock();

        handler = new_handler(handler_config);

        if(handler == NULL) {
                data_access_unlock();
                return -1;
        }

        g_hash_table_insert(global_handler_table,
                            &(handler->id),
                            handler);

        data_access_unlock();

        return 0;
}

void unload_handler(struct oh_handler *handler)
{
        if (!handler) {
                dbg("ERROR unloading handler. Invalid handler passed.");
                return;
        }

        if (handler->abi && handler->abi->close)
                handler->abi->close(handler->hnd);

        g_hash_table_remove(global_handler_table, &(handler->id));

        free(handler);
}
