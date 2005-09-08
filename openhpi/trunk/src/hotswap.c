/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003, 2004, 2005
 * Copyright (c) 2004 by FORCE Computers.
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
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Racing Guo <racing.guo@intel.com>
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Renier Morales <renierm@users.sf.net>
 */

#include <string.h>

#include <oh_hotswap.h>
#include <oh_handler.h>
#include <oh_plugin.h>
#include <oh_lock.h>
#include <oh_domain.h>
#include <oh_utils.h>
#include <oh_error.h>

GSList *hs_eq = NULL;

void process_hotswap_policy()
{
        SaHpiTimeT cur, est;
        struct oh_event e;
        struct oh_handler *handler = NULL;
        struct oh_domain *domain = NULL;
        RPTable *rpt;
        GSList *tmp_hs_eq = NULL;
        SaHpiEventT *event = NULL;
        SaHpiRptEntryT *rptentry = NULL;

        int (*get_hotswap_state)(void *hnd, SaHpiResourceIdT rid,
                                 SaHpiHsStateT *state);

        while (hotswap_pop_event(&hs_eq, &e) > 0) {
                struct oh_resource_data *rd;

                event = &e.u.hpi_event.event;
                rptentry = &e.u.hpi_event.res;

                if (e.type != OH_ET_HPI) {
                        dbg("Non-hpi event!");
                        continue;
                }

                if (event->EventType != SAHPI_ET_HOTSWAP) {
                        dbg("Non-hotswap event!");
                        continue;
                }

                if (!(rptentry->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                        dbg("Non-hotswapable resource?!");
                        continue;
                }

                domain = oh_get_domain(e.did);
                if (!domain) {
                        dbg("No domain\n");
                        continue;
                }
                /*rpt is impossible NULL */
                rpt = &domain->rpt;

                rd = oh_get_resource_data(rpt, rptentry->ResourceId);
                if (!rd) {
                        dbg( "Can't find resource data for Resource %d", rptentry->ResourceId);
                        oh_release_domain(domain);
                        continue;
                }

                if (rd->controlled) {
                        trace("Hotswap policy was cancelled by user.");
                        oh_release_domain(domain);
                        continue;
                }

                handler = oh_get_handler(e.hid);
                if (!handler) {
                        dbg("handler is NULL\n");
                        oh_release_domain(domain);
                        continue;
                }

                get_hotswap_state = handler->abi->get_hotswap_state;
                if (!get_hotswap_state) {
                        oh_release_handler(handler);
                        oh_release_domain(domain);
                        dbg(" Very bad thing here or hotswap not yet supported");
                        continue;
                }

                oh_gettimeofday(&cur);
                if (event->EventDataUnion.HotSwapEvent.HotSwapState ==
                    SAHPI_HS_STATE_INSERTION_PENDING) {
                        if (get_hotswap_auto_insert_timeout() != SAHPI_TIMEOUT_BLOCK) {
                                est = event->Timestamp + get_hotswap_auto_insert_timeout();
                                if (cur>=est) {
                                        handler->abi->set_hotswap_state(handler->hnd,
                                                                        rptentry->ResourceId,
                                                                        SAHPI_HS_STATE_ACTIVE);
                                } else {
                                        /*push again in order to process in the feature*/
                                        hotswap_push_event(&tmp_hs_eq, &e);
                                }
                        }
                } else if (event->EventDataUnion.HotSwapEvent.HotSwapState ==
                           SAHPI_HS_STATE_EXTRACTION_PENDING) {
                        if (rd->auto_extract_timeout != SAHPI_TIMEOUT_BLOCK) {
                                est = e.u.hpi_event.event.Timestamp + rd->auto_extract_timeout;
                                if (cur>=est) {
                                        handler->abi->set_hotswap_state(handler->hnd,
                                                                        rptentry->ResourceId,
                                                                        SAHPI_HS_STATE_INACTIVE);
                                } else {
                                        /*push again in order to process in the feature*/
                                        hotswap_push_event(&tmp_hs_eq, &e);
                                }
                        }
                }
                oh_release_handler(handler);
                oh_release_domain(domain);
        }

        /* TODO:
           1. Make sure hs_eq has no events (hotswap_pop_event(&e) > 0)
           2. Make process_hotswap_policy reentrant (called by one thread for now).
        */

        hs_eq = tmp_hs_eq;
}

/*
 * session_push_event pushs and event into a session.
 * We store a copy of event so that caller of the function
 * needn't care about ref counter of the event.
*/

int hotswap_push_event(GSList **hs_eq, struct oh_event *e)
{
        struct oh_event *e1;

        data_access_lock();

        e1 = malloc(sizeof(*e1));
        if (!e1) {
                dbg("Out of memory!");
                data_access_unlock();
                return -1;
        }
        memcpy(e1, e, sizeof(*e));

        *hs_eq = g_slist_append(*hs_eq, (gpointer *) e1);

        data_access_unlock();

        return 0;
}

/*
 * session_pop_event - pops events off the session.
 *
 * return codes are left as was, but it seems that return 1 for success
 * here doesn't jive with the rest of the exit codes
 */

int hotswap_pop_event(GSList **hs_eq, struct oh_event *e)
{
        GSList *head;

        data_access_lock();

        if (g_slist_length(*hs_eq) == 0) {
                data_access_unlock();
                return 0;
        }

        head = *hs_eq;
        *hs_eq = g_slist_remove_link(*hs_eq, head);

        memcpy(e, head->data, sizeof(*e));

        free(head->data);
        g_slist_free_1(head);

        data_access_unlock();

        return 1;
}

/*
 * session_has_event - query if the session has events
 */
int hotswap_has_event(GSList *hs_eq)
{
        return (hs_eq == NULL) ? 0 : 1;
}

static SaHpiTimeoutT hotswap_auto_insert_timeout = 0;

SaHpiTimeoutT get_hotswap_auto_insert_timeout(void)
{
        return hotswap_auto_insert_timeout;
}

void set_hotswap_auto_insert_timeout(SaHpiTimeoutT to)
{
        hotswap_auto_insert_timeout = to;
}


/* default auto extract timeout */
static SaHpiTimeoutT hotswap_auto_extract_timeout = 0;

SaHpiTimeoutT get_default_hotswap_auto_extract_timeout(void)
{
        return hotswap_auto_extract_timeout;
}

void set_default_hotswap_auto_extract_timeout(SaHpiTimeoutT to)
{
        hotswap_auto_extract_timeout = to;
}

/*
 * this function determines whether a hotswap transition is allowed
 */

SaHpiBoolT oh_allowed_hotswap_transition(SaHpiHsStateT from, SaHpiHsStateT to)
{
        switch(from) {
        case SAHPI_HS_STATE_INACTIVE:
                if((to == SAHPI_HS_STATE_INSERTION_PENDING) ||
                   (to == SAHPI_HS_STATE_NOT_PRESENT)) {
                        return SAHPI_TRUE;
                } else {
                        return SAHPI_FALSE;
                }
                break;
        case SAHPI_HS_STATE_INSERTION_PENDING:
                if((to == SAHPI_HS_STATE_INACTIVE) ||
                   (to == SAHPI_HS_STATE_NOT_PRESENT) ||
                   (to == SAHPI_HS_STATE_ACTIVE)) {
                        return SAHPI_TRUE;
                } else {
                        return SAHPI_FALSE;
                }
                break;
        case SAHPI_HS_STATE_ACTIVE:
                if((to == SAHPI_HS_STATE_EXTRACTION_PENDING) ||
                   to == SAHPI_HS_STATE_NOT_PRESENT) {
                        return SAHPI_TRUE;
                } else {
                        return SAHPI_FALSE;
                }
                break;
        case SAHPI_HS_STATE_EXTRACTION_PENDING:
                if((to == SAHPI_HS_STATE_ACTIVE) ||
                   (to == SAHPI_HS_STATE_NOT_PRESENT) ||
                   (to == SAHPI_HS_STATE_INACTIVE)) {
                        return SAHPI_TRUE;
                } else {
                        return SAHPI_FALSE;
                }
                break;
        case SAHPI_HS_STATE_NOT_PRESENT:
                if(to == SAHPI_HS_STATE_INSERTION_PENDING) {
                        return SAHPI_TRUE;
                } else {
                        return SAHPI_FALSE;
                }
                break;
        default:
                return SAHPI_FALSE;
        }
}
