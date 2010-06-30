/** 
 * @file    new_sim_dimi.h
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
 *    
 */

#ifndef __NEW_SIM_DIMI_H__
#define __NEW_SIM_DIMI_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_DIMI_DATA_H__
#include "new_sim_dimi_data.h"
#endif

class  NewSimulatorDomain;

/**
 * @class NewSimulatorDimi
 * 
 * Class for simulating Dimi
 * 
 **/
class NewSimulatorDimi : public NewSimulatorRdr {

private:
  /// Record with the Dimi rdr information
  SaHpiDimiRecT        m_dimi_rec;
  /// Record with dimi information
  SaHpiDimiInfoT       m_dimi_info;
  /// Array including dimi tests 
  cArray<NewSimulatorDimiTest>  m_tests;
  /// Number of max test id
  SaHpiDimiTestNumT    m_test_id;
  
  NewSimulatorDimiTest *GetTest( SaHpiDimiTestNumT num );
  
  
  
public:
  NewSimulatorDimi( NewSimulatorResource *res );
  NewSimulatorDimi( NewSimulatorResource *res, SaHpiRdrT rdr );    
  virtual ~NewSimulatorDimi();

  
  /// Return DimiId
  virtual unsigned int Num() const { return ( unsigned int ) m_dimi_rec.DimiNum; }
 
  /// Get a valid test id
  SaHpiDimiTestNumT GetTestId() { return m_test_id++; }
  bool SetData( SaHpiDimiRecT dimiRec );
  bool SetInfo( SaHpiDimiInfoT spec );
  bool AddTest( NewSimulatorDimiTest *test );
  
  // create a RDR record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );
  
  // Official HPI functions
  SaErrorT GetResults( SaHpiDimiTestNumT id, SaHpiDimiTestResultsT &results);
  SaErrorT GetStatus( SaHpiDimiTestNumT id, SaHpiDimiTestPercentCompletedT &perc,
                      SaHpiDimiTestRunStatusT &status );
  SaErrorT CancelTest( SaHpiDimiTestNumT id );
  SaErrorT StartTest( SaHpiDimiTestNumT id, SaHpiUint8T number,
                      SaHpiDimiTestVariableParamsT *param); 
  SaErrorT GetReadiness( SaHpiDimiTestNumT id, SaHpiDimiReadyT &ready );
  SaErrorT GetTestInfo( SaHpiDimiTestNumT id, SaHpiDimiTestT &tinfo ); 
  SaErrorT GetInfo( SaHpiDimiInfoT &info );
                                                                                                           
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
