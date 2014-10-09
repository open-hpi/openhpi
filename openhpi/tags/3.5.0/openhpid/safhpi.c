/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2007
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
 *     Renier Morales <renier@openhpi.org>
 *     Racing Guo <racing.guo@intel.com>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *     Lars Wetzel <larswetzel@users.sourceforge.net>
 */

#include <string.h>

#include <SaHpi.h>

#include <oh_domain.h>
#include <oh_error.h>
#include <oh_handler.h>
#include <oh_plugin.h>
#include <oh_session.h>
#include <oh_utils.h>
#include <sahpimacros.h>

#include "alarm.h"
#include "conf.h"
#include "event.h"
#include "hotswap.h"
#include "init.h"
#include "threaded.h"


/*********************************************************************
 *
 * Begin SAHPI B.03.01 Functions. For full documentation please see
 * the specification
 *
 ********************************************************************/

SaHpiVersionT SAHPI_API saHpiVersionGet ()
{
        return SAHPI_INTERFACE_VERSION;
}

SaErrorT SAHPI_API saHpiSessionOpen(
        SAHPI_IN  SaHpiDomainIdT  DomainId,
        SAHPI_OUT SaHpiSessionIdT *SessionId,
        SAHPI_IN  void            *SecurityParams)
{
        SaHpiSessionIdT sid;
        SaHpiDomainIdT did = DomainId;


        if (SessionId == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Security Params required to be NULL by the spec at this point */
        if (SecurityParams != NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Initialize Library - This will only run once */
        if (oh_init()) return SA_ERR_HPI_INTERNAL_ERROR;

        sid = oh_create_session(did);

        if(!sid) {
                return SA_ERR_HPI_INVALID_DOMAIN;
        }

        *SessionId = sid;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiSessionClose(
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        OH_CHECK_INIT_STATE(SessionId);

        return oh_destroy_session(SessionId);
}

SaErrorT SAHPI_API saHpiDiscover(
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        OH_CHECK_INIT_STATE(SessionId);

        /* This will wake the discovery thread up
         * and wait until it does a round throughout the
         * plugin instances. If the thread is already running,
         * it will wait for it until it completes the round.
         */
        oh_wake_discovery_thread();

        return SA_OK;
}


/*********************************************************************
 *
 *  Domain Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiDomainInfoGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_OUT SaHpiDomainInfoT *DomainInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_global_param param = { .type = OPENHPI_DAT_USER_LIMIT };

        if (!DomainInfo) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d); /* Lock domain */
        /* General */
        DomainInfo->DomainId = d->id;
        DomainInfo->DomainCapabilities = d->capabilities;
        DomainInfo->IsPeer = 0;
        /* DRT */
        DomainInfo->DrtUpdateCount = d->drt.update_count;
        DomainInfo->DrtUpdateTimestamp = d->drt.update_timestamp;
        /* RPT */
        DomainInfo->RptUpdateCount = d->rpt.update_count;
        DomainInfo->RptUpdateTimestamp = d->rpt.update_timestamp;
        /* DAT */
        DomainInfo->DatUpdateCount = d->dat.update_count;
        DomainInfo->DatUpdateTimestamp = d->dat.update_timestamp;
        DomainInfo->ActiveAlarms = oh_count_alarms(d, SAHPI_ALL_SEVERITIES);
        DomainInfo->CriticalAlarms = oh_count_alarms(d, SAHPI_CRITICAL);
        DomainInfo->MajorAlarms = oh_count_alarms(d, SAHPI_MAJOR);
        DomainInfo->MinorAlarms = oh_count_alarms(d, SAHPI_MINOR);
        if (oh_get_global_param(&param))
                param.u.dat_user_limit = OH_MAX_DAT_USER_LIMIT;
        DomainInfo->DatUserAlarmLimit = param.u.dat_user_limit;
        DomainInfo->DatOverflow = d->dat.overflow;

        memcpy(DomainInfo->Guid, d->guid, sizeof(SaHpiGuidT));
        DomainInfo->DomainTag = d->tag;
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiDrtEntryGet (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiEntryIdT       EntryId,
        SAHPI_OUT SaHpiEntryIdT       *NextEntryId,
        SAHPI_OUT SaHpiDrtEntryT      *DrtEntry)
{
        SaHpiDomainIdT did;
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        if((DrtEntry == NULL) ||
           (NextEntryId == NULL) ||
           (EntryId == SAHPI_LAST_ENTRY)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        return oh_drt_entry_get(did, EntryId, NextEntryId, DrtEntry);
}

SaErrorT SAHPI_API saHpiDomainTagSet (
        SAHPI_IN  SaHpiSessionIdT      SessionId,
        SAHPI_IN  SaHpiTextBufferT     *DomainTag)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!DomainTag || !oh_valid_textbuffer(DomainTag))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        OH_GET_DOMAIN(did, d); /* Lock domain */
        d->tag = *DomainTag;
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

/*********************************************************************
 *
 *  Resource Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRptEntryGet(
        SAHPI_IN  SaHpiSessionIdT SessionId,
        SAHPI_IN  SaHpiEntryIdT   EntryId,
        SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
        SAHPI_OUT SaHpiRptEntryT  *RptEntry)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *req_entry;
        SaHpiRptEntryT *next_entry;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* Test pointer parameters for invalid pointers */
        if ((NextEntryId == NULL) || (RptEntry == NULL)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* I believe this is the only current reserved value
           here, though others may come in the future. */
        if (EntryId == SAHPI_LAST_ENTRY) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (EntryId == SAHPI_FIRST_ENTRY) {
                req_entry = oh_get_resource_next(&(d->rpt), SAHPI_FIRST_ENTRY);
        } else {
                req_entry = oh_get_resource_by_id(&(d->rpt), EntryId);
        }

        /* if the entry was NULL, clearly have an issue */
        if (req_entry == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));

        next_entry = oh_get_resource_next(&(d->rpt), req_entry->EntryId);

        if(next_entry != NULL) {
                *NextEntryId = next_entry->EntryId;
        } else {
                *NextEntryId = SAHPI_LAST_ENTRY;
        }

        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiRptEntryT   *RptEntry)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *req_entry;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* Test pointer parameters for invalid pointers */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID ||
            RptEntry == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_GET_DOMAIN(did, d); /* Lock domain */

        req_entry = oh_get_resource_by_id(&(d->rpt), ResourceId);

        /*
         * is this case really supposed to be an error?  I thought
         * there was a valid return for "not found in domain"
         */

        if (req_entry == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        memcpy(RptEntry, req_entry, sizeof(*RptEntry));

        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceSeveritySet(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiSeverityT   Severity)
{
        SaHpiDomainIdT did;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
        SaErrorT error = SA_OK;
        SaHpiRptEntryT  *rptentry;

        OH_CHECK_INIT_STATE(SessionId);

        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_lookup_severity(Severity) ||
                   Severity == SAHPI_ALL_SEVERITIES) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_resource_severity, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, Severity);
        if (error != SA_OK) {
                oh_release_handler(h);
                return error;
        }
        oh_release_handler(h);

        /* Alarm Handling */
        if (error == SA_OK) {
                oh_detect_res_sev_alarm(did, ResourceId, Severity);
        }

        /* to get rpt entry into infrastructure */
        OH_GET_DOMAIN(did, d); /* Lock domain */
        rptentry = oh_get_resource_by_id(&(d->rpt), ResourceId);
        if (!rptentry) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        rptentry->ResourceSeverity = Severity;
        oh_release_domain(d); /* Unlock domain */

        return error;
}

SaErrorT SAHPI_API saHpiResourceTagSet(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
        SaErrorT rv;
        SaHpiDomainIdT did;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rptentry;

        OH_CHECK_INIT_STATE(SessionId);
        if (ResourceTag == NULL || !oh_valid_textbuffer(ResourceTag))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_resource_tag, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, ResourceTag);
        if (rv != SA_OK) {
                oh_release_handler(h);
                return rv;
        }
        oh_release_handler(h);

        OH_GET_DOMAIN(did, d); /* Lock domain */
        rptentry = oh_get_resource_by_id(&(d->rpt), ResourceId);
        if (!rptentry) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        rptentry->ResourceTag = *ResourceTag;
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiMyEntityPathGet(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_OUT SaHpiEntityPathT *EntityPath)
{
        // This function is now implemented on the base library level
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceIdGet(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
        // This function is now implemented on the base library level
        return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiGetIdByEntityPath (
        SAHPI_IN        SaHpiSessionIdT         SessionId,
        SAHPI_IN        SaHpiEntityPathT        EntityPath,
        SAHPI_IN        SaHpiRdrTypeT           InstrumentType,
        SAHPI_INOUT     SaHpiUint32T            *InstanceId,
        SAHPI_OUT       SaHpiResourceIdT        *ResourceId,
        SAHPI_OUT       SaHpiInstrumentIdT      *InstrumentId,
        SAHPI_OUT       SaHpiUint32T            *RptUpdateCount)
{
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;
        SaHpiRptEntryT *rptentry;
        SaHpiRdrT *rdr = NULL;
        
        if (InstanceId == NULL || ResourceId == NULL ||
            *InstanceId == SAHPI_LAST_ENTRY ||
            (InstrumentId == NULL && InstrumentType != SAHPI_NO_RECORD) ||
            RptUpdateCount == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);        
        OH_GET_DOMAIN(did, d); /* Lock domain */
        
        rptentry = oh_get_resource_by_ep(&(d->rpt), &EntityPath);
        if (!rptentry) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        *ResourceId = rptentry->ResourceId;
        *RptUpdateCount = d->rpt.update_count;        
        if (InstrumentType == SAHPI_NO_RECORD) {
                *InstanceId = SAHPI_LAST_ENTRY;
                oh_release_domain(d);                
                return SA_OK;
        }        

        /* Get Rdr indicated by InstanceId (Num) and Type */
        if (*InstanceId == SAHPI_FIRST_ENTRY) {
                rdr = oh_get_rdr_by_type_first(&d->rpt, *ResourceId, InstrumentType);
        } else {
                rdr = oh_get_rdr_by_type(&d->rpt, *ResourceId,
                                         InstrumentType, oh_get_rdr_num(*InstanceId));
        }
        
        if (rdr == NULL) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
       
	*InstrumentId = oh_get_rdr_num(rdr->RecordId);
        rdr = oh_get_rdr_by_type_next(&d->rpt, *ResourceId, InstrumentType,
                                      oh_get_rdr_num(rdr->RecordId));
        if (rdr == NULL) {
                *InstanceId = SAHPI_LAST_ENTRY;
        } else {
                *InstanceId = oh_get_rdr_uid(rdr->RdrType, oh_get_rdr_num(rdr->RecordId) );
        }                
        
        oh_release_domain(d);
        return SA_OK;
}

SaErrorT SAHPI_API saHpiGetChildEntityPath (
    SAHPI_IN    SaHpiSessionIdT         SessionId,
    SAHPI_IN    SaHpiEntityPathT        ParentEntityPath,
    SAHPI_INOUT SaHpiUint32T            *InstanceId,
    SAHPI_OUT   SaHpiEntityPathT        *ChildEntityPath,
    SAHPI_OUT   SaHpiUint32T            *RptUpdateCount)
{
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;
        oh_entitypath_pattern epp;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiBoolT found_match = SAHPI_FALSE;
        SaErrorT error;
        int i, j;
        
        if (InstanceId == NULL || *InstanceId == SAHPI_LAST_ENTRY ||
            RptUpdateCount == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        
        /* Check to see the parent entity path exists */
        /* There is special handling for {ROOT, 0} */
        if (
            (ParentEntityPath.Entry[0].EntityType != SAHPI_ENT_ROOT) ||
            (ParentEntityPath.Entry[0].EntityLocation != 0)
           )
        {
            rpte = oh_get_resource_by_ep(&d->rpt, &ParentEntityPath);
            if (rpte == NULL) {
                    oh_release_domain(d);
                    return SA_ERR_HPI_INVALID_DATA;
            }
            rpte = NULL;
        }
        
        /* Create an entity path pattern from the parent entity path
         * that looks like the following: <ParentEntityPath>{.,.}
         * This will match direct childs of ParentEntityPath.
         **/
        memset(&epp, 0, sizeof(oh_entitypath_pattern));
        epp.epattern[0].etp.is_dot = 1;
        epp.epattern[0].elp.is_dot = 1;
        for(i = 0, j = 1; i < SAHPI_MAX_ENTITY_PATH; i++) {
                epp.epattern[j].etp.type =
                        ParentEntityPath.Entry[i].EntityType;
                epp.epattern[j].elp.location =
                        ParentEntityPath.Entry[i].EntityLocation;
                j++;
        }
        
        /* Find a matching child */
        for (rpte = oh_get_resource_by_id(&d->rpt, *InstanceId);
             rpte;
             rpte = oh_get_resource_next(&d->rpt, rpte->ResourceId)) {
                if (oh_match_entitypath_pattern(&epp, &rpte->ResourceEntity)) {
                        found_match = SAHPI_TRUE;
                        break;
                }
                if (*InstanceId != SAHPI_FIRST_ENTRY) break;
        }
                
        if (found_match) { /* Found matching InstanceId */
                /* Now look next matching InstanceId */
                SaHpiRptEntryT *nrpte = NULL;
                found_match = SAHPI_FALSE;
                for (nrpte = oh_get_resource_next(&d->rpt, rpte->ResourceId);
                     nrpte;
                     nrpte = oh_get_resource_next(&d->rpt, nrpte->ResourceId)) {
                        if (oh_match_entitypath_pattern(&epp,
                                        &nrpte->ResourceEntity)) {
                                found_match = SAHPI_TRUE;
                                break;
                        }
                }
                *InstanceId = SAHPI_LAST_ENTRY;
                if (found_match) {
                        *InstanceId = nrpte->ResourceId;
                }                
                *ChildEntityPath = rpte->ResourceEntity;
                *RptUpdateCount = d->rpt.update_count;
                error = SA_OK;
        } else {
                error = SA_ERR_HPI_NOT_PRESENT;
        }
        
        oh_release_domain(d);
	return error;
}

SaErrorT SAHPI_API saHpiResourceFailedRemove (
    SAHPI_IN    SaHpiSessionIdT        SessionId,
    SAHPI_IN    SaHpiResourceIdT       ResourceId)
{
        SaErrorT error;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;
        unsigned int hid;
        SaHpiDomainIdT did;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRptEntryT saved_res;
        struct oh_event *e = NULL;
        SaHpiHsStateT hsstate;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        
        rpte = oh_get_resource_by_id(&d->rpt, ResourceId);
        if (rpte == NULL) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        if (!rpte->ResourceFailed) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }
        
        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        saved_res = *rpte;
        OH_HANDLER_GET(d, ResourceId, h);        
        oh_release_domain(d);

        if (h && h->abi->resource_failed_remove) {
                OH_CALL_ABI(h, resource_failed_remove, SA_ERR_HPI_INTERNAL_ERROR, error,
                            ResourceId);
       	        oh_release_handler(h);
	        return error;
        }
        hid = h->id;

	/* If the resource_failed_remove ABI is not defined, then remove the
	 * resource from rptcache */
        if (rpte->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
                OH_CALL_ABI(h, get_hotswap_state, SA_ERR_HPI_INTERNAL_ERROR, error,
                            ResourceId, &hsstate);
	        if (error != SA_OK) {
        	        oh_release_handler(h);
                        return error;
                }
	} else { 
		hsstate = SAHPI_HS_STATE_ACTIVE;
        }
        oh_release_handler(h);
        
        e = g_new0(struct oh_event, 1);
        e->hid = hid;
        e->resource = saved_res;
        e->event.Source = ResourceId;
        e->event.Severity = saved_res.ResourceSeverity;
        oh_gettimeofday(&e->event.Timestamp);
        e->event.EventType = SAHPI_ET_HOTSWAP;
        e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = hsstate;
        e->event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        e->event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                SAHPI_HS_CAUSE_USER_UPDATE;
        oh_evt_queue_push(oh_process_q, e);
        
        return SA_OK;
}

/*********************************************************************
 *
 *  Event Log Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiEventLogInfoGet (
        SAHPI_IN  SaHpiSessionIdT    SessionId,
        SAHPI_IN  SaHpiResourceIdT   ResourceId,
        SAHPI_OUT SaHpiEventLogInfoT *Info)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        /* Test pointer parameters for invalid pointers */
        if (Info == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                rv = oh_el_info(d->del, Info);
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_el_info, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, Info);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogCapabilitiesGet (
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_OUT SaHpiEventLogCapabilitiesT  *EventLogCapabilities)
{
        SaErrorT error;
        SaHpiRptEntryT *rpte = NULL;
        struct oh_handler *h = NULL;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;
        
        if (EventLogCapabilities == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);        
        
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                *EventLogCapabilities = SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD |
                        SAHPI_EVTLOG_CAPABILITY_CLEAR |
                        SAHPI_EVTLOG_CAPABILITY_TIME_SET |
                        SAHPI_EVTLOG_CAPABILITY_STATE_SET |
                        SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET;
                
                return SA_OK;
        }
        
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);
        if(!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {                
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_el_caps, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, EventLogCapabilities);
        oh_release_handler(h);        

        return error;
}

SaErrorT SAHPI_API saHpiEventLogEntryGet (
        SAHPI_IN    SaHpiSessionIdT       SessionId,
        SAHPI_IN    SaHpiResourceIdT      ResourceId,
        SAHPI_IN    SaHpiEventLogEntryIdT EntryId,
        SAHPI_OUT   SaHpiEventLogEntryIdT *PrevEntryId,
        SAHPI_OUT   SaHpiEventLogEntryIdT *NextEntryId,
        SAHPI_OUT   SaHpiEventLogEntryT   *EventLogEntry,
        SAHPI_INOUT SaHpiRdrT             *Rdr,
        SAHPI_INOUT SaHpiRptEntryT        *RptEntry)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        oh_el_entry *elentry;
        SaErrorT retc;
        SaHpiDomainIdT did;

        /* Test pointer parameters for invalid pointers */
        if (!PrevEntryId || !EventLogEntry || !NextEntryId ||
            EntryId == SAHPI_NO_MORE_ENTRIES)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                retc = oh_el_get(d->del, EntryId, PrevEntryId, NextEntryId,
                                 &elentry);
                if (retc == SA_OK) {
                        memcpy(EventLogEntry, &elentry->event, sizeof(SaHpiEventLogEntryT));
                        if (Rdr)
                                memcpy(Rdr, &elentry->rdr, sizeof(SaHpiRdrT));
                        if (RptEntry)
                                memcpy(RptEntry, &elentry->res, sizeof(SaHpiRptEntryT));
                }
                oh_release_domain(d); /* Unlock domain */
                return retc;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_el_entry, SA_ERR_HPI_INVALID_CMD, rv, 
                    ResourceId, EntryId, PrevEntryId, NextEntryId,
                    EventLogEntry, Rdr, RptEntry);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiEventT       *EvtEntry)
{
        SaErrorT rv;
        SaHpiEventLogInfoT info;
        SaHpiEventLogCapabilitiesT caps;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];

        OH_CHECK_INIT_STATE(SessionId);

        if (EvtEntry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (EvtEntry->EventType != SAHPI_ET_USER) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (EvtEntry->Source != SAHPI_UNSPECIFIED_RESOURCE_ID) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (EvtEntry->Severity == SAHPI_ALL_SEVERITIES) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_lookup_severity(EvtEntry->Severity)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_valid_textbuffer(&EvtEntry->EventDataUnion.UserEvent.UserEventData)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);
        if (rv != SA_OK) {
                return rv;
        }

        rv = saHpiEventLogCapabilitiesGet(SessionId, ResourceId, &caps);
#if 0
        if (rv != SA_OK) {
                return rv;
        }
        if ((caps & SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD) == 0) {
                return SA_ERR_HPI_INVALID_CMD;
        }
#endif
        // This is a workaround
        // TODO use the variant above instead
        // when all plug-ins will support oh_get_el_caps
        if ((rv == SA_OK) && ((caps & SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD) == 0)) {
                return SA_ERR_HPI_INVALID_CMD;
        }


        if (EvtEntry->EventDataUnion.UserEvent.UserEventData.DataLength > info.UserEventMaxSize) {
                return SA_ERR_HPI_INVALID_DATA;
        }

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                struct oh_global_param param = { .type = OPENHPI_DEL_SAVE };

                oh_get_global_param(&param);
                rv = oh_el_append(d->del, EvtEntry, NULL, NULL);
                if (param.u.del_save) {
                        param.type = OPENHPI_VARPATH;
                        oh_get_global_param(&param);
                        snprintf(del_filepath,
                                 SAHPI_MAX_TEXT_BUFFER_LENGTH*2,
                                 "%s/del.%u", param.u.varpath, did);
                        oh_el_map_to_file(d->del, del_filepath);
                }
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, add_el_entry, SA_ERR_HPI_INVALID_CMD, rv, ResourceId, EvtEntry);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogClear (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT rv;
        SaHpiEventLogCapabilitiesT caps;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;


        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                rv = oh_el_clear(d->del);
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rv = saHpiEventLogCapabilitiesGet(SessionId, ResourceId, &caps);
#if 0
        if (rv != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }
        if ((caps & SAHPI_EVTLOG_CAPABILITY_CLEAR) == 0) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }
#endif
        // This is a workaround
        // TODO use the variant above instead
        // when all plug-ins will support oh_get_el_caps
        if ((rv == SA_OK) && ((caps & SAHPI_EVTLOG_CAPABILITY_CLEAR) == 0)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }


        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, clear_el, SA_ERR_HPI_INVALID_CMD, rv, ResourceId);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogTimeGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiTimeT       *Time)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;

        /* Test pointer parameters for invalid pointers */
        if (Time == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv != SA_OK) {
                return rv;
        }

        *Time = info.CurrentTime;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogTimeSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiTimeT       Time)
{
        SaErrorT rv;
        SaHpiEventLogCapabilitiesT caps;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                rv = oh_el_timeset(d->del, Time);
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rv = saHpiEventLogCapabilitiesGet(SessionId, ResourceId, &caps);
#if 0
        if (rv != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }
        if ((caps & SAHPI_EVTLOG_CAPABILITY_TIME_SET) == 0) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }
#endif
        // This is a workaround
        // TODO use the variant above instead
        // when all plug-ins will support oh_get_el_caps
        if ((rv == SA_OK) && ((caps & SAHPI_EVTLOG_CAPABILITY_TIME_SET) == 0)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }

        if (Time == SAHPI_TIME_UNSPECIFIED) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_el_time, SA_ERR_HPI_INVALID_CMD, rv, ResourceId, Time);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogStateGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiBoolT       *Enable)
{
        SaHpiEventLogInfoT info;
        SaErrorT rv;

        /* Test pointer parameters for invalid pointers */
        if (Enable == NULL)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = saHpiEventLogInfoGet(SessionId, ResourceId, &info);

        if(rv != SA_OK) {
                return rv;
        }

        *Enable = info.Enabled;

        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventLogStateSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiBoolT       Enable)
{
        struct oh_domain *d;
        struct oh_handler *h;
        SaErrorT rv;
        SaHpiDomainIdT did;
        SaHpiRptEntryT *res;
        SaHpiEventLogCapabilitiesT caps;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                oh_el_enableset(d->del, Enable);
                oh_release_domain(d); /* Unlock domain */
                return SA_OK;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rv = saHpiEventLogCapabilitiesGet(SessionId, ResourceId, &caps);
#if 0
        if (rv != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }
        if ((caps & SAHPI_EVTLOG_CAPABILITY_STATE_SET) == 0) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }
#endif
        // This is a workaround
        // TODO use the variant above instead
        // when all plug-ins will support oh_get_el_caps
        if ((rv == SA_OK) && ((caps & SAHPI_EVTLOG_CAPABILITY_STATE_SET) == 0)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }


        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_el_state, SA_ERR_HPI_INVALID_CMD, rv, ResourceId, Enable);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiEventLogOverflowReset (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        struct oh_handler *h;
        struct oh_domain *d;
        SaHpiDomainIdT did;
        SaHpiRptEntryT *res;
        SaErrorT rv = SA_OK;
        SaHpiEventLogCapabilitiesT caps;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* test for special domain case */
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                rv = oh_el_overflowreset(d->del);
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }

        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rv = saHpiEventLogCapabilitiesGet(SessionId, ResourceId, &caps);
#if 0
        if (rv != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }
        if ((caps & SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET) == 0) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }
#endif
        // This is a workaround
        // TODO use the variant above instead
        // when all plug-ins will support oh_get_el_caps
        if ((rv == SA_OK ) && ((caps & SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET) == 0)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, reset_el_overflow, SA_ERR_HPI_INVALID_CMD, rv, ResourceId);
        oh_release_handler(h);

        return rv;

}

/*********************************************************************
 *
 *  Event Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSubscribe (
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        SaHpiDomainIdT did;
        SaHpiBoolT subscribed;
        SaErrorT error;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        error = oh_get_session_subscription(SessionId, &subscribed);
        if (error != SA_OK) {
                return error;
        }

        if (subscribed) {
                return SA_ERR_HPI_DUPLICATE;
        }

        error = oh_set_session_subscription(SessionId, SAHPI_TRUE);

        return error;
}

SaErrorT SAHPI_API saHpiUnsubscribe (
        SAHPI_IN SaHpiSessionIdT SessionId)
{
        SaHpiDomainIdT did;
        SaHpiBoolT subscribed;
        SaErrorT error;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        error = oh_get_session_subscription(SessionId, &subscribed);
        if (error != SA_OK) {
                return error;
        }

        if (!subscribed) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        error = oh_set_session_subscription(SessionId, SAHPI_FALSE);
        if (error != SA_OK) {
                return error;
        }

        return error;
}

SaErrorT SAHPI_API saHpiEventGet (
        SAHPI_IN    SaHpiSessionIdT      SessionId,
        SAHPI_IN    SaHpiTimeoutT        Timeout,
        SAHPI_OUT   SaHpiEventT          *Event,
        SAHPI_INOUT SaHpiRdrT            *Rdr,
        SAHPI_INOUT SaHpiRptEntryT       *RptEntry,
        SAHPI_INOUT SaHpiEvtQueueStatusT *EventQueueStatus)
{

        SaHpiDomainIdT did;
        SaHpiBoolT subscribed;
        struct oh_event e;
        SaErrorT error = SA_OK;
        SaHpiEvtQueueStatusT qstatus = 0;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        if (!Event) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if ((Timeout <= 0) && (Timeout != SAHPI_TIMEOUT_BLOCK) &&
                   (Timeout != SAHPI_TIMEOUT_IMMEDIATE)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        error = oh_get_session_subscription(SessionId, &subscribed);
        if (error != SA_OK) return error;

        if (!subscribed) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* See if there is already an event in the queue */
        error = oh_dequeue_session_event(SessionId,
                                         SAHPI_TIMEOUT_IMMEDIATE,
                                         &e, &qstatus);
        if (error == SA_ERR_HPI_TIMEOUT) { /* If queue empty then fetch more events */
                /* Sent for more events. Ready to wait on queue. */
                error = oh_dequeue_session_event(SessionId, Timeout,
                                                 &e, &qstatus);
        }
        if (error != SA_OK) {
            /* If no events after trying to fetch them, return error */
            return error;
        }

        /* If there was overflow before or after getting events, return it */
        if (EventQueueStatus)
                *EventQueueStatus = qstatus;

        /* Return event, resource and rdr */
        *Event = e.event;

        if (RptEntry) *RptEntry = e.resource;

        if (Rdr) {
                if (e.rdrs) {
                        memcpy(Rdr, e.rdrs->data, sizeof(SaHpiRdrT));
                } else {
                        Rdr->RdrType = SAHPI_NO_RECORD;
                }
        }

        oh_event_free(&e, TRUE);
        return SA_OK;
}

SaErrorT SAHPI_API saHpiEventAdd (
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiEventT     *EvtEntry)
{
        SaHpiDomainIdT did;
        struct oh_event * e;
        SaHpiEventLogInfoT info;
        SaErrorT error = SA_OK;

        error = oh_valid_addevent(EvtEntry);
        if (error != SA_OK) {
                return error;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        error = saHpiEventLogInfoGet(SessionId, SAHPI_UNSPECIFIED_RESOURCE_ID, &info);
        if (error != SA_OK) {
                return error;
        }

        if (EvtEntry->EventDataUnion.UserEvent.UserEventData.DataLength >
            info.UserEventMaxSize) {
                return SA_ERR_HPI_INVALID_DATA;
        }

        e = oh_new_event();
        e->hid = 0;
        /* Timestamp the incoming user event
         * only if it is SAHPI_TIME_UNSPECIFIED */
        if (EvtEntry->Timestamp == SAHPI_TIME_UNSPECIFIED) {
                oh_gettimeofday(&EvtEntry->Timestamp);
        }
        /* Copy SaHpiEventT into oh_event struct */
        e->event = *EvtEntry;
        /* indicate there is no rdr or resource */
        e->rdrs = NULL;
        e->rdrs_to_remove = NULL;
        e->resource.ResourceId = did;
        e->resource.ResourceCapabilities = 0;
        /* indicate this is a user-added event */
        e->resource.ResourceSeverity = SAHPI_INFORMATIONAL;

        oh_evt_queue_push(oh_process_q, e);

        return error;
}

/*********************************************************************
 *
 *  DAT Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiAlarmGetNext (
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiSeverityT  Severity,
                SAHPI_IN SaHpiBoolT      UnacknowledgedOnly,
                SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
        SaHpiDomainIdT did = 0;
        SaHpiAlarmT *a = NULL;
        struct oh_domain *d = NULL;
        SaErrorT error = SA_ERR_HPI_NOT_PRESENT;

        OH_CHECK_INIT_STATE(SessionId);

        if (!oh_lookup_severity(Severity) || !Alarm) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (Alarm->AlarmId == SAHPI_LAST_ENTRY) {
                return error;
        }

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (Alarm->AlarmId != SAHPI_FIRST_ENTRY) {
                /* Lookup timestamp for previous alarm, first*/
                a = oh_get_alarm(d, &Alarm->AlarmId, &Severity, NULL,
                                 NULL, NULL, NULL, NULL,
                                 UnacknowledgedOnly, 0);
                if (a && a->Timestamp != Alarm->Timestamp) {
                        error = SA_ERR_HPI_INVALID_DATA;
                }
        }

        a = oh_get_alarm(d, &Alarm->AlarmId, &Severity, NULL,
                         NULL, NULL, NULL, NULL,
                         UnacknowledgedOnly, 1); /* get next alarm */
        if (a) {
                if (error != SA_ERR_HPI_INVALID_DATA) {
                        error = SA_OK;
                }
                memcpy(Alarm, a, sizeof(SaHpiAlarmT));
        }

        oh_release_domain(d);
        return error;
}

SaErrorT SAHPI_API saHpiAlarmGet(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiAlarmIdT   AlarmId,
                SAHPI_OUT SaHpiAlarmT    *Alarm)
{
        SaHpiDomainIdT did = 0;
        struct oh_domain *d = NULL;
        SaHpiAlarmT *a = NULL;
        SaErrorT error = SA_ERR_HPI_NOT_PRESENT;

        OH_CHECK_INIT_STATE(SessionId);

        if (!Alarm ||
            AlarmId == SAHPI_FIRST_ENTRY ||
            AlarmId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        a = oh_get_alarm(d, &AlarmId, NULL, NULL,
                         NULL, NULL, NULL, NULL,
                         0, 0);
        if (a) {
                memcpy(Alarm, a, sizeof(SaHpiAlarmT));
                error = SA_OK;
        }

        oh_release_domain(d);
        return error;
}

SaErrorT SAHPI_API saHpiAlarmAcknowledge(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiAlarmIdT   AlarmId,
                SAHPI_IN SaHpiSeverityT  Severity)
{
        SaHpiDomainIdT did = 0;
        struct oh_domain *d = NULL;
        SaHpiAlarmT *a = NULL;
        SaErrorT error = SA_ERR_HPI_NOT_PRESENT;

        OH_CHECK_INIT_STATE(SessionId);

        if (AlarmId == SAHPI_ENTRY_UNSPECIFIED &&
            !oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (AlarmId != SAHPI_ENTRY_UNSPECIFIED) { /* Acknowledge specific alarm */
                a = oh_get_alarm(d, &AlarmId, NULL, NULL,
                                 NULL, NULL, NULL, NULL,
                                 0, 0);
                if (a) {
                        a->Acknowledged = SAHPI_TRUE;
                        error = SA_OK;
                }
        } else { /* Acknowledge group of alarms, by severity */
                SaHpiAlarmIdT aid = SAHPI_FIRST_ENTRY;
                a = oh_get_alarm(d, &aid, &Severity, NULL,
                                 NULL, NULL, NULL, NULL,
                                 0, 1);
                while (a) {
                        a->Acknowledged = SAHPI_TRUE;
                        a = oh_get_alarm(d, &a->AlarmId, &Severity, NULL,
                                         NULL, NULL, NULL, NULL,
                                         0, 1);
                }
                error = SA_OK;
        }

        oh_release_domain(d);
        return error;
}

SaErrorT SAHPI_API saHpiAlarmAdd(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
        SaHpiDomainIdT did = 0;
        struct oh_domain *d = NULL;
        SaHpiAlarmT *a = NULL;

        OH_CHECK_INIT_STATE(SessionId);

        if (!Alarm ||
                !((Alarm->Severity == SAHPI_CRITICAL) ||
                  (Alarm->Severity == SAHPI_MAJOR) ||
                  (Alarm->Severity == SAHPI_MINOR)) ||
            (Alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER)||
            !oh_valid_textbuffer(&Alarm->AlarmCond.Data))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        /* Add new alarm */
        a = oh_add_alarm(d, Alarm, 0);

        oh_release_domain(d);

        if (a == NULL)
                return SA_ERR_HPI_OUT_OF_SPACE;
        else
                return SA_OK;
}

SaErrorT SAHPI_API saHpiAlarmDelete(
                SAHPI_IN SaHpiSessionIdT SessionId,
                SAHPI_IN SaHpiAlarmIdT   AlarmId,
                SAHPI_IN SaHpiSeverityT  Severity)
{
        SaHpiDomainIdT did = 0;
        struct oh_domain *d = NULL;
        SaHpiAlarmT *a = NULL;
        SaHpiStatusCondTypeT type = SAHPI_STATUS_COND_TYPE_USER;
        SaErrorT error = SA_ERR_HPI_NOT_PRESENT;

        OH_CHECK_INIT_STATE(SessionId);

        if (AlarmId == SAHPI_ENTRY_UNSPECIFIED &&
            !oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (AlarmId != SAHPI_ENTRY_UNSPECIFIED) { /* Look for specific alarm */
                a = oh_get_alarm(d, &AlarmId, NULL, NULL,
                                 NULL, NULL, NULL, NULL,
                                 0, 0);
                if (a) {
                        if (a->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER) {
                                error = SA_ERR_HPI_READ_ONLY;
                        } else {
                                d->dat.list = g_slist_remove(d->dat.list, a);
                                g_free(a);
                                error = SA_OK;
                        }
                }
        } else { /* Delete group of alarms by severity */
                oh_remove_alarm(d, &Severity, &type, NULL, NULL,
                                NULL, NULL, NULL, 1);
                error = SA_OK;
        }

        oh_release_domain(d);
        return error;
}

/*********************************************************************
 *
 *  RDR Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiRdrGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_IN  SaHpiEntryIdT    EntryId,
        SAHPI_OUT SaHpiEntryIdT    *NextEntryId,
        SAHPI_OUT SaHpiRdrT        *Rdr)
{
        struct oh_domain *d;
        SaHpiDomainIdT did;
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        SaHpiRdrT *rdr_next;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);

        /* Test pointer parameters for invalid pointers */
        if (EntryId == SAHPI_LAST_ENTRY || !Rdr || !NextEntryId)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_GET_DOMAIN(did, d); /* Lock domain */

        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        if(EntryId == SAHPI_FIRST_ENTRY) {
                rdr_cur = oh_get_rdr_next(&(d->rpt), ResourceId, SAHPI_FIRST_ENTRY);
        } else {
                rdr_cur = oh_get_rdr_by_id(&(d->rpt), ResourceId, EntryId);
        }

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(Rdr, rdr_cur, sizeof(*Rdr));

        rdr_next = oh_get_rdr_next(&(d->rpt), ResourceId, rdr_cur->RecordId);
        if(rdr_next == NULL) {
                *NextEntryId = SAHPI_LAST_ENTRY;
        } else {
                *NextEntryId = rdr_next->RecordId;
        }

        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiRdrGetByInstrumentId (
        SAHPI_IN  SaHpiSessionIdT    SessionId,
        SAHPI_IN  SaHpiResourceIdT   ResourceId,
        SAHPI_IN  SaHpiRdrTypeT      RdrType,
        SAHPI_IN  SaHpiInstrumentIdT InstrumentId,
        SAHPI_OUT SaHpiRdrT          *Rdr)
{
        SaHpiRptEntryT *res = NULL;
        SaHpiRdrT *rdr_cur;
        SaHpiDomainIdT did;
        SaHpiCapabilitiesT cap;
        struct oh_domain *d = NULL;

        /* Test pointer parameters for invalid pointers */
        if (!oh_lookup_rdrtype(RdrType) ||
            RdrType == SAHPI_NO_RECORD || !Rdr)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        OH_RESOURCE_GET(d, ResourceId, res);
        cap = res->ResourceCapabilities;

        if(!(cap & SAHPI_CAPABILITY_RDR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        /* ensure that the resource has something of that type */
        switch(RdrType) {
        case SAHPI_CTRL_RDR:
                if(!(cap & SAHPI_CAPABILITY_CONTROL)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_SENSOR_RDR:
                if(!(cap & SAHPI_CAPABILITY_SENSOR)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_INVENTORY_RDR:
                if(!(cap & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_WATCHDOG_RDR:
                if(!(cap & SAHPI_CAPABILITY_WATCHDOG)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_ANNUNCIATOR_RDR:
                if(!(cap & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_DIMI_RDR:
                if(!(cap & SAHPI_CAPABILITY_DIMI)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        case SAHPI_FUMI_RDR:
                if(!(cap & SAHPI_CAPABILITY_FUMI)) {
                        oh_release_domain(d); /* Unlock domain */
                        return SA_ERR_HPI_CAPABILITY;
                }
                break;
        default:
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        /* now that we have a pretty good notion that all is well, try the lookup */

        rdr_cur = oh_get_rdr_by_type(&(d->rpt), ResourceId, RdrType, InstrumentId);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        memcpy(Rdr, rdr_cur, sizeof(*Rdr));
        oh_release_domain(d); /* Unlock domain */


        return SA_OK;
}

SaErrorT SAHPI_API saHpiRdrUpdateCountGet(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiUint32T     *UpdateCount)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *res;

        /* Test pointer parameters for invalid pointers */
        if (UpdateCount == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_RDR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        SaErrorT rv = oh_get_rdr_update_count(&(d->rpt), ResourceId, UpdateCount);

        oh_release_domain(d); /* Unlock domain */

        return rv;
}


/*********************************************************************
 *
 *  Sensor Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiSensorReadingGet (
        SAHPI_IN    SaHpiSessionIdT     SessionId,
        SAHPI_IN    SaHpiResourceIdT    ResourceId,
        SAHPI_IN    SaHpiSensorNumT     SensorNum,
        SAHPI_INOUT SaHpiSensorReadingT *Reading,
        SAHPI_INOUT SaHpiEventStateT    *EventState)
{
        SaErrorT rv;
        struct oh_handler *h;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_sensor_reading, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, Reading, EventState);

	/* If the Reading->IsSupported is set to False, then Reading->Type and
	 * Reading->Value fields are not valid. Hence, these two fields may not
	 * be modified by the plugin. But the marshalling code expects all
	 * the fields of return structure to have proper values.
	 * 
	 * The below code is added to overcome the marshalling limitation.
	 */
	if (rv == SA_OK && Reading && Reading->IsSupported == SAHPI_FALSE) {
		Reading->Type = 0;
		memset(&(Reading->Value), 0, sizeof(SaHpiSensorReadingUnionT));
	}

        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
        SAHPI_IN SaHpiSessionIdT        SessionId,
        SAHPI_IN SaHpiResourceIdT       ResourceId,
        SAHPI_IN SaHpiSensorNumT        SensorNum,
        SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        SaHpiSensorThdDefnT *thd;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!SensorThresholds) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        thd = &rdr_cur->RdrTypeUnion.SensorRec.ThresholdDefn;
        if (thd->IsAccessible == SAHPI_FALSE ||
            thd->ReadThold == 0) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_sensor_thresholds, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorThresholds);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
        SAHPI_IN  SaHpiSessionIdT        SessionId,
        SAHPI_IN  SaHpiResourceIdT       ResourceId,
        SAHPI_IN  SaHpiSensorNumT        SensorNum,
        SAHPI_IN  SaHpiSensorThresholdsT *SensorThresholds)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!SensorThresholds) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        rv = oh_valid_thresholds(SensorThresholds, rdr);
        if (rv != SA_OK) { /* Invalid sensor threshold */
                oh_release_domain(d);
                return rv;
        }

        /* Checking the ordering*/
        SaHpiSensorThresholdsT tmp;

        rv = saHpiSensorThresholdsGet( SessionId, ResourceId, SensorNum, &tmp );

        if (rv != SA_OK) {
            oh_release_domain(d);
            return rv;
        }

#define COPY_TH( TH, MASK ) \
{ \
    if ( SensorThresholds->TH.IsSupported == SAHPI_TRUE ) { \
       tmp.TH = SensorThresholds->TH; \
    } \
}
        COPY_TH( UpCritical, SAHPI_STM_UP_CRIT );
        COPY_TH( UpMajor, SAHPI_STM_UP_MAJOR );
        COPY_TH( UpMinor, SAHPI_STM_UP_MINOR );
        COPY_TH( LowCritical, SAHPI_STM_LOW_CRIT );
        COPY_TH( LowMajor, SAHPI_STM_LOW_MAJOR );
        COPY_TH( LowMinor, SAHPI_STM_LOW_MINOR );
        COPY_TH( PosThdHysteresis, SAHPI_STM_UP_HYSTERESIS );
        COPY_TH( NegThdHysteresis, SAHPI_STM_LOW_HYSTERESIS );
#undef COPY_TH

        rv = oh_valid_ordering(&tmp, rdr);
        if (rv != SA_OK) { /* Invalid sensor threshold */
                oh_release_domain(d);
                return rv;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_sensor_thresholds, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorThresholds);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorTypeGet (
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiResourceIdT    ResourceId,
        SAHPI_IN  SaHpiSensorNumT     SensorNum,
        SAHPI_OUT SaHpiSensorTypeT    *Type,
        SAHPI_OUT SaHpiEventCategoryT *Category)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!Type || !Category) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_SENSOR_RDR, SensorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(Type,
               &(rdr->RdrTypeUnion.SensorRec.Type),
               sizeof(SaHpiSensorTypeT));

        memcpy(Category,
               &(rdr->RdrTypeUnion.SensorRec.Category),
               sizeof(SaHpiEventCategoryT));

        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorEnableGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_IN  SaHpiSensorNumT  SensorNum,
        SAHPI_OUT SaHpiBoolT       *SensorEnabled)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!SensorEnabled) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_sensor_enable, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorEnabled);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEnableSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiSensorNumT  SensorNum,
        SAHPI_IN SaHpiBoolT       SensorEnabled)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        if (!rdr_cur->RdrTypeUnion.SensorRec.EnableCtrl) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_READ_ONLY;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_sensor_enable, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorEnabled);
        oh_release_handler(h);
        if (rv == SA_OK) {
                oh_detect_sensor_enable_alarm(did, ResourceId,
                                              SensorNum, SensorEnabled);
        }

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventEnableGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_IN  SaHpiSensorNumT  SensorNum,
        SAHPI_OUT SaHpiBoolT       *SensorEventsEnabled)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!SensorEventsEnabled) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_sensor_event_enables, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorEventsEnabled);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventEnableSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiSensorNumT  SensorNum,
        SAHPI_IN SaHpiBoolT       SensorEventsEnabled)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiSensorEventCtrlT sec;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        sec = rdr_cur->RdrTypeUnion.SensorRec.EventCtrl;
        if (sec  == SAHPI_SEC_READ_ONLY) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_READ_ONLY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_sensor_event_enables, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, SensorEventsEnabled);
        oh_release_handler(h);
        if (rv == SA_OK) {
                oh_detect_sensor_enable_alarm(did, ResourceId,
                                              SensorNum, SensorEventsEnabled);
        }

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventMasksGet (
        SAHPI_IN  SaHpiSessionIdT         SessionId,
        SAHPI_IN  SaHpiResourceIdT        ResourceId,
        SAHPI_IN  SaHpiSensorNumT         SensorNum,
        SAHPI_INOUT SaHpiEventStateT      *AssertEventMask,
        SAHPI_INOUT SaHpiEventStateT      *DeassertEventMask)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_sensor_event_masks, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, AssertEventMask, DeassertEventMask);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiSensorEventMasksSet (
        SAHPI_IN  SaHpiSessionIdT                 SessionId,
        SAHPI_IN  SaHpiResourceIdT                ResourceId,
        SAHPI_IN  SaHpiSensorNumT                 SensorNum,
        SAHPI_IN  SaHpiSensorEventMaskActionT     Action,
        SAHPI_IN  SaHpiEventStateT                AssertEventMask,
        SAHPI_IN  SaHpiEventStateT                DeassertEventMask)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr_cur;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiSensorEventCtrlT sec;

        OH_CHECK_INIT_STATE(SessionId);

        if (!oh_lookup_sensoreventmaskaction(Action))
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr_cur = oh_get_rdr_by_type(&(d->rpt),
                                     ResourceId,
                                     SAHPI_SENSOR_RDR,
                                     SensorNum);

        if (rdr_cur == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        sec = rdr_cur->RdrTypeUnion.SensorRec.EventCtrl;
        if ((sec == SAHPI_SEC_READ_ONLY_MASKS) || (sec == SAHPI_SEC_READ_ONLY)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_READ_ONLY;
        }
        
        if (Action == SAHPI_SENS_ADD_EVENTS_TO_MASKS) {
        	if (AssertEventMask != SAHPI_ALL_EVENT_STATES &&
        	    (rdr_cur->RdrTypeUnion.SensorRec.Events | AssertEventMask) !=
        	    rdr_cur->RdrTypeUnion.SensorRec.Events) {
        		oh_release_domain(d);
        		return SA_ERR_HPI_INVALID_DATA;        		
        	}
        	
        	if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) &&
        	    (DeassertEventMask != SAHPI_ALL_EVENT_STATES) &&
        	    ((rdr_cur->RdrTypeUnion.SensorRec.Events | DeassertEventMask) !=
        	    rdr_cur->RdrTypeUnion.SensorRec.Events)) {
        		oh_release_domain(d);
        		return SA_ERR_HPI_INVALID_DATA;
        	}
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_sensor_event_masks, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, SensorNum, Action, AssertEventMask, DeassertEventMask);
        oh_release_handler(h);

        if (rv == SA_OK) {
                oh_detect_sensor_mask_alarm(did, ResourceId,
                                            SensorNum,
                                            Action, DeassertEventMask);
        }

        return rv;
}

/* End Sensor functions */

/*********************************************************************
 *
 *  Control Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiControlTypeGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_IN  SaHpiCtrlNumT    CtrlNum,
        SAHPI_OUT SaHpiCtrlTypeT   *Type)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!Type) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_CTRL_RDR, CtrlNum);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(Type,
               &(rdr->RdrTypeUnion.CtrlRec.Type),
               sizeof(SaHpiCtrlTypeT));

        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiControlGet (
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiCtrlNumT    CtrlNum,
        SAHPI_OUT   SaHpiCtrlModeT   *CtrlMode,
        SAHPI_INOUT SaHpiCtrlStateT  *CtrlState)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        struct oh_handler *h = NULL;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;


        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_CTRL_RDR, CtrlNum);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if(rdr->RdrTypeUnion.CtrlRec.WriteOnly) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }

        if (CtrlMode == NULL && CtrlState == NULL) {
                oh_release_domain(d);
                return SA_OK;
        } else if (CtrlState &&
                    rdr->RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_TEXT) {
                if (CtrlState->StateUnion.Text.Line != SAHPI_TLN_ALL_LINES &&
                    CtrlState->StateUnion.Text.Line >
                    rdr->RdrTypeUnion.CtrlRec.TypeUnion.Text.MaxLines) {
                        oh_release_domain(d);
                        return SA_ERR_HPI_INVALID_DATA;
                }
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_control_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, CtrlNum, CtrlMode, CtrlState);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiControlSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiCtrlNumT    CtrlNum,
        SAHPI_IN SaHpiCtrlModeT   CtrlMode,
        SAHPI_IN SaHpiCtrlStateT  *CtrlState)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiCtrlModeT cur_mode;
        SaHpiCtrlStateT cur_state;

        if (!oh_lookup_ctrlmode(CtrlMode) ||
            (CtrlMode != SAHPI_CTRL_MODE_AUTO && !CtrlState)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (CtrlMode != SAHPI_CTRL_MODE_AUTO &&
            ((CtrlState->Type == SAHPI_CTRL_TYPE_DIGITAL &&
              !oh_lookup_ctrlstatedigital(CtrlState->StateUnion.Digital)) ||
             (CtrlState->Type == SAHPI_CTRL_TYPE_STREAM &&
              CtrlState->StateUnion.Stream.StreamLength
              > SAHPI_CTRL_MAX_STREAM_LENGTH) ||
             (CtrlState->Type == SAHPI_CTRL_TYPE_TEXT &&
              !oh_valid_textbuffer(&CtrlState->StateUnion.Text.Text)))) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&d->rpt, ResourceId, SAHPI_CTRL_RDR, CtrlNum);
        if (!rdr || rdr->RdrType != SAHPI_CTRL_RDR) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check CtrlMode and CtrlState */
        rv = oh_valid_ctrl_state_mode(&rdr->RdrTypeUnion.CtrlRec,
                                      CtrlMode, CtrlState);
        if (rv != SA_OK) {
                oh_release_domain(d);
                return rv;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */
	
	if (!rdr->RdrTypeUnion.CtrlRec.WriteOnly &&
	    rdr->RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL) {
	    
		OH_CALL_ABI(h, get_control_state, SA_ERR_HPI_INVALID_CMD, rv,
        	    	    ResourceId, CtrlNum, &cur_mode, &cur_state);

		if (CtrlMode != SAHPI_CTRL_MODE_AUTO) {		
        		if (((cur_state.StateUnion.Digital == SAHPI_CTRL_STATE_PULSE_ON || cur_state.StateUnion.Digital == SAHPI_CTRL_STATE_ON) &&
        	     CtrlState->StateUnion.Digital == SAHPI_CTRL_STATE_PULSE_ON) ||
        	    ((cur_state.StateUnion.Digital == SAHPI_CTRL_STATE_PULSE_OFF || cur_state.StateUnion.Digital == SAHPI_CTRL_STATE_OFF) &&
        	     CtrlState->StateUnion.Digital == SAHPI_CTRL_STATE_PULSE_OFF)) {
        			oh_release_handler(h);
				return SA_ERR_HPI_INVALID_REQUEST;
        		}
		}
	}
	
        OH_CALL_ABI(h, set_control_state, SA_ERR_HPI_INVALID_CMD, rv,
        	    ResourceId, CtrlNum, CtrlMode, CtrlState);
        oh_release_handler(h);

        return rv;
}


/*********************************************************************
 *
 *  Inventory Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiIdrInfoGet(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_IN  SaHpiIdrIdT      IdrId,
        SAHPI_OUT SaHpiIdrInfoT    *IdrInfo)
{

        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;

        if (IdrInfo == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_idr_info, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, IdrInfo);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrAreaHeaderGet(
        SAHPI_IN  SaHpiSessionIdT     SessionId,
        SAHPI_IN  SaHpiResourceIdT    ResourceId,
        SAHPI_IN  SaHpiIdrIdT         IdrId,
        SAHPI_IN  SaHpiIdrAreaTypeT   AreaType,
        SAHPI_IN  SaHpiEntryIdT       AreaId,
        SAHPI_OUT SaHpiEntryIdT       *NextAreaId,
        SAHPI_OUT SaHpiIdrAreaHeaderT *Header)
{

        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if ( ((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
             ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
             (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
             (AreaType != SAHPI_IDR_AREATYPE_OEM)) ||
             (AreaId == SAHPI_LAST_ENTRY)||
             (NextAreaId == NULL) ||
             (Header == NULL)))   {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_idr_area_header, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, AreaType, AreaId, NextAreaId, Header);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrAreaAdd(
        SAHPI_IN  SaHpiSessionIdT   SessionId,
        SAHPI_IN  SaHpiResourceIdT  ResourceId,
        SAHPI_IN  SaHpiIdrIdT       IdrId,
        SAHPI_IN  SaHpiIdrAreaTypeT AreaType,
        SAHPI_OUT SaHpiEntryIdT     *AreaId)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if (!oh_lookup_idrareatype(AreaType) ||
            AreaId == NULL)   {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) {
                return SA_ERR_HPI_INVALID_DATA;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, add_idr_area, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, AreaType, AreaId);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrAreaAddById(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiIdrIdT        IdrId,
    SAHPI_IN SaHpiIdrAreaTypeT  AreaType,
    SAHPI_IN SaHpiEntryIdT      AreaId)
{
        SaHpiRptEntryT *rpte;
        SaHpiRdrT *rdr;
        SaErrorT error = SA_OK;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;
        SaHpiIdrInfoT info;
        SaHpiIdrAreaHeaderT header;
        SaHpiEntryIdT next;

        if (!oh_lookup_idrareatype(AreaType) ||
            AreaId == SAHPI_LAST_ENTRY)   {                
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) {                
                return SA_ERR_HPI_INVALID_DATA;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        /* Interface and conformance checking */
        if(!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {                
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId,
                                 SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */
        
        /* Check if IDR is read-only */
        OH_CALL_ABI(h, get_idr_info, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, IdrId, &info);
        if (error != SA_OK) {
                oh_release_handler(h);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (info.ReadOnly) {
                oh_release_handler(h);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Check if the AreaId requested already exists */
        if (AreaId != SAHPI_FIRST_ENTRY) {
                OH_CALL_ABI(h, get_idr_area_header, SA_ERR_HPI_INTERNAL_ERROR, error,
                            ResourceId, IdrId, AreaType, AreaId, &next, &header);
                if (error == SA_OK) {
                        oh_release_handler(h);
                        return SA_ERR_HPI_DUPLICATE;
                }
        }

        OH_CALL_ABI(h, add_idr_area_id, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, IdrId, AreaType, AreaId);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiIdrAreaDelete(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiIdrIdT      IdrId,
        SAHPI_IN SaHpiEntryIdT    AreaId)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if (AreaId == SAHPI_LAST_ENTRY)   {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, del_idr_area, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, AreaId);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrFieldGet(
        SAHPI_IN  SaHpiSessionIdT    SessionId,
        SAHPI_IN  SaHpiResourceIdT   ResourceId,
        SAHPI_IN  SaHpiIdrIdT        IdrId,
        SAHPI_IN  SaHpiEntryIdT      AreaId,
        SAHPI_IN  SaHpiIdrFieldTypeT FieldType,
        SAHPI_IN  SaHpiEntryIdT      FieldId,
        SAHPI_OUT SaHpiEntryIdT      *NextFieldId,
        SAHPI_OUT SaHpiIdrFieldT     *Field)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if (!Field ||
            !oh_lookup_idrfieldtype(FieldType) ||
            AreaId == SAHPI_LAST_ENTRY ||
            FieldId == SAHPI_LAST_ENTRY ||
            !NextFieldId)    {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_idr_field, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, AreaId, FieldType, FieldId, NextFieldId, Field);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrFieldAdd(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiIdrIdT      IdrId,
        SAHPI_INOUT SaHpiIdrFieldT   *Field)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if (!Field)   {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_lookup_idrfieldtype(Field->Type)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (Field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (oh_valid_textbuffer(&Field->Field) != SAHPI_TRUE) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, add_idr_field, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, Field);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrFieldAddById( 
        SAHPI_IN SaHpiSessionIdT          SessionId,
        SAHPI_IN SaHpiResourceIdT         ResourceId,
        SAHPI_IN SaHpiIdrIdT              IdrId,
        SAHPI_INOUT SaHpiIdrFieldT        *Field)
{
        SaHpiRptEntryT *rpte;
        SaHpiRdrT *rdr;
        SaErrorT error = SA_OK;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;
        SaHpiEntryIdT nextid;
        SaHpiIdrAreaHeaderT header;
        SaHpiIdrFieldT field;

        if (!Field)   {                
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_lookup_idrfieldtype(Field->Type)) {                
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (Field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {                
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (oh_valid_textbuffer(&Field->Field) != SAHPI_TRUE) {                
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (Field->AreaId == SAHPI_LAST_ENTRY ||
                   Field->FieldId == SAHPI_LAST_ENTRY) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        /* Interface and conformance checking */
        if(!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {                
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */                
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */
        
        /* Check if AreaId specified in Field exists */
        OH_CALL_ABI(h, get_idr_area_header, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, IdrId, SAHPI_IDR_AREATYPE_UNSPECIFIED,
                    Field->AreaId, &nextid, &header);
        if (error != SA_OK) {
                oh_release_handler(h);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (header.ReadOnly) {
                oh_release_handler(h);
                return SA_ERR_HPI_READ_ONLY;
        }
        /* Check if FieldId requested does not already exists */
        if ( Field->FieldId != SAHPI_FIRST_ENTRY ) {
           OH_CALL_ABI(h, get_idr_field, SA_ERR_HPI_INTERNAL_ERROR, error,
                       ResourceId, IdrId, Field->AreaId,
                       SAHPI_IDR_FIELDTYPE_UNSPECIFIED,
                       Field->FieldId, &nextid, &field);
           if (error == SA_OK) {
                oh_release_handler(h);
                return SA_ERR_HPI_DUPLICATE;
           }
        }        
        /* All checks done. Pass call down to plugin */
        OH_CALL_ABI(h, add_idr_field_id, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, IdrId, Field);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiIdrFieldSet(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiIdrIdT      IdrId,
        SAHPI_IN SaHpiIdrFieldT   *Field)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h;

        if (!Field)   {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!oh_valid_textbuffer(&Field->Field)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_idr_field, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, Field);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiIdrFieldDelete(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiIdrIdT      IdrId,
        SAHPI_IN SaHpiEntryIdT    AreaId,
        SAHPI_IN SaHpiEntryIdT    FieldId)
{
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaErrorT rv = SA_OK;    /* Default to SA_OK */
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if (FieldId == SAHPI_LAST_ENTRY || AreaId == SAHPI_LAST_ENTRY)   {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        /* Interface and conformance checking */
        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt), ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, del_idr_field, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, IdrId, AreaId, FieldId);
        oh_release_handler(h);

        return rv;
}

/* End of Inventory Functions  */

/*********************************************************************
 *
 *  Watchdog Functions
 *
 ********************************************************************/

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
        SAHPI_IN  SaHpiSessionIdT   SessionId,
        SAHPI_IN  SaHpiResourceIdT  ResourceId,
        SAHPI_IN  SaHpiWatchdogNumT WatchdogNum,
        SAHPI_OUT SaHpiWatchdogT    *Watchdog)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        if (!Watchdog) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_watchdog_info, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, WatchdogNum, Watchdog);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
        SAHPI_IN SaHpiSessionIdT   SessionId,
        SAHPI_IN SaHpiResourceIdT  ResourceId,
        SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
        SAHPI_IN SaHpiWatchdogT    *Watchdog)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        if (!Watchdog ||
            !oh_lookup_watchdogtimeruse(Watchdog->TimerUse) ||
            !oh_lookup_watchdogaction(Watchdog->TimerAction) ||
            !oh_lookup_watchdogpretimerinterrupt(Watchdog->PretimerInterrupt)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (Watchdog->PreTimeoutInterval > Watchdog->InitialCount) {
                return SA_ERR_HPI_INVALID_DATA;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_watchdog_info, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, WatchdogNum, Watchdog);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
        SAHPI_IN SaHpiSessionIdT   SessionId,
        SAHPI_IN SaHpiResourceIdT  ResourceId,
        SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiDomainIdT did;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, reset_watchdog, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, WatchdogNum);
        oh_release_handler(h);

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

        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if (Announcement == NULL) return SA_ERR_HPI_INVALID_PARAMS;

        if (!oh_lookup_severity(Severity)) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_next_announce, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, Severity, UnacknowledgedOnly,
                    Announcement);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorGet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_OUT SaHpiAnnouncementT        *Announcement)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if (Announcement == NULL ||
            EntryId == SAHPI_FIRST_ENTRY ||
            EntryId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_announce, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, EntryId, Announcement);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorAcknowledge(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if ((EntryId == SAHPI_ENTRY_UNSPECIFIED) && !oh_lookup_severity(Severity)) {
                 return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, ack_announce, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, EntryId, Severity);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorAdd(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_INOUT SaHpiAnnouncementT      *Announcement)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        SaHpiAnnunciatorModeT mode;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if (Announcement == NULL) return SA_ERR_HPI_INVALID_PARAMS;

        if (Announcement->Severity == SAHPI_ALL_SEVERITIES ||
            !oh_lookup_severity(Announcement->Severity) ||
            !oh_valid_textbuffer(&Announcement->StatusCond.Data) ||
            !oh_lookup_statuscondtype(Announcement->StatusCond.Type)||
            Announcement->StatusCond.Name.Length > SA_HPI_MAX_NAME_LENGTH)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        rv = saHpiAnnunciatorModeGet(SessionId,
                                     ResourceId,
                                     AnnunciatorNum,
                                     &mode);
        if(rv != SA_OK) {
                oh_release_domain(d);
                return rv;
        }
        if(mode == SAHPI_ANNUNCIATOR_MODE_AUTO) {
                oh_release_domain(d);
                return SA_ERR_HPI_READ_ONLY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, add_announce, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, Announcement);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorDelete(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiEntryIdT              EntryId,
        SAHPI_IN SaHpiSeverityT             Severity)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        SaHpiAnnunciatorModeT mode;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if ((EntryId == SAHPI_ENTRY_UNSPECIFIED) && !oh_lookup_severity(Severity)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        rv = saHpiAnnunciatorModeGet(SessionId,
                                     ResourceId,
                                     AnnunciatorNum,
                                     &mode);
        if(rv != SA_OK) {
                oh_release_domain(d);
                return rv;
        }
        if(mode == SAHPI_ANNUNCIATOR_MODE_AUTO) {
                oh_release_domain(d);
                return SA_ERR_HPI_READ_ONLY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, del_announce, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, EntryId, Severity);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeGet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_OUT SaHpiAnnunciatorModeT     *Mode)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        if (Mode == NULL) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_annunc_mode, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, Mode);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiAnnunciatorModeSet(
        SAHPI_IN SaHpiSessionIdT            SessionId,
        SAHPI_IN SaHpiResourceIdT           ResourceId,
        SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
        SAHPI_IN SaHpiAnnunciatorModeT      Mode)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        SaHpiRdrT *rdr;
        SaHpiDomainIdT did;
        struct oh_handler *h;
        struct oh_domain *d = NULL;

        /* if no valid mode, then this won't find a lookup */
        if (!oh_lookup_annunciatormode(Mode)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if(!(res->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_ANNUNCIATOR_RDR,
                                 AnnunciatorNum);

        if (!rdr) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (rdr->RdrTypeUnion.AnnunciatorRec.ModeReadOnly) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_READ_ONLY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, set_annunc_mode, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, AnnunciatorNum, Mode);
        oh_release_handler(h);

        return rv;
}

/*******************************************************************************
 *
 * DIMI Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiDimiInfoGet (
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiDimiNumT       DimiNum,
    SAHPI_OUT   SaHpiDimiInfoT      *DimiInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
	SaErrorT error = SA_OK;
	struct oh_handler *h = NULL;
        
        if (DimiInfo == NULL) {
        	return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);
        
        if(!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);
        
        OH_CALL_ABI(h, get_dimi_info, SA_ERR_HPI_INVALID_CMD, error,
        	    ResourceId, DimiNum, DimiInfo);
	oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestInfoGet (
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiTestT       *DimiTest)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (!DimiTest) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);
        
        if(!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);
        
        OH_CALL_ABI(h, get_dimi_test, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum, DimiTest);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestReadinessGet (
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiReadyT      *DimiReady)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (DimiReady == NULL) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_dimi_test_ready, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum, DimiReady);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestStart (
    SAHPI_IN    SaHpiSessionIdT                SessionId,
    SAHPI_IN    SaHpiResourceIdT               ResourceId,
    SAHPI_IN    SaHpiDimiNumT                  DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT              TestNum,
    SAHPI_IN    SaHpiUint8T                    NumberOfParams,
    SAHPI_IN    SaHpiDimiTestVariableParamsT   *ParamsList)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (ParamsList == NULL && NumberOfParams != 0)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_dimi_test, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum, NumberOfParams, ParamsList);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestCancel (
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, cancel_dimi_test, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestStatusGet (
    SAHPI_IN    SaHpiSessionIdT                   SessionId,
    SAHPI_IN    SaHpiResourceIdT                  ResourceId,
    SAHPI_IN    SaHpiDimiNumT                     DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT                 TestNum,
    SAHPI_OUT   SaHpiDimiTestPercentCompletedT    *PercentCompleted,
    SAHPI_OUT   SaHpiDimiTestRunStatusT           *RunStatus)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (RunStatus == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_dimi_test_status, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum, PercentCompleted, RunStatus);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiDimiTestResultsGet (
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiDimiNumT            DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT        TestNum,
    SAHPI_OUT   SaHpiDimiTestResultsT    *TestResults)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (TestResults == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_DIMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_DIMI_RDR,
                                 DimiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_dimi_test_results, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, DimiNum, TestNum, TestResults);
        oh_release_handler(h);
        
        return error;
}

/*******************************************************************************
 *
 * FUMI Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiFumiSpecInfoGet(
        SAHPI_IN    SaHpiSessionIdT    SessionId,
        SAHPI_IN    SaHpiResourceIdT   ResourceId,
        SAHPI_IN    SaHpiFumiNumT      FumiNum,
        SAHPI_OUT   SaHpiFumiSpecInfoT *SpecInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;

        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (SpecInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_spec, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, SpecInfo);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiFumiServiceImpactGet(
        SAHPI_IN   SaHpiSessionIdT              SessionId,
        SAHPI_IN   SaHpiResourceIdT             ResourceId,
        SAHPI_IN   SaHpiFumiNumT                FumiNum,
        SAHPI_OUT  SaHpiFumiServiceImpactDataT  *ServiceImpact)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;

        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (ServiceImpact == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_service_impact, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, ServiceImpact);

        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiFumiSourceSet (
    SAHPI_IN    SaHpiSessionIdT         SessionId,
    SAHPI_IN    SaHpiResourceIdT        ResourceId,
    SAHPI_IN    SaHpiFumiNumT           FumiNum,
    SAHPI_IN    SaHpiBankNumT           BankNum,
    SAHPI_IN    SaHpiTextBufferT        *SourceUri)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (SourceUri == NULL || SourceUri->DataType != SAHPI_TL_TYPE_TEXT)
                return SA_ERR_HPI_INVALID_PARAMS;

        /* TODO: Add URI format validation */

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, set_fumi_source, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, SourceUri);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiSourceInfoValidateStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, validate_fumi_source, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiSourceInfoGet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiSourceInfoT  *SourceInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (SourceInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_source, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, SourceInfo);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiSourceComponentInfoGet(
        SAHPI_IN    SaHpiSessionIdT         SessionId,
        SAHPI_IN    SaHpiResourceIdT        ResourceId,
        SAHPI_IN    SaHpiFumiNumT           FumiNum,
        SAHPI_IN    SaHpiBankNumT           BankNum,
        SAHPI_IN    SaHpiEntryIdT           ComponentEntryId,
        SAHPI_OUT   SaHpiEntryIdT           *NextComponentEntryId,
        SAHPI_OUT   SaHpiFumiComponentInfoT *ComponentInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (NextComponentEntryId == NULL || ComponentInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
        if ( ComponentEntryId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_COMPONENTS)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_source_component, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, ComponentEntryId,
                    NextComponentEntryId, ComponentInfo);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiFumiTargetInfoGet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiBankInfoT    *BankInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (BankInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_target, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, BankInfo);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiTargetComponentInfoGet(
        SAHPI_IN    SaHpiSessionIdT         SessionId,
        SAHPI_IN    SaHpiResourceIdT        ResourceId,
        SAHPI_IN    SaHpiFumiNumT           FumiNum,
        SAHPI_IN    SaHpiBankNumT           BankNum,
        SAHPI_IN    SaHpiEntryIdT           ComponentEntryId,
        SAHPI_OUT   SaHpiEntryIdT           *NextComponentEntryId,
        SAHPI_OUT   SaHpiFumiComponentInfoT *ComponentInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (NextComponentEntryId == NULL || ComponentInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
        if ( ComponentEntryId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_COMPONENTS)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_target_component, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, ComponentEntryId,
                    NextComponentEntryId, ComponentInfo);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiLogicalTargetInfoGet(
        SAHPI_IN    SaHpiSessionIdT             SessionId,
        SAHPI_IN    SaHpiResourceIdT            ResourceId,
        SAHPI_IN    SaHpiFumiNumT               FumiNum,
        SAHPI_OUT   SaHpiFumiLogicalBankInfoT   *BankInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (BankInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_logical_target, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankInfo);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiLogicalTargetComponentInfoGet(
        SAHPI_IN    SaHpiSessionIdT                SessionId,
        SAHPI_IN    SaHpiResourceIdT               ResourceId,
        SAHPI_IN    SaHpiFumiNumT                  FumiNum,
        SAHPI_IN    SaHpiEntryIdT                  ComponentEntryId,
        SAHPI_OUT   SaHpiEntryIdT                  *NextComponentEntryId,
        SAHPI_OUT   SaHpiFumiLogicalComponentInfoT *ComponentInfo)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (NextComponentEntryId == NULL || ComponentInfo == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
        if ( ComponentEntryId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_COMPONENTS)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_logical_target_component, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, ComponentEntryId,
                    NextComponentEntryId, ComponentInfo);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiBackupStart(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_BACKUP)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_backup, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiBankBootOrderSet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_IN    SaHpiUint32T          Position)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_BANKREORDER)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, set_fumi_bank_order, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, Position);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiBankCopyStart(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         SourceBankNum,
    SAHPI_IN    SaHpiBankNumT         TargetBankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (SourceBankNum == TargetBankNum) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_BANKCOPY)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_bank_copy, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, SourceBankNum, TargetBankNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiInstallStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;
        SaHpiFumiSourceInfoT sourceinfo;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        error = saHpiFumiSourceInfoGet(SessionId, ResourceId, FumiNum,
                                       BankNum, &sourceinfo);
        if (error != SA_OK) {
                oh_release_domain(d);
                return error;
        } else if (sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALID &&
                   sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALIDITY_UNKNOWN) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_install, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiUpgradeStatusGet (
    SAHPI_IN    SaHpiSessionIdT         SessionId,
    SAHPI_IN    SaHpiResourceIdT        ResourceId,
    SAHPI_IN    SaHpiFumiNumT           FumiNum,
    SAHPI_IN    SaHpiBankNumT           BankNum,
    SAHPI_OUT   SaHpiFumiUpgradeStatusT *UpgradeStatus)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (UpgradeStatus == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_status, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum, UpgradeStatus);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiTargetVerifyStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;
        SaHpiFumiSourceInfoT sourceinfo;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_TARGET_VERIFY)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        error = saHpiFumiSourceInfoGet(SessionId, ResourceId, FumiNum,
                                       BankNum, &sourceinfo);
        if (error != SA_OK) {
                oh_release_domain(d);
                return error;
        } else if (sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALID &&
                   sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALIDITY_UNKNOWN) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_verify, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiTargetVerifyMainStart(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiFumiNumT    FumiNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;
        SaHpiFumiSourceInfoT sourceinfo;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_TARGET_VERIFY_MAIN)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        error = saHpiFumiSourceInfoGet(SessionId, ResourceId, FumiNum,
                                       0, &sourceinfo);
        if (error != SA_OK) {
                oh_release_domain(d);
                return error;
        } else if (sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALID &&
                   sourceinfo.SourceStatus != SAHPI_FUMI_SRC_VALIDITY_UNKNOWN) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_verify_main, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiUpgradeCancel (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, cancel_fumi_upgrade, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiAutoRollbackDisableGet(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiFumiNumT    FumiNum,
        SAHPI_OUT   SaHpiBoolT       *Disable)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        if (Disable == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, get_fumi_autorollback_disable, SA_ERR_HPI_INVALID_CMD,
                    error, ResourceId, FumiNum, Disable);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiAutoRollbackDisableSet(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiFumiNumT    FumiNum,
        SAHPI_IN    SaHpiBoolT       Disable)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_AUTOROLLBACK_CAN_BE_DISABLED)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, set_fumi_autorollback_disable, SA_ERR_HPI_INVALID_CMD,
                    error, ResourceId, FumiNum, Disable);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiRollbackStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        } else if (!(rdr->RdrTypeUnion.FumiRec.Capability & SAHPI_FUMI_CAP_ROLLBACK)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_rollback, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiActivate (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, activate_fumi, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiActivateStart(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiFumiNumT    FumiNum,
        SAHPI_IN    SaHpiBoolT       Logical)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, start_fumi_activate, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, Logical);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiFumiCleanup(
        SAHPI_IN    SaHpiSessionIdT  SessionId,
        SAHPI_IN    SaHpiResourceIdT ResourceId,
        SAHPI_IN    SaHpiFumiNumT    FumiNum,
        SAHPI_IN    SaHpiBankNumT    BankNum)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaHpiRptEntryT *rpte = NULL;
        SaHpiRdrT *rdr = NULL;
        
        SaErrorT error = SA_OK;
        struct oh_handler *h = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_FUMI)) {
                oh_release_domain(d);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(&(d->rpt),
                                 ResourceId,
                                 SAHPI_FUMI_RDR,
                                 FumiNum);

        if (!rdr) {
                oh_release_domain(d);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        
        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d);

        OH_CALL_ABI(h, cleanup_fumi, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, FumiNum, BankNum);
        oh_release_handler(h);
        
        return error;
}

/*******************************************************************************
 *
 *  Hotswap Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiHotSwapPolicyCancel (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaHpiRptEntryT *res;
        SaHpiDomainIdT did;
        SaHpiHsStateT currentstate;
        SaErrorT error;
        struct oh_handler *h;
        struct oh_domain *d = NULL;
        SaHpiTimeoutT timeout;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        /* per spec, we only allow a cancel from certain states */
        error = saHpiHotSwapStateGet(SessionId, ResourceId, &currentstate);
        if (error != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return error;
        }

        if ((currentstate != SAHPI_HS_STATE_INSERTION_PENDING) &&
           (currentstate != SAHPI_HS_STATE_EXTRACTION_PENDING)) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        timeout = d->ai_timeout;
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, hotswap_policy_cancel, SA_OK, error,
                    ResourceId, timeout);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiResourceActiveSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT error;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        SaHpiHsStateT from;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        error = saHpiHotSwapStateGet(SessionId, ResourceId, &from);
        if (error != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return error;
        }

        if (!oh_allowed_hotswap_transition(from, SAHPI_HS_STATE_ACTIVE)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_hotswap_state, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, SAHPI_HS_STATE_ACTIVE);
        oh_release_handler(h);


        return error;
}

SaErrorT SAHPI_API saHpiResourceInactiveSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId)
{
        SaErrorT error;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        SaHpiHsStateT from;
        struct oh_domain *d = NULL;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        error = saHpiHotSwapStateGet(SessionId, ResourceId, &from);
        if (error != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return error;
        }

        if (!oh_allowed_hotswap_transition(from, SAHPI_HS_STATE_INACTIVE)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_hotswap_state, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, SAHPI_HS_STATE_INACTIVE);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
        SAHPI_IN  SaHpiSessionIdT SessionId,
        SAHPI_OUT SaHpiTimeoutT   *Timeout)
{

        SaHpiDomainIdT did;
        struct oh_domain *domain;

        if (!Timeout) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, domain); /* Lock domain */

        *Timeout = get_hotswap_auto_insert_timeout(domain);

        oh_release_domain(domain); /* Unlock domain */

        return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
        SAHPI_IN SaHpiSessionIdT SessionId,
        SAHPI_IN SaHpiTimeoutT   Timeout)
{
        SaHpiDomainIdT did;
        struct oh_domain *domain = NULL;
        SaErrorT error = SA_OK;
        SaHpiRptEntryT *rpte = NULL;
        RPTable *rpt;
        GArray *hids = NULL;
        int i;

        if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
            Timeout != SAHPI_TIMEOUT_BLOCK &&
            Timeout < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, domain); /* Lock domain */

        if (domain->capabilities & SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY) {
                oh_release_domain(domain); /* Unlock domain */
                return SA_ERR_HPI_READ_ONLY;
        }

        set_hotswap_auto_insert_timeout(domain, Timeout);
        rpt = &domain->rpt;
        /* 1. Get a list of unique handler ids where the resources in this
         *    domain came from.
         */
        hids = g_array_new(FALSE, TRUE, sizeof(guint));
        for (rpte = oh_get_resource_by_id(rpt, SAHPI_FIRST_ENTRY);
             rpte;
             rpte = oh_get_resource_next(rpt, rpte->ResourceId)) {
                guint *hidp =
                        (guint *)oh_get_resource_data(rpt, rpte->ResourceId);
                if (hidp) {
                        int found_hid = 0;
                        /* Store id if we don't have it in the list already */
                        for (i = 0; i < hids->len; i++) {
#if defined(__sparc) || defined(__sparc__)
                                if (((guint *)((void *)(hids->data)))[i] == *hidp) {
#else
                                if (g_array_index(hids, guint, i) == *hidp) {
#endif
                                        found_hid = 1;
                                        break;
                                }
                        }
                        if (!found_hid)
                                g_array_append_val(hids, *hidp);
                }
        }
        oh_release_domain(domain); /* Unlock domain */

        if (!hids->len) CRIT("Did not find any handlers for domain resources?!");
        /* 2. Use list to push down autoInsertTimeoutSet() to those handlers.
         */
        for (i = 0; i < hids->len; i++) {
#if defined(__sparc) || defined(__sparc__)
                guint hid = ((guint *)((void *)(hids->data)))[i];
#else
                guint hid = g_array_index(hids, guint, i);
#endif
                struct oh_handler *h = oh_get_handler(hid);
                if (!h || !h->hnd) {
                        CRIT("No such handler %u", hid);
                        error = SA_ERR_HPI_INTERNAL_ERROR;
                        break;
                }

                if (h->abi->set_autoinsert_timeout) {
                        OH_CALL_ABI(h, set_autoinsert_timeout, SA_ERR_HPI_INTERNAL_ERROR,
                                    error, Timeout);
                }
                oh_release_handler(h);

                if (error != SA_OK) break;
        }
        g_array_free(hids, TRUE);

        return error;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiTimeoutT    *Timeout)
{
        SaHpiRptEntryT *res;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;
        SaErrorT error = SA_OK;

        if (!Timeout) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock Domain */

        OH_CALL_ABI(h, get_autoextract_timeout, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, Timeout);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiTimeoutT    Timeout)
{
        SaHpiRptEntryT *res;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;
        SaErrorT error = SA_OK;

        if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
            Timeout != SAHPI_TIMEOUT_BLOCK &&
            Timeout < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock Domain */

        OH_CALL_ABI(h, set_autoextract_timeout, SA_ERR_HPI_INVALID_CMD, error,
                    ResourceId, Timeout);
        oh_release_handler(h);

        return error;
}

SaErrorT SAHPI_API saHpiHotSwapStateGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiHsStateT    *State)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!State) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_hotswap_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, State);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiHsActionT   Action)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        SaHpiHsStateT currentstate;
        struct oh_domain *d = NULL;

        if (!oh_lookup_hsaction(Action)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        rv = saHpiHotSwapStateGet(SessionId, ResourceId, &currentstate);
        if(rv != SA_OK) {
                oh_release_domain(d); /* Unlock domain */
                return rv;
        }
        /* Action already validated, let's check HS state */
        if(   ((Action == SAHPI_HS_ACTION_INSERTION) &&
               (currentstate != SAHPI_HS_STATE_INACTIVE))
           || ((Action == SAHPI_HS_ACTION_EXTRACTION) &&
               (currentstate != SAHPI_HS_STATE_ACTIVE))) {
                oh_release_domain(d);
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, request_hotswap_action, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, Action);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
        SAHPI_IN  SaHpiSessionIdT        SessionId,
        SAHPI_IN  SaHpiResourceIdT       ResourceId,
        SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!State) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        if (!(res->HotSwapCapabilities & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_indicator_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, State);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
        SAHPI_IN SaHpiSessionIdT        SessionId,
        SAHPI_IN SaHpiResourceIdT       ResourceId,
        SAHPI_IN SaHpiHsIndicatorStateT State)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!oh_lookup_hsindicatorstate(State)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }
        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        if (!(res->HotSwapCapabilities & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_indicator_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, State);
        oh_release_handler(h);

        return rv;
}

/*******************************************************************************
 *
 *  Configuration Function(s)
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiParmControl (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiParmActionT Action)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!oh_lookup_parmaction(Action)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, control_parm, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, Action);
        oh_release_handler(h);

        return rv;
}

/*******************************************************************************
 *
 *  Load Management
 * 
 ******************************************************************************/

SaErrorT SAHPI_API saHpiResourceLoadIdGet (
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_OUT SaHpiLoadIdT         *LoadId)
{
        SaErrorT error;
        SaHpiRptEntryT *rpte;
        struct oh_handler *h = NULL;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (LoadId == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_LOAD_ID)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */
        
        OH_CALL_ABI(h, load_id_get, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, LoadId);
        oh_release_handler(h);
        
        return error;
}

SaErrorT SAHPI_API saHpiResourceLoadIdSet (
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_IN  SaHpiLoadIdT         *LoadId) 
{
        SaErrorT error;
        SaHpiRptEntryT *rpte;
        struct oh_handler *h = NULL;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (LoadId == NULL) {
                /* Spec doesn't say what to return in this case */
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, rpte);

        if (!(rpte->ResourceCapabilities & SAHPI_CAPABILITY_LOAD_ID)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */
        
        OH_CALL_ABI(h, load_id_set, SA_ERR_HPI_INTERNAL_ERROR, error,
                    ResourceId, LoadId);
        oh_release_handler(h);
        
        return error;
}

/*******************************************************************************
 *
 *  Reset Functions
 *
 ******************************************************************************/


SaErrorT SAHPI_API saHpiResourceResetStateGet (
        SAHPI_IN  SaHpiSessionIdT   SessionId,
        SAHPI_IN  SaHpiResourceIdT  ResourceId,
        SAHPI_OUT SaHpiResetActionT *ResetAction)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!ResetAction) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_reset_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, ResetAction);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiResourceResetStateSet (
        SAHPI_IN SaHpiSessionIdT   SessionId,
        SAHPI_IN SaHpiResourceIdT  ResourceId,
        SAHPI_IN SaHpiResetActionT ResetAction)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!oh_lookup_resetaction(ResetAction)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_reset_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, ResetAction);
        oh_release_handler(h);

        return rv;
}

/*******************************************************************************
 *
 *  Power Functions
 *
 ******************************************************************************/

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
        SAHPI_IN  SaHpiSessionIdT  SessionId,
        SAHPI_IN  SaHpiResourceIdT ResourceId,
        SAHPI_OUT SaHpiPowerStateT *State)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (State == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, get_power_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, State);
        oh_release_handler(h);

        return rv;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
        SAHPI_IN SaHpiSessionIdT  SessionId,
        SAHPI_IN SaHpiResourceIdT ResourceId,
        SAHPI_IN SaHpiPowerStateT State)
{
        SaErrorT rv;
        SaHpiRptEntryT *res;
        struct oh_handler *h;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (!oh_lookup_powerstate(State)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OH_CHECK_INIT_STATE(SessionId);
        OH_GET_DID(SessionId, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
        OH_RESOURCE_GET_CHECK(d, ResourceId, res);

        if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_CAPABILITY;
        }

        OH_HANDLER_GET(d, ResourceId, h);
        oh_release_domain(d); /* Unlock domain */

        OH_CALL_ABI(h, set_power_state, SA_ERR_HPI_INVALID_CMD, rv,
                    ResourceId, State);
        oh_release_handler(h);

        return rv;
}
