/** 
 * @file    new_sim_fumi.h
 *
 * The file includes a class for fumi handling:\n
 * NewSimulatorFumi
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

#ifndef __NEW_SIM_FUMI_H__
#define __NEW_SIM_FUMI_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_FUMI_DATA_H__
#include "new_sim_fumi_data.h"
#endif

class  NewSimulatorDomain;

/**
 * @class NewSimulatorFumi
 * 
 * Class for simulating Fumi
 * 
 **/
class NewSimulatorFumi : public NewSimulatorRdr {

private:
  /// Record with the Fumi rdr information
  SaHpiFumiRecT        m_fumi_rec;
  /// Record with underlying spec framework information
  SaHpiFumiSpecInfoT   m_spec_info;
  /// Record with entities affected by an upgrade process
  SaHpiFumiServiceImpactDataT m_impact_data;
  /// Flag for automatic rollback
  SaHpiBoolT           m_dis_rb; 
  /// Array including the banks with source and target information
  cArray<NewSimulatorFumiBank>  m_banks;
  
  NewSimulatorFumiBank *GetOrAddBank( SaHpiUint8T id );
  NewSimulatorFumiBank *GetBank( SaHpiUint8T id );
  
  
public:
  NewSimulatorFumi( NewSimulatorResource *res );
  NewSimulatorFumi( NewSimulatorResource *res, SaHpiRdrT rdr );    
  virtual ~NewSimulatorFumi();

  
  /// Return FumiId
  virtual unsigned int Num() const { return ( unsigned int ) m_fumi_rec.Num; }

  bool SetData( SaHpiFumiRecT fumiRec );
  bool SetInfo( SaHpiFumiSpecInfoT spec, SaHpiFumiServiceImpactDataT impact, 
                 SaHpiBoolT rbDis );
  bool SetBankSource( NewSimulatorFumiBank *bank );
  bool SetBankTarget( NewSimulatorFumiBank *bank );
  bool SetBankLogical( NewSimulatorFumiBank *bank );
  
  // create a RDR record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );
  
  // Official HPI functions
  SaErrorT GetSpecInfo( SaHpiFumiSpecInfoT &spec );
  SaErrorT GetImpact( SaHpiFumiServiceImpactDataT &impact );
  SaErrorT SetSource( SaHpiBankNumT bank, SaHpiTextBufferT &src );
  SaErrorT ValidateSource( SaHpiBankNumT bank );
  SaErrorT GetSource( SaHpiBankNumT bank, SaHpiFumiSourceInfoT &src );
  SaErrorT GetComponentSource( SaHpiBankNumT bank, SaHpiEntryIdT comp, 
                           SaHpiEntryIdT &next, SaHpiFumiComponentInfoT &inf );
  SaErrorT GetTarget( SaHpiBankNumT bank, SaHpiFumiBankInfoT &trg );
  SaErrorT GetComponentTarget( SaHpiBankNumT bank, SaHpiEntryIdT comp, 
                           SaHpiEntryIdT &next, SaHpiFumiComponentInfoT &inf );
  SaErrorT GetTargetLogical( SaHpiFumiLogicalBankInfoT &trg );
  SaErrorT GetComponentTargetLogical( SaHpiEntryIdT comp, SaHpiEntryIdT &next, 
                                      SaHpiFumiLogicalComponentInfoT &inf );
  SaErrorT StartBackup();
  SaErrorT SetOrder( SaHpiBankNumT bank, SaHpiUint32T pos );
  SaErrorT CopyBank( SaHpiBankNumT bank, SaHpiBankNumT dest );
  SaErrorT Install( SaHpiBankNumT bank );
  SaErrorT GetStatus( SaHpiBankNumT bank, SaHpiFumiUpgradeStatusT &status );
  SaErrorT VerifyTarget( SaHpiBankNumT bank );
  SaErrorT VerifyTargetMain();
  SaErrorT CancelUpgrade( SaHpiBankNumT bank );
  SaErrorT GetRollbackFlag( SaHpiBoolT &rollb );
  SaErrorT SetRollbackFlag( SaHpiBoolT rollb );
  SaErrorT Rollback();
  SaErrorT Activate();
  SaErrorT Activate( SaHpiBoolT log );
  SaErrorT Cleanup( SaHpiBankNumT bank );
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
