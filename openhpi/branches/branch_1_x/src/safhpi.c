/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003
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
 */

#include <stdlib.h>
#include <string.h>
#include <oh_config.h>
#include <openhpi.h>
#include <oh_plugin.h>
#include <SaHpi.h>
#include <uid_utils.h>
#include <epath_utils.h>

#include <pthread.h>

/******************************************************************************
 *
 *  Macros needed for clarity
 *
 *****************************************************************************/
static enum {
        OH_STAT_UNINIT,
        OH_STAT_READY,
} oh_hpi_state = OH_STAT_UNINIT;

#define OH_STATE_READY_CHECK                                      \
        do {                                                      \
                if (OH_STAT_READY != oh_hpi_state) {              \
                        dbg("Uninitialized HPI");                 \
                        data_access_unlock();                     \
                        return SA_ERR_HPI_UNINITIALIZED;          \
                }                                                 \
        } while(0)

/*
 * OH_SESSION_SETUP gets the session pointer for the session
 * id.  It returns badly if required.  This is only to be used for sahpi
 * function.
 */

#define OH_SESSION_SETUP(sid, ses)                         \
        do {                                               \
                ses = session_get(sid);                    \
                if (!ses) {                                \
                        dbg("Invalid SessionId %d", sid);  \
                        data_access_unlock();              \
                        return SA_ERR_HPI_INVALID_SESSION; \
                }                                          \
        } while (0)

/*
 * OH_HANDLER_GET gets the hander for the rpt and resource id.  It
 * returns INVALID PARAMS if the handler isn't there
 */
#define OH_HANDLER_GET(rpt,rid,h)                                       \
        do {                                                            \
                struct oh_resource_data *rd;                            \
                if(rpt == NULL) {                                       \
                        dbg("Invalide RPTable");                        \
                        data_access_unlock();                           \
                        return SA_ERR_HPI_INVALID_SESSION;              \
                }                                                       \
                rd = oh_get_resource_data(rpt, rid);                    \
                if(!rd || !rd->handler) {                               \
                        dbg("Can't find handler for Resource %d", rid); \
                        data_access_unlock();                           \
                        return SA_ERR_HPI_INVALID_PARAMS;               \
                }                                                       \
                h = rd->handler;                                        \
        } while(0)

/*
 * OH_RPT_GET - sets up rpt table, and does a sanity check that it
 * is actually valid, returning badly if it isn't
 */

#define OH_RPT_GET(SessionId, rpt)                                     \
        do {                                                           \
                rpt = default_rpt;                                     \
                if(rpt == NULL) {                                      \
                        dbg("RPT for Session %d not found",SessionId); \
                        data_access_unlock();                          \
                        return SA_ERR_HPI_INVALID_SESSION;             \
                }                                                      \
        } while(0)


/*
 * OH_RESOURCE_GET gets the resource for an resource id and rpt
 * it returns invalid resource if no resource id is found
 */
#define OH_RESOURCE_GET(rpt, rid, r)                            \
        do {                                                    \
                r = oh_get_resource_by_id(rpt, rid);            \
                if(!r) {                                        \
                        dbg("Resource %d doesn't exist", rid);  \
                        data_access_unlock();                   \
                        return SA_ERR_HPI_INVALID_RESOURCE;     \
                }                                               \
        } while(0)

/***********************************************************************
 * Begin SAHPI functions.  For full documentation please see
 * the specification
 **********************************************************************/

SaErrorT SAHPI_API saHpiInitialize(SAHPI_OUT SaHpiVersionT *HpiImplVersion)
{
        struct oh_plugin_config *tmpp;
        GHashTable *tmph;

        unsigned int i;
        char *openhpi_conf;

        int rval;

        data_access_lock();

        if (OH_STAT_UNINIT != oh_hpi_state) {
                dbg("Cannot initialize twice");
                data_access_unlock();
                return SA_ERR_HPI_DUPLICATE;
        }

        *HpiImplVersion = SAHPI_INTERFACE_VERSION;
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

SaErrorT SAHPI_API saHpiFinalize(void)
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

SaErrorT SAHPI_API saHpiSessionOpen(
                SAHPI_IN SaHpiDomainIdT DomainId,
                SAHPI_OUT SaHpiSessionIdT *SessionId,
                SAHPI_IN void *SecurityParams)
{
        struct oh_session *s;
        int rv;

        if (SecurityParams != NULL) {
                dbg("SecurityParams must be NULL");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_STATE_READY_CHECK;

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


SaErrorT SAHPI_API saHpiResourcesDiscover(SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
        GSList *i;
        int rv = -1;

        OH_STATE_READY_CHECK;

        data_access_lock();

        OH_SESSION_SETUP(SessionId, s);

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

        rv = get_events();
        if (rv < 0) {
                dbg("Error attempting to process resources");
                return SA_ERR_HPI_UNKNOWN;
        }

        return SA_OK;
}

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

        rv = get_events();

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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        if (set_res_sev(h->hnd, ResourceId, Severity) < 0) {
                dbg("SEL add entry failed");
                data_access_unlock();
                return SA_ERR_HPI_UNKNOWN;
        }

         data_access_unlock();

        /* to get rpt entry into infrastructure */
        get_events();
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
                return SA_ERR_HPI_UNSUPPORTED_API;
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
        get_events();
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

SaErrorT SAHPI_API saHpiEntitySchemaGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_OUT SaHpiUint32T *SchemaId)
{
        /* we must return 0 for now, as we don't determine
           schemas yet */
        *SchemaId = 0;
        return SA_OK;
}


SaErrorT SAHPI_API saHpiEventLogInfoGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiSelInfoT *Info)
{
        SaErrorT rv;
        SaErrorT (*get_func) (void *, SaHpiResourceIdT, SaHpiSelInfoT *);
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
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_info(d->sel, Info);
                data_access_unlock();

                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId,rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sel_info;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                SAHPI_IN SaHpiSelEntryIdT EntryId,
                SAHPI_OUT SaHpiSelEntryIdT *PrevEntryId,
                SAHPI_OUT SaHpiSelEntryIdT *NextEntryId,
                SAHPI_OUT SaHpiSelEntryT *EventLogEntry,
                SAHPI_INOUT SaHpiRdrT *Rdr,
                SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
        SaErrorT rv;
        SaErrorT (*get_sel_entry)(void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT current,
                                  SaHpiSelEntryIdT *prev, SaHpiSelEntryIdT *next, SaHpiSelEntryT *entry);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiSelEntryT *selentry;
        SaErrorT retc;
        
        /* Test pointer parameters for invalid pointers */
        if (EventLogEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }


        data_access_lock();

        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                retc = oh_sel_get(d->sel, EntryId, PrevEntryId, NextEntryId,
                                  &selentry);
                if (retc != SA_OK) {
                        data_access_unlock();
                        return retc;
                }

                memcpy(EventLogEntry, selentry, sizeof(SaHpiSelEntryT));
                data_access_unlock();
                return SA_OK;
        }

        OH_SESSION_SETUP(SessionId,s);

        OH_RPT_GET(SessionId, rpt);

        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_sel_entry = h->abi->get_sel_entry;

        if (!get_sel_entry) {
                dbg("This api is not supported");
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = get_sel_entry(h->hnd, ResourceId, EntryId, PrevEntryId,
                           NextEntryId, EventLogEntry);

        if(rv != SA_OK) {
                dbg("SEL entry get failed");
        }

        if ((RptEntry != NULL) && 
            (EventLogEntry != NULL) &&
            (EventLogEntry->Event.Source)) {
                
                OH_RESOURCE_GET(rpt, EventLogEntry->Event.Source, res);
                if(res != NULL) {
                        *RptEntry = *res;
                }
        }
        if ((EventLogEntry != NULL) &&
            (res != NULL) &&
            (Rdr != NULL)) {
                SaHpiRdrT *tmprdr = NULL;
                SaHpiUint8T num;
                switch (EventLogEntry->Event.EventType) {
                        case SAHPI_ET_SENSOR:
                                num = EventLogEntry->Event.EventDataUnion.SensorEvent.SensorNum;
                                tmprdr = oh_get_rdr_by_type(rpt,res->ResourceId,SAHPI_SENSOR_RDR,num);
                                if (tmprdr)
                                         memcpy(Rdr,tmprdr,sizeof(SaHpiRdrT));
                                else dbg("saHpiEventLogEntryGet: Could not find rdr.");
                                break;
                        case SAHPI_ET_WATCHDOG:
                                num = EventLogEntry->Event.EventDataUnion.WatchdogEvent.WatchdogNum;
                                tmprdr = oh_get_rdr_by_type(rpt,res->ResourceId,SAHPI_WATCHDOG_RDR,num);
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
                SAHPI_IN SaHpiSelEntryT *EvtEntry)
{
        SaErrorT rv;
        SaErrorT (*add_sel_entry)(void *hnd, SaHpiResourceIdT id,
                                  const SaHpiSelEntryT *Event);
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
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_add(d->sel, EvtEntry);
                data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        add_sel_entry = h->abi->add_sel_entry;

        if (!add_sel_entry) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = add_sel_entry(h->hnd, ResourceId, EvtEntry);
        if(rv != SA_OK) {
                dbg("SEL add entry failed");
        }

        data_access_unlock();

        /* to get RSEL entry into infrastructure */
        rv = get_events();
        if(rv != SA_OK) {
                dbg("Event loop failed");
        }

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryDelete (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSelEntryIdT EntryId)
{
        SaErrorT rv;
        SaErrorT (*del_sel_entry)(void *hnd, SaHpiResourceIdT id,
                                  SaHpiSelEntryIdT sid);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();

        OH_STATE_READY_CHECK;

        /* test for special domain case */
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                dbg("SEL does not support delete");
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        del_sel_entry = h->abi->del_sel_entry;

        if (!del_sel_entry) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = del_sel_entry(h->hnd, ResourceId, EntryId);
        if(rv != SA_OK) {
                dbg("SEL delete entry failed");
        }

              data_access_unlock();

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
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_clear(d->sel);
                      data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                      data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        clear_sel = h->abi->clear_sel;
        if (!clear_sel) {
                      data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
        SaHpiSelInfoT info;
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
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                rv = oh_sel_timeset(d->sel, Time);
                      data_access_unlock();
                return rv;
        }

        OH_SESSION_SETUP(SessionId,s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                dbg("Resource %d does not have SEL", ResourceId);
                      data_access_unlock();
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_sel_time = h->abi->set_sel_time;

        if (!set_sel_time) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
        SaHpiSelInfoT info;
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
        if (ResourceId == SAHPI_DOMAIN_CONTROLLER_ID) {
                d = get_domain_by_id(OH_DEFAULT_DOMAIN_ID);
                d->sel->enabled = Enable;
                      data_access_unlock();

                return SA_OK;
        }

        data_access_unlock();

        /* this request is not valid on an RSEL */
        return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT SAHPI_API saHpiSubscribe (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiBoolT ProvideActiveAlarms)
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

        if (!ProvideActiveAlarms) {
                /* Flush session's event queue */
                GSList *eventq = NULL;
                for (eventq = s->eventq; eventq != NULL;
                     eventq = eventq->next) {
                         g_free(eventq->data);
                }
                g_slist_free(s->eventq);
                s->eventq = NULL;
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
                SAHPI_INOUT SaHpiRptEntryT *RptEntry)
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

                if (get_events() < 0) {
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
                return SA_ERR_HPI_INVALID_REQUEST;
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

/* Sensor data functions */

SaErrorT SAHPI_API saHpiSensorReadingGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorReadingT *Reading)
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        if(sensor->Ignore) {
                dbg("Sensor %d, ResourceId %d set to ignore", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sensor_data;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = get_func(h->hnd, ResourceId, SensorNum, Reading);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorReadingConvert (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiSensorReadingT *ReadingInput,
                SAHPI_OUT SaHpiSensorReadingT *ConvertedReading)
{
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiSensorRecT *sensor;
        SaHpiSensorReadingFormatsT format;

        data_access_lock();

        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                dbg("Resource %d doesn't have sensors",ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        if(sensor->Ignore) {
                dbg("Sensor %d, ResourceId %d set to ignore", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* if ReadingInput neither contains a raw nor a intepreted value or
           if it contains both, return an error */
        format = ReadingInput->ValuesPresent
                & (SAHPI_SRF_RAW|SAHPI_SRF_INTERPRETED);

        if (format == 0 || format == (SAHPI_SRF_RAW|SAHPI_SRF_INTERPRETED)) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* cannot convert non numeric values */
        if (!sensor->DataFormat.IsNumeric) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_DATA;
        }

        /* if the conversion factors vary over sensor
           range, return an error */
        if (!sensor->DataFormat.FactorsStatic) {
                data_access_unlock();
                return  SA_ERR_HPI_INVALID_DATA;
        }

        /* there might be event bits turned on, so we need this to be & not = */
        if (ReadingInput->ValuesPresent & SAHPI_SRF_RAW) {
                ConvertedReading->ValuesPresent = SAHPI_SRF_INTERPRETED;
                ConvertedReading->Interpreted.Type =
                        SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;

                SaErrorT rv = sensor_convert_from_raw( sensor,
                                                          ReadingInput->Raw,
                                                       &ConvertedReading->Interpreted
                                                       .Value.SensorFloat32);

                data_access_unlock();
                return rv;
        }

        /* only float is supported */
        if (   ReadingInput->Interpreted.Type
            != SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID_DATA;
        }

        ConvertedReading->ValuesPresent = SAHPI_SRF_RAW;

        SaErrorT rv = sensor_convert_to_raw(sensor,
                                     ReadingInput->Interpreted.Value
                                      .SensorFloat32,
                                     &ConvertedReading->Raw);

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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        rdr = oh_get_rdr_by_type(rpt, ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                dbg("No Sensor %d found for ResourceId %d", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_NOT_PRESENT;
        }

        sensor = &(rdr->RdrTypeUnion.SensorRec);

        if(sensor->Ignore) {
                dbg("Sensor %d, ResourceId %d set to ignore", SensorNum, ResourceId);
                data_access_unlock();
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_sensor_thresholds;

        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_sensor_thresholds;

        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID_REQUEST;
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

SaErrorT SAHPI_API saHpiSensorEventEnablesGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_OUT SaHpiSensorEvtEnablesT *Enables)
{
        SaErrorT rv;
        SaErrorT (*get_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             SaHpiSensorEvtEnablesT *enables);
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_sensor_event_enables = h->abi->get_sensor_event_enables;

        if (!get_sensor_event_enables) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = get_sensor_event_enables(h->hnd, ResourceId, SensorNum, Enables);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventEnablesSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiSensorNumT SensorNum,
                SAHPI_IN SaHpiSensorEvtEnablesT *Enables)
{
        SaErrorT rv;
        SaErrorT (*set_sensor_event_enables)(void *hnd, SaHpiResourceIdT,
                                             SaHpiSensorNumT,
                                             const SaHpiSensorEvtEnablesT *enables);
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_sensor_event_enables = h->abi->set_sensor_event_enables;

        if (!set_sensor_event_enables) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = set_sensor_event_enables(h->hnd, ResourceId, SensorNum, Enables);
        data_access_unlock();

        return rv;
}

/* End Sensor functions */

/* Control data functions */

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
                return SA_ERR_HPI_INVALID_REQUEST;
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

SaErrorT SAHPI_API saHpiControlStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_INOUT SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*get_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT, SaHpiCtrlStateT *);
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_control_state;
        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = get_func(h->hnd, ResourceId, CtrlNum, CtrlState);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiControlStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiCtrlNumT CtrlNum,
                SAHPI_IN SaHpiCtrlStateT *CtrlState)
{
        SaErrorT rv;
        SaErrorT (*set_func)(void *, SaHpiResourceIdT, SaHpiCtrlNumT, SaHpiCtrlStateT *);

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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_control_state;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = set_func(h->hnd, ResourceId, CtrlNum, CtrlState);
        data_access_unlock();

        return rv;
}

/* current sahpi.h missed SA_ERR_INVENT_DATA_TRUNCATED */
#ifndef SA_ERR_INVENT_DATA_TRUNCATED
//#warning "No 'SA_ERR_INVENT_DATA_TRUNCATED 'definition in sahpi.h!"
#define SA_ERR_INVENT_DATA_TRUNCATED    (SaErrorT)(SA_HPI_ERR_BASE - 1000)
#endif

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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_size = h->abi->get_inventory_size;
        get_func = h->abi->get_inventory_info;
        if (!get_func || !get_size) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_inventory_info;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = set_func(h->hnd, ResourceId, EirId, InventData);
        data_access_unlock();

        return rv;
}

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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_func = h->abi->get_watchdog_info;
        if (!get_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_func = h->abi->set_watchdog_info;
        if (!set_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        reset_func = h->abi->reset_watchdog;
        if (!reset_func) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = reset_func(h->hnd, ResourceId, WatchdogNum);
        data_access_unlock();

        return rv;
}

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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        /* this was done in the old code, so we do it here */
        rd->controlled = 0;

        rv = set_hotswap_state(h->hnd, ResourceId, SAHPI_HS_STATE_ACTIVE_HEALTHY);

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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_INVALID;
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
                return SA_ERR_HPI_INVALID;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_hotswap_state = h->abi->get_hotswap_state;
        if (!get_hotswap_state) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        request_hotswap_action = h->abi->request_hotswap_action;
        if (!request_hotswap_action) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = request_hotswap_action(h->hnd, ResourceId, Action);
        data_access_unlock();

        get_events();

        return rv;
}

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_OUT SaHpiHsPowerStateT *State)
{
        SaErrorT rv;
        SaErrorT (*get_power_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiHsPowerStateT *state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

#if 0
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        }
#endif

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_power_state = h->abi->get_power_state;
        if (!get_power_state) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = get_power_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiResourceIdT ResourceId,
                SAHPI_IN SaHpiHsPowerStateT State)
{
        SaErrorT rv;
        SaErrorT (*set_power_state)(void *hnd, SaHpiResourceIdT id,
                                    SaHpiHsPowerStateT state);
        struct oh_session *s;
        RPTable *rpt;
        SaHpiRptEntryT *res;
        struct oh_handler *h;

        data_access_lock();
        OH_STATE_READY_CHECK;
        OH_SESSION_SETUP(SessionId, s);
        OH_RPT_GET(SessionId, rpt);
        OH_RESOURCE_GET(rpt, ResourceId, res);

#if 0
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        }
#endif

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_power_state = h->abi->set_power_state;
        if (!set_power_state) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = set_power_state(h->hnd, ResourceId, State);
        data_access_unlock();

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
                return SA_ERR_HPI_INVALID;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        get_indicator_state = h->abi->get_indicator_state;
        if (!get_indicator_state) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
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
                return SA_ERR_HPI_INVALID;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        set_indicator_state = h->abi->set_indicator_state;
        if (!set_indicator_state) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = set_indicator_state(h->hnd, ResourceId, State);
        data_access_unlock();

        return rv;
}

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
                return SA_ERR_HPI_INVALID;
        }

        OH_HANDLER_GET(rpt, ResourceId, h);

        control_parm = h->abi->control_parm;
        if (!control_parm) {
                data_access_unlock();
                return SA_ERR_HPI_UNSUPPORTED_API;
        }

        rv = control_parm(h->hnd, ResourceId, Action);
        data_access_unlock();

        return rv;
}

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

#if 0
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        }
#endif

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

#if 0
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        }
#endif

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
