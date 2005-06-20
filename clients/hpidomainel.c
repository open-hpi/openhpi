/* -*- linux-c -*-
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2004, 2005
 *
 * Author(s):
 *     Peter D Phan pdphan@users.sourceforge.net
 *
 */
/*

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


char progver[] = "1.0 HPI B";
int fdebug = 0;
int fclear = 0;
int frdr   = 0;
int frpt   = 0;

int main(int argc, char **argv)
{
        int c;
        SaErrorT rv, rv_2;
        SaHpiVersionT hpiVer;
        SaHpiSessionIdT sessionid;
        
        /* Domain */
        SaHpiDomainInfoT domainInfo;
        SaHpiEventLogEntryIdT entryid;
        SaHpiEventLogEntryIdT nextentryid;
        SaHpiEventLogEntryIdT preventryid;
        SaHpiEventLogInfoT info;
        SaHpiEventLogEntryT  el;
	SaHpiRdrT rdr, *rdrptr;
	SaHpiRptEntryT rpt, *rptptr;


        printf("%s: version %s\n",argv[0],progver);

        while ( (c = getopt( argc, argv,"cdpx?")) != EOF )
                switch(c) {
                case 'c': fclear = 1; break;
                case 'd': frdr   = 1; break;
                case 'p': frpt   = 1; break;				
                case 'x': fdebug = 1; break;
                default:
                        printf("\n\n\nUsage %s [-cdpx]\n",argv[0]);
			printf("    Where                             \n");
                        printf("        -c clears the event log\n");
			printf("        -d also get RDR with the event log\n");
			printf("        -p also get RPT with the event log\n");
                        printf("        -x displays eXtra debug messages\n\n\n");
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

	/* initialize structure */
	info.Entries = 0;
	info.Size = 0;
	info.Enabled = 0;
		    
	rv_2 = saHpiEventLogInfoGet(sessionid,SAHPI_UNSPECIFIED_RESOURCE_ID,&info);
			
	if (fdebug)
			printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
	if (rv_2 == SA_OK) {
		printf("Domain EventLogInfo\n");
		oh_print_eventloginfo(&info, 4);
	} else { 
		printf("Domain saHpiEventLogInfoGet %s\n", oh_lookup_error(rv_2));
	}
				    
	if (fclear) {
		rv = saHpiEventLogClear(sessionid,SAHPI_UNSPECIFIED_RESOURCE_ID);
		if (rv == SA_OK)
			printf("EventLog successfully cleared\n");
		else
			printf("EventLog clear, error = %d, %s\n", rv, oh_lookup_error(rv));
		return(0);
	}
				    
	if (info.Entries != 0){
		rv = SA_OK;
		entryid = SAHPI_OLDEST_ENTRY;
		while ((rv == SA_OK) && (entryid != SAHPI_NO_MORE_ENTRIES))
		{
		
			if (frdr) rdrptr = &rdr;
			else rdrptr = NULL;
					
			if (frpt) rptptr = &rpt;
			else rptptr = NULL;
				
			rv = saHpiEventLogEntryGet(sessionid,SAHPI_UNSPECIFIED_RESOURCE_ID,
						entryid,&preventryid,
						&nextentryid, &el, rdrptr, rptptr);
								
			if (fdebug) printf("saHpiEventLogEntryGet %s\n",
					    			oh_lookup_error(rv));
								   
			if (rv == SA_OK) {
				oh_print_eventlogentry(&el, 6);
				
				if (frdr) {
					if (rdrptr->RdrType == SAHPI_NO_RECORD)
						printf("            No RDR associated with EventType =  %s\n\n", 
						oh_lookup_eventtype(el.Event.EventType));
					else	
						oh_print_rdr(rdrptr, 12);
				}
				
				if (frpt) {
					if (rptptr->ResourceCapabilities == 0)
						printf("            No RPT associated with EventType =  %s\n\n", 
						oh_lookup_eventtype(el.Event.EventType));							
					else 
						oh_print_rptentry(rptptr, 10);
				}
				
				preventryid = entryid;
				entryid = nextentryid;
			} else {
				printf("Domain EventLogEntryGet, error = %d, %s\n", rv, oh_lookup_error(rv));
				break;
			}
		}
				
	} else 
		printf("\tSEL is empty.\n\n");		    
			    
	
        rv = saHpiSessionClose(sessionid);
	return(0);
}


