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
 * set_ep_instance test10.
 *   call set_ep_instance with entity type not in multi element entity path
 *   expected result: set_ep_instance returns failure
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_GROUP, 100},
                                {SAHPI_ENT_REMOTE, 200},
                                {SAHPI_ENT_EXTERNAL_ENVIRONMENT, 300},
                                {SAHPI_ENT_BATTERY, 400},
                                {SAHPI_ENT_CHASSIS_SPECIFIC, 500},
                                {SAHPI_ENT_BOARD_SET_SPECIFIC, 600},
                                {SAHPI_ENT_OEM_SYSINT_SPECIFIC, 700},
                                {SAHPI_ENT_ROOT, 800},
                                {SAHPI_ENT_RACK, 900},
                                {SAHPI_ENT_SUBRACK, 1000},
                                {0}}};
        SaHpiEntityInstanceT x = 11000;
        int mydebug = 0;
         
        if (mydebug) printf(" test10\n");
        if(set_ep_instance(&ep, SAHPI_ENT_FAN, x) == 0) {
                if (mydebug) printf("set_ep_inst test10 checkpoint 1 failed\n");
                return 1;
        }
        if((ep.Entry[0].EntityInstance != 100) ||
           (ep.Entry[0].EntityType != SAHPI_ENT_GROUP) ||
           (ep.Entry[1].EntityInstance != 200) ||
           (ep.Entry[1].EntityType != SAHPI_ENT_REMOTE) ||
           (ep.Entry[2].EntityInstance != 300) ||
           (ep.Entry[2].EntityType != SAHPI_ENT_EXTERNAL_ENVIRONMENT) ||
           (ep.Entry[3].EntityInstance != 400) ||
           (ep.Entry[3].EntityType != SAHPI_ENT_BATTERY) ||
           (ep.Entry[4].EntityInstance != 500) ||
           (ep.Entry[4].EntityType != SAHPI_ENT_CHASSIS_SPECIFIC) ||
           (ep.Entry[5].EntityInstance != 600) ||
           (ep.Entry[5].EntityType != SAHPI_ENT_BOARD_SET_SPECIFIC) ||
           (ep.Entry[6].EntityInstance != 700) ||
           (ep.Entry[6].EntityType != SAHPI_ENT_OEM_SYSINT_SPECIFIC) ||
           (ep.Entry[7].EntityInstance != 800) ||
           (ep.Entry[7].EntityType != SAHPI_ENT_ROOT) ||
           (ep.Entry[8].EntityInstance != 900) ||
           (ep.Entry[8].EntityType != SAHPI_ENT_RACK) ||
           (ep.Entry[9].EntityInstance != 1000) ||
           (ep.Entry[9].EntityType != SAHPI_ENT_SUBRACK)) {
                if (mydebug) printf("set_ep_inst test10 checkpoint 2 failed\n");
                return 1;
        }

        return 0;
}
