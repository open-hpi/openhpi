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
 * set_epath_instance test4.
 *   call set_epath_instance with entity path that has 3 elements
 *   expected result: only one element's instance number changed
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_GROUP, 11},
                                {SAHPI_ENT_REMOTE, 12},
                                {SAHPI_ENT_FAN, 13},
                                {0}}};
        SaHpiEntityInstanceT x = 99999;
        int mydebug = 0;
         
        if (mydebug) printf(" test4\n");
        if(set_epath_instance(&ep, SAHPI_ENT_FAN, x)) {
                if (mydebug) printf("set_ep_inst test4 checkpoint 1 failed\n");
                return 1;
        }
        if (ep.Entry[2].EntityInstance != x) {
                if (mydebug) printf("set_ep_inst test failed, entInst %d != %d\n",
                                   ep.Entry[2].EntityInstance, x);
                return 1;
        }
        if (ep.Entry[2].EntityType != SAHPI_ENT_FAN) {
                if (mydebug) printf("set_ep_inst test4 failed, entType %d != SAHPI_ENT_FAN\n",
                                   ep.Entry[2].EntityType);
                return 1;
        }
        if (ep.Entry[0].EntityInstance != 11) {
                if (mydebug) printf("set_ep_inst test4 failed, entInst %d != 11\n",
                                   ep.Entry[0].EntityInstance);
                return 1;
        }
        if (ep.Entry[0].EntityType != SAHPI_ENT_GROUP) {
                if (mydebug) printf("set_ep_inst test4 failed, entType %d != SAHPI_ENT_GROUP\n",
                                   ep.Entry[0].EntityType);
                return 1;
        }
        if (ep.Entry[1].EntityInstance != 12) {
                if (mydebug) printf("set_ep_inst test4 failed, entInst %d != 12\n",
                                   ep.Entry[1].EntityInstance);
                return 1;
        }
        if (ep.Entry[1].EntityType != SAHPI_ENT_REMOTE) {
                if (mydebug) printf("set_ep_inst test4 failed, entType %d! = SAHPI_ENT_REMOTE\n",
                                   ep.Entry[1].EntityType);
                return 1;
        }

        return 0;
}
