/* -*- linux-c -*-
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2004
 *
 * Author(s):
 *     Andy Cress  arcress@users.sourceforge.net
 *     Sean Dague <http://dague.net/sean>
 *
 */
/*
Copyright (c) 2003, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

  a.. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
  b.. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
  c.. Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SaHpi.h>
#include <oh_utils.h>


char progver[] = "1.3 HPI B";
int fdebug = 0;
int fclear = 0;

int main(int argc, char **argv)
{
        int c;
        SaErrorT rv;
        SaHpiVersionT hpiVer;
        SaHpiSessionIdT sessionid;
        
        /* Domain */
        SaHpiDomainInfoT domainInfo;

        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;
        SaHpiEventLogEntryIdT entryid;
        SaHpiEventLogEntryIdT nextentryid;
        SaHpiEventLogEntryIdT preventryid;
        SaHpiEventLogInfoT info;
        SaHpiEventLogEntryT  el;
        SaHpiRdrT rdr;

        int free = 50;

        printf("%s: version %s\n",argv[0],progver);

        while ( (c = getopt( argc, argv,"cx?")) != EOF )
                switch(c) {
                case 'c': fclear = 1; break;
                case 'x': fdebug = 1; break;
                default:
                        printf("Usage %s [-cx]\n",argv[0]);
                        printf("where -c clears the event log\n");
                        printf("      -x displays eXtra debug messages\n");
                        exit(1);
                }
	
	
	/* 
	 * House keeping:
	 * 	-- get (check?) hpi implementation version
	 *      -- open hpi session	
	 */
	if (fdebug) printf("saHpiVersionGet\n");
	hpiVer = saHpiVersionGet();
	printf("Hpi Version %d Implemented.\n", hpiVer);
	
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
                exit(-1);
        }

        rv = saHpiDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));
	

        rv = saHpiDomainInfoGet(sessionid, &domainInfo);

        if (fdebug) printf("saHpiDomainInfoGet %s\n", oh_lookup_error(rv));
        printf("DomainInfo: UpdateCount = %d, UpdateTime = %lx\n",
               domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
	{
		rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);

		if (fdebug)
			printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));
			    
		if (rv == SA_OK) {
			resourceid = rptentry.ResourceId;
				    
			if (fdebug)
				printf("RPT %d capabilities = %x\n", resourceid,
						   rptentry.ResourceCapabilities);
				    
			if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
					    
				if (fdebug) printf("RPT doesn't have SEL\n");
				rptentryid = nextrptentryid;
				continue;  /* no SEL here, try next RPT */
			}
				    
			rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
			printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);
				    
			/* initialize structure */
			info.Entries = 0;
			info.Size = 0;
			info.Enabled = 0;
				    
			rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);
			
			if (fdebug)
				printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
			if (rv == SA_OK) {
				printf("EventLogInfo for %s, ResourceId %d\n",
						rptentry.ResourceTag.Data, resourceid);
				oh_print_evenloginfo(&info, 4);
			} else 
				printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
				    
			if (fclear) {
				rv = saHpiEventLogClear(sessionid,resourceid);
				if (rv == SA_OK)
					printf("EventLog successfully cleared\n");
				else
					printf("EventLog clear, error = %d, %s\n", rv, oh_lookup_error(rv));
				break;
			}
				    
			if (info.Entries != 0){
				entryid = SAHPI_OLDEST_ENTRY;
				while ((rv == SA_OK) && (entryid != SAHPI_NO_MORE_ENTRIES))
				{
					rv = saHpiEventLogEntryGet(sessionid,resourceid,
								entryid,&preventryid,
								&nextentryid, &el,&rdr,NULL);
								
					if (fdebug) printf("saHpiEventLogEntryGet %s\n",
							    			oh_lookup_error(rv));
								   
					if (rv == SA_OK) {
						oh_print_eventlogentry(&el, 6);
						preventryid = entryid;
						entryid = nextentryid;
					}
				}
				
			} else
				printf("SEL is empty\n");
				    
			if (free < 6) {
				printf("WARNING: Log free space is very low (%d records)\n",free);
				printf("\tClear log with hpiel -c\n");
			}
		}		    
		rptentryid = nextrptentryid;
	}
	
	
	printf("done.\n");
        rv = saHpiSessionClose(sessionid);
	return(0);
}


