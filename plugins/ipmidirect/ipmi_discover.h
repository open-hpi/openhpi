/*
 *
 * Copyright (c) 2004 by FORCE Computers
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#ifndef dIpmiDiscover_h
#define dIpmiDiscover_h


class cIpmiDomain;
class cIpmiMcThread;
class cIpmiMcTask;


/*
enum tIpmiMcType
{
  eIpmiMcTypeBoard,
  eIpmiMcTypePower,
  eIpmiMcTypeFan
};


// cIpmiMcThread::m_types
#define dIpmiMcTypeBitBoard (1<<eIpmiMcTypeBoard)
#define dIpmiMcTypeBitPower (1<<eIpmiMcTypePower)
#define dIpmiMcTypeBitFan   (1<<eIpmiMcTypeFan)
#define dIpmiMcTypeBitAll   0xffffffff
*/


// cIpmiMcThread::m_properties
#define dIpmiMcThreadInitialDiscover 1 // discover at startup
#define dIpmiMcThreadPollAliveMc     2 // poll mc if found
#define dIpmiMcThreadPollDeadMc      4 // poll mc if not found
#define dIpmiMcThreadCreateM0        8 // create hotswap state M0


typedef void (cIpmiMcThread::*tIpmiMcTask)( void *userdata );

class cIpmiMcThread : public cThread
{
private:
  cIpmiDomain  *m_domain;

  void WriteLock()   { /*m_domain->WriteLock();*/ }
  void WriteUnlock() { /*m_domain->WriteUnlock();*/ }
  void ReadLock()    { /*m_domain->ReadLock();*/ }
  void ReadUnlock()  { /*m_domain->ReadUnlock();*/ }
  
  unsigned char m_addr;
  //  unsigned int  m_type; // dIpmiMcTypeBitXXXX
  //int           m_slot;
  cIpmiMc      *m_mc;

  // properties
  unsigned int m_properties; // dIpmiMcThreadXXXX

public:
  //unsigned int Type() { return m_type; }
  //int          Slot() { return m_slot; }
  cIpmiMc     *Mc()   { return m_mc; }

protected:
  virtual void *Run();

public:
  // signal to MC thread to exit
  bool m_exit;

  cIpmiMcThread( cIpmiDomain  *domain,
                 unsigned char addr,
                 unsigned int  properties /*,
                 unsigned int  mc_type,
                 int           slot */ );

  ~cIpmiMcThread();

protected:
  cIpmiMcTask *m_tasks;

public:
  // add a task to MC thread
  void AddMcTask( tIpmiMcTask task, const cTime &timeout,
                  void *userdata );

  // add a task to MC thread
  void AddMcTask( tIpmiMcTask task, unsigned int diff_ms,
                  void *userdata );

  // remove MC task from list
  bool RemMcTask( void *userdata );

  // clear the MC task list
  void ClearMcTaskList();

protected:
  // discover MC
  void Discover( cIpmiMsg *get_device_id_rsp = 0 );

  // poll mc task
  void PollAddr( void *userdata );

  cIpmiSel *m_sel;

  // read SEL task
  void ReadSel( void *userdata );

  GList *m_events;
  cThreadLock m_events_lock;

  void HandleEvents();

  void HandleEvent( cIpmiEvent *event );
  void HandleHotswapEvent( cIpmiSensorHotswap *sensor, cIpmiEvent *event );

public:
  void AddEvent( cIpmiEvent *event );
};


#endif
