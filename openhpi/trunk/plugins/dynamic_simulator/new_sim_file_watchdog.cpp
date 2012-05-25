/** 
 * @file    new_sim_file_watchdog.cpp
 *
 * The file includes the class for parsing the watchdog data:\n
 * NewSimulatorFileWatchdog
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
#include "new_sim_file_watchdog.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"
#include "new_sim_watchdog.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileWatchdog::NewSimulatorFileWatchdog( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ) { 

   m_wdt_rec = &m_rdr.RdrTypeUnion.WatchdogRec;
   memset(&m_data, 0, sizeof( SaHpiWatchdogT ));
}


/**
 * Destructor
 **/
NewSimulatorFileWatchdog::~NewSimulatorFileWatchdog() {
}


/**
 * Parse inside the \c WATCHDOG_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Startpoint is the \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY of the \c 
 * WATCHDOG_TOKEN_HANDLER.
 *  
 * @param res Pointer on the resource which includes the watchdog
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileWatchdog::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   NewSimulatorWatchdog *wdt = NULL;
   
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
            
            if (!strcmp(field, "WatchdogNum")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_wdt_rec->WatchdogNum = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_wdt_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case WDT_GET_TOKEN_HANDLER:
            stdlog << "DBG: Start parsing watchdog data.\n";
            success = process_watchdog_data();
            stdlog << "DBG: Parsing returns success = " << success << "\n";
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {
      wdt = new NewSimulatorWatchdog( res, m_rdr, m_data ); 
      stdlog << "DBG: Parse Watchdog successfully\n";
         
      return wdt;
   }
   
   if (wdt != NULL)
      delete wdt;

   return NULL;
}


/**
 * Parse the Watchdog Get section
 *
 * Startpoint is the \c WDT_GET_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileWatchdog::process_watchdog_data() {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse control rdr entry - Missing left curly in WDT_GET section");
    	   success = false;
   }

   m_depth++;
   if (!success)
      return success;
      
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
         
            if (!strcmp(field, "Log")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.Log = ( SaHpiBoolT ) m_scanner->value.v_int;
                                    
            } else if (!strcmp(field, "Running")) {
               if (cur_token == G_TOKEN_INT)
                  if ( m_scanner->value.v_int )
                     stdlog << "WARN: Watchdog is set to not running - you have to restart it";
               m_data.Running = SAHPI_FALSE;
            
            } else if (!strcmp(field, "TimerUse")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.TimerUse = ( SaHpiWatchdogTimerUseT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "TimerAction")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.TimerAction = ( SaHpiWatchdogActionT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "PretimerInterrupt")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.PretimerInterrupt = ( SaHpiWatchdogPretimerInterruptT ) m_scanner->value.v_int;
 
            } else if (!strcmp(field, "PreTimeoutInterval")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.PreTimeoutInterval = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "TimerUseExpFlags")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.TimerUseExpFlags = ( SaHpiWatchdogExpFlagsT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "InitialCount")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.InitialCount = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "PresentCount")) {
               if (cur_token == G_TOKEN_INT) 
                  m_data.PresentCount = m_scanner->value.v_int;
 
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         default: 
            err("Processing Watchog data: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}

