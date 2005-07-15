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
 *      W. David Ashley <dashley@us.ibm.com>
 */


#include <sim_init.h>


static SaErrorT new_watchdog(struct oh_handler_state * state,
                             SaHpiResourceIdT ResId,
                             struct sim_watchdog *mywatchdog) {
        SaHpiRdrT res_rdr;
        SaHpiRptEntryT *RptEntry;
        struct simWatchdogInfo *info = NULL;

        // set up res_rdr
        res_rdr.RdrType = SAHPI_WATCHDOG_RDR;
        memcpy(&res_rdr.RdrTypeUnion.WatchdogRec,
               &mywatchdog->watchdogrec, sizeof(SaHpiWatchdogRecT));
        oh_init_textbuffer(&res_rdr.IdString);
        oh_append_textbuffer(&res_rdr.IdString, mywatchdog->comment);

        // get the RptEntry
        RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
        if(!RptEntry){
                dbg("NULL rpt pointer\n");
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        else {
                res_rdr.Entity = RptEntry->ResourceEntity;
        }

        // set up our private info
	info = (struct simWatchdogInfo *)g_malloc0(sizeof(struct simWatchdogInfo));
        if(!info){
                dbg("Out of memory creating watchdog info");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        memcpy(&info->watchdog,&mywatchdog->wd, sizeof(SaHpiWatchdogT));

        /* everything ready so inject the rdr */
        sim_inject_rdr(state, ResId, &res_rdr, info);

        return 0;
}


SaErrorT sim_discover_chassis_watchdogs(struct oh_handler_state * state,
                                        SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_chassis_watchdogs[i].watchdogrec.WatchdogNum != 0) {
                rc = new_watchdog(state, resid, &sim_chassis_watchdogs[i]);
                if (rc) {
                        dbg("Error %d returned when adding chassis watchdog", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d chassis watchdogs injected", j, i);


	return 0;
}


SaErrorT sim_discover_cpu_watchdogs(struct oh_handler_state * state,
                                    SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_cpu_watchdogs[i].watchdogrec.WatchdogNum != 0) {
                rc = new_watchdog(state, resid, &sim_cpu_watchdogs[i]);
                if (rc) {
                        dbg("Error %d returned when adding cpu watchdog", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d cpu watchdogs injected", j, i);

	return 0;
}


SaErrorT sim_discover_dasd_watchdogs(struct oh_handler_state * state,
                                     SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_dasd_watchdogs[i].watchdogrec.WatchdogNum != 0) {
                rc = new_watchdog(state, resid, &sim_dasd_watchdogs[i]);
                if (rc) {
                        dbg("Error %d returned when adding dasd watchdog", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d dasd watchdogs injected", j, i);

	return 0;
}


SaErrorT sim_discover_fan_watchdogs(struct oh_handler_state * state,
                                    SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_fan_watchdogs[i].watchdogrec.WatchdogNum != 0) {
                rc = new_watchdog(state, resid, &sim_fan_watchdogs[i]);
                if (rc) {
                        dbg("Error %d returned when adding fan watchdog", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d fan watchdogs injected", j, i);

	return 0;
}

SaErrorT sim_get_watchdog_info(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
        struct simWatchdogInfo *info;

	if (!hnd) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return SA_ERR_HPI_CAPABILITY;
	}

        /* get our private info */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_WATCHDOG_RDR, num);
        if (rdr == NULL)
                return SA_ERR_HPI_NOT_PRESENT;
        info = (struct simWatchdogInfo *)oh_get_rdr_data(state->rptcache, rid,
                                                         rdr->RecordId);
        if (info == NULL) {
                dbg("No watchdog data. Watchdog=%s", rdr->IdString.Data);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memcpy(wdt, &info->watchdog, sizeof(SaHpiWatchdogT));
	return SA_OK;
}

SaErrorT sim_set_watchdog_info(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
        struct simWatchdogInfo *info;

	if (!hnd) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return SA_ERR_HPI_CAPABILITY;
	}

        /* get our private info */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_WATCHDOG_RDR, num);
        if (rdr == NULL)
                return SA_ERR_HPI_NOT_PRESENT;
        info = (struct simWatchdogInfo *)oh_get_rdr_data(state->rptcache, rid,
                                                         rdr->RecordId);
        if (info == NULL) {
                dbg("No watchdog data. Watchdog=%s", rdr->IdString.Data);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memcpy(&info->watchdog, wdt, sizeof(SaHpiWatchdogT));
	return SA_OK;
}

SaErrorT sim_reset_watchdog(void *hnd,
				SaHpiResourceIdT rid,
				SaHpiWatchdogNumT num)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return SA_ERR_HPI_CAPABILITY;
	}

        // since no timer is actually running we can just do nothing here
	return SA_OK;
}


void * oh_get_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("sim_get_watchdog_info")));

void * oh_set_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("sim_set_watchdog_info")));

void * oh_reset_watchdog (void *, SaHpiResourceIdT, SaHpiWatchdogNumT)
                __attribute__ ((weak, alias("sim_reset_watchdog")));

