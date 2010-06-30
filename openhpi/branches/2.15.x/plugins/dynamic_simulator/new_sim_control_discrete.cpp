/** 
 * @file    new_sim_control_discrete.cpp
 *
 * The file includes a class for discrete control handling:\n
 * NewSimulatorControlDiscrete
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
#include "new_sim_control_discrete.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControlDiscrete::NewSimulatorControlDiscrete( NewSimulatorResource *res,
                                                        SaHpiRdrT rdr, 
                                                        SaHpiCtrlStateDiscreteT state,
                                                        SaHpiCtrlModeT mode )
: NewSimulatorControl( res, rdr, mode ) {
   	
   memcpy(&m_rec, &rdr.RdrTypeUnion.CtrlRec.TypeUnion.Discrete, sizeof( SaHpiCtrlRecDiscreteT ));
   memcpy(&m_state, &state, sizeof( SaHpiCtrlStateDiscreteT ));
}


/**
 * Destructor
 **/
NewSimulatorControlDiscrete::~NewSimulatorControlDiscrete() {}


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
bool NewSimulatorControlDiscrete::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorControl::CreateRdr( resource, rdr ) == false )
      return false;

   memcpy(&rdr.RdrTypeUnion.CtrlRec.TypeUnion.Discrete, 
          &m_rec, sizeof( SaHpiCtrlRecDiscreteT )); 

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
SaErrorT NewSimulatorControlDiscrete::GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) {
   
   if (m_write_only == SAHPI_TRUE)
      return SA_ERR_HPI_INVALID_CMD;
      
   if ( &mode != NULL ) {
      mode = m_ctrl_mode;
   }

   if ( &state != NULL ) {
      state.Type = m_type;
      memcpy( &state.StateUnion.Discrete, &m_state, sizeof( SaHpiCtrlStateDiscreteT ));
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
SaErrorT NewSimulatorControlDiscrete::SetState( const SaHpiCtrlModeT &mode, 
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
   
   m_state = state.StateUnion.Discrete; 
   m_ctrl_mode = mode;
   return SA_OK;
}

/** 
 * Dump the control information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorControlDiscrete::Dump( NewSimulatorLog &dump ) const {

  dump << "Discrete control " << m_id_string << ";\n";
  dump << "ControlNum " << m_num << ";\n";
  dump << "Oem" << m_oem << ";\n";
  dump << "State" << m_state << ";\n";
  dump << "Mode" << m_ctrl_mode << ";\n";

}
