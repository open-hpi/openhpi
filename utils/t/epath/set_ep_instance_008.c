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
 * set_ep_instance test8.
 *   call set_ep_instance with 4 element entity path, victim element at head 
 *   expected result: only head element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_ADD_IN_CARD, 101},
                                {SAHPI_ENT_POWER_MODULE, 2020},
                                {SAHPI_ENT_POWER_MGMNT, 30303},
                                {SAHPI_ENT_SUB_CHASSIS, 404040},
                                {0}}};
        SaHpiEntityLocationT x = 555555;
        int mydebug = 0;
         
        if (mydebug) printf(" test8\n");
        if(set_ep_instance(&ep, SAHPI_ENT_ADD_IN_CARD, x)) {
                if (mydebug) printf("set_ep_inst test8 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[0].EntityLocation != x) {
                if (mydebug) printf("set_ep_inst test8 failed, entInst %d != %d\n",
                                   ep.Entry[0].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_ADD_IN_CARD) {
                if (mydebug) printf("set_ep_inst test8 failed, entType %d != SAHPI_ENT_ADD_IN_CARD\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        if (ep.Entry[1].EntityLocation != 2020) {
                if (mydebug) printf("set_ep_inst test8 failed, entInst %d != 2020\n",
                                   ep.Entry[1].EntityLocation);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_POWER_MODULE) {
                if (mydebug) printf("set_ep_inst test8 failed, entType %d != SAHPI_ENT_POWER_MODULE\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }
        if (ep.Entry[2].EntityLocation != 30303) {
                if (mydebug) printf("set_ep_inst test8 failed, entInst %d != 30303\n",
                                   ep.Entry[2].EntityLocation);
                return 1;
        }
        if (ep.Entry[2].EntityType != SAHPI_ENT_POWER_MGMNT) {
                if (mydebug) printf("set_ep_inst test8 failed, entType %d! = SAHPI_ENT_POWER_MGMNT\n",
                                   ep.Entry[2].EntityType);
                return 1;
        }
        if (ep.Entry[3].EntityLocation != 404040) {
                if (mydebug) printf("set_ep_inst test8 failed, entInst %d != 404040\n",
                                   ep.Entry[3].EntityLocation);
                return 1;
        }
        if (ep.Entry[3].EntityType != SAHPI_ENT_SUB_CHASSIS) {
                if (mydebug) printf("set_ep_inst test8 failed, entType %d != SAHPI_ENT_SUB_CHASSIS\n",
                                   ep.Entry[3].EntityType);
                return 1;
        }

        return 0;
}
