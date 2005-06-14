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


static int sim_get_next_control_num(RPTable *rptcache, SaHpiResourceIdT ResId,
                             SaHpiRdrTypeT type) {
	int i = 0;
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


static SaErrorT new_control(struct oh_handler_state * state,
                            SaHpiResourceIdT ResId,
                            struct sim_control *mycontrol) {
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;
        struct sim_control_info *info;

	// Copy information from rdr array to res_rdr
	res_rdr.RdrType = SAHPI_CTRL_RDR;
	memcpy(&res_rdr.RdrTypeUnion.CtrlRec, &mycontrol->control,
               sizeof(SaHpiCtrlRecT));

	oh_init_textbuffer(&res_rdr.IdString);
	oh_append_textbuffer(&res_rdr.IdString, mycontrol->comment);
	res_rdr.RdrTypeUnion.CtrlRec.Num =
                sim_get_next_control_num(state->rptcache, ResId,
                                         res_rdr.RdrTypeUnion.CtrlRec.Type);
	res_rdr.RecordId =
                get_rdr_uid(SAHPI_CTRL_RDR, res_rdr.RdrTypeUnion.CtrlRec.Num);

	RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
	if(!RptEntry){
		dbg("NULL rpt pointer\n");
                return SA_ERR_HPI_INVALID_RESOURCE;
	} else {
		res_rdr.Entity = RptEntry->ResourceEntity;
	}

        //set up our private data
        info = (struct sim_control_info *)g_malloc(sizeof(struct sim_control_info));
	if(!info){
		dbg("NULL rpt pointer\n");
                return SA_ERR_HPI_OUT_OF_MEMORY;
	}
        info->mode = mycontrol->mode;

        // everything ready so add the rdr and extra info to the rptcache
        sim_inject_rdr(state, ResId, &res_rdr, info);

	return 0;
}


SaErrorT sim_discover_controls(struct oh_handler_state * state) {
        SaErrorT rc;

        /* chassis sensors */
        int i = 0;
        while (sim_chassis_controls[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_CHASSIS, &sim_chassis_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding chassis control", rc);
                } else {
                        i++;
                }
        }
        dbg("%d chassis sensors injected", i);

        /* cpu sensors */
        i = 0;
        while (sim_cpu_sensors[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_CPU, &sim_cpu_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding cpu control", rc);
                } else {
                        i++;
                }
        }
        dbg("%d cpu sensors injected", i);

        /* dasd sensors */
        i = 0;
        while (sim_dasd_sensors[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_DASD, &sim_dasd_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding dasd control", rc);
                } else {
                        i++;
                }
        }
        dbg("%d dasd sensors injected", i);

        /* fan sensors */
        i = 0;
        while (sim_fan_sensors[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_FAN, &sim_fan_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding fan control", rc);
                } else {
                        i++;
                }
        }
        dbg("%d fan sensors injected", i);

        return 0;
}

