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
 *      W. David Ashley <dashley@us.ibm.com>
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
#include <rsa_resources.h>
#include <rsa_str2event.h>
#include <rsa_errorlog.h>
#include <snmp_util.h>
#include <snmp_rsa_sel.h>
#include <snmp_rsa_utils.h>
#include <snmp_rsa_event.h>

typedef enum {
	EVENT_NOT_MAPPED,
	EVENT_NOT_ALERTABLE,
} OEMReasonCodeT;


static void free_hash_data(gpointer key, 
			   gpointer value, 
			   gpointer user_data);

static int parse_sel_entry(char *text, 
			   rsa_sel_entry *sel, 
			   int isdst);

static int parse_threshold_str(gchar *str, 
			       gchar *root_str, 
			       gchar *read_value_str, 
			       gchar *trigger_value_str);

static int set_previous_event_state(void *hnd, 
				    SaHpiEventT *event, 
				    int recovery_event, 
				    int *event_enabled);

static int map2oem(void *hnd, 
		   SaHpiEventT *event, 
		   LogSource2ResourceT *resinfo,
		   rsa_sel_entry *sel_entry,
		   OEMReasonCodeT reason);

static Str2EventInfoT *findevent4dupstr(gchar *search_str, 
					Str2EventInfoT *dupstrhash_data, 
					LogSource2ResourceT *resinfo);


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

int find_res_events(SaHpiEntityPathT *ep, const struct RSA_ResourceInfo *rsa_res_info)
{
	int i;
	int max = MAX_RESOURCE_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	
	for (i=0; rsa_res_info->event_array[i].event != NULL && i < max; i++) {

		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, rsa_res_info->event_array[i].event);
		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    rsa_res_info->event_array[i].event);
			return -1;
		}

		/*  Add to hash; Set HPI values */
		if (g_hash_table_lookup_extended(event2hpi_hash, normalized_str,
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
				rsa_res_info->event_array[i].event_state;

			/* Overload field with recovery state - this will be overwritten
			   when event is processed with current resource's state */
			hpievent->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
				rsa_res_info->event_array[i].recovery_state;

			g_hash_table_insert(event2hpi_hash, normalized_str, hpievent);
		}
		else {
			/* Event already exists */
			g_free(normalized_str);
		}
	}

	return 0;
}

int find_sensor_events(SaHpiEntityPathT *ep, SaHpiSensorNumT sid, const struct snmp_rsa_sensor *rpt_sensor)
{
	int i;
	int max = MAX_SENSOR_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid = oh_uid_from_entity_path(ep);
	
	for (i=0; rpt_sensor->rsa_sensor_info.event_array[i].event != NULL && i < max; i++) {
	
		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, rpt_sensor->rsa_sensor_info.event_array[i].event);

		if (normalized_str == NULL) {
			dbg("NULL string returned for event=%s\n", 
			    rpt_sensor->rsa_sensor_info.event_array[i].event);
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
				rpt_sensor->rsa_sensor_info.event_array[i].event_state;

			/* Overload field with recovery state - this will be overwritten
			   when event is processed with current resource's state */
			hpievent->EventDataUnion.SensorEvent.PreviousState = 
				rpt_sensor->rsa_sensor_info.event_array[i].recovery_state;

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
			/* Event already exists */
			g_free(normalized_str);
		}
	}

	return 0;
}

/****************************************************************************
 * log2event maps RSA error log entries to HPI events. 
 * isdst ("is DayLight Savings Time") parameter is a performance hack.
 * Design assumes the event's timestamp is the time local to the RSA box itself.
 * So instead of forcing RSA SNMP accesses for each log entry to determine if
 * DST is in effect on the RSA box, the isdst parameter allows the caller to 
 * query the RSA for DST info once then make multiple translation calls.
 *****************************************************************************/

int log2event(void *hnd, gchar *logstr, SaHpiEventT *event, int isdst, int *event_enabled)
{
	rsa_sel_entry log_entry;
	gchar *recovery_str, *login_str, search_str[RSA_SEL_ENTRY_STRING];
	gchar thresh_read_value[MAX_THRESHOLD_VALUE_STRINGSIZE]; 
	gchar thresh_trigger_value[MAX_THRESHOLD_VALUE_STRINGSIZE];
	int is_recovery_event, is_threshold_event;
	LogSource2ResourceT resinfo;
	SaHpiEventT working, *event_ptr;
	SaHpiSeverityT severity;
	Str2EventInfoT *strhash_data;

	memset(&working, 0, sizeof(SaHpiEventT));
	is_recovery_event = is_threshold_event = 0;

        /* Parse RSA error log entry into its various components */
	if (parse_sel_entry(logstr, &log_entry, isdst)) {
		dbg("Couldn't parse BladeCenter error log entry=%s", logstr);
		return -1;
	}

	/* Find default RID and BladeCenter resource info for error log's source */
	if (rsasrc2rid(hnd, log_entry.source, &resinfo)) {
		dbg("Cannot translate BladeCenter Source string=%s to RID\n", log_entry.source);
		return -1;
	}

	/* Assume event is enabled; unless we find out differently below */
	*event_enabled = 1;

	/* Fill-in HPI event time */
	working.Timestamp = (SaHpiTimeT)mktime(&log_entry.time) * 1000000000;

        /* Set default severity; usually overwritten below with RSAT-level severity */
	severity = log_entry.sev; 

	/******************************************************************
	 * Find event search string
         *  
	 * For some types of events (e.g. thresholds), RSA appends dynamic
	 * data which cannot be in the static str2event hash table. Need to
         * find the non-dynamic root of these strings.
         ******************************************************************/

	/* Set default search string */
	strncpy(search_str, log_entry.text, RSA_SEL_ENTRY_STRING);

	/* Discover Recovery event strings */
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
			memset(search_str, 0, RSA_SEL_ENTRY_STRING);
			strncpy(search_str, log_entry.text, (id_str - log_entry.text));
			search_str[(id_str - log_entry.text)] = '\0';
		}
	}

	/* Adjust threshold event strings */
	if (strstr(log_entry.text, LOG_THRESHOLD_VALUE_STRING)) {
		is_threshold_event = 1;
		if (parse_threshold_str(log_entry.text, search_str, thresh_read_value, thresh_trigger_value)) {
			dbg("Cannot parse threshold event string=%s\n", log_entry.text);
			strncpy(search_str, log_entry.text, RSA_SEL_ENTRY_STRING);	
		}
	}

	/* See if adjusted search string is a recognized RSA "alertable" event */
	strhash_data = (Str2EventInfoT *)g_hash_table_lookup(str2event_hash, search_str);
	if (strhash_data) {
		
		/* Handle strings that have multiple event numbers */
		int dupovrovr = 0;
		if (strhash_data->event_dup) {
			strhash_data = findevent4dupstr(search_str, strhash_data, &resinfo);
			if (strhash_data == NULL) {
				dbg("Could not find valid event for duplicate string=%s and RID=%d", 
				    search_str, resinfo.rid);
				if (map2oem(hnd, &working, &resinfo, &log_entry, EVENT_NOT_MAPPED)) {
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

                /* Use the RID calculated from error log string unless OVR_RID is set. 
		 * Unless overridden due to a dup string have OVR_RID set incorrectly.
		 */

		if (!(strhash_data->event_ovr & OVR_RID) || dupovrovr) {
			working.Source = resinfo.rid;
		}

		/* OVR_SEV overrides the event log's severity and uses 
		 * RSAT-level severity from calculated in off-line scripts 
		 */
		if (strhash_data->event_ovr & OVR_SEV) {
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

				/* Set sensor's current/last state; see if sensor's events are disabled */
				if (set_previous_event_state(hnd, &working, is_recovery_event, event_enabled)) {
					dbg("Cannot set previous state for sensor event = %s\n", log_entry.text);
					return -1;
				}
				
				if (!(*event_enabled)) {
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
				if (set_previous_event_state(hnd, &working, is_recovery_event, event_enabled)) {
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
			if (map2oem(hnd, &working, &resinfo, &log_entry, EVENT_NOT_MAPPED)) {
				dbg("Cannot map to OEM Event %s", log_entry.text);
				return -1;
			}
		} /* End found "alertable" event string in hash */
	}
	else { /* Map to OEM Event - String not recognized as RSA "alertable" */
		if (map2oem(hnd, &working, &resinfo, &log_entry, EVENT_NOT_ALERTABLE)) {
			dbg("Cannot map to OEM Event %s", log_entry.text);
			return -1;
		}
	}

 DONE:
	working.Severity = severity;
	memcpy((void *)event, (void *)&working, sizeof(SaHpiEventT));

	return 0;
}

static Str2EventInfoT *findevent4dupstr(gchar *search_str, Str2EventInfoT *strhash_data, LogSource2ResourceT *resinfo)
{
	gchar dupstr[RSA_SEL_ENTRY_STRING];
	Str2EventInfoT *dupstr_hash_data;
	short strnum;

	strncpy(dupstr, search_str, RSA_SEL_ENTRY_STRING);
	dupstr_hash_data = strhash_data;
	strnum = strhash_data->event_dup + 1; /* Original string plus dups */
       
	while (strnum && (dupstr_hash_data != NULL)) {
		int i,j;
		gchar *normalized_event; 

		/* Search sensor array for the duplicate string's event */
		for (i=0; (resinfo->sensor_array_ptr + i)->sensor.Num != 0; i++) {
			for (j=0; (resinfo->sensor_array_ptr + i)->rsa_sensor_info.event_array[j].event != NULL; j++) {
				normalized_event = snmp_derive_objid(resinfo->ep,
						   (resinfo->sensor_array_ptr + i)->rsa_sensor_info.event_array[j].event);
				if (!strcmp(dupstr_hash_data->event, normalized_event)) {
					g_free(normalized_event);
					return dupstr_hash_data;
				}
				g_free(normalized_event);
			}
		}
		
		/* Search resource array for the duplicate string's event */
		for (i=0; snmp_rpt_array[resinfo->rpt].rsa_res_info.event_array[i].event != NULL; i++) {
			normalized_event = snmp_derive_objid(resinfo->ep,
					   snmp_rpt_array[resinfo->rpt].rsa_res_info.event_array[i].event);
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
			strncpy(dupstr, tmpstr, RSA_SEL_ENTRY_STRING);
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
 * This function parses RSA threshold log messages into their various 
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
static int set_previous_event_state(void *hnd, SaHpiEventT *event, int recovery_event, int *event_enabled) 
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
		
		gpointer rsa_data = oh_get_rdr_data(
			((struct oh_handler_state *)hnd)->rptcache,
			event->Source, rdr->RecordId);
		
		if (rsa_data == NULL) {
			dbg("Sensor Data Pointer is NULL; RID=%x; SID=%d",
			    event->Source, 
			    event->EventDataUnion.SensorEvent.SensorNum);
			return -1;
		}

		/* Check to see if events are disabled for this sensor */
		if (!((struct RSA_SensorInfo *)rsa_data)->sensor_evt_enablement.SensorStatus &
		    SAHPI_SENSTAT_EVENTS_ENABLED) {
			*event_enabled = 0;
			return 0;
		}

		/********************************
		 * Set Current and Previous State 
		 ********************************/
		if (recovery_event) {
			/* Recovery state is stored in the previous state field in the
			   event's hash table - too lazy to create a different field */
			SaHpiEventStateT tmpstate;
			
			tmpstate = ((struct RSA_SensorInfo *)rsa_data)->cur_state;

			((struct RSA_SensorInfo *)rsa_data)->cur_state = 
				event->EventDataUnion.SensorEvent.PreviousState;

			event->EventDataUnion.SensorEvent.PreviousState = tmpstate;
		}
		else { /* Normal non-recovery case */
			event->EventDataUnion.SensorEvent.PreviousState = 
				((struct RSA_SensorInfo *)rsa_data)->cur_state;
			
			((struct RSA_SensorInfo *)rsa_data)->cur_state = 
				event->EventDataUnion.SensorEvent.EventState;
		}

		break;
	}
	case SAHPI_ET_HOTSWAP:
	{
		gpointer rsa_data = 
			oh_get_resource_data(((struct oh_handler_state *)hnd)->rptcache, event->Source);
		
		if (rsa_data == NULL) {
			dbg("HotSwap Data Pointer is NULL; RID=%x\n", event->Source);
			return -1;
		}
		
		/********************************
		 * Set Current and Previous State 
		 ********************************/
		if (recovery_event) {
			/* Recovery state is stored in the previous state field in the
			   event's hash table - too lazy to create a different field */
			SaHpiHsStateT tmpstate;
			
			tmpstate = ((struct RSA_ResourceInfo *)rsa_data)->cur_state;

			((struct RSA_ResourceInfo *)rsa_data)->cur_state = 
				event->EventDataUnion.HotSwapEvent.PreviousHotSwapState;

			event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = tmpstate;
		}
		else { /* Normal non-recovery case */
			event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
				((struct RSA_ResourceInfo *)rsa_data)->cur_state;
			((struct RSA_ResourceInfo *)rsa_data)->cur_state = 
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
 * Translate RSA log message to HPI OEM Event
 ****************************************************/
static int map2oem(void *hnd, SaHpiEventT *event, LogSource2ResourceT *resinfo,
		   rsa_sel_entry *sel_entry, OEMReasonCodeT reason)
{
	event->Source = resinfo->rid;
	event->EventType = SAHPI_ET_OEM;
	event->EventDataUnion.OemEvent.MId = IBM_MANUFACTURING_ID;
	strncpy(event->EventDataUnion.OemEvent.OemEventData,
		sel_entry->text, SAHPI_OEM_EVENT_DATA_SIZE - 1);
	event->EventDataUnion.OemEvent.OemEventData
		[SAHPI_OEM_EVENT_DATA_SIZE - 1] = '\0';
	
	return 0;
}

/*****************************************************************
 * Translate RSA error log "Source" text to HPI Resource ID. 
 * Calculate Entity Path to find RID.
 * All Source text strings map to the Chassis's resource ID
 *****************************************************************/
int rsasrc2rid(void *hnd, gchar *src, LogSource2ResourceT *resinfo)
{
	int rpt_index;
	guint instance;
	gchar **src_parts = NULL;
	SaHpiEntityPathT ep, ep_root;
	SaHpiEntityTypeT entity_type;
	struct snmp_rsa_sensor *array_ptr;

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

	rpt_index = RSA_RPT_ENTRY_CHASSIS;
	array_ptr = &snmp_rsa_chassis_sensors[0];

	g_strfreev(src_parts);

	/* Find rest of Entity Path and calculate RID */
	ep = snmp_rpt_array[rpt_index].rpt.ResourceEntity;
	ep_concat(&ep, &ep_root);
	set_epath_instance(&ep, entity_type, instance);

	/* Fill in RID and RPT table info about Error Log's Source */
	resinfo->rid = oh_uid_from_entity_path(&ep);
	resinfo->rpt = rpt_index;
	resinfo->sensor_array_ptr = array_ptr;
	resinfo->ep = ep;
	
	return 0;
}

/**********************************************************
 * Parse RSA error log entries into their various components
 **********************************************************/
static int parse_sel_entry(char *text, rsa_sel_entry *sel, int isdst) 
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

/*******************************************
 * Add event to Infrastructure's event queue
 *******************************************/
int snmp_rsa_add_to_eventq(void *hnd, SaHpiEventT *thisEvent)
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
			dbg("Something is wrong mapping RSA SNMP log entry into HPI Event\n");
			return -1;
			break;
	} 
	
        working.type = OH_ET_HPI;
        working.u.hpi_event.parent  = thisEvent->Source;       
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
