/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Racing Guo <racing.guo@intel.com>
 */

#ifndef __OH_HOTSWAP_H
#define __OH_HOTSWAP_H

#include <SaHpi.h>
#include <glib.h>
#include <oh_plugin.h>
#include <oh_config.h>
#include <oh_init.h>
#include <oh_lock.h>
#include <oh_error.h>
#include <oh_domain.h>
#include <oh_session.h>
#include <oh_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GSList *hs_eq;

void process_hotswap_policy(void);
int hotswap_push_event(GSList **hs_eq, struct oh_event *e);
int hotswap_pop_event(GSList **hs_eq, struct oh_event *e);
int hotswap_has_event(GSList *hs_eq);

SaHpiTimeoutT get_hotswap_auto_insert_timeout(void);
void set_hotswap_auto_insert_timeout(SaHpiTimeoutT);
SaHpiTimeoutT get_default_hotswap_auto_extract_timeout(void);
void set_default_hotswap_auto_extract_timeout(SaHpiTimeoutT to);

#ifdef __cplusplus
}
#endif

#endif /* __OH_HOTSWAP_H */
