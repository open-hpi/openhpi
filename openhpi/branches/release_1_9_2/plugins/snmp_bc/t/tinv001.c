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
	SaHpiIdrIdT       idrId = 0;
	SaHpiEntryIdT     areaId = 0;
	SaHpiEntryIdT     fieldId = 0;
	SaHpiIdrFieldT    field; 
	memset (&field, 0, sizeof(SaHpiIdrFieldT));	
        /* *************************************                 
	 * Find a resource with Sensor type rdr
	 * * ************************************* */
	struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
	SaHpiRptEntryT rptentry;

	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! can not setup test environment\n");
		return -1;
	}

	err = tfind_resource(&sessionid, SAHPI_CAPABILITY_INVENTORY_DATA, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;
	}

	id = rptentry.ResourceId;
	/************************** 
	 * Test :
	 **************************/
	expected_err = SA_ERR_HPI_READ_ONLY;                   
	err = snmp_bc_add_idr_area((void *)h->hnd, id, idrId, SAHPI_IDR_AREATYPE_UNSPECIFIED, &areaId);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 * expected_err = SA_ERR_HPI_READ_ONLY;                   
	 **************************/
	err = snmp_bc_add_idr_field((void *)h->hnd, id, idrId, &field);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 * expected_err = SA_ERR_HPI_READ_ONLY;                   
	 **************************/
	err = snmp_bc_del_idr_area((void *)h->hnd, id, idrId, areaId);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 * expected_err = SA_ERR_HPI_READ_ONLY;                   
	 **************************/
	err = snmp_bc_del_idr_field((void *)h->hnd, id, idrId, areaId, fieldId);
	checkstatus(&err, &expected_err, &testfail);

	/************************** 
	 * Test :
	 * expected_err = SA_ERR_HPI_READ_ONLY;                   
	 **************************/
	err = snmp_bc_set_idr_field((void *)h->hnd, id, idrId, &field); 
	checkstatus(&err, &expected_err, &testfail);

	/**************************&*
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
