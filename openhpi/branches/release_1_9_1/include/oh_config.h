/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003-2004
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
 *     Renier Morales <renierm@users.sf.net>
 */

#ifndef __OH_CONFIG_H
#define __OH_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>

struct oh_parsed_config {
        GSList *plugin_names;
        GSList *handler_configs;
};

/* Plugin configuration information prototypes */
int oh_load_config(char *filename, struct oh_parsed_config *config);
void oh_clean_config(void);
void oh_unload_config(void);

/* For handling global parameters */
int oh_lookup_global_param(char *param, char *value, int size);
int oh_set_global_param(const char *param, char *value);

#ifdef __cplusplus
}
#endif

#endif/*__OH_CONFIG_H*/
