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

#include <math.h>
#include <string.h>
#include "sahpi_wrappers.h"
#include "ov_rest_inventory.h"
#include "ov_rest_utils.h"

/**
 * ov_rest_build_enclosure_inv_rdr:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID of the Enclosure
 *      @rdr: Rdr Structure for inventory data
 *      @inventory: Rdr private data structure
 *      @response: Pointer to enclosureInfo structure
 *
 * Purpose:
 *      Creates an inventory rdr for enclosure.
 *
 * Detailed Description:
 *      - Populates the enclosure inventory rdr.
 *      - Inventory data repository is created and associated in the private
 *        data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_build_enclosure_inv_rdr(struct oh_handler_state *oh_handler,
                                 struct enclosureInfo *response,
                                 SaHpiRdrT *rdr,
                                 struct ov_rest_inventory **inventory)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	char enclosure_inv_str[] = ENCLOSURE_INVENTORY_STRING, *tmp = NULL;
	struct ov_rest_inventory *local_inventory = NULL;
	struct ovRestArea *head_area = NULL;
	SaHpiInt32T add_success_flag = 0;
	SaHpiInt32T product_area_success_flag = 0;
	SaHpiInt32T area_count = 0;
	struct ov_rest_handler *ov_handler = NULL;
	SaHpiResourceIdT resource_id;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiFloat64T fm_version;
	SaHpiInt32T major;

	if (oh_handler == NULL || response == NULL || rdr == NULL ||
			inventory == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	resource_id = ov_handler->ov_rest_resources.enclosure->enclosure_rid;
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for the enclosure id %d",
							resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Populating inventory rdr with default values and resource name */
	rdr->Entity = rpt->ResourceEntity;
	rdr->RecordId = 0;
	rdr->RdrType  = SAHPI_INVENTORY_RDR;
	rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
	rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->name);
	rdr->IdString.DataLength = strlen(response->name);
	snprintf((char *)rdr->IdString.Data,
			strlen(response->name) + 1,
			"%s", response->name);

	/* Create inventory IDR and populate the IDR header */
	local_inventory = (struct ov_rest_inventory*)
		g_malloc0(sizeof(struct ov_rest_inventory));
	if (!local_inventory) {
		err("OV REST out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.IdrId =
		rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.UpdateCount = 1;
	local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
	local_inventory->info.idr_info.NumAreas = 0;
	local_inventory->info.area_list = NULL;
	local_inventory->comment =
		(char *)g_malloc0(strlen(enclosure_inv_str) + 1);
	strcpy(local_inventory->comment, enclosure_inv_str);

	/* Create and add product area if resource name and/or manufacturer
	 * information exist
	 */
	rv = ov_rest_add_product_area(&local_inventory->info.area_list,
			response->name,
			response->manufacturer,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add product area failed for the enclosure id %d",
							resource_id);
		return rv;
	}

	/* add_success_flag will be true if product area is added,
	 * if this is the first successful creation of IDR area, then have
	 * area pointer stored as the head node for area list
	 */
	if (add_success_flag != SAHPI_FALSE) {
		product_area_success_flag = SAHPI_TRUE;
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add chassis area if resource part number and/or
	 * serial number exist
	 */
	rv = ov_rest_add_chassis_area(&local_inventory->info.area_list,
			response->partNumber,
			response->serialNumber,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add chassis area failed for the enclosure id %d",
						resource_id);
		return rv;
	}
	if (add_success_flag != SAHPI_FALSE) {
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	local_inventory->info.area_list = head_area;
	*inventory = local_inventory;

	/* Adding the product version in IDR product area.  It is added at
	 * the end of the field list.
	 */
	if (product_area_success_flag == SAHPI_TRUE) {
		/* Add the product version field if the enclosure hardware info
		 * is available
		 */
		if (response->hwVersion != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
			strcpy ((char *)hpi_field.Field.Data,
					response->hwVersion);

			rv = ov_rest_idr_field_add(&(
						local_inventory->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field hwVersion failed for the"
					" enclosure id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
			
			/* Check whether Firmware version is NULL. */
			if (!response->hwVersion) {
				err("Firmware version is not vailable for the"
					"resource %d", resource_id);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			/* Store Firmware MajorRev & MinorRev data in rpt */
			fm_version = atof(response->hwVersion);
			rpt->ResourceInfo.FirmwareMajorRev = major =
					(SaHpiUint8T)floor(fm_version);
			rpt->ResourceInfo.FirmwareMinorRev = rintf((
					fm_version - major) * 100);
		}

		if (response->uri != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
			WRAP_ASPRINTF(&tmp,"URI = %s",response->uri);
			strcpy ((char *)hpi_field.Field.Data, tmp);
			wrap_free(tmp);

			rv = ov_rest_idr_field_add(&(
						local_inventory->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field uri failed for the"
					" enclosure id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
		}

	}
	return SA_OK;
}

/**
 * ov_rest_build_server_inv_rdr:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID of the server-hardware
 *      @rdr: Rdr Structure for inventory data
 *      @inventory: Rdr private data structure
 *      @response: Pointer to serverhardwareInfo structure
 *
 * Purpose:
 *      Creates an inventory rdr for server-hardware.
 *
 * Detailed Description:
 *      - Populates the server-hardware inventory rdr.
 *      - Inventory data repository is created and associated in the private
 *        data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_build_server_inv_rdr(struct oh_handler_state *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiRdrT *rdr,
                              struct ov_rest_inventory **inventory,
			      struct serverhardwareInfo *response)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	char server_inv_str[] = SERVER_INVENTORY_STRING, *tmp = NULL;
	struct ov_rest_inventory *local_inventory = NULL;
	struct ovRestArea *head_area = NULL;
	SaHpiInt32T add_success_flag = 0;
	SaHpiInt32T product_area_success_flag = 0;
	SaHpiInt32T area_count = 0;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiFloat64T fm_version;
	SaHpiInt32T major;

	if (oh_handler == NULL || rdr == NULL ||
			inventory == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (!rpt) {
		err("Could not find blade resource rpt for resource id %d",
							resource_id);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	rdr->Entity = rpt->ResourceEntity;


	/* Populating the inventory rdr with rpt values for the resource */
	rdr->RecordId = 0;
	rdr->RdrType  = SAHPI_INVENTORY_RDR;
	rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
	rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->model);
	rdr->IdString.DataLength = strlen(response->model);
	snprintf((char *)rdr->IdString.Data,
			strlen(response->model) + 1,"%s",
			response->model );

	/* Create inventory IDR and populate the IDR header */
	local_inventory = (struct ov_rest_inventory*)
		g_malloc0(sizeof(struct ov_rest_inventory));
	if (!local_inventory) {
		err("OV REST out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.IdrId =
		rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.UpdateCount = 1;
	local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
	local_inventory->info.idr_info.NumAreas = 0;
	local_inventory->info.area_list = NULL;
	local_inventory->comment =
		(char *)g_malloc0(strlen(server_inv_str) + 1);
	strcpy(local_inventory->comment, server_inv_str);

	/* Create and add product area if resource name and/or manufacturer
	 *information exist
	 */
	rv = ov_rest_add_product_area(&local_inventory->info.area_list,
			response->model,
			response->manufacturer,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add product area failed for the server id %d",
							resource_id);
		return rv;
	}

	/* add_success_flag will be true if product area is added,
	 * if this is the first successful creation of IDR area, then have
	 * area pointer stored as the head node for area list
	 */
	if (add_success_flag != SAHPI_FALSE) {
		product_area_success_flag = SAHPI_TRUE;
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add board area if resource part number and/or
	 * serial number exist
	 */
	rv = ov_rest_add_board_area(&local_inventory->info.area_list,
			response->partNumber,
			response->serialNumber,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add board area failed for the server id %d", resource_id);
		return rv;
	}
	if (add_success_flag != SAHPI_FALSE) {
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}
	local_inventory->info.area_list = head_area;
	*inventory = local_inventory;

	/* Adding the product version in IDR product area.  It is added at
	 * the end of the field list.
	 */
	if (product_area_success_flag == SAHPI_TRUE) {

		/* Add the product version field if the firmware info
		 * is available
		 */
		if (response->fwVersion != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
			strcpy ((char *)hpi_field.Field.Data,
					response->fwVersion);

			rv = ov_rest_idr_field_add(&(
						local_inventory->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field fwVersion failed for"
					" server id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;

			/* Check whether Firmware version is NULL. */
			if (!response->fwVersion) {
				err("Firmware version is not vailable for the"
					"resource %d", resource_id);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			/* Store Firmware MajorRev & MinorRev data in rpt */
			fm_version = atof(response->fwVersion);
			rpt->ResourceInfo.FirmwareMajorRev = major =
				(SaHpiUint8T)floor(fm_version);
			rpt->ResourceInfo.FirmwareMinorRev = rintf((
						fm_version - major) * 100);
		}
		if (response->uri != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
			WRAP_ASPRINTF(&tmp,"URI = %s",response->uri);
			strcpy ((char *)hpi_field.Field.Data, tmp);
			wrap_free(tmp);

			rv = ov_rest_idr_field_add(&(
						local_inventory->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field uri failed for the server"
						"  id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
		}
	}
	return SA_OK;
}

/**
 * ov_rest_build_drive_enclosure_inv_rdr
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID of the Drive Enclosure
 *      @rdr: Rdr Structure for inventory data
 *      @inventory: Rdr private data structure
 *      @response: Pointer to driveEnclocureInfo Structure
 *
 * Purpose:
 *      Creates an inventory rdr for drive enclosure/ storage blade
 *
 * Detailed Description:
 *      - Populates the Drive Enclosure inventory rdr
 *      - Inventory data repository is created and associated in the private
 *        data area of the Inventory RDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_build_drive_enclosure_inv_rdr(
                              struct oh_handler_state *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiRdrT *rdr,
                              struct ov_rest_inventory **inventory,
                              struct driveEnclosureInfo *response)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	char server_inv_str[] = SERVER_INVENTORY_STRING, *tmp = NULL;
	struct ov_rest_inventory *local_inventory = NULL;
	struct ovRestArea *head_area = NULL;
	SaHpiInt32T add_success_flag = 0;
	SaHpiInt32T product_area_success_flag = 0;
	SaHpiInt32T area_count = 0;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiFloat64T fm_version;
	SaHpiInt32T major;

	if (oh_handler == NULL || rdr == NULL ||
			inventory == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (!rpt) {
		err("Could not find Drive Enclosure resource rpt for"
				" resource id %d", resource_id);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	rdr->Entity = rpt->ResourceEntity;

	/* Populating the inventory rdr with rpt values for the resource */
	rdr->RecordId = 0;
	rdr->RdrType  = SAHPI_INVENTORY_RDR;
	rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
	rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->model);
	rdr->IdString.DataLength = strlen(response->model);
	snprintf((char *)rdr->IdString.Data,
			strlen(response->model) + 1,"%s",
			response->model );

	/* Create inventory IDR and populate the IDR header */
	local_inventory = (struct ov_rest_inventory*)
		g_malloc0(sizeof(struct ov_rest_inventory));
	if (!local_inventory) {
		err("OV REST out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.IdrId =
		rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.UpdateCount = 1;
	local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
	local_inventory->info.idr_info.NumAreas = 0;
	local_inventory->info.area_list = NULL;
	local_inventory->comment =
		(char *)g_malloc0(strlen(server_inv_str) + 1);
	if (!local_inventory->comment) {
		err("OV REST out of memory");
		wrap_g_free(local_inventory);
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	strcpy(local_inventory->comment, server_inv_str);

	/* Create and add product area if resource name and/or manufacturer
	 * information exist
	 */
	rv = ov_rest_add_product_area(&local_inventory->info.area_list,
			response->model,
			response->manufacturer,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add product area failed for drive enclosure id %d",
							resource_id);
		wrap_g_free(local_inventory);
		wrap_g_free(local_inventory->comment);
		return rv;
	}

	/* add_success_flag will be true if product area is added,
	 * if this is the first successful creation of IDR area, then have
	 * area pointer stored as the head node for area list
	 */
	if (add_success_flag != SAHPI_FALSE) {
		product_area_success_flag = SAHPI_TRUE;
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add board area if resource part number and/or
	 * serial number exist
	 */
	rv = ov_rest_add_board_area(&local_inventory->info.area_list,
			response->partNumber,
			response->serialNumber,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add board area failed for drive enclosure id %d",
							resource_id);
		wrap_g_free(local_inventory);
		wrap_g_free(local_inventory->comment);
		return rv;
	}
	if (add_success_flag != SAHPI_FALSE) {
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}
	local_inventory->info.area_list = head_area;
	*inventory = local_inventory;

	/* Adding the product version in IDR product area.  It is added at
	 * the end of the field list.
	 */
	if (product_area_success_flag == SAHPI_TRUE) {

		/* Add the product version field if the firmware info
		 * is available
		 */
		if (response->fwVersion != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
			strcpy ((char *)hpi_field.Field.Data,
					response->fwVersion);

			rv = ov_rest_idr_field_add(&(local_inventory
						->info.area_list
						->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field fwVersion failed"
					" for drive enclosure id %d",
						resource_id);
				wrap_g_free(local_inventory);
				wrap_g_free(local_inventory->comment);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;

			/* Check whether Firmware version is NULL. */
			if (!response->fwVersion) {
				err("Firmware version is not vailable for the"
					"resource %d", resource_id);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			/* Store Firmware MajorRev & MinorRev data in rpt */
			fm_version = atof(response->fwVersion);
			rpt->ResourceInfo.FirmwareMajorRev = major =
				(SaHpiUint8T)floor(fm_version);
			rpt->ResourceInfo.FirmwareMinorRev = 
				rintf((fm_version - major) * 100);
		}
		if (response->uri != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
			WRAP_ASPRINTF(&tmp,"URI = %s",response->uri);
			strcpy ((char *)hpi_field.Field.Data, tmp);
			wrap_free(tmp);

			rv = ov_rest_idr_field_add(&(local_inventory
						->info.area_list
						->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field uri failed for drive "
					"enclosure id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
		}
	}
	return SA_OK;
}

/**
 * build_interconnect_inv_rdr
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource Id of the interconnect
 *      @rdr: Rdr Structure for inventory data
 *      @inventory: Rdr private data structure
 *
 * Purpose:
 *      Creates an inventory rdr for interconnect blade
 *
 * Detailed Description:
 *      - Populates the interconnect inventory rdr with default values
 *      - Inventory data repository is created and associated as part of the
 *        private data area of the Inventory RDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT build_interconnect_inv_rdr(struct oh_handler_state *oh_handler,
				SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
				struct interconnectInfo *response)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	char interconnect_inv_str[] = INTERCONNECT_INVENTORY_STRING, 
	     *tmp = NULL;
	struct ov_rest_inventory *local_inventory = NULL;
	struct ovRestArea *head_area = NULL;
	SaHpiInt32T add_success_flag = 0;
	SaHpiInt32T product_area_success_flag = 0;
	SaHpiInt32T area_count = 0;
	SaHpiRptEntryT *rpt = NULL;
	char temp[256];

	if (oh_handler == NULL || rdr == NULL || response == NULL ||
			inventory == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for interconnect in bay %d of resource id %d",
					response->bayNumber, resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Populating the inventory rdr with rpt values for the resource */
	rdr->Entity = rpt->ResourceEntity;
	rdr->RecordId = 0;
	rdr->RdrType  = SAHPI_INVENTORY_RDR;
	rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
	rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->model);
	rdr->IdString.DataLength = strlen(response->model);
	snprintf((char *)rdr->IdString.Data,
			strlen(response->model)+ 1,
			"%s",response->model );

	/* Create inventory IDR and populate the IDR header */
	local_inventory = (struct ov_rest_inventory*)
		g_malloc0(sizeof(struct ov_rest_inventory));
	if (!local_inventory) {
		err("OV REST out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.IdrId =
		rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.UpdateCount = 1;
	local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
	local_inventory->info.idr_info.NumAreas = 0;
	local_inventory->comment =
		(char *)g_malloc0(strlen(interconnect_inv_str) + 1);
	strcpy(local_inventory->comment, interconnect_inv_str);

	/* Create and add product area if resource name and/or manufacturer
	 * information exist
	 */
	ov_rest_lower_to_upper(response->model, strlen(response->model),temp,
			256);
	if (strstr(temp, "CISCO") != NULL)
		rv = ov_rest_add_product_area(&local_inventory->info.area_list,
				response->model,
				"CISCO",
				&add_success_flag);
	else
		rv = ov_rest_add_product_area(&local_inventory->info.area_list,
				response->model,
				"HPE",
				&add_success_flag);

	if (rv != SA_OK) {
		err("Add product area failed for interconnect id %d",
							resource_id);
		return rv;
	}

	/* add_success_flag will be true if product area is added,
	 * if this is the first successful creation of IDR area, then have
	 * area pointer stored as the head node for area list
	 */
	if (add_success_flag != SAHPI_FALSE) {
		product_area_success_flag = SAHPI_TRUE;
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add board area if resource part number and/or
	 * serial number exist
	 */
	rv = ov_rest_add_board_area(&local_inventory->info.area_list,
			response->partNumber,
			response->serialNumber,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add board area failed for interconnect id %d",
							resource_id);
		return rv;
	}
	if (add_success_flag != SAHPI_FALSE) {
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	local_inventory->info.area_list = head_area;
	*inventory = local_inventory;
	/* Adding the product version in IDR product area.  It is added at
	 * the end of the field list.
	 */
	if (product_area_success_flag == SAHPI_TRUE) {
		/* Add the product version field if the firmware info
		 * is available
		 */
		if (response->uri != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
			WRAP_ASPRINTF(&tmp,"URI = %s",response->uri);
			strcpy ((char *)hpi_field.Field.Data, tmp);
			wrap_free(tmp);
			rv = ov_rest_idr_field_add(&(
						local_inventory->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr field uri failed for interconnect"
					" id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
		}
	}
	return SA_OK;
}


/*
 * ov_rest_add_product_area
 *      @area: IDR area pointer
 *      @name: Resource name
 *      @manufacturer: Resource manufacturer
 *      @success_flag: Flag for checking area creation
 *
 * Purpose:
 *      Creates an IDR product area with required fields
 *
 * Detailed Description:
 *      - Checks whether the name and manufacturer details are available for
 *        the resource which has called this module
 *      - If either or both of these information is available then IDR area
 *        of Product info type is created and these informations are added as
 *        individual IDR fields in the newly create product area
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_product_area(struct ovRestArea **area,
                          char *name,
                          char *manufacturer,
                          SaHpiInt32T *success_flag)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	struct ovRestArea *local_area = NULL;
	struct ovRestField *field = NULL;
	struct ovRestField *head_field = NULL;
	SaHpiInt32T field_count = 0;

	if (area == NULL || success_flag == NULL) {
		err("Invalid Parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* If both name and manufacturer information is NULL
         * then product area is not created
         */
	if (name == NULL &&  manufacturer == NULL) {
		err("Product Area:Required information not available");
		err("Product area not created");
		*success_flag = SAHPI_FALSE;
		return SA_OK;
	}

	/* Create area of type PRODUCT INFO */

	rv = ov_rest_idr_area_add(area,
			SAHPI_IDR_AREATYPE_PRODUCT_INFO,
			&local_area);
	if (rv != SA_OK) {
		err("Add idr area failed for %s", name);
		return rv;
	}
	*success_flag = SAHPI_TRUE;

	/* Add the fields to the newly created product area */
	field = local_area->field_list;
	if (name != NULL) {
		/* Add product name field to the IDR product area */
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
		strcpy ((char *)hpi_field.Field.Data, name);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field failed for %s", name);
			return rv;
		}
		++field_count;

		/* if this is the first successful creation of IDR field in
                 * the IDR area, then have field pointer stored as the head
                 * node for field list
                 */
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	if (manufacturer != NULL) {

		/* Add manufacturer field to the IDR product area */
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER;
		strcpy ((char *)hpi_field.Field.Data, manufacturer);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field failed for %s", name);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	local_area->field_list = head_field;
	return SA_OK;
}


/**
 * ov_rest_add_chassis_area
 *      @area: IDR area pointer
 *      @partNumber: Resource part number
 *      @serialNumber: Resource serialNumber
 *      @success_flag: Flag for checking area creation
 *
 * Purpose:
 *      Creates an IDR chassis area with required fields
 *
 * Detailed Description:
 *      - Checks whether the part number and serial number details are
 *        available for the resource which has called this module
 *      - If either or both of these information is available then IDR area
 *        of Chassis info type is created and these informations are added as
 *        individual IDR fields in the newly create chassis area
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_chassis_area(struct ovRestArea **area,
                          char *partNumber,
                          char *serialNumber,
                          SaHpiInt32T *success_flag)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	struct ovRestArea *local_area = NULL;
	struct ovRestField *field = NULL;
	struct ovRestField *head_field = NULL;
	SaHpiInt32T field_count = 0;

	if (area == NULL || success_flag == NULL) {
		err("Invalid Parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* If both part number and serial number information is NULL
         * then chassis area is not created
         */
	if (partNumber == NULL && serialNumber == NULL) {
		err("Chassis Area:Required information not available");
		err("Chassis area not created");
		*success_flag = SAHPI_FALSE;
		return SA_OK;
	}

	rv = ov_rest_idr_area_add(area,
			SAHPI_IDR_AREATYPE_CHASSIS_INFO,
			&local_area);

	if (rv != SA_OK) {
		err("Add idr area failed for resource with serialNumber %s",
						serialNumber);
		return rv;
	}
	field_count = 0;
	*success_flag = SAHPI_TRUE;

	/* Add the fields to the newly created chassis area */
	field = local_area->field_list;
	if (partNumber != NULL) {

		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, partNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field partNumber failed for"
				" resource with serialNumber %s", 
					serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	if (serialNumber != NULL) {

		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, serialNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field serialNumber failed for"
				" resource with serialNumber %s",
					serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	local_area->field_list = head_field;
	return SA_OK;
}


/**
 * ov_rest_add_board_area
 *      @area: IDR area pointer
 *      @partNumber: Resource part number
 *      @serialNumber: Resource serialNumber
 *      @success_flag: Flag for checking area creation
 *
 * Purpose:
 *      Creates an IDR board area with required fields
 *
 * Detailed Description:
 *      - Checks whether the part number and serial number details are
 *        available for the resource which has called this module
 *      - If either or both of these information is available then IDR area
 *        of board info type is created and these informations are added as
 *        individual IDR fields in the newly created board area
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_board_area(struct ovRestArea **area,
                        char *partNumber,
                        char *serialNumber,
                        SaHpiInt32T *success_flag)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	struct ovRestArea *local_area = NULL;
	struct ovRestField *field = NULL;
	struct ovRestField *head_field = NULL;
	SaHpiInt32T field_count = 0;

	if (area == NULL || success_flag == NULL) {
		err("Invalid Parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* If both part number and serial number information is NULL
         * then board area is not created
         */
	if ((partNumber == NULL && serialNumber == NULL) &&
			(partNumber[0] == '\0' && serialNumber[0] == '\0')) {
		err("Board Area:Required information not available");
		err("Board area not created");
		*success_flag = SAHPI_FALSE;
		return SA_OK;
	}

	rv = ov_rest_idr_area_add(area,
			SAHPI_IDR_AREATYPE_BOARD_INFO,
			&local_area);
	if (rv != SA_OK) {
		err("Add idr area failed for resource with serialNumber %s",
					serialNumber);
		return rv;
	}
	*success_flag = SAHPI_TRUE;

	field_count = 0;

	/* Add the fields to the newly created product area */
	field = local_area->field_list;
	if (partNumber != NULL && partNumber[0] != '\0') {

		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, partNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field partNumber failed for"
				" resource with serialNumber %s",
					serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	if (serialNumber != NULL && serialNumber[0] != '\0') {
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, serialNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field serialNumber failed for"
				" resource with serialNumber %s",
					serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	local_area->field_list = head_field;
	return SA_OK;
}

/**
 * ov_rest_add_internal_area
 *      @area: IDR area pointer
 *      @manufacturer: Resource manufacturer
 *      @name: Resource name
 *      @partNumber: Resource part number
 *      @serialNumber: Resource serialNumber
 *      @success_flag: Flag for checking area creation
 *
 * Purpose:
 *      Creates an IDR internal area with required fields
 *
 * Detailed Description:
 *      - Checks whether the required details for internal area are
 *        available for the enclosure resource which has called this module
 *      - If any of these information is available then IDR area
 *        of INTERNAL USE type is created and available informations are added
 *        as individual IDR fields in the newly create chassis area
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 */
SaErrorT ov_rest_add_internal_area(struct ovRestArea **area,
                           char *manufacturer,
                           char *name,
                           char *partNumber,
                           char *serialNumber,
                           SaHpiInt32T *success_flag)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field = {0};
	struct ovRestArea *local_area = NULL;
	struct ovRestField *field = NULL;
	struct ovRestField *head_field = NULL;
	SaHpiInt32T field_count = 0;

	if (area == NULL || success_flag == NULL) {
		err("Invalid Parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* If none of the required inventory info is availble, then INTERNAL
         * area is not created
         */
	if (manufacturer == NULL && name == NULL &&
			partNumber == NULL && serialNumber == NULL) {
		err("Internal Area:Required information not available");
		err("Internal area not created");
		*success_flag = SAHPI_FALSE;
		return SA_OK;
	}

	/* Create IDR area of INTERNAL USE type */
	rv = ov_rest_idr_area_add(area,
			(SaHpiIdrAreaTypeT)SAHPI_IDR_AREATYPE_INTERNAL_USE,
			&local_area);
	if (rv != SA_OK) {
		err("Add idr area failed for %s with serialNumber %s",
					name, serialNumber);
		return rv;
	}

	*success_flag = SAHPI_TRUE;
	field_count = 0;
	/* Add the fields to the newly created internal use area */
	field = local_area->field_list;
	if (manufacturer != NULL) {
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER;
		strcpy ((char *)hpi_field.Field.Data, "HPE");

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field manufacturer failed for"
				" %s with serialNumber %s",
					name, serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}

	if (name != NULL) {
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
		strcpy ((char *)hpi_field.Field.Data, name);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field name failed for"
				" %s with serialNumber %s",
					name, serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}

	if (partNumber != NULL) {
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, partNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field partNumber failed for"
				" %s with serialNumber %s",
					name, serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
			head_field = local_area->field_list = field;
		}
		local_area->idr_area_head.NumFields++;
	}

	if (serialNumber != NULL) {
		hpi_field.AreaId = local_area->idr_area_head.AreaId;
		hpi_field.Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
		strcpy ((char *)hpi_field.Field.Data, serialNumber);

		rv = ov_rest_idr_field_add(&(local_area->field_list),
				&hpi_field);
		if (rv != SA_OK) {
			err("Add idr field serialNumber failed for"
				" %s with serialNumber %s",
					name, serialNumber);
			return rv;
		}
		++field_count;
		if (field_count == 1) {
			head_field = field = local_area->field_list;
		}
		local_area->idr_area_head.NumFields++;
	}
	local_area->field_list = head_field;
	return SA_OK;
}

/**
* ov_rest_idr_area_add
*      @head_area: Pointer to IDR area
*      @area_type: Type of IDR area
*      @area: Pointer to new allocated area
*
* Purpose:
*      Adds an Inventory Data Repository(IDR) area to Inventory data 
*      repository and returns the area pointer
*
* Detailed Description:
*      - Creates an IDR area of the specified type
*        If the area list for the resource IDR exists, then the
*        newly created area will be added to end of area list
*        If the area list is empty then the created area will become head node
*        (first area) for the area list
*      - Area id is will start from 1  and will increment for every new area
*        added
*
* Return values:
*      SA_OK - Normal case
*      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
*      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
*/
SaErrorT ov_rest_idr_area_add(struct ovRestArea **head_area,
                      SaHpiIdrAreaTypeT area_type,
                      struct ovRestArea **area)
{
	struct ovRestArea *local_area = NULL;
	SaHpiEntryIdT local_area_id;

	if (head_area == NULL || area == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	local_area = *head_area;
	/* Check whether the area list is empty */
	if (local_area == NULL) {
		local_area = (struct ovRestArea*)
			g_malloc0(sizeof(struct ovRestArea));
		if (!local_area) {
			err("OV REST out of memory");
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* Create the area and make it as head node(first area) in
                 * the area list
                 */
		*head_area = local_area;
		local_area_id = 1;
	} else {
		/* Area list is not empty, traverse to the end of the list
		 * and add the IDR area
                 */
		while (local_area->next_area != NULL) {
			local_area = local_area->next_area;
		}
		local_area->next_area = (struct ovRestArea*)
			g_malloc0(sizeof(struct ovRestArea));
		if (!local_area->next_area) {
			err("OV REST out of memory");
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		local_area_id = local_area->idr_area_head.AreaId + 1;
		local_area = local_area->next_area;
	}
	/* Initialize the area with specified area type and generated area id
         */
	local_area->idr_area_head.AreaId = local_area_id;
	local_area->idr_area_head.Type = area_type;
	local_area->idr_area_head.ReadOnly = SAHPI_FALSE;
	local_area->idr_area_head.NumFields = 0;
	local_area->field_list = NULL;
	local_area->next_area = NULL;

	*area = local_area;
	return SA_OK;
}

/**
 * ov_rest_idr_area_add_by_id:
 *      @head_area: Pointer to IDR area
 *      @area_type: Type of IDR area
 *      @area_id: area id to be added
 *
 * Purpose:
 *      Adds an Inventory Data Repository(IDR) area to Inventory data
 *      repository with the specified area id
 *
 * Detailed Description:
 *      - Creates an IDR area of a specified type with specified id and adds it
 *        to Inventory Data Repository(IDR).
 *        If the area list is empty then the created area will become head node
 *        (first area) for the area list.
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - Input parameters are not valid
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_idr_area_add_by_id(struct ovRestArea **head_area,
                            SaHpiIdrAreaTypeT area_type,
                            SaHpiEntryIdT area_id)
{
        struct ovRestArea *local_area = NULL;
        struct ovRestArea *temp_area = NULL;

        if (head_area == NULL || area_id == SAHPI_LAST_ENTRY) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        temp_area = *head_area;
        local_area = (struct ovRestArea*)g_malloc0(
                      sizeof(struct ovRestArea));
        if (!local_area) {
               err("OV REST out of memory");
               return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        /* Initialize the area with specified area type and ID */
        local_area->idr_area_head.AreaId = area_id;
        local_area->idr_area_head.Type = area_type;
        local_area->idr_area_head.ReadOnly = SAHPI_FALSE;
        local_area->idr_area_head.NumFields = 0;
        local_area->field_list = NULL;

        /* Check whether the area list is empty  or if the new area
         * is to be inserted before first area
         */
        if (*head_area == NULL ||
            (*head_area)->idr_area_head.AreaId > area_id) {
                *head_area = local_area;
                (*head_area)->next_area = temp_area;
        } else {
                /* Traverse through the area list and insert the area
		 * at appropriate place
                 */
                while (temp_area != NULL) {
                        if ((temp_area->idr_area_head.AreaId < area_id) &&
                                ((temp_area->next_area == NULL) ||
                                 (temp_area->next_area->idr_area_head.AreaId >
                                  area_id))) {
                                local_area->next_area = temp_area->next_area;
                                temp_area->next_area = local_area;
                                break;
                        }
                        temp_area = temp_area->next_area;
                }
        }
        return SA_OK;
}


/**
 * ov_rest_idr_field_add
 *      @ov_field: Pointer to field structure
 *      @area_id: Identifier of the area to be deleted
 *      @field_type: Type of IDR field
 *      @str: Field text content
 *      @hpi_field: Pointer to hpi field structure
 *
 * Purpose:
 *      Adds an Inventory Data Repository(IDR) field to Inventory data
 *      repository Area
 *
 * Detailed Description:
 *      - Creates an IDR field of a specified type in an IDR area
 *        If the field list in the IDR area exists, then the
 *        newly created field will be added to end of field list.
 *        If the field list is empty then the created field will become head
 *        node (first field) of the field list
 *      - Field id is will start from 1  and will increment for every new field
 *        added
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT  ov_rest_idr_field_add(struct ovRestField **ov_field,
                        SaHpiIdrFieldT *hpi_field)
{
	SaHpiEntryIdT field_id;
	struct ovRestField *field = NULL;

	if (ov_field == NULL || hpi_field == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	field = *ov_field;
	/* Check whether the field list is empty */
	if (field == NULL) {
		/* Create the area and make it as head node(first area) in
		 * the area list
		 */
		field = (struct ovRestField*)
			g_malloc0(sizeof(struct ovRestField));
		if (! (field)) {
			err("OV REST out of memory");
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		*ov_field = field;
		field_id = 1;
	} else {
		/* Field list is not empty, traverse to the end of the list
		 * and add the IDR field
		 */
		while (field->next_field != NULL) {
			field = field->next_field;
		}
		field->next_field = (struct ovRestField*)
			g_malloc0(sizeof(struct ovRestField));
		if (!(field->next_field)) {
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		field_id = field->field.FieldId + 1;
		field = field->next_field;
	}
	/* Initialize the field of specified field type and generated
	 * field id
	 */
	field->field.AreaId = hpi_field->AreaId;
	field->field.FieldId = field_id;
	field->field.Type = hpi_field->Type;
	field->field.ReadOnly = SAHPI_FALSE;
	hpi_field->ReadOnly = SAHPI_FALSE;
	field->field.Field.DataType = SAHPI_TL_TYPE_TEXT;
	field->field.Field.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace((char *) hpi_field->Field.Data);
	field->field.Field.DataLength =
		strlen ((char *)hpi_field->Field.Data);
	snprintf((char *)field->field.Field.Data,
			field->field.Field.DataLength + 1,
			"%s", hpi_field->Field.Data);

	field->next_field = NULL;
	hpi_field->FieldId = field_id;
	return SA_OK;
}

/**
 * ov_rest_fetch_idr_area_header
 *      @inventory_info: Pointer to rdr private data
 *      @area_id: Identifier of the area to be deleted
 *      @area_type: Type of IDR area
 *      @area_header: Structure to receive area header information
 *      @next_area_id: Next area Id of requested type
 *
 * Purpose:
 *      Gets an Inventory Data Repository(IDR) area header from Inventory data
 *      repository
 *
 * Detailed Description:
 *      - This function allows retrieval of an IDR Area Header by one of
 *        two ways: by AreaId regardless of type or by AreaType and AreaId
 *      - All areas within an IDR can be retrieved by setting area type as
 *        SAHPI_IDR_AREATYPE_UNSPECIFIED, area id set to  SAHPI_FIRST_ENTRY for
 *        the first call. For each subsequent call, the area id will be set to
 *        the value returned in the Next area id parameter,
 *        this will work until next area id becomes SAHPI_LAST_ENTRY
 *      - To retrieve all areas of specific type,  the above step is repeated
 *        with the specified area type
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_fetch_idr_area_header(struct ov_rest_inventory_info 
			*inventory_info, SaHpiEntryIdT area_id,
			SaHpiIdrAreaTypeT area_type,
			SaHpiIdrAreaHeaderT *area_header,
			SaHpiEntryIdT *next_area_id)
{
        SaHpiInt32T i = 0;;
        struct ovRestArea *local_area = NULL;
        SaHpiInt32T found = SAHPI_FALSE;
        SaHpiInt32T area_found = SAHPI_FALSE;

        if (inventory_info == NULL) {
                return SA_ERR_HPI_ERROR;
        }

        if ((area_header == NULL) && (next_area_id == NULL)) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        local_area = inventory_info->area_list;

        /* Check whether area id is set to return all areas
         * (of a particular type if specified)
         */
        if (area_id == SAHPI_FIRST_ENTRY) {
                i = 1;
                /* Traversing the IDR area list to find the area and next area
                 * of specified area type
                 */
                while ((i <= inventory_info->idr_info.NumAreas) &&
                       (local_area != NULL)) {
                        if (area_type == SAHPI_IDR_AREATYPE_UNSPECIFIED ||
                            area_type == local_area->idr_area_head.Type) {
                                area_found = SAHPI_TRUE;
                                memcpy(area_header, &local_area->idr_area_head,
                                       sizeof(SaHpiIdrAreaHeaderT));
                                local_area = local_area->next_area;
                                *next_area_id = SAHPI_LAST_ENTRY;
                                while (local_area) {
                                        if (area_type ==
                                              SAHPI_IDR_AREATYPE_UNSPECIFIED ||
                                            area_type ==
                                             local_area->idr_area_head.Type) {
                                                *next_area_id = local_area->
                                                        idr_area_head.AreaId;
                                                found = SAHPI_TRUE;
                                                break;
                                        }
                                        local_area = local_area->next_area;
                                }
                                break;
                        } else {
                                local_area = local_area->next_area;
                        }
                        i++;
                }
        } else {
                /* Traverse the area list to find area of specified id and
                 * type
                 */
                while (local_area != NULL) {
                        if (found == SAHPI_TRUE) {
                                break;
                        }
                        /* If specified area is present then retrive area 
                         * header along with next area of same type
                         */
                        if ((area_id == local_area->idr_area_head.AreaId)) {
                                if (area_type ==
                                      SAHPI_IDR_AREATYPE_UNSPECIFIED ||
                                    area_type ==
                                      local_area->idr_area_head.Type) {
                                        area_found = SAHPI_TRUE;
                                        memcpy(area_header,
                                               &local_area->idr_area_head,
                                               sizeof(SaHpiIdrAreaHeaderT));
                                        *next_area_id = SAHPI_LAST_ENTRY;
                                        while (local_area->next_area != NULL) {
                                                local_area =
                                                        local_area->next_area;
                                                if (area_type ==
                                                 SAHPI_IDR_AREATYPE_UNSPECIFIED
                                                   || area_type == local_area->
                                                      idr_area_head.Type) {
                                                        *next_area_id =
                                                                local_area->
                                                                idr_area_head.
                                                                AreaId;
                                                        found = SAHPI_TRUE;
                                                        break;
                                                }
                                        }
                                        break;
                                } else {
                                        break;
                                }
                        }
                        local_area = local_area->next_area;
                }
        }
        if (!area_found) {
                return SA_ERR_HPI_NOT_PRESENT;
        }
        return SA_OK;
}

/**
 * idr_field_update
 *      @ov_field: Pointer to field structure
 *      @field: Field structure containing modification information
 *
 * Purpose:
 *      Modifies an Inventory data repository field in Inventory data
 *      repository Area
 *
 * Detailed Description:
 *      - Sets an IDR field of a specified field contents if it exists
 *        else an appropriate error will be returned
 *      - Validation of the field content is not handled in OV REST plug-in
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_idr_field_update(struct ovRestField *ov_field,
                          SaHpiIdrFieldT *field)
{
	if (ov_field == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}

	if (field == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Traverse the field list to find the IDR field with specified id */
	while (ov_field) {

		if (field->FieldId == ov_field->field.FieldId) {
			/* If the specified IDR field is read only, then delete
			 * will be failed and READ ONLY error will be returned
			 **/
			if (ov_field->field.ReadOnly == SAHPI_TRUE) {
				return SA_ERR_HPI_READ_ONLY;
			}
			/* Update the field contents with the new data */
			ov_field->field.Type = field->Type;
			ov_field->field.Field.DataType = field->Field.DataType;
			ov_field->field.Field.Language = field->Field.Language;
			ov_field->field.Field.DataLength =
				field->Field.DataLength;
			snprintf((char *)ov_field->field.Field.Data,
					ov_field->field.Field.DataLength + 1,
					"%s", field->Field.Data);
			return SA_OK;
		} else {
			ov_field = ov_field->next_field;
		}
	}
	return SA_ERR_HPI_NOT_PRESENT;
}


/**
 * ov_rest_fetch_idr_field
 *      @inventory_info: Pointer to rdr private data
 *      @area_id: Identifier of the area to be deleted
 *      @field_type: Type of IDR field
 *      @field_id: Identifier of the IDR field
 *      @next_field_id: Identifier of the next IDR field of the requested type
 *      @field: Pointer to field structure
 *
 * Purpose:
 *      Gets an Inventory Data Repository(IDR) field from Inventory data
 *      repository area
 *
 * Detailed Description:
 *      - This function allows retrieval of an IDR field by one of
 *        two ways: by field id regardless of type or by field type and id
 *      - All fields within an IDR area can be retrieved by setting area type
 *        as SAHPI_IDR_FIELDTYPE_UNSPECIFIED, field id set to SAHPI_FIRST_ENTRY
 *        for the first call. For each subsequent call, the field id will be
 *        set to the value returned in the next field id parameter,
 *        this will work until next field id becomes SAHPI_LAST_ENTRY
 *     -  To retrieve all field of specific type,  the above step is repeated
 *        with the specified field type
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_fetch_idr_field(struct ov_rest_inventory_info *inventory_info,
                         SaHpiEntryIdT area_id,
                         SaHpiIdrFieldTypeT field_type,
                         SaHpiEntryIdT field_id,
                         SaHpiEntryIdT *next_field_id,
                         SaHpiIdrFieldT *field)
{
	SaHpiInt32T i = 0;
	struct ovRestArea *local_area = NULL;
	struct ovRestField *local_field = NULL;
	SaHpiInt32T found = SAHPI_FALSE;
	SaHpiIdrFieldTypeT usf_type = SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
	SaHpiInt32T fieldFound = SAHPI_FALSE;

	if (inventory_info == NULL) {
		err("IDR not present");
		return SA_ERR_HPI_NOT_PRESENT;
	}

	if (field == NULL || next_field_id == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	local_area = inventory_info->area_list;
	while (local_area != NULL) {
		if ((area_id == local_area->idr_area_head.AreaId)) {
			break;
		}
		local_area = local_area->next_area;
	}

	if (!local_area) {
		return SA_ERR_HPI_NOT_PRESENT;
	}

	local_field = local_area->field_list;

	/* Check whether field id is set to return all fields
	 * (of a particular type if specified)
	 */
	if (field_id == SAHPI_FIRST_ENTRY) {
		i = 1;
		while ((i <= local_area->idr_area_head.NumFields) &&
				(local_field != NULL)) {
			if (found == SAHPI_TRUE) {
				break;
			}
			if (field_type == usf_type ||
					field_type == local_field->field.Type){
				fieldFound = SAHPI_TRUE;
				memcpy(field, &local_field->field,
						sizeof(SaHpiIdrFieldT));
				*next_field_id = SAHPI_LAST_ENTRY;
				while (local_field->next_field != NULL) {
					local_field = local_field->next_field;
					if (field_type == usf_type 
						 || field_type ==
						local_field->field.Type) {
						*next_field_id =
							local_field->
							field.FieldId;
						found = SAHPI_TRUE;
						break;
					}
				}
				break;
			} else {
				local_field = local_field->next_field;
			}
			i++;
		}
	} else {
		while (local_field != NULL) {
			if (found == SAHPI_TRUE) {
				break;
			}
			if ((field_id == local_field->field.FieldId)) {
				if (field_type == usf_type ||
					field_type == local_field->field.Type){
					fieldFound = SAHPI_TRUE;
					memcpy(field, &local_field->field,
						sizeof(SaHpiIdrFieldT));
					*next_field_id = SAHPI_LAST_ENTRY;
					while (local_field->next_field !=
							NULL) {
						local_field =
							local_field->next_field;
						if (field_type == usf_type ||
							field_type ==
							local_field->
								field.Type) {
							*next_field_id =
								local_field->
								field.FieldId;
							found = SAHPI_TRUE;
							break;
						}
					}
					break;
				} else {
					break;
				}
			}
			local_field = local_field->next_field;
		}
	}
	if (fieldFound == SAHPI_FALSE) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
	return SA_OK;
}

/**
 * ov_rest_free_inventory_info
 *      @handler:     Handler data pointer
 *      @resource_id: Resource ID
 *
 * Purpose:
 *     To  delete the Areas and Fields present in
 *     Inventory Data Repository(IDR)
 *
 * Detailed Description:
 *     Get the IDR and traverse through each area and deletes the area.
 *     If any error occours while deleting appropriate error is returned
 *
 * Return values:
 *      SA_OK                     - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT    - Requested object not present
 **/
SaErrorT ov_rest_free_inventory_info(struct oh_handler_state *handler,
                             SaHpiResourceIdT resource_id)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_inventory *inventory = NULL;
        SaHpiEntryIdT area_id;
        SaHpiRdrT *rdr = NULL;

        if (handler == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the inventory RDR */
        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR,
                                 SAHPI_DEFAULT_INVENTORY_ID);
        if (rdr == NULL) {
                err("Inventory RDR is not found for resource id %d",
					resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. IdrId=%s", rdr->IdString.Data);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /*Traverse through area list  and delete the area*/
        while (inventory->info.area_list) {
                area_id = inventory->info.area_list->idr_area_head.AreaId;
                rv = ov_rest_idr_area_delete(&(inventory->info.area_list),
                                     area_id);
                if (rv != SA_OK) {
                        err("IDR Area delete failed for resource id %d",
						resource_id);
                        return rv;
                }
        }

        wrap_g_free(inventory->comment);
        return SA_OK;
}

/**
 * ov_rest_idr_area_delete
 *      @head_area: Pointer to IDR area
 *      @area_id: Identifier of the area to be deleted
 *
 * Purpose:
 *      Deletes an Inventory Data Repository(IDR) area from Inventory data
 *      repository
 *
 * Detailed Description:
 *      - Deleted an IDR area of a specified id if it exists in the area
 *        list else an appropriate error will be returned
 *      - If the specified area id exists, then all the IDR fields in IDR
 *        area is deleted first and then the IDR area is deleted
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_idr_area_delete(struct ovRestArea **head_area,
                         SaHpiEntryIdT area_id)
{
        SaErrorT rv = SA_OK;
        struct ovRestArea *local_area = NULL;
        struct ovRestArea *tmp_area = NULL;
        struct ovRestArea *next_area = NULL;
        SaHpiInt32T count = -1;

        if (head_area == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        local_area = *head_area;
        /* If area list is empty, then return error */
        if (local_area == NULL) {
                return SA_ERR_HPI_NOT_PRESENT;
        } else {
                /* Check if specified area id matches with first area id */
                if (area_id == local_area->idr_area_head.AreaId) {
                        /* If the specified IDR area is read only, then delete
                         * will be failed and READ ONLY error will be returned
                         */
                        if (local_area->idr_area_head.ReadOnly == SAHPI_TRUE) {
                                return SA_ERR_HPI_READ_ONLY;
                        }
                        tmp_area = local_area;
                        /* If the specified area id is found,
                         * then traverse the IDR field list of this area and
                         * delete the fields
                         */
                        for (count = 0;
                             count < local_area->idr_area_head.NumFields;
                             count++) {
                                rv = ov_rest_idr_field_delete(&(
					local_area->field_list),
                                        local_area->field_list->field.FieldId);
                                if (rv != SA_OK) {
                                        return rv;
                                }
                        }
                        tmp_area = local_area;
                        local_area = local_area->next_area;
                        *head_area = local_area;
                        wrap_g_free(tmp_area);
                        return SA_OK;
                }
                /* Traverse the area list to find the specified IDR area */
                while (local_area->next_area) {
                        next_area = local_area->next_area;
                        if (area_id == next_area->idr_area_head.AreaId) {
                                if (next_area->idr_area_head.ReadOnly ==
                                    SAHPI_TRUE) {
                                        return SA_ERR_HPI_READ_ONLY;
                                }
                                /* If the specified area id is found, then
                                 * traverse the IDR field list of this area and
                                 * delete the fields
                                 */
                                for (count = 0;
                                     count< next_area->idr_area_head.NumFields;
                                     count++) {
                                        rv = ov_rest_idr_field_delete(
                                                &(next_area->field_list),
                                                next_area->field_list->
                                                        field.FieldId);
                                        if (rv != SA_OK) {
                                                return rv;
                                        }
                                }
                                local_area->next_area = next_area->next_area;
                                wrap_g_free(next_area);
                                return SA_OK;
                       } else {
                               local_area = local_area->next_area;
                       }
                }
        }
        return SA_ERR_HPI_NOT_PRESENT;
}

/**
 * ov_rest_idr_field_add_by_id:
 *      @ov_field: Pointer to field structure
 *      @area_id: Identifier of the area to be added
 *      @field_type: Type of IDR field
 *      @field_data: pointer to field text content
 *      @fied_id: field id to be added
 *
 * Purpose:
 *      Adds an Inventory Data Repository(IDR) field with specified id to
 *      Inventory data repository Area
 *
 * Detailed Description:
 *      - Creates an IDR field of a specified type in an IDR area
 *        Newly created field will be inserted at the proper position
 *        If the field list is empty then the created field will become head
 *        node (first field) for the field list
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - Input parameters are not valid
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_idr_field_add_by_id(struct ovRestField **head_field,
                             SaHpiEntryIdT area_id,
                             SaHpiIdrFieldTypeT field_type,
                             char *field_data,
                             SaHpiEntryIdT field_id)
{
        struct ovRestField *field = NULL;
        struct ovRestField *temp_field = NULL;

        if (head_field == NULL || field_data == NULL ||
            area_id == SAHPI_LAST_ENTRY ||
            field_id == SAHPI_LAST_ENTRY) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        temp_field = *head_field;
        field = (struct ovRestField*)g_malloc0(sizeof(struct ovRestField));
        if (!(field)) {
                err("OV REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Initialize the field with specified field type and ID */
        field->field.AreaId = area_id;
        field->field.FieldId = field_id;
        field->field.Type = field_type;
        field->field.ReadOnly =  SAHPI_FALSE;
        field->field.Field.DataType = SAHPI_TL_TYPE_TEXT;
        field->field.Field.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(field_data);
        field->field.Field.DataLength = strlen (field_data);
        snprintf((char *)field->field.Field.Data,
                 field->field.Field.DataLength + 1,
                 "%s", field_data);

        /* Check whether the field list is empty  or if the new field is
         * to be inserted before first field
         */
        if (*head_field == NULL || (*head_field)->field.FieldId > field_id) {
                *head_field = field;
                (*head_field)->next_field = temp_field;
        } else {
                while (temp_field != NULL) {
                        if ((temp_field->field.FieldId < field_id) &&
                                ((temp_field->next_field == NULL) ||
                                 (temp_field->next_field->field.FieldId >
                                        field_id))) {
                                field->next_field = temp_field->next_field;
                                temp_field->next_field = field;
                                break;
                        }
                        temp_field = temp_field->next_field;
               }
        }
        return SA_OK;
}

/**
 * ov_rest_idr_field_delete
 *      @ov_field: Pointer to field structure
 *      @field_id: Identifier of the IDR field
 *
 * Purpose:
 *      Deletes an Inventory Data Repository(IDR) field from Inventory data
 *      repository Area
 *
 * Detailed Description:
 *      - Deleted an IDR field of a specified id if it exists in the area
 *        list else an appropriate error will be returned
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_idr_field_delete(struct ovRestField **ov_field,
                          SaHpiEntryIdT field_id)
{
	struct ovRestField *field = NULL, *tmp_field = NULL;

	if (ov_field == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	field = *ov_field;
	/* If field list is empty, then return an error */
	if (field == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	} else {
		/* Check whether specified field id matches with first field id
		 */
		if (field_id == field->field.FieldId) {
			tmp_field = field;
			/* If the specified IDR field is read only, then delete
			 * will be failed and READ ONLY error will be returned
			 */
			if (field->field.ReadOnly == SAHPI_TRUE) {
				return SA_ERR_HPI_READ_ONLY;
			}
			field = field->next_field;
			*ov_field = field;
			wrap_g_free(tmp_field);
			return SA_OK;
		}

		/* Traverse the field list to find the specified IDR field */
		while (field->next_field) {
			tmp_field = field->next_field;
			if (field_id == tmp_field->field.FieldId) {
				if (tmp_field->field.ReadOnly == SAHPI_TRUE) {
					return SA_ERR_HPI_READ_ONLY;
				}
				field->next_field = tmp_field->next_field;
				wrap_g_free(tmp_field);
				return SA_OK;
			} else {
				field = field->next_field;
			}
		}
	}
	return SA_ERR_HPI_NOT_PRESENT;
}

/**
 * ov_rest_get_idr_info
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @idr_info: Structure for receiving idr information
 *
 * Purpose:
 *      Gets the Inventory Data Repository(IDR) information
 *
 * Detailed Description:
 *      - Fetches idr info structure from the private data area of
 *        inventory rdr of the specified resource_id
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_get_idr_info(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr,
                              SaHpiIdrInfoT *idr_info)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;

        if (oh_handler == NULL || idr_info == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_INVENTORY_RDR,
                                 idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(idr_info, &inventory->info.idr_info, sizeof(SaHpiIdrInfoT));
        return SA_OK;
}

/**
 * ov_rest_get_idr_area_header
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @area_type: Type of the inventory data area
 *      @area_id: Identifier of the area entry
 *      @next_area_id: Structure for receiving the next area of the requested
 *                     type
 *      @header: Structure for receiving idr information
 *
 * Purpose:
 *      Gets the Inventory Data Repository(IDR) area header information
 *      for a specific area
 *
 * Detailed Description:
 *      - This function gets the inventory data area header information
 *        of a particular area associated with IDR of resource_id
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 **/
SaErrorT ov_rest_get_idr_area_header(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiIdrIdT idr,
                                     SaHpiIdrAreaTypeT area_type,
                                     SaHpiEntryIdT area_id,
                                     SaHpiEntryIdT *next_area_id,
                                     SaHpiIdrAreaHeaderT *header)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        char *type = NULL;

        if (oh_handler == NULL || next_area_id == NULL || header == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check whether area_type supplied is in list of
         * valid area types specified by the framework
         */
        type = oh_lookup_idrareatype(area_type);
        if (type == NULL) {
                err("Invalid area type for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (area_id == SAHPI_LAST_ENTRY) {
                err("Invalid area id for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

         /* Check whether the area list of the resource IDR is empty */
        if (inventory->info.idr_info.NumAreas == 0) {
                err("IDR Area not present for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Fetch the area header of IDR area of the specified area_id.
         * Next area shall contain reference to next area of
         * the same area type if existing, else it will be set to NULL
         */
        rv = ov_rest_fetch_idr_area_header(&(inventory->info), area_id, 
			area_type, header, next_area_id);
        if (rv != SA_OK) {
                err("IDR Area not present for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        return rv;
}

/**
 * ov_rest_add_idr_area
 *      @oh_handler: Handler data pointer.
 *      @resource_id: Resource ID.
 *      @idr: IDR ID.
 *      @area_type: Type of the inventory data area.
 *      @area_id: Area id of the newly created area.
 *
 * Purpose:
 *      Creates the Inventory Data Repository(IDR) area of requested type
 *
 * Detailed Description:
 *      - Creates an IDR area of the specified area type and adds it to end of
 *        area list of the resource IDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_DATA - On Invalid area type
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 *      SA_ERR_HPI_OUT_OF_SPACE - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr,
                              SaHpiIdrAreaTypeT area_type,
                              SaHpiEntryIdT *area_id)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ovRestArea *local_area = NULL;
        struct ov_rest_inventory *inventory = NULL;
        char *type = NULL;

        if (oh_handler == NULL || area_id == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check whether area_type supplied is in list of
         * valid area types specified by the framework
         */
        type = oh_lookup_idrareatype(area_type);
        if (type == NULL) {
                err("Area_type s NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* It is not valid to create the area of UNSPECIFIED type */
        if (area_type == SAHPI_IDR_AREATYPE_UNSPECIFIED) {
                err("Invalid area_type %x for resource id %d",
				area_type, resource_id);
                return SA_ERR_HPI_INVALID_DATA;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the resource IDR is read only */
        if (inventory->info.idr_info.ReadOnly == SAHPI_TRUE) {
                err("IDR is read only for resource id %d", resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Create and add the new area to the end of resource IDR area list */
        rv = ov_rest_idr_area_add(&(inventory->info.area_list), area_type, 
				&local_area);
        if (rv != SA_OK) {
                err("Addition of IDR area failed for resource id %d",
							resource_id);
                if (rv == SA_ERR_HPI_OUT_OF_MEMORY) {
                        return SA_ERR_HPI_OUT_OF_SPACE;
                }
                return rv;
        }

        /* Increment area count in resource IDR  */
        inventory->info.idr_info.NumAreas++;

        /* Increment modification count of resource IDR */
        inventory->info.idr_info.UpdateCount++;

        *area_id = local_area->idr_area_head.AreaId;

        return SA_OK;
}

/**
 * ov_rest_add_idr_area_by_id:
 *      @oh_handler: Handler data pointer.
 *      @resource_id: Resource ID.
 *      @idr: IDR ID.
 *      @area_type: Type of the inventory data area.
 *      @area_id: Area id of the newly created area.
 *
 * Purpose:
 *      Creates the Inventory Data Repository(IDR) area of requested type
 *
 * Detailed Description:
 *      - Creates an IDR area of the specified area type and area id and adds
 *        to area list of the resource IDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY - Resource don't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 *      SA_ERR_HPI_DUPLICATE - Area ID already exists
 *      SA_ERR_HPI_OUT_OF_SPACE - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_idr_area_by_id (void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiIdrIdT idr,
                                     SaHpiIdrAreaTypeT area_type,
                                     SaHpiEntryIdT area_id)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        SaHpiEntryIdT *next_area =  NULL;
        SaHpiIdrAreaHeaderT *area_header = NULL;
        char *type = NULL;

        if (oh_handler == NULL || area_id == SAHPI_LAST_ENTRY) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check whether supplied area_type is in list of
         * valid area types specified by the framework
         */
        type = oh_lookup_idrareatype(area_type);
        if (type == NULL) {
                err("Area_type is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* It is not valid to create the area of UNSPECIFIED type */
        if (area_type == SAHPI_IDR_AREATYPE_UNSPECIFIED) {
                err("Invalid area_type %x for resource id %d",
				area_type, resource_id);
                return SA_ERR_HPI_INVALID_DATA;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache,
                                resource_id,
                                rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the resource IDR is read only */
        if (inventory->info.idr_info.ReadOnly == SAHPI_TRUE) {
                err("IDR is read only for resource id %d", resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Check if the Area ID already exists */
        rv = ov_rest_fetch_idr_area_header(&(inventory->info),
                                   area_id,area_type,
                                   area_header,
                                   next_area);
        if (rv == SA_OK) {
                err("AreaId already exists in the IDR for resource id %d",
							resource_id);
                return SA_ERR_HPI_DUPLICATE;
        }

        /* Create and add the new area to the resource IDR area list */
        rv = ov_rest_idr_area_add_by_id(&(inventory->info.area_list),
                                area_type, area_id);
        if (rv != SA_OK) {
                err("Addition of IDR area failed for resource id %d",
							resource_id);
                if (rv == SA_ERR_HPI_OUT_OF_MEMORY) {
                        return SA_ERR_HPI_OUT_OF_SPACE;
                }
                return rv;
        }

        /* Increment area count in resource IDR  */
        inventory->info.idr_info.NumAreas++;

        /* Increment modification count of resource IDR */
        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}

/**
 * ov_rest_del_idr_area
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @area_id: Area id of the newly created area
 *
 * Purpose:
 *      Deletes Inventory Data Repository(IDR) area with specific identifier
 *
 * Detailed Description:
 *      - Check whether the IDR area of specified area id exists
 *      - If specified IDR area does not exist, then it is deleted from
 *        the area list else an appropriate error is returned
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_del_idr_area(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiIdrIdT idr,
                              SaHpiEntryIdT area_id)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        if (area_id == SAHPI_LAST_ENTRY) {
                err("Invalid area id for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. IdrId=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the resource IDR is read only */
        if (inventory->info.idr_info.ReadOnly == SAHPI_TRUE) {
                err("IDR is read only for resource id %d", resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Delete the specified area if it exists, else return an error */
        rv = ov_rest_idr_area_delete(&(inventory->info.area_list), area_id);
        if (rv != SA_OK) {
                err("IDR Area not found for resource id %d", resource_id);
                return rv;
        }

        /* Decrement area count in resource IDR  */
        inventory->info.idr_info.NumAreas--;

        /* Increment modification count of resource IDR */
        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}

/**
 * ov_rest_get_idr_field
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @area_id: Area id
 *      @field_type: Type of the IDR field
 *      @field_id: Identifier of the field to be retrieved
 *      @next_field_id: Identifier of the next field of the requested type
 *      @field: Structure to retrieve the field contents
 *
 * Purpose:
 *      Gets Inventory Data Repository(IDR) field from a particular IDR area
 *
 * Detailed Description:
 *      - Check whether the IDR field of the specified field id exists
 *      - If specified IDR field exists, then field structure is returned
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_get_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr,
                               SaHpiEntryIdT area_id,
                               SaHpiIdrFieldTypeT field_type,
                               SaHpiEntryIdT field_id,
                               SaHpiEntryIdT *next_field_id,
                               SaHpiIdrFieldT *field)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        char *type = NULL;

        if (oh_handler == NULL || next_field_id == NULL || field == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if ((area_id == SAHPI_LAST_ENTRY) || (field_id == SAHPI_LAST_ENTRY)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

         /* Check whether field_type supplied is in list of
          * valid field types specified by the framework
          */
        type = oh_lookup_idrfieldtype(field_type);
        if (type == NULL) {
                err("Invalid field type %x for resource id %d",
			field_type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_INVENTORY_RDR,
                                 idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the area list of the resource IDR is empty */
        if (inventory->info.idr_info.NumAreas == 0) {
                err("IDR Area not present for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Fetch the IDR field of the specified field_id.
         * next_field shall contain reference to next field of
         * the same field type if is exists, else it will be set to NULL
         */
        rv = ov_rest_fetch_idr_field(&(inventory->info),
                           area_id,
                           field_type,
                           field_id,
                           next_field_id,
                           field);
        if (rv != SA_OK) {
                err("IDR Field not present for resource id %d", resource_id);
                return rv;
        }
        return SA_OK;
}

/**
 * ov_rest_add_idr_field
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @field: structure containing the new field information
 *
 * Purpose:
 *      Add a field to a specified Inventory Data Repository(IDR) Area
 *
 * Detailed Description:
 *      - Creates an IDR field of the specified field type and adds to end of
 *        field list of the specified IDR area id in resource IDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_DATA - On Invalid field type
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 *      SA_ERR_HPI_OUT_OF_SPACE - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr,
                               SaHpiIdrFieldT *field)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        struct ovRestArea *local_area = NULL;
        char *type = NULL;

        if (oh_handler == NULL || field == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        type = oh_lookup_idrfieldtype(field->Type);
        if (type == NULL) {
                err("Invalid field type %x for resource id %d",
				field->Type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* It is not valid to create the field of UNSPECIFIED type */
        if (field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
                err("Invalid field type %x for resource id %d",
				field->Type, resource_id);
                return SA_ERR_HPI_INVALID_DATA;
        }
        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (inventory->info.idr_info.NumAreas == 0) {
                err("No areas in the specified IDR for resource id %d",
						resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Start traversing the area list of resource IDR from the
         * head node
         */
        local_area = inventory->info.area_list;
        while (local_area != NULL) {
                if ((field->AreaId == local_area->idr_area_head.AreaId)) {
                         break;
                }
                local_area = local_area->next_area;
        }

        /* If the area id specified in field structure does exist, then
         * local_area will point to that area, else it is NULL
         */
        if (!local_area) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the specified IDR area is read only */
        if (local_area->idr_area_head.ReadOnly == SAHPI_TRUE) {
                err("IDR Area is read only for resource id %d", resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Create and add the new field to end of filed list in IDR area */
        rv = ov_rest_idr_field_add(&(local_area->field_list),
                           field);
        if (rv != SA_OK) {
                err("IDR field add failed for resource id %d", resource_id);
                if (rv == SA_ERR_HPI_OUT_OF_MEMORY) {
                        return SA_ERR_HPI_OUT_OF_SPACE;
                }
                return rv;
        }

        /* Increment the field count in IDR area */
        local_area->idr_area_head.NumFields++;

        /* Increment the update cound of resource IDR */
        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}

/**
 * ov_rest_add_idr_field_by_id:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @field: structure containing the new field information
 *
 * Purpose:
 *      Add a field to a specified Inventory Data Repository(IDR)
 *      Area with the specified field id
 *
 * Detailed Description:
 *      - Creates an IDR field of the specified field type and adds to
 *        field list of the specified IDR area id in resource IDR
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 *      SA_ERR_HPI_OUT_OF_SPACE - Request failed due to insufficient memory
 **/
SaErrorT ov_rest_add_idr_field_by_id(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiIdrIdT idr_id,
                                     SaHpiIdrFieldT *field)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        struct ovRestArea *local_area = NULL;
        char *type = NULL;

        if (oh_handler == NULL || field == NULL ||
            field->AreaId == SAHPI_LAST_ENTRY ||
            field->FieldId == SAHPI_LAST_ENTRY) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        type = oh_lookup_idrfieldtype(field->Type);
        if (type == NULL) {
                err("Invalid field type %x for resource id %d",
			field->Type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* It is not valid to create the field of UNSPECIFIED type */
        if (field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
                err("Invalid field type %x for resource id %d",
				field->Type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr_id);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache,
                                resource_id,
                                rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (inventory->info.idr_info.NumAreas == 0) {
                err("No areas in the specified IDR for resource id %d",
						resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Start traversing the area list of resource IDR from the
         * head node of the area linked list
         */
        local_area = inventory->info.area_list;
        while (local_area != NULL) {
                if ((field->AreaId == local_area->idr_area_head.AreaId)) {
                        break;
                }
                local_area = local_area->next_area;
        }

        /* If the area id specified in field structure is existing, then
         * local_area will point to that area, else it is NULL
         */
        if (!local_area) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the specified IDR area is read only */
        if (local_area->idr_area_head.ReadOnly == SAHPI_TRUE) {
                err("IDR Area is read only for resource id %d", resource_id);
        }

        /* Create and add the new field to  field list in IDR area */
        rv = ov_rest_idr_field_add_by_id(&(local_area->field_list),
                                 field->AreaId,
                                 field->Type,
                                 (char *)field->Field.Data,
                                 field->FieldId);
        if (rv != SA_OK) {
                err("IDR field add failed for resource id %d", resource_id);
                if (rv == SA_ERR_HPI_OUT_OF_MEMORY) {
                        return SA_ERR_HPI_OUT_OF_SPACE;
                }
                return rv;
        }

        /* Increment the field count in IDR area */
        local_area->idr_area_head.NumFields++;

        /* Increment the update cound of resource IDR */
        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}

/**
 * ov_rest_set_idr_field
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @field: Structure containing the modification field information
 *
 * Purpose:
 *      Updates the specified Inventory Data Repository(IDR) field
 *
 * Detailed Description:
 *      - Updates the field contents of the IDR field of specified
 *        field id from field list of the specified IDR area id in
 *        resource IDR
 *      - If the specified field does not exist, then an appropriate error is
 *        returned
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_set_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr,
                               SaHpiIdrFieldT *field)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        struct ovRestArea *local_area = NULL;
        char *type = NULL;

        if (oh_handler == NULL || field == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check whether the specified field type is in valid list
         * of field types
         */
        type = oh_lookup_idrfieldtype(field->Type);
        if (type == NULL) {
                err("Invalid field type %x for resource id %d",
				field->Type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* It is invalid to specify the field type as
         * SAHPI_IDR_FIELDTYPE_UNSPECIFIED
         */
        if (field->Type == SAHPI_IDR_FIELDTYPE_UNSPECIFIED) {
                err("Invalid field type %x for resource id %d",
			field->Type, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s", rdr->IdString.Data);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (inventory->info.idr_info.NumAreas == 0) {
                err("No areas in the specified IDR for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Start traversing the area list of resource IDR from the
         * head node of the area linked list
         */
        local_area = inventory->info.area_list;
        while (local_area != NULL) {
                if ((field->AreaId == local_area->idr_area_head.AreaId)) {
                         break;
                }
                local_area = local_area->next_area;
        }

        /* If the area id specified in field structure exists, then
         * local_area will point to that area, else it is NULL
         */
        if (!local_area) {
                err("IDR area not present for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Update the specified field with latest data */
        rv = ov_rest_idr_field_update(local_area->field_list, field);
        if (rv != SA_OK) {
                err("IDR field update failed for resource id %d", resource_id);
                return rv;
        }

        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}

/**
 * ov_rest_del_idr_field
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @idr: IDR ID
 *      @area_id: Area id
 *      @field_id: Identifier of the field to be deleted
 *
 * Purpose:
 *      Deletes the Inventory Data Repository(IDR) field with
 *      specific identifier from IDR area
 *
 * Detailed Description:
 *      - Deletes an IDR field of the specified field id from field list
 *        of the specified IDR area id in resource IDR
 *      - If the specified field does not exist, then an appropriate error is
 *        returned
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_CAPABILITY -Resource doesn't have SAHPI_CAPABILITY_INVENTORY
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_NOT_PRESENT - Requested object not present
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_del_idr_field(void *oh_handler,
                               SaHpiResourceIdT resource_id,
                               SaHpiIdrIdT idr,
                               SaHpiEntryIdT area_id,
                               SaHpiEntryIdT field_id)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_inventory *inventory = NULL;
        struct ovRestArea *local_area = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if ((area_id == SAHPI_LAST_ENTRY) || (field_id == SAHPI_LAST_ENTRY)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *)oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (!rpt) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
                err("No INVENTORY_DATA Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_INVENTORY_RDR, idr);
        if (rdr == NULL) {
                err("Failed to get Inventory rdr for resource id %d",
							resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        inventory = (struct ov_rest_inventory *)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (inventory == NULL) {
                err("No inventory data. idr=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (inventory->info.idr_info.NumAreas == 0) {
                err("No areas in the specified IDR for resource id %d",
								resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Start traversing the area list of resource IDR from the
         * head node of the area linked list
         */
        local_area = inventory->info.area_list;
        while (local_area != NULL) {
                if ((area_id == local_area->idr_area_head.AreaId)) {
                         break;
                }
                local_area = local_area->next_area;
        }
        if (!local_area) {
                err("IDR Area not present for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Check whether the specified IDR area is read only */
        if (local_area->idr_area_head.ReadOnly == SAHPI_TRUE) {
                err("IDR area is read only for resource id %d", resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Delete the specified field from the field list*/
        rv = ov_rest_idr_field_delete(&(local_area->field_list),
                              field_id);
        if (rv != SA_OK) {
                return rv;
        }

        /* Decrement the field count in IDR area */
        local_area->idr_area_head.NumFields--;
        inventory->info.idr_info.UpdateCount++;

        return SA_OK;
}


void * oh_get_idr_info(void *,
                       SaHpiResourceIdT,
                       SaHpiIdrIdT,
                       SaHpiIdrInfoT)
                __attribute__ ((weak, alias("ov_rest_get_idr_info")));

void * oh_get_idr_area_header(void *,
                              SaHpiResourceIdT,
                              SaHpiIdrIdT,
                              SaHpiIdrAreaTypeT,
                              SaHpiEntryIdT,
                              SaHpiEntryIdT,
                              SaHpiIdrAreaHeaderT)
                __attribute__ ((weak, alias("ov_rest_get_idr_area_header")));

void * oh_add_idr_area(void *,
                       SaHpiResourceIdT,
                       SaHpiIdrIdT,
                       SaHpiIdrAreaTypeT,
                       SaHpiEntryIdT)
                __attribute__ ((weak, alias("ov_rest_add_idr_area")));

void * oh_add_idr_area_id(void *,
                       SaHpiResourceIdT,
                       SaHpiIdrIdT,
                       SaHpiIdrAreaTypeT,
                       SaHpiEntryIdT)
                __attribute__ ((weak, alias("ov_rest_add_idr_area_by_id")));

void * oh_del_idr_area(void *,
                       SaHpiResourceIdT,
                       SaHpiIdrIdT,
                       SaHpiEntryIdT)
                __attribute__ ((weak, alias("ov_rest_del_idr_area")));

void * oh_get_idr_field(void *,
                        SaHpiResourceIdT,
                        SaHpiIdrIdT,
                        SaHpiEntryIdT,
                        SaHpiIdrFieldTypeT,
                        SaHpiEntryIdT,
                        SaHpiEntryIdT,
                        SaHpiIdrFieldT)
                __attribute__ ((weak, alias("ov_rest_get_idr_field")));


void * oh_add_idr_field(void *,
                        SaHpiResourceIdT,
                        SaHpiIdrIdT,
                        SaHpiIdrFieldT)
                __attribute__ ((weak, alias("ov_rest_add_idr_field")));

void * oh_add_idr_field_id(void *,
                        SaHpiResourceIdT,
                        SaHpiIdrIdT,
                        SaHpiIdrFieldT)
                __attribute__ ((weak, alias("ov_rest_add_idr_field_by_id")));

void * oh_set_idr_field(void *,
                        SaHpiResourceIdT,
                        SaHpiIdrIdT,
                        SaHpiIdrFieldT)
                __attribute__ ((weak, alias("ov_rest_set_idr_field")));

void * oh_del_idr_field(void *,
                        SaHpiResourceIdT,
                        SaHpiIdrIdT,
                        SaHpiEntryIdT,
                        SaHpiEntryIdT)
                __attribute__ ((weak, alias("ov_rest_del_idr_field")));

