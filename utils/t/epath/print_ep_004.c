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
 * print_ep test4
 *          call print_ep with multi element entity path and expect inspect result
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        SaHpiEntityPathT ep = {{{SAHPI_ENT_FAN,8},
                                {SAHPI_ENT_DEVICE_BAY,7},
                                {SAHPI_ENT_SYSTEM_BUS,6},
                                {SAHPI_ENT_SBC_BLADE,5},
                                {SAHPI_ENT_SYSTEM_SLOT,4},
                                {SAHPI_ENT_COMPACTPCI_CHASSIS,3},
                                {SAHPI_ENT_SUBRACK,2},
                                {SAHPI_ENT_RACK,1},
                                {0}}};
        int mydebug = 0;

        if (print_ep(&ep) == 0) {
                if (mydebug) printf ("print_ep test4 printed\n"); 
                return 0;
        }    
        else {
                if (mydebug) printf ("print_ep test4 failed\n");
                return 1;
        }    

}
