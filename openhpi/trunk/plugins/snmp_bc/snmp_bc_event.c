/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SaHpi.h>

#include <openhpi.h>
#include <oh_plugin.h>
#include <rpt_utils.h>
#include <uid_utils.h>
#include <epath_utils.h>
#include <bc_resources.h>
#include <bc_str2event.h>
#include <bc_errorlog.h>
#include <snmp_util.h>
#include <snmp_bc_sel.h>
#include <snmp_bc_utils.h>
#include <snmp_bc_event.h>

typedef enum {
	EVENT_NOT_MAPPED,
	EVENT_NOT_ALERTABLE,
} OEMReasonCodeT;

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);
static int  parse_sel_entry(char *text, bc_sel_entry *sel, int isdst);
static int  parse_threshold_str(gchar *str, gchar *root_str, gchar *read_value_str, gchar *trigger_value_str);
static int  set_previous_event_state(void *hnd, SaHpiEventT *event);
static int  map2oem(void *hnd, SaHpiEventT *event, bc_sel_entry *sel_entry, OEMReasonCodeT reason);

int event2hpi_hash_init()
{
	event2hpi_hash = g_hash_table_new(g_str_hash, g_str_equal);
	if (event2hpi_hash == NULL) {
		dbg("Cannot allocate event2hpi_hash table");
		return -1;
	}
	
	return 0;
}

int event2hpi_hash_free()
{
        g_hash_table_foreach(event2hpi_hash, free_hash_data, NULL);
	g_hash_table_destroy(event2hpi_hash);

	return 0;
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key); /* Memory was created for these during normalization process */
        g_free(value);
}


int find_res_events(SaHpiEntityPathT *ep, const struct BC_ResourceInfo *bc_res_info)
{
	int i;
	int max = MAX_RESOURCE_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	
	for (i=0; bc_res_info->event_array[i].event != NULL && i < max; i++) {

		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, bc_res_info->event_array[i].event);
		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    bc_res_info->event_array[i].event);
			return -1;
		}

		/*  Add to hash; Set HPI values */
		if (g_hash_table_lookup_extended(event2hpi_hash, normalized_str,
						 (gpointer)&hash_existing_key,
						 (gpointer)&hash_value)) {
			/* Set resource's event back to default state */
			((SaHpiEventT *)hash_value)->EventDataUnion.HotSwapEvent.HotSwapState =
				bc_res_info->def_state;
                        /* Recover space created in snmp_derive_objid */
			g_free(normalized_str);
		}		
		else {
			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Cannot allocate memory for HPI event");
				return -1;
			}

			/* Set values */
			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_HOTSWAP;
			hpievent->EventDataUnion.HotSwapEvent.HotSwapState = 
				bc_res_info->def_state;

			g_hash_table_insert(event2hpi_hash, normalized_str, hpievent);
		}
	}

	if (i >= max) { 
		dbg("Warning! Too many events defined for Res=%d\n", rid);
		dbg("Warning! Last event was %s\n", bc_res_info->event_array[max].event);
	}

	return 0;
}

int find_sensor_events(SaHpiEntityPathT *ep, SaHpiSensorNumT sid, const struct snmp_bc_sensor *rpt_sensor)
{
	int i;
	int max = MAX_SENSOR_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	
	for (i=0; rpt_sensor->bc_sensor_info.event_array[i].event != NULL && i < max; i++) {
	
		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, rpt_sensor->bc_sensor_info.event_array[i].event);
		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    rpt_sensor->bc_sensor_info.event_array[i].event);
			return -1;
		}

		/*  Add to hash; Set HPI values */
		if (g_hash_table_lookup_extended(event2hpi_hash, 
						 normalized_str,
						 (gpointer)&hash_existing_key, 
						 (gpointer)&hash_value)) {
			
                        /* Recover space created in snmp_derive_objid */
			g_free(normalized_str); 
		}		
		else {
			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Cannot allocate memory for HPI event");
				return -1;
			}

			/* Set values */
			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_SENSOR;
			hpievent->EventDataUnion.SensorEvent.SensorNum = sid;
			hpievent->EventDataUnion.SensorEvent.SensorType = rpt_sensor->sensor.Type;
			hpievent->EventDataUnion.SensorEvent.EventCategory = rpt_sensor->sensor.Category;
			hpievent->EventDataUnion.SensorEvent.Assertion = SAHPI_TRUE;

			/* Setup trigger values for threshold sensors */
			if ((rpt_sensor->sensor.ThresholdDefn.IsThreshold == SAHPI_TRUE) &&
			    (rpt_sensor->sensor.DataFormat.Range.Max.ValuesPresent & SAHPI_SRF_INTERPRETED)) {
				
				hpievent->EventDataUnion.SensorEvent.TriggerReading.ValuesPresent = 
					hpievent->EventDataUnion.SensorEvent.TriggerReading.ValuesPresent |
					SAHPI_SRF_INTERPRETED;
				hpievent->EventDataUnion.SensorEvent.TriggerReading.Interpreted.Type = 
					rpt_sensor->sensor.DataFormat.Range.Max.Interpreted.Type;

				hpievent->EventDataUnion.SensorEvent.TriggerThreshold.ValuesPresent =
					hpievent->EventDataUnion.SensorEvent.TriggerThreshold.ValuesPresent |
					SAHPI_SRF_INTERPRETED;
				hpievent->EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Type =
					rpt_sensor->sensor.DataFormat.Range.Max.Interpreted.Type;
			}

			g_hash_table_insert(event2hpi_hash, normalized_str, hpievent);
		}
	}

	if (i >= max) { 
		dbg("Warning! Too many events defined for Sensor=%d\n", sid);
		dbg("Warning! Last event was %s\n", 
		    rpt_sensor->bc_sensor_info.event_array[max].event);
		return -1;
	}

	return 0;
}

/****************************************************************************
 * log2event maps BC error log entries to HPI events. 
 * isdst ("is DayLight Savings Time") parameter is a performance hack.
 * Design assumes the event's timestamp is the time local to the BC box itself.
 * So instead of forcing BC SNMP accesses for each log entry to determine if
 * DST is in effect on the BC box, the isdst parameter allows the caller to 
 * query the BC for DST info once then make multiple translation calls.
 *****************************************************************************/

int log2event(void *hnd, gchar *logstr, SaHpiEventT *event, int isdst)
{
	bc_sel_entry log_entry;
	gchar *recovery_str, *login_str, search_str[BC_SEL_ENTRY_STRING];
	gchar thresh_read_value[MAX_THRESHOLD_VALUE_STRINGSIZE]; 
	gchar thresh_trigger_value[MAX_THRESHOLD_VALUE_STRINGSIZE];
	int is_recovery_event, is_threshold_event;
	SaHpiEventT working, *event_ptr;
	SaHpiSeverityT severity;
	Str2EventInfoT *strhash_data;

	memset(&working, 0, sizeof(SaHpiEventT));
	is_recovery_event = is_threshold_event = 0;

        /* Parse BC error log entry into its various components */
	if (parse_sel_entry(logstr, &log_entry, isdst)) {
		dbg("Couldn't parse BladeCenter error log entry=%s", logstr);
		return -1;
	}

	/* Fill-in HPI event time */
	working.Timestamp = (SaHpiTimeT)mktime(&log_entry.time) * 1000000000;

        /* Set default severity; usually overwritten below with BCT-level severity */
	severity = log_entry.sev; 

	/* Set event search string; this is sometimes overwritten below 
	 * since BC dynamically adds strings to canned event messages */
	strncpy(search_str, log_entry.text, BC_SEL_ENTRY_STRING);

	/************************************************
	 * Discover Recovery event strings
	 * Set Assertion to FALSE for sensor events below
         ************************************************/
	recovery_str = strstr(search_str, EVT_RECOVERY);
	if (recovery_str && (recovery_str == search_str)) {
		is_recovery_event = 1;
		strcpy(search_str, (log_entry.text + strlen(EVT_RECOVERY)));
	}

	/* Adjust login event strings - strip username */
	login_str = strstr(log_entry.text, LOG_LOGIN_STRING);
	if (login_str) {
		gchar *id_str = strstr(log_entry.text, LOG_LOGIN_CHAR);
		if (id_str != NULL) {
			memset(search_str, 0, BC_SEL_ENTRY_STRING);
			strncpy(search_str, log_entry.text, (id_str - log_entry.text));
			search_str[(id_str - log_entry.text)] = '\0';
		}
	}

	/* Adjust threshold event strings */
	if (strstr(log_entry.text, LOG_THRESHOLD_VALUE_STRING)) {
		is_threshold_event = 1;
		if (parse_threshold_str(log_entry.text, search_str, thresh_read_value, thresh_trigger_value)) {
			dbg("Cannot parse threshold event string=%s\n", log_entry.text);
			strncpy(search_str, log_entry.text, BC_SEL_ENTRY_STRING);	
		}
	}

	/* See if adjusted search string is a recognized BC "alertable" event */
	strhash_data = (Str2EventInfoT *)g_hash_table_lookup(str2event_hash, search_str);
	if (strhash_data) {
		/* Check to see if we can use BCT-level severity; OVR means use
		 *  default parse log severity; NOOVR means use BCT-level 
		 *  severity from str2event_hash table 
		 */
		if (strhash_data->event_sev_ovr == NOOVR) {
			severity = strhash_data->event_sev;
		}
		/* Look to see if event is mapped to an HPI entity */
		event_ptr = (SaHpiEventT *)g_hash_table_lookup(event2hpi_hash, strhash_data->event);
		if (event_ptr) {
			working = *event_ptr;

			/* Handle sensor events */
			if (working.EventType == SAHPI_ET_SENSOR) {
				if (is_recovery_event) {
					working.EventDataUnion.SensorEvent.Assertion = SAHPI_FALSE;
				}
				/* Convert threshold strings into event values */
				if (is_threshold_event) {
					if (get_interpreted_value(thresh_read_value,
					    working.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Type,
					    &working.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value)) {
						dbg("Cannot convert trigger reading=%s for text=%s\n",
						    thresh_read_value, log_entry.text);
						return -1;
					}
					if (get_interpreted_value(thresh_trigger_value,
   					    working.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Type,
					    &working.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value)) {
						dbg("Cannot convert trigger threshold=%s for text=%s\n",
						    thresh_trigger_value, log_entry.text);
						return -1;
					}		
				}
				/* Set current/last state */
				if (set_previous_event_state(hnd, &working)) {
					dbg("Cannot set previous state for sensor event = %s\n", log_entry.text);
					return -1;
				}
			}

                        /* Handle hot-swap events */
			else if (working.EventType == SAHPI_ET_HOTSWAP) {
				if (set_previous_event_state(hnd, &working)) {
					dbg("Cannot set previous state for hot-swap event = %s\n", log_entry.text);
					return -1;
				}
			}
			else {
				dbg("Design Error! BladeCenter doesn't support events of type=%d\n", working.EventType);
				return -1;
			}
		} /* End found mapped event */
		else { /* Map to OEM Event - Log Not Mapped */
			if (map2oem(hnd, &working, &log_entry, EVENT_NOT_MAPPED)) {
				dbg("Cannot map to OEM Event %s", log_entry.text);
				return -1;
			}
		} /* End found "alertable" event string in hash */
	}
	else { /* Map to OEM Event - String not recognized as BC "alertable" */
		if (map2oem(hnd, &working, &log_entry, EVENT_NOT_ALERTABLE)) {
			dbg("Cannot map to OEM Event %s", log_entry.text);
			return -1;
		}
	}

	working.Severity = severity;
	memcpy((void *)event, (void *)&working, sizeof(SaHpiEventT));
	
	return 0;
}

/*
 * This function parses BladeCenter threshold log messages into their various 
 * sub-strings. Format is root string (in str2event_hash) followed by read threshold 
 * value string  followed by trigger threshold value string. Unfortunately can't
 * convert directly to sensor values yet because don't yet know if event mapped or
 * if it is, what the sensor's threshold data type is.
 */
static int parse_threshold_str(gchar *str, gchar *root_str, gchar *read_value_str, gchar *trigger_value_str) 
{
	gchar  **event_substrs = NULL;
	gchar  **thresh_substrs = NULL;
	int rtn_code=0;

	if (str == NULL || root_str == NULL || read_value_str == NULL || trigger_value_str == NULL) {
		dbg("Cannot parse threshold string; Input string(s) NULL\n");
		return -1;
	}

	event_substrs = g_strsplit(str, LOG_READ_VALUE_STRING, -1);
	thresh_substrs = g_strsplit(event_substrs[1], LOG_THRESHOLD_VALUE_STRING, -1);

	if (thresh_substrs == NULL ||
	    (thresh_substrs[0] == NULL || thresh_substrs[0][0] == '\0') ||
	    (thresh_substrs[1] == NULL || thresh_substrs[1][0] == '\0') ||
	    (thresh_substrs[2] != NULL)) {
		dbg("Cannot split threshold event format %s properly", str);
		rtn_code = -1;
		goto CLEANUP;
	}

	if ((strlen(thresh_substrs[0]) > MAX_THRESHOLD_VALUE_STRINGSIZE) ||
	    (strlen(thresh_substrs[1]) > MAX_THRESHOLD_VALUE_STRINGSIZE)) {
		dbg("Threshold value string(s) exceed max size for string=%s\n", str);
		rtn_code = -1;
		goto CLEANUP;
	}
	
	strcpy(root_str, event_substrs[0]);
	strcpy(read_value_str, thresh_substrs[0]);
	strcpy(trigger_value_str, thresh_substrs[1]);

	if (root_str == NULL || read_value_str == NULL || trigger_value_str == NULL) {
		dbg("Cannot parse threshold str=%s into non-NULL parts\n", str);
		rtn_code = -1;
	}

 CLEANUP:
	g_strfreev(event_substrs);
	g_strfreev(thresh_substrs);

	return rtn_code;
}

/**********************************
 * Set previous state info in event
 **********************************/
static int set_previous_event_state(void *hnd, SaHpiEventT *event) 
{
	switch (event->EventType) {
	case SAHPI_ET_SENSOR:
	{
		SaHpiRdrT *rdr = oh_get_rdr_by_type(
			((struct oh_handler_state *)hnd)->rptcache, 
			event->Source, SAHPI_SENSOR_RDR, 
			event->EventDataUnion.SensorEvent.SensorNum);
		
		if (rdr == NULL) {
			return SA_ERR_HPI_NOT_PRESENT;
		}
		
		gpointer bc_data = oh_get_rdr_data(
			((struct oh_handler_state *)hnd)->rptcache,
			event->Source, rdr->RecordId);
		
		if (bc_data == NULL) {
			dbg("Sensor Data Pointer is NULL; RID=%x; SID=%d",
			    event->Source, 
			    event->EventDataUnion.SensorEvent.SensorNum);
			return -1;
		}
		
		event->EventDataUnion.SensorEvent.PreviousState = 
			((struct BC_SensorInfo *)bc_data)->cur_state;

		((struct BC_SensorInfo *)bc_data)->cur_state = 
			event->EventDataUnion.SensorEvent.EventState;
		break;
	}
	case SAHPI_ET_HOTSWAP:
	{
		gpointer bc_data = 
			oh_get_resource_data(((struct oh_handler_state *)hnd)->rptcache, 
					     event->Source);
		
		if (bc_data == NULL) {
			dbg("HotSwap Data Pointer is NULL; RID=%x\n", event->Source);
			return -1;
		}
		
		event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
			((struct BC_ResourceInfo *)bc_data)->cur_state;
		((struct BC_ResourceInfo *)bc_data)->cur_state = 
			event->EventDataUnion.HotSwapEvent.HotSwapState;
		break;
	}
	default:
		dbg("Unrecognized Event Type=%d\n", event->EventType);
		return -1;
	}
	
	return 0;
}

/****************************************************
 * Translate BladeCenter log message to HPI OEM Event
 ****************************************************/
static int map2oem(void *hnd, SaHpiEventT *event, bc_sel_entry *sel_entry,
		   OEMReasonCodeT reason)
{
	/* Set RID for OEM Event */
	if (bcsrc2rid(hnd, sel_entry->source, &(event->Source))) {
		dbg("Cannot translate BladeCenter Source string=%s to RID\n", 
		    sel_entry->source);
		return -1;
	}

	event->EventType = SAHPI_ET_OEM;
	event->EventDataUnion.OemEvent.MId = IBM_MANUFACTURING_ID;
	strncpy(event->EventDataUnion.OemEvent.OemEventData,
		sel_entry->text, SAHPI_OEM_EVENT_DATA_SIZE - 1);
	event->EventDataUnion.OemEvent.OemEventData
		[SAHPI_OEM_EVENT_DATA_SIZE - 1] = '\0';

	return 0;
}

/*****************************************************************
 * Translate BC error log "Source" text to HPI Resource ID. 
 * Calculate Entity Path to find RID. Assume Source string format:
 *
 *   "BLADE_0x" - map to blade x RID
 *   "SWITCH_x" - map to switch x RID
 *
 * All other Source text strings map to the Chassis's resource ID
 *****************************************************************/
int bcsrc2rid(void *hnd, gchar *src, SaHpiResourceIdT *rid)
{
	int rpt_index, isblade, ischassis, isswitch;
	guint instance;
	gchar **src_parts = NULL, *endptr = NULL;
	SaHpiEntityPathT ep, ep_root;
	SaHpiEntityTypeT entity_type;

	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	char *root_tuple = (char *)g_hash_table_lookup(handle->config, "entity_root");

	/* Find top-level chassis entity path */
	memset(&ep, 0, sizeof(SaHpiEntityPathT));
        string2entitypath(root_tuple, &ep_root);
        append_root(&ep_root);

        /* Chassis instance/type in root; other resources override these below */
	instance = ep_root.Entry[0].EntityInstance;
	entity_type = ep_root.Entry[0].EntityType;

	/* Break down Source text string to find source's rpt index and instance */
	src_parts = g_strsplit(src, "_", -1);
	if (src_parts == NULL) {
		dbg("Could not split error log's Source text string.");
		g_strfreev(src_parts);
		return -1;
	}

	isblade = isswitch = ischassis = 0;
	if (!strcmp(src_parts[0], "BLADE")) { isblade = 1; }
	else {
		if (!strcmp(src_parts[0], "SWITCH")) { isswitch = 1; }
	}

	if (isblade || isswitch) {
		instance = strtol(src_parts[1], &endptr, 10);
		if (isblade) { rpt_index = BC_RPT_ENTRY_BLADE; }
		else { rpt_index = BC_RPT_ENTRY_SWITCH_MODULE; }
		entity_type = snmp_rpt_array[rpt_index].rpt.ResourceEntity.Entry[0].EntityType;
	}
	else {
		ischassis = 1;
		rpt_index = BC_RPT_ENTRY_CHASSIS;
	}

	g_strfreev(src_parts);

	/* Find rest of Entity Path and calculate RID */
	ep = snmp_rpt_array[rpt_index].rpt.ResourceEntity;
	ep_concat(&ep, &ep_root);
	set_epath_instance(&ep, entity_type, instance);

	*rid = oh_uid_from_entity_path(&ep);

	return 0;
}

/**********************************************************
 * Parse BC error log entries into their various components
 **********************************************************/
static int parse_sel_entry(char *text, bc_sel_entry *sel, int isdst) 
{
        bc_sel_entry ent;
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
                dbg("Couldn't parse Severity from BladeCenter Log Entry");
                return -1;
        }
                
        while(start && (strncmp(start,"Source:",7) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Source:%19s",ent.source)) {
                dbg("Couldn't parse Source from BladeCenter Log Entry");
                return -1;
        }

        while(start && (strncmp(start,"Name:",5) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Name:%19s",ent.sname)) {
                dbg("Couldn't parse Name from BladeCenter Log Entry");
                return -1;
        }
        
        while(start && (strncmp(start,"Date:",5) != 0)) { start++; }
        if(!start) { return -2; }
        
        if(sscanf(start,"Date:%2d/%2d/%2d  Time:%2d:%2d:%2d",
                  &ent.time.tm_mon, &ent.time.tm_mday, &ent.time.tm_year, 
                  &ent.time.tm_hour, &ent.time.tm_min, &ent.time.tm_sec)) {
#if 0
		set_bc_dst(ss, &ent.time);
#else
		ent.time.tm_isdst = isdst;
#endif
                ent.time.tm_mon--;
                ent.time.tm_year += 100;
        } else {
                dbg("Couldn't parse Date/Time from BladeCenter Log Entry");
                return -1;
        }
        
        while(start && (strncmp(start,"Text:",5) != 0)) { start++; }
        if(!start) { return -2; }
        
        /* advance to data */
        start += 5;
        strncpy(ent.text,start,BC_SEL_ENTRY_STRING - 1);
        ent.text[BC_SEL_ENTRY_STRING - 1] = '\0';
        
        *sel = ent;
        return 0;
}


int snmp_bc_add_to_eventq(void *hnd, SaHpiEventT *thisEvent)
{
        struct oh_event working;
        struct oh_event *e = NULL;
        struct oh_handler_state *handle = hnd;

        memset(&working, 0, sizeof(struct oh_event));

	/* Setting rdr id to event struct */	
	switch (thisEvent->EventType == SAHPI_ET_OEM)
	{
		case SAHPI_ET_OEM:
		case SAHPI_ET_HOTSWAP:
		case SAHPI_ET_USER:
			working.u.hpi_event.id = 0;	/* There is no rdr associated to OEM event */
			break;			/* Set rdrid to invalid value of 0         */
		case SAHPI_ET_SENSOR:
			working.u.hpi_event.id = 
				get_rdr_uid(SAHPI_SENSOR_RDR,
						thisEvent->EventDataUnion.SensorEvent.SensorNum);
			break;
		case SAHPI_ET_WATCHDOG:
			working.u.hpi_event.id = 
				get_rdr_uid(SAHPI_WATCHDOG_RDR,
						thisEvent->EventDataUnion.WatchdogEvent.WatchdogNum);
			break;			
		default:
			dbg("Something is wrong mapping BC SNMP log entry into HPI Event\n");
			return -1;
			break;
	} 
	
        working.type = OH_ET_HPI;
        working.u.hpi_event.parent  = thisEvent->Source;       
        memcpy(&working.u.hpi_event.event, thisEvent, sizeof(SaHpiEventT));

        /* 
         * Insert entry to eventq for processing
        */
        e = g_malloc0(sizeof(struct oh_event));
        if (!e) {
                dbg("Out of memory!\n");
                return -1;
        }
        memcpy(e, &working, sizeof(struct oh_event));
        handle->eventq = g_slist_append(handle->eventq, e);

        return 0;
}
