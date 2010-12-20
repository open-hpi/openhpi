/** 
 * @file    new_sim_domain.h
 *
 * The file includes the definition of the abstract domain class:\n
 * NewSimulatorDomain\n
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.2
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
 */
 

#ifndef __NEW_SIM_DOMAIN_H__
#define __NEW_SIM_DOMAIN_H__


#include <stdlib.h>
#include <string.h>


extern "C" {
#include "SaHpi.h"
}

#include <oh_utils.h>

#ifndef __NEW_SIM_FILE_H__
#include "new_sim_file.h"
#endif

#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_EVENT_LOG_H__
#include "new_sim_event_log.h"
#endif

#ifndef __NEW_SIM_SENSOR_H__
#include "new_sim_sensor.h"
#endif

#ifndef __NEW_SIM_SENSOR_THRESHOLD_H__
#include "new_sim_sensor_threshold.h"
#endif

#ifndef __NEW_SIM_CONTROL_H__
#include "new_sim_control.h"
#endif

#ifndef __NEW_SIM_ANNUNCIATOR_H__
#include "new_sim_annunciator.h"
#endif

#ifndef __NEW_SIM_ANNOUNCEMENT_H__
#include "new_sim_announcement.h"
#endif

#ifndef __NEW_SIM_INVENTORY_H__
#include "new_sim_inventory.h"
#endif

#ifndef __NEW_SIM_WATCHDOG_H__
#include "new_sim_watchdog.h"
#endif

#ifndef __NEW_SIM_FUMI_H__
#include "new_sim_fumi.h"
#endif

#ifndef __NEW_SIM_DIMI_H__
#include "new_sim_dimi.h"
#endif

/**
 * @class NewSimulatorDomain
 * 
 * The domain class includes funtions for the access of NewSimulatorResource objects 
 * and the verification of the rdr classes.
 * Inside the method NewSimulatorDomain:Init the preparation for reading the
 * simulation data file is done.
 **/
class NewSimulatorDomain : public NewSimulatorEventLog {

protected:
   /// pointer on the Simulation file to be used
   NewSimulatorFile        *m_file;
   /// own domain id
   SaHpiDomainIdT          m_did;
   /// own domain tag
   NewSimulatorTextBuffer  m_domain_tag;
   /// insertion timeout value
   SaHpiTimeoutT           m_insert_timeout;
   /// extraction timeout value
   SaHpiTimeoutT           m_extract_timeout;
   /// TBD: to be clarified if needed 
   bool                   m_own_domain;
   /// id of the used plugin handler
   int                    m_handler_id;
   /// flag if a watchdog is running and should be checked
   bool                   m_running_wdt;
   /// flag if a fumi is running and should be checked
   bool                   m_running_fumi;

public:
  /// return the own domain id
  SaHpiDomainIdT DomainId() { return m_did; }
  /// return a reference to the insert timeout parameter
  SaHpiTimeoutT &InsertTimeout() { return m_insert_timeout; }
  /// return a reference to the extraction timeout parameter
  SaHpiTimeoutT &ExtractTimeout() { return m_extract_timeout; }
  /// return the handler id
  int HandlerId() { return m_handler_id; }
  /// return if a watchdog timer is running
  bool HasRunningWdt() { return m_running_wdt; }
  /// return if a fumi is running
  bool HasRunningFumi() { return m_running_fumi; }
  /// set running watchdog flag
  void SetRunningWdt( bool flag ) { m_running_wdt = flag; }
  /// set running fumi flag
  void SetRunningFumi( bool flag ) { m_running_fumi = flag; }

protected:
  /// Major version 
  unsigned int  m_major_version;
  /// Minor version.
  unsigned int  m_minor_version;
  
protected:
  /// global lock for reading/writing:
  //   mcs, entities, sensors, frus, sels
  cThreadLockRw                 m_lock;
  /// Array with all resources of the rpt
  cArray<NewSimulatorResource>  m_resources; 

public:
  /// Setting a read lock
  void ReadLock()    { m_lock.ReadLock(); }
  /// Unlock the reading
  void ReadUnlock()  { m_lock.ReadUnlock(); }
  /// Setting a write lock
  void WriteLock()   { m_lock.WriteLock(); }
  /// Unlock the writing
  void WriteUnlock() { m_lock.WriteUnlock(); }
  /// Check if a lock is set
  bool CheckLock()   { return m_lock.CheckLock(); }

  /// lock m_initial_discover
  cThreadLock          m_initial_discover_lock;

  /// > 0 => initial discover in progress
  int                 m_initial_discover;


public:
  /// Return the number of resource in the domain
  int Num() { return m_resources.Num(); }
  void AddResource( NewSimulatorResource *res );
  bool CleanupResource( NewSimulatorResource *res );
  void RemResource( NewSimulatorResource *res );
  NewSimulatorResource *GetResource( int i );
  NewSimulatorResource *FindResource( NewSimulatorResource *res );
  NewSimulatorResource *FindResource( const NewSimulatorEntityPath &ep );

public:
  NewSimulatorDomain();
  virtual ~NewSimulatorDomain();

  bool Init( NewSimulatorFile *file );
  void Cleanup();

  NewSimulatorResource  *VerifyResource( NewSimulatorResource *res );
  NewSimulatorRdr       *VerifyRdr( NewSimulatorRdr *rdr );
  NewSimulatorSensor    *VerifySensor( NewSimulatorSensor *s );
  NewSimulatorControl   *VerifyControl( NewSimulatorControl *c );
  NewSimulatorAnnunciator *VerifyAnnunciator( NewSimulatorAnnunciator *a );
  NewSimulatorWatchdog  *VerifyWatchdog( NewSimulatorWatchdog *c );
  NewSimulatorFumi      *VerifyFumi( NewSimulatorFumi *f );
  NewSimulatorDimi      *VerifyDimi( NewSimulatorDimi *d );
  NewSimulatorInventory *VerifyInventory( NewSimulatorInventory *i );

  /// abstract method for adding a hpi event
  virtual void AddHpiEvent( oh_event *event ) = 0;
  /// abstract method for getting a hpi event
  virtual oh_evt_queue *GetHpiEventList() = 0;

  /// abstract method for getting a reference on the entity root
  virtual const NewSimulatorEntityPath &EntityRoot() = 0;
  /// abstract method for getting a pointer on the handler
  virtual oh_handler_state *GetHandler() = 0;
  /// abstract method for finding a resource with a given id 
  virtual SaHpiRptEntryT *FindResource( SaHpiResourceIdT id ) = 0;

  void Dump( NewSimulatorLog &dump ) const;
};


#endif
