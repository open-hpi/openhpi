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
 * set_ep_instance test3.
 *   call set_ep_instance with entity path that has 2 elements
 *   expected result: only one element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_BIOS, 1},{SAHPI_ENT_UNKNOWN, 2},{0}}};
        SaHpiEntityLocationT x = 777;
        int mydebug = 0;
         
        if (mydebug) printf(" test3\n");
        if(set_ep_instance(&ep, SAHPI_ENT_UNKNOWN, x)) {
                if (mydebug) printf("set_ep_inst test3 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[1].EntityLocation != x) {
                if (mydebug) printf("set_ep_inst test3 failed, entInst %d != %d\n",
                                   ep.Entry[1].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_UNKNOWN) {
                if (mydebug) printf("set_ep_inst test3 failed, entType %d != SAHPI_ENT_UNKNOWN\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }
        if (ep.Entry[0].EntityLocation != 1) {
                if (mydebug) printf("set_ep_inst test3 failed, entInst %d != 1\n",
                                   ep.Entry[0].EntityLocation);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_BIOS) {
                if (mydebug) printf("set_ep_inst test3 failed, entType %d != SAHPI_ENT_BIOS\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        return 0;
}
