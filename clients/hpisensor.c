/*      -*- linux-c -*-
 *      $Id$
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <SaHpi.h>
#include <ecode_utils.h>
#include <epath_utils.h>
#include <print_structs.h>

char progver[] = "1.0";
int fdebug = 0;
int fshowthr = 0;
char *rtypes[5] = {"None    ", "Control ", "Sensor  ", "Invent  ", "Watchdog"};

static void ShowSensor(
        SaHpiSessionIdT sessionid,
        SaHpiResourceIdT resourceid,
        SaHpiSensorRecT *sensorrec )
{
        SaHpiSensorNumT sensornum;
        SaHpiSensorReadingT reading;
        SaHpiEventStateT events;
        SaHpiSensorThresholdsT senstbuff;
        SaHpiTextBufferT text;
        SaErrorT rv;
        char *unit;
        int i;
        
        sensornum = sensorrec->Num;
        rv = saHpiSensorReadingGet(sessionid,resourceid, sensornum, &reading, &events);
        if (rv != SA_OK)  {
                printf("ReadingGet ret=%d\n", rv);
                return;
        }
        
        if (reading.IsSupported) {
                oh_sensor_reading2str(reading, sensorrec->DataFormat, &text);
                printf(" = %s\n", text.Data);
                
                if (fshowthr) {
                        /* ok, this code needs major rework, why was this ever hardcoded
                           to certain types? */
                        if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MAX ) {
                                if(oh_sensor_reading2str(sensorrec->DataFormat.Range.Max, 
                                                         sensorrec->DataFormat, &text) == SA_OK) {
                                        printf( "    Max of Range: %s\n", text.Data);
                                } else {
                                        printf( "    Max of Range: FAILED\n");
                                }
                        }
                        if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MIN ) {
                                if(oh_sensor_reading2str(sensorrec->DataFormat.Range.Min, 
                                                         sensorrec->DataFormat, &text) == SA_OK) {
                                        printf( "    Min of Range: %s\n", text.Data);
                                } else {
                                        printf( "    Min of Range: FAILED\n");
                                }
                        }
                } /* endif valid threshold */
        } /* endif showthr */
        return;
}  /*end ShowSensor*/

int main(int argc, char **argv)
{
        int c;
        SaErrorT rv;
        // SaHpiVersionT hpiVer;
        SaHpiSessionIdT sessionid;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiEntryIdT entryid;
        SaHpiEntryIdT nextentryid;
        SaHpiResourceIdT resourceid;
        SaHpiRdrT rdr;
        SaHpiEntityPathT ep_target;
        char *ep_string = NULL;
                
        
        printf("%s: version %s\n",argv[0],progver); 
        
        while ( (c = getopt( argc, argv,"te:x?")) != EOF )
                switch(c) {
                case 't': fshowthr = 1; break;
                case 'x': fdebug = 1; break;
                case 'e':
                        if (optarg) {
                                ep_string = (char *)strdup(optarg);
                        }
                        string2entitypath(ep_string,&ep_target);
                        break;
                default:
                        printf("Usage %s [-t -x]\n",argv[0]);
                        printf("where -t = show Thresholds also\n");
                        printf("      -x = show eXtra debug messages\n");
                        exit(1);
                }

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, daemon not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n",decode_error(rv));
                exit(-1);
        }
        
        rv = saHpiDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n",decode_error(rv));

        /*
        rv = saHpiRptInfoGet(sessionid,&rptinfo);

        if (fdebug) printf("saHpiRptInfoGet %s\n",decode_error(rv));
        printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
               rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
        */
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (fdebug) printf("saHpiRptEntryGet %s\n",decode_error(rv));
                                                
                if (rv == SA_OK) {
                        /* Walk the RDR list for this RPT entry */

                        /* Filter by entity path if specified */
                        if (ep_string && ep_cmp(&ep_target,&(rptentry.ResourceEntity))) {
                                rptentryid = nextrptentryid;
                                continue;
                        }
                        
                        entryid = SAHPI_FIRST_ENTRY;
                        resourceid = rptentry.ResourceId;
                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 
                        printf("RPTEntry[%d] tag: %s\n",
                               resourceid,rptentry.ResourceTag.Data);
			print_ep(&rptentry.ResourceEntity);
                        while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
                        {
                                rv = saHpiRdrGet(sessionid,resourceid,
                                                 entryid,&nextentryid, &rdr);
                                if (fdebug) printf("saHpiRdrGet[%d] rv = %d\n",entryid,rv);
                                if (rv == SA_OK) {
                                        char *eol;
                                        rdr.IdString.Data[rdr.IdString.DataLength] = 0;
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                                eol = "";
                                        } else {
                                                eol = "\n";
                                        }
                                        printf("    RDR[%6d]: %s %s %s",rdr.RecordId,
                                               rtypes[rdr.RdrType],rdr.IdString.Data,eol);
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                                if(rdr.RdrTypeUnion.SensorRec.DataFormat.IsSupported) {
                                                        ShowSensor(sessionid,resourceid,
                                                                   &rdr.RdrTypeUnion.SensorRec);
                                                } else {
                                                        printf("Sensor ignored probably due to Resource not present\n");
                                                }
                                        }

                                        entryid = nextentryid;
                                } else {
                                        rv = SA_OK;
                                        entryid = SAHPI_LAST_ENTRY;
                                }
                        }
                        rptentryid = nextrptentryid;
                }
        }
        
        rv = saHpiSessionClose(sessionid);
        
        exit(0);
        return(0);
}
 
/* end hpisensor.c */
