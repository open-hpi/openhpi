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

attr_t	Def_rpt[] = {
	{ "EntryId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  0
	{ "ResourceId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  1
	{ "ResourceInfo",	STRUCT_TYPE,	0, { .d = 0}, { .d = 0} },	//  2
	{ "ResourceEntity",	ARRAY_TYPE,	0, { .d = 0}, { .d = 0} },	//  3
	{ "Capabilities",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  4
	{ "HotSwapCapabilities",INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  5
	{ "ResourceSeverity",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  6
	{ "ResourceFailed",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  7
	{ "Tag",		STR_TYPE,	0, { .d = 0}, { .d = 0} },	//  8
	{ "TagLength",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  9
	{ "TagType",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	// 10
	{ "TagLanguage",	INT_TYPE,	0, { .d = 0}, { .d = 0} }	// 11
};

#define RESINFO_ATTRS_NUM	9

attr_t	Def_resinfo[] = {
	{ "ResourceRev",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  0
	{ "SpecificVer",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  1
	{ "DeviceSupport",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  2
	{ "ManufacturerId",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  3
	{ "ProductId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  4
	{ "FirmwareMajorRev",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  5
	{ "FirmwareMinorRev",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  6
	{ "AuxFirmwareRev",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  7
	{ "Guid",		STR_TYPE,	0, { .d = 0}, { .d = 0} }	//  8
};

static int find_attr(Attributes_t *Attrs, char *name)
{
	int	i;

	for (i = 0; i < Attrs->n_attrs; i++) {
		if (strcmp(Attrs->Attrs[i].name, name) == 0)
			return(i);
	};
	return(-1);
}

static void *resize_array(void *Ar, int item_size, int *last_num, int add_num)
/* Resize array Ar:
 *	item_size - item size (bytes)
 *	last_num  - current array size
 *	add_num   - new array size = last_num + add_num
 * Return:	new pointer
 */
{
	void	*R;
	int	new_num = *last_num + add_num;

	if (new_num <= 0)
		return((void *)NULL);
	R = malloc(item_size * new_num);
	memset(R, 0, item_size * new_num);
	if (*last_num > 0) {
		memcpy(R, Ar, *last_num * item_size);
		free(Ar);
	}
	*last_num = new_num;
	return(R);
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
	attr_t			*A, *A1;
	int			len, i = 0, newrpt = 1;
	Attributes_t		*At;
	SaHpiRptEntryT		*Obj;

	Obj = Rpt->Rpt;
	if (Rpt->Attrutes.n_attrs > 0)
		newrpt = 0;
	if (newrpt) {
		Rpt->Attrutes.n_attrs = RPT_ATTRS_NUM;
		Rpt->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RPT_ATTRS_NUM);
		memcpy(Rpt->Attrutes.Attrs, Def_rpt, sizeof(attr_t) * RPT_ATTRS_NUM);
	};
	A = Rpt->Attrutes.Attrs;
	A[i++].value.i = Obj->EntryId;
	A[i++].value.i = Obj->ResourceId;
	if (newrpt) {
		At = (Attributes_t *)malloc(sizeof(Attributes_t));
		At->n_attrs = RESINFO_ATTRS_NUM;
		A1 = (attr_t *)malloc(sizeof(attr_t) * RESINFO_ATTRS_NUM);
		memcpy(A1, Def_resinfo, sizeof(attr_t) * RESINFO_ATTRS_NUM);
		At->Attrs = A1;
		A[i++].value.a = At;
	} else {
		At = (Attributes_t *)(A[i++].value.a);
		A1 = At->Attrs;
	};
	A1[0].value.i = Obj->ResourceInfo.ResourceRev;
	A1[1].value.i = Obj->ResourceInfo.SpecificVer;
	A1[2].value.i = Obj->ResourceInfo.DeviceSupport;
	A1[3].value.i = Obj->ResourceInfo.ManufacturerId;
	A1[4].value.i = Obj->ResourceInfo.ProductId;
	A1[5].value.i = Obj->ResourceInfo.FirmwareMajorRev;
	A1[6].value.i = Obj->ResourceInfo.FirmwareMinorRev;
	A1[7].value.i = Obj->ResourceInfo.AuxFirmwareRev;
	A1[8].value.s = (char *)(Obj->ResourceInfo.Guid);
	A[i++].value.a = &(Obj->ResourceEntity);
	A[i++].value.i = Obj->ResourceCapabilities;
	A[i++].value.i = Obj->HotSwapCapabilities;
	A[i++].value.i = Obj->ResourceSeverity;
	A[i++].value.i = Obj->ResourceFailed;
	len = Obj->ResourceTag.DataLength;
	if (len > 0) {
		A[i++].value.s = Obj->ResourceTag.Data;
		Obj->ResourceTag.Data[len] = 0;
		A[i++].value.i = len;
		A[i++].value.i = Obj->ResourceTag.DataType;
		A[i++].value.i = Obj->ResourceTag.Language;
	}
}

#define RDR_ATTRS_COMMON_NUM	8

attr_t	Def_common_rdr[] = {
	{ "RecordId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  0
	{ "RdrType",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  1
	{ "IsFru",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  2
	{ "Record",		STRUCT_TYPE,	0, { .d = 0}, { .d = 0} },	//  3
	{ "IdString",		STR_TYPE,	0, { .d = 0}, { .d = 0} },	//  4
	{ "IdStringLength",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  5
	{ "IdStringType",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  6
	{ "IdStringLang",	INT_TYPE,	0, { .d = 0}, { .d = 0} }	//  7
//	{ "Entity",	INT_TYPE,	0, { .d = 0}, { .d = 0} },
};

#define RDR_ATTRS_SENSOR_NUM	9

attr_t	Def_sensor_rdr[] = {
	{ "Num",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  0
	{ "Type",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  1
	{ "Category",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  2
	{ "EnableCtrl",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  3
	{ "EventCtrl",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  4
	{ "Events",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  5
	{ "DataFormat",		STRUCT_TYPE,	0, { .d = 0}, { .d = 0} },	//  6
	{ "ThresholdDefn",	STRUCT_TYPE,	0, { .d = 0}, { .d = 0} },	//  7
	{ "Oem",		INT_TYPE,	0, { .d = 0}, { .d = 0} }	//  8
};

static void make_rdr_attrs(Rdr_t *Rdr)
{
	attr_t		*A, *A1;
	int		len, i = 0, newrdr = 1;
	Attributes_t	*At;
	SaHpiUint8T	*S;
	SaHpiRdrT	*Obj;
	SaHpiSensorRecT	*Sens;

	Obj = Rdr->Rdr;
	if (Rdr->Attrutes.n_attrs > 0)
		newrdr = 0;
	if (newrdr) {
		Rdr->Attrutes.n_attrs = RDR_ATTRS_COMMON_NUM;
		Rdr->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
		memcpy(Rdr->Attrutes.Attrs, Def_common_rdr, sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	};
	A = Rdr->Attrutes.Attrs;
	A[i++].value.i = Obj->RecordId;
	A[i++].value.i = Obj->RdrType;
	A[i++].value.i = Obj->IsFru;
	len = Obj->IdString.DataLength;
	switch (Obj->RdrType) {
		case SAHPI_SENSOR_RDR:
			if (newrdr) {
				At = (Attributes_t *)malloc(sizeof(Attributes_t));
				At->n_attrs = RDR_ATTRS_SENSOR_NUM;
				A1 = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
				memcpy(A1, Def_sensor_rdr, sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
				At->Attrs = A1;
				A[i].name = "Sensor";
				A[i++].value.a = At;
			} else {
				At = (Attributes_t *)(A[i++].value.a);
				A1 = At->Attrs;
			};
			Sens = &(Obj->RdrTypeUnion.SensorRec);
			A1[0].value.i = Sens->Num;
			A1[1].value.i = Sens->Type;
			A1[2].value.i = Sens->Category;
			A1[3].value.i = Sens->EnableCtrl;
			A1[4].value.i = Sens->EventCtrl;
			A1[5].value.i = Sens->Events;
			A1[8].value.i = Sens->Oem;
			break;
		default: break;
	};
	len = Obj->IdString.DataLength;
	if (len > 0) {
		S = (SaHpiUint8T *)malloc(len + 1);
		memset(S, 0, len + 1);
		memcpy(S, Obj->IdString.Data, len);
		A[i++].value.s = S;
		A[i++].value.i = len;
		A[i++].value.i = Obj->IdString.DataType;
		A[i++].value.i = Obj->IdString.Language;
	};
}

static int find_rpt(Domain_t *Domain, SaHpiResourceIdT rptId)
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
	attr_t		*A;

	for (i = 0, A = At->Attrs; i < At->n_attrs; i++, A++) {
		if (A->type == STRUCT_TYPE)
			free_attrs((Attributes_t *)(A->value.a));
			free(A->value.a);
	};
	free(At->Attrs);
}

static void delete_rdr(Rpt_t *Rpt, int ind)
{
	Rdr_t		*Rdr, *R;
	int		n, i;

	n = Rpt->n_rdrs;
	if ((ind < 0) || (ind >= n))
		return;
	Rdr = Rpt->rdrs + ind;
	free_attrs(&(Rdr->Attrutes));
	free(Rdr->Rdr);
	free(Rdr);
	if (n == 1) {
		Rpt->n_rdrs = 0;
		free(Rpt->rdrs);
		return;
	};
	R = Rdr = (Rdr_t *)malloc(sizeof(Rdr_t) * (n - 1));
	for (i = 0; i < n; i++)
		if (i != ind) *R++ = Rpt->rdrs[i];
	free(Rpt->rdrs);
	Rpt->rdrs = Rdr;
}

static void delete_rpt(Domain_t *Domain, int ind)
{
	Rpt_t		*Rpt, *R;
	int		n, i;

	n = Domain->n_rpts;
	if ((ind < 0) || (ind >= n))
		return;
	Rpt = Domain->rpts + ind;
	free_attrs(&(Rpt->Attrutes));
	free(Rpt->Rpt);
	free(Rpt);
	if (n == 1) {
		Domain->n_rpts = 0;
		free(Domain->rpts);
		return;
	};
	R = Rpt = (Rpt_t *)malloc(sizeof(Rpt_t) * (n - 1));
	for (i = 0; i < n; i++)
		if (i != ind) *R++ = Domain->rpts[i];
	free(Domain->rpts);
	Domain->rpts = Rpt;
}

static SaErrorT init_rpt(Domain_t *Domain, Rpt_t *Rpt)
{
	SaHpiRdrT		rdr;
	Rdr_t			*Rdr;
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
			Rdr = Rpt->rdrs + i;
			*(Rdr->Rdr) = rdr;
		} else {
			n = Rpt->n_rdrs;
			Rpt->rdrs = (Rdr_t *)resize_array(Rpt->rdrs, sizeof(Rdr_t),
				&(Rpt->n_rdrs), 1);
			Rdr = Rpt->rdrs + n;
			Rdr->Rdr = (SaHpiRdrT *)malloc(sizeof(SaHpiRdrT));
			*(Rdr->Rdr) = rdr;
			Rpt->rdrs[n] = *Rdr;
		};
		Rdr->RecordId = rdr.RecordId;
		Rdr->RdrType = rdr.RdrType;
		Rdr->is_inited = 1;
		make_rdr_attrs(Rdr);
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

SaErrorT check_rdr(Domain_t *Domain, SaHpiResourceIdT rptId, SaHpiEntryIdT rdrId)
{
	SaHpiRdrT		rdr;
	Rdr_t			*Rdr;
	Rpt_t			*Rpt;
	SaErrorT		ret;
	int			rpt_ind, rdr_ind;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;

	if (Domain == (Domain_t *)NULL)
		return(SA_ERR_HPI_INVALID_PARAMS);
	rpt_ind = find_rpt(Domain, rptId);
	if (rpt_ind < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	Rpt = Domain->rpts + rpt_ind;
	rdr_ind = find_rdr(Rpt, rdrId);
	if (rdr_ind < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	Rdr = Rpt->rdrs + rdr_ind;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		ret = saHpiRdrGet(Domain->sessionId, rptId, entryid, &nextentryid, &rdr);
		if (ret != SA_OK)
			return(ret);
		if (rdr.RecordId == rdrId) {
			make_rdr_attrs(Rdr);
			return(SA_OK);
		};
		entryid = nextentryid;
	};
	//    FIX ME!    delete Rdr
	return(SA_ERR_HPI_ERROR);
}

SaErrorT check_rpt(Domain_t *Domain, SaHpiResourceIdT rptId)
{
	SaHpiEntryIdT	rptentryid, nextrptentryid;
	SaErrorT	ret;
	Rpt_t		*Rpt;
	SaHpiRptEntryT	rptentry;
	int		rpt_ind;

	if (Domain == (Domain_t *)NULL)
		return(SA_ERR_HPI_INVALID_PARAMS);
	rpt_ind = find_rpt(Domain, rptId);
	if (rpt_ind < 0)
		return(SA_ERR_HPI_INVALID_PARAMS);
	Rpt = Domain->rpts + rpt_ind;
	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		ret = saHpiRptEntryGet(Domain->sessionId, rptentryid, &nextrptentryid,
			&rptentry);
		if (ret != SA_OK)
			return(ret);
		if (rptentry.ResourceId == rptId) {
			*(Rpt->Rpt) = rptentry;
			make_rpt_attrs(Rpt);
			return(SA_OK);
		};
		rptentryid = nextrptentryid;
	};
	//    FIX ME!    delete Rdr
	return(SA_ERR_HPI_ERROR);
}

static SaErrorT get_resources(Domain_t *Domain)
{
	SaHpiEntryIdT	rptentryid, nextrptentryid;
	SaErrorT	ret;
	SaHpiRptEntryT	rptentry;
	Rpt_t		*Rpt;
	int		n, i, found;

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		found = 0;
		ret = saHpiRptEntryGet(Domain->sessionId, rptentryid, &nextrptentryid,
			&rptentry);
		if (ret != SA_OK)
			return(-1);
		i = find_rpt(Domain, rptentry.ResourceId);
		if (i != -1) {
			Rpt = Domain->rpts + i;
			memcpy(Rpt->Rpt, &rptentry, sizeof(SaHpiRptEntryT));
			found = 1;
		};
		if (found == 0) {
			n = Domain->n_rpts;
			Domain->rpts = (Rpt_t *)resize_array(Domain->rpts, sizeof(Rpt_t),
				&(Domain->n_rpts), 1);
			Rpt = Domain->rpts + n;
			Rpt->Rpt = (SaHpiRptEntryT *)malloc(sizeof(SaHpiRptEntryT));
			memcpy(Rpt->Rpt, &rptentry, sizeof(SaHpiRptEntryT));
		};
		Rpt->EntryId = rptentry.EntryId;
		Rpt->ResourceId = rptentry.ResourceId;
		Rpt->is_inited = 1;
		make_rpt_attrs(Rpt);
		ret = init_rpt(Domain, Rpt);
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
	Domain_t	*Domain;

	Domain = (Domain_t *)malloc(sizeof(Domain_t));
	memset(Domain, 0, sizeof(Domain_t));
	Domain->sessionId = session;
	ret = get_resources(Domain);
	if (ret != SA_OK)
		return((Domain_t *)NULL);
	return(Domain);
}

SaErrorT check_resources(Domain_t *Domain)
{
	SaErrorT	ret;

	clear_inited(Domain);
	ret = get_resources(Domain);
	return(ret);
}

SaErrorT set_rpt_attr(Rpt_t *rpt, char *attr_name, union_type_t val)
{
	return(SA_OK);
}

SaErrorT set_rdr_attr(Rdr_t *rdr, char *attr_name, union_type_t val)
{
	return(SA_OK);
}

Rpt_t *get_rpt(Domain_t *Domain, SaHpiResourceIdT rptId)
{
	int	i;

	if (Domain == (Domain_t *)NULL)
		return((Rpt_t *)NULL);
	i = find_rpt(Domain, rptId);
	if (i < 0)
		return((Rpt_t *)NULL);
	return(Domain->rpts + i);
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
	Rpt_t	*Rpt;

	if (Domain == (Domain_t *)NULL)
		return((Rdr_t *)NULL);
	i = find_rpt(Domain, rptId);
	if (i < 0)
		return((Rdr_t *)NULL);
	Rpt = Domain->rpts + i;
	if (num >= Rpt->n_rdrs)
		return((Rdr_t *)NULL);
	return(Rpt->rdrs + num);
}
