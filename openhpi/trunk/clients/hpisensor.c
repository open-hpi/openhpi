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

char progver[] = "1.0";
int fdebug = 0;
int fshowthr = 0;
char *rtypes[5] = {"None    ", "Control ", "Sensor  ", "Invent  ", "Watchdog"};

#define NSU  77
char *units[NSU] = {
        "units", "degrees C", "degrees F", "degrees K", "volts", "amps", 
        "watts", "joules",    "coulombs",  "va",        "nits",  "lumen",
        "lux",   "candela",   "kpa",       "psi",     "newton",  "cfm",
        "rpm",   "Hz",        "us",        "ms",      "sec",     "min",
        "hours", "days",      "weeks",     "mil",     "in",     "ft",
        /*30*/ "cu in", "cu ft", "mm", "cm", "m", "cu cm", "cu m", 
        "liters", "fl oz", "radians", "sterad", "rev", 
        "cycles", "grav", "oz", "lbs", "ft lb", "oz in", "gauss", 
        "gilberts", "henry", "mhenry", "farad", "ufarad", "ohms",
        /*55*/  "", "", "", "", "", "", "", "", "", "", 
        "", "", "", "", "Gb", 
        /*70*/ "bytes", "KB", "MB",  "GB", 
        /*74*/ "", "", "line"
};


static void ShowSensor(
        SaHpiSessionIdT sessionid,
        SaHpiResourceIdT resourceid,
        SaHpiSensorRecT *sensorrec )
{
        SaHpiSensorNumT sensornum;
        SaHpiSensorReadingT reading;
        SaHpiEventStateT events;
        SaHpiSensorThresholdsT senstbuff; 
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
                /* Also show units of interpreted reading */
                i = sensorrec->DataFormat.BaseUnits;
                if (i >= NSU) i = 0;
                unit = units[i];
                
                switch(reading.Type)
                {
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                        printf(" = %8.2f %s\n", 
                               reading.Value.SensorFloat64,unit);
                        break;
                case SAHPI_SENSOR_READING_TYPE_INT64:
                        printf(" = %lld %s\n", 
                               reading.Value.SensorInt64, unit);
                        break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                        printf(" = %s\n",
                               reading.Value.SensorBuffer);
                        break;
                default: 
                        printf(" = %lld (itype=%d)\n", 
                               reading.Value.SensorUint64, 
                               reading.Type);
                }
     
                /* all the code below is completely invalid.  It assumes uint32 in all 
                   kinds of cases that are invalid entirely.  This can be fixed later */
#if 0
                if (fshowthr) {
                        /* ok, this code needs major rework, why was this ever hardcoded
                           to certain types? */
                        if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MAX )
                                printf( "    Max of Range: %5.2f\n",
                                        sensorrec->DataFormat.Range.Max.Value.SensorFloat64);
                        if ( sensorrec->DataFormat.Range.Flags & SAHPI_SRF_MIN )
                                printf( "    Min of Range: %5.2f\n",
                                        sensorrec->DataFormat.Range.Min.Value.SensorFloat64);
                        /* Show thresholds, if any */
                        if ((!sensorrec->Ignore) && (sensorrec->ThresholdDefn.IsThreshold)) {
                                rv = saHpiSensorThresholdsGet(sessionid, resourceid, 
                                                              sensornum, &senstbuff);
                                printf( "\t\t\t");
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_LOW_MINOR ) {
                                        printf( "LoMin %5.2f ",
                                                senstbuff.LowMinor.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_LOW_MAJOR ) {
                                        printf( "LoMaj %5.2f ",
                                                senstbuff.LowMajor.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_LOW_CRIT ) {
                                        printf( "LoCri %5.2f ",
                                                senstbuff.LowCritical.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_UP_MINOR ) {
                                        printf( "HiMin %5.2f ",
                                                senstbuff.UpMinor.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_UP_MAJOR ) {
                                        printf( "HiMaj %5.2f ",
                                                senstbuff.UpMajor.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_UP_CRIT ) {
                                        printf( "HiCri %5.2f ",
                                                senstbuff.UpCritical.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_UP_HYSTERESIS ) {
                                        printf( "Hi Hys %5.2f ",
                                                senstbuff.PosThdHysteresis.Interpreted.Value.SensorFloat32);
                                } 
                                if ( sensorrec->ThresholdDefn.ReadThold & SAHPI_STM_LOW_HYSTERESIS ) {
                                        printf( "Lo Hys %5.2f ",
                                                senstbuff.NegThdHysteresis.Interpreted.Value.SensorFloat32);
                                } 
                                printf( "\n");
                        }
                } /* endif valid threshold */
#endif
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
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) eol = "";
                                        else eol = "\n";
                                        printf("    RDR[%6d]: %s %s %s",rdr.RecordId,
                                               rtypes[rdr.RdrType],rdr.IdString.Data,eol);
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                                ShowSensor(sessionid,resourceid,
                                                           &rdr.RdrTypeUnion.SensorRec);
                                        } else {
						printf("Sensor ignored probably due to Resource not present\n");
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
