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
 * Changes:
 *     11/03/2004  kouzmich   change timeout type
 *     11/22/2004  kouzmich   calling oh_print_event instead of PrintEvent function
 *                            remove PrintEvent function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <SaHpi.h>
#include <oh_utils.h>

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
		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_IMMEDIATE, &event,
			NULL, NULL, NULL);
		if (rv != SA_OK) {
			if (rv == SA_ERR_HPI_TIMEOUT) { 
				rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_BLOCK, &event,
					NULL, NULL, NULL);
				if (rv != SA_OK)
					break;
			}
		}
		oh_print_event(&event, 1);
	};
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
	printf("OpenHPI client %s, HPI Version %d\n", argv[0], hpiver);

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
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
