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
 * prt_ep test1
 *          call prt_ep with zero element entity path and expect unknown result
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{0}}};
        int mydebug = 0;

        if (prt_ep(&ep) == 0) {
                if (mydebug) printf ("zero element entity path printed\n"); 
        }    
        else {
                if (mydebug) printf ("zero element ep can\'t be printed\n");
        }    

        return 0;
}
