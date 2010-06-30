/** 
 * @file    new_sim_fumi_data.h
 *
 * The file includes the classes for fumi bank and component handling:\n
 * NewSimulatorFumiBank
 * NewSimulatorFumiComponent
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

#ifndef __NEW_SIM_FUMI_DATA_H__
#define __NEW_SIM_FUMI_DATA_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

class  NewSimulatorDomain;


/**
 * @class NewSimulatorFumiComponent
 * 
 * Class for simulating fumi components
 * 
 **/
class NewSimulatorFumiComponent {

private:
  /// Record with component target information
  SaHpiFumiComponentInfoT        m_target_info;
  /// Record with component source information
  SaHpiFumiComponentInfoT        m_source_info;
  /// Recourd with logical component information
  SaHpiFumiLogicalComponentInfoT m_logical;
    
public:
  NewSimulatorFumiComponent();
  virtual ~NewSimulatorFumiComponent();
  
  /// Return componentId
  virtual SaHpiUint32T Num() const { return m_target_info.ComponentId; }

  /// Return the values of source record
  SaHpiFumiComponentInfoT GetSourceData() const { return m_source_info; }
  bool SetSourceData( SaHpiFumiComponentInfoT cinf );

  /// Return the values of target record
  SaHpiFumiComponentInfoT GetTargetData() const { return m_target_info; }
  bool SetTargetData( SaHpiFumiComponentInfoT cinf );
  
  /// Return the values of target record
  SaHpiFumiComponentInfoT GetData() const { return m_target_info; }
  bool SetData( SaHpiFumiComponentInfoT cinf );

  /// Return the values of logical target record
  SaHpiFumiLogicalComponentInfoT GetLogicalData() const { return m_logical; }
  bool SetData( SaHpiFumiLogicalComponentInfoT cinf );
  
  // methods for HPI functions
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


/**
 * @class NewSimulatorFumiBank
 * 
 * Class for simulating a fumi bank
 * 
 **/
class NewSimulatorFumiBank {
   private:
   /// Record with source information
   SaHpiFumiSourceInfoT      m_source;
   /// Source location
   SaHpiTextBufferT          m_source_loc;
   /// Record with target information
   SaHpiFumiBankInfoT        m_target;
   /// Record with logical target information
   SaHpiFumiLogicalBankInfoT m_logical;
   
   /// Array including source components information
   cArray<NewSimulatorFumiComponent>  m_comps;

   NewSimulatorFumiComponent *GetComponent( SaHpiUint32T id );
     
   public:
   NewSimulatorFumiBank();
   virtual ~NewSimulatorFumiBank();

   /// Return bank id
   virtual SaHpiUint8T Num() const { return m_target.BankId; }   
   bool SetId( SaHpiUint8T id ); 

   /// Return source information
   SaHpiFumiSourceInfoT GetSource() const { return m_source; }
   bool SetData( SaHpiFumiSourceInfoT src );

   /// Return target information
   SaHpiFumiBankInfoT GetTarget() const { return m_target; }   
   bool SetData( SaHpiFumiBankInfoT trg );

   /// Return logical target information
   SaHpiFumiLogicalBankInfoT GetLogical() const { return m_logical; }
   bool SetData( SaHpiFumiLogicalBankInfoT lgc );

   bool AddSourceComponent( NewSimulatorFumiComponent *component );
   bool AddTargetComponent( NewSimulatorFumiComponent *component );
   bool AddLogicalTargetComponent( NewSimulatorFumiComponent *component );
   
   // HPI functions
   SaErrorT SetSource( SaHpiTextBufferT &src );
   SaErrorT GetSource( SaHpiFumiSourceInfoT &src );
   SaErrorT GetTarget( SaHpiFumiBankInfoT &trg );
   SaErrorT GetLogicalTarget( SaHpiFumiLogicalBankInfoT &trg );
   
   virtual void Dump( NewSimulatorLog &dump ) const;
};




#endif

