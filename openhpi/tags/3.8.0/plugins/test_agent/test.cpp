/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTAREALITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <string>

#include <oh_utils.h>

#include "codec.h"
#include "dimi.h"
#include "handler.h"
#include "structs.h"
#include "test.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiDimiTestT MakeDefaultTestInfo( SaHpiDimiTestNumT num )
{
    SaHpiDimiTestT info;

    FormatHpiTextBuffer( info.TestName, "test %u", (unsigned int)num );
    info.ServiceImpact = SAHPI_DIMITEST_NONDEGRADING;
    for ( size_t i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
        MakeUnspecifiedHpiEntityPath( info.EntitiesImpacted[i].EntityImpacted );
        info.EntitiesImpacted[i].ServiceImpact = SAHPI_DIMITEST_NONDEGRADING;
    }
    info.NeedServiceOS = SAHPI_FALSE;
    MakeHpiTextBuffer( info.ServiceOS, "Unspecified OS" );
    info.ExpectedRunDuration = 2000000000LL; // 2 sec
    info.TestCapabilities = SAHPI_DIMITEST_CAPABILITY_TESTCANCEL;

    for ( size_t i = 0; i < SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
        SaHpiDimiTestParamsDefinitionT& pd = info.TestParameters[i];
        memset( &pd.ParamName[0], 0, SAHPI_DIMITEST_PARAM_NAME_LEN );
        snprintf( reinterpret_cast<char *>(&pd.ParamName[0]),
                  SAHPI_DIMITEST_PARAM_NAME_LEN,
                  "Param %u",
                  (unsigned int)i );
        FormatHpiTextBuffer( pd.ParamInfo, "This is param %u", (unsigned int)i );
        pd.ParamType = SAHPI_DIMITEST_PARAM_TYPE_INT32;
        pd.MinValue.IntValue     = 0;
        pd.MaxValue.IntValue     = 255;
        pd.DefaultParam.paramint = i;
    }

    return info;
}

static SaHpiDimiTestResultsT MakeDefaultTestResults()
{
    SaHpiDimiTestResultsT results;

    results.ResultTimeStamp       = SAHPI_TIME_UNSPECIFIED;
    results.RunDuration           = SAHPI_TIMEOUT_IMMEDIATE;
    results.LastRunStatus         = SAHPI_DIMITEST_STATUS_NOT_RUN;
    results.TestErrorCode         = SAHPI_DIMITEST_STATUSERR_NOERR;
    MakeHpiTextBuffer( results.TestResultString, "http://openhpi.org" );
    results.TestResultStringIsURI = SAHPI_TRUE;

    return results;
}


/**************************************************************
 * class cTest
 *************************************************************/
const std::string cTest::classname( "test" );

cTest::cTest( cHandler& handler, cDimi& dimi, SaHpiDimiTestNumT num )
    : cObject( AssembleNumberedObjectName( classname, num ) ),
      m_handler( handler ),
      m_dimi( dimi ),
      m_num( num ),
      m_info( MakeDefaultTestInfo( num ) ),
      m_ready( SAHPI_DIMI_READY ),
      m_status( SAHPI_DIMITEST_STATUS_NOT_RUN ),
      m_progress( 0xff ),
      m_prev_results( MakeDefaultTestResults() ),
      m_start_timestamp( SAHPI_TIME_UNSPECIFIED )
{
    m_next.run_duration         = m_info.ExpectedRunDuration;
    m_next.err                  = SAHPI_DIMITEST_STATUSERR_NOERR;
    MakeHpiTextBuffer( m_next.result_string, "No error has been detected" );
    m_next.result_string_is_uri = SAHPI_FALSE;
}

cTest::~cTest()
{
    m_handler.CancelTimer( this );
}


// HPI Interface
SaErrorT cTest::GetInfo( SaHpiDimiTestT& info ) const
{
    info = m_info;

    return SA_OK;
}

SaErrorT cTest::GetReadiness( SaHpiDimiReadyT& ready )
{
    if ( m_status == SAHPI_DIMITEST_STATUS_RUNNING ) {
        ready = SAHPI_DIMI_BUSY;
    } else {
        ready = m_ready;
    }

    return SA_OK;
}

SaErrorT cTest::Start( SaHpiUint8T nparams,
                       const SaHpiDimiTestVariableParamsT * params )
{
    SaHpiDimiReadyT ready;
    SaErrorT rv = GetReadiness( ready );
    if ( rv != SA_OK ) {
        return rv;
    }
    if ( ready != SAHPI_DIMI_READY ) {
        return SA_ERR_HPI_INVALID_STATE;
    }
    bool rc= CheckParams( nparams, params );
    if ( !rc ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    ChangeStatus( SAHPI_DIMITEST_STATUS_RUNNING );

    m_handler.SetTimer( this, m_next.run_duration );

    return SA_OK;
}

SaErrorT cTest::Cancel()
{
    if ( m_status != SAHPI_DIMITEST_STATUS_RUNNING ) {
        return SA_ERR_HPI_INVALID_STATE;
    }
    if ( ( m_info.TestCapabilities & SAHPI_DIMITEST_CAPABILITY_TESTCANCEL ) == 0 ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    m_handler.CancelTimer( this );

    ChangeStatus( SAHPI_DIMITEST_STATUS_CANCELED );

    return SA_OK;
}

SaErrorT cTest::GetStatus( SaHpiDimiTestPercentCompletedT& progress,
                           SaHpiDimiTestRunStatusT& status ) const
{
    progress = m_progress;
    status   = m_status;

    return SA_OK;
}

SaErrorT cTest::GetResults( SaHpiDimiTestResultsT& results ) const
{
    results = m_prev_results;

    return SA_OK;
}


// cObject virtual functions
void cTest::GetVars( cVars& vars )
{
    cObject::GetVars( vars );
    Structs::GetVars( m_info, vars );

    vars << "Readiness"
         << dtSaHpiDimiReadyT
         << DATA( m_ready )
         << VAR_END();
    vars << "Status"
         << dtSaHpiDimiTestRunStatusT
         << DATA( m_status )
         << READONLY()
         << VAR_END();
    vars << "Progress"
         << dtSaHpiDimiTestPercentCompletedT
         << DATA( m_progress )
         << READONLY()
         << VAR_END();
    vars << "Next.RunDuration"
         << dtSaHpiTimeoutT
         << DATA( m_next.run_duration )
         << VAR_END();
    vars << "Next.TestErrorCode"
         << dtSaHpiDimiTestErrCodeT
         << DATA( m_next.err )
         << VAR_END();
    vars << "Next.TestResultString"
         << dtSaHpiTextBufferT
         << DATA( m_next.result_string )
         << VAR_END();
    vars << "Next.TestResultStringIsURI"
         << dtSaHpiBoolT
         << DATA( m_next.result_string_is_uri )
         << VAR_END();
}

void cTest::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );
}


bool cTest::CheckParams( SaHpiUint8T nparams,
                         const SaHpiDimiTestVariableParamsT * params ) const
{
    for ( size_t i = 0; i < nparams; ++i ) {
        const SaHpiDimiTestVariableParamsT * p = &params[i];
        const SaHpiDimiTestParamsDefinitionT * pd;
        size_t j;
        for ( j = 0; j < SAHPI_DIMITEST_MAX_PARAMETERS; ++j ) {
            pd = &m_info.TestParameters[j];
            int cc = strncmp( reinterpret_cast<const char *>( &p->ParamName[0] ),
                              reinterpret_cast<const char *>( &pd->ParamName[0] ),
                              SAHPI_DIMITEST_PARAM_NAME_LEN );
            if ( cc == 0 ) {
                break;
            }
        }
        if ( j == SAHPI_DIMITEST_MAX_PARAMETERS ) {
            continue;
        }
        if ( p->ParamType != pd->ParamType ) {
            return false;
        }
        if ( p->ParamType == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            if ( p->Value.paramint < pd->MinValue.IntValue ) {
                return false;
            }
            if ( p->Value.paramint > pd->MaxValue.IntValue ) {
                return false;
            }
        }
        if ( p->ParamType == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            if ( p->Value.paramfloat < pd->MinValue.FloatValue ) {
                return false;
            }
            if ( p->Value.paramfloat > pd->MaxValue.FloatValue ) {
                return false;
            }
        }
    }

    return true;
}

void cTest::ChangeStatus( SaHpiDimiTestRunStatusT status )
{
    m_status = status;

    SaHpiTimeT now;
    oh_gettimeofday( &now );

    if ( m_status == SAHPI_DIMITEST_STATUS_RUNNING ) {
        m_start_timestamp = now;
    } else if ( m_status == SAHPI_DIMITEST_STATUS_NOT_RUN ) {
        // Do nothing
    } else {
        // Update results
        m_prev_results.ResultTimeStamp = now;
        m_prev_results.RunDuration     = now - m_start_timestamp;
        m_prev_results.LastRunStatus   = m_status;
        m_prev_results.TestErrorCode   = m_next.err;
        if ( m_status != SAHPI_DIMITEST_STATUS_CANCELED ) {
            m_prev_results.TestResultString      = m_next.result_string;
            m_prev_results.TestResultStringIsURI = m_next.result_string_is_uri;
        } else {
            MakeHpiTextBuffer( m_prev_results.TestResultString, "The test has been cancelled" );
            m_prev_results.TestResultStringIsURI = SAHPI_FALSE;
        }
    }

    // Post event
    if ( !IsVisible() ) {
        return;
    }

    m_dimi.PostEvent( m_num, m_status, m_progress );
}


// cTimerCallback virtual functions
void cTest::TimerEvent()
{
    cLocker<cHandler> al( &m_handler );

    SaHpiDimiTestRunStatusT status;
    if ( m_next.err == SAHPI_DIMITEST_STATUSERR_NOERR ) {
        status = SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS;
    } else {
        status = SAHPI_DIMITEST_STATUS_FINISHED_ERRORS;
    }

    ChangeStatus( status );
}


}; // namespace TA

