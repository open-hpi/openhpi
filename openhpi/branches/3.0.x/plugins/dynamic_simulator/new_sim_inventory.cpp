/** 
 * @file    new_sim_inventory.cpp
 *
 * The file includes a class for inventory handling:\n
 * NewSimulatorInventory
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
#include "new_sim_inventory.h"
#include "new_sim_inventory_data.h"
#include "new_sim_entity.h"


/**
 * Constructor
 **/
NewSimulatorInventory::NewSimulatorInventory( NewSimulatorResource *res )
  : NewSimulatorRdr( res, SAHPI_INVENTORY_RDR ), 
    m_area_id( 0 ) {
    	
   memset( &m_inv_rec, 0, sizeof( SaHpiInventoryRecT ));
   memset( &m_inv_info, 0, sizeof( SaHpiIdrInfoT ));
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorInventory::NewSimulatorInventory( NewSimulatorResource *res,
                                        SaHpiRdrT rdr)
  : NewSimulatorRdr( res, SAHPI_INVENTORY_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_area_id( 0 ) {

   memcpy(&m_inv_rec, &rdr.RdrTypeUnion.InventoryRec, sizeof( SaHpiInventoryRecT ));
   memset( &m_inv_info, 0, sizeof( SaHpiIdrInfoT ));
}

/**
 * Full qualified constructor to fill an object with the parsed data including idr_info
 **/
NewSimulatorInventory::NewSimulatorInventory( NewSimulatorResource *res,
                                        SaHpiRdrT rdr, 
                                        SaHpiIdrInfoT inv_info)
  : NewSimulatorRdr( res, SAHPI_INVENTORY_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_area_id( 0 ) {

   memcpy(&m_inv_rec, &rdr.RdrTypeUnion.InventoryRec, sizeof( SaHpiInventoryRecT ));
   memcpy(&m_inv_info, &inv_info, sizeof( SaHpiIdrInfoT ));
}
  
                      
/**
 * Destructor
 **/
NewSimulatorInventory::~NewSimulatorInventory() {
   m_areas.RemAll();
}


/** 
 * Dump the Inventory information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorInventory::Dump( NewSimulatorLog &dump ) const {

  dump << "Inventory: " << m_inv_rec.IdrId << "\n";
  dump << "Persistent: " << m_inv_rec.Persistent << "\n";
  dump << "Oem: " << m_inv_rec.Oem << "\n";
  dump << "Area(s): " << "\n";
  dump << "-------------------\n";
  for (int i= 0; i < m_areas.Num(); i++) {
      m_areas[i]->Dump( dump );
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
bool NewSimulatorInventory::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
  
  if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
       return false;

  // Inventory record
  memcpy(&rdr.RdrTypeUnion.InventoryRec, &m_inv_rec, sizeof(SaHpiInventoryRecT));

  return true;
}


/**
 * Find area by area pointer
 * 
 * @param area pointer on the inventory area to be found 
 * @return return the same pointer if it could be found successfully 
 **/
NewSimulatorInventoryArea *NewSimulatorInventory::FindInventoryArea( 
                                           NewSimulatorInventoryArea *area ) {
	
   for( int i = 0; i < m_areas.Num(); i++ ) {
      NewSimulatorInventoryArea *ia = m_areas[i];
      if ( ia == area ) return area;
   }

   return 0;
}


/**
 * Add a inventory area to the array if it isn't already included in the array
 * 
 * @param area pointer to area to be added
 * @return bool if successful 
 **/ 
bool NewSimulatorInventory::AddInventoryArea( NewSimulatorInventoryArea *area ) {
	
   if ( FindInventoryArea( area ) ) {
      return false;
   }
   
   if (area->Num() > m_area_id)
      m_area_id = area->Num();
      
   m_areas.Add( area );
   m_inv_info.NumAreas = m_areas.Num();

   return true;
}


/**
 * Set inventory record data 
 * 
 * @param idrRec InventoryRecord data
 * @return true (simple copy)
 **/ 
bool NewSimulatorInventory::SetData( SaHpiInventoryRecT idrRec ) {
   
   memcpy( &m_inv_rec, &idrRec, sizeof( SaHpiInventoryRecT ));

   return true;
}


/**
 * Set inventory information data 
 * The fields UpdateCount and NumAreas are filled with internal values and are not 
 * overwritten.
 * 
 * @param idrInfo record with IdrInfo data
 * @return true 
 **/
bool NewSimulatorInventory::SetInfo( SaHpiIdrInfoT idrInfo ) {
   
   m_inv_info.IdrId = idrInfo.IdrId;
   m_inv_info.ReadOnly = idrInfo.ReadOnly;
   
   // UpdateCount is incremented if an official HPI function is called    
   // NumAreas is filled with the own value
   m_inv_info.NumAreas = m_areas.Num();
   
   return true;
}


// Official HPI functions
 
 /**
 * HPI function saHpiIdrInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param idrInfo address of the IdrInfo record to be filled
 * 
 * @return HPI return code
 **/    
SaErrorT NewSimulatorInventory::GetIdrInfo( SaHpiIdrInfoT &idrInfo ) {
   
   if ( &idrInfo == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   m_inv_info.NumAreas = m_areas.Num();
   memcpy( &idrInfo, &m_inv_info, sizeof( SaHpiIdrInfoT ));

   return SA_OK;
}


/**
 * HPI function saHpiIdrAreaHeaderGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param type type of area to be found
 * @param areaId Id of area to be found
 * @param nextId address to copy the Id of the next area header
 * @param header address to be filled with the area header record
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::GetAreaHeader( SaHpiIdrAreaTypeT type,
                                               SaHpiEntryIdT areaId,
                                               SaHpiEntryIdT &nextId, 
                                               SaHpiIdrAreaHeaderT &header) {
   bool found = false, foundId = false, foundType = false;
   
   if ((areaId == SAHPI_LAST_ENTRY) ||
        (&nextId == NULL) || (&header == NULL)) 
      return SA_ERR_HPI_INVALID_PARAMS;

   for (int i = 0; i < m_areas.Num(); i++) {
      if ( (areaId == SAHPI_FIRST_ENTRY) ||
            (areaId == m_areas[i]->Num()) )
         foundId = true;
      
      if ( (type == SAHPI_IDR_AREATYPE_UNSPECIFIED) ||
            (type ==  m_areas[i]->Type()) ) 
         foundType = true;

      if (found) {
        nextId = m_areas[i]->Num();
        return SA_OK; 
      }
      
      if (foundType && foundId) {
         if (!found) {
            found = true;
            memcpy( &header, &m_areas[i]->AreaHeader(), sizeof( SaHpiIdrAreaHeaderT ));
         }
      }
      foundId = false;
      foundType = false;
   }
   if (found) {
      nextId = SAHPI_LAST_ENTRY;
      return SA_OK;
   }
   
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * HPI function saHpiIdrAreaAdd()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param type type of area to add
 * @param newId address to copy the Id of the new area header
 * 
 * @return HPI return code
 **/
SaErrorT  NewSimulatorInventory::AddArea( SaHpiIdrAreaTypeT type, 
                                          SaHpiEntryIdT &newId ) {

   if ( IsReadOnly() )
      return SA_ERR_HPI_READ_ONLY;
      
   if ( &newId == NULL ) 
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if ( type == SAHPI_IDR_AREATYPE_UNSPECIFIED )
      return SA_ERR_HPI_INVALID_DATA;
   
   if ( !(( type == SAHPI_IDR_AREATYPE_INTERNAL_USE ) ||
           ( type == SAHPI_IDR_AREATYPE_CHASSIS_INFO ) ||
           ( type == SAHPI_IDR_AREATYPE_BOARD_INFO ) ||
           ( type == SAHPI_IDR_AREATYPE_PRODUCT_INFO ) || 
           ( type == SAHPI_IDR_AREATYPE_OEM )) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   // Ok, we can add a new area
   SaHpiIdrAreaHeaderT ah;
   ah.AreaId    = ValidAreaId();
   ah.Type      = type;
   ah.ReadOnly  = SAHPI_FALSE;
   ah.NumFields = 0;
   
   NewSimulatorInventoryArea *ida = new NewSimulatorInventoryArea( ah );
   if ( AddInventoryArea( ida ) ) {
   	   newId = ida->Num();
   	   IncUpdateCount();
   	   return SA_OK;

   } else {
   	   return SA_ERR_HPI_INVALID_DATA;
   }
}

/**
 * HPI function saHpiIdrAreaAddById()
 *
 * See also the description of the function inside the specification or header file.
 * 
 * @param type Type of Area to add
 * @param id AreaId for the new Area to be created
 * 
 * @return HPI return code
 **/  
SaErrorT NewSimulatorInventory::AddAreaById( SaHpiIdrAreaTypeT type, 
                                             SaHpiEntryIdT id ) {
                                             	
   // Ok, we can try to add a new area
   stdlog << "DBG: NewSimulatorInventory::AddAreaById Try to add a new area by id.\n";
   
   if ( IsReadOnly() )
      return SA_ERR_HPI_READ_ONLY;
   
   if ( type == SAHPI_IDR_AREATYPE_UNSPECIFIED )
      return SA_ERR_HPI_INVALID_DATA;
   
   if ( !(( type == SAHPI_IDR_AREATYPE_INTERNAL_USE ) ||
           ( type == SAHPI_IDR_AREATYPE_CHASSIS_INFO ) ||
           ( type == SAHPI_IDR_AREATYPE_BOARD_INFO ) ||
           ( type == SAHPI_IDR_AREATYPE_PRODUCT_INFO ) || 
           ( type == SAHPI_IDR_AREATYPE_OEM )) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   if ( id == SAHPI_LAST_ENTRY )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   SaHpiIdrAreaHeaderT ah;
   NewSimulatorInventoryArea *ida;
   
   ah.Type      = type;
   ah.ReadOnly  = SAHPI_FALSE;
   ah.NumFields = 0;
   
   if ( id == SAHPI_FIRST_ENTRY ) {
     ah.AreaId    = ValidAreaId();
     ida = new NewSimulatorInventoryArea( ah );
     m_areas.Insert( 0, ida );
     IncUpdateCount();
     stdlog << "DBG: Area was added with id " << ah.AreaId << "\n";
     
     return SA_OK;	  
   } 
   
   ah.AreaId    = id;
   for (int i=0; i < m_areas.Num(); i++) {
      if ( m_areas[i]->Num() == id )
         return SA_ERR_HPI_DUPLICATE;
   }

   ida = new NewSimulatorInventoryArea( ah );
   if ( AddInventoryArea( ida ) ) {
   	   IncUpdateCount();
   	   return SA_OK;
   	   
   } else {
   	   return SA_ERR_HPI_INVALID_DATA;
   }
}


/**
 * HPI function saHpiIdrAreaDel()
 *
 * See also the description of the function inside the specification or header file.
 * 
 * @param id Identifier of Area entry to delete from the IDR 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::DeleteArea( SaHpiEntryIdT id ) {
   NewSimulatorInventoryArea *ida = NULL;
   int idx = -1;
   
   if ( IsReadOnly() )
      return SA_ERR_HPI_READ_ONLY;
   
   if ( id == SAHPI_LAST_ENTRY )
      return SA_ERR_HPI_INVALID_PARAMS;

   // Find the correct area          
   if ( id == SAHPI_FIRST_ENTRY ) {
      ida = m_areas[0];
      idx = 0;
   } else {
      for (int i=0; i < m_areas.Num(); i++) {
         if (m_areas[i]->Num() == id) {
            ida = m_areas[i];
            idx = i;
            break;
         }
      }
      
      if ( ida == NULL )
         return SA_ERR_HPI_NOT_PRESENT;
   }
   
   if ( ida->IsReadOnly() )
      return SA_ERR_HPI_READ_ONLY;
   
   if ( ida->IncludesReadOnlyField() )
      return SA_ERR_HPI_READ_ONLY;
      
   ida->DeleteFields();
   m_areas.Rem( idx );
   IncUpdateCount();
   
   return SA_OK;
}


/**
 * Start for HPI function saHpiIdrFieldGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Inside the method the correct AreaId is identified and afterwards 
 * NewSimulatorInventoryArea::GetField() is called.
 * 
 * @param areaId  Identifier for the IDA
 * @param fieldType Type of Inventory Data Field
 * @param fieldId Identifier of Field to retrieve
 * @param nextId address to store the FieldId of next field
 * @param field address into which the field information is placed
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::GetField( SaHpiEntryIdT areaId,
                                          SaHpiIdrFieldTypeT fieldType, 
                                          SaHpiEntryIdT fieldId, 
                                          SaHpiEntryIdT &nextId, 
                                          SaHpiIdrFieldT &field ) {
   
   if ( ( areaId == SAHPI_LAST_ENTRY ) ||
         ( fieldId == SAHPI_LAST_ENTRY ) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   for (int i=0; i < m_areas.Num(); i++) {
      if ( ( m_areas[i]->Num() == areaId ) ||
            ( areaId == SAHPI_FIRST_ENTRY ) )
         return m_areas[i]->GetField( fieldType, fieldId, nextId, field );
   }
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * Start for HPI function saHpiIdrFieldAdd()
 * 
 * See also the description of the function inside the specification or header file.
 * Inside the method the correct AreaId is identified and afterwards 
 * NewSimulatorInventoryArea::AddField() is called
 * 
 * @param field  address to the field to add to the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::AddField( SaHpiIdrFieldT &field ) {
   
   int i;
   SaErrorT rv;
   
   if ( ( field.AreaId == SAHPI_LAST_ENTRY ) ||
         ( field.FieldId == SAHPI_LAST_ENTRY ) )
      return SA_ERR_HPI_INVALID_PARAMS;
            
   for (i=0; i < m_areas.Num(); i++) {
      if ( ( m_areas[i]->Num() == field.AreaId ) ||
            ( field.AreaId == SAHPI_FIRST_ENTRY ) ) {
         
         if (  m_areas[i]->IsReadOnly() )
            return SA_ERR_HPI_READ_ONLY;
         
         rv = m_areas[i]->AddField( field );
         if ( rv == SA_OK )
            IncUpdateCount();  	

         return rv;
      }
   }
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * Start for HPI function saHpiIdrFieldAddById()
 * 
 * See also the description of the function inside the specification or header file.
 * Inside the method the correct AreaId is identified and afterwards 
 * NewSimulatorInventoryArea::AddFieldById() is called
 * 
 * @param field address of field to add to the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::AddFieldById( SaHpiIdrFieldT &field ) {
   SaErrorT rv;
   int i;
   
   if ( ( field.AreaId == SAHPI_LAST_ENTRY ) ||
         ( field.FieldId == SAHPI_LAST_ENTRY ) )
      return SA_ERR_HPI_INVALID_PARAMS;
   
   for (i=0; i < m_areas.Num(); i++) {
      if ( ( m_areas[i]->Num() == field.AreaId ) ||
            ( field.AreaId == SAHPI_FIRST_ENTRY ) ) {
         
         if (  m_areas[i]->IsReadOnly() )
            return SA_ERR_HPI_READ_ONLY;
         
         rv = m_areas[i]->AddFieldById( field );
         if ( rv == SA_OK )
            IncUpdateCount();   	

         return rv;
      }
   }
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * Start for HPI function saHpiIdrFieldSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Inside the method the correct AreaId is identified and afterwards 
 * NewSimulatorInventoryArea::SetField() is called
 * 
 * @param field field to set in the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::SetField( SaHpiIdrFieldT field ) {
   SaErrorT rv;
   int i;
   
   if ( ( field.AreaId == SAHPI_LAST_ENTRY ) ||
         ( field.FieldId == SAHPI_LAST_ENTRY ) )
      return SA_ERR_HPI_INVALID_PARAMS;   
   
   for (i=0; i < m_areas.Num(); i++) {
      if ( ( m_areas[i]->Num() == field.AreaId ) ||
            ( field.AreaId == SAHPI_FIRST_ENTRY ) ) {
      
         rv = m_areas[i]->SetField( field );
         if ( rv == SA_OK )
            IncUpdateCount();

         return rv;
      }
   }
   return SA_ERR_HPI_NOT_PRESENT;

}


/**
 * Start for HPI function saHpiIdrFieldDelete()
 * 
 * See also the description of the function inside the specification or header file.
 * Inside the method the correct AreaId is identified and afterwards 
 * NewSimulatorInventoryArea::DeleteField() is called
 * 
 * @param areaId identifier for the IDA
 * @param fieldId Identifier of Field to delete from the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventory::DeleteField( SaHpiEntryIdT areaId, SaHpiEntryIdT fieldId ) {
   SaErrorT rv;
   
   if ( ( areaId == SAHPI_LAST_ENTRY ) ||
        ( fieldId == SAHPI_LAST_ENTRY ) )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   for (int i=0; i < m_areas.Num(); i++) {
      if ( ( m_areas[i]->Num() == areaId ) ||
            ( areaId == SAHPI_FIRST_ENTRY ) ) {
         
         if ( m_areas[i]->IsReadOnly() )
            return SA_ERR_HPI_READ_ONLY;
         
         rv = m_areas[i]->DeleteField( fieldId );
         if ( rv == SA_OK )
            IncUpdateCount();   

         return rv;
      }
   }
   
   return SA_ERR_HPI_NOT_PRESENT;

}

