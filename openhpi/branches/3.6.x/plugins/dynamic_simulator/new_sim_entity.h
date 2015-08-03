/** 
 * @file    new_sim_entity.h
 *
 * Defines the class NewSimulatorEntityPath.
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
 * The new simulator plugin was adapted from the ipmidirect plugin.
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 */

#ifndef __NEW_SIM_ENTITY_H__
#define __NEW_SIM_ENTITY_H__


#include <glib.h>
#include <string.h>


extern "C" {
#include "SaHpi.h"
}

#ifndef __NEW_SIM_LOG_h__
#include "new_sim_log.h"
#endif


/**
 * @class NewSimulatorEntityPath
 * 
 * Wrapper class for entity path
 **/
class NewSimulatorEntityPath {
public:
   SaHpiEntityPathT  m_entity_path;  ///< Holding the entityPath

   /// Constructor
   NewSimulatorEntityPath(); 
   /// Constructor to fill the class with an entity path    
   NewSimulatorEntityPath( const SaHpiEntityPathT &entity_path );
   /// return the HPI structure
   operator SaHpiEntityPathT() { return m_entity_path; }

   void                SetEntry( int idx, SaHpiEntityTypeT type, SaHpiEntityLocationT instance );
   SaHpiEntityTypeT     GetEntryType( int idx );
   void                SetEntryType( int idx, SaHpiEntityTypeT type );
   SaHpiEntityLocationT GetEntryInstance( int idx );
   void                SetEntryInstance( int idx, SaHpiEntityLocationT instance );
 
   void                AppendRoot( int idx );
   void                ReplaceRoot( NewSimulatorEntityPath root );
   bool                FromString( const char *str );
                           
   NewSimulatorEntityPath &operator+=( const NewSimulatorEntityPath &epath );
   bool                   operator==( const NewSimulatorEntityPath &epath ) const;
   bool                   operator!=( const NewSimulatorEntityPath &epath ) const;
 
};


NewSimulatorLog &operator<<( NewSimulatorLog &log, const NewSimulatorEntityPath &epath ); 



#endif
