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
 * ep_concat test4.
 *   concatenate 2 two element entity path and verify result
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{SAHPI_ENT_UNKNOWN,11},
                                 {SAHPI_ENT_OTHER,22},
                                 {SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep2 = {{{SAHPI_ENT_PROCESSOR,39},
                                 {SAHPI_ENT_DISK_BAY,44},
                                 {SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep3 = {{{SAHPI_ENT_UNKNOWN,11},
                                 {SAHPI_ENT_OTHER,22},
                                 {SAHPI_ENT_PROCESSOR,39},
                                 {SAHPI_ENT_DISK_BAY,44},
                                 {SAHPI_ENT_ROOT,0}}};
        int mydebug = 0;

        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test4 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test4 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test4 OK\n");
        return 0;
}
