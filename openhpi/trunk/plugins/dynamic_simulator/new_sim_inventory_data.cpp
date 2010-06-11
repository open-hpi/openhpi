/** 
 * @file    new_sim_inventory_data.cpp
 *
 * The file includes a class for inventory data handling:\n
 * NewSimulatorInventoryArea
 * NewSimulatorInventoryField
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
NewSimulatorInventoryArea::NewSimulatorInventoryArea( )
  : m_field_id( 0 ) {
    	
   memset( &m_area_header, 0, sizeof( SaHpiIdrAreaHeaderT ));
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorInventoryArea::NewSimulatorInventoryArea( SaHpiIdrAreaHeaderT area )
  : m_field_id( 0 ) {

   memcpy(&m_area_header, &area, sizeof( SaHpiIdrAreaHeaderT ));
}
  
                      
/**
 * Destructor
 **/
NewSimulatorInventoryArea::~NewSimulatorInventoryArea() {
   m_fields.RemAll();
}


/** 
 * Dump the Inventory Area information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorInventoryArea::Dump( NewSimulatorLog &dump ) const {

  dump << "Area: " << m_area_header.AreaId << "\n";
  dump << "Type: " << m_area_header.Type << "\n";
  dump << "ReadOnly; " << m_area_header.ReadOnly << "\n";
  dump << "Area: " << "\n";
  for (int i= 0; i < m_fields.Num(); i++) {
      m_fields[i]->Dump( dump );
  }
}

/**
 * Find field by field pointer
 * 
 * @param field pointer on the inventory field to be found 
 * @return return the same pointer if it could be found  
 **/
NewSimulatorInventoryField *NewSimulatorInventoryArea::FindInventoryField( 
                                           NewSimulatorInventoryField *field ) {
	
   for( int i = 0; i < m_fields.Num(); i++ ) {
      NewSimulatorInventoryField *idf = m_fields[i];
      if ( idf == field ) return field;
   }

   return 0;
}


/**
 * Add a inventory field to the array if it isn't already included in the array
 * 
 * @param field pointer to field to be added
 * @return bool if successful 
 **/ 
bool NewSimulatorInventoryArea::AddInventoryField( NewSimulatorInventoryField *field ) {
	
   if ( FindInventoryField( field ) ) {
      return false;
   }
   
   if (field->Num() > m_field_id)
      m_field_id = field->Num();
      
   m_fields.Add( field );
   m_area_header.NumFields = m_fields.Num();

   return true;
}

/**
 * Check the Inventory fields if one field is set to ReadOnly
 * 
 * @return bool if one field is set to read only 
 **/
bool NewSimulatorInventoryArea::IncludesReadOnlyField() {
	
   for( int i = 0; i < m_fields.Num(); i++ ) {
      if ( m_fields[i]->IsReadOnly() )
         return true;
   }
   
   return false;
}

/**
 * Delete all fields from the fields array 
 **/
void NewSimulatorInventoryArea::DeleteFields() {
   
   m_fields.RemAll();
}


/**
 * Set area header information 
 * The field NumAreas is filled with a internal values and will not be 
 * overwritten.
 * 
 * @param aheader record with AreaHeader data
 * @return true 
 **/
bool NewSimulatorInventoryArea::SetData(SaHpiIdrAreaHeaderT aheader) {

   m_area_header.AreaId = aheader.AreaId;
   m_area_header.Type = aheader.Type;
   m_area_header.ReadOnly = aheader.ReadOnly;
   m_area_header.NumFields = m_fields.Num();
   
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
SaErrorT NewSimulatorInventoryArea::GetField( SaHpiIdrFieldTypeT fieldType, 
                                              SaHpiEntryIdT fieldId, 
                                              SaHpiEntryIdT &nextId, 
                                              SaHpiIdrFieldT &field ) {

   bool found = false, foundId = false, foundType = false;
   
   if ( fieldId == SAHPI_LAST_ENTRY )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if ( &nextId == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if ( &field == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;

   
   for (int i = 0; i < m_fields.Num(); i++) {
      if ( (fieldId == SAHPI_FIRST_ENTRY) ||
            (fieldId == m_fields[i]->Num()) )
         foundId = true;
      
      if ( (fieldType == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) ||
            (fieldType ==  m_fields[i]->Type()) ) 
         foundType = true;

      if (found) {
        nextId = m_fields[i]->Num();
        return SA_OK; 
      }
      
      if (foundType && foundId) {
         if (!found) {
            found = true;
            memcpy( &field, &m_fields[i]->FieldData(), sizeof( SaHpiIdrFieldT ));
         }
         foundId = false;
         foundType = false;
      }
   }
   if (found) {
      nextId = SAHPI_LAST_ENTRY;
      return SA_OK;
   }
   
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * HPI function saHpiIdrFieldAdd()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param field address of Inventory Data Field, which contains the new field information to add
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventoryArea::AddField( SaHpiIdrFieldT &field ) {
   	
   	if ( field.Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED )
   	   return SA_ERR_HPI_INVALID_PARAMS;
   	   
   	field.FieldId = ValidFieldId();
   	field.ReadOnly = SAHPI_FALSE;
   	
   	NewSimulatorInventoryField *idf = new NewSimulatorInventoryField( field );
   	
   	if ( AddInventoryField( idf ) )
   	   return SA_OK;
   	
   	return SA_ERR_HPI_INVALID_DATA;
}


/**
 * HPI function saHpiIdrFieldAddById()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param field address of Inventory Data Field, which contains the new field information to add
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventoryArea::AddFieldById( SaHpiIdrFieldT &field ) {
   
   if ( field.Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED )
   	   return SA_ERR_HPI_INVALID_PARAMS;

   // Ok, we can try to add the field
   NewSimulatorInventoryField *idf;
   
   field.ReadOnly = SAHPI_FALSE;
   
   if ( field.FieldId == SAHPI_FIRST_ENTRY ) {
      field.FieldId    = ValidFieldId();
      idf = new NewSimulatorInventoryField( field );
      m_fields.Insert( 0, idf );

      return SA_OK;	  
   } 
   
   for (int i=0; i < m_fields.Num(); i++) {
      if ( m_fields[i]->Num() == field.FieldId )
         return SA_ERR_HPI_DUPLICATE;
   }

   idf = new NewSimulatorInventoryField( field );
   if ( AddInventoryField( idf ) ) {
   	   return SA_OK;
   } else {
   	   return SA_ERR_HPI_INVALID_DATA;
   }
     	
}


/**
 * HPI function saHpiIdrFieldSet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param field field to set in the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventoryArea::SetField( SaHpiIdrFieldT field ) {
   
   if ( field.Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED )
   	   return SA_ERR_HPI_INVALID_PARAMS;   
   
   for (int i=0; i < m_fields.Num(); i++) {
      if ( m_fields[i]->Num() == field.FieldId ) {

         if ( m_fields[i]->IsReadOnly() )
            return SA_ERR_HPI_READ_ONLY;
         
         SaHpiIdrFieldT *data = &m_fields[i]->FieldData();
         data->Type = field.Type;
         memcpy( &data->Field, &field.Field, sizeof( SaHpiTextBufferT ));
         return SA_OK; 
      }
   }
   
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * HPI function saHpiIdrFieldDelete()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param fieldId Identifier of Field to delete from the IDA
 *
 * @return HPI return code
 **/
SaErrorT NewSimulatorInventoryArea::DeleteField( SaHpiEntryIdT fieldId ) {
   
   for (int i=0; i < m_fields.Num(); i++) {
      if ( ( m_fields[i]->Num() == fieldId ) ||
            ( fieldId == SAHPI_FIRST_ENTRY ) ) {
         
         if ( m_fields[i]->IsReadOnly() )
            return SA_ERR_HPI_READ_ONLY;
         
         m_fields.Rem( i );
         return SA_OK;
      }
   }
   
   return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * Constructor
 **/
NewSimulatorInventoryField::NewSimulatorInventoryField() {
}
  
/**
 * Full qualified Constructor
 **/
NewSimulatorInventoryField::NewSimulatorInventoryField( SaHpiIdrFieldT field ) {

   memcpy( &m_field, &field, sizeof( SaHpiIdrFieldT ));
}

                     
/**
 * Destructor
 **/
NewSimulatorInventoryField::~NewSimulatorInventoryField() {
}


/**
 * Set field data 
 * 
 * @param field IdrField data
 * @return true (simple copy)
 **/ 
bool NewSimulatorInventoryField::SetData( SaHpiIdrFieldT field ) {

   memcpy( &m_field, &field, sizeof( SaHpiIdrFieldT ));
   return true;
}


/** 
 * Dump the Inventory Field information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorInventoryField::Dump( NewSimulatorLog &dump ) const {

  dump << "   Field.AreaID: " << m_field.AreaId << "\n";
  dump << "   Field.FieldID: " << m_field.FieldId << "\n";
  dump << "   Type: " << m_field.Type << "\n";
  dump << "   ReadOnly; " << m_field.ReadOnly << "\n";
  
  NewSimulatorTextBuffer tmp( m_field.Field );
  char str[256];
  tmp.GetAscii( str, 256 );
  dump << "   Field: " << str << "\n";
}

