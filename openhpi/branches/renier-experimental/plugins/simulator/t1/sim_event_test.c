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
#include <string.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <sahpi_struct_utils.h>


int main(int argc, char **argv)
{
    SaHpiSessionIdT sid = 0;
    SaErrorT rc = SA_OK;
    SaHpiEventT event;
    SaHpiRdrT rdr;
    SaHpiRptEntryT rpt;

    rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
	if(rc != SA_OK) {
        printf("Error: session open. rc = %d\n", rc);
		return -1;
    }

	rc = saHpiDiscover(sid);
	if (rc != SA_OK) {
        printf("Error: discover error. rc = %d\n", rc);
		return -1;
    }

    rc = saHpiSubscribe(sid);
	if (rc != SA_OK) {
        printf("Error: subscribe. rc = %d\n", rc);
		return -1;
    }

    /* loop forever displaying events */
    while(SAHPI_TRUE) {
        rc = saHpiEventGet(sid, SAHPI_TIMEOUT_BLOCK, &event, &rdr, &rpt, NULL);
        if (rc != SA_OK) {
            printf("Error: event get. rc = %d\n", rc);
            return -1;
        }

        /* print the event info */
        oh_print_event(&event, 0);
        oh_print_rptentry(&rpt, 0);
        oh_print_rdr(&rdr, 0);
        printf("\n\n");
    }

    saHpiSessionClose(sid);

    return 0;
}
