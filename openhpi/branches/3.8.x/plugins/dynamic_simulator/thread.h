/** 
 * @file    thread.h
 *
 * The file includes classes for thread handling:\n
 * cThread\n
 * cThreadLock\n
 * cThreadLockAuto\n
 * cThreadLockRw\n
 * cThreadCond\n
 * 
 * @author  Thomas Kanngieser <thomas.kanngieser@fci.com>
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net> (documentation only)
 * @version 1.0
 * @date    2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Copyright (c) 2004 by FORCE Computers.  
 */
#ifndef __THREAD_H__
#define __THREAD_H__


#include <pthread.h>
#include <unistd.h>

/// states of a thread
enum tTheadState
{
  eTsUnknown,
  eTsSuspend,
  eTsRun,
  eTsExit
};


/**
 * @class cThread
 * 
 * abstract class to support threads
 **/
class cThread
{
protected:
  /// thread structure
  pthread_t   m_thread;
  bool        m_main;  //!< true => main thread
  /// state of the thread
  tTheadState m_state;

  static void *Thread( void *param );

public:
  cThread();
  cThread( const pthread_t &thread, bool main_thread, tTheadState state );
  virtual ~cThread();

  // get the current thread class
  static cThread *GetThread();

  // start thread
  virtual bool Start();

  // wait for thread termination
  virtual bool Wait( void *&rv );

  /// return if the thread is running
  bool IsRunning() { return m_state == eTsRun; }
  /// return if it is the main thread
  bool IsMain()  { return m_main; }

protected:
  /// abstract run method to be implemented by childs
  virtual void *Run() = 0;
  virtual void Exit( void *rv );
};


/**
 * @class cThreadLock
 * 
 * Supports the locking
 **/
class cThreadLock
{
protected:
  /// mutex lock variable
  pthread_mutex_t m_lock;

public:
  cThreadLock();
  virtual ~cThreadLock();

  virtual void Lock();
  virtual void Unlock();

  virtual bool TryLock();
};

/**
 * @class cThreadLockAuto
 * 
 * The lock is set at calling the constructor and unlock 
 * in the destructor
 **/
class cThreadLockAuto
{
  /// holding the lock
  cThreadLock &m_lock;

public:
  /// Constructor with lock address which is set
  cThreadLockAuto( cThreadLock &lock )
    : m_lock( lock )
  {
    m_lock.Lock();
  }
  /// Destructor with unlock
  ~cThreadLockAuto()
  {
    m_lock.Unlock();
  }
};


/**
 * @class cThreadLockRw
 * 
 * Using read and write locks 
 **/
class cThreadLockRw
{
protected:
  /// holding the read/write lock
  pthread_rwlock_t m_rwlock;

public:
  cThreadLockRw();
  virtual ~cThreadLockRw();

  virtual void ReadLock();
  virtual void ReadUnlock();
  virtual bool TryReadLock();

  virtual void WriteLock();
  virtual void WriteUnlock();
  virtual bool TryWriteLock();

  // true => no lock held
  bool CheckLock();
};

/**
 * @class cThreadCond
 * 
 * holding the thread conditions
 **/
class cThreadCond : public cThreadLock
{
protected:
  /// conditions
  pthread_cond_t m_cond;

public:
  cThreadCond();
  virtual ~cThreadCond();

  // call Lock before Signal
  virtual void Signal();

  // call Lock before Wait
  virtual void Wait();
};


#endif
