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
 *      W. david Ashley <dashley@us.ibm.com>
 *
 */


#include <SaHpi.h>
#include <glib.h>
#include <oh_utils.h>
#include <sim_injector.h>


/* inject a resource */
SaErrorT sim_inject_resource(struct oh_handler_state *state,
                             SaHpiRptEntryT *data) {

        /* check arguments */
        if (state == NULL || data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        oh_add_resource(state->rptcache, data, NULL, FREE_RPT_DATA);
        return SA_OK;
}


/* inject a resource */
SaErrorT sim_inject_rdr(struct oh_handler_state *state, SaHpiResourceIdT resid,
                        SaHpiRdrT *data) {

        /* check arguments */
        if (state == NULL || resid = 0 || data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
	oh_add_rdr(state->rptcache, resid, data, NULL, 0);
        return SA_OK;
}


/* inject an event */
SaErrorT sim_inject_event(struct oh_handler_state *state, struct oh_event *data) {

        /* check arguments */
        if (state== NULL || data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        g_async_queue_push(handle->eventq_async,
	oh_add_rdr(state->eventq_async, data);
        return SA_OK;
}

