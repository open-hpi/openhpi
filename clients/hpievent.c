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
 *     Racing Guo <racing.guo@intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <SaHpi.h>

static char progname[] = "hpievent";

static const char* st_map[] = {
	"INACTIVE",
	"INSERTION_PENDING",
	"ACTIVE_HEALTHY",
	"ACTIVE_UNHEALTHY",
	"EXTRACTION_PENDING",
	"NOT_PRESENT",
};



static
void PrintEvent(SaHpiEventT *event)
{
	printf("\n------Event------\n");
	printf("Event Source:%d\n", event->Source);
	printf("Event Timestamp:\n");
	printf("Event Severity:%d\n", event->Severity);
	switch (event->EventType) {
		case SAHPI_ET_SENSOR:
			printf("Event Type: Sensor\n");
			break;
			
		case SAHPI_ET_HOTSWAP:
			printf("Event Type: HotSwap\n");
			printf("Hotswap State:%s\n",
				st_map[event->EventDataUnion.HotSwapEvent.HotSwapState]);
			printf("Hotswap Prev State:%s\n",
				st_map[event->EventDataUnion.HotSwapEvent.PreviousHotSwapState]); 
			break;
			
		case SAHPI_ET_WATCHDOG:
			printf("Event Type: Watchdog\n");
			break;
			
		case SAHPI_ET_OEM:
			printf("Event Type: Oem\n");
			break;
			
		case SAHPI_ET_USER:
			printf("Event Type: User\n");
			break;
		default:
			printf("Event Type: Error\n");
	}
}

static
void DoEvent(SaHpiSessionIdT sessionid)
{
	SaHpiEventT event;
	SaErrorT    rv;

	printf("Subscribe\n");
	rv = saHpiSubscribe(sessionid);
	if (rv != SA_OK) {
		printf( "saHpiSubscribe error %d\n",rv);
		return;
	}	
	while(1) {
		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_BLOCK, &event, NULL, NULL, NULL);
		if (rv != SA_OK) { 
			goto out;
		}
		PrintEvent(&event);
	}
out:
	printf( "Unsubscribe\n");
	rv = saHpiUnsubscribe( sessionid );
	return;
}

int
main(int argc, char **argv)
{
	SaHpiVersionT hpiver;
	SaHpiSessionIdT sessionid;
	SaErrorT    rv;

	hpiver = saHpiVersionGet();
	printf("OpenHPI client %s, HPI Version %d\n", progname, hpiver);

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
	if (rv != SA_OK) {
		printf("saHpiSessionOpen error %d\n",rv);
		exit(-1);
	}
	printf("Discovery\n");
  	rv = saHpiDiscover(sessionid);
	printf("please use Ctrl+C to quit\n");
	DoEvent(sessionid);  
	rv = saHpiSessionClose(sessionid);
	return 0;
}
