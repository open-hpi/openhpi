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
#include <el_utils.h>


#include "el_test.h"


/**
 * main: EL test
 *
 * This test tests creates an EL and adds one event.
 *
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv)
{
        oh_el *el;
        SaErrorT retc;

        /* create the EL */
        el = oh_el_create(5);

        if(el == NULL) {
                dbg("ERROR: el == NULL.");
                return 1;
        }

        /* add a single event */
        if (add_event(el, 0)) {
                dbg("ERROR: add_event failed.");
                return 1;
        }

        /* close the EL with bad pointer */
        retc = oh_el_close(NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_close failed with bad el pointer.");
                return 1;
        }

        /* close the EL */
        retc = oh_el_close(el);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close failed.");
                return 1;
        }

        return 0;
}

