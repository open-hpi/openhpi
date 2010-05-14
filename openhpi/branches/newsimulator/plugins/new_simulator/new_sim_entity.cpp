/** 
 * @file    new_sim_entity.cpp
 *
 * The file includes the implementation for the entity wrapper class:\n
 * NewSimulatorEntityPath
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
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *      
 */

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include <oh_utils.h>

#include "new_sim_entity.h"
#include "new_sim_log.h"

// #include <oh_utils.h>


/**
 * Constructor
 **/
NewSimulatorEntityPath::NewSimulatorEntityPath() {
   memset( &m_entity_path, 0, sizeof( SaHpiEntityPathT ) );
}


/**
 * Fully qualified Constructor
 * 
 * @param entity_path Address of entity path to be set internally
 **/
NewSimulatorEntityPath::NewSimulatorEntityPath( const SaHpiEntityPathT &entity_path ) {
   m_entity_path = entity_path;
}


/**
 * Set an entry on a fix position
 * 
 * @param idx number of position
 * @param type entity type
 * @param instance instance of entity
 **/
void NewSimulatorEntityPath::SetEntry( int idx, SaHpiEntityTypeT type,
                                       SaHpiEntityLocationT instance ) {
   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   m_entity_path.Entry[idx].EntityType     = type;
   m_entity_path.Entry[idx].EntityLocation = instance;
}


/**
 * Return the type of entity on position idx
 * 
 * @param idx number of position
 *
 * @return entity type
 **/ 
SaHpiEntityTypeT NewSimulatorEntityPath::GetEntryType( int idx ) {
   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   return m_entity_path.Entry[idx].EntityType;
}


/**
 * Set entry type on position idx
 *
 * @param idx number of position
 * @param type entity type to be set
 *
 * @return entity type
 **/ 
void NewSimulatorEntityPath::SetEntryType( int idx, SaHpiEntityTypeT type ) {
   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   m_entity_path.Entry[idx].EntityType = type;
}


/**
 * Get entry instance for position idx
 *
 * @param idx number of position
 *
 * @return entity instance
 **/ 
SaHpiEntityLocationT NewSimulatorEntityPath::GetEntryInstance( int idx ) {
   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   return m_entity_path.Entry[idx].EntityLocation;
}


/**
 * Set entry instance on position idx
 *
 * @param idx number of position
 * @param instance instance to be set
 *
 * @return entity type
 **/ 
void NewSimulatorEntityPath::SetEntryInstance( int idx,
                                                SaHpiEntityLocationT instance ) {
  
   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   m_entity_path.Entry[idx].EntityLocation = instance;
}


/**
 * Operator to add an entity path on an existing path
 **/ 
NewSimulatorEntityPath & NewSimulatorEntityPath::operator+=( const NewSimulatorEntityPath &epath ) {
   oh_concat_ep( &m_entity_path, &epath.m_entity_path );

   return *this;
}


/**
 * Operator for equal comparision of two entity path
 **/ 
bool NewSimulatorEntityPath::operator==( const NewSimulatorEntityPath &epath ) const {
   SaHpiBoolT cmp_result;

   cmp_result = oh_cmp_ep( &m_entity_path, &epath.m_entity_path );

   if ( cmp_result == SAHPI_TRUE )
       return true;
   else
       return false;
}


/**
 * Operator for not equal comparision of two entity path
 **/ 
bool NewSimulatorEntityPath::operator!=( const NewSimulatorEntityPath &epath ) const {
   SaHpiBoolT cmp_result;

   cmp_result = oh_cmp_ep( &m_entity_path, &epath.m_entity_path );

   if ( cmp_result == SAHPI_TRUE )
       return false;
   else
       return true;
}


/**
 * Append a root entity on position idx
 * 
 * @param idx number of position
 **/ 
void NewSimulatorEntityPath::AppendRoot( int idx ) {

   assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

   m_entity_path.Entry[idx].EntityType     = SAHPI_ENT_ROOT;
   m_entity_path.Entry[idx].EntityLocation = 0;
}

/**
 * Replace the entity path root entry or append a root entry if 
 * it is missing. 
 * 
 * @param root new root entry
 **/
void NewSimulatorEntityPath::ReplaceRoot( NewSimulatorEntityPath root ) {
   int pos = 0;
   bool notfound = true;

   while ((pos < SAHPI_MAX_ENTITY_PATH) && (notfound)) {
   	  if (m_entity_path.Entry[pos].EntityType == SAHPI_ENT_ROOT) {
   	     notfound = false;
   	     if (pos > 0) {
   	        m_entity_path.Entry[pos-1].EntityLocation = root.GetEntryInstance( 0 );
   	        m_entity_path.Entry[pos-1].EntityType == root.GetEntryType( 0 );
   	     }
   	  }
      pos++;
   }
   
   if ( notfound ) {
      oh_concat_ep( &m_entity_path, &root.m_entity_path );  
   }
   stdlog << "DBG: Replace root - new path: " << m_entity_path << "\n";
}


/**
 * Operator for printing an entity path 
 **/
NewSimulatorLog & operator<<( NewSimulatorLog &log, const NewSimulatorEntityPath &epath ) {
   oh_big_textbuffer path_text;
   char str[OH_MAX_TEXT_BUFFER_LENGTH+1];

   oh_decode_entitypath( &epath.m_entity_path, &path_text );

   memcpy( str, path_text.Data, path_text.DataLength );
   str[path_text.DataLength] = 0;

   log << str;

   return log;
}


/**
 * Encode an entity path to a string
 * 
 * @param str char pointer to hold the information
 * 
 * @return return code of successful completion of oh_encode_entitypath()
 **/
bool NewSimulatorEntityPath::FromString( const char *str ) {
   
   return oh_encode_entitypath( str, &m_entity_path ) ? false : true;
}


