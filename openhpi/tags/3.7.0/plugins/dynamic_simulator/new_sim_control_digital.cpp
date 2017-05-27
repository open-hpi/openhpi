/** 
 * @file    new_sim_control_digital.cpp
 *
 * The file includes a class for digital control handling:\n
 * NewSimulatorControlDigital
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
#include "new_sim_control_digital.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControlDigital::NewSimulatorControlDigital( NewSimulatorResource *res,
                                                        SaHpiRdrT rdr, 
                                                        SaHpiCtrlStateDigitalT state,
                                                        SaHpiCtrlModeT mode )
: NewSimulatorControl( res, rdr, mode ) {
   	
   memcpy(&m_rec, &rdr.RdrTypeUnion.CtrlRec.TypeUnion.Digital, sizeof( SaHpiCtrlRecDigitalT ));
   memcpy(&m_state, &state, sizeof( SaHpiCtrlStateDigitalT ));
}


/**
 * Destructor
 **/
NewSimulatorControlDigital::~NewSimulatorControlDigital() {}


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
bool NewSimulatorControlDigital::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorControl::CreateRdr( resource, rdr ) == false )
      return false;

   memcpy(&rdr.RdrTypeUnion.CtrlRec.TypeUnion.Digital, 
          &m_rec, sizeof( SaHpiCtrlRecDigitalT )); 

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
SaErrorT NewSimulatorControlDigital::GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) {
   
   if (m_write_only == SAHPI_TRUE)
      return SA_ERR_HPI_INVALID_CMD;
      
   if ( &mode != NULL ) {
      mode = m_ctrl_mode;
   }

   if ( &state != NULL ) {
      state.Type = m_type;
      memcpy( &state.StateUnion.Digital, &m_state, sizeof( SaHpiCtrlStateDigitalT ));
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
SaErrorT NewSimulatorControlDigital::SetState( const SaHpiCtrlModeT &mode, 
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
      
   switch ( state.StateUnion.Digital ) {
      case SAHPI_CTRL_STATE_PULSE_ON:
         if (m_state == SAHPI_CTRL_STATE_ON)
            return SA_ERR_HPI_INVALID_REQUEST;
         break;
         
      case SAHPI_CTRL_STATE_PULSE_OFF:
         if (m_state == SAHPI_CTRL_STATE_OFF)
            return SA_ERR_HPI_INVALID_REQUEST;
         break;
      
      case SAHPI_CTRL_STATE_ON:
      case SAHPI_CTRL_STATE_OFF:
        m_state = state.StateUnion.Digital;
        break;
        
      default:
         return SA_ERR_HPI_INVALID_PARAMS;
   } 	
   
   m_ctrl_mode = mode;
   return SA_OK;
}

/** 
 * Dump the control information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorControlDigital::Dump( NewSimulatorLog &dump ) const {

  dump << "Digital control " << m_id_string << ";\n";
  dump << "ControlNum " << m_num << ";\n";
  dump << "Oem " << m_oem << ";\n";
  dump << "State " << m_state << ";\n";
  dump << "Mode " << m_ctrl_mode << ";\n";

}
