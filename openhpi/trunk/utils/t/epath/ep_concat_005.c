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
 * ep_concat test5.
 *   concatenate two 3 element entity path and verify result
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{SAHPI_ENT_PERIPHERAL_BAY,55},
                                 {SAHPI_ENT_SYS_MGMNT_MODULE,66},
                                 {SAHPI_ENT_SYSTEM_BOARD,77},
                                 {SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep2 = {{{SAHPI_ENT_MEMORY_MODULE,88},
                                 {SAHPI_ENT_PROCESSOR_MODULE,99},
                                 {SAHPI_ENT_POWER_SUPPLY,101},
                                 {SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep3 = {{{SAHPI_ENT_PERIPHERAL_BAY,55},
                                 {SAHPI_ENT_SYS_MGMNT_MODULE,66},
                                 {SAHPI_ENT_SYSTEM_BOARD,77},
                                 {SAHPI_ENT_MEMORY_MODULE,88},
                                 {SAHPI_ENT_PROCESSOR_MODULE,99},
                                 {SAHPI_ENT_POWER_SUPPLY,101},
                                 {SAHPI_ENT_ROOT,0}}};
        int mydebug = 0;

        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test5 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test5 checkpoint 2 failed\n");
                return 1;
        }
        if (mydebug) printf("ep_concat test5 OK\n");
        return 0;
}
