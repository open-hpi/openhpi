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
 * HPI API
 * Specific functions that were hard to auto-generate.
 * So they were written manually.
 *********************************************************/
public class Hpi extends HpiGen
{
    // Just to ensure nobody creates it
    protected Hpi()
    {
        // empty
    }


    public static long saHpiSessionOpen(
        long DomainId,
        Object SecurityParams,
        saHpiSessionOpenOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        long rv;
        boolean rc;

        if ( SecurityParams != null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.createSession( DomainId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_DOMAIN;
        }
        OhpiMarshal m = s.getMarshal();
        if ( m == null ) {
            HpiCore.removeSession( s );
            return SA_ERR_HPI_NO_RESPONSE;
        }

        m.marshalSaHpiDomainIdT( s.getRemoteDid() );
        rc = m.interchange( RPC_SAHPI_SESSION_OPEN );
        if ( !rc ) {
            m.close();
            HpiCore.removeSession( s );
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            out.SessionId = m.demarshalSaHpiSessionIdT();
            s.setRemoteSid( out.SessionId );
            out.SessionId = s.getLocalSid();
        }
        s.putMarshal( m );
        if ( rv != SA_OK ) {
            HpiCore.removeSession( s );
        }

        return rv;
    }

    public static long saHpiSessionClose(
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
        rc = m.interchange( RPC_SAHPI_SESSION_CLOSE );
        if ( !rc ) {
            m.close();
            return SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.demarshalSaErrorT();
        if ( rv == SA_OK ) {
            // No output arguments
        }
        s.putMarshal( m );
        if ( rv == SA_OK ) {
            HpiCore.removeSession( s );
        }

        return rv;
    }

    public static long saHpiMyEntityPathGet(
        long SessionId,
        saHpiMyEntityPathGetOutputParamsT out
    ) throws HpiException
    {
        if ( out == null ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.getSession( SessionId );
        if ( s == null ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }

        out.EntityPath = HpiCore.getMyEntity();
        if ( out.EntityPath == null ) {
            return SA_ERR_HPI_UNKNOWN;
        }

        return SA_OK;
    }

    public static long saHpiDimiTestStart(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        long NumberOfParams,
        SaHpiDimiTestVariableParamsT[] ParamsList
    ) throws HpiException
    {
        long rv;
        boolean rc;

        if ( NumberOfParams != 0 ) {
            if ( ParamsList == null ) {
                return SA_ERR_HPI_INVALID_PARAMS;
            }
            if ( NumberOfParams > ParamsList.length ) {
                return SA_ERR_HPI_INVALID_PARAMS;
            }
            for ( int i = 0; i < NumberOfParams; ++i ) {
                rc = HpiUtil.check( ParamsList[i] );
                if ( !rc ) {
                    return SA_ERR_HPI_INVALID_PARAMS;
                }
            }
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
        m.marshalSaHpiDimiNumT( DimiNum );
        m.marshalSaHpiDimiTestNumT( TestNum );
        m.marshalSaHpiUint8T( NumberOfParams );
        for ( int i = 0; i < NumberOfParams; ++i ) {
            m.marshalSaHpiDimiTestVariableParamsT( ParamsList[i] );
        }
        rc = m.interchange( RPC_SAHPI_DIMI_TEST_START );
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

