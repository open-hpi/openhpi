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
using System.Reflection;


namespace org {
namespace openhpi {


/**********************************************************
 * OHPI API (NB: Partly implemented)
 *********************************************************/
// TODO Implement the rest of OHPI API
public static partial class Api
{
    public static long oHpiVersionGet()
    {
        Version v = Assembly.GetExecutingAssembly().GetName().Version;
        long vmajor = unchecked( (ushort)( v.Major & 0xFFFF ) );
        long vminor = unchecked( (ushort)( v.Minor & 0xFFFF ) );
        long vaux   = unchecked( (ushort)( v.Build & 0xFFFF ) );

        return ( vmajor << 48 ) | ( vminor << 32 ) | ( vaux << 16 );
    }

    public static long oHpiDomainAdd(
        SaHpiTextBufferT Host,
        int Port,
        SaHpiEntityPathT EntityRoot,
        out long DomainId
    )
    {
        DomainId = 0;

        string s = HpiUtil.FromSaHpiTextBufferT( Host );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiDomain d = HpiCore.CreateDomain( s, Port, EntityRoot );
        if ( d == null ) {
            return HpiConst.SA_ERR_HPI_INTERNAL_ERROR;
        }

        DomainId = d.GetLocalDid();

        return HpiConst.SA_OK;
    }

    public static long oHpiHandlerCreate(
        long SessionId,
        oHpiHandlerConfigT HandlerConfig,
        out long HandlerId
    )
    {
        long rv;
        bool rc;

        HandlerId = HpiConst.SAHPI_LAST_ENTRY;

        rc = OhpiUtil.Check( HandlerConfig );
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
        m.MarshaloHpiHandlerConfigT( HandlerConfig );
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_CREATE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            HandlerId = m.DemarshaloHpiHandlerIdT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long oHpiHandlerDestroy(
        long SessionId,
        long HandlerId
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
        m.MarshaloHpiHandlerIdT( HandlerId );
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_DESTROY );
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

    public static long oHpiHandlerInfo(
        long SessionId,
        long HandlerId,
        out oHpiHandlerInfoT HandlerInfo,
        out oHpiHandlerConfigT HandlerConfig
    )
    {
        long rv;
        bool rc;

        HandlerInfo = null;
        HandlerConfig = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshaloHpiHandlerIdT( HandlerId );
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_INFO );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            HandlerInfo = m.DemarshaloHpiHandlerInfoT();
            HandlerConfig = m.DemarshaloHpiHandlerConfigT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long oHpiHandlerGetNext(
        long SessionId,
        long HandlerId,
        out long NextHandlerId
    )
    {
        long rv;
        bool rc;

        NextHandlerId = HpiConst.SAHPI_LAST_ENTRY;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiSessionIdT( s.GetRemoteSid() );
        m.MarshaloHpiHandlerIdT( HandlerId );
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_GET_NEXT );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            NextHandlerId = m.DemarshaloHpiHandlerIdT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long oHpiHandlerFind(
        long SessionId,
        long ResourceId,
        out long HandlerId
    )
    {
        long rv;
        bool rc;

        HandlerId = HpiConst.SAHPI_LAST_ENTRY;

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
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_FIND );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            HandlerId = m.DemarshaloHpiHandlerIdT();
        }
        s.PutMarshal( m );

        return rv;
    }

    public static long oHpiHandlerRetry(
        long SessionId,
        long HandlerId
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
        m.MarshaloHpiHandlerIdT( HandlerId );
        rc = m.Interchange( OhpiConst.RPC_OHPI_HANDLER_RETRY );
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

