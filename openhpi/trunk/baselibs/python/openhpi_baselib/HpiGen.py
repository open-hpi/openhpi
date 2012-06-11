
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#

from openhpi_baselib.HpiCore import HpiCore
from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib import HpiUtil
from openhpi_baselib import OhpiDataTypes


#**********************************************************
# HPI API (auto-generated)
#**********************************************************

#**********************************************************
def saHpiVersionGet():
    return SAHPI_INTERFACE_VERSION

#**********************************************************
def saHpiDiscover(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DISCOVER )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiDomainInfoGet(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DOMAIN_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        DomainInfo = m.demarshalSaHpiDomainInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, DomainInfo )

#**********************************************************
def saHpiDrtEntryGet(
    SessionId,
    EntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiEntryIdT( EntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DRT_ENTRY_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextEntryId = m.demarshalSaHpiEntryIdT()
        DrtEntry = m.demarshalSaHpiDrtEntryT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextEntryId, DrtEntry )

#**********************************************************
def saHpiDomainTagSet(
    SessionId,
    DomainTag
):
    rc = HpiUtil.checkSaHpiTextBufferT( DomainTag )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiTextBufferT( DomainTag )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DOMAIN_TAG_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiRptEntryGet(
    SessionId,
    EntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiEntryIdT( EntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RPT_ENTRY_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextEntryId = m.demarshalSaHpiEntryIdT()
        RptEntry = m.demarshalSaHpiRptEntryT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextEntryId, RptEntry )

#**********************************************************
def saHpiRptEntryGetByResourceId(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RPT_ENTRY_GET_BY_RESOURCE_ID )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        RptEntry = m.demarshalSaHpiRptEntryT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, RptEntry )

#**********************************************************
def saHpiResourceSeveritySet(
    SessionId,
    ResourceId,
    Severity
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSeverityT( Severity )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_SEVERITY_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourceTagSet(
    SessionId,
    ResourceId,
    ResourceTag
):
    rc = HpiUtil.checkSaHpiTextBufferT( ResourceTag )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiTextBufferT( ResourceTag )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_TAG_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiGetIdByEntityPath(
    SessionId,
    EntityPath,
    InstrumentType,
    InstanceId
):
    rc = HpiUtil.checkSaHpiEntityPathT( EntityPath )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None, None, None, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiEntityPathT( EntityPath )
    m.marshalSaHpiRdrTypeT( InstrumentType )
    m.marshalSaHpiUint32T( InstanceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_GET_ID_BY_ENTITY_PATH )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        InstanceId = m.demarshalSaHpiUint32T()
        ResourceId = m.demarshalSaHpiResourceIdT()
        InstrumentId = m.demarshalSaHpiInstrumentIdT()
        RptUpdateCount = m.demarshalSaHpiUint32T()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None, None, None )
    return ( SA_OK, InstanceId, ResourceId, InstrumentId, RptUpdateCount )

#**********************************************************
def saHpiGetChildEntityPath(
    SessionId,
    ParentEntityPath,
    InstanceId
):
    rc = HpiUtil.checkSaHpiEntityPathT( ParentEntityPath )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None, None, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiEntityPathT( ParentEntityPath )
    m.marshalSaHpiUint32T( InstanceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_GET_CHILD_ENTITY_PATH )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        InstanceId = m.demarshalSaHpiUint32T()
        ChildEntityPath = m.demarshalSaHpiEntityPathT()
        RptUpdateCount = m.demarshalSaHpiUint32T()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None, None )
    return ( SA_OK, InstanceId, ChildEntityPath, RptUpdateCount )

#**********************************************************
def saHpiResourceFailedRemove(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_FAILED_REMOVE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventLogInfoGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Info = m.demarshalSaHpiEventLogInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Info )

#**********************************************************
def saHpiEventLogCapabilitiesGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_CAPABILITIES_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        EventLogCapabilities = m.demarshalSaHpiEventLogCapabilitiesT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, EventLogCapabilities )

#**********************************************************
def saHpiEventLogEntryGet(
    SessionId,
    ResourceId,
    EntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None, None, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiEventLogEntryIdT( EntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_ENTRY_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        PrevEntryId = m.demarshalSaHpiEventLogEntryIdT()
        NextEntryId = m.demarshalSaHpiEventLogEntryIdT()
        EventLogEntry = m.demarshalSaHpiEventLogEntryT()
        Rdr = m.demarshalSaHpiRdrT()
        RptEntry = m.demarshalSaHpiRptEntryT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None, None, None, None )
    return ( SA_OK, PrevEntryId, NextEntryId, EventLogEntry, Rdr, RptEntry )

#**********************************************************
def saHpiEventLogEntryAdd(
    SessionId,
    ResourceId,
    EvtEntry
):
    rc = HpiUtil.checkSaHpiEventT( EvtEntry )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiEventT( EvtEntry )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_ENTRY_ADD )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventLogClear(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_CLEAR )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventLogTimeGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_TIME_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Time = m.demarshalSaHpiTimeT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Time )

#**********************************************************
def saHpiEventLogTimeSet(
    SessionId,
    ResourceId,
    Time
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiTimeT( Time )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_TIME_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventLogStateGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_STATE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        EnableState = m.demarshalSaHpiBoolT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, EnableState )

#**********************************************************
def saHpiEventLogStateSet(
    SessionId,
    ResourceId,
    EnableState
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiBoolT( EnableState )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_STATE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventLogOverflowReset(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_LOG_OVERFLOW_RESET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiSubscribe(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SUBSCRIBE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiUnsubscribe(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_UNSUBSCRIBE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiEventGet(
    SessionId,
    Timeout
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiTimeoutT( Timeout )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Event = m.demarshalSaHpiEventT()
        Rdr = m.demarshalSaHpiRdrT()
        RptEntry = m.demarshalSaHpiRptEntryT()
        EventQueueStatus = m.demarshalSaHpiEvtQueueStatusT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None, None, None )
    return ( SA_OK, Event, Rdr, RptEntry, EventQueueStatus )

#**********************************************************
def saHpiEventAdd(
    SessionId,
    EvtEntry
):
    rc = HpiUtil.checkSaHpiEventT( EvtEntry )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiEventT( EvtEntry )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_EVENT_ADD )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAlarmGetNext(
    SessionId,
    Severity,
    UnacknowledgedOnly,
    Alarm
):
    rc = HpiUtil.checkSaHpiAlarmT( Alarm )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiSeverityT( Severity )
    m.marshalSaHpiBoolT( UnacknowledgedOnly )
    m.marshalSaHpiAlarmT( Alarm )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ALARM_GET_NEXT )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Alarm = m.demarshalSaHpiAlarmT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Alarm )

#**********************************************************
def saHpiAlarmGet(
    SessionId,
    AlarmId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiAlarmIdT( AlarmId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ALARM_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Alarm = m.demarshalSaHpiAlarmT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Alarm )

#**********************************************************
def saHpiAlarmAcknowledge(
    SessionId,
    AlarmId,
    Severity
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiAlarmIdT( AlarmId )
    m.marshalSaHpiSeverityT( Severity )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ALARM_ACKNOWLEDGE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAlarmAdd(
    SessionId,
    Alarm
):
    rc = HpiUtil.checkSaHpiAlarmT( Alarm )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiAlarmT( Alarm )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ALARM_ADD )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Alarm = m.demarshalSaHpiAlarmT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Alarm )

#**********************************************************
def saHpiAlarmDelete(
    SessionId,
    AlarmId,
    Severity
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiAlarmIdT( AlarmId )
    m.marshalSaHpiSeverityT( Severity )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ALARM_DELETE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiRdrGet(
    SessionId,
    ResourceId,
    EntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiEntryIdT( EntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RDR_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextEntryId = m.demarshalSaHpiEntryIdT()
        Rdr = m.demarshalSaHpiRdrT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextEntryId, Rdr )

#**********************************************************
def saHpiRdrGetByInstrumentId(
    SessionId,
    ResourceId,
    RdrType,
    InstrumentId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiRdrTypeT( RdrType )
    m.marshalSaHpiInstrumentIdT( InstrumentId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RDR_GET_BY_INSTRUMENT_ID )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Rdr = m.demarshalSaHpiRdrT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Rdr )

#**********************************************************
def saHpiRdrUpdateCountGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RDR_UPDATE_COUNT_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        UpdateCount = m.demarshalSaHpiUint32T()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, UpdateCount )

#**********************************************************
def saHpiSensorReadingGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_READING_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Reading = m.demarshalSaHpiSensorReadingT()
        EventState = m.demarshalSaHpiEventStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, Reading, EventState )

#**********************************************************
def saHpiSensorThresholdsGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_THRESHOLDS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SensorThresholds = m.demarshalSaHpiSensorThresholdsT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, SensorThresholds )

#**********************************************************
def saHpiSensorThresholdsSet(
    SessionId,
    ResourceId,
    SensorNum,
    SensorThresholds
):
    rc = HpiUtil.checkSaHpiSensorThresholdsT( SensorThresholds )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    m.marshalSaHpiSensorThresholdsT( SensorThresholds )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_THRESHOLDS_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiSensorTypeGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_TYPE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Type = m.demarshalSaHpiSensorTypeT()
        Category = m.demarshalSaHpiEventCategoryT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, Type, Category )

#**********************************************************
def saHpiSensorEnableGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_ENABLE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SensorEnabled = m.demarshalSaHpiBoolT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, SensorEnabled )

#**********************************************************
def saHpiSensorEnableSet(
    SessionId,
    ResourceId,
    SensorNum,
    SensorEnabled
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    m.marshalSaHpiBoolT( SensorEnabled )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_ENABLE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiSensorEventEnableGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_EVENT_ENABLE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SensorEventsEnabled = m.demarshalSaHpiBoolT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, SensorEventsEnabled )

#**********************************************************
def saHpiSensorEventEnableSet(
    SessionId,
    ResourceId,
    SensorNum,
    SensorEventsEnabled
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    m.marshalSaHpiBoolT( SensorEventsEnabled )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_EVENT_ENABLE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiSensorEventMasksGet(
    SessionId,
    ResourceId,
    SensorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_EVENT_MASKS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        AssertEventMask = m.demarshalSaHpiEventStateT()
        DeassertEventMask = m.demarshalSaHpiEventStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, AssertEventMask, DeassertEventMask )

#**********************************************************
def saHpiSensorEventMasksSet(
    SessionId,
    ResourceId,
    SensorNum,
    Action,
    AssertEventMask,
    DeassertEventMask
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiSensorNumT( SensorNum )
    m.marshalSaHpiSensorEventMaskActionT( Action )
    m.marshalSaHpiEventStateT( AssertEventMask )
    m.marshalSaHpiEventStateT( DeassertEventMask )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_SENSOR_EVENT_MASKS_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiControlTypeGet(
    SessionId,
    ResourceId,
    CtrlNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiCtrlNumT( CtrlNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_CONTROL_TYPE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Type = m.demarshalSaHpiCtrlTypeT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Type )

#**********************************************************
def saHpiControlGet(
    SessionId,
    ResourceId,
    CtrlNum,
    CtrlState
):
    rc = HpiUtil.checkSaHpiCtrlStateT( CtrlState )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiCtrlNumT( CtrlNum )
    m.marshalSaHpiCtrlStateT( CtrlState )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_CONTROL_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        CtrlMode = m.demarshalSaHpiCtrlModeT()
        CtrlState = m.demarshalSaHpiCtrlStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, CtrlMode, CtrlState )

#**********************************************************
def saHpiControlSet(
    SessionId,
    ResourceId,
    CtrlNum,
    CtrlMode,
    CtrlState
):
    rc = HpiUtil.checkSaHpiCtrlStateT( CtrlState )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiCtrlNumT( CtrlNum )
    m.marshalSaHpiCtrlModeT( CtrlMode )
    m.marshalSaHpiCtrlStateT( CtrlState )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_CONTROL_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiIdrInfoGet(
    SessionId,
    ResourceId,
    IdrId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        IdrInfo = m.demarshalSaHpiIdrInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, IdrInfo )

#**********************************************************
def saHpiIdrAreaHeaderGet(
    SessionId,
    ResourceId,
    IdrId,
    AreaType,
    AreaId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrAreaTypeT( AreaType )
    m.marshalSaHpiEntryIdT( AreaId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_AREA_HEADER_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextAreaId = m.demarshalSaHpiEntryIdT()
        Header = m.demarshalSaHpiIdrAreaHeaderT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextAreaId, Header )

#**********************************************************
def saHpiIdrAreaAdd(
    SessionId,
    ResourceId,
    IdrId,
    AreaType
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrAreaTypeT( AreaType )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_AREA_ADD )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        AreaId = m.demarshalSaHpiEntryIdT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, AreaId )

#**********************************************************
def saHpiIdrAreaAddById(
    SessionId,
    ResourceId,
    IdrId,
    AreaType,
    AreaId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrAreaTypeT( AreaType )
    m.marshalSaHpiEntryIdT( AreaId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_AREA_ADD_BY_ID )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiIdrAreaDelete(
    SessionId,
    ResourceId,
    IdrId,
    AreaId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiEntryIdT( AreaId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_AREA_DELETE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiIdrFieldGet(
    SessionId,
    ResourceId,
    IdrId,
    AreaId,
    FieldType,
    FieldId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiEntryIdT( AreaId )
    m.marshalSaHpiIdrFieldTypeT( FieldType )
    m.marshalSaHpiEntryIdT( FieldId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_FIELD_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextFieldId = m.demarshalSaHpiEntryIdT()
        Field = m.demarshalSaHpiIdrFieldT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextFieldId, Field )

#**********************************************************
def saHpiIdrFieldAdd(
    SessionId,
    ResourceId,
    IdrId,
    Field
):
    rc = HpiUtil.checkSaHpiIdrFieldT( Field )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrFieldT( Field )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_FIELD_ADD )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Field = m.demarshalSaHpiIdrFieldT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Field )

#**********************************************************
def saHpiIdrFieldAddById(
    SessionId,
    ResourceId,
    IdrId,
    Field
):
    rc = HpiUtil.checkSaHpiIdrFieldT( Field )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrFieldT( Field )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_FIELD_ADD_BY_ID )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiIdrFieldSet(
    SessionId,
    ResourceId,
    IdrId,
    Field
):
    rc = HpiUtil.checkSaHpiIdrFieldT( Field )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiIdrFieldT( Field )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_FIELD_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiIdrFieldDelete(
    SessionId,
    ResourceId,
    IdrId,
    AreaId,
    FieldId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiIdrIdT( IdrId )
    m.marshalSaHpiEntryIdT( AreaId )
    m.marshalSaHpiEntryIdT( FieldId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_IDR_FIELD_DELETE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiWatchdogTimerGet(
    SessionId,
    ResourceId,
    WatchdogNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiWatchdogNumT( WatchdogNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_WATCHDOG_TIMER_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Watchdog = m.demarshalSaHpiWatchdogT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Watchdog )

#**********************************************************
def saHpiWatchdogTimerSet(
    SessionId,
    ResourceId,
    WatchdogNum,
    Watchdog
):
    rc = HpiUtil.checkSaHpiWatchdogT( Watchdog )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiWatchdogNumT( WatchdogNum )
    m.marshalSaHpiWatchdogT( Watchdog )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_WATCHDOG_TIMER_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiWatchdogTimerReset(
    SessionId,
    ResourceId,
    WatchdogNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiWatchdogNumT( WatchdogNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_WATCHDOG_TIMER_RESET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAnnunciatorGetNext(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    Severity,
    UnacknowledgedOnly,
    Announcement
):
    rc = HpiUtil.checkSaHpiAnnouncementT( Announcement )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiSeverityT( Severity )
    m.marshalSaHpiBoolT( UnacknowledgedOnly )
    m.marshalSaHpiAnnouncementT( Announcement )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_GET_NEXT )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Announcement = m.demarshalSaHpiAnnouncementT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Announcement )

#**********************************************************
def saHpiAnnunciatorGet(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    EntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiEntryIdT( EntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Announcement = m.demarshalSaHpiAnnouncementT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Announcement )

#**********************************************************
def saHpiAnnunciatorAcknowledge(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    EntryId,
    Severity
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiEntryIdT( EntryId )
    m.marshalSaHpiSeverityT( Severity )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_ACKNOWLEDGE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAnnunciatorAdd(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    Announcement
):
    rc = HpiUtil.checkSaHpiAnnouncementT( Announcement )
    if not rc:
        return ( SA_ERR_HPI_INVALID_PARAMS, None )

    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiAnnouncementT( Announcement )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_ADD )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Announcement = m.demarshalSaHpiAnnouncementT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Announcement )

#**********************************************************
def saHpiAnnunciatorDelete(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    EntryId,
    Severity
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiEntryIdT( EntryId )
    m.marshalSaHpiSeverityT( Severity )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_DELETE )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAnnunciatorModeGet(
    SessionId,
    ResourceId,
    AnnunciatorNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_MODE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Mode = m.demarshalSaHpiAnnunciatorModeT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Mode )

#**********************************************************
def saHpiAnnunciatorModeSet(
    SessionId,
    ResourceId,
    AnnunciatorNum,
    Mode
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum )
    m.marshalSaHpiAnnunciatorModeT( Mode )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_ANNUNCIATOR_MODE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiDimiInfoGet(
    SessionId,
    ResourceId,
    DimiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        DimiInfo = m.demarshalSaHpiDimiInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, DimiInfo )

#**********************************************************
def saHpiDimiTestInfoGet(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        DimiTest = m.demarshalSaHpiDimiTestT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, DimiTest )

#**********************************************************
def saHpiDimiTestReadinessGet(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_READINESS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        DimiReady = m.demarshalSaHpiDimiReadyT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, DimiReady )

#**********************************************************
def saHpiDimiTestCancel(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_CANCEL )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiDimiTestStatusGet(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_STATUS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        PercentCompleted = m.demarshalSaHpiDimiTestPercentCompletedT()
        RunStatus = m.demarshalSaHpiDimiTestRunStatusT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, PercentCompleted, RunStatus )

#**********************************************************
def saHpiDimiTestResultsGet(
    SessionId,
    ResourceId,
    DimiNum,
    TestNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiDimiNumT( DimiNum )
    m.marshalSaHpiDimiTestNumT( TestNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_DIMI_TEST_RESULTS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        TestResults = m.demarshalSaHpiDimiTestResultsT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, TestResults )

#**********************************************************
def saHpiFumiSpecInfoGet(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SPEC_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SpecInfo = m.demarshalSaHpiFumiSpecInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, SpecInfo )

#**********************************************************
def saHpiFumiServiceImpactGet(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SERVICE_IMPACT_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        ServiceImpact = m.demarshalSaHpiFumiServiceImpactDataT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, ServiceImpact )

#**********************************************************
def saHpiFumiSourceSet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum,
    SourceUri
):
    rc = HpiUtil.checkSaHpiTextBufferT( SourceUri )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    m.marshalSaHpiTextBufferT( SourceUri )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SOURCE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiSourceInfoValidateStart(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SOURCE_INFO_VALIDATE_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiSourceInfoGet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SOURCE_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        SourceInfo = m.demarshalSaHpiFumiSourceInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, SourceInfo )

#**********************************************************
def saHpiFumiSourceComponentInfoGet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum,
    ComponentEntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    m.marshalSaHpiEntryIdT( ComponentEntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_SOURCE_COMPONENT_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextComponentEntryId = m.demarshalSaHpiEntryIdT()
        ComponentInfo = m.demarshalSaHpiFumiComponentInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextComponentEntryId, ComponentInfo )

#**********************************************************
def saHpiFumiTargetInfoGet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_TARGET_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        BankInfo = m.demarshalSaHpiFumiBankInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, BankInfo )

#**********************************************************
def saHpiFumiTargetComponentInfoGet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum,
    ComponentEntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    m.marshalSaHpiEntryIdT( ComponentEntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_TARGET_COMPONENT_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextComponentEntryId = m.demarshalSaHpiEntryIdT()
        ComponentInfo = m.demarshalSaHpiFumiComponentInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextComponentEntryId, ComponentInfo )

#**********************************************************
def saHpiFumiLogicalTargetInfoGet(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_LOGICAL_TARGET_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        BankInfo = m.demarshalSaHpiFumiLogicalBankInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, BankInfo )

#**********************************************************
def saHpiFumiLogicalTargetComponentInfoGet(
    SessionId,
    ResourceId,
    FumiNum,
    ComponentEntryId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiEntryIdT( ComponentEntryId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_LOGICAL_TARGET_COMPONENT_INFO_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        NextComponentEntryId = m.demarshalSaHpiEntryIdT()
        ComponentInfo = m.demarshalSaHpiFumiLogicalComponentInfoT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None, None )
    return ( SA_OK, NextComponentEntryId, ComponentInfo )

#**********************************************************
def saHpiFumiBackupStart(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_BACKUP_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiBankBootOrderSet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum,
    Position
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    m.marshalSaHpiUint32T( Position )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_BANK_BOOT_ORDER_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiBankCopyStart(
    SessionId,
    ResourceId,
    FumiNum,
    SourceBankNum,
    TargetBankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( SourceBankNum )
    m.marshalSaHpiBankNumT( TargetBankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_BANK_COPY_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiInstallStart(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_INSTALL_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiUpgradeStatusGet(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_UPGRADE_STATUS_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        UpgradeStatus = m.demarshalSaHpiFumiUpgradeStatusT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, UpgradeStatus )

#**********************************************************
def saHpiFumiTargetVerifyStart(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_TARGET_VERIFY_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiTargetVerifyMainStart(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_TARGET_VERIFY_MAIN_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiUpgradeCancel(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_UPGRADE_CANCEL )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiAutoRollbackDisableGet(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Disable = m.demarshalSaHpiBoolT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Disable )

#**********************************************************
def saHpiFumiAutoRollbackDisableSet(
    SessionId,
    ResourceId,
    FumiNum,
    Disable
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBoolT( Disable )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiRollbackStart(
    SessionId,
    ResourceId,
    FumiNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_ROLLBACK_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiActivateStart(
    SessionId,
    ResourceId,
    FumiNum,
    Logical
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBoolT( Logical )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_ACTIVATE_START )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiFumiCleanup(
    SessionId,
    ResourceId,
    FumiNum,
    BankNum
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiFumiNumT( FumiNum )
    m.marshalSaHpiBankNumT( BankNum )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_FUMI_CLEANUP )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiHotSwapPolicyCancel(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_HOTSWAP_POLICY_CANCEL )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourceActiveSet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_ACTIVE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourceInactiveSet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_INACTIVE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAutoInsertTimeoutGet(
    SessionId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_AUTO_INSERT_TIMEOUT_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Timeout = m.demarshalSaHpiTimeoutT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Timeout )

#**********************************************************
def saHpiAutoInsertTimeoutSet(
    SessionId,
    Timeout
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiTimeoutT( Timeout )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_AUTO_INSERT_TIMEOUT_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiAutoExtractTimeoutGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        Timeout = m.demarshalSaHpiTimeoutT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, Timeout )

#**********************************************************
def saHpiAutoExtractTimeoutSet(
    SessionId,
    ResourceId,
    Timeout
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiTimeoutT( Timeout )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiHotSwapStateGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_HOTSWAP_STATE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        State = m.demarshalSaHpiHsStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, State )

#**********************************************************
def saHpiHotSwapActionRequest(
    SessionId,
    ResourceId,
    Action
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiHsActionT( Action )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_HOTSWAP_ACTION_REQUEST )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiHotSwapIndicatorStateGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_HOTSWAP_INDICATOR_STATE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        State = m.demarshalSaHpiHsIndicatorStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, State )

#**********************************************************
def saHpiHotSwapIndicatorStateSet(
    SessionId,
    ResourceId,
    State
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiHsIndicatorStateT( State )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_HOTSWAP_INDICATOR_STATE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiParmControl(
    SessionId,
    ResourceId,
    Action
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiParmActionT( Action )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_PARM_CONTROL )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourceLoadIdGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_LOADID_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        LoadId = m.demarshalSaHpiLoadIdT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, LoadId )

#**********************************************************
def saHpiResourceLoadIdSet(
    SessionId,
    ResourceId,
    LoadId
):
    rc = HpiUtil.checkSaHpiLoadIdT( LoadId )
    if not rc:
        return SA_ERR_HPI_INVALID_PARAMS

    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiLoadIdT( LoadId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_LOADID_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourceResetStateGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_RESET_STATE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        ResetAction = m.demarshalSaHpiResetActionT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, ResetAction )

#**********************************************************
def saHpiResourceResetStateSet(
    SessionId,
    ResourceId,
    ResetAction
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiResetActionT( ResetAction )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_RESET_STATE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

#**********************************************************
def saHpiResourcePowerStateGet(
    SessionId,
    ResourceId
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return ( SA_ERR_HPI_INVALID_SESSION, None )
    m = s.getMarshal()
    if m is None:
        return ( SA_ERR_HPI_NO_RESPONSE, None )

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_POWER_STATE_GET )
    if not rc:
        m.close()
        return ( SA_ERR_HPI_NO_RESPONSE, None )
    rv = m.demarshalSaErrorT()
    if rv == SA_OK:
        State = m.demarshalSaHpiPowerStateT()

    s.putMarshal( m )

    if rv != SA_OK:
        return ( rv, None )
    return ( SA_OK, State )

#**********************************************************
def saHpiResourcePowerStateSet(
    SessionId,
    ResourceId,
    State
):
    s = HpiCore.getSession( SessionId )
    if s is None:
        return SA_ERR_HPI_INVALID_SESSION
    m = s.getMarshal()
    if m is None:
        return SA_ERR_HPI_NO_RESPONSE

    m.marshalSaHpiSessionIdT( s.getRemoteSid() )
    m.marshalSaHpiResourceIdT( ResourceId )
    m.marshalSaHpiPowerStateT( State )
    rc = m.interchange( OhpiDataTypes.RPC_SAHPI_RESOURCE_POWER_STATE_SET )
    if not rc:
        m.close()
        return SA_ERR_HPI_NO_RESPONSE
    rv = m.demarshalSaErrorT()
    # No output arguments

    s.putMarshal( m )

    if rv != SA_OK:
        return rv
    return SA_OK

