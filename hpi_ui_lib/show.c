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
	char			buf[1024];

	rv = saHpiSensorEventEnableGet(sessionid, resourceid, sensornum, &status);
	if (rv != SA_OK) {
		snprintf(buf, 1024, "ERROR: saHpiSensorEventEnableGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	}

	rv = saHpiSensorEventMasksGet(
			sessionid, resourceid, sensornum, &assert, &deassert);
	if (rv != SA_OK) {
		snprintf(buf, 1024, "ERROR: saHpiSensorEventMasksGet error %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1; 
	}

	snprintf(buf, 1024, "Sensor Event Masks: \nSensor Event Status: %x\n"
		"Assert Events: %x\nDeassert Events: %x\n", status, assert, deassert);
	proc(buf);
        return SA_OK;
}

int print_thres_value(SaHpiSensorReadingT *item, char *mes, hpi_ui_print_cb_t proc)
{
	char	*val;
	char	buf[1024];

	if (item->IsSupported != SAHPI_TRUE)
		return(0);
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			snprintf(buf, 1024, "%s %lld\n", mes, item->Value.SensorInt64);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			snprintf(buf, 1024, "%s %llu\n", mes, item->Value.SensorUint64);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			snprintf(buf, 1024, "%s %10.3f\n", mes, item->Value.SensorFloat64);
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = (char *)(item->Value.SensorBuffer);
			if (val != NULL) {
				snprintf(buf, 1024, "%s %s\n", mes, val);
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
	char			buf[1024];
	int			res;

	rv = saHpiSensorThresholdsGet(sessionid, resourceid, sensornum, &senstbuff);
	if (rv != SA_OK) {
		snprintf(buf, 1024, "ERROR: saHpiSensorThresholdsGet error = %s\n",
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
	char			buf[1024];

        rv = saHpiSensorReadingGet(sessionid, resourceid, sensornum,
		&reading, &status);
        if (rv != SA_OK)  {
		snprintf(buf, 1024, "ERROR: saHpiSensorReadingGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return rv;
        }

        if (reading.IsSupported) {
		snprintf(buf, 1024, " : Sensor status = %x", status);
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
	char			buf[1024];
	char			date[30], date1[30];

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
		if (rv != SA_OK) {
			snprintf(buf, 1024, "ERROR: saHpiRptEntryGet error = %s\n",
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
		snprintf(buf, 1024, "ERROR: no resource for resourceId = %d\n", resourceid);
		proc(buf);
		return rv;
	}

	rv = saHpiEventLogInfoGet(sessionid, resourceid, &info);
	if (rv != SA_OK) {
		snprintf(buf, 1024, "ERROR: saHpiEventLogInfoGet error = %s\n",
			oh_lookup_error(rv));
		proc(buf);
		return -1;
	}
	snprintf(buf, 1024, "EventLog: entries = %d, size = %d, enabled = %d\n",
		info.Entries, info.Size, info.Enabled);
	proc(buf);
	time2str(info.UpdateTimestamp, date, 30);
	time2str(info.CurrentTime, date1, 30);
	snprintf(buf, 1024, "UpdateTime = %s  CurrentTime = %s  Overflow = %d\n",
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
				snprintf(buf, 1024, "ERROR: saHpiEventLogEntryGet error = %s\n",
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
		snprintf(buf, 1024, "SEL is empty\n");
		proc(buf);
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
	char		buf[256];

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRdrGet(sessionid, resourceid, entryid, &nextentryid, &rdr);
		if (rv != SA_OK)
			break;
		if (rdr.RdrType == SAHPI_SENSOR_RDR) {
			snprintf(buf, 256, "Resource Id: %d, Sensor Num: %d\n",
				resourceid, rdr.RdrTypeUnion.SensorRec.Num);
			if (proc(buf) != 0)
				return(-1);
		};
		entryid = nextentryid;
	};
	return(rv);
}

SaErrorT show_rpt_list(Domain_t *D, hpi_ui_print_cb_t proc)
{
	Rpt_t			*Rpt;
	int			i, ind = 0;
	char			buf[1024];
//	char			A[256];
	SaErrorT		rv;
	oh_big_textbuffer	tmpbuf;
	SaHpiCapabilitiesT	cap;
	SaHpiHsCapabilitiesT	hscap;

	Rpt = D->rpts;
	for (i = 0; i < D->n_rpts; i++, Rpt++) {
		snprintf(buf, 1024, "Id: %3.3d", Rpt->Rpt->ResourceId);
		ind = strlen(buf);
		strcat(buf, "  Cflags={");
		cap = Rpt->Rpt->ResourceCapabilities;
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
		hscap = Rpt->Rpt->HotSwapCapabilities;
		strcat(buf, "  HS={");
		if (hscap & SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY) strcat(buf, "RD|");
		if (hscap & SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED) strcat(buf, "SUP|");
		ind  = strlen(buf);
		if (buf[ind - 1] == '|')
			buf[ind - 1] = 0;
		strcat(buf, "}");
		ind = strlen(buf);
		oh_init_bigtext(&tmpbuf);
		rv = oh_decode_entitypath(&(Rpt->Rpt->ResourceEntity), &tmpbuf);
		if (rv == SA_OK) {
			snprintf(buf + ind, 1024 - ind, "  EPath: %s", tmpbuf.Data);
		};
		strcat(buf, "\n");
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}

static int show_attrs(Attributes_t *Attrs, int del, hpi_ui_print_cb_t proc)
{
	int		i, type, len;
	char		A[256], *name;
	char		buf[1024];
	union_type_t	val;
	SaErrorT	rv;

	memset(buf, ' ', 1024);
	del <<= 1;
	len = 1024 - del;
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
	Attributes_t		*Attrs;
	union_type_t		val;
	char			buf[1024];
	char			A[256], *name;
	SaHpiTextBufferT	tbuf;
	SaErrorT		rv;

	Attrs = &(Rpt->Attrutes);
	for (i = 0; i < Attrs->n_attrs; i++) {
		name = get_attr_name(Attrs, i);
		if (name == (char *)NULL)
			break;
		type = get_attr_type(Attrs, i);
		if (type == STRUCT_TYPE) {
			snprintf(buf, 1024, "%s:\n", name);
			if (proc(buf) != 0)
				return(-1);
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK) continue;
			if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
				return(-1);
		};
		if (strcmp(name, "TagLanguage") == 0) {
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			strncpy(A, oh_lookup_language(val.i), 256);
		} else if (strcmp(name, "TagType") == 0) {
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			strncpy(A, oh_lookup_texttype(val.i), 256);
		} else if (strcmp(name, "ResourceEntity") == 0) {
			oh_big_textbuffer tmpbuf;

			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			oh_init_bigtext(&tmpbuf);
			rv = oh_decode_entitypath((SaHpiEntityPathT *)(val.a), &tmpbuf);
			if (rv != SA_OK)
				continue;
			strncpy(A, tmpbuf.Data, 256);
		} else if (strcmp(name, "Capabilities") == 0) {
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			rv = oh_decode_capabilities(val.i, &tbuf);
			if (rv != SA_OK)
				continue;
			strncpy(A, tbuf.Data, 256);
		} else if (strcmp(name, "HotSwapCapabilities") == 0) {
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			rv = oh_decode_hscapabilities(val.i, &tbuf);
			if (rv != SA_OK)
				continue;
			strncpy(A, tbuf.Data, 256);
		} else {
			rv = get_value_as_string(Attrs, i, A, 256);
			if (rv != SA_OK)
				continue;
		};
		snprintf(buf, 1024, "%s: %s\n", name, A);
		if (proc(buf) != 0)
			return(-1);
	};
	return(SA_OK);
}

SaErrorT show_Rdr(Rdr_t *Rdr, hpi_ui_print_cb_t proc)
{
	int		i = 0, type;
	Attributes_t	*Attrs;
	char		buf[1024];
	char		A[256], *name;
	SaErrorT	rv;
	union_type_t	val;

	Attrs = &(Rdr->Attrutes);
	for (i = 0; i < Attrs->n_attrs; i++) {
		name = get_attr_name(Attrs, i);
		if (name == (char *)NULL)
			break;
		type = get_attr_type(Attrs, i);
		if (type == STRUCT_TYPE) {
			snprintf(buf, 1024, "%s:\n", name);
			if (proc(buf) != 0)
				return(-1);
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK) continue;
			if (show_attrs((Attributes_t *)(val.a), 1, proc) != 0)
				return(-1);
			continue;
		};
		if (strcmp(name, "RdrType") == 0) {
			rv = get_value(Attrs, i, &val);
			if (rv != SA_OK)
				continue;
			strncpy(A, oh_lookup_rdrtype(val.i), 256);
		} else {
			rv = get_value_as_string(Attrs, i, A, 256);
			if (rv != SA_OK)
				continue;
		};
		snprintf(buf, 1024, "%s: %s\n", name, A);
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
					printf("DISASSERTED ");
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
