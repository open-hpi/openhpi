/* -*- linux-c -*-
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2004-2005
 *
 * Author(s):
 *     Andy Cress  arcress@users.sourceforge.net
 *     Sean Dague <http://dague.net/sean>
 *     Renier Morales <renierm@users.sf.net>
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
int fdebug              = 0;
int fclear              = 0;
int frdr                = 0;
int frpt                = 0;
SaHpiResourceIdT fresid = 0;
int fraw                = 0;

unsigned int frawcount  = 0;

int main(int argc, char **argv)
{
        int c;
	oh_big_textbuffer bigbuf2;
        SaErrorT rv, rv_2;
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
        SaHpiRdrT rdr, *rdrptr;
	SaHpiRptEntryT rpt_2nd, *rptptr;

        int free = 50;

        while ( (c = getopt( argc, argv,"cdpxr:w?")) != EOF )
                switch(c) {
                case 'c': fclear = 1; break;
                case 'd': frdr   = 1; break;
                case 'p': frpt   = 1; break;				
                case 'x': fdebug = 1; break;
		case 'r': fresid = strtoul(optarg, NULL, 10); break;
                case 'w': fraw = 1; fdebug = 0; break;
                default:
                        printf("\n\nUsage %s [-cdpx]\n",argv[0]);
			printf("    Where                             \n");
                        printf("        -c clears the event log\n");
			printf("        -d also get RDR with the event log\n");
			printf("        -p also get RPT with the event log\n");
                        printf("        -r <Resource Id>, prints log for a specific resource\n");
                        printf("        -w Print raw output\n");
                        printf("        -x displays eXtra debug messages\n\n\n");
                        exit(1);
                }
        if (!fraw) printf("%s: version %s\n",argv[0],progver);
                
	/* 
	 * House keeping:
	 * 	-- get (check?) hpi implementation version
	 *      -- open hpi session	
	 */
	if (fdebug) printf("saHpiVersionGet\n");
	hpiVer = saHpiVersionGet();
	if (!fraw) printf("Hpi Version %d Implemented.\n", hpiVer);
	
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, HpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
                exit(-1);
        }

        rv = saHpiDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));
	

        rv = saHpiDomainInfoGet(sessionid, &domainInfo);

        if (fdebug) printf("saHpiDomainInfoGet %s\n", oh_lookup_error(rv));
        if (!fraw) printf("DomainInfo: UpdateCount = %d, UpdateTime = %lx\n",
               domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
	{
                if (fresid) rptentryid = fresid; // Doing only one resource.
                
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
	
			rv_2 = oh_init_bigtext(&bigbuf2);
			if (rv_2) return(rv_2);
			rv  = oh_decode_entitypath(&rptentry.ResourceEntity, &bigbuf2);
			if (!fraw) printf("%s\n", bigbuf2.Data);
			if (!fraw)
                                printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);
				    
			/* initialize structure */
			info.Entries = 0;
			info.Size = 0;
			info.Enabled = 0;
				    
			rv_2 = saHpiEventLogInfoGet(sessionid,resourceid,&info);
			
			if (fdebug)
				printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
			if (rv_2 == SA_OK) {
				if (!fraw) {
                                        printf("EventLogInfo for %s, ResourceId %d\n",
						rptentry.ResourceTag.Data, resourceid);
				        oh_print_eventloginfo(&info, 4);
                                }
			} else { 
				printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv_2));
			}
				    
			if (fclear) {
				rv = saHpiEventLogClear(sessionid,resourceid);
				if (rv == SA_OK) {
					if (!fraw) printf("EventLog successfully cleared\n");
				} else
					printf("EventLog clear, error = %d, %s\n", rv, oh_lookup_error(rv));
				break;
			}
				    
			if (info.Entries != 0){
				entryid = SAHPI_OLDEST_ENTRY;
				while ((rv == SA_OK) && (entryid != SAHPI_NO_MORE_ENTRIES))
				{
				
					if (frdr || fraw) rdrptr = &rdr;
                                        else rdrptr = NULL;
					
					if (frpt || fraw) rptptr = &rpt_2nd;
					else rptptr = NULL;
						
						
					if(fdebug) printf("rdrptr %p, rptptr %p\n", rdrptr, rptptr);
					rv = saHpiEventLogEntryGet(sessionid,resourceid,
								entryid,&preventryid,
								&nextentryid, &el,rdrptr,rptptr);
								
					if (fdebug) printf("saHpiEventLogEntryGet %s\n",
							    			oh_lookup_error(rv));
								   
					if (rv == SA_OK) {
						if (!fraw) oh_print_eventlogentry(&el, 6);
						if (frdr) {
							if (rdrptr->RdrType == SAHPI_NO_RECORD) {
								if (!fraw)
                                                                        printf("            No RDR associated with EventType =  %s\n\n",
                                                                                oh_lookup_eventtype(el.Event.EventType));
							} else	
								if (!fraw) oh_print_rdr(rdrptr, 12);
						}
						if (frpt) {
							if (rptptr->ResourceCapabilities == 0) {
								if (!fraw)
                                                                        printf("            No RPT associated with EventType =  %s\n\n", 
									       oh_lookup_eventtype(el.Event.EventType));
							} else 
								if (!fraw)
                                                                        oh_print_rptentry(rptptr, 10);
						}

						preventryid = entryid;
						entryid = nextentryid;
					}

                                        if (fraw && rv == SA_OK) {
                                                SaHpiTextBufferT rawbuffer;
                                                oh_big_textbuffer rawbigbuf;

                                                oh_init_textbuffer(&rawbuffer);
                                                oh_init_bigtext(&rawbigbuf);
                                                frawcount++;
                                                // No.,
                                                printf("%u,", frawcount);
                                                // Name,
                                                if (el.Event.EventType == SAHPI_ET_OEM) {
                                                        printf("\"%s %u - %s\",", oh_lookup_entitytype(rptptr->ResourceEntity.Entry[0].EntityType),
                                                                rptptr->ResourceEntity.Entry[0].EntityLocation,
                                                                el.Event.EventDataUnion.OemEvent.OemEventData.Data);
                                                } else if (el.Event.EventType == SAHPI_ET_HOTSWAP) {
                                                        printf("\"%s %u - %s\",", oh_lookup_entitytype(rptptr->ResourceEntity.Entry[0].EntityType),
                                                                rptptr->ResourceEntity.Entry[0].EntityLocation, rptptr->ResourceTag.Data);
                                                } else {
                                                        printf("\"%s %u - %s\",", oh_lookup_entitytype(rptptr->ResourceEntity.Entry[0].EntityType),
                                                                rptptr->ResourceEntity.Entry[0].EntityLocation, rdrptr->IdString.Data);
                                                }
                                                if (el.Event.EventType == SAHPI_ET_SENSOR) {
                                                        // Sensor Type,
                                                        printf("\"%s\",", oh_lookup_sensortype(el.Event.EventDataUnion.SensorEvent.SensorType));
                                                        // Sensor Num,
                                                        printf("%u,", rdrptr->RdrTypeUnion.SensorRec.Num);
                                                        // Assertion,
                                                        if (el.Event.EventDataUnion.SensorEvent.Assertion)
                                                                printf("\"%s\",", "Assertion");
                                                        else
                                                                printf("\"%s\",", "Deassertion");
                                                        // Event State, Blank Field,
                                                        oh_decode_eventstate(el.Event.EventDataUnion.SensorEvent.EventState,
                                                                             el.Event.EventDataUnion.SensorEvent.EventCategory,
                                                                             &rawbuffer);
                                                        printf("\"%s\",\"\",", rawbuffer.Data);
                                                } else printf("\"\",\"\",\"\",\"\",\"\",");
                                                // Record Id, Rdr Type,
                                                printf("%u,\"\",%d,", rdrptr->RecordId, rdrptr->RdrType);
                                                // Timestamp, Manufacturer Id, Resource Rev,
                                                printf("%llu,\"\",\"\",\"\",%u,\"\",%u,", el.Event.Timestamp, rptptr->ResourceInfo.ManufacturerId,
                                                       rptptr->ResourceInfo.ResourceRev);
                                                if (el.Event.EventType == SAHPI_ET_SENSOR) {
                                                        // Sensor Type, Sensor Num,
                                                        printf("%d,%d,",rdrptr->RdrTypeUnion.SensorRec.Type,
                                                               rdrptr->RdrTypeUnion.SensorRec.Num);
                                                } else printf("\"\",\"\",");
                                                // Event Type,
                                                printf("%d,", el.Event.EventType);
                                                // Event Data 1, Event Data 2, Event Data 3,
                                                printf("\"\",\"\",\"\",");
                                                // Source, Event Type,
                                                printf("%u,\"%s\",", el.Event.Source, oh_lookup_eventtype(el.Event.EventType));
                                                // Timestamp, Severity,
                                                oh_init_textbuffer(&rawbuffer);
                                                oh_decode_time(el.Event.Timestamp, &rawbuffer);
                                                printf("\"%s\",\"%s\",", rawbuffer.Data, oh_lookup_severity(el.Event.Severity));
                                                if (el.Event.EventType == SAHPI_ET_SENSOR) {
                                                        // Sensor Num,
                                                        printf("%u,", rdrptr->RdrTypeUnion.SensorRec.Num);
                                                        // Sensor Type,
                                                        printf("\"%s\",", oh_lookup_sensortype(rdrptr->RdrTypeUnion.SensorRec.Type));
                                                        // Event Category,
                                                        printf("\"%s\",",
                                                                oh_lookup_eventcategory(el.Event.EventDataUnion.SensorEvent.EventCategory));
                                                        // Assertion,
                                                        printf("%d,", el.Event.EventDataUnion.SensorEvent.Assertion);
                                                        // Event State,
                                                        oh_init_textbuffer(&rawbuffer);
                                                        oh_decode_eventstate(el.Event.EventDataUnion.SensorEvent.EventState,
                                                                             el.Event.EventDataUnion.SensorEvent.EventCategory,
                                                                             &rawbuffer);
                                                        printf("\"%s\",", rawbuffer.Data);
                                                        // Sensor Optional Data, Trigger Reading, Trigger Threshold,
                                                        oh_init_textbuffer(&rawbuffer);
                                                        oh_decode_sensoroptionaldata(
                                                                el.Event.EventDataUnion.SensorEvent.OptionalDataPresent, &rawbuffer);
                                                        printf("\"%s\",\"\",\"\",", rawbuffer.Data);
                                                        // Oem, Sensor Specific,
                                                        printf("%u,%u,", el.Event.EventDataUnion.SensorEvent.Oem,
                                                               el.Event.EventDataUnion.SensorEvent.SensorSpecific);
                                                } else printf("\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
                                                // Record Id, Rdr Type, Entity,
                                                oh_init_bigtext(&rawbigbuf);
                                                oh_decode_entitypath(&rdrptr->Entity, &rawbigbuf);
                                                printf("%u,\"%s\",\"%s\",", rdrptr->RecordId,
                                                       oh_lookup_rdrtype(rdrptr->RdrType), rawbigbuf.Data);
                                                // IsFru, Data Type, Language, Data Length,
                                                printf("%d,\"%s\",\"%s\",%u,",
                                                       rdrptr->IsFru,
                                                       oh_lookup_texttype(rdrptr->IdString.DataType),
                                                       oh_lookup_language(rdrptr->IdString.Language),
                                                       rdrptr->IdString.DataLength);
                                                // Data
                                                printf("\"%s\",", rdrptr->IdString.Data);
                                                if (el.Event.EventType == SAHPI_ET_SENSOR) {
                                                        // Sensor Num,
                                                        printf("%u,", rdrptr->RdrTypeUnion.SensorRec.Num);
                                                        // Sensor Type,
                                                        printf("\"%s\",", oh_lookup_sensortype(rdrptr->RdrTypeUnion.SensorRec.Type));
                                                        // Event Category,
                                                        printf("\"%s\",", oh_lookup_eventcategory(
                                                                          el.Event.EventDataUnion.SensorEvent.EventCategory));
                                                        // EnableCtrl,
                                                        printf("%d,", rdrptr->RdrTypeUnion.SensorRec.EnableCtrl);
                                                        // EventCtrl,
                                                        printf("\"%s\",", oh_lookup_sensoreventctrl(rdrptr->RdrTypeUnion.SensorRec.EventCtrl));
                                                        // Events
                                                        oh_init_textbuffer(&rawbuffer);
                                                        oh_decode_eventstate(rdrptr->RdrTypeUnion.SensorRec.Events,
                                                                             el.Event.EventDataUnion.SensorEvent.EventCategory,
                                                                             &rawbuffer);
                                                        printf("\"%s\",", rawbuffer.Data);
                                                        // Reading Formats,
                                                        printf("\"%s\",",
                                                               oh_lookup_sensorreadingtype(rdrptr->RdrTypeUnion.SensorRec.DataFormat.ReadingType));
                                                        // IsNumeric, Base Units,
                                                        printf("\"\",\"%s\",",
                                                               oh_lookup_sensorunits(rdrptr->RdrTypeUnion.SensorRec.DataFormat.BaseUnits));
                                                        // IsAccessible,
                                                        printf("%d,",
                                                                rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible);
                                                        // ReadThold,
                                                        oh_init_bigtext(&rawbigbuf);
                                                        oh_build_threshold_mask(&rawbigbuf,
                                                                rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold, 0);
                                                        if (rawbigbuf.DataLength > 0 && rawbigbuf.Data[rawbigbuf.DataLength-1] == '\n')
                                                                rawbigbuf.Data[rawbigbuf.DataLength-1] = 0;
                                                        printf("\"%s\",", rawbigbuf.Data);
                                                        // WriteThold,
                                                        oh_init_bigtext(&rawbigbuf);
                                                        oh_build_threshold_mask(&rawbigbuf,
                                                                rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.WriteThold, 0);
                                                        if (rawbigbuf.DataLength > 0 && rawbigbuf.Data[rawbigbuf.DataLength-1] == '\n')
                                                                rawbigbuf.Data[rawbigbuf.DataLength-1] = 0;
                                                        printf("\"%s\",", rawbigbuf.Data);
                                                        // Non linear,
                                                        printf("%d,", rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.Nonlinear);
                                                        // Oem,
                                                        printf("%u,", rdrptr->RdrTypeUnion.SensorRec.Oem);
                                                        // Is Supported,
                                                        printf("%d,", rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported);
                                                        // Reading Type,
                                                        printf("\"%s\",",
                                                                oh_lookup_sensorreadingtype(rdrptr->RdrTypeUnion.SensorRec.DataFormat.ReadingType));
                                                } else printf("\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
                                                // Log Entry Id, Timestamp,
                                                oh_init_textbuffer(&rawbuffer);
                                                oh_decode_time(el.Timestamp, &rawbuffer);
                                                printf("%u,\"%s\",", el.EntryId, rawbuffer.Data);
                                                // Source, Event Type,
                                                printf("%u,\"%s\",", el.Event.Source, oh_lookup_eventtype(el.Event.EventType));
                                                // Timestamp, Severity,
                                                oh_init_textbuffer(&rawbuffer);
                                                oh_decode_time(el.Event.Timestamp, &rawbuffer);
                                                printf("\"%s\",\"%s\",", rawbuffer.Data, oh_lookup_severity(el.Event.Severity));
                                                if (el.Event.EventType == SAHPI_ET_SENSOR) {
                                                        // Sensor Num,
                                                        printf("%u,", rdrptr->RdrTypeUnion.SensorRec.Num);
                                                        // Sensor Type,
                                                        printf("\"%s\",", oh_lookup_sensortype(rdrptr->RdrTypeUnion.SensorRec.Type));
                                                        // Event Category,
                                                        printf("\"%s\",",
                                                                oh_lookup_eventcategory(el.Event.EventDataUnion.SensorEvent.EventCategory));
                                                        // Assertion,
                                                        printf("%d,", el.Event.EventDataUnion.SensorEvent.Assertion);
                                                        // Event State,
                                                        oh_init_textbuffer(&rawbuffer);
                                                        oh_decode_eventstate(el.Event.EventDataUnion.SensorEvent.EventState,
                                                                el.Event.EventDataUnion.SensorEvent.EventCategory, &rawbuffer);
                                                        printf("\"%s\",", rawbuffer.Data);
                                                        // Sensor Optional Data,
                                                        oh_init_textbuffer(&rawbuffer);
                                                        oh_decode_sensoroptionaldata(
                                                                el.Event.EventDataUnion.SensorEvent.OptionalDataPresent,
                                                                &rawbuffer);
                                                        printf("\"%s\",\"\",\"\",", rawbuffer.Data);
                                                        // Oem, Sensor Specific,
                                                        printf("%u,%u,",
                                                                el.Event.EventDataUnion.SensorEvent.Oem,
                                                                el.Event.EventDataUnion.SensorEvent.SensorSpecific);
                                                } else printf("\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",");
                                                printf("\n");
                                        }
				}
				
			} else
				printf("\tSEL is empty.\n\n");
				    
			if (free < 6) {
				if (!fraw) {
                                        printf("WARNING: Log free space is very low (%d records)\n",
                                               free);
                                        printf("\tClear log with hpiel -c\n");
                                }
			}
		}		    
		rptentryid = nextrptentryid;

                if (fresid) break; // Doing only one resource.
	}
	
	
	if (!fraw) printf("done.\n");
        rv = saHpiSessionClose(sessionid);
	return(0);
}


