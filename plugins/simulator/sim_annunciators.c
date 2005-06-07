/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *	  Christina Hernandez <hernanc@us.ibm.com>
 *        W. David Ashley <dashley@us.ibm.com>
 */

#include <sim_annunciators.h>
#include <sim_init.h>
#include <sim_resources.h>
#include <rpt_utils.h>

SaErrorT sim_discover_annunc(RPTable *rpt)
{
        SaHpiRptEntryT *res;

        /* add to first resource */
        res = oh_get_resource_next(rpt, SAHPI_FIRST_ENTRY);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 1); /* add #1 ... */
        new_annunc(rpt, res->ResourceId, 5); /* add #5 ... */

        /* add to second resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 1);
        new_annunc(rpt, res->ResourceId, 2);
        new_annunc(rpt, res->ResourceId, 3);

        /* add to third resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 2);
        new_annunc(rpt, res->ResourceId, 6);

        /* add to fourth resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 3);
        new_annunc(rpt, res->ResourceId, 5);

        /* add to fifth resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 2);

        /* add to sixth resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 1);
        new_annunc(rpt, res->ResourceId, 2);
        new_annunc(rpt, res->ResourceId, 3);
        new_annunc(rpt, res->ResourceId, 4);

        /* add to seventh resource */
        res = oh_get_resource_next(rpt, res->ResourceId);
        if (!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 4);
        new_annunc(rpt, res->ResourceId, 2);

        /*add to eighth resource*/
        res = oh_get_resource_next(rpt, res->ResourceId);
        if(!res){
                dbg("Resource Not Found");
                return 1;
        }
        new_annunc(rpt, res->ResourceId, 5);

        return 0;

}

SaErrorT new_annunc(RPTable *rptcache, SaHpiResourceIdT ResId, int Index){
        SaHpiRdrT res_rdr;
        SaHpiRptEntryT *RptEntry;

        // Copy information from rdr array to res_rdr
        res_rdr.RdrType = SAHPI_CONTROL_RDR;
        memcpy(&res_rdr.RdrTypeUnion.CtrlRec, &sim_annunc[Index].RdrTypeUnion.CtrlRec, sizeof(SaHpiCtrlRecT));

        oh_init_textbuffer(&res_rdr.IdString);
        oh_append_textbuffer(&res_rdr.IdString, sim_annunc[Index].comment);
        res_rdr.RdrTypeUnion.CtrlRec.Num = sim_get_next_annunc_num(rptcache, ResId, res_rdr.RdrTypeUnion.CtrlRec.Type);
        res_rdr.RecordId = get_rdr_uid(res_rdr.RdrType, res_rdr.RdrTypeUnion.CtrlRec.Num);

        RptEntry = oh_get_resource_by_id(rptcache, ResId);
        if(!RptEntry){
                dbg("NULL rpt pointer\n");
        }
        else{
                res_rdr.Entity = RptEntry->ResourceEntity;
        }

        oh_add_rdr(rptcache, ResId, &res_rdr, NULL, 0);

        return 0;
}

int sim_get_next_annunc_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type)
{
        int i=0;
        SaHpiRdrT *RdrEntry;
        RdrEntry = oh_get_rdr_next(rptcache, ResId, SAHPI_FIRST_ENTRY);
        while(RdrEntry){
                if (RdrEntry->RdrType == type){
                        i++;
                }
                if (RdrEntry->RecordId != 0){
                        RdrEntry = oh_get_rdr_next(rptcache, ResId, RdrEntry->RecordId);
                }
        }
        return i;

        if(!(oh_get_rdr_by_type(rptcache, ResId, type, i))){
                printf("I hit sim_get_next_annunc_num\n");
                return i;
        }
        else{
                while(oh_get_rdr_by_type(rptcache, ResId, type, i)){
                        i++;
                }
                return i;
        }
}

/************************************************************************/
/* Resource one annunciator data                                          */
/************************************************************************/

sim_annunc sim_annunc[] = {
	{
		.Num = 1,
		.mode = SAHPI_ANNUNCIATOR_MODE_USER,
		.count = ANNOUN_MAX,
		.def_announs[0] = {
			.EntryId = 1,
			.Timestamp = 0,
			.AddedByUser = SAHPI_FALSE,
			.Severity = SAHPI_MAJOR,
			.Acknowledged = SAHPI_FALSE,
			.StatusCond = {
				.Type = SAHPI_STATUS_COND_TYPE_SENSOR,
				.Entity   = {
					.Entry = {
						{SAHPI_ENT_SYSTEM_BOARD, 1},
						{SAHPI_ENT_ROOT, 0}
					},
				},
				.DomainId = 1,
				.ResourceId = 1,
				.SensorNum = 1,
				.EventState = SAHPI_ES_UNSPECIFIED,
				.Name = {
					.Length = 5,
					.Value = "announ"
				},
				.Mid = 123,
				.Data = def_text_buffer("Announcement-1")
			}
		},
		.announs = NULL
	}
};

#ifdef 0
static int __build_the_rpt_cache(struct oh_handler_state *oh_hnd)
{
        int i;
        int id;
        SaHpiRptEntryT res;
        SaHpiRptEntryT *rpt;
        SaHpiRdrT res_rdr;
        SaHpiRdrT *rdr;
        SaHpiEntityPathT root_ep;
        char *entity_root;

        entity_root = (char *)g_hash_table_lookup(oh_hnd->config,"entity_root");
	oh_encode_entitypath(entity_root, &root_ep);
        /* append entity root to resource entity paths */
        for (i=0; i < NUM_RESOURCES; i++) {
                memcpy(&res, &dummy_resources[i], sizeof(SaHpiRptEntryT));

                oh_concat_ep( &res.ResourceEntity, &root_ep);

                res.ResourceId = oh_uid_from_entity_path(&res.ResourceEntity);

                /* add the resource */
                if (oh_add_resource(oh_hnd->rptcache, &res, NULL, 0)) {
                        dbg("oh_add_resource failed for RESOURCE %d", i);
			return -1;
		}

                /* save the resource id for tracking resource status */
                dummy_resource_status[i].ResourceId = res.ResourceId;

		dbg("oh_add_resource succeeded for RESOURCE %d", i);

        }
/*
	for (i = 0; dummy_inventory[i].idrinfo.IdrId != 0; i++) {
		int	j, k;
		char	bf[256];

		for (j = 0; j < dummy_inventory[i].idrinfo.NumAreas; j++) {
			for (k = 0; k < dummy_inventory[i].my_idr_area[j].NumFields; k++) {
				if (dummy_inventory[i].my_idr_area[j].idrfields[k].Field.DataType !=
						SAHPI_TL_TYPE_UNICODE) continue;
			}
		}
	};
*/
        /* append entity root to rdrs entity paths */
        for (i=0; i < NUM_RDRS; i++) {
                memcpy(&res_rdr, &dummy_rdrs[i], sizeof(SaHpiRdrT));
                oh_concat_ep( &res_rdr.Entity, &root_ep);

                id = oh_uid_lookup(&res_rdr.Entity);

                if( id < 0 ) {
			dbg("error looking up uid in dummy_open");
			return-1;
		}

                /* add rdrs */
                if (oh_add_rdr(oh_hnd->rptcache, id, &res_rdr, NULL, 0)) {
                        dbg("oh_add_resource failed for RDR %d", i);
			return -1;
                }

		dbg("oh_add_resource succeeded for RDR %d", i);
        };

	i = 0;
	rpt = oh_get_resource_next(oh_hnd->rptcache, SAHPI_FIRST_ENTRY);
	while (rpt) {
		if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
        		rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, SAHPI_FIRST_ENTRY);
        		while (rdr) {
				memcpy(&(Save_rdr[i].rdr_default), rdr, sizeof(SaHpiRdrT));
				Save_rdr[i].is_save = 0;
				Save_rdr[i].res_Id = rpt->EntryId;
				Save_rdr[i].rdr_type = rdr->RdrType;
				switch (rdr->RdrType) {
					case SAHPI_SENSOR_RDR:
						Save_rdr[i].num = rdr->RdrTypeUnion.SensorRec.Num;
						break;
					case SAHPI_CTRL_RDR:
						Save_rdr[i].num = rdr->RdrTypeUnion.CtrlRec.Num;
						break;
					case SAHPI_INVENTORY_RDR:
						Save_rdr[i].num = rdr->RdrTypeUnion.InventoryRec.IdrId;
						break;
					case SAHPI_WATCHDOG_RDR:
						Save_rdr[i].num = rdr->RdrTypeUnion.WatchdogRec.WatchdogNum;
						break;
					case SAHPI_ANNUNCIATOR_RDR:
						Save_rdr[i].num =
							rdr->RdrTypeUnion.AnnunciatorRec.AnnunciatorNum;
						break;
					default:
						rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId,
							rdr->RecordId);
						continue;
				};
				i++;
                		rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, rdr->RecordId);
			}
		};
		rpt = oh_get_resource_next(oh_hnd->rptcache, rpt->EntryId);
        }

	rpt = oh_get_resource_next(oh_hnd->rptcache, SAHPI_FIRST_ENTRY);
	while (rpt) {
		SaHpiAnnunciatorNumT	num;

		rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, SAHPI_FIRST_ENTRY);
		while (rdr) {
			switch (rdr->RdrType) {
				case SAHPI_ANNUNCIATOR_RDR:
					num = rdr->RdrTypeUnion.AnnunciatorRec.AnnunciatorNum;
					for (i = 0; i < ANNUN_MAX; i++)
						if (dummy_announs[i].Num == num) break;
					if (i >= ANNUN_MAX) break;
					dummy_announs[i].announs =
						(SaHpiAnnouncementT *)malloc(sizeof(SaHpiAnnouncementT) *
							dummy_announs[i].count);
					memcpy(dummy_announs[i].announs, dummy_announs[i].def_announs,
						sizeof(SaHpiAnnouncementT) * dummy_announs[i].count);
					break;
				default:
					break;
			};
               		rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, rdr->RecordId);
		};
		rpt = oh_get_resource_next(oh_hnd->rptcache, rpt->EntryId);
        };

        return(0);
}
#endif
