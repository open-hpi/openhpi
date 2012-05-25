/** 
 * @file    new_sim_file_control.cpp
 *
 * The file includes the class for parsing the control data:\n
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
 
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <SaHpi.h>

#include "new_sim_file_rdr.h"
#include "new_sim_file_control.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"
#include "new_sim_control.h"
#include "new_sim_control_digital.h"
#include "new_sim_control_discrete.h"
#include "new_sim_control_analog.h"
#include "new_sim_control_oem.h"
#include "new_sim_control_stream.h"
#include "new_sim_control_text.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileControl::NewSimulatorFileControl( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ),
                       m_diff_mode( false ),
                       m_diff_state( false ) { 
   
   m_ctrl_rec = &m_rdr.RdrTypeUnion.CtrlRec;
   memset(&m_ctrl_state, 0, sizeof( SaHpiCtrlStateT ));
   memset(&m_ctrl_mode, 0, sizeof( SaHpiCtrlModeT ));
}


/**
 * Destructor
 **/
NewSimulatorFileControl::~NewSimulatorFileControl() {
}


/**
 * Parse inside the \c CONTROL_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Depend on which control type is parsed several help methods are called. Startpoint is the
 * \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY of the \c 
 * CONTROL_TOKEN_HANDLER.
 *  
 * @param res Pointer on the resource which includes the control
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileControl::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token.");
      return NULL;
   }
   m_depth++;
   
   while ( (m_depth > 0) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {

         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
       	    field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "Num")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ctrl_rec->Num = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "OutputType")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ctrl_rec->OutputType = (SaHpiCtrlOutputTypeT) m_scanner->value.v_int;
         	   
            } else if (!strcmp(field, "Type")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ctrl_rec->Type = (SaHpiCtrlTypeT) m_scanner->value.v_int;
         	   
         	} else if (!strcmp(field, "TypeUnion.Digital")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing control - Missing left curly at TypeUnion.Digital");
         	      success = false;
         	   } else {
         	      success = process_type_digital( );
         	   }
         	   
         	} else if (!strcmp(field, "TypeUnion.Discrete")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing control - Missing left curly at TypeUnion.Digital");
         	      success = false;
         	   } else {
         	      success = process_type_discrete();
         	   }
         	
            } else if (!strcmp(field, "TypeUnion.Analog")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at TypeUnion");
         	      success = false;
         	   } else {
         	      success = process_type_analog();
         	   }
            
            } else if (!strcmp(field, "TypeUnion.Stream")) {
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at TypeUnion");
         	      success = false;
         	   } else {
         	      success = process_type_stream();
               }
               
            } else if (!strcmp(field, "TypeUnion.Text")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at TypeUnion");
         	      return NULL;
         	   } else {
         	      success = process_type_text();
               }
         	   
         	} else if (!strcmp(field, "TypeUnion.Oem")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at TypeUnion");
         	      return NULL;
         	   } else {
         	      success = process_type_oem();
               }

            } else if (!strcmp(field, "DefaultMode")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	      success = false;
         	   } else {
         	      success = process_control_mode();
         	   }
         	
         	} else if (!strcmp(field, "WriteOnly")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ctrl_rec->WriteOnly = (SaHpiBoolT) m_scanner->value.v_int;
         	         
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ctrl_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }

            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token != G_TOKEN_LEFT_CURLY) {
         	   err("Processing parse control rdr entry - Missing left curly at ControlMode");
         	   success = false;

         	} else {
         	   m_depth++;
         	   cur_token = g_scanner_get_next_token(m_scanner);

         	   if (cur_token == G_TOKEN_STRING) {
         	      field = g_strdup(m_scanner->value.v_string);
         	   } else {
         	   	 err("Processing parse control GET token - don't find field string");
         	   	 return NULL;
         	   }
         	   
               cur_token = g_scanner_get_next_token(m_scanner);
               if (cur_token != G_TOKEN_EQUAL_SIGN) {
                  err("Processing parse rdr entry: Missing equal sign");
                  success = false;
               }
               
               cur_token = g_scanner_get_next_token(m_scanner);
               
               if (!strcmp(field, "Mode")) {
            	      if (cur_token == G_TOKEN_INT) {
         	         m_ctrl_mode = ( SaHpiCtrlModeT ) m_scanner->value.v_int;
         	         m_diff_mode = true;
            	      }
         	   }
         	}
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {
      stdlog << "DBG: Parse Control successfully\n";
      
      if (!m_diff_mode)
         m_ctrl_mode = m_ctrl_rec->DefaultMode.Mode;
         
      switch ( m_ctrl_rec->Type ) {
         case SAHPI_CTRL_TYPE_DIGITAL:
            if (!m_diff_state)
               m_ctrl_state.StateUnion.Digital = m_ctrl_rec->TypeUnion.Digital.Default;
               
            return ( new NewSimulatorControlDigital( res, m_rdr, 
                                                      m_ctrl_state.StateUnion.Digital,
                                                      m_ctrl_mode ) );
            break;
            
         case SAHPI_CTRL_TYPE_DISCRETE:
            if (!m_diff_state)
               m_ctrl_state.StateUnion.Discrete = m_ctrl_rec->TypeUnion.Discrete.Default;
               
            return ( new NewSimulatorControlDiscrete( res, m_rdr, 
                                                       m_ctrl_state.StateUnion.Discrete,
                                                       m_ctrl_mode ) );
            break;
            
         case SAHPI_CTRL_TYPE_ANALOG:
            if (!m_diff_state)
               m_ctrl_state.StateUnion.Analog = m_ctrl_rec->TypeUnion.Analog.Default;
               
            return ( new NewSimulatorControlAnalog( res, m_rdr, 
                                                      m_ctrl_state.StateUnion.Analog,
                                                      m_ctrl_mode ) );
            break;
            
         case SAHPI_CTRL_TYPE_STREAM:
            if (!m_diff_state)
               memcpy(&m_ctrl_state.StateUnion.Stream, &m_ctrl_rec->TypeUnion.Stream.Default,
                      sizeof( SaHpiCtrlStateStreamT ));
               
            return ( new NewSimulatorControlStream( res, m_rdr, 
                                                      m_ctrl_state.StateUnion.Stream,
                                                      m_ctrl_mode ) );
            break;
            
         case SAHPI_CTRL_TYPE_TEXT:
            if (!m_diff_state)
               memcpy(&m_ctrl_state.StateUnion.Text, &m_ctrl_rec->TypeUnion.Text.Default,
                      sizeof( SaHpiCtrlStateTextT ));
               
            return ( new NewSimulatorControlText( res, m_rdr, 
                                                    m_ctrl_state.StateUnion.Text,
                                                    m_ctrl_mode ) );
            break;
            
         case SAHPI_CTRL_TYPE_OEM:
            if (!m_diff_state)
               memcpy(&m_ctrl_state.StateUnion.Oem, &m_ctrl_rec->TypeUnion.Oem.Default,
                      sizeof( SaHpiCtrlStateOemT ));
               
            return ( new NewSimulatorControlOem( res, m_rdr, 
                                                   m_ctrl_state.StateUnion.Oem,
                                                   m_ctrl_mode ) );
            break;
            
         default:
            err("Unknown Control Type");
            return NULL;
      }
   }

   return NULL;

}


/**
 * Parse the digital control type structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_digital( ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Default")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Digital.Default = ( SaHpiCtrlStateDigitalT ) m_scanner->value.v_int;
                  
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token == G_TOKEN_INT) {
                  m_ctrl_state.StateUnion.Digital = ( SaHpiCtrlStateDigitalT ) m_scanner->value.v_int;
                  m_ctrl_state.Type = m_ctrl_rec->Type;
                  m_diff_state = true;
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}

/**
 * Parse the discrete control type structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_discrete( ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Default")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Discrete.Default = m_scanner->value.v_int;
                  
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token == G_TOKEN_INT) {
                  m_ctrl_state.StateUnion.Discrete = m_scanner->value.v_int;
                  m_ctrl_state.Type = m_ctrl_rec->Type;
                  m_diff_state = true;
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}


/**
 * Parse the analog control type structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_analog() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Default")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Analog.Default = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Min")) {
            	   if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Analog.Min = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Max")) {
            	   if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Analog.Max = m_scanner->value.v_int;
                       
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token == G_TOKEN_INT) {
                  m_ctrl_state.StateUnion.Analog = m_scanner->value.v_int;
                  m_ctrl_state.Type = m_ctrl_rec->Type;
                  m_diff_state = true;
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}

/**
 * Parse the control type stream structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_stream() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Default")) {
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	      success = false;
         	   } else {
         	      success = process_state_stream( &m_ctrl_rec->TypeUnion.Stream.Default );
         	   }
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);

            if (cur_token != G_TOKEN_LEFT_CURLY) {
         	   err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	   success = false;
         	} else {
         	   success = process_state_stream( &m_ctrl_state.StateUnion.Stream );
         	   m_ctrl_state.Type = m_ctrl_rec->Type;
               m_diff_state = true;
         	}
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}


/**
 * Parse the control state stream
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *
 * @param stream pointer on SaHpiCtrlStateStreamT structure to be filled
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_state_stream( SaHpiCtrlStateStreamT *stream ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Repeat")) {
               if (cur_token == G_TOKEN_INT)
                  stream->Repeat = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "StreamLength")) {
            	   if (cur_token == G_TOKEN_INT)
                  stream->StreamLength = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Stream")) {
            	   if (cur_token != G_TOKEN_STRING) {
            	      err("Processing parse control stream.Stream: Wrong token type");
            	      return false;
            	   }
            	   
            	   success = process_hexstring( SAHPI_CTRL_MAX_STREAM_LENGTH, 
            	                                g_strdup(m_scanner->value.v_string), 
            	                                &(stream->Stream[0]) );
            	   unsigned int i;
         	   stdlog << "DBG: control: Parsing stream ";
         	   for (i = 0; i < stream->StreamLength; i++) {
         	         stdlog << stream->Stream[i] << " ";
         	   }
         	   stdlog << "\n";
                       
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      } // switch  
   } // while
   
   return success;	
}


/**
 * Parse control type text structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_text() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "MaxChars")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Text.MaxChars = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "MaxLines")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Text.MaxLines = m_scanner->value.v_int;
                   
            } else if (!strcmp(field, "Language")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Text.Language = ( SaHpiLanguageT ) m_scanner->value.v_int;
            	
            	} else if (!strcmp(field, "DataType")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Text.DataType = ( SaHpiTextTypeT ) m_scanner->value.v_int;
                      
            } else if (!strcmp(field, "Default")) {
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	      success = false;
         	   } else {
         	      success = process_state_text( &m_ctrl_rec->TypeUnion.Text.Default );
         	   }
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);

            if (cur_token != G_TOKEN_LEFT_CURLY) {
         	   err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	   success = false;
         	   
         	} else {
         	   success = process_state_text( &m_ctrl_state.StateUnion.Text );
         	   m_ctrl_state.Type = m_ctrl_rec->Type;
               m_diff_state = true;
         	}
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}


/**
 * Parse the control state text
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *
 * @param stream pointer on SaHpiCtrlStateStreamT structure to be filled
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_state_text( SaHpiCtrlStateTextT *text ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "Line")) {
               if (cur_token == G_TOKEN_INT)
                  text->Line = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Text"))  {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( text->Text );
               } else {
                  err("Processing parse control entry: Couldn't parse state text");
               }
         	          
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}         

/**
 * Parse control type oem structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_type_oem() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "MId")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->TypeUnion.Oem.MId = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "ConfigData")) {
            	   if (cur_token == G_TOKEN_STRING)
            	      success = process_hexstring(SAHPI_CTRL_OEM_CONFIG_LENGTH,
            	                                  g_strdup(m_scanner->value.v_string),
            	                                  &(m_ctrl_rec->TypeUnion.Oem.ConfigData[0]));
                  stdlog << "DBG: control - oem: Parse config data\n";
                      
            } else if (!strcmp(field, "Default")) {
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	      success = false;
         	   } else {
         	      success = process_state_oem( &m_ctrl_rec->TypeUnion.Oem.Default );
         	   }
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case CONTROL_GET_TOKEN_HANDLER:
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);

            if (cur_token != G_TOKEN_LEFT_CURLY) {
         	   err("Processing parse control rdr entry - Missing left curly at DefaultMode");
         	   success = false;
         	   
         	} else {
         	   success = process_state_oem( &m_ctrl_state.StateUnion.Oem );
         	   m_ctrl_state.Type = m_ctrl_rec->Type;
               m_diff_state = true;
         	}
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}


/**
 * Parse the control state oem
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *
 * @param stream pointer on SaHpiCtrlStateStreamT structure to be filled
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_state_oem( SaHpiCtrlStateOemT *oem ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rdr entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "MId")) {
               if (cur_token == G_TOKEN_INT)
                  oem->MId = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "BodyLength"))  {
         	   if (cur_token == G_TOKEN_INT)
         	      oem->BodyLength = m_scanner->value.v_int;
         	      
            } else if (!strcmp(field, "Body")) {
            	   if (cur_token == G_TOKEN_STRING)
            	      success = process_hexstring(oem->BodyLength, 
            	                                 g_strdup(m_scanner->value.v_string),
            	                                 &(oem->Body[0]));
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}
/**
 * Parse the SaHpiCtrlDefaultModeT structure 
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY 
 * of DefaultMode.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileControl::process_control_mode() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "Mode")) {
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->DefaultMode.Mode = ( SaHpiCtrlModeT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ReadOnly")) {     
               if (cur_token == G_TOKEN_INT)
                  m_ctrl_rec->DefaultMode.ReadOnly = m_scanner->value.v_int;
         	   
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing DefaultMode: Unknown token");
            success = false;
            break; 
      }  
   }
   
   return success;	
}

