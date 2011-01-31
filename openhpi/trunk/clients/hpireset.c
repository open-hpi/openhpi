/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Andy Cress <arcress@users.sourceforge.net>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *     10/13/2004  kouzmich   porting to HPI B
 *     09/06/2010  ulikleber  New option -D to select domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <SaHpi.h>

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

#define  uchar  unsigned char
char fdebug = 0;

static void Usage(char *pname)
{
                printf("Usage: %s [-D <n> ] [ -r -d -w -c -n -o -s -x]\n", pname);
                printf(" where -D <n> Select domain\n");
                printf("       -r  hard resets the system\n");
      //          printf("       -d  powers Down the system\n");
                printf("       -w  warm resets the system\n");
      //          printf("       -c  power cycles the system\n");
      //          printf("       -n  sends NMI to the system\n");
      //          printf("       -o  soft-shutdown OS\n");
      //          printf("       -s  reboots to Service Partition\n");
                printf("       -x  show eXtra debug messages\n");
}

int
main(int argc, char **argv)
{
  int c;
  int is_reset = 0;
  SaErrorT rv;
  SaHpiDomainIdT domainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
  SaHpiSessionIdT sessionid;
  SaHpiDomainInfoT domainInfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiResourceIdT resourceid;
  SaHpiResetActionT action = -1;
  //uchar breset;        not really implemented that way!
  //uchar bopt;          not really implemented that way!
  //uchar fshutdown = 0; not really implemented that way!
 
  oh_prog_version(argv[0], OH_SVN_REV);
  //breset = 3; /* hard reset as default */
  //bopt = 0;    /* Boot Options default */
  while ( (c = getopt( argc, argv,"D:rdwconsxq?")) != EOF )
     switch(c) {
          case 'D':
                if (optarg) domainid = atoi(optarg);
                else {
                     printf("hpipower: option requires an argument -- D");
		     Usage(argv[0]);
                }
                break;
    //      case 'd': breset = 0;     break;  /* power down */
          case 'r':
    //    	breset = 3;
	      	action = SAHPI_COLD_RESET;
     		break;  /* hard reset */
	  case 'w':
		action = SAHPI_WARM_RESET;
		break;
          case 'x': fdebug = 1;     break;  /* debug messages */
          case 'q': fdebug = 42;    break;  /* just for testing, will exit before any reset */
    //      case 'c': breset = 2;     break;  /* power cycle */
    //      case 'o': fshutdown = 1;  break;  /* shutdown OS */
    //      case 'n': breset = 4;     break;  /* interrupt (NMI) */
    //      case 's': bopt   = 1;     break;  /* hard reset to svc part */
          default:
		Usage(argv[0]);
                exit(1);
  }
    // if (fshutdown) breset = 5;     /* soft shutdown option */

  if (fdebug) {
	if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) printf("saHpiSessionOpen\n");
	else printf("saHpiSessionOpen to domain %u\n",domainid);
  }
  rv = saHpiSessionOpen(domainid, &sessionid, NULL);
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
     if (rv != SA_OK) printf("saHpiRptEntryGet: rv = %d\n",rv);
     if (rv == SA_OK) {
	/* walk the RDR list for this RPT entry */
	entryid = SAHPI_FIRST_ENTRY;
	resourceid = rptentry.ResourceId;
	rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	printf("rptentry[%u] resourceid=%u tag: %s\n",
		entryid,resourceid, rptentry.ResourceTag.Data);
        if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RESET) {
		is_reset = 1;

		/* Allow debugging the beginning of program without reset */
		if (fdebug == 42) exit (42);

		rv1 = saHpiResourceResetStateSet(sessionid, 
	     		resourceid, action);
	     		//resourceid, SAHPI_POWER_OFF);
        	printf("ResetStateSet status = %d...requested %d\n",rv1, action);
	}
     }
     rptentryid = nextrptentryid;
  }
 
  rv = saHpiSessionClose(sessionid);

	if (is_reset == 0)
		printf("No resources with Reset capability found\n");

  return(0);
}
 
/* end hpireset.c */
