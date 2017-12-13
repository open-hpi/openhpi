/** 
 * @file    new_sim_file_util.cpp
 *
 * The file includes some helper classes for parsing the simulation file:\n
 * SimulatorTokens\n
 * NewSimulatorFileUtil
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.2
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

#include <glib.h>
#include <string.h>

#include <oh_error.h>

#include "new_sim_file_util.h"
#include "new_sim_text_buffer.h"
#include "new_sim_entity.h"
#include "new_sim_log.h"


/**
 * Constructor with the name and value of the token
 * 
 * @param name Name of the token
 * @param token Value of the token
 **/
SimulatorToken::SimulatorToken(const gchar *name, const guint token) {
   int len = strlen(name);
   m_name = new char[len+1];
   strcpy(m_name, name);
   m_token = token;
}

/**
 * Destructor
 **/
SimulatorToken::~SimulatorToken() { 
}

/**
 * Constructor
 **/
NewSimulatorFileUtil::NewSimulatorFileUtil( NewSimulatorEntityPath root ):
                       m_root_ep( root ) {
}


/**
 * Constructor with Scanner information
 **/
NewSimulatorFileUtil::NewSimulatorFileUtil(GScanner *scanner) :
                                           m_scanner( scanner ) {
}


/**
 * Destructor
 **/
NewSimulatorFileUtil::~NewSimulatorFileUtil() {
}

/**
 * Parse a textbuffer
 * 
 * A textbuffer is read from the simulation file and the information is stored inside 
 * the class NewSimulatorTextBuffer. The scanner should be on token G_TOKEN_LEFT_CURLY
 * and will be afterwards on token G_TOKEN_RIGHT_CURLY.\n\n
 * \b Input example:\n
 * DataType=3\n
 * Language=25\n
 * DataLength=9\n
 * Data="Chassis 1"\n 
 *
 * @param buffer  Address of a textbuffer which is filled with the following input values
 * @return success of parsing
 * 
 **/
bool NewSimulatorFileUtil::process_textbuffer( NewSimulatorTextBuffer &buffer ) {
   bool success = true;
   SaHpiTextBufferT tmp;
   char *datafield = NULL;
   char *field = NULL;
   guint val=0;
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token == G_TOKEN_STRING) { 
      field = g_strdup(m_scanner->value.v_string);
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token != G_TOKEN_EQUAL_SIGN) {
         err("Processing parse textbuffer: Missing equal sign");
         success = false;
      }
      cur_token = g_scanner_get_next_token(m_scanner);
      
   } else if (cur_token == G_TOKEN_RIGHT_CURLY) {
   	  err("Processing parse textbuffer: Empty buffer field");

   } else {
      err("Processing parse textbuffer: Unknown token");
      success = false;
   }
   
   while ((cur_token != G_TOKEN_RIGHT_CURLY) && success) {

      if (cur_token == G_TOKEN_INT) {
         val = m_scanner->value.v_int;
      } else if (cur_token == G_TOKEN_STRING) {
      	 datafield = g_strdup(m_scanner->value.v_string);
      } else {
         err("Processing parse textbuffer: unknow value type %u", cur_token);
         success = false;
         break;
      }
      
      if (!strcmp( "DataType", field )) {
         tmp.DataType = ( SaHpiTextTypeT ) val;
      } else if (!strcmp( "Language", field )) {
         tmp.Language = ( SaHpiLanguageT ) val;
      } else if (!strcmp( "DataLength", field )) {
         tmp.DataLength = val;
      } else if (!strcmp( "Data", field )) {
         strncpy ((char *) tmp.Data, datafield, SAHPI_MAX_TEXT_BUFFER_LENGTH);
      } else {
         err("Processing parse textbuffer: unknown field %s", field);
      }
      
      // Take the next token
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token == G_TOKEN_STRING) { 
         field = g_strdup(m_scanner->value.v_string);
         cur_token = g_scanner_get_next_token(m_scanner);
         if (cur_token != G_TOKEN_EQUAL_SIGN) {
            err("Processing parse textbuffer: Missing equal sign");
            success = false;
         }
         cur_token = g_scanner_get_next_token(m_scanner);
      }
   }
   
   if (success)
      return buffer.SetData( tmp );
    
   return success;
}


/**
 * Parse a textbuffer
 * 
 * With another method signature
 * 
 * @param text  Address of a SaHpiTextBufferT structure
 * @return success of parsing
 * 
 **/
bool NewSimulatorFileUtil::process_textbuffer( SaHpiTextBufferT &text ) {
   NewSimulatorTextBuffer textbuffer;
   bool success;

   success = process_textbuffer( textbuffer );
   text = textbuffer;

   return success;
}

 
/** 
 * Parse a hex string
 *
 * Common function to parse strings filled with hex values
 *
 * @param max_len maximal length of list with the hex values
 * @param str string with the values
 * @param hexlist list to be filled with the values
 * @return success value
 **/
bool NewSimulatorFileUtil::process_hexstring( guint max_len, gchar *str, SaHpiUint8T *hexlist) {
   bool success = true;
   guint i;
   guint len = strlen(str);
   guint val_uint;
   
   if (len % 2) {
      err("Processing parse rpt info: Wrong Stream string length\n");
      return false;
   }
   if (len > max_len*2) {
      err("String is longer than allowed by max_len\n");
      return false;
   }
            	  
   for (i = 0; (i< max_len) || (i*2 < len); i++) {
      sscanf(str, "%02X", &val_uint);
      hexlist[i] = static_cast<SaHpiUint8T>(val_uint);
      str++;
      str++;
   }

   return success; 	
}

/** 
 * Parse the entity string
 *
 * Common function to parse entity string.\n
 * The scanner should be on token \c G_TOKEN_LEFT_CURLY and will be afterwards 
 * on token \c G_TOKEN_RIGHT_CURLY.\n
 * 
 * @param path address of entity structure
 * @return success value
 **/
bool NewSimulatorFileUtil::process_entity( SaHpiEntityPathT &path ) {
   NewSimulatorEntityPath ep;
   bool success=true;
   guint cur_token = g_scanner_get_next_token(m_scanner);
               
   if (cur_token == G_TOKEN_STRING) {
      gchar *val_str;
      val_str = g_strdup(m_scanner->value.v_string);
      ep.FromString(val_str);
      ep.ReplaceRoot( m_root_ep );
      path = ep;

   } else {
      success = false;
      err("Processing parse rdr - wrong Entity value");

   }
               
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_RIGHT_CURLY) {
      success = false;
      err("Processing parse rdr entity - Missing right culy");
   }

   return success;
}
