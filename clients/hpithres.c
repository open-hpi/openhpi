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
 *     Bill Barkely 
 *     Andy Cress <arcress@users.sourceforge.net>
 * Changes:
 * 02/26/04 ARCress - added general search for any Fan sensor.
 * 03/17/04 ARCress - changed to Temp sensor (always has Lower Major)
 * 11/03/2004  kouzmich   porting to HPI B
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <SaHpi.h>
#include <oh_utils.h>


int fdebug = 0;
int fxdebug = 0;
int fsensor = 0;
int slist = 0;
int i,j,k = 0;
int sensor_name_len = 0;
char *sensor_name;
char s_name[] = "80mm Sys Fan (R)";  /*TSRLT2, default*/
char sm_name[] = "Baseboard Fan 2";  /*TSRMT2, Mullins*/
char s_pattn[] = "Temp";  /* else use the first temperature SDR found */
char progver[] = "1.1";
SaHpiUint32T buffersize;
SaHpiUint32T actualsize;
//SaHpiInventGeneralDataT *dataptr;
SaHpiTextBufferT *strptr;
SaHpiBoolT enables1;
SaHpiBoolT enables2;
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

static void print_value(SaHpiSensorReadingT *item, char *mes)
{
	char *val;

	if (item->IsSupported != SAHPI_TRUE)
		return;
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			printf("%s %lld\n", mes, item->Value.SensorInt64);
			return;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			printf("%s %llu\n", mes, item->Value.SensorUint64);
			return;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			printf("%s %10.3f\n", mes, item->Value.SensorFloat64);
			return;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = item->Value.SensorBuffer;
			if (val != NULL)
				printf("%s %s\n", mes, val);
			return;
	}
}

static void Set_value(SaHpiSensorReadingT *item, SaHpiSensorReadingUnionT value)
{
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			item->Value.SensorInt64 = value.SensorInt64;
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			item->Value.SensorUint64 = value.SensorUint64;
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			item->Value.SensorFloat64 = value.SensorFloat64;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			printf("Set_value: Buffer type is not supported\n");
			break;
	}
}

static void Mul_value(SaHpiSensorReadingT *item, SaHpiFloat64T value)
{
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			item->Value.SensorInt64 =
				(SaHpiInt64T)((SaHpiFloat64T)(item->Value.SensorInt64) * value);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			item->Value.SensorUint64 =
				(SaHpiUint64T)((SaHpiFloat64T)(item->Value.SensorUint64) * value);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			item->Value.SensorFloat64 *= value;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			printf("Mul_value: Buffer type is not supported\n");
			break;
	}
}

static void Sum_value(SaHpiSensorReadingT *item, SaHpiFloat64T value)
{
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			item->Value.SensorInt64 =
				(SaHpiInt64T)((SaHpiFloat64T)(item->Value.SensorInt64) + value);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			item->Value.SensorUint64 =
				(SaHpiUint64T)((SaHpiFloat64T)(item->Value.SensorUint64) + value);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			item->Value.SensorFloat64 += value;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			printf("Sum_value: Buffer type is not supported\n");
			break;
	}
}

static void
ShowThresh(SaHpiSensorThresholdsT *sensbuff)
{
	printf( "    Supported Thresholds:\n");
	print_value(&(sensbuff->LowCritical), "      Lower Critical Threshold:");
	print_value(&(sensbuff->LowMajor), "      Lower Major Threshold:");
	print_value(&(sensbuff->LowMinor), "      Lower Minor Threshold:");
	print_value(&(sensbuff->UpCritical), "      Upper Critical Threshold:");
	print_value(&(sensbuff->UpMajor), "      Upper Major Threshold:");
	print_value(&(sensbuff->UpMinor), "      Upper Minor Threshold:");
	print_value(&(sensbuff->PosThdHysteresis), "      Positive Threshold Hysteresis:");
	print_value(&(sensbuff->NegThdHysteresis), "      Negative Threshold Hysteresis:");
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
	SaHpiSensorReadingT item;
	SaHpiSensorThresholdsT senstbuff1;
	SaHpiSensorThresholdsT senstbuff2;
// 	SaHpiTimeoutT timeout = (SaHpiInt64T)(120 * HPI_NSEC_PER_SEC); /* 120 seconds */
	SaHpiTimeoutT timeout = SAHPI_TIMEOUT_IMMEDIATE;
	SaHpiEventT event;
	SaHpiRptEntryT rptentry;
	SaHpiRdrT rdr;
	char *unit;
	int eventflag = 0;
	int i;

	sensornum = sensorrec->Num;

	/* Get current sensor reading */

	rv = saHpiSensorReadingGet( sessionid, resourceid, sensornum, &reading, NULL);
	if (rv != SA_OK)  {
		printf( "saHpiSensorReadingGet error %d\n",rv);
		return;
	}

	/* Determine units of interpreted reading */

	i = sensorrec->DataFormat.BaseUnits;

	if (i > NSU)
		i = 0;
	unit = units[i];

	print_value(&reading, unit);

	/* Retrieve current threshold setings, twice */
	/* once for backup and once for modification */

	/* Get backup copy */
	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff1);
	if (rv != SA_OK) {
		printf( "saHpiSensorThresholdsGet error %d\n",rv);
		return;
	}

	/* Get modification copy */
	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff2);
	if (rv != SA_OK) {
		printf( "saHpiSensorThresholdsGet error %d\n",rv);
		return;
	}

	/* Display current thresholds */ 
	printf( "   Current\n");
	ShowThresh( &senstbuff2 );

	senstbuff2.LowMajor.IsSupported = 1;

	/* Set new threshold to current reading + 10% */
	switch (senstbuff2.LowMajor.Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			senstbuff2.LowMajor.Value.SensorInt64 =
        			reading.Value.SensorInt64 * 1.10;
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			senstbuff2.LowMajor.Value.SensorFloat64 =
        			reading.Value.SensorUint64 * 1.10;
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			senstbuff2.LowMajor.Value.SensorFloat64 =
        			reading.Value.SensorFloat64 * 1.10;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			printf("Buffer type is not supported\n");
			break;
	};
	item = reading;
	Mul_value(&item, (SaHpiFloat64T)1.10);
	Set_value(&(senstbuff2.LowMajor), item.Value);

/* In this case, LowMinor > LowMajor */

	senstbuff2.LowMinor.IsSupported = 1;
	switch (senstbuff2.LowMinor.Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			senstbuff2.LowMinor.Value.SensorInt64 =
        			reading.Value.SensorInt64 * (SaHpiInt64T)1.10 + 10;
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			senstbuff2.LowMinor.Value.SensorFloat64 =
        			reading.Value.SensorUint64 * (SaHpiUint64T)1.10 + 10;
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			senstbuff2.LowMinor.Value.SensorFloat64 =
        			reading.Value.SensorFloat64 * (SaHpiFloat64T)1.10 + 10;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			printf("Buffer type is not supported\n");
			break;
	};

	item = reading;
	Mul_value(&item, (SaHpiFloat64T)1.10);
	Sum_value(&item, (SaHpiFloat64T)10);
	Set_value(&(senstbuff2.LowMinor), item.Value);
   
	printf( "   New\n");
	ShowThresh( &senstbuff2 );

	/* See what Events are Enabled */

	rv = saHpiSensorEventEnableGet(sessionid, resourceid, sensornum, &enables1);
	 if (rv != SA_OK) {
		printf( "saHpiSensorEventEnablesGet error %d\n",rv);
		return;
	}

	/* Subscribe to New Events, only */
	printf( "Subscribe to events\n");
	rv = saHpiSubscribe( sessionid );
	if (rv != SA_OK) {
		printf( "saHpiSubscribe error %d\n",rv);
		return;
	}

	/* Set new thresholds */
	printf( "Set new thresholds\n");

	rv = saHpiSensorThresholdsSet(sessionid, resourceid, sensornum, &senstbuff2);
	if (rv != SA_OK) {
		printf( "saHpiSensorThresholdsSet error %d\n",rv);
		goto out;
	}

	/* Go wait on event to occur */
	printf( "Go and get the event\n");
	eventflag = 0;
	while ( eventflag == 0) {
		rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry, NULL );
		if (rv != SA_OK) { 
			if (rv != SA_ERR_HPI_TIMEOUT) {
				printf( "Error during EventGet - Test FAILED\n");
				goto out;
			};
			rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff2);
			printf( "Result:\n");
			ShowThresh( &senstbuff2 );
			if (memcmp( &senstbuff1, &senstbuff2, sizeof(SaHpiSensorThresholdsT)) == 0)
				printf( "Test PASSED\n");
			else
				printf( "Test FAILED\n");			/* Reset to the original thresholds */
			printf( "Reset thresholds\n");
			rv = saHpiSensorThresholdsSet(sessionid, resourceid, sensornum, &senstbuff1);
			if (rv != SA_OK)
				goto out;
			/* Re-read threshold values */
			saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff2);
			goto out;
		}

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
	rv = saHpiSensorThresholdsSet(sessionid, resourceid, sensornum, &senstbuff1);
	if (rv != SA_OK) {
		printf( "saHpiSensorThresholdsSet error %d\n",rv);
		goto out;
	}

	/* Re-read threshold values */
	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff2);
	if (rv != SA_OK) {
		printf( "saHpiSensorThresholdsGet error %d\n",rv);
		goto out;
	}

	/* Display reset thresholds */ 
	printf( "   Reset\n");
	ShowThresh( &senstbuff2 );

out:
	printf( "Unsubscribe\n");
	rv = saHpiUnsubscribe( sessionid );
}  /*end DoEvent*/

/*
 * findmatch
 * returns offset of the match if found, or -1 if not found.
 */
static int
findmatch(char *buffer, int sbuf, char *pattern, int spattern, char figncase)
{
    int c, j, imatch;

    for (j = 0, imatch = 0; j < sbuf; j++) {
        if (sbuf - j < spattern && imatch == 0) return (-1);
        c = buffer[j];
        if (c == pattern[imatch]) {
            imatch++;
            if (imatch == spattern) return (++j - imatch);
        } else if (pattern[imatch] == '?') {  /*wildcard char*/
            imatch++;
            if (imatch == spattern) return (++j - imatch);
        } else if (figncase == 1) {
            if ((c & 0x5f) == (pattern[imatch] & 0x5f)) {
                imatch++;
                if (imatch == spattern) return (++j - imatch);
            } else
                imatch = 0;
        } else
            imatch = 0;
    }
    return (-1);
}                               /*end findmatch */

int
main(int argc, char **argv)
{
	int c;
	SaHpiSessionIdT sessionid;
	SaHpiDomainInfoT domainInfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiEntryIdT entryid;
	SaHpiEntryIdT nextentryid;
	SaHpiResourceIdT resourceid;
	SaHpiRdrT rdr;
	int firstpass = 1;
	int sensor_found = 0;

	printf("%s  ver %s\n", argv[0], progver);
	sensor_name = (char *)strdup(s_name);
	while ( (c = getopt( argc, argv,"ms:xz?")) != EOF )
		switch(c)  {
			case 'z': fxdebug = 1; /* fall thru to include next setting */
			case 'x':
				fdebug = 1;
				break;
			/*
			case 'l': slist = 1; break;
			*/
			case 'm': 
				sensor_name = (char *)strdup(sm_name);
				break;
			case 's':
				fsensor = 1;
				if (optarg)
					sensor_name = (char *)strdup(optarg);
				break;
			default:
				printf("Usage: %s [-mxz] [-s sensor_descriptor]\n", argv[0]);
				printf("   -s  Sensor descriptor\n");
				printf("   -m  use Mullins sensor descriptor\n");
				/*
				printf("   -l  Display entire sensor list\n");
				*/
				printf("   -x  Display debug messages\n");
				printf("   -z  Display extra debug messages\n");
				exit(1);
		}

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID ,&sessionid, NULL);

	if (rv != SA_OK) {
		printf("saHpiSessionOpen error %d\n",rv);
		exit(-1);
	}
 
	rv = saHpiDiscover(sessionid);

	if (fxdebug) printf("saHpiDiscover rv = %d\n",rv);

	rv = saHpiDomainInfoGet(sessionid,&domainInfo);

	if (fxdebug) printf("saHpiDomainInfoGet: rv = %d\n",rv);
	if (fdebug) printf("DomainInfo: UpdateCount = %x, UpdateTime = %lx\n",
		domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);
 
	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {
		rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if (rv == SA_OK) {
			/* walk the RDR list for this RPT entry */
			entryid = SAHPI_FIRST_ENTRY;
			rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
			resourceid = rptentry.ResourceId;
      
			if (fdebug) printf("rptentry[%d] resourceid=%d\n", rptentryid,resourceid);
			printf("Resource Tag: %s\n", rptentry.ResourceTag.Data);

			while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) {
				rv = saHpiRdrGet(sessionid,resourceid, entryid,&nextentryid, &rdr);

				if (fxdebug) printf("saHpiRdrGet[%d] rv = %d\n",entryid,rv);

				if (rv == SA_OK) {
					if (rdr.RdrType == SAHPI_SENSOR_RDR) { 
						/*type 2 includes sensor and control records*/
						rdr.IdString.Data[rdr.IdString.DataLength] = 0;

						if (findmatch(rdr.IdString.Data,rdr.IdString.DataLength,
								sensor_name, strlen(sensor_name),0) >= 0) {
							sensor_found = 1;
							printf( "%02d %s\t", rdr.RecordId, rdr.IdString.Data);
							DoEvent( sessionid, resourceid, &rdr.RdrTypeUnion.SensorRec);
							if (rv != SA_OK)
								printf( "Returned Error from DoEvent: rv=%d\n", rv);
							break;  /* done with rdr loop */
						}
					} 
				} else {
					printf( "saHpiRdrGet: Returned HPI Error: rv=%d\n", rv);
					break;
				};
				entryid = nextentryid;

				if (firstpass && entryid == SAHPI_LAST_ENTRY) {
					/* Not found yet, so try again, looking for any Fan */
					if (sensor_found)
						break;
					sensor_name = s_pattn;
					entryid = SAHPI_FIRST_ENTRY;
					firstpass = 0;
				}
			}  /*while rdr loop */

			if (entryid != SAHPI_LAST_ENTRY)
				break;  /*found one, done*/
			rptentryid = nextrptentryid;
		}  else
			break;
	}  /*while rpt loop*/
	rv = saHpiSessionClose(sessionid);
	return(0);
}
 /* end hpievent.c */
