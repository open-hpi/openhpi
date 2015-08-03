/** 
 * @file    new_sim_control.cpp
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
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *      
 */
 
#include "new_sim_control.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControl::NewSimulatorControl( NewSimulatorResource *res,
                                          SaHpiRdrT rdr, 
		                                  SaHpiCtrlModeT ctrl_mode )
 : NewSimulatorRdr( res, SAHPI_CTRL_RDR, rdr.Entity, rdr.IsFru, rdr.IdString  ),
   m_num ( rdr.RdrTypeUnion.CtrlRec.Num ),
   m_output_type( rdr.RdrTypeUnion.CtrlRec.OutputType ),
   m_type( rdr.RdrTypeUnion.CtrlRec.Type ),
   m_write_only( rdr.RdrTypeUnion.CtrlRec.WriteOnly ),
   m_oem( rdr.RdrTypeUnion.CtrlRec.Oem ),
   m_ctrl_mode( ctrl_mode ) {
   	
   memcpy(&m_def_mode, &rdr.RdrTypeUnion.CtrlRec.DefaultMode, sizeof(SaHpiCtrlDefaultModeT));                                     
}


/**
 * Destructor
 **/
NewSimulatorControl::~NewSimulatorControl() {}


/**
 * A rdr structure is filled with the data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorControl::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
      return false;

   SaHpiCtrlRecT &rec = rdr.RdrTypeUnion.CtrlRec;
   rec.Num = m_num;
   rec.OutputType = m_output_type;
   rec.Type       = m_type;
   rec.WriteOnly  = m_write_only;
   rec.Oem        = m_oem;

   memcpy(&rec.DefaultMode, &m_def_mode, sizeof(SaHpiCtrlDefaultModeT)); 

   return true;
}


/**
 * HPI function saHpiControlTypeGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param type address 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorControl::GetType( SaHpiCtrlTypeT &type ) {
   
   if ( &type == NULL )
      return SA_ERR_HPI_INVALID_PARAMS;
      
   type = m_type;
   
   return SA_OK;
}
