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
 * This test verifies failure of oh_el_prepend when (1) el == NULL,
 * (2) event == NULL, and (3) el->enabled == SAHPI_FALSE
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el, *el2, *el3;
        SaErrorT retc;
	SaHpiEventT event, event3;
	static char *data[1] = {
        	"Test data one"

	};


	/* test oh_el_prepend with el==NULL*/
	el = NULL;


        event.Source = 1;
        event.EventType = SAHPI_ET_USER;
        event.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event.Severity = SAHPI_DEBUG;

        strcpy((char *) &event.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

	
        retc = oh_el_prepend(el, &event, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_prepend failed.");
                return 1;
        }       

	/*test oh_el_prepend with event==NULL*/
	el2 = oh_el_create(20);

        retc = oh_el_prepend(el, NULL, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_prepend failed.");
                return 1;
        } 

	/*test oh_el_prepend with el->enabled == SAHPI_FALSE*/
	el3 = oh_el_create(20);

	el3->enabled = SAHPI_FALSE;
        event3.Source = 1;
        event3.EventType = SAHPI_ET_USER;
        event3.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event3.Severity = SAHPI_DEBUG;

        strcpy((char *) &event3.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_prepend(el, &event3, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_add failed.");
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


        return 0;
}

