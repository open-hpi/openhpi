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
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <hpi_ui.h>
#include "hpi_cmd.h"
#include "resource.h"

#define READ_BUF_SIZE	1024

int ui_print(char *Str)
{
	printf("%s", Str);
	return(0);
}

static int get_int_param(char *mes, int *val)
{
	char	buf[READ_BUF_SIZE];

	printf("%s", mes);
	fgets(buf, READ_BUF_SIZE, stdin);
	return(sscanf(buf, "%d", val));
}

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
		} else if (strcmp(argv[1], "short") == 0) {
			show_event_short = 1;
			prt_flag = 1;
			printf("Event short display enable successfully\n"); 
		} else if (strcmp(argv[1], "full") == 0) {
			show_event_short = 0;
			prt_flag = 1;
			printf("Event full display enable successfully\n"); 
		} else {
			return HPI_SHELL_PARM_ERROR;
		}
	}

	return SA_OK;
}

static int sa_sen_evt_set(
	SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum )
{
	SaErrorT rv;
	SaHpiSensorEventMaskActionT	enables;
	SaHpiEventStateT		assert;
	SaHpiEventStateT		deassert;
	SaHpiBoolT			status;

	assert = 0x0400;
	deassert = 0x0400;
	enables = SAHPI_SENS_ADD_EVENTS_TO_MASKS;
	rv = saHpiSensorEventMasksSet(
			sessionid, resourceid, sensornum, enables, assert, deassert);
	if (rv != SA_OK) {
		printf("saHpiSensorEventMasksSet error %d\n",rv); 
		return -1; 
	}
	printf("Sensor Event Enables Successfully\n");

	rv = saHpiSensorEventEnableGet(
			sessionid, resourceid, sensornum, &status);
	if (rv != SA_OK) {
		printf("saHpiSensorEventEnableGet error %d\n",rv); 
		return -1; 
	}
	rv = saHpiSensorEventMasksGet(
			sessionid, resourceid, sensornum, &assert, &deassert);
	if (rv != SA_OK) {
		printf("saHpiSensorEventMasksGet error %d\n",rv); 
		return -1; 
	}

	printf("Sensor Event Masks: \n");
	printf("  Sensor Status: %x\n", status);
	printf("  Assert Events: %x\n", assert);
	printf("  Deassert Events: %x\n", deassert);

	return SA_OK;
}

static void Set_thres_value(SaHpiSensorReadingT *item, double value)
{
	item->IsSupported = 1;
	item->Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
	item->Value.SensorFloat64 = value;
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
			Set_thres_value(&(stbuff.LowCritical),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"la")) {
			Set_thres_value(&(stbuff.LowMajor),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"li")) {
			Set_thres_value(&(stbuff.LowMinor),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"uc")) {
			Set_thres_value(&(stbuff.UpCritical),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"ua")) {
			Set_thres_value(&(stbuff.UpMajor),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"ui")) {
			Set_thres_value(&(stbuff.UpMinor),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"ph")) {
			Set_thres_value(&(stbuff.PosThdHysteresis),
				(SaHpiFloat64T)atof(argv[i+1]));
		} else if (!strcmp(argv[i],"nh")) {
			Set_thres_value(&(stbuff.NegThdHysteresis),
				(SaHpiFloat64T)atof(argv[i+1]));
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

static int sa_show_hs_ind(SaHpiResourceIdT resourceid)
{
	SaErrorT rv;
	SaHpiHsIndicatorStateT state;

	rv = saHpiHotSwapIndicatorStateGet(sessionid, resourceid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapStateGet error %s\n", oh_lookup_error(rv));
		return -1;
	}

	printf("Current HS Indicator for resource %d is:", resourceid);
	switch(state) {
		case SAHPI_HS_INDICATOR_OFF:
			printf(" OFF.\n");
			break;
		case SAHPI_HS_INDICATOR_ON:
			printf(" ON.\n");
			break;
		default:
			printf(" Unknown.\n");
	}
	return SA_OK;
}
	
static int sa_hotswap_stat(SaHpiResourceIdT resourceid)
{
	SaErrorT rv;
	SaHpiHsStateT state;

	rv = saHpiHotSwapStateGet(sessionid, resourceid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapStateGet error %s\n", oh_lookup_error(rv));
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
	case SAHPI_HS_STATE_ACTIVE:
		printf("  Active.\n");
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
	SaHpiPowerStateT state;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);

	if (argc == 2) goto L1;
	else if (!strcmp(argv[2], "on")) {
		state = SAHPI_POWER_ON;
	} else if (!strcmp(argv[2], "off")) {
		state = SAHPI_POWER_OFF;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

        rv = saHpiResourcePowerStateSet(sessionid, resourceid, state);
        if (rv != SA_OK) {
                printf("saHpiResourcePowerStateSet error %s\n", oh_lookup_error(rv));
		return -1;
	}

L1:
        rv = saHpiResourcePowerStateGet(sessionid, resourceid, &state);
        if (rv != SA_OK) {
                printf("saHpiResourcePowerStateGet error %s\n", oh_lookup_error(rv));
		return -1;
	}
	if (state == SAHPI_POWER_ON) {
                printf("Resource %d is power on now.\n",resourceid);
	} else if (state == SAHPI_POWER_OFF) {
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
                printf("saHpiResourceResetStateSet error %s\n", oh_lookup_error(rv));
		return -1;
	}

L1:
	rv = saHpiResourceResetStateGet(sessionid, resourceid, &state);
	if (rv != SA_OK) {
		printf("saHpiResourceResetStateGet error %s\n", oh_lookup_error(rv));
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
	resourcetag.DataType = SAHPI_TL_TYPE_TEXT;
	resourcetag.Language = SAHPI_LANG_ENGLISH;

	if (resourcetag.DataLength == 0)
		return HPI_SHELL_PARM_ERROR;
	else
		strcpy((char *)(resourcetag.Data), argv[2]);


	rv = saHpiResourceTagSet(sessionid,resourceid,&resourcetag);
	if (rv != SA_OK) {
		printf("saHpiResourceTagSet error = %s\n", oh_lookup_error(rv));
		return -1;
	}

	return SA_OK;
}

static int sa_clear_evtlog(SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;

	rv = saHpiEventLogClear(sessionid,resourceid);
	if (rv != SA_OK) {
		printf("EventLog clear, error = %s\n", oh_lookup_error(rv));
		return -1;
	}

	printf("EventLog successfully cleared\n");
	return SA_OK;
}


#ifdef MY   // my

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
SaHpiTextBufferT *strptr;

static void
fixstr(SaHpiTextBufferT *strptr)
{ 
	size_t datalen;
	show_short_event
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
#endif

static int sa_show_inv(SaHpiResourceIdT resourceid)
{
	SaErrorT		rv = SA_OK, rva, rvf;
	SaHpiEntryIdT		rdrentryid;
	SaHpiEntryIdT		nextrdrentryid;
	SaHpiRdrT		rdr;
	SaHpiIdrIdT		idrid;
	SaHpiIdrInfoT		idrInfo;
	SaHpiEntryIdT		areaId;
	SaHpiEntryIdT		nextareaId;
	SaHpiIdrAreaTypeT	areaType;
	int			numAreas;
	SaHpiEntryIdT		fieldId;
	SaHpiEntryIdT		nextFieldId;
	SaHpiIdrFieldTypeT	fieldType;
	SaHpiIdrFieldT		thisField;
	SaHpiIdrAreaHeaderT	areaHeader;

	rdrentryid = SAHPI_FIRST_ENTRY;
	while (rdrentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRdrGet(sessionid, resourceid, rdrentryid, &nextrdrentryid, &rdr);
		if (rv != SA_OK) {
			printf("saHpiRdrGet error %s\n", oh_lookup_error(rv));
			return -1;
		}

		if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
			rdrentryid = nextrdrentryid;
			continue;
		};
		
		idrid = rdr.RdrTypeUnion.InventoryRec.IdrId;
		rv = saHpiIdrInfoGet(sessionid, resourceid, idrid, &idrInfo);
		if (rv != SA_OK) {
			printf("saHpiIdrInfoGet error %s\n", oh_lookup_error(rv));
			return -1;
		}
		
		numAreas = idrInfo.NumAreas;
		areaType = SAHPI_IDR_AREATYPE_UNSPECIFIED;
		areaId = SAHPI_FIRST_ENTRY; 
		while (areaId != SAHPI_LAST_ENTRY) {
			rva = saHpiIdrAreaHeaderGet(sessionid, resourceid, idrInfo.IdrId,
				areaType, areaId, &nextareaId, &areaHeader);
			if (rva != SA_OK) {
				printf("saHpiIdrAreaHeaderGet error %s\n", oh_lookup_error(rva));
				break;
			}
			oh_print_idrareaheader(&areaHeader, 2);

			fieldType = SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
			fieldId = SAHPI_FIRST_ENTRY;
			while (fieldId != SAHPI_LAST_ENTRY) {
				rvf = saHpiIdrFieldGet(sessionid, resourceid,
						idrInfo.IdrId, areaHeader.AreaId, 
						fieldType, fieldId, &nextFieldId,
						&thisField);
				if (rvf != SA_OK) {
					printf("saHpiIdrFieldGet error %s\n", oh_lookup_error(rvf));
					break;
				}
				oh_print_idrfield(&thisField, 4);
				fieldId = nextFieldId;
			}
			areaId = nextareaId;
		}
		rdrentryid = nextrdrentryid;
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
	sensor_list(sessionid, ui_print);
	return SA_OK;
}

static int show_sensor1(int argc, char *argv[])
{
	if (argc < 3)
		return HPI_SHELL_PARM_ERROR;
	return show_sensor(sessionid, (SaHpiResourceIdT)atoi(argv[1]),
				(SaHpiSensorNumT)atoi(argv[2]), ui_print);
}

static int get_thres(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
	return show_threshold(sessionid, (SaHpiResourceIdT)atoi(argv[1]),
			(SaHpiSensorNumT)atoi(argv[2]), ui_print);
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
	return show_sensor_status(sessionid, (SaHpiResourceIdT)atoi(argv[1]),
		(SaHpiSensorNumT)atoi(argv[2]), ui_print);
}

static int sen_evt_set(int argc, char *argv[])
{
        if (argc < 3)
                return HPI_SHELL_PARM_ERROR;
        return sa_sen_evt_set((SaHpiResourceIdT)atoi(argv[1]),
                                (SaHpiSensorNumT)atoi(argv[2]));
}

static int show_hs_ind(int argc, char *argv[])
{
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;

	return sa_show_hs_ind((SaHpiResourceIdT)atoi(argv[1]));
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
	return show_rpt_list(Domain, ui_print);
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
			
	return show_event_log(sessionid, (SaHpiResourceIdT)atoi(argv[1]),
		show_event_short, ui_print);
}

static int show_inv(int argc, char *argv[])
{
#ifdef MY   // my
	fixstr((SaHpiTextBufferT *)S);
	prtchassinfo();
	prtprodtinfo();
	prtboardinfo();
#endif
	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;
			
	return sa_show_inv((SaHpiResourceIdT)atoi(argv[1]));
}

static int show_rpt(int argc, char *argv[])
{
	Rpt_t			*Rpt;
	int			i, res;
	SaHpiResourceIdT	Id;

	if (argc < 2) {
		i = get_int_param("RPT ID ==> ", &res);
		if (i == 1) Id = (SaHpiResourceIdT)res;
		else return HPI_SHELL_PARM_ERROR;
	} else {
		Id = (SaHpiResourceIdT)atoi(argv[1]);
	};
	Rpt = get_rpt(Domain, Id);
	if (Rpt != (Rpt_t *)NULL)
		show_Rpt(Rpt, ui_print);
	return (SA_OK);
}

static int show_rdr(int argc, char *argv[])
{
	Rdr_t	*Rdr;

	if (argc < 3)
		return HPI_SHELL_PARM_ERROR;

	Rdr = get_rdr(Domain, (SaHpiResourceIdT)atoi(argv[1]),
		(SaHpiSensorNumT)atoi(argv[2]));
	if (Rdr != (Rdr_t *)NULL)
		show_Rdr(Rdr, ui_print);
	return SA_OK;
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
			"Usage: event [enable|disable|short|full] ";
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
			"Usage: showrpt [<resource id>]";
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
    { "showsensor",	show_sensor1,		showsorhelp },
    { "hotswap_ind",	show_hs_ind,		NULL },
    { "?",		help,			helphelp },
    { NULL,		NULL,			NULL }
};
