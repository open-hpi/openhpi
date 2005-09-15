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
#include<oh_domain.h>
#include <oh_utils.h>
#include <string.h>




static void trace_ipmi_entity(char *str, int inst, ipmi_entity_t *entity)
{
	if (!getenv("OHOI_TRACE_ENTITY") && !IHOI_TRACE_ALL) {
		return;
	}

	char *type;
	
	switch (ipmi_entity_get_type(entity)) {
	case IPMI_ENTITY_UNKNOWN:
		type = "UNKNOWN"; break;
	case IPMI_ENTITY_MC:
		type = "MC"; break;
	case IPMI_ENTITY_FRU:
		type = "FRU"; break;
	case IPMI_ENTITY_GENERIC:
		type = "GENERIC"; break;
	case IPMI_ENTITY_EAR:
		type = "EAR"; break;
	case IPMI_ENTITY_DREAR:
		type = "DREAR"; break;
	default :
		type = "INVALID"; break;
	}

	fprintf(stderr, "*** Entity %s %s: %d.%d(%d).%d.%d (%s)\n", type, str,
		ipmi_entity_get_entity_id(entity), 
		inst,
		ipmi_entity_get_entity_instance(entity),
		ipmi_entity_get_device_channel(entity),
		ipmi_entity_get_device_address(entity),
		ipmi_entity_get_entity_id_string(entity));
}


void entity_rpt_set_updated(struct ohoi_resource_info *res_info,
		struct ohoi_handler *ipmi_handler)
{
	g_static_rec_mutex_lock(&ipmi_handler->ohoih_lock);
	if (!res_info->presence) {
		g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);
		return;
	}	
	res_info->updated = 1;
	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);
}

void entity_rpt_set_presence(struct ohoi_resource_info *res_info,
		struct ohoi_handler *ipmi_handler, int present)
{
	g_static_rec_mutex_lock(&ipmi_handler->ohoih_lock);	
	trace_ipmi("res_info %p: old presence %d, new presence %d",
		res_info, res_info->presence, present);
	if (present == res_info->presence) {
		g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);	
		return;
	}
	res_info->presence =  present;
	res_info->updated = 1;
	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);
}



int entity_presence(ipmi_entity_t		*entity,
			    int			present,
			    void		*cb_data,
			    ipmi_event_t	*event)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)cb_data;

	SaHpiRptEntryT  *rpt;
	SaHpiResourceIdT rid;
	ipmi_entity_id_t ent_id;
  	struct ohoi_resource_info	*res_info;
	
	ent_id = ipmi_entity_convert_to_id(entity);

	rpt = ohoi_get_resource_by_entityid(handler->rptcache, &ent_id);
	if (!rpt) {
		dbg("No rpt");
		return SA_ERR_HPI_NOT_PRESENT;
	}
	rid = rpt->ResourceId;
	res_info = oh_get_resource_data(handler->rptcache, rid);;
	trace_ipmi("%s(%d)  %s",ipmi_entity_get_entity_id_string(entity),
		rid, present ? "present" : "not present");
	entity_rpt_set_presence(res_info, handler->data,  present);
	return SA_OK;
}


static void append_parent_epath(SaHpiRptEntryT	*entry, SaHpiRptEntryT	*parent)
{
	const SaHpiEntityPathT *ep = &(parent->ResourceEntity);
	oh_concat_ep(&entry->ResourceEntity, ep);
}


static void init_rpt(SaHpiRptEntryT	*entry)
{
	int i;
	
	entry->ResourceInfo.ResourceRev = 0;
	entry->ResourceInfo.SpecificVer = 0;
	entry->ResourceInfo.DeviceSupport = 0;
	entry->ResourceInfo.ManufacturerId = 0;
	entry->ResourceInfo.ProductId = 0;
	entry->ResourceInfo.FirmwareMajorRev = 0;
	entry->ResourceInfo.FirmwareMinorRev = 0;	
	entry->ResourceInfo.AuxFirmwareRev = 0;

	entry->EntryId = 0;
	entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;	
	entry->HotSwapCapabilities = 0;
	
	for (i=0;i<15;i++) {
		entry->ResourceInfo.Guid[i] = 0;
	}
	entry->ResourceSeverity = SAHPI_MAJOR;	/* Default Value -- not related to IPMI */
	entry->ResourceFailed = SAHPI_FALSE;
	oh_init_textbuffer(&entry->ResourceTag);
}


static void update_resource_capabilities(ipmi_entity_t	*entity,
					 SaHpiRptEntryT	*entry)
{
	if (ipmi_entity_supports_managed_hot_swap(entity)) {
		trace_ipmi("Entity is hot swapable");
		entry->ResourceCapabilities |= SAHPI_CAPABILITY_MANAGED_HOTSWAP;
		/* if entity supports managed hot swap
		 * check if it has indicator */

		/* we need only return value from function */
		if (!ipmi_entity_get_hot_swap_indicator(entity, NULL, NULL)) {
			trace_ipmi("setting SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED");
			entry->HotSwapCapabilities |= SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED;
		}
	}

	/* OpenIPMI used ipmi_entity_hot_swappable to indicate it's FRU
	 * do not use ipmi_entity_get_is_fru()
	 * it's used only for checking if entity has FRU data
	 */
	if(ipmi_entity_hot_swappable(entity)) {
		trace_ipmi("Entity supports simplified hotswap");
		entry->ResourceCapabilities |= SAHPI_CAPABILITY_FRU;
	}
}

struct add_parent_ep_s {
	struct oh_handler_state *handler;
	SaHpiRptEntryT	*entry;
};

static void add_parent_ep(ipmi_entity_t *ent, ipmi_entity_t *parent, void *cb_data)
{
	struct add_parent_ep_s *info = cb_data;
	ipmi_entity_id_t parent_id = ipmi_entity_convert_to_id(parent);
	SaHpiRptEntryT *pr_rpt;


	pr_rpt = ohoi_get_resource_by_entityid(info->handler->rptcache, &parent_id);
	if (pr_rpt == NULL) {
		dbg("               Couldn't find out res-info for parent: %d.%d.%d.%d %s",
			ipmi_entity_get_entity_id(parent),
			ipmi_entity_get_entity_instance(parent),
			ipmi_entity_get_device_channel(parent),
			ipmi_entity_get_device_address(parent),
			ipmi_entity_get_entity_id_string(parent));
		trace_ipmi_entity("CAN NOT FIND OUT PARENT. NO RES_INFO",
			0, parent);
		
		return;
	}
	append_parent_epath(info->entry, pr_rpt);
}


static void get_entity_event(ipmi_domain_t            *domain,
			     ipmi_entity_t	*entity,
			     struct ohoi_resource_info *ohoi_res_info,
			     SaHpiRptEntryT	*entry,
			     void *cb_data)
{
	SaHpiEntityPathT entity_ep;
	struct oh_handler_state *handler = cb_data;
	struct ohoi_handler *ipmi_handler = handler->data;
	int rv;
	const char *str;


	init_rpt(entry);

	entry->ResourceEntity.Entry[0].EntityType 
                = ipmi_entity_get_entity_id(entity);
	if(ipmi_entity_get_entity_instance(entity) >= 96) {
		entry->ResourceEntity.Entry[0].EntityLocation 
                	= ipmi_entity_get_entity_instance(entity)- 96;
	} else {
		entry->ResourceEntity.Entry[0].EntityLocation 
                	= ipmi_entity_get_entity_instance(entity);	
	}

	entry->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
	entry->ResourceEntity.Entry[1].EntityLocation = 0;


	update_resource_capabilities(entity, entry);

			
	if (entry->ResourceEntity.Entry[0].EntityType == SAHPI_ENT_SYSTEM_BOARD) {
		/* This is the BMC entry, so we need to add watchdog. */
		entry->ResourceCapabilities |= SAHPI_CAPABILITY_WATCHDOG;
	}
	if (entry->ResourceEntity.Entry[0].EntityType ==SAHPI_ENT_SYSTEM_CHASSIS) {
		entry->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_ROOT;
		entry->ResourceEntity.Entry[0].EntityLocation = 0;
	}

	//ipmi_entity_get_id(entity, entry->ResourceTag.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	str = ipmi_entity_get_entity_id_string(entity);
	oh_append_textbuffer(&entry->ResourceTag, str);


	if (ipmi_domain_get_type(domain) != IPMI_DOMAIN_TYPE_ATCA) {
		goto no_atca;
	}
	/* Since OpenIPMI does not hand us a more descriptive
	   tag which is an SDR issue in the chassis really, we'll over-ride
	   it here until things change
	*/

	
		/*
		 * If entity has a slot try to get it's number
		 */
	int no_slot;
	unsigned int slot_val;
	no_slot = ipmi_entity_get_physical_slot_num(entity, &slot_val);
	if (no_slot) {
		/* will use device address */
		slot_val = ipmi_entity_get_device_address(entity);
	}
	if (ipmi_entity_get_entity_id(entity) == 160) {
		// create Resource for phisical blade slot if it hasn't already been created
		SaHpiEntityPathT ep;
		SaHpiEntityPathT rootep;
		
		if (no_slot) {
			dbg("couldn't get slot for device");
			goto end_of_slot;
		}
		ep.Entry[0].EntityType = SAHPI_ENT_PHYSICAL_SLOT;     
		ep.Entry[0].EntityLocation = slot_val;
		ep.Entry[1].EntityType = SAHPI_ENT_ROOT;
		ep.Entry[1].EntityLocation = 0;   
		oh_encode_entitypath(ipmi_handler->entity_root, &rootep);
		oh_concat_ep(&ep, &rootep);
		if (oh_get_resource_by_ep(handler->rptcache, &ep) == NULL) {
			// create rpt for slot
			SaHpiRptEntryT srpt;
			struct ohoi_resource_info *s_r_info;
			char n[8];
			int i;
			init_rpt(&srpt);
			for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i ++) {
				srpt.ResourceEntity.Entry[i].EntityLocation =
					ep.Entry[i].EntityLocation;
				srpt.ResourceEntity.Entry[i].EntityType =
					ep.Entry[i].EntityType;
				if (ep.Entry[i].EntityType == SAHPI_ENT_ROOT) {
					break;
				}
			}
			oh_append_textbuffer(&srpt.ResourceTag, "Blade Slot ");
			snprintf(n, 8, "%d", slot_val);
			oh_append_textbuffer(&srpt.ResourceTag, n);
			srpt.ResourceId = oh_uid_from_entity_path(&srpt.ResourceEntity);
			
			s_r_info = malloc(sizeof(*ohoi_res_info));
			if (s_r_info == NULL) {
				dbg("Out of Memory");
				goto end_of_slot;
			}
			memset(s_r_info, 0, sizeof(*ohoi_res_info));
			if (oh_add_resource(handler->rptcache, &srpt, s_r_info, 1)) {
				dbg("couldn't add resource for slot %d",ep.Entry[0].EntityLocation);
			} else {
				trace_ipmi_entity("CREATE SLOT for ", 0, entity); 
				entity_rpt_set_presence(s_r_info, ipmi_handler, 1);
				
			}
		} else {
			dbg("slot %d already created", ep.Entry[0].EntityLocation);
		}
	}
	end_of_slot:
		
	if ((ipmi_entity_get_entity_id(entity) == 160) &&
			(ipmi_entity_get_entity_instance(entity) >= 96)) {
		dbg("SBC Blade");
		oh_init_textbuffer(&entry->ResourceTag);
		if ((ipmi_entity_get_device_address(entity) == 130)
			|| (ipmi_entity_get_device_address(entity) == 132)) {
			oh_append_textbuffer(&entry->ResourceTag,
							"Switch Blade");
			entry->ResourceEntity.Entry[0].EntityType =
							SAHPI_ENT_SWITCH_BLADE;
		} else {
			oh_append_textbuffer(&entry->ResourceTag, "SBC Blade");
			entry->ResourceEntity.Entry[0].EntityType =
							SAHPI_ENT_SBC_BLADE;
		}
		entry->ResourceEntity.Entry[1].EntityType =
						SAHPI_ENT_PHYSICAL_SLOT;
		entry->ResourceEntity.Entry[1].EntityLocation = slot_val;
		entry->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
		entry->ResourceEntity.Entry[2].EntityLocation = 0;
        }

	/* this is here for Force-made Storage blades
	 * until we have access to latest hardware
	 * DO NOT CHANGE
	*/
	if ((ipmi_entity_get_entity_id(entity) == 160) &&
			(ipmi_entity_get_entity_instance(entity) == 102)) {
		dbg("DISK Blade");
		oh_init_textbuffer(&entry->ResourceTag);
		oh_append_textbuffer(&entry->ResourceTag, "Storage/Disk Blade");
		entry->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_DISK_BLADE;
        }
	
	if ((ipmi_entity_get_entity_id(entity) == 10) &&
			(ipmi_entity_get_entity_instance(entity) >= 96)) {
		dbg("Power Unit");

#if 0
		entry->ResourceEntity.Entry[1].EntityLocation = val;
		entry->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_CHASSIS_SPECIFIC + 1;
		entry->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
		entry->ResourceEntity.Entry[2].EntityLocation = 0;
#else
		entry->ResourceEntity.Entry[0].EntityLocation = slot_val;
#endif
        }
	
       if ((ipmi_entity_get_entity_id(entity) == 0xf0) &&
                        (ipmi_entity_get_entity_instance(entity) >= 96))  {
		dbg("ShMC");
		if ((ipmi_entity_get_device_channel(entity) != 0) ||
				(ipmi_entity_get_device_address(entity) != 32)) {
			entry->ResourceEntity.Entry[1].EntityLocation = slot_val;
			entry->ResourceEntity.Entry[1].EntityType =
					SAHPI_ENT_CHASSIS_SPECIFIC + 3;
			entry->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
			entry->ResourceEntity.Entry[2].EntityLocation = 0;
		}
        }        

       if ((ipmi_entity_get_entity_id(entity) == 0xf2) &&
                        (ipmi_entity_get_entity_instance(entity) >= 96))  {
		dbg("Shelf FRU");
		entry->ResourceEntity.Entry[1].EntityLocation = slot_val;
		entry->ResourceEntity.Entry[1].EntityType =
					SAHPI_ENT_CHASSIS_SPECIFIC + 2;
		entry->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
		entry->ResourceEntity.Entry[2].EntityLocation = 0;
        }        
	/* End AdvancedTCA Fix-ups */

no_atca:
	if (ipmi_entity_get_is_child(entity)) {
		struct add_parent_ep_s info;
		info.handler = handler;
		info.entry = entry;
		ipmi_entity_iterate_parents(entity, add_parent_ep, &info);
	}
	oh_encode_entitypath(ipmi_handler->entity_root, &entity_ep);
	oh_concat_ep(&entry->ResourceEntity, &entity_ep);

	entry->ResourceId = oh_uid_from_entity_path(&entry->ResourceEntity);

			/* controls */
			rv = ipmi_entity_add_control_update_handler(entity,
								    ohoi_control_event,
								    handler);
                                                                                
			if (rv) {
				dbg("ipmi_entity_set_control_update_handler: %#x", rv);
				return;
			}
}

static void add_entity_event(ipmi_domain_t            *domain,
			     ipmi_entity_t	        *entity,
			     struct oh_handler_state    *handler)
{
      	struct ohoi_resource_info *ohoi_res_info;
	SaHpiRptEntryT	entry;
	int rv;
	int inst;
	
	inst = ipmi_entity_get_entity_instance(entity);
	if (inst >= 96) {
		inst -= 96;
	}

	memset(&entry, 0, sizeof (entry));

	ohoi_res_info = malloc(sizeof(*ohoi_res_info));

	if (!ohoi_res_info) {
	      	dbg("Out of memory");
		trace_ipmi_entity("CAN NOT ADD ENTITY. OUT OF MEMORY",
			inst, entity);
		return;
	}
	memset(ohoi_res_info, 0, sizeof (*ohoi_res_info));

	ohoi_res_info->type       = OHOI_RESOURCE_ENTITY; 
	ohoi_res_info->u.entity_id= ipmi_entity_convert_to_id(entity);

	get_entity_event(domain, entity, ohoi_res_info, &entry, handler);	
	rv = oh_add_resource(handler->rptcache, &entry, ohoi_res_info, 1);
	if (rv) {
	      	dbg("oh_add_resource failed for %d = %s\n",
			entry.ResourceId, oh_lookup_error(rv));
		trace_ipmi_entity("CAN NOT ADD ENTITY. ADD RESOURCE FAILED",
			inst, entity);
		return;
	}
}

void ohoi_remove_entity(struct oh_handler_state *handler,
			SaHpiResourceIdT res_id)
{
      	struct oh_event *e;
	struct ohoi_resource_info *res_info;

	res_info = oh_get_resource_data(handler->rptcache, res_id);


	/* Now put an event for the resource to DEL */
	e = malloc(sizeof(*e));
	if (e == NULL) {
		dbg("Out of memory");
		return;
	}
	memset(e, 0, sizeof(*e));

	e->did = oh_get_default_domain_id();
	e->type = OH_ET_RESOURCE_DEL;
	e->u.res_event.entry.ResourceId = res_id;

	handler->eventq = g_slist_append(handler->eventq, e);
	entity_rpt_set_updated(res_info, handler->data);
}



static void change_entity(struct oh_handler_state	*handler,
		          ipmi_entity_t			*entity)
{
	ipmi_entity_id_t entity_id = ipmi_entity_convert_to_id(entity);
	SaHpiRptEntryT	*rpt;
	
	rpt = ohoi_get_resource_by_entityid(handler->rptcache, &entity_id);
	if (rpt == NULL) {
		dbg("Couldn't find out resource by entity %d.%.d.%d.%d  %s",
			ipmi_entity_get_entity_id(entity), 
			ipmi_entity_get_entity_instance(entity),
			ipmi_entity_get_device_channel(entity),
			ipmi_entity_get_device_address(entity),
			ipmi_entity_get_entity_id_string(entity));
		trace_ipmi_entity("CAN NOT CHANGE RESOURCE. NO RPT",
			0, entity);
		return;
	}
	update_resource_capabilities(entity, rpt);
}

static void delete_entity(struct oh_handler_state	*handler,
		          ipmi_entity_t			*entity)
{
	ipmi_entity_id_t entity_id = ipmi_entity_convert_to_id(entity);
	SaHpiRptEntryT	*rpt;
	struct ohoi_resource_info *res_info;

	rpt = ohoi_get_resource_by_entityid(handler->rptcache, &entity_id);
	if (rpt == NULL) {
		dbg("couldn't find out resource");
		return;
	}
	res_info =  oh_get_resource_data(handler->rptcache, rpt->ResourceId);
	entity_rpt_set_presence(res_info, handler->data, 0);
}

void ohoi_entity_event(enum ipmi_update_e       op,
                       ipmi_domain_t            *domain,
                       ipmi_entity_t            *entity,
                       void                     *cb_data)
{
  	struct oh_handler_state *handler = cb_data;
	struct ohoi_handler *ipmi_handler = handler->data;
	int rv;
	int inst=0;
	
	inst=ipmi_entity_get_entity_instance(entity);
	if(inst >=96) {
		inst = inst - 96;
	}
	g_static_rec_mutex_lock(&ipmi_handler->ohoih_lock);			
	switch (op) {
	  	case IPMI_ADDED:
			add_entity_event(domain, entity, handler);
			trace_ipmi_entity("ADDED", inst, entity);

			/* entity presence overall */
			rv = ipmi_entity_add_presence_handler(entity,
							      entity_presence,
							      handler);       		
			if (rv) 
				dbg("ipmi_entity_set_presence_handler: %#x", rv);

			/* hotswap handler */
			rv = ipmi_entity_add_hot_swap_handler(entity,
							      ohoi_hot_swap_cb,
							      cb_data);
			if(rv)
			  	dbg("Failed to set entity hot swap handler");

			/* sensors */
			rv= ipmi_entity_add_sensor_update_handler(entity,
								  ohoi_sensor_event,
								  handler);
			if (rv) {
				dbg("ipmi_entity_set_sensor_update_handler: %#x", rv);
				break;
			}
                                                                                
			/* inventory (a.k.a FRU) */
			rv = ipmi_entity_add_fru_update_handler(entity,
								ohoi_inventory_event,
								handler);
			if (rv) {
			  	dbg("ipmi_entity_set_fru_update_handler: %#x", rv);
				break;
			}                                                                          
			break;
			
		case IPMI_DELETED:
			delete_entity(handler, entity);
			trace_ipmi_entity("DELETED", inst, entity);
			break;
			
		case IPMI_CHANGED:
			change_entity(handler, entity);
			trace_ipmi_entity("CHANGED", inst, entity);
			break;
		default:
			dbg("Entity: Unknow change?!");
	}
	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);	
}
