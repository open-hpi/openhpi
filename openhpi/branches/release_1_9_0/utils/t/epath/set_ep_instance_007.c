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
 * set_ep_instance test7.
 *   call set_ep_instance with entity path that has 4 elements, victim element in middle
 *   expected result: only victim element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_INTERCONNECT, 1515},
                                {SAHPI_ENT_PHYSICAL_SLOT, 2525},
                                {SAHPI_ENT_SUBRACK, 3535},
                                {SAHPI_ENT_IO_SUBBOARD, 4545},
                                {0}}};
        SaHpiEntityLocationT x = 98765;
        int mydebug = 0;
         
        if (mydebug) printf(" test7\n");
        if(set_ep_instance(&ep, SAHPI_ENT_PHYSICAL_SLOT, x)) {
                if (mydebug) printf("set_ep_inst test7 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[1].EntityLocation != x) {
                if (mydebug) printf("set_ep_inst test7 failed, entInst %d != %d\n",
                                   ep.Entry[1].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_PHYSICAL_SLOT) {
                if (mydebug) printf("set_ep_inst test7 failed, entType %d != SAHPI_ENT_PHYSICAL_SLOT\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }
        if (ep.Entry[0].EntityLocation != 1515) {
                if (mydebug) printf("set_ep_inst test7 failed, entInst %d != 1515\n",
                                   ep.Entry[0].EntityLocation);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_INTERCONNECT) {
                if (mydebug) printf("set_ep_inst test7 failed, entType %d != SAHPI_ENT_INTERCONNECT\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        if (ep.Entry[2].EntityLocation != 3535) {
                if (mydebug) printf("set_ep_inst test7 failed, entInst %d != 3535\n",
                                   ep.Entry[2].EntityLocation);
                return 1;
        }
        if (ep.Entry[2].EntityType != SAHPI_ENT_SUBRACK) {
                if (mydebug) printf("set_ep_inst test7 failed, entType %d != SAHPI_ENT_SUBRACK\n",
                                   ep.Entry[2].EntityType);
                return 1;
        }
        if (ep.Entry[3].EntityLocation != 4545) {
                if (mydebug) printf("set_ep_inst test7 failed, entInst %d != 4545\n",
                                   ep.Entry[3].EntityLocation);
                return 1;
        }
        if (ep.Entry[3].EntityType != SAHPI_ENT_IO_SUBBOARD) {
                if (mydebug) printf("set_ep_inst test7 failed, entType %d! = SAHPI_ENT_IO_SUBBOARD\n",
                                   ep.Entry[3].EntityType);
                return 1;
        }

        return 0;
}
