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
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "SaHpi.h"

#define  uchar  unsigned char
char *progver  = "1.0";
char fdebug = 0;

static void Usage(char *pname)
{
                printf("Usage: %s -pdc [-x]\n", pname);
                printf(" where: -p  powers Up the system\n");
                printf("        -d  powers Down the system\n");
		printf("        -c  powers Cycles the system\n");
                printf("        -x  show eXtra debug messages\n");
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
  SaHpiPowerStateT state = SAHPI_POWER_ON;
  SaHpiPowerStateT state_old;
 
  printf("%s ver %s\n", argv[0],progver);

  while ( (c = getopt( argc, argv,"pdcx?")) != EOF )
     switch(c) {
          case 'p':	/* power up */
	  	state = SAHPI_POWER_ON;
		is_state = 1;
		break;
	  case 'd':	/* power down */
	  	state = SAHPI_POWER_OFF;
		is_state = 1;
		break;
	  case 'c':	/* power cycle */
	  	state = SAHPI_POWER_CYCLE;
		is_state = 1;
		break;
          case 'x':	/* debug messages */
	  	fdebug = 1;
		break;
          default:
		Usage(argv[0]);
                exit(1);
  }
  if ( ! is_state) {
	Usage(argv[0]);
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
  if (fdebug) printf("saHpiDiscover rv = %d\n",rv);
  rv = saHpiDomainInfoGet(sessionid, &domainInfo);
  if (fdebug) printf("saHpiDomainInfoGet rv = %d\n",rv);
  printf("RptInfo: UpdateCount = %x, UpdateTime = %lx\n",
         domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
     SaErrorT rv1;
     rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
     if (rv != SA_OK) printf("RptEntryGet: rv = %d\n",rv);
     else {
	/* walk the RDR list for this RPT entry */
	resourceid = rptentry.ResourceId;
	rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	printf("rptentry[%d] resourceid=%d tag: %s\n",
		rptentryid,resourceid, rptentry.ResourceTag.Data);
	if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_POWER) {
		is_power = 1;
		/* read the current power state */
		rv1 = saHpiResourcePowerStateGet(sessionid, resourceid, &state_old);
		if (rv1 != SA_OK) {
			printf("saHpiResourcePowerStateGet: error = %d\n", rv1);
			rptentryid = nextrptentryid;
			continue;
		} else {
			printf("Current power state: %d\n", state_old);
		};
		/* set new power state */
		printf("Set power state    : %d\n", state);
		rv1 = saHpiResourcePowerStateSet(sessionid, resourceid, state);
		if (fdebug) printf("PowerStateSet status = %d\n",rv1);
		/* setting control */
		rv1 = saHpiResourcePowerStateGet(sessionid, resourceid, &state_old);
		if (rv1 != SA_OK)
			printf("saHpiResourcePowerStateGet: error = %d\n", rv1);
		else
			printf("New power state    : %d\n", state_old);
	};	
	rptentryid = nextrptentryid;
     }
  }
 
  rv = saHpiSessionClose(sessionid);

	if (is_power == 0)
		printf("No resources with Power capability found\n");
  return(0);
}
 
/* end hpireset.c */
