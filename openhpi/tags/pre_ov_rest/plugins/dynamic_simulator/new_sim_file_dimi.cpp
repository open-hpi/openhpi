/** 
 * @file    new_sim_file_dimi.cpp
 *
 * The file includes the class for parsing dimi data:\n
 * NewSimulatorFileDimi
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
#include "new_sim_file_dimi.h"
#include "new_sim_dimi.h"
#include "new_sim_dimi_data.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileDimi::NewSimulatorFileDimi( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ) { 
   
   m_dimi_rec = &m_rdr.RdrTypeUnion.DimiRec;
}


/**
 * Destructor
 **/
NewSimulatorFileDimi::~NewSimulatorFileDimi() {
}


/**
 * Parse inside the \c DIMI_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Startpoint is the \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last 
 * \c G_TOKEN_RIGHT_CURLY of the \c DIMI_TOKEN_HANDLER. For 
 * \c DIMI_DATA_TOKEN_HANDLER the method 
 * NewSimulatorFileInventory::process_dimi_data is called.
 *  
 * @param res Pointer on the resource which includes the dimi data
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileDimi::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   NewSimulatorDimi *dimi = NULL;
   
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
            
            if (!strcmp(field, "DimiNum")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_dimi_rec->DimiNum = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_dimi_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case DIMI_DATA_TOKEN_HANDLER:
            dimi = new NewSimulatorDimi( res, m_rdr );
            success = process_dimi_data( dimi );
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {

      stdlog << "DBG: Parse Dimi successfully\n";
      // necessary if some inventory_rec data was read after the DATA section
      if ( dimi != NULL )       
         dimi->SetData( *m_dimi_rec );
         
      return dimi;
   }
   
   if (dimi != NULL)
      delete dimi;

   return NULL;
}

/**
 * Parse the Dimi Data section
 *
 * Startpoint is the \c DIMI_DATA_TOKEN_HANDLER. Endpoint is the last 
 * \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileDimi::process_dimi_data( NewSimulatorDimi *dimi ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiDimiInfoT dimiInfo;
   NewSimulatorDimiTest *dt;
   
   memset( &dimiInfo, 0, sizeof( SaHpiDimiInfoT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse control rdr entry - Missing left curly in DimiData section");
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
         
            if (!strcmp(field, "NumberOfTests")) {
            	   if (cur_token == G_TOKEN_INT)
         	      dimiInfo.NumberOfTests = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "TestNumUpdateCounter")) {
            	   if (cur_token == G_TOKEN_INT)
            	      dimiInfo.TestNumUpdateCounter = m_scanner->value.v_int;
            	      
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case DIMI_TESTCASE_TOKEN_HANDLER:
         
            dt = new NewSimulatorDimiTest( dimi->GetTestId() );
            success = process_dimi_test( dt );  
            dimi->AddTest( dt );
            
            break;
            
         default: 
            err("Processing Dimi data: Unknown token");
            success = false;
            break;
      }   
   }
   
   dimi->SetInfo( dimiInfo );
   
   return success;	
}

/**
 * Parse a test case
 *
 * Startpoint is the \c DIMI_TESTCASE_TOKEN_HANDLER. 
 * Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param dt pointer on a NewSimulatorDimiTest to be filled
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileDimi::process_dimi_test( NewSimulatorDimiTest *dt ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   int entcnt = 0, tpcnt = 0;
   SaHpiDimiTestT tinfo;
   
   memset( &tinfo, 0, sizeof( SaHpiDimiTestT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse dimi test case entry - Missing left curly");
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
         
            if (!strcmp(field, "TestName")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( tinfo.TestName );
            
            } else if (!strcmp(field, "ServiceImpact")) {
            	   if (cur_token == G_TOKEN_INT)
                  tinfo.ServiceImpact = 
                      ( SaHpiDimiTestServiceImpactT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "EntitiesImpacted")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
            	      if ( entcnt == SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ) {
            	         err("Processing dimi test: Too many impacted entities are in the file");
            	      } else {
            	         success = process_dimi_entities( tinfo.EntitiesImpacted[entcnt] );
            	         entcnt++;
            	      }
            	   }
            	
            } else if (!strcmp(field, "NeedServiceOS")) {
            	   if (cur_token == G_TOKEN_INT)
            	      tinfo.NeedServiceOS = ( SaHpiBoolT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "ServiceOS")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( tinfo.ServiceOS );
            
            } else if (!strcmp(field, "ExpectedRunDuration")) {
               if (cur_token == G_TOKEN_INT)
            	      tinfo.ExpectedRunDuration = ( SaHpiTimeT ) m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "TestCapabilities")) {
               if (cur_token == G_TOKEN_INT)
            	      tinfo.TestCapabilities = 
            	           ( SaHpiDimiTestCapabilityT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "TestParameters")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
            	      if ( tpcnt == SAHPI_DIMITEST_MAX_PARAMETERS ) {
            	         err("Processing dimi test: Too many test parameters are in the file");
            	      } else {
            	         success = process_dimi_testparameters( 
            	                                 tinfo.TestParameters[tpcnt] );
            	         tpcnt++;
            	      }
            	   }
            
            } else if (!strcmp(field, "TestReadiness")) {
               if (cur_token == G_TOKEN_INT) {
                  dt->SetReadiness( (SaHpiDimiReadyT) m_scanner->value.v_int );
               } 
            	
            } else {
               // Unknown Token 
               err("Processing parse dimi test entry: Unknown type field %s!", field);
               success = false;	
            }
            break;
         
         case DIMI_TEST_DATA_TOKEN_HANDLER:
            success = process_dimi_testdata( dt );               
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   dt->SetData( tinfo );
   
   return success;	
}


/**
 * Parse test case data
 *
 * Startpoint is the \c DIMI_TEST_DATA_TOKEN_HANDLER. 
 * Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param dt pointer on a NewSimulatorDimiTest object to be filled with
 * test results
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileDimi::process_dimi_testdata( NewSimulatorDimiTest *dt ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiDimiTestResultsT tresults;
   
   memset( &tresults, 0, sizeof( SaHpiDimiTestResultsT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse dimi test data entry - Missing left curly");
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
         
            if (!strcmp(field, "ResultTimeStamp")) {
            	   if (cur_token == G_TOKEN_INT)
            	      tresults.ResultTimeStamp = ( SaHpiTimeT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "RunDuration")) {
               if (cur_token == G_TOKEN_INT)
            	      tresults.RunDuration = ( SaHpiTimeoutT ) m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "LastRunStatus")) {
               if (cur_token == G_TOKEN_INT)
                  tresults.LastRunStatus = ( SaHpiDimiTestRunStatusT ) 
                                                        m_scanner->value.v_int;

            } else if (!strcmp(field, "TestErrorCode")) {
            	   if (cur_token == G_TOKEN_INT)
            	      tresults.TestErrorCode = ( SaHpiDimiTestErrCodeT ) 
            	                                             m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "TestResultString")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( tresults.TestResultString );
            
            } else if (!strcmp(field, "TestResultStringIsURI")) {
               if (cur_token == G_TOKEN_INT)
            	      tresults.TestResultStringIsURI = ( SaHpiBoolT ) 
            	                                             m_scanner->value.v_int;
            	      
            } else {
               // Unknown Token 
               err("Processing parse dimi test results: Unknown type field %s", field);
               success = false;	
            }
            break;
         
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   dt->SetResults( tresults );
   
   return success;	
}


/**
 * Parse a test afffected entity structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. 
 * Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param tae address of structure SaHpiDimiTestAffectedEntityT 
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileDimi::process_dimi_entities( 
                             SaHpiDimiTestAffectedEntityT &tae ) {
   bool success = true;
   char *field = NULL;
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token == G_TOKEN_STRING) { 
      field = g_strdup(m_scanner->value.v_string);
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token != G_TOKEN_EQUAL_SIGN) {
         err("Processing dimi entities: Missing equal sign");
         success = false;
      }
      cur_token = g_scanner_get_next_token(m_scanner);
      
   } else if (cur_token == G_TOKEN_RIGHT_CURLY) {
   	  err("Processing dimi entities: Empty entities field");
   	  success = false;

   } else {
      err("Processing dimi entitier: Unknown token");
      success = false;
   }
   
   while ((cur_token != G_TOKEN_RIGHT_CURLY) && success) {

      if (!strcmp( "EntityImpacted", field )) {
         if (cur_token == G_TOKEN_LEFT_CURLY)
            success = process_entity( tae.EntityImpacted );
         if ( !success )
            err("Processing entity in dimi entities returns false");
      	
      } else if (!strcmp( "ServiceImpact", field )) {
         if (cur_token == G_TOKEN_INT)
            tae.ServiceImpact = ( SaHpiDimiTestServiceImpactT ) m_scanner->value.v_int;

      } else {
         err("Processing dimi entities: unknown field %s", field);
      }
      
      // Take the next token
      cur_token = g_scanner_get_next_token(m_scanner);

      if (cur_token == G_TOKEN_STRING) { 
         field = g_strdup(m_scanner->value.v_string);
         cur_token = g_scanner_get_next_token(m_scanner);
         if (cur_token != G_TOKEN_EQUAL_SIGN) {
            err("Processing dimi entities: Missing equal sign");
            success = false;
         }
         cur_token = g_scanner_get_next_token(m_scanner);
      }
   }
   
   return success;
}


/**
 * Parse a test parameters definition
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. 
 * Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param tp address of structure SaHpiDimiTestParamsDefinitionT 
 * 
 * @return bool value success
 * 
 **/
bool NewSimulatorFileDimi::process_dimi_testparameters( 
                                         SaHpiDimiTestParamsDefinitionT &tp ) {
   bool success = true;
   char *datafield = NULL;
   int dlen;
   char *field = NULL;
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token == G_TOKEN_STRING) { 
      field = g_strdup(m_scanner->value.v_string);
      cur_token = g_scanner_get_next_token(m_scanner);
      if (cur_token != G_TOKEN_EQUAL_SIGN) {
         err("Processing dimi entities: Missing equal sign");
         success = false;
      }
      cur_token = g_scanner_get_next_token(m_scanner);
      
   } else if (cur_token == G_TOKEN_RIGHT_CURLY) {
   	  err("Processing dimi entities: Empty entities field");
   	  success = false;

   } else {
      err("Processing dimi entitier: Unknown token");
      success = false;
   }
   
   while ((cur_token != G_TOKEN_RIGHT_CURLY) && success) {

      if (!strcmp( "ParamName", field )) {
         if (cur_token == G_TOKEN_STRING) {
            datafield = g_strdup(m_scanner->value.v_string);
            dlen = strlen( datafield );
            for (int i=0; (i<SAHPI_DIMITEST_PARAM_NAME_LEN) && (i<dlen); i++) { 
               tp.ParamName[i] = ( SaHpiUint8T ) *datafield;
               datafield++;
            }
         }
      	
      } else if (!strcmp( "ParamInfo", field )) {
         if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( tp.ParamInfo );
            	      
      } else if (!strcmp( "ParamType", field )) {
         if (cur_token == G_TOKEN_INT)
            tp.ParamType = ( SaHpiDimiTestParamTypeT ) m_scanner->value.v_int;
      
      } else if (!strcmp( "MinValue", field )) {
         if (cur_token == G_TOKEN_INT) {
            tp.MinValue.IntValue = m_scanner->value.v_int;
         } else if ( cur_token == G_TOKEN_FLOAT ) {
            tp.MinValue.FloatValue = m_scanner->value.v_float;
         } else {
            err("Unknown datatype for test parameter");
         }	
         
      } else if (!strcmp( "MaxValue", field )) {
         if (cur_token == G_TOKEN_INT) {
            tp.MaxValue.IntValue = m_scanner->value.v_int;
         } else if ( cur_token == G_TOKEN_FLOAT ) {
            tp.MaxValue.FloatValue = m_scanner->value.v_float;
         } else {
            err("Unknown datatype for test parameter");
         }	

      } else if (!strcmp( "DefaultParam", field )) {
         if (cur_token == G_TOKEN_INT) {
         	if ( tp.ParamType == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN) {
         	   tp.DefaultParam.parambool = ( SaHpiBoolT ) m_scanner->value.v_int;
         	} else {
               tp.DefaultParam.paramint = m_scanner->value.v_int;
         	}
         
         } else if ( cur_token == G_TOKEN_FLOAT ) {
            tp.DefaultParam.paramfloat = m_scanner->value.v_float;
         
         } else if (  cur_token == G_TOKEN_LEFT_CURLY) {
            	success = process_textbuffer( tp.DefaultParam.paramtext );
         
         } else {
            err("Unknown datatype for test parameter");
         }	
         
      } else {
         err("Processing dimi testparametes: unknown field %s", field);
      }
      
      // Take the next token
      cur_token = g_scanner_get_next_token(m_scanner);

      if (cur_token == G_TOKEN_STRING) { 
         field = g_strdup(m_scanner->value.v_string);
         cur_token = g_scanner_get_next_token(m_scanner);
         if (cur_token != G_TOKEN_EQUAL_SIGN) {
            err("Processing dimi testparameters: Missing equal sign");
            success = false;
         }
         cur_token = g_scanner_get_next_token(m_scanner);
      }
   }
   
   return success;
}


