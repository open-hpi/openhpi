/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Hu Yin     <hu.yin@intel.com>
 *     Racing Guo <racing.guo@intel.com>
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SaHpi.h>
#include <epath_utils.h>
#include "hpi_cmd.h"
#include "resource.h"
#include "printevent_utils.h"


static int help(int argc, char *argv[])
{
	struct command *cmd = NULL;
	int len;

	if (argc ==1) {
		int width = 0;

		printf("Available commands are: \n\n");
		for (cmd = commands; cmd->cmd != NULL; cmd++) {
			printf("%-20s", cmd->cmd);
			if ((++width % 4) == 0)
				printf("\n");
		}
		printf("\n");
        	return 0;
	}

	while (--argc > 0) {
		register char *arg;
		arg = *++argv;
		len = strlen(arg);
                for (cmd = commands; cmd->cmd != NULL; cmd++) {
                        if (strncmp(cmd->cmd, arg, len) == 0)
				break;
                }
		if (cmd->cmd == NULL)
			printf("Invalid help command %s\n", arg);
		else
			printf("%s\n", cmd->help);
	}

	return 0;
};

static int sa_event(int argc, char *argv[])
{
	if (argc == 1) {
		printf("Event display: %s\n", prt_flag?"Enable":"Disable"); 
	} else if (argc == 2) {
		if (strcmp(argv[1], "enable") == 0) {
			prt_flag = 1;
			printf("Event display enable successfully\n"); 
		} else if (strcmp(argv[1], "disable") == 0) {
			prt_flag = 0;
			printf("Event display disable successfully\n"); 
		} else {
			return HPI_SHELL_PARM_ERROR;
		}
	}

	return SA_OK;
}

static int sa_list_sensor(void)
{
        SaErrorT rv = SA_OK;
        SaHpiEntryIdT rptentryid;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT nextrptentryid;
        SaHpiEntryIdT entryid;
        SaHpiEntryIdT nextentryid;
        SaHpiResourceIdT resourceid;
        SaHpiRdrT rdr;
        SaHpiEntityPathT ep_target;
        char *ep_string = NULL;

        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (rv == SA_OK) {
                        /* Walk the RDR list for this RPT entry */

                        /* Filter by entity path if specified */
                        if (ep_string && !oh_cmp_ep(&ep_target,&(rptentry.ResourceEntity))) {
                                rptentryid = nextrptentryid;
                                continue;
                        }

                        entryid = SAHPI_FIRST_ENTRY;
                        resourceid = rptentry.ResourceId;
                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
                        while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
                        {
                                rv = saHpiRdrGet(sessionid,resourceid,
                                                 entryid,&nextentryid, &rdr);
                                if (rv == SA_OK) {
                                        if (rdr.RdrType == SAHPI_SENSOR_RDR &&
                                                rdr.RdrTypeUnion.SensorRec.Ignore != TRUE) {
						printf("Resource Id: %d, Sensor Id: %d\n",
							resourceid, rdr.RdrTypeUnion.SensorRec.Num);
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
	return rv;
}

static int sa_sen_evt_get(
	SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum )
{
	SaErrorT rv;
	SaHpiSensorEvtEnablesT enables;

	rv = saHpiSensorEventEnablesGet(
			sessionid, resourceid, sensornum, &enables);
	if (rv != SA_OK) {
		printf("saHpiSensorEventEnablesGet error %d\n",rv); 
		return -1; 
	}

	printf("Sensor Event Enables: \n");
	printf("  Sensor Status: %x\n", enables.SensorStatus);
	printf("  Assert Events: %x\n", enables.AssertEvents);
	printf("  Deassert Events: %x\n", enables.DeassertEvents);

        return SA_OK;
}

static int sa_get_thres(
	SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum )
{
	SaErrorT rv;
	SaHpiSensorThresholdsT senstbuff;

	rv = saHpiSensorThresholdsGet(sessionid, resourceid,
					sensornum, &senstbuff);
	printf("Supported Thresholds:\n");
	if (senstbuff.LowCritical.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Lower Critical Threshold(lc): %5.2f\n",
			senstbuff.LowCritical.Interpreted.Value.SensorFloat32);
	if (senstbuff.LowMajor.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Lower Major Threshold(la): %5.2f\n",
			senstbuff.LowMajor.Interpreted.Value.SensorFloat32);
	if (senstbuff.LowMinor.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Lower Minor Threshold(li): %5.2f\n",
			senstbuff.LowMinor.Interpreted.Value.SensorFloat32);

	if (senstbuff.UpCritical.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Upper Critical Threshold(uc): %5.2f\n",
			senstbuff.UpCritical.Interpreted.Value.SensorFloat32);
	if (senstbuff.UpMajor.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Upper Major Threshold(ua): %5.2f\n",
			senstbuff.UpMajor.Interpreted.Value.SensorFloat32);
	if (senstbuff.UpMinor.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Upper Minor Threshold(ui): %5.2f\n",
			senstbuff.UpMinor.Interpreted.Value.SensorFloat32);

	if (senstbuff.PosThdHysteresis.ValuesPresent & SAHPI_SRF_RAW)
		printf("  Positive Threshold Hysteresis in RAW\n");
	if (senstbuff.PosThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Positive Threshold Hysteresis(ph): %5.2f\n",
			senstbuff.PosThdHysteresis.Interpreted.Value.SensorFloat32);
	if (senstbuff.NegThdHysteresis.ValuesPresent & SAHPI_SRF_RAW)
		printf("  Negative Threshold Hysteresis in RAW\n");
	if (senstbuff.NegThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("  Negative Threshold Hysteresis(nh): %5.2f\n",
			senstbuff.NegThdHysteresis.Interpreted.Value.SensorFloat32);

        return SA_OK;
}

static int sa_show_sensor(
        SaHpiResourceIdT resourceid,
        SaHpiSensorNumT sensornum )
{
        SaHpiSensorReadingT reading;
        SaHpiSensorReadingT conv_reading;
        SaErrorT rv;

        rv = saHpiSensorReadingGet(sessionid,resourceid, sensornum, &reading);
        if (rv != SA_OK)  {
                printf("ReadingGet ret=%d\n", rv);
                return rv;
        }

        if ((reading.ValuesPresent & SAHPI_SRF_INTERPRETED) == 0) {
                if ((reading.ValuesPresent & SAHPI_SRF_RAW) == 0) {
                        /* no raw or interpreted, so just show event status */
                        /* This is a Compact Sensor */
                        if (reading.ValuesPresent & SAHPI_SRF_EVENT_STATE)
                                printf(" = %x %x\n", reading.EventStatus.SensorStatus,
                                       reading.EventStatus.EventStatus);
                        else printf(" = event-only sensor\n");
                        return SA_OK;
                } else {
                        /* have raw, but not interpreted, so try convert. */
                        rv = saHpiSensorReadingConvert(sessionid, resourceid, sensornum,
                                                       &reading, &conv_reading);
                        if (rv != SA_OK) {
                                printf("raw=%x conv_ret=%d\n", reading.Raw, rv);
                                /* printf("conv_rv=%s\n", decode_error(rv)); */
                                return rv;
                        }
                        else {
                                reading.Interpreted.Type = conv_reading.Interpreted.Type;
                                if (reading.Interpreted.Type == SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER)
                                {
                                        memcpy(reading.Interpreted.Value.SensorBuffer,
                                               conv_reading.Interpreted.Value.SensorBuffer,
                                               4); /* SAHPI_SENSOR_BUFFER_LENGTH); */
                                        /* IPMI 1.5 only returns 4 bytes */
                                } else
                                        reading.Interpreted.Value.SensorUint32 =
                                                conv_reading.Interpreted.Value.SensorUint32;
                        }
                }
        }
        switch(reading.Interpreted.Type)
        {
        case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
                printf("Value: %5.2f \n",
                       reading.Interpreted.Value.SensorFloat32);
                break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
                printf("Value: %d \n",
                       reading.Interpreted.Value.SensorUint32);
                break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
                printf("Value: %02x %02x %02x %02x\n",
                       reading.Interpreted.Value.SensorBuffer[0],
                       reading.Interpreted.Value.SensorBuffer[1],
                       reading.Interpreted.Value.SensorBuffer[2],
                       reading.Interpreted.Value.SensorBuffer[3]);
                break;
        default:
                printf("Value: %x (itype=%x)\n",
                       reading.Interpreted.Value.SensorUint32,
                       reading.Interpreted.Type);
        }

#if 0
	printf("Sensor Status: ");
        if (reading.EventStatus.SensorStatus & SAHPI_SENSTAT_EVENTS_ENABLED) {
                printf("Event Enabled.\n");
	} else if (reading.EventStatus.SensorStatus & SAHPI_SENSTAT_SCAN_ENABLED) {
                printf("Scan Enabled.\n");
	} else if (reading.EventStatus.SensorStatus & SAHPI_SENSTAT_BUSY) {
                printf("Busy.\n");
	} else {
                printf("Unknown.\n");
	}
#endif

	sa_sen_evt_get(resourceid, sensornum);
	sa_get_thres(resourceid, sensornum);

	return SA_OK;
}

static int sa_sen_evt_set(
	SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum )
{
	SaErrorT rv;
	SaHpiSensorEvtEnablesT enables;

	enables.AssertEvents = 0x0400;
	enables.DeassertEvents = 0x0400;
	rv = saHpiSensorEventEnablesSet(
			sessionid, resourceid, sensornum, &enables);
	if (rv != SA_OK) {
		printf("saHpiSensorEventEnablesSet error %d\n",rv); 
		return -1; 
	}
	printf("Sensor Event Enables Successfully\n");

	rv = saHpiSensorEventEnablesGet(
			sessionid, resourceid, sensornum, &enables);
	if (rv != SA_OK) {
		printf("saHpiSensorEventEnablesGet error %d\n",rv); 
		return -1; 
	}

	printf("Sensor Event Enables: \n");
	printf("  Sensor Status: %x\n", enables.SensorStatus);
	printf("  Assert Events: %x\n", enables.AssertEvents);
	printf("  Deassert Events: %x\n", enables.DeassertEvents);

	return SA_OK;
}

static int sa_set_thres(int argc, char *argv[])
{
	SaHpiResourceIdT resourceid;
	SaHpiSensorNumT sensornum; 
	SaErrorT rv;
	SaHpiSensorThresholdsT stbuff;
	int i;

	resourceid = (SaHpiResourceIdT)atoi(argv[1]);
	sensornum = (SaHpiResourceIdT)atoi(argv[2]);

	rv = saHpiSensorThresholdsGet(sessionid, resourceid,
					sensornum, &stbuff);
	if (rv != SA_OK) 
		printf("saHpiSensorThresholdsGet error %d\n",rv);

	for (i=3; i<argc; i+=2) {
		if (!argv[i+1]) {
	 		return HPI_SHELL_PARM_ERROR;
	 	}
		printf("%s", argv[i]);
		if (!strcmp(argv[i],"lc")) {
			stbuff.LowCritical.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.LowCritical.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"la")) {
			stbuff.LowMajor.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.LowMajor.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"li")) {
			stbuff.LowMinor.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.LowMinor.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"uc")) {
			stbuff.UpCritical.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.UpCritical.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"ua")) {
			stbuff.UpMajor.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.UpMajor.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"ui")) {
			stbuff.UpMinor.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.UpMinor.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"ph")) {
			stbuff.PosThdHysteresis.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.PosThdHysteresis.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else if (!strcmp(argv[i],"nh")) {
			stbuff.NegThdHysteresis.ValuesPresent = SAHPI_SRF_INTERPRETED;
			stbuff.NegThdHysteresis.Interpreted.Value.SensorFloat32 =
				(SaHpiFloat32T)atof(argv[i+1]);
		} else {
			return HPI_SHELL_PARM_ERROR;
		}
	}

	rv = saHpiSensorThresholdsSet(
			sessionid, resourceid, sensornum, &stbuff);
	if (rv != SA_OK) 
		printf("saHpiSensorThresholdsSet error %d\n",rv);
	else
		printf("Sensor Threshold Value Set Succeed.\n");

	return SA_OK;
}

static int sa_hotswap_stat(SaHpiResourceIdT resourceid)
{
	SaErrorT rv;
	SaHpiHsStateT state;

	rv = saHpiHotSwapStateGet(sessionid, resourceid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapStateGet error %d\n",rv);
		return -1;
	}

	printf("Current hot swap state of resource %d is:", resourceid);
	switch (state) {
	case SAHPI_HS_STATE_INACTIVE:
		printf("  Inactive.\n");
		break;
	case SAHPI_HS_STATE_INSERTION_PENDING:
		printf("  Insertion Pending.\n");
		break;
	case SAHPI_HS_STATE_ACTIVE_HEALTHY:
		printf("  Active and Healthy.\n");
		break;
	case SAHPI_HS_STATE_ACTIVE_UNHEALTHY:
		printf("  Active but Unhealthy.\n");
		break;
	case SAHPI_HS_STATE_EXTRACTION_PENDING:
		printf("  Extraction Pending.\n");
		break;
	case SAHPI_HS_STATE_NOT_PRESENT:
		printf("  Not Present.\n");
		break;
	default:
		printf("  Unknown.\n");
	}

	return SA_OK;
}

static int sa_power(int argc, char *argv[])
{
	SaErrorT rv;
	SaHpiResourceIdT resourceid;
	SaHpiHsPowerStateT state;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);

	if (argc == 2) goto L1;
	else if (!strcmp(argv[2], "on")) {
		state = SAHPI_HS_POWER_ON;
	} else if (!strcmp(argv[2], "off")) {
		state = SAHPI_HS_POWER_OFF;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

        rv = saHpiResourcePowerStateSet(sessionid, resourceid, state);
        if (rv != SA_OK) {
                printf("saHpiResourcePowerStateSet error %d\n",rv);
		return -1;
	}

L1:
        rv = saHpiResourcePowerStateGet(sessionid, resourceid, &state);
        if (rv != SA_OK) {
                printf("saHpiResourcePowerStateGet error %d\n",rv);
		return -1;
	}
	if (state == SAHPI_HS_POWER_ON) {
                printf("Resource %d is power on now.\n",resourceid);
	} else if (state == SAHPI_HS_POWER_OFF) {
                printf("Resource %d is power off now.\n",resourceid);
	}

	return SA_OK;
}

static int sa_reset(int argc, char *argv[])
{
	SaErrorT rv;
	SaHpiResourceIdT resourceid;
	SaHpiResetActionT state;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);

	if (argc == 2) goto L1;
	else if (!strcmp(argv[2], "cold")) {
		state = SAHPI_COLD_RESET;
	} else if (!strcmp(argv[2], "warm")) {
		state = SAHPI_WARM_RESET;
	} else if (!strcmp(argv[2], "assert")) {
		state = SAHPI_RESET_ASSERT;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

        rv = saHpiResourceResetStateSet(sessionid, resourceid, state);
        if (rv != SA_OK) {
                printf("saHpiResourceResetStateSet error %d\n",rv);
		return -1;
	}

L1:
	rv = saHpiResourceResetStateGet(sessionid, resourceid, &state);
	if (rv != SA_OK) {
		printf("saHpiResourceResetStateGet error %d\n",rv);
		return -1;
	}
	if (state == SAHPI_RESET_ASSERT) {
		printf("Entity's reset of %d is asserted now.\n",resourceid);
	} else if (state == SAHPI_RESET_DEASSERT) {
		printf("Entity's reset of %d is not asserted now.\n",resourceid);
	} else {
		printf("Entity's reset of %d is not setted now.\n",resourceid);
	}

	return SA_OK;
}

static int sa_set_tag(int argc, char *argv[])
{
	SaErrorT rv;
	SaHpiResourceIdT resourceid;
	SaHpiTextBufferT resourcetag;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);
	resourcetag.DataLength = strlen(argv[2]);
	resourcetag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	resourcetag.Language = SAHPI_LANG_ENGLISH;

	if (resourcetag.DataLength == 0)
		return HPI_SHELL_PARM_ERROR;
	else
		strcpy(resourcetag.Data, argv[2]);


	rv = saHpiResourceTagSet(sessionid,resourceid,&resourcetag);
	if (rv != SA_OK) {
		printf("saHpiResourceTagSet error = %d\n",rv);
		return -1;
	}

	return SA_OK;
}

static int sa_clear_evtlog(SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;

	rv = saHpiEventLogClear(sessionid,resourceid);
	if (rv != SA_OK) {
		printf("EventLog clear, error = %d\n",rv);
		return -1;
	}

	printf("EventLog successfully cleared\n");
	return SA_OK;
}

static int sa_show_evtlog(SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;
	SaHpiSelInfoT info;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiSelEntryIdT entryid;
	SaHpiSelEntryIdT nextentryid;
	SaHpiSelEntryIdT preventryid;
	SaHpiSelEntryT  sel;
	SaHpiRdrT rdr;
	char date[30];

	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
	{
		rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
			rptentryid = nextrptentryid;
			continue;  /* no SEL here, try next RPT */
		}
		if (rptentry.ResourceId == resourceid) {
			break;
		}
		rptentryid = nextrptentryid;
	}
	if ((rv != SA_OK) || (rptentryid == SAHPI_LAST_ENTRY)) {
		printf("The designated resource hasn't SEL.\n");
		return SA_OK;
	}

	rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);
	if (rv != SA_OK) {
		printf("saHpiEventLogInfoGet error %d\n",rv);
		return -1;
	}
	printf("EventLog entries=%d, size=%d, enabled=%d\n",
		info.Entries,info.Size,info.Enabled);
	saftime2str(info.UpdateTimestamp,date,30);
	printf("UpdateTime = %s, ", date);
	saftime2str(info.CurrentTime,date,30);
	printf("CurrentTime = %s\n", date);
	printf("Overflow = %d\n", info.OverflowFlag);
	printf("DeleteEntrySupported = %d\n", info.DeleteEntrySupported);

	if (info.Entries != 0){
		entryid = SAHPI_OLDEST_ENTRY;
		while (entryid != SAHPI_NO_MORE_ENTRIES)
		{
			rv = saHpiEventLogEntryGet(sessionid,resourceid,
					entryid,&preventryid,&nextentryid,
					&sel,&rdr,NULL);
			if (rv != SA_OK) {
				printf("saHpiEventLogEntryGet error %d\n",rv);
				return -1;
			}

			ShowSel(&sel, &rdr, &rptentry);
			preventryid = entryid;
			entryid = nextentryid;
		}
	} else
		printf("SEL is empty\n");

	return SA_OK;
}

/* functions for sa_show_inv */
#define NCT 25
char *chasstypes[NCT] = {
	"Not Defined", "Other", "Unknown", "Desktop", "Low Profile Desktop",
	"Pizza Box", "Mini Tower", "Tower", "Portable", "Laptop",
	"Notebook", "Handheld", "Docking Station", "All in One", "Subnotebook",
	"Space Saving", "Lunch Box", "Main Server", "Expansion", "SubChassis",
	"Buss Expansion Chassis", "Peripheral Chassis", "RAID Chassis", 
	"Rack-Mount Chassis", "New"
};
int i;
char outbuff[256];
SaHpiInventoryDataT *inv;
SaHpiInventChassisTypeT chasstype;
SaHpiInventGeneralDataT *dataptr;
SaHpiTextBufferT *strptr;

static void
fixstr(SaHpiTextBufferT *strptr)
{ 
	size_t datalen;
	
       	memset(outbuff,0,256);        
	if (!strptr) return;

 	datalen = strptr->DataLength;
	if (datalen > 0) {
		strncpy ((char *)outbuff, (char *)strptr->Data, datalen);
		outbuff[datalen] = 0;
	}

}

static void
prtchassinfo(void)
{
	int k;

	chasstype = (SaHpiInventChassisTypeT)inv->DataRecords[i]->RecordData.ChassisInfo.Type;
	for (k=0; k<NCT; k++) {
		if ((unsigned int)k == chasstype)
			printf( "\tChassis Type        : %s\n", chasstypes[k]);
	}	  

	dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.ChassisInfo.GeneralData;
	strptr=dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Manufacturer: %s\n", outbuff);

	strptr=dataptr->ProductName;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Name        : %s\n", outbuff);

	strptr=dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Version     : %s\n", outbuff);

	strptr=dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Model Number: %s\n", outbuff);

	strptr=dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Serial #    : %s\n", outbuff);

	strptr=dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Part Number : %s\n", outbuff);

	strptr=dataptr->FileId;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis FRU File ID : %s\n", outbuff);

	strptr=dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tChassis Asset Tag   : %s\n", outbuff);
	if (dataptr->CustomField[0] != 0)
	{
		if (dataptr->CustomField[0]->DataLength != 0)
		strncpy ((char *)outbuff, (char *)dataptr->CustomField[0]->Data,
						dataptr->CustomField[0]->DataLength);
		outbuff[dataptr->CustomField[0]->DataLength] = 0;
		printf( "\tChassis OEM Field   : %s\n", outbuff);
	}
}

static void
prtprodtinfo(void)
{
	int j;
	dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.ProductInfo;
	strptr=dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Manufacturer: %s\n", outbuff);

	strptr=dataptr->ProductName;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Name        : %s\n", outbuff);

	strptr=dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Version     : %s\n", outbuff);

	strptr=dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Model Number: %s\n", outbuff);

	strptr=dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Serial #    : %s\n", outbuff);

	strptr=dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Part Number : %s\n", outbuff);

	strptr=dataptr->FileId;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct FRU File ID : %s\n", outbuff);

	strptr=dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tProduct Asset Tag   : %s\n", outbuff);

	for (j = 0; j < 10; j++) {
		int ii;
		if (dataptr->CustomField[j] != NULL) {
			if ((dataptr->CustomField[j]->DataType == 0) &&
				(dataptr->CustomField[j]->DataLength == 16)) { /*binary GUID*/
				printf( "IPMI SystemGUID     : ");
				for (ii=0; ii< dataptr->CustomField[j]->DataLength; ii++)
					printf("%02x", dataptr->CustomField[j]->Data[ii]);
				printf("\n");
			} else {  /* other text field */
				dataptr->CustomField[j]->Data[
				dataptr->CustomField[j]->DataLength] = 0;
				printf( "\tProduct OEM Field   : %s\n",
				dataptr->CustomField[j]->Data);
			}
		} else /* NULL pointer */
			break;
	}/*end for*/
}

static void
prtboardinfo(void)
{
	int j;

	dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.BoardInfo;
	strptr=dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Manufacturer  : %s\n", outbuff);

	strptr=dataptr->ProductName;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Product Name  : %s\n", outbuff);

	strptr=dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Model Number  : %s\n", outbuff);

	strptr=dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Part Number   : %s\n", outbuff);

	strptr=dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Version       : %s\n", outbuff);

	strptr=dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Serial #      : %s\n", outbuff);

	strptr=dataptr->FileId;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard FRU File ID   : %s\n", outbuff);

	strptr=dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *)strptr);
	printf( "\tBoard Asset Tag     : %s\n", outbuff);

	for (j = 0; j < 10 && dataptr->CustomField[j] ; j++) {
		if (dataptr->CustomField[j]->DataLength != 0) {
                        strncpy ((char *)outbuff, (char *)dataptr->CustomField[j]->Data,
							dataptr->CustomField[j]->DataLength);
                        outbuff[dataptr->CustomField[j]->DataLength] = 0;
                        printf( "\tBoard OEM Field     : %s\n", outbuff);
                }
        }
}

static int sa_show_inv(SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;
	SaHpiSelEntryIdT entryid;
	SaHpiSelEntryIdT nextentryid;
	SaHpiRdrT rdr;
	SaHpiEirIdT eirid;
	SaHpiUint32T actualsize;
	int prodrecindx = 0;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRdrGet(sessionid,resourceid, entryid,&nextentryid, &rdr);
		if (rv != SA_OK) {
			printf("saHpiRdrGet error %d\n",rv);
			return -1;
		}

		if (rdr.RdrType == SAHPI_INVENTORY_RDR) {
			unsigned int invsize = 0;
			eirid = rdr.RdrTypeUnion.InventoryRec.EirId;
			rdr.IdString.Data[rdr.IdString.DataLength] = 0;
			rv = saHpiEntityInventoryDataRead(sessionid,resourceid,
							eirid, 0, NULL, &actualsize);
			invsize = actualsize;
			inv = (SaHpiInventoryDataT *)malloc(invsize);
			memset(inv, 0, invsize);
			rv = saHpiEntityInventoryDataRead(sessionid,resourceid,
							eirid, invsize, inv, &actualsize);
			if (rv != SA_OK) {
				printf("saHpiEntityInventoryDataRead error %d\n",rv);
				free(inv);
				return -1;
			}
			if (inv->Validity == SAHPI_INVENT_DATA_VALID) {
				for (i=0; inv->DataRecords[i]!=NULL; i++) {
					switch (inv->DataRecords[i]->RecordType)
					{
					case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
						printf("Internal Use\n");
						break;
					case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
						prodrecindx = 0;
						prtprodtinfo();
						break;
					case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
						prtchassinfo();
						break;
					case SAHPI_INVENT_RECTYPE_BOARD_INFO:
						prtboardinfo();
						break;
					case SAHPI_INVENT_RECTYPE_OEM:
						printf("OEM Record\n");
						break;
					default:
						printf("Invalid Invent Rec Type = %x\n", 
								inv->DataRecords[i]->RecordType);
						break;
					}
				}
			}
			free(inv);
		}
		entryid = nextentryid;
	}

	return SA_OK;
}

/* interface functions */
static int event(int argc, char *argv[])
{
        if ((argc != 1) && (argc != 2))
                return HPI_SHELL_PARM_ERROR;
        return sa_event(argc, argv);
}

static int list_sensor(int argc, char *argv[])
{
	return sa_list_sensor();
}

static int show_sensor(int argc, char *argv[])
{
	if (argc < 3)
		return HPI_SHELL_PARM_ERROR;
	return sa_show_sensor((SaHpiResourceIdT)atoi(argv[1]),
				(SaHpiSensorNumT)atoi(argv[2]));
}

static int get_thres(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
        return sa_get_thres((SaHpiResourceIdT)atoi(argv[1]),
                                (SaHpiSensorNumT)atoi(argv[2]));
}

static int set_thres(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
        return sa_set_thres(argc, argv);
}

static int sen_evt_get(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
        return sa_sen_evt_get((SaHpiResourceIdT)atoi(argv[1]),
                                (SaHpiSensorNumT)atoi(argv[2]));
}

static int sen_evt_set(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
        return sa_sen_evt_set((SaHpiResourceIdT)atoi(argv[1]),
                                (SaHpiSensorNumT)atoi(argv[2]));
}

static int hotswap_stat(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_hotswap_stat((SaHpiResourceIdT)atoi(argv[1]));
}

static int power(int argc, char *argv[])
{
	if ((argc != 2) && (argc != 3))
		return HPI_SHELL_PARM_ERROR;

	return sa_power(argc, argv);
}

static int reset(int argc, char *argv[])
{
	if ((argc != 2) && (argc != 3))
		return HPI_SHELL_PARM_ERROR;

	return sa_reset(argc, argv);
}

static int set_tag(int argc, char *argv[])
{
	if (argc != 3)
		return HPI_SHELL_PARM_ERROR;

	return sa_set_tag(argc, argv);
}

static int discovery(int argc, char *argv[])
{
	return sa_discover();
}

static int listres(int argc, char *argv[])
{	
	return sa_list_res();
}

static int clear_evtlog(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_clear_evtlog((SaHpiResourceIdT)atoi(argv[1]));
}

static int show_evtlog(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_show_evtlog((SaHpiResourceIdT)atoi(argv[1]));
}

static int show_inv(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_show_inv((SaHpiResourceIdT)atoi(argv[1]));
}

static int show_rpt(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_show_rpt((SaHpiResourceIdT)atoi(argv[1]));
}

static int show_rdr(int argc, char *argv[])
{	
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;

	return sa_show_rdr((SaHpiResourceIdT)atoi(argv[1]));
}

static int quit(int argc, char *argv[])
{
        printf("quit\n");
        close_session();
        exit(0);
}

/* command table */
const char clearevtloghelp[] = "clearevtlog: clear system event logs\n"    \
			"Usage: clearevtlog <resource id>";
const char dscvhelp[] = "dscv: discovery resources\n"                      \
			"Usage: dscv ";
const char eventhelp[] = "event: enable or disable event display on screen\n" \
			"Usage: event [enable|disable] ";
const char getthreshelp[] = "getthreshold: get sensor threshold values\n"  \
			"Usage: getthreshold <resource id> <sensor id>";
const char helphelp[] = "help: help information for OpenHPI commands\n"    \
			"Usage: help [optional commands]";
const char hsstathelp[] = "hotswapstat: retrieve hot swap state of a resource\n" \
			"Usage: hotswapstat <resource id> ";
const char lreshelp[] = "lsres: list resources\n"                          \
			"Usage: lsres ";
const char lsorhelp[] = "lsensor: list sensors\n"                          \
			"Usage: lsensor ";
const char powerhelp[] = "power: power the resource on(off)\n"             \
			"Usage: power <resource id> [on|off]";
const char quithelp[] = "quit: close session and quit console\n"           \
			"Usage: quit ";
const char resethelp[] = "reset: perform specified reset on the entity\n"  \
			"Usage: reset <resource id> [cold|warm|assert]";
const char senevtgethelp[] = "senevtget: get sensor event status\n"        \
			"Usage: senevtget <resource id> <sensor id>";
const char senevtsethelp[] = "senevtelb: enable sensor event message\n"    \
			"Usage: senevtelb <resource id> <sensor id>";
const char setthreshelp[] = "setthreshold: set sensor threshold values\n"  \
			"Usage: setthreshold <resource id> <sensor id>\n"  \
			"                    [lc val] [la val] [li val]\n" \
			"                    [uc val] [ua val] [ui val]\n" \
			"                    [ph val] [nh val]";
const char settaghelp[] = "settag: set tag for a particular resource \n"   \
			"Usage: settag <resource id> <tag string>";
const char showrdrhelp[] = "showrdr: show resource data record\n"          \
			"Usage: showrdr <resource id>";
const char showevtloghelp[] = "showevtlog: show system event logs\n"       \
			"Usage: showevtlog <resource id>";
const char showinvhelp[] = "showinv: show inventory data of a resource\n"  \
			"Usage: showinv <resource id>";
const char showrpthelp[] = "showrpt: show resource information\n"          \
			"Usage: showrpt <resource id>";
const char showsorhelp[] = "showsensor: show sensor information\n"         \
			"Usage: showsensor <resource id> <sensor id>";

struct command commands[] = {
    { "clearevtlog",	clear_evtlog,		clearevtloghelp },
    { "dscv",		discovery,		dscvhelp },
    { "event",		event,			eventhelp },
    { "gethreshold",	get_thres,		getthreshelp },
    { "help",		help,			helphelp },
    { "hotswapstat",	hotswap_stat,		hsstathelp },
    { "lsres",		listres,		lreshelp },
    { "lsensor",	list_sensor,		lsorhelp },
    { "power",		power,			powerhelp },
    { "quit",		quit,			quithelp },
    { "reset",		reset,			resethelp },
    { "senevtget",	sen_evt_get,		senevtgethelp },
    { "senevtelb",	sen_evt_set,		senevtsethelp },
    { "sethreshold",	set_thres,		setthreshelp },
    { "settag",		set_tag,		settaghelp },
    { "showevtlog",	show_evtlog,		showevtloghelp },
    { "showinv",	show_inv,		showinvhelp },
    { "showrdr",	show_rdr,		showrdrhelp },
    { "showrpt",	show_rpt,		showrpthelp },
    { "showsensor",	show_sensor,		showsorhelp },
    { "?",		help,			helphelp },
    { NULL,		NULL,			NULL }
};
