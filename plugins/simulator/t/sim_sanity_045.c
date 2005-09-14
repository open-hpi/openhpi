/*      -*- linux-c -*-
*
*(C) Copyright IBM Corp. 2005
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
* file and program are licensed under a BSD style license.  See
* the Copying file included with the OpenHPI distribution for
* full licensing terms.
*
* Authors:
*     W. david Ashley <dashley@us.ibm.com>
*/

#include <stdlib.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <sahpi_struct_utils.h>


/**
 * Run a series of sanity tests on the simulator
 * Return 0 on success, otherwise return -1
 **/

int main(int argc, char **argv)
{
	SaHpiSessionIdT sid = 0;
	SaErrorT rc = SA_OK;
    int event_ctr = 0;
    SaHpiEventT event;
    SaHpiRdrT rdr;
    SaHpiRptEntryT rpt;
    SaHpiEvtQueueStatusT status;

    rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
	if(rc != SA_OK)
		return -1;

    rc = saHpiSubscribe(sid);
	if(rc != SA_OK)
		return -1;

	rc = saHpiDiscover(sid);
	if (rc != SA_OK)
		return -1;

    /* count discovery events */
    rc = saHpiEventGet(sid, SAHPI_TIMEOUT_IMMEDIATE, &event, &rdr, &rpt,
                       &status);
    while (rc == SA_OK) {
        oh_print_event(&event, 3);
        event_ctr++;
        rc = saHpiEventGet(sid, SAHPI_TIMEOUT_IMMEDIATE, &event, &rdr, &rpt,
                           &status);
    }
	if (rc != SA_ERR_HPI_TIMEOUT) {
        printf("SaHpiEventGet returned %d.\n", rc);
		return -1;
    }
    /* A FRU device does NOT generate an ADD event. So our Hot Swap Disk
       Drive Bay resource will not generate an ADD event since it is marked
       as a FRU. If you change this, this test will see an additional event.
      */
    if (event_ctr != 4) {
        printf("Incorrect number of events returned. Found %d events.\n",
               event_ctr);
		return -1;
    }

	return 0;
}
