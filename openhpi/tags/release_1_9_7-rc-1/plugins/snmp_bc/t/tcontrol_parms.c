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
#include <tsetup.h>


int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	int testfail = 0;
	SaHpiResourceIdT  id;
	SaHpiParmActionT  act;
	SaErrorT          err;
	SaErrorT expected_err;

        SaHpiSessionIdT sessionid;
	 
	/* ************************	 	 
	 * Find a resource with Control type rdr
	 * ***********************/
        SaHpiRptEntryT rptentry;
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! Can not setup session for test environment.\n");
		printf("       File=%s, Line=%d\n", __FILE__, __LINE__);
		return -1;

	}
	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_CONTROL, SAHPI_FIRST_ENTRY, &rptentry, SAHPI_TRUE);
	if (err != SA_OK) {
		printf("Error! Can not find resource for test environment.\n");
		printf("       File=%s, Line=%d\n", __FILE__, __LINE__);
		err = tcleanup(&sessionid);
		return -1;

	}

	struct oh_handler_state handle;
	memset(&handle, 0, sizeof(struct oh_handler_state));
			
	/************************** 
	 * Test 1: Invalid Control Action
	 **************************/
	id = rptentry.ResourceId;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	act = 0xFF;
																																														
	err = snmp_bc_control_parm(&handle, id, act);
	checkstatus(err, expected_err, testfail);
	
	/************************** 
	 * Test 2: Invalid ResourceId
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	act = SAHPI_RESTORE_PARM;

	err = snmp_bc_control_parm(&handle, 5000, act);
	checkstatus(err, expected_err, testfail);

#if 0
	/************************** 
	 * Test 3: Resource configuration saving not supported
	 *************************/
	rptentry.ResourceCapabilities |= SAHPI_CAPABILITY_CONFIGURATION;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	expected_err = SA_ERR_HPI_INTERNAL_ERROR;

	err = snmp_bc_control_parm(&handle, id, act);
	checkstatus(err, expected_err, testfail);
#endif

	/************************** 
	 * Test 4: Normal Path
	 **************************/
	expected_err = SA_ERR_HPI_CAPABILITY;

	err = saHpiParmControl(sessionid, id, act);
	checkstatus(err, expected_err, testfail);
	
	/***************************
	 * Cleanup after all tests
	 ***************************/

	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
