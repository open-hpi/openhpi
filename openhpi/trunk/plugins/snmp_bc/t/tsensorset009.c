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
					
	SaHpiResourceIdT  id = 0;
        SaHpiSessionIdT sessionid;
	 
	SaHpiSensorNumT sid = 0;
	SaHpiEventStateT assertMask = SAHPI_ES_LOWER_MINOR;    
	SaHpiEventStateT deassertMask = SAHPI_ES_LOWER_CRIT;    
	SaHpiSensorEventMaskActionT act = SAHPI_SENS_ADD_EVENTS_TO_MASKS;
        /* *************************************                 
	 * Find a resource with Sensor type rdr
	 * * ************************************* */
	struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
	SaHpiRptEntryT rptentry;

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

	id = rptentry.ResourceId;
	/************************** 
	 * Test : snmp_bc_set_sensor_event_masks()
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;                   
	err = snmp_bc_set_sensor_event_masks(NULL, id, sid, act, assertMask, deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test   
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;                   
	err = snmp_bc_set_sensor_event_masks((void *)h->hnd, 5000, sid, act, assertMask, deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test  
	 **************************/
	expected_err = SA_ERR_HPI_NOT_PRESENT;                   
	err = snmp_bc_set_sensor_event_masks((void *)h->hnd, id, 5000, act, assertMask, deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
