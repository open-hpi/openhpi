/* -*- linux-c -*-
 * 
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Peter D Phan <pdphan@users.sourceforge.net>
 */


#include <snmp_bc_plugin.h>
#include <sahpimacros.h>
#include <tsetup.h>

int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	int testfail = 0;
	SaErrorT          err;
	SaErrorT expected_err;
					
	SaHpiResourceIdT  id;
        SaHpiSessionIdT sessionid;
	 
	SaHpiSensorNumT sid = 0;
	SaHpiSensorEventMaskActionT act   = SAHPI_SENS_ADD_EVENTS_TO_MASKS;
	SaHpiEventStateT assertMask       = SAHPI_ES_UPPER_MINOR;
	SaHpiEventStateT deassertMask      = SAHPI_ES_UPPER_CRIT;
	
	/* *************************************	 	 
	 * Find a resource with Sensor type rdr
	 * ************************************* */
        struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
        SaHpiRptEntryT rptentry;
	SaHpiRdrT *rdrptr;
		
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! bc_sensor, can not setup test environment\n");
		return -1;

	}
	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_SENSOR, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! bc_sensor, can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;

	}

	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	id = rptentry.ResourceId;
	/************************** 
	 *  
	 **************************/
	sid = 0;
	do {
		sid++;
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.EventCtrl != SAHPI_SEC_PER_EVENT) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (sid < 128));

	if (rdrptr == NULL) testfail = -1;

	/************************** 
	 * Alter sensor characteristic
	 **************************/
	rdrptr->RdrTypeUnion.SensorRec.EventCtrl = SAHPI_SEC_PER_EVENT;
					 
	/************************** 
	 * Test  
	 **************************/
	act   = SAHPI_SENS_ADD_EVENTS_TO_MASKS;
	deassertMask = ~(rdrptr->RdrTypeUnion.SensorRec.Events);
	assertMask =  SAHPI_ALL_EVENT_STATES;
	expected_err = SA_OK;                   
	err = snmp_bc_set_sensor_event_masks((void *)h->hnd, id, sid, act, assertMask, deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test  
	 **************************/
	assertMask =  rdrptr->RdrTypeUnion.SensorRec.Events;
	expected_err = SA_OK;                   
	err = snmp_bc_set_sensor_event_masks((void *)h->hnd, id, sid, act, assertMask, deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
