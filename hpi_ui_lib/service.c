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
#include <hpi_ui.h>

//	function numbers for lookup_proc

#define LANG_PROC	1
#define TAGTYPE_PROC	2
#define RDRTYPE_PROC	3
#define SENREADT_PROC	4
#define SENUNIT_PROC	5
#define SENMODU_PROC	6
#define SENTYPE_PROC	7
#define CATEGORY_PROC	8

//	function numbers for decode_proc

#define EPATH_PROC	1

//	function numbers for decode1_proc

#define CAPAB_PROC	1
#define HSCAPAB_PROC	2
#define THDMASK_PROC	3
#define RANGEMASK_PROC	4

extern char	*lookup_proc(int num, int val);
extern SaErrorT	decode_proc(int num, void *val, char *buf, int bufsize);
extern SaErrorT	decode1_proc(int num, int val, char *buf, int bufsize);
extern SaErrorT	thres_value(SaHpiSensorReadingT *item, char *buf, int size);

#define RPT_ATTRS_NUM	12

attr_t	Def_rpt[] = {
	{ "EntryId",		INT_TYPE,	0, { .d = 0} },	//  0
	{ "ResourceId",		INT_TYPE,	0, { .d = 0} },	//  1
	{ "ResourceInfo",	STRUCT_TYPE,	0, { .d = 0} },	//  2
	{ "ResourceEntity",	DECODE_TYPE,	EPATH_PROC, { .d = 0} },	//  3
	{ "Capabilities",	DECODE1_TYPE,	CAPAB_PROC, { .d = 0} },	//  4
	{ "HotSwapCapabilities",DECODE1_TYPE,	HSCAPAB_PROC, { .d = 0} },	//  5
	{ "ResourceSeverity",	INT_TYPE,	0, { .d = 0} },	//  6
	{ "ResourceFailed",	INT_TYPE,	0, { .d = 0} },	//  7
	{ "Tag",		STR_TYPE,	0, { .d = 0} },	//  8
	{ "TagLength",		INT_TYPE,	0, { .d = 0} },	//  9
	{ "TagType",		LOOKUP_TYPE,	TAGTYPE_PROC,	{ .d = 0} },	// 10
	{ "TagLanguage",	LOOKUP_TYPE,	LANG_PROC,	{ .d = 0} }	// 11
};

#define RESINFO_ATTRS_NUM	9

attr_t	Def_resinfo[] = {
	{ "ResourceRev",	INT_TYPE,	0, { .d = 0} },	//  0
	{ "SpecificVer",	INT_TYPE,	0, { .d = 0} },	//  1
	{ "DeviceSupport",	INT_TYPE,	0, { .d = 0} },	//  2
	{ "ManufacturerId",	INT_TYPE,	0, { .d = 0} },	//  3
	{ "ProductId",		INT_TYPE,	0, { .d = 0} },	//  4
	{ "FirmwareMajorRev",	INT_TYPE,	0, { .d = 0} },	//  5
	{ "FirmwareMinorRev",	INT_TYPE,	0, { .d = 0} },	//  6
	{ "AuxFirmwareRev",	INT_TYPE,	0, { .d = 0} },	//  7
	{ "Guid",		STR_TYPE,	0, { .d = 0} }	//  8
};

char *lookup_proc(int num, int val)
{
	char	*string = (char *)NULL;

	switch (num) {
		case LANG_PROC:
			string = oh_lookup_language(val); break;
		case TAGTYPE_PROC:
			string = oh_lookup_texttype(val); break;
		case RDRTYPE_PROC:
			string = oh_lookup_rdrtype(val); break;
		case SENREADT_PROC:
			string = oh_lookup_sensorreadingtype(val); break;
		case SENUNIT_PROC:
			string = oh_lookup_sensorunits(val); break;
		case SENMODU_PROC:
			string = oh_lookup_sensormodunituse(val); break;
		case SENTYPE_PROC:
			string = oh_lookup_sensortype(val); break;
		case CATEGORY_PROC:
			string = oh_lookup_eventcategory(val); break;
	};
	if (string == (char *)NULL)
		return("");
	return(string);
}

SaErrorT decode_proc(int num, void *val, char *buf, int bufsize)
{
	oh_big_textbuffer	tmpbuf;
	SaErrorT		rv;

	oh_init_bigtext(&tmpbuf);
	memset(buf, 0, bufsize);
	switch (num) {
		case EPATH_PROC:
			rv = oh_decode_entitypath((SaHpiEntityPathT *)val, &tmpbuf);
			if (rv != SA_OK) return(-1);
			break;
	};
	strncpy(buf, tmpbuf.Data, bufsize);
	return(SA_OK);
}

static void oh_threshold_mask(SaHpiSensorThdMaskT mask, char *buf, int bufsize)
{
	char	tmp[256];
	int	ind;

	memset(buf, 0, 256);
	if (mask == 0) return;
	strcpy(tmp, "{ ");
	if (mask & SAHPI_STM_LOW_MINOR)
		strcat(tmp, "LOW_MINOR | ");
	if (mask & SAHPI_STM_LOW_MAJOR)
		strcat(tmp, "LOW_MAJOR | ");
	if (mask & SAHPI_STM_LOW_CRIT)
		strcat(tmp, "LOW_CRIT | ");
	if (mask & SAHPI_STM_LOW_HYSTERESIS)
		strcat(tmp, "LOW_HYSTERESIS | ");
	if (mask & SAHPI_STM_UP_MINOR)
		strcat(tmp, "UP_MINOR | ");
	if (mask & SAHPI_STM_UP_MAJOR)
		strcat(tmp, "UP_MAJOR | ");
	if (mask & SAHPI_STM_UP_CRIT)
		strcat(tmp, "UP_CRIT | ");
	if (mask & SAHPI_STM_UP_HYSTERESIS)
		strcat(tmp, "UP_HYSTERESIS | ");

	ind = strlen(tmp);
	/* Remove last delimiter */
	if (tmp[ind - 2] == '{')
		// null mask
		return;
	tmp[ind - 2] = '}';
	tmp[ind - 1] = 0;
	strncpy(buf, tmp, bufsize);
	return;
}

static void oh_range_mask(SaHpiSensorRangeFlagsT mask, char *buf, int bufsize)
{
	char	tmp[256];
	int	ind;

	memset(buf, 0, 256);
	if (mask == 0) return;
	strcpy(tmp, "{ ");
	if (mask & SAHPI_SRF_MIN)
		strcat(tmp, "MIN | ");
	if (mask & SAHPI_SRF_MAX)
		strcat(tmp, "MAX | ");
	if (mask & SAHPI_SRF_NORMAL_MIN)
		strcat(tmp, "NORMAL MIN | ");
	if (mask & SAHPI_SRF_NORMAL_MAX)
		strcat(tmp, "NORMAL MAX | ");
	if (mask & SAHPI_SRF_NOMINAL)
		strcat(tmp, "NOMINAL | ");

	ind = strlen(tmp);
	/* Remove last delimiter */
	if (tmp[ind - 2] == '{')
		// null mask
		return;
	tmp[ind - 2] = '}';
	tmp[ind - 1] = 0;
	strncpy(buf, tmp, bufsize);
	return;
}

SaErrorT decode1_proc(int num, int val, char *buf, int bufsize)
{
	SaHpiTextBufferT	tbuf;
	SaErrorT		rv;

	memset(buf, 0, bufsize);
	switch (num) {
		case CAPAB_PROC:
			rv = oh_decode_capabilities(val, &tbuf);
			if (rv != SA_OK) return(-1);
			break;
		case HSCAPAB_PROC:
			rv = oh_decode_hscapabilities(val, &tbuf);
			if (rv != SA_OK) return(-1);
			break;
		case THDMASK_PROC:
			oh_threshold_mask(val, buf, bufsize);
			return(SA_OK);
		case RANGEMASK_PROC:
			oh_range_mask(val, buf, bufsize);
			return(SA_OK);
	};
	strncpy(buf, tbuf.Data, bufsize);
	return(SA_OK);
}

SaErrorT thres_value(SaHpiSensorReadingT *item, char *buf, int size)
{
	char	*val;

	memset(buf, 0, size);
	if (item->IsSupported != SAHPI_TRUE)
		return(-1);
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			snprintf(buf, size, "%lld", item->Value.SensorInt64);
			break;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			snprintf(buf, size, "%llu", item->Value.SensorUint64);
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			snprintf(buf, size, "%10.3f", item->Value.SensorFloat64);
			break;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = (char *)(item->Value.SensorBuffer);
			if (val != NULL) {
				snprintf(buf, size, "%s", val);
				break;
			}
			return(-1);
	};
	return(SA_OK);
}

static int find_attr(Attributes_t *attrs, char *name)
{
	int	i;

	for (i = 0; i < attrs->n_attrs; i++) {
		if (strcmp(attrs->Attrs[i].name, name) == 0)
			return(i);
	};
	return(-1);
}

void make_attrs_rpt(Rpt_t *Rpt, SaHpiRptEntryT *rptentry)
{
	attr_t			*att, *att1;
	int			len, i = 0;
	Attributes_t		*at;
	SaHpiRptEntryT		*obj;

	Rpt->Table = *rptentry;
	obj = &(Rpt->Table);
	Rpt->Attrutes.n_attrs = RPT_ATTRS_NUM;
	Rpt->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RPT_ATTRS_NUM);
	memcpy(Rpt->Attrutes.Attrs, Def_rpt, sizeof(attr_t) * RPT_ATTRS_NUM);
	att = Rpt->Attrutes.Attrs;
	att[i++].value.i = obj->EntryId;
	att[i++].value.i = obj->ResourceId;
	at = (Attributes_t *)malloc(sizeof(Attributes_t));
	at->n_attrs = RESINFO_ATTRS_NUM;
	att1 = (attr_t *)malloc(sizeof(attr_t) * RESINFO_ATTRS_NUM);
	memcpy(att1, Def_resinfo, sizeof(attr_t) * RESINFO_ATTRS_NUM);
	at->Attrs = att1;
	att[i++].value.a = at;
	att1[0].value.i = obj->ResourceInfo.ResourceRev;
	att1[1].value.i = obj->ResourceInfo.SpecificVer;
	att1[2].value.i = obj->ResourceInfo.DeviceSupport;
	att1[3].value.i = obj->ResourceInfo.ManufacturerId;
	att1[4].value.i = obj->ResourceInfo.ProductId;
	att1[5].value.i = obj->ResourceInfo.FirmwareMajorRev;
	att1[6].value.i = obj->ResourceInfo.FirmwareMinorRev;
	att1[7].value.i = obj->ResourceInfo.AuxFirmwareRev;
	att1[8].value.s = (char *)(obj->ResourceInfo.Guid);
	att[i++].value.a = &(obj->ResourceEntity);
	att[i++].value.i = obj->ResourceCapabilities;
	att[i++].value.i = obj->HotSwapCapabilities;
	att[i++].value.i = obj->ResourceSeverity;
	att[i++].value.i = obj->ResourceFailed;
	len = obj->ResourceTag.DataLength;
	if (len > 0) {
		att[i++].value.s = obj->ResourceTag.Data;
		obj->ResourceTag.Data[len] = 0;
		att[i++].value.i = len;
		att[i++].value.i = obj->ResourceTag.DataType;
		att[i++].value.i = obj->ResourceTag.Language;
	}
}

#define RDR_ATTRS_COMMON_NUM	9

attr_t	Def_common_rdr[] = {
	{ "RecordId",		INT_TYPE,	0, { .d = 0} },	//  0
	{ "RdrType",		LOOKUP_TYPE,	RDRTYPE_PROC, { .d = 0} },	//  1
	{ "EntityPath",		DECODE_TYPE,	EPATH_PROC, { .d = 0} },	//  2
	{ "IsFru",		INT_TYPE,	0, { .d = 0} },	//  3
	{ "Record",		STRUCT_TYPE,	0, { .d = 0} },	//  4
	{ "IdString",		STR_TYPE,	0, { .d = 0} },	//  5
	{ "IdStringLength",	INT_TYPE,	0, { .d = 0} },	//  6
	{ "IdStringType",	LOOKUP_TYPE,	TAGTYPE_PROC,	{ .d = 0} },	//  7
	{ "IdStringLang",	LOOKUP_TYPE,	LANG_PROC,	{ .d = 0} }	//  8
};

#define RDR_ATTRS_SENSOR_NUM	9

attr_t	Def_sensor_rdr[] = {
	{ "Num",		INT_TYPE,	0, { .d = 0} },	//  0
	{ "Type",		LOOKUP_TYPE,	SENTYPE_PROC, { .d = 0} },  //  1
	{ "Category",		LOOKUP_TYPE,	CATEGORY_PROC, { .d = 0} }, //  2
	{ "EnableCtrl",		INT_TYPE,	0, { .d = 0} },	//  3
	{ "EventCtrl",		INT_TYPE,	0, { .d = 0} },	//  4
	{ "Events",		INT_TYPE,	0, { .d = 0} },	//  5
	{ "DataFormat",		STRUCT_TYPE,	0, { .d = 0} },	//  6
	{ "ThresholdDefn",	STRUCT_TYPE,	0, { .d = 0} },	//  7
	{ "Oem",		INT_TYPE,	0, { .d = 0} }	//  8
};

#define ATTRS_SENSOR_DATAFORMAT	8

attr_t	DataForm_rdr[] = {
	{ "IsSupported",	INT_TYPE,	0, { .d = 0} },	//  0
	{ "ReadingType",	LOOKUP_TYPE,	SENREADT_PROC, { .d = 0} }, //  1
	{ "BaseUnits",		LOOKUP_TYPE,	SENUNIT_PROC, { .d = 0} },  //  2
	{ "ModifierUnits",	LOOKUP_TYPE,	SENUNIT_PROC, { .d = 0} },  //  3
	{ "ModifierUse",	LOOKUP_TYPE,	SENMODU_PROC, { .d = 0} },  //  4
	{ "Percentage",		INT_TYPE,	0, { .d = 0} },	//  5
	{ "Range",		STRUCT_TYPE,	0, { .d = 0} },	//  6
	{ "AccuracyFactor",	FLOAT_TYPE,	0, { .d = 0} }	//  7
};

#define ATTRS_SENSOR_THDDEF	4

attr_t	ThresDef_rdr[] = {
	{ "IsAccessible",	INT_TYPE,	0, { .d = 0} },	//  0
	{ "ReadMask",		DECODE1_TYPE,	THDMASK_PROC, { .d = 0} },	//  1
	{ "WriteMask",		DECODE1_TYPE,	THDMASK_PROC, { .d = 0} }, 	//  2
	{ "Nonlinear",		INT_TYPE,	0, { .d = 0} }	//  3
};

#define ATTRS_SENSOR_RANGE	6

attr_t	Range_rdr[] = {
	{ "Flags",		DECODE1_TYPE,	RANGEMASK_PROC, { .d = 0} },	//  0
	{ "Max",		READING_TYPE,	0, { .d = 0} },	//  1
	{ "Min",		READING_TYPE,	0, { .d = 0} }, 	//  2
	{ "Nominal",		READING_TYPE,	0, { .d = 0} },	//  3
	{ "NormalMax",		READING_TYPE,	0, { .d = 0} }, 	//  4
	{ "NormalMin",		READING_TYPE,	0, { .d = 0} }	//  5
};

void make_attrs_rdr(Rdr_t *Rdr, SaHpiRdrT *rdrentry)
{
	attr_t		*att, *att1, *att2, *att3;
	int		len, i = 0;
	Attributes_t	*at, *at2, *at3;
	SaHpiRdrT	*obj;
	SaHpiSensorRecT	*sensor;

	Rdr->Record = *rdrentry;
	obj = &(Rdr->Record);
	Rdr->Attrutes.n_attrs = RDR_ATTRS_COMMON_NUM;
	Rdr->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	memcpy(Rdr->Attrutes.Attrs, Def_common_rdr, sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	att = Rdr->Attrutes.Attrs;
	att[i++].value.i = obj->RecordId;
	att[i++].value.i = obj->RdrType;
	att[i++].value.a = &(obj->Entity);
	att[i++].value.i = obj->IsFru;
	len = obj->IdString.DataLength;
	switch (obj->RdrType) {
		case SAHPI_SENSOR_RDR:
			at = (Attributes_t *)malloc(sizeof(Attributes_t));
			at->n_attrs = RDR_ATTRS_SENSOR_NUM;
			att1 = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
			memcpy(att1, Def_sensor_rdr, sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
			at->Attrs = att1;
			att[i].name = "Sensor";
			att[i++].value.a = at;
			sensor = &(obj->RdrTypeUnion.SensorRec);
			att1[0].value.i = sensor->Num;
			att1[1].value.i = sensor->Type;
			att1[2].value.i = sensor->Category;
			att1[3].value.i = sensor->EnableCtrl;
			att1[4].value.i = sensor->EventCtrl;
			att1[5].value.i = sensor->Events;

			at2 = (Attributes_t *)malloc(sizeof(Attributes_t));
			at2->n_attrs = ATTRS_SENSOR_DATAFORMAT;
			att2 = (attr_t *)malloc(sizeof(attr_t) * ATTRS_SENSOR_DATAFORMAT);
			memcpy(att2, DataForm_rdr, sizeof(attr_t) * ATTRS_SENSOR_DATAFORMAT);
			at2->Attrs = att2;
			att2[0].value.i = sensor->DataFormat.IsSupported;
			att2[1].value.i = sensor->DataFormat.ReadingType;
			att2[2].value.i = sensor->DataFormat.BaseUnits;
			att2[3].value.i = sensor->DataFormat.ModifierUnits;
			att2[4].value.i = sensor->DataFormat.ModifierUse;
			att2[5].value.i = sensor->DataFormat.Percentage;

			at3 = (Attributes_t *)malloc(sizeof(Attributes_t));
			at3->n_attrs = ATTRS_SENSOR_RANGE;
			att3 = (attr_t *)malloc(sizeof(attr_t) * ATTRS_SENSOR_RANGE);
			memcpy(att3, Range_rdr, sizeof(attr_t) * ATTRS_SENSOR_RANGE);
			at3->Attrs = att3;
			att3[0].value.i = sensor->DataFormat.Range.Flags;
			att3[1].value.a = &(sensor->DataFormat.Range.Max);
			att3[2].value.a = &(sensor->DataFormat.Range.Min);
			att3[3].value.a = &(sensor->DataFormat.Range.Nominal);
			att3[4].value.a = &(sensor->DataFormat.Range.NormalMax);
			att3[5].value.a = &(sensor->DataFormat.Range.NormalMin);

			att2[6].value.a = at3;
			att2[7].value.d = sensor->DataFormat.AccuracyFactor;

			att1[6].value.a = at2;

			at2 = (Attributes_t *)malloc(sizeof(Attributes_t));
			at2->n_attrs = ATTRS_SENSOR_THDDEF;
			att2 = (attr_t *)malloc(sizeof(attr_t) * ATTRS_SENSOR_THDDEF);
			memcpy(att2, ThresDef_rdr, sizeof(attr_t) * ATTRS_SENSOR_THDDEF);
			att2[0].value.i = sensor->ThresholdDefn.IsAccessible;
			att2[1].value.i = sensor->ThresholdDefn.ReadThold;
			att2[2].value.i = sensor->ThresholdDefn.WriteThold;
			at2->Attrs = att2;
			att1[7].value.a = at2;

			att1[8].value.i = sensor->Oem;
			break;
		default: break;
	};
	len = obj->IdString.DataLength;
	if (len > 0) {
		att[i++].value.s = obj->IdString.Data;
		obj->IdString.Data[len] = 0;
		att[i++].value.i = len;
		att[i++].value.i = obj->IdString.DataType;
		att[i++].value.i = obj->IdString.Language;
	};
}

void free_attrs(Attributes_t *At)
{
	int		i;
	attr_t		*attr;

	for (i = 0, attr = At->Attrs; i < At->n_attrs; i++, attr++) {
		if (attr->type == STRUCT_TYPE) {
			if (attr->value.a == 0) continue;
			free_attrs((Attributes_t *)(attr->value.a));
			free(attr->value.a);
		}
	};
	free(At->Attrs);
}

void time2str(SaHpiTimeT time, char * str, size_t size)
{
	struct tm t;
	time_t tt;
	
	if (!str) return;

        if (time > SAHPI_TIME_MAX_RELATIVE) { /*absolute time*/
                tt = time / 1000000000;
                strftime(str, size, "%F %T", localtime(&tt));
        } else if (time ==  SAHPI_TIME_UNSPECIFIED) { 
                strcpy(str,"SAHPI_TIME_UNSPECIFIED     ");
        } else if (time > SAHPI_TIME_UNSPECIFIED) { /*invalid time*/
                strcpy(str,"invalid time     ");
        } else {   /*relative time*/
		tt = time / 1000000000;
		localtime_r(&tt, &t);
		strftime(str, size, "%b %d, %Y - %H:%M:%S", &t);
	}
}

char *get_attr_name(Attributes_t *Attrs, int num)
{
	if ((num < 0) || (num >= Attrs->n_attrs))
		return((char *)NULL);
	return(Attrs->Attrs[num].name);
}

int get_attr_type(Attributes_t *Attrs, int num)
{
	if ((num < 0) || (num >= Attrs->n_attrs))
		return(-1);
	return(Attrs->Attrs[num].type);
}

SaErrorT get_value_as_string(Attributes_t *Attrs, int num, char *val, int len)
{
	int	type;

	if ((num < 0) || (num >= Attrs->n_attrs) || (val == (char *)NULL) || (len == 0))
		return SA_ERR_HPI_INVALID_PARAMS;

	type = Attrs->Attrs[num].type;
	switch (type) {
		case INT_TYPE:
			snprintf(val, len, "%d", Attrs->Attrs[num].value.i);
			break;
		case FLOAT_TYPE:
			snprintf(val, len, "%f", Attrs->Attrs[num].value.d);
			break;
		case STR_TYPE:
			if (Attrs->Attrs[num].value.s != (char *)NULL)
				snprintf(val, len, "%s", Attrs->Attrs[num].value.s);
			else *val = 0;
			break;
		default:	return(SA_ERR_HPI_ERROR);
	};
	return(SA_OK);
}

SaErrorT get_rdr_attr_as_string(Rdr_t *rdr, char *attr_name, char *val, int len)
{
	int		i;
	SaErrorT	ret;

	if ((attr_name == (char *)NULL) || (val == (char *)NULL) || (len == 0))
		return(SA_ERR_HPI_INVALID_PARAMS);

	i = find_attr(&(rdr->Attrutes), attr_name);
	if (i < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	ret = get_value_as_string(&(rdr->Attrutes), i, val, len);
	return(ret);
}

SaErrorT get_rdr_attr(Rdr_t *rdr, char *attr_name, union_type_t *val)
{
	int	i;

	if ((attr_name == (char *)NULL) || (val == (union_type_t *)NULL))
		return(SA_ERR_HPI_INVALID_PARAMS);

	i = find_attr(&(rdr->Attrutes), attr_name);
	if (i < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	*val = rdr->Attrutes.Attrs[i].value;
	return(SA_OK);
}

SaErrorT get_rpt_attr_as_string(Rpt_t *rpt, char *attr_name, char *val, int len)
{
	int		i;
	SaErrorT	ret;

	if ((attr_name == (char *)NULL) || (val == (char *)NULL) || (len == 0))
		return(SA_ERR_HPI_INVALID_PARAMS);

	i = find_attr(&(rpt->Attrutes), attr_name);
	if (i < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	ret = get_value_as_string(&(rpt->Attrutes), i, val, len);
	return(ret);
}

SaErrorT get_rpt_attr(Rpt_t *rpt, char *attr_name, union_type_t *val)
{
	int	i;

	if ((attr_name == (char *)NULL) || (val == (union_type_t *)NULL))
		return(SA_ERR_HPI_INVALID_PARAMS);

	i = find_attr(&(rpt->Attrutes), attr_name);
	if (i < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	*val = rpt->Attrutes.Attrs[i].value;
	return(SA_OK);
}

Domain_t *init_resources(SaHpiSessionIdT session)
{
	Domain_t	*domain;

	domain = (Domain_t *)malloc(sizeof(Domain_t));
	memset(domain, 0, sizeof(Domain_t));
	domain->sessionId = session;
	return(domain);
}

SaErrorT get_value(Attributes_t *Attrs, int num, union_type_t *val)
{
	int	type;

	if ((num < 0) || (num >= Attrs->n_attrs) || (val == (union_type_t *)NULL))
		return SA_ERR_HPI_INVALID_PARAMS;
	type = Attrs->Attrs[num].type;
	if ((type == STR_TYPE) || (type == STRUCT_TYPE) || (type == ARRAY_TYPE)) {
		if (Attrs->Attrs[num].value.s == (char *)NULL)
			return(-1);
	};
	*val = Attrs->Attrs[num].value;
	return(SA_OK);
}
