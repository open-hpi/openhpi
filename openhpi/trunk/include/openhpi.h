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

/*
 *  Global RPT Table (implemented as a linked list).
 *
 *  This list contains all resources (wrapped as oh_resource structures),
 *  regardless of whether an HPI caller can see the resources for the given
 *  permission level or domain view used.
 *
 *  This list is populated by calls to saHpiDiscoverResources()
 */
extern RPTable *default_rpt;

/*
 *  Global listing of all active sessions (oh_session).  This list is
 *  populated and depopulated by calls to saHpiSessionOpen() and
 *  saHpiSessionClose()
 */
extern GSList *global_session_list;

struct oh_session *session_get(SaHpiSessionIdT);
int session_add(SaHpiDomainIdT, struct oh_session**);
int session_del(struct oh_session*);
int session_count(void);
/* malloc/copy/add event into the tail of event_list */
int session_push_event(struct oh_session*, struct oh_session_event*);
/* del/copy/free event from the head of event_list */
int session_pop_event(struct oh_session*, struct oh_session_event*);
/*query if the session has events*/
int session_has_event(struct oh_session *s);

struct oh_domain *get_domain_by_id(SaHpiDomainIdT did);
int is_in_domain_list(SaHpiDomainIdT domain_id);
int add_domain(SaHpiDomainIdT domain_id);
void oh_cleanup_domain(void);

/* howswap - ***This all needs to be cleaned up,
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

static __inline__
 void gettimeofday1(SaHpiTimeT *t)
{
        struct timeval now;
        gettimeofday(&now, NULL);
        *t = (SaHpiTimeT) now.tv_sec * 1000000000 + now.tv_usec*1000;
}

#define g_slist_for_each(pos, head) \
        for (pos = head; pos != NULL; pos = g_slist_next(pos))

#define g_slist_for_each_safe(pos, pos1, head) \
        for (pos = head, pos1 = g_slist_next(pos); pos; pos = pos1, pos1 = g_slist_next(pos1))

#ifdef __cplusplus
}
#endif

#endif/*__OPENHPI_H*/
