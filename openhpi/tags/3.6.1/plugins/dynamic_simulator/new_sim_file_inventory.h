/** 
 * @file    new_sim_file_inventory.h
 *
 * The file includes helper class for parsing inventory data from the simulation file:\n
 * NewSimulatorFileInventory
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
 
#ifndef __NEW_SIM_FILE_INVENTORY_H__
#define __NEW_SIM_FILE_INVENTORY_H__

#include <glib.h>

extern "C" {
#include "SaHpi.h"
}

#ifndef __NEW_SIM_FILE_RDR_H__
#include "new_sim_file_rdr.h"
#endif

#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_INVENTORY_H__
#include "new_sim_inventory.h"
#endif

#ifndef __NEW_SIM_INVENTORY_DATA_H__
#include "new_sim_inventory_data.h"
#endif

/**
 * @class NewSimulatorFileInventory
 * 
 * Provides some functions for parsing the inventory section of the simulation file.
 **/
class NewSimulatorFileInventory : public NewSimulatorFileRdr {
   private:
   /// static control rdr information
   SaHpiInventoryRecT        *m_idr_rec;
   
   bool process_idr_data( NewSimulatorInventory *idr );
   bool process_idr_area( NewSimulatorInventoryArea *ida );
   bool process_idr_field( NewSimulatorInventoryField *idf );
   
   public:
   NewSimulatorFileInventory(GScanner *scanner);
   virtual ~NewSimulatorFileInventory();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_INVENTORY_H_*/
