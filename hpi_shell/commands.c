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

//SaErrorT oh_process_config_file(char *conf_file_name);

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

#define CTRL_AV_COM 	"   Available commands:\n" \
			"	setstate - set control state\n" \
			"	show     - show control info\n" \
			"	help	 - command list\n" \
			"	q | quit - exit"

#define INV_AV_COM 	"   Available commands:\n" \
			"	addarea  - add inventory area\n" \
			"	delarea  - delete inventory area\n" \
			"	addfield - add inventory field\n" \
			"	setfield - set inventory field\n" \
			"	delfield - delete inventory field\n" \
			"	show     - show inventory info\n" \
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

static int add_config(int argc, char *argv[])
{
	SaErrorT rv = SA_OK;

	if (argc != 2) {
		printf("no config file\n");
		return -1;
	}
//	rv = oh_process_config_file(argv[1]);
//	if (rv == SA_ERR_HPI_BUSY) {
//		printf("Hold on. Another configuration changing is"
//		" processing\n");
//	}
//	if (rv == SA_ERR_HPI_NOT_PRESENT) {
//		printf("Hold on. Initialization is processing\n");
//	}
	return rv;
}
  
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
	};
	set_Subscribe((Domain_t *)NULL, prt_flag);

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
	SaErrorT		rv;
	SaHpiHsIndicatorStateT	state = 0;
	SaHpiResourceIdT	rid;
	int			do_set = 0;

	if (argc < 2)
		return HPI_SHELL_PARM_ERROR;

	rid = (SaHpiResourceIdT)atoi(argv[1]);
	if (argc > 2) {
		do_set = 1;
		if (strcmp(argv[2], "on") == 0) state = SAHPI_HS_INDICATOR_ON;
		else if (strcmp(argv[2], "off") == 0) state = SAHPI_HS_INDICATOR_OFF;
		else return HPI_SHELL_PARM_ERROR;
	};
	if (do_set) {
		rv = saHpiHotSwapIndicatorStateSet(Domain->sessionId, rid, state);
		if (rv != SA_OK) { 
			printf("saHpiHotSwapIndicatorStateSet error %s\n", oh_lookup_error(rv));
			return -1;
		};
		return(SA_OK);
	};

	rv = saHpiHotSwapIndicatorStateGet(Domain->sessionId, rid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapIndicatorStateGet error %s\n", oh_lookup_error(rv));
		return -1;
	}

	printf("Current HS Indicator for resource %d is:", rid);
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

static int evtlog_state(int argc, char *argv[])
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiBoolT		state = SAHPI_TRUE;
	int			i = 1;
	int			do_set = 0;
	char			*str;

	rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	if (argc >= 2) {
		if (isdigit(argv[i][0])) {
			rptid = (SaHpiResourceIdT)atoi(argv[i]);
			i++;
		}
	};

	if (argc >= (i + 1)) {
		do_set = 1;
		if (strcmp(argv[i], "enable") == 0) state = SAHPI_TRUE;
		else if (strcmp(argv[i], "disable") == 0) state = SAHPI_FALSE;
		else return(HPI_SHELL_PARM_ERROR);
	};
	if (do_set) {
		rv = saHpiEventLogStateSet(Domain->sessionId, rptid, state);
		if (rv != SA_OK) {
			printf("saHpiEventLogStateSet %s\n", oh_lookup_error(rv));
			return(rv);
		};
		return(SA_OK);
	};
	rv = saHpiEventLogStateGet(Domain->sessionId, rptid, &state);
	if (rv != SA_OK) {
		printf("saHpiEventLogStateGet %s\n", oh_lookup_error(rv));
		return(rv);
	};
	if (state == SAHPI_TRUE) str = "Enable";
	else str = "Disable";
	printf("Event Log State: %s\n", str);
	return SA_OK;
}

static int evtlog_reset(int argc, char *argv[])
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;

	rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	if (argc >= 2) {
		if (isdigit(argv[1][0])) {
			rptid = (SaHpiResourceIdT)atoi(argv[1]);
		} else return(HPI_SHELL_PARM_ERROR);
	};

	rv = saHpiEventLogOverflowReset(Domain->sessionId, rptid);
	if (rv != SA_OK) {
		printf("saHpiEventLogOverflowReset %s\n", oh_lookup_error(rv));
		return(rv);
	};
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


static int set_sensor_threshold(SaHpiResourceIdT rptid, SaHpiRdrT *rdr)
{
	SaErrorT		rv;
	SaHpiSensorTypeT	type;
	SaHpiEventCategoryT	categ;
	SaHpiSensorThresholdsT	senstbuff;
	SaHpiSensorRangeT	*range;
	float			f;
	SaHpiSensorNumT		num = rdr->RdrTypeUnion.SensorRec.Num;
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

	printf("Range (");
	range = &(rdr->RdrTypeUnion.SensorRec.DataFormat.Range);
	if ((range->Flags & SAHPI_SRF_MIN) == 0) printf("-");
	else printf("%s", get_thres_value(&(range->Min), tmp, 256));
	printf(":");
	if ((range->Flags & SAHPI_SRF_MAX) == 0) printf("-)\n");
	else printf("%s)\n", get_thres_value(&(range->Max), tmp, 256));
	if (senstbuff.LowCritical.IsSupported) {
		printf("Lower Critical(%s) ==> ",
			get_thres_value(&(senstbuff.LowCritical), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowCritical), f);
		}
	};
	if (senstbuff.LowMajor.IsSupported) {
		printf("Lower Major(%s) ==> ",
			get_thres_value(&(senstbuff.LowMajor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowMajor), f);
		}
	};
	if (senstbuff.LowMinor.IsSupported) {
		printf("Lower Minor(%s) ==> ",
			get_thres_value(&(senstbuff.LowMinor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.LowMinor), f);
		}
	};
	if (senstbuff.UpCritical.IsSupported) {
		printf("Upper Critical(%s) ==> ",
			get_thres_value(&(senstbuff.UpCritical), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpCritical), f);
		}
	};
	if (senstbuff.UpMajor.IsSupported) {
		printf("Upper Major(%s) ==> ",
			get_thres_value(&(senstbuff.UpMajor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpMajor), f);
		}
	};
	if (senstbuff.UpMinor.IsSupported) {
		printf("Upper Minor(%s) ==> ",
			get_thres_value(&(senstbuff.UpMinor), tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.UpMinor), f);
		}
	};
	if (senstbuff.PosThdHysteresis.IsSupported) {
		printf("Positive Hysteresis(%s) ==> ",
			get_thres_value(&(senstbuff.PosThdHysteresis),
			tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.PosThdHysteresis), f);
		}
	};
	if (senstbuff.NegThdHysteresis.IsSupported) {
		printf("Negative Threshold Hysteresis(%s) ==> ",
			get_thres_value(&(senstbuff.NegThdHysteresis),
			tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(&(senstbuff.NegThdHysteresis), f);
		}
	};

	if (modify == 0) return(SA_OK);
		
	print_thres_value(&(senstbuff.LowCritical), "Lower Critical:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMajor), "Lower Major:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMinor), "Lower Minor:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpCritical), "Upper Critical:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMajor), "Upper Major:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMinor), "Upper Minor:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.PosThdHysteresis),
		"Positive Hysteresis:", NULL, 0, ui_print);
	print_thres_value(&(senstbuff.NegThdHysteresis),
		"Negative Hysteresis:", NULL, 0, ui_print);
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
			set_sensor_threshold(rptid, &rdr_entry);
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

static int set_control_state(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiCtrlNumT num)
{
        SaErrorT		rv;
	SaHpiRdrT		rdr;
	SaHpiCtrlRecT		*ctrl;
	SaHpiCtrlTypeT		type;
	SaHpiCtrlRecDigitalT	*digit;
	SaHpiCtrlStateDigitalT	state_val = 0;
	SaHpiCtrlStateT		state;
	char			buf[256];

	rv = saHpiRdrGetByInstrumentId(sessionid, resourceid, SAHPI_CTRL_RDR, num, &rdr);
	if (rv != SA_OK) {
		printf("saHpiRdrGetByInstrumentId: error: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	ctrl = &(rdr.RdrTypeUnion.CtrlRec);
	type = ctrl->Type;
	switch (type) {
		case SAHPI_CTRL_TYPE_DIGITAL:
			digit = &(ctrl->TypeUnion.Digital);
			printf("New state (on|off|pulseon|pulseoff): ");
			fgets(buf, 256, stdin);
			if (strncmp(buf, "on", 2) == 0) state_val = SAHPI_CTRL_STATE_ON;
			if (strncmp(buf, "off", 3) == 0) state_val = SAHPI_CTRL_STATE_OFF;
			if (strncmp(buf, "pulseon", 7) == 0)
				state_val = SAHPI_CTRL_STATE_PULSE_ON;
			if (strncmp(buf, "pulseoff", 8) == 0)
				state_val = SAHPI_CTRL_STATE_PULSE_OFF;
			state.StateUnion.Digital = state_val;
			state.Type = type;
			rv = saHpiControlSet(sessionid, resourceid, num,
				SAHPI_CTRL_MODE_MANUAL, &state);
			if (rv != SA_OK) {
				printf("saHpiControlSet: error: %s\n",
					oh_lookup_error(rv));
				return(rv);
			};
			break;
		default: strcpy(buf, "Unknown control type\n");
	};
	return(SA_OK);
}

static int ctrl_block(int argc, char *argv[])
{
	SaHpiRdrT			rdr_entry;
	SaHpiResourceIdT		rptid = 0;
	SaHpiInstrumentIdT		rdrnum;
	SaHpiRdrTypeT			type;
	SaErrorT			rv;
	int				res, i, first = 1;
	char				buf[256];

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
	type = SAHPI_CTRL_RDR;
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
	show_control(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		clear_input();
		if (first) {
			printf("%s\n", CTRL_AV_COM);
			first = 0;
		};
		i = get_int_param("command? ==> ", &res, buf, 9);
		if (i != 0) continue;
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
		if (strcmp(buf, "help") == 0) {
			first = 1;
			continue;
		};
		if (strcmp(buf, "setstate") == 0) {
			set_control_state(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "show") == 0) {
			show_control(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		printf("Invalid command\n");
		first = 1;
	};
	return SA_OK;
}

typedef struct {
	char			*name;
	SaHpiIdrAreaTypeT	val;
} Area_type_t;

static Area_type_t Area_types[] = {
	{ "inter",	SAHPI_IDR_AREATYPE_INTERNAL_USE },
	{ "chass",	SAHPI_IDR_AREATYPE_CHASSIS_INFO },
	{ "board",	SAHPI_IDR_AREATYPE_BOARD_INFO },
	{ "prod",	SAHPI_IDR_AREATYPE_PRODUCT_INFO },
	{ "oem",	SAHPI_IDR_AREATYPE_OEM },
	{ NULL,		SAHPI_IDR_AREATYPE_UNSPECIFIED } };

static int add_inventory_area(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaHpiEntryIdT	entry;
	SaErrorT	rv;
	char		buf[10];
	int		res, i;

	i = get_int_param("Area type (inter,chass,board,prod,oem): ", &res, buf, 9);
	if (i != 0) return(-1);
	for (i = 0; Area_types[i].name != (char *)NULL; i++)
		if (strcmp(Area_types[i].name, buf) == 0) break;
	if (Area_types[i].name == (char *)NULL) {
		printf("Error!!! Unknown Area type: %s\n", buf);
		return(-1);
	};
	rv = saHpiIdrAreaAdd(sessionId, rptid, rdrnum, Area_types[i].val, &entry);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiIdrAreaAdd: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	return(SA_OK);
}

typedef struct {
	char			*name;
	SaHpiIdrFieldTypeT	val;
} Field_type_t;

static Field_type_t Field_types[] = {
	{ "chass",	SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE },
	{ "time",	SAHPI_IDR_FIELDTYPE_MFG_DATETIME },
	{ "manuf",	SAHPI_IDR_FIELDTYPE_MANUFACTURER },
	{ "prodname",	SAHPI_IDR_FIELDTYPE_PRODUCT_NAME },
	{ "prodver",	SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION },
	{ "snum",	SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER },
	{ "pnum",	SAHPI_IDR_FIELDTYPE_PART_NUMBER },
	{ "file",	SAHPI_IDR_FIELDTYPE_FILE_ID },
	{ "tag",	SAHPI_IDR_FIELDTYPE_ASSET_TAG },
	{ "custom",	SAHPI_IDR_FIELDTYPE_CUSTOM },
	{ NULL,		SAHPI_IDR_FIELDTYPE_UNSPECIFIED } };

static int add_inventory_field(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaErrorT	rv;
	SaHpiIdrFieldT	field;
	char		buf[256], *str;
	int		res, i;

	i = get_int_param("Area Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	field.AreaId = res;

	i = get_int_param("Field type (chass,time,manuf,prodname,prodver,snum,pnum,file,tag,custom): ",
		&res, buf, 9);
	if (i != 0) {
		printf("Error!!! Invalid Field type: %s\n", buf);
		return(-1);
	};
	for (i = 0; Field_types[i].name != (char *)NULL; i++)
		if (strcmp(Field_types[i].name, buf) == 0) break;
	if (Field_types[i].name == (char *)NULL) {
		printf("Error!!! Unknown Field type: %s\n", buf);
		return(-1);
	};
	field.Type = Field_types[i].val;
	field.ReadOnly = SAHPI_FALSE;
	printf("Field value: ");
	memset(buf, 0, 256);
	fgets(buf, 255, stdin);
	i = strlen(buf);
	if (i < 2) *buf = 0;
	else buf[i - 1] = 0;
	str = buf;
	while (*str == ' ') str++;
	i = strlen(buf);
	field.Field.DataType = SAHPI_TL_TYPE_TEXT;
	field.Field.Language = SAHPI_LANG_ENGLISH;
	field.Field.DataLength = i;
	if (i > 0)
		strcpy(field.Field.Data, buf);
	rv = saHpiIdrFieldAdd(sessionId, rptid, rdrnum, &field);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiIdrFieldAdd: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	return(SA_OK);
}

static int set_inventory_field(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaErrorT	rv;
	SaHpiIdrFieldT	field, res_field;
	SaHpiEntryIdT	fentryid, nextfentryid;
	char		buf[256], *str;
	int		res, i;

	i = get_int_param("Area Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	field.AreaId = res;

	i = get_int_param("Field Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Field Id\n");
		return(-1);
	};
	field.FieldId = res;

	fentryid = SAHPI_FIRST_ENTRY;
	while (fentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiIdrFieldGet(sessionId, rptid, rdrnum, field.AreaId,
			SAHPI_IDR_FIELDTYPE_UNSPECIFIED, fentryid,
			&nextfentryid, &res_field);
		if (rv != SA_OK) {
			printf("No Field for AreaId: %d  FieldId: %d\n", field.AreaId, field.FieldId);
			return(-1);
		};
		if (res_field.FieldId == field.FieldId) break;
		fentryid = nextfentryid;
	};
	if (res_field.FieldId != field.FieldId) {
		printf("No Field for AreaId: %d  FieldId: %d\n", field.AreaId, field.FieldId);
		return(-1);
	};
	field = res_field;
	i = get_int_param("Field type (chass,time,manuf,prodname,prodver,snum,pnum,file,tag,custom): ",
		&res, buf, 9);
	if (i == 0) {
		for (i = 0; Field_types[i].name != (char *)NULL; i++)
			if (strcmp(Field_types[i].name, buf) == 0) break;
		if (Field_types[i].name == (char *)NULL) {
			printf("Error!!! Unknown Field type: %s\n", buf);
			return(-1);
		};
		field.Type = Field_types[i].val;
	};
	printf("Field value: ");
	memset(buf, 0, 256);
	fgets(buf, 255, stdin);
	i = strlen(buf);
	if (i < 2) *buf = 0;
	else buf[i - 1] = 0;
	str = buf;
	while (*str == ' ') str++;
	i = strlen(buf);
	if (i > 0) {
		field.Field.DataType = SAHPI_TL_TYPE_TEXT;
		field.Field.Language = SAHPI_LANG_ENGLISH;
		field.Field.DataLength = i;
		strcpy(field.Field.Data, buf);
	};
	rv = saHpiIdrFieldSet(sessionId, rptid, rdrnum, &field);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiIdrFieldSet: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	return(SA_OK);
}

static int del_inventory_field(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaErrorT	rv;
	SaHpiEntryIdT	areaId, fieldId;
	char		buf[256];
	int		res, i;

	i = get_int_param("Area Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	areaId = res;

	i = get_int_param("Field Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Field Id\n");
		return(-1);
	};
	fieldId = res;

	rv = saHpiIdrFieldDelete(sessionId, rptid, rdrnum, areaId, fieldId);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiIdrFieldDelete: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	return(SA_OK);
}

static int delete_inventory_area(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaErrorT	rv;
	char		buf[10];
	int		res, i;

	i = get_int_param("Area Id: ", &res, buf, 9);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	rv = saHpiIdrAreaDelete(sessionId, rptid, rdrnum, res);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiIdrAreaDelete: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	return(SA_OK);
}

static int inv_block(int argc, char *argv[])
{
	SaHpiRdrT			rdr_entry;
	SaHpiResourceIdT		rptid = 0;
	SaHpiInstrumentIdT		rdrnum;
	SaHpiRdrTypeT			type;
	SaErrorT			rv;
	int				res, i, first = 1;
	char				buf[256];

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
	type = SAHPI_INVENTORY_RDR;
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
	show_inventory(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		clear_input();
		if (first) {
			printf("%s\n", INV_AV_COM);
			first = 0;
		};
		i = get_int_param("command? ==> ", &res, buf, 9);
		if (i != 0) continue;
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
		if (strcmp(buf, "help") == 0) {
			first = 1;
			continue;
		};
		if (strcmp(buf, "show") == 0) {
			show_inventory(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		if (strcmp(buf, "addarea") == 0) {
			add_inventory_area(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "delarea") == 0) {
			delete_inventory_area(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "addfield") == 0) {
			add_inventory_field(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "setfield") == 0) {
			set_inventory_field(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "delfield") == 0) {
			del_inventory_field(Domain->sessionId, rptid, rdrnum);
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

static int domain_info(int argc, char *argv[])
{
	SaHpiDomainInfoT	info;
	SaHpiTextBufferT	*buf;
	SaErrorT		rv;
	char			date[30];

	rv = saHpiDomainInfoGet(Domain->sessionId, &info);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiDomainInfoGet: %s\n", oh_lookup_error(rv));
		return(rv);
	};
	printf("Domain: %d   Capabil: 0x%x   IsPeer: %d   Guid: %s\n",
		info.DomainId, info.DomainCapabilities,
		info.IsPeer, info.Guid);
	buf = &(info.DomainTag);
	if (buf->DataLength > 0)
		printf("    Tag: %s\n", buf->Data);
	time2str(info.DrtUpdateTimestamp, date, 30);
	printf("    DRT update count: %d   DRT Timestamp : %s\n", info.DrtUpdateCount, date);
	time2str(info.RptUpdateTimestamp, date, 30);
	printf("    RPT update count: %d   RPT Timestamp : %s\n", info.RptUpdateCount, date);
	time2str(info.DatUpdateTimestamp, date, 30);
	printf("    DAT update count: %d   DAT Timestamp : %s\n", info.DatUpdateCount, date);
	printf("        ActiveAlarms: %d   CriticalAlarms: %d   Major: %d   Minor: %d   Limit: %d\n",
		info.ActiveAlarms, info.CriticalAlarms, info.MajorAlarms, info.MinorAlarms,
		info.DatUserAlarmLimit);
	printf("        DatOverflow : %d\n", info.DatOverflow);
	return(SA_OK);
}

static int domain_proc(int argc, char *argv[])
{
	SaHpiDomainInfoT	info;
	SaHpiTextBufferT	*buf;
	SaHpiEntryIdT		entryid, nextentryid;
	SaHpiDrtEntryT		drtentry;
	SaErrorT		rv;
	SaHpiDomainIdT		id;
	SaHpiSessionIdT		sessionId;
	int			i, n, first;
	gpointer		ptr;
	Domain_t		*domain = (Domain_t *)NULL;
	Domain_t		*new_domain;

	if (argc < 2) {
		printf("Domain list:\n");
		printf("    ID: %d   SessionId: %d", Domain->domainId, Domain->sessionId);
		rv = saHpiDomainInfoGet(Domain->sessionId, &info);
		if (rv == SA_OK) {
			buf = &(info.DomainTag);
			if (buf->DataLength > 0)
				printf("    Tag: %s", buf->Data);
		};
		printf("\n");
		entryid = SAHPI_FIRST_ENTRY;
		first = 1;
		while (entryid != SAHPI_LAST_ENTRY) {
			rv = saHpiDrtEntryGet(Domain->sessionId, entryid, &nextentryid, &drtentry);
			if (rv != SA_OK) break;
			if (first) {
				first = 0;
				printf("        Domain Reference Table:\n");
			};
			printf("            ID: %d", drtentry.DomainId);
			entryid = nextentryid;
			rv = saHpiSessionOpen(drtentry.DomainId,
						&sessionId, NULL);
			if (rv != SA_OK) {
				printf("\n");
				continue;
			};
			rv = saHpiDomainInfoGet(sessionId, &info);
			if (rv == SA_OK) {
				buf = &(info.DomainTag);
				if (buf->DataLength > 0)
					printf("    Tag: %s", buf->Data);
			};
			saHpiSessionClose(sessionId);
			printf("\n");
		}
		return(SA_OK);
	};

	if (isdigit(argv[1][0]))
		id = (int)atoi(argv[1]);
	else
		return HPI_SHELL_PARM_ERROR;
	n = g_slist_length(domainlist);
	for (i = 0; i < n; i++) {
		ptr = g_slist_nth_data(domainlist, i);
		if (ptr == (gpointer)NULL) break;
		domain = (Domain_t *)ptr;
		if (domain->domainId == id) break;
	};
	if (i >= n) {
		new_domain = (Domain_t *)malloc(sizeof(Domain_t));
		memset(new_domain, 0, sizeof(Domain_t));
		new_domain->domainId = id;
		if (add_domain(new_domain) < 0) {
			free(new_domain);
			printf("Can not open domain: %d\n", id);
			return HPI_SHELL_PARM_ERROR;
		};
		domain = new_domain;
	};
	Domain = domain;
	set_Subscribe(Domain, prt_flag);
	add_domain(Domain);
	return(SA_OK);
}

/* command table */
const char addcfghelp[] = "addcfg: add plugins, domains, handlers from"
			" config file\nUsage: addcfg <config file>\n";
const char clearevtloghelp[] = "clearevtlog: clear system event logs\n"
			"Usage: clearevtlog [<resource id>]";
const char ctrlhelp[] =	"ctrl: control command block\n"
			"Usage: ctrl [<ctrlId>]\n"
			"	ctrlId:: <resourceId> <num>\n"
			CTRL_AV_COM;
const char dathelp[] = "dat: domain alarm table list\n"
			"Usage: dat";
const char debughelp[] = "debug: set or unset OPENHPI_DEBUG environment\n"
			"Usage: debug [ on | off ]";
const char domainhelp[] = "domain: show domain list and set current domain\n"
			"Usage: domain [<domain id>]";
const char domaininfohelp[] = "domaininfo: show current domain info\n"
			"Usage: domaininfo";
const char dscvhelp[] = "dscv: discovery resources\n"
			"Usage: dscv ";
const char eventhelp[] = "event: enable or disable event display on screen\n"
			"Usage: event [enable|disable|short|full] ";
const char evtlogresethelp[] = "evtlogreset: reset the OverflowFlag in the event log\n"
			"Usage: evtlogreset [<resource id>]";
const char evtlogstatehelp[] = "evtlogstate: show and set the event log state\n"
			"Usage: evtlogstate [<resource id>] [enable|disable]";
const char evtlogtimehelp[] = "evtlogtime: show the event log's clock\n"
			"Usage: evtlogtime [<resource id>]";
const char helphelp[] = "help: help information for OpenHPI commands\n"
			"Usage: help [optional commands]";
const char hsindhelp[] = "hotswap_ind: show hot swap indicator state\n"
			"Usage: hotswap_ind <resource id>";
const char hsstathelp[] = "hotswapstat: retrieve hot swap state of a resource\n"
			"Usage: hotswapstat <resource id>";
const char invhelp[] =	"inv: inventory command block\n"
			"Usage: inv [<InvId>]\n"
			"	InvId:: <resourceId> <IdrId>\n"
			INV_AV_COM;
const char lreshelp[] = "lsres: list resources\n"
			"Usage: lsres";
const char lsorhelp[] = "lsensor: list sensors\n"
			"Usage: lsensor";
const char powerhelp[] = "power: power the resource on, off or cycle\n"
			"Usage: power <resource id> [on|off|cycle]";
const char quithelp[] = "quit: close session and quit console\n"
			"Usage: quit";
const char resethelp[] = "reset: perform specified reset on the entity\n"
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
    { "addcfg",		add_config,		addcfghelp},
    { "clearevtlog",	clear_evtlog,		clearevtloghelp },
    { "ctrl",		ctrl_block,		ctrlhelp },
    { "dat",		dat_list,		dathelp },
    { "debug",		debugset,		debughelp },
    { "domain",		domain_proc,		domainhelp },
    { "domaininfo",	domain_info,		domaininfohelp },
    { "dscv",		discovery,		dscvhelp },
    { "event",		event,			eventhelp },
    { "evtlogtime",	evtlog_time,		evtlogtimehelp },
    { "evtlogreset",	evtlog_reset,		evtlogresethelp },
    { "evtlogstate",	evtlog_state,		evtlogstatehelp },
    { "help",		help,			helphelp },
    { "hotswap_ind",	show_hs_ind,		hsindhelp },
    { "hotswapstat",	hotswap_stat,		hsstathelp },
    { "inv",		inv_block,		invhelp },
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
