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
 * This test verifies failure of oh_el_get when (1) el == NULL,
 * (2) entry == NULL, (3) prev == NULL, (4) next == NULL, and
 * (5) empty EL 
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el, *el2, *el3, *el4, *el5;
	oh_el_entry *entry, *entry2, *entry3, *entry4, *entry5; 
	SaHpiEventLogEntryIdT prev, next, prev2, next2, next3, prev4, prev5, next5;
 	SaErrorT retc;

	/* test case 1: el == NULL */
	el = NULL;

        retc = oh_el_get(el, entry->event.EntryId, &prev, &next, &entry);
        if (retc == SA_OK) {
        	dbg("ERROR: oh_el_get failed.");
                return 1;
        }
	
	/* test case 2: entry == NULL */

        el2 = oh_el_create(20);
	retc = oh_el_map_from_file(el2, "./elTest.data");
	if (retc != SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
        }

 	retc = oh_el_get(el2, el2->nextId, &prev2, &next2, &entry2);
        if (retc == SA_OK) {
        	dbg("ERROR: oh_el_get failed.");
        	return 1;
        }
	
	/* test case 3: prev == NULL */

	el3 = oh_el_create(20);
	retc = oh_el_map_from_file(el3, "./elTest.data");
	if (retc != SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
        }

	entry3 = (oh_el_entry *)(g_list_first(el3->elentries)->data);

 	retc = oh_el_get(el3, entry3->event.EntryId, NULL, &next3, &entry3);
        if (retc == SA_OK) {
        	dbg("ERROR: oh_el_get failed.");
        	return 1;
        }
	
	/* test case 4: next == NULL */

	el4 = oh_el_create(20);
	retc = oh_el_map_from_file(el4, "./elTest.data");
	if (retc != SA_OK) {
                dbg("ERROR: oh_el_map_from_file failed.");
                return 1;
        }

	entry4 = (oh_el_entry *)(g_list_first(el4->elentries)->data);

 	retc = oh_el_get(el4, entry4->event.EntryId, &prev4, NULL, &entry4);
        if (retc == SA_OK) {
        	dbg("ERROR: oh_el_get failed.");
        	return 1;
        }
	
	/* test case 5: empty EL */

	el5 = oh_el_create (20);
        
	/* fetch the event for el5*/
        retc = oh_el_get(el5, SAHPI_FIRST_ENTRY, &prev5, &next5, &entry5);
        if (retc == SA_OK) {
        	dbg("ERROR: oh_el_get failed.");
                return 1;
        }
        
	/* close el2 */
        retc = oh_el_close(el2);
        if (retc != SA_OK) {
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
                dbg("ERROR: oh_el_close on el3 failed.");
                return 1;
        }

        /* close el5 */
        retc = oh_el_close(el5);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el3 failed.");
                return 1;
        }


        return 0;
}


