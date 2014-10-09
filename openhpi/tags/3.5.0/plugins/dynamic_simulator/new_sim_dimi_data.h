/** 
 * @file    new_sim_dimi_data.h
 *
 * The file includes the class for dimi test handling:\n
 * NewSimulatorDimiTest
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.1
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 *    
 */

#ifndef __NEW_SIM_DIMI_DATA_H__
#define __NEW_SIM_DIMI_DATA_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

class  NewSimulatorDomain;


/**
 * @class NewSimulatorDimiTest
 * 
 * Class for simulating dimi Test
 * 
 **/
class NewSimulatorDimiTest {
   private:
   /// Number of test case
   SaHpiDimiTestNumT         m_test_id;
   /// Record with test information
   SaHpiDimiTestT            m_info;
   /// Record with test results
   SaHpiDimiTestResultsT     m_results;
   /// State of test case
   SaHpiDimiTestRunStatusT   m_status;
   /// Readiness of test case
   SaHpiDimiReadyT           m_ready;
   
     
   public:
   NewSimulatorDimiTest( SaHpiDimiTestNumT id );
   virtual ~NewSimulatorDimiTest();

   /// Return test case id
   virtual SaHpiDimiTestNumT Num() const { return m_test_id; }
   bool SetData( SaHpiDimiTestT info );
   bool SetReadiness( SaHpiDimiReadyT ready );
   bool SetResults( SaHpiDimiTestResultsT results );
   
   bool IsRunning();
   
   // HPI functions
   SaErrorT GetInfo( SaHpiDimiTestT &tinfo );
   SaErrorT GetReady( SaHpiDimiReadyT &ready );
   SaErrorT StartTest( SaHpiUint8T number, SaHpiDimiTestVariableParamsT *param );
   SaErrorT Cancel();
   SaErrorT GetStatus( SaHpiDimiTestPercentCompletedT &perc,
                       SaHpiDimiTestRunStatusT &status);
   SaErrorT GetResults(SaHpiDimiTestResultsT &results);
   
   virtual void Dump( NewSimulatorLog &dump ) const;
};




#endif

