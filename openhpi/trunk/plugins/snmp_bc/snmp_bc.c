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

static SaErrorT snmp_bc_get_event(void *hnd, struct oh_event *event);
static SaErrorT snmp_bc_set_resource_tag(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag);
static SaErrorT snmp_bc_set_resource_severity(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev);
static SaErrorT snmp_bc_control_parm(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act);

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
        .get_sensor_reading		= snmp_bc_get_sensor_reading,
        .get_sensor_thresholds		= snmp_bc_get_sensor_thresholds,
        .set_sensor_thresholds		= snmp_bc_set_sensor_thresholds,
        .get_sensor_event_enables	= snmp_bc_get_sensor_event_enables,
#if 0
	.set_sensor_event_enables	= snmp_bc_set_sensor_event_enables,
#endif
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

SaErrorT get_interface(void **pp, const uuid_t uuid)
{
        if (uuid_compare(uuid, UUID_OH_ABI_V2) == 0) {
                *pp = &oh_snmp_bc_plugin;
                return(SA_OK);
        }

        *pp = NULL;
        return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_get_event:
 * @hnd: 
 * @event: 
 * @timeout: 
 *
 * Return values:
 **/

static SaErrorT snmp_bc_get_event(void *hnd, struct oh_event *event)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);

        if (g_slist_length(handle->eventq) > 0) {
                memcpy(event, handle->eventq->data, sizeof(*event));
                free(handle->eventq->data);
                handle->eventq = g_slist_remove_link(handle->eventq, handle->eventq);
                return 1; /* FIXME:: dbg message; what should error return be?? */
        } else {
                return(SA_OK);
	}
}

static int snmp_bc_set_resource_tag(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRptEntryT *resource = oh_get_resource_by_id(handle->rptcache, id);
        struct oh_event *e = NULL;
        guint datalength = tag->DataLength;

        if (!resource) {
                dbg("Error. Cannot set resource tag in plugin. No resource found by that id.");
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (datalength > SAHPI_MAX_TEXT_BUFFER_LENGTH) datalength = SAHPI_MAX_TEXT_BUFFER_LENGTH;

        strncpy(resource->ResourceTag.Data, tag->Data, datalength);
        resource->ResourceTag.DataLength = tag->DataLength;
        resource->ResourceTag.DataType = tag->DataType;
        resource->ResourceTag.Language = tag->Language;

        /** Can we persist this to hardware or disk? */

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *resource;

        handle->eventq = g_slist_append(handle->eventq, e);
        
        return(SA_OK);
}

static SaErrorT snmp_bc_set_resource_severity(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRptEntryT *resource = oh_get_resource_by_id(handle->rptcache, id);
        struct oh_event *e = NULL;

        if (!resource) {
                dbg("No RID.");
                return(SA_ERR_HPI_NOT_PRESENT);
        }

        resource->ResourceSeverity = sev;

        /** Can we persist this to disk? */

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *resource;
        
        handle->eventq = g_slist_append(handle->eventq, e);

        return(SA_OK);
}

static SaErrorT snmp_bc_control_parm(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	SaHpiRptEntryT *res = oh_get_resource_by_id(handle->rptcache, id);
	
	if (res->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
		dbg("Resource configuration saving not supported.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	else {
		return(SA_ERR_HPI_INVALID_CMD);
	}
}

SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          const char *objid,
                          struct snmp_value *value)
{
        SaErrorT status;
        struct snmp_session *ss = custom_handle->ss;
	
        status = snmp_get(ss, objid, value);
        if (status == SA_ERR_SNMP_TIMEOUT) {

                if (custom_handle->handler_retries == MAX_RETRY_ATTEMPTED) {
                        custom_handle->handler_retries = 0;
                        status = SA_ERR_HPI_NO_RESPONSE;
                } else {
                        custom_handle->handler_retries++;
                        status = SA_ERR_HPI_BUSY;
                }

        } else {
                custom_handle->handler_retries = 0;
        }

        return status;

}

SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          char *objid,
                          struct snmp_value value)
{
        SaErrorT status;
	struct snmp_session *ss = custom_handle->ss;

        status = snmp_set(ss, objid, value);
        if (status == SA_ERR_SNMP_TIMEOUT) {

                if (custom_handle->handler_retries == MAX_RETRY_ATTEMPTED) {
                        custom_handle->handler_retries = 0;
                        status = SA_ERR_HPI_NO_RESPONSE;
                } else {
                        custom_handle->handler_retries++;
                        status = SA_ERR_HPI_BUSY;
                }

        } else {
                custom_handle->handler_retries = 0;
        }

        return status;
}
