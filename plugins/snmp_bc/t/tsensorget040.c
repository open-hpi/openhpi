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
	 
	SaHpiSensorNumT sid = 0;
	SaHpiEventStateT assertMask;
	SaHpiEventStateT deassertMask;
													    
	/* *************************************	 	 
	 * Find a resource with Sensor type rdr
	 * ************************************* */
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
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	/**************************
	 * Test 40
	 **************************/
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_SENSOR;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, sid, &assertMask, &deassertMask);
	checkstatus(&err, &expected_err, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	 err = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
