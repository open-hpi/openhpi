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
 *     David Judkovics <djudkovi@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <epath_utils.h>
#include <uid_utils.h>

static int process_session_event(RPTable *rpt, struct oh_hpi_event *e)
{
        SaHpiRptEntryT *res;
        GSList *i;

        data_access_lock();
        
        res = oh_get_resource_by_id(rpt, e->parent);
        if (res == NULL) {
                dbg("No the resource");
                data_access_unlock();
                return -1;
        }
        
        if (res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP 
            && e->event.EventType == SAHPI_ET_HOTSWAP) {
                hotswap_push_event(e);
        }
        
        /* 
           here is where we need to apply rules about what 
           goes into the domain sel for this event.  Previous implementation
           is commented out, but not yet deleted.
        */
        /*
        g_slist_for_each(i, res->domain_list) {
                SaHpiDomainIdT domain_id;
                struct oh_domain *d;
                GSList *j;

                domain_id = GPOINTER_TO_UINT(i->data);
                d = get_domain_by_id(domain_id);
                if (d) 
                        dsel_add2(d, e);
                else 
                        dbg("Invalid domain");
        */
        
        
        g_slist_for_each(i, global_session_list) {
                struct oh_session *s = i->data;
                /* yes, we need to add real domain support later here */
                if (s->domain_id == SAHPI_DEFAULT_DOMAIN_ID
                    && s->event_state == OH_EVENT_SUBSCRIBE) {
                        session_push_event(s, e);
                }
        }

        data_access_unlock();
        return 0;
}

static int process_resource_event(struct oh_handler *h, RPTable *rpt, struct oh_resource_event *e) 
{
        int rv;
        data_access_lock();

        rv = oh_add_resource(rpt,e->entry,h);
        
        data_access_unlock();
        
        return rv;
}
/*
static void process_domain_event(struct oh_handler *h, struct oh_domain_event *e)
{
        struct oh_resource *res;
        struct oh_domain *domain;

        data_access_lock();

        res = get_res_by_oid(e->res_id);
        domain = get_domain_by_oid(e->domain_id);
        
        if (!res) {
                dbg("Cannot find corresponding resource");
                data_access_unlock();
                return;
        }               
        if (!domain) {
                dbg("Cannot find corresponding domain");
                data_access_unlock();
                return;
        }               
        res->domain_list = g_slist_append(res->domain_list, GUINT_TO_POINTER(domain->domain_id));
        
        data_access_unlock();

}
*/

static int process_rdr_event(struct oh_handler *h, RPTable *rpt, struct oh_rdr_event *e)
{
        int rv;
        SaHpiResourceIdT rid;

        rid = oh_uid_lookup(&(e->rdr.Entity));
        if(rid == 0) {
                dbg("Trying to use an unknown resource id for rdr");
                return -1;
        }
        
        rv = oh_add_rdr(rpt,rid,e->rdr,NULL);
        
        return rv;
}

static void process_rsel_event(struct oh_handler *h, struct oh_rsel_event *e)
{
        struct oh_resource *res;
        
        dbg("RSEL event! rsel->oid=%p", e->rsel.oid.ptr);
        res = get_res_by_oid(e->rsel.parent);
        if (!res) {
                dbg("Cannot find corresponding resource");
                return;
        }
        
        rsel_add2(res, e);
}

/**
 * get_handler_event:
 * @h: handler structure
 * 
 * get_handler_event pulls an event from a given handler.  It will normally
 * be called from get_events, which iterates across all handlers.
 * 
 * Return value: 0 on sucess, -1 on failure
 **/
static int get_handler_event(struct oh_handler *h, RPTable *rpt)
{
        struct oh_event event;
        struct timeval to = {0, 0};
        int rv;                
        rv = h->abi->get_event(h->hnd, &event, &to);
        if(rv < 0) {
                dbg("No event found");
                return -1;
        }
        switch (event.type) {
        case OH_ET_HPI:
                /* add the event to session event list */
                process_session_event(rpt, &event.u.hpi_event);
                break;
        case OH_ET_RESOURCE:
                process_resource_event(h, rpt, &event.u.res_event);
                break;
/* Domain events are not supported, pull them out for now */
/*                        case OH_ET_DOMAIN:
                          process_domain_event(h, &event.u.domain_event);
                          break;
*/
        case OH_ET_RDR:
                process_rdr_event(h, rpt, &event.u.rdr_event);
                break;
        case OH_ET_RSEL:
                process_rsel_event(h, &event.u.rsel_event);
                break;
        default:
                dbg("Error! Should not reach here!");
                return -1;
        }
        
        data_access_unlock();

        return 0;
}

/**
 * get_events:
 * @void: 
 * 
 * loops through all handlers, processes all outstanding events
 * 
 * Note: at some point we will probably have to pass it session info
 * to know which rpt to use
 *
 * Return value: 0 on success, < 0 on failure
 **/
int get_events(void)
{
        RPTable *rpt = &default_rpt;
        GSList *i;
        int has_event;
        
        data_access_lock();

        g_slist_for_each(i, global_handler_list) {
                do {
                        has_event = get_handler_event(i->data, rpt);
                } while (has_event > 0);
        }
        
        process_hotswap_policy();

        data_access_unlock();

        return 0;
}
