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
 * prt_ep test5
 *          call prt_ep with full entity path and expect inspect result
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep;
        int i;
        int mydebug = 0;

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                ep.Entry[i].EntityType = SAHPI_ENT_SBC_BLADE;
                ep.Entry[i].EntityInstance = i+10;
        }
        if (prt_ep(&ep) == 0) {
                if (mydebug) printf ("prt_ep test5 printed\n"); 
                return 0;
        }    
        else {
                if (mydebug) printf ("prt_ep test5 failed\n");
                return 1;
        }    

}
