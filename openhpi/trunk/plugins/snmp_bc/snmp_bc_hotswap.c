/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
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
	if (!hnd || !state) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
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
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsstate(state)) {
		dbg("Invalid hotswap state.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
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
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsaction(act)) {
		dbg("Invalid hotswap action.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
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
	if (!hnd || !state) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
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
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsindicatorstate(state)) {
		dbg("Invalid hotswap indicator state.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}
