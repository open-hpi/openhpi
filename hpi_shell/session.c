/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Racing Guo <racing.guo@intel.com>
 *     Aaron  Chen <yukun.chen@intel.com>
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include "hpi_cmd.h"
#include "resource.h"

SaHpiSessionIdT sessionid;
static pthread_t ge_thread;
int prt_flag = 0;

static void* get_event(void *unused)
{
	SaHpiEventT	event;
	SaErrorT	rv;        

	rv = saHpiSubscribe(sessionid);
	if (rv != SA_OK) {
		printf("OpenHPI>Fail to Subscribe event\n");
		return (void *)0;
	}	
	
	for(;;) {
		memset(&event, 0xF, sizeof(event));

		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_BLOCK, &event, NULL, NULL, NULL);		
		if (rv != SA_OK ) {
			saHpiUnsubscribe(sessionid);
			return (void *)1;
		}
		if ( prt_flag == 1 )
			oh_print_event(&event, 1);
	} /*the loop for retrieving event*/
	return (void *)1;
}

int open_session()
{
	SaErrorT rv;

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
     		printf("saHpiSessionOpen error %d\n", rv);
		return -1;
	}
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) 
		printf("saHpiDiscover rv = %d\n", rv);

	printf("Initial discovery done\n");	
	printf("\tEnter a command or \"help\" for list of commands\n");

	pthread_create(&ge_thread, NULL, get_event, NULL);
	return 0;
}

int close_session()
{
	SaErrorT rv;

	pthread_kill(ge_thread, SIGKILL);
	
	rv = saHpiSessionClose(sessionid);
	if (rv != SA_OK) {
                printf("saHpiSessionClose error %d\n", rv);
                return -1;
        }
	return 0;
}
