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


static void get_mc_entity_event(ipmi_mc_t	*mc,
			        SaHpiRptEntryT	*entry)
{
	uint8_t	vals[4];
	SaHpiEntityPathT mc_ep;
	char mc_name[128];
        
	memset(&mc_ep, 0, sizeof(SaHpiEntityPathT));
	dbg("entity_root: %s", entity_root);
	string2entitypath(entity_root, &mc_ep);
	append_root(&mc_ep);

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
	entry->ResourceEntity.Entry[0].EntityInstance = 0;
	entry->ResourceCapabilities = SAHPI_CAPABILITY_SEL;
	entry->ResourceSeverity = SAHPI_OK;
	entry->DomainId = 0;
	entry->ResourceTag.DataType = SAHPI_TL_TYPE_ASCII6;
	entry->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	entry->ResourceTag.DataLength = strlen(mc_name); 
	memcpy(entry->ResourceTag.Data, mc_name, strlen(mc_name)+1);

	ep_concat(&entry->ResourceEntity, &mc_ep);
        
	entry->ResourceId = oh_uid_from_entity_path(&mc_ep);
	dbg("MC ResourceId: %d", (int)entry->ResourceId);
}

static void mc_add(ipmi_mc_t                    *mc,
                   struct oh_handler_state      *handler)
{
        struct ohoi_resource_id *ohoi_res_id;
        struct oh_event *e;
        
        ohoi_res_id = g_malloc0(sizeof(*ohoi_res_id));
        if (!ohoi_res_id) {
                dbg("Out of space");
                return;
        }
        ohoi_res_id->type       = OHOI_RESOURCE_MC;
        ohoi_res_id->u.mc_id    = ipmi_mc_convert_to_id(mc);
                
	e = malloc(sizeof(*e));
	if (!e) {
		dbg("Out of space");   
		return;
	}
	memset(e, 0, sizeof(*e));

	e->type = OH_ET_RESOURCE;

	get_mc_entity_event(mc, &(e->u.res_event.entry));

	/* add to rptcache */
	oh_add_resource(handler->rptcache, &(e->u.res_event.entry), ohoi_res_id, 1);

}

void
ohoi_mc_event(enum ipmi_update_e op,
              ipmi_domain_t      *domain,
              ipmi_mc_t          *mc,
              void               *cb_data)
{
        struct oh_handler_state *handler = cb_data;

        switch (op) {
                case IPMI_ADDED:
                        mc_add(mc, handler);
                        dbg("MC added: (%d %x)\n", 
                            ipmi_mc_get_address(mc), 
                            ipmi_mc_get_channel(mc));
                        break;
                case IPMI_DELETED:
                        dbg("MC deleted: (%d %x)\n",
                            ipmi_mc_get_address(mc), 
                            ipmi_mc_get_channel(mc));
                        break;
                case IPMI_CHANGED:
                        dbg("MC changed: (%d %x)\n",
                            ipmi_mc_get_address(mc), 
                            ipmi_mc_get_channel(mc));
                        break;
        }
}

