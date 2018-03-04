/** 
 * @file    new_sim_file_fumi.cpp
 *
 * The file includes the class for parsing the fumi data:\n
 * NewSimulatorFileFumi
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
#include "new_sim_file_fumi.h"
#include "new_sim_fumi.h"
#include "new_sim_fumi_data.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileFumi::NewSimulatorFileFumi( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ) { 
   
   m_fumi_rec = &m_rdr.RdrTypeUnion.FumiRec;
}


/**
 * Destructor
 **/
NewSimulatorFileFumi::~NewSimulatorFileFumi() {
}


/**
 * Parse inside the \c FUMI_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Depend on which control type is parsed several help methods are called. Startpoint is the
 * \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY of the \c 
 * FUMI_TOKEN_HANDLER.
 *  
 * @param res Pointer on the resource which includes the fumi object
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileFumi::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   NewSimulatorFumi *fumi = NULL;
   
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
            err("Processing parse fumi entry: File ends too early");
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
         	      m_fumi_rec->Num = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "AccessProt")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_fumi_rec->AccessProt = (SaHpiFumiProtocolT) m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "Capability")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_fumi_rec->Capability = (SaHpiFumiCapabilityT) m_scanner->value.v_int;
         	 
         	} else if (!strcmp(field, "NumBanks")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_fumi_rec->NumBanks = m_scanner->value.v_int;
         	        
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_fumi_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case FUMI_DATA_TOKEN_HANDLER:
            fumi = new NewSimulatorFumi( res, m_rdr );
            success = process_fumi_data( fumi );
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {

      stdlog << "DBG: Parse Fumi successfully\n";
      // necessary if some fumi_rec data was read after the DATA section
      if ( fumi != NULL ) {       
         fumi->SetData( *m_fumi_rec );

      } else {
      	// Missing data section -> we have to initialize a new Fumi object
      	fumi = new NewSimulatorFumi( res, m_rdr );
      }
      
      return fumi;
   }
   
   if (fumi != NULL)
      delete fumi;

   return NULL;
}

/**
 * Parse the Fumi Data section
 *
 * Startpoint is the \c FUMI_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_data( NewSimulatorFumi *fumi ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   SaHpiFumiSpecInfoT fumiSpec;
   SaHpiFumiSpecInfoTypeT fumiSpecType = SAHPI_FUMI_SPEC_INFO_NONE;
   SaHpiBoolT rollbDisabled = SAHPI_TRUE;
   SaHpiFumiServiceImpactDataT fumiImpact;
   unsigned int  entCounter = 0;
   bool impactFlag = false;
   
   NewSimulatorFumiBank *fbank;
   
   memset( &fumiSpec, 0, sizeof( SaHpiFumiSpecInfoT ));
   memset( &fumiImpact, 0, sizeof( SaHpiFumiServiceImpactDataT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse fumi rdr entry - Missing left curly in FumiData section");
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
         
            if (!strcmp(field, "SpecInfoType")) {
            	   if (cur_token == G_TOKEN_INT)
         	      fumiSpec.SpecInfoType = ( SaHpiFumiSpecInfoTypeT ) m_scanner->value.v_int;
         	
         	} else if (!strcmp(field, "SafDefined")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Missing Left Curly for fumi SafDefined entry" );
         	      success = false;
         	   } else {
         	      if ( fumiSpecType != SAHPI_FUMI_SPEC_INFO_NONE ) {
         	         err("Too many fumi spec info fields include inside FUMI_DATA.");
         	         success = false;
         	         
         	      } else if ( fumiSpec.SpecInfoType != SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
         	      	 err("SpecInfoType doesn't fit to SafDefined inside FUMI_DATA.");
         	         success = false;
         	      
         	      }
         	      fumiSpecType = SAHPI_FUMI_SPEC_INFO_SAF_DEFINED;
         	      m_depth++;
         	   }
         	   
         	} else if (!strcmp(field, "OemDefined")) {
         	   
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Missing Left Curly for fumi OemDefined entry" );
         	      success = false;
         	   } else {
         	      if ( fumiSpecType != SAHPI_FUMI_SPEC_INFO_NONE ) {
         	         err("Too many fumi spec info fields include inside FUMI_DATA.");
         	         success = false;
         	         
         	      } else if ( fumiSpec.SpecInfoType != SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
         	         err("SpecInfoType doesn't fit to OemDefined inside FUMI_DATA.");
         	         success = false;
         	      
         	      }
         	      fumiSpecType = SAHPI_FUMI_SPEC_INFO_OEM_DEFINED;
         	      m_depth++;
         	   }
         	   
         	} else if (!strcmp(field, "SpecID")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiSpec.SpecInfoTypeUnion.SafDefined.SpecID = 
            	               ( SaHpiFumiSafDefinedSpecIdT ) m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "RevisionID")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiSpec.SpecInfoTypeUnion.SafDefined.RevisionID = m_scanner->value.v_int;
            	
            	} else if (!strcmp(field, "Mid")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiSpec.SpecInfoTypeUnion.OemDefined.Mid = m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "BodyLength")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiSpec.SpecInfoTypeUnion.OemDefined.BodyLength = m_scanner->value.v_int;	
            	
            } else if (!strcmp(field, "Body")) {
            	   if (cur_token == G_TOKEN_STRING)
            	      success = process_hexstring(fumiSpec.SpecInfoTypeUnion.OemDefined.BodyLength, 
            	                                 g_strdup(m_scanner->value.v_string),
            	                                 &(fumiSpec.SpecInfoTypeUnion.OemDefined.Body[0]));
            	
            } else if (!strcmp(field, "AutoRollbackDisable")) {
            	   if (cur_token == G_TOKEN_INT)
            	       rollbDisabled = ( SaHpiBoolT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "NumEntities")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiImpact.NumEntities = m_scanner->value.v_int;
            	   
            	   if ( fumiImpact.NumEntities > SAHPI_FUMI_MAX_ENTITIES_IMPACTED ) {
            	      err("Too many entities are defined for ImpactedEntities");
            	      success = false;
            	   }
            	      
            } else if (!strcmp(field, "ImpactedEntities")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Missing Left Curly for fumi ImpactedEntities entry" );
         	      success = false;
         	   
         	   } else {
            	      m_depth++;
            	      if ( entCounter > fumiImpact.NumEntities - 1 ) {
            	         err("Too many entries for ImpactedEntities are defined");
            	         success = false;
            	      }
            	   }
            	   
            } else if (!strcmp(field, "ImpactedEntity")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_entity( fumiImpact.ImpactedEntities[entCounter].ImpactedEntity );
            	   if ( !success )
            	      err("Error at parsing the entity path");
            	   
            	   if ( impactFlag ) {   
                  entCounter++;
                  impactFlag = false;
            	   } else {
            	   	  impactFlag = true;
            	   }
            } else if (!strcmp(field, "ServiceImpact")) {
            	   if (cur_token == G_TOKEN_INT)
            	      fumiImpact.ImpactedEntities[entCounter].ServiceImpact = 
            	                                 ( SaHpiFumiServiceImpactT ) m_scanner->value.v_int;
            	   
            	   if ( impactFlag ) {   
                  entCounter++;
                  impactFlag = false;
            	   } else {
            	   	  impactFlag = true;
            	   }
            	   
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case FUMI_SOURCE_DATA_TOKEN_HANDLER:
            
            fbank = new NewSimulatorFumiBank;
            success = process_fumi_source_info( fbank );
            fumi->SetBankSource( fbank );
            delete fbank;
            fbank = NULL;  
            break;
            
         case FUMI_TARGET_DATA_TOKEN_HANDLER:
         
            fbank = new NewSimulatorFumiBank;
            success = process_fumi_target_info( fbank );
            fumi->SetBankTarget( fbank );
            delete fbank;
            fbank = NULL;
            break;
         
         case FUMI_LOG_TARGET_DATA_TOKEN_HANDLER:
         
            fbank = new NewSimulatorFumiBank;
            success = process_fumi_logical_target_info( fbank );
            fumi->SetBankLogical( fbank );
            delete fbank;
            fbank = NULL;
            break;
            
         default: 
            err("Processing Fumi data: Unknown token");
            success = false;
            break;
      }   
   }
   
   fumi->SetInfo( fumiSpec, fumiImpact, rollbDisabled );
   
   return success;	
}

/**
 * Parse fumi source information
 *
 * Startpoint is the \c FUMI_SOURCE_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param fbank pointer on a NewSimulatorFumiBank to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_source_info( NewSimulatorFumiBank *bank ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   SaHpiFumiSourceInfoT source;
   NewSimulatorFumiComponent *comp;
   
   memset( &source, 0, sizeof( SaHpiFumiSourceInfoT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse idr area entry - Missing left curly");
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
         
            if (!strcmp(field, "ForBank")) {
               if (cur_token == G_TOKEN_INT)
                  bank->SetId( m_scanner->value.v_int );
            
            } else if (!strcmp(field, "SourceUri")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( source.SourceUri );
               } else {
                  err("Processing parse fumi entry: Couldn't parse SourceUri");
               }
            
            } else if (!strcmp(field, "SourceStatus")) {
            	   if (cur_token == G_TOKEN_INT)
            	      source.SourceStatus = ( SaHpiFumiSourceStatusT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "Identifier")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( source.Identifier );
               } else {
                  err("Processing parse fumi entry: Couldn't parse Identifier");
               }
            
            } else if (!strcmp(field, "Description")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( source.Description );
               } else {
                  err("Processing parse fumi entry: Couldn't parse Description");
               }
            
            } else if (!strcmp(field, "DateTime")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( source.DateTime );
               } else {
                  err("Processing parse fumi entry: Couldn't parse DateTime");
               }
               
            } else if (!strcmp(field, "MajorVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      source.MajorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;

            } else if (!strcmp(field, "MinorVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      source.MinorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;

            } else if (!strcmp(field, "AuxVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      source.AuxVersion = ( SaHpiUint32T ) m_scanner->value.v_int;
            
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            
            break;
         
         case FUMI_SOURCE_DATA_TOKEN_HANDLER:
            comp = new NewSimulatorFumiComponent();
            success = process_fumi_component( comp );   
            bank->AddSourceComponent( comp );
            delete comp;
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   bank->SetData( source );
   
   return success;	
}


/**
 * Parse a fumi source component
 *
 * Startpoint is the \c FUMI_SOURCE_DATA_TOKEN_HANDLER or \c FUMI_TARGET_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param comp pointer on a NewSimulatorFumiComponent to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_component( NewSimulatorFumiComponent *comp ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiFumiComponentInfoT compinfo;
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse idr field entry - Missing left curly");
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
         
            if (!strcmp(field, "EntryId")) {
               if (cur_token == G_TOKEN_INT)
                  compinfo.EntryId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ComponentId")) {
               if (cur_token == G_TOKEN_INT)
                  compinfo.ComponentId = m_scanner->value.v_int;

            } else if (!strcmp(field, "MainFwInstance")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_fumi_firmware( compinfo.MainFwInstance );
               } else {
                  err("Processing parse fumi entry: Couldn't parse MainFwInstance");
               }
            
            } else if (!strcmp(field, "ComponentFlags")) {
            	   if (cur_token == G_TOKEN_INT)
            	      compinfo.ComponentFlags = ( SaHpiUint32T ) m_scanner->value.v_int;
            	
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
   
   comp->SetData( compinfo );
   
   return success;	
}


/**
 * Parse a fumi firmware information field
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param fw address of firmware variable to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_firmware( SaHpiFumiFirmwareInstanceInfoT &fw ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++;
    
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
         
            if (!strcmp(field, "InstancePresent")) {
               if (cur_token == G_TOKEN_INT)
                  fw.InstancePresent = ( SaHpiBoolT ) m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Identifier")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( fw.Identifier );
            	      
            } else if (!strcmp(field, "Description")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( fw.Description );
            
            } else if (!strcmp(field, "DateTime")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( fw.DateTime );
            	      
            	} else if (!strcmp(field, "MajorVersion")) {
               if (cur_token == G_TOKEN_INT)
                  fw.MajorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "MinorVersion")) {
               if (cur_token == G_TOKEN_INT)
                  fw.MinorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "AuxVersion")) {
               if (cur_token == G_TOKEN_INT)
                  fw.AuxVersion = ( SaHpiUint32T ) m_scanner->value.v_int;
                  
            } else {
               // Unknown Token 
               err("Processing parse fumi firmware instance entry: Unknown type field %s", field);
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
 * Parse fumi target information
 *
 * Startpoint is the \c FUMI_TARGET_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param bank pointer on a NewSimulatorFumiBank to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_target_info( NewSimulatorFumiBank *bank ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   SaHpiFumiBankInfoT target;
   NewSimulatorFumiComponent *comp;
   
   memset( &target, 0, sizeof( SaHpiFumiBankInfoT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse idr area entry - Missing left curly");
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
         
            if (!strcmp(field, "BankId")) {
               if (cur_token == G_TOKEN_INT)
                  target.BankId =  m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "BankSize")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.BankSize = m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "Position")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.Position = m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "BankState")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.BankState = ( SaHpiFumiBankStateT )m_scanner->value.v_int;
            	      	
            } else if (!strcmp(field, "Identifier")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( target.Identifier );
               } else {
                  err("Processing parse fumi entry: Couldn't parse Identifier");
               }
            
            } else if (!strcmp(field, "Description")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( target.Description );
               } else {
                  err("Processing parse fumi entry: Couldn't parse Description");
               }
            
            } else if (!strcmp(field, "DateTime")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_textbuffer( target.DateTime );
               } else {
                  err("Processing parse fumi entry: Couldn't parse DateTime");
               }
               
            } else if (!strcmp(field, "MajorVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.MajorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;

            } else if (!strcmp(field, "MinorVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.MinorVersion = ( SaHpiUint32T ) m_scanner->value.v_int;

            } else if (!strcmp(field, "AuxVersion")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.AuxVersion = ( SaHpiUint32T ) m_scanner->value.v_int;
            
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            
            break;
         
         case FUMI_TARGET_DATA_TOKEN_HANDLER:
            comp = new NewSimulatorFumiComponent();
            success = process_fumi_component( comp );   
            bank->AddTargetComponent( comp );
            delete comp;
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   bank->SetData( target );
   
   return success;	
}


/**
 * Parse fumi logical target information
 *
 * Startpoint is the \c FUMI_LOG_TARGET_DATA_TOKEN_HANDLER. Endpoint is the last 
 * \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param bank pointer on a NewSimulatorFumiBank to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_logical_target_info( NewSimulatorFumiBank *bank ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   SaHpiFumiLogicalBankInfoT target;
   NewSimulatorFumiComponent *comp;
   
   memset( &target, 0, sizeof( SaHpiFumiLogicalBankInfoT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse idr area entry - Missing left curly");
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
         
            if (!strcmp(field, "FirmwarePersistentLocationCount")) {
               if (cur_token == G_TOKEN_INT)
                  target.FirmwarePersistentLocationCount =  ( SaHpiUint8T ) m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "BankStateFlags")) {
            	   if (cur_token == G_TOKEN_INT)
            	      target.BankStateFlags = ( SaHpiFumiLogicalBankStateFlagsT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "PendingFwInstance")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_fumi_firmware( target.PendingFwInstance );
               } else {
                  err("Processing parse fumi entry: Couldn't parse PendingFwInstance");
               }	
            	
           } else if (!strcmp(field, "RollbackFwInstance")) {
               if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_fumi_firmware( target.RollbackFwInstance );
               } else {
                  err("Processing parse fumi entry: Couldn't parse RollbackFwInstance");
               }	
            
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            
            break;
         
         case FUMI_LOG_TARGET_DATA_TOKEN_HANDLER:
            comp = new NewSimulatorFumiComponent();
            success = process_fumi_logical_component( comp );   
            bank->AddLogicalTargetComponent( comp );
            delete comp;
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   bank->SetData( target );
   
   return success;	
}


/**
 * Parse a fumi logical target component
 *
 * Startpoint is the \c FUMI_LOG_TARGET_DATA_TOKEN_HANDLER. Endpoint is the last 
 * \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param comp pointer on a NewSimulatorFumiComponent to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileFumi::process_fumi_logical_component( NewSimulatorFumiComponent *comp ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiFumiLogicalComponentInfoT compinfo;
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse idr field entry - Missing left curly");
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
         
            if (!strcmp(field, "EntryId")) {
               if (cur_token == G_TOKEN_INT)
                  compinfo.EntryId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ComponentId")) {
               if (cur_token == G_TOKEN_INT)
                  compinfo.ComponentId = m_scanner->value.v_int;

            } else if (!strcmp(field, "PendingFwInstance")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_fumi_firmware( compinfo.PendingFwInstance );
               } else {
                  err("Processing parse fumi entry: Couldn't parse PendingFwInstance");
               }
               
            } else if (!strcmp(field, "RollbackFwInstance")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_fumi_firmware( compinfo.RollbackFwInstance );
               } else {
                  err("Processing parse fumi entry: Couldn't parse RollbackFwInstance");
               }
               
            } else if (!strcmp(field, "ComponentFlags")) {
            	   if (cur_token == G_TOKEN_INT)
            	      compinfo.ComponentFlags = ( SaHpiUint32T ) m_scanner->value.v_int;
            	
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
   
   comp->SetData( compinfo );
   
   return success;	
}

