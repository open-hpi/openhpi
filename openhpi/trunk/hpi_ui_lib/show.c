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
#include <hpi_ui.h>

#define SHOW_BUF_SZ	1024

extern char	*lookup_proc(int num, int val);
extern SaErrorT	decode_proc(int num, void *val, char *buf, int bufsize);
extern SaErrorT	decode1_proc(int num, int val, char *buf, int bufsize);
extern SaErrorT	thres_value(SaHpiSensorReadingT *item, char *buf, int size);

SaErrorT sensor_list(SaHpiSessionIdT sessionid, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv = SA_OK;
	SaHpiRptEntryT		rptentry;
	SaHpiEntryIdT		rptentryid;
	SaHpiEntryIdT		nextrptentryid;

	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
		if (rv != SA_OK)
			break;
		rv = show_sensor_list(sessionid, rptentry.ResourceId, proc);
		if (rv == -1)
			return(-1);
		rptentryid = nextrptentryid;
	};
	return(rv);
}

int print_thres_value(SaHpiSensorReadingT *item, char *info,
	SaHpiSensorThdDefnT *def, int num, hpi_ui_print_cb_t proc)
{
	char	*val;
	char	mes[256];
	char	buf[SHOW_BUF_SZ];

	if (item->IsSupported != SAHPI_TRUE) {
		snprintf(buf, SHOW_BUF_SZ, "%s     not supported.\n", info);
		proc(buf);
		return(0);
	};
	strcpy(mes, info);
	if (def != (SaHpiSensorThdDefnT *)NULL) {
		if (def->ReadThold & num) {
			if (def->WriteThold & num) strcat(mes, "(RW)");
			else strcat(mes, "(RO)");
		} else {
			if (def->WriteThold & num) strcat(mes, "(WO)");
			else strcat(mes, "(NA)");
		}
	};
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			snprintf(buf, SHOW_BUF_SZ, "%s %lld\n", mes,
				item->Value.SensorInt64);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			snprintf(buf, SHOW_BUF_SZ, "%s %llu\n", mes,
				item->Value.SensorUint64);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			snprintf(buf, SHOW_BUF_SZ, "%s %10.3f\n", mes,
				item->Value.SensorFloat64);
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = (char *)(item->Value.SensorBuffer);
			if (val != NULL) {
				snprintf(buf, SHOW_BUF_SZ, "%s %s\n", mes, val);
				break;
			}
			return(0);
	};
	return(proc(buf));
}

int show_threshold(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum, SaHpiSensorRecT *sen, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv;
	SaHpiSensorThresholdsT	senstbuff;
	SaHpiSensorThdDefnT	*sendef;
	SaHpiSensorTypeT	type;
	SaHpiEventCategoryT	categ;
	char			buf[SHOW_BUF_SZ];
	int			res;

	sendef = &(sen->ThresholdDefn);
	rv = saHpiSensorTypeGet(sessionid, resourceid, sensornum, &type, &categ);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorTypeGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	};
	if (categ != SAHPI_EC_THRESHOLD)
		return(SA_OK);
	memset(&senstbuff, 0, sizeof(SaHpiSensorThresholdsT));
	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorThresholdsGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	};
	if (sendef->IsAccessible == 0) {
		proc("Thresholds are not accessible.\n");
		return -1; 
	};
	res = print_thres_value(&(senstbuff.LowMinor), "Lower Minor Threshold",
		sendef, SAHPI_ES_LOWER_MINOR, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.LowMajor), "Lower Major Threshold",
		sendef, SAHPI_ES_LOWER_MAJOR, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.LowCritical), "Lower Critical Threshold",
		sendef, SAHPI_ES_LOWER_CRIT, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpMinor), "Upper Minor Threshold",
		sendef, SAHPI_ES_UPPER_MINOR, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpMajor), "Upper Major Threshold",
		sendef, SAHPI_ES_UPPER_MAJOR, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpCritical), "Upper Critical Threshold",
		sendef, SAHPI_ES_UPPER_CRIT, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.PosThdHysteresis),
		"Positive Threshold Hysteresis", NULL, 0, proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.NegThdHysteresis),
		"Negative Threshold Hysteresis", NULL, 0, proc);
	return SA_OK;
}

SaErrorT show_sensor(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc)
{
        SaHpiSensorReadingT	reading;
	SaHpiEventStateT	status, assert, deassert;
	SaHpiRdrT		rdr;
        SaErrorT		rv;
	SaHpiBoolT		val;
	char			buf[SHOW_BUF_SZ];
	char			errbuf[SHOW_BUF_SZ];

	rv = saHpiRdrGetByInstrumentId(sessionid, resourceid, SAHPI_SENSOR_RDR,
		sensornum, &rdr);
	if (rv != SA_OK) {
		snprintf(errbuf, SHOW_BUF_SZ,
			"\nERROR: saHpiRdrGetByInstrumentId: error: %s\n",
			oh_lookup_error(rv));
		proc(errbuf);
		return(rv);
	};
	snprintf(buf, SHOW_BUF_SZ, "Sensor(%d/%d) ", resourceid, sensornum);
	rv = saHpiSensorEnableGet(sessionid, resourceid, sensornum, &val);
	if (rv != SA_OK) {
		snprintf(errbuf, SHOW_BUF_SZ,
			"\nERROR: saHpiSensorEnableGet: error: %s\n",
			oh_lookup_error(rv));
		if (proc(errbuf) != 0) return(rv);
	} else {
		if (val) strcat(buf, "Enable ");
		else strcat(buf, "Disable ");
	};
	rv = saHpiSensorEventEnableGet(sessionid, resourceid, sensornum, &val);
	if (rv != SA_OK) {
		snprintf(errbuf, SHOW_BUF_SZ,
			"\nERROR: saHpiSensorEventEnableGet: error: %s\n",
			oh_lookup_error(rv));
		if (proc(errbuf) != 0) return(rv);
	} else {
		strcat(buf, "    event : ");
		if (val) strcat(buf, "Enable");
		else strcat(buf, "Disable");
	};
	strcat(buf, "\n");
	if (proc(buf) != 0) return(SA_OK);
	rv = saHpiSensorEventMasksGet(sessionid, resourceid, sensornum, &assert, &deassert);
	if (rv != SA_OK)
		snprintf(errbuf, SHOW_BUF_SZ,
			"\nERROR: saHpiSensorEventMasksGet: error: %s\n",
			oh_lookup_error(rv));
	else
		snprintf(buf, SHOW_BUF_SZ,
			"   supported: 0x%4.4x  masks: assert = 0x%4.4x"
			"   deassert = 0x%4.4x\n",
			rdr.RdrTypeUnion.SensorRec.Events, assert, deassert);
	if (proc(buf) != 0) return(rv);
	rv = saHpiSensorReadingGet(sessionid, resourceid, sensornum,
		&reading, &status);
        if (rv != SA_OK)  {
		snprintf(buf, SHOW_BUF_SZ, "\nERROR: saHpiSensorReadingGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return rv;
        }

        if (reading.IsSupported) {
		snprintf(buf, SHOW_BUF_SZ, "     Sensor status = %x", status);
		proc(buf);
		print_thres_value(&reading, "     Reading Value =", NULL, 0, proc);
	};

	show_threshold(sessionid, resourceid, sensornum,
		&(rdr.RdrTypeUnion.SensorRec), proc);

	return SA_OK;
}

SaErrorT show_event_log(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	int show_short, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv = SA_OK;
	SaHpiRptEntryT		rptentry;
	SaHpiEventLogInfoT	info;
	SaHpiEventLogEntryIdT	entryid;
	SaHpiEventLogEntryIdT	nextentryid;
	SaHpiEventLogEntryIdT	preventryid;
	SaHpiEventLogEntryT	sel;
	SaHpiRdrT		rdr;
	char			buf[SHOW_BUF_SZ];
	char			date[30], date1[30];

	if (resourceid != SAHPI_UNSPECIFIED_RESOURCE_ID) {
		rv = saHpiRptEntryGetByResourceId(sessionid, resourceid, &rptentry);
		if (rv != SA_OK) {
			snprintf(buf, SHOW_BUF_SZ,
				"ERROR: saHpiRptEntryGetByResourceId error = %s\n",
				oh_lookup_error(rv));
			proc(buf);
			return rv;
		};
		if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
			proc("ERROR: The designated resource hasn't SEL.\n");
			return rv;
		}
	};

	rv = saHpiEventLogInfoGet(sessionid, resourceid, &info);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiEventLogInfoGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1;
	}
	snprintf(buf, SHOW_BUF_SZ, "EventLog: entries = %d, size = %d, enabled = %d\n",
		info.Entries, info.Size, info.Enabled);
	proc(buf);
	time2str(info.UpdateTimestamp, date, 30);
	time2str(info.CurrentTime, date1, 30);
	snprintf(buf, SHOW_BUF_SZ, "UpdateTime = %s  CurrentTime = %s  Overflow = %d\n",
		date, date1, info.OverflowFlag);
	proc(buf);

	if (info.Entries != 0){
		entryid = SAHPI_OLDEST_ENTRY;
		while (entryid != SAHPI_NO_MORE_ENTRIES)
		{
			rv = saHpiEventLogEntryGet(sessionid, resourceid,
					entryid, &preventryid, &nextentryid,
					&sel, &rdr, NULL);
			if (rv != SA_OK) {
				snprintf(buf, SHOW_BUF_SZ,
					"ERROR: saHpiEventLogEntryGet error = %s\n",
					oh_lookup_error(rv));
				proc(buf);
				return -1;
			};
			if (show_short)
				show_short_event(&(sel.Event), proc);
			else
				oh_print_eventlogentry(&sel, 1);

			preventryid = entryid;
			entryid = nextentryid;
		}
	} else {
		proc("SEL is empty\n");
	};
	return SA_OK;
}

SaErrorT show_sensor_list(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
		hpi_ui_print_cb_t proc)
{
	SaErrorT	rv = SA_OK;
	SaHpiEntryIdT	entryid;
	SaHpiEntryIdT	nextentryid;
	SaHpiRdrT	rdr;
	char		buf[SHOW_BUF_SZ];
	int		len;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRdrGet(sessionid, resourceid, entryid, &nextentryid, &rdr);
		if (rv != SA_OK)
			break;
		if (rdr.RdrType == SAHPI_SENSOR_RDR) {
			snprintf(buf, 256, "Resource Id: %d, Sensor Num: %d",
				resourceid, rdr.RdrTypeUnion.SensorRec.Num);
			len = rdr.IdString.DataLength;
			if (len > 0) {
				rdr.IdString.Data[len] = 0;
				strcat(buf, " Tag: ");
				strcat(buf, rdr.IdString.Data);
			};
			strcat(buf, "\n");
			if (proc(buf) != 0)
				return(-1);
		};
		entryid = nextentryid;
	};
	return(rv);
}

int show_rdr_list(Domain_t *domain, SaHpiResourceIdT rptid, SaHpiRdrTypeT passed_type,
	hpi_ui_print_cb_t proc)
//  return: list size
{
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;
	int			len, ind;
	char			buf[SHOW_BUF_SZ];
	SaHpiRdrTypeT		type;
	char			ar[256];
	SaHpiSensorRecT		*sensor;
	SaHpiCtrlRecT		*ctrl;
	SaHpiInventoryRecT	*inv;
	SaHpiWatchdogRecT	*wdog;
	SaErrorT		ret;
	int			res_num = 0;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		ret = saHpiRdrGet(domain->sessionId, rptid, entryid, &nextentryid, &rdr);
		if (ret != SA_OK)
			return(res_num);
		type = rdr.RdrType;
		if ((passed_type != SAHPI_NO_RECORD) && (type != passed_type)) {
			entryid = nextentryid;
			continue;
		};
		snprintf(buf, SHOW_BUF_SZ, "  (%d):%s NUM=", rdr.RecordId,
			oh_lookup_rdrtype(type));
		switch (type) {
			case SAHPI_SENSOR_RDR:
				sensor = &(rdr.RdrTypeUnion.SensorRec);
				snprintf(ar, 256, "%3.3d Ctrl=%d EvtCtrl=",
					sensor->Num, sensor->EnableCtrl);
				switch (sensor->EventCtrl) {
					case SAHPI_SEC_PER_EVENT:
						strcat(ar, "WR"); break;
					case SAHPI_SEC_READ_ONLY_MASKS:
						strcat(ar, "RM"); break;
					default:
						strcat(ar, "RO"); break;
				};
				break;
			case SAHPI_CTRL_RDR:
				ctrl = &(rdr.RdrTypeUnion.CtrlRec);
				snprintf(ar, 256, "%3.3d", ctrl->Num);
				break;
			case SAHPI_INVENTORY_RDR:
				inv = &(rdr.RdrTypeUnion.InventoryRec);
				snprintf(ar, 256, "%3.3d", inv->IdrId);
				break;
			case SAHPI_WATCHDOG_RDR:
				wdog = &(rdr.RdrTypeUnion.WatchdogRec);
				snprintf(ar, 256, "%3.3d", wdog->WatchdogNum);
				break;
			default:
				snprintf(ar, 256, "%c", '?');
		};
		strcat(buf, ar);
		len = rdr.IdString.DataLength;
		if (len > 0) {
			rdr.IdString.Data[len] = 0;
			ind = strlen(buf);
			snprintf(buf + ind, SHOW_BUF_SZ - ind - 1, " Tag=%s",
				rdr.IdString.Data);
		};
		strcat(buf, "\n");
		res_num++;
		if (proc(buf) != 0)
			return(res_num);
		entryid = nextentryid;
	};
	return(res_num);
}

int show_rpt_list(Domain_t *domain, int as, SaHpiResourceIdT rptid,
	hpi_ui_print_cb_t proc)
/*  as : SHOW_ALL_RPT  -  show all rpt entry only
 *	 SHOW_ALL_RDR  -  show all rdr for all rpt
 *	 SHOW_RPT_RDR  -  show all rdr for rptid
 *  return: list size
 */
{
	SaHpiRptEntryT		rpt_entry;
	SaHpiEntryIdT		rptentryid, nextrptentryid;
	int			ind = 0, len;
	char			buf[SHOW_BUF_SZ];
	SaErrorT		rv;
	SaHpiCapabilitiesT	cap;
	SaHpiHsCapabilitiesT	hscap;
	SaHpiHsStateT		state;
	int			res_num = 0;

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(domain->sessionId, rptentryid, &nextrptentryid,
			&rpt_entry);
		if (rv != SA_OK)
			return(res_num);
		if ((as == SHOW_RPT_RDR) && (rpt_entry.ResourceId != rptid)) {
			rptentryid = nextrptentryid;
			continue;
		};
		snprintf(buf, SHOW_BUF_SZ, "(%3.3d):", rpt_entry.ResourceId);
		ind = strlen(buf);
		len = rpt_entry.ResourceTag.DataLength;
		if (len > 0) {
			rpt_entry.ResourceTag.Data[len] = 0;
			strcat(buf, rpt_entry.ResourceTag.Data);
			strcat(buf, ":");
		};
		strcat(buf, "{");
		cap = rpt_entry.ResourceCapabilities;
		if (cap & SAHPI_CAPABILITY_SENSOR) strcat(buf, "S|");
		if (cap & SAHPI_CAPABILITY_RDR) strcat(buf, "RDR|");
		if (cap & SAHPI_CAPABILITY_EVENT_LOG) strcat(buf, "ELOG|");
		if (cap & SAHPI_CAPABILITY_INVENTORY_DATA) strcat(buf, "INV|");
		if (cap & SAHPI_CAPABILITY_RESET) strcat(buf, "RST|");
		if (cap & SAHPI_CAPABILITY_POWER) strcat(buf, "PWR|");
		if (cap & SAHPI_CAPABILITY_ANNUNCIATOR) strcat(buf, "AN|");
		if (cap & SAHPI_CAPABILITY_FRU) strcat(buf, "FRU|");
		if (cap & SAHPI_CAPABILITY_CONTROL) strcat(buf, "CNT|");
		if (cap & SAHPI_CAPABILITY_WATCHDOG) strcat(buf, "WTD|");
		if (cap & SAHPI_CAPABILITY_MANAGED_HOTSWAP) strcat(buf, "HS|");
		if (cap & SAHPI_CAPABILITY_CONFIGURATION) strcat(buf, "CF |");
		if (cap & SAHPI_CAPABILITY_AGGREGATE_STATUS) strcat(buf, "AG|");
		if (cap & SAHPI_CAPABILITY_EVT_DEASSERTS) strcat(buf, "DS|");
		if (cap & SAHPI_CAPABILITY_RESOURCE) strcat(buf, "RES|");
		ind  = strlen(buf);
		if (buf[ind - 1] == '|')
			buf[ind - 1] = 0;
		strcat(buf, "}");
		rv = saHpiHotSwapStateGet(domain->sessionId, rpt_entry.ResourceId,
			&state);
		hscap = rpt_entry.HotSwapCapabilities;
		if ((rv == SA_OK) || (hscap != 0)) {
			strcat(buf, "  HS={");
			if (rv == SA_OK)
				strcat(buf, oh_lookup_hsstate(state));
			if (hscap != 0) {
				if (hscap & SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY)
					strcat(buf, " RD|");
				if (hscap & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED)
					strcat(buf, " SUP|");
				ind  = strlen(buf);
				if (buf[ind - 1] == '|')
					buf[ind - 1] = 0;
			};
			strcat(buf, "}");
		};
		strcat(buf, "\n");
		res_num++;
		if (proc(buf) != 0)
			return(res_num);
		if (as == SHOW_ALL_RDR)
			show_rdr_list(domain, rpt_entry.ResourceId, SAHPI_NO_RECORD, proc);
		rptentryid = nextrptentryid;
	};
	return(res_num);
}

static int show_attrs(Attributes_t *Attrs, int delta, hpi_ui_print_cb_t proc)
{
	int		i, type, len, del;
	char		tmp[256], *name;
	char		buf[SHOW_BUF_SZ];
	union_type_t	val;
	SaErrorT	rv;

	memset(buf, ' ', SHOW_BUF_SZ);
	del = delta << 1;
	len = SHOW_BUF_SZ - del;
	for (i = 0; i < Attrs->n_attrs; i++) {
		name = get_attr_name(Attrs, i);
		if (name == (char *)NULL)
			break;
		rv = get_value(Attrs, i, &val);
		if (rv != SA_OK) continue;
		type = get_attr_type(Attrs, i);
		switch (type) {
			case NO_TYPE:	continue;
			case STRUCT_TYPE:
				snprintf(buf+ del, len, "%s:\n", name);
				if (proc(buf) != 0)
					return(-1);
				rv = get_value(Attrs, i, &val);
				if (rv != SA_OK) continue;
				show_attrs((Attributes_t *)(val.a), delta + 1, proc);
				continue;
			case LOOKUP_TYPE:
				strncpy(tmp, lookup_proc(Attrs->Attrs[i].lunum,
					val.i), 256);
				break;
			case DECODE_TYPE:
				rv = decode_proc(Attrs->Attrs[i].lunum, val.a, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case DECODE1_TYPE:
				rv = decode1_proc(Attrs->Attrs[i].lunum, val.i, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case READING_TYPE:
				if (thres_value(val.a, tmp, 256) != SA_OK)
					continue;
				break;
			default:
				rv = get_value_as_string(Attrs, i, tmp, 256);
				if (rv != SA_OK) continue;
		};
		snprintf(buf + del, len, "%s: %s\n", name, tmp);
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}
	
SaErrorT show_Rpt(Rpt_t *Rpt, hpi_ui_print_cb_t proc)
{
	int			i = 0, type;
	Attributes_t		*attrs;
	union_type_t		val;
	char			buf[SHOW_BUF_SZ];
	char			tmp[256], *name;
	SaErrorT		rv;

	attrs = &(Rpt->Attrutes);
	for (i = 0; i < attrs->n_attrs; i++) {
		name = get_attr_name(attrs, i);
		if (name == (char *)NULL)
			break;
		type = get_attr_type(attrs, i);
		rv = get_value(attrs, i, &val);
		if (rv != SA_OK) continue;
		switch (type) {
			case NO_TYPE:	continue;
			case STRUCT_TYPE:
				snprintf(buf, SHOW_BUF_SZ, "%s:\n", name);
				if (proc(buf) != 0) return(-1);
				if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
					return(-1);
				continue;
			case LOOKUP_TYPE:
				strncpy(tmp, lookup_proc(attrs->Attrs[i].lunum,
					val.i), 256);
				break;
			case DECODE_TYPE:
				rv = decode_proc(attrs->Attrs[i].lunum, val.a, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case DECODE1_TYPE:
				rv = decode1_proc(attrs->Attrs[i].lunum, val.i, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case READING_TYPE:
				thres_value(val.a, tmp, 256);
				continue;
			default:
				rv = get_value_as_string(attrs, i, tmp, 256);
				if (rv != SA_OK) continue;
		};
		snprintf(buf, SHOW_BUF_SZ, "%s: %s\n", name, tmp);
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}

SaErrorT show_Rdr(Rdr_t *Rdr, hpi_ui_print_cb_t proc)
{
	int		i = 0, type;
	Attributes_t	*attrs;
	char		buf[SHOW_BUF_SZ];
	char		tmp[256], *name;
	SaErrorT	rv;
	union_type_t	val;

	attrs = &(Rdr->Attrutes);
	for (i = 0; i < attrs->n_attrs; i++) {
		name = get_attr_name(attrs, i);
		if (name == (char *)NULL)
			break;
		type = get_attr_type(attrs, i);
		rv = get_value(attrs, i, &val);
		if (rv != SA_OK) continue;
		switch (type) {
			case NO_TYPE:	continue;
			case STRUCT_TYPE:
				snprintf(buf, SHOW_BUF_SZ, "%s:\n", name);
				if (proc(buf) != 0) return(-1);
				if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
					return(-1);
				continue;
			case LOOKUP_TYPE:
				strncpy(tmp, lookup_proc(attrs->Attrs[i].lunum,
					val.i), 256);
				break;
			case DECODE_TYPE:
				rv = decode_proc(attrs->Attrs[i].lunum, val.a, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case DECODE1_TYPE:
				rv = decode1_proc(attrs->Attrs[i].lunum, val.i, tmp, 256);
				if (rv != SA_OK) continue;
				break;
			case READING_TYPE:
				thres_value(val.a, tmp, 256);
				continue;
			default:
				rv = get_value_as_string(attrs, i, tmp, 256);
				if (rv != SA_OK) continue;
		};
		snprintf(buf, SHOW_BUF_SZ, "%s: %s\n", name, tmp);
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}

void show_short_event(SaHpiEventT *event, hpi_ui_print_cb_t proc)
{
	SaHpiTextBufferT	tmbuf;
	SaHpiSensorEventT	*sen;
	SaErrorT		rv;
	char			buf[SHOW_BUF_SZ];

	rv = oh_decode_time(event->Timestamp, &tmbuf);
	if (rv)
		snprintf(buf, SHOW_BUF_SZ, "%19s ", "TIME UNSPECIFIED");
	else
		snprintf(buf, SHOW_BUF_SZ, "%19s ", tmbuf.Data);
	proc(buf);
	snprintf(buf, SHOW_BUF_SZ, "%s ", oh_lookup_eventtype(event->EventType));
	proc(buf);
	switch (event->EventType) {
		case SAHPI_ET_SENSOR:
			sen = &(event->EventDataUnion.SensorEvent);
			snprintf(buf, SHOW_BUF_SZ, "%s %d/%d %s %s ",
				oh_lookup_sensortype(sen->SensorType),
				event->Source, sen->SensorNum,
				oh_lookup_severity(event->Severity),
				oh_lookup_eventcategory(sen->EventCategory));
			proc(buf);
			rv = oh_decode_eventstate(sen->EventState,  sen->EventCategory,
				   &tmbuf);
			if (rv == SA_OK) {
				snprintf(buf, SHOW_BUF_SZ, "%s:", tmbuf.Data);
				if (sen->Assertion == SAHPI_TRUE)
					strcat(buf, "ASSERTED");
				else
					strcat(buf, "DEASSERTED");
				proc(buf);
			};
			break;
		case SAHPI_ET_RESOURCE:
			snprintf(buf, SHOW_BUF_SZ, "%d %s %s",
				event->Source, oh_lookup_severity(event->Severity),
				oh_lookup_resourceeventtype(event->EventDataUnion.
					ResourceEvent.ResourceEventType));
			proc(buf);
			break;
		case SAHPI_ET_HOTSWAP:
			snprintf(buf, SHOW_BUF_SZ, "%d %s %s -> %s",
				event->Source, oh_lookup_severity(event->Severity),
				oh_lookup_hsstate(
				event->EventDataUnion.HotSwapEvent.PreviousHotSwapState),
				oh_lookup_hsstate(
				event->EventDataUnion.HotSwapEvent.HotSwapState));
			proc(buf);
			break;						 
		default:
			snprintf(buf, SHOW_BUF_SZ, "%d", event->Source);
			proc(buf);
			break;
	};
	proc("\n");
}

SaErrorT show_dat(Domain_t *domain, hpi_ui_print_cb_t proc)
{
	SaHpiAlarmT	alarm;
	SaErrorT	rv = SA_OK;
	char		buf[SHOW_BUF_SZ];
	char		time[256];
	int		ind;

	alarm.AlarmId = SAHPI_FIRST_ENTRY;
	while (rv == SA_OK) {
		rv = saHpiAlarmGetNext(domain->sessionId, SAHPI_ALL_SEVERITIES, FALSE, &alarm);
		if (rv != SA_OK) break;
		snprintf(buf, SHOW_BUF_SZ, "(%d) ", alarm.AlarmId);
		time2str(alarm.Timestamp, time, 256);
		strcat(buf, time);
		strcat(buf, " ");
		strcat(buf, oh_lookup_severity(alarm.Severity));
		if (alarm.Acknowledged) strcat(buf, " a ");
		else strcat(buf, " - ");
		strcat(buf, oh_lookup_statuscondtype(alarm.AlarmCond.Type));
		ind = strlen(buf);
		if (alarm.AlarmCond.Type == SAHPI_STATUS_COND_TYPE_SENSOR) {
			snprintf(buf + ind, SHOW_BUF_SZ - ind, " %d/%d 0x%x",
				alarm.AlarmCond.ResourceId, alarm.AlarmCond.SensorNum,
				alarm.AlarmCond.EventState);
		} else if (alarm.AlarmCond.Type == SAHPI_STATUS_COND_TYPE_OEM) {
			snprintf(buf + ind, SHOW_BUF_SZ - ind, " OEM = %d",
				alarm.AlarmCond.Mid);
			break;
		};
		strcat(buf, "\n");
		if (proc(buf) != 0)
			return(-1);
	};
	if (rv == SA_ERR_HPI_NOT_PRESENT)
		return(SA_OK);
	return(rv);
}
