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
        int x;
        SaErrorT retc;
        SaHpiEventLogInfoT info;
        SaHpiEventLogEntryIdT id, prev, next;
        oh_el_entry *entry;

        /* create the EL */
        el = oh_el_create(OH_ELTEST_MAX_ENTRIES);

        if(el == NULL) {
                dbg("ERROR: el == NULL.");
                return 1;
        }

        /* add a multiple events */
        for (x = 0; x < OH_ELTEST_MAX_ENTRIES + 2; x++) {
                retc = add_event(el, x);
                if (retc != SA_OK) {
                        dbg("ERROR: add_event failed.");
                        return 1;
                }
        }

        /* get el info */
        retc = oh_el_info(el, &info);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_info failed.");
                return 1;
        }
        if (info.Entries != OH_ELTEST_MAX_ENTRIES) {
                dbg("ERROR: oh_el_info returned incorrect number of entries.");
                return 1;
        }

        /* now refetch all entries */
        x = 0;
        id = SAHPI_OLDEST_ENTRY;
        next = 0;
        while (next != SAHPI_NO_MORE_ENTRIES) {
                retc = oh_el_get(el, id, &prev, &next, &entry);
                if (retc != SA_OK) {
                        dbg("ERROR: oh_el_get failed.");
                        return 1;
                }
                x++;
                id = next;
        }
        if (x != OH_ELTEST_MAX_ENTRIES) {
                dbg("ERROR: oh_el_get return incorrect number of entries.");
                return 1;
        }

        /* now fetch an entry that does not exist */
        id = 9999;
        retc = oh_el_get(el, id, &prev, &next, &entry);
        if (retc != SA_ERR_HPI_NOT_PRESENT) {
                dbg("ERROR: oh_el_get failed on invalid entryid.");
                return 1;
        }

        /* save the EL with invalid filename */
        retc = oh_el_map_to_file(el, NULL);
        if (retc != SA_ERR_HPI_INVALID_PARAMS) {
                dbg("ERROR: oh_el_map_to_file failed with invalid filename.");
                return 1;
        }

        /* save the EL */
        retc = oh_el_map_to_file(el, "./elTest.data");
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_map_to_file failed.");
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

