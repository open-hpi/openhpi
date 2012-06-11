/*      -*- c# -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

using System;


namespace org {
namespace openhpi {


/**********************************************************
 * HPI API (auto-generated)
 *********************************************************/
public static partial class Api
{
    public static long saHpiVersionGet()
    {
        return HpiConst.SAHPI_INTERFACE_VERSION;
    }

    public static long saHpiDiscover(
        long SessionId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DISCOVER );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDomainInfoGet(
        long SessionId,
        out SaHpiDomainInfoT DomainInfo
    )
    {
        long rv;
        bool rc;

        DomainInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DOMAIN_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            DomainInfo = m.DemarshalSaHpiDomainInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDrtEntryGet(
        long SessionId,
        long EntryId,
        out long NextEntryId,
        out SaHpiDrtEntryT DrtEntry
    )
    {
        long rv;
        bool rc;

        NextEntryId = 0;
        DrtEntry = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiEntryIdT( EntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DRT_ENTRY_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextEntryId = m.DemarshalSaHpiEntryIdT();
            DrtEntry = m.DemarshalSaHpiDrtEntryT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDomainTagSet(
        long SessionId,
        SaHpiTextBufferT DomainTag
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( DomainTag );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiTextBufferT( DomainTag );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DOMAIN_TAG_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiRptEntryGet(
        long SessionId,
        long EntryId,
        out long NextEntryId,
        out SaHpiRptEntryT RptEntry
    )
    {
        long rv;
        bool rc;

        NextEntryId = 0;
        RptEntry = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiEntryIdT( EntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RPT_ENTRY_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextEntryId = m.DemarshalSaHpiEntryIdT();
            RptEntry = m.DemarshalSaHpiRptEntryT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiRptEntryGetByResourceId(
        long SessionId,
        long ResourceId,
        out SaHpiRptEntryT RptEntry
    )
    {
        long rv;
        bool rc;

        RptEntry = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RPT_ENTRY_GET_BY_RESOURCE_ID );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            RptEntry = m.DemarshalSaHpiRptEntryT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceSeveritySet(
        long SessionId,
        long ResourceId,
        long Severity
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSeverityT( Severity );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_SEVERITY_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceTagSet(
        long SessionId,
        long ResourceId,
        SaHpiTextBufferT ResourceTag
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( ResourceTag );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiTextBufferT( ResourceTag );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_TAG_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiGetIdByEntityPath(
        long SessionId,
        SaHpiEntityPathT EntityPath,
        long InstrumentType,
        ref long InstanceId,
        out long ResourceId,
        out long InstrumentId,
        out long RptUpdateCount
    )
    {
        long rv;
        bool rc;

        ResourceId = 0;
        InstrumentId = 0;
        RptUpdateCount = 0;

        rc = HpiUtil.Check( EntityPath );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiEntityPathT( EntityPath );
        m.MarshalSaHpiRdrTypeT( InstrumentType );
        m.MarshalSaHpiUint32T( InstanceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_GET_ID_BY_ENTITY_PATH );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            InstanceId = m.DemarshalSaHpiUint32T();
            ResourceId = m.DemarshalSaHpiResourceIdT();
            InstrumentId = m.DemarshalSaHpiInstrumentIdT();
            RptUpdateCount = m.DemarshalSaHpiUint32T();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiGetChildEntityPath(
        long SessionId,
        SaHpiEntityPathT ParentEntityPath,
        ref long InstanceId,
        out SaHpiEntityPathT ChildEntityPath,
        out long RptUpdateCount
    )
    {
        long rv;
        bool rc;

        ChildEntityPath = null;
        RptUpdateCount = 0;

        rc = HpiUtil.Check( ParentEntityPath );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiEntityPathT( ParentEntityPath );
        m.MarshalSaHpiUint32T( InstanceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_GET_CHILD_ENTITY_PATH );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            InstanceId = m.DemarshalSaHpiUint32T();
            ChildEntityPath = m.DemarshalSaHpiEntityPathT();
            RptUpdateCount = m.DemarshalSaHpiUint32T();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceFailedRemove(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_FAILED_REMOVE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogInfoGet(
        long SessionId,
        long ResourceId,
        out SaHpiEventLogInfoT Info
    )
    {
        long rv;
        bool rc;

        Info = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Info = m.DemarshalSaHpiEventLogInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogCapabilitiesGet(
        long SessionId,
        long ResourceId,
        out long EventLogCapabilities
    )
    {
        long rv;
        bool rc;

        EventLogCapabilities = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_CAPABILITIES_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            EventLogCapabilities = m.DemarshalSaHpiEventLogCapabilitiesT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogEntryGet(
        long SessionId,
        long ResourceId,
        long EntryId,
        out long PrevEntryId,
        out long NextEntryId,
        out SaHpiEventLogEntryT EventLogEntry,
        out SaHpiRdrT Rdr,
        out SaHpiRptEntryT RptEntry
    )
    {
        long rv;
        bool rc;

        PrevEntryId = 0;
        NextEntryId = 0;
        EventLogEntry = null;
        Rdr = null;
        RptEntry = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiEventLogEntryIdT( EntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_ENTRY_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            PrevEntryId = m.DemarshalSaHpiEventLogEntryIdT();
            NextEntryId = m.DemarshalSaHpiEventLogEntryIdT();
            EventLogEntry = m.DemarshalSaHpiEventLogEntryT();
            Rdr = m.DemarshalSaHpiRdrT();
            RptEntry = m.DemarshalSaHpiRptEntryT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogEntryAdd(
        long SessionId,
        long ResourceId,
        SaHpiEventT EvtEntry
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( EvtEntry );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiEventT( EvtEntry );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_ENTRY_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogClear(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_CLEAR );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogTimeGet(
        long SessionId,
        long ResourceId,
        out long Time
    )
    {
        long rv;
        bool rc;

        Time = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_TIME_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Time = m.DemarshalSaHpiTimeT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogTimeSet(
        long SessionId,
        long ResourceId,
        long Time
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiTimeT( Time );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_TIME_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogStateGet(
        long SessionId,
        long ResourceId,
        out long EnableState
    )
    {
        long rv;
        bool rc;

        EnableState = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_STATE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            EnableState = m.DemarshalSaHpiBoolT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogStateSet(
        long SessionId,
        long ResourceId,
        long EnableState
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiBoolT( EnableState );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_STATE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventLogOverflowReset(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_LOG_OVERFLOW_RESET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSubscribe(
        long SessionId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SUBSCRIBE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiUnsubscribe(
        long SessionId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_UNSUBSCRIBE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventGet(
        long SessionId,
        long Timeout,
        out SaHpiEventT Event,
        out SaHpiRdrT Rdr,
        out SaHpiRptEntryT RptEntry,
        out long EventQueueStatus
    )
    {
        long rv;
        bool rc;

        Event = null;
        Rdr = null;
        RptEntry = null;
        EventQueueStatus = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiTimeoutT( Timeout );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Event = m.DemarshalSaHpiEventT();
            Rdr = m.DemarshalSaHpiRdrT();
            RptEntry = m.DemarshalSaHpiRptEntryT();
            EventQueueStatus = m.DemarshalSaHpiEvtQueueStatusT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiEventAdd(
        long SessionId,
        SaHpiEventT EvtEntry
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( EvtEntry );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiEventT( EvtEntry );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_EVENT_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAlarmGetNext(
        long SessionId,
        long Severity,
        long UnacknowledgedOnly,
        ref SaHpiAlarmT Alarm
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Alarm );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiSeverityT( Severity );
        m.MarshalSaHpiBoolT( UnacknowledgedOnly );
        m.MarshalSaHpiAlarmT( Alarm );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ALARM_GET_NEXT );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Alarm = m.DemarshalSaHpiAlarmT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAlarmGet(
        long SessionId,
        long AlarmId,
        out SaHpiAlarmT Alarm
    )
    {
        long rv;
        bool rc;

        Alarm = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiAlarmIdT( AlarmId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ALARM_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Alarm = m.DemarshalSaHpiAlarmT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAlarmAcknowledge(
        long SessionId,
        long AlarmId,
        long Severity
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiAlarmIdT( AlarmId );
        m.MarshalSaHpiSeverityT( Severity );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ALARM_ACKNOWLEDGE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAlarmAdd(
        long SessionId,
        ref SaHpiAlarmT Alarm
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Alarm );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiAlarmT( Alarm );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ALARM_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Alarm = m.DemarshalSaHpiAlarmT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAlarmDelete(
        long SessionId,
        long AlarmId,
        long Severity
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiAlarmIdT( AlarmId );
        m.MarshalSaHpiSeverityT( Severity );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ALARM_DELETE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiRdrGet(
        long SessionId,
        long ResourceId,
        long EntryId,
        out long NextEntryId,
        out SaHpiRdrT Rdr
    )
    {
        long rv;
        bool rc;

        NextEntryId = 0;
        Rdr = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiEntryIdT( EntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RDR_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextEntryId = m.DemarshalSaHpiEntryIdT();
            Rdr = m.DemarshalSaHpiRdrT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiRdrGetByInstrumentId(
        long SessionId,
        long ResourceId,
        long RdrType,
        long InstrumentId,
        out SaHpiRdrT Rdr
    )
    {
        long rv;
        bool rc;

        Rdr = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiRdrTypeT( RdrType );
        m.MarshalSaHpiInstrumentIdT( InstrumentId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RDR_GET_BY_INSTRUMENT_ID );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Rdr = m.DemarshalSaHpiRdrT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiRdrUpdateCountGet(
        long SessionId,
        long ResourceId,
        out long UpdateCount
    )
    {
        long rv;
        bool rc;

        UpdateCount = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RDR_UPDATE_COUNT_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            UpdateCount = m.DemarshalSaHpiUint32T();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorReadingGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out SaHpiSensorReadingT Reading,
        out long EventState
    )
    {
        long rv;
        bool rc;

        Reading = null;
        EventState = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_READING_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Reading = m.DemarshalSaHpiSensorReadingT();
            EventState = m.DemarshalSaHpiEventStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorThresholdsGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out SaHpiSensorThresholdsT SensorThresholds
    )
    {
        long rv;
        bool rc;

        SensorThresholds = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_THRESHOLDS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SensorThresholds = m.DemarshalSaHpiSensorThresholdsT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorThresholdsSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        SaHpiSensorThresholdsT SensorThresholds
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( SensorThresholds );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        m.MarshalSaHpiSensorThresholdsT( SensorThresholds );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_THRESHOLDS_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorTypeGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out long Type,
        out long Category
    )
    {
        long rv;
        bool rc;

        Type = 0;
        Category = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_TYPE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Type = m.DemarshalSaHpiSensorTypeT();
            Category = m.DemarshalSaHpiEventCategoryT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEnableGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out long SensorEnabled
    )
    {
        long rv;
        bool rc;

        SensorEnabled = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_ENABLE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SensorEnabled = m.DemarshalSaHpiBoolT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEnableSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long SensorEnabled
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        m.MarshalSaHpiBoolT( SensorEnabled );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_ENABLE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventEnableGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out long SensorEventsEnabled
    )
    {
        long rv;
        bool rc;

        SensorEventsEnabled = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_EVENT_ENABLE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SensorEventsEnabled = m.DemarshalSaHpiBoolT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventEnableSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long SensorEventsEnabled
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        m.MarshalSaHpiBoolT( SensorEventsEnabled );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_EVENT_ENABLE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventMasksGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        out long AssertEventMask,
        out long DeassertEventMask
    )
    {
        long rv;
        bool rc;

        AssertEventMask = 0;
        DeassertEventMask = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_EVENT_MASKS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            AssertEventMask = m.DemarshalSaHpiEventStateT();
            DeassertEventMask = m.DemarshalSaHpiEventStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventMasksSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long Action,
        long AssertEventMask,
        long DeassertEventMask
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiSensorNumT( SensorNum );
        m.MarshalSaHpiSensorEventMaskActionT( Action );
        m.MarshalSaHpiEventStateT( AssertEventMask );
        m.MarshalSaHpiEventStateT( DeassertEventMask );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SENSOR_EVENT_MASKS_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiControlTypeGet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        out long Type
    )
    {
        long rv;
        bool rc;

        Type = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiCtrlNumT( CtrlNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_CONTROL_TYPE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Type = m.DemarshalSaHpiCtrlTypeT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiControlGet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        out long CtrlMode,
        ref SaHpiCtrlStateT CtrlState
    )
    {
        long rv;
        bool rc;

        CtrlMode = 0;

        rc = HpiUtil.Check( CtrlState );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiCtrlNumT( CtrlNum );
        m.MarshalSaHpiCtrlStateT( CtrlState );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_CONTROL_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            CtrlMode = m.DemarshalSaHpiCtrlModeT();
            CtrlState = m.DemarshalSaHpiCtrlStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiControlSet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        long CtrlMode,
        SaHpiCtrlStateT CtrlState
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( CtrlState );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiCtrlNumT( CtrlNum );
        m.MarshalSaHpiCtrlModeT( CtrlMode );
        m.MarshalSaHpiCtrlStateT( CtrlState );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_CONTROL_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrInfoGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        out SaHpiIdrInfoT IdrInfo
    )
    {
        long rv;
        bool rc;

        IdrInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            IdrInfo = m.DemarshalSaHpiIdrInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaHeaderGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        long AreaId,
        out long NextAreaId,
        out SaHpiIdrAreaHeaderT Header
    )
    {
        long rv;
        bool rc;

        NextAreaId = 0;
        Header = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrAreaTypeT( AreaType );
        m.MarshalSaHpiEntryIdT( AreaId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_AREA_HEADER_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextAreaId = m.DemarshalSaHpiEntryIdT();
            Header = m.DemarshalSaHpiIdrAreaHeaderT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaAdd(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        out long AreaId
    )
    {
        long rv;
        bool rc;

        AreaId = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrAreaTypeT( AreaType );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_AREA_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            AreaId = m.DemarshalSaHpiEntryIdT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaAddById(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        long AreaId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrAreaTypeT( AreaType );
        m.MarshalSaHpiEntryIdT( AreaId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_AREA_ADD_BY_ID );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaDelete(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiEntryIdT( AreaId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_AREA_DELETE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId,
        long FieldType,
        long FieldId,
        out long NextFieldId,
        out SaHpiIdrFieldT Field
    )
    {
        long rv;
        bool rc;

        NextFieldId = 0;
        Field = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiEntryIdT( AreaId );
        m.MarshalSaHpiIdrFieldTypeT( FieldType );
        m.MarshalSaHpiEntryIdT( FieldId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_FIELD_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextFieldId = m.DemarshalSaHpiEntryIdT();
            Field = m.DemarshalSaHpiIdrFieldT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldAdd(
        long SessionId,
        long ResourceId,
        long IdrId,
        ref SaHpiIdrFieldT Field
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Field );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrFieldT( Field );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_FIELD_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Field = m.DemarshalSaHpiIdrFieldT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldAddById(
        long SessionId,
        long ResourceId,
        long IdrId,
        SaHpiIdrFieldT Field
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Field );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrFieldT( Field );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_FIELD_ADD_BY_ID );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldSet(
        long SessionId,
        long ResourceId,
        long IdrId,
        SaHpiIdrFieldT Field
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Field );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiIdrFieldT( Field );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_FIELD_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldDelete(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId,
        long FieldId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiIdrIdT( IdrId );
        m.MarshalSaHpiEntryIdT( AreaId );
        m.MarshalSaHpiEntryIdT( FieldId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_IDR_FIELD_DELETE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerGet(
        long SessionId,
        long ResourceId,
        long WatchdogNum,
        out SaHpiWatchdogT Watchdog
    )
    {
        long rv;
        bool rc;

        Watchdog = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiWatchdogNumT( WatchdogNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_WATCHDOG_TIMER_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Watchdog = m.DemarshalSaHpiWatchdogT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerSet(
        long SessionId,
        long ResourceId,
        long WatchdogNum,
        SaHpiWatchdogT Watchdog
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Watchdog );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiWatchdogNumT( WatchdogNum );
        m.MarshalSaHpiWatchdogT( Watchdog );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_WATCHDOG_TIMER_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerReset(
        long SessionId,
        long ResourceId,
        long WatchdogNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiWatchdogNumT( WatchdogNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_WATCHDOG_TIMER_RESET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorGetNext(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long Severity,
        long UnacknowledgedOnly,
        ref SaHpiAnnouncementT Announcement
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Announcement );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiSeverityT( Severity );
        m.MarshalSaHpiBoolT( UnacknowledgedOnly );
        m.MarshalSaHpiAnnouncementT( Announcement );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_GET_NEXT );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Announcement = m.DemarshalSaHpiAnnouncementT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorGet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        out SaHpiAnnouncementT Announcement
    )
    {
        long rv;
        bool rc;

        Announcement = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiEntryIdT( EntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Announcement = m.DemarshalSaHpiAnnouncementT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorAcknowledge(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        long Severity
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiEntryIdT( EntryId );
        m.MarshalSaHpiSeverityT( Severity );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_ACKNOWLEDGE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorAdd(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        ref SaHpiAnnouncementT Announcement
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( Announcement );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiAnnouncementT( Announcement );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_ADD );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Announcement = m.DemarshalSaHpiAnnouncementT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorDelete(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        long Severity
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiEntryIdT( EntryId );
        m.MarshalSaHpiSeverityT( Severity );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_DELETE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorModeGet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        out long Mode
    )
    {
        long rv;
        bool rc;

        Mode = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_MODE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Mode = m.DemarshalSaHpiAnnunciatorModeT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorModeSet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long Mode
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.MarshalSaHpiAnnunciatorModeT( Mode );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_ANNUNCIATOR_MODE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiInfoGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        out SaHpiDimiInfoT DimiInfo
    )
    {
        long rv;
        bool rc;

        DimiInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            DimiInfo = m.DemarshalSaHpiDimiInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestInfoGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        out SaHpiDimiTestT DimiTest
    )
    {
        long rv;
        bool rc;

        DimiTest = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            DimiTest = m.DemarshalSaHpiDimiTestT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestReadinessGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        out long DimiReady
    )
    {
        long rv;
        bool rc;

        DimiReady = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_READINESS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            DimiReady = m.DemarshalSaHpiDimiReadyT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestCancel(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_CANCEL );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestStatusGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        out long PercentCompleted,
        out long RunStatus
    )
    {
        long rv;
        bool rc;

        PercentCompleted = 0;
        RunStatus = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_STATUS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            PercentCompleted = m.DemarshalSaHpiDimiTestPercentCompletedT();
            RunStatus = m.DemarshalSaHpiDimiTestRunStatusT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestResultsGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        out SaHpiDimiTestResultsT TestResults
    )
    {
        long rv;
        bool rc;

        TestResults = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_RESULTS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            TestResults = m.DemarshalSaHpiDimiTestResultsT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiSpecInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        out SaHpiFumiSpecInfoT SpecInfo
    )
    {
        long rv;
        bool rc;

        SpecInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SPEC_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SpecInfo = m.DemarshalSaHpiFumiSpecInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiServiceImpactGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        out SaHpiFumiServiceImpactDataT ServiceImpact
    )
    {
        long rv;
        bool rc;

        ServiceImpact = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SERVICE_IMPACT_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            ServiceImpact = m.DemarshalSaHpiFumiServiceImpactDataT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        SaHpiTextBufferT SourceUri
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( SourceUri );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        m.MarshalSaHpiTextBufferT( SourceUri );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SOURCE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceInfoValidateStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SOURCE_INFO_VALIDATE_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        out SaHpiFumiSourceInfoT SourceInfo
    )
    {
        long rv;
        bool rc;

        SourceInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SOURCE_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SourceInfo = m.DemarshalSaHpiFumiSourceInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long ComponentEntryId,
        out long NextComponentEntryId,
        out SaHpiFumiComponentInfoT ComponentInfo
    )
    {
        long rv;
        bool rc;

        NextComponentEntryId = 0;
        ComponentInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        m.MarshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_SOURCE_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextComponentEntryId = m.DemarshalSaHpiEntryIdT();
            ComponentInfo = m.DemarshalSaHpiFumiComponentInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        out SaHpiFumiBankInfoT BankInfo
    )
    {
        long rv;
        bool rc;

        BankInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_TARGET_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            BankInfo = m.DemarshalSaHpiFumiBankInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long ComponentEntryId,
        out long NextComponentEntryId,
        out SaHpiFumiComponentInfoT ComponentInfo
    )
    {
        long rv;
        bool rc;

        NextComponentEntryId = 0;
        ComponentInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        m.MarshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_TARGET_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextComponentEntryId = m.DemarshalSaHpiEntryIdT();
            ComponentInfo = m.DemarshalSaHpiFumiComponentInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiLogicalTargetInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        out SaHpiFumiLogicalBankInfoT BankInfo
    )
    {
        long rv;
        bool rc;

        BankInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_LOGICAL_TARGET_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            BankInfo = m.DemarshalSaHpiFumiLogicalBankInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiLogicalTargetComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long ComponentEntryId,
        out long NextComponentEntryId,
        out SaHpiFumiLogicalComponentInfoT ComponentInfo
    )
    {
        long rv;
        bool rc;

        NextComponentEntryId = 0;
        ComponentInfo = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_LOGICAL_TARGET_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextComponentEntryId = m.DemarshalSaHpiEntryIdT();
            ComponentInfo = m.DemarshalSaHpiFumiLogicalComponentInfoT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiBackupStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_BACKUP_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiBankBootOrderSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long Position
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        m.MarshalSaHpiUint32T( Position );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_BANK_BOOT_ORDER_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiBankCopyStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long SourceBankNum,
        long TargetBankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( SourceBankNum );
        m.MarshalSaHpiBankNumT( TargetBankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_BANK_COPY_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiInstallStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_INSTALL_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiUpgradeStatusGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        out long UpgradeStatus
    )
    {
        long rv;
        bool rc;

        UpgradeStatus = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_UPGRADE_STATUS_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            UpgradeStatus = m.DemarshalSaHpiFumiUpgradeStatusT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetVerifyStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_TARGET_VERIFY_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetVerifyMainStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_TARGET_VERIFY_MAIN_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiUpgradeCancel(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_UPGRADE_CANCEL );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiAutoRollbackDisableGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        out long Disable
    )
    {
        long rv;
        bool rc;

        Disable = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Disable = m.DemarshalSaHpiBoolT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiAutoRollbackDisableSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long Disable
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBoolT( Disable );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiRollbackStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_ROLLBACK_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiActivateStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long Logical
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBoolT( Logical );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_ACTIVATE_START );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiFumiCleanup(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiFumiNumT( FumiNum );
        m.MarshalSaHpiBankNumT( BankNum );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_FUMI_CLEANUP );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapPolicyCancel(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_HOTSWAP_POLICY_CANCEL );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceActiveSet(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_ACTIVE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceInactiveSet(
        long SessionId,
        long ResourceId
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_INACTIVE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAutoInsertTimeoutGet(
        long SessionId,
        out long Timeout
    )
    {
        long rv;
        bool rc;

        Timeout = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_AUTO_INSERT_TIMEOUT_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Timeout = m.DemarshalSaHpiTimeoutT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAutoInsertTimeoutSet(
        long SessionId,
        long Timeout
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiTimeoutT( Timeout );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_AUTO_INSERT_TIMEOUT_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAutoExtractTimeoutGet(
        long SessionId,
        long ResourceId,
        out long Timeout
    )
    {
        long rv;
        bool rc;

        Timeout = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            Timeout = m.DemarshalSaHpiTimeoutT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiAutoExtractTimeoutSet(
        long SessionId,
        long ResourceId,
        long Timeout
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiTimeoutT( Timeout );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapStateGet(
        long SessionId,
        long ResourceId,
        out long State
    )
    {
        long rv;
        bool rc;

        State = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_HOTSWAP_STATE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            State = m.DemarshalSaHpiHsStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapActionRequest(
        long SessionId,
        long ResourceId,
        long Action
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiHsActionT( Action );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_HOTSWAP_ACTION_REQUEST );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapIndicatorStateGet(
        long SessionId,
        long ResourceId,
        out long State
    )
    {
        long rv;
        bool rc;

        State = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_HOTSWAP_INDICATOR_STATE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            State = m.DemarshalSaHpiHsIndicatorStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapIndicatorStateSet(
        long SessionId,
        long ResourceId,
        long State
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiHsIndicatorStateT( State );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_HOTSWAP_INDICATOR_STATE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiParmControl(
        long SessionId,
        long ResourceId,
        long Action
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiParmActionT( Action );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_PARM_CONTROL );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceLoadIdGet(
        long SessionId,
        long ResourceId,
        out SaHpiLoadIdT LoadId
    )
    {
        long rv;
        bool rc;

        LoadId = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_LOADID_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            LoadId = m.DemarshalSaHpiLoadIdT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceLoadIdSet(
        long SessionId,
        long ResourceId,
        SaHpiLoadIdT LoadId
    )
    {
        long rv;
        bool rc;
        rc = HpiUtil.Check( LoadId );
        if ( !rc ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiLoadIdT( LoadId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_LOADID_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceResetStateGet(
        long SessionId,
        long ResourceId,
        out long ResetAction
    )
    {
        long rv;
        bool rc;

        ResetAction = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_RESET_STATE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            ResetAction = m.DemarshalSaHpiResetActionT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourceResetStateSet(
        long SessionId,
        long ResourceId,
        long ResetAction
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiResetActionT( ResetAction );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_RESET_STATE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourcePowerStateGet(
        long SessionId,
        long ResourceId,
        out long State
    )
    {
        long rv;
        bool rc;

        State = 0;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_POWER_STATE_GET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            State = m.DemarshalSaHpiPowerStateT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long saHpiResourcePowerStateSet(
        long SessionId,
        long ResourceId,
        long State
    )
    {
        long rv;
        bool rc;
        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshalSaHpiResourceIdT( ResourceId );
        m.MarshalSaHpiPowerStateT( State );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_RESOURCE_POWER_STATE_SET );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );

        return rv;
    }

};


}; // namespace openhpi
}; // namespace org

