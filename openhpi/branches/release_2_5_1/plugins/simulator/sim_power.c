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


SaErrorT sim_get_power_state(void *hnd,
	        	     SaHpiResourceIdT rid,
			     SaHpiPowerStateT *pwrstate)
{
	if (!hnd || !pwrstate) {
		dbg("Invalid parameter");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has power capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
		return SA_ERR_HPI_CAPABILITY;
	}

	/* for the simulator the power state is always on */
        *pwrstate = SAHPI_POWER_ON;
        return SA_OK;
}


SaErrorT sim_set_power_state(void *hnd,
			     SaHpiResourceIdT rid,
			     SaHpiPowerStateT pwrstate)
{
	if (!hnd || NULL == oh_lookup_powerstate(pwrstate)) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has power capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
		return SA_ERR_HPI_CAPABILITY;
	}

	/* for the simulator the power state cannot be turned off */
        if (pwrstate == SAHPI_POWER_ON) {
                return SA_OK;
        }
        return SA_ERR_HPI_CAPABILITY;
}


void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
                __attribute__ ((weak, alias("sim_get_power_state")));

void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
                __attribute__ ((weak, alias("sim_set_power_state")));


