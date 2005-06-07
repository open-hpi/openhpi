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


#include <sim_init.h>
#include <rpt_utils.h>


static SaErrorT new_control(RPTable *rptcache, SaHpiResourceIdT ResId,
                            int Index) {
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;

	// Copy information from rdr array to res_rdr
	res_rdr.RdrType = SAHPI_CTRL_RDR;
	memcpy(&res_rdr.RdrTypeUnion.CtrlRec, &sim_controls[Index].control, sizeof(SaHpiCtrlRecT));

	oh_init_textbuffer(&res_rdr.IdString);
	oh_append_textbuffer(&res_rdr.IdString, sim_controls[Index].comment);
	res_rdr.RdrTypeUnion.CtrlRec.Num = sim_get_next_control_num(rptcache, ResId, res_rdr.RdrTypeUnion.CtrlRec.Type);
	res_rdr.RecordId = get_rdr_uid(SAHPI_CTRL_RDR, res_rdr.RdrTypeUnion.CtrlRec.Num);

	RptEntry = oh_get_resource_by_id(rptcache, ResId);
	if(!RptEntry){
		dbg("NULL rpt pointer\n");
	} else {
		res_rdr.Entity = RptEntry->ResourceEntity;
	}

	oh_add_rdr(rptcache, ResId, &res_rdr, NULL, 0);

	return 0;
}


SaErrorT sim_discover_controls(RPTable *rpt) {
	SaHpiRptEntryT *res;

	/* add to first resource */
	res = oh_get_resource_next(rpt, SAHPI_FIRST_ENTRY);
	if (!res) {
		dbg("resource not fond");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1); /* add #1 ... */
	new_control(rpt, res->ResourceId, 5); /* add #5 ... */

	/* add to second resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1);
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 3);

	/* add to third resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 6);

	/* add to fourth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 3);
	new_control(rpt, res->ResourceId, 5);

	/* add to fifth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 2);

	/* add to sixth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1);
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 3);
	new_control(rpt, res->ResourceId, 4);

	/* add to seventh resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 4);
	new_control(rpt, res->ResourceId, 2);

	/*add to eighth resource*/
	res = oh_get_resource_next(rpt, res->ResourceId);
	if(!res) {
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 5);

        return 0;
}


int sim_get_next_control_num(RPTable *rptcache, SaHpiResourceIdT ResId,
                             SaHpiRdrTypeT type) {
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
		printf("I hit sim_get_next_control_num\n");
		return i;
	}
	else{
		while(oh_get_rdr_by_type(rptcache, ResId, type, i)){
			i++;
		}
		return i;
	}
}


struct sim_control sim_controls[] = {
        /* Front Panel Identify LED. User controlled. */
  	/* 0 is Off; 1 is solid on; 2 is blinking */
	{
                .control = {
                        .Num = 1,
                        .OutputType = SAHPI_CTRL_LED,
                        .Type = SAHPI_CTRL_TYPE_DISCRETE,
                        .TypeUnion.Discrete.Default = 0,
			.DefaultMode = {
				.Mode = SAHPI_CTRL_MODE_MANUAL,
				.ReadOnly = SAHPI_TRUE,
			},
			.WriteOnly = SAHPI_FALSE,
                        .Oem = 0,
                },
                .control_info = {
                        .mib = {
                                .not_avail_indicator_num = 3,
                                .write_only = SAHPI_FALSE,
                        },
			.cur_mode = SAHPI_CTRL_MODE_MANUAL,
                },
                .comment = "Front Panel Identify LED"
        },

        {} /* Terminate array with a null element */
};
