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
 * main: append_root test.
 *          append root to fully populated entity path that has no root element
 *          expected result: no change, root element still not in entity path
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        int i;
        int mydebug = 0;

        for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
                ep.Entry[i].EntityType = SAHPI_ENT_GROUP;
                ep.Entry[i].EntityInstance = i + 1;
        }

        for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
                if (ep.Entry[i].EntityType == SAHPI_ENT_ROOT) {
                        if (mydebug) printf("append_root test failed, root found at element %d\n",
                                             i);
                        return 1;
                }
        }

        return 0;
}
