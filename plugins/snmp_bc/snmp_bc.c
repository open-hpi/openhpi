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

#include <glib.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <oh_plugin.h>
#include <snmp_util.h>
#include <rpt_utils.h>
#include <epath_utils.h>
#include <bc_resources.h>
#include <snmp_bc_control.h>
#include <snmp_bc_discover.h>
#include <snmp_bc_hotswap.h>
#include <snmp_bc_sel.h>
#include <snmp_bc_sensor.h>
#include <snmp_bc_session.h>
#include <snmp_bc_watchdog.h>
#include <snmp_bc_utils.h>
#include <snmp_bc_event.h>
#include <snmp_bc_inventory.h>

#include <snmp_bc.h>

/**
 * snmp_bc_get_event:
 * @hnd: 
 * @event: 
 * @timeout: 
 *
 * Return value: 
 **/

static int snmp_bc_get_event(void *hnd, struct oh_event *event, struct timeval *timeout)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        
        if(g_slist_length(handle->eventq)>0) {
                memcpy(event, handle->eventq->data, sizeof(*event));
                free(handle->eventq->data);
                handle->eventq = g_slist_remove_link(handle->eventq, handle->eventq);
                return 1;
        } else {
                return 0;
	}
}

#define find_sensors(rdr_array) \
do { \
        int j; \
	for(j=0; rdr_array[j].bc_sensor_info.mib.oid != NULL; j++) { \
		e = snmp_bc_discover_sensors(custom_handle->ss, \
                                             parent_ep, \
                                             &rdr_array[j]); \
                if(e != NULL) { \
                        struct BC_SensorInfo *bc_data = g_memdup(&(rdr_array[j].bc_sensor_info), \
                                                                 sizeof(struct BC_SensorInfo)); \
                        oh_add_rdr(tmpcache,rid,&(e->u.rdr_event.rdr), bc_data, 0); \
                        tmpqueue = g_slist_append(tmpqueue, e); \
                } \
        } \
} while(0)

#define find_controls(rdr_array) \
do { \
        int j; \
        for(j=0; rdr_array[j].bc_control_info.mib.oid != NULL; j++) { \
	        e = snmp_bc_discover_controls(custom_handle->ss, \
                                              parent_ep, \
                                              &rdr_array[j]); \
                if(e != NULL) { \
                        struct BC_ControlInfo *bc_data = g_memdup(&(rdr_array[j].bc_control_info), \
                                                                  sizeof(struct BC_ControlInfo)); \
                        oh_add_rdr(tmpcache,rid,&(e->u.rdr_event.rdr), bc_data, 0); \
                        tmpqueue = g_slist_append(tmpqueue, e); \
                } \
        } \
} while(0)

#define find_inventories(rdr_array) \
do { \
        int j; \
        for (j=0; rdr_array[j].bc_inventory_info.mib.oid.OidManufacturer != NULL; j++) { \
                e = snmp_bc_discover_inventories(custom_handle->ss, \
                                                 parent_ep, \
                                                 &rdr_array[j]); \
                if(e != NULL) { \
                        struct BC_InventoryInfo *bc_data = g_memdup(&(rdr_array[j].bc_inventory_info), \
                                                                    sizeof(struct BC_InventoryInfo)); \
                        oh_add_rdr(tmpcache,rid,&(e->u.rdr_event.rdr), bc_data, 0); \
                        tmpqueue = g_slist_append(tmpqueue, e); \
                } \
        } \
} while(0)

static int snmp_bc_discover_resources(void *hnd)
{
        SaHpiEntityPathT entity_root;        
        guint i;
        struct oh_event *e;
	struct snmp_value get_value, get_active;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
        RPTable *tmpcache = (RPTable *)g_malloc0(sizeof(RPTable));
        GSList *tmpqueue = NULL;
        char *root_tuple = (char *)g_hash_table_lookup(handle->config,"entity_root");        
                
        memset(&entity_root, 0, sizeof(SaHpiEntityPathT));
        string2entitypath(root_tuple, &entity_root);
        append_root(&entity_root);

	/* Blade vector gives us info for chassis, blades, and blade daughter (add in) cards */
        if((snmp_get(custom_handle->ss,SNMP_BC_BLADE_VECTOR,&get_value)) == 0 &&
	   (get_value.type == ASN_OCTET_STR)) {
                e = snmp_bc_discover_chassis(get_value.string,&entity_root);
                if(e != NULL) {
                        struct BC_ResourceInfo *bc_data =
                                g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].bc_res_info),
                                         sizeof(struct BC_ResourceInfo));
                        oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
                        tmpqueue = g_slist_append(tmpqueue, e);
                        SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                        SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
			find_sensors(snmp_bc_chassis_sensors);
			find_controls(snmp_bc_chassis_controls);
			find_inventories(snmp_bc_chassis_inventories); 
                }

                for(i=0; i < strlen(get_value.string); i++) {
                        e = snmp_bc_discover_blade(get_value.string, &entity_root, i);
                        if(e != NULL) {
                                struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_BLADE].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                                oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
                                tmpqueue = g_slist_append(tmpqueue, e);
                                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
				find_sensors(snmp_bc_blade_sensors);
				find_controls(snmp_bc_blade_controls);
				find_inventories(snmp_bc_blade_inventories);
                        }
			
			e = snmp_bc_discover_blade_addin(custom_handle->ss, get_value.string, &entity_root, i);
			if(e != NULL) {
                                struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                                oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
				tmpqueue = g_slist_append(tmpqueue, e);
                                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
				find_sensors(snmp_bc_blade_addin_sensors);
				find_controls(snmp_bc_blade_addin_controls);
				find_inventories(snmp_bc_blade_addin_inventories);
			}
                }
        } else {
                /* If we get here, something is hosed. No need to do more discovery */
                dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_BLADE_VECTOR,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
        }
        
        if((snmp_get(custom_handle->ss,SNMP_BC_FAN_VECTOR,&get_value)) == 0 &&
	   (get_value.type == ASN_OCTET_STR)) {
                for(i=0; i < strlen(get_value.string); i++) {
                        e = snmp_bc_discover_fan(get_value.string, &entity_root, i);
                        if(e != NULL) {
                                struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                                oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
                                tmpqueue = g_slist_append(tmpqueue, e);
                                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
				find_sensors(snmp_bc_fan_sensors);
				find_controls(snmp_bc_fan_controls);
				find_inventories(snmp_bc_fan_inventories); 
                        }
                }
        } else {
		dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_FAN_VECTOR,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
        }

        if((snmp_get(custom_handle->ss,SNMP_BC_POWER_VECTOR,&get_value)) == 0 &&
	   (get_value.type == ASN_OCTET_STR)) {
                for(i=0; i < strlen(get_value.string); i++) {
                        e = snmp_bc_discover_power(get_value.string, &entity_root, i);
                        if(e != NULL) {
                                struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                                oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
                                tmpqueue = g_slist_append(tmpqueue, e);
                                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
                                find_sensors(snmp_bc_power_sensors);
				find_controls(snmp_bc_power_controls);
				find_inventories(snmp_bc_power_inventories);
                        }
                }
        } else {
		dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_POWER_VECTOR,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
        }

        if((snmp_get(custom_handle->ss,SNMP_BC_SWITCH_VECTOR,&get_value)) == 0 &&
	   (get_value.type == ASN_OCTET_STR)) {
                for(i=0; i < strlen(get_value.string); i++) {
                        e = snmp_bc_discover_switch(get_value.string, &entity_root, i);
                        if(e != NULL) {
                                struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                                oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
                                tmpqueue = g_slist_append(tmpqueue, e);
                                SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;
				find_sensors(snmp_bc_switch_sensors);
				find_controls(snmp_bc_switch_controls);
				find_inventories(snmp_bc_switch_inventories);
                         }
                }
        } else {
		dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_SWITCH_VECTOR,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
        }

        if((snmp_get(custom_handle->ss,SNMP_BC_MEDIATRAY_EXISTS,&get_value)) == 0 &&
	   (get_value.type == ASN_INTEGER)) {
		e = snmp_bc_discover_mediatray(get_value.integer, &entity_root, 0);
		if(e != NULL) {
                        struct BC_ResourceInfo *bc_data =
                                        g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].bc_res_info),
                                                 sizeof(struct BC_ResourceInfo));
                        oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
			tmpqueue = g_slist_append(tmpqueue, e);
                        SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                        SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;                        
			find_sensors(snmp_bc_mediatray_sensors);
			find_controls(snmp_bc_mediatray_controls);
			find_inventories(snmp_bc_mediatray_inventories);
		}
        } else {
		dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_MEDIATRAY_EXISTS,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
        }

        if((snmp_get(custom_handle->ss,SNMP_BC_MGMNT_VECTOR,&get_value)) == 0 && 
	   (get_value.type == ASN_OCTET_STR)) {
                for(i=0; i < strlen(get_value.string); i++) {
			if((snmp_get(custom_handle->ss,SNMP_BC_MGMNT_ACTIVE,&get_active)) == 0 &&
			    (get_active.type == ASN_INTEGER) && (get_active.integer == i+1)) { 	
				e = snmp_bc_discover_mgmnt(get_value.string, &entity_root, i);
				if(e != NULL) {
                                        struct BC_ResourceInfo *bc_data =
                                            g_memdup(&(snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].bc_res_info),
                                                     sizeof(struct BC_ResourceInfo));
                                        oh_add_resource(tmpcache,&(e->u.res_event.entry),bc_data,0);
					tmpqueue = g_slist_append(tmpqueue, e);
                                        SaHpiResourceIdT rid = e->u.res_event.entry.ResourceId;
                                        SaHpiEntityPathT parent_ep = e->u.res_event.entry.ResourceEntity;                                        
					find_sensors(snmp_bc_mgmnt_sensors);
					find_controls(snmp_bc_mgmnt_controls);
					find_inventories(snmp_bc_mgmnt_inventories); 
				}
			}
		}
	} else {
		dbg("Couldn't fetch SNMP %s vector; Type=%d\n",SNMP_BC_MGMNT_VECTOR,get_value.type);
                dbg("There is either no chassis or the management modules are down.");
                g_free(tmpcache);
                return -1;
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
                if (rdr->RdrType == SAHPI_SENSOR_RDR) rdr_data_size = sizeof(struct BC_SensorInfo);
                else if (rdr->RdrType == SAHPI_CTRL_RDR)
                        rdr_data_size = sizeof(struct BC_ControlInfo);
                else if (rdr->RdrType == SAHPI_INVENTORY_RDR)
                        rdr_data_size = sizeof(struct BC_InventoryInfo);
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
        
        return SA_OK;
}

static int snmp_bc_set_resource_severity(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRptEntryT *resource = oh_get_resource_by_id(handle->rptcache, id);
        struct oh_event *e = NULL;

        if (!resource) {
                dbg("Error. Cannot set resource severity in plugin. No resource found by that id.");
                return SA_ERR_HPI_NOT_PRESENT;
        }

        resource->ResourceSeverity = sev;

        /** Can we persist this to disk? */

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *resource;
        
        handle->eventq = g_slist_append(handle->eventq, e);

        return SA_OK;
}

/**
 * snmp_bc_get_self_id:
 * @hnd: 
 * @id: 
 * 
 * Return value: 
 **/

static int snmp_bc_get_self_id(void *hnd, SaHpiResourceIdT id)
{
        return -1;
}

static int snmp_bc_get_inventory_info(  void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiEirIdT num,
                                        SaHpiInventoryDataT *data)
{
        SaHpiInventDataRecordTypeT l_recordtype;
        SaErrorT return_status = SA_OK;
        SaHpiInventGeneralDataT *working;
        SaHpiInventoryDataT *l_data;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_INVENTORY_RDR, num);

        l_data = data;
        l_data->Validity =  SAHPI_INVENT_DATA_INVALID;
        l_data->DataRecords[0] = (SaHpiInventDataRecordT *)l_data + sizeof(SaHpiInventoryDataT) +
                                        sizeof(l_data->DataRecords[0])+  sizeof(SaHpiInventDataRecordT);
        l_data->DataRecords[1] = NULL;

        if(rdr != NULL) {

                struct BC_InventoryInfo *s =
                        (struct BC_InventoryInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
                l_recordtype = s->mib.inventory_type;

                 switch (l_recordtype) {
                         case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
                                dbg("Not yet implemented SAHPI_INVENT_RECTYPE_INTERNAL_USE\n");
                                l_data->DataRecords[0]->DataLength = 0;
                                return_status = SA_ERR_HPI_UNKNOWN;
                                break;
                        case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
                        case SAHPI_INVENT_RECTYPE_BOARD_INFO:
                                working = (SaHpiInventGeneralDataT *)&l_data->DataRecords[0]->RecordData.ProductInfo;
                                get_inventory_data(hnd, rdr, s, l_data, working, &l_data->DataRecords[0]->DataLength);
                                l_data->Validity =  SAHPI_INVENT_DATA_VALID;
                                return_status = SA_OK;
                                break;
                        case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
                                working = (SaHpiInventGeneralDataT *)&l_data->DataRecords[0]->RecordData.ChassisInfo.GeneralData;
                                get_inventory_data(hnd, rdr, s, l_data, working, &l_data->DataRecords[0]->DataLength);
                                l_data->DataRecords[0]->RecordData.ChassisInfo.Type = s->mib.chassis_type;
                                l_data->Validity =  SAHPI_INVENT_DATA_VALID;
                                return_status = SA_OK;
                                break;
                        case SAHPI_INVENT_RECTYPE_OEM:
                                dbg("Not yet implemented SAHPI_INVENT_RECTYPE_OEM\n");
                                l_data->DataRecords[0]->DataLength = 0;
                                return_status = SA_ERR_HPI_UNKNOWN;
                                break;
                        default:
                                dbg("Invalid inventory type %x in RDR\n", l_recordtype);
                                l_data->DataRecords[0]->DataLength = 0;
                                return_status = SA_ERR_HPI_UNKNOWN;
                                break;
                    }

                l_data->DataRecords[0]->RecordType = l_recordtype;

                /* FIXME:: Do we need to add events as well? */

        } else {
                dbg("No Inventory RDR found for ResourceId %x, EirId %x\n", id, num);
                return_status = SA_ERR_HPI_NOT_PRESENT;
        }

        return(return_status);

}

static int snmp_bc_get_inventory_size(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,  /* yes, they don't call it a
                                                         * num, but it still is one
                                                        */
                                      SaHpiUint32T *size)
{
        SaErrorT rc, return_status = SA_OK;
        SaHpiInventoryDataT *l_inventdata;
        l_inventdata =g_malloc(1024 *  sizeof(guint));
        if (l_inventdata != NULL) {
                rc = snmp_bc_get_inventory_info(hnd, id, num, (SaHpiInventoryDataT *)l_inventdata);
               *size = (l_inventdata->DataRecords[0]->DataLength +
                          sizeof(SaHpiInventoryDataT) +
                          sizeof(l_inventdata->DataRecords[0]) +
                           sizeof(SaHpiInventDataRecordT));

                g_free(l_inventdata);
                return_status = SA_OK;
        } else {
                dbg("No space available ");
                return_status = SA_ERR_HPI_OUT_OF_SPACE ;
        }

        return return_status;
}


static int snmp_bc_set_inventory_info(void *hnd, SaHpiResourceIdT id,
                                      SaHpiEirIdT num,
                                      const SaHpiInventoryDataT *data)
{
        return -1;
}

static int snmp_bc_control_parm(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act)
{
        return -1;
}

struct oh_abi_v2 oh_snmp_bc_plugin = {
        .open				= snmp_bc_open,
        .close				= snmp_bc_close,
        .get_event			= snmp_bc_get_event,
        .discover_resources     	= snmp_bc_discover_resources,
        .set_resource_tag               = snmp_bc_set_resource_tag,
        .set_resource_severity          = snmp_bc_set_resource_severity,
        .get_self_id			= snmp_bc_get_self_id,
        .get_sel_info			= snmp_bc_get_sel_info,
        .set_sel_time			= snmp_bc_set_sel_time,
        .add_sel_entry			= snmp_bc_add_sel_entry,
        .del_sel_entry			= snmp_bc_del_sel_entry,
        .get_sel_entry			= snmp_bc_get_sel_entry,
	.clear_sel			= snmp_bc_clear_sel,
        .get_sensor_data		= snmp_bc_get_sensor_data,
        .get_sensor_thresholds		= snmp_bc_get_sensor_thresholds,
        .set_sensor_thresholds		= snmp_bc_set_sensor_thresholds,
        .get_sensor_event_enables	= snmp_bc_get_sensor_event_enables,
        .set_sensor_event_enables	= snmp_bc_set_sensor_event_enables,
        .get_control_state		= snmp_bc_get_control_state,
        .set_control_state		= snmp_bc_set_control_state,
        .get_inventory_size		= snmp_bc_get_inventory_size,
        .get_inventory_info		= snmp_bc_get_inventory_info,
        .set_inventory_info		= snmp_bc_set_inventory_info,
        .get_watchdog_info		= snmp_bc_get_watchdog_info,
        .set_watchdog_info		= snmp_bc_set_watchdog_info,
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

int get_interface(void **pp, const uuid_t uuid)
{
        if(uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
                *pp = &oh_snmp_bc_plugin;
                return 0;
        }

        *pp = NULL;
        return -1;
}
