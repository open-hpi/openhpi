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
#include <ctype.h>
#include <hpi_ui.h>
#include "hpi_cmd.h"

#define READ_BUF_SIZE	1024

#define SEN_AV_COM 	"   Available commands:\n" \
			"	disable  - set sensor disable\n" \
			"	enable   - set sensor enable\n" \
			"	evtenb   - set event enable\n" \
			"	evtdis   - set event disable\n" \
			"	maskadd  - mask add\n" \
			"	maskrm   - mask remove\n" \
			"	setthres - set threshold\n" \
			"	show     - show sensor status\n" \
			"	help	 - command list\n" \
			"	q | quit - exit"

static char	input_buffer[READ_BUF_SIZE];	// command line buffer
static char	*input_buf_ptr = input_buffer;	// current pointer in input_buffer

int ui_print(char *Str)
{
	printf("%s", Str);
	return(0);
}

static void clear_input(void)
{
	memset(input_buffer, 0, READ_BUF_SIZE);
	input_buf_ptr = input_buffer;
}

static int get_int_param(char *mes, int *val, char *string, int len)
{
	int	res, skip = 0;

	while (isblank(*input_buf_ptr)) input_buf_ptr++;
	if (strlen(input_buf_ptr) == 0) {
		printf("%s", mes);
		fgets(input_buffer, READ_BUF_SIZE, stdin);
		for (res = 0; res < READ_BUF_SIZE; res++)
			if (input_buffer[res] == '\n') input_buffer[res] = 0;
		input_buf_ptr = input_buffer;
	};
	while (isblank(*input_buf_ptr)) input_buf_ptr++;
	if (*input_buf_ptr == 0) return(-1);
	if (isdigit(*input_buf_ptr)) {
		res = sscanf(input_buf_ptr, "%d", val);
		if (res == 1) skip = 1;
		else res = -1;
	} else {
		if (string == (char *)NULL) return(-1);
		memset(string,  0, len);
		strncpy(string, input_buf_ptr, len);
		while (! isblank(*string) && (*string != 0)) string++;
		*string = 0;
		skip = 1;
		res = 0;
	};
		
	if (skip) {
		while (! isblank(*input_buf_ptr) && (*input_buf_ptr != 0))
			input_buf_ptr++;
	};
	return(res);
}

int help(int argc, char *argv[])
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

static int debugset(int argc, char *argv[])
{
	char	*val;

	if (argc == 1) {
		val = getenv("OPENHPI_DEBUG");
		if (val == (char *)NULL) val = "NO";
		printf("OPENHPI_DEBUG=%s\n", val);
		return(SA_OK);
	};
	if (strcmp(argv[1], "on") == 0)
		val = "YES";
	else if (strcmp(argv[1], "off") == 0)
		val = "NO";
	else
		return HPI_SHELL_PARM_ERROR;
	setenv("OPENHPI_DEBUG", val, 1);

	return SA_OK;
}

static void Set_thres_value(SaHpiSensorReadingT *item, double value)
{
	item->IsSupported = 1;
	item->Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
	item->Value.SensorFloat64 = value;
}

static int sa_show_hs_ind(SaHpiResourceIdT resourceid)
{
	SaErrorT rv;
	SaHpiHsIndicatorStateT state;

	rv = saHpiHotSwapIndicatorStateGet(Domain->sessionId, resourceid, &state);
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

	rv = saHpiHotSwapStateGet(Domain->sessionId, resourceid, &state);
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
	int	do_set = 1;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);

	if (argc == 2) do_set = 0;
	else if (!strcmp(argv[2], "on")) {
		state = SAHPI_POWER_ON;
	} else if (!strcmp(argv[2], "off")) {
		state = SAHPI_POWER_OFF;
	} else if (!strcmp(argv[2], "cycle")) {
		state = SAHPI_POWER_CYCLE;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

	if (do_set) {
	        rv = saHpiResourcePowerStateSet(Domain->sessionId, resourceid, state);
       		if (rv != SA_OK) {
			printf("saHpiResourcePowerStateSet error %s\n",
				oh_lookup_error(rv));
			return -1;
		};
		return SA_OK;
	}

        rv = saHpiResourcePowerStateGet(Domain->sessionId, resourceid, &state);
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
	int	do_set = 1;

        resourceid = (SaHpiResourceIdT)atoi(argv[1]);

	if (argc == 2) do_set = 0;
	else if (!strcmp(argv[2], "cold")) {
		state = SAHPI_COLD_RESET;
	} else if (!strcmp(argv[2], "warm")) {
		state = SAHPI_WARM_RESET;
	} else if (!strcmp(argv[2], "assert")) {
		state = SAHPI_RESET_ASSERT;
	} else if (!strcmp(argv[2], "deassert")) {
		state = SAHPI_RESET_DEASSERT;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

	if (do_set) {
		rv = saHpiResourceResetStateSet(Domain->sessionId, resourceid, state);
		if (rv != SA_OK) {
			printf("saHpiResourceResetStateSet error %s\n",
				oh_lookup_error(rv));
			return -1;
		}
	}

	rv = saHpiResourceResetStateGet(Domain->sessionId, resourceid, &state);
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

static int sa_clear_evtlog(SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;

	rv = saHpiEventLogClear(Domain->sessionId, resourceid);
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
		rv = saHpiRdrGet(Domain->sessionId, resourceid, rdrentryid,
			&nextrdrentryid, &rdr);
		if (rv != SA_OK) {
			printf("saHpiRdrGet error %s\n", oh_lookup_error(rv));
			return -1;
		}

		if (rdr.RdrType != SAHPI_INVENTORY_RDR) {
			rdrentryid = nextrdrentryid;
			continue;
		};
		
		idrid = rdr.RdrTypeUnion.InventoryRec.IdrId;
		rv = saHpiIdrInfoGet(Domain->sessionId, resourceid, idrid, &idrInfo);
		if (rv != SA_OK) {
			printf("saHpiIdrInfoGet error %s\n", oh_lookup_error(rv));
			return -1;
		}
		
		numAreas = idrInfo.NumAreas;
		areaType = SAHPI_IDR_AREATYPE_UNSPECIFIED;
		areaId = SAHPI_FIRST_ENTRY; 
		while (areaId != SAHPI_LAST_ENTRY) {
			rva = saHpiIdrAreaHeaderGet(Domain->sessionId, resourceid,
				idrInfo.IdrId, areaType, areaId, &nextareaId, &areaHeader);
			if (rva != SA_OK) {
				printf("saHpiIdrAreaHeaderGet error %s\n",
					oh_lookup_error(rva));
				break;
			}
			oh_print_idrareaheader(&areaHeader, 2);

			fieldType = SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
			fieldId = SAHPI_FIRST_ENTRY;
			while (fieldId != SAHPI_LAST_ENTRY) {
				rvf = saHpiIdrFieldGet(Domain->sessionId, resourceid,
						idrInfo.IdrId, areaHeader.AreaId, 
						fieldType, fieldId, &nextFieldId,
						&thisField);
				if (rvf != SA_OK) {
					printf("saHpiIdrFieldGet error %s\n",
						oh_lookup_error(rvf));
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
	sensor_list(Domain->sessionId, ui_print);
	return SA_OK;
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
	SaHpiResourceIdT	resid = 0;
	SaHpiTextBufferT	tbuf;
	int			res, i;
	char			buf[SAHPI_MAX_TEXT_BUFFER_LENGTH + 1];
	char			*str;
	SaErrorT		rv;
	SaHpiRptEntryT		rpt_entry;
	Rpt_t			tmp_rpt;

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, resid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res, (char *)NULL, 0);
		if (i == 1) resid = (SaHpiResourceIdT)res;
		else return SA_OK;
	} else {
		resid = (SaHpiResourceIdT)atoi(argv[1]);
	};
	printf("New tag: ");
	fgets(buf, SAHPI_MAX_TEXT_BUFFER_LENGTH, stdin);
	for (res = 0; res < SAHPI_MAX_TEXT_BUFFER_LENGTH; res++)
		if (buf[res] == '\n') buf[res] = 0;
	str = buf;
	while (*str == ' ') str++;
	i = strlen(str);
	while ((i > 0) && (str[i - 1] == ' ')) i--;
	str[i] = 0;
	if (i == 0) {
		printf("Invalid tag: %s\n", buf);
		return(-1);
	};
	strcpy(tbuf.Data, str);
	tbuf.DataType = SAHPI_TL_TYPE_TEXT;
	tbuf.Language = SAHPI_LANG_ENGLISH;
	tbuf.DataLength = i;
	rv = saHpiResourceTagSet(Domain->sessionId, resid, &tbuf);
	if (rv != SA_OK) {
		printf("saHpiResourceTagSet error = %s\n", oh_lookup_error(rv));
		return -1;
	};
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (SA_OK);
}

#define	SEV_BUF_SIZE	32

typedef struct {
	char		*name;
	SaHpiSeverityT	val;
} Sev_def_t;

static Sev_def_t Sev_array[] = {
	{"crit", 	SAHPI_CRITICAL},
	{"maj", 	SAHPI_MAJOR},
	{"min", 	SAHPI_MINOR},
	{"inf", 	SAHPI_INFORMATIONAL},
	{"ok", 		SAHPI_OK},
	{"debug", 	SAHPI_DEBUG},
	{"all", 	SAHPI_ALL_SEVERITIES},
	{NULL,	 	0}
};

static int set_sever(int argc, char *argv[])
{
	SaHpiResourceIdT	resid = 0;
	SaHpiSeverityT		sev = SAHPI_OK;
	int			res, i;
	char			buf[SEV_BUF_SIZE + 1];
	char			*str;
	SaErrorT		rv;
	SaHpiRptEntryT		rpt_entry;
	Rpt_t			tmp_rpt;

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, resid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res, (char *)NULL, 0);
		if (i == 1) resid = (SaHpiResourceIdT)res;
		else return SA_OK;
	} else {
		resid = (SaHpiResourceIdT)atoi(argv[1]);
	};
	printf("New severity (crit, maj, min, inf, ok, debug, all): ");
	memset(buf, 0, SEV_BUF_SIZE + 1);
	fgets(buf, SEV_BUF_SIZE, stdin);
	for (res = 0; res < SEV_BUF_SIZE; res++)
		if (buf[res] == '\n') buf[res] = 0;
	str = buf;
	while (*str == ' ') str++;
	i = strlen(str);
	while ((i > 0) && (str[i - 1] == ' ')) i--;
	str[i] = 0;
	if (i == 0) {
		printf("Invalid sevetity: %s\n", buf);
		return(-1);
	};
	for (i = 0; Sev_array[i].name != (char *)NULL; i++)
		if (strcmp(str, Sev_array[i].name) == 0) {
			sev = Sev_array[i].val;
			break;
		};
	if (Sev_array[i].name == (char *)NULL) {
		printf("Invalid sevetity type: %s\n", buf);
		return(-1);
	};
	rv = saHpiResourceSeveritySet(Domain->sessionId, resid, sev);
	if (rv != SA_OK) {
		printf("saHpiResourceSeveritySet error = %s\n", oh_lookup_error(rv));
		return -1;
	};
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (SA_OK);
}

static int discovery(int argc, char *argv[])
{
	SaErrorT        ret;

	do_progress("Discover");
        ret = saHpiDiscover(Domain->sessionId);
        if (SA_OK != ret) {
                printf("saHpiResourcesDiscover failed\n");
		delete_progress();
        	return ret;
	};
	delete_progress();
        return ret;
}

static int dat_list(int argc, char *argv[])
{
	return show_dat(Domain, ui_print);
}

static int listres(int argc, char *argv[])
{
	show_rpt_list(Domain, SHOW_ALL_RPT, 0, ui_print);
	return(SA_OK);
}

static int clear_evtlog(int argc, char *argv[])
{
	SaHpiResourceIdT	rid;

	if (argc < 2)
		rid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	else
		rid = (SaHpiResourceIdT)atoi(argv[1]);
			
	return sa_clear_evtlog(rid);
}

static int show_evtlog(int argc, char *argv[])
{
	SaHpiResourceIdT	rptid = 0;

	if (argc < 2) {
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	} else {
		rptid = (SaHpiResourceIdT)atoi(argv[1]);
	};
			
	return show_event_log(Domain->sessionId, rptid, show_event_short, ui_print);
}

static int evtlog_time(int argc, char *argv[])
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiTimeT		logtime;
	SaHpiTextBufferT	buffer;

	if (argc < 2) {
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	} else {
		rptid = (SaHpiResourceIdT)atoi(argv[1]);
	};
			
	rv = saHpiEventLogTimeGet(Domain->sessionId, rptid, &logtime);
	if (rv != SA_OK) 
	{
		printf("saHpiEventLogTimeGet %s\n", oh_lookup_error(rv));
		return (rv);
	}

	oh_decode_time(logtime, &buffer);
	printf ("Current event log time: %s\n", buffer.Data);
	return SA_OK;
}

static int settime_evtlog(int argc, char *argv[])
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiTimeT		newtime;
	struct tm		new_tm_time;
	char			buf[READ_BUF_SIZE];
	int			day_array[] =
					{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (argc < 2) {
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
		printf("Set date and time for Domain Event Log!\n");
	} else {
		rptid = (SaHpiResourceIdT)atoi(argv[1]);
		printf("Set date and time for Resource %d!\n", rptid);
	};
	memset(&new_tm_time, 0, sizeof(struct tm));
	printf("format: MM:DD:YYYY:hh:mm:ss ==> ");
	fgets(buf, READ_BUF_SIZE, stdin);
	sscanf(buf, "%d:%d:%d:%d:%d:%d", &new_tm_time.tm_mon, &new_tm_time.tm_mday,
		&new_tm_time.tm_year, &new_tm_time.tm_hour, &new_tm_time.tm_min,
		&new_tm_time.tm_sec);
	if ((new_tm_time.tm_mon < 1) || (new_tm_time.tm_mon > 12)) {
		printf("Month out of range: (%d)\n", new_tm_time.tm_mon);
		return(-1);
	};
	new_tm_time.tm_mon--;
	if (new_tm_time.tm_year < 1900) {
		printf("Year out of range: (%d)\n", new_tm_time.tm_year);
		return(-1);
	};
	if (new_tm_time.tm_mon == 1) {
	/* if the given year is a leap year */
		if ((((new_tm_time.tm_year % 4) == 0)
			&& ((new_tm_time.tm_year % 100) != 0))
			|| ((new_tm_time.tm_year % 400) == 0))
			day_array[1] = 29;
	};

	if ((new_tm_time.tm_mday < 1) ||
		(new_tm_time.tm_mday > day_array[new_tm_time.tm_mon])) {
		printf("Day out of range: (%d)\n", new_tm_time.tm_mday);
		return(-1);
	};

	new_tm_time.tm_year -= 1900;
	
	if ((new_tm_time.tm_hour < 0) || (new_tm_time.tm_hour > 24)) {
		printf("Hours out of range: (%d)\n", new_tm_time.tm_hour);
		return(-1);
	};
	if ((new_tm_time.tm_min < 0) || (new_tm_time.tm_min > 60)) {
		printf("Minutes out of range: (%d)\n", new_tm_time.tm_min);
		return(-1);
	};
	if ((new_tm_time.tm_sec < 0) || (new_tm_time.tm_sec > 60)) {
		printf("Seconds out of range: (%d)\n", new_tm_time.tm_sec);
		return(-1);
	};

	newtime = (SaHpiTimeT) mktime(&new_tm_time) * 1000000000;
 	rv = saHpiEventLogTimeSet(Domain->sessionId, rptid, newtime);
	if (rv != SA_OK) 
	{
		printf("saHpiEventLogTimeSet %s\n", oh_lookup_error(rv));
	}

	return (rv);
}

static char *get_thres_value(SaHpiSensorReadingT *item, char *buf, int len)
{
	char	*val;

	if (item->IsSupported != SAHPI_TRUE)
		return("");
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			snprintf(buf, len, "%lld", item->Value.SensorInt64);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			snprintf(buf, len, "%llu", item->Value.SensorUint64);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			snprintf(buf, len, "%10.3f", item->Value.SensorFloat64);
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = (char *)(item->Value.SensorBuffer);
			if (val != NULL) {
				snprintf(buf, len, "%s", val);
				break;
			}
			return("");
	};
	return(buf);
}


static int set_sensor_threshold(SaHpiResourceIdT rptid, SaHpiSensorNumT num)
{
	SaErrorT		rv;
	SaHpiSensorTypeT	type;
	SaHpiEventCategoryT	categ;
	SaHpiSensorThresholdsT	senstbuff;
	float			f;
	int			res, modify = 0;
	char			tmp[256];

	rv = saHpiSensorTypeGet(Domain->sessionId, rptid, num, &type, &categ);
	if (rv != SA_OK) {
		printf("ERROR: saHpiSensorTypeGet error = %s\n", oh_lookup_error(rv));
		return -1; 
	};
	if (categ != SAHPI_EC_THRESHOLD)
		return(-1);
	rv = saHpiSensorThresholdsGet(Domain->sessionId, rptid, num, &senstbuff);
	if (rv != SA_OK) {
		printf("ERROR: saHpiSensorThresholdsGet error = %s\n",
			oh_lookup_error(rv));
		return -1; 
	};

	if (senstbuff.LowCritical.IsSupported) {
		printf("lc(%s) ==> ", get_thres_value(&(senstbuff.LowCritical), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowCritical), f);
		}
	};
	if (senstbuff.LowMajor.IsSupported) {
		printf("la(%s) ==> ", get_thres_value(&(senstbuff.LowMajor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowMajor), f);
		}
	};
	if (senstbuff.LowMinor.IsSupported) {
		printf("li(%s) ==> ", get_thres_value(&(senstbuff.LowMinor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowMinor), f);
		}
	};
	if (senstbuff.UpCritical.IsSupported) {
		printf("uc(%s) ==> ", get_thres_value(&(senstbuff.UpCritical), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpCritical), f);
		}
	};
	if (senstbuff.UpMajor.IsSupported) {
		printf("ua(%s) ==> ", get_thres_value(&(senstbuff.UpMajor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpMajor), f);
		}
	};
	if (senstbuff.UpMinor.IsSupported) {
		printf("ui(%s) ==> ", get_thres_value(&(senstbuff.UpMinor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpMinor), f);
		}
	};
	if (senstbuff.PosThdHysteresis.IsSupported) {
		printf("ph(%s) ==> ", get_thres_value(&(senstbuff.PosThdHysteresis),
			tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.PosThdHysteresis), f);
		}
	};
	if (senstbuff.NegThdHysteresis.IsSupported) {
		printf("nh(%s) ==> ", get_thres_value(&(senstbuff.NegThdHysteresis),
			tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.NegThdHysteresis), f);
		}
	};

	if (modify == 0) return(SA_OK);
		
	print_thres_value(&(senstbuff.LowCritical), "Lower Critical Threshold(lc):",
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMajor), "Lower Major Threshold(la):", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMinor), "Lower Minor Threshold(li):", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpCritical), "Upper Critical Threshold(uc):",
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMajor), "Upper Major Threshold(ua):", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMinor), "Upper Minor Threshold(ui):", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.PosThdHysteresis),
		"Positive Threshold Hysteresis(ph):", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.NegThdHysteresis),
		"Negative Threshold Hysteresis(nh):", NULL, 0, ui_print);
	printf("Set new threshold (yes|no) : ");
	fgets(tmp, 256, stdin);
	if (strncmp(tmp, "yes", 3) != 0) {
		printf("No action.\n");
		return(SA_OK);
	};
	rv = saHpiSensorThresholdsSet(Domain->sessionId, rptid, num, &senstbuff);
	if (rv != SA_OK) 
		printf("saHpiSensorThresholdsSet error %s\n", oh_lookup_error(rv));
	else
		printf("Sensor Threshold Value Set Succeed.\n");
	return(rv);
}

static int sen_block(int argc, char *argv[])
{
	SaHpiRdrT			rdr_entry;
	SaHpiResourceIdT		rptid = 0;
	SaHpiInstrumentIdT		rdrnum;
	SaHpiRdrTypeT			type;
	SaErrorT			rv;
	SaHpiBoolT			val;
	SaHpiEventStateT		assert, deassert;
	SaHpiSensorEventMaskActionT	act;
	int				res, i, first = 1;
	char				buf[256], *str;
	char				rep[10];

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, rptid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res, buf, 9);
		if (i != 1) return SA_OK;
		rptid = (SaHpiResourceIdT)res;
	} else {
		rptid = (SaHpiResourceIdT)atoi(argv[1]);
	};
	type = SAHPI_SENSOR_RDR;
	if (argc < 3) {
		i = show_rdr_list(Domain, rptid, type, ui_print);
		if (i == 0) {
			printf("No rdr for rpt: %d\n", rptid);
			return(SA_OK);
		};
		i = get_int_param("RDR NUM ==> ", &res, buf, 9);
		if (i != 1) return SA_OK;
		rdrnum = (SaHpiInstrumentIdT)res;
	} else {
		rdrnum = (SaHpiInstrumentIdT)atoi(argv[2]);
	};
	rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum, &rdr_entry);
	if (rv != SA_OK) {
		printf("ERROR!!! Can not get rdr: ResourceId=%d RdrType=%d RdrNum=%d\n",
			rptid, type, rdrnum);
		return(rv);
	};
	show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		clear_input();
		if (first) {
			printf("%s\n", SEN_AV_COM);
			first = 0;
		};
		i = get_int_param("command? ==> ", &res, buf, 9);
		if (i != 0) continue;
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
		if (strcmp(buf, "show") == 0) {
			show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		if (strcmp(buf, "help") == 0) {
			first = 1;
			continue;
		};
		if (strcmp(buf, "setthres") == 0) {
			set_sensor_threshold(rptid, rdrnum);
			continue;
		};
		if ((strcmp(buf, "enable") == 0) || (strcmp(buf, "disable") == 0)) {
			if (strcmp(buf, "enable") == 0) val = 1;
			else val = 0;
			rv = saHpiSensorEnableSet(Domain->sessionId, rptid, rdrnum, val);
			if (rv != SA_OK) {
				printf("saHpiSensorEnableSet: error: %s\n",
					oh_lookup_error(rv));
				continue;
			};
			rv = saHpiSensorEnableGet(Domain->sessionId, rptid, rdrnum, &val);
			if (rv != SA_OK) {
				printf("saHpiSensorEnableGet: error: %s\n",
					oh_lookup_error(rv));
				continue;
			};
			if (val) strcpy(buf, "Enable");
			else strcpy(buf, "Disable");
			printf("Sensor:(%d/%d) %s\n", rptid, rdrnum, buf);
			continue;
		};
		if ((strcmp(buf, "evtenb") == 0) || (strcmp(buf, "evtdis") == 0)) {
			if (strcmp(buf, "evtenb") == 0) val = 1;
			else val = 0;
			rv = saHpiSensorEventEnableSet(Domain->sessionId, rptid, rdrnum,
				val);
			if (rv != SA_OK) {
				printf("saHpiSensorEventEnableSet: error: %s\n",
					oh_lookup_error(rv));
				continue;
			};
			rv = saHpiSensorEventEnableGet(Domain->sessionId, rptid, rdrnum,
				&val);
			if (rv != SA_OK) {
				printf("saHpiSensorEventEnableGet: error: %s\n",
					oh_lookup_error(rv));
				continue;
			};
			if (val) strcpy(buf, "Enable");
			else strcpy(buf, "Disable");
			printf("Sensor:(%d/%d) event %s\n", rptid, rdrnum, buf);
			continue;
		};
		if ((strcmp(buf, "maskadd") == 0) || (strcmp(buf, "maskrm") == 0)) {
			if (strcmp(buf, "maskadd") == 0) {
				act = SAHPI_SENS_ADD_EVENTS_TO_MASKS;
				strcpy(rep, "add");
			} else {
				act = SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS;
				strcpy(rep, "remove");
			};
			rv = saHpiSensorEventMasksGet(Domain->sessionId, rptid, rdrnum,
				&assert, &deassert);
			if (rv != SA_OK) {
				printf("saHpiSensorEventMasksGet: error: %s\n",
					oh_lookup_error(rv));
				continue;
			};
			printf("Assert mask = 0x");
			strcpy(buf, "0x");
			fgets(buf + 2, 254, stdin);
			str = buf;
			if (strlen(str) < 3) continue;
			assert = strtol(str, (char **)NULL, 16);
			printf("Deassert mask = 0x");
			strcpy(buf, "0x");
			fgets(buf + 2, 254, stdin);
			str = buf;
			if (strlen(str) < 3) continue;
			deassert = strtol(str, (char **)NULL, 16);
			printf("Sensor:(%d/%d) %s masks: assert = 0x%4.4x   "
				"deassert = 0x%4.4x  (yes/no)?", rptid, rdrnum, rep,
				assert, deassert);
			fgets(buf, 256, stdin);
			if (strncmp(buf, "yes", 3) != 0) {
				printf("No action.\n");
				continue;
			};
			rv = saHpiSensorEventMasksSet(Domain->sessionId, rptid, rdrnum,
				act, assert, deassert);
			if (rv != SA_OK) {
				printf("saHpiSensorEventMasksSet: error: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		printf("Invalid command\n");
		first = 1;
	};
	return SA_OK;
}

static int show_inv(int argc, char *argv[])
{
	SaHpiResourceIdT	resid = 0;
	int			i, res;

#ifdef MY   // my
	fixstr((SaHpiTextBufferT *)S);
	prtchassinfo();
	prtprodtinfo();
	prtboardinfo();
#endif

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, resid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res, (char *)NULL, 0);
		if (i == 1) resid = (SaHpiResourceIdT)res;
		else return SA_OK;
	} else {
		resid = (SaHpiResourceIdT)atoi(argv[1]);
	};
			
	return sa_show_inv(resid);
}

static int show_rpt(int argc, char *argv[])
{
	Rpt_t			tmp_rpt;
	SaHpiRptEntryT		rpt_entry;
	int			i, res;
	SaErrorT		rv;
	SaHpiResourceIdT	resid = 0;

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, resid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res, (char *)NULL, 0);
		if (i == 1) resid = (SaHpiResourceIdT)res;
		else return SA_OK;
	} else {
		resid = (SaHpiResourceIdT)atoi(argv[1]);
	};
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (SA_OK);
}

static int show_rdr(int argc, char *argv[])
{
	Rdr_t			tmp_rdr;
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid = 0;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	int			res, i;
	char			buf[10], t;

	clear_input();
	if (argc < 2) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, rptid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT (ID | all) ==> ", &res, buf, 9);
		if ((i == 0) && (strncmp(buf, "all", 3) == 0)) {
			show_rpt_list(Domain, SHOW_ALL_RDR, rptid, ui_print);
			return(SA_OK);
		};
		if (i != 1) return SA_OK;
		rptid = (SaHpiResourceIdT)res;
	} else {
		if (strcmp(argv[1], "all") == 0) {
			show_rpt_list(Domain, SHOW_ALL_RDR, rptid, ui_print);
			return(SA_OK);
		};
		if (isdigit(argv[1][0]))
			rptid = (SaHpiResourceIdT)atoi(argv[1]);
		else
			return HPI_SHELL_PARM_ERROR;
	};
	if (argc < 3) {
		i = get_int_param("RDR Type (s|a|c|w|i|all) ==> ", &res, buf, 9);
		if (i != 0) return HPI_SHELL_PARM_ERROR;
	} else {
		memset(buf, 0, 10);
		strncpy(buf, argv[2], 3);
	};
	if (strncmp(buf, "all", 3) == 0) t = 'n';
	else t = *buf;
	if (t == 'c') type = SAHPI_CTRL_RDR;
	else if (t == 's') type = SAHPI_SENSOR_RDR;
	else if (t == 'i') type = SAHPI_INVENTORY_RDR;
	else if (t == 'w') type = SAHPI_WATCHDOG_RDR;
	else if (t == 'a') type = SAHPI_ANNUNCIATOR_RDR;
	else type = SAHPI_NO_RECORD;
	if (argc < 4) {
		i = show_rdr_list(Domain, rptid, type, ui_print);
		if (i == 0) {
			printf("No rdr for rpt: %d\n", rptid);
			return(SA_OK);
		};
		i = get_int_param("RDR NUM ==> ", &res, buf, 9);
		if (i != 1) return SA_OK;
		rdrnum = (SaHpiInstrumentIdT)res;
	} else {
		rdrnum = (SaHpiInstrumentIdT)atoi(argv[3]);
	};
	if (type == SAHPI_NO_RECORD)
		rv = find_rdr_by_num(Domain->sessionId, rptid, rdrnum, type, 0,
			&rdr_entry);
	else
		rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum,
			&rdr_entry);
	if (rv != SA_OK) {
		printf("ERROR!!! Get rdr: ResourceId=%d RdrType=%d RdrNum=%d: %s\n",
			rptid, type, rdrnum, oh_lookup_error(rv));
		return(rv);
	};
	make_attrs_rdr(&tmp_rdr, &rdr_entry);
	show_Rdr(&tmp_rdr, ui_print);
	free_attrs(&(tmp_rdr.Attrutes));
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
			"Usage: clearevtlog [<resource id>]";
const char dathelp[] = "dat: domain alarm table list\n"
			"Usage: dat";
const char debughelp[] = "debug: set or unset OPENHPI_DEBUG environment\n"
			"Usage: debug [ on | off]";
const char dscvhelp[] = "dscv: discovery resources\n"                      \
			"Usage: dscv ";
const char eventhelp[] = "event: enable or disable event display on screen\n" \
			"Usage: event [enable|disable|short|full] ";
const char evtlogtimehelp[] = "evtlogtime: show the event log's clock\n"
			"Usage: evtlogtime [<resource id>]";
const char helphelp[] = "help: help information for OpenHPI commands\n"
			"Usage: help [optional commands]";
const char hsindhelp[] = "hotswap_ind: show hot swap indicator state\n"
			"Usage: hotswap_ind <resource id>";
const char hsstathelp[] = "hotswapstat: retrieve hot swap state of a resource\n"
			"Usage: hotswapstat <resource id>";
const char lreshelp[] = "lsres: list resources\n"
			"Usage: lsres";
const char lsorhelp[] = "lsensor: list sensors\n"
			"Usage: lsensor";
const char powerhelp[] = "power: power the resource on, off or cycle\n"
			"Usage: power <resource id> [on|off|cycle]";
const char quithelp[] = "quit: close session and quit console\n"
			"Usage: quit";
const char resethelp[] = "reset: perform specified reset on the entity\n"  \
			"Usage: reset <resource id> [cold|warm|assert|deassert]";
const char senhelp[] =	"sen: sensor command block\n"
			"Usage: sen [<sensorId>]\n"
			"	sensorId:: <resourceId> <num>\n"
			SEN_AV_COM;
const char setseverhelp[] = "setsever: set severity for a resource\n"
			"Usage: setsever [<resource id>]";
const char settaghelp[] = "settag: set tag for a particular resource\n"
			"Usage: settag [<resource id>]";
const char settimeevtloghelp[] = "settimeevtlog: sets the event log's clock\n"
			"Usage: settimeevtlog [<resource id>]";
const char showevtloghelp[] = "showevtlog: show system event logs\n"
			"Usage: showevtlog [<resource id>]";
const char showinvhelp[] = "showinv: show inventory data of a resource\n"
			"Usage: showinv [<resource id>]";
const char showrdrhelp[] = "showrdr: show resource data record\n"
			"Usage: showrdr [<resource id> [type [<rdr num>]]]\n"
			"   or  rdr [<resource id> [type [<rdr num>]]]\n"
			"	type =	c - control rdr, s - sensor, i - inventory rdr\n"
			"		w - watchdog, a - annunciator, all - all rdr";
const char showrpthelp[] = "showrpt: show resource information\n"
			"Usage: showrpt [<resource id>]\n"
			"   or  rpt [<resource id>]";

struct command commands[] = {
    { "clearevtlog",	clear_evtlog,		clearevtloghelp },
    { "dat",		dat_list,		dathelp },
    { "debug",		debugset,		debughelp },
    { "dscv",		discovery,		dscvhelp },
    { "event",		event,			eventhelp },
    { "evtlogtime",	evtlog_time,		evtlogtimehelp },
    { "help",		help,			helphelp },
    { "hotswap_ind",	show_hs_ind,		hsindhelp },
    { "hotswapstat",	hotswap_stat,		hsstathelp },
    { "lsres",		listres,		lreshelp },
    { "lsensor",	list_sensor,		lsorhelp },
    { "power",		power,			powerhelp },
    { "quit",		quit,			quithelp },
    { "rdr",		show_rdr,		showrdrhelp },
    { "reset",		reset,			resethelp },
    { "rpt",		show_rpt,		showrpthelp },
    { "sen",		sen_block,		senhelp },
    { "settag",		set_tag,		settaghelp },
    { "setsever",	set_sever,		setseverhelp },
    { "settimeevtlog",	settime_evtlog,		settimeevtloghelp },
    { "showevtlog",	show_evtlog,		showevtloghelp },
    { "showinv",	show_inv,		showinvhelp },
    { "showrdr",	show_rdr,		showrdrhelp },
    { "showrpt",	show_rpt,		showrpthelp },
    { "?",		help,			helphelp },
    { NULL,		NULL,			NULL }
};
