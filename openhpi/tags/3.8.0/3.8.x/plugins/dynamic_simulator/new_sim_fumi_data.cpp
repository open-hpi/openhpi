/** 
 * @file    new_sim_fumi_data.cpp
 *
 * The file includes a class for fumi data handling:\n
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
NewSimulatorFumiBank::NewSimulatorFumiBank() {

   memset( &m_source, 0, sizeof( SaHpiFumiSourceInfoT ));
   memset( &m_target, 0, sizeof( SaHpiFumiBankInfoT ));
   memset( &m_logical, 0, sizeof( SaHpiFumiLogicalBankInfoT ));
   memset( &m_source_loc, 0, sizeof( SaHpiTextBufferT ));
}

                      
/**
 * Destructor
 **/
NewSimulatorFumiBank::~NewSimulatorFumiBank() {

   m_comps.RemAll();
}


/** 
 * Dump Fumi Bank information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorFumiBank::Dump( NewSimulatorLog &dump ) const {

  dump << "Bank information\n";
  dump << "----------------\n";
  dump << "BankId:      " << m_target.BankId << "\n";
  dump << "BankSize:    " << m_target.BankSize << "\n";
  dump << "Position:    " << m_target.Position << "\n";
  dump << "BankState:   " << m_target.BankState << "\n";
  dump << "Identifier:  " << m_target.Identifier << "\n";
  dump << "Description: " << m_target.Description << "\n";
  dump << "DateTime:    " << m_target.DateTime << "\n";
  dump << "MajorVersion:" << m_target.MajorVersion << "\n";
  dump << "MinorVersion:" << m_target.MinorVersion << "\n";
  dump << "AuxVersion:  " << m_target.AuxVersion << "\n";
  
  dump << "Source information\n";
  dump << "------------------\n";
  dump << "SourceUri:    " << m_source.SourceUri << "\n";
  dump << "SourceStatus: " << m_source.SourceStatus << "\n";
  dump << "Identifier:   " << m_source.Identifier << "\n";
  dump << "Description:  " << m_source.Description << "\n";
  dump << "DateTime:     " << m_source.DateTime << "\n";
  dump << "MajorVersion: " << m_source.MajorVersion << "\n";
  dump << "MinorVersion: " << m_source.MinorVersion << "\n";
  dump << "AuxVersion:   " << m_source.AuxVersion << "\n";
  
  dump << "Logical target information:\n";
  dump << "---------------------------\n";
  dump << "FirmwarePersistentLocationCount: " << m_logical.FirmwarePersistentLocationCount << "\n";
  dump << "BankStateFlags:                  " << m_logical.BankStateFlags  << "\n";
  dump << "Pend.InstancePresent:            " << m_logical.PendingFwInstance.InstancePresent << "\n";
  dump << "Pend.Identifier:                 " << m_logical.PendingFwInstance.Identifier << "\n";
  dump << "Pend.Description:                " << m_logical.PendingFwInstance.Description << "\n";
  dump << "Pend.DateTime:                   " << m_logical.PendingFwInstance.DateTime << "\n";
  dump << "Pend.MajorVersion:               " << m_logical.PendingFwInstance.MajorVersion << "\n";
  dump << "Pend.MinorVersion:               " << m_logical.PendingFwInstance.MinorVersion << "\n";
  dump << "Pend.AuxVersion:                 " << m_logical.PendingFwInstance.AuxVersion << "\n";
  dump << "Rollb.InstancePresent:            " << m_logical.RollbackFwInstance.InstancePresent << "\n";
  dump << "Rollb.Identifier:                 " << m_logical.RollbackFwInstance.Identifier << "\n";
  dump << "Rollb.Description:                " << m_logical.RollbackFwInstance.Description << "\n";
  dump << "Rollb.DateTime:                   " << m_logical.RollbackFwInstance.DateTime << "\n";
  dump << "Rollb.MajorVersion:               " << m_logical.RollbackFwInstance.MajorVersion << "\n";
  dump << "Rollb.MinorVersion:               " << m_logical.RollbackFwInstance.MinorVersion << "\n";
  dump << "Rollb.AuxVersion:                 " << m_logical.RollbackFwInstance.AuxVersion << "\n";
  
  dump << "Components: " << "\n";
  for (int i= 0; i < m_comps.Num(); i++) {
      m_comps[i]->Dump( dump );
  }  
}


/**
 * Set the bank id 
 * 
 * @param id value to be set
 * @return bool if successful 
 **/ 
bool NewSimulatorFumiBank::SetId( SaHpiUint8T id ) {
 
   m_target.BankId = id;

   return true;
}


/**
 * Set bank source information 
 * 
 * @param src source information 
 * @return true
 **/ 
bool NewSimulatorFumiBank::SetData( SaHpiFumiSourceInfoT src ) {

   memcpy( &m_source, &src, sizeof( SaHpiFumiSourceInfoT ));
   
   return true;
}


/**
 * Set bank target information 
 * 
 * @param trg bank information 
 * @return true
 **/ 
bool NewSimulatorFumiBank::SetData( SaHpiFumiBankInfoT trg ) {

   memcpy( &m_target, &trg, sizeof( SaHpiFumiBankInfoT ));
   
   return true;
}


/**
 * Set bank logical target information 
 * 
 * @param lgc source information 
 * @return true
 **/ 
bool NewSimulatorFumiBank::SetData( SaHpiFumiLogicalBankInfoT lgc ) {

   memcpy( &m_logical, &lgc, sizeof( SaHpiFumiLogicalBankInfoT ));
   
   return true;
}


/**
 * HPI function saHpiFumiSourceSet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param src text buffer including the source location information
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumiBank::SetSource( SaHpiTextBufferT &src ) {

   memcpy( &m_source_loc, &src, sizeof( SaHpiTextBufferT ));
   
   return SA_OK;
}

/**
 * HPI function saHpiFumiSourceGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param src address of SaHpiFumiSourceInfo structure to be filled
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorFumiBank::GetSource( SaHpiFumiSourceInfoT &src ) {

   memcpy( &src, &m_source, sizeof( SaHpiFumiSourceInfoT ));
   
   return SA_OK;
}


/**
 * HPI function saHpiFumiTargetInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param trg address of bank information to be filled
 * 
 * @return SA_OK
 **/ 
SaErrorT NewSimulatorFumiBank::GetTarget( SaHpiFumiBankInfoT &trg ) {

   memcpy( &trg, &m_target, sizeof( SaHpiFumiBankInfoT ));
   
   return SA_OK;
}


/**
 * HPI function saHpiFumiLogicalTargetInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param trg address of logical bank information to be filled
 * 
 * @return SA_OK
 **/ 
SaErrorT NewSimulatorFumiBank::GetLogicalTarget( SaHpiFumiLogicalBankInfoT &trg ) {

   memcpy( &trg, &m_logical, sizeof( SaHpiFumiLogicalBankInfoT ));
   
   return SA_OK;
}


/**
 * Add or find a component by id
 * 
 * A new NewSimulatorFumiComponent is generated inside the function, 
 * if the component doesn't exist. If a component with the same id already exists, 
 * it is returned.
 * 
 * @param id componentId of NewSimulatorFumiComponent object

 * @return pointer on a NewSimulatorFumiComponent object
 **/
NewSimulatorFumiComponent *NewSimulatorFumiBank::GetComponent( SaHpiUint32T id ) {
   NewSimulatorFumiComponent *c = NULL;
   
   for (int i=0; i < m_comps.Num(); i++) {
      if ( id == m_comps[i]->Num() )
         c = m_comps[i];
   }
   
   if (c == NULL) {
      c = new NewSimulatorFumiComponent();
      m_comps.Add( c );
   }
   
   return c;
}


/**
 * Add source information to a component
 * 
 * The source information is copied from the Component object.
 * 
 * @param component pointer of NewSimulatorFumiComponent object including information
 * @return true
 **/
bool NewSimulatorFumiBank::AddSourceComponent( NewSimulatorFumiComponent *component ) {
   SaHpiUint32T id = component->Num();
   NewSimulatorFumiComponent *comp;
   
   comp = GetComponent( id );
   comp->SetSourceData( component->GetData() );
   
   return true;
}


/**
 * Add target information to a component
 * 
 * The target information is copied from the Component object.
 * 
 * @param component pointer of NewSimulatorFumiComponent object including information 
 * @return true
 **/
bool NewSimulatorFumiBank::AddTargetComponent( NewSimulatorFumiComponent *component ) {
   SaHpiUint32T id = component->Num();
   NewSimulatorFumiComponent *comp;
   
   comp = GetComponent( id );
   comp->SetTargetData( component->GetData() );
   
   return true;
}


/**
 * Add logical target information to a component
 * 
 * The logical target information is copied from the Component object.
 * 
 * @param component pointer of NewSimulatorFumiComponent object including information 
 * @return true
 **/
bool NewSimulatorFumiBank::AddLogicalTargetComponent( NewSimulatorFumiComponent *component ) {
   SaHpiUint32T id = component->Num();
   NewSimulatorFumiComponent *comp;
   
   comp = GetComponent( id );
   comp->SetData( component->GetLogicalData() );
   
   return true;
}



// Official HPI functions
/**
 * HPI function saHpiIdrFieldGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param fieldType Type of Inventory Data Field
 * @param fieldId Identifier of Field to retrieve
 * @param nextId address to store the FieldId of next field
 * @param field address into which the field information is placed
 * 
 * @return HPI return code
 **/


/**
 * Constructor
 **/
NewSimulatorFumiComponent::NewSimulatorFumiComponent() {

   memset( &m_target_info, 0, sizeof( SaHpiFumiComponentInfoT ));
   memset( &m_source_info, 0, sizeof( SaHpiFumiComponentInfoT ));
   memset( &m_logical, 0, sizeof( SaHpiFumiLogicalComponentInfoT ));
}
  
/**
 * Destructor
 **/
NewSimulatorFumiComponent::~NewSimulatorFumiComponent() {
}


/**
 * Set source data 
 * 
 * @param cinf record with source information
 * @return true (simple copy)
 **/ 
bool NewSimulatorFumiComponent::SetSourceData( SaHpiFumiComponentInfoT cinf ) {

   memcpy( &m_source_info, &cinf, sizeof( SaHpiFumiComponentInfoT ));
   return true;
}


/**
 * Set target data 
 * 
 * @param cinf record with target information
 * @return true (simple copy)
 **/ 
bool NewSimulatorFumiComponent::SetTargetData( SaHpiFumiComponentInfoT cinf ) {

   memcpy( &m_target_info, &cinf, sizeof( SaHpiFumiComponentInfoT ));
   return true;
}


/**
 * Set data 
 * 
 * The target data record is taken as default record.
 *
 * @param cinf record with information to be stored
 * @return true (simple copy)
 **/ 
bool NewSimulatorFumiComponent::SetData( SaHpiFumiComponentInfoT cinf ) {

   return SetTargetData( cinf );
}


/**
 * Set logical data 
 * 
 * @param cinf record with logical target information
 * @return true (simple copy)
 **/ 
bool NewSimulatorFumiComponent::SetData( SaHpiFumiLogicalComponentInfoT cinf ) {

   memcpy( &m_logical, &cinf, sizeof( SaHpiFumiLogicalComponentInfoT ));
   return true;
}


/** 
 * Dump the Component information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorFumiComponent::Dump( NewSimulatorLog &dump ) const {

  dump << " Target Component information:\n";
  dump << "   EntryId:         " << m_target_info.EntryId << "\n";
  dump << "   ComponentId:     " << m_target_info.ComponentId << "\n";
  dump << "   InstancePresent: " << m_target_info.MainFwInstance.InstancePresent << "\n";
  dump << "   Identifier:      " << m_target_info.MainFwInstance.Identifier << "\n";
  dump << "   Description:     " << m_target_info.MainFwInstance.Description << "\n";
  dump << "   DateTime:        " << m_target_info.MainFwInstance.DateTime << "\n";
  dump << "   MajorVersion:    " << m_target_info.MainFwInstance.MajorVersion << "\n";
  dump << "   MinorVersion:    " << m_target_info.MainFwInstance.MinorVersion << "\n";
  dump << "   AuxVersion:      " << m_target_info.MainFwInstance.AuxVersion << "\n";
  dump << "   ComponentFlags:  " << m_target_info.ComponentFlags << "\n";
  dump << " Source Component information:\n";
  dump << "   EntryId:         " << m_source_info.EntryId << "\n";
  dump << "   ComponentId:     " << m_source_info.ComponentId << "\n";
  dump << "   InstancePresent: " << m_source_info.MainFwInstance.InstancePresent << "\n";
  dump << "   Identifier:      " << m_source_info.MainFwInstance.Identifier << "\n";
  dump << "   Description:     " << m_source_info.MainFwInstance.Description << "\n";
  dump << "   DateTime:        " << m_source_info.MainFwInstance.DateTime << "\n";
  dump << "   MajorVersion:    " << m_source_info.MainFwInstance.MajorVersion << "\n";
  dump << "   MinorVersion:    " << m_source_info.MainFwInstance.MinorVersion << "\n";
  dump << "   AuxVersion:      " << m_source_info.MainFwInstance.AuxVersion << "\n";
  dump << "   ComponentFlags:  " << m_source_info.ComponentFlags << "\n";
  dump << " Logical Component information:\n"; 
  dump << "   EntryId:                         " << m_logical.EntryId  << "\n";
  dump << "   ComponentId:                     " << m_logical.ComponentId  << "\n";
  dump << "   Pend.InstancePresent:            " << m_logical.PendingFwInstance.InstancePresent << "\n";
  dump << "   Pend.Identifier:                 " << m_logical.PendingFwInstance.Identifier << "\n";
  dump << "   Pend.Description:                " << m_logical.PendingFwInstance.Description << "\n";
  dump << "   Pend.DateTime:                   " << m_logical.PendingFwInstance.DateTime << "\n";
  dump << "   Pend.MajorVersion:               " << m_logical.PendingFwInstance.MajorVersion << "\n";
  dump << "   Pend.MinorVersion:               " << m_logical.PendingFwInstance.MinorVersion << "\n";
  dump << "   Pend.AuxVersion:                 " << m_logical.PendingFwInstance.AuxVersion << "\n";
  dump << "   Rollb.InstancePresent:            " << m_logical.RollbackFwInstance.InstancePresent << "\n";
  dump << "   Rollb.Identifier:                 " << m_logical.RollbackFwInstance.Identifier << "\n";
  dump << "   Rollb.Description:                " << m_logical.RollbackFwInstance.Description << "\n";
  dump << "   Rollb.DateTime:                   " << m_logical.RollbackFwInstance.DateTime << "\n";
  dump << "   Rollb.MajorVersion:               " << m_logical.RollbackFwInstance.MajorVersion << "\n";
  dump << "   Rollb.MinorVersion:               " << m_logical.RollbackFwInstance.MinorVersion << "\n";
  dump << "   Rollb.AuxVersion:                 " << m_logical.RollbackFwInstance.AuxVersion << "\n";
  dump << "   ComponentFlags:                   " << m_logical.ComponentFlags << "\n";
}

