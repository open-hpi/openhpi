/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

/**
 * snmp_bc_get_hotswap_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Location to store resource's hotswap state.
 *
 * Retrieves a resource's hotswap state.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_MANAGED_HOTSWAP.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_hotswap_state(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiHsStateT *state)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
	SaHpiRptEntryT *rpt;

	if (!hnd || !state) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}


	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has managed hotswap capabilities */
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	snmp_bc_unlock_handler(custom_handle);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_set_hotswap_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Hotswap state to set.
 *
 * Sets a resource's hotswap state.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_MANAGED_HOTSWAP.
 * SA_ERR_HPI_INVALID_REQUEST - @state invalid.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_set_hotswap_state(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiHsStateT state)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
	SaHpiRptEntryT *rpt;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsstate(state)) {
		dbg("Invalid hotswap state.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}


	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has managed hotswap capabilities */
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	snmp_bc_unlock_handler(custom_handle);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_request_hotswap_action:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @act: Hotswap state to set.
 *
 * Sets a resource insertion or extraction action.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_MANAGED_HOTSWAP.
 * SA_ERR_HPI_INVALID_REQUEST - @act invalid.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_request_hotswap_action(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiHsActionT act)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
	SaHpiRptEntryT *rpt;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsaction(act)) {
		dbg("Invalid hotswap action.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}


	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has managed hotswap capabilities */
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	snmp_bc_unlock_handler(custom_handle);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_get_indicator_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Location to store the hotswap indicator state.
 *
 * Gets a resource's hotswap indicator state.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_MANAGED_HOTSWAP.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_indicator_state(void *hnd,
				     SaHpiResourceIdT rid,
				     SaHpiHsIndicatorStateT *state)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
	SaHpiRptEntryT *rpt;

	if (!hnd || !state) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has managed hotswap capabilities */
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	snmp_bc_unlock_handler(custom_handle);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_set_indicator_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Hotswap indicator state to set.
 *
 * Sets a resource's hotswap indicator.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_MANAGED_HOTSWAP.
 * SA_ERR_HPI_INVALID_REQUEST - @state invalid.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_set_indicator_state(void *hnd,
				     SaHpiResourceIdT rid,
				     SaHpiHsIndicatorStateT state)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
	SaHpiRptEntryT *rpt;

	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsindicatorstate(state)) {
		dbg("Invalid hotswap indicator state.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

	handle = (struct oh_handler_state *)hnd;
	custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has managed hotswap capabilities */
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	snmp_bc_unlock_handler(custom_handle);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("snmp_bc_get_hotswap_state")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("snmp_bc_set_hotswap_state")));

void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("snmp_bc_request_hotswap_action")));

void * oh_set_indicator_state (void *, SaHpiResourceIdT, SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("snmp_bc_set_indicator_state")));
		
void * oh_get_indicator_state (void *, SaHpiResourceIdT, SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("snmp_bc_get_indicator_state")));


