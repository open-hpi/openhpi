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
	SaHpiBoolT disabled = SAHPI_FALSE;
	SaErrorT          err;
	SaErrorT expected_err;

        SaHpiDomainIdT did;
        struct oh_domain *d;					
	SaHpiResourceIdT  id;
	SaHpiResourceIdT  dd_id;
        SaHpiSessionIdT sessionid;
	 
	SaHpiSensorNumT sid = 0, dd_sid = 0;
	SaHpiEventStateT state;
	SaHpiSensorReadingT reading;
	struct SensorInfo *sinfo;
													    
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
	/************************** 
	 * Test 1: snmp_bc_get_sensor_reading
	 **************************/
	testcase++;
        struct oh_handler_state dd_hnd = {
              .rptcache = (RPTable *)NULL,
              .eventq = NULL,
              .config = NULL,
              .data = (void *)NULL
        };

	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_get_sensor_reading(NULL, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 2: snmp_bc_get_sensor_reading
	 **************************/
	testcase++;
	/* expected_err = SA_ERR_HPI_INVALID_PARAMS; -- same as above */
	err = snmp_bc_get_sensor_reading(&dd_hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 3: Invalid ResourceId
	 **************************/
	testcase++;
	dd_id = 5000;
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;

	err = snmp_bc_get_sensor_reading((void *)h->hnd, dd_id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 4: Invalid Capability
	 *************************/
	struct oh_handler_state *handle = (struct oh_handler_state *)h->hnd;
	testcase++;
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_SENSOR;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */
	
	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 5: Invalid sensor id
	 **************************/
	testcase++;
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities |=  SAHPI_CAPABILITY_SENSOR;
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	sid = 5000;	
	expected_err = SA_ERR_HPI_NOT_PRESENT;
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 6: 
	 **************************/
	testcase++;
	SaHpiRdrT *rdrptr;
	sid = 0;
	do {
		sid++,
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (sid < 128 ));

	if (rdrptr == NULL) {
		testfail = -1;
		failed++;
	} else passed++;
		
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdrptr->RecordId);	
	/**************************
	 * Test 7: 
	 **************************/
	testcase++;	
	if (sinfo->sensor_enabled == SAHPI_TRUE)  {
		sinfo->sensor_enabled = SAHPI_FALSE;
		disabled = SAHPI_TRUE;
	}
	expected_err = SA_ERR_HPI_INVALID_REQUEST;
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	if (disabled) {
		sinfo->sensor_enabled = SAHPI_TRUE; 
		disabled = SAHPI_FALSE;
	}

	/************************** 
	 * Test 8: 
	 **************************/
	testcase++;	
	expected_err = SA_OK;                   
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 9: 
	 **************************/
	testcase++;
	if (!reading.IsSupported ) {
		printf("\t  Reading Is Not Supported for sensor %d!\n\n", sid);
		testfail = -1;
		failed++;
        } else passed++;
		

	/************************** 
	 * Test 10 
	 **************************/
	testcase++;
	dd_sid = 0;
	do {
		dd_sid++;
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, dd_sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_FALSE) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (dd_sid < 128));

	if (rdrptr == NULL) {
		testfail = -1;
		failed++;
	} else passed++;

	/************************** 
	 * Test 11 
	 **************************/
	testcase++;	
	expected_err = SA_OK;                   
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, dd_sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 12 
	 **************************/
	testcase++;
	if (reading.IsSupported ) {
		printf("\t  Reading Is Supported for sensor %d!\n\n", dd_sid);
		testfail = -1;
		failed++;
        } else passed++;


	/************************** 
	 * Test 13 
	 **************************/
	testcase++;
	expected_err = SA_OK;                   
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, NULL, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 14 
	 **************************/
	testcase++;
	expected_err = SA_OK;                   
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, &reading, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 15:
	 **************************/
	testcase++;
	expected_err = SA_OK;                   
	err = snmp_bc_get_sensor_reading((void *)h->hnd, id, sid, NULL, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 16 : Start of snmp_bc_get_sensor_eventstate()
	 **************************/
	testcase++;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;                   
	err = snmp_bc_get_sensor_eventstate(NULL, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 17 : 
	 **************************/
	testcase++;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;                   
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, NULL, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 18 : 
	 **************************/
	testcase++;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;                   
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 19: 
	 **************************/
	testcase++;
	expected_err = SA_ERR_HPI_INVALID_PARAMS;                   
	err = snmp_bc_get_sensor_eventstate(NULL , id, sid, NULL, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 20: Invalid ResourceId
	 **************************/
	testcase++;
	dd_id = 5000;
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;

	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, dd_id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 21: Invalid Capability
	 *************************/
	testcase++;
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_SENSOR;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */
	
	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 22: Invalid sensor id
	 **************************/
	testcase++;
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities |=  SAHPI_CAPABILITY_SENSOR;
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	sid = 5000;	
	expected_err = SA_ERR_HPI_NOT_PRESENT;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/************************** 
	 * Test 23: 
	 **************************/
	testcase++;
	sid = 0;
	do {
		sid++,
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (sid < 128 ));

	if (rdrptr == NULL) {
		testfail = -1;
		failed++;
	} else passed++;
		
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdrptr->RecordId);	
	/**************************
	 * Test 24: 
	 **************************/
	testcase++;	
	if (sinfo->sensor_enabled == SAHPI_TRUE) {
		sinfo->sensor_enabled = SAHPI_FALSE;
		disabled = SAHPI_TRUE;
	}
	expected_err = SA_ERR_HPI_INVALID_REQUEST;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	if(disabled) {
		sinfo->sensor_enabled = SAHPI_TRUE; 
		disabled = SAHPI_FALSE;
	}

	/**************************
	 * Test 25
	 **************************/
	testcase++;	
	SaHpiEventStateT saved_state = 0;
	if (rdrptr->RdrTypeUnion.SensorRec.Events != SAHPI_ES_UNSPECIFIED) {
		saved_state = rdrptr->RdrTypeUnion.SensorRec.Events;	
		rdrptr->RdrTypeUnion.SensorRec.Events = SAHPI_ES_UNSPECIFIED;
		err = oh_add_rdr(handle->rptcache, id, rdrptr, sinfo, 0);
		disabled = SAHPI_TRUE;
	}
	expected_err = SA_OK;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 26 
	 **************************/
	testcase++;
	if (state != SAHPI_ES_UNSPECIFIED) {
		printf("tsensor fails, Test %d\n",testcase);     
		printf("snmp_bc_get_sensor_eventstate fails.\n");
		printf("Not getting expected event state SAHPI_ES_UNSPECIFIED \n");
		testfail = -1;
		failed++;
	} else passed++;

	if(disabled) {
		rdrptr->RdrTypeUnion.SensorRec.Events = saved_state;
		err = oh_add_rdr(handle->rptcache, id, rdrptr, sinfo, 0);
		disabled = SAHPI_FALSE;
	}

	/**************************
	 * Test 27 
	 **************************/
	testcase++;	
	dd_sid = 0;
	do {
		dd_sid++,
		rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, dd_sid);
		if (rdrptr != NULL) {
			if (rdrptr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
				break;
			} else 
				rdrptr = NULL;
		}
	} while ((rdrptr == NULL) && (dd_sid < 128 ));

	if (rdrptr == NULL) {
		testfail = -1;
		failed++;
	} else passed++;
		
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdrptr->RecordId);	
	/**************************
	 * Test 28 
	 **************************/
	testcase++;	
	expected_err = SA_OK;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, dd_sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);
	/**************************
	 * Test 29 
	 **************************/
	testcase++;	
	if (state != sinfo->cur_state) {
		printf("tsensor fails, Test %d\n",testcase);     
		printf("snmp_bc_get_sensor_eventstate fails.\n");
		printf("Not getting expected event state SAHPI_ES_UNSPECIFIED \n");
		testfail = -1;
		failed++;
	} else passed++;

	/**************************
	 * Test 30 
	 **************************/
	testcase++;	
	expected_err = SA_OK;
	err = snmp_bc_get_sensor_eventstate((void *)h->hnd, id, sid, &reading, &state);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 31 snmp_bc_get_sensor_event_enable
	 **************************/
	SaHpiBoolT enable;
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_get_sensor_event_enable((void *)h->hnd, id, sid, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 32
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	err = snmp_bc_get_sensor_event_enable((void *)h->hnd, 5000, sid, &enable);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 33
	 **************************/
	testcase++;	
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_SENSOR;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_get_sensor_event_enable((void *)h->hnd, id, dd_sid, &enable);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities |=  SAHPI_CAPABILITY_SENSOR;
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	/**************************
	 * Test 34
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_NOT_PRESENT;
	err = snmp_bc_get_sensor_event_enable((void *)h->hnd, id, 5000, &enable);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 35
	 **************************/
	testcase++;	
	expected_err = SA_OK;                 
	err = snmp_bc_get_sensor_event_enable((void *)h->hnd, id, sid, &enable);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 36 snmp_bc_get_sensor_event_masks 
	 **************************/
	SaHpiEventStateT assertMask;
	SaHpiEventStateT deassertMask;
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, sid, NULL, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 37
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, sid, &assertMask, NULL);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 38
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_PARAMS;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, sid, NULL, &deassertMask);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 39
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_INVALID_RESOURCE;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, 5000, sid, &assertMask, &deassertMask);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 40
	 **************************/
	testcase++;	
        OH_GET_DID(sessionid, did);
	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities &= !SAHPI_CAPABILITY_SENSOR;  
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	expected_err = SA_ERR_HPI_CAPABILITY;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, dd_sid, &assertMask, &deassertMask);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	OH_GET_DOMAIN(did, d); /* Lock domain */
	rptentry.ResourceCapabilities |=  SAHPI_CAPABILITY_SENSOR;
	oh_add_resource(handle->rptcache, &rptentry, NULL, 0);
	oh_release_domain(d); /* Unlock domain */

	/**************************
	 * Test 41
	 **************************/
	testcase++;	
	expected_err = SA_ERR_HPI_NOT_PRESENT;
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, 5000, &assertMask, &deassertMask);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);

	/**************************
	 * Test 42
	 **************************/
	testcase++;	
	expected_err = SA_OK;                 
	err = snmp_bc_get_sensor_event_masks((void *)h->hnd, id, sid, &assertMask, &deassertMask);
	checkstatus(&err, &expected_err, &failed, &passed, &testcase, &testfail);
	
	/***************************
	 * Cleanup after all tests
	 ***************************/
	 printf("tsensor: %d passed, %d failed\n", passed, failed);
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
		printf("Error! tsensor: TestCase %d\n", *testcase);
		printf("Error! tsensor: returned err=%s, expected=%s\n",
		oh_lookup_error(*err), oh_lookup_error(*expected_err));
		*testfail = -1;
		(*failed)++;
	} else (*passed)++;

}
#include <tsetup.c>
