/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *	pdphan	03/22/04	Initial code
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "SaHpi.h"
#include <printevent_utils.h>
#include "ecode_utils.h"

#define HPI_NSEC_PER_SEC 1000000000LL

char progver[] = "0.1";
int fdebug = 0;
int ftimer = 0;

int main(int argc, char **argv)
{
	int c, wait = 0;
	SaErrorT rv;
	SaHpiVersionT hpiVer;
	SaHpiSessionIdT sessionid;
	SaHpiRptInfoT rptinfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiResourceIdT resourceid;
	SaHpiSelInfoT info;
	SaHpiRdrT rdr;
	SaHpiTimeoutT timeout; 
	SaHpiEventT event;
        
	printf("%s: version %s\n",argv[0],progver); 
        
        while ( (c = getopt( argc, argv,"t:x?")) != EOF )
                switch(c) {
			case 't':
				ftimer = 1;
				wait = atoi(optarg);
				break;
			case 'x': fdebug = 1; break;
			default:
				printf("Usage %s [-tx]\n",argv[0]);
				printf("      -t <value>:wait <value> seconds for event\n");
				printf("      -x	:displays eXtra debug messages\n");
				exit(1);
		}


	if (ftimer) 
		timeout = (SaHpiInt64T)(wait * HPI_NSEC_PER_SEC);  
	else
		timeout = (SaHpiInt64T) SAHPI_TIMEOUT_IMMEDIATE;

        rv = saHpiInitialize(&hpiVer);
        if (rv != SA_OK) {
                printf("saHpiInitialize: %s\n",decode_error(rv));
                exit(-1);
        }
        
	rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n",decode_error(rv));
                exit(-1);
        }
 
        rv = saHpiResourcesDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n",decode_error(rv));
        rv = saHpiRptInfoGet(sessionid,&rptinfo);
        if (fdebug) printf("saHpiRptInfoGet %s\n",decode_error(rv));
        printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
               rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
        
	printf( "Subscribe to events\n");
	rv = saHpiSubscribe( sessionid, (SaHpiBoolT)0 );
	if (rv != SA_OK) return rv;


        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (fdebug) printf("saHpiRptEntryGet %s\n",decode_error(rv));
                if (rv == SA_OK) {
                        resourceid = rptentry.ResourceId;
                        if (fdebug) printf("RPT %x capabilities = %x\n", resourceid,
                                           rptentry.ResourceCapabilities);
                        if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                                if (fdebug) printf("RPT doesn't have SEL\n");
                                rptentryid = nextrptentryid;
                                continue;  /* no SEL here, try next RPT */
                        }
                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 
                        printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);
                        
			/* Using EventLogInfo to build up event queue - for now */
                        rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);
                        if (fdebug) printf("saHpiEventLogInfoGet %s\n",decode_error(rv));
                        if (rv == SA_OK) {
				break;
                        }
                        
                        rptentryid = nextrptentryid;
                }
        }
        
	printf( "Go and get the event\n");
	while (1) {
		rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry );
     		if (rv != SA_OK) { 
			if (rv != SA_ERR_HPI_TIMEOUT) {
	  			printf( "Error during EventGet - Test FAILED\n");
	  			break;
        		} else {
	  			printf( "\n\n****** Time, %d seconds, expired waiting for event.\n", wait);
	  			break;
			}
		} else {
			print_event(sessionid, &event);
		}
     	}

	
	
	/* Unsubscribe to future events */
	printf( "Unsubscribe\n");
	rv = saHpiUnsubscribe( sessionid );

	rv = saHpiSessionClose(sessionid);
        rv = saHpiFinalize();
        
        exit(0);
        return(0);
}
 
/* end hpigetevents.c */
