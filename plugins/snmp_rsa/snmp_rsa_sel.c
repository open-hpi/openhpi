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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include <glib.h>
#include <time.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <snmp_util.h>
#include <rsa_resources.h>
#include <snmp_rsa.h>
#include <snmp_rsa_sel.h>
#include <snmp_rsa_event.h>


/**
 * get_rsa_sel_size_from_hardware:
 * @ss: 
 * 
 * 
 * Return value:  Number of event log entries read from RSA 
 **/
static int get_rsa_sel_size_from_hardware(struct snmp_session *ss)
{
        struct snmp_value run_value;
        char oid[50];
        int i = 1;

        /*
          Since RSA snmp agent does not provide this count,
          this is the only way to figure out how entries there are
	  in RSA SEL.
        */
        do {
                sprintf(oid, "%s.%d", RSA_SEL_INDEX_OID, i);
                i++;
        } while(snmp_get(ss,oid,&run_value) == 0);
        
        /* think about it, and it makes sense */
        i -= 2;
        return i;
}

/**
 * snmp_rsa_get_sel_entry:
 * @hnd: 
 * @id: 
 * @info: 
 * 
 * Return value: 0 on success, < 0 on error
 **/
int snmp_rsa_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiSelInfoT *info) 
{
        struct oh_handler_state *handle = hnd;

	/* Go synchronize cache and hardware copy of the SEL */
        snmp_rsa_check_selcache(hnd, id, SAHPI_NEWEST_ENTRY);

        oh_sel_info(handle->selcache, info);
        
        return 0;
}

/**
 * snmp_rsa_get_sel_entry:
 * @hnd: 
 * @id: 
 * @current: 
 * @prev: 
 * @next: 
 * @entry: 
 * 
 * See saHpiEventLogEntryGet for params
 * 
 * Return value: 0 on success, < 0 on error
 **/
int snmp_rsa_get_sel_entry(void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT current,
                          SaHpiSelEntryIdT *prev, SaHpiSelEntryIdT *next,
                          SaHpiSelEntryT *entry)
{
        SaHpiSelEntryT tmpentry, *tmpentryptr;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	tmpentryptr = &tmpentry; 
	SaErrorT rc;

	rc = snmp_rsa_check_selcache(hnd, id, current);
	if (handle->selcache != NULL) {
		rc = oh_sel_get(handle->selcache, current, prev, next, &tmpentryptr);
		if (rc != SA_OK) {
			printf("entryId %d, oh_sel_get returncode %d\n", current, rc);
			dbg("Error fetching entry number %d from cache    >>>\n", current);
		} else {
			memcpy(entry, tmpentryptr, sizeof(SaHpiSelEntryT));
		}
	} else {
		dbg("Missing handle->selcache");
	}
		
        return rc;
}

/**
 * snmp_rsa_build_selcache
 * @hnd: 
 * @id: 
 * 
 * 
 * Return value: 0 on success, < 0 on error
 **/
SaErrorT snmp_rsa_build_selcache(void *hnd, SaHpiResourceIdT id)
{
	int current;
	SaErrorT rv;
	struct oh_handler_state *handle = hnd;
	struct snmp_rsa_hnd *custom_handle = handle->data;
	
	current = get_rsa_sel_size_from_hardware(custom_handle->ss);
	if (current != 0) {
		do {
			rv = snmp_rsa_sel_read_add (hnd, id, current); 
			current--;
		} while(current > 0);
	}
	return SA_OK;
}

/**
 * snmp_rsa_check_selcache
 * @hnd: 
 * @id: 
 * @entryId: 
 * 
 * 
 * Return value: 0 on success, < 0 on error
 **/
int snmp_rsa_check_selcache(void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT entryId)
{
        struct oh_handler_state *handle = hnd;
	SaErrorT rv;

	if (g_list_length(handle->selcache->selentries) == 0) {
		rv = snmp_rsa_build_selcache(hnd, id);
	} else {
		rv = snmp_rsa_selcache_sync(hnd, id, entryId);
	}
	return rv;
}

/**
 * snmp_rsa_selcache_sync
 * @hnd: 
 * @id: 
 * @entryId: 
 * 
 * 
 * Return value: 0 on success, < 0 on error
 **/
int snmp_rsa_selcache_sync(void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT entryId)
{
	SaHpiSelEntryIdT current;
	SaHpiSelEntryIdT prev;
	SaHpiSelEntryIdT next;
        struct snmp_value get_value;
        struct oh_handler_state *handle = hnd;
        struct snmp_rsa_hnd *custom_handle = handle->data;
        rsa_sel_entry sel_entry;
        SaHpiSelEntryT  *fetchentry;
        SaHpiTimeT new_timestamp;
	char oid[50];
	SaErrorT rv;
	int rc, cacheupdate = 0;

	rv = oh_sel_get(handle->selcache, SAHPI_NEWEST_ENTRY, &prev, &next, &fetchentry);
	if (rv != SA_OK)
		fetchentry = NULL;
		
	current = 1;
	sprintf(oid, "%s.%d", RSA_SEL_ENTRY_OID, current);
       	rc = snmp_get(custom_handle->ss,oid,&get_value);
       	if (rc != 0) 
	{
		/* 
		 * snmp_get() returns -1 if snmp agent does not respond *or*
		 * snmp log entry does not exist. 
		 *
		 * For now, assuming the best i.e. snmp log is empty. 
		 */
		dbg("snmp log is empty.\n");
		rv = oh_sel_clear(handle->selcache);
	
	} else {
		if (fetchentry == NULL) {
			rv = snmp_rsa_build_selcache(hnd, id);
			return SA_OK;
		}
		
		if(snmp_rsa_parse_sel_entry(custom_handle->ss,get_value.string, &sel_entry) < 0) {
			dbg("Couldn't parse SEL Entry");
       			return -1;
		}

		new_timestamp = (SaHpiTimeT) mktime(&sel_entry.time) * 1000000000;
		if (fetchentry->Event.Timestamp != new_timestamp)
		{
			while (1) {
				current++;
               			sprintf(oid, "%s.%d", RSA_SEL_ENTRY_OID, current);
               			rv = snmp_get(custom_handle->ss,oid,&get_value);
				if (rv == 0) {
               				if(snmp_rsa_parse_sel_entry(custom_handle->ss,get_value.string, &sel_entry) < 0) {
               					dbg("Couldn't parse SEL Entry");
                       				return -1;
					}

					if ((fetchentry->Event.Timestamp == 
						 (SaHpiTimeT) mktime(&sel_entry.time) * 1000000000)) 
					{
						current--;
						cacheupdate = 1;	
						break;
					}
               			} else {
					dbg("Old entry not found and end of snmp log reached.\n");
					break;
				}
				
			}

			if (cacheupdate) {
	        		do {
					rv = snmp_rsa_sel_read_add (hnd, id, current); 
                       			current--;
               			} while(current > 0);
			} else {
				rv = oh_sel_clear(handle->selcache);
				snmp_rsa_build_selcache(hnd, id);				
			}
		} else {
			; /* dbg("There are no new entry indicated.\n"); */
		}
	}
	return SA_OK;  
}

/**
 * snmp_rsa_set_sel_time:
 * @hnd: 
 * @id: 
 * @time: 
 * 
 * 
 * 
 * Return value: 
 **/
int snmp_rsa_set_sel_time(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time)
{
        struct oh_handler_state *handle = hnd;

        return oh_sel_timeset(handle->selcache,time);
}

/**
 * snmp_rsa_add_sel_entry:
 * @hnd: 
 * @id: 
 * @Event: 
 * 
 * Add is not supported with tihs hardware, so -1 is always returned
 * 
 * Return value: -1
 **/
int snmp_rsa_add_sel_entry(void *hnd, SaHpiResourceIdT id, const SaHpiSelEntryT *Event)
{
        return SA_ERR_HPI_INVALID_CMD;
}

/**
 * snmp_rsa_del_sel_entry:
 * @hnd: 
 * @id: 
 * @sid: 
 * 
 * Delete is not supported with this hardware, so -1 is always returned
 * 
 * Return value: -1
 **/
int snmp_rsa_del_sel_entry(void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT sid)
{
        return SA_ERR_HPI_INVALID_CMD;
}


/**
 * snmp_rsa_sel_read_add:
 * @hnd: 
 * @id: 
 * @current: 
 * 
 * 
 * Return value: -1 if fails. 0 SA_OK
 **/
int snmp_rsa_sel_read_add (void *hnd, SaHpiResourceIdT id, SaHpiSelEntryIdT current)
{
        struct snmp_value get_value;
        struct oh_handler_state *handle = hnd;
        struct snmp_rsa_hnd *custom_handle = handle->data;
        SaHpiSelEntryT tmpentry;
        char oid[50];
        SaErrorT rv;
	int isdst = 0;

	sprintf(oid, "%s.%d", RSA_SEL_ENTRY_OID, current);
	snmp_get(custom_handle->ss,oid,&get_value);

	if(get_value.type == ASN_OCTET_STR) {
		int event_enabled;

                log2event(hnd, get_value.string, &tmpentry.Event, isdst, &event_enabled);
                tmpentry.EntryId = current;
                tmpentry.Timestamp = tmpentry.Event.Timestamp;
                rv = oh_sel_add(handle->selcache, &tmpentry);
		
		if (event_enabled) {
			rv = snmp_rsa_add_to_eventq(hnd, &tmpentry.Event);
		}
	} else {
		dbg("Couldn't fetch SEL Entry from RSA snmp");
		return -1;
	}

        return SA_OK;
}

/**
 * snmp_rsa_parse_sel_entry:
 * @text: text as returned by snmpget call for an event log entry
 * @sel: RSA system event log
 * 
 * This call is used to create a RSA sel entry from the returned
 * snmp string.  Another transform will have to happen to turn this into 
 * an SAHPI sel entry. 
 * 
 * Return value: 0 for success, -1 for format error, -2 for premature data termination
 **/
int snmp_rsa_parse_sel_entry(struct snmp_session *ss, char * text, rsa_sel_entry * sel) 
{
        rsa_sel_entry ent;
        char level[8];
        char * start = text;
        
        /* Severity first */
        if(sscanf(start,"Severity:%7s",level)) {
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
                dbg("Couldn't parse Severity from RSA Log Entry");
                return -1;
        }
                
        while(start && (strncmp(start,"Source:",7) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Source:%19s",ent.source)) {
                dbg("Couldn't parse Source from RSA Log Entry");
                return -1;
        }

        while(start && (strncmp(start,"Name:",5) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Name:%19s",ent.sname)) {
                dbg("Couldn't parse Name from RSA Log Entry");
                return -1;
        }
        
        while(start && (strncmp(start,"Date:",5) != 0)) { start++; }
        if(!start) { return -2; }
        
        if(sscanf(start,"Date:%2d/%2d/%2d  Time:%2d:%2d:%2d",
                  &ent.time.tm_mon, &ent.time.tm_mday, &ent.time.tm_year, 
                  &ent.time.tm_hour, &ent.time.tm_min, &ent.time.tm_sec)) {
// FIXME!
//		set_rsa_dst(ss, &ent.time);
                ent.time.tm_mon--;
                ent.time.tm_year += 100;
        } else {
                dbg("Couldn't parse Date/Time from RSA Log Entry");
                return -1;
        }
        
        while(start && (strncmp(start,"Text:",5) != 0)) { start++; }
        if(!start) { return -2; }
        
        /* advance to data */
        start += 5;
        strncpy(ent.text,start,RSA_SEL_ENTRY_STRING - 1);
        ent.text[RSA_SEL_ENTRY_STRING - 1] = '\0';
        
        *sel = ent;
        return 0;
}

/**
 * snmp_rsa_clear_sel:
 * @hnd: 
 * @id: 
 * @info: 
 * 
 * Return value: 0 on success, < 0 on error
 **/
SaErrorT snmp_rsa_clear_sel(void *hnd, SaHpiResourceIdT id) 
{
        char oid[50];
	int retcode;
	struct snmp_value set_value;
        struct oh_handler_state *handle = hnd;
        struct snmp_rsa_hnd *custom_handle = handle->data;
	SaErrorT rv;
		
	rv = oh_sel_clear(handle->selcache);

	sprintf(oid, "%s", RSA_CLEAR_SEL_OID);
	set_value.type = ASN_INTEGER;
	set_value.str_len = 1;
	set_value.integer = (long) clearEventLogExecute;
	retcode = snmp_set(custom_handle->ss, oid, set_value);
	
	if (retcode != 0)
		rv = SA_ERR_HPI_ERROR;
	else 
		rv = SA_OK;
		
	return rv;

}
/* end of snmp_rsa_sel.c */
