/** 
 * @file    new_sim_file_dimi.h
 *
 * The file includes helper class for parsing dimi data from the simulation file:\n
 * NewSimulatorFileDimi
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
 
#ifndef __NEW_SIM_FILE_DIMI_H__
#define __NEW_SIM_FILE_DIMI_H__

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

#ifndef __NEW_SIM_DIMI_H__
#include "new_sim_dimi.h"
#endif

#ifndef __NEW_SIM_DIMI_DATA_H__
#include "new_sim_dimi_data.h"
#endif

/**
 * @class NewSimulatorFileDimi
 * 
 * Provides some functions for parsing the dimi section of 
 * the simulation file.
 **/
class NewSimulatorFileDimi : public NewSimulatorFileRdr {
   private:
   /// static control rdr information
   SaHpiDimiRecT        *m_dimi_rec;
   
   bool process_dimi_data( NewSimulatorDimi *dimi );
   
   public:
   NewSimulatorFileDimi(GScanner *scanner);
   virtual ~NewSimulatorFileDimi();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);
   bool process_dimi_test( NewSimulatorDimiTest *dt );
   bool process_dimi_testdata( NewSimulatorDimiTest *dt );
   bool process_dimi_entities( SaHpiDimiTestAffectedEntityT &tae );
   bool process_dimi_testparameters( SaHpiDimiTestParamsDefinitionT &tp );

};

#endif /*__NEW_SIM_FILE_DIMI_H_*/
