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
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			item->Value.SensorInt64 = (SaHpiInt64T)value;
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			item->Value.SensorUint64 = (SaHpiUint64T)value;
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			item->Value.SensorFloat64 = (SaHpiFloat64T)value;
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			break;
	}
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
	char		tmp[256];
	float		f;
	int		res, modify = 0;

	if (item->IsSupported) {
		printf("%s(%s) ==> ", thres_names[num].name,
			get_thres_value(item, tmp, 256));
		fgets(tmp, 256, stdin);
		res = sscanf(tmp, "%f", &f);
		if (res == 1) {
			modify = 1;
			Set_thres_value(item, f);
		}
	};
	return(modify);
}

static ret_code_t set_threshold_packet(SaHpiSensorThresholdsT *senstbuff)
{
	term_def_t		*term;
	int			i;
	float			f;
	int			res;
	SaHpiSensorReadingT	*item;

	for (;;) {
		term = get_next_term();
		if (term == NULL) break;
		for (i = 0; i <= THRES_NH; i++) {
			if (strcmp(term->term, thres_names[i].short_name) == 0) break;
		};
		switch (i) {
			case THRES_LI: item = &(senstbuff->LowMinor); break;
			case THRES_LA: item = &(senstbuff->LowMajor); break;
			case THRES_LC: item = &(senstbuff->LowCritical); break;
			case THRES_UI: item = &(senstbuff->UpMinor); break;
			case THRES_UA: item = &(senstbuff->UpMajor); break;
			case THRES_UC: item = &(senstbuff->UpCritical); break;
			case THRES_PH: item = &(senstbuff->PosThdHysteresis); break;
			case THRES_NH: item = &(senstbuff->NegThdHysteresis); break;
			default: return(HPI_SHELL_PARM_ERROR);
		};
		term = get_next_term();
		if (term == NULL) return(HPI_SHELL_PARM_ERROR);
		res = sscanf(term->term, "%f", &f);
		if (res != 1) return(HPI_SHELL_PARM_ERROR);
		Set_thres_value(item, f);
	};
	return(HPI_SHELL_OK);
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
	ret_code_t		ret;

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

	if (read_stdin) {
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
		printf("Set new threshold (yes|no) : ");
		fgets(tmp, 256, stdin);
		if (strncmp(tmp, "yes", 3) != 0) {
			printf("No action.\n");
			return(HPI_SHELL_OK);
		}
	} else {
		ret = set_threshold_packet(&senstbuff);
		if (ret != HPI_SHELL_OK) return(ret);
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
	char				rep[10];
	char				buf[256];
	term_def_t			*term;
	int				res;

	show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
	for (;;) {
		block_type = SEN_COM;
		res = get_new_command("sensor block ==> ");
		if (res == 2) {
			unget_term();
			return HPI_SHELL_OK;
		};
		term = get_next_term();
		if (term == NULL) continue;
		snprintf(buf, 256, "%s", term->term);
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) {
			break;
		};
		if (strcmp(buf, "show") == 0) {
			show_sensor(Domain->sessionId, rptid, rdrnum, ui_print);
			continue;
		};
		if (strcmp(buf, "setthres") == 0) {
			set_threshold(rptid, rdr_entry);
			continue;
		};
		if ((strcmp(buf, "enable") == 0) || (strcmp(buf, "disable") == 0)) {
			if (strcmp(buf, "enable") == 0) val = 1;
			else val = 0;
			rv = saHpiSensorEnableSet(Domain->sessionId, rptid,
				rdrnum, val);
			if (rv != SA_OK) {
				printf("saHpiSensorEnableSet: error: %s\n",
					oh_lookup_error(rv));
				if (read_file) return(HPI_SHELL_PARM_ERROR);
				continue;
			};
			rv = saHpiSensorEnableGet(Domain->sessionId, rptid,
				rdrnum, &val);
			if (rv != SA_OK) {
				printf("saHpiSensorEnableGet: error: %s\n",
					oh_lookup_error(rv));
				if (read_file) return(HPI_SHELL_PARM_ERROR);
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
			rv = saHpiSensorEventEnableSet(Domain->sessionId,
				rptid, rdrnum, val);
			if (rv != SA_OK) {
				printf("saHpiSensorEventEnableSet: error: %s\n",
					oh_lookup_error(rv));
				if (read_file) return(HPI_SHELL_PARM_ERROR);
				continue;
			};
			rv = saHpiSensorEventEnableGet(Domain->sessionId,
				rptid, rdrnum, &val);
			if (rv != SA_OK) {
				printf("saHpiSensorEventEnableGet: error: %s\n",
					oh_lookup_error(rv));
				if (read_file) return(HPI_SHELL_PARM_ERROR);
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
				if (read_file) return(HPI_SHELL_PARM_ERROR);
				continue;
			};
			get_hex_int_param("Assert mask = 0x", &res);
			assert = res;
			get_hex_int_param("Deassert mask = 0x", &res);
			deassert = res;
			if (read_stdin) {
				snprintf(buf, 256,
					"Sensor:(%d/%d) %s masks: assert = 0x%4.4x   "
					"deassert = 0x%4.4x  (yes/no)?", rptid,
					rdrnum, rep, assert, deassert);
				get_string_param(buf, rep, 4);
				if (strcmp(rep, "yes") != 0) {
					printf("No action.\n");
					continue;
				}
			};
			rv = saHpiSensorEventMasksSet(Domain->sessionId, rptid, rdrnum,
				act, assert, deassert);
			if (rv != SA_OK) {
				printf("saHpiSensorEventMasksSet: error: %s\n",
					oh_lookup_error(rv));
				if (read_file) return(HPI_SHELL_PARM_ERROR);
			};
			continue;
		}
	};
	return(HPI_SHELL_OK);
}

ret_code_t sen_block(void)
{
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	ret_code_t		ret;

	ret = ask_rpt(&rptid);
	if (ret != HPI_SHELL_OK) return(ret);
	type = SAHPI_SENSOR_RDR;
	ret = ask_rdr(rptid, type, &rdrnum);
	if (ret != HPI_SHELL_OK) return(ret);
	rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type,
		rdrnum, &rdr_entry);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiRdrGetByInstrumentId"
			"(Rpt=%d RdrType=%d Rdr=%d): %s\n",
			rptid, type, rdrnum, oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	ret = sensor_command_block(rptid, rdrnum, &rdr_entry);
	block_type = MAIN_COM;
	return(ret);
}

ret_code_t list_sensor(void)
{
	sensor_list(Domain->sessionId, ui_print);
	return(HPI_SHELL_OK);
}

ret_code_t hs_block(void)
{
	SaHpiResourceIdT	rptid;
	SaHpiTimeoutT		timeout;
	SaHpiHsStateT		state;
	SaHpiHsActionT		action;
	SaHpiHsIndicatorStateT	ind_state;
	SaErrorT		rv;
	char			buf[256];
	char			*str;
	ret_code_t		ret;
	term_def_t		*term;
	int			res, i, ins, get;

	ret = ask_rpt(&rptid);
	if (ret != HPI_SHELL_OK) return(ret);
//	show_rdr_attrs(&rdr_entry);
	for (;;) {
		block_type = HS_COM;
		res = get_new_command("Hot swap block ==> ");
		if (res == 2) {
			unget_term();
			block_type = MAIN_COM;
			return HPI_SHELL_OK;
		};
		term = get_next_term();
		if (term == NULL) continue;
		snprintf(buf, 256, "%s", term->term);
		if ((strcmp(buf, "q") == 0) || (strcmp(buf, "quit") == 0)) break;
		if (strcmp(buf, "policycancel") == 0) {
			rv = saHpiHotSwapPolicyCancel(Domain->sessionId, rptid);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiHotSwapPolicyCancel: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		if (strcmp(buf, "active") == 0) {
			rv = saHpiResourceActiveSet(Domain->sessionId, rptid);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiResourceActiveSet: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		if (strcmp(buf, "inactive") == 0) {
			rv = saHpiResourceInactiveSet(Domain->sessionId, rptid);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiResourceInactiveSet: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		if ((strcmp(buf, "gettimeout") == 0) || (strcmp(buf, "settimeout") == 0)) {
			ins = -1;
			if (strcmp(buf, "gettimeout") == 0) get = 1;
			else get = 0;
			res = get_string_param("Timeout type(insert|extract): ",
				buf, 256);
			if (res != 0) {
				printf("Invalid timeout type");
				continue;
			};
			if (strcmp(buf, "insert") == 0) ins = 1;
			if (strcmp(buf, "extract") == 0) ins = 0;
			if (ins < 0) {
				printf("Invalid timeout type: %s\n", buf);
				continue;
			};
			if (get) {
				if (ins) {
					rv = saHpiAutoInsertTimeoutGet(Domain->sessionId,
						&timeout);
					if (rv != SA_OK) {
						printf("ERROR!!! saHpiAutoInsertTimeoutGet:"
							" %s\n", oh_lookup_error(rv));
					} else
						printf("Auto-insert timeout: %lld\n",
							timeout);
					continue;
				};
				rv = saHpiAutoExtractTimeoutGet(Domain->sessionId,
					rptid, &timeout);
				if (rv != SA_OK) {
					printf("ERROR!!! saHpiAutoExtractTimeoutGet: %s\n",
						oh_lookup_error(rv));
				} else {
					printf("Auto-extract timeout: %lld\n", timeout);
				};
				continue;
			};
			i = get_int_param("Timeout: ", &res);
			if (i != 1) {
				printf("Invalid timeout\n");
				continue;
			};
			timeout = res;
			if (ins) {
				rv = saHpiAutoInsertTimeoutSet(Domain->sessionId, timeout);
				if (rv != SA_OK) {
					printf("ERROR!!! saHpiAutoInsertTimeoutSet: %s\n",
						oh_lookup_error(rv));
				};
				continue;
			};
			rv = saHpiAutoExtractTimeoutSet(Domain->sessionId,
				rptid, timeout);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiAutoExtractTimeoutSet: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		if (strcmp(buf, "state") == 0) {
			rv = saHpiHotSwapStateGet(Domain->sessionId, rptid, &state);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiHotSwapStateGet: %s\n",
					oh_lookup_error(rv));
			} else {
				switch (state) {
					case SAHPI_HS_STATE_INACTIVE:
						str = "Inactive"; break;
					case SAHPI_HS_STATE_INSERTION_PENDING:
						str = "Ins. Pending"; break;
					case SAHPI_HS_STATE_ACTIVE:
						str = "Active"; break;
					case SAHPI_HS_STATE_EXTRACTION_PENDING:
						str = "Ext. Pending"; break;
					case SAHPI_HS_STATE_NOT_PRESENT:
						str = "Not present"; break;
					default:
						str = "Unknown"; break;
				};
				printf("Hot Swap State: %s\n", str);
			};
			continue;
		};
		if (strcmp(buf, "action") == 0) {
			res = get_string_param("Action type(insert|extract): ",
				buf, 256);
			if (res != 0) {
				printf("Invalid action type");
				continue;
			};
			if (strcmp(buf, "insert") == 0)
				action = SAHPI_HS_ACTION_INSERTION;
			else if (strcmp(buf, "extract") == 0)
				action = SAHPI_HS_ACTION_EXTRACTION;
			else {
				printf("Invalid action type: %s\n", buf);
				continue;
			};
			rv = saHpiHotSwapActionRequest(Domain->sessionId, rptid, action);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiHotSwapActionRequest: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		};
		if (strcmp(buf, "ind") == 0) {
			res = get_string_param("Action type(get|on|off): ",
				buf, 256);
			if (res != 0) {
				printf("Invalid action type");
				continue;
			};
			if (strcmp(buf, "get") == 0) {
				rv = saHpiHotSwapIndicatorStateGet(Domain->sessionId,
					rptid, &ind_state);
				if (rv != SA_OK) {
					printf("ERROR!!! saHpiHotSwapIndicatorStateGet: %s\n",
						oh_lookup_error(rv));
				} else {
					if (ind_state == SAHPI_HS_INDICATOR_OFF)
						str = "OFF";
					else str = "ON";
					printf("Hot Swap Indicator: %s\n", str);
				};
				continue;
			};
			if (strcmp(buf, "on") == 0)
				ind_state = SAHPI_HS_INDICATOR_ON;
			else if (strcmp(buf, "off") == 0)
				ind_state = SAHPI_HS_INDICATOR_OFF;
			else {
				printf("Invalid action type: %s\n", buf);
				continue;
			};
			rv = saHpiHotSwapIndicatorStateSet(Domain->sessionId,
				rptid, ind_state);
			if (rv != SA_OK) {
				printf("ERROR!!! saHpiHotSwapIndicatorStateSet: %s\n",
					oh_lookup_error(rv));
			};
			continue;
		}
	};
	block_type = MAIN_COM;
	return SA_OK;
}
