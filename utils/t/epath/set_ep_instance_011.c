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
 * set_ep_instance test11.
 *   call set_ep_instance with full entity path, victim element at end
 *   expected result: only end element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        SaHpiEntityTypeT     w = SAHPI_ENT_POWER_DISTRIBUTION_UNIT;
        SaHpiEntityLocationT x = 87654;
        unsigned int    y = 77;
        unsigned int    z = 29;
        unsigned int    i = 0;
        int mydebug = 0;
         
        if (mydebug) printf(" test11\n");
        for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++ ) {
                ep.Entry[i].EntityType = w;
                ep.Entry[i].EntityLocation = y;
        }
        ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType = SAHPI_ENT_REMOTE;
        ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityLocation = z;

        if(set_ep_instance(&ep, SAHPI_ENT_REMOTE, x)) {
                if (mydebug) printf("set_ep_inst test11 checkpoint 1 failed\n");
                return 1;
        }
        if ( ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityLocation != x ) {
                if (mydebug) printf("set_ep_inst test11 failed, entInst %d != %d\n",
                                   ep.Entry[0].EntityLocation, x);
                return 1;
        }
        if (ep.Entry[SAHPI_MAX_ENTITY_PATH-1].EntityType != SAHPI_ENT_REMOTE) {
                if (mydebug) printf("set_ep_inst test11 failed, entType %d != SAHPI_ENT_REMOTE\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        for ( i=0; i<SAHPI_MAX_ENTITY_PATH-1; i++ ) {
                if((ep.Entry[i].EntityType != w) ||
                   (ep.Entry[i].EntityLocation != y)) {
                        if (mydebug) printf("set_ep_inst test11 failed at element %d\n", i);
                        return 1;
                }
        }

        return 0;
}
