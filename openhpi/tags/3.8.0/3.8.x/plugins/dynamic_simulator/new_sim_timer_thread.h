/** 
 * @file    new_sim_timer_thread.h
 *
 * The file includes a class for starting a timer in a thread:\n
 * NewSimulatorTimerThread
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

#ifndef __NEW_SIM_TIMER_THREAD_H__
#define __NEW_SIM_TIMER_THREAD_H__

#ifndef __NEW_SIM_UTILS_H__
#include "new_sim_utils.h"
#endif

#ifndef __THREAD_H__
#include "thread.h"
#endif

class NewSimulatorWatchdog;
class NewSimulatorHotSwap;
class NewSimulatorTimerThread;

/// us for which the timer will sleep 
#define THREAD_SLEEPTIME 10000

/**
 * @class NewSimulatorTimerThread
 * 
 * Starts a thread in which a timer will trigger a function after expiration.
 **/
class NewSimulatorTimerThread : public cThread {

private:

  /// Timeout in ms
  unsigned int     m_timeout;
  /// Start time of timer
  cTime             m_start;

protected:
  virtual void *Run();
  /// Flag if a thread is already running
  bool             m_running;
  /// Abstract method which is called after the timre expires
  virtual bool TriggerAction() = 0;

public:
  /// signal thread to exit
  bool m_exit;

  NewSimulatorTimerThread( unsigned int ms_timeout );
  virtual ~NewSimulatorTimerThread();
  
  void Stop();
  unsigned int Reset( unsigned int new_timeout );
  
};


#endif
