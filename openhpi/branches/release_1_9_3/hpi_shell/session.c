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
#include "hpi_cmd.h"
#include "resource.h"
#include "printevent_utils.h"

SaHpiSessionIdT sessionid;
static pthread_t ge_thread;
int prt_flag = 0;

static void* get_event(void *unused)
{
	SaHpiEventT	event;
	SaErrorT	rv;        
	
	
	rv = saHpiSubscribe(sessionid, SAHPI_FALSE);
	if (rv != SA_OK) {
		printf("OpenHPI>Fail to Subscribe event\n");
		return (void *)0;
	}	
		
	for(;;){
		memset(&event, 0xF, sizeof(event));

		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_BLOCK, &event, NULL, NULL);		
		if (rv != SA_OK ) {
			goto out;
		}
		
		if ( prt_flag == 1 ) 
			print_event(sessionid,&event);
	} /*the loop for retrieving event*/

out:
	printf( "Unsubscribe\n");
	rv = saHpiUnsubscribe( sessionid );

	return (void *)1;
}

int open_session()
{
	SaErrorT rv;
	SaHpiVersionT hpiVer;

	rv = saHpiInitialize(&hpiVer);
	if (rv != SA_OK) {
		printf("saHpiInitialize error %d\n", rv);
		return -1;
	}

	rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
     		printf("saHpiSessionOpen error %d\n", rv);
		return -1;
	}
	rv = saHpiResourcesDiscover(sessionid);
	if (rv != SA_OK) 
		printf("saHpiResourcesDiscover rv = %d\n", rv);

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
	rv = saHpiFinalize();
	 if (rv != SA_OK) {
                printf("saHpiFinalize error %d\n", rv);
                return -1;
        }
	return 0;
}
