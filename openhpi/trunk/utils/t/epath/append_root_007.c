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
 *
 */

#include <string.h>
#include <stdio.h>
#include <SaHpi.h>
#include <epath_utils.h>

/**
 * append_root test 7.
 *          append root to fully populated entity path with root element at end
 *          expected result: no change, root element still at end
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        int i;
        int mydebug = 0;

        for (i=0; i<SAHPI_MAX_ENTITY_PATH-2; i++) {
                ep.Entry[i].EntityType = SAHPI_ENT_GROUP;
                ep.Entry[i].EntityInstance = i+1;
        }
        ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType = SAHPI_ENT_ROOT;
        ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityInstance = 0;

        if(append_root(&ep)) {
                if (mydebug) printf("append_root function call failed\n");
                return 1;
        }

        for (i=0; i<SAHPI_MAX_ENTITY_PATH-2; i++) {
                if (ep.Entry[i].EntityType == SAHPI_ENT_ROOT) {
                        if (mydebug) printf("append_root test failed, root found at element %d\n",
                                             i);
                        return 1;
                }
        }
        if (ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType != SAHPI_ENT_ROOT) {
                if (mydebug) printf("append_root test failed, %d != SAHPI_ENT_ROOT\n",
                                     ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType);
                return 1;
        }

        return 0;
}
