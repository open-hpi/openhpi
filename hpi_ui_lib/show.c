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

extern int find_rpt(Domain_t *Domain, SaHpiResourceIdT rptId);

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

SaErrorT show_sensor_status(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv;
	SaHpiEventStateT	assert;
	SaHpiEventStateT	deassert;
	SaHpiBoolT		status;
	char			buf[SHOW_BUF_SZ];

	rv = saHpiSensorEventEnableGet(sessionid, resourceid, sensornum, &status);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorEventEnableGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	}

	rv = saHpiSensorEventMasksGet(
			sessionid, resourceid, sensornum, &assert, &deassert);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorEventMasksGet error %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	}

	snprintf(buf, SHOW_BUF_SZ, "Sensor Event Masks: \nSensor Event Status: %x\n"
		"Assert Events: %x\nDeassert Events: %x\n", status, assert, deassert);
	proc(buf);
        return SA_OK;
}

int print_thres_value(SaHpiSensorReadingT *item, char *mes, hpi_ui_print_cb_t proc)
{
	char	*val;
	char	buf[SHOW_BUF_SZ];

	if (item->IsSupported != SAHPI_TRUE)
		return(0);
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
	SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv;
	SaHpiSensorThresholdsT	senstbuff;
	char			buf[SHOW_BUF_SZ];
	int			res;

	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff);
	if (rv != SA_OK) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorThresholdsGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	};
	res = print_thres_value(&(senstbuff.LowCritical), "Lower Critical Threshold(lc):",
		proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.LowMajor), "Lower Major Threshold(la):", proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.LowMinor), "Lower Minor Threshold(li):", proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpCritical), "Upper Critical Threshold(uc):",
		proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpMajor), "Upper Major Threshold(ua):", proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.UpMinor), "Upper Minor Threshold(ui):", proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.PosThdHysteresis),
		"Positive Threshold Hysteresis(ph):", proc);
	if (res != 0) return(0);
	res = print_thres_value(&(senstbuff.NegThdHysteresis),
		"Negative Threshold Hysteresis(nh):", proc);
	return SA_OK;
}

SaErrorT show_sensor(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc)
{
        SaHpiSensorReadingT	reading;
	SaHpiEventStateT	status;
        SaErrorT		rv;
	char			buf[SHOW_BUF_SZ];

        rv = saHpiSensorReadingGet(sessionid, resourceid, sensornum,
		&reading, &status);
        if (rv != SA_OK)  {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiSensorReadingGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return rv;
        }

        if (reading.IsSupported) {
		snprintf(buf, SHOW_BUF_SZ, " : Sensor status = %x", status);
		proc(buf);
		print_thres_value(&reading, "  Reading Value =", proc);
	};

	show_sensor_status(sessionid, resourceid, sensornum, proc);
	show_threshold(sessionid, resourceid, sensornum, proc);

	return SA_OK;
}

SaErrorT show_event_log(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
	int show_short, hpi_ui_print_cb_t proc)
{
	SaErrorT		rv = SA_OK;
	SaHpiRptEntryT		rptentry;
	SaHpiEntryIdT		rptentryid;
	SaHpiEntryIdT		nextrptentryid;
	SaHpiEventLogInfoT	info;
	SaHpiEventLogEntryIdT	entryid;
	SaHpiEventLogEntryIdT	nextentryid;
	SaHpiEventLogEntryIdT	preventryid;
	SaHpiEventLogEntryT	sel;
	SaHpiRdrT		rdr;
	char			buf[SHOW_BUF_SZ];
	char			date[30], date1[30];

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
		if (rv != SA_OK) {
			snprintf(buf, SHOW_BUF_SZ, "ERROR: saHpiRptEntryGet error = %s\n",
				oh_lookup_error(rv));
			proc(buf);
			return rv;
		};
		if (rptentry.ResourceId == resourceid) {
			if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)
				break;
			else {
				proc("ERROR: The designated resource hasn't SEL.\n");
				return rv;
			}
		};
		rptentryid = nextrptentryid;
	}
	if (rptentryid == SAHPI_LAST_ENTRY) {
		snprintf(buf, SHOW_BUF_SZ, "ERROR: no resource for resourceId = %d\n",
			resourceid);
		proc(buf);
		return rv;
	}

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
				show_short_event(&(sel.Event));
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

SaErrorT show_rdr_list(Domain_t *domain, SaHpiResourceIdT rptid, SaHpiRdrTypeT passed_type,
	hpi_ui_print_cb_t proc)
{
	SaHpiRdrT		rdr;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;
	int			len, ind;
	char			buf[SHOW_BUF_SZ];
	SaHpiRdrTypeT		type;
	char			ar[256];
	SaHpiSensorRecT		*sensor;
	SaErrorT		ret;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		ret = saHpiRdrGet(domain->sessionId, rptid, entryid, &nextentryid, &rdr);
		if (ret != SA_OK)
			return(ret);
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
				snprintf(ar, 256, "%d", sensor->Num);
				break;
			default:
				snprintf(ar, 256, "%d", 0);
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
		if (proc(buf) != 0)
			return(-1);
		entryid = nextentryid;
	};
	return(SA_OK);
}

SaErrorT show_rpt_list(Domain_t *domain, int as, SaHpiResourceIdT rptid,
	hpi_ui_print_cb_t proc)
/*  as : SHOW_ALL_RPT  -  show all rpt entry only
 *	 SHOW_ALL_RDR  -  show all rdr for all rpt
 *	 SHOW_RPT_RDR  -  show all rdr for rptid
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

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(domain->sessionId, rptentryid, &nextrptentryid,
			&rpt_entry);
		if (rv != SA_OK)
			return(-1);
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
		if (proc(buf) != 0)
			return(-1);
		if (as == SHOW_ALL_RDR)
			show_rdr_list(domain, rpt_entry.ResourceId, SAHPI_NO_RECORD, proc);
		rptentryid = nextrptentryid;
	};
	return(SA_OK);
}

static int show_attrs(Attributes_t *Attrs, int del, hpi_ui_print_cb_t proc)
{
	int		i, type, len;
	char		A[256], *name;
	char		buf[SHOW_BUF_SZ];
	union_type_t	val;
	SaErrorT	rv;

	memset(buf, ' ', SHOW_BUF_SZ);
	del <<= 1;
	len = SHOW_BUF_SZ - del;
	for (i = 0; i < Attrs->n_attrs; i++) {
		name = get_attr_name(Attrs, i);
		if (name == (char *)NULL)
			break;
		type = get_attr_type(Attrs, i);
		if (type == STRUCT_TYPE) {
			snprintf(buf+ del, len, "%s:\n", name);
			if (proc(buf) != 0)
				return(-1);
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK) continue;
			show_attrs((Attributes_t *)(val.a), 1, proc);
		};
		rv = get_value_as_string(Attrs, i, A, 256);
		if (rv != SA_OK)
			continue;
		snprintf(buf + del, len, "%s: %s\n", name, A);
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
	SaHpiTextBufferT	tbuf;
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
			case STRUCT_TYPE:
				snprintf(buf, SHOW_BUF_SZ, "%s:\n", name);
				if (proc(buf) != 0) return(-1);
				if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
					return(-1);
				continue;
			case LOOKUP_TYPE:
				strncpy(tmp, attrs->Attrs[i].proc(attrs->Attrs[i].lunum,
					val.i), 256);
				break;
			default:
				if (strcmp(name, "ResourceEntity") == 0) {
					oh_big_textbuffer tmpbuf;

					oh_init_bigtext(&tmpbuf);
					rv = oh_decode_entitypath(
						(SaHpiEntityPathT *)(val.a), &tmpbuf);
					if (rv != SA_OK) continue;
					strncpy(tmp, tmpbuf.Data, 256);
					break;
				};
				if (strcmp(name, "Capabilities") == 0) {
					rv = oh_decode_capabilities(val.i, &tbuf);
					if (rv != SA_OK) continue;
					strncpy(tmp, tbuf.Data, 256);
					break;
				};
				if (strcmp(name, "HotSwapCapabilities") == 0) {
					rv = oh_decode_hscapabilities(val.i, &tbuf);
					if (rv != SA_OK) continue;
					strncpy(tmp, tbuf.Data, 256);
					break;
				};
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
			case STRUCT_TYPE:
				snprintf(buf, SHOW_BUF_SZ, "%s:\n", name);
				if (proc(buf) != 0) return(-1);
				if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
					return(-1);
				continue;
			case LOOKUP_TYPE:
				strncpy(tmp, attrs->Attrs[i].proc(attrs->Attrs[i].lunum,
					val.i), 256);
				break;
			default:
				if (strcmp(name, "EntityPath") == 0) {
					oh_big_textbuffer tmpbuf;

					oh_init_bigtext(&tmpbuf);
					rv = oh_decode_entitypath(
						(SaHpiEntityPathT *)(val.a), &tmpbuf);
					if (rv != SA_OK) continue;
					strncpy(tmp, tmpbuf.Data, 256);
				} else {
					rv = get_value_as_string(attrs, i, tmp, 256);
					if (rv != SA_OK) continue;
				}
		};
		snprintf(buf, SHOW_BUF_SZ, "%s: %s\n", name, tmp);
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}

void show_short_event(SaHpiEventT *event)
{
	SaHpiTextBufferT	tmbuf;
	SaHpiSensorEventT	*sen;
	SaErrorT		rv;

	rv = oh_decode_time(event->Timestamp, &tmbuf);
	if (rv)
		printf("%19s ", "TIME UNSPECIFIED");
	else
		printf("%19s ", tmbuf.Data);
	printf("%s ", oh_lookup_eventtype(event->EventType));
	switch (event->EventType) {
		case SAHPI_ET_SENSOR:
			sen = &(event->EventDataUnion.SensorEvent);
			printf("%s ", oh_lookup_sensortype(sen->SensorType));
			printf("%d/%d ", event->Source, sen->SensorNum);
			printf("%s ", oh_lookup_severity(event->Severity));
			printf("%s ", oh_lookup_eventcategory(sen->EventCategory));
			rv = oh_decode_eventstate(sen->EventState,  sen->EventCategory,
				   &tmbuf);
			if (rv == SA_OK) {
				printf("%s:", tmbuf.Data);
				if (sen->Assertion == SAHPI_TRUE)
					printf("ASSERTED ");
				else
					printf("DEASSERTED ");
			};
			break;
		case SAHPI_ET_RESOURCE:
			printf("%d ", event->Source);
			printf("%s ", oh_lookup_severity(event->Severity));
			printf("%s  ", oh_lookup_resourceeventtype(event->EventDataUnion.
					ResourceEvent.ResourceEventType)); 
			break;
		case SAHPI_ET_HOTSWAP:
			printf("%d ", event->Source);
			printf("%s ", oh_lookup_severity(event->Severity));
			printf("%s -> %s", oh_lookup_hsstate(
				event->EventDataUnion.HotSwapEvent.PreviousHotSwapState),
				oh_lookup_hsstate(
				event->EventDataUnion.HotSwapEvent.HotSwapState));
			break;						 
		default:
			printf("%d", event->Source);
			break;
	};
	printf("\n");
}

SaErrorT show_dat(Domain_t *domain, hpi_ui_print_cb_t proc)
{
	SaHpiAlarmT	alarm;
	SaErrorT	rv = SA_OK;
	char		buf[SHOW_BUF_SZ];
	char		time[256];

	alarm.AlarmId = SAHPI_FIRST_ENTRY;
	while (rv == SA_OK) {
		rv = saHpiAlarmGetNext(domain->sessionId, SAHPI_ALL_SEVERITIES, FALSE, &alarm);
		if (rv != SA_OK) break;
		snprintf(buf, SHOW_BUF_SZ, "ID: %d Time: ", alarm.AlarmId);
		time2str(alarm.Timestamp, time, 256);
		strcat(buf, time);
		strcat(buf, "Sever: ");
		strcat(buf, oh_lookup_severity(alarm.Severity));
		strcat(buf, "Acknol: ");
		if (alarm.Acknowledged) strcat(buf, "TRUE");
		else strcat(buf, "FALSE");
		strcat(buf, "CondType: ");
		strcat(buf, oh_lookup_statuscondtype(alarm.AlarmCond.Type));
		strcat(buf, "\n");
		if (proc(buf) != 0)
			return(-1);
	};
	if (rv == SA_ERR_HPI_NOT_PRESENT)
		return(SA_OK);
	return(rv);
}
