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

#define OHOI_IDR_DEFAULT_ID            0

#define OHOI_AREA_INTERNAL_USE_ID      0
#define OHOI_AREA_CHASSIS_INFO_ID      1
#define OHOI_AREA_BOARD_INFO_ID        2
#define OHOI_AREA_PRODUCT_INFO_ID      3
#define OHOI_AREA_MAX_ID               3


#define OHOI_CHECK_RPT_ENTRY()                               \
do{                                                          \
rpt_entry = oh_get_resource_by_id(handler->rptcache, rid);   \
	if (!rpt_entry) {                                    \
		dbg("No rptentry");                          \
		return SA_ERR_HPI_INVALID_PARAMS;            \
	}                                                    \
}while(0)

#define OHOI_CHECK_CAPABILITY()                              \
do{                                                          \
	if (rpt_entry->ResourceCapabilities &                \
	    SAHPI_CAPABILITY_INVENTORY_DATA) {               \
		dbg("no inventory capability");              \
		return SA_ERR_HPI_INVALID_PARAMS;            \
	}                                                    \
}while(0)

#define OHOI_CHECK_IRD_ID()                                  \
do{                                                          \
	if (idrid != OHOI_IDR_DEFAULT_ID) {                  \
		dbg("error id");                             \
		return SA_ERR_HPI_INVALID_PARAMS;            \
	}                                                    \
}while(0)

#define OHOI_CHECK_AREA_TYPE_ID()                                           \
do {                                                                        \
	if (areatype == SAHPI_IDR_AREATYPE_UNSPECIFIED) {                   \
		if (areaid == SAHPI_FIRST_ENTRY)                            \
			areaid = OHOI_AREA_INTERNAL_USE_ID;                 \
                                                                            \
		switch (areaid) {                                           \
			case OHOI_AREA_INTERNAL_USE_ID:                     \
				areatype = SAHPI_IDR_AREATYPE_INTERNAL_USE; \
				break;                                      \
			case OHOI_AREA_CHASSIS_INFO_ID:                     \
				areatype = SAHPI_IDR_AREATYPE_CHASSIS_INFO; \
				break;                                      \
			case OHOI_AREA_BOARD_INFO_ID:                       \
				areatype = SAHPI_IDR_AREATYPE_BOARD_INFO;   \
				break;                                      \
			case OHOI_AREA_PRODUCT_INFO_ID:                     \
				areatype = SAHPI_IDR_AREATYPE_PRODUCT_INFO; \
				break;                                      \
			default:                                            \
				return SA_ERR_HPI_INVALID_PARAMS;           \
		}                                                           \
	}                                                                   \
	switch (areatype) {                                                 \
		case SAHPI_IDR_AREATYPE_INTERNAL_USE:                       \
			fnum = 1;                                           \
			if (areaid == SAHPI_FIRST_ENTRY)                    \
				areaid = OHOI_AREA_INTERNAL_USE_ID;         \
			if (areaid != OHOI_AREA_INTERNAL_USE_ID)            \
				return SA_ERR_HPI_INVALID_PARAMS;           \
		case SAHPI_IDR_AREATYPE_CHASSIS_INFO:                       \
			fnum = 2;                                           \
			if (areaid == SAHPI_FIRST_ENTRY)                    \
				areaid = OHOI_AREA_CHASSIS_INFO_ID;         \
			if (areaid != OHOI_AREA_CHASSIS_INFO_ID)            \
				return SA_ERR_HPI_INVALID_PARAMS;           \
		case SAHPI_IDR_AREATYPE_BOARD_INFO:                         \
			fnum = 5;                                           \
			if (areaid == SAHPI_FIRST_ENTRY)                    \
				areaid = OHOI_AREA_BOARD_INFO_ID;           \
			if (areaid != OHOI_AREA_BOARD_INFO_ID)              \
				return SA_ERR_HPI_INVALID_PARAMS;           \
		case SAHPI_IDR_AREATYPE_PRODUCT_INFO:                       \
			fnum = 7;                                           \
			if (areaid == SAHPI_FIRST_ENTRY)                    \
				areaid = OHOI_AREA_PRODUCT_INFO_ID;         \
			if (areaid != OHOI_AREA_PRODUCT_INFO_ID);           \
				return SA_ERR_HPI_INVALID_PARAMS;           \
			break;                                              \
		default:                                                    \
			return SA_ERR_HPI_INVALID_PARAMS;                   \
	}                                                                   \
}while(0)


#define OHOI_CHECK_FIELDID(num)                  \
do {                                             \
	if (fieldid < (num-1))                   \
		*nextfieldid = fieldid + 1;      \
	else if(fieldid == (num-1))              \
		*nextfieldid = SAHPI_LAST_ENTRY; \
	else  return SA_ERR_HPI_INVALID_PARAMS;  \
} while(0)


static void _get_inventroy_internal_use(ipmi_entity_t *ent,
		                        void          *cb_data)
{
	SaHpiIdrFieldT *field;
	int len;
	int rv;
	
	field = cb_data;
	
	field->AreaId = 0;
	field->FieldId = 0; 
	field->Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
	field->ReadOnly = SAHPI_TRUE;

	field->Field.DataType = SAHPI_TL_TYPE_ASCII6;
	field->Field.Language = SAHPI_LANG_ENGLISH;
	field->Field.DataLength = 0;

	rv = ipmi_entity_get_internal_use_length(ent, &len);
	if (rv) {
		dbg("Error on ipmi_entity_get_internal_use_length: %d", rv);
		return;
	}

	if (len > SAHPI_MAX_TEXT_BUFFER_LENGTH)
		len = SAHPI_MAX_TEXT_BUFFER_LENGTH;

	rv = ipmi_entity_get_internal_use_data(
		        ent,
		        &field->Field.Data[0],
		        &len);
	if (!rv)
		field->Field.DataLength = len;
	else 
		dbg("Error on  ipmi_entity_get_internal_use_data: %d", rv);

}

static SaErrorT get_inventroy_internal_use(ipmi_entity_id_t       ent_id,
		                           SaHpiIdrFieldT *field)
{
	int rv;

	rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_internal_use, field);
	if (rv) {
		dbg("Error on ipmi_entity_pointer_cb: %d", rv);
		return SA_ERR_HPI_NOT_PRESENT;
	}

	return SA_OK;
}


static void _get_inventroy_chassis_info(ipmi_entity_t *ent,
		                        void          *cb_data)
{
	return;	
}

static SaErrorT get_inventory_chassis_info(ipmi_entity_id_t       ent_id)
{
	int rv;

	rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_chassis_info, NULL);
	if (rv) {
		dbg("Error on ipmi_entity_pointer_cb: %d", rv);
		return SA_ERR_HPI_NOT_PRESENT;
	}

	return SA_OK;
}

static void _get_inventroy_board_info(ipmi_entity_t *ent,
		                      void          *cb_data)
{
	return;
}

static SaErrorT get_inventory_board_info(ipmi_entity_id_t       ent_id)
{
	int rv;

	rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_board_info, NULL);
	if (rv) {
		dbg("Error on ipmi_entity_pointer_cb: %d", rv);
		return SA_ERR_HPI_NOT_PRESENT;
	}

	return SA_OK;
}

static void _get_inventroy_product_info(ipmi_entity_t *ent,
		                        void          *cb_data)
{
	return;
}

static SaErrorT get_inventory_product_info(ipmi_entity_id_t ent_id)
{
	int rv;

	rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_product_info, NULL);
	if (rv) {
		dbg("Error on ipmi_entity_pointer_cb: %d", rv);
		return SA_ERR_HPI_NOT_PRESENT;
	}

	return SA_OK;
}

SaErrorT ohoi_get_idr_info(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                           SaHpiIdrInfoT *idrinfo)
{
	struct oh_handler_state  *handler = hnd;
	SaHpiRptEntryT           *rpt_entry;

	OHOI_CHECK_RPT_ENTRY();
	OHOI_CHECK_CAPABILITY();
	OHOI_CHECK_IRD_ID();

	idrinfo->IdrId = OHOI_IDR_DEFAULT_ID;
	idrinfo->UpdateCount = 0;
	idrinfo->ReadOnly = SAHPI_TRUE;

	/*provide inventory:
          INTERNAL_USE, CHASSIS_INFO, BOARD_INFO, PRODUCT_INFO */
	idrinfo->NumAreas = 4;

	return SA_OK;
}

SaErrorT ohoi_get_idr_area_header(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                                  SaHpiIdrAreaTypeT areatype, SaHpiEntryIdT areaid,
                                  SaHpiEntryIdT *nextareaid, SaHpiIdrAreaHeaderT *header)
{
	struct oh_handler_state  *handler = hnd;
	SaHpiRptEntryT           *rpt_entry;

	int fnum;

	OHOI_CHECK_RPT_ENTRY();
	OHOI_CHECK_CAPABILITY();
	OHOI_CHECK_IRD_ID();
	OHOI_CHECK_AREA_TYPE_ID();

	if (areaid < OHOI_AREA_MAX_ID)
		*nextareaid = areaid + 1;
	else 
		*nextareaid = SAHPI_LAST_ENTRY;
	
	header->AreaId = areaid;
	header->Type = areatype;
	header->ReadOnly = SAHPI_TRUE;
	header->NumFields = fnum;

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
	SaHpiRptEntryT           *rpt_entry;
	ipmi_entity_id_t         ent_id;

	OHOI_CHECK_RPT_ENTRY();
	OHOI_CHECK_CAPABILITY();
	OHOI_CHECK_IRD_ID();

	/*Fix Me : ent_id = 0*/

	switch (areaid) {
		case OHOI_AREA_INTERNAL_USE_ID:
			OHOI_CHECK_FIELDID(1);
			get_inventroy_internal_use(ent_id, field);
			break; 
		case OHOI_AREA_CHASSIS_INFO_ID:
			OHOI_CHECK_FIELDID(2);
			get_inventory_chassis_info(ent_id);			
			break;
		case OHOI_AREA_BOARD_INFO_ID:
			OHOI_CHECK_FIELDID(5);
			get_inventory_board_info(ent_id);	
			break;
		case OHOI_AREA_PRODUCT_INFO_ID:
			OHOI_CHECK_FIELDID(7);
			get_inventory_product_info(ent_id);
			break;
		default:
			return SA_ERR_HPI_INVALID_PARAMS;
	}

	return SA_OK;
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
