/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Christina Hernandez<hernanc@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_utils.h>
#include <el_utils.h>


#include "el_test.h"

/**
 * main: EL test
 *
 * This test verifies the failure of oh_el_map_from_file when
 * (1) el == NULL, (2) el->enabled == SAHPI_FALSE, (3) filename == NULL
 * and (4) filename does not exist.
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el, *el2, *el3, *el4;
        SaErrorT retc;


	/* test failure of oh_el_map_from_file with el==NULL */
	el = NULL;

        retc = oh_el_map_from_file(el, "./elTest.data");
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
	}


	/* test failure of oh_el_map_from_file with el->enabled == SAHPI_FALSE */

	el2 = oh_el_create(20);
	el2->enabled = SAHPI_FALSE;

        retc = oh_el_map_from_file(el2, "./elTest.data");
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
	}
	
	/* test failure of oh_el_map_from_file with filename == NULL */

	el3 = oh_el_create(20);

        retc = oh_el_map_from_file(el3, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
	}


	/* test failure of oh_el_map_from_file with nonexistant filename */
	
	el4 = oh_el_create(20);

        retc = oh_el_map_from_file(el4, "./notthere.data");
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
	}

        /* close el2 */
        retc = oh_el_close(el2);
	if (retc != SA_OK){
                dbg("ERROR: oh_el_close on el2 failed.");
                return 1;
        }

        /* close el3 */
        retc = oh_el_close(el3);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el3 failed.");
                return 1;
        }

        /* close el4 */
        retc = oh_el_close(el4);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el4 failed.");
                return 1;
        }

        return 0;
}






