/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include "ipmi.h"
#include <epath_utils.h>
#include <uid_utils.h>
#include <string.h>


static void entity_presence(ipmi_entity_t	*entity,
			    int			present,
			    void		*cb_data,
			    ipmi_event_t	*event)
{
	struct oh_event	*e;
	SaHpiResourceIdT *rid = cb_data;

	e = malloc(sizeof(*e));
	if (!e) {
		dbg("Out of space");
		return;
	}

	memset(e, 0, sizeof(*e));
	e->type = OH_ET_HPI;
	e->u.hpi_event.parent = *rid;
	e->u.hpi_event.id = 0;

	e->u.hpi_event.event.Source = 0;
	/* Do not find EventType in IPMI */
	e->u.hpi_event.event.EventType = SAHPI_ET_HOTSWAP;

	/* FIXIT! */
	if (event)
		e->u.hpi_event.event.Timestamp 
                        = ipmi_get_uint32(ipmi_event_get_data_ptr(event));

	/* Do not find the severity of hotswap event */
	e->u.hpi_event.event.Severity = SAHPI_MAJOR;

	if (present)
		e->u.hpi_event.event.EventDataUnion.HotSwapEvent.HotSwapState
			= SAHPI_HS_STATE_ACTIVE_HEALTHY;
	else
		e->u.hpi_event.event.EventDataUnion.HotSwapEvent.HotSwapState
			= SAHPI_HS_STATE_NOT_PRESENT;
	
}

static void get_entity_event(ipmi_entity_t	*entity,
			     SaHpiRptEntryT	*entry)
{
	char	*str;
	SaHpiEntityPathT entity_ep;

	entry->ResourceInfo.ResourceRev = 0;
	entry->ResourceInfo.SpecificVer = 0;
	entry->ResourceInfo.DeviceSupport = 0;
	entry->ResourceInfo.ManufacturerId = 0;
	entry->ResourceInfo.ProductId = 0;
	entry->ResourceInfo.FirmwareMajorRev = 0;
	entry->ResourceInfo.FirmwareMinorRev = 0;
	
	entry->ResourceInfo.AuxFirmwareRev = 0;
	entry->ResourceEntity.Entry[0].EntityType 
                = ipmi_entity_get_entity_id(entity);
	entry->ResourceEntity.Entry[0].EntityInstance 
                = ipmi_entity_get_entity_instance(entity);
	entry->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
	entry->ResourceEntity.Entry[1].EntityInstance = 0;
	
	/* let's append entity_root from config */

	string2entitypath(entity_root, &entity_ep);

	ep_concat(&entry->ResourceEntity, &entity_ep);

	entry->EntryId = 0;
	entry->ResourceId = oh_uid_from_entity_path(&entry->ResourceEntity);

	entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;
	entry->ResourceSeverity = SAHPI_OK;
	entry->DomainId = 0;
	entry->ResourceTag.DataType = SAHPI_TL_TYPE_ASCII6;
	
	str = ipmi_entity_get_entity_id_string(entity);
	
	entry->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	entry->ResourceTag.DataLength = strlen(str); 

	memcpy(entry->ResourceTag.Data, str, strlen(str) + 1);
}

static void add_entity_event(ipmi_entity_t	        *entity,
			     struct oh_handler_state    *handler)
{
         struct ohoi_resource_info *ohoi_res_info;
         struct oh_event	*e;
         int 		rv;
        
        ohoi_res_info = g_malloc0(sizeof(*ohoi_res_info));
        if (!ohoi_res_info) {
                dbg("Out of memory");
                return;
        }	
	e = g_malloc0(sizeof(*e));
	if (!e) {
                free(ohoi_res_info);
		dbg("Out of space");   
		return;
	}

        ohoi_res_info->type       = OHOI_RESOURCE_ENTITY; 
        ohoi_res_info->u.entity_id= ipmi_entity_convert_to_id(entity);
        
	memset(e, 0, sizeof(*e));
	e->type = OH_ET_RESOURCE;			
	get_entity_event(entity, &(e->u.res_event.entry));	

	handler->eventq = g_slist_append(handler->eventq, e);

	/* sensors */
	oh_add_resource(handler->rptcache, &(e->u.res_event.entry), ohoi_res_info, 1);
	rv= ipmi_entity_set_sensor_update_handler(entity, ohoi_sensor_event,
						  handler);
	if (rv) {
		dbg("ipmi_entity_set_sensor_update_handler: %#x", rv);
		return;
	}
	
	/* controls */
	rv = ipmi_entity_set_control_update_handler(entity, ohoi_control_event,
							handler);
	if (rv) {
		dbg("ipmi_entity_set_control_update_handler: %#x", rv);
		return;
	}

        /* inventory (a.k.a FRU) */
        rv = ipmi_entity_set_fru_update_handler(entity, ohoi_inventory_event, handler);

        if (rv) {
                dbg("ipmi_entity_set_fru_update_handler: %#x", rv);
                return;
        }
        
	/* entity presence overall */
	rv = ipmi_entity_set_presence_handler(entity, entity_presence, &e->u.res_event.entry.ResourceId);
	if (rv) {
		dbg("ipmi_entity_set_presence_handler: %#x", rv);
		return;
	}

}

void ohoi_entity_event(enum ipmi_update_e       op,
                       ipmi_domain_t            *domain,
                       ipmi_entity_t            *entity,
                       void                     *cb_data)
{
	struct oh_handler_state *handler = cb_data;

	if (op == IPMI_ADDED) {
		add_entity_event(entity, handler);
        } else if (op == IPMI_DELETED) {
		dbg("Entity deleted: %d.%d", 
                    ipmi_entity_get_entity_id(entity), 
                    ipmi_entity_get_entity_instance(entity));
        } else if (op == IPMI_CHANGED) {
		dbg("Entity changed: %d.%d",
                    ipmi_entity_get_entity_id(entity), 
                    ipmi_entity_get_entity_instance(entity));
        } else {
                dbg("Entity: Unknow change?!");
        }

}

