/*
 * hpievent.c
 *
 * Author:  Bill Barkley
 * Copyright (c) 2003 Intel Corporation.
 *
 * 05/14/03 
 * 06/06/03 - complete working version
 * 06/20/03 ARCress - added option for Mullins w default=Langley-type sensor
 *                    added progver
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
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "SaHpi.h"

int fdebug = 0;
int fxdebug = 0;
int fsensor = 0;
int slist = 0;
int i,j,k = 0;
int sensor_name_len = 0;
char *sensor_name;
char s_name[] = "80mm Sys Fan (R)";
char sm_name[] = "Baseboard Fan 2";
char progname[] = "hpievent";
char progver[] = "0.5";
char inbuff[1024];
char outbuff[256];
SaHpiUint32T buffersize;
SaHpiUint32T actualsize;
SaHpiInventoryDataT *inv;
SaHpiInventGeneralDataT *dataptr;
SaHpiTextBufferT *strptr;
SaHpiSensorEvtEnablesT enables1;
SaHpiSensorEvtEnablesT enables2;
SaErrorT rv;

char *rdrtypes[5] = {
	"None    ",
	"Control ",
	"Sensor  ",
	"Invent  ",
	"Watchdog"};

#define HPI_NSEC_PER_SEC 1000000000LL
#define NSU   32
char *units[NSU] = {
	"units", "deg C",     "deg F",     "deg K",     "volts", "amps",
	"watts", "joules",    "coulombs",  "va",        "nits",  "lumen",
	"lux",   "candela",   "kpa",       "psi",     "newton",  "cfm",
	"rpm",   "Hz",        "us",        "ms",      "sec",     "min",
	"hours", "days",      "weeks",     "mil",     "in",     "ft",
	"mm",    "cm"
};

void
fixstr(SaHpiTextBufferT *strptr)
{ 
	size_t datalen;
	if ((datalen=strptr->DataLength) != 0)
		strncpy ((char *)outbuff, (char *)strptr->Data, datalen);
	outbuff[datalen] = 0;
}

void ShowThresh(
   SaHpiSensorThresholdsT *sensbuff)
{
      printf( "    Supported Thresholds:\n");
      if (sensbuff->LowCritical.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Lower Critical Threshold: %5.2f\n",
               sensbuff->LowCritical.Interpreted.Value.SensorFloat32);
      if (sensbuff->LowMajor.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Lower Major Threshold: %5.2f\n",
               sensbuff->LowMajor.Interpreted.Value.SensorFloat32);
      if (sensbuff->LowMinor.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Lower Minor Threshold: %5.2f\n",
               sensbuff->LowMinor.Interpreted.Value.SensorFloat32);

      if (sensbuff->UpCritical.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Upper Critical Threshold: %5.2f\n",
               sensbuff->UpCritical.Interpreted.Value.SensorFloat32);
      if (sensbuff->UpMajor.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Upper Major Threshold: %5.2f\n",
               sensbuff->UpMajor.Interpreted.Value.SensorFloat32);
      if (sensbuff->UpMinor.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Upper Minor Threshold: %5.2f\n",
               sensbuff->UpMinor.Interpreted.Value.SensorFloat32);

      if (sensbuff->PosThdHysteresis.ValuesPresent & SAHPI_SRF_RAW)
         printf( "      Positive Threshold Hysteresis in RAW\n");
      if (sensbuff->PosThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Positive Threshold Hysteresis: %5.2f\n",
               sensbuff->PosThdHysteresis.Interpreted.Value.SensorFloat32);
      if (sensbuff->NegThdHysteresis.ValuesPresent & SAHPI_SRF_RAW)
         printf( "      Negative Threshold Hysteresis in RAW\n");
      if (sensbuff->NegThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
         printf( "      Negative Threshold Hysteresis: %5.2f\n",
               sensbuff->NegThdHysteresis.Interpreted.Value.SensorFloat32);
      printf( "\n");
}

static
void DoEvent(
   SaHpiSessionIdT sessionid,
   SaHpiResourceIdT resourceid,
   SaHpiSensorRecT *sensorrec )
{
   SaHpiSensorNumT sensornum;
   SaHpiSensorReadingT reading;
   SaHpiSensorReadingT conv_reading;
   SaHpiSensorThresholdsT senstbuff1;
   SaHpiSensorThresholdsT senstbuff2;

   SaHpiTimeoutT timeout = (SaHpiInt64T)(12 * HPI_NSEC_PER_SEC); /* 12 seconds */
   SaHpiEventT event;
   SaHpiRptEntryT rptentry;
   SaHpiRdrT rdr;
   char *unit;
   int eventflag = 0;
   int i;

   sensornum = sensorrec->Num;

/* Get current sensor reading */

   rv = saHpiSensorReadingGet( sessionid, resourceid, sensornum, &reading);
   if (rv != SA_OK)  {
	printf( "\n");
/*	printf("ReadingGet ret=%d\n", rv); */
	return;
   }

   if ((reading.ValuesPresent & SAHPI_SRF_INTERPRETED) == 0 &&
       (reading.ValuesPresent & SAHPI_SRF_RAW)) {

        /* only try convert if intrepreted not available. */

        rv = saHpiSensorReadingConvert(sessionid, resourceid, sensornum,
					&reading, &conv_reading);
        if (rv != SA_OK) {
		printf("raw=%x conv_ret=%d\n", reading.Raw, rv);
	     /* printf("conv_rv=%s\n", decode_error(rv)); */
		return;
        } else {
	   reading.Interpreted.Type = conv_reading.Interpreted.Type;
	   reading.Interpreted.Value.SensorUint32 = 
		   conv_reading.Interpreted.Value.SensorUint32;
	}
   }
   /* Determine units of interpreted reading */

   i = sensorrec->DataFormat.BaseUnits;

   if (i > NSU) i = 0;
   unit = units[i];

   printf(" = %05.2f %s \n", 
	reading.Interpreted.Value.SensorFloat32, unit);

/* Retrieve current threshold setings, twice */
/* once for backup and once for modification */

   /* Get backup copy */
   rv = saHpiSensorThresholdsGet(
	sessionid, resourceid, sensornum, &senstbuff1);
   if (rv != SA_OK) return;

   /* Get modification copy */
   rv = saHpiSensorThresholdsGet(
	sessionid, resourceid, sensornum, &senstbuff2);
   if (rv != SA_OK) return;

   /* Display current thresholds */ 
   if (rv == SA_OK) {
     printf( "   Current\n");
     ShowThresh( &senstbuff2 );
   }

/* Set new threshold to current reading + 10% */
   senstbuff2.LowMajor.Interpreted.Value.SensorFloat32 =
        reading.Interpreted.Value.SensorFloat32 * (SaHpiFloat32T)1.10;

printf( "ValuesPresent = %x\n", senstbuff2.LowMajor.ValuesPresent);
printf( "Values Mask   = %x\n", (SAHPI_SRF_RAW));
   senstbuff2.LowMajor.ValuesPresent =
        senstbuff2.LowMajor.ValuesPresent ^ (SAHPI_SRF_RAW);
printf( "ValuesPresent = %x\n", senstbuff2.LowMajor.ValuesPresent);

   /* Display new current thresholds */ 
   if (rv == SA_OK) {
     printf( "   New\n");
      ShowThresh( &senstbuff2 );
   }

   /* See what Events are Enabled */

   rv = saHpiSensorEventEnablesGet(
		sessionid, resourceid, sensornum, &enables1);
   if (rv != SA_OK) return;

   printf( "Sensor Event Enables: \n");
   printf( "  Sensor Status = %x\n", enables1.SensorStatus);
   printf( "  Assert Events = %x\n", enables1.AssertEvents);
   printf( "  Deassert Events = %x\n", enables1.DeassertEvents);

/* 
   enables1.AssertEvents = 0x0400;
   enables1.DeassertEvents = 0x0400;
   rv = saHpiSensorEventEnablesSet(
		sessionid, resourceid, sensornum, &enables1);
   if (rv != SA_OK) return;
*/


/************************
   Temporary exit */
/*
 return;
*/

/* Subscribe to New Events, only */
printf( "Subscribe to events\n");
   rv = saHpiSubscribe( sessionid, (SaHpiBoolT)0 );
   if (rv != SA_OK) return;

/* Set new thresholds */
printf( "Set new thresholds\n");

   rv = saHpiSensorThresholdsSet(
	sessionid, resourceid, sensornum, &senstbuff2);
   if (rv != SA_OK) return;

/* Go wait on event to occur */
printf( "Go and get the event\n");
   eventflag = 0;
   while ( eventflag == 0) {
     rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry );
     if (rv != SA_OK) { 
	if (rv != SA_ERR_HPI_TIMEOUT) {
	  printf( "Error during EventGet - Test FAILED\n");
	  return;
        } else {
	  printf( "Time expired during EventGet - Test FAILED\n");
            /* Reset to the original thresholds */
            printf( "Reset thresholds\n");
               rv = saHpiSensorThresholdsSet(
                    sessionid, resourceid, sensornum, &senstbuff1);
               if (rv != SA_OK) return;

            /* Re-read threshold values */
               rv = saHpiSensorThresholdsGet(
                    sessionid, resourceid, sensornum, &senstbuff2);
               if (rv != SA_OK) return;

	  return;
	}
     }
/* Decode the event information */
printf( "Decode event info\n");

     if (event.EventType == SAHPI_ET_SENSOR) {
       printf( "Sensor # = %2d  Severity = %2x\n", 
	    event.EventDataUnion.SensorEvent.SensorNum, event.Severity );
       if (event.EventDataUnion.SensorEvent.SensorNum == sensornum) {
	 eventflag = 1;
	 printf( "Got it - Test PASSED\n");
       }
     }
   }
/* Reset to the original thresholds */
printf( "Reset thresholds\n");
   rv = saHpiSensorThresholdsSet(
	sessionid, resourceid, sensornum, &senstbuff1);
   if (rv != SA_OK) return;

/* Re-read threshold values */
   rv = saHpiSensorThresholdsGet(
	sessionid, resourceid, sensornum, &senstbuff2);
   if (rv != SA_OK) return;

/* Display reset thresholds */ 
   if (rv == SA_OK) {
     printf( "   Reset\n");
     ShowThresh( &senstbuff2 );
   }
/* Unsubscribe to future events */
printf( "Unsubscribe\n");
   rv = saHpiUnsubscribe( sessionid );

   return;
}  /*end DoEvent*/

int
main(int argc, char **argv)
{
  char c;
  SaHpiVersionT hpiVer;
  SaHpiSessionIdT sessionid;
  SaHpiRptInfoT rptinfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiEntryIdT nextentryid;
  SaHpiResourceIdT resourceid;
  SaHpiRdrT rdr;

  printf("%s  ver %s\n", progname,progver);
  sensor_name = (char *)strdup(s_name);
  while ( (c = getopt( argc, argv,"ms:xz?")) != EOF )
  switch(c)
  {
    case 'z': fxdebug = 1; /* fall thru to include next setting */
    case 'x': fdebug = 1; break;
	      /*
    case 'l': slist = 1; break;
	      */
    case 'm': 
	      sensor_name = (char *)strdup(sm_name);
	      break;
    case 's':
	  fsensor = 1;
          if (optarg) {
	    sensor_name = (char *)strdup(optarg);
	  }
          break;
    default:
          printf("Usage: %s [-xm] [-s sensor_descriptor]\n", progname);
          printf("   -s  Sensor descriptor\n");
          printf("   -m  use Mullins sensor descriptor\n");
	  /*
          printf("   -l  Display entire sensor list\n");
	  */
          printf("   -x  Display debug messages\n");
          printf("   -z  Display extra debug messages\n");
          exit(1);
  }
  inv = (SaHpiInventoryDataT *)&inbuff[0];

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

  if (fxdebug) printf("saHpiResourcesDiscover rv = %d\n",rv);

  rv = saHpiRptInfoGet(sessionid,&rptinfo);

  if (fxdebug) printf("saHpiRptInfoGet rv = %d\n",rv);
  if (fdebug) printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
         rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
 
  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
    rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
    if (rv == SA_OK)
    {
      /* walk the RDR list for this RPT entry */
      entryid = SAHPI_FIRST_ENTRY;
      rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
      resourceid = rptentry.ResourceId;
      
      if (fdebug) printf("rptentry[%d] resourceid=%d\n", entryid,resourceid);

      printf("Resource Tag: %s\n", rptentry.ResourceTag.Data);
      while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
      {
        rv = saHpiRdrGet(sessionid,resourceid, entryid,&nextentryid, &rdr);

  	if (fxdebug) printf("saHpiRdrGet[%d] rv = %d\n",entryid,rv);

	if (rv == SA_OK)
	{
	  if (rdr.RdrType == SAHPI_SENSOR_RDR)
	  { 
	    /*type 2 includes sensor and control records*/
	    rdr.IdString.Data[rdr.IdString.DataLength] = 0;
	    if (strncmp(rdr.IdString.Data, sensor_name,
		rdr.IdString.DataLength) == 0)
	    {
	      printf( "%02d %s\t", rdr.RecordId, rdr.IdString.Data);
	      DoEvent( sessionid, resourceid, &rdr.RdrTypeUnion.SensorRec);
	      if (rv != SA_OK)
	        printf( "Returned Error from DoEvent: rv=%d\n", rv);
	    }
	  } 
	  if (rv != SA_OK)
	      printf( "Returned HPI Error: rv=%d\n", rv);
	  entryid = nextentryid;
        }
      }
      rptentryid = nextrptentryid;
    }
  }
  rv = saHpiSessionClose(sessionid);
  rv = saHpiFinalize();
  exit(0);
}
 /* end hpievent.c */
