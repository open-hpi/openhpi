/*      -*- linux-c -*-
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
 *     Racing Guo <racing.guo@intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <SaHpi.h>

static char* sensor_name;
static char* pname;
static int need_set;
static float thres_value;

static char*
gettext(SaHpiTextBufferT *text)
{
        static char name[SAHPI_MAX_TEXT_BUFFER_LENGTH + 1];

        memcpy(name, text->Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        name[text->DataLength] = 0;
        return name;
}

static void
reading_print(SaHpiSensorReadingT *reading)
{
        if (reading->ValuesPresent & SAHPI_SRF_RAW) {
                printf("%u", reading->Raw);
        } else if ((reading->ValuesPresent & SAHPI_SRF_INTERPRETED)&&
                   (reading->Interpreted.Type == 
                               SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32) ) {
                printf("%5.2f", reading->Interpreted.Value.SensorFloat32);
        }
}

static void
thres_print(SaHpiSensorThresholdsT *thres)
{
#define THRES_PRINT(thres, field)          \
do {                                       \
        printf(#field":");                 \
        reading_print(&thres->field);      \
        printf("\n");                      \
}while (0)
        THRES_PRINT(thres, LowCritical);
        THRES_PRINT(thres, LowMajor);
        THRES_PRINT(thres, LowMinor);
        THRES_PRINT(thres, UpCritical);
        THRES_PRINT(thres, UpMajor);
        THRES_PRINT(thres, UpMinor);
        THRES_PRINT(thres, PosThdHysteresis);
        THRES_PRINT(thres, NegThdHysteresis);
        printf( "\n");
}


#if 0
static void
thres_read(SaHpiSessionIdT sessionid,
           SaHpiResourceIdT resourceid,
           SaHpiRdrT *rdr)
{
        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thres;
        SaHpiSensorRecT  *rec;
        SaErrorT  rv;
        
        rec = &rdr->RdrTypeUnion.SensorRec;
        rv = saHpiSensorReadingGet(sessionid, resourceid, rec->Num, &reading);
        if (rv != SA_OK)  {
                printf( "saHpiSensorReadingGet error %d\n",rv);
                return;
        }

        printf("sensor(%s)\n",gettext(&rdr->IdString));
        printf("current reading:");
        reading_print(&reading);
        printf("\n");

        rv = saHpiSensorThresholdsGet(sessionid, resourceid, rec->Num, &thres);
        if (rv != SA_OK) {
                printf( "saHpiSensorThresholdsGet error %d\n",rv);
                return;
        }
        printf("current threshold:\n");
        thres_print(&thres);
}
#endif


static void 
dordr(SaHpiSessionIdT sessionid,
      SaHpiResourceIdT resourceid,
      SaHpiRdrT *rdr)
{

        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thres;
        SaHpiSensorRecT  *rec;
        SaErrorT  rv;


        if ( (rdr->RdrType == SAHPI_SENSOR_RDR) && 
              !strcmp(sensor_name, gettext(&rdr->IdString))) {

                rec = &rdr->RdrTypeUnion.SensorRec;

                rv = saHpiSensorReadingGet(sessionid, resourceid, rec->Num, &reading);
                if (rv != SA_OK)  {
                        printf( "saHpiSensorReadingGet error %d\n",rv);
                        return;
                }

                printf("sensor(%s)\n",gettext(&rdr->IdString));
                printf("current reading:");
                reading_print(&reading);
                printf("\n");

                rv = saHpiSensorThresholdsGet(sessionid, resourceid, rec->Num, &thres);
                if (rv != SA_OK) {
                        printf( "saHpiSensorThresholdsGet error %d\n",rv);
                        return;
                }
                printf("current threshold:\n");
                thres_print(&thres);

                
                if (need_set) {
                        struct timeval tv;

                        thres.LowMinor.ValuesPresent = SAHPI_SRF_INTERPRETED;
                        thres.LowMinor.Interpreted.Type = 
                                           SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
                        thres.LowMinor.Interpreted.Value.SensorFloat32 = thres_value;
                        
                        gettimeofday(&tv, NULL);
                        printf("set thres at :%ld\n", tv.tv_sec);

                        rv = saHpiSensorThresholdsSet(sessionid, resourceid, rec->Num, &thres);
                        if (rv != SA_OK) {
                                printf( "saHpiSensorThresholdsSet error %d\n",rv);
                                return;
                        }
                }
                exit(0);
        }
}


static void
usage(void)
{
        printf("%s <sensor name> <LowMinor Reading Value>\n", pname);
}

int
main(int argc, char **argv)
{
        SaHpiVersionT    hpiVer;
        SaHpiSessionIdT  sessionid;
        SaHpiRptInfoT    rptinfo;
        SaHpiRptEntryT   rptentry;
        SaHpiEntryIdT    rptentryid;
        SaHpiEntryIdT    nextrptentryid;
        SaHpiEntryIdT    entryid;
        SaHpiEntryIdT    nextentryid;
        SaHpiResourceIdT resourceid;
        SaHpiRdrT        rdr;
        SaErrorT         rv;


        pname = argv[0];

        if (argc < 2) {
                usage();
                exit(-1);
        }

        sensor_name = argv[1];
        need_set = 0;

        if (argc == 3) {
                need_set = 1;
                thres_value = atof(argv[2]);
        }

        rv = saHpiInitialize(&hpiVer);

        if (rv != SA_OK) {
                printf("saHpiInitialize error %d\n",rv);
                exit(-1);
        }
        rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID,&sessionid,NULL);

        if (rv != SA_OK) {
                printf("saHpiSessionOpen error %d\n",rv);
                exit(-1);
        }
 
        rv = saHpiResourcesDiscover(sessionid);

        rv = saHpiRptInfoGet(sessionid,&rptinfo);

        rptentryid = SAHPI_FIRST_ENTRY;
        while (rptentryid != SAHPI_LAST_ENTRY) {
                rv = saHpiRptEntryGet(sessionid,rptentryid,
                                      &nextrptentryid,&rptentry);
                if (rv != SA_OK) {
                        printf("saHpiRptEntryGet(%s)Error: %d\n",
                               gettext(&rptentry.ResourceTag), rv);
                        break;
                }

                resourceid = rptentry.ResourceId;
                printf("Resource Id:%d Tag: %s\n", resourceid,
                        gettext(&rptentry.ResourceTag));

                entryid = SAHPI_FIRST_ENTRY;
                while (entryid != SAHPI_LAST_ENTRY) {
                        rv = saHpiRdrGet(sessionid, resourceid, entryid,
                                        &nextentryid, &rdr);
               	        if (rv != SA_OK) {
                            printf( "saHpiRdrGet(%s) Error: %d\n",
                                    gettext(&rptentry.ResourceTag), rv);
                            break;
                        }
                        dordr(sessionid, resourceid, &rdr);
	                entryid = nextentryid;
                }

                rptentryid = nextrptentryid;
         }
         rv = saHpiSessionClose(sessionid);
         rv = saHpiFinalize();
         exit(0);
}
