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
 *     W. David Ashley <dashley@us.ibm.com>
 */

/**********************************************************************
 * NOTES: 
 * All these test cases depend on values defined in rsa_str2event.c and
 * sensor and resource defintions in rsa_resources.c.
 * These are real RSA events and sensors, which hopefully 
 * won't change much.
 * 
 * We assume the Chassis resource ID = 1.
 ***********************************************************************/

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include <SaHpi.h>

#include <snmp_util.h>
#include <sim_resources.h>
#include <printevent_utils.h>

#define ERROR_LOG_MSG_OID ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2.1"
#define CHASSIS_RID 1

int main(int argc, char **argv)
{
        SaErrorT err;
	SaHpiRdrT rdr;
	SaHpiRptEntryT rpt;
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

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

	/************************************************************
	 * TestCase - Mapped Chassis Event (EN_CUTOFF_HI_FAULT_3_35V)
	 ************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System shutoff due to Continuous 3.3V over max allowed.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((logentry.Event.Source == CHASSIS_RID) &&
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

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

	/*************************************************************
	 * TestCase - Non-mapped Event (Severity=INFO)
	 *************************************************************/
	logstr = "Severity:INFO  Source:RSA  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus message not in string to event table";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == CHASSIS_RID)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Non-mapped Event (Severity=INFO)\n");
		print_event(&(logentry.Event));
		return -1;
	}

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

	/* Better to test a mapped login event - don't have one yet */
	/*************************************************************
	 * TestCase - Non-mapped Login Event (Severity=WARN)
	 *************************************************************/
	logstr = "Severity:WARN  Source:SWITCH_4  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus login message Login ID:\'\'myid\' @ someaddress\'";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!((!(logentry.Event.Source == CHASSIS_RID)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_MINOR))) {
		printf("Error! TestCase - Non-mapped Login Event (Severity=WARN)\n");
		print_event(&(logentry.Event));
		return -1;
	}

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

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

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check expected values */
	if (!(((logentry.Event.Source == CHASSIS_RID)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Bogus threshold strings\n");
		print_event(&(logentry.Event));
		return -1;
	}

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

	/******************************************************************
	 * TestCase - Recovery string not first character of text string
         * (blank is first character). Should not treat as a recovery event
	 ******************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text: Recovery System shutoff due to +3.3v over voltage. Read value 3.5 Threshold value 3.4";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/* Check expected values */
	if (!(((logentry.Event.Source == CHASSIS_RID)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - Recovery string not first character of text string\n");
		print_event(&(logentry.Event));
		return -1;
	}

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }

	/******************************************************************
	 * TestCase - In string table but not mapped
         * Uses special defined Test event in rsa_str2event.c
	 ******************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Bogus Test Event.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/* Check expected values */
	if (!(((logentry.Event.Source == CHASSIS_RID)) &&
	      (logentry.Event.EventType == SAHPI_ET_OEM) &&
	      (logentry.Event.Severity == SAHPI_INFORMATIONAL))) {
		printf("Error! TestCase - In string table but not mapped\n");
		print_event(&(logentry.Event));
		return -1;
	}

//	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
//	if (err != SA_OK) {
//		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
//		return -1;
//      }
	
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
