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
#include <oh_utils.h>
#include <string.h>


static void get_mc_entity_event(ipmi_mc_t	*mc,
			        SaHpiRptEntryT	*entry, void *cb_data)
{
	uint8_t	vals[4];
	SaHpiEntityPathT mc_ep;
	char mc_name[128];
	int sel_support;
        
	struct ohoi_handler *ipmi_handler = cb_data;

	dbg("entity_root: %s", ipmi_handler->entity_root);
	oh_encode_entitypath(ipmi_handler->entity_root, &mc_ep);

        snprintf(mc_name, sizeof(mc_name),
                 "Management Controller(%x, %x)",
                 ipmi_mc_get_channel(mc), 
                 ipmi_mc_get_address(mc));
        
	entry->EntryId = 0;
	entry->ResourceInfo.ResourceRev = 0;
	entry->ResourceInfo.SpecificVer = 0;
	entry->ResourceInfo.DeviceSupport = 0;
	entry->ResourceInfo.ManufacturerId =
		(SaHpiManufacturerIdT)ipmi_mc_manufacturer_id(mc);
	entry->ResourceInfo.ProductId = (SaHpiUint16T)ipmi_mc_product_id(mc);
	entry->ResourceInfo.FirmwareMajorRev =
		(SaHpiUint8T)ipmi_mc_major_fw_revision(mc);
	entry->ResourceInfo.FirmwareMinorRev =
		(SaHpiUint8T)ipmi_mc_minor_fw_revision(mc);
	
	ipmi_mc_aux_fw_revision(mc, vals);
	/* There are 4, and we only use first. */
	entry->ResourceInfo.AuxFirmwareRev = (SaHpiUint8T)vals[0];
	entry->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_SYS_MGMNT_MODULE ;
	/*we get MC number on IPMB for unique identifier */
	entry->ResourceEntity.Entry[0].EntityLocation = ipmi_mc_get_address(mc);
        
        entry->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entry->ResourceEntity.Entry[1].EntityLocation = 0;
        
	dbg ("MC Instance: %d", entry->ResourceEntity.Entry[0].EntityLocation);
	sel_support = ipmi_mc_sel_device_support(mc);
	if (sel_support == 1) {
		dbg("MC supports SEL");
		entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE |
			SAHPI_CAPABILITY_EVENT_LOG;
	}
	else {
		entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;
		dbg("MC does not support SEL");
	}
	entry->ResourceSeverity = SAHPI_CRITICAL;
	entry->ResourceFailed = SAHPI_FALSE;
	entry->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	entry->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	entry->ResourceTag.DataLength = strlen(mc_name); 
	memcpy(entry->ResourceTag.Data, mc_name, strlen(mc_name)+1);

	oh_concat_ep(&entry->ResourceEntity, &mc_ep);
        
	entry->ResourceId = oh_uid_from_entity_path(&entry->ResourceEntity);
	dbg("MC ResourceId: %d", (int)entry->ResourceId);
}

static void mc_add(ipmi_mc_t                    *mc,
                   struct oh_handler_state      *handler)
{
        struct ohoi_resource_info *ohoi_res_info;
        struct oh_event *e;

		struct ohoi_handler *ipmi_handler = handler->data;
        
        ohoi_res_info = g_malloc0(sizeof(*ohoi_res_info));
        if (!ohoi_res_info) {
                dbg("Out of space");
                return;
        }
        ohoi_res_info->type      = OHOI_RESOURCE_MC;
        ohoi_res_info->u.mc_id    = ipmi_mc_convert_to_id(mc);
	dbg("Set updated for resource %p . MC", mc);
	/* always present if active thus here */
	entity_rpt_set_presence(ohoi_res_info, ipmi_handler, 1);
                
	e = malloc(sizeof(*e));
	if (!e) {
		dbg("Out of space");   
		return;
	}
	memset(e, 0, sizeof(*e));

	e->type = OH_ET_RESOURCE;

	get_mc_entity_event(mc, &(e->u.res_event.entry), ipmi_handler);

	/* add to rptcache */
	oh_add_resource(handler->rptcache, &(e->u.res_event.entry), ohoi_res_info, 1);

}

static
void mc_SDRs_read_done(ipmi_mc_t *mc, void *cb_data)
{
		int *mc_count = cb_data;

		/* one less MC to wait for */
		*mc_count = *mc_count - 1;

		dbg("mc(%d %x) SDRs read done mc count left: %d",
						ipmi_mc_get_address(mc),
						ipmi_mc_get_channel(mc), *mc_count);
}


static
void mc_active(ipmi_mc_t *mc, int active, void *cb_data)
{
		struct oh_handler_state *handler = cb_data;
		struct ohoi_handler *ipmi_handler = handler->data;

		int rv;

		if (active) {
				dbg("MC added and active...(%d %x)\n",
								ipmi_mc_get_address(mc),
								ipmi_mc_get_channel(mc));

				mc_add(mc, handler);

				ipmi_handler->mc_count++;
				/* register MC level SDRs read */
				rv = ipmi_mc_set_sdrs_first_read_handler(mc, mc_SDRs_read_done,
								&ipmi_handler->mc_count);
				if (rv)
						dbg("mc level SDRs read handler failed");
		}
}

void
ohoi_mc_event(enum ipmi_update_e op,
              ipmi_domain_t      *domain,
              ipmi_mc_t          *mc,
              void               *cb_data)
{
        struct oh_handler_state *handler = cb_data;
		struct ohoi_handler *ipmi_handler = handler->data;
		int rv;

        switch (op) {
                case IPMI_ADDED:
						
						/* if we get an MC but inactive, register a call to add
						   it once it goes active */
						rv = ipmi_mc_add_active_handler(mc, mc_active, handler);
						
						if(!ipmi_mc_is_active(mc)) {
								dbg("MC updated but inactive...we ignore (%d %x)\n",
												ipmi_mc_get_address(mc),
												ipmi_mc_get_channel(mc));
								break;
						} else {

								/* MC is active */
								dbg("MC Added(%d %x)\n",
												ipmi_mc_get_address(mc),
												ipmi_mc_get_channel(mc));
								mc_add(mc, handler);
						
								ipmi_handler->mc_count++;
								dbg("OP:ADD - mc count increment: %d", ipmi_handler->mc_count);
								
								/* register MC level SDRs read */
								ipmi_mc_set_sdrs_first_read_handler(mc, mc_SDRs_read_done,
											   	&ipmi_handler->mc_count);
								
								dbg("MC updated and is active: (%d %x)\n", 
												ipmi_mc_get_address(mc), 
												ipmi_mc_get_channel(mc));
								break;
						}
					case IPMI_DELETED:
						dbg("MC deleted: (%d %x)\n",
										ipmi_mc_get_address(mc), 
										ipmi_mc_get_channel(mc));
						/* most likely won't get called during discovery so comment
						   out for now */
						//ipmi_handler->mc_count--;

						/* need to add call to remove from RPT */
						break;

					case IPMI_CHANGED:
						if(!ipmi_mc_is_active(mc)) {
								dbg("MC changed and is inactive: (%d %x)\n",
												ipmi_mc_get_address(mc), 
												ipmi_mc_get_channel(mc));
						} else {
								mc_add(mc, handler);
								dbg("MC changed and is active: (%d %x)\n",
												ipmi_mc_get_address(mc),
												ipmi_mc_get_channel(mc));
						}
						break;
		}
}

