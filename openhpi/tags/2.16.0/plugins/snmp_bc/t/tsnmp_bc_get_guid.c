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
 *     Chris Chia <cchia@users.sf.net>
 */


#include <snmp_bc_plugin.h>
#include <sahpimacros.h>
#include <tsetup.h>
#include <snmp_bc_utils.h>

int main(int argc, char **argv) 
{

	/* ************************
	 * Local variables
	 * ***********************/	 
	gchar *CH_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.1.1.4.0";
	gchar *MM1_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.2.1.1.6.1";
  	gchar *MM2_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.2.1.1.6.2";
	gchar *SW1_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.6.1.1.8.1";
	gchar *SW2_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.6.1.1.8.2";
	gchar *SW3_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.6.1.1.8.3";
	gchar *SW4_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.6.1.1.8.4";
	gchar *BLADE1_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.4.1.1.8.1";
	gchar *BLADE2_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.4.1.1.8.2";
	gchar *BLADE3_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.4.1.1.8.3";
	gchar *BLADE4_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.4.1.1.8.4";
	gchar *BLADE5_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.4.1.1.8.5";
	gchar *MEDIA_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.9.8.0";
	gchar *POWER1_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.8.1.1.8.1";
	gchar *POWER2_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.8.1.1.8.2";
  	gchar *POWER3_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.8.1.1.8.3";
	gchar *POWER4_UUID_OID = ".1.3.6.1.4.1.2.3.51.2.2.21.8.1.1.8.4";
        gchar *BUSY_OID = ".1.3.6.1.4.1.2.3.51.2.4.4.1.7777";

	int         testfail = 0;
	int                i = 0;
        char            *oid = NULL;
        SaHpiGuidT      guid = {0};
	SaErrorT          rc = 0;
	SaErrorT expected_rc = 0;

        SaHpiSessionIdT sessionid;
        gchar *test_ok[]  = {CH_UUID_OID, MM1_UUID_OID, MM2_UUID_OID, 
                             SW1_UUID_OID, SW3_UUID_OID,
                             BLADE1_UUID_OID, BLADE2_UUID_OID, BLADE4_UUID_OID,
                             MEDIA_UUID_OID, POWER1_UUID_OID, POWER2_UUID_OID, 
                             POWER3_UUID_OID, POWER4_UUID_OID, NULL};
        gchar *test_bad[] = {SW2_UUID_OID, SW4_UUID_OID, BLADE3_UUID_OID, 
                             BLADE5_UUID_OID, NULL};
		
	/* ************************	 	 
	 * Find a resource with Control type rdr
	 * ***********************/
        struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
        SaHpiRptEntryT rptentry;
	
	rc = tsetup(&sessionid);
	if (rc != SA_OK) {
		printf("Error! bc_set_resource_tag, can not setup test environment\n");
		return -1;
	}
	rc = tfind_resource(&sessionid, (SaHpiCapabilitiesT) SAHPI_CAPABILITY_CONTROL, h, &rptentry);
	if (rc != SA_OK) {
		printf("Error! bc_set_resource_tag, can not setup test environment\n");
		rc = tcleanup(&sessionid);
		return -1;
	}
		
        struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
		
	/************************** 
	 * Test 1: Valid case
	 **************************/
        i = 0;
        while ((oid = test_ok[i++]) != NULL) {
	        expected_rc = SA_OK;
                rc = snmp_bc_get_guid(custom_handle, oid, &guid);
                //int j = 0;
                //if ( rc == SA_OK ) {
                //        printf("chassis GUID ");
                //        for ( j=0; j<16; j++ ) printf("%02x", guid[j]); printf("\n");
                //}
        	checkstatus(&rc, &expected_rc, &testfail);
        }

	/************************** 
	 * Test 2: Invalid case
	 **************************/
        i = 0;
        while ((oid = test_bad[i++]) != NULL) {
	        expected_rc = SA_ERR_HPI_ERROR;
                rc = snmp_bc_get_guid(custom_handle, oid, &guid);
                if ( rc != expected_rc ) {
                        printf("Error: test for oid %s failed\n", oid);
                }
        	checkstatus(&rc, &expected_rc, &testfail);
        }

	/************************** 
	 * Test 3: Bad parameters
	 **************************/
	expected_rc = SA_ERR_HPI_INVALID_PARAMS;
        rc = snmp_bc_get_guid((struct snmp_bc_hnd *)NULL, CH_UUID_OID, &guid);
        checkstatus(&rc, &expected_rc, &testfail);
        
        rc = snmp_bc_get_guid(custom_handle, (char *)NULL, &guid);
        checkstatus(&rc, &expected_rc, &testfail);
        
        rc = snmp_bc_get_guid(custom_handle, CH_UUID_OID, (SaHpiGuidT *)NULL);
        checkstatus(&rc, &expected_rc, &testfail);
        
        expected_rc = SA_ERR_HPI_BUSY;
        rc = snmp_bc_get_guid(custom_handle, BUSY_OID, &guid);
        checkstatus(&rc, &expected_rc, &testfail);

	/***************************
	 * Cleanup after all tests
	 ***************************/
	 rc = tcleanup(&sessionid);
	 return testfail;

}

#include <tsetup.c>
