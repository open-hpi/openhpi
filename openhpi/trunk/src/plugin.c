/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2004
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

/*
 * List of plugins (oh_plugin_config). This list is populated
 * by the plugin loader.
 */
GSList *plugin_list = NULL;

/*
 * Table of handlers (oh_handler). This list is populated
 * by the first call the saHpiSessionOpen().
 */
GHashTable *handler_table = NULL;


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

static struct oh_plugin *get_plugin(char *plugin_name)
{
        GSList *node = NULL;
        struct oh_plugin *plugin = NULL;

        if (!plugin_name) {
                dbg("Error getting plugin. Invalid parameter.");
                return NULL;
        }

        for (node = plugin_list; node != NULL; node = node->next) {
                struct oh_plugin *p = node->data;
                if(strcmp(p->name, plugin_name) == 0) {
                        plugin = p;
                        break;
                }
        }
                
        return plugin;
}

/*
static int get_plugin_refcount(char *plugin_name)
{
        struct oh_plugin *plugin = NULL;

        if (!plugin_name) {
                dbg("Error getting plugin refcount. Invalid parameter.");
                return -1;
        }

        plugin = get_plugin(plugin_name);

        if(!plugin) {
                return 0;
        }

        return plugin->refcount;
}
*/

/* list of static plugins. defined in plugin_static.c.in */
extern struct oh_static_plugin static_plugins[];

/*
 * Load plugin by name and make a instance.
 * FIXME: the plugins with multi-instances should reuse 'lt_dlhandler'
 */
int oh_load_plugin(char *plugin_name)
{
        struct oh_plugin *plugin = NULL;
        int (*get_interface) (struct oh_abi_v2 ** pp, const uuid_t uuid);
        int err;
        struct oh_static_plugin *p = static_plugins;        

        if (!plugin_name) {
                dbg("Error. NULL plugin name passed.");
                return -1;
        }

        if (get_plugin(plugin_name)) {
                dbg("Warning. Plugin %s already loaded. Not loading twice.",
                    plugin_name);
                return -1;
        }

        plugin = (struct oh_plugin *)g_malloc0(sizeof(struct oh_plugin));
        if (!plugin) {
                dbg("Out of memory.");
                return -1;
        }
        plugin->name = plugin_name;
        plugin->refcount = 1;

        /* first take search plugin in the array of static plugin */
        while( p->name ) {
                if (!strcmp(plugin->name, p->name)) {
                        plugin->dl_handle = 0;
                        err = (*p->get_interface)((void **)&plugin->abi, UUID_OH_ABI_V2);

                        if (err < 0 || !plugin->abi || !plugin->abi->open) {
                                dbg("Can not get ABI V1");
                                goto err1;
                        }

                        trace( "found static plugin %s", p->name );
                                                
                        plugin_list = g_slist_append(plugin_list, plugin);

                        return 0;
                }

                p++;
        }

        plugin->dl_handle = lt_dlopenext(plugin->name);
        if (plugin->dl_handle == NULL) {
                dbg("Can not open %s plugin: %s", plugin->name, lt_dlerror());
                goto err1;
        }

        get_interface = lt_dlsym(plugin->dl_handle, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }

        err = get_interface(&plugin->abi, UUID_OH_ABI_V2);
        if (err < 0 || !plugin->abi || !plugin->abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
        }
        plugin_list = g_slist_append(plugin_list, plugin);

        return 0;
        
 err1:
        if (plugin->dl_handle) {
                lt_dlclose(plugin->dl_handle);
                plugin->dl_handle = 0;
        }
        g_free(plugin);

        return -1;
}


void oh_unload_plugin(struct oh_plugin *plugin)
{
        if (!plugin) {
                dbg("ERROR unloading plugin. NULL parameter passed.");
                return;
        }

        if (plugin->dl_handle)
        {
                lt_dlclose(plugin->dl_handle);
                plugin->dl_handle = 0;
        }

        plugin_list = g_slist_remove(plugin_list, plugin);

        if (plugin->name)
                g_free(plugin->name);

        g_free(plugin);
}

static struct oh_handler *new_handler(GHashTable *handler_config)
{
        struct oh_plugin *plugin = NULL;
        struct oh_handler *handler = NULL;
        static unsigned int handler_id = 1;

        if (!handler_config) {
                dbg("ERROR creating new handler. Invalid parameter.");
                return NULL;
        }

        handler = (struct oh_handler *)g_malloc0(sizeof(struct oh_handler));
        if (!handler) {
                dbg("Out of Memory!");
                return NULL;
        }

        plugin = get_plugin((char *)g_hash_table_lookup(handler_config, "plugin"));
        if(!plugin || plugin->refcount < 1) {
                dbg("Attempt to create handler for unknown plugin %s",
                        (char *)g_hash_table_lookup(handler_config, "plugin"));
                goto err;
        }

        handler->abi = plugin->abi;
        handler->config = handler_config;

        /* FIXME: this should be done elsewhere.  if 0 it for now to make it
           easier to migrate */
        handler->hnd = handler->abi->open(handler->config);
        if (!handler->hnd) {
                dbg("A plugin instance could not be opened.");
                goto err;
        }
        handler->id = handler_id++;
        plugin->refcount++;

        return handler;
err:
        g_free(handler);
        return NULL;
}

int oh_load_handler (GHashTable *handler_config)
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

        g_hash_table_insert(handler_table,
                            &(handler->id),
                            handler);

        data_access_unlock();

        return 0;
}

void oh_unload_handler(struct oh_handler *handler)
{
        if (!handler) {
                dbg("ERROR unloading handler. Invalid handler passed.");
                return;
        }

        if (handler->abi && handler->abi->close)
                handler->abi->close(handler->hnd);

        data_access_lock();
        g_hash_table_remove(handler_table, &(handler->id));
        data_access_unlock();

        g_free(handler);
}
