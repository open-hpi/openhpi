/* -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renierm@users.sf.net>
 */

#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <rpt_resources.h>

/**
 * main: Starting with an empty RPTable, adds 1 resource with an invalid
 * entity path (has a bogus entity type) to it.
 * Passes the test if the interface returns an error, else it fails.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        RPTable *rptable = (RPTable *)g_malloc0(sizeof(RPTable));
        oh_init_rpt(rptable, NULL);
        guint i;

        for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
                if (rptentries[0].ResourceEntity.Entry[i].EntityType == SAHPI_ENT_ROOT) {
                        rptentries[0].ResourceEntity.Entry[i].EntityType = 999999;
                        break;
                }
        }
        
        if (!oh_add_resource(rptable, rptentries, NULL, 1))
                return 1;

        return 0;
}
