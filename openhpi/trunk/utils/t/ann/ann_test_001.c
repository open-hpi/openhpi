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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_error.h>
#include <oh_utils.h>


/**
 * main: Announcement test
 *
 * This test tests the creation of an announcement list.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        oh_announcement *ann;

        ann = oh_announcement_create();

        if(ann == NULL) {
                CRIT("ann pointer == NULL.");
                return 1;
        }

        if(ann->nextId != SAHPI_OLDEST_ENTRY + 1) {
                CRIT("ann->nextId invalid.");
                return 1;
        }


        if(ann->annentries != NULL) {
                CRIT("ann->annentries invalid.");
                return 1;
        }

        return 0;
}
