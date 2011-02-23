/** 
 * @file    new_sim_event_log.h
 *
 * The file includes the class for an event log wrapper:\n
 * NewSimulatorEventLog
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
 * This EventLog class is adapted from the simulator plugin.
 * Thanks to 
 *      W. David Ashley <dashley@us.ibm.com>
 *      Suntrupth S Yadav <suntrupth@in.ibm.com>
 */

#ifndef __NEW_SIM_EVENT_LOG_H__
#define __NEW_SIM_EVENT_LOG_H__


extern "C" {
#include "SaHpi.h"
}

#include <oh_utils.h>
#include <oh_handler.h>


/**
 * @class NewSimulatorEventLog
 * 
 * Class for wrapping event log functions
 * 
 **/
class NewSimulatorEventLog {

private:
   SaHpiEventLogCapabilitiesT capability;
   
public:
  NewSimulatorEventLog();
  virtual ~NewSimulatorEventLog();
  
  // methods for HPI functions
  SaErrorT IfELGetInfo(oh_handler_state *hstate, SaHpiEventLogInfoT *info);
  SaErrorT IfELSetState(oh_handler_state *hstate, SaHpiBoolT state);
  SaErrorT IfELGetState(oh_handler_state *hstate, SaHpiBoolT *state);
  SaErrorT IfELSetTime(oh_handler_state *hstate, SaHpiTimeT time);
  SaErrorT IfELAddEntry(oh_handler_state *hstate, const SaHpiEventT *event);
  SaErrorT IfELGetEntry(oh_handler_state *hstate, 
                        SaHpiEventLogEntryIdT current,
		                SaHpiEventLogEntryIdT *prev, 
		                SaHpiEventLogEntryIdT *next,
		                SaHpiEventLogEntryT *entry, 
		                SaHpiRdrT  *rdr, 
		                SaHpiRptEntryT *rptentry);
  SaErrorT IfELClear(oh_handler_state *hstate);
  SaErrorT IfELOverflow(oh_handler_state *hstate);
  SaErrorT IfELGetCaps(oh_handler_state *hstate, 
                       SaHpiEventLogCapabilitiesT *caps);
                       
};


#endif

