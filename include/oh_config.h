/*      -*- linux-c -*-
 *
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
 *     Sean Dague <sean@dague.net>
 */

#ifndef __OH_CONFIG_H
#define __OH_CONFIG_H

#include <glib.h>
#include <SaHpi.h>

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

/* oh_plugin_config, currently with only one item.  There are thoughts of
   having explicit path to the plugin, but I'm not sure this is a good plan */
struct oh_plugin_config {
        char *name;
};

struct oh_handler_config {
        char *plugin;
        char *name;
        char *address;
};

struct oh_config {
        /* 
         *  list of plugin config structures
         */
        GSList *plugins;
        /*
         * list of handler config structures
         */
        GSList *handlers;
};

int oh_load_config(struct oh_config*);

void oh_free_config(struct oh_config*);

#endif/*__OH_CONFIG_H*/
