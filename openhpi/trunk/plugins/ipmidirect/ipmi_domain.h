/*
 *
 * Copyright (c) 2003,2004 by FORCE Computers
 *
 * Note that this file is based on parts of OpenIPMI
 * written by Corey Minyard <minyard@mvista.com>
 * of MontaVista Software. Corey's code was helpful
 * and many thanks go to him. He gave the permission
 * to use this code in OpenHPI under BSD license.
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

#ifndef dIpmiDomain_h
#define dIpmiDomain_h


#include <stdlib.h>
#include <string.h>


__BEGIN_DECLS
#include "SaHpi.h"
#include <openhpi.h>
#include <epath_utils.h>
#include <uid_utils.h>
__END_DECLS


#ifndef dIpmiCon_h
#include "ipmi_con.h"
#endif

#ifndef dIpmiMc_h
#include "ipmi_mc.h"
#endif

#ifndef dIpmiEntity_h
#include "ipmi_entity.h"
#endif

#ifndef dIpmiSensorHotswap_h
#include "ipmi_sensor_hotswap.h"
#endif

#ifndef dIpmiSensorThreshold_h
#include "ipmi_sensor_threshold.h"
#endif

#ifndef dIpmiControl_h
#include "ipmi_control.h"
#endif

#ifndef dIpmiDiscover_h
#include "ipmi_discover.h"
#endif


enum tIpmiAtcaSiteType
{
  eIpmiAtcaSiteTypeAtcaBoard = 0,
  eIpmiAtcaSiteTypePowerEntryModule = 1,
  eIpmiAtcaSiteTypeShelfFruInformation = 2,
  eIpmiAtcaSiteTypeDedicatedShMc = 3,
  eIpmiAtcaSiteTypeFanTray = 4,
  eIpmiAtcaSiteTypeFanFilterTray = 5,
  eIpmiAtcaSiteTypeAlarm = 6,
  eIpmiAtcaSiteTypeAtcaModule = 7,
  eIpmiAtcaSiteTypePMC = 8,
  eIpmiAtcaSiteTypeRearTransitionModule = 9,
};

// property for site types
// found by get address info
class cIpmiAtcaSiteProperty
{
public:
  unsigned int m_property;
  int          m_max_side_id;
  unsigned int m_mc_type; // dIpmiMcTypeBitXXX ipmi_discover.cpp
};


// Re-query the SEL every 5 seconds by default.
#define dIpmiSelQueryInterval 5000

// Default poll interval for MCs
#define dIpmiMcPollInterval 1000


class cIpmiDomain
{
public:
  unsigned int m_con_timeout;
  unsigned int m_con_atca_timeout;

protected:
  // ipmi connection
  cIpmiCon     *m_con;

  // true => ATCA
  int           m_is_atca;

public:
  bool IsAtca() { return m_is_atca; }

protected:
  // properties for site types
  // found by get address info
  cIpmiAtcaSiteProperty m_atca_site_property[256];

  void SetAtcaSiteProperty( tIpmiAtcaSiteType type,
                            unsigned int property, int max_id, 
                            unsigned int mc_type )
  {
    cIpmiAtcaSiteProperty *p = &m_atca_site_property[type];
    p->m_property    = property;
    p->m_max_side_id = max_id;
    p->m_mc_type     = mc_type;
  }

  // The main set of SDRs on a BMC.
  cIpmiSdrs    *m_main_sdrs;

  // The sensors that came from the main SDR.
  GList        *m_sensors_in_main_sdr;

  // Major and minor versions of the connection.
  unsigned int  m_major_version;
  unsigned int  m_minor_version;
  bool          m_sdr_repository_support;

  // A special MC used to represent the system interface.
  cIpmiMc      *m_si_mc;

public:
  cIpmiMc *SiMc()   { return m_si_mc; }

protected:
  // global lock for reading/writing:
  //   mcs, entities, sensors, frus, sels
  cThreadLockRw m_lock;

  GList        *m_mcs; // list of all MCs

public:
  void ReadLock()    { m_lock.ReadLock(); }
  void ReadUnlock()  { m_lock.ReadUnlock(); }
  void WriteLock()   { m_lock.WriteLock(); }
  void WriteUnlock() { m_lock.WriteUnlock(); }

  // lock m_initial_discover
  cThreadLock m_initial_discover_lock;

  // > 0 => initial discover in progress
  int m_initial_discover;

protected:
  // m_mc_to_check[addr] = properties
  // (dIpmiMcThreadXXXX)
  // of thread to handle mc
  unsigned int m_mc_to_check[256];

  // slot id for ATCA
  int m_mc_slot[256];

  // possible mc types like board, power unit, fan
  // (dIpmiMcTypeBitXXX ipmi_discover.cpp)
  unsigned int m_mc_type[256];

  // array of mc threads
  cIpmiMcThread *m_mc_thread[256];

public:
  unsigned int GetMcType( int slave_addr )
  {
    if ( slave_addr < 0 || slave_addr > 0xff )
       {
         assert( 0 );
         return 0;
       }

    return m_mc_type[slave_addr];
  }

  int           m_num_mc_threads;
  cThreadLock   m_mc_thread_lock;

public:
  void AddMcToScan( unsigned int addr,
                    unsigned int properties,
                    unsigned mc_type,
                    int slot )
  {
    assert( addr < 256 );

    m_mc_to_check[addr] = properties;
    m_mc_type[addr]    |= mc_type;
    m_mc_slot[addr]     = slot;
  }

  // time between mc poll in ms
  unsigned int m_mc_poll_interval;

  // time between sel rescan in ms
  unsigned int m_sel_rescan_interval;

protected:
  GList *m_entities;

public:
  cIpmiEntity *FindEntity( tIpmiDeviceNum device_num,
                           tIpmiEntityId entity_id, 
                           unsigned int entity_instance );
  void AddEntity( cIpmiEntity *ent );
  void RemEntity( cIpmiEntity *ent );


  int CheckAtca();
  int GetChannels();

/*
  enum tScanState
  {
    eScanOk,
    eScanStateRescan,
    eScanStateError
  };

  cIpmiMc *Scan( const cIpmiAddr &addr, int &missed_responses, tScanState &state );
  void SelTimerAddToList( cIpmiMc *mc );
*/

public:
  cIpmiMc *NewMc( const cIpmiAddr &addr );
  bool     CleanupMc( cIpmiMc *mc );

public:
  cIpmiDomain();
  virtual ~cIpmiDomain();

  bool Init( cIpmiCon *c );
  void Cleanup();

  cIpmiMc *FindMcByAddr( const cIpmiAddr &addr );
  cIpmiMc *FindOrCreateMcBySlaveAddr( unsigned int slave_addr );
  int      SendCommand( const cIpmiAddr &addr, const cIpmiMsg &msg, cIpmiMsg &rsp_msg,
                        int retries = dIpmiDefaultRetries );
  GList *GetSdrSensors( cIpmiMc *mc );
  void   SetSdrSensors( cIpmiMc *mc, GList *sensors );
  cIpmiMc *GetEventRcvr();

  // called from cIpmiCon to handle async events
  void HandleAsyncEvent( const cIpmiAddr &addr, const cIpmiMsg &msg );

  // called with a list of events to handle from cIpmiMcThread
  void HandleEvents( GList *list );

  // handle a single event
  void HandleEvent( cIpmiEvent *event );

  cIpmiEntity  *VerifyEntity( cIpmiEntity *ent );
  cIpmiMc      *VerifyMc( cIpmiMc *mc );
  cIpmiSensor  *VerifySensor( cIpmiSensor *s );
  cIpmiControl *VerifyControl( cIpmiControl *c );
  cIpmiFru     *VerifyFru( cIpmiFru *f );

  virtual void AddHpiEvent( oh_event *event ) = 0;
  virtual GSList *GetHpiEventList() = 0;

  // called in cIpmiEntity::AddSensor to set the initial hotswap state
  void IfHotswapSetInitialState( cIpmiEntity *ent, cIpmiSensorHotswap *sensor );

  virtual const char *EntityRoot() = 0;
  virtual oh_handler_state *GetHandler() = 0;

  virtual SaHpiRptEntryT *FindResource( SaHpiResourceIdT id ) = 0;

  void Dump( cIpmiLog &dump ) const;
};


#endif
