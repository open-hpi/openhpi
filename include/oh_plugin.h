/*      -*- linux-c -*-
 *
 * (C) Copright IBM Corp 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renierm@users.sourceforge.net>
 */

#ifndef __OH_PLUGIN_H
#define __OH_PLUGIN_H

#include <glib.h>
#include <oh_handler.h>

struct oh_plugin {
        char *name;
        /* handle returned by lt_dlopenext or 0 for static plugins */
        void *dl_handle;
        int refcount;
        struct oh_abi_v2 *abi;
};

/*
 *  Representation of a plugin instance
 */
struct oh_handler {
        unsigned int id; /* handler id */
        char *plugin_name;

        GHashTable *config; /* pointer to configuration */
                
        struct oh_abi_v2 *abi; /* pointer to associated plugin interface */

        /*
          private pointer used by plugin implementations to distinguish
          between different instances
        */
        void *hnd;
};

/* Initialization and closing of ltdl structures. */
int oh_init_ltdl(void);
int oh_exit_ltdl(void);

/* Plugin and handler prototypes. */
struct oh_plugin *oh_lookup_plugin(char *plugin_name);
int oh_lookup_next_plugin(char *plugin_name,
                          char *next_plugin_name,
                          unsigned int size);
			  
int oh_load_plugin(char *plugin_name);
int oh_unload_plugin(char *plugin_name);

void oh_init_handler_table(void);
struct oh_handler *oh_lookup_handler(unsigned int hid);
int oh_lookup_next_handler(unsigned int hid, unsigned int *next_hid);
unsigned int oh_load_handler(GHashTable *handler_config);
int oh_unload_handler(unsigned int hid);

#endif /*__OH_PLUGIN_H*/
