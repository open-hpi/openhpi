/** 
 * @file    new_sim_inventory.h
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
 *    
 */

#ifndef __NEW_SIM_INVENTORY_H__
#define __NEW_SIM_INVENTORY_H__


extern "C" {
#include "SaHpi.h"
}


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_INVENTORY_DATA_H__
#include "new_sim_inventory_data.h"
#endif

class  NewSimulatorDomain;

/**
 * @class NewSimulatorInventory
 * 
 * Class for simulating Inventory
 * 
 **/
class NewSimulatorInventory : public NewSimulatorRdr {

private:
  /// Record with the Inventory rdr information
  SaHpiInventoryRecT        m_inv_rec;
  /// Record with the IdrInfo record
  SaHpiIdrInfoT             m_inv_info;
  
  /// Array including the areas
  cArray<NewSimulatorInventoryArea>  m_areas;
  /// Valid areaId
  SaHpiEntryIdT m_area_id;
  
public:
  NewSimulatorInventory( NewSimulatorResource *res );
  NewSimulatorInventory( NewSimulatorResource *res, SaHpiRdrT rdr );
  NewSimulatorInventory( NewSimulatorResource *res,
                           SaHpiRdrT rdr,
                           SaHpiIdrInfoT inv_info );
                      
  virtual ~NewSimulatorInventory();

  
  /// Return Inventory Id
  virtual unsigned int Num() const { return ( unsigned int ) m_inv_rec.IdrId; }

  /// Return a new entryId
  SaHpiEntryIdT ValidAreaId() { return ++m_area_id; }

  /// Return the ReadOnly flag
  bool IsReadOnly() { return (bool) m_inv_info.ReadOnly; }

  /// Increment Update Counter
  void IncUpdateCount() { m_inv_info.UpdateCount++; }
  
  // Find and add Area
  NewSimulatorInventoryArea *FindInventoryArea( NewSimulatorInventoryArea *area );
  bool AddInventoryArea( NewSimulatorInventoryArea *area );
  bool SetData( SaHpiInventoryRecT idrRec );
  bool SetInfo( SaHpiIdrInfoT idrInfo );

  // create a RDR record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );
  
  // Official HPI functions
  SaErrorT  GetIdrInfo( SaHpiIdrInfoT &idrInfo );
  SaErrorT  GetAreaHeader( SaHpiIdrAreaTypeT type, SaHpiEntryIdT areaId,
                           SaHpiEntryIdT &nextId, SaHpiIdrAreaHeaderT &header);
  SaErrorT  AddArea( SaHpiIdrAreaTypeT type, SaHpiEntryIdT &newId );
  SaErrorT  AddAreaById( SaHpiIdrAreaTypeT type, SaHpiEntryIdT id );
  SaErrorT  DeleteArea( SaHpiEntryIdT id );
  
  // Identify the correct area and call the HPI functions of the area afterwards
  SaErrorT  GetField( SaHpiEntryIdT areaId, SaHpiIdrFieldTypeT fieldType,
                      SaHpiEntryIdT fieldId, SaHpiEntryIdT &nextId,
                      SaHpiIdrFieldT &field );
  SaErrorT  AddField( SaHpiIdrFieldT &field );
  SaErrorT  AddFieldById( SaHpiIdrFieldT &field );
  SaErrorT  SetField( SaHpiIdrFieldT field );
  SaErrorT  DeleteField( SaHpiEntryIdT areaId, SaHpiEntryIdT fieldId );
  
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
