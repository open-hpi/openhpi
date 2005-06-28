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

SaErrorT sim_get_watchdog_info(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Watchdog is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

SaErrorT sim_set_watchdog_info(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Watchdog is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

SaErrorT sim_reset_watchdog(void *hnd,
				SaHpiResourceIdT rid,
				SaHpiWatchdogNumT num)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Watchdog is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


void * oh_get_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("sim_get_watchdog_info")));

void * oh_set_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("sim_set_watchdog_info")));

void * oh_reset_watchdog (void *, SaHpiResourceIdT, SaHpiWatchdogNumT)
                __attribute__ ((weak, alias("sim_reset_watchdog")));

