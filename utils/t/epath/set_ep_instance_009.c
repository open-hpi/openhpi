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
 * set_ep_instance test9.
 *   call set_ep_instance with entity type not in entity path
 *   expected result: set_ep_instance returns failure
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_FAN, 494949},{0}}};
        SaHpiEntityInstanceT x = 6767;
        int mydebug = 0;
         
        if (mydebug) printf(" test9\n");
        if(set_ep_instance(&ep, SAHPI_ENT_DISK_BLADE, x) == 0) {
                if (mydebug) printf("set_ep_inst test9 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[0].EntityInstance != 494949) {
                if (mydebug) printf("set_ep_inst test9 failed, entInst %d != 494949\n",
                                   ep.Entry[0].EntityInstance);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_FAN) {
                if (mydebug) printf("set_ep_inst test9 failed, entType %d != SAHPI_ENT_FAN\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }

        return 0;
}
