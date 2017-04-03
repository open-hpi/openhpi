/** 
 * @file    new_sim_file_control.h
 *
 * The file includes helper classes for parsing control data from the simulation file:\n
 * NewSimulatorFileControl
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
 
#ifndef __NEW_SIM_FILE_CONTROL_H__
#define __NEW_SIM_FILE_CONTROL_H__

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

#ifndef __NEW_SIM_CONTROL_H__
#include "new_sim_control.h"
#endif

/**
 * @class NewSimulatorFileControl
 * 
 * Provides some functions for parsing the control section of the simulation file.
 **/
class NewSimulatorFileControl : public NewSimulatorFileRdr {
   private:
   /// static control rdr information
   SaHpiCtrlRecT        *m_ctrl_rec;
   /// state of the control
   SaHpiCtrlStateT      m_ctrl_state;
   /// mode of the control
   SaHpiCtrlModeT       m_ctrl_mode;
   /// exists different mode
   bool                m_diff_mode;
   /// exists different state
   bool                m_diff_state;
   
   bool process_type_digital();
   bool process_type_discrete();
   bool process_type_analog();
   bool process_type_stream();
   bool process_state_stream(SaHpiCtrlStateStreamT *stream);
   bool process_type_text();
   bool process_state_text(SaHpiCtrlStateTextT *text);
   bool process_type_oem();
   bool process_state_oem(SaHpiCtrlStateOemT *oem);
   
   bool process_control_mode();
   
   public:
   NewSimulatorFileControl(GScanner *scanner);
   virtual ~NewSimulatorFileControl();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_CONTROL_H_*/
