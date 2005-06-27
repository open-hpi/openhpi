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
 *	Authors:
 *     	Sean Dague <http://dague.net/sean>
*/

#include <stdlib.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include <sahpi_struct_utils.h>
#include <oh_error.h>


/**
 * Run a series of sanity tests on the simulator
 * Return 0 on success, otherwise return -1
 **/

int main(int argc, char **argv)
{
	SaHpiSessionIdT sid = 0;
        SaHpiAnnouncementT announ;
	SaErrorT rc = SA_OK;

        rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
	if (rc != SA_OK) {
		dbg("Failed to open session");
                return -1;
	}

	rc = saHpiDiscover(sid);
	if (rc != SA_OK) {
		dbg("Failed to run discover");
                return -1;
	}

        /* add an announcement */
        announ.EntryId = 0;         // modified by oh_announcement_append
        announ.Timestamp = 0;       // modified by oh_announcement_append
        announ.AddedByUser = FALSE; // modified by oh_announcement_append
        announ.Severity = SAHPI_CRITICAL;
        announ.Acknowledged = FALSE;
        announ.StatusCond.Type= SAHPI_STATUS_COND_TYPE_SENSOR;
        announ.StatusCond.Entity.Entry[0].EntityType = SAHPI_ENT_SYSTEM_BOARD;
        announ.StatusCond.Entity.Entry[0].EntityLocation = 1;
        announ.StatusCond.Entity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        announ.StatusCond.Entity.Entry[1].EntityLocation = 0;
        announ.StatusCond.DomainId = 1;
        announ.StatusCond.ResourceId = 1;
        announ.StatusCond.SensorNum = 1;
        announ.StatusCond.EventState = SAHPI_ES_UNSPECIFIED;
        announ.StatusCond.Name.Length = 5;
        memcpy(&announ.StatusCond.Name.Value,"announ", 5);
        announ.StatusCond.Mid = 123;
        /* we will not worry about the Data field for this test */
        rc = saHpiAnnunciatorAdd(sid, 1, 1, &announ);
        if (rc != SA_OK) {
		dbg("Couldn't add announcement");
		dbg("Error %s",oh_lookup_error(rc));
                return -1;
	}

	return 0;
}

