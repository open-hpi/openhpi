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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SaHpi.h>

#include <openhpi.h>
#include <oh_plugin.h>
#include <rpt_utils.h>
#include <uid_utils.h>
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
static int  map2event(void *hnd, SaHpiEventT *event);
static int  map2oemevent(SaHpiEventT *event, gchar *oemstr, SaHpiSeverityT sev, OEMReasonCodeT reason);

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
	
#if 0
	printf("find_res_events RID = %d\n", rid);
	printf("Entity Path: Entry0=%d:%d Entry1=%d:%d Entry2=%d:%d Entry3=%d:%d\n",
	       ep->Entry[0].EntityType,
	       ep->Entry[0].EntityInstance,
	       ep->Entry[1].EntityType,
	       ep->Entry[1].EntityInstance,
	       ep->Entry[2].EntityType,
	       ep->Entry[2].EntityInstance,
	       ep->Entry[3].EntityType,
	       ep->Entry[3].EntityInstance);
#endif

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
	Str2EventInfoT *strhash_data;
	SaHpiEventT working, *event_ptr;
	SaHpiSeverityT severity;
	bc_sel_entry log_entry;

	memset(&working, 0, sizeof(SaHpiEventT));
	
	if (parse_sel_entry(logstr, &log_entry, isdst)) {
		dbg("Couldn't parse SEL entry");
		return -1;
	}

	/* Fill-in HPI time */
	working.Timestamp = (SaHpiTimeT)mktime(&log_entry.time) * 1000000000;

        /* Set default severity; usually overwritten below with BCT-level severity */
	severity = log_entry.sev; 

	/* See if string is a recognized BC "alertable" event */
	strhash_data = 
		(Str2EventInfoT *)g_hash_table_lookup(str2event_hash, log_entry.text);

	if (strhash_data) {
		printf ("Event Found in String Table: Event=%s; Sev=%d; OVR=%d\n",
			strhash_data->event,
			strhash_data->event_sev,
			strhash_data->event_sev_ovr);
		
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
			printf ("Found event No exceptions: Source=%d Type=%d Sev=%d\n",
				working.Source, working.EventType, severity);
			if (map2event (hnd, &working)) {
				dbg("Cannot map dynamic event data");
				return -1;
			}
		}
		else { /* Map to OEM Event - Log Not Mapped */
			printf ("In string table but not in event table - Map to OEM event\n");
			if (map2oemevent(&working, log_entry.text, severity, EVENT_NOT_MAPPED)) {
				dbg("Cannot map to OEM Event %s", log_entry.text);
				return -1;
			}
		}
	}
	else {
		printf("NOT Found %s in String Hash: Look for exceptions\n", log_entry.text);
		/* Look for threshold events */
		if (strstr(log_entry.text, LOG_THRESHOLD_VALUE_STRING)) {
			gchar  **event_substrs = NULL;
			gchar  **thresh_substrs = NULL;

			event_substrs = g_strsplit(log_entry.text, LOG_READ_VALUE_STRING, -1);
			thresh_substrs = g_strsplit(event_substrs[1], LOG_THRESHOLD_VALUE_STRING, -1);

			printf ("Event string = %s AND %s; Thresh parts are %s AND %s\n", 
				event_substrs[0], event_substrs[1],
				thresh_substrs[0], thresh_substrs[1]);

			if (thresh_substrs == NULL ||
			    (thresh_substrs[0] == NULL || thresh_substrs[0][0] == '\0') ||
			    (thresh_substrs[1] == NULL || thresh_substrs[1][0] == '\0') ||
			    (thresh_substrs[2] != NULL)) {
				dbg("Cannot split threshold event %s properly", log_entry.text);
				g_strfreev(event_substrs);
				g_strfreev(thresh_substrs);
				return -1;
			}

			/* Re-check string table */
			/* See if root string is a recognized BC "alertable" event */
			strhash_data = 
				(Str2EventInfoT *)g_hash_table_lookup(str2event_hash, event_substrs[0]);
			if (strhash_data) {

				/* Check severity override */
				if (strhash_data->event_sev_ovr == NOOVR) {
					severity = strhash_data->event_sev;
				}

				/* Look to see if event is mapped to an HPI entity */
				event_ptr = (SaHpiEventT *)g_hash_table_lookup(event2hpi_hash, strhash_data->event);
				if (event_ptr) {

					working = *event_ptr;
					printf("Threshold event is mapped: Source=%d Type=%d Sev=%d\n",
					       working.Source, working.EventType, severity);

					/* Convert strings read and thresholds values into trigger info */
					if (get_interpreted_value(thresh_substrs[0], 
								  working.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Type, 
								  &working.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value)) {
						dbg("Cannot convert trigger reading=%s for text=%s\n",
						    thresh_substrs[0],
						    log_entry.text);

						/* g_strfreev(event_substrs); g_strfreev(thresh_substrs); */
						return -1;
					}

					if (get_interpreted_value(thresh_substrs[1], 
								  working.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Type, 
								  &working.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value)) {
						dbg("Cannot convert trigger threshold=%s for text=%s\n",
						    thresh_substrs[1],
						    log_entry.text);
						/* g_strfreev(event_substrs); g_strfreev(thresh_substrs); */
						return -1;
					}					

					printf("Trigger Reading=%f; Threadhold=%f\n",
					       working.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32,
					       working.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32);

					if (map2event (hnd, &working)) {
						dbg("Cannot map dynamic event data");
						/* g_strfreev(event_substrs); g_strfreev(thresh_substrs); */
						return -1;
					}
				}
				else { /* Map to OEM Event - Log Not Mapped */
					printf ("In string table but not in event table - Map to OEM \n");
					if (map2oemevent(&working, logstr, severity, EVENT_NOT_MAPPED)) {
						dbg("Cannot map to OEM Event %s", logstr);
						/* g_strfreev(event_substrs); g_strfreev(thresh_substrs); */
						return -1;
					}
				}
			}

			g_strfreev(event_substrs); 
			g_strfreev(thresh_substrs);
		}  /* End look for threshold extensions */

		/* Look for login events */
		/* Look for recovery events - Set Assertion to FALSE */
		/* #define EVT_RECOVERY "Recovery events " */

		else {
			printf ("Not in string table - Map to OEM \n");
			if (map2oemevent(&working, log_entry.text, severity, EVENT_NOT_ALERTABLE)) {
				dbg("Cannot map to OEM Event %s", log_entry.text);
				return -1;
			}
		}
	}
/*
- Need to verify that event states match category (DEBUG only?)
on sensors defs in bc_resources.h/c
*/
	
	working.Severity = severity;
	memcpy((void *)event, (void *)&working, sizeof(SaHpiEventT));
	
	return 0;
}

/*********************************************************
 * Fill in dynamic event data. 
 * This is info not statically contained in bc_resources.c
 *********************************************************/
static int map2event(void *hnd, SaHpiEventT *event) 
{
	/* Handle Previous State */
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
			dbg("Sensor Data Pointer is NULL; RID=%d; SID=%d",
			    event->Source, 
			    event->EventDataUnion.SensorEvent.SensorNum);
			return -1;
		}
		
		event->EventDataUnion.SensorEvent.PreviousState = 
			((struct BC_SensorInfo *)bc_data)->cur_state;

		((struct BC_SensorInfo *)bc_data)->cur_state = 
			event->EventDataUnion.SensorEvent.EventState;

		printf("Writing Sensor cur_state\n");
		printf("Sensor RESOURCE=%d. Sensor=%d; PreviousState=%d, CurrentState=%d\n",
		       event->Source,
		       event->EventDataUnion.SensorEvent.SensorNum,
		       event->EventDataUnion.SensorEvent.PreviousState,
		       event->EventDataUnion.SensorEvent.EventState);
		break;
	}
	case SAHPI_ET_HOTSWAP:
	{
		gpointer bc_data = 
			oh_get_resource_data(((struct oh_handler_state *)hnd)->rptcache, 
					     event->Source);
		
		if (bc_data == NULL) {
			dbg("HotSwap Data Pointer is NULL; RID=%d\n", event->Source);
			return -1;
		}
		
		event->EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
			((struct BC_ResourceInfo *)bc_data)->cur_state;
		((struct BC_ResourceInfo *)bc_data)->cur_state = 
			event->EventDataUnion.HotSwapEvent.HotSwapState;
		
		printf("Writing HotSwap cur_state\n");
		printf("HOTSWAP RESOURCE=%d. PreviousState=%d, CurrentState=%d\n",
		       event->Source,
		       event->EventDataUnion.HotSwapEvent.PreviousHotSwapState,
		       event->EventDataUnion.HotSwapEvent.HotSwapState);
		break;
	}
	default:
		dbg("Unrecognized Event Type=%d\n", event->EventType);
		return -1;
	}
	
	return 0;
}

/************************
 * Map event to OEM Event
 ************************/
static int map2oemevent(SaHpiEventT *event, char *oemstr, SaHpiSeverityT sev, OEMReasonCodeT reason)
{
	
/* ??? FIXME: What should RID be?
 * For error log processing could pass in rid to log2event but what about
 * events in log2event that aren't mapped? What id to assign these? How do we
 * find this ID?
 */
	event->Source = 0;
	event->Severity = sev;
	event->EventType = SAHPI_ET_OEM;
	event->EventDataUnion.OemEvent.MId = 2;             /* Should hardcode this number */  
	strncpy(event->EventDataUnion.OemEvent.OemEventData,
		oemstr, SAHPI_OEM_EVENT_DATA_SIZE - 1);
	event->EventDataUnion.OemEvent.OemEventData
		[SAHPI_OEM_EVENT_DATA_SIZE - 1] = '\0';

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
                dbg("Couldn't parse Severity from Blade Center Log Entry");
                return -1;
        }
                
        while(start && (strncmp(start,"Source:",7) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Source:%19s",ent.source)) {
                dbg("Couldn't parse Source from Blade Center Log Entry");
                return -1;
        }

        while(start && (strncmp(start,"Name:",5) != 0)) { start++; }
        if(!start) { return -2; }

        if(!sscanf(start,"Name:%19s",ent.sname)) {
                dbg("Couldn't parse Name from Blade Center Log Entry");
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
                dbg("Couldn't parse Date/Time from Blade Center Log Entry");
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

/**
 * snmp_bc_add_to_eventq
 * @hnd: 
 * @thisEvent: 
 * 
 * Return value:	-1 Error encountered - no Event entry created
 			0  SA_OK
 **/

int snmp_bc_add_to_eventq(void *hnd, SaHpiEventT *thisEvent  )
{

        struct oh_event working;
        struct oh_event *e = NULL;
        struct oh_handler_state *handle = hnd;
        SaHpiRptEntryT *rptentry;

        memset(&working, 0, sizeof(struct oh_event));

        rptentry = oh_get_resource_by_id(handle->rptcache, thisEvent->Source);
        if (!rptentry) {
                dbg("Warning: RPT entry not found. Cannot find RDR.\n");
                return -1; /*  No resource found by that id */
        }

        working.type = OH_ET_HPI;
        working.u.hpi_event.parent  = rptentry->ResourceId;
	/* TODO:  working.u.hpi_event.id = rdrId, not to rptId*/
        working.u.hpi_event.id = rptentry->EntryId;
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

