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
#include <SaHpi.h>

#define OH_GLOBAL_STR_MAX_LENGTH SAHPI_MAX_TEXT_BUFFER_LENGTH*2

struct oh_parsed_config {
        GSList *plugin_names;
        GSList *handler_configs;
};

typedef enum {
        OPENHPI_ON_EP = 1,
        OPENHPI_LOG_ON_SEV,
        //OPENHPI_DEBUG,
        //OPENHPI_DEBUG_TRACE,
        //OPENHPI_DEBUG_LOCK,
        OPENHPI_THREADED,
        OPENHPI_PATH,
        OPENHPI_CONF
} oh_global_param_type;

typedef union {
        SaHpiEntityPathT on_ep;
        SaHpiSeverityT log_on_sev;
        //unsigned char dbg; /* 1 = YES, 0 = NO */
        //unsigned char dbg_trace; /* !0 = YES, 0 = NO */
        //unsigned char dbg_lock; /* !0 = YES, 0 = NO */
        unsigned char threaded; /* !0 = YES, 0 = NO */
        char path[OH_GLOBAL_STR_MAX_LENGTH];
        char conf[OH_GLOBAL_STR_MAX_LENGTH];
} oh_global_param_union;

struct oh_global_param {
        oh_global_param_type type;
        oh_global_param_union u;
};

/* Plugin configuration information prototypes */
int oh_load_config(char *filename, struct oh_parsed_config *config);
void oh_clean_config(void);

/* For handling global parameters */
int oh_get_global_param(struct oh_global_param *param);
int oh_set_global_param(struct oh_global_param *param);
//unsigned char oh_get_global_bool(oh_global_param_type type);

#ifdef __cplusplus
}
#endif

#endif/*__OH_CONFIG_H*/
