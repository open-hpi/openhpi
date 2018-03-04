/** 
 * @file    new_sim_fumi.cpp
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "new_sim_domain.h"
#include "new_sim_fumi.h"
#include "new_sim_fumi_data.h"
#include "new_sim_entity.h"


/**
 * Constructor
 **/
NewSimulatorFumi::NewSimulatorFumi( NewSimulatorResource *res )
  : NewSimulatorRdr( res, SAHPI_FUMI_RDR ), 
    m_dis_rb( SAHPI_TRUE ) {
    	
   memset( &m_fumi_rec, 0, sizeof( SaHpiFumiRecT ));
   memset( &m_spec_info, 0, sizeof( SaHpiFumiSpecInfoT ));
   memset( &m_impact_data, 0, sizeof( SaHpiFumiServiceImpactDataT ));
   
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorFumi::NewSimulatorFumi( NewSimulatorResource *res,
                                    SaHpiRdrT rdr)
  : NewSimulatorRdr( res, SAHPI_FUMI_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_dis_rb( SAHPI_TRUE ) {

   memcpy(&m_fumi_rec, &rdr.RdrTypeUnion.FumiRec, sizeof( SaHpiFumiRecT ));
   memset( &m_spec_info, 0, sizeof( SaHpiFumiSpecInfoT ));
   memset( &m_impact_data, 0, sizeof( SaHpiFumiServiceImpactDataT ));
}

                      
/**
 * Destructor
 **/
NewSimulatorFumi::~NewSimulatorFumi() {
   m_banks.RemAll();   
}


/** 
 * Dump the Fumi information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorFumi::Dump( NewSimulatorLog &dump ) const {

  dump << "Fumi:       " << m_fumi_rec.Num << "\n";
  dump << "AccessProt: " << m_fumi_rec.AccessProt << "\n";
  dump << "Capability: " << m_fumi_rec.Capability << "\n";
  dump << "NumBanks:   " << m_fumi_rec.NumBanks << "\n";
  dump << "Oem:        " << m_fumi_rec.Oem << "\n";
  dump << "Bank(s) Information: " << "\n";
  dump << "-------------------\n";
  for (int i= 0; i < m_banks.Num(); i++) {
      m_banks[i]->Dump( dump );
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
bool NewSimulatorFumi::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
  
  if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
       return false;

  // Inventory record
  memcpy(&rdr.RdrTypeUnion.FumiRec, &m_fumi_rec, sizeof(SaHpiFumiRecT));

  return true;
}

/**
 * Set fumi record data 
 * 
 * @param fumiRec FumiRecord data
 * @return true (simple copy)
 **/ 
bool NewSimulatorFumi::SetData( SaHpiFumiRecT fumiRec ) {
   
   memcpy( &m_fumi_rec, &fumiRec, sizeof( SaHpiFumiRecT ));

   return true;
}


/**
 * Set fumi information data 
 * 
 * @param spec record with specification information
 * @param impact record with information about affected entities
 * @param rbDis  flag if automatic rollback is disabled or not
 * @return true 
 **/
bool NewSimulatorFumi::SetInfo( SaHpiFumiSpecInfoT spec, SaHpiFumiServiceImpactDataT impact, 
                                 SaHpiBoolT rbDis ) {
   
   memcpy( &m_spec_info, &spec, sizeof( SaHpiFumiSpecInfoT ));
   memcpy( &m_impact_data, &impact, sizeof( SaHpiFumiServiceImpactDataT ));
   m_dis_rb = rbDis;
   
   return true;
}


/**
 * Add or find a bank by id
 * 
 * A new NewSimulatorFumiBank is generated inside the function, if the bank doesn't
 * exist. If a bank with the same id already exists, it is returned.
 * 
 * @param id bankId of NewSimulatorFumiBank object

 * @return pointer on a NewSimulatorFumiBank object
 **/
NewSimulatorFumiBank *NewSimulatorFumi::GetOrAddBank( SaHpiUint8T id ) {
   NewSimulatorFumiBank *b = NULL;
   
   for (int i=0; i < m_banks.Num(); i++) {
      if ( id == m_banks[i]->Num() )
         b = m_banks[i];
   }
   
   if (b == NULL) {
      b = new NewSimulatorFumiBank();
      b->SetId( id );
      m_banks.Add( b );
   }
   
   return b;
}


/**
 * Find a bank by id
 * 
 * If a bank with the same id already exists, it is returned.
 * 
 * @param id bankId of NewSimulatorFumiBank object

 * @return pointer on a NewSimulatorFumiBank object
 **/
NewSimulatorFumiBank *NewSimulatorFumi::GetBank( SaHpiUint8T id ) {
   NewSimulatorFumiBank *b = NULL;
   
   for (int i=0; i < m_banks.Num(); i++) {
      if ( id == m_banks[i]->Num() )
         b = m_banks[i];
   }
   
   return b;
} 
 
/**
 * Set source information for a FumiBank
 * 
 * @param bank pointer on a NewSimulatorFumiBank object with the source information

 * @return true 
 **/
bool NewSimulatorFumi::SetBankSource( NewSimulatorFumiBank *bank ) {
   NewSimulatorFumiBank *b;

   b = GetOrAddBank( bank->Num() );
   b->SetData( bank->GetSource() );
   
   return true;
}


/**
 * Set target information for a FumiBank
 * 
 * @param bank pointer on a NewSimulatorFumiBank object with the target information

 * @return true 
 **/
bool NewSimulatorFumi::SetBankTarget( NewSimulatorFumiBank *bank ) {
   NewSimulatorFumiBank *b;

   b = GetOrAddBank( bank->Num() );
   b->SetData( bank->GetTarget() );

   return true;
}


/**
 * Set logical target information for a FumiBank
 * 
 * @param bank pointer on a NewSimulatorFumiBank object with the logical target information

 * @return true 
 **/
bool NewSimulatorFumi::SetBankLogical( NewSimulatorFumiBank *bank ) {
   NewSimulatorFumiBank *b;

   b = GetOrAddBank( bank->Num() );
   b->SetData( bank->GetLogical() );

   return true;
}



// Official HPI functions
 
/**
 * HPI function saHpiFumiSpecInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param spec address of the FumiSpecInfo record to be filled
 * 
 * @return HPI return code
 **/    
SaErrorT NewSimulatorFumi::GetSpecInfo( SaHpiFumiSpecInfoT &spec ) {
   SaErrorT rv = SA_OK;

   if ( &spec == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   memcpy( &spec, &m_spec_info, sizeof( SaHpiFumiSpecInfoT ));
   return rv;
}

/**
 * HPI function saHpiFumiServiceImpactGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param impact address of the FumiServiceImpactData record to be filled
 * 
 * @return HPI return code
 **/  
SaErrorT NewSimulatorFumi::GetImpact( SaHpiFumiServiceImpactDataT &impact ) {
   SaErrorT rv = SA_OK;
   
   if ( &impact == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   memcpy( &impact, &m_impact_data, sizeof( SaHpiFumiServiceImpactDataT ));
   return rv;
}


/**
 * HPI function saHpiFumiSourceSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param bank number of bank
 * @param src text buffer including the source information
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumi::SetSource( SaHpiBankNumT bank, 
                                      SaHpiTextBufferT &src ) {

   NewSimulatorFumiBank *b = GetBank(bank);
   
   if ( b == NULL )
      return SA_ERR_HPI_NOT_PRESENT;
   
   b->SetSource( src );
   
   return b->SetSource( src );
}



SaErrorT NewSimulatorFumi::ValidateSource( SaHpiBankNumT bank ) {
   SaErrorT rv = SA_OK;

   return rv;
}


/**
 * HPI function saHpiFumiSourceGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param bank number of bank
 * @param src address of source info structure to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumi::GetSource( SaHpiBankNumT bank, 
                                      SaHpiFumiSourceInfoT &src ) {

   NewSimulatorFumiBank *b = GetBank(bank);
   
   if ( b == NULL )
      return SA_ERR_HPI_NOT_PRESENT;

   return b->GetSource( src );
}


SaErrorT NewSimulatorFumi::GetComponentSource( SaHpiBankNumT bank, 
                                               SaHpiEntryIdT comp, 
                                               SaHpiEntryIdT &next, 
                                               SaHpiFumiComponentInfoT &inf ) {
   SaErrorT rv = SA_ERR_HPI_UNSUPPORTED_API;

   return rv;
}


/**
 * HPI function saHpiFumiTargetInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * Inside the function the bank information is get by a call of 
 * NewSimulatorFumiBank::GetTarget.
 * 
 * @param bank number of bank
 * @param trg address of target info structure to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumi::GetTarget( SaHpiBankNumT bank, 
                                      SaHpiFumiBankInfoT &trg ) {

   NewSimulatorFumiBank *b = GetBank(bank);
   
   if ( b == NULL )
      return SA_ERR_HPI_NOT_PRESENT;

   return b->GetTarget( trg );   
}


SaErrorT NewSimulatorFumi::GetComponentTarget( SaHpiBankNumT bank, 
                                               SaHpiEntryIdT comp, 
                                               SaHpiEntryIdT &next, 
                                               SaHpiFumiComponentInfoT &inf )  {
   SaErrorT rv = SA_ERR_HPI_UNSUPPORTED_API;

   return rv;
}


/**
 * HPI function saHpiFumiLogicalTargetInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * Inside the function the bank information is get by a call of 
 * NewSimulatorFumiBank::GetLogicalTarget.
 * 
 * @param trg address of logical target info structure to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumi::GetTargetLogical( SaHpiFumiLogicalBankInfoT &trg )  {

   NewSimulatorFumiBank *b = GetBank( 0 );
   
   if ( b == NULL )
      return SA_ERR_HPI_NOT_PRESENT;

   return b->GetLogicalTarget( trg );
}


SaErrorT NewSimulatorFumi::GetComponentTargetLogical( SaHpiEntryIdT comp, 
                                                      SaHpiEntryIdT &next, 
                                     SaHpiFumiLogicalComponentInfoT &inf ) {
   SaErrorT rv = SA_ERR_HPI_UNSUPPORTED_API;

   return rv;
}
SaErrorT NewSimulatorFumi::StartBackup() {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::SetOrder( SaHpiBankNumT bank, SaHpiUint32T pos ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::CopyBank( SaHpiBankNumT bank, 
                                     SaHpiBankNumT dest ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::Install( SaHpiBankNumT bank ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::GetStatus( SaHpiBankNumT bank, 
                                      SaHpiFumiUpgradeStatusT &status ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::VerifyTarget( SaHpiBankNumT bank ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::VerifyTargetMain() {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::CancelUpgrade( SaHpiBankNumT bank ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::GetRollbackFlag( SaHpiBoolT &rollb ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::SetRollbackFlag( SaHpiBoolT rollb ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::Rollback() {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::Activate() {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::Activate( SaHpiBoolT log ) {
   SaErrorT rv = SA_OK;

   return rv;
}
SaErrorT NewSimulatorFumi::Cleanup( SaHpiBankNumT bank )  {
   SaErrorT rv = SA_OK;

   return rv;
}

