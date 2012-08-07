/** 
 * @file    new_sim_control_stream.cpp
 *
 * The file includes a class for stream control handling:\n
 * NewSimulatorControlStream
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
#include "new_sim_control_stream.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControlStream::NewSimulatorControlStream( NewSimulatorResource *res,
                                                        SaHpiRdrT rdr, 
                                                        SaHpiCtrlStateStreamT state,
                                                        SaHpiCtrlModeT mode )
: NewSimulatorControl( res, rdr, mode ) {
   	
   memcpy(&m_rec, &rdr.RdrTypeUnion.CtrlRec.TypeUnion.Stream, sizeof( SaHpiCtrlRecStreamT ));
   memcpy(&m_state, &state, sizeof( SaHpiCtrlStateStreamT ));
}


/**
 * Destructor
 **/
NewSimulatorControlStream::~NewSimulatorControlStream() {}


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
bool NewSimulatorControlStream::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorControl::CreateRdr( resource, rdr ) == false )
      return false;

   memcpy(&rdr.RdrTypeUnion.CtrlRec.TypeUnion.Stream, 
          &m_rec, sizeof( SaHpiCtrlRecStreamT )); 

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
SaErrorT NewSimulatorControlStream::GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) {
   
   if (m_write_only == SAHPI_TRUE)
      return SA_ERR_HPI_INVALID_CMD;
      
   if ( &mode != NULL ) {
      mode = m_ctrl_mode;
   }

   if ( &state != NULL ) {
      state.Type = m_type;
      memcpy( &state.StateUnion.Stream, &m_state, sizeof( SaHpiCtrlStateStreamT ));
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
SaErrorT NewSimulatorControlStream::SetState( const SaHpiCtrlModeT &mode, 
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
   
   if (state.StateUnion.Stream.StreamLength > SAHPI_CTRL_MAX_STREAM_LENGTH)
      return SA_ERR_HPI_INVALID_PARAMS;
      
   memcpy(&m_state.Stream, &state.StateUnion.Stream.Stream, 
          sizeof( SaHpiUint8T ) * state.StateUnion.Stream.StreamLength);
   m_state.StreamLength = state.StateUnion.Stream.StreamLength;
   m_state.Repeat = state.StateUnion.Stream.Repeat;
   
   m_ctrl_mode = mode;
   return SA_OK;
}

/** 
 * Dump the control information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorControlStream::Dump( NewSimulatorLog &dump ) const {
  unsigned int i;
  
  dump << "Stream control " << m_id_string << ";\n";
  dump << "ControlNum " << m_num << ";\n";
  dump << "Oem" << m_oem << ";\n";
  dump << "State.StreamLength " << m_state.StreamLength << ";\n";
  dump << "State.Repeat " << m_state.Repeat << ";\n";
  dump << "State.Stream";
  for (i=0; i < m_state.StreamLength; i++)
     dump << " " << m_state.Stream[i];
  dump << ";\n";
  dump << "Mode" << m_ctrl_mode << ";\n";

}
