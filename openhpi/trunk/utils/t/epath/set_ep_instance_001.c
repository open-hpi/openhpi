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
 * set_ep_instance test1
 *   call set_ep_instance with zero entry entity path
 *   expected result: set_ep_instance call should fail
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{0,0}}};
        SaHpiEntityLocationT x = 3;
        int mydebug = 0;
         
        if (mydebug) printf(" test1\n");
        if(set_ep_instance(&ep, SAHPI_ENT_BACK_PANEL_BOARD, x) == 0) {
                if (mydebug) printf("set_ep_inst test1 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[0].EntityLocation != 0) {
                if (mydebug) printf("set_ep_inst test1 failed, entInst %d != 0\n",
                                   ep.Entry[0].EntityLocation);
                return 1;
        }
        if (ep.Entry[0].EntityType != 0) {
                if (mydebug) printf("set_ep_inst test1 failed, entType %d != 0\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        return 0;
}
