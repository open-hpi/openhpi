/** 
 * @file    new_sim_inventory_data.h
 *
 * The file includes the classes for inventory area and field handling:\n
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
 *    
 */

#ifndef __NEW_SIM_INVENTORY_DATA_H__
#define __NEW_SIM_INVENTORY_DATA_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

class  NewSimulatorDomain;

/**
 * @class NewSimulatorInventoryField
 * 
 * Class for simulating Inventory fields
 * 
 **/
class NewSimulatorInventoryField {
   private:
   /// Record with the Inventory Field Information
   SaHpiIdrFieldT  m_field;
   
   public:
   NewSimulatorInventoryField();
   NewSimulatorInventoryField( SaHpiIdrFieldT field );
   virtual ~NewSimulatorInventoryField();
   
   /// Return Inventory Area Number
   virtual SaHpiEntryIdT Num() const { return m_field.FieldId; }
   /// Return Field Type
   virtual SaHpiIdrFieldTypeT Type() const { return m_field.Type; }
   /// Return ReadOnly Flag
   bool IsReadOnly() const { return (bool) m_field.ReadOnly; }
   
   /// Return address of field data
   SaHpiIdrFieldT &FieldData() { return m_field; }
   
   virtual bool SetData( SaHpiIdrFieldT field );
   virtual void Dump( NewSimulatorLog &dump ) const;
};


/**
 * @class NewSimulatorInventoryArea
 * 
 * Class for simulating InventoryArea
 * 
 **/
class NewSimulatorInventoryArea {

private:
  /// Record with the AreaHeader information
  SaHpiIdrAreaHeaderT       m_area_header;
  
  /// Array including the areas
  cArray<NewSimulatorInventoryField>  m_fields;
  /// Valid fieldId
  SaHpiEntryIdT m_field_id;
  
public:
  NewSimulatorInventoryArea( );
  NewSimulatorInventoryArea( SaHpiIdrAreaHeaderT area );
                      
  virtual ~NewSimulatorInventoryArea();
  
  /// Return Inventory AreaId
  virtual SaHpiEntryIdT Num() const { return m_area_header.AreaId; }
  /// Return Area Type
  virtual SaHpiIdrAreaTypeT Type() const { return m_area_header.Type; }
  /// Return a new entryId
  SaHpiEntryIdT ValidFieldId() { return ++m_field_id; }
  /// Return the ReadOnly flag
  bool IsReadOnly() { return (bool) m_area_header.ReadOnly; }
 
  /// Return the complete header information
  SaHpiIdrAreaHeaderT &AreaHeader() { m_area_header.NumFields = m_fields.Num();
  	                                  return m_area_header; }

  // Find and add inventory field
  NewSimulatorInventoryField *FindInventoryField( NewSimulatorInventoryField *field );
  bool AddInventoryField( NewSimulatorInventoryField *field );
  bool IncludesReadOnlyField();
  void DeleteFields();
  bool SetData( SaHpiIdrAreaHeaderT aheader );
  
  // methods for HPI functions
  SaErrorT  GetField( SaHpiIdrFieldTypeT fieldType, SaHpiEntryIdT fieldId, 
                      SaHpiEntryIdT &nextId, SaHpiIdrFieldT &field );
  SaErrorT  AddField( SaHpiIdrFieldT &field );
  SaErrorT  AddFieldById( SaHpiIdrFieldT &field );
  SaErrorT  SetField( SaHpiIdrFieldT field );
  SaErrorT  DeleteField( SaHpiEntryIdT fieldId );
  
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};

#endif

