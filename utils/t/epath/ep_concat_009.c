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
 * ep_concat test9.
 *   concatenate a zero element and a full entity path and verify result
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{0}}};
        SaHpiEntityPathT ep2;
        SaHpiEntityPathT ep3;
        int i;
        int mydebug = 0;

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                ep2.Entry[i].EntityType = SAHPI_ENT_RACK;
                ep2.Entry[i].EntityInstance = 3456;
                ep3.Entry[i].EntityType = SAHPI_ENT_RACK;
                ep3.Entry[i].EntityInstance = 3456;
        }
        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test9 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test9 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test9 OK\n");
        return 0;
}
