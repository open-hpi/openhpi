/** 
 * @file    new_sim_file_watchdog.h
 *
 * The file includes helper classes for parsing watchdog data from the simulation file:\n
 * NewSimulatorFileWatchdog
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
 
#ifndef __NEW_SIM_FILE_WATCHDOG_H__
#define __NEW_SIM_FILE_WATCHDOG_H__

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

#ifndef __NEW_SIM_WATCHDOG_H__
#include "new_sim_watchdog.h"
#endif

/**
 * @class NewSimulatorFileWatchdog
 * 
 * Provides some functions for parsing the watchdog sections of the simulation file.
 **/
class NewSimulatorFileWatchdog : public NewSimulatorFileRdr {
   private:
   /// static watchdog rdr information
   SaHpiWatchdogRecT *m_wdt_rec;
   /// watchdog data
   SaHpiWatchdogT     m_data;
   
   bool process_watchdog_data();
   
   public:
   NewSimulatorFileWatchdog(GScanner *scanner);
   virtual ~NewSimulatorFileWatchdog();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_WATCHDOG_H_*/
