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
 *     Andy Cress       <arcress@users.sourceforge.net>
 *     Louis Zhuang     <louis.zhuang@linux.intel.com>
 * Changes:
 *     10/14/2004  kouzmich   porting to HPI B
 *     10/27/2004  Tariq Shureih <tariq.shureih@intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "SaHpi.h"

#define  uchar  unsigned char
char *progver  = "1.0";

void state2str(SaHpiPowerStateT state);

static void usage(char *pname)
{
	printf("Usage: %s -pdc\n", pname);
        printf(" where: -u  powers Up the system\n");
        printf("        -d  powers Down the system\n");
	printf("        -c  powers Cycles the system\n");
}

int
main(int argc, char **argv)
{
	int c;
	int is_state = 0;
	int is_power = 0;
	SaErrorT rv;
	SaHpiSessionIdT sessionid;
	SaHpiDomainInfoT domainInfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiResourceIdT resourceid;
	SaHpiPowerStateT in_state = 0;
	SaHpiPowerStateT current_state;

	printf("%s ver %s\n", argv[0],progver);

	while ( (c = getopt( argc, argv,"udcx?")) != EOF )
		switch(c) {
			case 'u':	/* power up */
				in_state = SAHPI_POWER_ON;
				is_state = 1;
				break;

			case 'd':	/* power down */
				in_state = SAHPI_POWER_OFF;
				is_state = 1;
				break;
			case 'c':	/* power cycle */
				in_state = SAHPI_POWER_CYCLE;
				is_state = 1;
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	if ( ! is_state) {
		usage(argv[0]);
		exit(1);
	};

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
		if (rv == SA_ERR_HPI_ERROR)
			printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
		else
			printf("saHpiSessionOpen error %d\n",rv);
			exit(-1);
	}
	
  	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK)
	      printf("saHpiDiscover rv = %d\n",rv);

	rv = saHpiDomainInfoGet(sessionid, &domainInfo);
	if (rv != SA_OK)
		printf("saHpiDomainInfoGet rv = %d\n",rv);

	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {
		
		SaErrorT rv1;
		
		rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if (rv != SA_OK)
		       printf("RptEntryGet: rv = %d\n",rv);
		else {
			/* walk the RDR list for this RPT entry */
			resourceid = rptentry.ResourceId;
			rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
			printf("rptentry[%d] resourceid=%d tag: %s\n",
					rptentryid,resourceid, (char *)rptentry.ResourceTag.Data);
			
			if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_POWER) {
				is_power = 1;
				
				/* read the current power state */
				rv1 = saHpiResourcePowerStateGet(sessionid, resourceid,
								&current_state);
				if (rv1 != SA_OK) {
					printf("saHpiResourcePowerStateGet: error = %d\n", rv1);
					rptentryid = nextrptentryid;
					continue;
				} 

				printf("Current power state:");
				state2str(current_state);
					
				
				/* set new power state */
				printf("Setting power state to:");
				state2str(in_state);

				rv1 = saHpiResourcePowerStateSet(sessionid, resourceid, in_state);
				if (rv1 != SA_OK)
				       printf("PowerStateSet status = %d\n",rv1);

				/* check new state again */
				rv1 = saHpiResourcePowerStateGet(sessionid, resourceid,
								&current_state);
				if (rv1 != SA_OK)
					printf("saHpiResourcePowerStateGet: error = %d\n", rv1);
				else
					printf("New power state:");
					state2str(current_state);
				};	

			rptentryid = nextrptentryid;

		}
	}

	 rv = saHpiSessionClose(sessionid);

	 if (is_power == 0)
		 printf("No resources with Power capability found\n");

	 return(0);
}

void state2str(SaHpiPowerStateT state)
{
	if (state == SAHPI_POWER_ON)
		printf("\tSAHPI_POWER_ON\n");
	if (state == SAHPI_POWER_OFF)
		printf("\tSAHPI_POWER_OFF\n");
	if (state != SAHPI_POWER_ON && state != SAHPI_POWER_OFF)
		printf("\tInvalid Power State\n");
}
	
 
/* end hpipower2.c */
