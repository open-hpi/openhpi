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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <oh_utils.h>


/**
 * main: EL test
 *
 * This test tests the creation of an EL.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        oh_el *el;

        el = oh_el_create(5);

        if(el == NULL) {
                dbg("ERROR: el pointer == NULL.");
                return 1;
        }

        if(el->enabled != TRUE) {
                dbg("ERROR: el->enabled invalid.");
                return 1;
        }

        if(el->overflow != FALSE) {
                dbg("ERROR: el->overflow invalid.");
                return 1;
        }

        if(el->lastUpdate != SAHPI_TIME_UNSPECIFIED) {
                dbg("ERROR: el->lastUpdate invalid.");
                return 1;
        }

        if(el->offset != 0) {
                dbg("ERROR: el->offset invalid.");
                return 1;
        }

        if(el->nextId != SAHPI_OLDEST_ENTRY + 1) {
                dbg("ERROR: el->nextId invalid.");
                return 1;
        }

        if(el->maxsize != 5) {
                dbg("ERROR: el->maxsize invalid.");
                return 1;
        }

        if(el->elentries != NULL) {
                dbg("ERROR: el->elentries invalid.");
                return 1;
        }

        return 0;
}
