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

/*
 * This initializes the plugin loader.  Don't load anything
 * right now though.
 */

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

#if 0
/*
 * Load plugin by name.  This needs to be done before the plugin is used
 */

int load_plugin(const char *plugin_name)
{
        lt_dlhandle h;
        int (*get_interface) (struct oh_abi_v1 ** pp, const uuid_t uuid);
        struct oh_abi_v1 *abi;
        int err;
        
        h = lt_dlopenext(plugin_name);
        if (h == NULL) {
                dbg("Can not find %s plugin", plugin_name);
                goto err1;
        }

        get_interface = lt_dlsym(h, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }
        
        err = get_interface(&abi, UUID_OH_ABI_V1);
        if (err < 0 || !abi || !abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
        }

#if 0
        /* this should be done elsewhere.  if 0 it for now to make it
           easier to migrate */
        hnd = abi->open(name, addr);
        if (!hnd) {
                dbg("Bootstrap plugin can not work");
                goto err1;
        }
        
        domain_add_zone(d, abi, hnd);
#endif
        
        return 0;
 err1:
        lt_dlclose(h);
        return -1;
}
#endif

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

/*
 * Load plugin by name and make a instance.
 * FIXME: the plugins with multi-instances should reuse 'lt_dlhandler'
 */

struct oh_handler *new_handler(const char *plugin_name, const char *name, const char *addr)
{
        lt_dlhandle h;
        int (*get_interface) (struct oh_abi_v1 ** pp, const uuid_t uuid);
        struct oh_handler *handler;
        int err;
        
        handler = malloc(sizeof(*handler));
	if (!handler) {
		dbg("Out of Memory!");
		goto err;
	}
	
        h = lt_dlopenext(plugin_name);
        if (h == NULL) {
                dbg("Can not find %s plugin", plugin_name);
                goto err1;
        } else {
                dbg("Loaded %s plugin", plugin_name);
        }

        get_interface = lt_dlsym(h, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }
        
        err = get_interface(&handler->abi, UUID_OH_ABI_V1);
        if (err < 0 || !handler->abi || !handler->abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
        }

        /* this should be done elsewhere.  if 0 it for now to make it
           easier to migrate */
        handler->hnd = handler->abi->open(name, addr);
        if (!handler->hnd) {
                dbg("The plugin can not work");
                goto err1;
        }
        
        return handler;
err1:
        lt_dlclose(h);
err:
 	free(handler);
        return NULL;
}

