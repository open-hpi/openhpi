/*      -*- linux-c -*-
 *
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
 *     Sean Dague <http://dague.net/sean>
 */

#ifndef __OH_CONFIG_H
#define __OH_CONFIG_H

#include <oh_plugin.h>

/*
 * Eventually this will contain the definitions for parsing the config file
 *
 * For right now, this will just be static config stanzas
 *
 */

/*
 * search path for plugins
 */
#define OH_PLUGIN_PATH "/usr/lib/openhpi:/usr/local/lib/openhpi:/usr/local/lib"

#ifdef __cplusplus
extern "C" {
#endif 

struct oh_plugin_config {
        char *name;
        /* handle returned by lt_dlopenext or 0 for static plugins */
        void *dl_handle;
        int refcount;
        struct oh_abi_v2 *abi;
};

/* Plugin configuration information prototypes */
int oh_load_config(char *);
void oh_unload_config(void);

int plugin_refcount (char *);

struct oh_plugin_config * plugin_config (char *);

/* Plugin and instances prototypes. Implemented in plugin.c */
int init_plugin(void);
void uninit_plugin(void);
int load_plugin(struct oh_plugin_config *);
void unload_plugin(struct oh_plugin_config *config);

int load_handler(GHashTable *handler_config);
void unload_handler( struct oh_handler *handler );
struct oh_handler *new_handler(GHashTable *handler_config);

#ifdef __cplusplus
}
#endif

#endif/*__OH_CONFIG_H*/
