/** 
 * @file    new_sim_timer_thread.cpp
 *
 * The file includes a class Timer which runs in another thread:\n
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

#include <assert.h>
#include <errno.h>
#include "new_sim.h"
#include "new_sim_utils.h"
#include "new_sim_watchdog.h"


 
/**
 * Constructor for Watchdogs
 **/
// NewSimulatorTimerThread::NewSimulatorTimerThread( NewSimulatorWatchdog  *wdt, 
NewSimulatorTimerThread::NewSimulatorTimerThread(
                                                  unsigned int ms_timeout )
                       : m_timeout( ms_timeout ),
                         m_running( false ),
                         m_exit( false ) {

}


/**
 * Destructor
 **/
NewSimulatorTimerThread::~NewSimulatorTimerThread() {
}


/**
 * Set the exit flag and sleep THREAD_SLEEPTIME to be sure it is read
 **/
void NewSimulatorTimerThread::Stop() {

   m_exit = true;
   usleep( THREAD_SLEEPTIME );
}


/**
 * Set a new timeout value
 * 
 * @param new_timeout new timeout value in ms
 * 
 * @return latest timeout value of the object
 **/
unsigned int NewSimulatorTimerThread::Reset( unsigned int new_timeout ) {
   
   m_timeout = new_timeout;
   m_start = cTime::Now();
   stdlog << "DBG: Reset timeout value " << m_timeout << "\n";

   return m_timeout;
}


/** 
 * Main loop of the timer.
 *
 * If the timer expires the method TriggerAction() is called. 
 **/
void * NewSimulatorTimerThread::Run() {
   cTime now;
   int delta;
   m_start = cTime::Now();
   m_running = true;
   m_exit = false;
   stdlog << "DBG: Run Timerloop - with timeout " << m_timeout << "\n";

   while( !m_exit ) {
      now = cTime::Now();
      now -= m_start;
      delta = m_timeout - now.GetMsec();
      
      if ( delta <= 0 ) {
         m_exit = TriggerAction();	

      } else if ( delta <= THREAD_SLEEPTIME / 1000 ) {
         usleep( delta*1000 );

      } else {
      	 usleep( THREAD_SLEEPTIME );
      }
   }
   m_running = false;
   stdlog << "DBG: Exit TimerLoop\n";
   
   return 0;
}

