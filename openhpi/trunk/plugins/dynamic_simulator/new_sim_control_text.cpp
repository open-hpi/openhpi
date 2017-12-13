/** 
 * @file    new_sim_control_text.cpp
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

#include <oh_error.h>
 
#include "new_sim_control.h"
#include "new_sim_control_text.h"
#include "new_sim_domain.h"

/**
 * Constructor
 **/
NewSimulatorControlText::NewSimulatorControlText( NewSimulatorResource *res,
                                                        SaHpiRdrT rdr, 
                                                        SaHpiCtrlStateTextT state,
                                                        SaHpiCtrlModeT mode )
: NewSimulatorControl( res, rdr, mode ) {
   	
   memcpy(&m_rec, &rdr.RdrTypeUnion.CtrlRec.TypeUnion.Text, sizeof( SaHpiCtrlRecTextT ));
   memcpy(&m_state, &state, sizeof( SaHpiCtrlStateTextT ));
}


/**
 * Destructor
 **/
NewSimulatorControlText::~NewSimulatorControlText() {}


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
bool NewSimulatorControlText::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   
   if ( NewSimulatorControl::CreateRdr( resource, rdr ) == false )
      return false;

   memcpy(&rdr.RdrTypeUnion.CtrlRec.TypeUnion.Text, 
          &m_rec, sizeof( SaHpiCtrlRecTextT )); 

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
SaErrorT NewSimulatorControlText::GetState( SaHpiCtrlModeT &mode, SaHpiCtrlStateT &state ) {
   int pos = 0;
   int factor = 1;
   
   if (m_write_only == SAHPI_TRUE)
      return SA_ERR_HPI_INVALID_CMD;
      
   if ( &mode != NULL ) {
      mode = m_ctrl_mode;
   }

   if ( &state != NULL ) {
      state.Type = m_type;
      
      if (state.StateUnion.Text.Line != SAHPI_TLN_ALL_LINES) {
         if (state.StateUnion.Text.Line > m_rec.MaxLines)
            return SA_ERR_HPI_INVALID_DATA;
         
         state.StateUnion.Text.Text.DataType = m_state.Text.DataType;
         state.StateUnion.Text.Text.Language = m_state.Text.Language;
         if (m_rec.DataType == SAHPI_TL_TYPE_UNICODE) 
            factor = 2;
         
         pos = state.StateUnion.Text.Line - factor * m_rec.MaxChars;
         memcpy( &state.StateUnion.Text.Text.Data[0], &m_state.Text.Data[pos], 
                 sizeof( SaHpiUint8T )*m_rec.MaxChars*factor );
         state.StateUnion.Text.Text.DataLength = m_rec.MaxChars*factor;

      } else {
         memcpy( &state.StateUnion.Text, &m_state, sizeof( SaHpiCtrlStateTextT ));
      }
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
SaErrorT NewSimulatorControlText::SetState( const SaHpiCtrlModeT &mode, 
                                               const SaHpiCtrlStateT &state ) {
   int factor = 1;
   SaHpiTextTypeT tmpType;
   NewSimulatorTextBuffer tb;
   int i, pos;
   unsigned uint_val;
   
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
   
   if (state.StateUnion.Text.Text.DataType != m_rec.DataType)
      return SA_ERR_HPI_INVALID_DATA;
   
   if ( ((m_rec.DataType == SAHPI_TL_TYPE_UNICODE) ||
         (m_rec.DataType == SAHPI_TL_TYPE_TEXT)) &&
        (state.StateUnion.Text.Text.Language != m_rec.Language) )
      return SA_ERR_HPI_INVALID_DATA;
   
   if ( m_rec.DataType == SAHPI_TL_TYPE_UNICODE )
      factor = 2;
      
   if ( ((state.StateUnion.Text.Line - 1) * m_rec.MaxChars * factor + 
          state.StateUnion.Text.Text.DataLength) >
          (m_rec.MaxChars * factor * m_rec.MaxLines ) )
      return SA_ERR_HPI_INVALID_DATA;
   
   switch (state.StateUnion.Text.Text.DataType) {
      case SAHPI_TL_TYPE_UNICODE:
         if ( state.StateUnion.Text.Text.DataLength % 2 )
            return SA_ERR_HPI_INVALID_PARAMS;
         
         for (i = 0; i < state.StateUnion.Text.Text.DataLength; i++) {
	    	uint_val = 0;
        	uint_val = state.StateUnion.Text.Text.Data[i];
         	uint_val = uint_val << 8;
         	uint_val = uint_val & state.StateUnion.Text.Text.Data[++i];
         	if ((uint_val >= 0xD800) && (uint_val <= 0xDFFF))
         	   return SA_ERR_HPI_INVALID_PARAMS;
         }   
      break;
      
      case SAHPI_TL_TYPE_BCDPLUS:
      case SAHPI_TL_TYPE_ASCII6:
      case SAHPI_TL_TYPE_TEXT:
         tmpType = tb.CheckAscii( (const char *) state.StateUnion.Text.Text.Data );
         if (tmpType > state.StateUnion.Text.Text.DataType) 
            return SA_ERR_HPI_INVALID_PARAMS;
         break;

      case SAHPI_TL_TYPE_BINARY:
         break;
      default:
         err("Unknown Text type");
   }
   
   if (state.StateUnion.Text.Line == SAHPI_TLN_ALL_LINES) {
   	  memset(&m_state, 0, sizeof ( SaHpiUint8T ) * m_rec.MaxChars * factor * m_rec.MaxLines);
      memcpy(&m_state, &state.StateUnion.Text, sizeof( SaHpiCtrlStateTextT ));

   } else {
   	  // Check whether wrapping occurs
   	  i = state.StateUnion.Text.Text.DataLength / (m_rec.MaxChars * factor);
   	  pos = ((state.StateUnion.Text.Line - 1) + i) * m_rec.MaxChars * factor;
   	  // and clear this line
   	  memset( &m_state.Text.Data[pos], 0, sizeof ( SaHpiUint8T ) * m_rec.MaxChars * factor);
   	  
   	  // Copy the stuff
      pos = (state.StateUnion.Text.Line - 1) * m_rec.MaxChars * factor;
      memcpy( &m_state.Text.Data[pos], state.StateUnion.Text.Text.Data, 
              sizeof ( SaHpiUint8T ) * state.StateUnion.Text.Text.DataLength );
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
void NewSimulatorControlText::Dump( NewSimulatorLog &dump ) const {

  dump << "Text control " << m_id_string << ";\n";
  dump << "ControlNum " << m_num << ";\n";
  dump << "Oem " << m_oem << ";\n";
  dump << "State.Line " << m_state.Line << ";\n";
  dump << "State.Text.DataLength " << m_state.Text.DataLength << ";\n";
  dump << "State.Text.Data " << m_state.Text.Data << ";\n";
  dump << "Mode" << m_ctrl_mode << ";\n";

}
