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
 * snmp_bc_get_reset_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @act: Location to store resource's reset action state.
 *
 * Retrieves a resource's reset action state.
 * Always return  SAHPI_RESET_DEASSERT.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_RESET.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_reset_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiResetActionT *act)
{
	if (!hnd || !act) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has reset capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) return(SA_ERR_HPI_CAPABILITY);

	*act = SAHPI_RESET_DEASSERT;

	return(SA_OK);
}

/**
 * snmp_bc_set_reset_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @act: Reset action state to set.
 *
 * Sets a resource's reset action state.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_RESET.
 * SA_ERR_HPI_INVALID_REQUEST - @act invalid or SAHPI_RESET_ASSERT.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_set_reset_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiResetActionT act)
{
	gchar *oid;
	SaErrorT err;
	struct ResourceInfo *resinfo;
        struct snmp_value set_value;

	if (!hnd){
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	if (NULL == oh_lookup_resetaction(act) || act == SAHPI_RESET_ASSERT) {
		dbg("Invalid reset action.");
		return SA_ERR_HPI_INVALID_REQUEST;
	}
	
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has reset capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) return(SA_ERR_HPI_CAPABILITY);

	resinfo = (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, rid);
 	if (resinfo == NULL) {
		dbg("No resource data. Resource=%s", rpt->ResourceTag.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	if (resinfo->mib.OidReset == NULL) {
		dbg("No Reset OID.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	switch (act) {
	case SAHPI_RESET_ASSERT: /* RESET_ASSERT = RESET_DEASSERT Action */
	case SAHPI_RESET_DEASSERT:
		return(SA_ERR_HPI_INVALID_REQUEST);
	case SAHPI_COLD_RESET: /* COLD = WARM Reset Action */
	case SAHPI_WARM_RESET:
		oid = oh_derive_string(&(rpt->ResourceEntity), resinfo->mib.OidReset);
		if (oid == NULL) {
			dbg("NULL SNMP OID returned for %s", resinfo->mib.OidReset);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		
		set_value.type = ASN_INTEGER;
		set_value.str_len = 1;
		set_value.integer = 1;
		
		err = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (err) {
			dbg("SNMP could not set %s; Type=%d.", oid, set_value.type);
			g_free(oid);
			if (err == SA_ERR_HPI_BUSY) return(err);
			else return(SA_ERR_HPI_NO_RESPONSE);
		}
		g_free(oid);
		break;
	default:
		dbg("Invalid Reset Action Type - %d", act);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

        return(SA_OK);
}
