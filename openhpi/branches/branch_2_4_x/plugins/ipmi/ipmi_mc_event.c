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




static void trace_ipmi_mc(char *str, ipmi_mc_t *mc)
{
	if (!getenv("OHOI_TRACE_MC") && !IHOI_TRACE_ALL) {
		return;
	}
	fprintf(stderr, "*** MC (%d, %d):  %s. sel support = %d\n",
                 ipmi_mc_get_channel(mc), 
                 ipmi_mc_get_address(mc), str,
		 ipmi_mc_sel_device_support(mc));
}
#if 0

static void get_mc_entity_event(ipmi_mc_t	*mc,
			        SaHpiRptEntryT	*entry, void *cb_data)
{
	uint8_t	vals[4];
	SaHpiEntityPathT mc_ep;
	char mc_name[128];
	int sel_support;
        
	struct ohoi_handler *ipmi_handler = cb_data;

	trace_ipmi("entity_root: %s", ipmi_handler->entity_root);
	oh_encode_entitypath(ipmi_handler->entity_root, &mc_ep);

        snprintf(mc_name, sizeof(mc_name),
                 "Management Controller(%x, %x)",
                 ipmi_mc_get_channel(mc), 
                 ipmi_mc_get_address(mc));
        
	entry->EntryId = 0;
	entry->ResourceInfo.ResourceRev = ipmi_mc_device_revision(mc);
	entry->ResourceInfo.SpecificVer = 0;
	entry->ResourceInfo.DeviceSupport = ipmi_mc_sensor_device_support(mc);
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
        
	trace_ipmi("MC Instance: %d", entry->ResourceEntity.Entry[0].EntityLocation);
	sel_support = ipmi_mc_sel_device_support(mc);
	if (sel_support == 1) {
		trace_ipmi("MC supports SEL");
		entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE |
			SAHPI_CAPABILITY_EVENT_LOG;
	}
	else {
		entry->ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;
		trace_ipmi("MC does not support SEL");
	}
	entry->ResourceSeverity = SAHPI_CRITICAL;
	entry->ResourceFailed = SAHPI_FALSE;
	entry->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	entry->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	entry->ResourceTag.DataLength = strlen(mc_name); 
	memcpy(entry->ResourceTag.Data, mc_name, strlen(mc_name)+1);

	oh_concat_ep(&entry->ResourceEntity, &mc_ep);
        
	entry->ResourceId = oh_uid_from_entity_path(&entry->ResourceEntity);
	trace_ipmi("MC ResourceId: %d", (int)entry->ResourceId);
}
#endif
static void mc_add(ipmi_mc_t                    *mc,
                   struct oh_handler_state      *handler)
{
#if 0
        struct ohoi_resource_info *ohoi_res_info;
        struct oh_event *e;

		struct ohoi_handler *ipmi_handler = handler->data;
        
        ohoi_res_info = malloc(sizeof(*ohoi_res_info));
        if (!ohoi_res_info) {
                dbg("Out of space");
                return;
        }
	memset(ohoi_res_info, 0, sizeof(*ohoi_res_info));
        ohoi_res_info->type      = OHOI_RESOURCE_MC;
        ohoi_res_info->u.mc_id    = ipmi_mc_convert_to_id(mc);
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
#endif
	trace_ipmi_mc("ADDED and ACTIVE", mc);

}


static void mc_remove(ipmi_mc_t                    *mc,
                   struct oh_handler_state      *handler)
{
#if 0
        struct ohoi_resource_info *res_info;
	struct ohoi_handler *ipmi_handler = handler->data;
	ipmi_mcid_t mcid;
	SaHpiRptEntryT *rpt;
        

        mcid    = ipmi_mc_convert_to_id(mc);
	rpt = ohoi_get_resource_by_mcid(handler->rptcache, &mcid);
	if (rpt == NULL) {
		dbg("couldn't find out resource");
		return;
	}
	res_info =  oh_get_resource_data(handler->rptcache, rpt->ResourceId);
	/* always present if active thus here */
	entity_rpt_set_presence(res_info, ipmi_handler, 0);
#endif
	trace_ipmi_mc("REMOVED (not present)", mc);
}

#if 0
static
void mc_SDRs_read_done(ipmi_mc_t *mc, void *cb_data)
{
		int *mc_count = cb_data;

		/* one less MC to wait for */
		*mc_count = *mc_count - 1;

		trace_ipmi("mc(%d %x) SDRs read done mc count left: %d",
						ipmi_mc_get_address(mc),
						ipmi_mc_get_channel(mc), *mc_count);
}

#endif

static
void mc_active(ipmi_mc_t *mc, int active, void *cb_data)
{
	struct oh_handler_state *handler = cb_data;
//	struct ohoi_handler *ipmi_handler = handler->data;

	if (active) {
		mc_add(mc, handler);

//		ipmi_handler->mc_count++;

	} else {
		mc_remove(mc, handler);
	}
}


static
void process_sel_support(ipmi_mc_t *mc, struct oh_handler_state *handler)
{
        struct ohoi_resource_info *res_info;
	struct ohoi_handler *ipmi_handler = handler->data;
	ipmi_mcid_t mcid;
	SaHpiRptEntryT *rpt;
        

        mcid    = ipmi_mc_convert_to_id(mc);
	rpt = ohoi_get_resource_by_mcid(handler->rptcache, &mcid);
	if (rpt == NULL) {
		trace_ipmi_mc("COULDN'T FIND RPT", mc);
		dbg("couldn't find out resource");
		return;
	}
	res_info =  oh_get_resource_data(handler->rptcache, rpt->ResourceId);
	if (ipmi_mc_sel_device_support(mc)) {
		rpt->ResourceCapabilities |=
					SAHPI_CAPABILITY_EVENT_LOG;
		entity_rpt_set_updated(res_info, ipmi_handler);
	}
}
	


static
void mc_processed(ipmi_mc_t *mc, void *cb_data)
{
	struct oh_handler_state *handler = cb_data;
	struct ohoi_handler *ipmi_handler = handler->data;

	g_static_rec_mutex_lock(&ipmi_handler->ohoih_lock);
	if (ipmi_mc_is_active(mc)) {
		process_sel_support(mc, handler);
	} else {
		trace_ipmi_mc("NOT ACTIVE IN PROCESSED", mc);
	}
	if (!ipmi_handler->fully_up) {
		// do nothing. we''ll process everything when
		// domain is fully up
	 	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);
		trace_ipmi_mc("PROCESSED, domain not fully up", mc);
		return;
	}
	trace_ipmi_mc("PROCESSED, handle this event", mc);
	if (IS_ATCA(ipmi_handler->d_type)) {
		ohoi_atca_create_fru_rdrs(handler);
	}
	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);
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
		
	if ((ipmi_mc_get_channel(mc) == 0) && 
                 (ipmi_mc_get_address(mc) == 32) &&
		 ipmi_handler->d_type == IPMI_DOMAIN_TYPE_ATCA) {
		 	ipmi_handler->virt_mcid = ipmi_mc_convert_to_id(mc);
	}

	g_static_rec_mutex_lock(&ipmi_handler->ohoih_lock);			
        switch (op) {
                case IPMI_ADDED:
			/* if we get an MC but inactive, register a call to add
			it once it goes active */
			rv = ipmi_mc_add_active_handler(mc, mc_active, handler);
			rv = ipmi_mc_set_sdrs_first_read_handler(mc,
				mc_processed, handler);
			if(!ipmi_mc_is_active(mc)) {
				trace_ipmi_mc("ADDED but inactive...we ignore", mc);
				break;
			} else {
				mc_add(mc, handler);
//				ipmi_handler->mc_count++;
//				trace_ipmi("OP:ADD - mc count increment: %d", ipmi_handler->mc_count);
								
				/* register MC level SDRs read */
//				ipmi_mc_set_sdrs_first_read_handler(mc, mc_SDRs_read_done,
//								&ipmi_handler->mc_count);
				break;
			}
		case IPMI_DELETED:
			trace_ipmi_mc("DELETED, but nothing done", mc);
			break;

		case IPMI_CHANGED:
			if(!ipmi_mc_is_active(mc)) {
				trace_ipmi("CHANGED and is inactive: (%d %x)\n",
						ipmi_mc_get_address(mc), 
						ipmi_mc_get_channel(mc));
			} else {
				mc_add(mc, handler);
			}
			break;
		}
	g_static_rec_mutex_unlock(&ipmi_handler->ohoih_lock);

}

