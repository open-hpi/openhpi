/** 
 * @file    new_sim_control_analog.h
 *
 * The file includes a class for analog control handling:\n
 * NewSimulatorControlAnalog
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
#ifndef __NEW_SIM_CONTROL_ANALOG_H__
#define __NEW_SIM_CONTROL_ANALOG_H__


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
 * @class NewSimulatorControlAnalog
 * 
 * Class for simulating Analog controls
 * 
 **/
class NewSimulatorControlAnalog : public NewSimulatorControl {

protected:
   /// rdr information - Analog record
   SaHpiCtrlRecAnalogT  m_rec;

   /// state of the control 
   SaHpiCtrlStateAnalogT m_state;
   
public:
  NewSimulatorControlAnalog( NewSimulatorResource *res,
                              SaHpiRdrT rdr, 
                              SaHpiCtrlStateAnalogT state,
                              SaHpiCtrlModeT mode );
  virtual ~NewSimulatorControlAnalog();

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  virtual SaErrorT SetState( const SaHpiCtrlModeT &mode, const SaHpiCtrlStateT &state );
  virtual SaErrorT GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state );
  
  virtual void Dump( NewSimulatorLog &dump ) const;
};


#endif
