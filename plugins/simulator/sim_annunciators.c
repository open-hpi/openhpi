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


static SaErrorT new_annunciator(struct oh_handler_state * state,
                                SaHpiResourceIdT ResId,
                                struct sim_annunciator * myannun) {
        SaHpiRdrT res_rdr;
        SaHpiRptEntryT *RptEntry;
        struct simAnnunciatorInfo *info = NULL;

        // set up res_rdr
        res_rdr.RdrType = SAHPI_ANNUNCIATOR_RDR;
        memcpy(&res_rdr.RdrTypeUnion.AnnunciatorRec,
               &myannun->annun, sizeof(SaHpiAnnunciatorRecT));
        oh_init_textbuffer(&res_rdr.IdString);
        oh_append_textbuffer(&res_rdr.IdString, myannun->comment);

        // get the RptEntry
        RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
        if(!RptEntry){
                dbg("NULL rpt pointer\n");
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        else {
                res_rdr.Entity = RptEntry->ResourceEntity;
        }

        // save the announcements for the annunciator
        int i = 0;
        while (myannun->announs[i].EntryId != 0) {
                if (info == NULL) {
                        info = (struct simAnnunciatorInfo *)g_malloc(sizeof(struct simAnnunciatorInfo *));
                        if (info == NULL) {
                                return SA_ERR_HPI_OUT_OF_SPACE;
                        }
                        // set the default mode value
                        info->mode = SAHPI_ANNUNCIATOR_MODE_SHARED;
                        // set up the announcement list
                        info->announs = oh_announcement_create();
                        if (info->announs == NULL) {
                                return SA_ERR_HPI_OUT_OF_SPACE;
                        }
                }
                oh_announcement_append(info->announs, &myannun->announs[i]);
                i++;
        }

        /* everything ready so inject the rdr */
        sim_inject_rdr(state, ResId, &res_rdr, info);

        return 0;
}


SaErrorT sim_discover_chassis_annunciators(struct oh_handler_state * state,
                                           SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_chassis_annunciators[i].index != 0) {
                rc = new_annunciator(state, resid, &sim_chassis_annunciators[i]);
                if (rc) {
                        dbg("Error %d returned when adding chassis annunciator", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d chassis annunciators injected", j, i);

	return 0;
}


SaErrorT sim_discover_cpu_annunciators(struct oh_handler_state * state,
                                       SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_cpu_annunciators[i].index != 0) {
                rc = new_annunciator(state, resid, &sim_cpu_annunciators[i]);
                if (rc) {
                        dbg("Error %d returned when adding cpu annunciator", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d cpu annunciators injected", j, i);

	return 0;
}


SaErrorT sim_discover_dasd_annunciators(struct oh_handler_state * state,
                                        SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_dasd_annunciators[i].index != 0) {
                rc = new_annunciator(state, resid, &sim_dasd_annunciators[i]);
                if (rc) {
                        dbg("Error %d returned when adding dasd annunciator", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d dasd annunciators injected", j, i);

	return 0;
}


SaErrorT sim_discover_fan_annunciators(struct oh_handler_state * state,
                                       SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_fan_annunciators[i].index != 0) {
                rc = new_annunciator(state, resid, &sim_fan_annunciators[i]);
                if (rc) {
                        dbg("Error %d returned when adding fan annunciator", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d fan annunciators injected", j, i);

	return 0;
}

