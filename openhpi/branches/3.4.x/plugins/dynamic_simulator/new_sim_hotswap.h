/** 
 * @file    new_sim_hotswap.h
 *
 * The file includes a class for hotswap handling:\n
 * NewSimulatorHotswap
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

#ifndef __NEW_SIM_HOTSWAP_H__
#define __NEW_SIM_HOTSWAP_H__

#include <oh_utils.h>

#ifndef __NEW_SIM_UTILS_H__
#include "new_sim_utils.h"
#endif

#ifndef __NEW_SIM_TIMER_THREAD_H__
#include "new_sim_timer_thread.h"
#endif

extern "C" {
#include "SaHpi.h"
}

class NewSimulatorResource;
class NewSimulatorLog;
class NewSimulatorTimerThread;

/**
 * @class NewSimulatorHotSwap
 * 
 * Provides functions for simulating the hotswap behaviour.
 **/
class NewSimulatorHotSwap : public NewSimulatorTimerThread {
private:
   /// HotSwap auto-insertion value 
   SaHpiTimeoutT           m_insert_time;
   /// HotSwap auto-extraction value
   SaHpiTimeoutT           m_extract_time;
   /// HotSwap transition start time
   cTime                   m_start;
   /// Flag if a state transition is running
   bool                   m_running;
   /// State of resource
   SaHpiHsStateT           m_state;
   /// Pointer on the resource for which the object belongs to
   NewSimulatorResource    *m_res;
   
   SaErrorT TriggerTransition( SaHpiHsStateT state );
   void SendEvent( SaHpiHsStateT newState, SaHpiHsStateT prevState, 
                    SaHpiHsCauseOfStateChangeT cause, SaHpiSeverityT severity );
     
   
protected:
    virtual bool TriggerAction();
      
public:
   NewSimulatorHotSwap( NewSimulatorResource *res );
   NewSimulatorHotSwap( NewSimulatorResource *res, SaHpiTimeoutT insertTime,
                        SaHpiTimeoutT extractTime, SaHpiHsStateT startState );
  ~NewSimulatorHotSwap();

  // Dump 
  void Dump( NewSimulatorLog &dump ) const;
  /// Get the HotSwap State
  SaHpiHsStateT GetState() { return m_state; }
  /// Get Extraction Timeout
  SaHpiTimeoutT GetExtractTimeout() { return m_extract_time; }
  
  // Start a resource
  SaErrorT StartResource( oh_event *e );
  // Set the timeout values
  void SetTimeouts( SaHpiTimeoutT insert, SaHpiTimeoutT extract );
  
  // HPI functions
  SaErrorT CancelPolicy();
  SaErrorT SetActive();
  SaErrorT SetInactive();
  SaErrorT GetExtractTimeout(SaHpiTimeoutT &timeout);
  SaErrorT SetExtractTimeout(SaHpiTimeoutT timeout);
  SaErrorT GetState( SaHpiHsStateT &state );
  SaErrorT ActionRequest( SaHpiHsActionT action );
  
};


#endif
