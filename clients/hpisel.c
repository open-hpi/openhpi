/* -*- linux-c -*-
 *
 * $Id$
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2003
 * Authors:  
 *     Andy Cress  arcress@users.sourceforge.net
 *     Sean Dague <http://dague.net/sean>
 *  
 * 04/28/03 Andy Cress - created
 * 04/30/03 Andy Cress v0.6 first good run with common use cases
 * 05/06/03 Andy Cress v0.7 added -c option to clear it
 * 05/29/03 Andy Cress v0.8 fixed pstr warnings
 * 06/13/03 Andy Cress v0.9 fixed strcpy bug, 
 *                          workaround for SensorEvent.data3
 * 06/19/03 Andy Cress 0.91 added low SEL free space warning
 * 06/25/03 Andy Cress v1.0 rework event data logic 
 * 07/23/03 Andy Cress workaround for OpenHPI BUGGY stuff
 * 11/12/03 Andy Cress v1.1 check for CAPABILITY_SEL
 * 03/03/04 Andy Cress v1.2 use better free space logic now 
 * 
 */
/*M*
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
 *M*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "ecode_utils.h"
#include "printevent_utils.h"
#include <SaHpi.h>

char progver[] = "1.2";
int fdebug = 0;
int fclear = 0;

int main(int argc, char **argv)
{
        int c;
        SaErrorT rv;
        SaHpiVersionT hpiVer;
        SaHpiSessionIdT sessionid;
        SaHpiRptInfoT rptinfo;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;
        SaHpiSelEntryIdT entryid;
        SaHpiSelEntryIdT nextentryid;
        SaHpiSelEntryIdT preventryid;
        SaHpiSelInfoT info;
        SaHpiSelEntryT  sel;
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
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
				{
	    	            rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
						
    	    	        if (fdebug)
								printf("saHpiRptEntryGet %s\n",decode_error(rv));
						
                		if (rv == SA_OK) {
								resourceid = rptentry.ResourceId;
								
								if (fdebug)
										printf("RPT %d capabilities = %x\n", resourceid,
                                        				   rptentry.ResourceCapabilities);
								
								if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
										if (fdebug)
												printf("RPT doesn't have SEL\n");
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
										printf("saHpiEventLogInfoGet %s\n",decode_error(rv));
								if (rv == SA_OK) {
										char date[30];
										printf("EventLog entries=%d, size=%d, enabled=%d\n",
														info.Entries,info.Size,info.Enabled);
										free = info.Size - info.Entries;
										saftime2str(info.UpdateTimestamp,date,30);
										printf("UpdateTime = %s, ", date);
										saftime2str(info.CurrentTime,date,30);
										printf("CurrentTime = %s\n", date);
										printf("Overflow = %d\n", info.OverflowFlag);
										printf("DeleteEntrySupported = %d\n", info.DeleteEntrySupported);
								}

								if (fclear) {
										rv = saHpiEventLogClear(sessionid,resourceid);
										if (rv == SA_OK)
												printf("EventLog successfully cleared\n");
										else 
												printf("EventLog clear, error = %d, %s\n",rv, decode_error(rv));
										break;
								}

								if (info.Entries != 0){
										entryid = SAHPI_OLDEST_ENTRY;
										while ((rv == SA_OK) && (entryid != SAHPI_NO_MORE_ENTRIES))
												{
														rv = saHpiEventLogEntryGet(sessionid,resourceid,
																		entryid,&preventryid,&nextentryid,
																		&sel,&rdr,NULL);
														if (fdebug)
																printf("saHpiEventLogEntryGet %s\n",
																				decode_error(rv));
														if (rv == SA_OK) {

																ShowSel(&sel, &rdr, &rptentry);
																preventryid = entryid;
																entryid = nextentryid;
														}
												}
								} else
										printf("SEL is empty\n");

								if (free < 6) {
										printf("WARNING: Log free space is very low (%d records)\n",free);
										printf("\tClear log with hpisel -c\n");
								}

								rptentryid = nextrptentryid;
						}
				}
		printf("done.\n"); 
		rv = saHpiSessionClose(sessionid);
		rv = saHpiFinalize();

//		exit(0);

		return(0);
}
 
/* end hpisel.c */
