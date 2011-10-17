/** 
 * @file    new_sim.h
 *
 * The file includes the interface class to the abi:\n
 * NewSimulator\n
 * It also includes implementation of some HPI APIs.
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
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *     Andy Cress        <arcress@users.sourceforge.net>
 *      
 */
 
#ifndef __NEW_SIM_H__
#define __NEW_SIM_H__


#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef __NEW_SIM_DOMAIN_H__
#include "new_sim_domain.h"
#endif

/// Simulator magic number
#define dNewSimulatorMagic 0x47110815

/**
 * @class NewSimulator
 * 
 * Interface to the openhpi abi
 **/
class NewSimulator : public NewSimulatorDomain {
   /// Magic number
   unsigned int         m_magic;
   /// pointer on the oh_handler
   oh_handler_state      *m_handler;
   /// root entity path
   NewSimulatorEntityPath m_entity_root;

   bool GetParams( GHashTable *handler_config );

public:
  /**
   * Check the Magic number
   * 
   * @return true if it is the same defined by dNewSimulatorMagic
   **/
  bool CheckMagic() {
    if ( m_magic == dNewSimulatorMagic ) return true;
    return false;
  }

  /**
   * Check the oh_handler
   * 
   * @param handler pointer to be verified
   * @return true if it is the same
   **/
  bool CheckHandler( oh_handler_state *handler ) {
    if ( handler == m_handler ) return true;
    return false;
  }

  virtual void IfEnter();
  virtual void IfLeave();

  // openhpi abi interface functions
  virtual bool IfOpen( GHashTable *handler_config );
  virtual void IfClose();
  virtual SaErrorT IfGetEvent( oh_event *event );
  virtual SaErrorT IfDiscoverResources();

  virtual SaErrorT IfSetResourceTag( NewSimulatorResource *ent, SaHpiTextBufferT *tag );
  virtual SaErrorT IfSetResourceSeverity( NewSimulatorResource *res, SaHpiSeverityT sev );

  // hot swap
  virtual SaErrorT IfSetAutoInsertTimeout( SaHpiTimeoutT  timeout);

  virtual SaErrorT IfGetPowerState    ( NewSimulatorResource *res, SaHpiPowerStateT &state );
  virtual SaErrorT IfSetPowerState    ( NewSimulatorResource *res, SaHpiPowerStateT state );
  virtual SaErrorT IfGetIndicatorState( NewSimulatorResource *res, SaHpiHsIndicatorStateT &state );
  virtual SaErrorT IfSetIndicatorState( NewSimulatorResource *res, SaHpiHsIndicatorStateT state );
  virtual SaErrorT IfGetResetState    ( NewSimulatorResource *res, SaHpiResetActionT &state );
  virtual SaErrorT IfSetResetState    ( NewSimulatorResource *res, SaHpiResetActionT state );

  virtual SaErrorT IfControlParm( NewSimulatorResource *res, SaHpiParmActionT act );

  /// lock for the hpi event queue
  cThreadLock m_event_lock;

  virtual void AddHpiEvent( oh_event *event );
  
  /**
   * Return the HPI event list 
   * 
   * @return pointer on the HPI event queue
   **/
  virtual oh_evt_queue *GetHpiEventList() { return m_handler->eventq; }

  NewSimulator();
  ~NewSimulator();

  void SetHandler( oh_handler_state *handler );
  oh_handler_state *GetHandler();

  virtual const NewSimulatorEntityPath &EntityRoot();
  virtual SaHpiRptEntryT *FindResource( SaHpiResourceIdT id );
};


#endif
