/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 * (C) Copyright IBM Corp. 2005
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
 *     Renier Morales <renierm@users.sf.net>
 */

#ifndef __OH_HOTSWAP_H
#define __OH_HOTSWAP_H

#include <SaHpi.h>
#include <glib.h>
#include <oh_event.h>
#include <oh_domain.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Representation of additional resource data
 * stored in the data field rpt_utils for each
 * resource
 */

struct oh_resource_data
{
        /*
           The handler id of the resource
        */
        unsigned int hid;

        /*
         * The two fields are valid when resource is
         * CAPABILITY_HOTSWAP
         */

        int                controlled;
        SaHpiTimeoutT      auto_extract_timeout;
};

extern GSList *hs_eq;

void process_hotswap_policy(void);
int hotswap_push_event(GSList **hs_eq, struct oh_event *e);
int hotswap_pop_event(GSList **hs_eq, struct oh_event *e);
int hotswap_has_event(GSList *hs_eq);

SaHpiTimeoutT get_hotswap_auto_insert_timeout(struct oh_domain *domain);
void set_hotswap_auto_insert_timeout(struct oh_domain *domain, SaHpiTimeoutT t);
SaHpiTimeoutT get_default_hotswap_auto_extract_timeout(void);
void set_default_hotswap_auto_extract_timeout(SaHpiTimeoutT to);
SaHpiBoolT oh_allowed_hotswap_transition(SaHpiHsStateT from, SaHpiHsStateT to);


#ifdef __cplusplus
}
#endif

#endif /* __OH_HOTSWAP_H */
