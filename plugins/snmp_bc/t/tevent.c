/* -*- linux-c -*-
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
 *     Steve Sherman <stevees@us.ibm.com>
 */

/**********************************************************************
 * NOTES: 
 * All these test cases depend on values defined in bc_str2event.c and
 * sensor and resource defintions in bc_resources.c.
 * These are real BladeCenter events and sensors, which hopefully 
 * won't change much.
 ***********************************************************************/

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <SaHpi.h>

#include <snmp_util.h>
#include <sim_resources.h>
#include <printevent_utils.h>

#define ERROR_LOG_MSG_OID ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.2.1"

int main(int argc, char **argv)
{
        SaErrorT err;
	SaHpiRdrT rdr;
	SaHpiEntryIdT rptid, next_rptid;
	SaHpiRptEntryT rpt;
	SaHpiResourceIdT chassis_rid=0;
	SaHpiSelEntryT logentry;
	SaHpiSelEntryIdT prev_logid, next_logid;
        SaHpiSessionIdT sessionid;
        SaHpiVersionT hpiVer;
	char *hash_key, *logstr;
	SnmpMibInfoT *hash_value;

	/* Setup Infra-structure */
        err = saHpiInitialize(&hpiVer);
        if (err != SA_OK) {
                printf("Error! saHpiInitialize: err=%d\n", err);
                return -1;
        }

        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &sessionid, NULL);
        if (err != SA_OK) {
	  printf("Error! saHpiSessionOpen: err=%d\n", err);
	  return -1;
        }
 
        err = saHpiResourcesDiscover(sessionid);
        if (err != SA_OK) {
	  printf("Error! saHpiResourcesDiscover: err=%d\n", err);
	  return -1;
        }

	/* Find first SEL capable Resource - assume its the chassis */
	rptid = SAHPI_FIRST_ENTRY;
	while ((err == SA_OK) && (rptid != SAHPI_LAST_ENTRY))
	{
		err = saHpiRptEntryGet(sessionid, rptid, &next_rptid, &rpt);
		if (err != SA_OK) {
			printf("Error! saHpiRptEntryGet: err=%d\n", err);
			return -1;
		}
		
		if ((rpt.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
			chassis_rid = rpt.ResourceId;
			break;
		}
		else {
			rptid = next_rptid;
			continue;
		}
	}
	if (chassis_rid == 0) {
		printf("Error! saHpiRptEntryGet couldn't find SEL RID\n");
		return -1;
	}

	/* If test OID not already in sim hash table; create it */
	if (!g_hash_table_lookup_extended(sim_hash, 
					  ERROR_LOG_MSG_OID,
					  (gpointer)&hash_key, 
					  (gpointer)&hash_value)) {

		hash_key = g_strdup(ERROR_LOG_MSG_OID);
		if (!hash_key) {
			printf("Error: Cannot allocate memory for oid key=%s\n", ERROR_LOG_MSG_OID);
			return -1;
		}
		
		hash_value = g_malloc0(sizeof(SnmpMibInfoT));
		if (!hash_value) {
			printf("Cannot allocate memory for hash value for oid=%s", ERROR_LOG_MSG_OID);
			return -1;
		}
	}

	printf("Calling Clear\n");

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	printf("Calling EntryGet\n");

	/************************************************************
	 * TestCase - Mapped Chassis Event (EN_CUTOFF_HI_FAULT_3_35V)
         * Event recovered in next testcase
	 ************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System shutoff due to +3.3v over voltage. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((logentry.Event.Source == chassis_rid) &&
	      (logentry.Event.EventType == SAHPI_ET_SENSOR) &&
	      (logentry.Event.Severity == SAHPI_CRITICAL) &&
	      (logentry.Event.EventDataUnion.SensorEvent.SensorType == SAHPI_VOLTAGE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.Assertion == SAHPI_TRUE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_CRIT) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState == SAHPI_ES_UNSPECIFIED) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32 == (float)3.5) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32 == (float)3.4))) {
		printf("Error! TestCase - Mapped Chassis Event (EN_CUTOFF_HI_FAULT_3_35V)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/*************************************************************
	 * TestCase - Chassis Recovery Event (EN_CUTOFF_HI_FAULT_3_35V)
         * Recover event in previous testcase
	 *************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Recovery System shutoff due to +3.3v over voltage. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((logentry.Event.Source == chassis_rid) &&
	      (logentry.Event.EventType == SAHPI_ET_SENSOR) &&
	      (logentry.Event.Severity == SAHPI_CRITICAL) &&
	      (logentry.Event.EventDataUnion.SensorEvent.SensorType == SAHPI_VOLTAGE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.Assertion == SAHPI_FALSE) &&
	      (!(logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_CRIT)) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_CRIT) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32 == (float)3.5) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32 == (float)3.4))) {
		printf("Error! TestCase - Chassis Recovery Event (EN_CUTOFF_HI_FAULT_3_35V)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/*************************************************************
	 * TestCase - Chassis Duplicate Event ( EN_PFA_HI_FAULT_3_35V)
         * Previous state check depends on previous testcase!
	 *************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +3.3v. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((logentry.Event.Source == chassis_rid) &&
	      (logentry.Event.EventType == SAHPI_ET_SENSOR) &&
	      (logentry.Event.Severity == SAHPI_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.SensorType == SAHPI_VOLTAGE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.Assertion == SAHPI_TRUE) &&
	      (!(logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_CRIT)) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MINOR) &&
	      (!(logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_CRIT)) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32 == (float)3.5) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32 == (float)3.4))) {
		printf("Error! TestCase - Chassis Duplicate Event ( EN_PFA_HI_FAULT_3_35V)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/*************************************************************
	 * TestCase - Blade Duplicate Event ( EN_PFA_HI_FAULT_3_35V)
         * Same as previous testcase only for the blade not chassis
	 *************************************************************/
	logstr = "Severity:INFO  Source:BLADE_10  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +3.3v. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_SENSOR) &&
	      (logentry.Event.Severity == SAHPI_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.SensorType == SAHPI_VOLTAGE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.Assertion == SAHPI_TRUE) &&
	      (!(logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_CRIT)) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState == SAHPI_ES_UNSPECIFIED) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32 == (float)3.5) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32 == (float)3.4))) {
		printf("Error! TestCase - Blade Duplicate Event ( EN_PFA_HI_FAULT_3_35V)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/*************************************************************
	 * TestCase - Non-mapped Event (Severity=INFO)
	 *************************************************************/
	logstr = "Severity:INFO  Source:BLADE_01  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus message not in string to event table";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Non-mapped Event (Severity=INFO)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/* Better to test a mapped login event - don't have one yet */
	/*************************************************************
	 * TestCase - Non-mapped Login Event (Severity=WARN)
	 *************************************************************/
	logstr = "Severity:WARN  Source:SWITCH_4  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus login message Login ID:\'\'myid\' @ someaddress\'";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_MINOR))) {
		printf("Error! TestCase - Non-mapped Login Event (Severity=WARN)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/************************************************************
	 * TestCase - Daughter Card Event (EN_PFA_HI_OVER_TEMP_DASD1)
	 ************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:BSE Option over recommended temperature.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_SENSOR) &&
	      (logentry.Event.Severity == SAHPI_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.SensorType == SAHPI_TEMPERATURE) &&
	      (logentry.Event.EventDataUnion.SensorEvent.Assertion == SAHPI_TRUE) &&
	      (!(logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_CRIT)) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MAJOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.EventState & SAHPI_ES_UPPER_MINOR) &&
	      (logentry.Event.EventDataUnion.SensorEvent.PreviousState == SAHPI_ES_UNSPECIFIED) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted.Value.SensorFloat32 == (float)0) &&
	      (logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted.Value.SensorFloat32 == (float)0))) {
		printf("Error! TestCase - Daughter Card Event (EN_PFA_HI_OVER_TEMP_DASD1)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/*************************************************************
	 * TestCase - Hot-swap switch installed (EN_SWITCH_3_INSTALLED) 
	 *************************************************************/
	logstr = "Severity:INFO  Source:SWITCH_3  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:I/O module 3 was installed.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_HOTSWAP) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL) &&
	      (logentry.Event.EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_ACTIVE_HEALTHY) && 
	      (logentry.Event.EventDataUnion.HotSwapEvent.PreviousHotSwapState == SAHPI_HS_STATE_ACTIVE_HEALTHY))) {
		printf("Error! TestCase - Hot-swap switch installed (EN_SWITCH_3_INSTALLED)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/****************************************************************
	 * TestCase - Hot-swap Media Tray removal (EN_MEDIA_TRAY_REMOVED)
         * This event is recovered in the next testcase
	 ****************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:The media tray was removed.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_HOTSWAP) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL) &&
	      (logentry.Event.EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_NOT_PRESENT) && 
	      (logentry.Event.EventDataUnion.HotSwapEvent.PreviousHotSwapState == SAHPI_HS_STATE_ACTIVE_HEALTHY))) {
		printf("Error! TestCase - Hot-swap Media Tray removal (EN_MEDIA_TRAY_REMOVED)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/****************************************************************
	 * TestCase - Hot-swap Media Tray recovery (EN_MEDIA_TRAY_REMOVED)
         * Recovery of previous event
	 ****************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Recovery The media tray was removed.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_HOTSWAP) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL) &&
	      (logentry.Event.EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_ACTIVE_HEALTHY) &&
	      (logentry.Event.EventDataUnion.HotSwapEvent.PreviousHotSwapState == SAHPI_HS_STATE_NOT_PRESENT))) {
		printf("Error! TestCase - Hot-swap Media Tray recovery (EN_MEDIA_TRAY_REMOVED)\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/************************************
	 * Drive some error paths in the code
         ************************************/ 

	/******************************************************************
	 * TestCase - Bogus threshold strings
 	 ******************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System shutoff due to +3.3v over voltage. Bogus Read value 3.5 Bogus Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!(((logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Bogus threshold strings\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/******************************************************************
	 * TestCase - Recovery string not first character of text string
         * (blank is first character). Should not treat as a recovery event
	 ******************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text: Recovery System shutoff due to +3.3v over voltage. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/* Check expected values */
	if (!(((logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Recovery string not first character of text string\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/******************************************************************
	 * TestCase - In string table but not mapped
         * Uses special defined Test event in bc_str2event.c
	 ******************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus Test Event.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, chassis_rid, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/* Check expected values */
	if (!(((logentry.Event.Source == chassis_rid)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - In string table but not mapped\n");
		print_event(&(logentry.Event));
		return -1;
	}

	err = saHpiEventLogClear(sessionid, chassis_rid);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/****************** 
	 * End of testcases 
         ******************/

        err = saHpiSessionClose(sessionid);
        if (err != SA_OK) {
	  printf("Error! saHpiSessionClose: err=%d\n", err);
	  return -1;
        }

        err = saHpiFinalize();
        if (err != SA_OK) {
	  printf("Error! saHpiFinalize: err=%d\n", err);
	  return -1;
        }

        return 0;
}
