/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <http://dague.net/sean>
 *      Peter Phan <pdphan@sourceforge.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <time.h>

#include <snmp_bc_plugin.h>

oh_el *bc_selcache = NULL;

/**
 * snmp_bc_get_sel_size:
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * 
 * Get size of event log. 
 *
 * Return values:
 * Number of event log entries - normal case.
 **/
static int snmp_bc_get_sel_size(struct oh_handler_state *handle, SaHpiResourceIdT id)
{
        int i=1;
	
	/* Go synchronize cache and hardware copy of the SEL */
        snmp_bc_check_selcache(handle, id, SAHPI_NEWEST_ENTRY);
	
	/* Return the entry count */
        i = g_list_length(handle->elcache->elentries);
        return i;
}

/**
 * snmp_bc_get_sel_size_from_hardware:
 * @ss: Pointer to SNMP session data.
 * 
 * Unfortunately, BladeCenter SNMP support does not provide access to the number 
 * of entries in the event log. This routine finds the number by sequentially 
 * reading the entire log index and counting the number of entries.
 *
 * Notice that this routine always reads one past the max event number's OID. 
 * It relies on a non-zero return code from SNMP to determine when there are 
 * no more entries.
 *
 * Return values:
 * Number of event log entries - normal case.
 **/
static int snmp_bc_get_sel_size_from_hardware(struct snmp_session *ss)
{
        struct snmp_value run_value;
        char oid[SNMP_BC_MAX_OID_LENGTH];
        int i = 1;

        do {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_INDEX_OID, i);
                i++;
        } while(snmp_get(ss, oid, &run_value) == 0);
        
        /* Think about it, and it makes sense */
        i -= 2;
        return i;
}

/**
 * snmp_bc_get_sel_info:
 * @hnd: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * @info: Location to store Event Log information.
 *
 * Returns SaHpiEventLogInfoT information about Event Log.
 * 
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - Any pointer parameter is NULL.
 **/
SaErrorT snmp_bc_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info) 
{
        char oid[SNMP_BC_MAX_OID_LENGTH];
	SaErrorT err;
        struct snmp_value first_value;
        struct oh_handler_state *handle = hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
        struct tm curtime;
        bc_sel_entry sel_entry;

	if (!hnd || !info) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
        /* Build local copy of EventLogInfo  */
        SaHpiEventLogInfoT sel = {
			     /* Max number of entries that can be stored  */
			     /* in bc EventLog varies, depending on       */
			     /* what events have been logged so far and   */
			     /* the information each logged event contains*/    	
                .Size = 512, /* This is clearly a guess but looks about right 
                              * from the 75% full errors we have seen.    */
		.UserEventMaxSize = SAHPI_MAX_TEXT_BUFFER_LENGTH,
                .Enabled = SAHPI_TRUE,
                .OverflowFlag = SAHPI_FALSE,
		.OverflowResetable = SAHPI_FALSE,
                .OverflowAction = SAHPI_EL_OVERFLOW_OVERWRITE,
        };
        

	/* In BladeCenter (bc), the newest entry is index at index 1 */
	/* Need first value to figure out what update time is */
        snprintf(oid, SNMP_BC_MAX_OID_LENGTH,"%s.%d", SNMP_BC_SEL_ENTRY_OID, 1);
        err = snmp_get(custom_handle->ss, oid, &first_value);
	if (err == SA_OK) {
        	if (first_value.type == ASN_OCTET_STR) {
			err = snmp_bc_parse_sel_entry(handle, first_value.string, &sel_entry);
                	if (err) {
                        	dbg("Cannot get first date");
				return(err);
                	} else {
                        	sel.UpdateTimestamp = (SaHpiTimeT) mktime(&sel_entry.time) * 1000000000;
                	}
        	}
        } else
		/* FIXME:                                                      */
		/* Not all err returned from snmp_get() is of SA_ERR_HPI type */
		/* May need to use the other routine, snmp_bc_snmp_get()       */
		return(err);
	
	
	err = snmp_bc_get_sp_time(handle, &curtime); 
        if ( err == SA_OK) {
                sel.CurrentTime = (SaHpiTimeT) mktime(&curtime) * 1000000000;
        } else 
		return(err);


        sel.Entries = snmp_bc_get_sel_size(handle, id);
        *info = sel;
        return(SA_OK);
}

/**
 * snmp_bc_get_sel_entry:
 * @hnd: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * @current: Current event's ID.
 * @prev: Location to store previous event's ID.
 * @next: Location to store next event's ID.
 * @entry: Location to store retrieved event.
 *
 * Gets an entry from the system Event Log.
 * 
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - Any pointer parameter is NULL.
 **/
SaErrorT snmp_bc_get_sel_entry(void *hnd,
			       SaHpiResourceIdT id,
			       SaHpiEventLogEntryIdT current,
			       SaHpiEventLogEntryIdT *prev,
			       SaHpiEventLogEntryIdT *next,
			       SaHpiEventLogEntryT *entry)
{

	SaErrorT err = SA_OK;
	oh_el_entry tmpentry, *tmpentryptr;
	tmpentryptr = &tmpentry; 

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd || !prev || !next || !entry) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Force a cache sync before servicing the request */
	err = snmp_bc_check_selcache(handle, id, current);
	if (err) {
		dbg("Event Log cache check failed.");
		return(err);
	}
	
	if (handle->elcache != NULL) {
		err = oh_el_get(handle->elcache, current, prev, next, &tmpentryptr);
		if (err) {
			dbg("Getting Event Log entry=%d from cache failed. Error=%s.", 
			    current, oh_lookup_error(err));
			return(err);
		} else {
			memcpy(entry, &(tmpentryptr->event), sizeof(SaHpiEventLogEntryT));
		}
	} else {
		dbg("Missing handle->elcache");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
		
        return(SA_OK);
}

/**
 * snmp_bc_build_selcache
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log. 
 * 
 * Builds internal event log cache.
 * 
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle is NULL.
 **/
SaErrorT snmp_bc_build_selcache(struct oh_handler_state *handle, SaHpiResourceIdT id)
{
	int current;
	SaErrorT err;
	struct snmp_bc_hnd *custom_handle = handle->data;
	
	if (!handle) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	current = snmp_bc_get_sel_size_from_hardware(custom_handle->ss);
	if (current) {
		do {
			err = snmp_bc_sel_read_add(handle, id, current);
			/* FIXME:: What do we do on error - break or just record ???*/
			current--;
		} while(current > 0);
	}

	return(SA_OK);
}

/**
 * snmp_bc_check_selcache:
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns Event Log.
 * @entryId: Event Log entry ID.
 * 
 * Sync Event Log's cache. If this is first entry, then create the
 * event log cache.
 * 
 * Return values:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @handler is NULL.
**/
SaErrorT snmp_bc_check_selcache(struct oh_handler_state *handle,
				SaHpiResourceIdT id,
				SaHpiEventLogEntryIdT entryId)
{
	SaErrorT err;

	if (!handle) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (g_list_length(handle->elcache->elentries) == 0) {
		err = snmp_bc_build_selcache(handle, id);
	} else {
		err = snmp_bc_selcache_sync(handle, id, entryId);
	}
	
	if (err) {
		dbg("Event Log cache build/sync failed. Error=%s", oh_lookup_error(err));
		return(err);
	}

	return(SA_OK);
}

/**
 * snmp_bc_selcache_sync
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns Event Log.
 * @entryId: Event Log entry ID.
 * 
 * Synchronizes interal event log cache. 
 * 
 * Return values:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @handle is NULL.
 **/
SaErrorT snmp_bc_selcache_sync(struct oh_handler_state *handle,
			       SaHpiResourceIdT id,
			       SaHpiEventLogEntryIdT entryId)
{
	SaHpiEventLogEntryIdT current;
	SaHpiEventLogEntryIdT prev;
	SaHpiEventLogEntryIdT next;
        struct snmp_value get_value;
        struct snmp_bc_hnd *custom_handle = handle->data;
        bc_sel_entry sel_entry;
        oh_el_entry *fetchentry;
        SaHpiTimeT new_timestamp;
	char oid[SNMP_BC_MAX_OID_LENGTH];
	SaErrorT err;
	int cacheupdate = 0;

	if (!handle) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_el_get(handle->elcache, SAHPI_NEWEST_ENTRY, &prev, &next, &fetchentry);
	if (err) fetchentry = NULL;
		
	current = 1;
	snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID, current);
       	err = snmp_get(custom_handle->ss, oid, &get_value);
       	if (err) {
		dbg("SNMP log is empty.");
		err = oh_el_clear(handle->elcache);
		return(err);
	}

	if (fetchentry == NULL) {
		err = snmp_bc_build_selcache(handle, id);
		return(err);
	}

	if (snmp_bc_parse_sel_entry(handle, get_value.string, &sel_entry) < 0) {
		dbg("Cannot parse Event Log entry");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	new_timestamp = (SaHpiTimeT)mktime(&sel_entry.time) * 1000000000;
	if (fetchentry->event.Event.Timestamp != new_timestamp) {
		while (1) {
			current++;
			snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID, current);
			err = snmp_get(custom_handle->ss,oid,&get_value);
			if (err == 0) {
				if (snmp_bc_parse_sel_entry(handle, get_value.string, &sel_entry) < 0) {
					dbg("Cannot parse SEL entry.");
					return(SA_ERR_HPI_INTERNAL_ERROR);
				}
				
				if ((fetchentry->event.Event.Timestamp == 
				     (SaHpiTimeT)mktime(&sel_entry.time) * 1000000000)) {
					current--;
					cacheupdate = 1;	
					break;
				}
			} else {
				dbg("Old entry not found and end of SNMP log reached.");
				break;
			}
		}
		
		if (cacheupdate) {
			do {
				err = snmp_bc_sel_read_add (handle, id, current);
				/* FIXME:: What if err ??? */
				current--;
			} while(current > 0);
		} else {
			err = oh_el_clear(handle->elcache);
			/* FIXME:: What if err ??? */
			err =snmp_bc_build_selcache(handle, id);
			/* FIXME:: What if err ??? */
		}
	} else {
		trace("EL Sync: there are no new entry indicated.\n");
	}
	
	return(SA_OK);  
}

/**
 * snmp_bc_set_sel_time:
 * @hnd: Pointer to handler's data. 
 * @id: Resource's ID that owns Event Log.
 * @time: HPI time.
 * 
 * Set Event Log's time.
 * 
 * Return values: 
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @hnd is NULL.
 **/
SaErrorT snmp_bc_set_sel_time(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time)
{
        struct oh_handler_state *handle = hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
        struct tm tv;
        time_t tt;
        SaErrorT err;

	if (!hnd) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);    
	}

        tt = time / 1000000000;
        localtime_r(&tt, &tv);
	
	err = snmp_bc_set_sp_time(custom_handle->ss, &tv);
        if (err) {
		dbg("Cannot set time. Error=%s.", oh_lookup_error(err));
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

        return(SA_OK);
}

/**
 * snmp_bc_add_sel_entry:
 * @hnd: Pointer to handler's data. 
 * @id: Resource's ID that owns Event Log.
 * @Event: Location of event to be added. 
 * 
 * Add is not supported by the hardware.
 * 
 * Return values:
 * SA_ERR_HPI_INVALID_CMD - normal operation.
 **/
SaErrorT snmp_bc_add_sel_entry(void *hnd, SaHpiResourceIdT id, const SaHpiEventT *Event)
{
        return(SA_ERR_HPI_INVALID_CMD);
}

/**
 * snmp_bc_sel_read_add:
 * @hnd: Pointer to handler's data. 
 * @id: Resource's ID that owns Event Log.
 * @current: Current Event Log entry ID.
 * 
 *  Add event to Event Log.
 * 
 * Return values:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @hnd is NULL.
 **/
SaErrorT snmp_bc_sel_read_add (struct oh_handler_state *handle,
			       SaHpiResourceIdT id, 
			       SaHpiEventLogEntryIdT current)
{

	int isdst=0;
	int event_enabled;
        char oid[SNMP_BC_MAX_OID_LENGTH];
	bc_sel_entry sel_entry;

	SaErrorT err;
        SaHpiEventT tmpevent;
	SaHpiEntryIdT rdrid=0;
	SaHpiRdrT *rdr_ptr=NULL; 

        struct snmp_value get_value;
        struct snmp_bc_hnd *custom_handle = handle->data;

	if (!handle) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID, current);
	err = snmp_get(custom_handle->ss, oid, &get_value);
	if (err != SA_OK)
		 return(err); 
	else if ((err == SA_OK) && (get_value.type != ASN_OCTET_STR)) {
		dbg("Cannot get EL entry");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	err = snmp_bc_parse_sel_entry(handle,get_value.string, &sel_entry);
	if (err != SA_OK) return(err);
		
	isdst = sel_entry.time.tm_isdst;
	snmp_bc_log2event(handle, get_value.string, &tmpevent, isdst, &event_enabled);
		
/* FIXME:: Nice to have an event to rdr pointer function - this same code appears in snmp_bc_event.c */
/* in rpt_utils.c ??? */		
/* FIXME:: Add B.1.1. types */		
	switch (tmpevent.EventType) {
		case SAHPI_ET_OEM:
		case SAHPI_ET_HOTSWAP:
		case SAHPI_ET_USER:
			/* FIXME:: Why do this - just set rdr_ptr = NULL ??? */
			/* rdr storage is temporary */
			#if 0
			rdr.RecordId = 0; /* There is no RDR associated to OEM event */
                                          /* Set RDR ID to invalid value of 0        */
			#endif
			rdr_ptr = NULL;
			break;		  
		case SAHPI_ET_SENSOR:
			rdrid = get_rdr_uid(SAHPI_SENSOR_RDR,
					    tmpevent.EventDataUnion.SensorEvent.SensorNum); 
			rdr_ptr = oh_get_rdr_by_id(handle->rptcache, tmpevent.Source, rdrid);
			break;
		case SAHPI_ET_WATCHDOG:
			rdrid = get_rdr_uid(SAHPI_WATCHDOG_RDR,
					    tmpevent.EventDataUnion.WatchdogEvent.WatchdogNum);
			rdr_ptr = oh_get_rdr_by_id(handle->rptcache, tmpevent.Source, rdrid);
			break;
		default:
			dbg("Unrecognized Event Type=%d.", tmpevent.EventType);
			return(SA_ERR_HPI_INTERNAL_ERROR);
			break;
	} 

	/* Since oh_el_append() does a copy of RES and RDR into it own data struct, */ 
	/* just pass the pointers to it.                                            */
	err = oh_el_append(handle->elcache, &tmpevent,
			rdr_ptr, oh_get_resource_by_id(handle->rptcache, id));
	
	if (err != SA_OK)
		 dbg("Err adding entry to elcache, error %s\n", oh_lookup_error(err));
		
	if (event_enabled)
		err = snmp_bc_add_to_eventq(handle, &tmpevent);
			
        return(err);
}

/**
 * snmp_bc_parse_sel_entry:
 * @handle: Pointer to handler data.
 * @logstr: Hardware log string.
 * @sel: blade center system event log
 * 
 * Parses a hardware log entry into its various components. 
 * Another transform has to happen to turn this into an HPI entry. 
 * 
 * Return values:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @handle, @logstr, @sel NULL.
 **/
SaErrorT snmp_bc_parse_sel_entry(struct oh_handler_state *handle, char *logstr, bc_sel_entry *sel)
{
        bc_sel_entry ent;
        char level[8];
        char *findit;
	
	if (!handle || !logstr || !sel) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        /* Severity first */
	findit = strstr(logstr, "Severity:");
	if (findit != NULL) {
        	if(sscanf(findit,"Severity:%7s",level)) {
                	if(strcmp(level,"INFO") == 0) {
                        	ent.sev = SAHPI_INFORMATIONAL;
                	} else if(strcmp(level,"WARN") == 0) {
                        	ent.sev = SAHPI_MINOR;
                	} else if(strcmp(level,"ERR") == 0) {
                        	ent.sev = SAHPI_CRITICAL;
                	} else {
                        	ent.sev = SAHPI_DEBUG;
                	}
        	} else {
                	dbg("Cannot parse severity from log entry.");
                	return(SA_ERR_HPI_INTERNAL_ERROR);
        	}
	}

	findit = strstr(logstr, "Source:");
	if (findit != NULL) {
        	if(!sscanf(findit,"Source:%19s",ent.source)) {
                	dbg("Cannot parse source from log entry.");
                	return(SA_ERR_HPI_INTERNAL_ERROR);
        	}
	} else {
		dbg("Premature data termination.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	findit = strstr(logstr, "Name:");
	if (findit != NULL) {
        	if(!sscanf(findit,"Name:%19s",ent.sname)) {
                	dbg("Cannot parse name from log entry.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
        	}
	} else {
		dbg("Premature data termination.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
        
	findit = strstr(logstr, "Date:");
	if (findit != NULL) {
        	if(sscanf(findit,"Date:%2d/%2d/%2d  Time:%2d:%2d:%2d",
                	  &ent.time.tm_mon, &ent.time.tm_mday, &ent.time.tm_year, 
                  	&ent.time.tm_hour, &ent.time.tm_min, &ent.time.tm_sec)) {
			snmp_bc_set_dst(handle, &ent.time);
                	ent.time.tm_mon--;
                	ent.time.tm_year += 100;
        	} else {
                	dbg("Cannot parse date/time from log entry.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
        	}
	} else {
		dbg("Premature data termination.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
        }
        
	findit = strstr(logstr, "Text:");
	if (findit != NULL) {
        	/* Advance to data */
        	findit += 5;
        	strncpy(ent.text,findit, SNMP_BC_MAX_SEL_ENTRY_LENGTH - 1);
        	ent.text[SNMP_BC_MAX_SEL_ENTRY_LENGTH - 1] = '\0';
	} else {
		dbg("Premature data termination.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}        

        *sel = ent;
        return(SA_OK);
}

/**
 * snmp_bc_clear_sel:
 * @hnd: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * 
 * Clears the system event log.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @hnd is NULL.
 **/
SaErrorT snmp_bc_clear_sel(void *hnd, SaHpiResourceIdT id)
{
	struct snmp_value set_value;
        struct oh_handler_state *handle = hnd;
        struct snmp_bc_hnd *custom_handle = handle->data;
	SaErrorT err;

	if (!hnd) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	err = oh_el_clear(handle->elcache);
	if (err) {
		dbg("Cannot clear system Event Log. Error=%s.", oh_lookup_error(err));
		return(err);
	}

	set_value.type = ASN_INTEGER;
	set_value.str_len = 1;
	set_value.integer = (long) clearEventLogExecute;
	err = snmp_set(custom_handle->ss, SNMP_BC_CLEAR_SEL_OID, set_value);
	if (err) {
		dbg("SNMP set failed. Error=%s.", oh_lookup_error(err));
		return(err);
	}
		
	return(SA_OK);
}
