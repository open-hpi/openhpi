/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

struct oh_abi_v2 oh_snmp_bc_plugin = {
        .open				= snmp_bc_open,
        .close				= snmp_bc_close,
        .get_event			= snmp_bc_get_event,
        .discover_resources     	= snmp_bc_discover_resources,
        .set_resource_tag               = snmp_bc_set_resource_tag,
        .set_resource_severity          = snmp_bc_set_resource_severity,
        .get_el_info			= snmp_bc_get_sel_info,
        .set_el_time			= snmp_bc_set_sel_time,
        .add_el_entry			= snmp_bc_add_sel_entry,
        .get_el_entry			= snmp_bc_get_sel_entry,
	.clear_el			= snmp_bc_clear_sel,
	.reset_el_overflow		= snmp_bc_sel_overflowreset,
        .get_sensor_reading		= snmp_bc_get_sensor_reading,
        .get_sensor_thresholds		= snmp_bc_get_sensor_thresholds,
        .set_sensor_thresholds		= snmp_bc_set_sensor_thresholds,
	.get_sensor_enable              = snmp_bc_get_sensor_enable,
	.set_sensor_enable              = snmp_bc_set_sensor_enable,
        .get_sensor_event_enables	= snmp_bc_get_sensor_event_enable,
	.set_sensor_event_enables	= snmp_bc_set_sensor_event_enable,
	.get_sensor_event_masks         = snmp_bc_get_sensor_event_masks,
	.set_sensor_event_masks         = snmp_bc_set_sensor_event_masks,
        .get_control_state		= snmp_bc_get_control_state,
        .set_control_state		= snmp_bc_set_control_state,
	.get_idr_info			= snmp_bc_get_idr_info,
	.get_idr_area_header		= snmp_bc_get_idr_area_header,
	.add_idr_area			= snmp_bc_add_idr_area,
	.del_idr_area			= snmp_bc_del_idr_area,
	.get_idr_field			= snmp_bc_get_idr_field,
	.add_idr_field			= snmp_bc_add_idr_field,
	.set_idr_field			= snmp_bc_set_idr_field,
	.del_idr_field			= snmp_bc_del_idr_field,
	.get_watchdog_info 		= snmp_bc_get_watchdog_info,
	.set_watchdog_info 		= snmp_bc_set_watchdog_info,
        .reset_watchdog			= snmp_bc_reset_watchdog,
        .get_hotswap_state		= snmp_bc_get_hotswap_state,
        .set_hotswap_state		= snmp_bc_set_hotswap_state,
        .request_hotswap_action		= snmp_bc_request_hotswap_action,
        .get_power_state		= snmp_bc_get_power_state,
        .set_power_state		= snmp_bc_set_power_state,
        .get_indicator_state		= snmp_bc_get_indicator_state,
        .set_indicator_state		= snmp_bc_set_indicator_state,
        .control_parm			= snmp_bc_control_parm,
        .get_reset_state		= snmp_bc_get_reset_state,
        .set_reset_state		= snmp_bc_set_reset_state
};

/**
 * get_interface:
 * @pp: Location of a pointer to the plugin interface structure.
 * @uuid: Intra-structure's ABI version.
 *
 * Checks that the ABI version supported by the plugin is compatible with that
 * supported by the core infra-structure code.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INTERNAL_ERROR - Infra-structure and plugin ABI versions are incompatible.
 **/
SaErrorT get_interface(void **pp, const uuid_t uuid)
{
        if (uuid_compare(uuid, UUID_OH_ABI_V2) == 0) {
                *pp = &oh_snmp_bc_plugin;
                return(SA_OK);
        }

        *pp = NULL;
	dbg("Incompatable plugin ABI version");
        return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_get_event:
 * @hnd: Handler data pointer.
 * @event: Infra-structure event pointer. 
 *
 * Passes plugin events up to the infra-structure for processing.
 *
 * Return values:
 * 1 - events to be processed.
 * SA_OK - No events to be processed.
 * SA_ERR_HPI_INVALID_PARAMS - @event is NULL.
 **/
SaErrorT snmp_bc_get_event(void *hnd, struct oh_event *event)
{
	SaErrorT err;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!event) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);
	if (err) {
		dbg("Event Log cache build/sync failed. Error=%s", oh_lookup_error(err));
		return(err);
	}

        if (g_slist_length(handle->eventq) > 0) {
                memcpy(event, handle->eventq->data, sizeof(*event));
                free(handle->eventq->data);
                handle->eventq = g_slist_remove_link(handle->eventq, handle->eventq);
                return(1);
        } 

	/* No events for infra-structure to process */
	return(SA_OK);
}

/**
 * snmp_bc_set_resource_tag:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @tag: Pointer to SaHpiTextBufferT.
 *
 * Sets resource's tag.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @tag is NULL or invalid.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory to allocate event.
 **/
SaErrorT snmp_bc_set_resource_tag(void *hnd, SaHpiResourceIdT rid, SaHpiTextBufferT *tag)
{
	SaErrorT err;
        SaHpiRptEntryT *rpt;
        struct oh_event *e;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!oh_valid_textbuffer(tag)) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
                dbg("No RID.");
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

	err = oh_copy_textbuffer(&(rpt->ResourceTag), tag);
	if (err) {
		dbg("Cannot copy textbuffer");
		return(err);
	}

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *rpt;
        handle->eventq = g_slist_append(handle->eventq, e);
        
        return(SA_OK);
}

/**
 * snmp_bc_set_resource_severity:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @tag: Resource's severity.
 *
 * Sets severity of events when resource unexpectedly becomes unavailable.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @sev is invalid.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory to allocate event.
 **/
SaErrorT snmp_bc_set_resource_severity(void *hnd, SaHpiResourceIdT rid, SaHpiSeverityT sev)
{
        SaHpiRptEntryT *rpt;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct oh_event *e;

	if (oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
                dbg("No RID.");
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        rpt->ResourceSeverity = sev;

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *rpt;
        handle->eventq = g_slist_append(handle->eventq, e);

        return(SA_OK);
}

/**
 * snmp_bc_control_parm:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @act: Configuration action.
 *
 * Save and restore saved configuration parameters.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @act is invalid.
 **/
SaErrorT snmp_bc_control_parm(void *hnd, SaHpiResourceIdT rid, SaHpiParmActionT act)
{

	SaHpiRptEntryT *rpt;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (oh_lookup_parmaction(act) == NULL) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	rpt = oh_get_resource_by_id(handle->rptcache, rid);
	if (!rpt) {
                dbg("No RID.");
                return(SA_ERR_HPI_INVALID_RESOURCE);
	}

	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
		dbg("Resource configuration saving not supported.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	else {
		return(SA_ERR_HPI_CAPABILITY);
	}
}


#define snmp_bc_internal_retry()                   \
	if (l_retry >= 2) {                        \
        	custom_handle->handler_retries++;  \
		err = SA_ERR_HPI_BUSY;             \
		break;                             \
	} else {                                   \
		l_retry++;                         \
		continue;                          \
	}


/**
 * snmp_bc_snmp_get:
 * @custom_handle:  Plugin's data pointer.
 * @objid: SNMP OID.
 * @value: Location to store returned SNMP value.
 * @retry: retry is requested on snmp timeout
 *
 * Plugin wrapper for SNMP get call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          const char *objid,
			  struct snmp_value *value,
			  SaHpiBoolT retry)
{
        SaErrorT err;
        struct snmp_session *ss = custom_handle->ss;
	int l_retry;
	
	if (retry) l_retry = 0;
	else l_retry = 2;
	
	do {	
        	err = snmp_get(ss, objid, value);
        	if (err == SA_ERR_HPI_TIMEOUT) {
                	if (custom_handle->handler_retries == SNMP_BC_MAX_SNMP_RETRY_ATTEMPTED) {
                        	custom_handle->handler_retries = 0;
                        	err = SA_ERR_HPI_NO_RESPONSE;
				break;
                	} else {
				trace("HPI_TIMEOUT %s\n", objid);
				snmp_bc_internal_retry();			
			}
        	} else {
                	custom_handle->handler_retries = 0;
			if ((err == SA_OK) && (value->type == ASN_OCTET_STR)) {
				if ( (strncmp(value->string,"(No temperature)", sizeof("(No temperature)")) == 0) ||
			     		(strncmp(value->string,"(No voltage)", sizeof("(No voltage)")) == 0) )
				{
					snmp_bc_internal_retry();
				} else if (strncmp(value->string,"Not Readable!", sizeof("Not Readable!")) == 0) {
                        		custom_handle->handler_retries = 0;
                        		err = SA_ERR_HPI_NO_RESPONSE;
					break;
				} else {
					break;
				} 
			} else break;
	        }               

	} while(l_retry < 3);
	
        return(err);
}


/**
 * snmp_bc_oid_snmp_get:
 * @custom_handle:  Plugin's data pointer.
 * @ep: Entity path of the resource
 * @oidstr: raw SNMP OID.
 * @value: Location to store returned SNMP value.
 * @retry: Retry requested on snmp timeout
 *
 * Plugin wrapper for SNMP get call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_oid_snmp_get(struct snmp_bc_hnd *custom_handle,
				SaHpiEntityPathT *ep, const gchar *oidstr,
				struct snmp_value *value, SaHpiBoolT retry)
{

	SaErrorT rv = SA_OK;
	gchar *oid;
	
	oid = oh_derive_string(ep, oidstr);
	if (oid == NULL) {
		dbg("Cannot derive %s.", oidstr); 
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	rv = snmp_bc_snmp_get(custom_handle, oid, value, retry);
	g_free(oid);
	
	return(rv);
}


/**
 * snmp_bc_snmp_set:
 * @custom_handle:  Plugin's data pointer.
 * @objid: SNMP OID.
 * @value: SNMP value to set.
 *
 * Plugin wrapper for SNMP set call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          char *objid,
			  struct snmp_value value)
{
        SaErrorT err;
	struct snmp_session *ss = custom_handle->ss;

        err = snmp_set(ss, objid, value);
        if (err == SA_ERR_HPI_TIMEOUT) {
                if (custom_handle->handler_retries == SNMP_BC_MAX_SNMP_RETRY_ATTEMPTED) {
                        custom_handle->handler_retries = 0;
                        err = SA_ERR_HPI_NO_RESPONSE;
                } 
		else {
                        custom_handle->handler_retries++;
                        err = SA_ERR_HPI_BUSY;
                }
        } 
	else {
                custom_handle->handler_retries = 0;
        }

        return(err);
}


/**
 * snmp_bc_oid_snmp_set:
 * @custom_handle:  Plugin's data pointer.
 * @ep: Entity path of the resource
 * @oidstr: raw SNMP OID.
 * @value: SNMP value to set.
 *
 * Plugin wrapper for SNMP set call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_oid_snmp_set(struct snmp_bc_hnd *custom_handle,
				SaHpiEntityPathT *ep, const gchar *oidstr,
			  	struct snmp_value value)
{
	SaErrorT rv = SA_OK;
	gchar *oid;

	oid = oh_derive_string(ep , oidstr);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned for %s.", oidstr);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	rv = snmp_bc_snmp_set(custom_handle, oid, value);
	g_free(oid);
	return(rv);
}
