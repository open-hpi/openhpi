/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ltdl.h>

#include <SaHpi.h>
#include <openhpi.h>

#ifndef BOOTSTRAP_PLUGIN
#define BOOTSTRAP_PLUGIN "libdummy"
#endif

int init_plugin()
{
	lt_dlhandle hnd;
	void *instance;
	int (*get_interface)(struct oh_abi_v1 **pp, const uuid_t uuid);
	struct oh_abi_v1 *abi;
	struct oh_domain *domain;
	int err;
	
	err = lt_dlinit();
	if (err!=0) {
		dbg("Can not init ltdl");
		goto err1;
	}

	hnd = lt_dlopenext(BOOTSTRAP_PLUGIN);
	if (!hnd) {
		dbg("Can not find %s plugin", BOOTSTRAP_PLUGIN);
		goto err2;
	}

	get_interface = lt_dlsym(hnd, "get_interface");
	if (!get_interface) {
		dbg("Can not get 'get_interface' symbol, is it a plugin?!");
		goto err3;
	}

	err = get_interface(&abi, UUID_OH_ABI_V1);
	if (err<0 || !abi || !abi->open) {
		dbg("Can not get ABI V1");
		goto err3;
	}

	/* bootstrap plugin must work without parameter */
	instance = abi->open(NULL, NULL);
	if (!instance) {
		dbg("Bootstrap plugin can not work");
		goto err3;
	}

	domain_add(&domain);
	domain->abi = abi;
	domain->hnd = instance;
	
	return 0;
err3:
	lt_dlclose(hnd);
err2:
	lt_dlexit();
err1:
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
