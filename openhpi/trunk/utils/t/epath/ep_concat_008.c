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
 * ep_concat test8.
 *   concatenate two full entity path and verify result
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

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                ep1.Entry[i].EntityType = SAHPI_ENT_IO_BLADE;
                ep1.Entry[i].EntityLocation = 896;
                ep3.Entry[i].EntityType = SAHPI_ENT_IO_BLADE;
                ep3.Entry[i].EntityLocation = 896;
                ep2.Entry[i].EntityType = SAHPI_ENT_SBC_BLADE,14;
                ep2.Entry[i].EntityLocation = 123;
        }
        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test8 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test8 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test8 OK\n");
        return 0;
}
