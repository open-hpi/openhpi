/*      -*- linux-c -*-
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
#include <unistd.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

/* debug macros */
#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", "error") 

/* Function prototypes */
SaErrorT discover_domain(SaHpiDomainIdT, SaHpiSessionIdT, SaHpiRptEntryT);
int print_wdt(SaHpiWatchdogT wdt);
int test_watchdogtimer(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr);
int test_wdtget(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num);
int test_wdtset(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num);
int test_wdtreset(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num);

/* Global variables */
int g_test = 0;

/**
 * main: main program loop
 *
 *
 * This program discovers all resources and then tests the watchdog
 * timer according to the test case number sent as input.
 *
 * Return value: int 0
 **/
int main(int arc, const char *argv[])
{
        SaErrorT 		err;
        SaHpiVersionT		version;
        SaHpiSessionIdT 	session_id;
        SaHpiRptEntryT		entry;
        
	/* First parameter is number of test case to execute */
	if(argv[1]) {
		g_test=atoi(argv[1]);
	}
	
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

/**
 * print_wdt:
 * @wdt: watchdog timer structure
 *
 * Print the info in wdt to standard output
 *
 * Return value: 0 for success | Error code
 **/
int print_wdt(SaHpiWatchdogT wdt)
{
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

	return 0;
}

/**
 * test_watchdogtimer:
 * @session_id: session id of program
 * @resource_id: resource id with watchdog timer
 * @rdr: RDR for the watchdog timer
 *
 * Test the Get, Set, and Reset functions of the watchdog timer.
 *
 * Return value: 0 for success | Error code
 **/
int test_watchdogtimer(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr)
{
        SaHpiWatchdogNumT wdt_num = rdr.RdrTypeUnion.WatchdogRec.WatchdogNum;

	switch(g_test) {
		case 1:
			printf("*****Test saHpiWatchdogTimerGet()*****\n");
			test_wdtget(session_id, resource_id, rdr, wdt_num);
			break;
		case 2:
			printf("*****Test saHpiWatchdogTimerSet()*****\n");
			test_wdtset(session_id, resource_id, rdr, wdt_num);
			break;
		case 3:
			printf("*****Test saHpiWatchdogTimerReset()*****\n");
			test_wdtreset(session_id, resource_id, rdr, wdt_num);
			break;
		default:
			printf("Usage:\n\tsudo ./test_watchdog 1|2|3\n");

	}

	return 0;
}

/**
 * test_wdtget:
 * @session_id: session id of program
 * @resource_id: resource id with watchdog timer
 * @rdr: RDR for the watchdog timer
 * @wdt_num:  Watchdog timer number
 *
 * Test the Get function of the watchdog timer with the following
 * test cases:
 * - all valid values sent; running = FALSE
 *   ->values should be accepted; wdt should not start running
 * - different valid values sent; running = TRUE
 *   ->values should be accepted; since wdt isn't running, running should be
 *     set to FALSE; wdt should not start running
 *
 * Return value: 0 for success | Error code
 **/
int test_wdtget(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num)
{
	int ret = 0;
	int err;

	SaHpiWatchdogT wdt_allvalid = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_FALSE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 1000, /* 1 second */
        	.PresentCount           = 0
	};
	SaHpiWatchdogT wdt_allvalidruntrue = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_TRUE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 3000, /* 3 seconds */
        	.PresentCount           = 0
	};
	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_allvalid);
	if (SA_OK != err) {
		printf("Error during set of not running wdt\n");
		ret = err;
	}

	err = saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt_allvalid);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerGet() failed\n");
		return err;
	}
	printf("TEST:  Manually verify:\n \
			1. wdt not running (no expiry)\n \
			2. timeout set to 1000msec\n \
			3. .Running is FALSE\n");
	print_wdt(wdt_allvalid);
	sleep(2);

	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_allvalidruntrue);
	if (SA_OK != err) {
		printf("Error during set of not running wdt\n");
		ret = err;
	}

	err = saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt_allvalidruntrue);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerGet() failed\n");
		return err;
	}
	printf("TEST:  Manually verify:\n \
			1. wdt not running (no expiry)\n \
			2. timeout set to 3000msec\n \
			3. .Running is FALSE\n");
	print_wdt(wdt_allvalidruntrue);
	return ret;
}

/**
 * test_wdtset:
 * @session_id: session id of program
 * @resource_id: resource id with watchdog timer
 * @rdr: RDR for the watchdog timer
 * @wdt_num:  Watchdog timer number
 *
 * Test the Set function of the watchdog timer for the following watchdog
 * test cases:
 * - watchdog data with invalid (unsupported parameters)
 *   -> error message should be returned, and parameters should be
 *      overwritten with valid values
 * - .Running = TRUE when watchdog already running
 *   -> watchdog should be reset to the InitialCount
 * - .Running = FALSE when watchdog already running
 *   -> watchdog should be stopped
 * - .InitialCount = 0
 *   -> watchdog should immediately time out
 *
 * Return value: 0 for success | Error code
 **/
int test_wdtset(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num)
{
        SaErrorT err;
	SaHpiWatchdogT wdt;
	SaHpiWatchdogT wdt_bogusparams = {
        	.Log                    = SAHPI_TRUE, /* should cause error */
        	.Running                = SAHPI_TRUE,
        	.TimerUse               = SAHPI_WTU_OS_LOAD,
        	.TimerAction            = SAHPI_WA_POWER_DOWN, /* should cause error */
        	.PretimerInterrupt      = SAHPI_WPI_SMI, /* should cause error */
        	.PreTimeoutInterval     = 10, /* should cause error */
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 10000, /* 10 seconds */
        	.PresentCount           = 0
	};
	SaHpiWatchdogT wdt_runtrue = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_TRUE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 3000, /* 3 seconds */
        	.PresentCount           = 0
	};
	SaHpiWatchdogT wdt_runfalse = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_FALSE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 3000, /* 3 seconds */
        	.PresentCount           = 0
	};
	SaHpiWatchdogT wdt_iczero = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_TRUE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 0, 
        	.PresentCount           = 0
	};
	int ret = 0;

	/* test set with bogus params */
	printf("************TEST 1************\n");
	printf("Data sending:\nTEST:  Ensure error received for Log, Action, Interrupt, and Interval\n");
	print_wdt(wdt_bogusparams);
	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_bogusparams);
	if (SA_OK == err) {
		printf("saHpiWatchdogTimerSet did not return error as expected\n");
		ret = -1;
	}

	printf("Data returned\nTEST:  Ensure invalid values have been changed to valid.:\n");

	saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	print_wdt(wdt);

	/* test setting running = TRUE when watchdog already running */
	printf("************TEST 2***********\n");
	/* startup watchdog */
	saHpiWatchdogTimerReset(session_id, resource_id, wdt_num);

	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_runtrue);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerSet() failed\n");
		return err;
	}
	printf("TEST:  Manually verify wdt expires in 3 (not 10) seconds.\n");
	printf("TEST:  .Running should be SAHPI_TRUE now.\n");
	saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	print_wdt(wdt);
	sleep(4);

	/* test setting running = FALSE when watchdog already running */
	printf("************TEST 3************\n");
	/* watchdog will restart after reboot triggered */
	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_runfalse);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerSet() failed\n");
		return err;
	}
	printf("TEST:  Ensure .Running is false and no expiry after 3 seconds.\n");
	saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	print_wdt(wdt);
	sleep(4);

	/* test with an initial count = 0 */
	printf("************TEST 4************\n");
	/* watchdog should immediately timeout */
	/* first, ensure watchdog is running */
	saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_runtrue);
	saHpiWatchdogTimerReset(session_id, resource_id, wdt_num);

	/* give watchdog initial count of 0 */
	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_iczero);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerSet() failed\n");
		return err;
	}
	printf("TEST:  Ensure wdt immediately expires\n");

	return ret;
}

/**
 * test_wdtreset:
 * @session_id: session id of program
 * @resource_id: resource id with watchdog timer
 * @rdr: RDR for the watchdog timer
 * @wdt_num:  Watchdog timer number
 *
 * Test the reset function of the watchdog timer for the following
 * test cases:
 * - wdt is not running
 *   -> wdt is started; .Running = TRUE
 * - wdt is running
 *   -> wdt is restarted to .InitialCount
 *
 * Return value: 0 for success | Error code
 **/
int test_wdtreset(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id, SaHpiRdrT rdr,
		SaHpiWatchdogNumT wdt_num)
{
	int ret = 0;
	int err;
	SaHpiWatchdogT wdt;

	SaHpiWatchdogT wdt_notrunning = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_FALSE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 1000, /* 1 second */
        	.PresentCount           = 0
	};
	SaHpiWatchdogT wdt_running = {
        	.Log                    = SAHPI_FALSE,
        	.Running                = SAHPI_TRUE,
        	.TimerUse               = SAHPI_WTU_SMS_OS,
        	.TimerAction            = SAHPI_WA_RESET,
        	.PretimerInterrupt      = SAHPI_WPI_NONE,
        	.PreTimeoutInterval     = 0,
        	.TimerUseExpFlags       = 0,
        	.InitialCount           = 3000, /* 3 seconds */
        	.PresentCount           = 0
	};
	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_notrunning);
	sleep(1);

	err = saHpiWatchdogTimerReset(session_id, resource_id, wdt_num);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerReset() failed\n");
		return err;
	}
	printf("TEST:  Manually verify: \n \
			1)wdt running (expiry in 1 sec)\n \
			2).Running = TRUE\n");
	saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	print_wdt(wdt);
	sleep(2);

	err = saHpiWatchdogTimerSet(session_id, resource_id, wdt_num, &wdt_running);
	sleep(1);
	err = saHpiWatchdogTimerReset(session_id, resource_id, wdt_num);
	if (SA_OK != err) {
		printf("saHpiWatchdogTimerReset() failed\n");
		return err;
	}
	printf("TEST:  Manually verify: \n \
			1)wdt restarted (expiry in 3 sec from now)\n \
			2).Running = TRUE\n");
	saHpiWatchdogTimerGet(session_id, resource_id, wdt_num, &wdt);
	print_wdt(wdt);

	return ret;
}
