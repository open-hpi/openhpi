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
 * This test verifies the failure of oh_el_timeset when
 * (1) el == NULL and (2) timestamp == SAHPI_TIME_UNSPECIFIED
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el, *el2;
	SaHpiTimeT timestamp = 0;
	SaErrorT retc;

	/* tests oh_el_timeset when el == NULL */

	el = NULL;

	retc = oh_el_timeset(el, timestamp + 20);
	if (retc == SA_OK){
		dbg("ERROR: oh_el_timeset failed");
		return 1;
	}

	/* tests oh_el_timeset when timestamp = SAHPI_TIME_UNSPECIFIED */
	
	/* create a new EL of size 5*/
	el2 = oh_el_create(20);     

	retc = oh_el_timeset(el2, SAHPI_TIME_UNSPECIFIED);
	if (retc == SA_OK){
		dbg("ERROR: oh_el_timeset on el2 failed");
		return 1;
	}

        /* close el2 without saving to file*/
        retc = oh_el_close(el2);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el2 failed.");
                return 1;
        }


        return 0;
}






