/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003, 2005
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
#include <oh_domain.h>
#include <config.h>

/*
 * List of plugins (oh_plugin).
 */
static GSList *plugin_list = NULL;
/*
 * Table of handlers (oh_handler).
 */
static GHashTable *handler_table = NULL;
/*
 * List of handler ids (unsigned int).
 */
static GSList *handler_ids = NULL;

/**
 * close_handler_atexit
 *
 * When a client calls exit() this function
 * gets called to clean up the handlers
 * and any handler connections to the hardware.
 * Until clients use dynamic oHPI interface
 **/
static void close_handlers_atexit(void)
{
      	struct oh_handler *handler = NULL;
	GSList *node = NULL;
	unsigned int *id;

	if (handler_ids == NULL) {
	      	return;
	}

	for (node = handler_ids; node; node=node->next) {
	      	id = node->data;
		handler = g_hash_table_lookup(handler_table, id);
		if(handler->abi && handler->abi->close) {
		      	handler->abi->close(handler->hnd);
		}
	}
}
	      
extern GCond *oh_event_thread_wait;
extern GMutex *oh_event_thread_mutex;

void oh_wake_event_thread(SaHpiBoolT wait)
{
        if(oh_threaded_mode()) {
                trace("Waking thread");
                g_cond_broadcast(oh_event_thread_wait);
                if(wait) {
                        /* the wait concept is important.  By taking these locks
                           we ensure that the thread is forced to go through
                           at least one cycle (though it could be 2 based on 
                           racing) This is important for the infrastructure on
                           calls like discover, which need to know *now* what
                           is going on.  Plugins probably don't care, but we
                           leave it as an option anyway. */
                        g_mutex_lock(oh_event_thread_mutex);
                        trace("Got the lock on the thread");
                        g_mutex_unlock(oh_event_thread_mutex);
                        trace("Gave back the thread lock");
                }
	}
}

/**
 * oh_init_ltdl
 *
 * Does all the initialization needed for the ltdl process to
 * work. It takes no arguments, and returns 0 on success, < 0 on error
 *
 * Returns: 0 on Success.
 **/
static int oh_init_ltdl(void)
{
        struct oh_global_param path_param = { .type = OPENHPI_PATH };
        int err;
        static int init_done = 0;
        
        data_access_lock();
        if (init_done) {
                data_access_unlock();
                return 0;
        }

        err = lt_dlinit();
        if (err != 0) {
                dbg("Can not init ltdl");
                data_access_unlock();
                return -1;
        }
        
        oh_get_global_param(&path_param);

        err = lt_dlsetsearchpath(path_param.u.path);
        if (err != 0) {
                dbg("Can not set lt_dl search path");
                oh_exit_ltdl();
                data_access_unlock();
                return -1;
        }
        
        init_done = 1;
        data_access_unlock();

        return 0;
}

/**
 * oh_exit_ltdl
 *
 * Does everything needed to close the ltdl structures.
 *
 * Returns: 0 on Success.
 **/
int oh_exit_ltdl()
{
        int rv;

        rv = lt_dlexit();
        if (rv < 0) {
                dbg("Could not exit ltdl!");
                return -1;
        }

        return 0;
}

/**
 * oh_lookup_plugin
 * @plugin_name
 *
 * Lookup and get a reference for @plugin_name.
 *
 * Returns: oh_plugin reference or NULL if plugin_name was not found.
 **/
struct oh_plugin *oh_lookup_plugin(char *plugin_name)
{
        GSList *node = NULL;
        struct oh_plugin *plugin = NULL;

        if (!plugin_name) {
                dbg("ERROR getting plugin. Invalid parameter.");
                return NULL;
        }

        data_access_lock();
        for (node = plugin_list; node != NULL; node = node->next) {
                struct oh_plugin *p = node->data;
                if(strcmp(p->name, plugin_name) == 0) {
                        plugin = p;
                        break;
                }
        }
        data_access_unlock();

        return plugin;
}

/**
 * oh_lookup_next_plugin
 * @plugin_name: IN. If NULL is passed, the first plugin in the list is returned.
 * @next_plugin_name: OUT. Buffer to print the next plugin name to.
 * @size: IN. Size of the buffer at @next_plugin_name.
 *
 * Returns: 0 on Success.
 **/
int oh_lookup_next_plugin(char *plugin_name,
                          char *next_plugin_name,
                          unsigned int size)
{
        GSList *node = NULL;

        if (!next_plugin_name) {
                dbg("ERROR. Invalid parameter.");
                return -1;
        }
        memset(next_plugin_name, '\0', size);

        data_access_lock();
        if (!plugin_name) {
                if (plugin_list) {
                        struct oh_plugin *plugin = plugin_list->data;
                        strncpy(next_plugin_name, plugin->name, size);
                        data_access_unlock();
                        return 0;
                } else {
                        trace("No plugins have been loaded yet.");
                        data_access_unlock();
                        return -1;
                }
        } else {
                for (node = plugin_list; node != NULL; node = node->next) {
                        struct oh_plugin *p = node->data;
                        if (strcmp(p->name, plugin_name) == 0) {
                                if (node->next) {
                                        p = node->next->data;
                                        strncpy(next_plugin_name, p->name, size);
                                        data_access_unlock();
                                        return 0;
                                } else
                                        break;
                        }
                }
        }
        data_access_unlock();

        return -1;
}

/* list of static plugins. defined in plugin_static.c.in */
extern struct oh_static_plugin static_plugins[];
/**
 * oh_unload_plugin
 * @plugin_name
 *
 * Load plugin by name and make a instance.
 *
 * Returns: 0 on Success.
 **/
int oh_load_plugin(char *plugin_name)
{
        
	struct oh_plugin *plugin = NULL;
        /*int (*get_interface) (struct oh_abi_v2 ** pp, const uuid_t uuid); */
        int err;
        struct oh_static_plugin *p = static_plugins;

        if (!plugin_name) {
                dbg("ERROR. NULL plugin name passed.");
                return -1;
        }

        data_access_lock();
        if (oh_init_ltdl()) {                
                data_access_unlock();
                dbg("ERROR. Could not initialize ltdl for loading plugins.");
                return -1;
        }
        
        if (oh_lookup_plugin(plugin_name)) {
                dbg("Warning. Plugin %s already loaded. Not loading twice.",
                    plugin_name);
                data_access_unlock();
                return -1;
        }

        plugin = (struct oh_plugin *)g_malloc0(sizeof(struct oh_plugin));
        if (!plugin) {
                dbg("Out of memory.");
                data_access_unlock();
                return -1;
        }
        plugin->name = g_strdup(plugin_name);
        plugin->refcount = 1;
        
        /* first take search plugin in the array of static plugin */
        while( p->name ) {
                if (!strcmp(plugin->name, p->name)) {
                        plugin->dl_handle = 0;
                        err = (*p->get_interface)((void **)&plugin->abi, UUID_OH_ABI_V2);

                        if (err < 0 || !plugin->abi || !plugin->abi->open) {
                                dbg("Can not get ABI V2");
                                goto err1;
                        }

                        trace( "found static plugin %s", p->name );

                        plugin_list = g_slist_append(plugin_list, plugin);
                        data_access_unlock();

                        return 0;
                }

                p++;
        }

        plugin->dl_handle = lt_dlopenext(plugin->name);
        if (plugin->dl_handle == NULL) {
                dbg("Can not open %s plugin: %s", plugin->name, lt_dlerror());
                goto err1;
        }

	err = oh_load_plugin_functions(plugin, &plugin->abi);
	
	if (err < 0 || !plugin->abi || !plugin->abi->open) {
                dbg("Can not get ABI");
                goto err1;
	}
	
        plugin_list = g_slist_append(plugin_list, plugin);
        data_access_unlock();

        return 0;

 err1:
        if (plugin->dl_handle) {
                lt_dlclose(plugin->dl_handle);
                plugin->dl_handle = 0;
        }
        data_access_unlock();
        g_free(plugin);

        return -1;
}

/**
 * oh_unload_plugin
 * @plugin_name
 *
 * Returns: 0 on Success.
 **/
int oh_unload_plugin(char *plugin_name)
{
        struct oh_plugin *plugin = NULL;

        if (!plugin_name) {
                dbg("ERROR unloading plugin. NULL parameter passed.");
                return -1;
        }
        
        data_access_lock();

        plugin = oh_lookup_plugin(plugin_name);
        if (!plugin) {
                dbg("ERROR unloading plugin. Plugin not found.");
                data_access_unlock();
                return -2;
        }        

        if (plugin->refcount > 1) {
                dbg("ERROR unloading plugin. Handlers are still referencing it.");
                data_access_unlock();
                return -3;
        }

        if (plugin->dl_handle) {
                lt_dlclose(plugin->dl_handle);
                plugin->dl_handle = 0;
        }

        plugin_list = g_slist_remove(plugin_list, plugin);
        data_access_unlock();

        if (plugin->name)
                g_free(plugin->name);

        g_free(plugin);

        return 0;
}

static int oh_init_handler_table(void)
{
        if (!handler_table)
                handler_table = g_hash_table_new(g_int_hash, g_int_equal);
                
        return (handler_table) ? 0 : -1;
}

/**
 * oh_lookup_handler
 * @hid
 *
 * Returns: NULL on Failure.
 **/
struct oh_handler *oh_lookup_handler(unsigned int hid)
{
        struct oh_handler *handler = NULL;

        data_access_lock();
        if (!handler_table) {
                data_access_unlock();                
                return NULL;
        }
        
        handler = g_hash_table_lookup(handler_table, &hid);
        data_access_unlock();

        return handler;
}

/**
 * oh_lookup_next_handler
 * @hid: If 0, will return the first handler id in the list.
 * @next_hid
 *
 * Returns: 0 on Success.
 **/
int oh_lookup_next_handler(unsigned int hid, unsigned int *next_hid)
{
        GSList *node = NULL;

        if (!next_hid) {
                dbg("ERROR. Invalid parameter.");
                return -1;
        }
        *next_hid = 0;

        data_access_lock();
        if (!hid) {
                if (handler_ids) {
                        unsigned int *id = handler_ids->data;
                        *next_hid = *id;
                        data_access_unlock();
                        return 0;
                } else {
                        data_access_unlock();
                        return -1;
                }
        } else {
                for (node = handler_ids; node; node = node->next) {
                        unsigned int *id = node->data;
                        if (*id == hid) {
                                if (node->next) {
                                        id = node->next->data;
                                        *next_hid = *id;
                                        data_access_unlock();
                                        return 0;
                                } else
                                        break;
                        }
                }
        }
        data_access_unlock();

        return -1;
}

static struct oh_handler *new_handler(GHashTable *handler_config)
{
        struct oh_plugin *plugin = NULL;
        struct oh_handler *handler = NULL;
        static unsigned int handler_id = 1;
	unsigned int *hidp;
	char *hid_strp;

        if (!handler_config) {
                dbg("ERROR creating new handler. Invalid parameter.");
                return NULL;
        }

        handler = (struct oh_handler *)g_malloc0(sizeof(struct oh_handler));
        if (!handler) {
                dbg("Out of Memory!");
                return NULL;
        }
	hidp = (unsigned int *)g_malloc(sizeof(unsigned int));
        if (!hidp) {
                dbg("Out of Memory!");
                return NULL;
        }
	hid_strp = strdup("handler-id");
        if (!hid_strp) {
                dbg("Out of Memory!");
                return NULL;
        }
        
	plugin = oh_lookup_plugin((char *)g_hash_table_lookup(handler_config, "plugin"));
        if(!plugin || plugin->refcount < 1) {
                dbg("Attempt to create handler for unknown plugin %s",
                        (char *)g_hash_table_lookup(handler_config, "plugin"));
                goto err;
        }

        handler->plugin_name = g_strdup(plugin->name);
        handler->abi = plugin->abi;
        handler->config = handler_config;
        handler->dids = NULL;
	handler->id = handler_id++;
        plugin->refcount++;
	*hidp = handler->id;
	g_hash_table_insert(handler_config, (gpointer)hid_strp,(gpointer)hidp);

        return handler;
err:
        g_free(handler);
        return NULL;
}

/**
 * oh_load_handler
 * @handler_config
 *
 * Returns: 0 on Failure, otherwise the handler id
 **/
unsigned int oh_load_handler (GHashTable *handler_config)
{
      	struct oh_handler *handler;
	static int first_handler = 1;

        if (!handler_config) {
                dbg("ERROR loading handler. Invalid handler configuration passed.");
                return 0;
        }

        data_access_lock();
        if (oh_init_handler_table()) {
                data_access_unlock();
                dbg("ERROR. Could not initialize handler table.");
                return 0;
        }

        handler = new_handler(handler_config);

        if (handler == NULL) {
                data_access_unlock();
                return 0;
        }

        g_hash_table_insert(handler_table,
                            &(handler->id),
                            handler);
        handler_ids = g_slist_append(handler_ids, &(handler->id));

	handler->hnd = handler->abi->open(handler->config);
        if (!handler->hnd) {
                dbg("A plugin instance could not be opened.");
		g_hash_table_remove(handler_table, &(handler->id));
		handler_ids = g_slist_remove(handler_ids, &(handler->id));
		g_free(handler);
                data_access_unlock();
                return 0;
        }

	/* register atexit callback to close handlers
	 * and handler connections to avoid zombies
	 */
	if (first_handler) {
	      	(void) atexit(close_handlers_atexit);
		first_handler = 0;
	}
	

        data_access_unlock();

        return handler->id;
}

/**
 * oh_unload_handler
 * @hid
 *
 * Returns: 0 on Success.
 **/
int oh_unload_handler(unsigned int hid)
{
        struct oh_handler *handler = NULL;
        struct oh_plugin *plugin = NULL;

        if (!hid) {
                dbg("ERROR unloading handler. Invalid handler id passed.");
                return -1;
        }

        data_access_lock();
        handler = oh_lookup_handler(hid);
        if (!handler) {
                dbg("ERROR unloading handler. Handler not found.");
                data_access_unlock();
                return -1;
        }
        
        if (handler->abi && handler->abi->close)
                handler->abi->close(handler->hnd);

        g_hash_table_remove(handler_table, &(handler->id));
        handler_ids = g_slist_remove(handler_ids, &(handler->id));
        plugin = oh_lookup_plugin(handler->plugin_name);
        if (!plugin) {
                dbg("WHAT?! Handler loaded, but plugin does not exist!");                
        } else {
                plugin->refcount--;
        }
        data_access_unlock();
        
        g_free(handler->plugin_name);
        g_free(handler);

        return 0;
}


int oh_domain_served_by_handler(unsigned int h_id,
				SaHpiDomainIdT did)
{
	gint i;
	struct oh_handler *h;
	
	data_access_lock();
	h = oh_lookup_handler(h_id);
	if (h == NULL) {
		data_access_unlock();
		return 0;
	}
	if (h->dids == NULL) {
		// plugin didn't create domains, it serves default domain
		data_access_unlock();
		return (did == oh_get_default_domain_id());
	}
		
	for (i = 0; ; i++) {
		if (g_array_index(h->dids, SaHpiDomainIdT, i) == 0) {
			break;
		}
		if (g_array_index(h->dids, SaHpiDomainIdT, i) == did) {
			data_access_unlock();
			return 1;
		}
	}
	data_access_unlock();
	return 0;
}

int oh_add_domain_to_handler(unsigned int handler_id,
			     SaHpiDomainIdT did)
{
	SaHpiDomainIdT default_did;
	struct oh_handler *h;
	
	data_access_lock();
	h = oh_lookup_handler(handler_id);
	if (h == NULL) {
		data_access_unlock();
		return -1;
	}
	if (h->dids == NULL) {
		h->dids =
			g_array_new(TRUE, FALSE, sizeof(SaHpiDomainIdT));
		if (h->dids == NULL) {
			data_access_unlock();
			return -1;
		}
		default_did = oh_get_default_domain_id();
		h->dids = g_array_append_val(h->dids, default_did);
	}
	h->dids = g_array_append_val(h->dids, did);
	data_access_unlock();
	return 0;
}

int oh_remove_domain_from_handler(unsigned int h_id,
				SaHpiDomainIdT did)
{
	gint i;
	struct oh_handler *h;
	
	data_access_lock();
	h = oh_lookup_handler(h_id);
	if (h == NULL) {
		data_access_unlock();
		return -1;
	}
	for (i = 0; ; i++) {
		if (g_array_index(h->dids, SaHpiDomainIdT, i) == 0) {
			break;
		}
		if (g_array_index(h->dids, SaHpiDomainIdT, i) == did) {
			h->dids = g_array_remove_index_fast(h->dids, i);
			data_access_unlock();
			return 0;
		}
	}
	data_access_unlock();
	return -1;
}

/**
 * oh_domain_resource_discovery
 * @did:
 *
 *
 *
 * Returns:
 **/
int oh_domain_resource_discovery(SaHpiDomainIdT did)
{
        data_access_lock();
        oh_lookup_next_handler(hid, &next_hid);
        while (next_hid) {
                hid = next_hid;
                h = oh_lookup_handler(hid);
		if (oh_domain_served_by_handler(hid, did)) {
			if (h->abi->discover_domain_resources != NULL) {
				if (h->abi->discover_domain_resources(h->hnd,
						did) == SA_OK && rv) {
					rv = SA_OK;
				}
                	} else if (h->abi->discover_resources(h->hnd) == SA_OK && rv)
                        	rv = SA_OK;
		}

                oh_lookup_next_handler(hid, &next_hid);
        }
        data_access_unlock();
}

/**
 * oh_load_plugin_functions
 * @plugin: plugin structure.
 * @abi: oh_abi struct
 *
 * This function will load the symbol table from the plugin name and 
 * assign the plugin functions to the abi struct. 
 *
 * Return value: 0 on success, otherwise any negative value on failure.
 **/
int oh_load_plugin_functions(struct oh_plugin *plugin, struct oh_abi_v2 **abi)
{  	
        
	*abi = (struct oh_abi_v2 *)g_malloc0(sizeof(struct oh_abi_v2));         
	
	 
        if (!(*abi)) {
                dbg("Out of Memory!");
                return -1;
        }
	
	(*abi)->open			  = lt_dlsym(plugin->dl_handle,
	  					"oh_open");
	(*abi)->close			  = lt_dlsym(plugin->dl_handle,
	  					"oh_close");		    
	(*abi)->get_event		  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_event");
	(*abi)->discover_resources	  = lt_dlsym(plugin->dl_handle,
	  					"oh_discover_resources");
	(*abi)->discover_domain_resources = lt_dlsym(plugin->dl_handle,
	  					"oh_discover_domain_resource");
	(*abi)->set_resource_tag	  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_resource_tag");
	(*abi)->set_resource_severity	  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_resource_severity");
	(*abi)->get_el_info		  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_el_info");
	(*abi)->set_el_time		  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_el_time");
	(*abi)->add_el_entry		  = lt_dlsym(plugin->dl_handle,
	  					"oh_add_el_entry");
	(*abi)->get_el_entry		  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_el_entry");
	(*abi)->clear_el		  = lt_dlsym(plugin->dl_handle,
	  					"oh_clear_el");
	(*abi)->set_el_state		  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_el_state");
	(*abi)->reset_el_overflow	  = lt_dlsym(plugin->dl_handle,
	  					"oh_reset_el_overflow");
	(*abi)->get_sensor_reading	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_sensor_reading");
	(*abi)->get_sensor_thresholds	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_sensor_thresholds");
	(*abi)->set_sensor_thresholds	  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_sensor_thresholds");
	(*abi)->get_sensor_enable	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_sensor_enable");
	(*abi)->set_sensor_enable	  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_sensor_enable");				    
	(*abi)->get_sensor_event_enables  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_sensor_event_enables");
	(*abi)->set_sensor_event_enables  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_sensor_event_enables");
	(*abi)->get_sensor_event_masks    = lt_dlsym(plugin->dl_handle,
	  					"oh_get_sensor_event_masks");
	(*abi)->set_sensor_event_masks    = lt_dlsym(plugin->dl_handle,
	  					"oh_set_sensor_event_masks");
	(*abi)->get_control_state	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_control_state");
	(*abi)->set_control_state	  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_control_state");
	(*abi)->get_idr_info		  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_idr_info");
	(*abi)->get_idr_area_header	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_idr_area_header");
	(*abi)->add_idr_area		  = lt_dlsym(plugin->dl_handle,
	  					"oh_add_idr_area");
	(*abi)->del_idr_area		  = lt_dlsym(plugin->dl_handle,
	  					"oh_del_idr_area");
	(*abi)->get_idr_field		  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_idr_field");
	(*abi)->add_idr_field		  = lt_dlsym(plugin->dl_handle,
	  					"oh_add_idr_field");
	(*abi)->set_idr_field		  = lt_dlsym(plugin->dl_handle,
	  					"oh_set_idr_field");
	(*abi)->del_idr_field		  = lt_dlsym(plugin->dl_handle,
	  					"oh_del_idr_field");
	(*abi)->get_watchdog_info	  = lt_dlsym(plugin->dl_handle,
	  					"oh_get_watchdog_info");
	(*abi)->set_watchdog_info	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_watchdog_info");
	(*abi)->reset_watchdog  	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_reset_watchdog");
	(*abi)->get_next_announce	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_next_announce");
	(*abi)->get_announce		  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_announce");
	(*abi)->ack_announce		  = lt_dlsym(plugin->dl_handle,
	                                        "oh_ack_announce");
	(*abi)->add_announce		  = lt_dlsym(plugin->dl_handle,
	                                        "oh_add_announce");
	(*abi)->del_announce		  = lt_dlsym(plugin->dl_handle,
	                                        "oh_del_announce");
	(*abi)->get_annunc_mode 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_annunc_mode");
	(*abi)->set_annunc_mode 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_annunc_mode");
	(*abi)->get_hotswap_state	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_hotswap_state");
	(*abi)->set_hotswap_state	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_hotswap_state");
	(*abi)->request_hotswap_action    = lt_dlsym(plugin->dl_handle,
	                                        "oh_request_hotswap_action");
	(*abi)->get_power_state 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_power_state");
	(*abi)->set_power_state 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_power_state");
	(*abi)->get_indicator_state	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_indicator_state");
	(*abi)->set_indicator_state	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_indicator_state");
	(*abi)->control_parm		  = lt_dlsym(plugin->dl_handle,
	                                        "oh_control_parm");
	(*abi)->get_reset_state 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_get_reset_state");
        (*abi)->set_reset_state 	  = lt_dlsym(plugin->dl_handle,
	                                        "oh_set_reset_state");

	return 0;
  
}  
