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
 */

#include "ipmi.h"
#include <epath_utils.h>
#include <uid_utils.h>
#include <string.h>

#define OHOI_FULFILL_BUFFER(hpi_name, ipmi_name, part_name)     \
        do {                                                    \
                int rv;                                         \
                gen->hpi_name = (void *)off;                    \
                gen->hpi_name->DataType = SAHPI_TL_TYPE_ASCII6; \
                gen->hpi_name->Language = SAHPI_LANG_ENGLISH;   \
                rv = ipmi_entity_get_ ## part_name ## _info_  ## ipmi_name ## _len(ent, &len);  \
                if (rv) {                                       \
                        dbg("%s is not supported", #part_name #hpi_name);                       \
                        break;                                  \
                }                                               \
                if (len>255) {                                  \
                        dbg("%s is too long to HPI: %d", #part_name #hpi_name, len);            \
                        break;                                  \
                }                                               \
                gen->hpi_name->DataLength = len;                \
                len++; /*nil will be put but OpenIPMI*/         \
                ipmi_entity_get_ ## part_name ## _info_ ## ipmi_name(  \
                                ent,                            \
                                &gen->hpi_name->Data[0],        \
                                &len);                          \
                off+= sizeof(SaHpiTextBufferT)                  \
                        + gen->hpi_name->DataLength;            \
        }while(0)


static void _get_inventroy_internal_use(ipmi_entity_t *ent,
                                        void          *cb_data)
{
        SaHpiInventDataRecordT *rec;
        int len;
        int rv;
        
        rec = cb_data;
        
        rec->RecordType = SAHPI_INVENT_RECTYPE_INTERNAL_USE;
        
        rec->DataLength = 0;
        rv = ipmi_entity_get_internal_use_length(ent, &len);
        if (rv) {
                dbg("Error on ipmi_entity_get_internal_use_length: %d", rv);
                return;
        }
        rec->DataLength = len;
        
        rv = ipmi_entity_get_internal_use_data(
                        ent,
                        &rec->RecordData.InternalUse.Data[0],
                        &len);
        if (rv) 
                dbg("Error on  ipmi_entity_get_internal_use_data: %d", rv);
}

static SaErrorT get_inventroy_internal_use(ipmi_entity_id_t       ent_id,
                                           SaHpiInventDataRecordT *rec)
{
        int rv;

        rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_internal_use, rec);
        if (rv) {
                dbg("Error on ipmi_entity_pointer_cb: %d", rv);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        return SA_OK;
}

static void _get_inventroy_chassis_info(ipmi_entity_t *ent,
                                        void          *cb_data)
{
        SaHpiInventDataRecordT  *rec;
        SaHpiInventChassisDataT *chas;
        SaHpiInventGeneralDataT *gen;
        int len;
        unsigned char type;
        int rv;
        unsigned char *off;
        
        rec = cb_data;
        memset(rec, 0, sizeof(*rec));

        rec->RecordType = SAHPI_INVENT_RECTYPE_CHASSIS_INFO;
        rec->DataLength = 0;
        
        chas = &rec->RecordData.ChassisInfo;
        rv   = ipmi_entity_get_chassis_info_type(ent, &type);
        if (rv) {
                dbg("Error on ipmi_entity_get_chassis_info_type:%d", rv);
                return;
        }
        chas->Type = type;
        
        gen = &chas->GeneralData;
        gen->MfgDateTime = SAHPI_TIME_UNSPECIFIED;

        off = cb_data;
        off+= sizeof(SaHpiInventDataRecordT);

        OHOI_FULFILL_BUFFER(SerialNumber, serial_number, chassis);
        OHOI_FULFILL_BUFFER(PartNumber, part_number, chassis);

        rec->DataLength = off - (unsigned char *)&rec->RecordData;
}

static SaErrorT get_inventory_chassis_info(ipmi_entity_id_t       ent_id,
                                           SaHpiInventDataRecordT *rec)
{
        int rv;

        rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_chassis_info, rec);
        if (rv) {
                dbg("Error on ipmi_entity_pointer_cb: %d", rv);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        return SA_OK;
}

static void _get_inventroy_board_info(ipmi_entity_t *ent,
                                      void          *cb_data)
{
        SaHpiInventDataRecordT  *rec;
        SaHpiInventGeneralDataT *gen;
        time_t mfg_time;
        int len;
        unsigned char *off;
        
        rec = cb_data;
        memset(rec, 0, sizeof(*rec));

        rec->RecordType = SAHPI_INVENT_RECTYPE_BOARD_INFO;
        rec->DataLength = 0; 

        gen = &rec->RecordData.BoardInfo;
        
        ipmi_entity_get_board_info_mfg_time(
                        ent,
                        &mfg_time);
        gen->MfgDateTime *= 1000000000L * mfg_time;

        off = cb_data;
        off+= sizeof(SaHpiInventDataRecordT);

        OHOI_FULFILL_BUFFER(Manufacturer, board_manufacturer, board);
        OHOI_FULFILL_BUFFER(ProductName, board_product_name, board);
        OHOI_FULFILL_BUFFER(SerialNumber, board_serial_number, board);
        OHOI_FULFILL_BUFFER(PartNumber, board_part_number, board);
        OHOI_FULFILL_BUFFER(FileId, fru_file_id, board);
        
        rec->DataLength = off - (unsigned char *)&rec->RecordData;        
}

static SaErrorT get_inventory_board_info(ipmi_entity_id_t       ent_id,
                                         SaHpiInventDataRecordT *rec)
{
        int rv;

        rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_board_info, rec);
        if (rv) {
                dbg("Error on ipmi_entity_pointer_cb: %d", rv);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        return SA_OK;
}

static void _get_inventroy_product_info(ipmi_entity_t *ent,
                                        void          *cb_data)
{
        SaHpiInventDataRecordT  *rec;
        SaHpiInventGeneralDataT *gen;
        int len;
        unsigned char *off;
        
        rec = cb_data;
        memset(rec, 0, sizeof(*rec));

        rec->RecordType = SAHPI_INVENT_RECTYPE_PRODUCT_INFO;
        rec->DataLength = 0; 

        gen = &rec->RecordData.ProductInfo;
        
        gen->MfgDateTime = SAHPI_TIME_UNSPECIFIED;

        off = cb_data;
        off+= sizeof(SaHpiInventDataRecordT);

        OHOI_FULFILL_BUFFER(Manufacturer, manufacturer_name, product);
        OHOI_FULFILL_BUFFER(ProductName, product_name, product);
        OHOI_FULFILL_BUFFER(ProductVersion, product_version, product);
        OHOI_FULFILL_BUFFER(SerialNumber, product_serial_number, product);
        OHOI_FULFILL_BUFFER(PartNumber, product_part_model_number, product);
        OHOI_FULFILL_BUFFER(FileId, fru_file_id, product);
        OHOI_FULFILL_BUFFER(AssetTag, asset_tag, product);
        
        rec->DataLength = off - (unsigned char *)&rec->RecordData;        
}

static SaErrorT get_inventory_product_info(ipmi_entity_id_t       ent_id,
                                           SaHpiInventDataRecordT *rec)
{
        int rv;

        rv = ipmi_entity_pointer_cb(ent_id, _get_inventroy_product_info, rec);
        if (rv) {
                dbg("Error on ipmi_entity_pointer_cb: %d", rv);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        return SA_OK;
}

static const int reserved_records_pointer = 4;

static SaErrorT get_inventory_info(ipmi_entity_id_t     ent_id,
                                   SaHpiInventoryDataT  *data)
{
        unsigned char *start, *off;
        
        /* XXX: It is horrible to calculate offset in a structure */
        start = off = (unsigned char *)data;
        off+= sizeof(SaHpiInventoryDataT) 
                + sizeof(SaHpiInventDataRecordT *)*reserved_records_pointer;
        
        memset(data, 0, off-start);
        
        /* XXX: We need to make sure cast-align 
         * stupid code to avoid compiler complain
         */
        data->DataRecords[0] = (void *)off;
        get_inventroy_internal_use(ent_id, data->DataRecords[0]);
        
        /* XXX: this will waste some memory, but still better than overlap */
        off+= sizeof(SaHpiInventDataRecordT) 
                + data->DataRecords[0]->DataLength;
        data->DataRecords[1] = (void *)off;
        get_inventory_chassis_info(ent_id, data->DataRecords[1]);
        
        /* XXX: this will waste some memory, but still better than overlap */
        off+= sizeof(SaHpiInventDataRecordT) 
                + data->DataRecords[1]->DataLength;
        data->DataRecords[2] = (void *)off;
        get_inventory_board_info(ent_id, data->DataRecords[2]);
        
        /* XXX: this will waste some memory, but still better than overlap */
        off+= sizeof(SaHpiInventDataRecordT) 
                + data->DataRecords[2]->DataLength;
        data->DataRecords[3] = (void *)off;
        get_inventory_product_info(ent_id, data->DataRecords[3]);
        
        return SA_OK;
}

static SaErrorT get_inventory_size(ipmi_entity_id_t     ent_id,
                                   int                  *size)
{
        SaErrorT rv;
        SaHpiInventoryDataT *inv;
        int i, len;

        inv = malloc(1024 *  sizeof(int));
        if (!inv) {
                dbg("Out of memory!");
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        rv = get_inventory_info(ent_id, inv);
        if (rv != SA_OK) {
                dbg("Error on getting size");
                free(inv);
                return rv;
        }

        len = sizeof(SaHpiInventoryDataT)
                + sizeof(SaHpiInventDataRecordT *)*reserved_records_pointer;
        for (i=0; inv->DataRecords[i]!=NULL; i++) {
                len += sizeof(SaHpiInventDataRecordT);
                len += inv->DataRecords[i]->DataLength;
        }

        *size = len;
        free(inv);
        return SA_OK;
}

/* ABI stubs */

SaErrorT ohoi_get_inventory_size(void                   *hnd, 
                                 SaHpiResourceIdT       id,
                                 SaHpiEirIdT            num,
                                 SaHpiUint32T           *size)
{        
        struct oh_handler_state         *handler;
        const struct ohoi_resource_id   *ohoi_res_id;
        
        handler  = hnd;

        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("Bug: try to get fru in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        return get_inventory_size(ohoi_res_id->u.entity_id, size);
}

SaErrorT ohoi_get_inventory_info(void *hnd, SaHpiResourceIdT id,
                          SaHpiEirIdT num,
                          SaHpiInventoryDataT *data)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_id   *ohoi_res_id;
        
        handler  = hnd;

        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("Bug: try to get fru in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        return get_inventory_info(ohoi_res_id->u.entity_id, data);;
}

