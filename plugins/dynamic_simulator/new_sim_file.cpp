/** 
 * @file    new_sim_file.cpp
 *
 * The file includes the main class for parsing:\n
 * NewSimulatorFile\n
 * and some functions for the configuration of GScanner.
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
 
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <SaHpi.h>
#include <oh_error.h>
#include "new_sim_file.h"
#include "new_sim_file_util.h"
#include "new_sim_file_rdr.h"
#include "new_sim_file_sensor.h"
#include "new_sim_file_control.h"
#include "new_sim_file_annunciator.h"
#include "new_sim_file_inventory.h"
#include "new_sim_file_watchdog.h"
#include "new_sim_file_fumi.h"
#include "new_sim_file_dimi.h"
#include "new_sim_domain.h"
#include "new_sim_entity.h"
#include "new_sim_utils.h"
#include "new_sim_log.h"

#define GEN_SIM_DATA_VERSION 0.901

///< Global skip characters for oh_scanner_config
static gchar skip_characters[] = " \t\n";  
///< Global identifier_first for oh_scanner_config
static gchar identifier_first[] = G_CSET_a_2_z"_/."G_CSET_A_2_Z; 
///< Global identifier_nth for oh_scanner_config
static gchar identifier_nth[] = G_CSET_a_2_z"_-0123456789/."G_CSET_A_2_Z;
///< Global comment signs for oh_scanner_config
static gchar comment_single[] = "#\n";


/*******************************************************************************
 * The configuration was taken from source/config.c 
 * 
 * G_TOKEN_STRING will be created when anything starts with a-zA-z_/.
 * due to cset_identifier_first and identifier2string values below.
 * Therefor, if you want 0 to be scanned as a string, you need to quote
 * it (e.g. "0")
 *
 *******************************************************************************/
static GScannerConfig oh_scanner_config = {
                skip_characters         /* cset_skip_characters */,
                identifier_first        /* cset_identifier_first */,
                identifier_nth          /* cset_identifier_nth */,
                comment_single          /* cpair_comment_single */,
                FALSE                   /* case_sensitive */,
                TRUE                    /* skip_comment_multi */,
                TRUE                    /* skip_comment_single */,
                TRUE                    /* scan_comment_multi */,
                TRUE                    /* scan_identifier */,
                TRUE                    /* scan_identifier_1char */,
                TRUE                    /* scan_identifier_NULL */,
                TRUE                    /* scan_symbols */,
                TRUE                    /* scan_binary */,
                TRUE                    /* scan_octal */,
                TRUE                    /* scan_float */,
                TRUE                    /* scan_hex */,
                TRUE                    /* scan_hex_dollar */,
                TRUE                    /* scan_string_sq */,
                TRUE                    /* scan_string_dq */,
                TRUE                    /* numbers_2_int */,
                FALSE                   /* int_2_float */,
                TRUE                    /* identifier_2_string */,
                TRUE                    /* char_2_token */,
                TRUE                    /* symbol_2_token */,
                FALSE                   /* scope_0_fallback */,
};
        
/**
 * scanner_msg_handler
 * A reference of this function is passed into the GScanner.\n
 * Used by the GScanner object to output messages that come up during parsing.
 *
 * @param scanner Object used to parse the config file.
 * @paran message Message string.
 * @param is_error: Bit to say the message is an error.
 *
 * @return None (void).
 **/
extern "C" {
   static void scanner_msg_handler (GScanner *scanner, gchar *message, gboolean is_error) {

      g_return_if_fail (scanner != NULL);
      err("%s:%d: %s%s\n",
          scanner->input_name ? scanner->input_name : "<memory>",
          scanner->line, is_error ? "error: " : "", message );
   }
}


/**
 * Constructor
 * Open the file \<filename\> and initialize a GScanner.
 * 
 * @param filename Pointer with the simulation filename
 */
NewSimulatorFile::NewSimulatorFile(const char *filename, NewSimulatorEntityPath root) 
  : NewSimulatorFileUtil( root ),
    m_version ( GEN_SIM_DATA_VERSION ) {
   
   stdlog << "DBG: NewSimulatorFile.constructor with " << filename << "\n";
   m_scanner = g_scanner_new(&oh_scanner_config);
   if (!m_scanner) {
      err("Couldn't allocate g_scanner for file parsing");
   }

   m_scanner->msg_handler = scanner_msg_handler;
   m_scanner->input_name = filename;

   m_file = open(filename, O_RDONLY);
   if (m_file < 0) {
      err("Configuration file '%s' could not be opened", filename);
   }
   
   m_mode = UNKNOWN;
   m_depth = 0;
}

/**
 * Destructor
 */
NewSimulatorFile::~NewSimulatorFile() {
   int i;
   
   stdlog << "DBG: NewSimulatorFile.Destructor\n";
   
   g_scanner_destroy(m_scanner);
   
   if (close(m_file) != 0) {
      err("Couldn't close the file");
   }
   
   for (i=m_tokens.Num()-1; i >= 0; i--)
      m_tokens.Rem(i);	
}



/**
 * 
 * Configuration of the GScanner and reading the configuration part of the simulator file. 
 *
 * Inside the method some additional tokens are put to GScanner. Afterwards the \c CONFIGURATION
 * part is read from the simulation file by calling NewSimulatorFile::process_configuration_token()
 * to configure the class NewSimulatorFile properly.
 * 
 * @return true
 **/
bool NewSimulatorFile::Open() {
   int done=0;
   int i;

   m_tokens.Add(new SimulatorToken( "CONFIGURATION",  CONFIG_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "RPT", RPT_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "RDR", RDR_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "RDR_DETAIL", RDR_DETAIL_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "SENSOR", SENSOR_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "CONTROL", CONTROL_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "INVENTORY", INVENTORY_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "WATCHDOG", WATCHDOG_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "ANNUNCIATOR", ANNUNCIATOR_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "DIMI", DIMI_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "FUMI", FUMI_TOKEN_HANDLER ));
   
   m_tokens.Add(new SimulatorToken( "CONTROL_GET",  CONTROL_GET_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "SENSOR_DATA",  SENSOR_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "INVENTORY_DATA",  INVENTORY_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "INV_AREA",  INV_AREA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "INV_FIELD",  INV_FIELD_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "WDT_GET",  WDT_GET_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "ANNUNCIATOR_DATA",  ANNUNCIATOR_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "ANNOUNCEMENT",  ANNOUNCEMENT_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "DIMI_DATA",  DIMI_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "DIMI_TESTCASE",  DIMI_TESTCASE_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "DIMI_TEST_DATA", DIMI_TEST_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "FUMI_DATA",  FUMI_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "FUMI_SOURCE_DATA",  FUMI_SOURCE_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "FUMI_TARGET_DATA",  FUMI_TARGET_DATA_TOKEN_HANDLER ));
   m_tokens.Add(new SimulatorToken( "FUMI_LOG_TARGET_DATA", FUMI_LOG_TARGET_DATA_TOKEN_HANDLER ));
   
   stdlog << "DBG: NewSimulatorFile::Open()\n";
   stdlog << "DBG: Working with entity path: " << m_root_ep << "\n";
 
   if (m_depth != 0) 
      return false;
     
   g_scanner_input_file(m_scanner, m_file);

   for (i = 0; i < m_tokens.Num(); i++) {
      g_scanner_scope_add_symbol(m_scanner, 0, m_tokens[i]->Name(),
                                (void *)((unsigned long)m_tokens[i]->Token()));
   }

   while (!done) {
      guint my_token;
      my_token = g_scanner_peek_next_token (m_scanner);
      stdlog << "DBG: token " << my_token << "\n";
      //dbg("token: %d", my_token);
      
      switch (my_token) {
      case G_TOKEN_EOF: 
                        done = 1;
                        break;
      case CONFIG_TOKEN_HANDLER:
                        return process_configuration_token();
                        break;
      case RPT_TOKEN_HANDLER:
      case RDR_TOKEN_HANDLER:
                        err("Configuration settings are missing in the file - setting default values");
                        m_mode = INIT;
                        done = 1; 
                        break;
      default:
                        // need to advance it 
                        my_token = g_scanner_get_next_token(m_scanner);
                        g_scanner_unexp_token(m_scanner, G_TOKEN_SYMBOL, NULL, 
                        "\"CONFIGURATION\"", NULL, NULL, TRUE);
                        break;
      }
   }
   
   return true;
}

/** 
 * Parse the simulation file to discover the HPI values
 *  
 * Starting with \c RPT_TOKEN_HANDLER the rpt information
 * is read inside NewSimulatorFile::process_rpt_token.
 *
 * @param domain Pointer to the domain to which the information should be linked
 * 
 * @return success 
 **/
bool NewSimulatorFile::Discover(NewSimulatorDomain *domain) {
   guint cur_token = g_scanner_peek_next_token (m_scanner);
   int done = 0;
   bool success = true;
   
   while (!done) {
      switch (cur_token) {

      case G_TOKEN_EOF: 
         done = 1;
         break;

      case RPT_TOKEN_HANDLER:
         stdlog << "DBG: NewSimulatorFile::Discover: Discover RPT entry\n";
         success = process_rpt_token(domain);
         if (!success) {
            done = 1;
            err("Stop parsing due to the error before");
         } else {
            cur_token = g_scanner_peek_next_token (m_scanner);
         }   
         break;

      default:
         cur_token = g_scanner_get_next_token(m_scanner);
         g_scanner_unexp_token(m_scanner, G_TOKEN_SYMBOL, NULL, 
                               "\"CONFIGURATION\"", NULL, NULL, TRUE);
         done = 1;
         break;
      
      };
   }
   
   return success;
}

/** 
 * Read the configuration information from the simulation file
 *
 * Startpoint is token \c CONFIG_TOKEN_HANDLER. Endpoint is the last
 * \c G_TOKEN_RIGHT_CURLY of the \c CONFIGURATION section.
 * 
 **/
bool NewSimulatorFile::process_configuration_token() {
   char *conf_value;
   double conf_float;
   guint cur_token = g_scanner_get_next_token(m_scanner);
   gchar *field;
   
   if (g_scanner_get_next_token(m_scanner) != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token.");
      return false;
   }
   m_depth++;
   
   while (m_depth) {
   	   cur_token = g_scanner_get_next_token(m_scanner);

       switch (cur_token) {
          case G_TOKEN_STRING:
             field = g_strdup(m_scanner->value.v_string);
          
       	     if (g_scanner_get_next_token(m_scanner) != G_TOKEN_EQUAL_SIGN) {
       	        err("Processing parse configuration: Expected equal sign.");
       	        return false;
       	     }
       	     
       	     cur_token = g_scanner_get_next_token(m_scanner);
       	     if (cur_token == G_TOKEN_STRING) {
       	        conf_value = g_strdup(m_scanner->value.v_string);
       	        if (!strcmp(field, "MODE")) {
       	           if (!strcmp(conf_value, "INIT")) {
       	  	         m_mode = INIT;
       	           } else if (!strcmp(conf_value, "UPDDATE")) {
       	  	         m_mode = UPDATE;
       	           } else {
       	  	         err("Processing parse configuration: Unknown File mode");
       	  	         return false;
       	           }
       	        } else {
       	     	   stdlog << "WARN: Unknown configuration string - we will ignore it: " << field << "\n";
       	        }
       	        
       	     } else if (cur_token == G_TOKEN_FLOAT) {
       	        conf_float = m_scanner->value.v_float;
       	     	if (!strcmp(field, "VERSION")) {
       	     		if (conf_float != m_version) {
       	     			stdlog << "WARN: Version of file " << conf_float << " is not equal ";
       	     			stdlog << "to own version " << m_version << "! - Hope it works\n";
       	     		}
       	     	}
       	     } else {
       	     	stdlog << "WARN: Unknow kind of configuration value\n";
       	     }
             break;
          
          case G_TOKEN_LEFT_CURLY:
             m_depth++;
             break;
          
          case G_TOKEN_RIGHT_CURLY:
             m_depth--;
             break;
       
          default:
       	     err("Processing parse configuration: Unknown token");
       	     return false;
       }
   }
   stdlog << "DBG: process_configuration_token - Set file mode on " << m_mode << "\n";
   
   return true;
}

/** 
 * Read one RPT section
 *
 * Startpoint is token \c G_TOKEN_RIGHT_CURLY. Endpoint is the last
 * \c G_TOKEN_RIGHT_CURLY of one \c RPT section.\n
 * The information is stored inside a NewSimulatorResource object which
 * is added to the domain by calling NewSimulatorDomain::AddResource().
 * 
 * @param domain Pointer to a NewSimulatorDomain object
 * 
 * @return success bool
 * 
 **/
bool NewSimulatorFile::process_rpt_token( NewSimulatorDomain *domain ) {
   guint cur_token = g_scanner_get_next_token(m_scanner);
   char *rptfield;
   NewSimulatorResource *res = new NewSimulatorResource( domain );
   bool success = true;
   
   if (g_scanner_get_next_token(m_scanner) != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token.");
      return false;
   }
   m_depth++;
   
   while ((m_depth > 0) && success) {
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
         rptfield = g_strdup(m_scanner->value.v_string);
         cur_token = g_scanner_get_next_token(m_scanner);
         if (cur_token != G_TOKEN_EQUAL_SIGN) {
            err("Processing parse rpt entry: Missing equal sign");
            success = false;
         }
         
         cur_token = g_scanner_get_next_token(m_scanner);
         
         if (!strcmp("EntryId", rptfield)) {
         	if (cur_token == G_TOKEN_INT) {
         	   res->EntryId() = m_scanner->value.v_int;
         	   stdlog << "DBG: rpt - entityId " << res->EntryId() << "\n";
         	} else {	
         	   err("Processing parse rpt entry: Wrong kind of EntryId");
         	} 
         	
         } else if (!strcmp("ResourceId", rptfield)) {
         	// will be assigned by the daemon -> ignore it
         	stdlog << "DBG: rpt - ResourceId is ignored\n";
         	
         } else if (!strcmp("ResourceInfo", rptfield)) {
            stdlog << "DBG: rpt - ResourceInfo must be proccessed\n";
            SaHpiResourceInfoT info;
            if ( (cur_token == G_TOKEN_LEFT_CURLY) &&
                 process_rpt_info( &info ) ) {
               res->SetResourceInfo( info );
               
            } else {
            	   success = false;
            	   err("Processing parse rpt info returns parse error");
            }
            
         } else if (!strcmp("ResourceEntity", rptfield)) {
            stdlog << "DBG: rpt - ResourceEntity\n";
            if (cur_token == G_TOKEN_LEFT_CURLY) {
            	   m_depth++;
            	   cur_token = g_scanner_get_next_token(m_scanner);
               
               if (cur_token == G_TOKEN_STRING) {
                  gchar *val_str;
                  
                  val_str = g_strdup(m_scanner->value.v_string);
                  res->EntityPath().FromString(val_str);
                  res->EntityPath().ReplaceRoot( m_root_ep );
                  stdlog << "DBG: rpt - Enitity " << res->EntityPath() << "\n";
                  
               } else {
                  success = false;
                  err("Processing parse rpt - wrong EntityPath value");
               }
               
               cur_token = g_scanner_get_next_token(m_scanner);
               if (cur_token != G_TOKEN_RIGHT_CURLY) {
                  success = false;
                  err("Processing parse rpt - Missing right culy");
               }
               m_depth--;
            } else {	
         	   err("Processing parse rpt entry: Missing left curly");
         	}
 
         } else if (!strcmp("ResourceCapabilities", rptfield)) {
            if (cur_token == G_TOKEN_INT) {
               res->ResourceCapabilities() = m_scanner->value.v_int;
               stdlog << "DBG: rpt - ResourceCapabilities " << res->ResourceCapabilities() << "\n";
            } else {	
         	   err("Processing parse rpt entry: Wrong kind of ResourceCapabilities");
         	} 
         	
         } else if (!strcmp("HotSwapCapabilities", rptfield)) {
            if (cur_token == G_TOKEN_INT) {
     	       res->HotSwapCapabilities() = m_scanner->value.v_int;
         	   stdlog << "DBG: rpt - HotSwapCapabilities " << res->HotSwapCapabilities() << "\n";
            } else {	
               err("Processing parse rpt entry: Wrong kind of HotSwapCapabilities");
            }
         	   	
        	 } else if (!strcmp("ResourceSeverity", rptfield)) {
            if (cur_token == G_TOKEN_INT) {
              res->ResourceSeverity() = (SaHpiSeverityT) m_scanner->value.v_int;
         	  stdlog << "DBG: rpt - ResourceSeverity " << res->ResourceSeverity() << "\n";
           } else {	
              err("Processing parse rpt entry: Wrong kind of ResourceSeverity");
           }
            	
         } else if (!strcmp("ResourceFailed", rptfield)) {
            if (cur_token == G_TOKEN_INT) {
               res->ResourceFailed() = m_scanner->value.v_int;
               stdlog << "DBG: rpt - ResourceFailed " << res->ResourceFailed() << "\n";
            } else {	
               err("Processing parse rpt entry: Wrong kind of ResourceFailed");
            }
         	
         } else if (!strcmp("ResourceTag", rptfield)) {
            if ( cur_token == G_TOKEN_LEFT_CURLY ) {
               success = process_textbuffer( res->ResourceTag() );    
            } else {
               err("Processing parse rpt entry: Couldn't parse ResourceTag");
            }
            
         } else {
            // Unknown Token 
            err("Processing parse rpt entry: Unknown Rpt field %s", rptfield);
            success = false;	
         }
         break;

      case RDR_TOKEN_HANDLER:
         stdlog << "DBG: Add resource to domain\n";
         domain->AddResource(res);
         success = process_rdr_token(res);
         break;
         
      default:
         err("Processing parse rpt entry: Unknown token");
         success = false;
         break;
      }
   }
   
   return success;
}

/** 
 * Read one RDR section
 *
 * Startpoint is token \c RDR_TOKEN_HANDLER. Endpoint is the last
 * \c G_TOKEN_RIGHT_CURLY of the \c RDR section. The rdr entries are added to
 * the resource.\n
 * 
 * @param resource pointer 
 * 
 * @return success bool
 * 
 **/
bool NewSimulatorFile::process_rdr_token( NewSimulatorResource *res ) {
   int start = m_depth;
   bool success = true;
   bool emptyrun = false;
   NewSimulatorRdr *rdr = NULL;
   NewSimulatorFileRdr *filerdr = NULL;
         
   guint cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
         err("Processing parse configuration: Expected left curly token.");
         return false;
   }

   m_depth++;
         
   while ((m_depth > start) && success) {
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
            
         case SENSOR_TOKEN_HANDLER: 
        	   	filerdr = new NewSimulatorFileSensor( m_scanner );
            break;
            
         case CONTROL_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileControl( m_scanner );
            break;
            
         case INVENTORY_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileInventory( m_scanner );
            break;
            
         case ANNUNCIATOR_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileAnnunciator( m_scanner );
            break;
            
         case WATCHDOG_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileWatchdog( m_scanner );
            break;
         
         case FUMI_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileFumi( m_scanner );
            break;
              
         case DIMI_TOKEN_HANDLER:
            filerdr = new NewSimulatorFileDimi( m_scanner );
            break;
            
         default:
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;
            
      }
            
      // Process the token section
      if (filerdr != NULL) {
         filerdr->setRoot( m_root_ep );
         success = filerdr->process_rdr_token();
         stdlog << "DBG: process_rdr_token returns " << success << "\n";
         if (success)
            rdr = filerdr->process_token( res );
         if ( rdr != NULL ) {
            stdlog << "DBG: Dump the input.\n";
            rdr->Dump( stdlog );
            stdlog << "DBG: End Dump -----.\n";
         }
         
         delete filerdr;
         filerdr = NULL;
         
      } else if (emptyrun) {
      	 success = process_empty();
      	 emptyrun = false;
      }
            
      if (success && (rdr != NULL)) {
         success = res->AddRdr( rdr );
         rdr = NULL;
      }
            
   } // while RDR TOKEN
         
   stdlog << "DBG: Populate the resource including all rdr information.\n";
   if (res->Populate()) {
      stdlog << "DBG: Resource::Populate was successful.\n";
   } else {
      stdlog << "DBG: Resource::Populate returns an error.\n"; 
      success = false;
   }
      
   return success;
}

	
/** 
 * Read SaHpiResourceInfoT information 
 *
 * Startpoint is token \c G_TOKEN_LEFT_CURLY. Endpoint is \c G_TOKEN_RIGHT_CURLY if 
 * no error occurs during parsing.\n
 * 
 * @param rptinfo Pointer to SaHpiResourceInfoT structure
 * 
 * @return success bool
 **/ 
bool NewSimulatorFile::process_rpt_info(SaHpiResourceInfoT *rptinfo) {
   bool success = true;
   char *field = NULL;
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   
   if (cur_token == G_TOKEN_STRING) { 
      field = g_strdup(m_scanner->value.v_string);
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token != G_TOKEN_EQUAL_SIGN) {
         err("Processing parse rpt entry: Missing equal sign");
         success = false;
      }
      cur_token = g_scanner_get_next_token(m_scanner);
      
   } else if (cur_token == G_TOKEN_RIGHT_CURLY) {
   	  err("Processing parse rpt info: Empty Info field");
   	  success = false;

   } else {
      err("Processing parse rpt info: Unknown token");
      success = false;
   }
   
   while ((cur_token != G_TOKEN_RIGHT_CURLY) && success) {
      gulong val = 0L;
      gchar *val_str = NULL;
      int i;
      
      if (cur_token == G_TOKEN_INT) {
         val = m_scanner->value.v_hex;
      } else if (cur_token == G_TOKEN_STRING) {
      	 val_str = g_strdup(m_scanner->value.v_string);
      } else {
         err("Processing parse rpt info: unknow value type %u", cur_token);
         success = false;
      }
      
      if (!strcmp( "ResourceRev", field )) {
         rptinfo->ResourceRev = val;
      } else if (!strcmp( "SpecificVer", field )) {
         rptinfo->SpecificVer = val;
      } else if (!strcmp( "DeviceSupport", field )) {
         rptinfo->DeviceSupport = val;
      } else if (!strcmp( "ManufacturerId", field )) {
         rptinfo->ManufacturerId = val;
      } else if (!strcmp( "ProductId", field )) {
         rptinfo->ProductId = val;
      } else if (!strcmp( "FirmwareMajorRev", field )) {
         rptinfo->FirmwareMajorRev = val;
      } else if (!strcmp( "FirmwareMinorRev", field )) {
         rptinfo->FirmwareMinorRev = val;
      } else if (!strcmp( "AuxFirmwareRev", field )) {
         rptinfo->AuxFirmwareRev = val;
      } else if (!strcmp( "Guid", field )) {
         success = process_hexstring( 16, val_str, &(rptinfo->Guid[0]) );
         stdlog << "DBG: rptinfo: Parsing GUID ";
         for (i = 0; i < 16; i++)
            stdlog << rptinfo->Guid[i] << " ";
         stdlog << "\n";
      } else {
      	err("Processing parse rpt info: unknown field %s", field);
      }
      
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token == G_TOKEN_STRING) { 
         field = g_strdup(m_scanner->value.v_string);
         cur_token = g_scanner_get_next_token(m_scanner);
         if (cur_token != G_TOKEN_EQUAL_SIGN) {
            err("Processing parse rpt entry: Missing equal sign");
            success = false;
         }
         cur_token = g_scanner_get_next_token(m_scanner);
      }
   }        	
   return success;	
}


/** 
 * Go to the end of a section without reading values
 * 
 * Startpoint is token \c TOKEN. Endpoint is \c G_TOKEN_RIGHT_CURLY of this token.
 * 
 * @return success bool
 **/ 
bool NewSimulatorFile::process_empty() {
   int start = m_depth;
   
   guint cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
         err("Processing parse configuration: Expected left curly token.");
         return false;
   }

   m_depth++;
   while (m_depth > start) {
      
      cur_token = g_scanner_get_next_token(m_scanner);
      
      if (cur_token == G_TOKEN_LEFT_CURLY) {
         m_depth++;
      
      } else if (cur_token == G_TOKEN_RIGHT_CURLY) { 
         m_depth--;
      }
   }
   
   return true;
}
