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
#include <snmp_bc.h>
#include <snmp_bc_sel.h>
#include <snmp_bc_utils.h>
#include <snmp_bc_event.h>

unsigned int str2event_use_count = 0; /* It is here for initialization */ 
 
typedef enum {
	EVENT_NOT_MAPPED,
	EVENT_NOT_ALERTABLE,
} OEMReasonCodeT;

static void free_hash_data(gpointer key, 
			   gpointer value, 
			   gpointer user_data);

static int parse_sel_entry(char *text, 
			   bc_sel_entry *sel, 
			   int isdst);

static int parse_threshold_str(gchar *str, 
			       gchar *root_str, 
			       gchar *read_value_str, 
			       gchar *trigger_value_str);

static int set_previous_event_state(void *hnd, 
				    SaHpiEventT *event,
				    int recovery_event,
				    int *event_enabled_ptr);

static int map2oem(SaHpiEventT *event, 
		   bc_sel_entry *sel_entry,
		   OEMReasonCodeT reason);

static Str2EventInfoT *findevent4dupstr(gchar *search_str, 
					Str2EventInfoT *dupstrhash_data, 
					LogSource2ResourceT *resinfo);


int event2hpi_hash_init(struct oh_handler_state *handle)
{
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	GHashTable *event2hpi_hash = g_hash_table_new(g_str_hash, g_str_equal);
	custom_handle->event2hpi_hash_ptr = event2hpi_hash;
	
	if (event2hpi_hash == NULL) {
		dbg("Cannot allocate event2hpi_hash table");
		return -1;
	}
	
	return 0;
}

int event2hpi_hash_free(struct oh_handler_state *handle)
{
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	GHashTable *event2hpi_hash = custom_handle->event2hpi_hash_ptr;
	
        g_hash_table_foreach(event2hpi_hash, free_hash_data, NULL);
	g_hash_table_destroy(event2hpi_hash);

	return 0;
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key); /* Memory was created for these during normalization process */
        g_free(value);
}

int find_res_events(struct oh_handler_state *handle,SaHpiEntityPathT *ep, const struct BC_ResourceInfo *bc_res_info)
{
	int i;
	int max = MAX_RESOURCE_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	GHashTable *event2hpi_hash = custom_handle->event2hpi_hash_ptr;
	
	for (i=0; bc_res_info->event_array[i].event != NULL && i < max; i++) {

		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, bc_res_info->event_array[i].event);
		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    bc_res_info->event_array[i].event);
			return -1;
		}

		/*  Add to hash; Set HPI values */
		if (!g_hash_table_lookup_extended(event2hpi_hash, normalized_str,
						  (gpointer)&hash_existing_key,
						  (gpointer)&hash_value)) {

			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Cannot allocate memory for HPI event");
				g_free(normalized_str);
				return -1;
			}

			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_HOTSWAP;
			hpievent->EventDataUnion.HotSwapEvent.HotSwapState = 
				bc_res_info->event_array[i].event_state;

			/* Overload field with recovery state. Event hash doesn't store
			   dynamic data so the hash can use this field for recovery data */
			hpievent->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
				bc_res_info->event_array[i].recovery_state;

			g_hash_table_insert(event2hpi_hash, normalized_str, hpievent);
			/* normalized_str space is recovered when hash is freed */
		}
		else {
			/* Event already exists (e.g. same event for multiple blades) */
			g_free(normalized_str);
		}
	}

	return 0;
}

int find_sensor_events(struct oh_handler_state *handle,SaHpiEntityPathT *ep, SaHpiSensorNumT sid, const struct snmp_bc_sensor *rpt_sensor)
{
	int i;
	int max = MAX_SENSOR_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	GHashTable *event2hpi_hash = custom_handle->event2hpi_hash_ptr;
	
		
	for (i=0; rpt_sensor->bc_sensor_info.event_array[i].event != NULL && i < max; i++) {
		
		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, rpt_sensor->bc_sensor_info.event_array[i].event);

		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    rpt_sensor->bc_sensor_info.event_array[i].event);
			return -1;
		}

		/*  Add to hash; Set HPI values */
		if (!g_hash_table_lookup_extended(event2hpi_hash, 
						  normalized_str,
						  (gpointer)&hash_existing_key, 
						  (gpointer)&hash_value)) {

			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Cannot allocate memory for HPI event");
				g_free(normalized_str);
				return -1;
			}

			/* Set values */
			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_SENSOR;
			hpievent->EventDataUnion.SensorEvent.SensorNum = sid;
			hpievent->EventDataUnion.SensorEvent.SensorType = rpt_sensor->sensor.Type;
			hpievent->EventDataUnion.SensorEvent.EventCategory = rpt_sensor->sensor.Category;
			hpievent->EventDataUnion.SensorEvent.Assertion = SAHPI_TRUE;
			hpievent->EventDataUnion.SensorEvent.EventState = 
				rpt_sensor->bc_sensor_info.event_array[i].event_state;

			/* Overload field with recovery state. Event hash doesn't store
			   dynamic data so the hash can use this field for recovery data */
			hpievent->EventDataUnion.SensorEvent.PreviousState = 
				rpt_sensor->bc_sensor_info.event_array[i].recovery_state;

			/* Setup trigger values for threshold sensors */
			if (rpt_sensor->sensor.ThresholdDefn.IsThreshold == SAHPI_TRUE) {
				if (rpt_sensor->sensor.DataFormat.Range.Max.ValuesPresent & SAHPI_SRF_INTERPRETED) {
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
				else {
					dbg("Max.Interpreted must be defined for threshold sensor=%s\n",
					    rpt_sensor->comment);
					g_free(normalized_str);
					g_free(hpievent);
					return -1;
				}
			}

			g_hash_table_insert(event2hpi_hash, normalized_str, hpievent);
			/* normalized_str space is recovered when hash is freed */
		}
		else {
			/* Event already exists (e.g. same event for multiple blades) */
			g_free(normalized_str);
		}
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

int log2event(void *hnd, gchar *logstr, SaHpiEventT *event, int isdst, int *event_enabled_ptr)
{
	bc_sel_entry log_entry;
	gchar *recovery_str, *login_str; 
	gchar root_str[BC_SEL_ENTRY_STRING], search_str[BC_SEL_ENTRY_STRING];
	gchar thresh_read_value[MAX_THRESHOLD_VALUE_STRINGSIZE]; 
	gchar thresh_trigger_value[MAX_THRESHOLD_VALUE_STRINGSIZE];
	int is_recovery_event, is_threshold_event;
	LogSource2ResourceT resinfo;
	SaHpiEventT working, *event_ptr;
	SaHpiResourceIdT event_rid;
	SaHpiSeverityT event_severity;
	SaHpiTimeT event_time;
	Str2EventInfoT *strhash_data;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	GHashTable *event2hpi_hash = custom_handle->event2hpi_hash_ptr;

	memset(&working, 0, sizeof(SaHpiEventT));
	is_recovery_event = is_threshold_event = 0;

        /* Parse BC error log entry into its various components */
	if (parse_sel_entry(logstr, &log_entry, isdst)) {
		dbg("Couldn't parse BladeCenter error log entry=%s", logstr);
		return -1;
	}

	/* Find default RID and BladeCenter resource info from error log's source */
	if (bcsrc2rid(hnd, log_entry.source, &resinfo)) {
		dbg("Cannot translate BladeCenter Source string=%s to RID\n", log_entry.source);
		return -1;
	}
	
	/* Set dynamic event fields with default values from the error log string
	   These can be overwritten in the code below */
	event_rid = resinfo.rid;
	event_time = (SaHpiTimeT)mktime(&log_entry.time) * 1000000000;
	event_severity = log_entry.sev; 

	/* Assume event is enabled; unless we find out differently */
	*event_enabled_ptr = 1;

	/******************************************************************
	 * Find event search string
         *  
	 * For some types of events (e.g. thresholds), BC appends dynamic
	 * data which cannot be in the static str2event hash table. Need to
         * find the non-dynamic root of these strings.
         ******************************************************************/

	/* Set default search string */
	strncpy(search_str, log_entry.text, BC_SEL_ENTRY_STRING);

	/* Discover Recovery event strings */
	recovery_str = strstr(search_str, EVT_RECOVERY);
	if (recovery_str && (recovery_str == search_str)) {
		is_recovery_event = 1;
		memset(search_str, 0, BC_SEL_ENTRY_STRING);
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
		if (parse_threshold_str(search_str, root_str, thresh_read_value, thresh_trigger_value)) {
			dbg("Cannot parse threshold event string=%s\n", search_str);
		}
		else {
			memset(search_str, 0, BC_SEL_ENTRY_STRING);
			strcpy(search_str, root_str);	
		}
	}

#if 0
	dbg ("Search string=%s xxx", search_str);
#endif
	/* See if adjusted search string is a recognized BC "alertable" event */
	strhash_data = (Str2EventInfoT *)g_hash_table_lookup(str2event_hash, search_str);
	if (strhash_data) {

		/* Handle strings that have multiple event numbers */
		int dupovrovr = 0;
		if (strhash_data->event_dup) {
			strhash_data = findevent4dupstr(search_str, strhash_data, &resinfo);
			if (strhash_data == NULL) {
				dbg("Could not find valid event for duplicate string=%s and RID=%d", 
				    search_str, resinfo.rid);
				if (map2oem(&working, &log_entry, EVENT_NOT_MAPPED)) {
					dbg("Cannot map to OEM Event %s", log_entry.text);
					return -1;
				}
				goto DONE;
			}
			if (strhash_data->event_ovr & OVR_RID) {
				dbg("Cannot have RID override on duplicate string=%s", search_str);
				dupovrovr = 1;
			}
		}

		/* If OVR_SEV, use BCT-level severity calculated in off-line scripts */
		if (strhash_data->event_ovr & OVR_SEV) {
			event_severity = strhash_data->event_sev;
		}

		/* Look to see if event is mapped to an HPI entity */
		event_ptr = (SaHpiEventT *)g_hash_table_lookup(event2hpi_hash, strhash_data->event);
		if (event_ptr) {

                        /* Set static event data defined during resource discovery */
			working = *event_ptr; 

			/* If OVR_RID, use rid from bc_resources.c
			   (unless dup strings have OVR_RID set incorrectly) */
			if ((strhash_data->event_ovr & OVR_RID) && !dupovrovr) {
				event_rid = event_ptr->Source;
			}
			else {
				working.Source = event_rid; /* Restore error log's RID */
			}
	
			/* Handle sensor events */
			if (working.EventType == SAHPI_ET_SENSOR) {

				/* ??? Should we read sensors on recovery events 
				 * ??? to determine the real state of the sensor. 
				 * ??? Currently recovery state is hardcoded in bc_resources.c
				 */
				if (is_recovery_event) {
					working.EventDataUnion.SensorEvent.Assertion = SAHPI_FALSE;
				}

				/* Set sensor's current/last state; see if sensor's events are disabled */
				if (set_previous_event_state(hnd, &working, is_recovery_event, event_enabled_ptr)) {
					dbg("Cannot set previous state for sensor event = %s\n", log_entry.text);
					return -1;
				}
				
				if (!(*event_enabled_ptr)) {
					return 0;
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
			}

                        /* Handle hot-swap events */
			else if (working.EventType == SAHPI_ET_HOTSWAP) {
				if (set_previous_event_state(hnd, &working, is_recovery_event, event_enabled_ptr)) {
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
			if (map2oem(&working, &log_entry, EVENT_NOT_MAPPED)) {
				dbg("Cannot map to OEM Event %s", log_entry.text);
				return -1;
			}
		} /* End found "alertable" event string in hash */
	}
	else { /* Map to OEM Event - String not recognized as BC "alertable" */
		if (map2oem(&working, &log_entry, EVENT_NOT_ALERTABLE)) {
			dbg("Cannot map to OEM Event %s", log_entry.text);
			return -1;
		}
	}

 DONE:
	working.Source = event_rid;
	working.Timestamp = event_time;
	working.Severity = event_severity;
	memcpy((void *)event, (void *)&working, sizeof(SaHpiEventT));

	return 0;
}

static Str2EventInfoT *findevent4dupstr(gchar *search_str, Str2EventInfoT *strhash_data, LogSource2ResourceT *resinfo)
{
	gchar dupstr[BC_SEL_ENTRY_STRING];
	Str2EventInfoT *dupstr_hash_data;
	short strnum;


	strncpy(dupstr, search_str, BC_SEL_ENTRY_STRING);
	dupstr_hash_data = strhash_data;
	strnum = strhash_data->event_dup + 1; /* Original string plus dups */
       
	while (strnum && (dupstr_hash_data != NULL)) {
		int i,j;
		gchar *normalized_event; 

		/* Search sensor array for the duplicate string's event */
		for (i=0; (resinfo->sensor_array_ptr + i)->sensor.Num != 0; i++) {
			for (j=0; (resinfo->sensor_array_ptr + i)->bc_sensor_info.event_array[j].event != NULL; j++) {
				normalized_event = snmp_derive_objid(resinfo->ep,
						   (resinfo->sensor_array_ptr + i)->bc_sensor_info.event_array[j].event);
				if (!strcmp(dupstr_hash_data->event, normalized_event)) {
					g_free(normalized_event);
					return dupstr_hash_data;
				}
				g_free(normalized_event);
			}
		}
		
		/* Search resource array for the duplicate string's event */
		for (i=0; snmp_rpt_array[resinfo->rpt].bc_res_info.event_array[i].event != NULL; i++) {
			normalized_event = snmp_derive_objid(resinfo->ep,
					   snmp_rpt_array[resinfo->rpt].bc_res_info.event_array[i].event);
			if (!strcmp(dupstr_hash_data->event, normalized_event)) {
				g_free(normalized_event);
				return dupstr_hash_data;
			}
			g_free(normalized_event);
		}

		/* Find next duplicate string */
		strnum--;
		if (strnum) {
			gchar strnum_str[6];
			gchar *tmpstr;

			sprintf(strnum_str,"%d", strnum);
			tmpstr = g_strconcat(search_str, HPIDUP_STRING, strnum_str, NULL);
			strncpy(dupstr, tmpstr, BC_SEL_ENTRY_STRING);
			g_free(tmpstr);

			dupstr_hash_data = (Str2EventInfoT *)g_hash_table_lookup(str2event_hash, dupstr);
			if (dupstr_hash_data == NULL) {
				dbg("Cannot find duplicate string=%s\n", dupstr);
			}
		}
	}

	return NULL;
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
static int set_previous_event_state(void *hnd, SaHpiEventT *event, 
				    int recovery_event, int *event_enabled_ptr) 
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

		/* Check to see if events are disabled for this sensor */
		if (!((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement.SensorStatus &
		    SAHPI_SENSTAT_EVENTS_ENABLED) {
			*event_enabled_ptr = 0;
			return 0;
		}

		/********************************
		 * Set Current and Previous State 
		 ********************************/
		if (recovery_event) {

			/* Recovery state is stored in the previous state field in the
			   event's hash table. Dynamic data is stored in RDR not the 
			   event hash table, so event hash table uses previous
			   field to hold recovery data */
			
			SaHpiEventStateT tmpstate;
			tmpstate = ((struct BC_SensorInfo *)bc_data)->cur_state;

			/* Set both RDR/event's current state; Remember for recovery 
			   event hash's previous = recovery state */
			((struct BC_SensorInfo *)bc_data)->cur_state = 
				event->EventDataUnion.SensorEvent.PreviousState; 
			event->EventDataUnion.SensorEvent.EventState = 
				event->EventDataUnion.SensorEvent.PreviousState;
			event->EventDataUnion.SensorEvent.PreviousState = tmpstate;
		}
		else { /* Normal non-recovery case */
			event->EventDataUnion.SensorEvent.PreviousState = 
				((struct BC_SensorInfo *)bc_data)->cur_state;
			((struct BC_SensorInfo *)bc_data)->cur_state = 
				event->EventDataUnion.SensorEvent.EventState;
		}

		break;
	}
	case SAHPI_ET_HOTSWAP:
	{
		gpointer bc_data = 
			oh_get_resource_data(((struct oh_handler_state *)hnd)->rptcache, event->Source);
		
		if (bc_data == NULL) {
			dbg("HotSwap Data Pointer is NULL; RID=%x\n", event->Source);
			return -1;
		}
		
		/********************************
		 * Set Current and Previous State 
		 ********************************/
		if (recovery_event) {

			/* Recovery state is stored in the previous state field in the
			   event's hash table. Dynamic data is stored in RDR not the 
			   event hash table, so event hash table uses previous
			   field to hold recovery data */
			
			SaHpiHsStateT tmpstate;
			tmpstate = ((struct BC_ResourceInfo *)bc_data)->cur_state;

			/* Set both RDR/event's current state; Remember for recovery 
			   event hash's previous = recovery state */
			((struct BC_ResourceInfo *)bc_data)->cur_state = 
				event->EventDataUnion.HotSwapEvent.PreviousHotSwapState;
			event->EventDataUnion.HotSwapEvent.HotSwapState = 
				event->EventDataUnion.HotSwapEvent.PreviousHotSwapState;
			event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = tmpstate;
		}
		else { /* Normal non-recovery case */
			event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
				((struct BC_ResourceInfo *)bc_data)->cur_state;
			((struct BC_ResourceInfo *)bc_data)->cur_state = 
				event->EventDataUnion.HotSwapEvent.HotSwapState;
		}
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
static int map2oem(SaHpiEventT *event, bc_sel_entry *sel_entry, OEMReasonCodeT reason)
{
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
int bcsrc2rid(void *hnd, gchar *src, LogSource2ResourceT *resinfo)
{
	int rpt_index, isblade, ischassis, isswitch;
	guint instance;
	gchar **src_parts = NULL, *endptr = NULL;
	SaHpiEntityPathT ep, ep_root;
	SaHpiEntityTypeT entity_type;
	struct snmp_bc_sensor *array_ptr;

	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	char *root_tuple = (char *)g_hash_table_lookup(handle->config, "entity_root");

	/* Find top-level chassis entity path */
	memset(&ep, 0, sizeof(SaHpiEntityPathT));
	append_root(&ep);
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
		if (isblade) { 
			rpt_index = BC_RPT_ENTRY_BLADE; 
			array_ptr = &snmp_bc_blade_sensors[0];
		}
		else { 
			rpt_index = BC_RPT_ENTRY_SWITCH_MODULE;
			array_ptr = &snmp_bc_switch_sensors[0];
		}
		entity_type = snmp_rpt_array[rpt_index].rpt.ResourceEntity.Entry[0].EntityType;
	}
	else {
		ischassis = 1;
		rpt_index = BC_RPT_ENTRY_CHASSIS;
		array_ptr = &snmp_bc_chassis_sensors[0];
	}

	g_strfreev(src_parts);

	/* Find rest of Entity Path and calculate RID */
	if (ep_concat(&ep, &snmp_rpt_array[rpt_index].rpt.ResourceEntity)) {
		dbg("ep_concat failed for ep init");
		return -1;
	}
	append_root(&ep);
	if (ep_concat(&ep, &ep_root)) {
		dbg("ep_concat failed to append root");
		return -1;
	}
	if (set_epath_instance(&ep, entity_type, instance)) {
		dbg("set_epath_instance failed. type=%d; instance=%d\n", entity_type, instance);
		return -1;
	}

	/* Fill in RID and RPT table info about Error Log's Source */
	resinfo->rid = oh_uid_from_entity_path(&ep);
	resinfo->rpt = rpt_index;
	resinfo->sensor_array_ptr = array_ptr;
	resinfo->ep = ep;
	
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
                ent.time.tm_mon--;
                ent.time.tm_year += 100;
        } else {
                dbg("Couldn't parse Date/Time from BladeCenter Log Entry");
                return -1;
        }
        
        while(start && (strncmp(start,"Text:",5) != 0)) { start++; }
        if(!start) { return -2; }
        
        /* Advance to data */
        start += 5;
        strncpy(ent.text,start,BC_SEL_ENTRY_STRING - 1);
        ent.text[BC_SEL_ENTRY_STRING - 1] = '\0';
        
        *sel = ent;
        return 0;
}

/*******************************************
 * Add event to Infrastructure's event queue
 *******************************************/
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
			working.u.hpi_event.id = 0; /* There is no rdr associated to OEM event */
			break;			    /* Set rdrid to invalid value of 0         */
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
        working.u.hpi_event.parent = thisEvent->Source;       
        memcpy(&working.u.hpi_event.event, thisEvent, sizeof(SaHpiEventT));

        /* Insert entry to eventq for processing */
        e = g_malloc0(sizeof(struct oh_event));
        if (!e) {
                dbg("Out of memory!\n");
                return -1;
        }
        memcpy(e, &working, sizeof(struct oh_event));
        handle->eventq = g_slist_append(handle->eventq, e);

        return 0;
}
