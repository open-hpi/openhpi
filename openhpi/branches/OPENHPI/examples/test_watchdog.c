/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Unknown (initial author of list_resources.c)
 *     Julie Fleischer <julie.n.fleischer@intel.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include <SaHpi.h>
#include <unistd.h>
#include <string.h>
#include <epath_utils.h>

/* debug macros */
#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", "error") 

/* Function prototypes */
SaErrorT discover_domain(SaHpiDomainIdT, SaHpiSessionIdT, SaHpiRptEntryT);
int test_watchdogtimer(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr);

/**
 * main: main program loop
 *
 *
 *
 * Return value: int 0
 **/
int main(int arc, const char *argv[])
{
        SaErrorT 		err;
        SaHpiVersionT		version;
        SaHpiSessionIdT 	session_id;
        SaHpiRptEntryT		entry;
        
	
	/* First step in HPI and openhpi */
        err = saHpiInitialize(&version);
        if (SA_OK != err) {
                error("saHpiInitialize", err);
                exit(-1);
        }
        
	/* Every domain requires a new session */
	/* This example is for one domain, one session */
        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                error("saHpiSessionOpen", err);
                return err;
        }
        
        err = discover_domain(SAHPI_DEFAULT_DOMAIN_ID, session_id, entry);
        if (SA_OK != err) {
                warn("an error was encountered, results may be incomplete");
        }

        err = saHpiFinalize();

        if (SA_OK != err) {
                error("saHpiFinalize", err);
                exit(-1);

	}
	return 0;
}

SaErrorT discover_domain(SaHpiDomainIdT domain_id, SaHpiSessionIdT session_id, SaHpiRptEntryT entry)
{
	
	SaErrorT	err;
	SaHpiRptInfoT	rpt_info_before;
	SaHpiEntryIdT	current;
	SaHpiEntryIdT   next;

        err = saHpiResourcesDiscover(session_id);
        if (SA_OK != err) {
                error("saHpiResourcesDiscover", err);
                return err;
        }
        
        /* grab copy of the update counter before traversing RPT */
        err = saHpiRptInfoGet(session_id, &rpt_info_before);
        if (SA_OK != err) {
                error("saHpiRptInfoGet", err);
                return err;
        }
        
        warn("Scanning RPT...");
        next = SAHPI_FIRST_ENTRY;
        do {
                current = next;
                err = saHpiRptEntryGet(session_id, current, &next, &entry);
                if (SA_OK != err) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                error("saHpiRptEntryGet", err);
                                return err;
                        } else {
                                warn("Empty RPT\n");
                                break;
                        }
                }

        	SaErrorT             err;
        	SaHpiEntryIdT        current_rdr;
        	SaHpiEntryIdT        next_rdr;
        	SaHpiRdrT            rdr;
	
        	next_rdr = SAHPI_FIRST_ENTRY;
        	do {
                	current_rdr = next_rdr;
                	err = saHpiRdrGet(session_id, entry.ResourceId, current_rdr,
                                	&next_rdr, &rdr);
                	if (SA_OK != err) {
                        	if (current_rdr == SAHPI_FIRST_ENTRY)
                                	printf("Empty RDR table\n");
                        	else
                                	error("saHpiRdrGet", err);
                        	return -1;
                	}

        		if (rdr.RdrType == SAHPI_WATCHDOG_RDR) {
				test_watchdogtimer(session_id, entry.ResourceId, rdr);
			}
		}while(next_rdr != SAHPI_LAST_ENTRY);

        } while (next != SAHPI_LAST_ENTRY);

        return SA_OK;
}

int test_watchdogtimer(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr)
{
        SaHpiWatchdogT wdt;
        SaHpiWatchdogNumT wdt_num;
        SaErrorT err;

	wdt_num = rdr.RdrTypeUnion.WatchdogRec.WatchdogNum;

	err = saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerGet() failed\n");
		return err;
	}
	
	printf("\tWatchdog timer data:\n");
	printf("\t\tLog %s\n", (SAHPI_TRUE==wdt.Log)? "TRUE" : "FALSE");
	printf("\t\tRunning %s\n", (SAHPI_TRUE==wdt.Running)? "TRUE" : "FALSE");
	printf("\t\tTimerUse %s\n", (SAHPI_WTU_SMS_OS==wdt.TimerUse)? 
						"SAHPI_WTU_SMS_OS" : "OTHER");
	printf("\t\tTimerAction %s\n", (SAHPI_WA_RESET==wdt.TimerAction)? 
						"RESET" : "OTHER");
	printf("\t\tPretimerInterrupt %s\n", 
		(SAHPI_WPI_NONE==wdt.PretimerInterrupt)? "NONE" : "OTHER");
	printf("\t\tPreTimeoutInterval %d\n", wdt.PreTimeoutInterval);
	printf("\t\tIntitialCount %d\n", wdt.InitialCount);
	printf("\t\tPresentCount %d\n", wdt.PresentCount);

	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerSet failed\n");
		SaHpiWatchdogT wdreset = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_FALSE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 120000, //120 seconds
        	.PresentCount           = 0
		};

		if (SA_OK != saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdreset)) {
			printf("saHpiWatchdogTimerSet() failed again\n");
		}

		return err;
	}

	err = saHpiWatchdogTimerReset(session_id, resource_id, wdt_num);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerReset failed\n");
		return err;
	}

	return 0;
}
