/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2004
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
 *     Sean Dague <sdague@users.sf.net>
 *     Rusty Lynch
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Renier Morales <renierm@users.sf.net>
 */

#include <stdlib.h>
#include <string.h>
#include <oh_config.h>
#include <openhpi.h>
#include <oh_plugin.h>
#include <SaHpi.h>
#include <sahpimacros.h>
#include <uid_utils.h>
#include <epath_utils.h>

#include <pthread.h>

/***********************************************************************
 * Begin SAHPI functions.  For full documentation please see
 * the specification
 **********************************************************************/

/**********************************************************************
 *
 *  Finalize and Initialize were dropped in HPI B.  We keep
 *  them as a hold over for now, however once new plugin
 *  and handler utilities are provided, we'll rewrite them to 
 *  be cleaner.
 *
 *********************************************************************/

static SaErrorT saHpiFinalize(void)
{
        OH_STATE_READY_CHECK;

        /* free mutex */
        /* TODO: this wasn't here in branch, need to resolve history */
        data_access_lock();

        /* TODO: realy should have a oh_uid_finalize() that */
        /* frees memory,                                    */
        if(oh_uid_map_to_file())
                dbg("error writing uid entity path mapping to file");

        /* check for open sessions */
        if ( global_session_list ) {
                dbg("cannot saHpiFinalize because of open sessions" );
                data_access_unlock();
                return SA_ERR_HPI_BUSY;
        }

        /* close all plugins */
        while(global_handler_list) {
                struct oh_handler *handler = (struct oh_handler *)global_handler_list->data;
                /* unload_handler will remove handler from global_handler_list */
                unload_handler(handler);
        }

        /* unload plugins */
        while(global_plugin_list) {
                struct oh_plugin_config *plugin = (struct oh_plugin_config *)global_plugin_list->data;
                unload_plugin(plugin);
        }

        /* free global rpt */
        if (default_rpt) {
                oh_flush_rpt(default_rpt);
                g_free(default_rpt);
                default_rpt = 0;
        }

        /* free global_handler_configs and uninit_plugin */
        oh_unload_config();

        /* free domain list */
        oh_cleanup_domain();

        oh_hpi_state = OH_STAT_UNINIT;

        /* free mutex */
        data_access_unlock();

        return SA_OK;
}

static SaErrorT saHpiInitialize(void)
{
        struct oh_plugin_config *tmpp;
        GHashTable *tmph;

        unsigned int i;
        char *openhpi_conf;

        int rval;

	oh_will_block = 0;
        data_access_lock();

        if (OH_STAT_UNINIT != oh_hpi_state) {
                dbg("Cannot initialize twice");
                data_access_unlock();
                return SA_ERR_HPI_DUPLICATE;
        }

        /* initialize mutex used for data locking */
        /* in the future may want to add seperate */
        /* mutexes, one for each hash list        */

        /* set up our global domain */
        if (add_domain(OH_DEFAULT_DOMAIN_ID)) {
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }

        /* setup our global rpt_table */
        default_rpt = g_malloc0(sizeof(RPTable));
        if(!default_rpt) {
                dbg("Couldn't allocate RPT for Default Domain");
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }
        default_rpt->rpt_info.UpdateTimestamp = SAHPI_TIME_UNSPECIFIED;

        /* initialize uid_utils, and load uid map file if present */
        rval = oh_uid_initialize();
        if( (rval != SA_OK) && (rval != SA_ERR_HPI_ERROR) ) {
                dbg("uid_intialization failed");
                data_access_unlock();
                return(rval);
        }

        openhpi_conf = getenv("OPENHPI_CONF");

        if (openhpi_conf == NULL) {
                openhpi_conf = OH_DEFAULT_CONF;
        }

        if (oh_load_config(openhpi_conf) < 0) {
                dbg("Can not load config");
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        for(i = 0; i < g_slist_length(global_plugin_list); i++) {
                tmpp = (struct oh_plugin_config *) g_slist_nth_data(
                        global_plugin_list, i);
                if(load_plugin(tmpp) == 0) {
                        dbg("Loaded plugin %s", tmpp->name);
                        tmpp->refcount++;
                } else {
                        dbg("Couldn't load plugin %s", tmpp->name);
                }
        }

        for(i = 0; i < g_slist_length(global_handler_configs); i++) {
                tmph = (GHashTable *) g_slist_nth_data(
                        global_handler_configs, i);
                if(plugin_refcount((char *)g_hash_table_lookup(tmph, "plugin")) > 0) {
                        if(load_handler(tmph) == 0) {
                                dbg("Loaded handler for plugin %s",
                                        (char *)g_hash_table_lookup(tmph, "plugin"));
                        } else {
                                dbg("Couldn't load handler for plugin %s",
                                        (char *)g_hash_table_lookup(tmph, "plugin"));
                        }
                } else {
                        dbg("load handler for unknown plugin %s",
                                (char *)g_hash_table_lookup(tmph, "plugin"));
                }
        }

        oh_hpi_state = OH_STAT_READY;

        /* check if we have at least one handler */
        if ( global_handler_list == 0 ) {
                /* there is no handler => this can not work */
                dbg("no handler found. please check %s!", openhpi_conf);

                data_access_unlock();
                saHpiFinalize();

                return SA_ERR_HPI_NOT_PRESENT;
        }

        data_access_unlock();

        return SA_OK;
}

/*********************************************************************
 * 
 *  Begin SAHPI B.1.1 Functions
 *
 ********************************************************************/

SaHpiVersionT SAHPI_API saHpiVersionGet () 
{
        return SAHPI_INTERFACE_VERSION;
}


SaErrorT SAHPI_API saHpiSessionOpen(
                SAHPI_IN SaHpiDomainIdT DomainId,
                SAHPI_OUT SaHpiSessionIdT *SessionId,
                SAHPI_IN void *SecurityParams)
{
        struct oh_session *s;
        int rv;

        /* SLD 6/8/2004
          This is the worst of all possible solutions, however
          it should make us work for now.  We need a much better
          mechanism for making this actually init on first session open.
        */
        if(OH_STAT_READY != oh_hpi_state) {
                saHpiInitialize();
        }

        if (SecurityParams != NULL) {
                dbg("SecurityParams must be NULL");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_STATE_READY_CHECK;
        
        oh_event_init();
        data_access_lock();
        
        if(!is_in_domain_list(DomainId)) {
                dbg("domain does not exist!");
                data_access_unlock();

                return SA_ERR_HPI_INVALID_DOMAIN;
        }

        rv = session_add(DomainId, &s);
        if(rv < 0) {
                dbg("Out of space");
                data_access_unlock();
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        *SessionId = s->session_id;
        data_access_unlock();

        return SA_OK;
}


SaErrorT SAHPI_API saHpiSessionClose(SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;

        OH_STATE_READY_CHECK;
        data_access_lock();

        s = session_get(SessionId);
        if (!s) {
                dbg("Invalid session");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_SESSION;
        }

        session_del(s);
        data_access_unlock();

        return SA_OK;
}


SaErrorT SAHPI_API saHpiDiscover(SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
        GSList *i;
        RPTable *rpt;
        int rv = -1;

        OH_STATE_READY_CHECK;

        data_access_lock();

        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);

        g_slist_for_each(i, global_handler_list) {
                struct oh_handler *h = i->data;
                if (!(h->abi->discover_resources(h->hnd)))
                        rv = 0;
        }

        if (rv) {
                dbg("Error attempting to discover resource");
                data_access_unlock();
                return SA_ERR_HPI_UNKNOWN;
        }

        data_access_unlock();

        rv = get_events(rpt);
        if (rv < 0) {
                dbg("Error attempting to process resources");
                return SA_ERR_HPI_UNKNOWN;
        }

        return SA_OK;
}

#if 0

/*
  this needs to be domain info get, requires domain structure that
  Renier is working on
*/

SaErrorT SAHPI_API saHpiRptInfoGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiRptInfoT *RptInfo)
{
        struct oh_session *s;
        /* determine the right pointer later when we do multi domains */
        RPTable *rpt;
        int rv = 0;

        /* Test pointer parameters for invalid pointers */
        if (RptInfo == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = get_events(rpt);

        if (rv<0) {
                dbg("Error attempting to process events");
                return SA_ERR_HPI_UNKNOWN;
        }

        data_access_lock();

        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId, s);

        OH_RPT_GET(SessionId, rpt);

        RptInfo->UpdateCount = rpt->rpt_info.UpdateCount;
        RptInfo->UpdateTimestamp= rpt->rpt_info.UpdateTimestamp;

        data_access_unlock();

        return SA_OK;
}

#endif

/*********************************************************************
 *
 *  Domain Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiDomainInfoGet (
        SAHPI_IN  SaHpiSessionIdT      SessionId,
        SAHPI_OUT SaHpiDomainInfoT     *DomainInfo)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiDrtEntryGet (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiEntryIdT       EntryId,
        SAHPI_OUT SaHpiEntryIdT       *NextEntryId,
        SAHPI_OUT SaHpiDrtEntryT      *DrtEntry) 
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiDomainTagSet (
        SAHPI_IN  SaHpiSessionIdT      SessionId,
        SAHPI_IN  SaHpiTextBufferT     *DomainTag)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  Resource Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRptEntryGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiEntryIdT EntryId,
                SAHPI_OUT SaHpiEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
        struct oh_session *s;

        /* determine the right pointer later when we do multi domains */
        RPTable *rpt;
        SaHpiRptEntryT *req_entry;
        SaHpiRptEntryT *next_entry;

        OH_STATE_READY_CHECK;

        /* Test pointer parameters for invalid pointers */
        if ((NextEntryId == NULL) || (RptEntry == NULL))
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        data_access_lock();

        OH_SESSION_SETUP(SessionId, s);

        OH_RPT_GET(SessionId, rpt);

        if (EntryId == SAHPI_FIRST_ENTRY) {
                req_entry = oh_get_resource_next(rpt, SAHPI_FIRST_ENTRY);
        } else {
                req_entry = oh_get_resource_by_id(rpt, EntryId);
        }

        /* if the entry was NULL, clearly have an issue */
        if(req_entry == NULL) {
                dbg("Invalid EntryId");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));

        next_entry = oh_get_resource_next(rpt, req_entry->EntryId);

        if(next_entry != NULL) {
                *NextEntryId = next_entry->EntryId;
        } else {
                *NextEntryId = SAHPI_LAST_ENTRY;
        }

        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiRptEntryT *RptEntry)
{

        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *req_entry;

        /* Test pointer parameters for invalid pointers */
        if (RptEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        data_access_lock();

        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId, s);

        OH_RPT_GET(SessionId, rpt);

        req_entry = oh_get_resource_by_id(rpt, ResourceId);

        /*
         * is this case really supposed to be an error?  I thought
         * there was a valid return for "not found in domain"
         */

        if(req_entry == NULL) {
                dbg("No such resource id");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));

        data_access_unlock();
        return SA_OK;
}

/*
 * Question: do we need a lock here?
 */

SaErrorT SAHPI_API saHpiResourceSeveritySet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSeverityT Severity)
{
        /* this requires a new abi call to push down to the plugin */
        int (*set_res_sev)(void *hnd, SaHpiResourceIdT id,
                             SaHpiSeverityT sev);

        struct oh_session *s;
        RPTable *rpt;
        struct oh_handler *h;

        if (Severity < 0 || (Severity > 4 && Severity != 0xF0)) {
                dbg("Invalid severity %d passed.", Severity);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        data_access_lock();

        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId,s);

        OH_RPT_GET(SessionId, rpt);

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_res_sev = h->abi->set_resource_severity;

        if (!set_res_sev) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        if (set_res_sev(h->hnd, ResourceId, Severity) < 0) {
                dbg("SEL add entry failed");
                data_access_unlock();
                return SA_ERR_HPI_UNKNOWN;
        }

         data_access_unlock();

        /* to get rpt entry into infrastructure */
        get_events(rpt);
        return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceTagSet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
        SaErrorT rv;
        SaErrorT (*set_res_tag)(void *hnd, SaHpiResourceIdT id,
                                SaHpiTextBufferT *ResourceTag);

        struct oh_session *s;
        RPTable *rpt;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId,s);

        OH_RPT_GET(SessionId, rpt);

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_res_tag = h->abi->set_resource_tag;

        if (!set_res_tag) {
                 data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_res_tag(h->hnd, ResourceId, ResourceTag);

        if ( !rv ) {
                SaHpiRptEntryT *rptentry;

                rptentry = oh_get_resource_by_id(rpt, ResourceId);
                if (!rptentry) {
                         data_access_unlock();
                        return SA_ERR_HPI_NOT_PRESENT;
                }

                rptentry->ResourceTag = *ResourceTag;
        } else
                dbg("Tag set failed for Resource %d", ResourceId);

         data_access_unlock();

        /* to get RSEL entry into infrastructure */
        get_events(rpt);
        return rv;
}

SaErrorT SAHPI_API saHpiResourceIdGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
        RPTable *rpt = default_rpt;
        SaHpiEntityPathT ep;
        SaHpiRptEntryT *rptentry;
        char *on_entitypath = getenv("OPENHPI_ON_EP");

        if (!on_entitypath) {
                return SA_ERR_HPI_UNKNOWN;
        }

        data_access_lock();

        string2entitypath(on_entitypath, &ep);

        rptentry = oh_get_resource_by_ep(rpt, &ep);
        if (!rptentry) {
                data_access_lock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        *ResourceId = rptentry->ResourceId;
         data_access_unlock();

        return SA_OK;
}

/*********************************************************************
 *
 *  Event Log Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiEventLogInfoGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiEventLogInfoT *Info)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        
        /* Test pointer parameters for invalid pointers */
        if (Info == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	
        data_access_lock();
        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_info(d->sel, Info);
                data_access_unlock();

                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId,rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sel_info;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, Info);
        if (rv != SA_OK) {
                dbg("SEL info get failed");
        }

        data_access_unlock();
        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEventLogEntryIdT EntryId,
                SAHPI_OUT SaHpiEventLogEntryIdT *PrevEntryId,
                SAHPI_OUT SaHpiEventLogEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiEventLogEntryT *EventLogEntry,
                SAHPI_INOUT SaHpiRdrT *Rdr,
                SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
        SaErrorT rv;
        SaErrorT (*get_sel_entry)(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT current,
                                  SaHpiEventLogEntryIdT *prev, SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT *entry);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiEventLogEntryT *selentry;
        SaErrorT retc;
        
        /* Test pointer parameters for invalid pointers */
        if (EventLogEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }


        data_access_lock();

        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                retc = oh_sel_get(d->sel, EntryId, PrevEntryId, NextEntryId,
                                  &selentry);
                if (retc != SA_OK) {
                        data_access_unlock();
                        return retc;
                }

                memcpy(EventLogEntry, selentry, sizeof(SaHpiEventLogEntryT));
                data_access_unlock();
                return SA_OK;
        }

        OH_SESSION_SETUP(SessionId,s);

        OH_RPT_GET(SessionId, rpt);

        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_sel_entry = h->abi->get_sel_entry;

        if (!get_sel_entry) {
                dbg("This api is not supported");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_sel_entry(h->hnd, ResourceId, EntryId, PrevEntryId,
                           NextEntryId, EventLogEntry);

        if(rv != SA_OK) {
                dbg("SEL entry get failed");
        }

        if (RptEntry) *RptEntry = *res;
        if (Rdr) {
                SaHpiRdrT *tmprdr = NULL;
                SaHpiUint8T num;
                switch (EventLogEntry->Event.EventType) {
                        case SAHPI_ET_SENSOR:
                                num = EventLogEntry->Event.EventDataUnion.SensorEvent.SensorNum;
                                tmprdr = oh_get_rdr_by_type(rpt,ResourceId,SAHPI_SENSOR_RDR,num);
                                if (tmprdr)
                                         memcpy(Rdr,tmprdr,sizeof(SaHpiRdrT));
                                else dbg("saHpiEventLogEntryGet: Could not find rdr.");
                                break;
                        case SAHPI_ET_WATCHDOG:
                                num = EventLogEntry->Event.EventDataUnion.WatchdogEvent.WatchdogNum;
                                tmprdr = oh_get_rdr_by_type(rpt,ResourceId,SAHPI_WATCHDOG_RDR,num);
                                if (tmprdr)
                                         memcpy(Rdr,tmprdr,sizeof(SaHpiRdrT));
                                else dbg("saHpiEventLogEntryGet: Could not find rdr.");
                                break;
                        default:
                                ;
                }
        }

        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEventT *EvtEntry)
{
        SaErrorT rv;
        SaErrorT (*add_sel_entry)(void *hnd, SaHpiResourceIdT id,
                                  const SaHpiEventT *Event);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;

	if (EvtEntry == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        data_access_lock();

        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_add(d->sel, EvtEntry);
                data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        add_sel_entry = h->abi->add_sel_entry;

        if (!add_sel_entry) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = add_sel_entry(h->hnd, ResourceId, EvtEntry);
        if(rv != SA_OK) {
                dbg("SEL add entry failed");
        }

        data_access_unlock();

        /* to get RSEL entry into infrastructure */
        rv = get_events(rpt);
        if(rv != SA_OK) {
                dbg("Event loop failed");
        }

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogClear (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*clear_sel)(void *hnd, SaHpiResourceIdT id);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;

               data_access_lock();

        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_clear(d->sel);
                      data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have SEL", ResourceId);
                      data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        clear_sel = h->abi->clear_sel;
        if (!clear_sel) {
                      data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = clear_sel(h->hnd, ResourceId);
        if(rv != SA_OK) {
                dbg("SEL delete entry failed");
        }

              data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogTimeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiTimeT *Time)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;
        
        /* Test pointer parameters for invalid pointers */
        if (Time == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv < 0) {
                return rv;
        }

        *Time = info.CurrentTime;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogTimeSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiTimeT Time)
{
        SaErrorT rv;
        SaErrorT (*set_sel_time)(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;

               data_access_lock();
        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_timeset(d->sel, Time);
                      data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                dbg("Resource %d does not have SEL", ResourceId);
                      data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_sel_time = h->abi->set_sel_time;

        if (!set_sel_time) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_sel_time(h->hnd, ResourceId, Time);
        if(rv != SA_OK) {
                dbg("Set SEL time failed");
        }

              data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiBoolT *Enable)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;
        
        /* Test pointer parameters for invalid pointers */
        if (Enable == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv < 0) {
                return rv;
        }

        *Enable = info.Enabled;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiBoolT Enable)
{
        struct oh_domain *d;

               data_access_lock();

        OH_STATE_READY_CHECK;
        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                d->sel->enabled = Enable;
                      data_access_unlock();

                return SA_OK;
        }

        data_access_unlock();

        /* this request is not valid on an RSEL */
        return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT SAHPI_API saHpiEventLogOverflowReset (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiResourceIdT    ResourceId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  Event Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSubscribe (
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
        
        data_access_lock();
        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId,s);

        if (s->event_state != OH_EVENT_UNSUBSCRIBE) {
                dbg("Cannot subscribe if session is not unsubscribed.");
                data_access_unlock();
                return SA_ERR_HPI_DUPLICATE;
        }
        
        s->event_state = OH_EVENT_SUBSCRIBE;

        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiUnsubscribe (
                SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
               data_access_lock();

        OH_STATE_READY_CHECK;

        OH_SESSION_SETUP(SessionId,s);

        if (s->event_state != OH_EVENT_SUBSCRIBE) {
                dbg("Cannot unsubscribe if session is not subscribed.");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* Flush session's event queue */
        GSList *eventq = NULL;
        for (eventq = s->eventq; eventq != NULL;
             eventq = eventq->next) {
                 g_free(eventq->data);
        }
        g_slist_free(s->eventq);
        s->eventq = NULL;

        s->event_state = OH_EVENT_UNSUBSCRIBE;
        data_access_unlock();
        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiTimeoutT Timeout,
                SAHPI_OUT SaHpiEventT *Event,
                SAHPI_INOUT SaHpiRdrT *Rdr,
                SAHPI_INOUT SaHpiRptEntryT *RptEntry,
                SAHPI_INOUT SaHpiEvtQueueStatusT *EventQueueStatus
        )
{
        struct oh_session *s;
        RPTable *rpt;

        SaHpiTimeT now, end;

               data_access_lock();
        OH_STATE_READY_CHECK;

        if (Timeout < SAHPI_TIMEOUT_BLOCK || !Event) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);

        if (s->event_state != OH_EVENT_SUBSCRIBE) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        data_access_unlock();

        gettimeofday1(&now);
        end = now + Timeout;

        do {
                struct oh_session_event e;

                if (get_events(rpt) < 0) {
                        return SA_ERR_HPI_UNKNOWN;
                } else if (session_pop_event(s, &e) < 0) {
                        switch (Timeout) {
                                case SAHPI_TIMEOUT_IMMEDIATE:
                                        return SA_ERR_HPI_TIMEOUT;

                                case SAHPI_TIMEOUT_BLOCK:
                                        break;

                                default: /* Check if we have timed out */
                                        gettimeofday1(&now);
                                        if (now >= end) {
                                                return SA_ERR_HPI_TIMEOUT;
                                        }
                        }
                        /* Sleep for 1 milli second */
                        struct timespec req = { 0, 1000000 }, rem;
                        nanosleep(&req, &rem);

                } else { /* Return event, resource and rdr */
                        memcpy(Event, &e.event, sizeof(*Event));

                        data_access_lock();

                        if (RptEntry)
                             memcpy(RptEntry, &e.rpt_entry, sizeof(*RptEntry));

                        if (Rdr)
                             memcpy(Rdr, &e.rdr, sizeof(*Rdr));

                        data_access_unlock();

                        return SA_OK;
                }

        } while (1);
}

SaErrorT SAHPI_API saHpiEventAdd (
        SAHPI_IN SaHpiSessionIdT      SessionId,
        SAHPI_IN SaHpiEventT          *EvtEntry)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  DAT Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiAlarmGetNext ( 
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiSeverityT             Severity,
    SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
    SAHPI_INOUT SaHpiAlarmT             *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmGet( 
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_OUT SaHpiAlarmT               *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmAcknowledge( 
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmAdd( 
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_INOUT SaHpiAlarmT             *Alarm)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAlarmDelete( 
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  RDR Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRdrGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEntryIdT EntryId,
                SAHPI_OUT SaHpiEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiRdrT *Rdr)
{
        struct oh_session *s;
        RPTable *rpt = NULL;
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        SaHpiRdrT *rdr_next;

        /* Test pointer parameters for invalid pointers */
        if ((Rdr == NULL)||(NextEntryId == NULL))
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);

        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                dbg("No RDRs for Resource %d",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        if(EntryId == SAHPI_FIRST_ENTRY) {
                rdr_cur = oh_get_rdr_next(rpt, ResourceId, SAHPI_FIRST_ENTRY);
        } else {
                rdr_cur = oh_get_rdr_by_id(rpt, ResourceId, EntryId);
        }

        if (rdr_cur == NULL) {
                dbg("Requested RDR, Resource[%d]->RDR[%d], is not present",
                    ResourceId, EntryId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(Rdr, rdr_cur, sizeof(*Rdr));

        rdr_next = oh_get_rdr_next(rpt, ResourceId, rdr_cur->RecordId);
        if(rdr_next == NULL) {
                *NextEntryId = SAHPI_LAST_ENTRY;
        } else {
                *NextEntryId = rdr_next->RecordId;
        }

        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiRdrGetByInstrumentId (
        SAHPI_IN  SaHpiSessionIdT        SessionId,
        SAHPI_IN  SaHpiResourceIdT       ResourceId,
        SAHPI_IN  SaHpiRdrTypeT          RdrType,
        SAHPI_IN  SaHpiInstrumentIdT     InstrumentId,
        SAHPI_OUT SaHpiRdrT              *Rdr)
{
        struct oh_session *s;
        RPTable *rpt = NULL;
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        
        /* Test pointer parameters for invalid pointers */
        if (Rdr == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        data_access_lock();
        
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        
        OH_RESOURCE_GET(rpt, ResourceId, res);
        
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                dbg("No RDRs for Resource %d",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(rpt, ResourceId, RdrType, InstrumentId);

        if (rdr_cur == NULL) {
                dbg("Requested RDR, Resource[%d]->RDR[%d,%d], is not present",
                    ResourceId, RdrType, InstrumentId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        data_access_unlock();

        return SA_OK;
        // return SA_ERR_HPI_UNSUPPORTED_API;
}

/*********************************************************************
 *
 *  Sensor Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSensorReadingGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_INOUT SaHpiSensorReadingT *Reading,
                SAHPI_INOUT SaHpiEventStateT *EventState)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT, SaHpiSensorReadingT *);

        struct oh_session *s;
        RPTable *rpt;
        struct oh_handler *h;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiSensorRecT *sensor;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sensor_data;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, SensorNum, Reading);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaErrorT (*set_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                         const SaHpiSensorThresholdsT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiSensorRecT *sensor;
        struct oh_handler *h;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_sensor_thresholds;

        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, SensorNum, SensorThresholds);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiSensorNumT, SaHpiSensorThresholdsT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sensor_thresholds;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, SensorNum, SensorThresholds);

        data_access_unlock();

        return rv;
}

/* Function: SaHpiSensorTypeGet */
/* Core SAF_HPI function */
/* Not mapped to plugin */
/* Data in RDR */
SaErrorT SAHPI_API saHpiSensorTypeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorTypeT *Type,
                SAHPI_OUT SaHpiEventCategoryT *Category)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor num %d found for Resource %d", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        if (!memcpy(Type, &(rdr->RdrTypeUnion.SensorRec.Type),
                    sizeof(SaHpiSensorTypeT))) {
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }

        if (!memcpy(Category, &(rdr->RdrTypeUnion.SensorRec.Category),
                    sizeof(SaHpiEventCategoryT))) {
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }

        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorEventEnableGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiBoolT *SensorEventsEnabled)
{
        SaErrorT rv;
        SaErrorT (*get_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             SaHpiBoolT *enables);
        
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_sensor_event_enables = h->abi->get_sensor_event_enables;

        if (!get_sensor_event_enables) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_sensor_event_enables(h->hnd, ResourceId, SensorNum, SensorEventsEnabled);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventEnableSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiBoolT SensorEventsEnabled)
{
        SaErrorT rv;
        SaErrorT (*set_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             const SaHpiBoolT enables);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_sensor_event_enables = h->abi->set_sensor_event_enables;

        if (!set_sensor_event_enables) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_sensor_event_enables(h->hnd, ResourceId, SensorNum, SensorEventsEnabled);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventMasksGet (
        SAHPI_IN  SaHpiSessionIdT         SessionId,
        SAHPI_IN  SaHpiResourceIdT        ResourceId,
        SAHPI_IN  SaHpiSensorNumT         SensorNum,
        SAHPI_INOUT SaHpiEventStateT      *AssertEventMask,
        SAHPI_INOUT SaHpiEventStateT      *DeassertEventMask)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorEventMasksSet (
        SAHPI_IN  SaHpiSessionIdT                 SessionId,
        SAHPI_IN  SaHpiResourceIdT                ResourceId,
        SAHPI_IN  SaHpiSensorNumT                 SensorNum,
        SAHPI_IN  SaHpiSensorEventMaskActionT     Action,
        SAHPI_IN  SaHpiEventStateT                AssertEventMask,
        SAHPI_IN  SaHpiEventStateT                DeassertEventMask)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/* End Sensor functions */

/*********************************************************************
 *
 *  Control Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiControlTypeGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_OUT SaHpiCtrlTypeT *Type)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have .ResourceCapabilities flag:"
                    " SAHPI_CAPABILITY_CONTROL set ",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_CTRL_RDR, CtrlNum);
        if (!rdr) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        if (!memcpy(Type, &(rdr->RdrTypeUnion.CtrlRec.Type),
                    sizeof(SaHpiCtrlTypeT))) {
                data_access_unlock();
                return SA_ERR_HPI_ERROR;
        }

        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiControlGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_OUT SaHpiCtrlModeT *CtrlMode,
                SAHPI_INOUT SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT, 
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have controls",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_control_state;
        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, CtrlNum, CtrlMode, CtrlState);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiControlSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_IN SaHpiCtrlModeT CtrlMode,
                SAHPI_IN SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT, SaHpiCtrlModeT, SaHpiCtrlStateT *);

        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                dbg("Resource %d doesn't have controls",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_control_state;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, CtrlNum, CtrlMode, CtrlState);
        data_access_unlock();

        return rv;
}

/* current sahpi.h missed SA_ERR_INVENT_DATA_TRUNCATED */
// #ifndef SA_ERR_INVENT_DATA_TRUNCATED
//#warning "No 'SA_ERR_INVENT_DATA_TRUNCATED 'definition in sahpi.h!"
// #define SA_ERR_INVENT_DATA_TRUNCATED    (SaErrorT)(SA_HPI_ERR_BASE - 1000)
// #endif

#if 0

SaErrorT SAHPI_API saHpiEntityInventoryDataRead (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEirIdT EirId,
                SAHPI_IN SaHpiUint32T BufferSize,
                SAHPI_OUT SaHpiInventoryDataT *InventData,
                SAHPI_OUT SaHpiUint32T *ActualSize)
{
        SaErrorT rv;
        SaErrorT (*get_size)(void *, SaHpiResourceIdT, SaHpiEirIdT, SaHpiUint32T *);
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiEirIdT, SaHpiInventoryDataT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_size = h->abi->get_inventory_size;
        get_func = h->abi->get_inventory_info;
        if (!get_func || !get_size) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
         }

	if (ActualSize == NULL) {
                dbg("NULL pointer for ActualSize storage\n");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
	}

        rv = get_size(h->hnd, ResourceId, EirId, ActualSize);
        if ( rv ) {
                data_access_unlock();
                return rv;
        }

        if (*ActualSize>BufferSize) {
                data_access_unlock();
                return SA_ERR_INVENT_DATA_TRUNCATED;
        }

	if (InventData == NULL) {
                dbg("Call does not have buffer to store inventory data\n");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
	}

        rv = get_func(h->hnd, ResourceId, EirId, InventData);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiEntityInventoryDataWrite (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiEirIdT EirId,
                SAHPI_IN SaHpiInventoryDataT *InventData)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiEirIdT, const SaHpiInventoryDataT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                dbg("Resource %d doesn't have inventory data",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_inventory_info;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, EirId, InventData);
        data_access_unlock();

        return rv;
}

// Peter gets to recode inventory

#endif

SaErrorT SAHPI_API saHpiIdrInfoGet( 
        SAHPI_IN SaHpiSessionIdT         SessionId,
        SAHPI_IN SaHpiResourceIdT        ResourceId,
        SAHPI_IN SaHpiIdrIdT             IdrId,
        SAHPI_OUT SaHpiIdrInfoT          *IdrInfo)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrAreaHeaderGet( 
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
        SAHPI_IN SaHpiEntryIdT            AreaId,
        SAHPI_OUT SaHpiEntryIdT           *NextAreaId,
        SAHPI_OUT SaHpiIdrAreaHeaderT     *Header)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrAreaAdd(
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
        SAHPI_OUT SaHpiEntryIdT           *AreaId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrAreaDelete( 
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiIdrIdT            IdrId,
    SAHPI_IN SaHpiEntryIdT          AreaId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrFieldGet( 
        SAHPI_IN SaHpiSessionIdT         SessionId,
        SAHPI_IN SaHpiResourceIdT        ResourceId,
        SAHPI_IN SaHpiIdrIdT             IdrId,
        SAHPI_IN SaHpiEntryIdT           AreaId,
        SAHPI_IN SaHpiIdrFieldTypeT      FieldType,
        SAHPI_IN SaHpiEntryIdT           FieldId,
        SAHPI_OUT SaHpiEntryIdT          *NextFieldId,
        SAHPI_OUT SaHpiIdrFieldT         *Field)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrFieldAdd( 
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_INOUT SaHpiIdrFieldT        *Field)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrFieldSet( 
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiIdrFieldT           *Field)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiIdrFieldDelete( 
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_IN SaHpiEntryIdT            AreaId,
        SAHPI_IN SaHpiEntryIdT            FieldId)
{
        return SA_ERR_HPI_UNSUPPORTED_API; 
}

/*********************************************************************
 *
 *  Watchdog Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                SAHPI_OUT SaHpiWatchdogT *Watchdog)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT, SaHpiWatchdogT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_watchdog_info;
        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, WatchdogNum, Watchdog);

        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                SAHPI_IN SaHpiWatchdogT *Watchdog)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT, SaHpiWatchdogT *);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

              data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_watchdog_info;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, WatchdogNum, Watchdog);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
        SaErrorT rv;
        SaErrorT (*reset_func)(void *, SaHpiResourceIdT, SaHpiWatchdogNumT);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);
        
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                dbg("Resource %d doesn't have watchdog",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        reset_func = h->abi->reset_watchdog;
        if (!reset_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = reset_func(h->hnd, ResourceId, WatchdogNum);
        data_access_unlock();

        return rv;
}

/*******************************************************************************
 *
 *  Annunciator Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiAnnunciatorGetNext( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiSeverityT             Severity,
        SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
        SAHPI_INOUT SaHpiAnnouncementT      *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorGet( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_OUT SaHpiAnnouncementT        *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorAcknowledge( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorAdd( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_INOUT SaHpiAnnouncementT      *Announcement)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorDelete( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeGet( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_OUT SaHpiAnnunciatorModeT     *Mode)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeSet( 
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiAnnunciatorModeT      Mode)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/*******************************************************************************
 *
 *  Hotswap Functions
 *
 ******************************************************************************/

#if 0 // this is not in HPI B, will come out later
SaErrorT SAHPI_API saHpiHotSwapControlRequest (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;

              data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(rpt, ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rd->controlled = 1;
        data_access_unlock();

        return SA_OK;
}

#endif

SaErrorT SAHPI_API saHpiHotSwapPolicyCancel (
        SAHPI_IN SaHpiSessionIdT      SessionId,
        SAHPI_IN SaHpiResourceIdT     ResourceId)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceActiveSet (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*set_hotswap_state)(void *hnd, SaHpiResourceIdT,
                        SaHpiHsStateT state);

        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_resource_data *rd;

              data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        rd = oh_get_resource_data(rpt, ResourceId);
        if (!rd) {
                dbg( "Can't find resource data for Resource %d", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!rd->controlled) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        set_hotswap_state = h->abi->set_hotswap_state;
        if (!set_hotswap_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        /* this was done in the old code, so we do it here */
        rd->controlled = 0;

        rv = set_hotswap_state(h->hnd, ResourceId, SAHPI_HS_STATE_ACTIVE);

        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiResourceInactiveSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaErrorT (*set_hotswap_state)(void *hnd, SaHpiResourceIdT rid,
                                      SaHpiHsStateT state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_resource_data *rd;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        rd = oh_get_resource_data(rpt, ResourceId);
        if (!rd) {
                dbg( "Can't find resource data for Resource %d", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!rd->controlled) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        set_hotswap_state = h->abi->set_hotswap_state;
        if (!set_hotswap_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rd->controlled = 0;

        rv = set_hotswap_state(h->hnd, ResourceId, SAHPI_HS_STATE_INACTIVE);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiTimeoutT *Timeout)
{
        struct oh_session *s;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);

        *Timeout = get_hotswap_auto_insert_timeout();
               data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiTimeoutT Timeout)
{
        struct oh_session *s;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);

        set_hotswap_auto_insert_timeout(Timeout);
        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiTimeoutT *Timeout)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(rpt, ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        *Timeout = rd->auto_extract_timeout;
        data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiTimeoutT Timeout)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_resource_data *rd;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        rd = oh_get_resource_data(rpt, ResourceId);
        if (!rd) {
                dbg("Cannot find resource data for Resource %d", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rd->auto_extract_timeout = Timeout;
               data_access_unlock();

        return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiHsStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_hotswap_state)(void *hnd, SaHpiResourceIdT rid,
                                 SaHpiHsStateT *state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_hotswap_state = h->abi->get_hotswap_state;
        if (!get_hotswap_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_hotswap_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiHsActionT Action)
{
        SaErrorT rv;
        SaErrorT (*request_hotswap_action)(void *hnd, SaHpiResourceIdT rid,
                        SaHpiHsActionT act);

        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        request_hotswap_action = h->abi->request_hotswap_action;
        if (!request_hotswap_action) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = request_hotswap_action(h->hnd, ResourceId, Action);
        data_access_unlock();

        get_events(rpt);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                        SaHpiHsIndicatorStateT *state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_indicator_state = h->abi->get_indicator_state;
        if (!get_indicator_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_indicator_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiHsIndicatorStateT State)
{
        SaErrorT rv;
        SaErrorT (*set_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                        SaHpiHsIndicatorStateT state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_indicator_state = h->abi->set_indicator_state;
        if (!set_indicator_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_indicator_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

/*******************************************************************************
 *
 *  Configuration Function(s)
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiParmControl (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiParmActionT Action)
{
        SaErrorT rv;
        SaErrorT (*control_parm)(void *, SaHpiResourceIdT, SaHpiParmActionT);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        control_parm = h->abi->control_parm;
        if (!control_parm) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = control_parm(h->hnd, ResourceId, Action);
        data_access_unlock();

        return rv;
}

/*******************************************************************************
 *
 *  Reset Functions
 *
 ******************************************************************************/


SaErrorT SAHPI_API saHpiResourceResetStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiResetActionT *ResetAction)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiResetActionT *);

        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_reset_state;
        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_func(h->hnd, ResourceId, ResetAction);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiResourceResetStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiResetActionT ResetAction)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiResetActionT);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_reset_state;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_func(h->hnd, ResourceId, ResetAction);
        data_access_unlock();

        return rv;
}

/*******************************************************************************
 *
 *  Power Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiPowerStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_power_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiPowerStateT *state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_power_state = h->abi->get_power_state;
        if (!get_power_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = get_power_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiPowerStateT State)
{
        SaErrorT rv;
        SaErrorT (*set_power_state)(void *hnd, SaHpiResourceIdT id,
                                    SaHpiPowerStateT state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                data_access_unlock();
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_power_state = h->abi->set_power_state;
        if (!set_power_state) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = set_power_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}


