/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
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
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Sean Dague <http://dague.net/sean>
 *     Rusty Lynch
 *     Renier Morales <renierm@users.sf.net>
 */

#ifndef __OPENHPI_H
#define __OPENHPI_H

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

/* This is a short term hack to get this running properly.  Will 
  be changed to more robust mechanisms later */

#define OPENHPI_RUN_THREADED
#undef OPENHPI_RUN_THREADED

#ifdef OPENHPI_RUN_THREADED
#define oh_run_threaded() 1
#else
#define oh_run_threaded() 0
#endif


/*
 * Representation of additional resource data
 * stored in the data field rpt_utils for each
 * resource
 */

struct oh_resource_data
{
        /*
           The handler of the resource
        */
        struct oh_handler *handler;

        /*
         * The two fields are valid when resource is
         * CAPABILITY_HOTSWAP
         */

        int                controlled;
        SaHpiTimeoutT      auto_extract_timeout;
};

#define oh_session_event oh_hpi_event

/* hotswap - ***This all needs to be cleaned up,
 * all the way down to src/hotswap.c*** -- RM 7/16
 */
void process_hotswap_policy(struct oh_handler *h);
int hotswap_push_event(struct oh_hpi_event *e);
int hotswap_pop_event(struct oh_hpi_event *e);
int hotswap_has_event(void);
SaHpiTimeoutT get_hotswap_auto_insert_timeout(void);
void set_hotswap_auto_insert_timeout(SaHpiTimeoutT);
SaHpiTimeoutT get_default_hotswap_auto_extract_timeout(void);
void set_default_hotswap_auto_extract_timeout(SaHpiTimeoutT to);

#define g_slist_for_each(pos, head) \
        for (pos = head; pos != NULL; pos = g_slist_next(pos))

#define g_slist_for_each_safe(pos, pos1, head) \
        for (pos = head, pos1 = g_slist_next(pos); pos; pos = pos1, pos1 = g_slist_next(pos1))

#ifdef __cplusplus
}
#endif

#endif /* __OPENHPI_H */
