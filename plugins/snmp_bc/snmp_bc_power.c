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
 * snmp_bc_get_power_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Location to store resource's power state.
 *
 * Retrieves a resource's power state.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_POWER.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_power_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiPowerStateT *state)
{
	gchar *oid;
	SaErrorT err = SA_OK;
	struct ResourceInfo *resinfo;
        struct snmp_value get_value;

	if (!hnd || !state) {
		dbg("Invalid parameter");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has power capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) return(SA_ERR_HPI_CAPABILITY);

	resinfo = (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, rid);
 	if (resinfo == NULL) {
		dbg("No resource data. Resource=%s", rpt->ResourceTag.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	if (resinfo->mib.OidPowerState == NULL) {
		dbg("No Power OID.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* Read power state of resource */
	oid = oh_derive_string(&(rpt->ResourceEntity), resinfo->mib.OidPowerState);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned for %s", resinfo->mib.OidPowerState);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	err = snmp_bc_snmp_get(custom_handle, oid, &get_value);
	if (!err && (get_value.type == ASN_INTEGER)) {
		switch (get_value.integer) {
		case 0:
			*state = SAHPI_POWER_OFF;
			break;
		case 1:
			*state = SAHPI_POWER_ON;
			break;
		default:
			dbg("Invalid power state for OID=%s.", oid);
		        err = SA_ERR_HPI_INTERNAL_ERROR;
		}
        } else {
		dbg("Cannot read SNMP OID=%s; Type=%d.", oid, get_value.type);
	}

	g_free(oid);
        return(err);
}

/**
 * snmp_bc_set_power_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @state: Resource's power state to set.
 *
 * Sets a resource's power state.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_POWER.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_REQUEST - @state invalid.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_set_power_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiPowerStateT state)
{
	gchar *oid;
	SaErrorT err = SA_OK;
	struct ResourceInfo *resinfo;
        struct snmp_value set_value;

	if (!hnd || NULL == oh_lookup_powerstate(state)) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has power capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) return(SA_ERR_HPI_CAPABILITY);

	resinfo = (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, rid);
 	if (resinfo == NULL) {
		dbg("No resource data. Resource=%s", rpt->ResourceTag.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	if (resinfo->mib.OidPowerOnOff == NULL) {
		dbg("No Power OnOff OID.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* Set power on/off */
	oid = oh_derive_string(&(rpt->ResourceEntity), resinfo->mib.OidPowerOnOff);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned for %s.", resinfo->mib.OidPowerOnOff);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	set_value.type = ASN_INTEGER;
	set_value.str_len = 1;

	switch (state) {
	case SAHPI_POWER_OFF:
		set_value.integer = 0;
		err = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (err) {
			dbg("Cannot set SNMP OID=%s; Type=%d.",
			    resinfo->mib.OidPowerOnOff, set_value.type);
			if (err != SA_ERR_HPI_BUSY) err = SA_ERR_HPI_NO_RESPONSE;
		}
		break;
	case SAHPI_POWER_ON:
		set_value.integer = 1;
		err = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (err) {
			dbg("Cannot set SNMP OID=%s; Type=%d.",
			    resinfo->mib.OidPowerOnOff, set_value.type);
			if (err != SA_ERR_HPI_BUSY) err = SA_ERR_HPI_NO_RESPONSE;
		}
		break;
	case SAHPI_POWER_CYCLE:
	        {
			SaHpiResetActionT act = SAHPI_COLD_RESET;
			err = snmp_bc_set_reset_state(hnd, rid, act);
	        }
		break;
	default:
		dbg("Invalid Power Action Type=%d.", state);
		err = SA_ERR_HPI_INTERNAL_ERROR;
	}

	g_free(oid);
        return(err);
}
