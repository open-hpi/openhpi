/*      -*- linux-c -*-
 *      $Id$
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2004 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Andy Cress <arcress@users.sourceforge.net>
 *     Renier Morales <renierm@users.sf.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

char progver[] = "1.0";
int fdebug = 0;
int fshowthr = 0;
int fshowrange = 0;

static void ShowSensor(SaHpiSessionIdT sessionid,
                       SaHpiResourceIdT resourceid,
                       SaHpiSensorRecT *sensorrec )
{
        SaHpiSensorNumT sensornum;
        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thresh;
        SaHpiEventStateT events;
        SaHpiTextBufferT text;
        SaErrorT rv;
        
        sensornum = sensorrec->Num;
        rv = saHpiSensorReadingGet(sessionid,resourceid, sensornum, &reading, &events);
        if (rv != SA_OK)  {
                printf("\nReadingGet ret=%s\n", oh_lookup_error(rv));
                return;
        }
        
        if (!reading.IsSupported ) {
                printf("Reading Not Supported for this sensor!");
                return;
        }
        
        if((rv = oh_decode_sensorreading(reading, sensorrec->DataFormat, &text)) == SA_OK) {
                printf(" = %s\n", text.Data);
        } else {
                printf(" FAILED %s\n", oh_lookup_error(rv));
        }
                
        if (fshowrange) { // show ranges
                printf("\t    Ranges::\n");
                if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_NOMINAL ) {
                        if((rv = oh_decode_sensorreading(sensorrec->DataFormat.Range.Nominal, 
							 sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tNominal of Range: %s\n", text.Data);
                        } else {
                                printf( "\t\tNominal of Range: FAILED %s\n", oh_lookup_error(rv));
                        }
                }
                if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MAX ) {
                        if((rv = oh_decode_sensorreading(sensorrec->DataFormat.Range.Max, 
							 sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tMax of Range: %s\n", text.Data);
                        } else {
                                printf( "\t\tMax of Range: FAILED %s\n", oh_lookup_error(rv));
                        }
                }
                if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MIN ) {
                        if((rv = oh_decode_sensorreading(sensorrec->DataFormat.Range.Min, 
							 sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tMin of Range: %s\n", text.Data);
                        } else {
                                printf( "\t\tMin of Range: FAILED %s\n", oh_lookup_error(rv));
                        }
                }
                if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MAX ) {
                        if((rv = oh_decode_sensorreading(sensorrec->DataFormat.Range.NormalMax, 
                                                       sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tNormal Max of Range: %s\n", text.Data);
                        } else {
                                printf( "\t\tNormal Max of Range: FAILED %s\n", oh_lookup_error(rv));
                        }
                }
                if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_NORMAL_MIN ) {
                        if((rv = oh_decode_sensorreading(sensorrec->DataFormat.Range.NormalMin, 
							 sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tNormal Min of Range: %s\n", text.Data);
                        } else {
                                printf( "\t\tNormal Min of Range: FAILED %s\n", oh_lookup_error(rv));
                        }
                }
        }
        if(fshowthr) { // show thresholds
                rv = saHpiSensorThresholdsGet(sessionid,resourceid, sensornum, &thresh);
                 if (rv != SA_OK)  {
                         printf("\nThresholdsGet ret=%s\n", oh_lookup_error(rv));
                         return;
                 }
                 printf( "\t    Thresholds::\n" );

                 if (thresh.LowCritical.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.LowCritical, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tLow Critical Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tLow Critical Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.LowMajor.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.LowMajor, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tLow Major Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tLow Major Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.LowMinor.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.LowMinor, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tLow Minor Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tLow Minor Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.UpCritical.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.UpCritical, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tUp Critical Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tUp Critical Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.UpMajor.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.UpMajor, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tUp Major Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tUp Major Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.UpMinor.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.UpMinor, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tUp Minor Threshold: %s\n", text.Data);
                         } else {
                                 printf( "\t\tUp Minor Threshold: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.PosThdHysteresis.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.PosThdHysteresis, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tPos Threshold Hysteresis: %s\n", text.Data);
                         } else {
                                 printf( "\t\tPos Threshold Hysteresis: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
                 if (thresh.NegThdHysteresis.IsSupported) {
                         if((rv = oh_decode_sensorreading(thresh.NegThdHysteresis, 
                                                        sensorrec->DataFormat, &text)) == SA_OK) {
                                printf( "\t\tNeg Threshold Hysteresis: %s\n", text.Data);
                         } else {
                                 printf( "\t\tNeg Threshold Hysteresis: FAILED %s\n", oh_lookup_error(rv));
                         }
                 }
        }
        return;
}

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
        
        while ( (c = getopt( argc, argv,"rte:x?")) != EOF )
                switch(c) {
                case 't': fshowthr = 1; break;
                case 'r': fshowrange = 1; break;
                case 'x': fdebug = 1; break;
                case 'e':
                        if (optarg) {
                                ep_string = (char *)strdup(optarg);
                        }
                        string2entitypath(ep_string,&ep_target);
                        break;
                default:
                        printf("Usage %s [-t -r -x -e]\n",argv[0]);
                        printf("where -t = show Thresholds also\n");
                        printf("      -r = show Range values also\n");
                        printf("      -e entity path = limit to a single entity\n");
                        printf("      -x = show eXtra debug messages\n");
                        exit(1);
                }

        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                printf("saHpiSessionOpen: %s", oh_lookup_error(rv));
                exit(-1);
        }
        
        rv = saHpiDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));
        
        /*
        rv = saHpiRptInfoGet(sessionid,&rptinfo);

        if (fdebug) printf("saHpiRptInfoGet %s\n", oh_lookup_error(rv));
        printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
               rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
        */
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (fdebug) printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));
                                                
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
                                        printf("    RDR[%6d]: %s %s %s",
                                               rdr.RecordId,
                                               oh_lookup_rdrtype(rdr.RdrType),
                                               rdr.IdString.Data,
                                               eol);
                                        
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                                if(rdr.RdrTypeUnion.SensorRec.DataFormat.IsSupported) {
                                                        ShowSensor(sessionid,resourceid,
                                                                   &rdr.RdrTypeUnion.SensorRec);
                                                } else {
                                                        printf("Sensor reading isn't supported\n");
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
