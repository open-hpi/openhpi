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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_error.h>
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
                CRIT("el pointer == NULL.");
                return 1;
        }

        if(el->info.Enabled != TRUE) {
                CRIT("el->info.Enabled invalid.");
                return 1;
        }

        if(el->info.OverflowFlag != FALSE) {
                CRIT("el->info.OverflowFlag invalid.");
                return 1;
        }

        if(el->info.UpdateTimestamp != SAHPI_TIME_UNSPECIFIED) {
                CRIT("el->info.UpdateTimestamp invalid.");
                return 1;
        }

        if(el->basetime != 0) {
                CRIT("el->basetime invalid.");
                return 1;
        }

        if(el->nextid != SAHPI_OLDEST_ENTRY + 1) {
                CRIT("el->nextid invalid.");
                return 1;
        }

        if(el->info.Size != 5) {
                CRIT("el->info.Size invalid.");
                return 1;
        }

        if(el->list != NULL) {
                CRIT("el->list invalid.");
                return 1;
        }

        return 0;
}
