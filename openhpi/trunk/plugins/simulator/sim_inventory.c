/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      peter d phan  <dashley@us.ibm.com>
 *
 */

#include <sim_init.h>


SaErrorT sim_get_idr_info(void *hnd,
	                  SaHpiResourceIdT        rid,
		          SaHpiIdrIdT             IdrId,
		          SaHpiIdrInfoT          *IdrInfo)
{
	if (!hnd || !IdrInfo) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_get_idr_area_header(void *hnd,
		                 SaHpiResourceIdT         rid,
		                 SaHpiIdrIdT              IdrId,
		                 SaHpiIdrAreaTypeT        AreaType,
		                 SaHpiEntryIdT            AreaId,
		                 SaHpiEntryIdT           *NextAreaId,
		                 SaHpiIdrAreaHeaderT     *Header)
{
	if (!hnd || !NextAreaId || !Header) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_add_idr_area(void *hnd,
		          SaHpiResourceIdT         rid,
		          SaHpiIdrIdT              IdrId,
		          SaHpiIdrAreaTypeT        AreaType,
		          SaHpiEntryIdT           *AreaId)

{
	if (!hnd || !AreaId) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_del_idr_area(void *hnd,
		          SaHpiResourceIdT       rid,
		          SaHpiIdrIdT            IdrId,
		          SaHpiEntryIdT          AreaId)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_get_idr_field(void *hnd,
		           SaHpiResourceIdT       rid,
		           SaHpiIdrIdT            IdrId,
		           SaHpiEntryIdT          AreaId,
		           SaHpiIdrFieldTypeT     FieldType,
		           SaHpiEntryIdT          FieldId,
		           SaHpiEntryIdT          *NextFieldId,
		           SaHpiIdrFieldT         *Field)
{
	if (!hnd || !NextFieldId || !Field) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_add_idr_field(void *hnd,
		           SaHpiResourceIdT         rid,
		           SaHpiIdrIdT              IdrId,
		           SaHpiIdrFieldT           *Field)
{
	if (!hnd || !Field) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_set_idr_field(void *hnd,
		           SaHpiResourceIdT         rid,
		           SaHpiIdrIdT              IdrId,
		           SaHpiIdrFieldT           *Field)
{
	if (!hnd || !Field) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_del_idr_field(void *hnd,
		           SaHpiResourceIdT         rid,
		           SaHpiIdrIdT              IdrId,
		           SaHpiEntryIdT            AreaId,
		           SaHpiEntryIdT            FieldId)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has inventory capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Inventory is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


void * oh_get_idr_info (void *hnd, SaHpiResourceIdT, SaHpiIdrIdT,SaHpiIdrInfoT)
                __attribute__ ((weak, alias("sim_get_idr_info")));

void * oh_get_idr_area_header (void *, SaHpiResourceIdT, SaHpiIdrIdT,
                                SaHpiIdrAreaTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                                SaHpiIdrAreaHeaderT)
                __attribute__ ((weak, alias("sim_get_idr_area_header")));

void * oh_add_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT)
                __attribute__ ((weak, alias("sim_add_idr_area")));

void * oh_del_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT)
                __attribute__ ((weak, alias("sim_del_idr_field")));

void * oh_get_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldT)
                __attribute__ ((weak, alias("sim_get_idr_field")));

void * oh_add_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
                __attribute__ ((weak, alias("sim_add_idr_field")));

void * oh_set_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
                __attribute__ ((weak, alias("sim_set_idr_field")));

void * oh_del_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiEntryIdT)
                __attribute__ ((weak, alias("sim_del_idr_field")));


