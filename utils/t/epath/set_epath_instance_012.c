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
 * set_epath_instance test.
 *   call set_epath_instance with full entity path and victim element in the middle
 *   expected result: only victim element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        SaHpiEntityTypeT     w = SAHPI_ENT_SBC_BLADE;
        SaHpiEntityInstanceT x = 56873;
        unsigned int    y = 77002;
        unsigned int    z = 3;
        unsigned int    i = 0;
        int mydebug = 0;
         
        if (mydebug) printf(" test12\n");
        for ( i=0; i<z; i++ ) {
                ep.Entry[i].EntityType = w;
                ep.Entry[i].EntityInstance = y;
        }
        ep.Entry[z].EntityType = SAHPI_ENT_FAN;
        ep.Entry[z].EntityInstance = z;
        for ( i=z+1; i<SAHPI_MAX_ENTITY_PATH; i++ ) {
                ep.Entry[i].EntityType = w;
                ep.Entry[i].EntityInstance = y;
        }

        if(set_epath_instance(&ep, SAHPI_ENT_FAN, x)) {
                if (mydebug) printf("set_ep_inst test12 checkpoint 1 failed\n");
                return 1;
        }
        if ( ep.Entry[z].EntityInstance != x ) {
                if (mydebug) printf("set_ep_inst test12 failed, entInst %d != %d\n",
                                   ep.Entry[z].EntityInstance, x);
                return 1;
        }
        if (ep.Entry[z].EntityType != SAHPI_ENT_FAN) {
                if (mydebug) printf("set_ep_inst test12 failed, entType %d != SAHPI_ENT_FAN\n",
                                   ep.Entry[z].EntityType);
                return 1;
        }
        for ( i=0; i<z; i++ ) {
                if((ep.Entry[i].EntityType != w) ||
                   (ep.Entry[i].EntityInstance != y)) {
                        if (mydebug) printf("set_ep_inst test12 failed at element %d\n", i);
                return 1;
                }
        }
        for ( i=z+1; i<SAHPI_MAX_ENTITY_PATH; i++ ) {
                if((ep.Entry[i].EntityType != w) ||
                   (ep.Entry[i].EntityInstance != y)) {
                        if (mydebug) printf("set_ep_inst test12 failed at element %d\n", i);
                return 1;
                }
        }

        return 0;
}
