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
 * Retrieves a sensor's value and/or state. Both @data and @state
 * may be NULL, in which case this function can be used to test for
 * sensor presence.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_REQUEST - Sensor is disabled.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_reset_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiResetActionT *act)
{

	if (!hnd || !act){
		dbg("Missing handle\n");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	SaHpiRptEntryT *res = oh_get_resource_by_id(handle->rptcache, rid);
	if(res == NULL) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

        struct ResourceInfo *s =
                (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, rid);
	if(s == NULL) {
		return SA_ERR_HPI_INVALID_CMD;
	}
	if(s->mib.OidReset == NULL) { 
		return SA_ERR_HPI_INVALID_CMD;
	}

	*act = SAHPI_RESET_DEASSERT;

	return SA_OK;
}

SaErrorT snmp_bc_set_reset_state(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiResetActionT act)
{
	if (!hnd){
		dbg("Missing handle\n");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	gchar *oid;
	SaErrorT status;
        struct snmp_value set_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRptEntryT *res = oh_get_resource_by_id(handle->rptcache, rid);
	if(res == NULL) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
        struct ResourceInfo *s =
                (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, rid);
	if(s == NULL) {
		return SA_ERR_HPI_INVALID_CMD;
	}
	if(s->mib.OidReset == NULL) { 
		return SA_ERR_HPI_INVALID_CMD;
	}

	switch (act) {
	case SAHPI_RESET_ASSERT: /* RESET_ASSERT = RESET_DEASSERT Action */
	case SAHPI_RESET_DEASSERT:
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_COLD_RESET: /* COLD = WARM Reset Action */
	case SAHPI_WARM_RESET:
		oid = oh_derive_string(&(res->ResourceEntity), s->mib.OidReset);
		if(oid == NULL) {
			dbg("NULL SNMP OID returned for %s\n",s->mib.OidReset);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
		
		set_value.type = ASN_INTEGER;
		set_value.str_len = 1;
		set_value.integer = 1;
		
		status = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (status != SA_OK) {
			dbg("SNMP could not set %s; Type=%d.\n",oid,set_value.type);
			g_free(oid);
			if (status == SA_ERR_HPI_BUSY) return status;
			else return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);
		break;
	default:
		dbg("Invalid Reset Action Type - %d\n", act);
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        return SA_OK;
}
