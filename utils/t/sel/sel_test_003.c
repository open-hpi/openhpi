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
#include <sel_utils.h>


#include "sel_test.h"


/**
 * main: SEL test
 * 
 * This test tests creates an SEL and adds five events.
 * It then save the SEL to a file.
 * 
 * Return value: 0 on success, 1 on failure
 **/
int main(int argc, char **argv) 
{
        oh_sel *sel;
        int x;
        SaErrorT retc;
        SaHpiSelInfoT info;
        SaHpiSelEntryIdT id, prev, next;
        SaHpiSelEntryT *entry;

        /* create the SEL */
        sel = oh_sel_create(OH_SELTEST_MAX_ENTRIES);
        
        if(sel == NULL) {
                dbg("ERROR: sel == NULL.");
                return 1;
        }

        /* add a multiple events */
        for (x = 0; x < OH_SELTEST_MAX_ENTRIES + 2; x++) {
                retc = add_event(sel, x);
                if (retc != SA_OK) {
                        dbg("ERROR: add_event failed.");
                        return 1;
                }
        }

        /* get sel info */
        retc = oh_sel_info(sel, &info);
        if (retc != SA_OK) {
                dbg("ERROR: oh_sel_info failed.");
                return 1;
        }
        if (info.Entries != OH_SELTEST_MAX_ENTRIES) {
                dbg("ERROR: oh_sel_info returned incorrect number of entries.");
                return 1;
        }

        /* now refetch all entries */
        x = 0;
        id = SAHPI_OLDEST_ENTRY;
        next = 0;
        while (next != SAHPI_NO_MORE_ENTRIES) {
                retc = oh_sel_get(sel, id, &prev, &next, &entry);
                if (retc != SA_OK) {
                        dbg("ERROR: oh_sel_get failed.");
                        return 1;
                }
                x++;
                id = next;
        }
        if (x != OH_SELTEST_MAX_ENTRIES) {
                dbg("ERROR: oh_sel_get return incorrect number of entries.");
                return 1;
        }

        /* now fetch an entry that does not exist */
        id = 9999;
        retc = oh_sel_get(sel, id, &prev, &next, &entry);
        if (retc != SA_ERR_HPI_NOT_PRESENT) {
                dbg("ERROR: oh_sel_get failed on invalid entryid.");
                return 1;
        }

        /* save the SEL with invalid filename */
        retc = oh_sel_map_to_file(sel, NULL);
        if (retc != SA_ERR_HPI_INVALID_PARAMS) {
                dbg("ERROR: oh_sel_map_to_file failed with invalid filename.");
                return 1;
        }

        /* save the SEL */
        retc = oh_sel_map_to_file(sel, "./selTest.data");
        if (retc != SA_OK) {
                dbg("ERROR: oh_sel_map_to_file failed.");
                return 1;
        }

        /* close the sel */
        retc = oh_sel_close(sel);
        if (retc != SA_OK) {
                dbg("ERROR: oh_sel_close failed.");
                return 1;
        }

        
        return 0;
}

