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
	SaHpiRptEntryT rptentry;				
	SaHpiResourceIdT  id = 0;
        SaHpiSessionIdT sessionid;
	SaHpiHsStateT state = 0;
	struct oh_handler_state l_handle;

	/* *************************************	 	 
	 * Find a resource with Power capability
	 * ************************************* */
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! Can not open session for test environment\n");
		printf("       File=%s, Line=%d\n", __FILE__, __LINE__);
		return -1;
	}

	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_POWER, SAHPI_FIRST_ENTRY, &rptentry, SAHPI_TRUE);
	if (err != SA_OK) {
		printf("Can not find a Power resource for test environment\n");
		printf("       File=%s, Line=%d\n", __FILE__, __LINE__);
		err = tcleanup(&sessionid);
		return SA_OK;
	}
	
	id = rptentry.ResourceId;
	/************************** 
	 * Test: Invalid resource ID
	 **************************/
	memset(&l_handle, 0, sizeof(struct oh_handler_state));

	expected_err = SA_ERR_HPI_INVALID_RESOURCE;                   
	err = saHpiHotSwapStateGet(sessionid, 5000, &state);
	checkstatus(err, expected_err, testfail);

	/************************** 
	 * Test: Invalid handle 
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;      
	err = snmp_bc_get_hotswap_state(NULL, id, &state);
	checkstatus(err, expected_err, testfail);

	/************************** 
	 * Test:
	 * expected_err = SA_ERR_HPI_INVALID_PARAMS;      
	 **************************/
	err = snmp_bc_get_hotswap_state(&l_handle, id, NULL);
	checkstatus(err, expected_err, testfail);

	/************************** 
	 * Test: Capability checking
	 **************************/
	expected_err = SA_ERR_HPI_CAPABILITY;      
	err = saHpiHotSwapStateGet(sessionid, id, &state);
	checkstatus(err, expected_err, testfail);

	/**************************&*
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
