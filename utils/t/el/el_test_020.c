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
 * This test verifies failure of oh_el_append when (1) el == NULL,
 * (2) event == NULL,(3) el->enabled == SAHPI_FALSE &&
 * event->EventType == SAHPI_ET_USER, (4) el->enabled == SAHPI_FALSE &&
 * event->EventType != SAHPI_ET_USER, (5) entry = NULL, (6) el->maxsize !=
 * OH_EL_MAX_SIZE && g_list_length(el->elentries) == el->maxsize, and (7)
 * el-> gentimestamp == FALSE
 *
 * Return value: 0 on success, 1 on failure
 **/


int main(int argc, char **argv)
{
        oh_el *el, *el2, *el3, *el4, *el5, *el6, *el7;
        SaErrorT retc;
	int x;
	SaHpiEventT event, event2, event3, event4, event5, event6;
	oh_el_entry *entry;
	static char *data[1] = {
        	"Test data one"

	};


	/* test oh_el_append with el==NULL*/
	el = NULL;


        event.Source = 1;
        event.EventType = SAHPI_ET_USER;
        event.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event.Severity = SAHPI_DEBUG;

        strcpy((char *) &event.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_append(el, &event, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        }       

	/*test oh_el_append with event==NULL*/
	el2 = oh_el_create(20);
        retc = oh_el_append(el, NULL, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        } 

	/*test oh_el_append with el->enabled == SAHPI_FALSE &&
        event->EventType == SAHPI_ET_USER */
	
	el3 = oh_el_create(20);

	el3->enabled = FALSE;
        event2.Source = 1;
        event2.EventType = SAHPI_ET_USER;
        event2.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event2.Severity = SAHPI_DEBUG;

        strcpy((char *) &event2.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_append(el, &event2, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        }   
	
	/*test oh_el_append with el->enabled == SAHPI_FALSE &&
        event->EventType != SAHPI_ET_USER */
	
	el4 = oh_el_create(20);

	el4->enabled = FALSE;
        event3.Source = 1;
        event3.EventType = SAHPI_ET_DOMAIN;
        event3.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event3.Severity = SAHPI_DEBUG;

        strcpy((char *) &event3.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_append(el, &event3, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        }  
	
	/*test oh_el_append with entry == NULL */
	
	el5 = oh_el_create(20);

        event4.Source = 1;
        event4.EventType = SAHPI_ET_USER;
        event4.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event4.Severity = SAHPI_DEBUG;

        strcpy((char *) &event4.EventDataUnion.UserEvent.UserEventData.Data, data[0]);
	  entry = NULL;

        retc = oh_el_append(el, &event4, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        }  

	/*test oh_el_append with el->maxsize != OH_EL_MAX_SIZE && g_list_length(el->elentries) == el->maxsize */
	
	el6 = oh_el_create(20);
	x = g_list_length(el6->elentries);
	
	el6->maxsize = x;

        event5.Source = 1;
        event5.EventType = SAHPI_ET_USER;
        event5.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event5.Severity = SAHPI_DEBUG;

        strcpy((char *) &event5.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_append(el, &event5, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
                return 1;
        } 

	/*test oh_el_append with el->gentimestamp == FALSE*/
	
	el7 = oh_el_create(20);

        event6.Source = 1;
        event6.EventType = SAHPI_ET_USER;
        event6.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event6.Severity = SAHPI_DEBUG;
	  el7->gentimestamp = FALSE;

        strcpy((char *) &event6.EventDataUnion.UserEvent.UserEventData.Data, data[0]);

        retc = oh_el_append(el, &event6, NULL, NULL);
        if (retc == SA_OK) {
                dbg("ERROR: oh_el_append failed.");
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
                dbg("ERROR: oh_el_close on el4 failed.");
                return 1;
        }

	/* close el5 */
        retc = oh_el_close(el5);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el5 failed.");
                return 1;
        }

	/* close el6 */
        retc = oh_el_close(el6);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el6 failed.");
                return 1;
        }

	/* close el7 */
        retc = oh_el_close(el7);
        if (retc != SA_OK) {
                dbg("ERROR: oh_el_close on el7 failed.");
                return 1;
        }



        return 0;
}




