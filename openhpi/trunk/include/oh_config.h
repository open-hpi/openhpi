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

#include <glib.h>
#include <SaHpi.h>
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

#define OH_DEFAULT_CONF "/etc/openhpi/openhpi.conf"

/* oh_plugin_config, currently with only one item.  There are thoughts of
   having explicit path to the plugin, but I'm not sure this is a good plan */
struct oh_plugin_config {
        char *name;
        int refcount;
        struct oh_abi_v2 *abi;
};

/*struct oh_handler_config {
        char *plugin;
        char *name;
        char *addr;
};*/

int oh_load_config(char *);

int plugin_refcount (char *);

struct oh_plugin_config * plugin_config (char *);

#endif/*__OH_CONFIG_H*/
