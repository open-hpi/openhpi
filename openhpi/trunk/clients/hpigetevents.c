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
 *      Peter D Phan <pdphan@us.ibm.com>
 *      Steve Sherman <stevees@us.ibm.com>    
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

#include <unistd.h>

#define HPI_NSEC_PER_SEC 1000000000LL

char progver[] = "0.1";
int fdebug = 0;
int ftimer = 0;

int main(int argc, char **argv)
{
        int c, wait = 0;
        SaErrorT rv;

        /*SaHpiVersionT hpiVer;*/
        SaHpiSessionIdT sessionid;
        
        /* Domain */
        SaHpiDomainInfoT domainInfo;

        /*SaHpiRptInfoT rptinfo;*/
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;
        SaHpiEventLogInfoT info;
        SaHpiRdrT rdr;
        SaHpiTimeoutT timeout; 
        SaHpiEventT event;

        memset(&rptentry, 0, sizeof(rptentry));
        
        printf("%s: version %s\n",argv[0],progver); 

        while ( (c = getopt( argc, argv,"t:x?")) != EOF ) {
                switch(c) {
                        case 't':
                                ftimer = 1;
                                wait = atoi(optarg);
                                printf("-t:wait\n");
                                break;
                        case 'x': 
                                fdebug = 1; 
                                printf("fdebug\n");
                                break;
                        default:
                                printf("Usage %s [-tx]\n",argv[0]);
                                printf("      -t <value>:wait <value> seconds for event\n");
                                printf("      -x        :displays eXtra debug messages\n");
                                exit(1);
                }
        }
        if (ftimer) 
                timeout = (SaHpiTimeoutT)(wait * HPI_NSEC_PER_SEC);  
        else
                timeout = (SaHpiTimeoutT) SAHPI_TIMEOUT_IMMEDIATE;

        printf("************** timeout:[%lld] ****************\n", timeout);    

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
                exit(-1);
        }
 
        printf( "Subscribe to events\n");
        rv = saHpiSubscribe( sessionid );
        if (rv != SA_OK) return rv;

        rv = saHpiDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));

        rv = saHpiDomainInfoGet(sessionid, &domainInfo);

        if (fdebug) printf("saHpiRptInfoGet %s\n", oh_lookup_error(rv));
        printf("DomainInfo: UpdateCount = %d, UpdateTime = %lx\n",
               domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {       
                printf("**********************************************\n");

                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);

                if (fdebug) 
                        printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));

                if (rv == SA_OK) {

                        resourceid = rptentry.ResourceId;

                        if (fdebug) 
                                printf("RPT %x capabilities = %x\n", 
                                       resourceid,
                                       rptentry.ResourceCapabilities);

                        if ( (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                                /* Using EventLogInfo to build up event queue - for now */
                                rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);

                                if (fdebug) 
                                        printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
                        } else {
                                if (fdebug) 
                                        printf("RPT doesn't have SEL\n");
                        }

                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 

                        printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);

                        rptentryid = nextrptentryid;

                }

                printf("**********************************************\n");
        }
        
        printf( "Go and get the event\n");
	int count = 0;
        while (count < 10) {
            rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry, NULL);
                if (rv != SA_OK) { 
                        if (rv != SA_ERR_HPI_TIMEOUT) {
                                printf( "Error during EventGet - Test FAILED\n");
                                break;
                        } else {
                                printf( "\n\n****** Time, %d seconds, expired waiting for event.\n", wait);
                                break;
                        }
                } else {
                        printf("Received Event of Type: %s\n", 
                               oh_lookup_eventtype(event.EventType));
		       
			switch (event.EventType) {
		
			case SAHPI_ET_RESOURCE:
				printf("\tEvent Subtype:[%s]\n",
				       oh_lookup_resourceeventtype(event.EventDataUnion.ResourceEvent.ResourceEventType));
				printf("\tReouceId: [%d]\n", event.Source);
#if 0
				/* get any rdrs that may be present */
				SaErrorT             	err;
				SaHpiEntryIdT        	current_rdr;
				SaHpiEntryIdT        	next_rdr;
				SaHpiRdrT            	rdr;

				printf("\tRDR Info:\n");
				next_rdr = SAHPI_FIRST_ENTRY;
				do {
					char tmp_epath[128];
					current_rdr = next_rdr;
					err = saHpiRdrGet(session_id, resource_id, current_rdr, 
							  &next_rdr, &rdr);
					if (SA_OK != err) {
						if (current_rdr == SAHPI_FIRST_ENTRY)
							printf("Empty RDR table\n");
						else
							error("saHpiRdrGet", err);
						return;                        
					}

					printf("\tRecordId: %x\n", rdr.RecordId);
					printf("\tRdrType: %s\n", rdrtype2str(rdr.RdrType));

					printf("\tEntity: \n");
					//entitypath2string(&rdr.Entity, tmp_epath, sizeof(tmp_epath));
					printf("\t\t%s\n", tmp_epath);
					printf("\tIdString: ");
					//display_textbuffer(rdr.IdString);
					printf("\n"); /* Produce blank line between rdrs. */

				}while(next_rdr != SAHPI_LAST_ENTRY);
#endif
				break;

			case SAHPI_ET_DOMAIN:
				printf("\tEvent Subtype: [%s]\n",
				       oh_lookup_domaineventtype(event.EventDataUnion.DomainEvent.Type));
				printf("\tDomainId: [%d]\n", event.Source);
				break;

			case SAHPI_ET_SENSOR:
				//event.EventDataUnion.SensorEnableChangeEvent.SensorType);
				break;

			case SAHPI_ET_SENSOR_ENABLE_CHANGE:
				break;

			case SAHPI_ET_HOTSWAP:
				break;

			case SAHPI_ET_WATCHDOG:
				break;

			case SAHPI_ET_HPI_SW:
				printf("   Event Subtype: [%s]\n",
				       oh_lookup_sweventtype(event.EventDataUnion.HpiSwEvent.Type));
				printf("\tSW Source Id: [%d]\n", event.Source);
				break;

			case SAHPI_ET_OEM:
				break;

			case SAHPI_ET_USER:
				break;

			default:
				printf(" NOT FOUND, ERROR\n");
				break;


			}
/*
                        if(event.EventType == SAHPI_ET_RESOURCE) {
                                printf("   Subtype: %s\n", 
                                       oh_lookup_resourceeventtype(
                                               event.EventDataUnion.ResourceEvent.ResourceEventType));
                        }
*/			
                }
		count++;
        }

        /* Unsubscribe to future events */
        printf( "Unsubscribe\n");
        rv = saHpiUnsubscribe( sessionid );

        rv = saHpiSessionClose(sessionid);

        printf("HPIGETEVENT END\n");
        
        exit(0);
}
 
/* end hpigetevents.c */
