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


SaErrorT sim_get_hotswap_state(void *hnd,
			       SaHpiResourceIdT rid,
			       SaHpiHsStateT    *hsstate)
{
	if (!hnd || !hsstate) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_set_hotswap_state(void *hnd,
			       SaHpiResourceIdT rid,
			       SaHpiHsStateT    hsstate)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsstate(hsstate)) {
		dbg("Invalid hotswap state.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


SaErrorT sim_request_hotswap_action(void *hnd,
				    SaHpiResourceIdT rid,
				    SaHpiHsActionT act)
{
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (NULL == oh_lookup_hsaction(act)) {
		dbg("Invalid hotswap action.");
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Managed Hotswap is not supported by platform");
	return(SA_ERR_HPI_INTERNAL_ERROR);
}


void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("sim_get_hotswap_state")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("sim_set_hotswap_state")));

void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("sim_request_hotswap_action")));


