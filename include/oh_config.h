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

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Global listing of plugins (oh_plugin_config).  This list is populated
 *  by the configuration subsystem, and used by the plugin loader.
 */
extern GSList *global_plugin_list;

/*
 *  Global listing of handler configs (GHashTable).  This list is
 *  populated during config file parse, and used to build
 *  global_handler_table
 */
extern GSList *global_handler_configs;

/*
 *  Global table of handlers (oh_handler).  This list is populated
 *  by the first call the saHpiSessionOpen().
 */
extern GHashTable *global_handler_table;

/*
 *  Representation of a plugin instance
 */
struct oh_handler {
        unsigned int id; /* handler id */
        /*
         * pointer to configuration
         */
        GHashTable *config;

        /*
           pointer to associated plugin interface
        */
        struct oh_abi_v2 *abi;

        /*
          private pointer used by plugin implementations to distinguish
          between different instances
        */
        void *hnd;

        /*
          This is the list of resources which the handler reports
         */
        GSList *resource_list;

};

struct oh_plugin_config {
        char *name;
        /* handle returned by lt_dlopenext or 0 for static plugins */
        void *dl_handle;
        int refcount;
        struct oh_abi_v2 *abi;
};

/* Initialization and closing of ltdl structures */
int oh_init_ltdl(void);
void oh_exit_ltdl(void);

/* Plugin configuration information prototypes */
int oh_load_config(char *filename);
void oh_unload_config(void);

int plugin_refcount(char *name);

struct oh_plugin_config *plugin_config(char *name);

/* Plugin and instances prototypes. Implemented in plugin.c */
int load_plugin(struct oh_plugin_config *);
void unload_plugin(struct oh_plugin_config *config);

int load_handler(GHashTable *handler_config);
void unload_handler(struct oh_handler *handler);

#ifdef __cplusplus
}
#endif

#endif/*__OH_CONFIG_H*/
