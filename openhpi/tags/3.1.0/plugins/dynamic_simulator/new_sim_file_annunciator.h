/** 
 * @file    new_sim_file_annunciator.h
 *
 * The file includes helper classes for parsing control data from the simulation file:\n
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
 
#ifndef __NEW_SIM_FILE_ANNUNCIATOR_H__
#define __NEW_SIM_FILE_ANNUNCIATOR_H__

#include <glib.h>

extern "C" {
#include "SaHpi.h"
}

#ifndef __NEW_SIM_FILE_RDR_H__
#include "new_sim_file_rdr.h"
#endif

#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_ANNUNCIATOR_H__
#include "new_sim_annunciator.h"
#endif

/**
 * @class NewSimulatorFileAnnunciator
 * 
 * Provides some functions for parsing the annunciator and announcement sections of the simulation file.
 **/
class NewSimulatorFileAnnunciator : public NewSimulatorFileRdr {
   private:
   /// static annunciator rdr information
   SaHpiAnnunciatorRecT *m_ann_rec;
   
   bool process_annunciator_data( NewSimulatorAnnunciator *ann );
   bool process_announcement(SaHpiAnnouncementT *announce);
   bool process_announce_condition(SaHpiConditionT *cond);
   bool process_name( SaHpiNameT &name );
   
   public:
   NewSimulatorFileAnnunciator(GScanner *scanner);
   virtual ~NewSimulatorFileAnnunciator();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_ANNUNCIATOR_H_*/
