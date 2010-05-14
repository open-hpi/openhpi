/** 
 * @file    new_sim_file_rdr.h
 *
 * The file includes an abstract class for parsing special rdr data from the simulation file:\n
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

#ifndef __NEW_SIM_FILE_RDR_H__
#define __NEW_SIM_FILE_RDR_H__

#include <glib.h>

extern "C" {
#include "SaHpi.h"
}

#ifndef __NEW_SIM_FILE_UTIL_H__
#include "new_sim_file_util.h"
#endif

#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_ENTITY_H__
#include "new_sim_entity.h"
#endif

/**
 * @class NewSimulatorFileRdr
 * 
 * Provides functions for parsing the a rdr section of the simulation file.
 **/
class NewSimulatorFileRdr : public NewSimulatorFileUtil {
   protected:
   /// Hold the "depth" inside a file (how many parenthesis are open)
   int         m_depth;
   /// Rdr structure to be filled
   SaHpiRdrT    m_rdr;
   

   public:
   NewSimulatorFileRdr(GScanner *scanner);
   virtual ~NewSimulatorFileRdr();
   
   /// Set Root entity path
   void setRoot( NewSimulatorEntityPath root ) { m_root_ep = root; }
   bool process_rdr_token( void );
   /// abstract method in which the childs should parse the rdr type union information
   virtual NewSimulatorRdr * process_token( NewSimulatorResource *res ) = 0;
};


#endif /*__NEW_SIM_FILE_RDR_H__*/
