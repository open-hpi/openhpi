/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2004 
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
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
 *     Renier Morales <renier@openhpi.org>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *	
 * ChangeLog:
 *	09/08/04 pdphan@users.sf.net Add sensor number to the display.
 *      01/06/05 arcress  reduce number of display lines per sensor
 *      09/06/10 ulikleber New option -D to select domain
 *      02/02/11 ulikleber Refactoring to use glib for option parsing and
 *                         introduce common options for all clients
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

static gboolean fshowthr   = FALSE;
static gboolean fshowrange = FALSE;
static gboolean fshowstate = FALSE;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "threshold",  't', 0, G_OPTION_ARG_NONE, &fshowthr,   "Show Thresholds also",   NULL },
  { "range",      'r', 0, G_OPTION_ARG_NONE, &fshowrange, "Show Range values also", NULL },
  { "eventstate", 's', 0, G_OPTION_ARG_NONE, &fshowstate, "Show EventState also",   NULL },
  { NULL }
};

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
                printf("ReadingGet ret=%s\n", oh_lookup_error(rv));
                return;
        }
        
        if (reading.IsSupported ) {        
        	if((rv = oh_decode_sensorreading(reading, sensorrec->DataFormat, &text)) == SA_OK) {
                	printf("= %s\n", text.Data);
        	} else {
                	printf("= FAILED, %s\n", oh_lookup_error(rv));
        	}
	} else {
                printf("\n                Sensor reading is not supported\n");
        }
	
	if (fshowstate) {
        	if ((rv = oh_decode_eventstate(events, sensorrec->Category, &text)) == SA_OK) {
                	printf("                Current Sensor State = %s\n", text.Data);
        	} else {
			printf("                Can not decode Sensor EventState value %d\n", (int) events);
		}
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
                         printf("\t    ThresholdsGet ret=%s\n", oh_lookup_error(rv));
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
	/* if extra lines, double-space output */
        if (fshowthr || fshowrange) printf("\n");
        return;
}

int main(int argc, char **argv)
{
        SaErrorT rv;
        SaHpiDomainInfoT dinfo;
        SaHpiSessionIdT sessionid;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiEntryIdT entryid;
        SaHpiEntryIdT nextentryid;
        SaHpiResourceIdT resourceid;
        SaHpiRdrT rdr;
        GError *error = NULL;
        GOptionContext *context;
                
	oh_prog_version(argv[0], OH_SVN_REV);
        
        context = g_option_context_new ("- Display sensor info for resources with Sensor Capability");
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_VERBOSE_OPTION,    // no verbose mode implemented
                error)) { 
                g_print ("option parsing failed: %s\n", error->message);
                g_option_context_free (context);
		exit(1);
	}
        g_option_context_free (context);

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) exit(-1);

        
        if (copt.debug) printf("Starting Discovery ...\n");
        rv = saHpiDiscover(sessionid);
        if (copt.debug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));
        
        rv = saHpiDomainInfoGet(sessionid,&dinfo);

        if (copt.debug) printf("saHpiDomainInfoGet %s\n", oh_lookup_error(rv));
        
        printf("RptInfo: UpdateCount = %u, UpdateTime = %lx\n",
               dinfo.RptUpdateCount, (unsigned long)dinfo.RptUpdateTimestamp);
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (copt.debug) printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));
                                                
                if (rv == SA_OK) {
                        /* Walk the RDR list for this RPT entry */

                        /* Filter by entity path if specified */
                        if (copt.withentitypath && 
                            !oh_cmp_ep(&copt.entitypath, &(rptentry.ResourceEntity))) {
                                rptentryid = nextrptentryid;
                                continue;
                        }
                        
                        entryid = SAHPI_FIRST_ENTRY;
                        resourceid = rptentry.ResourceId;
                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 
                        printf("\nRPTEntry[%u] tag: %s\n",
                               resourceid,rptentry.ResourceTag.Data);
                        oh_print_ep(&rptentry.ResourceEntity, 0);
                        while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
                        {
                                rv = saHpiRdrGet(sessionid,resourceid,
                                                 entryid,&nextentryid, &rdr);
                                if (copt.debug) printf("saHpiRdrGet[%u] rv = %d\n",entryid,rv);
                                if (rv == SA_OK) {
                                        rdr.IdString.Data[rdr.IdString.DataLength] = 0;
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR) {
                                           printf("   RDR[%6d]: Sensor[%3d] %s   \t",
                                               rdr.RecordId,
					       rdr.RdrTypeUnion.SensorRec.Num,
                                               rdr.IdString.Data);	
                                                   ShowSensor(sessionid,resourceid,
                                                               &rdr.RdrTypeUnion.SensorRec);
                                        } else {
                                           printf("   RDR[%6d]: %s %s\n",
                                               rdr.RecordId,
					       oh_lookup_rdrtype(rdr.RdrType),
                                               rdr.IdString.Data);
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
