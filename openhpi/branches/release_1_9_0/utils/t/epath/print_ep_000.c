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
 * print_ep test0
 *          call print_ep with NULL pointer entity path and expect the unknown
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        int mydebug = 0;

        if (print_ep(NULL) == 0) {
                if (mydebug) printf ("Null pointer entity path printed\n"); 
        }    
        else {
                if (mydebug) printf ("Null pointer entity path can\'t be printed\n");
        }    

        return 0;
}
