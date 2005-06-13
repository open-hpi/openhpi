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
 * Authors:
 *     Sean Dague <http://dague.net/sean>
*/

#include <stdlib.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>

 /**
  *  * Run a series of sanity tests on the simulator
  *   * Pass on success, otherwise a failure.
  *    **/

 /**
  *  * Utility macro to make it easier to state what failed
  *   */

#define failed(err)                                \
	do {                                            \
		failcount++;                            \
		dbg("Failed Test %d: %s", testnum, err);    \
	} while(0)

#define runtest() testnum++

int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        SaHpiRptEntryT res;
        SaHpiEntryIdT id = SAHPI_FIRST_ENTRY;
        int failcount = 0;
        int testnum = 0;
        SaErrorT rc = SA_OK;

        rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
	if(rc != SA_OK) {
		failed("Failed to open session");
	}

	rc = saHpiDiscover(sid);
	if(rc != SA_OK) {
		failed("Failed to run discover");
	}


        /* loop over all resources, ensure that ResourceTag and
	 *            ManufacturerId have been set */
        while(saHpiRptEntryGet(sid, id, &id, &res) == SA_OK) {
		runtest();
		if(!res.ResourceTag.DataLength) {
			failed("Resource Tag has zero length");
		}
		runtest();
		if(!res.ResourceInfo.ManufacturerId) {
			failed("Resource has no Manufacturer Id");
		}

		/* there should be an inner loop here for Rdrs */
	}

	dbg("Ran %d tests\n", testnum);

	/* if there is any failures, the test fails */

        if(failcount) {
	                return -1;
		        }
        return(0);
}
