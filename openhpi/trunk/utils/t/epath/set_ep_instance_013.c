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
 * set_ep_instance test13
 *   call set_ep_instance with entity type not in entity path
 *   expected result: set_ep_instance returns failure
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        SaHpiEntityTypeT     w = SAHPI_ENT_IO_BLADE;
        SaHpiEntityLocationT x = 56873;
        unsigned int    y = 45321;
        unsigned int    i = 0;
        int mydebug = 0;
         
        if (mydebug) printf(" test13\n");
        for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++ ) {
                ep.Entry[i].EntityType = w;
                ep.Entry[i].EntityLocation = y+i;
        }

        if(set_ep_instance(&ep, SAHPI_ENT_IO_SUBBOARD, x) == 0) {
                if (mydebug) printf("set_ep_inst test13 checkpoint 1 failed\n");
                return 1;
        }
        for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++ ) {
                if((ep.Entry[i].EntityType != w) ||
                   (ep.Entry[i].EntityLocation != y+i)) {
                        if (mydebug) printf("set_ep_inst test13 failed at element %d\n", i);
                return 1;
                }
        }
         
        return 0;
}
