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

#include <glib-1.2/glib.h>
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

struct oh_handler_config {
        char *plugin;
        char *name;
        char *address;
};

struct oh_domain_config {
        char *domain_name;
        GSList *plugins;
};

int load_domain_config(struct oh_domain_config**, SaHpiDomainIdT);

void free_domain_config(struct oh_domain_config*);

#endif/*__OH_CONFIG_H*/
