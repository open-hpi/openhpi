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
 *     Tariq Shureih <tariq.shureih@intel.com>
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include "ipmi.h"
#include <epath_utils.h>
#include <uid_utils.h>
#include <string.h>

//static void add_control_event_data_format(ipmi_control_t	*control,
					   //SaHpiCtrlRecT	*rec)
//{


//}


static void add_control_event_control_rec(ipmi_control_t	*control,
					SaHpiCtrlRecT	*rec)
{
        static int control_num = 0;
	int	control_type;
	rec->Num = ++control_num;
	control_type = ipmi_control_get_type(control);
	switch(control_type) {

		/* This is a special case we're handle later */
		//case IPMI_CONTROL_RESET:
		//case IPMI_CONTROL_POWER:
		//case IPMI_CONTROL_ONE_SHOT_RESET:
			//break;
			
		/* **FIXME: need to verify this mapping */
		/* assuming binary control */
		case IPMI_CONTROL_ALARM:
		case IPMI_CONTROL_DISPLAY:
		case IPMI_CONTROL_LIGHT:
		case IPMI_CONTROL_RELAY:
		case IPMI_CONTROL_FAN_SPEED:
		case IPMI_CONTROL_IDENTIFIER:
			rec->Type = SAHPI_CTRL_TYPE_DIGITAL;
			break;
		default:
			rec->Type = SAHPI_CTRL_TYPE_OEM;
	}
		
	
	rec->Ignore = (SaHpiBoolT)ipmi_control_get_ignore_if_no_entity(control);

	//add_control_event_data_format(control, rec);

	/* We do not care about oem. */
	rec->Oem = 0;
}

static void add_control_event_rdr(ipmi_control_t		*control, 
				 SaHpiRdrT		*rdr,
				 SaHpiEntityPathT	parent_ep,
				 SaHpiResourceIdT	res_id)
{
	char	name[32];
	//SaHpiEntityPathT rdr_ep;

	memset(name,'\0',32);
	rdr->RecordId = 0;
	rdr->RdrType = SAHPI_CTRL_RDR;
	//rdr->Entity.Entry[0].EntityType = (SaHpiEntityTypeT)id;
	//rdr->Entity.Entry[0].EntityInstance = (SaHpiEntityInstanceT)instance;
	//rdr->Entity.Entry[1].EntityType = 0;
	//rdr->Entity.Entry[1].EntityInstance = 0;
	rdr->Entity = parent_ep;

	/* append ep */
	//string2entitypath(entity_root, &rdr_ep);

	//ep_concat (&rdr->Entity, &rdr_ep);

	add_control_event_control_rec(control, &rdr->RdrTypeUnion.CtrlRec);

	ipmi_control_get_id(control, name, 32);
	rdr->IdString.DataType = SAHPI_TL_TYPE_ASCII6;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	rdr->IdString.DataLength = 32;

	memcpy(rdr->IdString.Data,name, strlen(name) + 1);
}

static void add_control_event(ipmi_entity_t	*ent,
			     ipmi_control_t	*control,
			     struct oh_handler_state *handler,
			     SaHpiEntityPathT	parent_ep,
			     SaHpiResourceIdT	rid)
{
        ipmi_control_id_t        *control_id; 
	struct oh_event         *e;

        control_id = malloc(sizeof(*control_id));
        if (!control_id) {
                dbg("Out of memory");
                return;
        }
        *control_id = ipmi_control_convert_to_id(control);
        
	e = malloc(sizeof(*e));
	if (!e) {
                free(control_id);
		dbg("Out of space");   
		return;
	}
	memset(e, 0, sizeof(*e));

	e->type = OH_ET_RDR;

	add_control_event_rdr(control, &e->u.rdr_event.rdr, parent_ep, rid);	

	rid = oh_uid_lookup(&e->u.rdr_event.rdr.Entity);

	oh_add_rdr(handler->rptcache, rid, &e->u.rdr_event.rdr, control_id, 0);
}

void ohoi_control_event(enum ipmi_update_e op,
		        ipmi_entity_t      *ent,
			ipmi_control_t     *control,
			void               *cb_data)
{
	struct oh_handler_state *handler = cb_data;
	
	ipmi_entity_id_t	entity_id;	
	SaHpiRptEntryT		*rpt_entry;
        struct ohoi_resource_info *ohoi_res_info;
        
	entity_id = ipmi_entity_convert_to_id(ent);
	rpt_entry = ohoi_get_resource_by_entityid(
			handler->rptcache,
			&entity_id);   
        ohoi_res_info = oh_get_resource_data(handler->rptcache, 
                                           rpt_entry->ResourceId);
	
        if (!rpt_entry) {
		dump_entity_id("Control with RPT Entry?!", entity_id);
		return;
	}

	if (op == IPMI_ADDED) {
                int ctrl_type;
                
		/* skip Chassis with for now since all we have in hardware
		   is power and reset */
		dbg("resource: %s", rpt_entry->ResourceTag.Data);
                ctrl_type = ipmi_control_get_type(control);
                switch (ctrl_type) {
                        case IPMI_CONTROL_ONE_SHOT_RESET:
                                dbg("Attach reset control into entity");
                                ohoi_res_info->reset_ctrl
                                        = ipmi_control_convert_to_id(control);
                                break;
                        case IPMI_CONTROL_POWER:
                                dbg("Attach power control into entity");
                                ohoi_res_info->power_ctrl
                                        = ipmi_control_convert_to_id(control);
                                break;

                        default:
                                dbg("Other control(%d) is storaged in RDR", ctrl_type);
                                rpt_entry->ResourceCapabilities |= SAHPI_CAPABILITY_CONTROL;
                                add_control_event(ent, control, handler,
                                                  rpt_entry->ResourceEntity,
                                                  rpt_entry->ResourceId);
                }
                                
#if 0                        
		if((strcmp(rpt_entry->ResourceTag.Data,"system_chassis")) == 0) {
			dbg("controls on chassis are probably power/reset...skipping");
			return;
		} else {
			rpt_entry->ResourceCapabilities |= SAHPI_CAPABILITY_CONTROL; 
				 
			add_control_event(ent, control, handler, 
        	                         rpt_entry->ResourceEntity, 
                	                 rpt_entry->ResourceId);
		}
#endif		

	}
}
	
