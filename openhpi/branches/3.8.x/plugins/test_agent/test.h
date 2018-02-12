/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2012
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTTESTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef TEST_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define TEST_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>

#include <SaHpi.h>

#include "object.h"
#include "timers.h"


namespace TA {


/**************************************************************
 * class cTest
 *************************************************************/
class cHandler;
class cDimi;

class cTest : public cObject, private cTimerCallback
{
public:

    static const std::string classname;

    explicit cTest( cHandler& handler, cDimi& dimi, SaHpiDimiTestNumT num );
    virtual ~cTest();

public: // HPI interface

    SaErrorT GetInfo( SaHpiDimiTestT& info ) const;
    SaErrorT GetReadiness( SaHpiDimiReadyT& ready );
    SaErrorT Start( SaHpiUint8T nparams,
                    const SaHpiDimiTestVariableParamsT * params );
    SaErrorT Cancel();
    SaErrorT GetStatus( SaHpiDimiTestPercentCompletedT& progress,
                        SaHpiDimiTestRunStatusT& status ) const;
    SaErrorT GetResults( SaHpiDimiTestResultsT& results ) const;

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cTest( const cTest& );
    cTest& operator =( const cTest& );

private:

    bool CheckParams( SaHpiUint8T nparams,
                      const SaHpiDimiTestVariableParamsT * params ) const;
    void ChangeStatus( SaHpiDimiTestRunStatusT status );

private: // cTimerCallback virtual functions

    virtual void TimerEvent();

private: // data

    cHandler&                      m_handler;
    cDimi&                         m_dimi;
    const SaHpiDimiTestNumT        m_num;
    SaHpiDimiTestT                 m_info;
    SaHpiDimiReadyT                m_ready;
    SaHpiDimiTestRunStatusT        m_status;
    SaHpiDimiTestPercentCompletedT m_progress;

    SaHpiDimiTestResultsT m_prev_results;
    SaHpiTimeT            m_start_timestamp;
    struct
    {
        SaHpiTimeoutT           run_duration;
        SaHpiDimiTestErrCodeT   err;
        SaHpiTextBufferT        result_string;
        SaHpiBoolT              result_string_is_uri;
    } m_next;
};


}; // namespace TA


#endif // TEST_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

