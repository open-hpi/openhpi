/* -*- linux-c -*-
 * BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ltdl.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_plugin.h>

/*
 * This initializes the plugin loader.  Don't load anything
 * right now though.
 */

int init_plugin() 
{
        int err;
        
        err = lt_dlinit();
        if (err!=0) {
                dbg("Can not init ltdl");
                goto err1;
        }    
        
        err = lt_dlsetsearchpath(OH_PLUGIN_PATH);
        if(err != 0) {
                dbg("Can not set lt_dl search path");
                goto err2;
        }
        
        return 0;

err2:
        lt_dlexit();
err1:
        return -1;
}

/*
 * Load plugin by name.  This needs to be done before the plugin is used
 */

int load_plugin(const char *plugin_name, const char *name, const char *addr) 
{
        lt_dlhandle hnd;
        void *instance;
        int (*get_interface)(struct oh_abi_v1 **pp, const uuid_t uuid);
        struct oh_abi_v1 *abi;
        struct oh_domain *domain;
        int err;
        
        hnd = lt_dlopenext(plugin_name);
        if (!hnd) {
                dbg("Can not find %s plugin", plugin_name);
                goto err1;
        }
        
        get_interface = lt_dlsym(hnd, "get_interface");
        if (!get_interface) {
                dbg("Can not get 'get_interface' symbol, is it a plugin?!");
                goto err1;
        }
        
        err = get_interface(&abi, UUID_OH_ABI_V1);
        if (err<0 || !abi || !abi->open) {
                dbg("Can not get ABI V1");
                goto err1;
	}

	/* bootstrap plugin must work without parameter */
	instance = abi->open(name, addr);
	if (!instance) {
		dbg("Bootstrap plugin can not work");
		goto err1;
	}

	domain_add(&domain);
	domain->abi = abi;
	domain->hnd = instance;
	
	return 0;
err1:
	lt_dlclose(hnd);
	return -1;
}

int uninit_plugin(void)
{
	int rv;
	
	rv = lt_dlexit();
	if (rv<0) {
		dbg("Can not exit ltdl right");
		return -1;
	}
	return 0;
}
