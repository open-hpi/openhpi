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
	res_rdr.RecordId =
                get_rdr_uid(SAHPI_CTRL_RDR, res_rdr.RdrTypeUnion.CtrlRec.Num);

	RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
	if (!RptEntry) {
		dbg("NULL rpt pointer\n");
                return SA_ERR_HPI_INVALID_RESOURCE;
	}
	memcpy(&res_rdr.Entity, &RptEntry->ResourceEntity,
               sizeof(SaHpiEntityPathT));

        //set up our private data
        info = (struct sim_control_info *)g_malloc(sizeof(struct sim_control_info));
	if (!info) {
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

        /* chassis controls */
        int i = 0;
        int j = 0;
        while (sim_chassis_controls[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_CHASSIS, &sim_chassis_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding chassis control", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d chassis controls injected", j, i);

        /* cpu controls */
        i = 0;
        j = 0;
        while (sim_cpu_controls[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_CPU, &sim_cpu_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding cpu control", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d cpu controls injected", j, i);

        /* dasd controls */
        i = 0;
        j = 0;
        while (sim_dasd_controls[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_DASD, &sim_dasd_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding dasd control", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d dasd controls injected", j, i);

        /* fan controls */
        i = 0;
        j = 0;
        while (sim_fan_controls[i].index != 0) {
                rc = new_control(state, SIM_RPT_ENTRY_FAN, &sim_fan_controls[i]);
                if (rc) {
                        dbg("Error %d returned when adding fan control", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d fan controls injected", j, i);

        return 0;
}

