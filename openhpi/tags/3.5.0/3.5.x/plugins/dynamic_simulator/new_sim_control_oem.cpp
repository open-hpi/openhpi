/** 
 * @file    new_sim_control_oem.cpp
 *
 * The file includes a class for oem control handling:\n
 * NewSimulatorControlOem
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
 
#include "new_sim_control.h"
#include "new_sim_control_oem.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControlOem::NewSimulatorControlOem( NewSimulatorResource *res,
                                                        SaHpiRdrT rdr, 
                                                        SaHpiCtrlStateOemT state,
                                                        SaHpiCtrlModeT mode )
: NewSimulatorControl( res, rdr, mode ) {
   	
   memcpy(&m_rec, &rdr.RdrTypeUnion.CtrlRec.TypeUnion.Oem, sizeof( SaHpiCtrlRecOemT ));
   memcpy(&m_state, &state, sizeof( SaHpiCtrlStateOemT ));
}


/**
 * Destructor
 **/
NewSimulatorControlOem::~NewSimulatorControlOem() {}


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
bool NewSimulatorControlOem::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorControl::CreateRdr( resource, rdr ) == false )
      return false;

   memcpy(&rdr.RdrTypeUnion.CtrlRec.TypeUnion.Oem, 
          &m_rec, sizeof( SaHpiCtrlRecOemT )); 

   return true;
}


/**
 * HPI function saHpiControlGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param mode address to be filled
 * @param state address to be filled 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorControlOem::GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) {
   
   if (m_write_only == SAHPI_TRUE)
      return SA_ERR_HPI_INVALID_CMD;
      
   if ( &mode != NULL ) {
      mode = m_ctrl_mode;
   }

   if ( &state != NULL ) {
      state.Type = m_type;
      memcpy( &state.StateUnion.Oem, &m_state, sizeof( SaHpiCtrlStateOemT ));
   }
   
   return SA_OK;
}


/**
 * HPI function saHpiControlSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal values (if a read is allowed).
 * 
 * @param mode address to be set
 * @param state address to be set 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorControlOem::SetState( const SaHpiCtrlModeT &mode, 
                                               const SaHpiCtrlStateT &state ) {
   
   if (&mode == NULL) 
      return SA_ERR_HPI_INVALID_PARAMS;
   
   if ((m_def_mode.ReadOnly == SAHPI_TRUE) &&
       (mode != m_def_mode.Mode))
      return SA_ERR_HPI_READ_ONLY;
      
   if (mode == SAHPI_CTRL_MODE_AUTO) {
      m_ctrl_mode = mode;
      return SA_OK;
   }
   
   if (mode != SAHPI_CTRL_MODE_MANUAL) 
      return SA_ERR_HPI_INVALID_PARAMS;
      
   if (&state == NULL) 
      return SA_ERR_HPI_INVALID_PARAMS;
   
   if (state.Type != m_type)
      return SA_ERR_HPI_INVALID_DATA;
   
   memcpy(&m_state.Body, &state.StateUnion.Oem.Body, 
          sizeof( SaHpiUint8T ) * state.StateUnion.Oem.BodyLength);
   m_state.BodyLength = state.StateUnion.Oem.BodyLength;
   
   m_ctrl_mode = mode;
   return SA_OK;
}

/** 
 * Dump the control information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorControlOem::Dump( NewSimulatorLog &dump ) const {

  dump << "Oem control " << m_id_string << ";\n";
  dump << "ControlNum " << m_num << ";\n";
  dump << "Oem" << m_oem << ";\n";
  dump << "state.mid " << m_state.MId << ";\n";
  dump << "state.BodyLength " << m_state.BodyLength << ";\n";
  dump << "state.Body " << m_state.Body << ";\n";
  dump << "Mode" << m_ctrl_mode << ";\n";

}
