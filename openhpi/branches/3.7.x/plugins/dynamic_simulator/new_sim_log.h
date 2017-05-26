/** 
 * @file    new_sim_log.h
 *
 * The file includes the definition for a class for logfile handling:\n
 * NewSimulatorLog
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
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *    
 */
 
#ifndef __NEW_SIM_LOG_H__
#define __NEW_SIM_LOG_H__


#ifndef __THREAD_H__
#include "thread.h"
#endif

#include <stdio.h>
#include <assert.h>

/// default logfile name
#define dDefaultLogfile "log"

/* log file properties */
#define dIpmiLogPropNone 0 //!< no property
#define dIpmiLogStdOut   1 //!< use stdout
#define dIpmiLogStdErr   2 //!< use stderr
#define dIpmiLogLogFile  4 //!< use a log file
#define dIpmiLogFile     8 //!< use a file

/**
 * @class NewSimulatorLog
 * 
 * Class for the handling of logfiles
 * 
 **/
class NewSimulatorLog {
protected:
   /// Thread lock
   cThreadLock m_lock;
   /// count of locks
   int         m_lock_count;
   /// count of open files
   int         m_open_count;

   /// flag if hex values are written
   bool  m_hex;  // true => print int in hex
   /// flag if time should be written
   bool  m_time; // with time
   /// flag recursive
   bool  m_recursive;
   /// flag if output should be on stdout
   bool  m_std_out;
   /// flag if output should be on stderr
   bool  m_std_err;
   /// flag for newline
   bool  m_nl;
   /// file handler
   FILE *m_fd;

   void Start();
   void Output( const char *str );

public:
   NewSimulatorLog();
   virtual ~NewSimulatorLog();

   bool Open( int properties, const char *filename = "", int max_log_files = 1 );
   void Close();

   /**
    * Set a lock
    **/
   void Lock() {
      m_lock.Lock();
      m_lock_count++;
   }

   /**
    * Unset a lock
    **/
   void Unlock() {
      m_lock_count--;
      assert( m_lock_count >= 0 );
      m_lock.Unlock();
   }

   /**
    * Set/Unset hex flag
    * @param hex bool value - set yes/no
    **/
   void Hex( bool hex = true ) { m_hex = hex; }
   /// Is the hex flag set
   bool IsHex()                 { return m_hex;  }

   /**
    * Set/Unset time flag
    * @param t bool value - set yes/no
    **/
   void Time( bool t = true )  { m_time = t; }
   /// Is the time flag set
   bool WithTime()              { return m_time; }

   /**
    * Set/Unset recursive flag
    * @param r bool value - set yes/no
    **/
   void Recursive( bool r )    { m_recursive = true; }
   /// Is the recursive flag set
   bool IsRecursive()           { return m_recursive; }

   NewSimulatorLog &operator<<( bool b );
   NewSimulatorLog &operator<<( unsigned char c );
   NewSimulatorLog &operator<<( int i );
   NewSimulatorLog &operator<<( unsigned int i );
   NewSimulatorLog &operator<<( long l );
   NewSimulatorLog &operator<<( double d );
   NewSimulatorLog &operator<<( const char *str );

   void Log( const char *fmt, ... );
   void Hex( const unsigned char *data, int size );

   void Begin( const char *section, const char *name );
   void End();
   NewSimulatorLog &Entry( const char *entry );
};

/// Define object stdlog of class NewSimulatorLog
extern NewSimulatorLog stdlog;


#endif
