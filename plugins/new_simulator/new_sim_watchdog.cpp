/*
 * Copyright (c) 2009 by Lars Wetzel
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 * 
 * Author(s):
 *     Lars Wetzel       <larswetzel@users.sourceforge.net>
 */

#include "new_sim_watchdog.h"
#include "new_sim_domain.h"


NewSimulatorWatchdog::NewSimulatorWatchdog( NewSimulatorMc *mc,
                                            unsigned int num,
                                            unsigned int oem )
                    : NewSimulatorRdr( mc, SAHPI_WATCHDOG_RDR ),
                      m_num( num ),
                      m_oem( oem ) {}

NewSimulatorWatchdog::~NewSimulatorWatchdog() {}

bool NewSimulatorWatchdog::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
   if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
      return false;

   // update resource
   resource.ResourceCapabilities |= SAHPI_CAPABILITY_RDR|SAHPI_CAPABILITY_WATCHDOG;

   // watchdog record
   SaHpiWatchdogRecT &rec = rdr.RdrTypeUnion.WatchdogRec;

   rec.WatchdogNum = m_num;
   rec.Oem         = m_oem;

   return true;
}


SaErrorT NewSimulatorWatchdog::GetWatchdogInfo( SaHpiWatchdogT &watchdog ) {
   SaErrorT rv = SA_OK;
   stdlog << "GetWatchdogInfo: num " << m_num << "\n";

   return rv;
}

SaErrorT NewSimulatorWatchdog::SetWatchdogInfo( SaHpiWatchdogT &watchdog ) {
  SaErrorT rv = SA_OK;

  stdlog << "SetWatchdogInfo: num " << m_num << "\n";

  return (rv);
}

SaErrorT NewSimulatorWatchdog::ResetWatchdog() {
   // TODO: Here we have to start a WatchdogTimer in a new thread to be asynchron
   SaErrorT rv = SA_OK;

   stdlog << "ResetWatchdog: num " << m_num << "\n";
   // add functionality here 

   return (rv);
}
