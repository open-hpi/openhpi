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
        int i = 1;
	
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
static int snmp_bc_get_sel_size_from_hardware(struct snmp_bc_hnd *custom_handle)
{
        struct snmp_value run_value;
        char oid[SNMP_BC_MAX_OID_LENGTH];
        int i = 1;

        do {
		if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
			snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_INDEX_OID_RSA, i);
		}
		else {
			snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_INDEX_OID, i);
		}
                i++;
        } while(snmp_bc_snmp_get(custom_handle, oid, &run_value, SAHPI_TRUE) == 0);
        
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
        struct tm curtime;
        sel_entry sel_entry;

	if (!hnd || !info) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
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

	/* In Event Log, the newest entry is index at index 1 */
	/* Need first value to figure out what update time is */
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH,"%s.%d", SNMP_BC_SEL_ENTRY_OID_RSA, 1);
	}
	else {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH,"%s.%d", SNMP_BC_SEL_ENTRY_OID, 1);
	}

        err = snmp_bc_snmp_get(custom_handle, oid, &first_value, SAHPI_TRUE);
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
		return(err);
	
	
	err = snmp_bc_get_sp_time(handle, &curtime); 
        if ( err == SA_OK) {
                sel.CurrentTime = (SaHpiTimeT) mktime(&curtime) * 1000000000;
        } else 
		return(err);


        sel.Entries = snmp_bc_get_sel_size(handle, id);
	sel.OverflowFlag = handle->elcache->overflow;
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
			       SaHpiEventLogEntryT *entry,
			       SaHpiRdrT  *rdr,
                               SaHpiRptEntryT  *rptentry)
{

	SaErrorT err = SA_OK;
	oh_el_entry tmpentry, *tmpentryptr;
	tmpentryptr = &tmpentry; 

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd || !prev || !next || !entry) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}


	if (handle->elcache != NULL) {
		/* Force a cache sync before servicing the request */
		err = snmp_bc_check_selcache(handle, id, current);
		if (err) {
			dbg("Event Log cache check failed.");
			return(err);
		}
	
		err = oh_el_get(handle->elcache, current, prev, next, &tmpentryptr);
		if (err) {
			dbg("Getting Event Log entry=%d from cache failed. Error=%s.", 
			    current, oh_lookup_error(err));
			return(err);
		} else {
			memcpy(entry, &(tmpentryptr->event), sizeof(SaHpiEventLogEntryT));
                        if (rdr)
                                memcpy(rdr, &tmpentryptr->rdr, sizeof(SaHpiRdrT));
                        else
                                trace("NULL rdrptr with SaHpiEventLogEntryGet()\n");

                        if (rptentry)
                                memcpy(rptentry, &(tmpentryptr->res), sizeof(SaHpiRptEntryT));
                        else
                                trace("NULL rptptr with SaHpiEventLogEntryGet()\n");	
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
	
	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct snmp_bc_hnd *custom_handle = handle->data;

	current = snmp_bc_get_sel_size_from_hardware(custom_handle);
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
		dbg("Invalid parameter.");
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
        sel_entry sel_entry;
        oh_el_entry *fetchentry;
        SaHpiTimeT new_timestamp;
	char oid[SNMP_BC_MAX_OID_LENGTH];
	SaErrorT err;
	int cacheupdate = 0;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        struct snmp_bc_hnd *custom_handle = handle->data;

	err = oh_el_get(handle->elcache, SAHPI_NEWEST_ENTRY, &prev, &next, &fetchentry);
	if (err) fetchentry = NULL;
		
	current = 1;
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID_RSA, current);
	}
	else {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID, current);
	}

       	err = snmp_bc_snmp_get(custom_handle, oid, &get_value, SAHPI_TRUE);
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
			if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
				snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d",
					 SNMP_BC_SEL_ENTRY_OID_RSA, current);
			}
			else {
				snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d",
					 SNMP_BC_SEL_ENTRY_OID, current);
			}
			err = snmp_bc_snmp_get(custom_handle,oid,&get_value, SAHPI_TRUE);
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

        struct tm tv;
        time_t tt;
        SaErrorT err;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);    
	}
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        tt = time / 1000000000;
        localtime_r(&tt, &tv);
	
	err = snmp_bc_set_sp_time(custom_handle, &tv);
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
        char oid[SNMP_BC_MAX_OID_LENGTH];
	sel_entry sel_entry;

	SaErrorT err;
        SaHpiEventT tmpevent;
	SaHpiEntryIdT rdrid=0;
	SaHpiRdrT rdr, *rdr_ptr=NULL; 

        struct snmp_value get_value;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        struct snmp_bc_hnd *custom_handle = handle->data;

	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d",
			 SNMP_BC_SEL_ENTRY_OID_RSA, current);
	}
	else {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d",
			 SNMP_BC_SEL_ENTRY_OID, current);
	}

	err = snmp_bc_snmp_get(custom_handle, oid, &get_value, SAHPI_TRUE);
	if (err != SA_OK)
		 return(err); 
	else if ((err == SA_OK) && (get_value.type != ASN_OCTET_STR)) {
		dbg("Cannot get EL entry");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	err = snmp_bc_parse_sel_entry(handle,get_value.string, &sel_entry);
	if (err != SA_OK) return(err);
		
	if (strncmp(get_value.string, EVT_EN_LOG_FULL, sizeof(EVT_EN_LOG_FULL) == 0))
		 handle->elcache->overflow = SAHPI_TRUE;
		 
	isdst = sel_entry.time.tm_isdst;
	snmp_bc_log2event(handle, get_value.string, &tmpevent, isdst);
		
/* FIXME:: Nice to have an event to rdr pointer function - this same code appears in snmp_bc_event.c */
/* in rpt_utils.c ??? */		
/* FIXME:: Add B.1.1. types */		
	switch (tmpevent.EventType) {
		case SAHPI_ET_OEM:
		case SAHPI_ET_HOTSWAP:
		case SAHPI_ET_USER:
                        memset(&rdr, 0, sizeof(SaHpiRdrT));
                                        /* There is no RDR associated to OEM event */
                        rdr.RdrType = SAHPI_NO_RECORD;
                                          /* Set RDR Type to SAHPI_NO_RECORD, spec B-01.01 */
                                          /* It is redundant because SAHPI_NO_RECORD == 0  */
                                          /* This code is here for clarity.                */
                        rdr_ptr = &rdr;

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
	id = tmpevent.Source;
	if (NULL == oh_get_resource_by_id(handle->rptcache, id))
					dbg("NULL RPT for rid %d.", id);
	err = oh_el_append(handle->elcache, &tmpevent,
			rdr_ptr, oh_get_resource_by_id(handle->rptcache, id));
	
	if (err) dbg("Cannot add entry to elcache. Error=%s.", oh_lookup_error(err));
		
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
SaErrorT snmp_bc_parse_sel_entry(struct oh_handler_state *handle, char *logstr, sel_entry *sel)
{
        sel_entry ent;
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
	SaErrorT err;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        struct snmp_bc_hnd *custom_handle = handle->data;
			
	err = oh_el_clear(handle->elcache);
	if (err) {
		dbg("Cannot clear system Event Log. Error=%s.", oh_lookup_error(err));
		return(err);
	}

	set_value.type = ASN_INTEGER;
	set_value.str_len = 1;
	set_value.integer = (long) clearEventLogExecute;
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		err = snmp_bc_snmp_set(custom_handle, SNMP_BC_SEL_CLEAR_OID_RSA, set_value);
	}
	else {
		err = snmp_bc_snmp_set(custom_handle, SNMP_BC_SEL_CLEAR_OID, set_value);
	}

	if (err) {
		dbg("SNMP set failed. Error=%s.", oh_lookup_error(err));
		return(err);
	}
		
	return(SA_OK);
}


/**
 * snmp_bc_sel_overflowreset:
 * @hnd: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * 
 * Clear the overflow flag by itself is not supported by snmp_bc plugin.
 * Rather, it can be cleared implicitly via clear_sel command, saHpiEventLogClear()
 *
 * Return values:
 * SA_ERR_HPI_INVALID_CMD - normal case.
 * 
 **/			      
SaErrorT snmp_bc_sel_overflowreset(void *hnd,
			           SaHpiResourceIdT id)
{
        return(SA_ERR_HPI_INVALID_CMD);
}

			       
			    

