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


/* get the address of the rptcache */
RPTable * sim_inject_get_rptcache(oHpiHandlerIdT id) {
        oh_handler_state *state;
        oHpiHandlerInfoT info;
        SaErrorT rc;

        /* check arguments */
        if (data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rc = oHpiHandlerInfo(id, &info);
        if (rc) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        return ((oh_handler_state *)state.hnd)->rptcache;
}


/* inject a resource */
SaErrorT sim_inject_resource(oHpiHandlerIdT id, void *data) {
        oh_handler_state *state;
        oHpiHandlerInfoT info;

        /* check arguments */
        if (data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rc = oHpiHandlerInfo(id, &info);
        if (rc) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        oh_add_resource(((oh_handler_state *)state->hnd)->rptcache,
                        (SaHpiRptEntryT *)data, NULL, FREE_RPT_DATA);
        return SA_OK;
}


/* inject a resource */
SaErrorT sim_inject_rdr(oHpiHandlerIdT id, SaHpiResourceIdT resid, void *data) {
        oh_handler_state *state;
        oHpiHandlerInfoT info;

        /* check arguments */
        if (data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rc = oHpiHandlerInfo(id, &info);
        if (rc) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
	oh_add_rdr(((oh_handler_state *)state->hnd)->rptcache,
                   resid, (SaHpiRdrT *)data, NULL, 0);
        return SA_OK;
}


/* inject an event */
SaErrorT sim_inject_event(oHpiHandlerIdT id, void *data) {
        oh_handler_state *state;
        oHpiHandlerInfoT info;

        /* check arguments */
        if (data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rc = oHpiHandlerInfo(id, &info);
        if (rc) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        g_async_queue_push(handle->eventq_async,
	oh_add_rdr(((oh_handler_state *)state->hnd)->eventq_async,
                   (struct oh_event *)data);
        return SA_OK;
}

