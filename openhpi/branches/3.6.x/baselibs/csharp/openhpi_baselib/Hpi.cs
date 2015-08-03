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
 * HPI API
 * Specific functions that were hard to auto-generate.
 * So they were written manually.
 *********************************************************/
public static partial class Api
{
    public static long saHpiSessionOpen(
        long DomainId,
        out long SessionId,
        Object SecurityParams
    )
    {
        long rv;
        bool rc;

        SessionId = 0;

        if ( SecurityParams != null ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }

        HpiSession s = HpiCore.CreateSession( DomainId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_DOMAIN;
        }
        OhpiMarshal m = s.GetMarshal();
        if ( m == null ) {
            HpiCore.RemoveSession( s );
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }

        m.MarshalSaHpiDomainIdT( s.GetRemoteDid() );
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SESSION_OPEN );
        if ( !rc ) {
            m.Close();
            HpiCore.RemoveSession( s );
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            SessionId = m.DemarshalSaHpiSessionIdT();
            s.SetRemoteSid( SessionId );
            SessionId = s.GetLocalSid();
        }
        s.PutMarshal( m );
        if ( rv != HpiConst.SA_OK ) {
            HpiCore.RemoveSession( s );
        }

        return rv;
    }

    public static long saHpiSessionClose(
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
        rc = m.Interchange( OhpiConst.RPC_SAHPI_SESSION_CLOSE );
        if ( !rc ) {
            m.Close();
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        rv = m.DemarshalSaErrorT();
        if ( rv == HpiConst.SA_OK ) {
            // No output arguments
        }
        s.PutMarshal( m );
        if ( rv == HpiConst.SA_OK ) {
            HpiCore.RemoveSession( s );
        }

        return rv;
    }

    public static long saHpiMyEntityPathGet(
        long SessionId,
        out SaHpiEntityPathT EntityPath
    )
    {
        EntityPath = null;

        HpiSession s = HpiCore.GetSession( SessionId );
        if ( s == null ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        EntityPath = HpiCore.GetMyEntity();
        if ( EntityPath == null ) {
            return HpiConst.SA_ERR_HPI_UNKNOWN;
        }

        return HpiConst.SA_OK;
    }

    public static long saHpiDimiTestStart(
        long SessionId,
        long ResourceId,
        long DimiNum,
        long TestNum,
        long NumberOfParams,
        SaHpiDimiTestVariableParamsT[] ParamsList
    )
    {
        long rv;
        bool rc;
        if ( NumberOfParams != 0 ) {
            if ( ParamsList == null ) {
                return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
            }
            if ( NumberOfParams > ParamsList.Length ) {
                return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
            }
            for ( int i = 0; i < NumberOfParams; ++i ) {
                rc = HpiUtil.Check( ParamsList[i] );
                if ( !rc ) {
                    return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
                }
            }
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
        m.MarshalSaHpiDimiNumT( DimiNum );
        m.MarshalSaHpiDimiTestNumT( TestNum );
        m.MarshalSaHpiUint8T( NumberOfParams );
        for ( int i = 0; i < NumberOfParams; ++i ) {
            m.MarshalSaHpiDimiTestVariableParamsT( ParamsList[i] );
        }
        rc = m.Interchange( OhpiConst.RPC_SAHPI_DIMI_TEST_START );
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

