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
 * print_ep test2
 *          call print_ep with single element entity path and expect inspect result
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_ROOT,0},{0}}};
        int mydebug = 0;

        if (print_ep(&ep) == 0) {
                if (mydebug) printf ("print_ep test2 printed\n"); 
                return 0;
        }    
        else {
                if (mydebug) printf ("print_ep test2 failed\n");
                return 1;
        }    

}
