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
 * ep_concat test12.
 *   concatenate a 15 element and a zero element entity path and verify
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{SAHPI_ENT_GROUP,1},
                                 {SAHPI_ENT_REMOTE,2},
                                 {SAHPI_ENT_EXTERNAL_ENVIRONMENT,3},
                                 {SAHPI_ENT_BATTERY,4},
                                 {SAHPI_ENT_CHASSIS_SPECIFIC,5},
                                 {SAHPI_ENT_BOARD_SET_SPECIFIC,6},
                                 {SAHPI_ENT_OEM_SYSINT_SPECIFIC,7},
                                 {SAHPI_ENT_FAN,8},
                                 {SAHPI_ENT_RACK,9},
                                 {SAHPI_ENT_SUBRACK,10},
                                 {SAHPI_ENT_COMPACTPCI_CHASSIS,11},
                                 {SAHPI_ENT_ADVANCEDTCA_CHASSIS,12},
                                 {SAHPI_ENT_PHYSICAL_SLOT,13},
                                 {SAHPI_ENT_SBC_BLADE,14},
                                 {SAHPI_ENT_IO_BLADE,15},
                                 {SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep2 = {{{SAHPI_ENT_ROOT,0}}};
        SaHpiEntityPathT ep3 = {{{SAHPI_ENT_GROUP,1},
                                 {SAHPI_ENT_REMOTE,2},
                                 {SAHPI_ENT_EXTERNAL_ENVIRONMENT,3},
                                 {SAHPI_ENT_BATTERY,4},
                                 {SAHPI_ENT_CHASSIS_SPECIFIC,5},
                                 {SAHPI_ENT_BOARD_SET_SPECIFIC,6},
                                 {SAHPI_ENT_OEM_SYSINT_SPECIFIC,7},
                                 {SAHPI_ENT_FAN,8},
                                 {SAHPI_ENT_RACK,9},
                                 {SAHPI_ENT_SUBRACK,10},
                                 {SAHPI_ENT_COMPACTPCI_CHASSIS,11},
                                 {SAHPI_ENT_ADVANCEDTCA_CHASSIS,12},
                                 {SAHPI_ENT_PHYSICAL_SLOT,13},
                                 {SAHPI_ENT_SBC_BLADE,14},
                                 {SAHPI_ENT_IO_BLADE,15},
                                 {SAHPI_ENT_ROOT,0}}};
        int mydebug = 0;

        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test12 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test12 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test12 OK\n");
        return 0;
}
