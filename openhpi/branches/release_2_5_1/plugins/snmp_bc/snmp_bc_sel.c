/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
 * file and program are licensed under a BSD style license. See
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
#include <sim_init.h>

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
        int i;
	SaErrorT err;
	
	i = 1;
	err = SA_OK;
	
	/* Go synchronize cache and hardware copy of the SEL */
        err = snmp_bc_check_selcache(handle, id, SAHPI_NEWEST_ENTRY);
	if (err)
		/* --------------------------------------------------------------- */
		/* If an error is encounterred during building of snmp_bc elcache, */
		/* only log the error.  Do not do any recovery because log entries */
		/* are still kept in bc mm.  We'll pick them up during synch.      */
		/* --------------------------------------------------------------- */
		dbg("snmp_bc_discover, Error %s when building elcache.\n", oh_lookup_error(err));

	/* Return the entry count */
        i = g_list_length(handle->elcache->elentries);
        return i;
}

/**
 * snmp_bc_get_sel_size_from_hardware:
 * @ss: Pointer to SNMP session data.
 * 
 * Unfortunately, hardware SNMP support does not provide access to the number 
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
 /* ------------------------------------------------------------------------------------- */
 /* This routine is no longer needed. Keep it here (with #if 0) for documentation purpose */
#if 0
static int snmp_bc_get_sel_size_from_hardware(struct snmp_bc_hnd *custom_handle)
{
        struct snmp_value run_value;
        char oid[SNMP_BC_MAX_OID_LENGTH];
        int i;

	i = 1;
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

#endif
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
        struct oh_handler_state *handle;
        struct tm curtime;
        sel_entry sel_entry;
    	struct snmp_bc_hnd *custom_handle;
        SaHpiEventLogInfoT sel;
	
        if (!hnd || !info) {
                dbg("Invalid parameter.");
                return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
        snmp_bc_lock_handler(custom_handle);
        
        /* Build local copy of EventLogInfo  */
     /* Max number of entries that can be stored  */
     /* in bc EventLog varies, depending on       */
     /* what events have been logged so far and   */
     /* the information each logged event contains*/    	
        sel.Size = BC_EL_MAX_SIZE; /* This is clearly a guess but looks about right 
                                    * from the 75% full errors we have seen.    */
	sel.UserEventMaxSize = SAHPI_MAX_TEXT_BUFFER_LENGTH;
        sel.Enabled = SAHPI_TRUE;
        sel.OverflowFlag = SAHPI_FALSE; 
	sel.OverflowResetable = SAHPI_FALSE;
        sel.OverflowAction = SAHPI_EL_OVERFLOW_OVERWRITE;
        sel.UpdateTimestamp = 0;

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
                                snmp_bc_unlock_handler(custom_handle);

				return(err);
                	} else {
                        	sel.UpdateTimestamp = (SaHpiTimeT) mktime(&sel_entry.time) * 1000000000;
                	}
        	}
        } else {
                snmp_bc_unlock_handler(custom_handle);
                return(err);
        }
	
	
	err = snmp_bc_get_sp_time(handle, &curtime); 
        if ( err == SA_OK) {
                sel.CurrentTime = (SaHpiTimeT) mktime(&curtime) * 1000000000;
        } else {
                snmp_bc_unlock_handler(custom_handle);
                return(err);
        }


        sel.Entries = snmp_bc_get_sel_size(handle, id);
	sel.OverflowFlag = handle->elcache->overflow;
        *info = sel;
        snmp_bc_unlock_handler(custom_handle);
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
	SaErrorT err;
	oh_el_entry tmpentry, *tmpentryptr;
        struct oh_handler_state *handle;
	struct snmp_bc_hnd *custom_handle;
 
        if (!hnd || !prev || !next || !entry) {
                dbg("Invalid parameter.");
                return(SA_ERR_HPI_INVALID_PARAMS);
        }
	       
	err = SA_OK;
	tmpentryptr = &tmpentry; 
	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
        snmp_bc_lock_handler(custom_handle);
        
        if (handle->elcache != NULL) {
                /* Force a cache sync before servicing the request */
                err = snmp_bc_check_selcache(handle, id, current);
                if (err) {
                        dbg("Event Log cache sync failed %s\n", oh_lookup_error(err));
                        /* --------------------------------------------------------------- */
                        /* If an error is encounterred during building of snmp_bc elcache, */
                        /* only log the error.  Do not do any recovery because log entries */
                        /* are still kept in bc mm.  We'll pick them up during synch.      */
                        /* --------------------------------------------------------------- */
                }
                
                err = oh_el_get(handle->elcache, current, prev, next, &tmpentryptr);
                if (err) {
                        dbg("Getting Event Log entry=%d from cache failed. Error=%s.", 
                            current, oh_lookup_error(err));
                        snmp_bc_unlock_handler(custom_handle);
                        
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
                snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
        snmp_bc_unlock_handler(custom_handle);
		
        return(SA_OK);
}


/**
 * snmp_bc_bulk_selcache
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log. 
 * 
 * Builds internal event log cache using SNMP_MSG_GETBULK.
 * 
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle is NULL.
 **/

SaErrorT snmp_bc_bulk_selcache(	struct oh_handler_state *handle,
				SaHpiResourceIdT id)
{
        struct snmp_bc_hnd *custom_handle;
	SaErrorT 	err;
	int 		isdst;
	sel_entry 	sel_entry;
        SaHpiEventT 	tmpevent;
    	netsnmp_pdu	*pdu, *response;
    	netsnmp_variable_list *vars;
	LogSource2ResourceT logsrc2res;
	int             count;
    	int             running;
    	int             status;
	char 		logstring[MAX_ASN_STR_LEN];
    	char 		objoid[SNMP_BC_MAX_OID_LENGTH];	
    	oid             name[MAX_OID_LEN];
	oid             root[MAX_OID_LEN];
    	size_t          rootlen;
    	size_t          name_length;	
	size_t 		str_len;
    	int             reps;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	str_len = MAX_ASN_STR_LEN;
	isdst=0;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	reps = custom_handle->count_per_getbulk;
	
	/* --------------------------------------------------- */
     	/* Set initial Event Log Entry OID and root tree       */
     	/* --------------------------------------------------- */

	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		snprintf(objoid, SNMP_BC_MAX_OID_LENGTH, "%s", SNMP_BC_SEL_ENTRY_OID_RSA);
	} else {
		snprintf(objoid, SNMP_BC_MAX_OID_LENGTH, "%s",SNMP_BC_SEL_ENTRY_OID);
	}
	rootlen = MAX_OID_LEN;
        read_objid(objoid, root, &rootlen);

	/* --------------------------------------------------- */
     	/* Object ID for GETBULK request                       */ 
	/* --------------------------------------------------- */
    	g_memmove(name, root, rootlen * sizeof(oid));
    	name_length = rootlen;

    	running = 1;

    	while (running) {
    
       		/* --------------------------------------------------- */
         	/* Create PDU for GETBULK request                      */ 
         	/* --------------------------------------------------- */
        	pdu = snmp_pdu_create(SNMP_MSG_GETBULK);

		status = snmp_getn_bulk(custom_handle->sessp,
					 name,
					 name_length,
					 pdu, 
					 &response,
					 reps);
					 	
        	if (status == STAT_SUCCESS) {
            		if (response->errstat == SNMP_ERR_NOERROR) {
	                	for (vars = response->variables; vars;
                     			vars = vars->next_variable) {
	
				 	/* ------------------------------------------------- */
				 	/* Check if this variable is of the same OID tree    */
				 	/* ------------------------------------------------- */
                    			if ((vars->name_length < rootlen)
                        			|| (memcmp(root, vars->name, rootlen * sizeof(oid)) != 0))
                            		{
						/* Exit vars processing */
                        			running = 0;
                        			continue;
                    			}  			
		  
                    			if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                        			(vars->type != SNMP_NOSUCHOBJECT) &&
                        			(vars->type != SNMP_NOSUCHINSTANCE)) {
                        
                        			if (snmp_oid_compare(name, name_length,
                                                			vars->name,
									vars->name_length) >= 0) {
                            				fprintf(stderr, "Error: OID not increasing: ");
                            				fprint_objid(stderr, name, name_length);
                            				fprintf(stderr, " >= ");
                            				fprint_objid(stderr, vars->name,
                                        					 vars->name_length);
                            				fprintf(stderr, "\n");
                            				running = 0;
                        			}
			                        /* ---------------------------------- */
                         			/* Check if last variable,            */
						/* and if so, save for next request.  */ 
                         			/* ---------------------------------- */
                        			if (vars->next_variable == NULL) {
                            				g_memmove(name, vars->name,
                                    			vars->name_length * sizeof(oid));
                            				name_length = vars->name_length;
                        			}
						
						/* ---------------------------------- */
				 		/* ---------------------------------- */ 
				 		/* ---------------------------------- */
						if ((running == 1) && (vars->type == ASN_OCTET_STR)) {
						
                        				if (vars->val_len < MAX_ASN_STR_LEN) str_len = vars->val_len;
							else str_len = MAX_ASN_STR_LEN;
						
							/* ---------------------------------- */
				 			/* Guarantee NULL terminated string   */
				 			/* ---------------------------------- */
                        				// memcpy(logstring, vars->val.string, str_len);
							g_memmove(logstring, vars->val.string, str_len);
							logstring[str_len] = '\0';
							
							
							err = snmp_bc_parse_sel_entry(handle,logstring, &sel_entry);
							isdst = sel_entry.time.tm_isdst;
							snmp_bc_log2event(handle, logstring, &tmpevent, isdst, &logsrc2res);
							err = oh_el_prepend(handle->elcache, &tmpevent, NULL, NULL);
							if (custom_handle->first_discovery_done == SAHPI_TRUE)
		             					err = snmp_bc_add_to_eventq(handle, &tmpevent, SAHPI_TRUE);							
						}
                    			} else {
                        			/* Stop on an exception value */
                        			running = 0;
                    			}
                		}  /* end for */
              		} else {   /* if (response->errstat != SNMP_ERR_NOERROR) */
			
                		/* --------------------------------------------- */
				/* Error condition is seen in response,          */
                 		/* for now, print the error then exit            */ 
                 		/* Not sure what to do for recovery              */
                		running = 0;
                		if (response->errstat == SNMP_ERR_NOSUCHNAME) {
                    				printf("End of MIB\n");
                		} else {
                    			fprintf(stderr, "Error in packet.\nReason: %s\n",
                            				snmp_errstring(response->errstat));
                    		if (response->errindex != 0) {
                        		fprintf(stderr, "Failed object: ");
                        		for (count = 1, vars = response->variables;
                             				vars && count != response->errindex;
                             				vars = vars->next_variable, count++)
                        			if (vars)
                            				fprint_objid(stderr, vars->name,
                                         				vars->name_length);
                        			fprintf(stderr, "\n");
                    			}
                		}
            		}  
		} else if (status == STAT_TIMEOUT) {            
            		fprintf(stderr, "Timeout: No Response\n");
            		running = 0;
        	} else {                /* status == STAT_ERROR */
            		snmp_sess_perror("snmp_bulk_sel",custom_handle->sessp );
            		running = 0;
        	}

        	if (response)
            		snmp_free_pdu(response);
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
	
	err = SA_OK;

	if ((g_list_length(handle->elcache->elentries) == 0) && 
	     !(is_simulator())) {
		/* err = snmp_bc_build_selcache(handle, id); */
		trace("elcache sync called before discovery?\n");
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
 * Synchronizes internal event log cache. Although BladeCenter can read events from 
 * the hardware in any order;  RSA requires that they be read from first to last.
 * To support common code, events are read in order and preappended to the
 * cache log.
 * 
 * Return values:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @handle is NULL.
 **/
SaErrorT snmp_bc_selcache_sync(struct oh_handler_state *handle,
			       SaHpiResourceIdT id,
			       SaHpiEventLogEntryIdT entryId)
{
	SaHpiEventLogEntryIdT prev;
	SaHpiEventLogEntryIdT next;
        struct snmp_value get_value, *this_value;
        sel_entry sel_entry;
        oh_el_entry *fetchentry, tmpentry;
        SaHpiTimeT new_timestamp;
	char oid[SNMP_BC_MAX_OID_LENGTH];
	SaErrorT err;
	int current, cacheupdate;
        struct snmp_bc_hnd *custom_handle;
	int isdst;
        SaHpiEventT tmpevent;
	LogSource2ResourceT logsrc2res;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
			
	fetchentry = &tmpentry; 
	cacheupdate = 0;
	custom_handle = (struct snmp_bc_hnd *)handle->data;

	err = oh_el_get(handle->elcache, SAHPI_NEWEST_ENTRY, &prev, &next, &fetchentry);
	if (err) fetchentry = NULL;

	if (fetchentry == NULL) {
		err = snmp_bc_build_selcache(handle, id);
		return(err);
	}
				
	current = 1;
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID_RSA, current);
	}
	else {
		snprintf(oid, SNMP_BC_MAX_OID_LENGTH, "%s.%d", SNMP_BC_SEL_ENTRY_OID, current);
	}

       	err = snmp_bc_snmp_get(custom_handle, oid, &get_value, SAHPI_TRUE);
       	if (err) {
		dbg("Error %s snmp_get latest BC Event Log.\n", oh_lookup_error(err));
		
		/* Error attempting to sync elcache and BC Event Log */
		/* Leave thing as is, instead of clearing elcache.   */
		/* Clearing cache will cause snmp traffic to re-read */
		/* all BC Event Log.  We want to keep traffic to min */
		/* for snmp interface recovery                       */
		/*    err = oh_el_clear(handle->elcache);            */
		return(err);
	}

	if (snmp_bc_parse_sel_entry(handle, get_value.string, &sel_entry) < 0) {
		dbg("Cannot parse Event Log entry");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	new_timestamp = (SaHpiTimeT)mktime(&sel_entry.time) * 1000000000;
	if (fetchentry->event.Event.Timestamp != new_timestamp) {
		GList *sync_log = NULL;
		this_value = g_memdup(&get_value, sizeof(get_value));
		if (this_value != NULL) 
			sync_log = g_list_prepend(sync_log, this_value);
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
				} else {
					this_value = g_memdup(&get_value, sizeof(struct snmp_value));
					if (this_value != NULL) 
						sync_log = g_list_prepend(sync_log, this_value);
				}
			} else {
				dbg("End of BladeCenter log reached.");
				break;
			}
		}
		
		
		if (cacheupdate) {
			GList *proc_log = NULL;
			proc_log = g_list_first(sync_log);

			while(proc_log != NULL) {
				this_value = (struct snmp_value *)proc_log->data;
				err = snmp_bc_parse_sel_entry(handle,this_value->string, &sel_entry);
				if (err != SA_OK) return(err);
		
				if (strncmp(get_value.string, EVT_EN_LOG_FULL, sizeof(EVT_EN_LOG_FULL)) == 0 )
		 				handle->elcache->overflow = SAHPI_TRUE;
		 
				isdst = sel_entry.time.tm_isdst;
				snmp_bc_log2event(handle, this_value->string, &tmpevent, isdst, &logsrc2res);

				if ((tmpevent.EventType == SAHPI_ET_HOTSWAP) && 
							(custom_handle->first_discovery_done == SAHPI_TRUE))
				{
					err = snmp_bc_rediscover(handle, &tmpevent, &logsrc2res);
				}

				/*  append to end-of-elcache and end-of-eventq   */
				err = snmp_bc_add_entry_to_elcache(handle, &tmpevent, SAHPI_FALSE);
				proc_log = g_list_next(proc_log);
			
			}
			
		} else {
			err = oh_el_clear(handle->elcache);
			if (err != SA_OK)
				dbg("Invalid elcache pointer or mode, err %s\n", oh_lookup_error(err));
			err =snmp_bc_build_selcache(handle, id);
			if ( (err == SA_ERR_HPI_OUT_OF_SPACE) || (err == SA_ERR_HPI_INVALID_PARAMS)) {
				/* either of these 2 errors prevent us from doing anything meaningful */
				/* tell user about them                                               */
				return(err);
			}
		}
		g_list_free(sync_log);
	} else {
		trace("EL Sync: there are no new entry indicated.\n");
	}
	
	return(SA_OK);  
}

/**
 * snmp_bc_build_selcache
 * @handle: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log. 
 * 
 * Builds internal event log cache. Although BladeCenter can read events from 
 * the hardware in any order;  RSA requires that they be read from first to last.
 * To support common code, events are read in order and preappended to the
 * cache log.
 * 
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle is NULL.
 **/
SaErrorT snmp_bc_build_selcache(struct oh_handler_state *handle, SaHpiResourceIdT id)
{	
	int i;
	SaErrorT err;
	struct snmp_bc_hnd *custom_handle;
	
	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	err = SA_OK;
	custom_handle = (struct snmp_bc_hnd *)handle->data;

#if 0
	/* ------------------------------------------------------------------ */
	/*  This code block is disabled because we no longer reading EventLog */
	/*  backward for BladeCenter and forward for RSA.  Rather, we read    */
	/*  EventLog forward and re-order them by timestamps accordingly.     */
	/*  Just leave code it here for documentation purposes.               */
	/* ------------------------------------------------------------------ */  
	int current;
	current = snmp_bc_get_sel_size_from_hardware(custom_handle);

	if (current > 0) {
		for (i=1; i<=current; i++) {
			err = snmp_bc_sel_read_add(handle, id, i, SAHPI_TRUE);
			if ( (err == SA_ERR_HPI_OUT_OF_SPACE) || (err == SA_ERR_HPI_INVALID_PARAMS)) {
				/* Either of these 2 errors prevent us from doing anything meaningful */
				return(err);
			} else if (err != SA_OK) {
				/* other errors (mainly HPI_INTERNAL_ERROR or HPI_BUSY) means */
				/* only this record has problem. record error then go to next */
				dbg("Error, %s, encountered with EventLog entry %d\n", 
				    oh_lookup_error(err), i);
			}
		}
	}
#endif
	/*  End-of-obsolete code                                              */
	/* ------------------------------------------------------------------ */

	/* ------------------------------------------------------------------- */
	/* If user has configured for snmpV3, then use GETBULK for performance */
	/* Else, stay with individual GETs                                     */
	/* ------------------------------------------------------------------- */   
	if (custom_handle->session.version == SNMP_VERSION_3)
	{
	
		/* ------------------------------------------------- */
		/*      DO NOT remove this trace statement!!!!       */
		/* ------------------------------------------------- */		
		/* Without this trace statement,                     */
		/* -- stack corruption issue with multiple handlers  */
		/*	gcc (GCC) 4.0.2 20051125 (Red Hat 4.0.2-8)   */
		/* -- works OK  with with multiple handlers          */
		/*	gcc (GCC) 4.1.0 20060123 (SLES10 Beta 2)     */
		/* -- works OK with with multiple handlers           */
		/*	gcc (GCC) 4.0.1 (4.0.1-5mdk  Mandriva 2006)  */
		/* -- works OK with all gcc vers for single handler  */
		trace(">>>>>> bulk build selcache %p.\n", handle);
		/* ------------------------------------------------- */
				
		err = snmp_bc_bulk_selcache(handle, id);
	} else {
		i = 1;
		while(1) {
			err = snmp_bc_sel_read_add(handle, id, i, SAHPI_TRUE);
			if ( (err == SA_ERR_HPI_OUT_OF_SPACE) || (err == SA_ERR_HPI_INVALID_PARAMS)) {
				/* Either of these 2 errors prevent us from doing anything meaningful */
				return(err);
			} else if (err != SA_OK) {
				/* other errors (mainly HPI_INTERNAL_ERROR or HPI_BUSY) means */
				/* only this record has problem. record error then go to next */
				dbg("Error, %s, encountered with EventLog entry %d\n", 
				oh_lookup_error(err), i);
				break;
			}	
			i++;
		}
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
        struct tm tv;
        time_t tt;
        SaErrorT err;
	struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);    
	}

	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	snmp_bc_lock_handler(custom_handle);
        tt = time / 1000000000;
        localtime_r(&tt, &tv);
	
	err = snmp_bc_set_sp_time(custom_handle, &tv);
        if (err) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("Cannot set time. Error=%s.", oh_lookup_error(err));
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	snmp_bc_unlock_handler(custom_handle);
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
			       SaHpiEventLogEntryIdT current, 
			       SaHpiBoolT prepend)
{	
	int isdst;
        char oid[SNMP_BC_MAX_OID_LENGTH];
	sel_entry sel_entry;
	SaErrorT err;
        SaHpiEventT tmpevent;
	LogSource2ResourceT logsrc2res;
        struct snmp_value get_value;
	struct snmp_bc_hnd *custom_handle;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	isdst=0;

        custom_handle = (struct snmp_bc_hnd *)handle->data;

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
		
	if (strncmp(get_value.string, EVT_EN_LOG_FULL, sizeof(EVT_EN_LOG_FULL)) == 0 )
		 handle->elcache->overflow = SAHPI_TRUE;
		 
	isdst = sel_entry.time.tm_isdst;
	snmp_bc_log2event(handle, get_value.string, &tmpevent, isdst, &logsrc2res);
	err = snmp_bc_add_entry_to_elcache(handle, &tmpevent, prepend);

        return(err);
}


/**
 * snmp_bc_add_entry_to_elcache
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
SaErrorT snmp_bc_add_entry_to_elcache(struct oh_handler_state *handle,
        				SaHpiEventT *tmpevent,
			       		SaHpiBoolT prepend)
{

	SaHpiEntryIdT rdrid;
	SaHpiRdrT rdr, *rdr_ptr; 
	struct snmp_bc_hnd *custom_handle;
	SaHpiResourceIdT id;
	SaErrorT err;


	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	rdrid=0;
	rdr_ptr=NULL;			
        custom_handle = (struct snmp_bc_hnd *)handle->data;
			
	/* See feature 1077241 */
	switch (tmpevent->EventType) {
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
					    tmpevent->EventDataUnion.SensorEvent.SensorNum); 
			rdr_ptr = oh_get_rdr_by_id(handle->rptcache, tmpevent->Source, rdrid);
			break;
		case SAHPI_ET_WATCHDOG:
			rdrid = get_rdr_uid(SAHPI_WATCHDOG_RDR,
					    tmpevent->EventDataUnion.WatchdogEvent.WatchdogNum);
			rdr_ptr = oh_get_rdr_by_id(handle->rptcache, tmpevent->Source, rdrid);
			break;
		default:
			dbg("Unrecognized Event Type=%d.", tmpevent->EventType);
			return(SA_ERR_HPI_INTERNAL_ERROR);
			break;
	} 
	
	
	/* Since oh_el_append() does a copy of RES and RDR into it own data struct, */ 
	/* just pass the pointers to it.                                            */
	id = tmpevent->Source;
	if (NULL == oh_get_resource_by_id(handle->rptcache, id)) {
		trace("Warning: NULL RPT for rid %d.", id);
	}
	
	if (prepend) 
		err = oh_el_prepend(handle->elcache, tmpevent,
			    rdr_ptr, oh_get_resource_by_id(handle->rptcache, id));
	else 
		err = oh_el_append(handle->elcache, tmpevent,
			    rdr_ptr, oh_get_resource_by_id(handle->rptcache, id));
	
	
	/* If can not add el entry to elcache, do not add entry to eventq */
	/* If entry is not added to elcache and is added to eventq, there */
	/* will be an indefinite loop:                                    */
	/*        1. get_event is called. get_event call elcache_sync     */
	/*        2. elcache_sync finds new el entry                      */
	/*        3a. adding new el entry to elcache fails                */
	/*	  3b. adding new el entry to eventq                       */
	/*	  4. new event is propergate to infrastructure            */
	/*	  5. infrastructure consume new event                     */
	/*	  		*and*  call get_event for any more        */
	/*	  6. we repeat step 1 ... indefinite loop                 */   
	
	if (!err) {
		if (custom_handle->first_discovery_done == SAHPI_TRUE)
		             err = snmp_bc_add_to_eventq(handle, tmpevent, prepend);
		if (err) 
		 dbg("Cannot add el entry to eventq. Error=%s.", oh_lookup_error(err));
	} else { 
	 	dbg("Cannot add el entry to elcache. Error=%s.", oh_lookup_error(err));
	}
			
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
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !logstr || !sel) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) return(SA_ERR_HPI_INVALID_PARAMS);
	
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

	/* No Name field in RSA event messages */
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		strncpy(ent.sname, "RSA", sizeof("RSA"));
	}
	else {
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
        struct oh_handler_state *handle;
	SaErrorT err;
        struct snmp_bc_hnd *custom_handle;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
				
	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	snmp_bc_lock_handler(custom_handle);
	err = oh_el_clear(handle->elcache);
	if (err) {
		snmp_bc_unlock_handler(custom_handle);
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
		snmp_bc_unlock_handler(custom_handle);
		dbg("SNMP set failed. Error=%s.", oh_lookup_error(err));
		return(err);
	} else if (!is_simulator()) { 
		/* Pick up the newly created entry, Log Clear message */
		err = snmp_bc_build_selcache(handle, 1);
	}
		
	snmp_bc_unlock_handler(custom_handle);
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
 **/			      
SaErrorT snmp_bc_sel_overflowreset(void *hnd,
			           SaHpiResourceIdT id)
{
        return(SA_ERR_HPI_INVALID_CMD);
}


/**
 * snmp_bc_sel_state_set:
 * @hnd: Pointer to handler's data.
 * @id: Resource ID that owns the Event Log.
 * @enable: State to which to set Resource Event Log.
 * 
 * Enable/Disable logging of event is not allowed via SMMP for BladeCenter.
 *
 * Return values:
 * SA_ERR_HPI_READ_ONLY - normal case.
 **/			      
SaErrorT snmp_bc_sel_state_set(void      *hnd, 
                SaHpiResourceIdT   id, 
                SaHpiBoolT         enable)
{
	/* Other required test, SA_ERR_HPI_CAPABILITY, is done in infrastructure */ 
	return SA_ERR_HPI_READ_ONLY;
}

void * oh_get_el_info (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *)
                __attribute__ ((weak, alias("snmp_bc_get_sel_info")));

void * oh_set_el_time (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("snmp_bc_set_sel_time")));

void * oh_add_el_entry (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("snmp_bc_add_sel_entry")));

void * oh_get_el_entry (void *, SaHpiResourceIdT, SaHpiEventLogEntryIdT,
                       SaHpiEventLogEntryIdT *, SaHpiEventLogEntryIdT *,
                       SaHpiEventLogEntryT *, SaHpiRdrT *, SaHpiRptEntryT  *)
                __attribute__ ((weak, alias("snmp_bc_get_sel_entry")));

void * oh_clear_el (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("snmp_bc_clear_sel")));

void * oh_reset_el_overflow (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("snmp_bc_sel_overflowreset")));

void * oh_set_el_state(void *, SaHpiResourceIdT, SaHpiBoolT)
		__attribute__ ((weak, alias("snmp_bc_sel_state_set")));
