/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Racing Guo <racing.guo@intel.com>
 */

#include "ipmi.h"
#include <oh_utils.h>
#include <string.h>

#define OHOI_IDR_DEFAULT_ID         0
#define OHOI_CHECK_RPT_CAP_IDR()                                     \
do{                                                                  \
	SaHpiRptEntryT           *rpt_entry;                         \
	rpt_entry = oh_get_resource_by_id(handler->rptcache, rid);   \
	if (!rpt_entry) {                                            \
		dbg("Resource %d No rptentry", rid);                 \
		return SA_ERR_HPI_INVALID_PARAMS;                    \
	}                                                            \
	if (!(rpt_entry->ResourceCapabilities &                      \
	    SAHPI_CAPABILITY_INVENTORY_DATA)) {                      \
		dbg("Resource %d no inventory capability", rid);     \
		return SA_ERR_HPI_INVALID_PARAMS;                    \
	}                                                            \
	if (idrid != OHOI_IDR_DEFAULT_ID) {                          \
		dbg("error id");                                     \
		return SA_ERR_HPI_NOT_PRESENT;                    \
	}                                                            \
}while(0)

struct ohoi_field_data {
	SaHpiIdrFieldTypeT fieldtype;
	int (*get_len)(ipmi_entity_t *, unsigned int*);
	int (*get_data)(ipmi_entity_t *, char*, unsigned int*);
	SaHpiIdrFieldT  *field;
};

static struct ohoi_field_data chassis_fields[] = {
	{
		SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER, 
		ipmi_entity_get_chassis_info_serial_number_len,
		ipmi_entity_get_chassis_info_serial_number,
	},
	{
		SAHPI_IDR_FIELDTYPE_PART_NUMBER,
		ipmi_entity_get_chassis_info_part_number_len,
		ipmi_entity_get_chassis_info_part_number
	,},
};

static struct ohoi_field_data board_fields[] = {
	{
		SAHPI_IDR_FIELDTYPE_MANUFACTURER,
		ipmi_entity_get_board_info_board_manufacturer_len, 
		ipmi_entity_get_board_info_board_manufacturer,
	},
	{
		SAHPI_IDR_FIELDTYPE_PRODUCT_NAME, 
		ipmi_entity_get_board_info_board_product_name_len, 
		ipmi_entity_get_board_info_board_product_name
	},
	{
		SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER, 
		ipmi_entity_get_board_info_board_serial_number_len, 
		ipmi_entity_get_board_info_board_serial_number
	},
	{
		SAHPI_IDR_FIELDTYPE_PART_NUMBER, 
		ipmi_entity_get_board_info_board_part_number_len, 
		ipmi_entity_get_board_info_board_part_number
	},
	{
		SAHPI_IDR_FIELDTYPE_FILE_ID, 
		ipmi_entity_get_board_info_fru_file_id_len, 
		ipmi_entity_get_board_info_fru_file_id
	},
};

static struct ohoi_field_data product_fields[] = {
	{
		SAHPI_IDR_FIELDTYPE_MANUFACTURER, 
		ipmi_entity_get_product_info_manufacturer_name_len, 
		ipmi_entity_get_product_info_manufacturer_name
	},
	{
		SAHPI_IDR_FIELDTYPE_PRODUCT_NAME,
		ipmi_entity_get_product_info_product_name_len,
		ipmi_entity_get_product_info_product_name,
	},
	{
		SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER,
		ipmi_entity_get_product_info_product_serial_number_len,
		ipmi_entity_get_product_info_product_serial_number,
	},
	{
		SAHPI_IDR_FIELDTYPE_FILE_ID,
		ipmi_entity_get_product_info_fru_file_id_len,
		ipmi_entity_get_product_info_fru_file_id
	},
	{
		SAHPI_IDR_FIELDTYPE_ASSET_TAG,
		ipmi_entity_get_product_info_asset_tag_len,
		ipmi_entity_get_product_info_asset_tag,
	}
};

static int _ipmi_entity_get_internal_use_data(ipmi_entity_t *entity,
					      char *data,
					      unsigned int  *max_len)
{
	return ipmi_entity_get_internal_use_data(entity, data, max_len);
}

static struct ohoi_field_data internal_fields[] = {
	{
		SAHPI_IDR_FIELDTYPE_CUSTOM,
		ipmi_entity_get_internal_use_length,
		_ipmi_entity_get_internal_use_data,
	}
};

static struct ohoi_area_data {
	int field_num;
	SaHpiIdrAreaTypeT areatype;
	struct ohoi_field_data *fields;
} areas [] = {
	{	sizeof(chassis_fields)/sizeof(chassis_fields[0]),
		SAHPI_IDR_AREATYPE_CHASSIS_INFO,
		chassis_fields
	}, 
	{
		sizeof(board_fields)/sizeof(board_fields[0]),
		SAHPI_IDR_AREATYPE_BOARD_INFO,
		board_fields
	},
	{
		sizeof(product_fields)/sizeof(product_fields[0]),
		SAHPI_IDR_AREATYPE_PRODUCT_INFO,
		product_fields
	},
	{
		sizeof(internal_fields)/sizeof(internal_fields[0]),
		SAHPI_IDR_AREATYPE_INTERNAL_USE,
		internal_fields
	}
};

#define OHOI_AREA_NUM        (sizeof(areas)/sizeof(areas[0]))
#define OHOI_AREA_EMPTY_ID   0
#define OHOI_AREA_FIRST_ID   1
#define OHOI_AREA_LAST_ID    OHOI_AREA_NUM

#define OHOI_FIELD_NUM(area)         (area->field_num)
#define OHOI_FIELD_EMPTY_ID          0
#define OHOI_FIELD_FIRST_ID          1
#define OHOI_FIELD_LAST_ID(area)     (area->field_num)

static int get_first_area(SaHpiIdrAreaTypeT areatype)
{
	int i;
	for (i = 0; i < OHOI_AREA_NUM; i++)
		if (areas[i].areatype == areatype)
			return (i + 1);
	return OHOI_AREA_EMPTY_ID;
}

static int get_first_field(struct ohoi_area_data *area,
			   SaHpiIdrFieldTypeT fieldtype)
{
	int i;
	for (i = 0; i < OHOI_FIELD_NUM(area); i++)
		if (area->fields[i].fieldtype == fieldtype)
			return (i + 1);
	return OHOI_FIELD_EMPTY_ID;
}

static void get_field(ipmi_entity_t *ent,
		      void          *cb_data)
{
	int rv;
	int len;
	int (*get_len)(ipmi_entity_t *, unsigned int*);
	int (*get_data)(ipmi_entity_t *, char*, unsigned int*);
	struct ohoi_field_data *data = cb_data;	
	
	SaHpiIdrFieldT *field = data->field;
	get_len = data->get_len;
	get_data = data->get_data;
	
	field->Type = data->fieldtype;
	field->ReadOnly = SAHPI_TRUE;

	field->Field.DataType = SAHPI_TL_TYPE_ASCII6;
	field->Field.Language = SAHPI_LANG_ENGLISH;
	field->Field.DataLength = 0;

	rv = get_len(ent, &len);
	if (rv) {
		dbg("Error on get_len: %d", rv);
		return;
	}

	if (len > SAHPI_MAX_TEXT_BUFFER_LENGTH)
		len = SAHPI_MAX_TEXT_BUFFER_LENGTH;

	rv = get_data(ent, &field->Field.Data[0], &len);
	if (!rv)
		field->Field.DataLength = len;
	else 
		dbg("Error on  get_data: %d", rv);
}

SaErrorT ohoi_get_idr_info(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                           SaHpiIdrInfoT *idrinfo)
{
	struct oh_handler_state  *handler = hnd;

	OHOI_CHECK_RPT_CAP_IDR();

	idrinfo->IdrId = OHOI_IDR_DEFAULT_ID;
	idrinfo->UpdateCount = 0;
	idrinfo->ReadOnly = SAHPI_TRUE;

	idrinfo->NumAreas = OHOI_AREA_NUM;

	return SA_OK;
}

SaErrorT ohoi_get_idr_area_header(void *hnd, SaHpiResourceIdT rid,
				  SaHpiIdrIdT idrid,
                                  SaHpiIdrAreaTypeT areatype,
				  SaHpiEntryIdT areaid,
                                  SaHpiEntryIdT *nextareaid,
				  SaHpiIdrAreaHeaderT *header)
{
	struct oh_handler_state  *handler = hnd;
	struct ohoi_area_data *area;

	
	OHOI_CHECK_RPT_CAP_IDR();

	if (areaid > OHOI_AREA_LAST_ID)
		return SA_ERR_HPI_NOT_PRESENT;

	if ((areatype == SAHPI_IDR_AREATYPE_UNSPECIFIED) &&
	    (areaid == SAHPI_FIRST_ENTRY)) {

		areaid = OHOI_AREA_FIRST_ID;

	}else if ((areatype != SAHPI_IDR_AREATYPE_UNSPECIFIED) &&
	    (areaid == SAHPI_FIRST_ENTRY)) {

		areaid = get_first_area(areatype);
		if (areaid == OHOI_AREA_EMPTY_ID)
			return SA_ERR_HPI_NOT_PRESENT;

	}else if ((areatype == SAHPI_IDR_AREATYPE_UNSPECIFIED) &&
	    (areaid != SAHPI_FIRST_ENTRY)) {

		if (areaid > OHOI_AREA_LAST_ID)
			return SA_ERR_HPI_NOT_PRESENT;

	}else if ((areatype != SAHPI_IDR_AREATYPE_UNSPECIFIED) && 
	    (areaid != SAHPI_FIRST_ENTRY)) {

		if (areaid > OHOI_AREA_LAST_ID)
			return SA_ERR_HPI_NOT_PRESENT;
		if (areas[areaid - 1].areatype != areatype)
			return SA_ERR_HPI_INVALID_PARAMS;
	}

	if (areaid < OHOI_AREA_LAST_ID) {
		*nextareaid = areaid + 1;
	}else if (areaid == OHOI_AREA_LAST_ID) {
		*nextareaid = SAHPI_LAST_ENTRY;
	}

	area = &areas[areaid -1];

	header->AreaId = areaid;
	header->Type = area->areatype;

	header->ReadOnly = SAHPI_TRUE;
	header->NumFields = area->field_num;

	return SA_OK;
}

SaErrorT ohoi_add_idr_area(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                           SaHpiIdrAreaTypeT areatype, SaHpiEntryIdT *areaid)
{
	return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT ohoi_del_idr_area(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                           SaHpiEntryIdT areaid)
{
	return SA_ERR_HPI_INVALID_REQUEST;
}
SaErrorT ohoi_get_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
		            SaHpiEntryIdT areaid, SaHpiIdrFieldTypeT fieldtype,
                            SaHpiEntryIdT fieldid, SaHpiEntryIdT *nextfieldid,
                            SaHpiIdrFieldT *field)
{
	struct oh_handler_state  *handler = hnd;
	struct ohoi_resource_info   *ohoi_res_info;
	ipmi_entity_id_t         ent_id;
	struct ohoi_field_data   *field_data;
	struct ohoi_area_data  *area_data;

	OHOI_CHECK_RPT_CAP_IDR();

	ohoi_res_info = oh_get_resource_data(handler->rptcache, rid);
	if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
		dbg("Bug: try to get fru in unsupported resource");
		return SA_ERR_HPI_INVALID_CMD;
	}
	ent_id = ohoi_res_info->u.entity_id;
	
	if (areaid == SAHPI_FIRST_ENTRY)
		areaid = OHOI_AREA_FIRST_ID;

	if (areaid > OHOI_AREA_LAST_ID)
		return SA_ERR_HPI_NOT_PRESENT;


	area_data = &areas[areaid - 1];

	if (fieldid > OHOI_FIELD_LAST_ID(area_data))
		return SA_ERR_HPI_NOT_PRESENT;

	if ((fieldtype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) &&
	    (fieldid == SAHPI_FIRST_ENTRY)) {

		fieldid = OHOI_FIELD_FIRST_ID;
		
	}else if ((fieldtype != SAHPI_IDR_FIELDTYPE_UNSPECIFIED) &&
	    (fieldid == SAHPI_FIRST_ENTRY)) {

		fieldid = get_first_field(area_data, fieldtype);
		if (fieldid == OHOI_FIELD_EMPTY_ID)
			return SA_ERR_HPI_NOT_PRESENT;

	}else if ((fieldtype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) &&
	    (fieldid != SAHPI_FIRST_ENTRY)) {

		if (fieldid > OHOI_FIELD_LAST_ID(area_data))
			return SA_ERR_HPI_NOT_PRESENT;

	}else if ((fieldtype != SAHPI_IDR_FIELDTYPE_UNSPECIFIED) && 
	    (fieldid != SAHPI_FIRST_ENTRY)) {

		if (fieldid > OHOI_FIELD_LAST_ID(area_data))
			return SA_ERR_HPI_NOT_PRESENT;
		if (area_data->fields[fieldid - 1].fieldtype != fieldtype)
			return SA_ERR_HPI_INVALID_PARAMS;
	}

	field_data = &area_data->fields[fieldid - 1];

	if (fieldid < OHOI_FIELD_LAST_ID(area_data)) {
		*nextfieldid = fieldid + 1;
	}else if (fieldid == OHOI_FIELD_LAST_ID(area_data)) {
		*nextfieldid = SAHPI_LAST_ENTRY;
	}

	field->FieldId = fieldid;
	field->Type = field_data->fieldtype;

	field_data->field = field;
	return ipmi_entity_pointer_cb(ent_id, get_field, field_data);
}

SaErrorT ohoi_add_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                            SaHpiIdrFieldT *field )
{
	return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT ohoi_set_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                            SaHpiIdrFieldT *field )
{
	return SA_ERR_HPI_INVALID_REQUEST;
}

SaErrorT ohoi_del_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                            SaHpiEntryIdT areaid, SaHpiEntryIdT fieldid)
{
	return SA_ERR_HPI_INVALID_REQUEST;
}
