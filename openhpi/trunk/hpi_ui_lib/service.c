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

static void *resize_array(void *ar, int item_size, int *last_num, int add_num)
/* Resize array ar:
 *	item_size - item size (bytes)
 *	last_num  - current array size
 *	add_num   - new array size = last_num + add_num
 * Return:	new pointer
 */
{
	void	*tmp;
	int	new_num = *last_num + add_num;

	if (new_num <= 0)
		return((void *)NULL);
	tmp = malloc(item_size * new_num);
	memset(tmp, 0, item_size * new_num);
	if (*last_num > 0) {
		memcpy(tmp, ar, *last_num * item_size);
		free(ar);
	}
	*last_num = new_num;
	return(tmp);
}

static void clear_inited(Domain_t *Domain)
{
	int	i, j;

	for (i = 0; i < Domain->n_rpts; i++)
		for (j = 0; j < Domain->rpts[i].n_rdrs; j++)
			Domain->rpts[i].rdrs[j].is_inited = 0;
}

static void make_rpt_attrs(Rpt_t *Rpt)
{
	attr_t			*att, *att1;
	int			len, i = 0, newrpt = 1;
	Attributes_t		*at;
	SaHpiRptEntryT		*obj;

	obj = Rpt->Rpt;
	if (Rpt->Attrutes.n_attrs > 0)
		newrpt = 0;
	if (newrpt) {
		Rpt->Attrutes.n_attrs = RPT_ATTRS_NUM;
		Rpt->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RPT_ATTRS_NUM);
		memcpy(Rpt->Attrutes.Attrs, Def_rpt, sizeof(attr_t) * RPT_ATTRS_NUM);
	};
	att = Rpt->Attrutes.Attrs;
	att[i++].value.i = obj->EntryId;
	att[i++].value.i = obj->ResourceId;
	if (newrpt) {
		at = (Attributes_t *)malloc(sizeof(Attributes_t));
		at->n_attrs = RESINFO_ATTRS_NUM;
		att1 = (attr_t *)malloc(sizeof(attr_t) * RESINFO_ATTRS_NUM);
		memcpy(att1, Def_resinfo, sizeof(attr_t) * RESINFO_ATTRS_NUM);
		at->Attrs = att1;
		att[i++].value.a = at;
	} else {
		at = (Attributes_t *)(att[i++].value.a);
		att1 = at->Attrs;
	};
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

static void make_rdr_attrs(Rdr_t *Rdr)
{
	attr_t		*att, *att1;
	int		len, i = 0, newrdr = 1;
	Attributes_t	*at;
	SaHpiRdrT	*obj;
	SaHpiSensorRecT	*sensor;

	obj = Rdr->Rdr;
	if (Rdr->Attrutes.n_attrs > 0)
		newrdr = 0;
	if (newrdr) {
		Rdr->Attrutes.n_attrs = RDR_ATTRS_COMMON_NUM;
		Rdr->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
		memcpy(Rdr->Attrutes.Attrs, Def_common_rdr, sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	};
	att = Rdr->Attrutes.Attrs;
	att[i++].value.i = obj->RecordId;
	att[i++].value.i = obj->RdrType;
	att[i++].value.a = &(obj->Entity);
	att[i++].value.i = obj->IsFru;
	len = obj->IdString.DataLength;
	switch (obj->RdrType) {
		case SAHPI_SENSOR_RDR:
			if (newrdr) {
				at = (Attributes_t *)malloc(sizeof(Attributes_t));
				at->n_attrs = RDR_ATTRS_SENSOR_NUM;
				att1 = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
				memcpy(att1, Def_sensor_rdr, sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
				at->Attrs = att1;
				att[i].name = "Sensor";
				att[i++].value.a = at;
			} else {
				at = (Attributes_t *)(att[i++].value.a);
				att1 = at->Attrs;
			};
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

int find_rpt(Domain_t *Domain, SaHpiResourceIdT rptId)
{
	int	i;

	for (i = 0; i < Domain->n_rpts; i++) {
		if (Domain->rpts[i].ResourceId == rptId)
			return(i);
	};
	return(-1);
}

static int find_rdr(Rpt_t *Rpt, SaHpiEntryIdT rdrId)
{
	int	i;

	for (i = 0; i < Rpt->n_rdrs; i++) {
		if (Rpt->rdrs[i].RecordId == rdrId)
			return(i);
	};
	return(-1);
}

static void free_attrs(Attributes_t *At)
{
	int		i;
	attr_t		*attr;

	for (i = 0, attr = At->Attrs; i < At->n_attrs; i++, attr++) {
		if (attr->type == STRUCT_TYPE)
			free_attrs((Attributes_t *)(attr->value.a));
			free(attr->value.a);
	};
	free(At->Attrs);
}

static void delete_rdr(Rpt_t *Rpt, int ind)
{
	Rdr_t		*rdr, *rdr1;
	int		n, i;

	n = Rpt->n_rdrs;
	if ((ind < 0) || (ind >= n))
		return;
	rdr = Rpt->rdrs + ind;
	free_attrs(&(rdr->Attrutes));
	free(rdr->Rdr);
	free(rdr);
	if (n == 1) {
		Rpt->n_rdrs = 0;
		free(Rpt->rdrs);
		return;
	};
	rdr1 = rdr = (Rdr_t *)malloc(sizeof(Rdr_t) * (n - 1));
	for (i = 0; i < n; i++)
		if (i != ind) *rdr1++ = Rpt->rdrs[i];
	free(Rpt->rdrs);
	Rpt->rdrs = rdr;
}

static void delete_rpt(Domain_t *Domain, int ind)
{
	Rpt_t		*rpt_entry, *tmp;
	int		n, i;

	n = Domain->n_rpts;
	if ((ind < 0) || (ind >= n))
		return;
	rpt_entry = Domain->rpts + ind;
	free_attrs(&(rpt_entry->Attrutes));
	free(rpt_entry->Rpt);
	free(rpt_entry);
	if (n == 1) {
		Domain->n_rpts = 0;
		free(Domain->rpts);
		return;
	};
	tmp = rpt_entry = (Rpt_t *)malloc(sizeof(Rpt_t) * (n - 1));
	for (i = 0; i < n; i++)
		if (i != ind) *tmp++ = Domain->rpts[i];
	free(Domain->rpts);
	Domain->rpts = rpt_entry;
}

static SaErrorT init_rpt(Domain_t *Domain, Rpt_t *Rpt)
{
	SaHpiRdrT		rdr;
	Rdr_t			*rdr_entry;
	SaErrorT		ret, n;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;
	int			found, i;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		found = 0;
		ret = saHpiRdrGet(Domain->sessionId, Rpt->ResourceId, entryid,
			&nextentryid, &rdr);
		if (ret != SA_OK)
			return(ret);
		i = find_rdr(Rpt, rdr.RecordId);
		if (i != -1) {
			rdr_entry = Rpt->rdrs + i;
			*(rdr_entry->Rdr) = rdr;
		} else {
			n = Rpt->n_rdrs;
			Rpt->rdrs = (Rdr_t *)resize_array(Rpt->rdrs, sizeof(Rdr_t),
				&(Rpt->n_rdrs), 1);
			rdr_entry = Rpt->rdrs + n;
			rdr_entry->Rdr = (SaHpiRdrT *)malloc(sizeof(SaHpiRdrT));
			*(rdr_entry->Rdr) = rdr;
			Rpt->rdrs[n] = *rdr_entry;
		};
		rdr_entry->RecordId = rdr.RecordId;
		rdr_entry->RdrType = rdr.RdrType;
		rdr_entry->is_inited = 1;
		make_rdr_attrs(rdr_entry);
		entryid = nextentryid;
	};
	for (i = Rpt->n_rdrs - 1; i >= 0; i--) {
		if (Rpt->rdrs[i].is_inited == 0)
			delete_rdr(Rpt, i);
	};
	return(SA_OK);
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

static SaErrorT get_resources(Domain_t *Domain)
{
	SaHpiEntryIdT	rptentryid, nextrptentryid;
	SaErrorT	ret;
	SaHpiRptEntryT	rptentry;
	Rpt_t		*rpt;
	int		n, i;

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		ret = saHpiRptEntryGet(Domain->sessionId, rptentryid, &nextrptentryid,
			&rptentry);
		if (ret != SA_OK)
			return(-1);
		i = find_rpt(Domain, rptentry.ResourceId);
		if (i != -1) {
			rpt = Domain->rpts + i;
			memcpy(rpt->Rpt, &rptentry, sizeof(SaHpiRptEntryT));
		} else {
			n = Domain->n_rpts;
			Domain->rpts = (Rpt_t *)resize_array(Domain->rpts, sizeof(Rpt_t),
				&(Domain->n_rpts), 1);
			rpt = Domain->rpts + n;
			rpt->Rpt = (SaHpiRptEntryT *)malloc(sizeof(SaHpiRptEntryT));
			memcpy(rpt->Rpt, &rptentry, sizeof(SaHpiRptEntryT));
		};
		rpt->EntryId = rptentry.EntryId;
		rpt->ResourceId = rptentry.ResourceId;
		rpt->is_inited = 1;
		make_rpt_attrs(rpt);
		ret = init_rpt(Domain, rpt);
		rptentryid = nextrptentryid;
	};
	for (i = Domain->n_rpts - 1; i >= 0; i--) {
		if (Domain->rpts[i].is_inited == 0)
			delete_rpt(Domain, i);
	};
	return(SA_OK);
}

Domain_t *init_resources(SaHpiSessionIdT session)
{
	SaErrorT	ret;
	Domain_t	*domain;

	domain = (Domain_t *)malloc(sizeof(Domain_t));
	memset(domain, 0, sizeof(Domain_t));
	domain->sessionId = session;
	ret = get_resources(domain);
	if (ret != SA_OK)
		return((Domain_t *)NULL);
	return(domain);
}

SaErrorT check_resources(Domain_t *Domain)
{
	SaErrorT	ret;

	clear_inited(Domain);
	ret = get_resources(Domain);
	return(ret);
}

Rpt_t *get_rpt(Domain_t *domain, SaHpiResourceIdT rptId)
{
	int		i;
	SaErrorT	ret;
	SaHpiRptEntryT	rpt_entry;
	Rpt_t		*rpt;

	if (domain == (Domain_t *)NULL)
		return((Rpt_t *)NULL);
	ret = saHpiRptEntryGetByResourceId(domain->sessionId, rptId, &rpt_entry);
	i = find_rpt(domain, rptId);
	if ((i < 0) || (ret != SA_OK)) {
		if (check_resources(domain) != SA_OK)
			return((Rpt_t *)NULL);
		i = find_rpt(domain, rptId);
		if (i < 0)
			return((Rpt_t *)NULL);
		rpt = domain->rpts + i;
	} else {
		rpt = domain->rpts + i;
		*(rpt->Rpt) = rpt_entry;
		make_rpt_attrs(rpt);
	};
	return(rpt);
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

Rdr_t *get_rdr(Domain_t *Domain, SaHpiResourceIdT rptId, SaHpiSensorNumT num)
{
	int	i;
	Rpt_t	*rpt;

	if (Domain == (Domain_t *)NULL)
		return((Rdr_t *)NULL);
	i = find_rpt(Domain, rptId);
	if (i < 0)
		return((Rdr_t *)NULL);
	rpt = Domain->rpts + i;
	if (num >= rpt->n_rdrs)
		return((Rdr_t *)NULL);
	return(rpt->rdrs + num);
}
