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
 * set_ep_instance test6.
 *   call set_ep_instance with entity path that has 4 elements, victim element in the middle
 *   expected result: only victim element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_FAN, 11099},
                                {SAHPI_ENT_BATTERY, 2002},
                                {SAHPI_ENT_RACK, 37373},
                                {SAHPI_ENT_DISK_BAY, 440044},
                                {0}}};
        SaHpiEntityLocationT x = 123456;
        int mydebug = 0;
         
        if (mydebug) printf(" test6\n");
        if(set_ep_instance(&ep, SAHPI_ENT_RACK, x)) {
                if (mydebug) printf("set_ep_inst test6 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[2].EntityLocation != x) {
                if (mydebug) printf("set_ep_inst test6 failed, entInst %d != %d\n",
                                   ep.Entry[2].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[2].EntityType != SAHPI_ENT_RACK) {
                if (mydebug) printf("set_ep_inst test6 failed, entType %d != SAHPI_ENT_RACK\n",
                                   ep.Entry[2].EntityType);
                return 1;
        }
        if (ep.Entry[0].EntityLocation != 11099) {
                if (mydebug) printf("set_ep_inst test6 failed, entInst %d != 11099\n",
                                   ep.Entry[0].EntityLocation);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_FAN) {
                if (mydebug) printf("set_ep_inst test6 failed, entType %d != SAHPI_ENT_FAN\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        if (ep.Entry[1].EntityLocation != 2002) {
                if (mydebug) printf("set_ep_inst test6 failed, entInst %d != 2002\n",
                                   ep.Entry[1].EntityLocation);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_BATTERY) {
                if (mydebug) printf("set_ep_inst test6 failed, entType %d! = SAHPI_ENT_BATTERY\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }
        if (ep.Entry[3].EntityLocation != 440044) {
                if (mydebug) printf("set_ep_inst test6 failed, entInst %d != 440044\n",
                                   ep.Entry[3].EntityLocation);
                return 1;
        }
        if (ep.Entry[3].EntityType != SAHPI_ENT_DISK_BAY) {
                if (mydebug) printf("set_ep_inst test6 failed, entType %d != SAHPI_ENT_DISK_BAY\n",
                                   ep.Entry[3].EntityType);
                return 1;
        }

        return 0;
}
