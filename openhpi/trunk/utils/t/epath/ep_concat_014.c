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
 *     Chris Chia <cchia@users.sf.net>
 */

#include <string.h>
#include <stdio.h>

#include <SaHpi.h>
#include <oh_utils.h>

/**
 * ep_concat test14.
 *   concatenate two one less than full entity path and verify result
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1;
        SaHpiEntityPathT ep2;
        SaHpiEntityPathT ep3;
        int i;
        int mydebug = 0;

        for (i=0; i<SAHPI_MAX_ENTITY_PATH-1; i++) {
                ep1.Entry[i].EntityType = SAHPI_ENT_GROUP;
                ep1.Entry[i].EntityLocation = 202;
                ep2.Entry[i].EntityType = SAHPI_ENT_BATTERY;
                ep2.Entry[i].EntityLocation = 404;
                ep3.Entry[i].EntityType = SAHPI_ENT_GROUP;
                ep3.Entry[i].EntityLocation = 202;
        }
        ep1.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType = SAHPI_ENT_ROOT;
        ep1.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityLocation = 0;
        ep2.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType = SAHPI_ENT_ROOT;
        ep2.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityLocation = 0;
        ep3.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType = SAHPI_ENT_BATTERY;
        ep3.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityLocation = 404;
        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test14 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test14 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test14 OK\n");
        return 0;
}
