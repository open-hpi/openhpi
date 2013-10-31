/** 
 * @file    new_sim_control.h
 *
 * The file includes an abstract class for control handling:\n
 * NewSimulatorControl
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
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *    
 */
#ifndef __NEW_SIM_CONTROL_H__
#define __NEW_SIM_CONTROL_H__


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

extern "C" {
#include "SaHpi.h"
}


/**
 * @class NewSimulatorControl
 * 
 * Abstract class for simulating controls
 * 
 **/
class NewSimulatorControl : public NewSimulatorRdr {

protected:
   /// rdr information - Num
   SaHpiCtrlNumT         m_num;
   /// rdr information - Output Type
   SaHpiCtrlOutputTypeT  m_output_type;
   /// rdr information - Type
   SaHpiCtrlTypeT        m_type;
   /// rdr information - Default Mode
   SaHpiCtrlDefaultModeT m_def_mode;
   /// rdr information - WriteOnly
   SaHpiBoolT            m_write_only;
   /// rdr information - Oem
   SaHpiUint32T          m_oem;

   /// mode of the control
   SaHpiCtrlModeT       m_ctrl_mode;

public:
  NewSimulatorControl( NewSimulatorResource *res,
                       SaHpiRdrT rdr,
		               SaHpiCtrlModeT ctrl_mode);
		               
  virtual ~NewSimulatorControl();

  virtual unsigned int Num() const { return (unsigned int) m_num; }

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  /// abstract method for hpi function saHpiControlStateSet
  virtual SaErrorT SetState( const SaHpiCtrlModeT &mode, const SaHpiCtrlStateT &state ) = 0;
  /// abstract method for hpi function saHpiControlStateGet
  virtual SaErrorT GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) = 0;
  /// method for hpi function saHpiControlTypeGet
  virtual SaErrorT GetType( SaHpiCtrlTypeT &type );
  
  /// abstract method to dump control information
  virtual void Dump( NewSimulatorLog &dump ) const = 0;
};


#endif
