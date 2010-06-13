/** 
 * @file    new_sim_dimi.cpp
 *
 * The file includes a class for dimi handling:\n
 * NewSimulatorDimi
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
NewSimulatorDimi::NewSimulatorDimi( NewSimulatorResource *res )
  : NewSimulatorRdr( res, SAHPI_DIMI_RDR ),
    m_test_id( 0 ) {
    	
   memset( &m_dimi_rec, 0, sizeof( SaHpiFumiRecT ));
   memset( &m_dimi_info, 0, sizeof( SaHpiFumiSpecInfoT ));
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorDimi::NewSimulatorDimi( NewSimulatorResource *res,
                                    SaHpiRdrT rdr)
  : NewSimulatorRdr( res, SAHPI_DIMI_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_test_id( 0 ) {

   memcpy(&m_dimi_rec, &rdr.RdrTypeUnion.DimiRec, sizeof( SaHpiDimiRecT ));
   memset( &m_dimi_info, 0, sizeof( SaHpiDimiInfoT ));
}

                      
/**
 * Destructor
 **/
NewSimulatorDimi::~NewSimulatorDimi() {
   m_tests.RemAll();   
}


/** 
 * Dump the dimi information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorDimi::Dump( NewSimulatorLog &dump ) const {

  dump << "Dimi:       " << m_dimi_rec.DimiNum << "\n";
  dump << "Oem:        " << m_dimi_rec.Oem << "\n";
  dump << "NumberOfTests: " << m_dimi_info.NumberOfTests << "\n";
  dump << "TestNumUpdateCounter: " << m_dimi_info.TestNumUpdateCounter << "\n";
  dump << "Test(s) Information: " << "\n";
  dump << "-------------------\n";
  for (int i= 0; i < m_tests.Num(); i++) {
      m_tests[i]->Dump( dump );
  }
}

/**
 * A rdr structure is filled with the data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorDimi::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
  
  if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
       return false;

  // Inventory record
  memcpy(&rdr.RdrTypeUnion.DimiRec, &m_dimi_rec, sizeof(SaHpiDimiRecT));

  return true;
}

/**
 * Set dimi record data 
 * 
 * @param dimiRec DimiRecord data
 * @return true (simple copy)
 **/ 
bool NewSimulatorDimi::SetData( SaHpiDimiRecT dimiRec ) {
   
   memcpy( &m_dimi_rec, &dimiRec, sizeof( SaHpiDimiRecT ));

   return true;
}


/**
 * Set dimi information data 
 * 
 * @param info record with information
 *
 * @return true 
 **/
bool NewSimulatorDimi::SetInfo( SaHpiDimiInfoT info ) {
   
   memcpy( &m_dimi_info, &info, sizeof( SaHpiDimiInfoT ));
   
   return true;
}


/**
 * Find a test by id
 * 
 * @param id id of NewSimulatorTest object

 * @return pointer on a NewSimulatorTest object, NULL if no test can be found
 **/
NewSimulatorDimiTest *NewSimulatorDimi::GetTest( SaHpiDimiTestNumT id ) {
   NewSimulatorDimiTest *t = NULL;
   
   for (int i=0; i < m_tests.Num(); i++) {
      if ( id == m_tests[i]->Num() )
         t = m_tests[i];
   }
   
   return t;
}

/**
 * Add a test 
 * 
 * @param t pointer on NewSimulatorDimiTest to be added

 * @return true
 **/
bool NewSimulatorDimi::AddTest( NewSimulatorDimiTest *t ) {
                                 	
   m_tests.Add( t );
   
   return true;
}


// Official HPI functions
 
/**
 * HPI function saHpiDimiInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param info address of the DimiInfo record to be filled
 * 
 * @return HPI return code
 **/    
SaErrorT NewSimulatorDimi::GetInfo( SaHpiDimiInfoT &info ) {
   SaErrorT rv = SA_OK;

   if ( &info == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   memcpy( &info, &m_dimi_info, sizeof( SaHpiDimiInfoT ));
   return rv;
}

/**
 * HPI function saHpiDimiTestInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param id id of test 
 * @param tinfo address of the Dimi test record to be filled
 * 
 * @return HPI return code
 **/  
SaErrorT NewSimulatorDimi::GetTestInfo( SaHpiDimiTestNumT id, SaHpiDimiTestT &tinfo ) {
   NewSimulatorDimiTest *t;
   
   if ( &tinfo == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
      
   return t->GetInfo( tinfo );
}


/**
 * HPI function saHpiDimiTestReadinessGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param id id of test
 * @param ready address to store the Ready information
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimi::GetReadiness( SaHpiDimiTestNumT id, 
                                         SaHpiDimiReadyT &ready ) {

   NewSimulatorDimiTest *t;
   
   if ( &ready == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
      
   return t->GetReady( ready );
}


/**
 * HPI function saHpiDimiTestStart()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param id id of test
 * @param number number of parameters
 * @param param pointer on array including the parameters
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimi::StartTest( SaHpiDimiTestNumT id,  
                                      SaHpiUint8T number,
                                      SaHpiDimiTestVariableParamsT *param) {

   NewSimulatorDimiTest *t;
   
   if ( (number != 0) && (param == NULL) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
      
   return t->StartTest( number, param );
}


/**
 * HPI function saHpiDimiTestCancel()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param id id of test
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimi::CancelTest( SaHpiDimiTestNumT id ) {

   NewSimulatorDimiTest *t;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
   
   if ( !t->IsRunning() )
      return SA_ERR_HPI_INVALID_STATE;
      
   return t->Cancel();
}


/**
 * HPI function saHpiDimiTestStatusGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param id id of test
 * @param perc address to store percentage of test completed
 * @param status address to store the status of the last run
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimi::GetStatus( SaHpiDimiTestNumT id,
                                      SaHpiDimiTestPercentCompletedT &perc,
                                      SaHpiDimiTestRunStatusT &status ) {

   NewSimulatorDimiTest *t;
   
   if ( &status == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
      
   return t->GetStatus( perc, status);
}


/**
 * HPI function saHpiDimiTestResultsGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param id id of test
 * @param results address to store the results of the last run
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorDimi::GetResults( SaHpiDimiTestNumT id,
                                       SaHpiDimiTestResultsT &results) {

   NewSimulatorDimiTest *t;
   
   if ( &results == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   t = GetTest( id );
   
   if ( t == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
      
   return t->GetResults( results );
}

