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
	SaHpiBoolT disabled = SAHPI_FALSE;
	SaErrorT          err;
	SaErrorT expected_err;
				
	SaHpiResourceIdT  id;
        SaHpiSessionIdT sessionid;
	 
	SaHpiSensorNumT sid = 0;
	SaHpiEventStateT state;
	SaHpiSensorReadingT reading;
	struct SensorInfo *sinfo;
													    
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
	err = tfind_resource(&sessionid, (SaHpiCapabilitiesT) SAHPI_CAPABILITY_SENSOR, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! bc_sensor, can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;

	}
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	id = rptentry.ResourceId;
	/************************** 
	 * Test 23: 
	 **************************/
	sid = 0;
	do {
		sid++,
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (sid < 128 ));

	if (rdrptr == NULL) testfail = -1;		
		
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdrptr->RecordId);	
	/**************************
	 * Test 25
	 **************************/
	SaHpiEventStateT saved_state = 0;
	if (rdrptr->RdrTypeUnion.SensorRec.Events != SAHPI_ES_UNSPECIFIED) {
		saved_state = rdrptr->RdrTypeUnion.SensorRec.Events;	
		rdrptr->RdrTypeUnion.SensorRec.Events = SAHPI_ES_UNSPECIFIED;
		err = oh_add_rdr(handle->rptcache, id, rdrptr, sinfo, 0);
		disabled = SAHPI_TRUE;
	}
	expected_err = SA_OK;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &testfail);

	/**************************
	 * Test 26 
	 **************************/
	if (state != SAHPI_ES_UNSPECIFIED) {  
		printf("snmp_bc_get_sensor_eventstate fails.\n");
		printf("Not getting expected event state SAHPI_ES_UNSPECIFIED \n");
		testfail = -1;
	}
	/***************************
	 * Cleanup after all tests
	 ***************************/
	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
