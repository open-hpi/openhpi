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

import java.io.InputStream;
import java.io.IOException;
import java.util.jar.Attributes;
import java.util.jar.Manifest;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.OhpiDataTypes.*;


/**********************************************************
 * OHPI API (NB: Partly implemented)
 *********************************************************/
// TODO Implement the rest of OHPI API
public class Ohpi
{
    // Just to ensure nobody creates it
    protected Ohpi()
    {
        // empty
    }


    public static long oHpiVersionGet()
    {
        long vmajor = 0L;
        long vminor = 0L;
        long vaux   = 0L;

        String sversion = null;
        try {
            // Trying to get version from jar manifest
            InputStream s = Hpi.class.getResourceAsStream( "/META-INF/MANIFEST.MF" );
            if ( s != null ) {
                Manifest m = new Manifest( s );
                Attributes a = m.getAttributes( "org.openhpi" );
                if ( a != null ) {
                    sversion = a.getValue( Attributes.Name.IMPLEMENTATION_VERSION );
                }
            }
        } catch ( IOException e ) {
            // do nothing
        }

        if ( sversion != null ) {
            String[] parts = sversion.split( "\\." );
            if ( parts.length >= 0 ) {
                vmajor = Long.parseLong( parts[0] );
            }
            if ( parts.length >= 1 ) {
                vminor = Long.parseLong( parts[1] );
            }
            if ( parts.length >= 2 ) {
                vaux = Long.parseLong( parts[2] );
            }
        }

        return ( vmajor << 48 ) | ( vminor << 32 ) | ( vaux << 16 );
    }

    public static long oHpiDomainAdd(
        SaHpiTextBufferT Host,
        int Port,
        SaHpiEntityPathT EntityRoot,
        oHpiDomainAddOutputParamsT out
    )
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        String s = HpiUtil.fromSaHpiTextBufferT( Host );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiDomain d = HpiCore.createDomain( s, Port, EntityRoot );
        if ( d == null ) {
            return SA_ERR_HPI_INTERNAL_ERROR;
        }

        out.DomainId = d.getLocalDid();

        return SA_OK;
    }

    public static long oHpiHandlerCreate(
        long SessionId,
        oHpiHandlerConfigT HandlerConfig,
        oHpiHandlerCreateOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        rc = OhpiUtil.check( HandlerConfig );
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
        m.marshaloHpiHandlerConfigT( HandlerConfig );
        rc = m.interchange( RPC_OHPI_HANDLER_CREATE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.HandlerId = m.demarshaloHpiHandlerIdT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long oHpiHandlerDestroy(
        long SessionId,
        long HandlerId
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
        m.marshaloHpiHandlerIdT( HandlerId );
        rc = m.interchange( RPC_OHPI_HANDLER_DESTROY );
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

    public static long oHpiHandlerInfo(
        long SessionId,
        long HandlerId,
        oHpiHandlerInfoOutputParamsT out
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
        m.marshaloHpiHandlerIdT( HandlerId );
        rc = m.interchange( RPC_OHPI_HANDLER_INFO );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.HandlerInfo = m.demarshaloHpiHandlerInfoT();
            out.HandlerConfig = m.demarshaloHpiHandlerConfigT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long oHpiHandlerGetNext(
        long SessionId,
        long HandlerId,
        oHpiHandlerGetNextOutputParamsT out
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
        m.marshaloHpiHandlerIdT( HandlerId );
        rc = m.interchange( RPC_OHPI_HANDLER_GET_NEXT );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.NextHandlerId = m.demarshaloHpiHandlerIdT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long oHpiHandlerFind(
        long SessionId,
        long ResourceId,
        oHpiHandlerFindOutputParamsT out
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
        rc = m.interchange( RPC_OHPI_HANDLER_FIND );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.HandlerId = m.demarshaloHpiHandlerIdT();
        }
        s.putMarshal( m );

        return rv;
    }

    public static long oHpiHandlerRetry(
        long SessionId,
        long HandlerId
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
        m.marshaloHpiHandlerIdT( HandlerId );
        rc = m.interchange( RPC_OHPI_HANDLER_RETRY );
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

