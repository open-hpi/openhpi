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
 *      David Ashley<dashley@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_utils.h>


#include "el_test.h"


/**
 * main: EL test
 *
 * This test tests creates an EL and adds five events.
 * It then save the EL to a file.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        oh_el *el;
        SaErrorT retc;

        /* create the EL */
        el = oh_el_create(OH_ELTEST_MAX_ENTRIES);

        if(el == NULL) {
                dbg("ERROR: el == NULL.");
                return 1;
        }

        /* get EL from file with invalid filename */
        retc = oh_el_map_from_file(el, NULL);
        if (retc != SA_ERR_HPI_INVALID_PARAMS) {
                dbg("ERROR: oh_el_map_from_file failed with invalid filename.");
                return 1;
        }

        /* get EL from file */
        retc = oh_el_map_from_file(el, "./elTest.data");
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
        }

        /* check out the EL */
        if(el->enabled != TRUE) {
                dbg("ERROR: el->enabled invalid.");
                return 1;
        }

        if(el->overflow != FALSE) {
                dbg("ERROR: el->overflow invalid.");
                return 1;
        }

        if(el->lastUpdate == SAHPI_TIME_UNSPECIFIED) {
                dbg("ERROR: el->lastUpdate invalid.");
                return 1;
        }

        if(el->offset != 0) {
                dbg("ERROR: el->offset invalid.");
                return 1;
        }

        /* note: if el_test_003.c changes this test may need to change */
        if(el->nextId < OH_ELTEST_MAX_ENTRIES) {
                dbg("ERROR: el->nextId invalid.");
                return 1;
        }

        if(el->maxsize != OH_ELTEST_MAX_ENTRIES) {
                dbg("ERROR: el->maxsize invalid.");
                return 1;
        }

        if(el->elentries == NULL) {
                dbg("ERROR: el->elentries invalid.");
                return 1;
        }

        /* close the el */
        retc = oh_el_close(el);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close failed.");
                return 1;
        }


        return 0;
}

