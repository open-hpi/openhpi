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
 * ep_concat test7.
 *   concatenate a 4 and a 12 element entity path and verify result
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep1 = {{{SAHPI_ENT_POWER_UNIT,199},
                                 {SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD,202},
                                 {SAHPI_ENT_SYSTEM_CHASSIS,211},
                                 {SAHPI_ENT_SUB_CHASSIS,222},
                                 {0}}};
        SaHpiEntityPathT ep2 = {{{SAHPI_ENT_OTHER_CHASSIS_BOARD,233},
                                 {SAHPI_ENT_DISK_DRIVE_BAY,244},
                                 {SAHPI_ENT_PERIPHERAL_BAY_2,255},
                                 {SAHPI_ENT_DEVICE_BAY,255},
                                 {SAHPI_ENT_COOLING_DEVICE,277},
                                 {SAHPI_ENT_COOLING_UNIT,288},
                                 {SAHPI_ENT_INTERCONNECT,299},
                                 {SAHPI_ENT_MEMORY_DEVICE,303},
                                 {SAHPI_ENT_SYS_MGMNT_SOFTWARE,311},
                                 {SAHPI_ENT_BIOS,322},
                                 {SAHPI_ENT_OPERATING_SYSTEM,333},
                                 {SAHPI_ENT_SYSTEM_BUS,344},
                                 {0}}};
        SaHpiEntityPathT ep3 = {{{SAHPI_ENT_POWER_UNIT,199},
                                 {SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD,202},
                                 {SAHPI_ENT_SYSTEM_CHASSIS,211},
                                 {SAHPI_ENT_SUB_CHASSIS,222},
                                 {SAHPI_ENT_OTHER_CHASSIS_BOARD,233},
                                 {SAHPI_ENT_DISK_DRIVE_BAY,244},
                                 {SAHPI_ENT_PERIPHERAL_BAY_2,255},
                                 {SAHPI_ENT_DEVICE_BAY,255},
                                 {SAHPI_ENT_COOLING_DEVICE,277},
                                 {SAHPI_ENT_COOLING_UNIT,288},
                                 {SAHPI_ENT_INTERCONNECT,299},
                                 {SAHPI_ENT_MEMORY_DEVICE,303},
                                 {SAHPI_ENT_SYS_MGMNT_SOFTWARE,311},
                                 {SAHPI_ENT_BIOS,322},
                                 {SAHPI_ENT_OPERATING_SYSTEM,333},
                                 {SAHPI_ENT_SYSTEM_BUS,344}}};
        int mydebug = 0;

        if (ep_concat(&ep1, &ep2)) {
                if (mydebug) printf("ep_concat test7 checkpoint 1 failed\n");
                return 1;
        }
        if (ep_cmp(&ep1, &ep3)) {
                if (mydebug) printf("ep_concat test7 checkpoint 2 failed\n");
                return 1;
        }

        if (mydebug) printf("ep_concat test7 OK\n");
        return 0;
}
