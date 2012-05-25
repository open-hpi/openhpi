/** 
 * @file    new_sim_file_annunciator.cpp
 *
 * The file includes the class for parsing the control data:\n
 * NewSimulatorFileAnnunciator
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
#include "new_sim_file_annunciator.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"
#include "new_sim_annunciator.h"
#include "new_sim_announcement.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileAnnunciator::NewSimulatorFileAnnunciator( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ) { 

   m_ann_rec = &m_rdr.RdrTypeUnion.AnnunciatorRec;
}


/**
 * Destructor
 **/
NewSimulatorFileAnnunciator::~NewSimulatorFileAnnunciator() {
}


/**
 * Parse inside the \c ANNUNCIATOR_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Startpoint is the \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY of the \c 
 * ANNUNCIATOR_TOKEN_HANDLER.
 *  
 * @param res Pointer on the resource which includes the annunciator
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileAnnunciator::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   NewSimulatorAnnunciator *ann = NULL;
   
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
            
            if (!strcmp(field, "AnnunciatorNum")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ann_rec->AnnunciatorNum = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "AnnunciatorType")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ann_rec->AnnunciatorType = (SaHpiAnnunciatorTypeT) m_scanner->value.v_int;
         	   
            } else if (!strcmp(field, "ModeReadOnly")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ann_rec->ModeReadOnly = (SaHpiAnnunciatorTypeT) m_scanner->value.v_int;
         	   
         	} else if (!strcmp(field, "MaxConditions")) {
         	   if (cur_token == G_TOKEN_INT) 
         	      m_ann_rec->MaxConditions = m_scanner->value.v_int;
         	         
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_ann_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case ANNUNCIATOR_DATA_TOKEN_HANDLER:
            stdlog << "DBG: Start parsing annunciator data.\n";
            ann = new NewSimulatorAnnunciator( res, m_rdr );           
            success = process_annunciator_data( ann );
            stdlog << "DBG: Parsing returns success = " << success << "\n";
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {

      stdlog << "DBG: Parse Annunciator successfully\n";
      // necessary if some ann_rec data was read after the DATA section
      if ( ann != NULL )       
         ann->SetData( *m_ann_rec );
         
      return ann;
   }
   
   if (ann != NULL)
      delete ann;

   return NULL;
}


/**
 * Parse the Annunciator Data seciton
 *
 * Startpoint is the \c ANNUNCIATOR_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileAnnunciator::process_annunciator_data( NewSimulatorAnnunciator *ann ) {
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
   	   err("Processing parse control rdr entry - Missing left curly in AnnunciatorData section");
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
         
            if (!strcmp(field, "Mode")) {

               if (cur_token == G_TOKEN_INT) {
                  ann->SetMode(( SaHpiAnnunciatorModeT ) m_scanner->value.v_int);                  
               } else {
                  err("Wrong typ of AnnunciatorMode");
       	          success = false;
               }
                 
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case ANNOUNCEMENT_TOKEN_HANDLER:
            SaHpiAnnouncementT announce;
            SaErrorT rv;
            
            success = process_announcement( &announce );
            stdlog << "DBG: Process Announcement with success = " << success <<"\n";
            
            rv = ann->AddAnnouncement( announce );
            if (rv != SA_OK ) {
               stdlog << "DBG: Ups AddAnnouncement returns an error: rv = " << rv << "\n";
               success = false;
            }

            break;
            
         default: 
            err("Processing Annunciator data: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}

/**
 * Parse an announcement
 *
 * Startpoint is the \c ANNOUNCEMENT_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param announce pointer on SaHpiAnnouncementT record to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileAnnunciator::process_announcement(SaHpiAnnouncementT *announce ) {
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
   	   err("Processing parse announcement entry - Missing left curly at Announcement");
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
                  announce->EntryId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Timestamp")) {
            	   if (cur_token == G_TOKEN_INT)
                  announce->Timestamp = m_scanner->value.v_int64;
            
            } else if (!strcmp(field, "AddedByUser")) {
            	   if (cur_token == G_TOKEN_INT)
            	      announce->AddedByUser = ( SaHpiBoolT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "Severity")) {
            	   if (cur_token == G_TOKEN_INT)
            	      announce->Severity = ( SaHpiSeverityT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "Acknowledged")) {
            	   if (cur_token == G_TOKEN_INT)
            	      announce->Acknowledged = ( SaHpiBoolT ) m_scanner->value.v_int;
            	      	
            } else if (!strcmp(field, "StatusCond")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) {
                  success = process_announce_condition( &announce->StatusCond );
                  if ( !success )
                     err("Processing StatusCond entry returns false");	
               } else {
               	  err("Processing StatusCond entry - Missing left curly");
                  success = false;
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
 * Parse the status condition structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileAnnunciator::process_announce_condition(SaHpiConditionT *cond) {
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
         
            if (!strcmp(field, "Type")) {
               if (cur_token == G_TOKEN_INT)
                  cond->Type = ( SaHpiStatusCondTypeT ) m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Entity")) {
                if (cur_token == G_TOKEN_LEFT_CURLY)
                  success = process_entity( cond->Entity );
                if ( !success )
                   err("Processing entity in status condition returns false");   	   
                  
            } else if (!strcmp(field, "DomainId")) {
            	   if (cur_token == G_TOKEN_INT)
                  cond->DomainId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ResourceId")) {
               if (cur_token == G_TOKEN_INT)
                  cond->ResourceId = m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "SensorNum")) {
               if (cur_token == G_TOKEN_INT)
                  cond->SensorNum = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "EventState")) {
               if (cur_token == G_TOKEN_INT)
                  cond->EventState = ( SaHpiEventStateT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Name")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY) 
            	      success = process_name( cond->Name );
               if ( !success )
                   err("Processing Name in status condition returns false");
                   
            	} else if (!strcmp(field, "Mid")) {
               if (cur_token == G_TOKEN_INT)
                  cond->Mid = ( SaHpiManufacturerIdT ) m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Data")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( cond->Data );
            	   if ( !success )
                   err("Processing Textbuffer in status condition returns false");
            	      
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
 * Parse the name record
 *
 * Common function to parse name record.\n
 * The scanner should be on token \c G_TOKEN_LEFT_CURLY and will be afterwards 
 * on token \c G_TOKEN_RIGHT_CURLY.\n
 * 
 * @param address of entity structure
 * @return success value
 **/
bool NewSimulatorFileAnnunciator::process_name( SaHpiNameT &name ) {
   int start = m_depth;
   bool success = true;
   char *field;
   guint cur_token;
   name.Length = SA_HPI_MAX_NAME_LENGTH;
   
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
         
            if (!strcmp(field, "Length")) {
               if (cur_token == G_TOKEN_INT)
                name.Length = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Value")) {
               if (cur_token == G_TOKEN_STRING)
                  strncpy((char *) &name.Value, g_strdup(m_scanner->value.v_string), name.Length);
            
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

