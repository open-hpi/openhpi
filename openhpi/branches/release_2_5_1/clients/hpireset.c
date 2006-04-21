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
 *     Andy Cress <arcress@users.sourceforge.net>
 * Changes:
 *     10/13/2004  kouzmich   porting to HPI B
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
                printf("Usage: %s [-r -d -x]\n", pname);
                printf(" where -r  hard Resets the system\n");
                printf("       -d  powers Down the system\n");
                printf("       -c  power Cycles the system\n");
                printf("       -n  sends NMI to the system\n");
                printf("       -o  soft-shutdown OS\n");
                printf("       -s  reboots to Service Partition\n");
                printf("       -x  show eXtra debug messages\n");
}

int
main(int argc, char **argv)
{
  int c;
  int is_reset = 0;
  SaErrorT rv;
  SaHpiSessionIdT sessionid;
  SaHpiDomainInfoT domainInfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiEntryIdT nextentryid;
  SaHpiResourceIdT resourceid;
  SaHpiRdrT rdr;
  SaHpiCtrlNumT   ctlnum;
  SaHpiResetActionT resetact;
  int j;
  uchar breset;
  uchar bopt;
  uchar fshutdown = 0;
  SaHpiCtrlStateT ctlstate;
 
  printf("%s ver %s\n", argv[0],progver);
  breset = 3; /* hard reset as default */
  bopt = 0;    /* Boot Options default */
  while ( (c = getopt( argc, argv,"rdconsx?")) != EOF )
     switch(c) {
          case 'd': breset = 0;     break;  /* power down */
          case 'r': breset = 3;     break;  /* hard reset */
          case 'x': fdebug = 1;     break;  /* debug messages */
          case 'c': breset = 2;     break;  /* power cycle */
          case 'o': fshutdown = 1;  break;  /* shutdown OS */
          case 'n': breset = 4;     break;  /* interrupt (NMI) */
          case 's': bopt   = 1;     break;  /* hard reset to svc part */
          default:
		Usage(argv[0]);
                exit(1);
  }
  if (fshutdown) breset = 5;     /* soft shutdown option */

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
  printf("DomainInfo: UpdateCount = %x, UpdateTime = %lx\n",
         domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
     rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
     if (rv != SA_OK) printf("RptEntryGet: rv = %d\n",rv);
     if (rv == SA_OK) {
	/* walk the RDR list for this RPT entry */
	entryid = SAHPI_FIRST_ENTRY;
	resourceid = rptentry.ResourceId;
	rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	printf("rptentry[%d] resourceid=%d tag: %s\n",
		entryid,resourceid, rptentry.ResourceTag.Data);
	while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY) &&
		rptentry.ResourceCapabilities & SAHPI_CAPABILITY_POWER)
	{
		is_reset = 1;
		rv = saHpiRdrGet(sessionid,resourceid,
				entryid,&nextentryid, &rdr);
  		if (fdebug) printf("saHpiRdrGet[%d] rv = %d\n",entryid,rv);
		if (rv == SA_OK) {
		   if (rdr.RdrType == SAHPI_CTRL_RDR) { 
			/*type 1 includes Cold Reset Control */
			ctlnum = rdr.RdrTypeUnion.CtrlRec.Num;
			rdr.IdString.Data[rdr.IdString.DataLength] = 0;
			if (fdebug) printf("Ctl[%d]: %d %d %s\n",
				ctlnum, rdr.RdrTypeUnion.CtrlRec.Type,
				rdr.RdrTypeUnion.CtrlRec.OutputType,
				rdr.IdString.Data);
			if ((rdr.RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL) &&
			   (rdr.RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_GENERIC))
			{  /* This is the Reset control */
			   printf("RDR[%d]: %d,%d %s\n",
				rdr.RecordId, 
				rdr.RdrTypeUnion.CtrlRec.Type,
				rdr.RdrTypeUnion.CtrlRec.OutputType,
				rdr.IdString.Data);
			   rv = saHpiResourceResetStateGet(sessionid, 
					resourceid, &resetact);
			   printf("ResetStateGet status = %d, act=%d\n",
					rv,resetact);
			   rv = saHpiResourceResetStateSet(sessionid, 
					resourceid, resetact);
			   printf("ResetStateSet status = %d\n",rv);

			   ctlstate.Type = SAHPI_CTRL_TYPE_DIGITAL;
			   if (breset == 0) {  /*power off*/ 
			     ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_OFF;
			     printf("Powering down ... \n");
			   } else {
			     ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_ON;
			     printf("Resetting ... \n");
			   }
			   rv = saHpiControlSet(sessionid, resourceid, ctlnum,
			   	SAHPI_CTRL_MODE_AUTO, &ctlstate);
			   printf("Reset status = %d\n",rv);
			   break;
			}
		    }
		    j++;
		    entryid = nextentryid;
		}
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
