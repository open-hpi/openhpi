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

static void 
checkstatus(SaErrorT *err, SaErrorT *expected_err, int *failed, int *pass, int *testcase, int *testfail);

int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	int testfail = 0;
	int testcase = 0;
	int passed = 0, failed = 0;
	SaHpiResourceIdT  id;
	SaErrorT          err;
	SaErrorT expected_err;
	SaHpiSeverityT sev;
        SaHpiSessionIdT sessionid;
	 
	/* ************************	 	 
	 * Find a resource with Control type rdr
	 * ***********************/
        struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
        SaHpiRptEntryT rptentry;
	
	err = tsetup(&sessionid);
	if (err != SA_OK) {
		printf("Error! bc_set_resource_sev, can not setup test environment\n");
		return -1;

	}
	err = tfind_resource(&sessionid, (SaHpiCapabilitiesT) SAHPI_CAPABILITY_CONTROL, h, &rptentry);
	if (err != SA_OK) {
		printf("Error! bc_set_resource_sev, can not setup test environment\n");
		err = tcleanup(&sessionid);
		return -1;

	}

	id = rptentry.ResourceId;
	/************************** 
	 * Test 1: Invalid severity 
	 **************************/
	testcase++;
	sev = 0xFE;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_set_resource_severity((void *)h->hnd, id, sev);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);
	
	/************************** 
	 * Test 2: Invalid ResourceId
	 **************************/
	testcase++;
	sev = SAHPI_INFORMATIONAL;
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	id = 5000; /* set it way out */

	err = snmp_bc_set_resource_severity((void *)h->hnd, id, sev);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);
	
	/************************** 
	 * Test 3: Valid case
	 **************************/
	testcase++;
	sev = SAHPI_INFORMATIONAL;
	expected_err = SA_OK;
	id = rptentry.ResourceId; 

	err = snmp_bc_set_resource_severity((void *)h->hnd, id, sev);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	 printf("tset_resource_sev: %d passed, %d failed\n", passed, failed);
	 err = tcleanup(&sessionid);
	 return testfail;

}

/*
 *
 *
 *
 */
static void 
checkstatus(SaErrorT *err, SaErrorT *expected_err, int *failed, int *passed, int *testcase, int *testfail)
{
	if (*err != *expected_err) {
		printf("Error! bc_get_sensor_reading: TestCase %d\n", *testcase);
		printf("Error! snmp_bc_get_sensor_reading returned err=%s, expected=%s\n",
		oh_lookup_error(*err), oh_lookup_error(*expected_err));
		*testfail = -1;
		(*failed)++;
	} else (*passed)++;

}

#include <tsetup.c>
