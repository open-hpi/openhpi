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
 *      W. David Ashley<dashley@us.ibm.com>
 */

#include <glib.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <oh_plugin.h>
#include <snmp_util.h>
#include <rpt_utils.h>
#include <epath_utils.h>

#include <rsa_resources.h>
#include <snmp_rsa_control.h>
#include <snmp_rsa_discover.h>
#include <snmp_rsa_sel.h>
#include <snmp_rsa_sensor.h>
#include <snmp_rsa_session.h>
#include <snmp_rsa_watchdog.h>

#include <snmp_rsa.h>

/**
 * snmp_rsa_get_event:
 * @hnd: 
 * @event: 
 * @timeout: 
 *
 * Return value: 
 **/

static int snmp_rsa_get_event(void *hnd, struct oh_event *event, struct timeval *timeout)
{
        return 0;
}


static int snmp_rsa_discover_resources(void *hnd)
{
        SaHpiEntityPathT entity_root;        
        guint i;
        struct oh_event *e;
	struct snmp_value get_value, get_active;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_rsa_hnd *custom_handle = (struct snmp_rsa_hnd *)handle->data;
        RPTable *tmpcache = (RPTable *)g_malloc0(sizeof(RPTable));
        GSList *tmpqueue = NULL;
        char *root_tuple = (char *)g_hash_table_lookup(handle->config,"entity_root");        
                
        memset(&entity_root, 0, sizeof(SaHpiEntityPathT));
        string2entitypath(root_tuple, &entity_root);
        append_root(&entity_root);

        e = snmp_rsa_discover_chassis(&entity_root);
        if(e != NULL) {
                struct ResourceMibInfo *res_mib =
                        g_memdup(&(snmp_rpt_array[rsa_RPT_ENTRY_CHASSIS].mib),
                                 sizeof(struct snmp_rpt));
                oh_add_resource(tmpcache,&(e->u.res_event.entry),res_mib,0);
                tmpqueue = g_slist_append(tmpqueue, e);
                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
//		find_sensors(snmp_rsa_chassis_sensors);                        
//		find_controls(snmp_rsa_chassis_controls);
//		find_inventories(snmp_rsa_chassis_inventories);
        }

        e = snmp_rsa_discover_cpu(&entity_root);
        if(e != NULL) {
                struct ResourceMibInfo *res_mib =
                        g_memdup(&(snmp_rpt_array[rsa_RPT_ENTRY_CPU].mib),
                                 sizeof(struct snmp_rpt));
                oh_add_resource(tmpcache,&(e->u.res_event.entry),res_mib,0);
                tmpqueue = g_slist_append(tmpqueue, e);
                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
//		find_sensors(snmp_rsa_cpu_sensors);                        
//		find_inventories(snmp_rsa_cpu_inventories);
        }

        e = snmp_rsa_discover_dasd(&entity_root);
        if(e != NULL) {
                struct ResourceMibInfo *res_mib =
                        g_memdup(&(snmp_rpt_array[rsa_RPT_ENTRY_DASD].mib),
                                 sizeof(struct snmp_rpt));
                oh_add_resource(tmpcache,&(e->u.res_event.entry),res_mib,0);
                tmpqueue = g_slist_append(tmpqueue, e);
                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
//		find_sensors(snmp_rsa_dasd_sensors);                        
//		find_inventories(snmp_rsa_dasd_inventories);
        }

        /*
        Rediscovery: Get difference between current rptcache and tmpcache. Delete
        obsolete items from rptcache and add new items in.        
        */
        GSList *res_new = NULL, *rdr_new = NULL, *res_gone = NULL, *rdr_gone = NULL;
        GSList *node = NULL;
        
       	rpt_diff(handle->rptcache, tmpcache, &res_new, &rdr_new, &res_gone, &rdr_gone);
	dbg("%d resources have gone away.", g_slist_length(res_gone));
	dbg("%d resources are new or have changed", g_slist_length(res_new));

        for (node = rdr_gone; node != NULL; node = node->next) {
                SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                SaHpiRptEntryT *res = oh_get_resource_by_ep(handle->rptcache, &(rdr->Entity));
                /* Create remove rdr event and add to event queue */
                struct oh_event *e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                if (e) {
                        e->type = OH_ET_RDR_DEL;
                        e->u.rdr_del_event.record_id = rdr->RecordId;
                        e->u.rdr_del_event.parent_entity = rdr->Entity;
                        handle->eventq = g_slist_append(handle->eventq, e);
                } else dbg("Could not allocate more memory to create event.");
                /* Remove rdr from plugin's rpt cache */
                if (rdr && res)
                        oh_remove_rdr(handle->rptcache, res->ResourceId, rdr->RecordId);
                else dbg("No valid resource or rdr at hand. Could not remove rdr.");
                
        }
        g_slist_free(rdr_gone);

        for (node = res_gone; node != NULL; node = node->next) {
                SaHpiRptEntryT *res = (SaHpiRptEntryT *)node->data;
		/* Create remove resource event and add to event queue */
		struct oh_event *e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                if (e) {
                        e->type = OH_ET_RESOURCE_DEL;
                        e->u.res_del_event.resource_id = res->ResourceId;
                        handle->eventq = g_slist_append(handle->eventq, e);
                } else dbg("Could not allocate more memory to create event.");
		/* Remove resource from plugin's rpt cache */
                if (res)
                        oh_remove_resource(handle->rptcache, res->ResourceId);
                else dbg("No valid resource at hand. Could not remove resource.");
        }
        g_slist_free(res_gone);

        for (node = res_new; node != NULL; node = node->next) {
                GSList *tmpnode = NULL;
                SaHpiRptEntryT *res = (SaHpiRptEntryT *)node->data;
                if (!res) {
                        dbg("No valid resource at hand. Could not process new resource.");
                        continue;
                }
                gpointer data = oh_get_resource_data(tmpcache, res->ResourceId);
                oh_add_resource(handle->rptcache, res, g_memdup(data, sizeof(struct snmp_rpt)),0);
                /* Add new/changed resources to the event queue */
                for (tmpnode = tmpqueue; tmpnode != NULL; tmpnode = tmpnode->next) {
                        struct oh_event *e = (struct oh_event *)tmpnode->data;
                        if (e->type == OH_ET_RESOURCE &&
                            e->u.res_event.entry.ResourceId == res->ResourceId) {
                                handle->eventq = g_slist_append(handle->eventq, e);
                                tmpqueue = g_slist_remove_link(tmpqueue, tmpnode);
				g_slist_free_1(tmpnode);
                                break;
                        }
                }
        }
        g_slist_free(res_new);
        
        for (node = rdr_new; node != NULL; node = node->next) {
                guint rdr_data_size = 0;
                GSList *tmpnode = NULL;
                SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                SaHpiRptEntryT *res = oh_get_resource_by_ep(handle->rptcache, &(rdr->Entity));
                if (!res || !rdr) {
                        dbg("No valid resource or rdr at hand. Could not process new rdr.");
                        continue;
                }
                gpointer data = oh_get_rdr_data(tmpcache, res->ResourceId, rdr->RecordId);
                /* Need to figure out the size of the data associated with the rdr */
                if (rdr->RdrType == SAHPI_SENSOR_RDR) rdr_data_size = sizeof(struct SensorMibInfo);
                else if (rdr->RdrType == SAHPI_CTRL_RDR)
                        rdr_data_size = sizeof(struct ControlMibInfo);
                else if (rdr->RdrType == SAHPI_INVENTORY_RDR)
                        rdr_data_size = sizeof(struct InventoryMibInfo);
                oh_add_rdr(handle->rptcache, res->ResourceId, rdr, g_memdup(data, rdr_data_size),0);
                /* Add new/changed rdrs to the event queue */
                for (tmpnode = tmpqueue; tmpnode != NULL; tmpnode = tmpnode->next) {
                        struct oh_event *e = (struct oh_event *)tmpnode->data;
                        if (e->type == OH_ET_RDR &&                            
                            memcmp(&(e->u.rdr_event.rdr.Entity),&(rdr->Entity),sizeof(SaHpiEntityPathT)) == 0 &&
                            e->u.rdr_event.rdr.RecordId == rdr->RecordId) {
                                handle->eventq = g_slist_append(handle->eventq, e);
                                tmpqueue =  g_slist_remove_link(tmpqueue, tmpnode);
				g_slist_free_1(tmpnode);
                                break;
                        }
                }
        }        
        g_slist_free(rdr_new);
        
        /* Clean up tmpqueue and tmpcache */
        g_slist_free(tmpqueue);
        oh_flush_rpt(tmpcache);
        g_free(tmpcache);
        
        return SA_OK;        
}


static int snmp_rsa_set_resource_tag(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag)
{
        return SA_OK;
}


static int snmp_rsa_set_resource_severity(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev)
{
        return SA_OK;
}

/**
 * snmp_rsa_get_self_id:
 * @hnd: 
 * @id: 
 * 
 * Return value: 
 **/

static int snmp_rsa_get_self_id(void *hnd, SaHpiResourceIdT id)
{
        return -1;
}


static int snmp_rsa_get_inventory_info(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,
                                      SaHpiInventoryDataT *data)
{

        return(0);

}


static int snmp_rsa_get_inventory_size(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,  /* yes, they don't call it a
                                                         * num, but it still is one
                                                        */
                                      SaHpiUint32T *size)
{
        return 0;
}


static int snmp_rsa_set_inventory_info(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,
                                      const SaHpiInventoryDataT *data)
{
        return -1;
}


static int snmp_rsa_control_parm(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act)
{
        return -1;
}

struct oh_abi_v2 oh_snmp_rsa_plugin = {
        .open				= snmp_rsa_open,
        .close				= snmp_rsa_close,
        .get_event			= snmp_rsa_get_event,
        .discover_resources     	= snmp_rsa_discover_resources,
        .set_resource_tag               = snmp_rsa_set_resource_tag,
        .set_resource_severity          = snmp_rsa_set_resource_severity,
        .get_self_id			= snmp_rsa_get_self_id,
//      .get_sel_info			= snmp_rsa_get_sel_info,
        .get_sel_info			= NULL,
//      .set_sel_time			= snmp_rsa_set_sel_time,
        .set_sel_time			= NULL,
//      .add_sel_entry			= snmp_rsa_add_sel_entry,
        .add_sel_entry			= NULL,
//      .del_sel_entry			= snmp_rsa_del_sel_entry,
        .del_sel_entry			= NULL,
//      .get_sel_entry			= snmp_rsa_get_sel_entry,
        .get_sel_entry			= NULL,
//      .get_sensor_data		= snmp_rsa_get_sensor_data,
        .get_sensor_data		= NULL,
//      .get_sensor_thresholds		= snmp_rsa_get_sensor_thresholds,
        .get_sensor_thresholds		= NULL,
//      .set_sensor_thresholds		= snmp_rsa_set_sensor_thresholds,
        .set_sensor_thresholds		= NULL,
//      .get_sensor_event_enables	= snmp_rsa_get_sensor_event_enables,
        .get_sensor_event_enables	= NULL,
//      .set_sensor_event_enables	= snmp_rsa_set_sensor_event_enables,
        .set_sensor_event_enables	= NULL,
//      .get_control_state		= snmp_rsa_get_control_state,
        .get_control_state		= NULL,
//      .set_control_state		= snmp_rsa_set_control_state,
        .set_control_state		= NULL,
        .get_inventory_size		= snmp_rsa_get_inventory_size,
        .get_inventory_info		= snmp_rsa_get_inventory_info,
        .set_inventory_info		= snmp_rsa_set_inventory_info,
//      .get_watchdog_info		= snmp_rsa_get_watchdog_info,
        .get_watchdog_info		= NULL,
//      .set_watchdog_info		= snmp_rsa_set_watchdog_info,
        .set_watchdog_info		= NULL,
//      .reset_watchdog			= snmp_rsa_reset_watchdog,
        .reset_watchdog			= NULL,
//      .get_hotswap_state		= snmp_rsa_get_hotswap_state,
        .get_hotswap_state		= NULL,
//      .set_hotswap_state		= snmp_rsa_set_hotswap_state,
        .set_hotswap_state		= NULL,
//      .request_hotswap_action		= snmp_rsa_request_hotswap_action,
        .request_hotswap_action		= NULL,
//      .get_power_state		= snmp_rsa_get_power_state,
        .get_power_state		= NULL,
//      .set_power_state		= snmp_rsa_set_power_state,
        .set_power_state		= NULL,
//      .get_indicator_state		= snmp_rsa_get_indicator_state,
        .get_indicator_state		= NULL,
//      .set_indicator_state		= snmp_rsa_set_indicator_state,
        .set_indicator_state		= NULL,
        .control_parm			= snmp_rsa_control_parm,
//      .get_reset_state		= snmp_rsa_get_reset_state,
        .get_reset_state		= NULL,
//      .set_reset_state		= snmp_rsa_set_reset_state
        .set_reset_state		= NULL
};

int get_interface(void **pp, const uuid_t uuid)
{
        if(uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
                *pp = &oh_snmp_rsa_plugin;
                return 0;
        }

        *pp = NULL;
        return -1;
}
