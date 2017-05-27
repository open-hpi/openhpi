/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

#ifndef _OV_REST_INVENTORY_H
#define _OV_REST_INVENTORY_H

/* Include files */
#include <unistd.h>
#include <SaHpiOvRest.h>
#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_error.h>
#include "ov_rest_discover.h"

/* HPE Synergy resource inventory string */
#define APPLIANCE_INVENTORY_STRING "Appliance Inventory"
#define ENCLOSURE_INVENTORY_STRING "Enclosure Inventory"
#define SERVER_INVENTORY_STRING "Server Inventory"
#define INTERCONNECT_INVENTORY_STRING "Interconnect Inventory"
#define FAN_INVENTORY_STRING "Fan Inventory"
#define FAN_RDR_STRING "Fan"
#define POWER_SUPPLY_INVENTORY_STRING "Power Supply Inventory"
#define POWER_SUPPLY_RDR_STRING "Power Supply"

/* Inventory data field structure */
struct ov_rest_field
{
        SaHpiIdrFieldT      field;
        struct ov_rest_field *next_field;
};

/* Inventory data area structure */
struct ov_rest_area
{
        SaHpiIdrAreaHeaderT idr_area_head;
        struct ov_rest_field *field_list;
        struct ov_rest_area *next_area;
};

struct ov_rest_inventory_info
{
        SaHpiIdrInfoT       idr_info;
        struct ov_rest_area  *area_list;
};

/* Inventory data respository header structure */
struct ov_rest_inventory
{
        SaHpiInventoryRecT        inv_rec;
        struct ov_rest_inventory_info info;
        char *comment;
};

/* Maximum areas for a resource in OV REST
 * 
 * If a new area is added for a resource, then change the maximum inventory
 * area value. Accordingly, add dummy elements into global inventory RDR 
 * array in ov_rest_resources.c
 */
#define OV_REST_MAX_INV_AREAS 3

/* Maximum fields in an area in OV REST
 * 
 * If a new field in an area for a resource, then change the maximum inventory
 * fields value. Accordingly, add dummy elements into global inventory RDR 
 * array in ov_rest_resources.c
 */
#define OV_REST_MAX_INV_FIELDS 3

struct ov_rest_inv_area {
	struct ov_rest_area area;
	struct ov_rest_field field_array[OV_REST_MAX_INV_FIELDS];
};

struct ov_rest_inv_rdr {
	SaHpiRdrT rdr;
	struct ov_rest_inventory inventory;
	struct ov_rest_inv_area area_array[OV_REST_MAX_INV_AREAS];
};

/* The maximum size of the fan zone inventory field data.
 * In c3000, it can have the data "1,2,3,4,5,6,7,8" for device bays.
 * 3 bytes of char array is enough to accomodate above information.
 *
 * On supporting new enclosure type, change the below #define value as
 * appropriate
 */
#define	OV_REST_MAX_FZ_INV_SIZE 31

/* Maximum size of fan zone number digits. In c7000, 4 fan zones are supported.
 * 1 digit is required to represent the fan zone number.
 *
 * On supporting new enclosure type, change the below #define value as
 * appropriate
 */
#define OV_REST_MAX_FZ_NUM_SIZE 1

/* Structure for mapping the Fans to Fan zones with shared status information.
 * This will be used to construct the inventory data field for Fan
 */
struct ov_rest_fz_map {
	SaHpiInt32T zone;
	SaHpiInt32T secondary_zone;
	SaHpiBoolT shared;
};

/* Inventory function declarations */
SaErrorT ov_rest_get_idr_info(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiIdrInfoT *idr_info);

SaErrorT ov_rest_get_idr_area_header(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiIdrIdT idr_id,
                                    SaHpiIdrAreaTypeT area_type,
                                    SaHpiEntryIdT area_id,
                                    SaHpiEntryIdT *next_area_id,
                                    SaHpiIdrAreaHeaderT *area_header);

SaErrorT ov_rest_add_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiIdrAreaTypeT area_type,
                              SaHpiEntryIdT *area_id);

SaErrorT ov_rest_add_idr_area_by_id(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiIdrIdT idr,
                                    SaHpiIdrAreaTypeT area_type,
                                    SaHpiEntryIdT area_id);

SaErrorT ov_rest_del_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr_id,
                              SaHpiEntryIdT area_id);

SaErrorT ov_rest_get_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiEntryIdT area_id,
                               SaHpiIdrFieldTypeT field_type,
                               SaHpiEntryIdT field_id,
                               SaHpiEntryIdT *next_field_id,
                               SaHpiIdrFieldT *field);

SaErrorT ov_rest_add_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiIdrFieldT *field);

SaErrorT ov_rest_add_idr_field_by_id(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiIdrIdT idr_id,
                                     SaHpiIdrFieldT *field);

SaErrorT ov_rest_set_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiIdrFieldT *field);

SaErrorT ov_rest_del_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr_id,
                               SaHpiEntryIdT area_id,
                               SaHpiEntryIdT field_id);

SaErrorT ov_rest_build_appliance_inv_rdr(struct oh_handler_state *oh_handler,
                                 struct applianceNodeInfo *response,
                                 SaHpiRdrT *rdr,
                                 struct ov_rest_inventory **inventory);

SaErrorT ov_rest_build_enclosure_inv_rdr(struct oh_handler_state *oh_handler,
                                 struct enclosureInfo *response,
                                 SaHpiRdrT *rdr,
                                 struct ov_rest_inventory **inventory);

SaErrorT ov_rest_build_server_inv_rdr(struct oh_handler_state *oh_handler,
				SaHpiResourceIdT resource_id,
				SaHpiRdrT *rdr,
				struct ov_rest_inventory **pinv,
				struct serverhardwareInfo *response);

SaErrorT build_inserted_server_inv_rdr(struct oh_handler_state *oh_handler,
                                       SaHpiInt32T bay_number,
                                       SaHpiRdrT *rdr,
                                       struct ov_rest_inventory **inventory);

SaErrorT ov_rest_build_drive_enclosure_inv_rdr(
				struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **pinv,
                                struct driveEnclosureInfo *response);

SaErrorT build_interconnect_inv_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
                                struct interconnectInfo *response);

SaErrorT ov_rest_build_fan_inv_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
                                struct fanInfo *response);
SaErrorT build_powersupply_inv_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
                                struct powersupplyInfo *response);

SaErrorT ov_rest_add_product_area(struct ov_rest_area **parea,
                          char *name,
                          char *manufacturer,
                          SaHpiInt32T *success_flag);

SaErrorT ov_rest_add_chassis_area(struct ov_rest_area **parea,
                          char *part_number,
                          char *serial_number,
                          SaHpiInt32T *success_flag);

SaErrorT ov_rest_add_board_area(struct ov_rest_area **parea,
                        char *part_number,
                        char *serial_number,
                        SaHpiInt32T *success_flag);

SaErrorT ov_rest_add_internal_area(struct ov_rest_area **parea,
                           char *manufacturer,
                           char *name,
                           char *part_number,
                           char *serial_number,
                           SaHpiInt32T *success_flag);

SaErrorT  ov_rest_idr_area_add(struct ov_rest_area **area_ptr,
                       SaHpiIdrAreaTypeT area_type,
                       struct ov_rest_area **return_area);

SaErrorT  ov_rest_idr_area_add_by_id(struct ov_rest_area **head_area,
                             SaHpiIdrAreaTypeT area_type,
                             SaHpiEntryIdT area_id);

SaErrorT ov_rest_idr_area_delete(struct ov_rest_area **area_ptr,
                         SaHpiEntryIdT area_id);

SaErrorT ov_rest_fetch_idr_area_header(struct ov_rest_inventory_info *inv_ptr,
                             SaHpiEntryIdT area_id,
                             SaHpiIdrAreaTypeT area_type,
                             SaHpiIdrAreaHeaderT *area_header,
                             SaHpiEntryIdT *next_area_id);

SaErrorT  ov_rest_idr_field_add(struct ov_rest_field **field_ptr,
                        SaHpiIdrFieldT *field);

SaErrorT  ov_rest_idr_field_add_by_id(struct ov_rest_field **head_field,
                              SaHpiEntryIdT area_id,
                              SaHpiIdrFieldTypeT field_type,
                              char *field_data,
                              SaHpiEntryIdT field_id);

SaErrorT ov_rest_idr_field_delete(struct ov_rest_field **field_ptr,
                          SaHpiEntryIdT field_id);

SaErrorT ov_rest_idr_field_update(struct ov_rest_field *field_ptr,
                          SaHpiIdrFieldT *field);

SaErrorT ov_rest_fetch_idr_field(struct ov_rest_inventory_info *inv_ptr,
                       SaHpiEntryIdT area_id,
                       SaHpiIdrFieldTypeT field_type,
                       SaHpiEntryIdT field_id,
                       SaHpiEntryIdT *next_field_id,
                       SaHpiIdrFieldT *field);

SaErrorT ov_rest_free_inventory_info(struct oh_handler_state *handler,
             SaHpiResourceIdT resource_id);
#endif /* _OV_REST_INVENTORY_H */
