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

static void add_inventory_event_rdr(
                SaHpiRdrT		*rdr,
                SaHpiEntityPathT	parent_ep)
{
        char name[] = "FRU Inventory data";
        
        rdr->RecordId = 0;
        rdr->RdrType = SAHPI_INVENTORY_RDR;
        rdr->Entity = parent_ep;
	rdr->IsFru = SAHPI_TRUE;

	/* One Fru has only one inventory, so IdrId always is 0 */
	rdr->RdrTypeUnion.InventoryRec.IdrId = 0;
        rdr->RdrTypeUnion.InventoryRec.Persistent = SAHPI_TRUE;
	rdr->RdrTypeUnion.InventoryRec.Oem = 0;

        rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
        rdr->IdString.Language = SAHPI_LANG_ENGLISH;
        rdr->IdString.DataLength = sizeof(name);

        memcpy(rdr->IdString.Data, name, sizeof(name));
}
/*
#define IPMI_FRU_FTR_INTERNAL_USE_AREA 0
#define IPMI_FRU_FTR_CHASSIS_INFO_AREA 1
#define IPMI_FRU_FTR_BOARD_INFO_AREA   2
#define IPMI_FRU_FTR_PRODUCT_INFO_AREA 3
#define IPMI_FRU_FTR_MULTI_RECORD_AREA 4
*/

static void init_inventory_info(struct ohoi_resource_info *res_info,
			ipmi_entity_t     *ent)
{
	ipmi_fru_t   *fru = ipmi_entity_get_fru(ent);
	struct ohoi_inventory_info *i_info;
	unsigned int len;
	unsigned char uch;
	time_t tm;
	unsigned int i;
	
	if (fru == NULL) {
		dbg("ipmi_entity_get_fru returned NULL");
		return;
	}
	i_info = malloc(sizeof(*i_info));
        if (!i_info) {
                dbg("Out of memory");
                return;
        }
        memset(i_info, 0, sizeof(*i_info));
	i_info->mutex = g_mutex_new();
	if (ipmi_fru_area_get_length(
			fru, IPMI_FRU_FTR_INTERNAL_USE_AREA, &len) == 0) {
		i_info->iu = 255;
	}
	if (ipmi_fru_area_get_length(
			fru, IPMI_FRU_FTR_CHASSIS_INFO_AREA, &len) == 0) {
		i_info->ci = 255;
		if (ipmi_fru_get_chassis_info_type(fru, &uch) == 0) {
			i_info->ci_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE);
		}
		if (ipmi_fru_get_chassis_info_part_number_len(fru, &len) == 0) {
			i_info->ci_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PART_NUMBER);
		}
		if (ipmi_fru_get_chassis_info_serial_number_len(fru, &len) == 0) {
			i_info->ci_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER);
		}
		for (i = 0; ;i++) {
			if (ipmi_fru_get_chassis_info_custom_len(fru, i, &len) == 0) {
				break;
			}
			i_info->ci_fld_msk |=
					(1 << SAHPI_IDR_FIELDTYPE_CUSTOM);
			i_info->ci_custom_num++;
		}
	}
	if (ipmi_fru_get_board_info_lang_code(fru, &i_info->bi) == 0) {
		if (i_info->bi == 0) {
			i_info->bi = SAHPI_LANG_ENGLISH;
		}
		if (ipmi_fru_get_board_info_board_manufacturer_len(fru, &len) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_MANUFACTURER);
		}
		if (ipmi_fru_get_board_info_mfg_time(fru, &tm) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_MFG_DATETIME);
		}
		if (ipmi_fru_get_board_info_board_product_name_len(fru, &len) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PRODUCT_NAME);
		}
		if (ipmi_fru_get_board_info_board_serial_number_len(fru, &len) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER);
		}
		if (ipmi_fru_get_board_info_board_part_number_len(fru, &len) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PART_NUMBER);
		}
		if (ipmi_fru_get_board_info_fru_file_id_len(fru, &len) == 0) {
			i_info->bi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_FILE_ID);
		}
		for (i = 0; ;i++) {
			if (ipmi_fru_get_board_info_custom_len(fru, i, &len) != 0) {
				break;
			}
			i_info->bi_fld_msk |=
					(1 << SAHPI_IDR_FIELDTYPE_CUSTOM);
			i_info->bi_custom_num++;
		}
	}
	if (ipmi_fru_get_product_info_lang_code(fru, &i_info->pi) == 0) {
		if (i_info->pi == 0) {
			i_info->pi = SAHPI_LANG_ENGLISH;
		}
		if (ipmi_fru_get_product_info_manufacturer_name_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_MANUFACTURER);
		}
		if (ipmi_fru_get_product_info_product_name_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PRODUCT_NAME);
		}
		if (ipmi_fru_get_product_info_product_part_model_number_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PART_NUMBER);
		}
		if (ipmi_fru_get_product_info_product_version_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION);
		}
		if (ipmi_fru_get_product_info_product_serial_number_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER);
		}
		if (ipmi_fru_get_product_info_asset_tag_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_ASSET_TAG);
		}
		if (ipmi_fru_get_product_info_fru_file_id_len(fru, &len) == 0) {
			i_info->pi_fld_msk |=
				(1 << SAHPI_IDR_FIELDTYPE_FILE_ID);
		}
		for (i = 0; ;i++) {
			if (ipmi_fru_get_product_info_custom_len(fru, i, &len) != 0) {
				break;
			}
			i_info->pi_fld_msk |=
					(1 << SAHPI_IDR_FIELDTYPE_CUSTOM);
			i_info->pi_custom_num++;
		}
	}
	if (ipmi_fru_area_get_length(
			fru, IPMI_FRU_FTR_MULTI_RECORD_AREA, &len) == 0) {
		i_info->oem = 255;
		i_info->oem_fields_num = ipmi_fru_get_num_multi_records(fru);
	}
	res_info->fru = i_info;
}

static void add_inventory_event(struct ohoi_resource_info *res_info,
			      ipmi_entity_t     *ent,
			      struct oh_handler_state *handler,
			      SaHpiRptEntryT           *rpt_entry)
{
        struct oh_event *e;
	SaHpiEntityPathT  parent_ep = rpt_entry->ResourceEntity;
	SaHpiResourceIdT  rid = rpt_entry->ResourceId;
        
	init_inventory_info(res_info, ent);
	if (res_info->fru == NULL) {
		return;
	}
	e = malloc(sizeof(*e));
        if (!e) {
                dbg("Out of memory");
		free(res_info->fru);
		res_info->fru = NULL;
                return;
        }
        memset(e, 0, sizeof(*e));
        e->type = OH_ET_RDR;

        add_inventory_event_rdr(&e->u.rdr_event.rdr, parent_ep);

        rid = oh_uid_lookup(&e->u.rdr_event.rdr.Entity);
        
        if (!oh_add_rdr(handler->rptcache, rid, &e->u.rdr_event.rdr, NULL, 0)) {
		rpt_entry->ResourceCapabilities |= SAHPI_CAPABILITY_INVENTORY_DATA |
			SAHPI_CAPABILITY_RDR;
	}
}

/* Per IPMI spec., one FRU per entity */
void ohoi_inventory_event(enum ipmi_update_e    op,
                          ipmi_entity_t         *entity,
                          void                  *cb_data)
{
       struct oh_handler_state  *handler = cb_data;
       struct ohoi_resource_info *res_info;

       ipmi_entity_id_t         entity_id;
       SaHpiRptEntryT           *rpt_entry;

       entity_id = ipmi_entity_convert_to_id(entity);
       
       rpt_entry = ohoi_get_resource_by_entityid(
                       handler->rptcache,
                       &entity_id);
       if (!rpt_entry) {
               dump_entity_id("FRU without RPT entry?!", entity_id);
               return;
       }

       res_info = oh_get_resource_data(handler->rptcache,
				       rpt_entry->ResourceId);
       if (op == IPMI_ADDED) {
		trace_ipmi("FRU added");
		add_inventory_event(res_info, entity, handler, rpt_entry);
	}
	trace_ipmi("Set updated for res_info %p(%d). Inventory",
		res_info, rpt_entry->ResourceId);
	entity_rpt_set_updated(res_info, handler->data);;
			   
}
