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

#include <snmp_bc_plugin.h>

typedef enum {
	EVENT_NOT_MAPPED,
	EVENT_NOT_ALERTABLE,
} OEMReasonCodeT;

typedef struct {
	SaHpiResourceIdT       rid;
	BCRptEntryT            rpt;
	struct snmp_bc_sensor  *sensor_array_ptr;
	SaHpiEntityPathT       ep;
} LogSource2ResourceT;

static SaErrorT snmp_bc_parse_threshold_str(gchar *str,
					    gchar *root_str,
					    SaHpiTextBufferT *read_value_str,
					    SaHpiTextBufferT *trigger_value_str);

static SaErrorT snmp_bc_logsrc2rid(struct oh_handler_state *handle,
				   gchar *src,
				   LogSource2ResourceT *resinfo,
				   unsigned short ovr_flags);

static SaErrorT snmp_bc_set_previous_event_state(struct oh_handler_state *handle,
						 SaHpiEventT *event,
						 int recovery_event,
						 int *event_enabled_ptr);

static SaErrorT snmp_bc_map2oem(SaHpiEventT *event,
				bc_sel_entry *sel_entry,
				OEMReasonCodeT reason);

static Xml2EventInfoT *snmp_bc_findevent4dupstr(gchar *search_str,
						Xml2EventInfoT *dupstrhash_data,
						LogSource2ResourceT *resinfo);

SaErrorT event2hpi_hash_init(struct oh_handler_state *handle)
{
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	custom_handle->event2hpi_hash_ptr = g_hash_table_new(g_str_hash, g_str_equal);
	if (custom_handle->event2hpi_hash_ptr == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	return(SA_OK);
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key); /* Memory was created for these during normalization process */
        g_free(value);
}


SaErrorT event2hpi_hash_free(struct oh_handler_state *handle)
{
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (custom_handle->event2hpi_hash_ptr != NULL) {
		g_hash_table_foreach(custom_handle->event2hpi_hash_ptr, free_hash_data, NULL);
		g_hash_table_destroy(custom_handle->event2hpi_hash_ptr);
	}

	return(SA_OK);
}

/**
 * snmp_bc_discover_res_events: 
 * @handle: Pointer to handler's data.
 * @ep: Pointer resource's entity path.
 * @res_info_ptr: Pointer to resource's mapping info.
 *
 * Discovers resource's events and records static event 
 * information into a hash table that translates platform event
 * numbers into HPI event types.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle, @ep, @res_info_ptr, or event2hpi_hash_ptr is NULL.
 **/
SaErrorT snmp_bc_discover_res_events(struct oh_handler_state *handle,
				     SaHpiEntityPathT *ep,
				     const struct BC_ResourceInfo *res_info_ptr)
{
	int i;
	int max = SNMP_BC_MAX_RESOURCE_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	if (!handle || !ep || !res_info_ptr || !(custom_handle->event2hpi_hash_ptr)) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	rid = oh_uid_lookup(ep);
	if (rid == 0) {
		dbg("No RID.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	for (i=0; res_info_ptr->event_array[i].event != NULL && i < max; i++) {
		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, res_info_ptr->event_array[i].event);
		if (normalized_str == NULL) {
			dbg("Cannot derive %s.", res_info_ptr->event_array[i].event);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		/*  Add to hash; Set HPI values */
		if (!g_hash_table_lookup_extended(custom_handle->event2hpi_hash_ptr,
						  normalized_str,
						  (gpointer)&hash_existing_key,
						  (gpointer)&hash_value)) {

			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Out of memory.");
				g_free(normalized_str);
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}

			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_HOTSWAP;
			hpievent->EventDataUnion.HotSwapEvent.HotSwapState = 
				res_info_ptr->event_array[i].event_state;

			/* Overload field with recovery state. Event hash doesn't store
			   dynamic data so the hash can use this field for recovery data */
			hpievent->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
				res_info_ptr->event_array[i].recovery_state;

			trace("Discovered resource event=%s.", normalized_str);

			g_hash_table_insert(custom_handle->event2hpi_hash_ptr, normalized_str, hpievent);
			/* normalized_str space is recovered when hash is freed */
		}
		else {
			/* Event already exists (e.g. same event for multiple blades) */
			g_free(normalized_str);
		}
	}

	return(SA_OK);
}

/**
 * snmp_bc_discover_sensor_events: 
 * @handle: Pointer to handler's data.
 * @ep: Pointer parent resource's entity path.
 * @sid: Sensor ID.
 * @rpt_sensor: Pointer to sensor data.
 *
 * Discovers sensor's events and records static event 
 * information into a hash table that translates platform event
 * numbers into HPI event types.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle, @ep, @res_info_ptr, or event2hpi_hash_ptr NULL; 
 *                             sid not valid.
 **/
SaErrorT snmp_bc_discover_sensor_events(struct oh_handler_state *handle,
					SaHpiEntityPathT *ep,
					SaHpiSensorNumT sid,
					const struct snmp_bc_sensor *rpt_sensor)
{
	int i;
	int max = SNMP_BC_MAX_SENSOR_EVENT_ARRAY_SIZE;
	char *normalized_str;
	char *hash_existing_key, *hash_value;
	SaHpiEventT *hpievent;
	SaHpiResourceIdT rid;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!handle || !ep || !rpt_sensor || 
	    !(custom_handle->event2hpi_hash_ptr) || sid <= 0) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	rid = oh_uid_lookup(ep);
	if (rid == 0) {
		dbg("No RID.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
		
	for (i=0; rpt_sensor->bc_sensor_info.event_array[i].event != NULL && i < max; i++) {
		/* Normalized and convert event string */
		normalized_str = snmp_derive_objid(*ep, rpt_sensor->bc_sensor_info.event_array[i].event);
		if (normalized_str == NULL) {
			dbg("Cannot derive %s.", rpt_sensor->bc_sensor_info.event_array[i].event);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		/*  Add to hash; Set HPI values */
		if (!g_hash_table_lookup_extended(custom_handle->event2hpi_hash_ptr,
						  normalized_str,
						  (gpointer)&hash_existing_key, 
						  (gpointer)&hash_value)) {

			hpievent = g_malloc0(sizeof(SaHpiEventT));
			if (!hpievent) {
				dbg("Out of memory.");
				g_free(normalized_str);
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}

			/* Set values */
			hpievent->Source = rid;
			hpievent->EventType = SAHPI_ET_SENSOR;
			hpievent->EventDataUnion.SensorEvent.SensorNum = sid;
			hpievent->EventDataUnion.SensorEvent.SensorType = rpt_sensor->sensor.Type;
			hpievent->EventDataUnion.SensorEvent.EventCategory = rpt_sensor->sensor.Category;
			hpievent->EventDataUnion.SensorEvent.Assertion =
				rpt_sensor->bc_sensor_info.event_array[i].event_assertion;
			hpievent->EventDataUnion.SensorEvent.EventState =
				rpt_sensor->bc_sensor_info.event_array[i].event_state;

			/* Overload field with recovery state. Event hash doesn't store
			   dynamic data so the hash can use this field for recovery data */
			hpievent->EventDataUnion.SensorEvent.PreviousState = 
				rpt_sensor->bc_sensor_info.event_array[i].recovery_state;

			/* Setup static trigger info for threshold sensors - some may be event-only */
			if (rpt_sensor->sensor.Category == SAHPI_EC_THRESHOLD &&
			    rpt_sensor->sensor.DataFormat.IsSupported == SAHPI_TRUE &&
			    rpt_sensor->sensor.DataFormat.ReadingType != SAHPI_SENSOR_READING_TYPE_BUFFER) {
				hpievent->EventDataUnion.SensorEvent.TriggerReading.IsSupported = SAHPI_TRUE;
				hpievent->EventDataUnion.SensorEvent.TriggerThreshold.IsSupported = SAHPI_TRUE;
				hpievent->EventDataUnion.SensorEvent.TriggerReading.Type =
				hpievent->EventDataUnion.SensorEvent.TriggerThreshold.Type =
					rpt_sensor->sensor.DataFormat.ReadingType;
			}
			else {
				hpievent->EventDataUnion.SensorEvent.TriggerReading.IsSupported = SAHPI_FALSE;	
				hpievent->EventDataUnion.SensorEvent.TriggerThreshold.IsSupported = SAHPI_FALSE;
			}

			trace("Discovered sensor event=%s.", normalized_str);

			g_hash_table_insert(custom_handle->event2hpi_hash_ptr, normalized_str, hpievent);
			/* normalized_str space is recovered when hash is freed */
		}
		else {
			/* Event already exists (e.g. same event for multiple blades) */
			g_free(normalized_str);
		}
	}

	return(SA_OK);
}

/**
 * snmp_bc_log2event: 
 * @handle: Pointer to handler's data.
 * @logstr: Platform Event Log to be mapped.
 * @event: Pointer to put mapped event.
 * @isdst: Boolean to indicate of DST on on/off.
 * @event_enabled_ptr: 
 *
 * Maps platform error log entries to HPI events.
 * 
 * @isdst ("is DayLight Savings Time") parameter is a performance hack.
 * Design assumes the event's timestamp is the time local to the platform itself.
 * So instead of forcing platform accesses for each log entry to determine if
 * DST is in effect, the isdst parameter allows the caller to query the
 * hardware DST info once then make multiple translation calls.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle, @logstr, @event, @event_enabled_ptr NULL.
 **/
SaErrorT snmp_bc_log2event(struct oh_handler_state *handle,
			   gchar *logstr,
			   SaHpiEventT *event,
			   int isdst,
			   int *event_enabled_ptr)
{
	bc_sel_entry        log_entry;
	gchar               *recovery_str, *login_str;
	gchar               root_str[SNMP_BC_MAX_SEL_ENTRY_LENGTH], search_str[SNMP_BC_MAX_SEL_ENTRY_LENGTH];
	LogSource2ResourceT resinfo;
	SaErrorT            err;
	SaHpiBoolT          is_recovery_event, is_threshold_event;
	SaHpiEventT         working, *event_ptr;
	SaHpiResourceIdT    event_rid;
	SaHpiSeverityT      event_severity;
	SaHpiTextBufferT    thresh_read_value, thresh_trigger_value;
	SaHpiTimeT          event_time;
	Xml2EventInfoT      *strhash_data;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!handle || !logstr || !event || !event_enabled_ptr) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	memset(&working, 0, sizeof(SaHpiEventT));
	is_recovery_event = is_threshold_event = SAHPI_FALSE;

        /* Parse hardware log entry into its various components */
	err = snmp_bc_parse_sel_entry(handle, logstr, &log_entry);
	if (err) {
		dbg("Cannot parse log entry=%s. Error=%s.", logstr, oh_lookup_error(err));
		return(err);
	}

	/* Find default RID from log's "Source" field */
	err = snmp_bc_logsrc2rid(handle, log_entry.source, &resinfo, 0);
	if (err) {
		dbg("Cannot translate %s to RID. Error=%s", log_entry.source, oh_lookup_error(err));
		return(err);
	}
	
	/* Set dynamic event fields with default values from the log string.
	   These may be overwritten in the code below */
	event_rid = resinfo.rid;
	event_time = (SaHpiTimeT)mktime(&log_entry.time) * 1000000000;
	event_severity = log_entry.sev; 

	/* FIXME:: Do tmp and only set if no errors - really this is a boolean ?? */
	/* Assume event is enabled; unless we find out differently */
	*event_enabled_ptr = 1;

	/**********************************************************************
	 * For some types of events (e.g. thresholds), dynamic data is appended
	 * to some root string. Need to find this root string, since its the
         * root string which is mapped in the XML to event hash table.
         **********************************************************************/

	/* Set default search string */
	strncpy(search_str, log_entry.text, SNMP_BC_MAX_SEL_ENTRY_LENGTH);

	/* Discover "recovery" event strings */
	recovery_str = strstr(search_str, EVT_RECOVERY);
	if (recovery_str && (recovery_str == search_str)) {
		is_recovery_event = SAHPI_TRUE;
		memset(search_str, 0, SNMP_BC_MAX_SEL_ENTRY_LENGTH);
		strcpy(search_str, (log_entry.text + strlen(EVT_RECOVERY)));
	}

	/* Adjust "login" event strings - strip username */
	login_str = strstr(log_entry.text, LOG_LOGIN_STRING);
	if (login_str) {
		gchar *id_str = strstr(log_entry.text, LOG_LOGIN_CHAR);
		if (id_str != NULL) {
			memset(search_str, 0, SNMP_BC_MAX_SEL_ENTRY_LENGTH);
			strncpy(search_str, log_entry.text, (id_str - log_entry.text));
			search_str[(id_str - log_entry.text)] = '\0';
		}
	}

	/* Adjust "threshold" event strings */
	if (strstr(log_entry.text, LOG_THRESHOLD_VALUE_STRING)) {
		is_threshold_event = SAHPI_TRUE;
		oh_init_textbuffer(&thresh_read_value);
		oh_init_textbuffer(&thresh_trigger_value);
		err = snmp_bc_parse_threshold_str(search_str, root_str,
						  &thresh_read_value, &thresh_trigger_value);
		if (err) {
			dbg("Cannot parse threshold string=%s.", search_str);
		}
		else {
			memset(search_str, 0, SNMP_BC_MAX_SEL_ENTRY_LENGTH);
			strcpy(search_str, root_str);
		}
	}

	trace("Search string=%s.", search_str);

	/* See if adjusted root string is in the XML to event hash table */
	strhash_data = (Xml2EventInfoT *)g_hash_table_lookup(bc_xml2event_hash, search_str);
	if (strhash_data) {
		/* Handle strings that have multiple event numbers */
		int dupovrovr = 0;
		if (strhash_data->event_dup) {
			strhash_data = snmp_bc_findevent4dupstr(search_str, strhash_data, &resinfo);
			if (strhash_data == NULL) {
				dbg("Cannot find valid event for duplicate string=%s and RID=%d.", 
				    search_str, resinfo.rid);
				if (snmp_bc_map2oem(&working, &log_entry, EVENT_NOT_MAPPED)) {
					dbg("Cannot map to OEM Event %s.", log_entry.text);
					return(SA_ERR_HPI_INTERNAL_ERROR);
				}
				goto DONE;
			}
			if (strhash_data->event_ovr & OVR_RID) {
				dbg("Cannot have RID override on duplicate string=%s.", search_str);
				dupovrovr = 1;
			}
		}

		/* If OVR_SEV, use BCT-level severity calculated in off-line scripts */
		if (strhash_data->event_ovr & OVR_SEV) {
			event_severity = strhash_data->event_sev;
		}

		/* Look to see if event is mapped to an HPI entity */
		event_ptr = (SaHpiEventT *)g_hash_table_lookup(custom_handle->event2hpi_hash_ptr, 
							       strhash_data->event);
		if (event_ptr) {
                        /* Set static event data defined during resource discovery */
			working = *event_ptr;
			/* Find RID */
			if (strhash_data->event_ovr & OVR_EXP) {
				/* If OVR_EXP, find RID of expansion card */
				err = snmp_bc_logsrc2rid(handle, log_entry.source, &resinfo, 
							 strhash_data->event_ovr);
				if (err) {
					dbg("Cannot translate %s to RID. Error=%s.", 
					    log_entry.source, oh_lookup_error(err));
					return(err);
				}
				event_rid = resinfo.rid;
			}
			else {
				/* if OVR_RID, use RID from bc_resources.c */
				/* (unless dup strings have OVR_RID set incorrectly) */
 				if ((strhash_data->event_ovr & OVR_RID) && !dupovrovr) {
					event_rid = event_ptr->Source;
				}
				else {
					/* Restore original RID calculated from error log */
					working.Source = event_rid; 
				}
			}

			/* Handle sensor events */
			if (working.EventType == SAHPI_ET_SENSOR) {
				if (is_recovery_event) {
					/* FIXME:: we should read sensors on recovery events to get current
					 * state. Currently recovery state is hardcoded in bc_resources.c
					 * hardcoded recovery states should be removed. Set Optional bits
					 * for current state */
					/* FIXME: Assertion is not right - we shouldn't set here 
                                           Originally thought this meant sensor in a fail state */
					working.EventDataUnion.SensorEvent.Assertion = SAHPI_FALSE;
				}

				/* Set sensor's current/last state; see if sensor's events are disabled */
				err = snmp_bc_set_previous_event_state(handle, &working,
								       is_recovery_event, event_enabled_ptr);
				if (err) {
					dbg("Cannot set previous state for %s; Error=%s.", 
					    log_entry.text, oh_lookup_error(err));
					return(SA_ERR_HPI_INTERNAL_ERROR);
				}
				
				if (!(*event_enabled_ptr)) {
					return(SA_OK);
				}

				/* Convert threshold strings into event values */
				if (is_threshold_event) {
					/* FIXME:: Do we need to check mib.convert_snmpstr >= 0 */
					/* FIXME:: Need to check IsSupported??? */
					if (oh_encode_sensorreading(&thresh_read_value,
								    working.EventDataUnion.SensorEvent.TriggerReading.Type,
								    &working.EventDataUnion.SensorEvent.TriggerReading)) {
						dbg("Cannot convert trigger reading=%s for text=%s.",
						    thresh_read_value.Data, log_entry.text);
						return(SA_ERR_HPI_INTERNAL_ERROR);
					}
					working.EventDataUnion.SensorEvent.OptionalDataPresent =
						working.EventDataUnion.SensorEvent.OptionalDataPresent | SAHPI_SOD_TRIGGER_READING;
					
					if (oh_encode_sensorreading(&thresh_trigger_value,
								    working.EventDataUnion.SensorEvent.TriggerThreshold.Type,
								    &working.EventDataUnion.SensorEvent.TriggerThreshold)) {
						dbg("Cannot convert trigger threshold=%s for text=%s.",
						    thresh_trigger_value.Data, log_entry.text);
						return(SA_ERR_HPI_INTERNAL_ERROR);
					}
					working.EventDataUnion.SensorEvent.OptionalDataPresent =
						working.EventDataUnion.SensorEvent.OptionalDataPresent | SAHPI_SOD_TRIGGER_THRESHOLD;
				}
			}

                        /* Handle hot-swap events */
			else if (working.EventType == SAHPI_ET_HOTSWAP) {
				err = snmp_bc_set_previous_event_state(handle, &working, is_recovery_event, event_enabled_ptr);
				if (err) {
					dbg("Cannot set previous state for %s; Error=%s",
					    log_entry.text, oh_lookup_error(err));
					return(SA_ERR_HPI_INTERNAL_ERROR);
				}
			}
			else {
				dbg("Platform doesn't support events of type=%s.",
				    oh_lookup_eventtype(working.EventType));
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}
		} /* End found mapped event */
		else { /* Map to OEM Event - Log Not Mapped */
			if (snmp_bc_map2oem(&working, &log_entry, EVENT_NOT_MAPPED)) {
				dbg("Cannot map to OEM Event %s.", log_entry.text);
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}
		}
	}
	else { /* Map to OEM Event - String not in XML to event hash table */
		if (snmp_bc_map2oem(&working, &log_entry, EVENT_NOT_ALERTABLE)) {
			dbg("Cannot map to OEM Event %s.", log_entry.text);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

 DONE:
	working.Source = event_rid;
	working.Timestamp = event_time;
	working.Severity = event_severity;
	memcpy((void *)event, (void *)&working, sizeof(SaHpiEventT));

	return(SA_OK);
}

static Xml2EventInfoT *snmp_bc_findevent4dupstr(gchar *search_str,
						Xml2EventInfoT *strhash_data,
						LogSource2ResourceT *resinfo)
{
	gchar dupstr[SNMP_BC_MAX_SEL_ENTRY_LENGTH];
	Xml2EventInfoT *dupstr_hash_data;
	short strnum;

	strncpy(dupstr, search_str, SNMP_BC_MAX_SEL_ENTRY_LENGTH);
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
			gchar strnum_str[OH_MAX_LOCATION_DIGITS];
			gchar *tmpstr;

			snprintf(strnum_str, OH_MAX_LOCATION_DIGITS, "%d", strnum);
			tmpstr = g_strconcat(search_str, HPIDUP_STRING, strnum_str, NULL);
			strncpy(dupstr, tmpstr, SNMP_BC_MAX_SEL_ENTRY_LENGTH);
			g_free(tmpstr);

                        /* FIXME Fix for RSA */
			dupstr_hash_data = (Xml2EventInfoT *)g_hash_table_lookup(bc_xml2event_hash, dupstr);
			if (dupstr_hash_data == NULL) {
				dbg("Cannot find duplicate string=%s.", dupstr);
			}
		}
	}

	return NULL;
}

/**
 * snmp_bc_parse_threshold_str:
 * @str: Input threshold log string.
 * @root_str: Location to store threshold's root string.
 * @read_value_str: Location to store threshold's read value string.
 * @trigger_value_str: Location to store threshold's trigger value string.
 *
 * Parses a threshold log entry string into its root string and read 
 * and trigger value strings.
 * 
 * Format is a root string (in the XML to event hash table) followed by a
 * read threshold value string, followed by a trigger threshold value string.
 * Unfortunately cannot convert directly to sensor values yet because 
 * don't yet know if event is in the event to HPI event hash or if it is, 
 * what the sensor's threshold data type is.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @str, @root_str, @read_value_str, 
 *                             or @trigger_value_str is NULL.
 **/
static SaErrorT snmp_bc_parse_threshold_str(gchar *str,
					    gchar *root_str,
					    SaHpiTextBufferT *read_value_str,
					    SaHpiTextBufferT *trigger_value_str)
{
	gchar  **event_substrs = NULL;
	gchar  **thresh_substrs = NULL;
	int rtn_code = SA_OK;

	if (!str || !root_str || !read_value_str || !trigger_value_str) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	event_substrs = g_strsplit(str, LOG_READ_VALUE_STRING, -1);
	thresh_substrs = g_strsplit(event_substrs[1], LOG_THRESHOLD_VALUE_STRING, -1);

	if (thresh_substrs == NULL ||
	    (thresh_substrs[0] == NULL || thresh_substrs[0][0] == '\0') ||
	    (thresh_substrs[1] == NULL || thresh_substrs[1][0] == '\0') ||
	    (thresh_substrs[2] != NULL)) {
		dbg("Cannot split threshold string=%s.", str);
		rtn_code = SA_ERR_HPI_INTERNAL_ERROR;
		goto CLEANUP;
	}

	if ((strlen(thresh_substrs[0]) > SAHPI_MAX_TEXT_BUFFER_LENGTH) ||
	    (strlen(thresh_substrs[1]) > SAHPI_MAX_TEXT_BUFFER_LENGTH)) {
		dbg("Threshold value string(s) exceed max size for %s.", str);
		rtn_code = SA_ERR_HPI_INTERNAL_ERROR;
		goto CLEANUP;
	}
	
	strcpy(root_str, event_substrs[0]);
	oh_append_textbuffer(read_value_str, thresh_substrs[0]);
	oh_append_textbuffer(trigger_value_str, thresh_substrs[1]);

	if (root_str == NULL) {
		dbg("Cannot parse threshold string=%s.", str);
		rtn_code = SA_ERR_HPI_INTERNAL_ERROR;
	}

 CLEANUP:
	g_strfreev(event_substrs);
	g_strfreev(thresh_substrs);

	return rtn_code;
}

/**********************************
 * Set previous state info in event
 **********************************/
static SaErrorT snmp_bc_set_previous_event_state(struct oh_handler_state *handle,
						 SaHpiEventT *event,
						 int recovery_event,
						 int *event_enabled_ptr)
{
	switch (event->EventType) {
	case SAHPI_ET_SENSOR:
	{
		SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache,
						    event->Source, SAHPI_SENSOR_RDR,
						    event->EventDataUnion.SensorEvent.SensorNum);
		if (rdr == NULL) {
			return(SA_ERR_HPI_NOT_PRESENT);
		}
		
		gpointer bc_data = oh_get_rdr_data(handle->rptcache, event->Source, rdr->RecordId);
		if (bc_data == NULL) {
			dbg("No sensor data. RID=%x; SID=%d.",
			    event->Source, event->EventDataUnion.SensorEvent.SensorNum);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

#if 0
		/* FIXME:: Port to B.1.1. event enablement schemes */
		/* Check to see if events are disabled for this sensor */
		if (!((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement.SensorStatus &
		    SAHPI_SENSTAT_EVENTS_ENABLED) {
			*event_enabled_ptr = 0;
			return(SA_OK);
		}
#endif
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
			oh_get_resource_data(handle->rptcache, event->Source);
		
		if (bc_data == NULL) {
			dbg("No hotswap data. RID=%x", event->Source);
			return(SA_ERR_HPI_INTERNAL_ERROR);
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
		dbg("Unrecognized Event Type=%d.", event->EventType);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	event->EventDataUnion.SensorEvent.OptionalDataPresent = 
		event->EventDataUnion.SensorEvent.OptionalDataPresent | SAHPI_SOD_PREVIOUS_STATE;
	
	return(SA_OK);
}

/**
 * snmp_bc_map2oem:
 * @event: Pointer to handler's data.
 * @sel_entry: Log's "Source" field string.
 * @reason: Location to store HPI mapping data for resource
 *
 * Any event not explicitly recognized is mapped into an HPI 
 * OEM event. This routine performs the mapping. 
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @event or @sel_entry is NULL.
 **/
static SaErrorT snmp_bc_map2oem(SaHpiEventT *event,
				bc_sel_entry *sel_entry,
				OEMReasonCodeT reason)
{
	/* A reason code is passed, if in the future we record
           in some temp file, non-mapped events. Reason records
           why the event wasn't mapped */

	if (!event || !sel_entry) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	trace("OEM Event Reason Code=%s\n", reason ? "NOT_ALERTABLE" : "NOT MAPPED");

	event->EventType = SAHPI_ET_OEM;
	event->EventDataUnion.OemEvent.MId = IBM_MANUFACTURING_ID;

	/* Language set to ENGLISH, default  */
	oh_init_textbuffer(&(event->EventDataUnion.OemEvent.OemEventData));
	strncpy(event->EventDataUnion.OemEvent.OemEventData.Data,
		sel_entry->text, SAHPI_MAX_TEXT_BUFFER_LENGTH - 1);
	event->EventDataUnion.OemEvent.OemEventData.Data[SAHPI_MAX_TEXT_BUFFER_LENGTH - 1] = '\0';
	event->EventDataUnion.OemEvent.OemEventData.DataLength = 
								strlen(sel_entry->text);
	return(SA_OK);
}

/**
 * snmp_bc_logsrc2rid:
 * @handle: Pointer to handler's data.
 * @src: Log's "Source" field string.
 * @res_info: Location to store HPI mapping data for resource
 * @ovr_flags: Override flags
 *
 * Translates error log's "Source" field into an HPI resource ID
 * and stores HPI mapping info needed by other routines in
 * @res_info. Assume "Source" field text is in the following format:
 *
 *   "BLADE_0x" - map to blade x RID
 *   "SWITCH_x" - map to switch x RID
 *
 * All other "Source" field text strings are mapped to the 
 * Chassis's resource ID.
 *
 * @ovr_flags is used to indicate exception cases. The only one
 * currently is to indicate if the resource is an expansion card.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle, @src, or @resinfo is NULL.
 **/
static SaErrorT snmp_bc_logsrc2rid(struct oh_handler_state *handle,
				   gchar *src,
				   LogSource2ResourceT *resinfo,
				   unsigned short ovr_flags)
{
	int rpt_index;
	guint loc;
	gchar **src_parts = NULL, *endptr = NULL, *root_tuple;
	SaErrorT err;
	SaHpiBoolT isblade, isexpansioncard, ischassis, isswitch;
	SaHpiEntityPathT ep, ep_root;
	SaHpiEntityTypeT entity_type;
	struct snmp_bc_sensor *array_ptr;

	if (!handle || !src || !resinfo) {
		dbg("Invalid parameters.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Find top-level chassis entity path */
        ep_init(&ep);
	ep_init(&ep_root);
	root_tuple = (gchar *)g_hash_table_lookup(handle->config, "entity_root");
        string2entitypath(root_tuple, &ep_root);
        
        /* Assume chassis location/type unless another resource type is discovered */
	loc = ep_root.Entry[0].EntityLocation;
	entity_type = ep_root.Entry[0].EntityType;

	/* Break down "Source" text string to find source's RPT index and location */
	src_parts = g_strsplit(src, "_", -1);
	if (src_parts == NULL) {
		dbg("Cannot split Source text string.");
		g_strfreev(src_parts);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* See if resource is something other than the chassis */
	isblade = isexpansioncard = isswitch = ischassis = SAHPI_FALSE;
	if (!strcmp(src_parts[0], "BLADE")) { 
		/* All expansion card events are reported as blade events in Error Log */
		if (ovr_flags & OVR_EXP) { isexpansioncard = SAHPI_TRUE; }
		else { isblade = SAHPI_TRUE; }
	}
	else {
		if (!strcmp(src_parts[0], "SWITCH")) { isswitch = SAHPI_TRUE; }
	}
	/* If not the chassis, find the location value from last part of log's source string */
	if (isexpansioncard == SAHPI_TRUE || isblade == SAHPI_TRUE || isswitch == SAHPI_TRUE) {
		loc = strtoul(src_parts[1], &endptr, 10);
		if (isexpansioncard == SAHPI_TRUE) {
			rpt_index = BC_RPT_ENTRY_BLADE_ADDIN_CARD;
			array_ptr = &snmp_bc_blade_addin_sensors[0];	
		}
		else {
			if (isblade == SAHPI_TRUE) { 
				rpt_index = BC_RPT_ENTRY_BLADE; 
				array_ptr = &snmp_bc_blade_sensors[0];
			}
			else { 
				rpt_index = BC_RPT_ENTRY_SWITCH_MODULE;
				array_ptr = &snmp_bc_switch_sensors[0];
			}
		}
		entity_type = snmp_rpt_array[rpt_index].rpt.ResourceEntity.Entry[0].EntityType;
	}
	else {
		ischassis = SAHPI_TRUE;
		rpt_index = BC_RPT_ENTRY_CHASSIS;
		array_ptr = &snmp_bc_chassis_sensors[0];
	}
	g_strfreev(src_parts);

	/* Find rest of Entity Path and calculate RID */
	err = ep_concat(&ep, &snmp_rpt_array[rpt_index].rpt.ResourceEntity);
	if (err) {
		dbg("Cannot concat Entity Path. Error=%s.", oh_lookup_error(err));
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	err = ep_concat(&ep, &ep_root);
	if (err) {
		dbg("Cannot concat Entity Path. Error=%s.", oh_lookup_error(err));
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	err = set_ep_instance(&ep, entity_type, loc);
	if (err) {
		dbg("Cannot set location. Type=%s; Location=%d; Error=%s.",
		    oh_lookup_entitytype(entity_type), loc, oh_lookup_error(err));
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	/* Special case - if Expansion Card set location of parent blade as well */
	if (isexpansioncard == SAHPI_TRUE) {
		err = set_ep_instance(&ep, SAHPI_ENT_SBC_BLADE, loc);
		if (err) {
			dbg("Cannot set location. Type=%s; Location=%d; Error=%s.",
			    oh_lookup_entitytype(SAHPI_ENT_SBC_BLADE), loc, oh_lookup_error(err));  
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

	/* Fill in RID and RPT table info about "Source" */
	resinfo->rpt = rpt_index;
	resinfo->sensor_array_ptr = array_ptr;
	resinfo->ep = ep;
	resinfo->rid = oh_uid_lookup(&ep);
	if (resinfo->rid == 0) {
		dbg("No RID");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	return(SA_OK);
}

 /**
 * snmp_bc_add_to_eventq
 * @handle: Pointer to handler's data.
 * @thisEvent: Location to store event.
 *
 * Add event to Infrastructure's event queue.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @handle or @thisEvent is NULL.
 **/
SaErrorT snmp_bc_add_to_eventq(struct oh_handler_state *handle, SaHpiEventT *thisEvent)
{
	SaHpiEntryIdT rdrid=0;
        struct oh_event working;
        struct oh_event *e = NULL;
	SaHpiRptEntryT *thisRpt;
	
        memset(&working, 0, sizeof(struct oh_event));

	working.did = oh_get_default_domain_id();
	working.type = OH_ET_HPI;
	
	thisRpt = oh_get_resource_by_id(handle->rptcache, thisEvent->Source);
        if (thisRpt) 
		working.u.hpi_event.res = *thisRpt;
	else 
		dbg("NULL Rpt pointer for rid %d\n", thisEvent->Source);

        memcpy(&working.u.hpi_event.event, thisEvent, sizeof(SaHpiEventT));

	/* FIXME:: Merged with same type of code in snmp_bc_sel.c 
           to create common function???? */
	   
	/* FIXME:: Add other B.1.1 event types */
	/*   SAHPI_ET_RESOURCE                 */
	/*   SAHPI_ET_DOMAIN                   */
	/*   SAHPI_ET_SENSOR_ENABLE_CHANGE     */
	/*   SAHPI_ET_HPI_SW                   */
	/* ----------------------------------- */

	/* Setting RDR ID to event struct */	
	switch (thisEvent->EventType) {
		case SAHPI_ET_OEM:
		case SAHPI_ET_HOTSWAP:
		case SAHPI_ET_USER:
                                        /* There is no RDR associated to OEM event */
                        memset(&working.u.hpi_event.rdr, 0, sizeof(SaHpiRdrT));
                        working.u.hpi_event.rdr.RdrType = SAHPI_NO_RECORD;
                                          /* Set RDR Type to SAHPI_NO_RECORD, spec B-01.01 */
                                          /* It is redundant because SAHPI_NO_RECORD == 0 */
                                          /* Put code here for clarity      */
			break;			           
		case SAHPI_ET_SENSOR:
			rdrid = get_rdr_uid(SAHPI_SENSOR_RDR,
				    thisEvent->EventDataUnion.SensorEvent.SensorNum); 
			working.u.hpi_event.rdr = *(oh_get_rdr_by_id(handle->rptcache, thisEvent->Source, rdrid));
			break;
		case SAHPI_ET_WATCHDOG:
			rdrid = get_rdr_uid(SAHPI_WATCHDOG_RDR,
				    thisEvent->EventDataUnion.WatchdogEvent.WatchdogNum);
			working.u.hpi_event.rdr = *(oh_get_rdr_by_id(handle->rptcache, thisEvent->Source, rdrid));
			break;
		default:
			dbg("Unrecognized Event Type=%d.", thisEvent->EventType);
			return(SA_ERR_HPI_INTERNAL_ERROR);
			break;
	} 

        /* Insert entry to eventq for processing */
        e = g_malloc0(sizeof(struct oh_event));
        if (!e) {
                dbg("Out of memory.");
                return(SA_ERR_HPI_OUT_OF_SPACE);
        }
        memcpy(e, &working, sizeof(struct oh_event));
        handle->eventq = g_slist_append(handle->eventq, e);

        return(SA_OK);
}
