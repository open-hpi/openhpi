/** 
 * @file    new_sim_file_inventory.cpp
 *
 * The file includes the class for parsing inventory data:\n
 * NewSimulatorFileInventory
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
#include "new_sim_file_inventory.h"
#include "new_sim_inventory.h"
#include "new_sim_inventory_data.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileInventory::NewSimulatorFileInventory( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ) { 
   
   m_idr_rec = &m_rdr.RdrTypeUnion.InventoryRec;
}


/**
 * Destructor
 **/
NewSimulatorFileInventory::~NewSimulatorFileInventory() {
}


/**
 * Parse inside the \c INVENTORY_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Startpoint is the \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last 
 * \c G_TOKEN_RIGHT_CURLY of the \c INVENTORY_TOKEN_HANDLER. For 
 * \c INVENTORY_DATA_TOKEN_HANDLER the method 
 * NewSimulatorFileInventory::process_idr_data is called.
 *  
 * @param res Pointer on the resource which includes the inventory data
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileInventory::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *field;
   NewSimulatorInventory *idr = NULL;
   
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
            
            if (!strcmp(field, "IdrId")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_idr_rec->IdrId = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "Persistent")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_idr_rec->Persistent = (SaHpiBoolT) m_scanner->value.v_int;
         	   
         	} else if (!strcmp(field, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_idr_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         case INVENTORY_DATA_TOKEN_HANDLER:
            idr = new NewSimulatorInventory( res, m_rdr );
            success = process_idr_data( idr );
            break;

         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) {

      stdlog << "DBG: Parse Inventory successfully\n";
      // necessary if some inventory_rec data was read after the DATA section
      if ( idr != NULL )       
         idr->SetData( *m_idr_rec );
         
      return idr;
   }
   
   if (idr != NULL)
      delete idr;

   return NULL;
}

/**
 * Parse the Inventory Data section
 *
 * Startpoint is the \c INVENTORY_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return bool value success
 * 
 **/
bool NewSimulatorFileInventory::process_idr_data( NewSimulatorInventory *idr ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiIdrInfoT idrInfo;
   NewSimulatorInventoryArea *ida;
   
   memset( &idrInfo, 0, sizeof( SaHpiIdrInfoT ));
   
   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_EQUAL_SIGN) {
      err("Processing parse rdr entry: Missing equal sign");
      success = false;
   }

   cur_token = g_scanner_get_next_token(m_scanner);
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	   err("Processing parse control rdr entry - Missing left curly in InventoryData section");
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
         
            if (!strcmp(field, "IdrId")) {
            	   if (cur_token == G_TOKEN_INT)
         	      idrInfo.IdrId = m_scanner->value.v_int;
         	      
         	} else if (!strcmp(field, "UpdateCount")) {
            	   if (cur_token == G_TOKEN_INT)
            	      idrInfo.UpdateCount = m_scanner->value.v_int;
            	      
            } else if (!strcmp(field, "ReadOnly")) {
            	   if (cur_token == G_TOKEN_INT)
            	      idrInfo.ReadOnly = (SaHpiBoolT) m_scanner->value.v_int;
            	
            	} else if (!strcmp(field, "NumAreas")) {
            	   if (cur_token == G_TOKEN_INT)
            	      idrInfo.NumAreas = (SaHpiBoolT) m_scanner->value.v_int;
            	                                   
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            break;
            
         case INV_AREA_TOKEN_HANDLER:
         
            ida = new NewSimulatorInventoryArea();
            success = process_idr_area( ida );  
            idr->AddInventoryArea( ida );
            
            break;
            
         default: 
            err("Processing Annunciator data: Unknown token");
            success = false;
            break;
      }   
   }
   
   idr->SetInfo( idrInfo );
   
   return success;	
}

/**
 * Parse an idr area
 *
 * Startpoint is the \c INV_AREA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param ida pointer on a NewSimulatorInventoryArea to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileInventory::process_idr_area( NewSimulatorInventoryArea *ida ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiIdrAreaHeaderT aheader;
   NewSimulatorInventoryField *idf;
   
   memset( &aheader, 0, sizeof( SaHpiIdrAreaHeaderT ));
   
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
         
            if (!strcmp(field, "AreaId")) {
               if (cur_token == G_TOKEN_INT)
                  aheader.AreaId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Type")) {
            	   if (cur_token == G_TOKEN_INT)
                  aheader.Type = ( SaHpiIdrAreaTypeT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ReadOnly")) {
            	   if (cur_token == G_TOKEN_INT)
            	      aheader.ReadOnly = ( SaHpiBoolT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "NumFields")) {
            	   if (cur_token == G_TOKEN_INT)
            	      aheader.NumFields = m_scanner->value.v_int;
            	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown type field %s", field);
               success = false;	
            }
            
            break;
         
         case INV_FIELD_TOKEN_HANDLER:
            idf = new NewSimulatorInventoryField();
            success = process_idr_field( idf );   
            ida->AddInventoryField( idf );
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   ida->SetData( aheader );
   
   return success;	
}


/**
 * Parse an idr field
 *
 * Startpoint is the \c INV_FIELD_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 * 
 * @param idf pointer on a NewSimulatorInventoryField to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileInventory::process_idr_field( NewSimulatorInventoryField *idf ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   SaHpiIdrFieldT idrfield;
   
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
         
            if (!strcmp(field, "AreaId")) {
               if (cur_token == G_TOKEN_INT)
                  idrfield.AreaId = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "FieldId")) {
               if (cur_token == G_TOKEN_INT)
                  idrfield.FieldId = m_scanner->value.v_int;

            } else if (!strcmp(field, "Type")) {
            	   if (cur_token == G_TOKEN_INT)
                  idrfield.Type = ( SaHpiIdrFieldTypeT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ReadOnly")) {
            	   if (cur_token == G_TOKEN_INT)
            	      idrfield.ReadOnly = ( SaHpiBoolT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "Field")) {
            	   if (cur_token == G_TOKEN_LEFT_CURLY)
            	      success = process_textbuffer( idrfield.Field );
            	
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
   
   idf->SetData( idrfield );
   
   return success;	
}

