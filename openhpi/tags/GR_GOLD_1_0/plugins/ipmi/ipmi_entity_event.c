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

static void entity_update_rpt(RPTable *table, SaHpiResourceIdT rid, int present)
{
	SaHpiRdrT  *rdr;

	rdr = oh_get_rdr_next(table, rid, SAHPI_FIRST_ENTRY);
	while (rdr) {
		if(rdr->RdrType == SAHPI_SENSOR_RDR) {
			if (present) {
				dbg("present");
				rdr->RdrTypeUnion.SensorRec.Ignore = SAHPI_FALSE;
			}else {
				dbg("not present");
				rdr->RdrTypeUnion.SensorRec.Ignore = SAHPI_TRUE;
			}
		}
		rdr = oh_get_rdr_next(table, rid, rdr->RecordId);
	}
}

static void entity_presence(ipmi_entity_t	*entity,
			    int			present,
			    void		*cb_data,
			    ipmi_event_t	*event)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)cb_data;

	SaHpiRptEntryT  *rpt;
	SaHpiResourceIdT rid;
	ipmi_entity_id_t ent_id;

	ent_id = ipmi_entity_convert_to_id(entity);

	rpt = ohoi_get_resource_by_entityid(handler->rptcache, &ent_id);
	if (!rpt) {
		dbg("No rpt");
		return;
	}
	rid = rpt->ResourceId;
	dbg("%s %s",ipmi_entity_get_entity_id_string(entity), present?"present":"not present");
	entity_update_rpt(handler->rptcache, rid, present);
#if 0
/*
	The following code is masked by Racing. The reasons are listed as:

	1. The hotswap event is not populated to HPI
	2. The param 3 is changed to handler
           in ipmi_entity_set_presence_handler(entity, entity_presence, handler);
	3. The orignal param (&entry.ResourceID) is a bug. Because entity_presence 
	is callback function and entry.ResourceID is local variable, it may cause
	segment fault
*/
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
#endif	
}

static void get_entity_event(ipmi_entity_t	*entity,
			     SaHpiRptEntryT	*entry, void *cb_data)
{
	SaHpiEntityPathT entity_ep;

	struct ohoi_handler *ipmi_handler = cb_data;

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
	/* AdvancedTCA fixe-up */
	if (ipmi_entity_get_entity_instance(entity) >= 96)
			entry->ResourceEntity.Entry[0].EntityInstance -= 96;
	
	entry->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
	//entry->ResourceEntity.Entry[1].EntityInstance = 0;
	entry->ResourceEntity.Entry[1].EntityInstance = ipmi_entity_get_slave_address(entity);
	
	/* let's append entity_root from config */

	string2entitypath(ipmi_handler->entity_root, &entity_ep);

	ep_concat(&entry->ResourceEntity, &entity_ep);

	entry->EntryId = 0;
	entry->ResourceId = oh_uid_from_entity_path(&entry->ResourceEntity);

	entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;
	
	if (ipmi_entity_hot_swappable(entity)) {
			entry->ResourceCapabilities |= SAHPI_CAPABILITY_MANAGED_HOTSWAP
					| SAHPI_CAPABILITY_FRU;
	}

	if(ipmi_entity_get_is_fru(entity)) {
			entry->ResourceCapabilities |= SAHPI_CAPABILITY_FRU | SAHPI_CAPABILITY_INVENTORY_DATA;
	}
				
	entry->ResourceSeverity = SAHPI_OK;
	entry->DomainId = 0;
	entry->ResourceTag.DataType = SAHPI_TL_TYPE_ASCII6;
	
	entry->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	entry->ResourceTag.DataLength = (SaHpiUint32T) ipmi_entity_get_id_length(entity); 

	ipmi_entity_get_id(entity, entry->ResourceTag.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	if ((strlen(entry->ResourceTag.Data) == 0) || (!strcmp(entry->ResourceTag.Data, "invalid"))) {
			char *str;
			str = ipmi_entity_get_entity_id_string(entity);
			memcpy(entry->ResourceTag.Data, str, strlen(str) + 1);
	} 
}

static void add_entity_event(ipmi_entity_t	        *entity,
			     struct oh_handler_state    *handler)
{
		struct ohoi_resource_info *ohoi_res_info;
		SaHpiRptEntryT	entry /*, *old_entry */;
		int 		rv;

		dbg("adding ipmi entity: %s", ipmi_entity_get_entity_id_string(entity));

		struct ohoi_handler *ipmi_handler = handler->data;

		ohoi_res_info = g_malloc0(sizeof(*ohoi_res_info));

		if (!ohoi_res_info) {
				dbg("Out of memory");
				return;
		}

		ohoi_res_info->type       = OHOI_RESOURCE_ENTITY; 
		ohoi_res_info->u.entity_id= ipmi_entity_convert_to_id(entity);

		get_entity_event(entity, &entry, ipmi_handler);	

		/* bug #957513 keeping for histirical reasons until it's verified */
		//handler->eventq = g_slist_append(handler->eventq, e);

		oh_add_resource(handler->rptcache, &entry, ohoi_res_info, 1);

		/* sensors */

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

		rv = ipmi_entity_set_presence_handler(entity, entity_presence, handler);
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
		int rv;

		if (op == IPMI_ADDED) {
				add_entity_event(entity, handler);

				dbg("Entity added: %d.%d", 
								ipmi_entity_get_entity_id(entity), 
								ipmi_entity_get_entity_instance(entity));

				ipmi_entity_add_hot_swap_handler(entity, ohoi_hot_swap_cb, cb_data);
		
		} else if (op == IPMI_DELETED) {
				/* we need to remove_entity */

				dbg("Entity deleted: %d.%d", 
								ipmi_entity_get_entity_id(entity), 
								ipmi_entity_get_entity_instance(entity));
		
		} else if (op == IPMI_CHANGED) {
				add_entity_event(entity, handler);

				dbg("Entity changed: %d.%d",
								ipmi_entity_get_entity_id(entity), 
								ipmi_entity_get_entity_instance(entity));
		} else {
				dbg("Entity: Unknown change?!");
		}

		/* inventory (a.k.a FRU) */

		rv = ipmi_entity_set_fru_update_handler(entity, ohoi_inventory_event, handler);

		if (rv) {
                dbg("ipmi_entity_set_fru_update_handler: %#x", rv);
                return;
        }

}
