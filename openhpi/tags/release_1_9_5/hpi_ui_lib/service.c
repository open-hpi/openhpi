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

#define RPT_ATTRS_NUM	12

#define LANG_PROC	1
#define TAGTYPE_PROC	2
#define RDRTYPE_PROC	3

extern char *lookup_proc(int num, int val);

attr_t	Def_rpt[] = {
	{ "EntryId",		INT_TYPE,	0, { .d = 0}, 0 },	//  0
	{ "ResourceId",		INT_TYPE,	0, { .d = 0}, 0 },	//  1
	{ "ResourceInfo",	STRUCT_TYPE,	0, { .d = 0}, 0 },	//  2
	{ "ResourceEntity",	ARRAY_TYPE,	0, { .d = 0}, 0 },	//  3
	{ "Capabilities",	INT_TYPE,	0, { .d = 0}, 0 },	//  4
	{ "HotSwapCapabilities",INT_TYPE,	0, { .d = 0}, 0 },	//  5
	{ "ResourceSeverity",	INT_TYPE,	0, { .d = 0}, 0 },	//  6
	{ "ResourceFailed",	INT_TYPE,	0, { .d = 0}, 0 },	//  7
	{ "Tag",		STR_TYPE,	0, { .d = 0}, 0 },	//  8
	{ "TagLength",		INT_TYPE,	0, { .d = 0}, 0 },	//  9
	{ "TagType",		LOOKUP_TYPE,	TAGTYPE_PROC,	{ .d = 0}, lookup_proc },	// 10
	{ "TagLanguage",	LOOKUP_TYPE,	LANG_PROC,	{ .d = 0}, lookup_proc }	// 11
};

#define RESINFO_ATTRS_NUM	9

attr_t	Def_resinfo[] = {
	{ "ResourceRev",	INT_TYPE,	0, { .d = 0}, 0 },	//  0
	{ "SpecificVer",	INT_TYPE,	0, { .d = 0}, 0 },	//  1
	{ "DeviceSupport",	INT_TYPE,	0, { .d = 0}, 0 },	//  2
	{ "ManufacturerId",	INT_TYPE,	0, { .d = 0}, 0 },	//  3
	{ "ProductId",		INT_TYPE,	0, { .d = 0}, 0 },	//  4
	{ "FirmwareMajorRev",	INT_TYPE,	0, { .d = 0}, 0 },	//  5
	{ "FirmwareMinorRev",	INT_TYPE,	0, { .d = 0}, 0 },	//  6
	{ "AuxFirmwareRev",	INT_TYPE,	0, { .d = 0}, 0 },	//  7
	{ "Guid",		STR_TYPE,	0, { .d = 0}, 0 }	//  8
};

int find_rpt(Domain_t *Domain, SaHpiResourceIdT rptId);
int find_rdr_by_num(Rpt_t *Rpt, SaHpiInstrumentIdT num);

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
	};
	if (string == (char *)NULL)
		return("");
	return(string);
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
	{ "RecordId",		INT_TYPE,	0, { .d = 0}, 0 },	//  0
	{ "RdrType",		LOOKUP_TYPE,	RDRTYPE_PROC, { .d = 0}, lookup_proc },	//  1
	{ "EntityPath",		ARRAY_TYPE,	0, { .d = 0}, 0 },	//  2
	{ "IsFru",		INT_TYPE,	0, { .d = 0}, 0 },	//  3
	{ "Record",		STRUCT_TYPE,	0, { .d = 0}, 0 },	//  4
	{ "IdString",		STR_TYPE,	0, { .d = 0}, 0 },	//  5
	{ "IdStringLength",	INT_TYPE,	0, { .d = 0}, 0 },	//  6
	{ "IdStringType",	LOOKUP_TYPE,	TAGTYPE_PROC,	{ .d = 0}, lookup_proc },	//  7
	{ "IdStringLang",	LOOKUP_TYPE,	LANG_PROC,	{ .d = 0}, lookup_proc }	//  8
};

#define RDR_ATTRS_SENSOR_NUM	9

attr_t	Def_sensor_rdr[] = {
	{ "Num",		INT_TYPE,	0, { .d = 0}, 0 },	//  0
	{ "Type",		INT_TYPE,	0, { .d = 0}, 0 },	//  1
	{ "Category",		INT_TYPE,	0, { .d = 0}, 0 },	//  2
	{ "EnableCtrl",		INT_TYPE,	0, { .d = 0}, 0 },	//  3
	{ "EventCtrl",		INT_TYPE,	0, { .d = 0}, 0 },	//  4
	{ "Events",		INT_TYPE,	0, { .d = 0}, 0 },	//  5
	{ "DataFormat",		STRUCT_TYPE,	0, { .d = 0}, 0 },	//  6
	{ "ThresholdDefn",	STRUCT_TYPE,	0, { .d = 0}, 0 },	//  7
	{ "Oem",		INT_TYPE,	0, { .d = 0}, 0 }	//  8
};

void make_attrs_rdr(Rdr_t *Rdr, SaHpiRdrT *rdrentry)
{
	attr_t		*att, *att1;
	int		len, i = 0;
	Attributes_t	*at;
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
