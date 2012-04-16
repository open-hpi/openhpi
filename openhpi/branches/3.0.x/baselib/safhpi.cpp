/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#include <stddef.h>
#include <string.h>

#include <glib.h>

#include <SaHpi.h>
#include <oHpi.h>

#include <marshal_hpi.h>
#include <oh_rpc_params.h>
#include <oh_utils.h>

#include "conf.h"
#include "init.h"
#include "session.h"


#include <oh_error.h>
/*----------------------------------------------------------------------------*/
/* Utility functions                                                          */
/*----------------------------------------------------------------------------*/

static SaErrorT clean_reading(SaHpiSensorReadingT *read_in,
                              SaHpiSensorReadingT *read_out)
{
    /* This is a workaround against unknown bugs in the marshal code */
    if (!read_in || !read_out) return SA_ERR_HPI_INVALID_PARAMS;

    memset(read_out, 0, sizeof(SaHpiSensorReadingT));

    read_out->IsSupported = read_in->IsSupported;

    if (read_in->IsSupported == SAHPI_TRUE) {
        if (!oh_lookup_sensorreadingtype(read_in->Type)) {
            return SA_ERR_HPI_INVALID_DATA;
        }
        read_out->Type = read_in->Type;
    }
    else {
        // TODO: Do we need to set dummy & reading type
        // just to keep marshalling happy?
        read_out->Type = SAHPI_SENSOR_READING_TYPE_INT64;
        read_out->Value.SensorInt64 = 0;
        return SA_OK;
    }

    if (read_in->Type == SAHPI_SENSOR_READING_TYPE_INT64) {
        read_out->Value.SensorInt64 = read_in->Value.SensorInt64;
    } else if (read_in->Type == SAHPI_SENSOR_READING_TYPE_UINT64) {
        read_out->Value.SensorUint64 = read_in->Value.SensorUint64;
    } else if (read_in->Type == SAHPI_SENSOR_READING_TYPE_FLOAT64) {
        read_out->Value.SensorFloat64 = read_in->Value.SensorFloat64;
    } else if (read_in->Type == SAHPI_SENSOR_READING_TYPE_BUFFER) {
        memcpy(read_out->Value.SensorBuffer,
               read_in->Value.SensorBuffer,
               SAHPI_SENSOR_BUFFER_LENGTH);
    }

    return SA_OK;
}

static SaErrorT clean_thresholds(SaHpiSensorThresholdsT *thrds_in,
                                 SaHpiSensorThresholdsT *thrds_out)
{
    /* This is a workaround against unknown bugs in the marshal code */
    SaErrorT rv;
    if (!thrds_in || !thrds_out) return SA_ERR_HPI_INVALID_PARAMS;

    rv = clean_reading(&thrds_in->LowCritical, &thrds_out->LowCritical);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->LowMajor, &thrds_out->LowMajor);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->LowMinor, &thrds_out->LowMinor);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->UpCritical, &thrds_out->UpCritical);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->UpMajor, &thrds_out->UpMajor);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->UpMinor, &thrds_out->UpMinor);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->PosThdHysteresis,
    &thrds_out->PosThdHysteresis);
    if (rv != SA_OK) return rv;
    rv = clean_reading(&thrds_in->NegThdHysteresis,
    &thrds_out->NegThdHysteresis);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiVersionGet                                                            */
/*----------------------------------------------------------------------------*/

SaHpiVersionT SAHPI_API saHpiVersionGet (void)
{
    return SAHPI_INTERFACE_VERSION;
}

/*----------------------------------------------------------------------------*/
/* saHpiInitialize                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiInitialize(
    SAHPI_IN    SaHpiVersionT    RequestedVersion,
    SAHPI_IN    SaHpiUint32T     NumOptions,
    SAHPI_INOUT SaHpiInitOptionT *Options,
    SAHPI_OUT   SaHpiUint32T     *FailedOption,
    SAHPI_OUT   SaErrorT         *OptionError)
{
    if (RequestedVersion < OH_SAHPI_INTERFACE_VERSION_MIN_SUPPORTED ||
        RequestedVersion > OH_SAHPI_INTERFACE_VERSION_MAX_SUPPORTED)
    {
        return SA_ERR_HPI_UNSUPPORTED_API;
    }
    if ((NumOptions != 0) && (Options == 0)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    for ( size_t i = 0; i < NumOptions; ++i ) {
        const SaHpiInitOptionT& o = Options[i];
        if (o.OptionId >= SA_HPI_INITOPTION_FIRST_OEM ) {
            continue;
        }
        if ( o.OptionId != SA_HPI_INITOPTION_HANDLE_CREATE_THREAD ) {
            if ( FailedOption ) {
                *FailedOption = i;
            }
            if ( OptionError ) {
                *OptionError = SA_ERR_HPI_UNKNOWN;
            }
            return SA_ERR_HPI_INVALID_DATA;
        }
    }

    // TODO implement more checks from section 5.2.1 of B.03.01 spec
    // Current implementation does not cover options check

    // TODO implement any library initialization code here
    // Current implementation does not utilize this function
    //

    return ohc_init();
}

/*----------------------------------------------------------------------------*/
/* saHpiFinalize                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiFinalize()
{
    // TODO implement
    // TODO implement any library finalization code here
    // Current implementation does not utilize this function

    return ohc_finit();
}

/*----------------------------------------------------------------------------*/
/* saHpiSessionOpen                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSessionOpen(
    SAHPI_IN SaHpiDomainIdT   DomainId,
    SAHPI_OUT SaHpiSessionIdT *SessionId,
    SAHPI_IN  void            *SecurityParams)
{
    if (!SessionId || SecurityParams) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    if (DomainId == SAHPI_UNSPECIFIED_DOMAIN_ID) {
        DomainId = OH_DEFAULT_DOMAIN_ID;
    }

    return ohc_sess_open(DomainId, *SessionId);
}


/*----------------------------------------------------------------------------*/
/* saHpiSessionClose                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSessionClose(
    SAHPI_IN SaHpiSessionIdT SessionId)
{
    return ohc_sess_close(SessionId);
}


/*----------------------------------------------------------------------------*/
/* saHpiDiscover                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiDiscover(
    SAHPI_IN SaHpiSessionIdT SessionId)
{
    SaErrorT rv;

    ClientRpcParams iparams;
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiDiscover, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiDomainInfoGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiDomainInfoGet(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_OUT SaHpiDomainInfoT *DomainInfo)
{
    SaErrorT rv;

    if (!DomainInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams;
    ClientRpcParams oparams(DomainInfo);
    rv = ohc_sess_rpc(eFsaHpiDomainInfoGet, SessionId, iparams, oparams);

    /* Set Domain Id to real Domain Id */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, DomainInfo->DomainId);
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiDrtEntryGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiDrtEntryGet(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiEntryIdT   EntryId,
    SAHPI_OUT SaHpiEntryIdT  *NextEntryId,
    SAHPI_OUT SaHpiDrtEntryT *DrtEntry)
{
    SaErrorT rv;

    if ((!DrtEntry) || (!NextEntryId) || (EntryId == SAHPI_LAST_ENTRY))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&EntryId);
    ClientRpcParams oparams(NextEntryId, DrtEntry);
    rv = ohc_sess_rpc(eFsaHpiDrtEntryGet, SessionId, iparams, oparams);

    /* Set Domain Id to real Domain Id */
    if (rv == SA_OK) {
        // TODO: fix me
        rv = ohc_sess_get_did(SessionId, DrtEntry->DomainId);
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiDomainTagSet                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiDomainTagSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiTextBufferT *DomainTag)
{
    SaErrorT rv;

    if (!DomainTag) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_lookup_texttype(DomainTag->DataType)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(DomainTag);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiDomainTagSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiRptEntryGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiRptEntryGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiEntryIdT    EntryId,
    SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
    SAHPI_OUT SaHpiRptEntryT  *RptEntry)
{
    SaErrorT rv;

    if ((!NextEntryId) || (!RptEntry)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (EntryId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&EntryId);
    ClientRpcParams oparams(NextEntryId, RptEntry);
    rv = ohc_sess_rpc(eFsaHpiRptEntryGet, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            oh_concat_ep(&RptEntry->ResourceEntity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiRptEntryGetByResourceId                                               */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiRptEntryT  *RptEntry)
{
    SaErrorT rv;

    if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID || (!RptEntry)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(RptEntry);
    rv = ohc_sess_rpc(eFsaHpiRptEntryGetByResourceId, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            oh_concat_ep(&RptEntry->ResourceEntity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceSeveritySet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceSeveritySet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSeverityT   Severity)
{
    SaErrorT rv;

    if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_lookup_severity(Severity)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Severity);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceSeveritySet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceTagSet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceTagSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
    SaErrorT rv;

    if (!ResourceTag) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, ResourceTag);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceTagSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiMyEntityPathGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiMyEntityPathGet(
    SAHPI_IN  SaHpiSessionIdT  SessionId,
    SAHPI_OUT SaHpiEntityPathT *EntityPath)
{
    if (!EntityPath) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    const SaHpiEntityPathT *my_entity = ohc_get_my_entity();
    if ( !my_entity) {
        return SA_ERR_HPI_UNKNOWN;
    }

    *EntityPath = *my_entity;

    return SA_OK;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceIdGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceIdGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
    if (!ResourceId) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    SaErrorT rv;
    SaHpiEntityPathT ep;

    rv = saHpiMyEntityPathGet( SessionId, &ep );
    if ( rv != SA_OK ) {
        return SA_ERR_HPI_UNKNOWN;
    }

    SaHpiUint32T instance = SAHPI_FIRST_ENTRY;
    SaHpiUint32T rpt_update_count;
    rv = saHpiGetIdByEntityPath( SessionId,
                                 ep,
                                 SAHPI_NO_RECORD,
                                 &instance,
                                 ResourceId,
                                 0,
                                 &rpt_update_count );
    if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
        return SA_ERR_HPI_NOT_PRESENT;
    } else if ( rv != SA_OK ) {
        return SA_ERR_HPI_UNKNOWN;
    }

    return SA_OK;
}


/*----------------------------------------------------------------------------*/
/* saHpiGetIdByEntityPath                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiGetIdByEntityPath(
    SAHPI_IN    SaHpiSessionIdT    SessionId,
    SAHPI_IN    SaHpiEntityPathT   EntityPath,
    SAHPI_IN    SaHpiRdrTypeT      InstrumentType,
    SAHPI_INOUT SaHpiUint32T       *InstanceId,
    SAHPI_OUT   SaHpiResourceIdT   *ResourceId,
    SAHPI_OUT   SaHpiInstrumentIdT *InstrumentId,
    SAHPI_OUT   SaHpiUint32T       *RptUpdateCount)
{
    SaErrorT rv;
    SaHpiInstrumentIdT instrument_id;

    if ((!ResourceId) || (!InstanceId) ||
        (*InstanceId == SAHPI_LAST_ENTRY) || (!RptUpdateCount) ||
        ((!InstrumentId) && (InstrumentType != SAHPI_NO_RECORD)))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!InstrumentId) {
        InstrumentId = &instrument_id;
    }

    SaHpiEntityPathT entity_root;
    rv = ohc_sess_get_entity_root(SessionId, entity_root);
    if (rv != SA_OK) {
        return rv;
    }

    // Remove domain entity_root from the EntityPath
    SaHpiEntityPathT ep;
    rv = oh_get_child_ep(&EntityPath, &entity_root, &ep);
    if (rv != SA_OK) {
        return rv;
    }

    ClientRpcParams iparams(&ep, &InstrumentType, InstanceId);
    ClientRpcParams oparams(InstanceId, ResourceId, InstrumentId, RptUpdateCount);
    rv = ohc_sess_rpc(eFsaHpiGetIdByEntityPath, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiGetChildEntityPath                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiGetChildEntityPath(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiEntityPathT ParentEntityPath,
    SAHPI_INOUT SaHpiUint32T     *InstanceId,
    SAHPI_OUT   SaHpiEntityPathT *ChildEntityPath,
    SAHPI_OUT   SaHpiUint32T     *RptUpdateCount)
{
    SaErrorT rv;

    if ((!InstanceId) || (*InstanceId == SAHPI_LAST_ENTRY) || (!RptUpdateCount)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!ChildEntityPath) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    SaHpiEntityPathT entity_root;
    rv = ohc_sess_get_entity_root(SessionId, entity_root);
    if (rv != SA_OK) {
        return rv;
    }

    // Remove domain entity_root from the ParentEntityPath
    SaHpiEntityPathT parent;
    oh_init_ep(&parent);
    if (oh_ep_len(&ParentEntityPath) > 0) {
        rv = oh_get_child_ep(&ParentEntityPath, &entity_root, &parent);
        if (rv != SA_OK) {
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                rv = SA_ERR_HPI_INVALID_DATA;
            }
            return rv;
        }
    }

    ClientRpcParams iparams(&parent, InstanceId);
    ClientRpcParams oparams(InstanceId, ChildEntityPath, RptUpdateCount);
    rv = ohc_sess_rpc(eFsaHpiGetChildEntityPath, SessionId, iparams, oparams);

    // Add domain entity_root to obtained child
    if (rv == SA_OK)  {
        oh_concat_ep(ChildEntityPath, &entity_root);
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceFailedRemove                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceFailedRemove(
    SAHPI_IN    SaHpiSessionIdT        SessionId,
    SAHPI_IN    SaHpiResourceIdT       ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceFailedRemove, SessionId, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* saHpiEventLogInfoGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogInfoGet(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId,
    SAHPI_OUT SaHpiEventLogInfoT *Info)
{
    SaErrorT rv;

    if (!Info) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(Info);
    rv = ohc_sess_rpc(eFsaHpiEventLogInfoGet, SessionId, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* saHpiEventLogCapabilitiesGet                                               */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogCapabilitiesGet(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId,
    SAHPI_OUT SaHpiEventLogCapabilitiesT  *EventLogCapabilities)
{
    SaErrorT rv;

    if (!EventLogCapabilities) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(EventLogCapabilities);
    rv = ohc_sess_rpc(eFsaHpiEventLogCapabilitiesGet, SessionId, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* saHpiEventLogEntryGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogEntryGet(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiEntryIdT          EntryId,
    SAHPI_OUT SaHpiEventLogEntryIdT *PrevEntryId,
    SAHPI_OUT SaHpiEventLogEntryIdT *NextEntryId,
    SAHPI_OUT SaHpiEventLogEntryT   *EventLogEntry,
    SAHPI_INOUT SaHpiRdrT           *Rdr,
    SAHPI_INOUT SaHpiRptEntryT      *RptEntry)
{
    SaErrorT rv;

    if ((!PrevEntryId) || (!EventLogEntry) || (!NextEntryId) ||
        (EntryId == SAHPI_NO_MORE_ENTRIES))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    SaHpiRdrT rdr;
    SaHpiRptEntryT rpte;

    ClientRpcParams iparams(&ResourceId, &EntryId);
    ClientRpcParams oparams(PrevEntryId, NextEntryId, EventLogEntry, &rdr, &rpte);
    rv = ohc_sess_rpc(eFsaHpiEventLogEntryGet, SessionId, iparams, oparams);

    if (Rdr) {
        memcpy(Rdr, &rdr, sizeof(SaHpiRdrT));
    }
    if (RptEntry) {
        memcpy(RptEntry, &rpte, sizeof(SaHpiRptEntryT));
    }

    /* If this event is Domain Event, then adjust DomainId */
    if ((ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) &&
        (EventLogEntry->Event.EventType == SAHPI_ET_DOMAIN))
    {
        if (rv == SA_OK) {
            SaHpiDomainIdT did;
            rv = ohc_sess_get_did(SessionId, did );
            EventLogEntry->Event.EventDataUnion.DomainEvent.DomainId = did;
        }
    }
    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if ((rv == SA_OK) && RptEntry) {
            oh_concat_ep(&RptEntry->ResourceEntity, &entity_root);
        }
        if ((rv == SA_OK) && Rdr) {
            oh_concat_ep(&Rdr->Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogEntryAdd                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogEntryAdd(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiEventT      *EvtEntry)
{
    SaErrorT rv;

    if (!EvtEntry) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (EvtEntry->EventType != SAHPI_ET_USER ||
        EvtEntry->Source != SAHPI_UNSPECIFIED_RESOURCE_ID)
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_lookup_severity(EvtEntry->Severity)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_valid_textbuffer(&EvtEntry->EventDataUnion.UserEvent.UserEventData))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, EvtEntry);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventLogEntryAdd, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogClear                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogClear(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventLogClear, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogTimeGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogTimeGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiTimeT      *Time)
{
    SaErrorT rv;

    if (!Time) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(Time);
    rv = ohc_sess_rpc(eFsaHpiEventLogTimeGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogTimeSet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogTimeSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiTimeT       Time)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &Time);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventLogTimeSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogStateGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogStateGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiBoolT      *EnableState)
{
    SaErrorT rv;

    if (!EnableState) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(EnableState);
    rv = ohc_sess_rpc(eFsaHpiEventLogStateGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogStateSet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogStateSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiBoolT       EnableState)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &EnableState);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventLogStateSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogOverflowReset                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventLogOverflowReset(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventLogOverflowReset, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSubscribe                                                             */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSubscribe(
    SAHPI_IN SaHpiSessionIdT  SessionId)
{
    SaErrorT rv;

    ClientRpcParams iparams;
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiSubscribe, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiUnsSubscribe                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiUnsubscribe(
    SAHPI_IN SaHpiSessionIdT  SessionId)
{
    SaErrorT rv;

    ClientRpcParams iparams;
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiUnsubscribe, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventGet                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventGet(
    SAHPI_IN SaHpiSessionIdT         SessionId,
    SAHPI_IN SaHpiTimeoutT           Timeout,
    SAHPI_OUT SaHpiEventT            *Event,
    SAHPI_INOUT SaHpiRdrT            *Rdr,
    SAHPI_INOUT SaHpiRptEntryT       *RptEntry,
    SAHPI_INOUT SaHpiEvtQueueStatusT *EventQueueStatus)
{
    SaErrorT rv;

    if (Timeout < SAHPI_TIMEOUT_BLOCK || !Event) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    SaHpiRdrT rdr;
    SaHpiRptEntryT rpte;
    SaHpiEvtQueueStatusT status;

    ClientRpcParams iparams(&Timeout);
    ClientRpcParams oparams(Event, &rdr, &rpte, &status);
    rv = ohc_sess_rpc(eFsaHpiEventGet, SessionId, iparams, oparams);

    if (Rdr) {
        memcpy(Rdr, &rdr, sizeof(SaHpiRdrT));
    }
    if (RptEntry) {
        memcpy(RptEntry, &rpte, sizeof(SaHpiRptEntryT));
    }
    if (EventQueueStatus) {
        memcpy(EventQueueStatus, &status, sizeof(SaHpiEvtQueueStatusT));
    }

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if ((rv == SA_OK) && RptEntry) {
            oh_concat_ep(&RptEntry->ResourceEntity, &entity_root);
        }
        if ((rv == SA_OK) && Rdr) {
            oh_concat_ep(&Rdr->Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventAdd                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiEventAdd(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiEventT     *Event)
{
    SaErrorT rv;

    rv = oh_valid_addevent(Event);
    if (rv != SA_OK) return rv;

    ClientRpcParams iparams(Event);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiEventAdd, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmGetNext                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAlarmGetNext(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiSeverityT  Severity,
    SAHPI_IN SaHpiBoolT      Unack,
    SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
    SaErrorT rv;

    if (!Alarm) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_lookup_severity(Severity)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (Alarm->AlarmId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    ClientRpcParams iparams(&Severity, &Unack, Alarm);
    ClientRpcParams oparams(Alarm);
    rv = ohc_sess_rpc(eFsaHpiAlarmGetNext, SessionId, iparams, oparams);

    /* Set Alarm DomainId to DomainId that HPI Application sees */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, Alarm->AlarmCond.DomainId);
    }
    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            if (Alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER) {
                // We do not append entity root to user added alarm
                oh_concat_ep(&Alarm->AlarmCond.Entity, &entity_root);
            }
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmGet                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAlarmGet(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiAlarmIdT   AlarmId,
    SAHPI_OUT SaHpiAlarmT    *Alarm)
{
    SaErrorT rv;

    if (!Alarm) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&AlarmId);
    ClientRpcParams oparams(Alarm);
    rv = ohc_sess_rpc(eFsaHpiAlarmGet, SessionId, iparams, oparams);

    /* Set Alarm DomainId to DomainId that HPI Application sees */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, Alarm->AlarmCond.DomainId);
    }
    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            if (Alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER) {
                // We do not append entity root to user added alarm
                oh_concat_ep(&Alarm->AlarmCond.Entity, &entity_root);
            }
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmAcknowledge                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAlarmAcknowledge(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiAlarmIdT   AlarmId,
    SAHPI_IN SaHpiSeverityT  Severity)
{
    SaErrorT rv;

    if (AlarmId == SAHPI_ENTRY_UNSPECIFIED &&
        !oh_lookup_severity(Severity))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&AlarmId, &Severity);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAlarmAcknowledge, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmAdd                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAlarmAdd(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
    SaErrorT rv;

    if (!Alarm ||
        !oh_lookup_severity(Alarm->Severity) ||
        Alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER)
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(Alarm);
    ClientRpcParams oparams(Alarm);
    rv = ohc_sess_rpc(eFsaHpiAlarmAdd, SessionId, iparams, oparams);

    /* Set Alarm DomainId to DomainId that HPI Application sees */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, Alarm->AlarmCond.DomainId);
    }

    /* NB: We do not modify entity path here. */

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmDelete                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAlarmDelete(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiAlarmIdT   AlarmId,
    SAHPI_IN SaHpiSeverityT  Severity)
{
    SaErrorT rv;

    if (AlarmId == SAHPI_ENTRY_UNSPECIFIED && !oh_lookup_severity(Severity)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&AlarmId, &Severity);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAlarmDelete, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiRdrGet                                                                */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiRdrGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiEntryIdT    EntryId,
    SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
    SAHPI_OUT SaHpiRdrT       *Rdr)
{
    SaErrorT rv;

    if (EntryId == SAHPI_LAST_ENTRY || !Rdr || !NextEntryId) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &EntryId);
    ClientRpcParams oparams(NextEntryId, Rdr);
    rv = ohc_sess_rpc(eFsaHpiRdrGet, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            oh_concat_ep(&Rdr->Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiRdrGetByInstrumentId                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiRdrGetByInstrumentId(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiRdrTypeT      RdrType,
    SAHPI_IN SaHpiInstrumentIdT InstrumentId,
    SAHPI_OUT SaHpiRdrT         *Rdr)
{
    SaErrorT rv;

    if (!oh_lookup_rdrtype(RdrType) || RdrType == SAHPI_NO_RECORD || !Rdr) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &RdrType, &InstrumentId);
    ClientRpcParams oparams(Rdr);
    rv = ohc_sess_rpc(eFsaHpiRdrGetByInstrumentId, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            oh_concat_ep(&Rdr->Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiRdrUpdateCountGet                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiRdrUpdateCountGet(
    SAHPI_IN  SaHpiSessionIdT  SessionId,
    SAHPI_IN  SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiUint32T     *UpdateCount)
{
    SaErrorT rv;

    if (!UpdateCount) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(UpdateCount);
    rv = ohc_sess_rpc(eFsaHpiRdrUpdateCountGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorReadingGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorReadingGet(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiSensorNumT        SensorNum,
    SAHPI_INOUT SaHpiSensorReadingT *Reading,
    SAHPI_INOUT SaHpiEventStateT    *EventState)
{
    SaErrorT rv;

    SaHpiSensorReadingT reading;
    SaHpiEventStateT state;

    ClientRpcParams iparams(&ResourceId, &SensorNum);
    ClientRpcParams oparams(&reading, &state);
    rv = ohc_sess_rpc(eFsaHpiSensorReadingGet, SessionId, iparams, oparams);

    if (Reading) {
        memcpy(Reading, &reading, sizeof(SaHpiSensorReadingT));
    }
    if (EventState) {
        memcpy(EventState, &state, sizeof(SaHpiEventStateT));
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorThresholdsGet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorThresholdsGet(
    SAHPI_IN SaHpiSessionIdT         SessionId,
    SAHPI_IN SaHpiResourceIdT        ResourceId,
    SAHPI_IN SaHpiSensorNumT         SensorNum,
    SAHPI_OUT SaHpiSensorThresholdsT *Thresholds)
{
    SaErrorT rv;

    if (!Thresholds) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &SensorNum);
    ClientRpcParams oparams(Thresholds);
    rv = ohc_sess_rpc(eFsaHpiSensorThresholdsGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorThresholdsSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorThresholdsSet(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiSensorNumT        SensorNum,
    SAHPI_IN SaHpiSensorThresholdsT *Thresholds)
{
    SaErrorT rv;
    SaHpiSensorThresholdsT tholds;

    if (!Thresholds) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    rv = clean_thresholds(Thresholds, &tholds);
    if (rv != SA_OK) return rv;

    ClientRpcParams iparams(&ResourceId, &SensorNum, &tholds);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiSensorThresholdsSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorTypeGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorTypeGet(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiSensorNumT      SensorNum,
    SAHPI_OUT SaHpiSensorTypeT    *Type,
    SAHPI_OUT SaHpiEventCategoryT *Category)
{
    SaErrorT rv;

    if (!Type || !Category) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &SensorNum);
    ClientRpcParams oparams(Type, Category);
    rv = ohc_sess_rpc(eFsaHpiSensorTypeGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEnableGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEnableGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSensorNumT  SensorNum,
    SAHPI_OUT SaHpiBoolT      *Enabled)
{
    SaErrorT rv;

    if (!Enabled) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &SensorNum);
    ClientRpcParams oparams(Enabled);
    rv = ohc_sess_rpc(eFsaHpiSensorEnableGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEnableSet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEnableSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSensorNumT  SensorNum,
    SAHPI_IN SaHpiBoolT       Enabled)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &SensorNum, &Enabled);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiSensorEnableSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventEnableGet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEventEnableGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSensorNumT  SensorNum,
    SAHPI_OUT SaHpiBoolT      *Enabled)
{
    SaErrorT rv;

    if (!Enabled) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &SensorNum);
    ClientRpcParams oparams(Enabled);
    rv = ohc_sess_rpc(eFsaHpiSensorEventEnableGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventEnableSet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEventEnableSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSensorNumT  SensorNum,
    SAHPI_IN SaHpiBoolT       Enabled)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &SensorNum, &Enabled);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiSensorEventEnableSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventMasksGet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEventMasksGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiSensorNumT  SensorNum,
    SAHPI_INOUT SaHpiEventStateT *Assert,
    SAHPI_INOUT SaHpiEventStateT *Deassert)
{
    SaErrorT rv;

    SaHpiEventStateT assert, deassert;

    ClientRpcParams iparams(&ResourceId, &SensorNum, &assert, &deassert);
    ClientRpcParams oparams(&assert, &deassert);
    rv = ohc_sess_rpc(eFsaHpiSensorEventMasksGet, SessionId, iparams, oparams);

    if (Assert) {
        *Assert = assert;
    }
    if (Deassert) {
        *Deassert = deassert;
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventMasksSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiSensorEventMasksSet(
    SAHPI_IN SaHpiSessionIdT             SessionId,
    SAHPI_IN SaHpiResourceIdT            ResourceId,
    SAHPI_IN SaHpiSensorNumT             SensorNum,
    SAHPI_IN SaHpiSensorEventMaskActionT Action,
    SAHPI_IN SaHpiEventStateT            Assert,
    SAHPI_IN SaHpiEventStateT            Deassert)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &SensorNum, &Action, &Assert, &Deassert);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiSensorEventMasksSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlTypeGet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiControlTypeGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiCtrlNumT    CtrlNum,
    SAHPI_OUT SaHpiCtrlTypeT  *Type)
{
    SaErrorT rv;

    if (!Type) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &CtrlNum);
    ClientRpcParams oparams(Type);
    rv = ohc_sess_rpc(eFsaHpiControlTypeGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlGet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiControlGet(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiCtrlNumT      CtrlNum,
    SAHPI_OUT SaHpiCtrlModeT    *Mode,
    SAHPI_INOUT SaHpiCtrlStateT *State)
{
    SaErrorT rv;

    SaHpiCtrlModeT mode;
    SaHpiCtrlStateT state;

    if (State) {
        memcpy(&state, State, sizeof(SaHpiCtrlStateT));
        if (!oh_lookup_ctrltype(state.Type)) {
            state.Type = SAHPI_CTRL_TYPE_TEXT;
            state.StateUnion.Text.Line = SAHPI_TLN_ALL_LINES;
        }
    }
    else {
        state.Type = SAHPI_CTRL_TYPE_TEXT;
        state.StateUnion.Text.Line = SAHPI_TLN_ALL_LINES;
    }

    ClientRpcParams iparams(&ResourceId, &CtrlNum, &state);
    ClientRpcParams oparams(&mode, &state);
    rv = ohc_sess_rpc(eFsaHpiControlGet, SessionId, iparams, oparams);

    if (Mode) {
        memcpy(Mode, &mode, sizeof(SaHpiCtrlModeT));
    }
    if (State) {
        memcpy(State, &state, sizeof(SaHpiCtrlStateT));
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlSet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiControlSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiCtrlNumT    CtrlNum,
    SAHPI_IN SaHpiCtrlModeT   Mode,
    SAHPI_IN SaHpiCtrlStateT  *State)
{
    SaErrorT rv;
    SaHpiCtrlStateT mystate, *pmystate = 0;

    if (!oh_lookup_ctrlmode(Mode) ||
        (Mode != SAHPI_CTRL_MODE_AUTO && !State) ||
        (State && State->Type == SAHPI_CTRL_TYPE_DIGITAL &&
        !oh_lookup_ctrlstatedigital(State->StateUnion.Digital)) ||
        (State && State->Type == SAHPI_CTRL_TYPE_STREAM &&
        State->StateUnion.Stream.StreamLength > SAHPI_CTRL_MAX_STREAM_LENGTH))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    memset(&mystate, 0, sizeof(SaHpiCtrlStateT));
    if (Mode == SAHPI_CTRL_MODE_AUTO) {
        pmystate = &mystate;
    } else if (State && !oh_lookup_ctrltype(State->Type)) {
        return SA_ERR_HPI_INVALID_DATA;
    } else {
        pmystate = State;
    }

    ClientRpcParams iparams(&ResourceId, &CtrlNum, &Mode, pmystate);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiControlSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrInfoGet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrInfoGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiIdrIdT      Idrid,
    SAHPI_OUT SaHpiIdrInfoT   *Info)
{
    SaErrorT rv;

    if (!Info) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid);
    ClientRpcParams oparams(Info);
    rv = ohc_sess_rpc(eFsaHpiIdrInfoGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaHeaderGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrAreaHeaderGet(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiIdrIdT          Idrid,
    SAHPI_IN SaHpiIdrAreaTypeT    AreaType,
    SAHPI_IN SaHpiEntryIdT        AreaId,
    SAHPI_OUT SaHpiEntryIdT       *NextAreaId,
    SAHPI_OUT SaHpiIdrAreaHeaderT *Header)
{
    SaErrorT rv;

    if (((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
         ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
         (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
         (AreaType != SAHPI_IDR_AREATYPE_OEM)) ||
         (AreaId == SAHPI_LAST_ENTRY)||
         (!NextAreaId) ||
         (!Header)))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaType, &AreaId);
    ClientRpcParams oparams(NextAreaId, Header);
    rv = ohc_sess_rpc(eFsaHpiIdrAreaHeaderGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaAdd                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrAreaAdd(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiIdrIdT       Idrid,
    SAHPI_IN SaHpiIdrAreaTypeT AreaType,
    SAHPI_OUT SaHpiEntryIdT    *AreaId)
{
    SaErrorT rv;

    if (!oh_lookup_idrareatype(AreaType) || (!AreaId) ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaType);
    ClientRpcParams oparams(AreaId);
    rv = ohc_sess_rpc(eFsaHpiIdrAreaAdd, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaAddById                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrAreaAddById(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiIdrIdT       Idrid,
    SAHPI_IN SaHpiIdrAreaTypeT AreaType,
    SAHPI_IN SaHpiEntryIdT     AreaId)
{
    SaErrorT rv;

    if (!oh_lookup_idrareatype(AreaType))   {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaType, &AreaId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiIdrAreaAddById, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaDelete                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrAreaDelete(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiIdrIdT       Idrid,
    SAHPI_IN SaHpiEntryIdT     AreaId)
{
    SaErrorT rv;

    if (AreaId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiIdrAreaDelete, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrFieldGet(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiIdrIdT        Idrid,
    SAHPI_IN SaHpiEntryIdT      AreaId,
    SAHPI_IN SaHpiIdrFieldTypeT FieldType,
    SAHPI_IN SaHpiEntryIdT      FieldId,
    SAHPI_OUT SaHpiEntryIdT     *NextId,
    SAHPI_OUT SaHpiIdrFieldT    *Field)
{
    SaErrorT rv;

    if (!Field ||
        !oh_lookup_idrfieldtype(FieldType) ||
        AreaId == SAHPI_LAST_ENTRY ||
        FieldId == SAHPI_LAST_ENTRY ||
        !NextId)
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaId, &FieldType, &FieldId);
    ClientRpcParams oparams(NextId, Field);
    rv = ohc_sess_rpc(eFsaHpiIdrFieldGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldAdd                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrFieldAdd(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiIdrIdT        Idrid,
    SAHPI_INOUT SaHpiIdrFieldT  *Field)
{
    SaErrorT rv;

    if (!Field)   {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (!oh_lookup_idrfieldtype(Field->Type)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (Field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (oh_valid_textbuffer(&Field->Field) != SAHPI_TRUE) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, Field);
    ClientRpcParams oparams(Field);
    rv = ohc_sess_rpc(eFsaHpiIdrFieldAdd, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldAddById                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrFieldAddById(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiIdrIdT        Idrid,
    SAHPI_INOUT SaHpiIdrFieldT  *Field)
{
    SaErrorT rv;

    if (!Field)   {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (!oh_lookup_idrfieldtype(Field->Type)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (Field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
        return SA_ERR_HPI_INVALID_PARAMS;
    } else if (oh_valid_textbuffer(&Field->Field) != SAHPI_TRUE) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, Field);
    ClientRpcParams oparams(Field);
    rv = ohc_sess_rpc(eFsaHpiIdrFieldAddById, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldSet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrFieldSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiIdrIdT      Idrid,
    SAHPI_IN SaHpiIdrFieldT   *Field)
{
    SaErrorT rv;

    if (!Field) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, Field);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiIdrFieldSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldDelete                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiIdrFieldDelete(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiIdrIdT      Idrid,
    SAHPI_IN SaHpiEntryIdT    AreaId,
    SAHPI_IN SaHpiEntryIdT    FieldId)
{
    SaErrorT rv;

    if (FieldId == SAHPI_LAST_ENTRY || AreaId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Idrid, &AreaId, &FieldId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiIdrFieldDelete, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiWatchdogTimerGet(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
    SAHPI_OUT SaHpiWatchdogT   *Watchdog)
{
    SaErrorT rv;

    if (!Watchdog) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &WatchdogNum);
    ClientRpcParams oparams(Watchdog);
    rv = ohc_sess_rpc(eFsaHpiWatchdogTimerGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerSet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiWatchdogTimerSet(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
    SAHPI_IN SaHpiWatchdogT    *Watchdog)
{
    SaErrorT rv;

    if (!Watchdog ||
        !oh_lookup_watchdogtimeruse(Watchdog->TimerUse) ||
        !oh_lookup_watchdogaction(Watchdog->TimerAction) ||
        !oh_lookup_watchdogpretimerinterrupt(Watchdog->PretimerInterrupt))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    if (Watchdog->PreTimeoutInterval > Watchdog->InitialCount) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    ClientRpcParams iparams(&ResourceId, &WatchdogNum, Watchdog);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiWatchdogTimerSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerReset                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiWatchdogTimerReset(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &WatchdogNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiWatchdogTimerReset, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorGetNext                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorGetNext(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
    SAHPI_IN SaHpiSeverityT        Severity,
    SAHPI_IN SaHpiBoolT            Unack,
    SAHPI_INOUT SaHpiAnnouncementT *Announcement)
{
    SaErrorT rv;

    if (!Announcement) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!oh_lookup_severity(Severity)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, &Severity, &Unack, Announcement);
    ClientRpcParams oparams(Announcement);
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorGetNext, SessionId, iparams, oparams);

    /* Set Announcement DomainId to DomainId that HPI Application sees */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, Announcement->StatusCond.DomainId);
    }
    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if ((rv == SA_OK) && (Announcement->AddedByUser == SAHPI_FALSE)) {
            // We do not append entity root to user added announcement
            oh_concat_ep(&Announcement->StatusCond.Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorGet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorGet(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
    SAHPI_IN SaHpiEntryIdT        EntryId,
    SAHPI_OUT SaHpiAnnouncementT  *Announcement)
{
    SaErrorT rv;

    if (!Announcement) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, &EntryId);
    ClientRpcParams oparams(Announcement);
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorGet, SessionId, iparams, oparams);

    /* Set Announcement DomainId to DomainId that HPI Application sees */
    if (rv == SA_OK) {
        rv = ohc_sess_get_did(SessionId, Announcement->StatusCond.DomainId);
    }
    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if ((rv == SA_OK) && (Announcement->AddedByUser == SAHPI_FALSE)) {
            // We do not append entity root to user added announcement
            oh_concat_ep(&Announcement->StatusCond.Entity, &entity_root);
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorAcknowledge                                                */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorAcknowledge(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
    SAHPI_IN SaHpiEntryIdT        EntryId,
    SAHPI_IN SaHpiSeverityT       Severity)
{
    SaErrorT rv;

    SaHpiSeverityT sev = SAHPI_DEBUG;

    if (EntryId == SAHPI_ENTRY_UNSPECIFIED) {
        if (!oh_lookup_severity(Severity)) {
            return SA_ERR_HPI_INVALID_PARAMS;
        } else {
            sev = Severity;
        }
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, &EntryId, &sev);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorAcknowledge, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorAdd                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorAdd(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
    SAHPI_INOUT SaHpiAnnouncementT *Announcement)
{
    SaErrorT rv;

    if (!Announcement) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    if (Announcement->Severity == SAHPI_ALL_SEVERITIES ||
        !oh_lookup_severity(Announcement->Severity) ||
        !oh_valid_textbuffer(&Announcement->StatusCond.Data) ||
        !oh_lookup_statuscondtype(Announcement->StatusCond.Type))
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, Announcement);
    ClientRpcParams oparams(Announcement);
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorAdd, SessionId, iparams, oparams);

    /* NB: We do not modify entity path here. */

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorDelete                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorDelete(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
    SAHPI_IN SaHpiEntryIdT        EntryId,
    SAHPI_IN SaHpiSeverityT       Severity)
{
    SaErrorT rv;

    SaHpiSeverityT sev = SAHPI_DEBUG;

    if (EntryId == SAHPI_ENTRY_UNSPECIFIED) {
        if (!oh_lookup_severity(Severity)) {
            return SA_ERR_HPI_INVALID_PARAMS;
        } else {
            sev = Severity;
        }
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, &EntryId, &sev);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorDelete, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorModeGet                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorModeGet(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT   AnnNum,
    SAHPI_OUT SaHpiAnnunciatorModeT *Mode)
{
    SaErrorT rv;

    if (!Mode) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum);
    ClientRpcParams oparams(Mode);
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorModeGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorModeSet                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAnnunciatorModeSet(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
    SAHPI_IN SaHpiAnnunciatorModeT Mode)
{
    SaErrorT rv;

    if (!oh_lookup_annunciatormode(Mode)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &AnnNum, &Mode);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAnnunciatorModeSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiDimiInfoGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiDimiInfoGet(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiDimiNumT      DimiNum,
    SAHPI_OUT SaHpiDimiInfoT   *DimiInfo)
{
    SaErrorT rv;

    if (!DimiInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &DimiNum);
    ClientRpcParams oparams(DimiInfo);
    rv = ohc_sess_rpc(eFsaHpiDimiInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestInfoGet(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiTestT       *DimiTest)
{
    SaErrorT rv;

    if (!DimiTest) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum);
    ClientRpcParams oparams(DimiTest);
    rv = ohc_sess_rpc(eFsaHpiDimiTestInfoGet, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            for ( size_t i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
                oh_concat_ep(&DimiTest->EntitiesImpacted[i].EntityImpacted,
                             &entity_root);
            }
        }
    }

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestReadinessGet(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiReadyT      *DimiReady)
{
    SaErrorT rv;

    if (!DimiReady) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum);
    ClientRpcParams oparams(DimiReady);
    rv = ohc_sess_rpc(eFsaHpiDimiTestReadinessGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestStart(
    SAHPI_IN  SaHpiSessionIdT                SessionId,
    SAHPI_IN  SaHpiResourceIdT               ResourceId,
    SAHPI_IN  SaHpiDimiNumT                  DimiNum,
    SAHPI_IN  SaHpiDimiTestNumT              TestNum,
    SAHPI_IN  SaHpiUint8T                    NumberOfParams,
    SAHPI_IN  SaHpiDimiTestVariableParamsT   *ParamsList)
{
    SaErrorT rv;
    SaHpiDimiTestVariableParamsListT params_list;

    if ((!ParamsList) && (NumberOfParams != 0)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    params_list.NumberOfParams = NumberOfParams;
    params_list.ParamsList = ParamsList;

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum, &params_list);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiDimiTestStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestCancel(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiDimiTestCancel, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestStatusGet(
    SAHPI_IN    SaHpiSessionIdT                   SessionId,
    SAHPI_IN    SaHpiResourceIdT                  ResourceId,
    SAHPI_IN    SaHpiDimiNumT                     DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT                 TestNum,
    SAHPI_OUT   SaHpiDimiTestPercentCompletedT    *PercentCompleted,
    SAHPI_OUT   SaHpiDimiTestRunStatusT           *RunStatus)
{
    SaErrorT rv;
    SaHpiDimiTestPercentCompletedT percent;
    SaHpiDimiTestPercentCompletedT *ppercent = &percent;

    if (!RunStatus) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (PercentCompleted) {
        ppercent = PercentCompleted;
    }

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum);
    ClientRpcParams oparams(ppercent, RunStatus);
    rv = ohc_sess_rpc(eFsaHpiDimiTestStatusGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiDimiTestResultsGet(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiDimiNumT            DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT        TestNum,
    SAHPI_OUT   SaHpiDimiTestResultsT    *TestResults)
{
    SaErrorT rv;

    if (!TestResults) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &DimiNum, &TestNum);
    ClientRpcParams oparams(TestResults);
    rv = ohc_sess_rpc(eFsaHpiDimiTestResultsGet, SessionId, iparams, oparams);

    return rv;
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
    SaErrorT rv;

    if (!SpecInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams(SpecInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiSpecInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiServiceImpactGet(
    SAHPI_IN   SaHpiSessionIdT              SessionId,
    SAHPI_IN   SaHpiResourceIdT             ResourceId,
    SAHPI_IN   SaHpiFumiNumT                FumiNum,
    SAHPI_OUT  SaHpiFumiServiceImpactDataT  *ServiceImpact)
{
    SaErrorT rv;

    if (!ServiceImpact) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams(ServiceImpact);
    rv = ohc_sess_rpc(eFsaHpiFumiServiceImpactGet, SessionId, iparams, oparams);

    if (rv == SA_OK)  {
        SaHpiEntityPathT entity_root;
        rv = ohc_sess_get_entity_root(SessionId, entity_root);
        if (rv == SA_OK) {
            for ( size_t i = 0; i < ServiceImpact->NumEntities; ++i ) {
                oh_concat_ep(&ServiceImpact->ImpactedEntities[i].ImpactedEntity,
                             &entity_root);
            }
        }
    }

    return rv;
}



SaErrorT SAHPI_API saHpiFumiSourceSet (
    SAHPI_IN    SaHpiSessionIdT         SessionId,
    SAHPI_IN    SaHpiResourceIdT        ResourceId,
    SAHPI_IN    SaHpiFumiNumT           FumiNum,
    SAHPI_IN    SaHpiBankNumT           BankNum,
    SAHPI_IN    SaHpiTextBufferT        *SourceUri)
{
    SaErrorT rv;

    if ((!SourceUri) || SourceUri->DataType != SAHPI_TL_TYPE_TEXT) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum, SourceUri);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiSourceSet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiSourceInfoValidateStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiSourceInfoValidateStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiSourceInfoGet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiSourceInfoT  *SourceInfo)
{
    SaErrorT rv;

    if (!SourceInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams(SourceInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiSourceInfoGet, SessionId, iparams, oparams);

    return rv;
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
    SaErrorT rv;

    if ((!NextComponentEntryId) || (!ComponentInfo)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (ComponentEntryId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum, &ComponentEntryId);
    ClientRpcParams oparams(NextComponentEntryId, ComponentInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiSourceComponentInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiTargetInfoGet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiBankInfoT    *BankInfo)
{
    SaErrorT rv;

    if (!BankInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams(BankInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiTargetInfoGet, SessionId, iparams, oparams);

    return rv;
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
    SaErrorT rv;

    if ((!NextComponentEntryId) || (!ComponentInfo)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (ComponentEntryId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum, &ComponentEntryId);
    ClientRpcParams oparams(NextComponentEntryId, ComponentInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiTargetComponentInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiLogicalTargetInfoGet(
    SAHPI_IN    SaHpiSessionIdT             SessionId,
    SAHPI_IN    SaHpiResourceIdT            ResourceId,
    SAHPI_IN    SaHpiFumiNumT               FumiNum,
    SAHPI_OUT   SaHpiFumiLogicalBankInfoT   *BankInfo)
{
    SaErrorT rv;

    if (!BankInfo) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams(BankInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiLogicalTargetInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiLogicalTargetComponentInfoGet(
    SAHPI_IN    SaHpiSessionIdT                SessionId,
    SAHPI_IN    SaHpiResourceIdT               ResourceId,
    SAHPI_IN    SaHpiFumiNumT                  FumiNum,
    SAHPI_IN    SaHpiEntryIdT                  ComponentEntryId,
    SAHPI_OUT   SaHpiEntryIdT                  *NextComponentEntryId,
    SAHPI_OUT   SaHpiFumiLogicalComponentInfoT *ComponentInfo)
{
    SaErrorT rv;

    if ((!NextComponentEntryId) || (!ComponentInfo)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (ComponentEntryId == SAHPI_LAST_ENTRY) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &ComponentEntryId);
    ClientRpcParams oparams(NextComponentEntryId, ComponentInfo);
    rv = ohc_sess_rpc(eFsaHpiFumiLogicalTargetComponentInfoGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiBackupStart(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiBackupStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiBankBootOrderSet (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_IN    SaHpiUint32T          Position)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum, &Position);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiBankBootOrderSet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiBankCopyStart(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         SourceBankNum,
    SAHPI_IN    SaHpiBankNumT         TargetBankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &SourceBankNum, &TargetBankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiBankCopyStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiInstallStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiInstallStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiUpgradeStatusGet (
    SAHPI_IN    SaHpiSessionIdT         SessionId,
    SAHPI_IN    SaHpiResourceIdT        ResourceId,
    SAHPI_IN    SaHpiFumiNumT           FumiNum,
    SAHPI_IN    SaHpiBankNumT           BankNum,
    SAHPI_OUT   SaHpiFumiUpgradeStatusT *UpgradeStatus)
{
    SaErrorT rv;

    if (!UpgradeStatus) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams(UpgradeStatus);
    rv = ohc_sess_rpc(eFsaHpiFumiUpgradeStatusGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiTargetVerifyStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiTargetVerifyStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiTargetVerifyMainStart(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiResourceIdT ResourceId,
    SAHPI_IN    SaHpiFumiNumT    FumiNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiTargetVerifyMainStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiUpgradeCancel (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiUpgradeCancel, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiAutoRollbackDisableGet(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiResourceIdT ResourceId,
    SAHPI_IN    SaHpiFumiNumT    FumiNum,
    SAHPI_OUT   SaHpiBoolT       *Disable)
{
    SaErrorT rv;

    if (!Disable) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams(Disable);
    rv = ohc_sess_rpc(eFsaHpiFumiAutoRollbackDisableGet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiAutoRollbackDisableSet(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiResourceIdT ResourceId,
    SAHPI_IN    SaHpiFumiNumT    FumiNum,
    SAHPI_IN    SaHpiBoolT       Disable)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &Disable);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiAutoRollbackDisableSet, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiRollbackStart (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiRollbackStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiActivate (
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiActivate, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiActivateStart(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiResourceIdT ResourceId,
    SAHPI_IN    SaHpiFumiNumT    FumiNum,
    SAHPI_IN    SaHpiBoolT       Logical)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &Logical);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiActivateStart, SessionId, iparams, oparams);

    return rv;
}

SaErrorT SAHPI_API saHpiFumiCleanup(
    SAHPI_IN    SaHpiSessionIdT  SessionId,
    SAHPI_IN    SaHpiResourceIdT ResourceId,
    SAHPI_IN    SaHpiFumiNumT    FumiNum,
    SAHPI_IN    SaHpiBankNumT    BankNum)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId, &FumiNum, &BankNum);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiFumiCleanup, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapPolicyCancel                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiHotSwapPolicyCancel(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiHotSwapPolicyCancel, SessionId, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* saHpiResourceActiveSet                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceActiveSet(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceActiveSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceInactiveSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceInactiveSet(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceInactiveSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoInsertTimeoutGet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_OUT SaHpiTimeoutT  *Timeout)
{
    SaErrorT rv;

    if (!Timeout) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams;
    ClientRpcParams oparams(Timeout);
    rv = ohc_sess_rpc(eFsaHpiAutoInsertTimeoutGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoInsertTimeoutSet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
    SAHPI_IN SaHpiSessionIdT SessionId,
    SAHPI_IN SaHpiTimeoutT   Timeout)
{
    SaErrorT rv;

    if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
        Timeout != SAHPI_TIMEOUT_BLOCK &&
        Timeout < 0)
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&Timeout);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAutoInsertTimeoutSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoExtractGet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiTimeoutT   *Timeout)
{
    SaErrorT rv;

    if (!Timeout) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(Timeout);
    rv = ohc_sess_rpc(eFsaHpiAutoExtractTimeoutGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoExtractTimeoutSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiTimeoutT    Timeout)
{
    SaErrorT rv;

    if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
        Timeout != SAHPI_TIMEOUT_BLOCK &&
        Timeout < 0)
    {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Timeout);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiAutoExtractTimeoutSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapStateGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiHotSwapStateGet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiHsStateT   *State)
{
    SaErrorT rv;

    if (!State) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(State);
    rv = ohc_sess_rpc(eFsaHpiHotSwapStateGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapActionRequest                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiHotSwapActionRequest(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiHsActionT   Action)
{
    SaErrorT rv;

    if (!oh_lookup_hsaction(Action)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Action);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiHotSwapActionRequest, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapIndicatorStateGet                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet(
    SAHPI_IN SaHpiSessionIdT         SessionId,
    SAHPI_IN SaHpiResourceIdT        ResourceId,
    SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
    SaErrorT rv;

    if (!State) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(State);
    rv = ohc_sess_rpc(eFsaHpiHotSwapIndicatorStateGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapIndicatorStateSet                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiHsIndicatorStateT State)
{
    SaErrorT rv;

    if (!oh_lookup_hsindicatorstate(State)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &State);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiHotSwapIndicatorStateSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiParmControl                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiParmControl(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiParmActionT Action)
{
    SaErrorT rv;

    if (!oh_lookup_parmaction(Action)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Action);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiParmControl, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceLoadIdGet                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceLoadIdGet(
    SAHPI_IN  SaHpiSessionIdT  SessionId,
    SAHPI_IN  SaHpiResourceIdT ResourceId,
    SAHPI_OUT SaHpiLoadIdT *LoadId)
{
    SaErrorT rv;

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(LoadId);
    rv = ohc_sess_rpc(eFsaHpiResourceLoadIdGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceLoadIdSet                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceLoadIdSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiLoadIdT *LoadId)
{
    SaErrorT rv;

    if (!LoadId) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, LoadId);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceLoadIdSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceResetStateGet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceResetStateGet(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_OUT SaHpiResetActionT *Action)
{
    SaErrorT rv;

    if (!Action) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(Action);
    rv = ohc_sess_rpc(eFsaHpiResourceResetStateGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceResetStateSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourceResetStateSet(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_IN SaHpiResetActionT Action)
{
    SaErrorT rv;

    if (!oh_lookup_resetaction(Action)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &Action);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourceResetStateSet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourcePowerStateGet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourcePowerStateGet(
    SAHPI_IN SaHpiSessionIdT   SessionId,
    SAHPI_IN SaHpiResourceIdT  ResourceId,
    SAHPI_OUT SaHpiPowerStateT *State)
{
    SaErrorT rv;

    if (!State) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId);
    ClientRpcParams oparams(State);
    rv = ohc_sess_rpc(eFsaHpiResourcePowerStateGet, SessionId, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourcePowerStateSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API saHpiResourcePowerStateSet(
    SAHPI_IN SaHpiSessionIdT  SessionId,
    SAHPI_IN SaHpiResourceIdT ResourceId,
    SAHPI_IN SaHpiPowerStateT State)
{
    SaErrorT rv;

    if (!oh_lookup_powerstate(State)) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&ResourceId, &State);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFsaHpiResourcePowerStateSet, SessionId, iparams, oparams);

    return rv;
}

