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
 *      W. David Ashley <dashley@us.ibm.com>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

SaErrorT snmp_bc_discover_rsa(struct oh_handler_state *handle,
			      SaHpiEntityPathT *ep_root)
{
	int i;
	SaErrorT err;
        struct oh_event *e;
	struct snmp_value get_value;
	struct ResourceInfo *res_info_ptr;

	if (!handle || !ep_root) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        /******************
	 * Discover chassis
         ******************/
 	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_PLATFORM_OID_RSA, &get_value, SAHPI_TRUE);
        if (err || get_value.type != ASN_INTEGER) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		     SNMP_BC_PLATFORM_OID_RSA, get_value.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CHASSIS].rpt;

	e->u.res_event.entry.ResourceEntity = *ep_root;
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
				   snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CHASSIS].comment,
				   ep_root->Entry[0].EntityLocation);

	trace("Discovered resource=%s.", e->u.res_event.entry.ResourceTag.Data);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CHASSIS].res_info),
				sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(custom_handle->tmpcache, 
			      &(e->u.res_event.entry), 
			      res_info_ptr, 0);
	if (err) {
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);

	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_chassis_sensors_rsa, e);
 	snmp_bc_discover_controls(handle, snmp_bc_chassis_controls_rsa, e);
	snmp_bc_discover_inventories(handle, snmp_bc_chassis_inventories_rsa, e);

        /***************
	 * Discover CPUs
         ***************/
        for (i=0; i<RSA_MAX_CPU; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}
		
		e->type = OH_ET_RESOURCE;
		e->did = oh_get_default_domain_id();
		e->u.res_event.entry = snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CPU].rpt;
		
		oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
		oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
				   SAHPI_ENT_PROCESSOR, i + SNMP_BC_HPI_LOCATION_BASE);

		/* See if CPU exists */
		if (!rdr_exists(custom_handle, &(e->u.res_event.entry.ResourceEntity),
				SNMP_BC_CPU_OID_RSA, 0, 0 )) {
			g_free(e);
			continue;
		}

		e->u.res_event.entry.ResourceId = 
			oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
		snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
					   snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CPU].comment,
					   i + SNMP_BC_HPI_LOCATION_BASE);
		
		trace("Discovered resource=%s.", e->u.res_event.entry.ResourceTag.Data);
		
		/* Create platform-specific info space to add to infra-structure */
		res_info_ptr = g_memdup(&(snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_CPU].res_info),
					sizeof(struct ResourceInfo));
		if (!res_info_ptr) {
			dbg("Out of memory.");
			g_free(e);
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;
		
		/* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);
		
		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(custom_handle->tmpcache,
				      &(e->u.res_event.entry), 
				      res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
		
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_cpu_sensors_rsa, e);
		snmp_bc_discover_controls(handle, snmp_bc_cpu_controls_rsa, e);
		snmp_bc_discover_inventories(handle, snmp_bc_cpu_inventories_rsa, e);
	}

        /****************
	 * Discover DASDs
         ****************/
        for (i=0; i<RSA_MAX_DASD; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}
		
		e->type = OH_ET_RESOURCE;
		e->did = oh_get_default_domain_id();
		e->u.res_event.entry = snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_DASD].rpt;
		
		oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
		oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
				   SAHPI_ENT_DISK_BAY, i + SNMP_BC_HPI_LOCATION_BASE);

		/* See if DASD exists */
		if (!rdr_exists(custom_handle, &(e->u.res_event.entry.ResourceEntity),
				SNMP_BC_DASD_OID_RSA, 0, 0 )) {
			g_free(e);
			continue;
		}

		e->u.res_event.entry.ResourceId = 
			oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
		snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
					   snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_DASD].comment,
					   i + SNMP_BC_HPI_LOCATION_BASE);
		
		trace("Discovered resource=%s.", e->u.res_event.entry.ResourceTag.Data);
		
		/* Create platform-specific info space to add to infra-structure */
		res_info_ptr = g_memdup(&(snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_DASD].res_info),
					sizeof(struct ResourceInfo));
		if (!res_info_ptr) {
			dbg("Out of memory.");
			g_free(e);
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;
		
		/* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);
		
		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(custom_handle->tmpcache,
				      &(e->u.res_event.entry), 
				      res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
		
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_dasd_sensors_rsa, e);
		snmp_bc_discover_controls(handle, snmp_bc_dasd_controls_rsa, e);
		snmp_bc_discover_inventories(handle, snmp_bc_dasd_inventories_rsa, e);
	}

        /***************
	 * Discover Fans
         ***************/
        for (i=0; i<RSA_MAX_FAN; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}
		
		e->type = OH_ET_RESOURCE;
		e->did = oh_get_default_domain_id();
		e->u.res_event.entry = snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_FAN].rpt;
		
		oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
		oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
				   SAHPI_ENT_FAN, i + SNMP_BC_HPI_LOCATION_BASE);

		/* See if fan exists */
		if (!rdr_exists(custom_handle, &(e->u.res_event.entry.ResourceEntity),
				SNMP_BC_FAN_OID_RSA, 0, 0 )) {
			g_free(e);
			continue;
		}

		e->u.res_event.entry.ResourceId = 
			oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
		snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
					   snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_FAN].comment,
					   i + SNMP_BC_HPI_LOCATION_BASE);
		
		trace("Discovered resource=%s.", e->u.res_event.entry.ResourceTag.Data);
		
		/* Create platform-specific info space to add to infra-structure */
		res_info_ptr = g_memdup(&(snmp_bc_rpt_array_rsa[RSA_RPT_ENTRY_FAN].res_info),
					sizeof(struct ResourceInfo));
		if (!res_info_ptr) {
			dbg("Out of memory.");
			g_free(e);
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;
		
		/* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);
		
		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(custom_handle->tmpcache,
				      &(e->u.res_event.entry), 
				      res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
		
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_fan_sensors_rsa, e);
		snmp_bc_discover_controls(handle, snmp_bc_fan_controls_rsa, e);
		snmp_bc_discover_inventories(handle, snmp_bc_fan_inventories_rsa, e);
	}

  return(SA_OK);
}
