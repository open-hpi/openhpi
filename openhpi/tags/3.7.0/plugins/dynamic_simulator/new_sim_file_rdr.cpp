/** 
 * @file    new_sim_file_rdr.cpp
 *
 * The file includes an abstract class for parsing the special rdr data:\n
 * NewSimulatorFileRdr
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
#include "new_sim_file_util.h"

#include <oh_error.h>


 /**
 * Constructor
 **/
NewSimulatorFileRdr::NewSimulatorFileRdr(GScanner *scanner) 
   : NewSimulatorFileUtil ( scanner ),
     m_depth ( 0 ) {
                                          	
   memset( &m_rdr, 0, sizeof( SaHpiRdrT ));
}

/**
 * Destructor
 **/
NewSimulatorFileRdr::~NewSimulatorFileRdr() {
}


/** 
 * Read a RDR section 
 *
 * Startpoint is token \c RDR_TOKEN_HANDLER. Endpoint is \c G_TOKEN_RIGHT_CURLY if 
 * no error occurs during parsing.\n
 * Depend on wich RDR type is read, some helper functions are called.
 * 
 * @return success bool
 *
 **/ 
bool NewSimulatorFileRdr::process_rdr_token( void ) {
   guint cur_token = g_scanner_get_next_token(m_scanner);
   char *rdrfield;
   NewSimulatorEntityPath ep;
   bool success = true;
   bool commonBlock = true;
   
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token.");
      return false;
   }
   m_depth++;
   
   while ( commonBlock && success) {
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

         case RDR_DETAIL_TOKEN_HANDLER:
            commonBlock = false;
            break;
            
         case G_TOKEN_STRING:
            rdrfield = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
         
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(rdrfield, "RecordId")) {
               // will be assigned by the daemon -> ignore it
         	   stdlog << "DBG: rdr - RecordId is ignored\n";
         	
            } else if (!strcmp(rdrfield, "RdrType")) {
         	   if (cur_token == G_TOKEN_INT) {
         	      m_rdr.RdrType = (SaHpiRdrTypeT) m_scanner->value.v_int;
         	      stdlog << "DBG: rdr - RdrType " << m_rdr.RdrType << "\n";
         	   } else {	
         	      err("Processing parse rpt entry: Wrong kind of RdrType");
         	   }
         	 
            } else if (!strcmp(rdrfield, "Entity")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_entity( m_rdr.Entity );
            	   if ( !success )
            	      err("Error at parsing the entity path");
          	
            } else if (!strcmp(rdrfield, "IsFru")) {
               if (cur_token == G_TOKEN_INT) {
         	      m_rdr.IsFru = m_scanner->value.v_int;
         	      stdlog << "DBG: rdr - IsFru " << m_rdr.IsFru << "\n";
         	   
         	   } else {
         	   	  success = false;
         	      err("Processing parse rdr entry: Wrong kind of IsFru");
         	   }
         	
            } else if (!strcmp(rdrfield, "IdString")) {
               if ( cur_token == G_TOKEN_LEFT_CURLY ) {
               	  success = process_textbuffer( m_rdr.IdString );
                  stdlog << "DBG: IdString " << m_rdr.IdString << "\n";                  
               } else {
               	  success = false;
                  err("Processing parse rdr entry: Couldn't parse IdSting");
               }
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", rdrfield);
               success = false;	
            }
         
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;
      }
   }
   return success;
}
