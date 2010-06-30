/** 
 * @file    new_sim_watchdog.h
 *
 * The file includes a class for watchdog handling:\n
 * NewSimulatorWatchdog
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

#ifndef __NEW_SIM_WATCHDOG_H__
#define __NEW_SIM_WATCHDOG_H__


#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_UTILS_H__
#include "new_sim_utils.h"
#endif

#ifndef __NEW_SIM_TIMER_THREAD_H__
#include "new_sim_timer_thread.h"
#endif

extern "C" {
#include "SaHpi.h"
}

/**
 * @class NewSimulatorWatchdog
 * 
 * Provides functions for simulating a watchdog timer.
 **/
class NewSimulatorWatchdog : public NewSimulatorRdr, public NewSimulatorTimerThread {
private:
   /// Rdr information 
   SaHpiWatchdogRecT       m_wdt_rec;
   /// Watchdog information
   SaHpiWatchdogT          m_wdt_data;
   /// Watchdog start time
   cTime                   m_start;
   
   /// State of the simulator watchdog timer
   enum WdtStateT {
      NONE = 0,
      PRETIMEOUT,
      TIMEOUT };
   
   /// Internal Wdt state
   WdtStateT            m_state;
    
   void TriggerAction( WdtStateT state );
   void SendEvent( SaHpiWatchdogActionEventT wdtaction, SaHpiSeverityT sev );
   
protected:
    virtual bool TriggerAction();
      
public:
   NewSimulatorWatchdog( NewSimulatorResource *res );
   NewSimulatorWatchdog( NewSimulatorResource *res, SaHpiRdrT rdr, 
                         SaHpiWatchdogT wdt_data );
  ~NewSimulatorWatchdog();

  /// return the number of the watchdog
  unsigned int Num() const { return m_wdt_rec.WatchdogNum; }
  /// return the Oem data of the watchdog
  unsigned int Oem() const { return m_wdt_rec.Oem; }

  // create an RDR sensor record
  bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );
  // Dump 
  void Dump( NewSimulatorLog &dump ) const;
  
  // HPI functions
  SaErrorT GetWatchdogInfo( SaHpiWatchdogT &watchdog);
  SaErrorT SetWatchdogInfo( SaHpiWatchdogT &watchdog);
  SaErrorT ResetWatchdog();
};


#endif
