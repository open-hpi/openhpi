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

        SaHpiDomainIdT did;
        struct oh_domain *d;					
	SaHpiResourceIdT  id;
        SaHpiSessionIdT sessionid;
	 
	SaHpiCtrlNumT cid = 1;
	SaHpiCtrlModeT mode = SAHPI_CTRL_MODE_AUTO;
	SaHpiCtrlStateT state;
													   
	/* *************************************	 	 
	 * Find a resource with Sensor type rdr
	 * ************************************* */
        struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
        SaHpiRptEntryT rptentry;
	
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! bc_control, can not setup test environment\n");
		return -1;

	}
	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_CONTROL, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! bc_control, can not find a control resource.");
		err = tcleanup(&sessionid);
		return -1;
	}

	id = rptentry.ResourceId;
	/************************** 
	 * Test 1: Invalid Handle    
	 *************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_set_control_state(NULL, id, cid, mode, &state);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 2: Resource ID with no RPT
	 *************************/
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	err = snmp_bc_set_control_state((void *)h->hnd, 5000, cid, mode, &state);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 3: Control ID with no RDR 
	 *************************/
	expected_err = SA_ERR_HPI_NOT_PRESENT;
	err = snmp_bc_set_control_state((void *)h->hnd, id, 5000, mode, &state);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 4: NULL state with AUTO mode
	 *************************/
	expected_err = SA_ERR_HPI_READ_ONLY;
	err = snmp_bc_set_control_state((void *)h->hnd, id, cid, mode, NULL);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 5: NULL state with AUTO mode
	 *************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_set_control_state((void *)h->hnd, id, cid, SAHPI_CTRL_MODE_MANUAL, NULL);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test 6: Invalid Capability
	 *************************/
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;

        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_CONTROL;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */
	
	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_set_control_state((void *)h->hnd, id, cid, mode, &state);
	checkstatus(&err, &expected_err, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
