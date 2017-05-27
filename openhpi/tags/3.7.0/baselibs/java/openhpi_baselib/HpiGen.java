/*      -*- java -*-
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

package org.openhpi;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;


/**********************************************************
 * HPI API (auto-generated)
 *********************************************************/
public class HpiGen
{
    // Just to ensure nobody creates it
    protected HpiGen()
    {
        // empty
    }

    /**********************************************************
     * HPI API
     *********************************************************/

    public static long saHpiVersionGet()
    {
        return SAHPI_INTERFACE_VERSION;
    }

    public static long saHpiDiscover(
        long SessionId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        rc = m.interchange( RPC_SAHPI_DISCOVER );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDomainInfoGet(
        long SessionId,
        saHpiDomainInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        rc = m.interchange( RPC_SAHPI_DOMAIN_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.DomainInfo = m.demarshalSaHpiDomainInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDrtEntryGet(
        long SessionId,
        long EntryId,
        saHpiDrtEntryGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiEntryIdT( EntryId );
        rc = m.interchange( RPC_SAHPI_DRT_ENTRY_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextEntryId = m.demarshalSaHpiEntryIdT();
            out.DrtEntry = m.demarshalSaHpiDrtEntryT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDomainTagSet(
        long SessionId,
        SaHpiTextBufferT DomainTag
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( DomainTag );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiTextBufferT( DomainTag );
        rc = m.interchange( RPC_SAHPI_DOMAIN_TAG_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiRptEntryGet(
        long SessionId,
        long EntryId,
        saHpiRptEntryGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiEntryIdT( EntryId );
        rc = m.interchange( RPC_SAHPI_RPT_ENTRY_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextEntryId = m.demarshalSaHpiEntryIdT();
            out.RptEntry = m.demarshalSaHpiRptEntryT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiRptEntryGetByResourceId(
        long SessionId,
        long ResourceId,
        saHpiRptEntryGetByResourceIdOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RPT_ENTRY_GET_BY_RESOURCE_ID );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.RptEntry = m.demarshalSaHpiRptEntryT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceSeveritySet(
        long SessionId,
        long ResourceId,
        long Severity
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSeverityT( Severity );
        rc = m.interchange( RPC_SAHPI_RESOURCE_SEVERITY_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceTagSet(
        long SessionId,
        long ResourceId,
        SaHpiTextBufferT ResourceTag
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( ResourceTag );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiTextBufferT( ResourceTag );
        rc = m.interchange( RPC_SAHPI_RESOURCE_TAG_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiGetIdByEntityPath(
        long SessionId,
        SaHpiEntityPathT EntityPath,
        long InstrumentType,
        long InstanceId,
        saHpiGetIdByEntityPathOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( EntityPath );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiEntityPathT( EntityPath );
        m.marshalSaHpiRdrTypeT( InstrumentType );
        m.marshalSaHpiUint32T( InstanceId );
        rc = m.interchange( RPC_SAHPI_GET_ID_BY_ENTITY_PATH );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.InstanceId = m.demarshalSaHpiUint32T();
            out.ResourceId = m.demarshalSaHpiResourceIdT();
            out.InstrumentId = m.demarshalSaHpiInstrumentIdT();
            out.RptUpdateCount = m.demarshalSaHpiUint32T();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiGetChildEntityPath(
        long SessionId,
        SaHpiEntityPathT ParentEntityPath,
        long InstanceId,
        saHpiGetChildEntityPathOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( ParentEntityPath );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiEntityPathT( ParentEntityPath );
        m.marshalSaHpiUint32T( InstanceId );
        rc = m.interchange( RPC_SAHPI_GET_CHILD_ENTITY_PATH );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.InstanceId = m.demarshalSaHpiUint32T();
            out.ChildEntityPath = m.demarshalSaHpiEntityPathT();
            out.RptUpdateCount = m.demarshalSaHpiUint32T();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceFailedRemove(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_FAILED_REMOVE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogInfoGet(
        long SessionId,
        long ResourceId,
        saHpiEventLogInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Info = m.demarshalSaHpiEventLogInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogCapabilitiesGet(
        long SessionId,
        long ResourceId,
        saHpiEventLogCapabilitiesGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_CAPABILITIES_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.EventLogCapabilities = m.demarshalSaHpiEventLogCapabilitiesT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogEntryGet(
        long SessionId,
        long ResourceId,
        long EntryId,
        saHpiEventLogEntryGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiEventLogEntryIdT( EntryId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_ENTRY_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.PrevEntryId = m.demarshalSaHpiEventLogEntryIdT();
            out.NextEntryId = m.demarshalSaHpiEventLogEntryIdT();
            out.EventLogEntry = m.demarshalSaHpiEventLogEntryT();
            out.Rdr = m.demarshalSaHpiRdrT();
            out.RptEntry = m.demarshalSaHpiRptEntryT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogEntryAdd(
        long SessionId,
        long ResourceId,
        SaHpiEventT EvtEntry
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( EvtEntry );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiEventT( EvtEntry );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_ENTRY_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogClear(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_CLEAR );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogTimeGet(
        long SessionId,
        long ResourceId,
        saHpiEventLogTimeGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_TIME_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Time = m.demarshalSaHpiTimeT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogTimeSet(
        long SessionId,
        long ResourceId,
        long Time
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiTimeT( Time );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_TIME_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogStateGet(
        long SessionId,
        long ResourceId,
        saHpiEventLogStateGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_STATE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.EnableState = m.demarshalSaHpiBoolT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogStateSet(
        long SessionId,
        long ResourceId,
        long EnableState
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiBoolT( EnableState );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_STATE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventLogOverflowReset(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_EVENT_LOG_OVERFLOW_RESET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSubscribe(
        long SessionId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        rc = m.interchange( RPC_SAHPI_SUBSCRIBE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiUnsubscribe(
        long SessionId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        rc = m.interchange( RPC_SAHPI_UNSUBSCRIBE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventGet(
        long SessionId,
        long Timeout,
        saHpiEventGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiTimeoutT( Timeout );
        rc = m.interchange( RPC_SAHPI_EVENT_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Event = m.demarshalSaHpiEventT();
            out.Rdr = m.demarshalSaHpiRdrT();
            out.RptEntry = m.demarshalSaHpiRptEntryT();
            out.EventQueueStatus = m.demarshalSaHpiEvtQueueStatusT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiEventAdd(
        long SessionId,
        SaHpiEventT EvtEntry
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( EvtEntry );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiEventT( EvtEntry );
        rc = m.interchange( RPC_SAHPI_EVENT_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAlarmGetNext(
        long SessionId,
        long Severity,
        long UnacknowledgedOnly,
        SaHpiAlarmT Alarm,
        saHpiAlarmGetNextOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( Alarm );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiSeverityT( Severity );
        m.marshalSaHpiBoolT( UnacknowledgedOnly );
        m.marshalSaHpiAlarmT( Alarm );
        rc = m.interchange( RPC_SAHPI_ALARM_GET_NEXT );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Alarm = m.demarshalSaHpiAlarmT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAlarmGet(
        long SessionId,
        long AlarmId,
        saHpiAlarmGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiAlarmIdT( AlarmId );
        rc = m.interchange( RPC_SAHPI_ALARM_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Alarm = m.demarshalSaHpiAlarmT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAlarmAcknowledge(
        long SessionId,
        long AlarmId,
        long Severity
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiAlarmIdT( AlarmId );
        m.marshalSaHpiSeverityT( Severity );
        rc = m.interchange( RPC_SAHPI_ALARM_ACKNOWLEDGE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAlarmAdd(
        long SessionId,
        SaHpiAlarmT Alarm,
        saHpiAlarmAddOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( Alarm );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiAlarmT( Alarm );
        rc = m.interchange( RPC_SAHPI_ALARM_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Alarm = m.demarshalSaHpiAlarmT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAlarmDelete(
        long SessionId,
        long AlarmId,
        long Severity
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiAlarmIdT( AlarmId );
        m.marshalSaHpiSeverityT( Severity );
        rc = m.interchange( RPC_SAHPI_ALARM_DELETE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiRdrGet(
        long SessionId,
        long ResourceId,
        long EntryId,
        saHpiRdrGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiEntryIdT( EntryId );
        rc = m.interchange( RPC_SAHPI_RDR_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextEntryId = m.demarshalSaHpiEntryIdT();
            out.Rdr = m.demarshalSaHpiRdrT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiRdrGetByInstrumentId(
        long SessionId,
        long ResourceId,
        long RdrType,
        long InstrumentId,
        saHpiRdrGetByInstrumentIdOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiRdrTypeT( RdrType );
        m.marshalSaHpiInstrumentIdT( InstrumentId );
        rc = m.interchange( RPC_SAHPI_RDR_GET_BY_INSTRUMENT_ID );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Rdr = m.demarshalSaHpiRdrT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiRdrUpdateCountGet(
        long SessionId,
        long ResourceId,
        saHpiRdrUpdateCountGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RDR_UPDATE_COUNT_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.UpdateCount = m.demarshalSaHpiUint32T();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorReadingGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorReadingGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_READING_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Reading = m.demarshalSaHpiSensorReadingT();
            out.EventState = m.demarshalSaHpiEventStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorThresholdsGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorThresholdsGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_THRESHOLDS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SensorThresholds = m.demarshalSaHpiSensorThresholdsT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorThresholdsSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        SaHpiSensorThresholdsT SensorThresholds
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( SensorThresholds );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        m.marshalSaHpiSensorThresholdsT( SensorThresholds );
        rc = m.interchange( RPC_SAHPI_SENSOR_THRESHOLDS_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorTypeGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorTypeGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_TYPE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Type = m.demarshalSaHpiSensorTypeT();
            out.Category = m.demarshalSaHpiEventCategoryT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEnableGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorEnableGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_ENABLE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SensorEnabled = m.demarshalSaHpiBoolT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEnableSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long SensorEnabled
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        m.marshalSaHpiBoolT( SensorEnabled );
        rc = m.interchange( RPC_SAHPI_SENSOR_ENABLE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventEnableGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorEventEnableGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_EVENT_ENABLE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SensorEventsEnabled = m.demarshalSaHpiBoolT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventEnableSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long SensorEventsEnabled
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        m.marshalSaHpiBoolT( SensorEventsEnabled );
        rc = m.interchange( RPC_SAHPI_SENSOR_EVENT_ENABLE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventMasksGet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        saHpiSensorEventMasksGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        rc = m.interchange( RPC_SAHPI_SENSOR_EVENT_MASKS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.AssertEventMask = m.demarshalSaHpiEventStateT();
            out.DeassertEventMask = m.demarshalSaHpiEventStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiSensorEventMasksSet(
        long SessionId,
        long ResourceId,
        long SensorNum,
        long Action,
        long AssertEventMask,
        long DeassertEventMask
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiSensorNumT( SensorNum );
        m.marshalSaHpiSensorEventMaskActionT( Action );
        m.marshalSaHpiEventStateT( AssertEventMask );
        m.marshalSaHpiEventStateT( DeassertEventMask );
        rc = m.interchange( RPC_SAHPI_SENSOR_EVENT_MASKS_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiControlTypeGet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        saHpiControlTypeGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiCtrlNumT( CtrlNum );
        rc = m.interchange( RPC_SAHPI_CONTROL_TYPE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Type = m.demarshalSaHpiCtrlTypeT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiControlGet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        SaHpiCtrlStateT CtrlState,
        saHpiControlGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( CtrlState );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiCtrlNumT( CtrlNum );
        m.marshalSaHpiCtrlStateT( CtrlState );
        rc = m.interchange( RPC_SAHPI_CONTROL_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.CtrlMode = m.demarshalSaHpiCtrlModeT();
            out.CtrlState = m.demarshalSaHpiCtrlStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiControlSet(
        long SessionId,
        long ResourceId,
        long CtrlNum,
        long CtrlMode,
        SaHpiCtrlStateT CtrlState
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( CtrlState );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiCtrlNumT( CtrlNum );
        m.marshalSaHpiCtrlModeT( CtrlMode );
        m.marshalSaHpiCtrlStateT( CtrlState );
        rc = m.interchange( RPC_SAHPI_CONTROL_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrInfoGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        saHpiIdrInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        rc = m.interchange( RPC_SAHPI_IDR_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.IdrInfo = m.demarshalSaHpiIdrInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaHeaderGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        long AreaId,
        saHpiIdrAreaHeaderGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrAreaTypeT( AreaType );
        m.marshalSaHpiEntryIdT( AreaId );
        rc = m.interchange( RPC_SAHPI_IDR_AREA_HEADER_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextAreaId = m.demarshalSaHpiEntryIdT();
            out.Header = m.demarshalSaHpiIdrAreaHeaderT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaAdd(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        saHpiIdrAreaAddOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrAreaTypeT( AreaType );
        rc = m.interchange( RPC_SAHPI_IDR_AREA_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.AreaId = m.demarshalSaHpiEntryIdT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaAddById(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaType,
        long AreaId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrAreaTypeT( AreaType );
        m.marshalSaHpiEntryIdT( AreaId );
        rc = m.interchange( RPC_SAHPI_IDR_AREA_ADD_BY_ID );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrAreaDelete(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiEntryIdT( AreaId );
        rc = m.interchange( RPC_SAHPI_IDR_AREA_DELETE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldGet(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId,
        long FieldType,
        long FieldId,
        saHpiIdrFieldGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiEntryIdT( AreaId );
        m.marshalSaHpiIdrFieldTypeT( FieldType );
        m.marshalSaHpiEntryIdT( FieldId );
        rc = m.interchange( RPC_SAHPI_IDR_FIELD_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextFieldId = m.demarshalSaHpiEntryIdT();
            out.Field = m.demarshalSaHpiIdrFieldT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldAdd(
        long SessionId,
        long ResourceId,
        long IdrId,
        SaHpiIdrFieldT Field,
        saHpiIdrFieldAddOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( Field );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrFieldT( Field );
        rc = m.interchange( RPC_SAHPI_IDR_FIELD_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Field = m.demarshalSaHpiIdrFieldT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldAddById(
        long SessionId,
        long ResourceId,
        long IdrId,
        SaHpiIdrFieldT Field
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( Field );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrFieldT( Field );
        rc = m.interchange( RPC_SAHPI_IDR_FIELD_ADD_BY_ID );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldSet(
        long SessionId,
        long ResourceId,
        long IdrId,
        SaHpiIdrFieldT Field
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( Field );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiIdrFieldT( Field );
        rc = m.interchange( RPC_SAHPI_IDR_FIELD_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiIdrFieldDelete(
        long SessionId,
        long ResourceId,
        long IdrId,
        long AreaId,
        long FieldId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiIdrIdT( IdrId );
        m.marshalSaHpiEntryIdT( AreaId );
        m.marshalSaHpiEntryIdT( FieldId );
        rc = m.interchange( RPC_SAHPI_IDR_FIELD_DELETE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerGet(
        long SessionId,
        long ResourceId,
        long WatchdogNum,
        saHpiWatchdogTimerGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiWatchdogNumT( WatchdogNum );
        rc = m.interchange( RPC_SAHPI_WATCHDOG_TIMER_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Watchdog = m.demarshalSaHpiWatchdogT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerSet(
        long SessionId,
        long ResourceId,
        long WatchdogNum,
        SaHpiWatchdogT Watchdog
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( Watchdog );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiWatchdogNumT( WatchdogNum );
        m.marshalSaHpiWatchdogT( Watchdog );
        rc = m.interchange( RPC_SAHPI_WATCHDOG_TIMER_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiWatchdogTimerReset(
        long SessionId,
        long ResourceId,
        long WatchdogNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiWatchdogNumT( WatchdogNum );
        rc = m.interchange( RPC_SAHPI_WATCHDOG_TIMER_RESET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorGetNext(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long Severity,
        long UnacknowledgedOnly,
        SaHpiAnnouncementT Announcement,
        saHpiAnnunciatorGetNextOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( Announcement );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiSeverityT( Severity );
        m.marshalSaHpiBoolT( UnacknowledgedOnly );
        m.marshalSaHpiAnnouncementT( Announcement );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_GET_NEXT );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Announcement = m.demarshalSaHpiAnnouncementT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorGet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        saHpiAnnunciatorGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiEntryIdT( EntryId );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Announcement = m.demarshalSaHpiAnnouncementT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorAcknowledge(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        long Severity
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiEntryIdT( EntryId );
        m.marshalSaHpiSeverityT( Severity );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_ACKNOWLEDGE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorAdd(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        SaHpiAnnouncementT Announcement,
        saHpiAnnunciatorAddOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = HpiUtil.check( Announcement );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiAnnouncementT( Announcement );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_ADD );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Announcement = m.demarshalSaHpiAnnouncementT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorDelete(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long EntryId,
        long Severity
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiEntryIdT( EntryId );
        m.marshalSaHpiSeverityT( Severity );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_DELETE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorModeGet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        saHpiAnnunciatorModeGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_MODE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Mode = m.demarshalSaHpiAnnunciatorModeT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAnnunciatorModeSet(
        long SessionId,
        long ResourceId,
        long AnnunciatorNum,
        long Mode
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiAnnunciatorNumT( AnnunciatorNum );
        m.marshalSaHpiAnnunciatorModeT( Mode );
        rc = m.interchange( RPC_SAHPI_ANNUNCIATOR_MODE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiInfoGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        saHpiDimiInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        rc = m.interchange( RPC_SAHPI_DIMI_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.DimiInfo = m.demarshalSaHpiDimiInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestInfoGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        saHpiDimiTestInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.DimiTest = m.demarshalSaHpiDimiTestT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestReadinessGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        saHpiDimiTestReadinessGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_READINESS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.DimiReady = m.demarshalSaHpiDimiReadyT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestCancel(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_CANCEL );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestStatusGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        saHpiDimiTestStatusGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_STATUS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.PercentCompleted = m.demarshalSaHpiDimiTestPercentCompletedT();
            out.RunStatus = m.demarshalSaHpiDimiTestRunStatusT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiDimiTestResultsGet(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        saHpiDimiTestResultsGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_RESULTS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.TestResults = m.demarshalSaHpiDimiTestResultsT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiSpecInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        saHpiFumiSpecInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_SPEC_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SpecInfo = m.demarshalSaHpiFumiSpecInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiServiceImpactGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        saHpiFumiServiceImpactGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_SERVICE_IMPACT_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.ServiceImpact = m.demarshalSaHpiFumiServiceImpactDataT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        SaHpiTextBufferT SourceUri
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( SourceUri );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        m.marshalSaHpiTextBufferT( SourceUri );
        rc = m.interchange( RPC_SAHPI_FUMI_SOURCE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceInfoValidateStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_SOURCE_INFO_VALIDATE_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        saHpiFumiSourceInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_SOURCE_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SourceInfo = m.demarshalSaHpiFumiSourceInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiSourceComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long ComponentEntryId,
        saHpiFumiSourceComponentInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        m.marshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.interchange( RPC_SAHPI_FUMI_SOURCE_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextComponentEntryId = m.demarshalSaHpiEntryIdT();
            out.ComponentInfo = m.demarshalSaHpiFumiComponentInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        saHpiFumiTargetInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_TARGET_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.BankInfo = m.demarshalSaHpiFumiBankInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long ComponentEntryId,
        saHpiFumiTargetComponentInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        m.marshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.interchange( RPC_SAHPI_FUMI_TARGET_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextComponentEntryId = m.demarshalSaHpiEntryIdT();
            out.ComponentInfo = m.demarshalSaHpiFumiComponentInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiLogicalTargetInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        saHpiFumiLogicalTargetInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_LOGICAL_TARGET_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.BankInfo = m.demarshalSaHpiFumiLogicalBankInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiLogicalTargetComponentInfoGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long ComponentEntryId,
        saHpiFumiLogicalTargetComponentInfoGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiEntryIdT( ComponentEntryId );
        rc = m.interchange( RPC_SAHPI_FUMI_LOGICAL_TARGET_COMPONENT_INFO_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextComponentEntryId = m.demarshalSaHpiEntryIdT();
            out.ComponentInfo = m.demarshalSaHpiFumiLogicalComponentInfoT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiBackupStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_BACKUP_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiBankBootOrderSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        long Position
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        m.marshalSaHpiUint32T( Position );
        rc = m.interchange( RPC_SAHPI_FUMI_BANK_BOOT_ORDER_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiBankCopyStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long SourceBankNum,
        long TargetBankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( SourceBankNum );
        m.marshalSaHpiBankNumT( TargetBankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_BANK_COPY_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiInstallStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_INSTALL_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiUpgradeStatusGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum,
        saHpiFumiUpgradeStatusGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_UPGRADE_STATUS_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.UpgradeStatus = m.demarshalSaHpiFumiUpgradeStatusT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetVerifyStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_TARGET_VERIFY_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiTargetVerifyMainStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_TARGET_VERIFY_MAIN_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiUpgradeCancel(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_UPGRADE_CANCEL );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiAutoRollbackDisableGet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        saHpiFumiAutoRollbackDisableGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Disable = m.demarshalSaHpiBoolT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiAutoRollbackDisableSet(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long Disable
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBoolT( Disable );
        rc = m.interchange( RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiRollbackStart(
        long SessionId,
        long ResourceId,
        long FumiNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        rc = m.interchange( RPC_SAHPI_FUMI_ROLLBACK_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiActivateStart(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long Logical
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBoolT( Logical );
        rc = m.interchange( RPC_SAHPI_FUMI_ACTIVATE_START );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiFumiCleanup(
        long SessionId,
        long ResourceId,
        long FumiNum,
        long BankNum
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiFumiNumT( FumiNum );
        m.marshalSaHpiBankNumT( BankNum );
        rc = m.interchange( RPC_SAHPI_FUMI_CLEANUP );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapPolicyCancel(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_HOTSWAP_POLICY_CANCEL );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceActiveSet(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_ACTIVE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceInactiveSet(
        long SessionId,
        long ResourceId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_INACTIVE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAutoInsertTimeoutGet(
        long SessionId,
        saHpiAutoInsertTimeoutGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        rc = m.interchange( RPC_SAHPI_AUTO_INSERT_TIMEOUT_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Timeout = m.demarshalSaHpiTimeoutT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAutoInsertTimeoutSet(
        long SessionId,
        long Timeout
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiTimeoutT( Timeout );
        rc = m.interchange( RPC_SAHPI_AUTO_INSERT_TIMEOUT_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAutoExtractTimeoutGet(
        long SessionId,
        long ResourceId,
        saHpiAutoExtractTimeoutGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.Timeout = m.demarshalSaHpiTimeoutT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiAutoExtractTimeoutSet(
        long SessionId,
        long ResourceId,
        long Timeout
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiTimeoutT( Timeout );
        rc = m.interchange( RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapStateGet(
        long SessionId,
        long ResourceId,
        saHpiHotSwapStateGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_HOTSWAP_STATE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.State = m.demarshalSaHpiHsStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapActionRequest(
        long SessionId,
        long ResourceId,
        long Action
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiHsActionT( Action );
        rc = m.interchange( RPC_SAHPI_HOTSWAP_ACTION_REQUEST );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapIndicatorStateGet(
        long SessionId,
        long ResourceId,
        saHpiHotSwapIndicatorStateGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_HOTSWAP_INDICATOR_STATE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.State = m.demarshalSaHpiHsIndicatorStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiHotSwapIndicatorStateSet(
        long SessionId,
        long ResourceId,
        long State
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiHsIndicatorStateT( State );
        rc = m.interchange( RPC_SAHPI_HOTSWAP_INDICATOR_STATE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiParmControl(
        long SessionId,
        long ResourceId,
        long Action
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiParmActionT( Action );
        rc = m.interchange( RPC_SAHPI_PARM_CONTROL );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceLoadIdGet(
        long SessionId,
        long ResourceId,
        saHpiResourceLoadIdGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_LOADID_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.LoadId = m.demarshalSaHpiLoadIdT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceLoadIdSet(
        long SessionId,
        long ResourceId,
        SaHpiLoadIdT LoadId
    ) throws HpiException
    {

        long rv;
        boolean rc;

        rc = HpiUtil.check( LoadId );
        if ( !rc ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiLoadIdT( LoadId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_LOADID_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceResetStateGet(
        long SessionId,
        long ResourceId,
        saHpiResourceResetStateGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_RESET_STATE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.ResetAction = m.demarshalSaHpiResetActionT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourceResetStateSet(
        long SessionId,
        long ResourceId,
        long ResetAction
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiResetActionT( ResetAction );
        rc = m.interchange( RPC_SAHPI_RESOURCE_RESET_STATE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourcePowerStateGet(
        long SessionId,
        long ResourceId,
        saHpiResourcePowerStateGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        rc = m.interchange( RPC_SAHPI_RESOURCE_POWER_STATE_GET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.State = m.demarshalSaHpiPowerStateT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long saHpiResourcePowerStateSet(
        long SessionId,
        long ResourceId,
        long State
    ) throws HpiException
    {

        long rv;
        boolean rc;

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiSessionIdT( s.getRemoteSid() );
        m.marshalSaHpiResourceIdT( ResourceId );
        m.marshalSaHpiPowerStateT( State );
        rc = m.interchange( RPC_SAHPI_RESOURCE_POWER_STATE_SET );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );

        return rv;
    }

};

