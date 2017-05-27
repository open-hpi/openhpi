/** 
 * @file    new_sim_control_text.h
 *
 * The file includes a class for text control handling:\n
 * NewSimulatorControlText
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
#ifndef __NEW_SIM_CONTROL_TEXT_H__
#define __NEW_SIM_CONTROL_TEXT_H__


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_CONTROL_H__
#include "new_sim_control.h"
#endif

extern "C" {
#include "SaHpi.h"
}


/**
 * @class NewSimulatorControlText
 * 
 * Class for simulating Text controls
 * 
 **/
class NewSimulatorControlText : public NewSimulatorControl {

protected:
   /// rdr information - Text record
   SaHpiCtrlRecTextT  m_rec;

   /// state of the control 
   SaHpiCtrlStateTextT m_state;
   
public:
  NewSimulatorControlText( NewSimulatorResource *res,
                              SaHpiRdrT rdr, 
                              SaHpiCtrlStateTextT state,
                              SaHpiCtrlModeT mode );
  virtual ~NewSimulatorControlText();

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  virtual SaErrorT SetState( const SaHpiCtrlModeT &mode, const SaHpiCtrlStateT &state );
  virtual SaErrorT GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state );
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
