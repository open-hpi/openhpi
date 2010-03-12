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

#ifndef __NEW_SIM_WATCHDOG_H__
#define __NEW_SIM_WATCHDOG_H__


#ifndef __NEW_SIM_RDR_h__
#include "new_sim_rdr.h"
#endif

extern "C" {
#include "SaHpi.h"
}

class NewSimulatorWatchdog : public NewSimulatorRdr {
protected:
   unsigned int         m_num; // control num
   unsigned int         m_oem;

public:
   NewSimulatorWatchdog( NewSimulatorResource *res,
                         unsigned int num,
                         unsigned int oem );
  ~NewSimulatorWatchdog();

  unsigned int Num() const { return m_num; }
  unsigned int Oem() const { return m_oem; }

  // create an RDR sensor record
  bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  SaErrorT GetWatchdogInfo( SaHpiWatchdogT &watchdog);
  SaErrorT SetWatchdogInfo( SaHpiWatchdogT &watchdog);
  SaErrorT ResetWatchdog();
};


#endif
