/** 
 * @file    new_sim_dimi_data.cpp
 *
 * The file includes a class for dimi data handling:\n
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "new_sim_domain.h"
#include "new_sim_dimi.h"
#include "new_sim_dimi_data.h"
#include "new_sim_entity.h"


/**
 * Constructor
 **/
NewSimulatorDimiTest::NewSimulatorDimiTest( SaHpiDimiTestNumT id ) :
                      m_test_id( id ),
                      m_status ( SAHPI_DIMITEST_STATUS_NOT_RUN ),
                      m_ready( SAHPI_DIMI_READY ) {
}

                      
/**
 * Destructor
 **/
NewSimulatorDimiTest::~NewSimulatorDimiTest() {
}


/** 
 * Dump Dimi Test information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorDimiTest::Dump( NewSimulatorLog &dump ) const {

  dump << "Test information\n";
  dump << "----------------\n";
  dump << "TestName:         " << m_info.TestName << "\n";
  dump << "ServiceImpact:    " << (int) m_info.ServiceImpact << "\n";
  dump << "EntitiesImpacted:\n";
  for (int i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; i++)
     dump << "   " << m_info.EntitiesImpacted[i].EntityImpacted << "\n";
  
  dump << "NeedServiceOS:    " << m_info.NeedServiceOS << "\n";
  dump << "ServiceOS:        " << m_info.ServiceOS << "\n";
  dump << "ExpectedRunDuration: " << (long int) m_info.ExpectedRunDuration << "\n";
  dump << "TestCapabilities:    " << m_info.TestCapabilities << "\n";
  
}

/**
 * Set testcase data - info 
 * 
 * @param tinfo test information
 *  
 * @return true 
 **/
bool NewSimulatorDimiTest::SetData( SaHpiDimiTestT    tinfo ) {

   memcpy( &m_info, &tinfo, sizeof( SaHpiDimiTestT ));
   
   return true;
}


/**
 * Set testcase data - readiness 
 * 
 * @param ready readiness value
 *  
 * @return true 
 **/
bool NewSimulatorDimiTest::SetReadiness( SaHpiDimiReadyT ready ) {

   m_ready = ready;
   
   return true;
}

/**
 * Set testcase data - results
 * 
 * The status is set on \c SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS
 * 
 * @param results information about results
 *  
 * @return true 
 **/
bool NewSimulatorDimiTest::SetResults( SaHpiDimiTestResultsT results ) {

   memcpy( &m_results, &results, sizeof( SaHpiDimiTestResultsT ));
   m_status = SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS;
   
   return true;
}


/**
 * Test if a test case is running
 * 
 * @return true if the test case is marked as running
 **/
bool NewSimulatorDimiTest::IsRunning() {

   if ( m_status == SAHPI_DIMITEST_STATUS_RUNNING )
      return true;
   
   return false;
}



// HPI functions
/**
 * HPI function saHpiDimiTestInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param tinfo address of the Dimi test record to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::GetInfo( SaHpiDimiTestT &tinfo ) {
   
   memcpy( &tinfo, &m_info, sizeof( SaHpiDimiTestT ));
   
   return SA_OK;
}


/**
 * HPI function saHpiDimiTestReadinessGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param ready address to store the Ready information
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::GetReady( SaHpiDimiReadyT &ready ) {

   ready = m_ready;
      
   return SA_OK;
}


/**
 * HPI function saHpiDimiTestStart()
 * 
 * See also the description of the function inside the specification or header file.
 * @todo: add some functionality
 * 
 * @param number number of parameters
 * @param param pointer on array including the parameters
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::StartTest( SaHpiUint8T number, 
                                          SaHpiDimiTestVariableParamsT *param ) {
   
   SaErrorT rv = SA_OK;
   
   if ( m_ready != SAHPI_DIMI_READY ) 
      return SA_ERR_HPI_INVALID_STATE;
   
   if ( number != 0 && param == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   /// @todo some functionality behind start dimi test
      
   return rv;
}


/**
 * HPI function saHpiDimiTestCancel()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::Cancel() {
   
   if ( m_status != SAHPI_DIMITEST_STATUS_RUNNING )
      return SA_ERR_HPI_INVALID_STATE;
   
   return SA_OK;
}


/**
 * HPI function saHpiDimiTestStatusGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param perc address to store percentage of test completed
 * @param status address to store the status of the last run
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::GetStatus( SaHpiDimiTestPercentCompletedT &perc,
                       SaHpiDimiTestRunStatusT &status) {

   if ( &perc != NULL ) {
      switch ( m_status ) {
         case SAHPI_DIMITEST_STATUS_NOT_RUN:
         case SAHPI_DIMITEST_STATUS_CANCELED:
            perc = 0;
            break;
         case SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS:
         case SAHPI_DIMITEST_STATUS_FINISHED_ERRORS:
            perc = 100;
            break;
         case SAHPI_DIMITEST_STATUS_RUNNING:
            perc = 50;
            break;
         default:
            perc = 0;
      }
   }
   
   status = m_status;
   
   return SA_OK;
}

/**
 * HPI function saHpiDimiTestResultsGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param results address to store test results
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimiTest::GetResults(SaHpiDimiTestResultsT &results) {
   
   memcpy( &results, &m_results, sizeof( SaHpiDimiTestResultsT ));
   
   return SA_OK;  
}

