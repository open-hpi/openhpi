/** 
 * @file    new_sim_event_log.cpp
 *
 * The file includes the implementation of a class for an event log wrapper:
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

#include "new_sim_event_log.h"
#include "new_sim_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

/**
 * Constructor
 **/
NewSimulatorEventLog::NewSimulatorEventLog() {
	
   capability = SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD |
                SAHPI_EVTLOG_CAPABILITY_CLEAR |
                SAHPI_EVTLOG_CAPABILITY_TIME_SET |
                SAHPI_EVTLOG_CAPABILITY_STATE_SET;
}

                      
/**
 * Destructor
 **/
NewSimulatorEventLog::~NewSimulatorEventLog() {
}

// Official HPI functions
/**
 * HPI function saHpiEventLogInfoGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param info pointer to the returned Event Log information
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELGetInfo(oh_handler_state *hstate, 
                                           SaHpiEventLogInfoT *info) {
   if (info == NULL)
      return SA_ERR_HPI_INVALID_PARAMS;
   
   return oh_el_info(hstate->elcache, info);
}


/**
 * HPI function saHpiEventLogStateSet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param state event Log state to be set
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELSetState(oh_handler_state *hstate, 
                                            SaHpiBoolT state) {
                                            	
   return oh_el_enableset(hstate->elcache, state);
}


/**
 * HPI function saHpiEventLogStateGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param state pointer to the current event log enable state
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELGetState(oh_handler_state *hstate, 
                                            SaHpiBoolT *state) {
   SaHpiEventLogInfoT elinfo;
   SaErrorT rv;
   	
   rv = oh_el_info(hstate->elcache, &elinfo);
   *state = elinfo.Enabled;
	
   return rv;
}


/**
 * HPI function saHpiEventLogTimeSet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param time  time to which the Event Log clock should be set
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELSetTime(oh_handler_state *hstate, 
                                           SaHpiTimeT time) {

   return oh_el_timeset(hstate->elcache, time);
}


/**
 * HPI function saHpiEventLogEntryAdd()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param event  pointer to event data to write to the Event Log
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELAddEntry(oh_handler_state *hstate, 
                                            const SaHpiEventT *event) {
   
   if (!event)
       return SA_ERR_HPI_INVALID_PARAMS;
                             	
   return  oh_el_append(hstate->elcache, event, NULL, NULL);
}


/**
 * HPI function saHpiEventLogEntryGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param current  Identifier of event log entry to retrieve
 * @param prev Event Log entry identifier for the previous entry
 * @param next  Event Log entry identifier for the next entry
 * @param entry Pointer to retrieved Event Log entry
 * @param rdr Pointer to structure to receive resource data record
 * @param rptentry Pointer to structure to receive RPT entry
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELGetEntry(oh_handler_state *hstate, 
                        SaHpiEventLogEntryIdT current,
		                SaHpiEventLogEntryIdT *prev, 
		                SaHpiEventLogEntryIdT *next,
		                SaHpiEventLogEntryT   *entry, 
		                SaHpiRdrT             *rdr, 
		                SaHpiRptEntryT        *rptentry) {
   
   SaErrorT rv;
   oh_el_entry tmpentry, *tmpentryptr;
   tmpentryptr = &tmpentry;
   
   if (!prev || !next || !entry)
      return SA_ERR_HPI_INVALID_PARAMS;

   rv = oh_el_get(hstate->elcache, current, prev, next, &tmpentryptr);
   
   if (rv != SA_OK) 
      return rv;

   memcpy(entry, &(tmpentryptr->event), sizeof(SaHpiEventLogEntryT));
   
   if (rdr)
      memcpy(rdr, &tmpentryptr->rdr, sizeof(SaHpiRdrT));
   
   if (rptentry)
      memcpy(rptentry, &(tmpentryptr->res), sizeof(SaHpiRptEntryT));
   
   return rv;
}


/**
 * HPI function saHpiEventLogClear()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELClear(oh_handler_state *hstate) {

   return oh_el_clear(hstate->elcache); 
}


/**
 * HPI function saHpiEventLogOverflowReset()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELOverflow(oh_handler_state *hstate) {

   return oh_el_overflowreset(hstate->elcache);
}


/**
 * HPI function saHpiEventLogCapabilitiesGet()
 * 
 * See also the description of the function inside the specification or header file.
 * 
 * @param hstate pointer on the handler state struct
 * @param caps Pointer to store the Event Log Capabilities value
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorEventLog::IfELGetCaps(oh_handler_state *hstate, 
                                   SaHpiEventLogCapabilitiesT *caps) {

   *caps = capability;
   
   if (hstate->elcache->info.OverflowResetable) {
      *caps |= SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET;
   }
   
   return SA_OK;
}


