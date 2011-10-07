/** 
 * @file    new_sim_file.h
 *
 * The file includes the main class for parsing:\n
 * NewSimulatorFile\n
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
 
#ifndef __NEW_SIM_FILE_H__
#define __NEW_SIM_FILE_H__

#include <glib.h>
#include <oh_utils.h>

extern "C" {
#include "SaHpi.h"
}

#ifndef __ARRAY_H__
#include "array.h"
#endif

/**
#ifndef __NEW_SIM_DOMAIN_H__
#include "new_sim_domain.h"
#endif
**/
#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_FILE_UTIL_H__
#include "new_sim_file_util.h"
#endif


class NewSimulatorDomain;

/**
 * @class NewSimulatorFile
 * 
 * Main class for parsing the simulation file.
 **/
class NewSimulatorFile : public NewSimulatorFileUtil {
   private:
   double                m_version;  //!< Version number
   int                   m_file;     //!< Holding the FILE pointer
   int                   m_mode;     //!< Switch if the class is running in UPDATE or INIT mode
   int                   m_depth;    //!< Deepth concerning LEFT and RIGHT_CURLY
   cArray<SimulatorToken> m_tokens;   //!< Array with additional TOKENS of type SimulatorToken
   
   bool process_configuration_token();
   bool process_rpt_token(NewSimulatorDomain *domain);
   bool process_rpt_info(SaHpiResourceInfoT *rptinfo);
   bool process_rdr_token( NewSimulatorResource *res );
   bool process_empty();
   
   public:
   NewSimulatorFile(const char *filename, NewSimulatorEntityPath root);
   ~NewSimulatorFile();
   /** @enum mode in which the class can work **/
   enum mode {
               INIT = 0,
               UPDATE,
               UNKNOWN
   };
   bool                Open();
   bool                Discover( NewSimulatorDomain *domain );
   int                 &Mode() { return m_mode; }  //!< Read/Write function fo the private m_mode
};


#endif /*NEW_SIM_FILE_H_*/
