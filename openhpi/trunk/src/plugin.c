/*      -*- linux-c -*-
 * 
 * Copyright (c) 2003 by Intel Corp.
 * Copyright (c) 2003 by International Business Machines.
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
 *     Sean Dague <sean@dague.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ltdl.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_config.h>

/*******************************************************************************
 * init_plugin - does all the initialization needed for the ltdl process to
 * work.  It takes no arguments, and returns 0 on success, < 0 on error
 *******************************************************************************/

int init_plugin()
{
        int err;
        
        err = lt_dlinit();
        if (err != 0) {
                dbg("Can not init ltdl");
                goto err1;
        }
        
        err = lt_dlsetsearchpath(OH_PLUGIN_PATH);
        if (err != 0) {
                dbg("Can not set lt_dl search path");
                goto err2;
        }
        
        return 0;
        
 err2:
        lt_dlexit();
 err1:
        return -1;
}

/*******************************************************************************
 * load_plugin - loads a plugin by name
 *
 *******************************************************************************/

int load_plugin(struct oh_plugin_config *config)
{
        lt_dlhandle h;
        int (*get_interface) (struct oh_abi_v1 ** pp, const uuid_t uuid);
        int err;

        h = lt_dlopenext(config->name);
        if (h == NULL) {
                dbg("Can not find %s plugin", config->name);
                goto err1;
        }

        get_interface = lt_dlsym(h, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }
        
        err = get_interface(&config->abi, UUID_OH_ABI_V1);
        if (err < 0 || !config->abi || !config->abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
        }
        
        return 0;
 err1:
        lt_dlclose(h);
        return -1;
}

int uninit_plugin(void)
{
	int rv;

	rv = lt_dlexit();
	if (rv < 0) {
		dbg("Can not exit ltdl right");
		return -1;
	}
	return 0;
}

int load_handler (GHashTable *handler_config)
{
        struct oh_handler *handler;

        handler = new_handler(handler_config);

        if(handler == NULL) {
                return -1;
        }
        
        global_handler_list = g_slist_append(
                global_handler_list,
                (gpointer) handler
                );
        
        return 0;
}

/*
 * Load plugin by name and make a instance.
 * FIXME: the plugins with multi-instances should reuse 'lt_dlhandler'
 */

struct oh_handler *new_handler(GHashTable *handler_config)
{
        struct oh_plugin_config *p_config;
        struct oh_handler *handler;
        
        handler = malloc(sizeof(*handler));
        if (!handler) {
                dbg("Out of Memory!");
                goto err;
        }
        memset(handler, '\0', sizeof(*handler));
        
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
#if 0        
        err = get_interface(&handler->abi, UUID_OH_ABI_V1);
        if (err < 0 || !handler->abi || !handler->abi->open) {
                dbg("Can not get ABI V1");
                goto err;
        }
#endif

        /* this should be done elsewhere.  if 0 it for now to make it
           easier to migrate */
        handler->hnd = handler->abi->open(handler_config);
        if (!handler->hnd) {
                dbg("The plugin can not work");
                goto err;
        }
        
        return handler;
err:
 	free(handler);
        return NULL;
}

