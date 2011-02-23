/** 
 * @file    new_sim_file_fumi.h
 *
 * The file includes helper classes for parsing fumi data from the simulation file:\n
 * NewSimulatorFileFumi
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
 
#ifndef __NEW_SIM_FILE_FUMI_H__
#define __NEW_SIM_FILE_FUMI_H__

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

#ifndef __NEW_SIM_FUMI_H__
#include "new_sim_fumi.h"
#endif

#ifndef __NEW_SIM_FUMI_DATA_H__
#include "new_sim_fumi_data.h"
#endif

/**
 * @class NewSimulatorFileFumi
 * 
 * Provides some functions for parsing the fumi section of the simulation file.
 **/
class NewSimulatorFileFumi : public NewSimulatorFileRdr {
   private:
   /// static control rdr information
   SaHpiFumiRecT        *m_fumi_rec;
   
   bool process_fumi_data( NewSimulatorFumi *fumi );
   bool process_fumi_source_info( NewSimulatorFumiBank *bank );
   bool process_fumi_component( NewSimulatorFumiComponent *comp );
   bool process_fumi_firmware( SaHpiFumiFirmwareInstanceInfoT &fw );
   bool process_fumi_target_info( NewSimulatorFumiBank *bank );
   bool process_fumi_logical_target_info( NewSimulatorFumiBank *bank );
   bool process_fumi_logical_component( NewSimulatorFumiComponent *comp );
   
   public:
   NewSimulatorFileFumi(GScanner *scanner);
   virtual ~NewSimulatorFileFumi();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_FUMI_H_*/
