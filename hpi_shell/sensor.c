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

typedef enum {
	THRES_LI = 0,
	THRES_LA,
	THRES_LC,
	THRES_UI,
	THRES_UA,
	THRES_UC,
	THRES_PH,
	THRES_NH
} thres_enum_t;

typedef struct {
	char	*name;
	char	*short_name;
} thres_name_def_t;

static thres_name_def_t thres_names[] = {
	{ "Lower Minor:",		"li" },
	{ "Lower Major:",		"la" },
	{ "Lower Critical:",		"lc" },
	{ "Upper Minor:",		"ui" },
	{ "Upper Major:",		"ua" },
	{ "Upper Critical:",		"uc" },
	{ "Positive Hysteresis:",	"ph" },
	{ "Negative Hysteresis:",	"nh" }
};

static void Set_thres_value(SaHpiSensorReadingT *item, double value)
{
	item->IsSupported = 1;
	item->Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
	item->Value.SensorFloat64 = value;
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

static int Get_and_set_thres_value(thres_enum_t num, SaHpiSensorReadingT *item)
{
	char	tmp[256];
	float	f;
	int	res, modify = 0;

	if (item->IsSupported) {
		printf("%s(%s) ==> ", thres_names[num].name, get_thres_value(item, tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(item, f);
		}
	};
	return(modify);
}

static ret_code_t set_threshold(SaHpiResourceIdT rptid, SaHpiRdrT *rdr)
{
	SaErrorT		rv;
	SaHpiSensorTypeT	type;
	SaHpiEventCategoryT	categ;
	SaHpiSensorThresholdsT	senstbuff;
	SaHpiSensorRangeT	*range;
	SaHpiSensorNumT		num = rdr->RdrTypeUnion.SensorRec.Num;
	int			modify = 0;
	char			tmp[256];

	rv = saHpiSensorTypeGet(Domain->sessionId, rptid, num, &type, &categ);
	if (rv != SA_OK) {
		printf("ERROR: saHpiSensorTypeGet error = %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR); 
	};
	if (categ != SAHPI_EC_THRESHOLD)
		return(HPI_SHELL_CMD_ERROR);
	rv = saHpiSensorThresholdsGet(Domain->sessionId, rptid, num, &senstbuff);
	if (rv != SA_OK) {
		printf("ERROR: saHpiSensorThresholdsGet error = %s\n",
			oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR); 
	};

	printf("Range (");
	range = &(rdr->RdrTypeUnion.SensorRec.DataFormat.Range);
	if ((range->Flags & SAHPI_SRF_MIN) == 0) printf("-");
	else printf("%s", get_thres_value(&(range->Min), tmp, 256));
	printf(":");
	if ((range->Flags & SAHPI_SRF_MAX) == 0) printf("-)\n");
	else printf("%s)\n", get_thres_value(&(range->Max), tmp, 256));

	if (Get_and_set_thres_value(THRES_LI, &(senstbuff.LowMinor)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_LA, &(senstbuff.LowMajor)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_LC, &(senstbuff.LowCritical)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_UI, &(senstbuff.UpMinor)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_UA, &(senstbuff.UpMajor)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_UC, &(senstbuff.UpCritical)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_PH, &(senstbuff.PosThdHysteresis)))
		modify = 1;
	
	if (Get_and_set_thres_value(THRES_NH, &(senstbuff.NegThdHysteresis)))
		modify = 1;
	
	if (modify == 0) return(HPI_SHELL_OK);
		
	print_thres_value(&(senstbuff.LowCritical), thres_names[THRES_LC].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMajor), thres_names[THRES_LA].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.LowMinor), thres_names[THRES_LI].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpCritical), thres_names[THRES_UC].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMajor), thres_names[THRES_UA].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.UpMinor), thres_names[THRES_UI].name,
		NULL, 0, ui_print);
	print_thres_value(&(senstbuff.PosThdHysteresis),
		thres_names[THRES_PH].name, NULL, 0, ui_print);
	print_thres_value(&(senstbuff.NegThdHysteresis),
		thres_names[THRES_NH].name, NULL, 0, ui_print);
	if (read_stdin) {
		printf("Set new threshold (yes|no) : ");
		fgets(tmp, 256, stdin);
		if (strncmp(tmp, "yes", 3) != 0) {
			printf("No action.\n");
			return(HPI_SHELL_OK);
		}
	};
	rv = saHpiSensorThresholdsSet(Domain->sessionId, rptid, num, &senstbuff);
	if (rv != SA_OK) {
		printf("saHpiSensorThresholdsSet error %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	} else
		printf("Sensor Threshold Value Set Succeed.\n");
	return(HPI_SHELL_OK);
}

static ret_code_t sensor_command_block(SaHpiResourceIdT rptid, SaHpiInstrumentIdT rdrnum,
	SaHpiRdrT *rdr_entry)
{
	SaHpiBoolT			val;
	SaHpiEventStateT		assert, deassert;
	SaHpiSensorEventMaskActionT	act;
	SaErrorT			rv;
	int				i;
	char				rep[10], *str;
	char				buf[256];

	show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		block_type = SEN_COM;
		i = get_string_param("sensor block ==> ", buf, 9);
		if (i != 0) continue;
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) {
			break;
		};
		if (strcmp(buf, "show") == 0) {
			show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		if (strcmp(buf, "setthres") == 0) {
			if (read_file) return(HPI_SHELL_PARM_ERROR);
			set_threshold(rptid, rdr_entry);
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
		unget_term();
		if (run_command() == 2) {
			unget_term();
			return(SA_OK);
		}
	};
	block_type = MAIN_COM;
	return(HPI_SHELL_OK);
}

ret_code_t sen_block(void)
{
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid = 0;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	int			res, i;
	ret_code_t		ret;
	term_def_t		*term;

	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_PARM_ERROR);
		i = show_rpt_list(Domain, SHOW_ALL_RPT, rptid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(HPI_SHELL_OK);
		};
		i = get_int_param("RPT ID ==> ", &res);
		if (i != 1) return(HPI_SHELL_OK);
		rptid = (SaHpiResourceIdT)res;
	} else {
		rptid = (SaHpiResourceIdT)atoi(term->term);
	};
	type = SAHPI_SENSOR_RDR;
	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_PARM_ERROR);
		i = show_rdr_list(Domain, rptid, type, ui_print);
		if (i == 0) {
			printf("No rdr for rpt: %d\n", rptid);
			return(HPI_SHELL_OK);
		};
		i = get_int_param("RDR NUM ==> ", &res);
		if (i != 1) return(HPI_SHELL_OK);
		rdrnum = (SaHpiInstrumentIdT)res;
	} else {
		rdrnum = (SaHpiInstrumentIdT)atoi(term->term);
	};
	rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum, &rdr_entry);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiRdrGetByInstrumentId(Rpt=%d RdrType=%d Rdr=%d): %s\n",
			rptid, type, rdrnum, oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	ret = sensor_command_block(rptid, rdrnum, &rdr_entry);
	return(ret);
}

ret_code_t list_sensor(void)
{
	sensor_list(Domain->sessionId, ui_print);
	return(HPI_SHELL_OK);
}
