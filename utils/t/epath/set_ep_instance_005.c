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
 * set_ep_instance test5.
 *   call set_ep_instance with entity path that has 4 elements
 *   expected result: only one element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_PROCESSOR, 111},
                                {SAHPI_ENT_DISK_BAY, 2222},
                                {SAHPI_ENT_BATTERY, 33333},
                                {SAHPI_ENT_IO_SUBBOARD, 444444},
                                {0}}};
        SaHpiEntityLocationT x = 10101010;
        int mydebug = 0;
         
        if (mydebug) printf(" test5\n");
        if(set_ep_instance(&ep, SAHPI_ENT_IO_SUBBOARD, x)) {
                if (mydebug) printf("set_ep_inst test5 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[3].EntityLocation != x) {
                if (mydebug) printf("set_ep_inst test5 failed, entInst %d != %d\n",
                                   ep.Entry[3].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[3].EntityType != SAHPI_ENT_IO_SUBBOARD) {
                if (mydebug) printf("set_ep_inst test5 failed, entType %d != SAHPI_ENT_IO_SUBBOARD\n",
                                   ep.Entry[3].EntityType);
                return 1;
        }
        if (ep.Entry[0].EntityLocation != 111) {
                if (mydebug) printf("set_ep_inst test5 failed, entInst %d != 111\n",
                                   ep.Entry[0].EntityLocation);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_PROCESSOR) {
                if (mydebug) printf("set_ep_inst test5 failed, entType %d != SAHPI_ENT_PROCESSOR\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        if (ep.Entry[1].EntityLocation != 2222) {
                if (mydebug) printf("set_ep_inst test5 failed, entInst %d != 2222\n",
                                   ep.Entry[1].EntityLocation);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_DISK_BAY) {
                if (mydebug) printf("set_ep_inst test5 failed, entType %d! = SAHPI_ENT_DISK_BAY\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }
        if (ep.Entry[2].EntityLocation != 33333) {
                if (mydebug) printf("set_ep_inst test5 failed, entInst %d != 33333\n",
                                   ep.Entry[2].EntityLocation);
                return 1;
        }
        if (ep.Entry[2].EntityType != SAHPI_ENT_BATTERY) {
                if (mydebug) printf("set_ep_inst test5 failed, entType %d != SAHPI_ENT_BATTERY\n",
                                   ep.Entry[2].EntityType);
                return 1;
        }

        return 0;
}
