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
 * Author(s):
 *	Kouzmich	< Mikhail.V.Kouzmich@intel.com >
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

static int add_inventory_area(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaHpiEntryIdT	entry;
	SaErrorT	rv;
	char		buf[10];
	int		i;

	i = get_string_param("Area type (inter,chass,board,prod,oem): ", buf, 9);
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

static int add_inventory_field(SaHpiSessionIdT sessionId, SaHpiResourceIdT rptid, SaHpiIdrIdT rdrnum)
{
	SaErrorT	rv;
	SaHpiIdrFieldT	field;
	char		buf[256];
	int		res, i;

	i = get_int_param("Area Id: ", &res);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	field.AreaId = res;

	i = get_string_param("Field type(chass,time,manuf,prodname,prodver,"
		"snum,pnum,file,tag,custom): ", buf, 9);
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
	i = get_string_param("Field value: ", buf, 256);
	if (i != 0) {
		printf("Error!!! Invalid Field value: %s\n", buf);
		return(-1);
	};
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
	SaHpiIdrFieldT	field;
	char		buf[256];
	int		res, i;

	i = get_int_param("Area Id: ", &res);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	field.AreaId = res;

	i = get_int_param("Field Id: ", &res);
	if (i != 1) {
		printf("Error!!! Invalid Field Id\n");
		return(-1);
	};
	field.FieldId = res;

	i = get_string_param("Field type(chass,time,manuf,prodname,prodver,"
		"snum,pnum,file,tag,custom): ", buf, 9);
	if (i == 0) {
		for (i = 0; Field_types[i].name != (char *)NULL; i++)
			if (strcmp(Field_types[i].name, buf) == 0) break;
		if (Field_types[i].name == (char *)NULL) {
			printf("Error!!! Unknown Field type: %s\n", buf);
			return(-1);
		};
		field.Type = Field_types[i].val;
	};
	i = get_string_param("Field value: ", buf, 256);
	if (i != 0) {
		printf("Error!!! Invalid Field value: %s\n", buf);
		return(-1);
	};
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
	int		res, i;

	i = get_int_param("Area Id: ", &res);
	if (i != 1) {
		printf("Error!!! Invalid Area Id\n");
		return(-1);
	};
	areaId = res;

	i = get_int_param("Field Id: ", &res);
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
	int		res, i;

	i = get_int_param("Area Id: ", &res);
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
				idrInfo.IdrId, areaType, areaId, &nextareaId,
				&areaHeader);
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

ret_code_t inv_block(void)
{
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	char			buf[256];
	ret_code_t		ret;
	term_def_t		*term;
	int			res;

	ret = ask_rpt(&rptid);
	if (ret != HPI_SHELL_OK) return(ret);
	type = SAHPI_INVENTORY_RDR;
	ret = ask_rdr(rptid, type, &rdrnum);
	if (ret != HPI_SHELL_OK) return(ret);
	rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum,
		&rdr_entry);
	if (rv != SA_OK) {
		printf("saHpiRdrGetByInstrumentId error %s\n", oh_lookup_error(rv));
		printf("ERROR!!! Can not get rdr: ResourceId=%d RdrType=%d RdrNum=%d\n",
			rptid, type, rdrnum);
		return(HPI_SHELL_CMD_ERROR);
	};
	show_inventory(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		block_type = INV_COM;
		res = get_new_command("inventory block ==> ");
		if (res == 2) {
			unget_term();
			return HPI_SHELL_OK;
		};
		term = get_next_term();
		if (term == NULL) continue;
		snprintf(buf, 256, "%s", term->term);
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
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
		}
	};
	block_type = MAIN_COM;
	return SA_OK;
}

ret_code_t show_inv(void)
{
	SaHpiResourceIdT	resid = 0;
	int			i, res;
	term_def_t		*term;

	term = get_next_term();
	if (term == NULL) {
		i = show_rpt_list(Domain, SHOW_ALL_RPT, resid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(SA_OK);
		};
		i = get_int_param("RPT ID ==> ", &res);
		if (i == 1) resid = (SaHpiResourceIdT)res;
		else return SA_OK;
	} else {
		resid = (SaHpiResourceIdT)atoi(term->term);
	};
			
	return sa_show_inv(resid);
}

static int set_control_state(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiCtrlNumT num)
{
        SaErrorT		rv;
	SaHpiRdrT		rdr;
	SaHpiCtrlRecT		*ctrl;
	SaHpiCtrlTypeT		type;
	SaHpiCtrlStateDigitalT	state_val = 0;
	SaHpiCtrlStateT		state;
	char			buf[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	char			*str;
	int			i, res;

	rv = saHpiRdrGetByInstrumentId(sessionid, resourceid, SAHPI_CTRL_RDR, num, &rdr);
	if (rv != SA_OK) {
		printf("saHpiRdrGetByInstrumentId: error: %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	memset(&state, 0, sizeof(SaHpiCtrlStateT));
	ctrl = &(rdr.RdrTypeUnion.CtrlRec);
	type = ctrl->Type;
	state.Type = type;
	switch (type) {
		case SAHPI_CTRL_TYPE_DIGITAL:
			i = get_string_param(
				"New state(on|off|pulseon|pulseoff): ", buf, 9);
			if (i != 0) return(HPI_SHELL_CMD_ERROR);
			if (strcmp(buf, "on") == 0) state_val = SAHPI_CTRL_STATE_ON;
			if (strcmp(buf, "off") == 0) state_val = SAHPI_CTRL_STATE_OFF;
			if (strcmp(buf, "pulseon") == 0)
				state_val = SAHPI_CTRL_STATE_PULSE_ON;
			if (strcmp(buf, "pulseoff") == 0)
				state_val = SAHPI_CTRL_STATE_PULSE_OFF;
			state.StateUnion.Digital = state_val;
			break;
		case SAHPI_CTRL_TYPE_DISCRETE:
			i = get_int_param("Value ==> ", &res);
			if (i != 1) {
				printf("Invalid value\n");
				return HPI_SHELL_CMD_ERROR;
			};
			state.StateUnion.Discrete = res;
			break;
		case SAHPI_CTRL_TYPE_ANALOG:
			i = get_int_param("Value ==> ", &res);
			if (i != 1) {
				printf("Invalid value\n");
				return HPI_SHELL_CMD_ERROR;
			};
			state.StateUnion.Analog = res;
			break;
		case SAHPI_CTRL_TYPE_STREAM:
			i = get_string_param("Repeat(yes|no): ", buf, 4);
			if (i != 0) return(HPI_SHELL_CMD_ERROR);
			str = buf;
			while (*str == ' ') str++;
			if (strncmp(str, "yes", 3) == 0)
				state.StateUnion.Stream.Repeat = 1;
			i = get_string_param("Stream: ", buf, 4);
			i = strlen(buf);
			if (i > 4) i = 4;
			state.StateUnion.Stream.StreamLength = i;
			strncpy(state.StateUnion.Stream.Stream, buf, i);
			break;
		case SAHPI_CTRL_TYPE_TEXT:
			i = get_int_param("Line #: ", &res);
			if (i != 1) {
				printf("Invalid value\n");
				return HPI_SHELL_CMD_ERROR;
			};
			state.StateUnion.Text.Line = res;
			i = get_string_param("Text: ", buf, SAHPI_MAX_TEXT_BUFFER_LENGTH);
			str = buf;
			while (*str == ' ') str++;
			i = strlen(str);
			while ((i > 0) && (str[i - 1] == ' ')) i--;
			str[i] = 0;
			if (i == 0) {
				printf("Invalid text: %s\n", buf);
				return(HPI_SHELL_CMD_ERROR);
			};
			strcpy(state.StateUnion.Text.Text.Data, str);
			state.StateUnion.Text.Text.DataType = SAHPI_TL_TYPE_TEXT;
			state.StateUnion.Text.Text.Language = SAHPI_LANG_ENGLISH;
			state.StateUnion.Text.Text.DataLength = i;
			break;
		case SAHPI_CTRL_TYPE_OEM:
			i = get_int_param("Manufacturer Id: ", &res);
			if (i != 1) {
				printf("Invalid value\n");
				return HPI_SHELL_CMD_ERROR;
			};
			state.StateUnion.Oem.MId = res;
			i = get_string_param("Oem body: ", buf,
				SAHPI_MAX_TEXT_BUFFER_LENGTH);
			str = buf;
			while (*str == ' ') str++;
			i = strlen(str);
			while ((i > 0) && (str[i - 1] == ' ')) i--;
			str[i] = 0;
			if (i == 0) {
				printf("Invalid text: %s\n", buf);
				return(HPI_SHELL_CMD_ERROR);
			};
			if (i > SAHPI_CTRL_MAX_OEM_BODY_LENGTH)
				i = SAHPI_CTRL_MAX_OEM_BODY_LENGTH;
			memset(state.StateUnion.Oem.Body, 0,
				SAHPI_CTRL_MAX_OEM_BODY_LENGTH);
			strncpy(state.StateUnion.Oem.Body, str, i);
			state.StateUnion.Oem.BodyLength = i;
			break;
		default:
			strcpy(buf, "Unknown control type\n");
			return(HPI_SHELL_CMD_ERROR);
	};
	rv = saHpiControlSet(sessionid, resourceid, num,
		SAHPI_CTRL_MODE_MANUAL, &state);
	if (rv != SA_OK) {
		printf("saHpiControlSet: error: %s\n",
			oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	return(SA_OK);
}

ret_code_t ctrl_block(void)
{
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	char			buf[256];
	ret_code_t		ret;
	term_def_t		*term;
	int			res;

	ret = ask_rpt(&rptid);
	if (ret != HPI_SHELL_OK) return(ret);
	type = SAHPI_CTRL_RDR;
	ret = ask_rdr(rptid, type, &rdrnum);
	if (ret != HPI_SHELL_OK) return(ret);
	rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum,
		&rdr_entry);
	if (rv != SA_OK) {
		printf("saHpiRdrGetByInstrumentId error %s\n", oh_lookup_error(rv));
		printf("ERROR!!! Can not get rdr: ResourceId=%d RdrType=%d RdrNum=%d\n",
			rptid, type, rdrnum);
		return(HPI_SHELL_CMD_ERROR);
	};
	show_control(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		block_type = CTRL_COM;
		res = get_new_command("control block ==> ");
		if (res == 2) {
			unget_term();
			return HPI_SHELL_OK;
		};
		term = get_next_term();
		if (term == NULL) continue;
		snprintf(buf, 256, "%s", term->term);
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
		if (strcmp(buf, "state") == 0) {
			show_control_state(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		if (strcmp(buf, "setstate") == 0) {
			set_control_state(Domain->sessionId, rptid, rdrnum);
			continue;
		};
		if (strcmp(buf, "show") == 0) {
			show_control(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		}
	};
	block_type = MAIN_COM;
	return SA_OK;
}
