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
	SaHpiPowerStateT state = 0;
	/* *************************************                 
	 * Find a resource 
	 * * ************************************* */
	struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
	SaHpiRptEntryT rptentry;

	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! can not setup test environment\n");
		return -1;
	}

	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_POWER, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;
	}

	id = rptentry.ResourceId;
	/************************** 
	 * Test :
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_PARAMS;      
	err = snmp_bc_get_power_state(NULL, id, &state);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 * expected_err = SA_ERR_HPI_INVALID_PARAMS;      
	 **************************/
	err = snmp_bc_get_power_state((void *)h->hnd, id, NULL);   
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 **************************/
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;      
	err = snmp_bc_get_power_state((void *)h->hnd, 5000, &state);
	checkstatus(&err, &expected_err, &testfail);
	
	/************************** 
	 * Test :
	 **************************/
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	struct ResourceInfo *s =
		(struct ResourceInfo *)oh_get_resource_data(handle->rptcache, id);
	if (s != NULL)
		s->mib.OidPowerState = NULL;

	expected_err = SA_ERR_HPI_INTERNAL_ERROR;      
	err = snmp_bc_get_power_state((void *)h->hnd, id, &state);
	checkstatus(&err, &expected_err, &testfail);

	/**************************&*
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
