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

	SaHpiResourceIdT  id = 5000;
	 
	SaHpiSensorNumT sid = 0;
	SaHpiEventStateT state;
	SaHpiSensorReadingT reading;

#if 0  													    
	/* *************************************	 	 
	 * Find a resource with Sensor type rdr
	 * ************************************* */
	SaHpiSessionIdT sessionid;
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! Can not open session for test environment\n");
		printf("      File=%s, Line=%d\n", __FILE__, __LINE__);
		return -1;

	}
	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_SENSOR, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! Can not find resources for test environment\n");
		printf("      File=%s, Line=%d\n", __FILE__, __LINE__);
		err = tcleanup(&sessionid);
		return -1;

	}
#endif
	struct oh_handler_state handle;
	memset(&handle, 0, sizeof(struct oh_handler_state));

	/************************** 
	 * Test 3: Invalid custom_handle pointer
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;

	err = snmp_bc_get_sensor_reading(&handle, id, sid, &reading, &state);
	checkstatus(err, expected_err, testfail);

	
	/***************************
	 * Cleanup after all tests
	 ***************************/
	 return testfail;

}

#include <tsetup.c>
