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
 * append_root test 2.
 *          append root to entity path already has root as first element
 *          expected result: no change, root entity still at first element
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_ROOT, 0},{0}}};
        int mydebug = 0;

        if(append_root(&ep)) {
                if (mydebug) printf("append_root function call failed\n");
                return 1;
        }
        if(ep.Entry[0].EntityType != SAHPI_ENT_ROOT) {
                if (mydebug) printf("append_root failed, %d != SAHPI_ENT_ROOT\n",
                                   ep.Entry[0].EntityType);  
                return 1;
        }

        if(ep.Entry[0].EntityInstance != 0) {
                if (mydebug) printf("append_root test failed, instance number %d != 0\n",
                                   ep.Entry[0].EntityInstance);  
                return 1;
        } 

        return 0;
}
