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

#define RPT_ATTRS_NUM	11

attr_t	Def_rpt[] = {
	{ "EntryId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  0
	{ "ResourceId",		INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  1
	{ "ResourceInfo",	STRUCT_TYPE,	0, { .d = 0}, { .d = 0} },	//  2
	{ "ResCapabilities",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  3
	{ "HotSwapCapabilities",INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  4
	{ "ResourceSeverity",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  5
	{ "ResourceFailed",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  6
	{ "ResourceTag",	STR_TYPE,	0, { .d = 0}, { .d = 0} },	//  7
	{ "ResourceTagLength",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  8
	{ "ResourceTagType",	INT_TYPE,	0, { .d = 0}, { .d = 0} },	//  9
	{ "ResourceTagLang",	INT_TYPE,	0, { .d = 0}, { .d = 0} }	// 10
//	{ "ResourceEntity",	INT_TYPE,	0, { .d = 0}, { .d = 0} },
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

static void make_rpt_attrs(Rpt_t *Rpt, SaHpiRptEntryT *Obj)
{
	attr_t		*A, *A1;
	int		len, i = 0;
	SaHpiUint8T	*S;
	
	Rpt->Attrutes.n_attrs = RPT_ATTRS_NUM;
	Rpt->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RPT_ATTRS_NUM);
	A = Rpt->Attrutes.Attrs;
	memcpy(A, Def_rpt, sizeof(attr_t) * RPT_ATTRS_NUM);
	A[i++].value.i = Obj->EntryId;
	A[i++].value.i = Obj->ResourceId;
	A1 = (attr_t *)malloc(sizeof(attr_t) * RESINFO_ATTRS_NUM);
	memcpy(A1, Def_resinfo, sizeof(attr_t) * RESINFO_ATTRS_NUM);
	A[i++].value.a = A1;
	
	A1[0].value.i = Obj->ResourceInfo.ResourceRev;
	A1[1].value.i = Obj->ResourceInfo.SpecificVer;
	A1[2].value.i = Obj->ResourceInfo.DeviceSupport;
	A1[3].value.i = Obj->ResourceInfo.ManufacturerId;
	A1[4].value.i = Obj->ResourceInfo.ProductId;
	A1[5].value.i = Obj->ResourceInfo.FirmwareMajorRev;
	A1[6].value.i = Obj->ResourceInfo.FirmwareMinorRev;
	A1[7].value.i = Obj->ResourceInfo.AuxFirmwareRev;
	S = (SaHpiUint8T *)malloc(16);
	memcpy(S, Obj->ResourceInfo.Guid, 16);
	A1[8].value.s = S;
	
	A[i++].value.i = Obj->ResourceCapabilities;
	A[i++].value.i = Obj->HotSwapCapabilities;
	A[i++].value.i = Obj->ResourceSeverity;
	A[i++].value.i = Obj->ResourceFailed;
	len = Obj->ResourceTag.DataLength;
	if (len > 0) {
		S = (SaHpiUint8T *)malloc(len + 1);
		memset(S, 0, len + 1);
		memcpy(S, Obj->ResourceTag.Data, len);
		A[i++].value.s = S;
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

static void make_rdr_attrs(Rdr_t *Rdr, SaHpiRdrT *Obj)
{
	attr_t		*A, *A1;
	int		len, i = 0;
	SaHpiUint8T	*S;
	SaHpiSensorRecT	*Sens;
	
	Rdr->Attrutes.n_attrs = RDR_ATTRS_COMMON_NUM;
	Rdr->Attrutes.Attrs = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	A = Rdr->Attrutes.Attrs;
	memcpy(A, Def_common_rdr, sizeof(attr_t) * RDR_ATTRS_COMMON_NUM);
	A[i++].value.i = Obj->RecordId;
	A[i++].value.i = Obj->RdrType;
	A[i++].value.i = Obj->IsFru;
	len = Obj->IdString.DataLength;
	switch (Obj->RdrType) {
		case SAHPI_SENSOR_RDR:
			A1 = (attr_t *)malloc(sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
			memcpy(A1, Def_sensor_rdr, sizeof(attr_t) * RDR_ATTRS_SENSOR_NUM);
			Sens = &(Obj->RdrTypeUnion.SensorRec);
			A1[0].value.i = Sens->Num;
			A1[1].value.i = Sens->Type;
			A1[2].value.i = Sens->Category;
			A1[3].value.i = Sens->EnableCtrl;
			A1[4].value.i = Sens->EventCtrl;
			A1[5].value.i = Sens->Events;
			A1[6].value.i = Sens->Oem;
			A[i++].value.a = A1;
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

static SaErrorT init_rpt(Domain_t *Domain, Rpt_t *Rpt)
{
	SaHpiRdrT		rdr;
	Rdr_t			*Rdr;
	SaErrorT		ret, n;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		ret = saHpiRdrGet(Domain->sessionId, Rpt->ResourceId, entryid,
			&nextentryid, &rdr);
		if (ret != SA_OK)
			return(ret);
		n = Rpt->n_rdrs;
		Rpt->rdrs = (Rdr_t *)resize_array(Rpt->rdrs, sizeof(Rdr_t),
			&(Rpt->n_rdrs), 1);
		Rdr = Rpt->rdrs + n;
		Rdr->RecordId = rdr.RecordId;
		Rdr->RdrType = rdr.RdrType;
		make_rdr_attrs(Rdr, &rdr);
		Rpt->rdrs[n] = *Rdr;
		entryid = nextentryid;
	};
	return(SA_OK);
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
			snprintf(val, len, "%s", Attrs->Attrs[num].value.s);
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
			make_rdr_attrs(Rdr, &rdr);
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
			make_rpt_attrs(Rpt, &rptentry);
			return(SA_OK);
		};
		rptentryid = nextrptentryid;
	};
	//    FIX ME!    delete Rdr
	return(SA_ERR_HPI_ERROR);
}

Domain_t *init_resources(SaHpiSessionIdT session)
{
	SaHpiEntryIdT	rptentryid, nextrptentryid;
	SaErrorT	ret;
	Domain_t	*Domain;
	SaHpiRptEntryT	rptentry;
	Rpt_t		*Rpt;
	int		n;

	Domain = (Domain_t *)malloc(sizeof(Domain_t));
	memset(Domain, 0, sizeof(Domain_t));
	Domain->sessionId = session;
	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		ret = saHpiRptEntryGet(Domain->sessionId, rptentryid, &nextrptentryid,
			&rptentry);
		if (ret != SA_OK)
			return((Domain_t *)NULL);
		n = Domain->n_rpts;
		Domain->rpts = (Rpt_t *)resize_array(Domain->rpts, sizeof(Rpt_t),
			&(Domain->n_rpts), 1);
		Rpt = Domain->rpts + n;
		Rpt->EntryId = rptentry.EntryId;
		Rpt->ResourceId = rptentry.ResourceId;
		make_rpt_attrs(Rpt, &rptentry);
		ret = init_rpt(Domain, Rpt);
		rptentryid = nextrptentryid;
	}
	return(Domain);
}

SaErrorT check_resources(Domain_t *Domain)
{
	return(SA_OK);
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
	if ((num < 0) || (num >= Attrs->n_attrs) || (val == (union_type_t *)NULL))
		return SA_ERR_HPI_INVALID_PARAMS;
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
