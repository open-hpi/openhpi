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

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <SaHpi.h>

#include <snmp_util.h>
#include <sim_resources.h>
#include <printevent_utils.h>

#define ERROR_LOG_MSG_OID ".1.3.6.1.4.1.2.3.51.2.3.4.2.1.2.1"

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

	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/************************************************************
	 * TestCase - Mapped Chassis Event - EN_CUTOFF_HI_FAULT_3_35V
	 ************************************************************/
	logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System shutoff due to +3.3v over voltage.";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	print_event(&(logentry.Event));
	/* Check severity; RID=1; Non-OEM */

	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/***************************************************************
	 * TestCase - Mapped Blade Threshold Event - EN_IO_5V_WARNING_HI
         ***************************************************************/
	logstr = "Severity:WARN  Source:BLADE_10  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:IO Board +5V over recommended voltage. Read value 5.7 Threshold value 5.50";
	memset(&logentry, 0 , sizeof(SaHpiSelEntryT));
	strcpy(hash_value->value.string, logstr);
	g_hash_table_insert(sim_hash, hash_key, hash_value);

        err = saHpiEventLogEntryGet(sessionid, SAHPI_DEFAULT_DOMAIN_ID, SAHPI_NEWEST_ENTRY,
				    &prev_logid, &next_logid, &logentry, &rdr, &rpt);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogEntryGet: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }
	
	/* Check severity; RID!=1; threshold values; non-oem */
	print_event(&(logentry.Event));

	err = saHpiEventLogClear(sessionid, SAHPI_DEFAULT_DOMAIN_ID);
	if (err != SA_OK) {
		printf("Error! saHpiEventLogClear: line=%d; err=%d\n", __LINE__, err);
		return -1;
        }

	/************************************************************************
	 * TestCase - Recovery Mapped Blade Threshold Event - EN_IO_5V_WARNING_HI
	 * Depends on previous test case
         ************************************************************************/
        



	/* Mapped switch event */
	/* Dup event */
	/* Hot-swap */
	/* Check previous state for sensors & hotswap */
	/* severity 1,2,3 override */
	/* log string */
        /* Recovery event */
	/* Login event */
	/* Non-mapped event */
	/* In string table not mapped - need to create a bogus test event we put in bc_str2event.c 
	   and BC_STRING.MAP */

#if 0
	logentry.Event.Source =
	logentry.Event.EventType =
	logentry.Event.Severity =
	logentry.Event.EventDataUnion.SensorEvent.EventCategory =
	logentry.Event.EventDataUnion.SensorEvent.Assertion =
	logentry.Event.EventDataUnion.SensorEvent.EventState =
	logentry.Event.EventDataUnion.SensorEvent.TriggerReading.Interpreted =
	logentry.Event.EventDataUnion.SensorEvent.TriggerReading.EventStatus =
	logentry.Event.EventDataUnion.SensorEvent.TriggerThreshold.Interpreted =
	logentry.Event.EventDataUnion.SensorEvent.PreviousState =

	logentry.Event.EventDataUnion.HotSwapEvent.HotSwapState = 
	logentry.Event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = 
		
		/* Mapped Event */
		printf("\nMapped Event\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +3.3V Supply.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - mapped case\n");
		}
 	{
		SaHpiEventT event;

		/* bcsrc2rid tests */
		logstr = "Severity:INFO  Source:BLADE_02  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:BLADE_02 test string. Very long string yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyThisareattheend";

		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table case\n");
		}

		logstr = "Severity:INFO  Source:BLADE_14  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:BLADE_14 test string.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table case\n");
		}

		logstr = "Severity:INFO  Source:SWITCH_3  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:SWITCH_3 test string.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table case\n");
		}

		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:SERVPROC test string.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table case\n");
		}

		/* Mapped Recovery Event */
		printf("\nMapped Recovery Event\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Recovery System over recommended voltage on +3.3V Supply.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - mapped case\n");
		}
		/* Recovery but Recovery string not first char */
		printf("\nMapped Recovery but Recovery string not first char\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text: Recovery System over recommended voltage on +3.3V Supply.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - mapped case\n");
		}

		/* Login message */
		printf("\nLogin message\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Login test string for the login value LOGIN ID:'me'";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table case\n");
		}

		/* In string table but not mapped */
		printf("\nIn string table but not mapped\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Power Supply 1 3.3V Fault";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 0, &event_enabled)) {
			printf ("log2event returned err for - Not in string table but not mapped case\n");
		}


		
		/* Unmapped Threshold Event */
		printf("\nUnMapped Threshold Event\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Power Supply 1 3.3V Fault Read value 51.3 Threshold value 50.2";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for - Unmapped Threshold case\n");
		}

		/* Mapped Hot-swap Event */
		printf("\nMapped Hot-swap Event (under V)\n");
		logstr = "Severity:INFO  Source:BLADE_09  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:Blade Server 9 was removed.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for - Unmapped Threshold case\n");
		}
		
		/* Mapped Blade Event */
		printf("\nMapped Blade Event (under V)\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System shutoff due to VRM 1 over voltage.";
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for - Unmapped Threshold case\n");
		}

		/* Dup - find in blade sensor */
		printf("\nDuplicate - EN_PFA_HI_FAULT_2_5V|0x08031480 find in blade sensor\n");
		logstr = "Severity:INFO  Source:BLADE_02  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +2.5v.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for -  Dup - find in blade sensor\n");
		}

		/* Dup - find in chassis sensor */
		printf("\nDuplicate - EN_PFA_HI_FAULT_2_5V|0xFF031480  find in chassis sensor\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +2.5v.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for -  Dup - find in blade sensor\n");
		}
		
		/* Dup - No event found for resource  */ /* tmp remove from bc_resources.c */
		printf("\nDuplicate - EN_PFA_HI_FAULT_3_35V|0xFF033480 No event found for resource\n");
		logstr = "Severity:INFO  Source:SERVPROC  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +3.3v.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1, &event_enabled)) {
			printf ("log2event returned err for -  Dup - find in blade sensor\n");
		}

		printf("DONE WITH TEST\n");

blade
EN_PFA_HI_FAULT_2_5V|0x08031480|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage on +2.5v."
EN_PFA_LO_FAULT_2_5V|0x08031880|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage on +2.5v."
EN_PFA_HI_FAULT_3_35V|0x08033480|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage on +3.3v."
EN_PFA_LO_FAULT_3_35V|0x08033880|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage on +3.3v."
EN_PFA_HI_FAULT_12V_PLANAR|0x06037500|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage for +12v."
EN_PFA_LO_FAULT_12V_PLANAR|0x06037800|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage for +12v."

chassis 
EN_PFA_HI_FAULT_2_5V|0xFF031480|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage on +2.5v."
EN_PFA_LO_FAULT_2_5V|0xFF031880|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage on +2.5v."
EN_PFA_HI_FAULT_3_35V|0xFF033480|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage on +3.3v."
EN_PFA_LO_FAULT_3_35V|0xFF033880|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage on +3.3v."
EN_PFA_HI_FAULT_12V_PLANAR|0xFF037500|ALL|SAHPI_MAJOR|OVR_SEV|"System over recommended voltage for +12v."
EN_PFA_LO_FAULT_12V_PLANAR|0xFF037800|ALL|SAHPI_MAJOR|OVR_SEV|"System under recommended voltage for +12v."

		/* Dup - with RID override */
		printf("\nDup - with RID override\n");
		logstr = "Severity:INFO  Source:BLADE_09  Name:WMN315702424  Date:10/11/03  Time:09:09:46  Text:System over recommended voltage on +2.5v.";
		printf ("Test string = %s\n", logstr);
		if (log2event((void *)GLOBAL_HANDLE, logstr, &event, 1)) {
			printf ("log2event returned err for - Unmapped Threshold case\n");
		}


#endif

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
