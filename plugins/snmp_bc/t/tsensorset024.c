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
	 
	SaHpiSensorNumT sid = 0;
	SaHpiSensorThresholdsT thres;
        /* *************************************                 
	 * Find a resource with Sensor type rdr
	 * * ************************************* */
	struct oh_handler l_handler;
	struct oh_handler *h= &l_handler;
	SaHpiRptEntryT rptentry;
	SaHpiRdrT	*rdrptr;
	
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
	 *         
         **************************/
        sid = 0;
        do {
                sid++;
                rdrptr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, sid);
                if (rdrptr != NULL) {
                        if (rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible == SAHPI_TRUE) 
                                break;
	                else
				rdrptr = NULL;
		}

        } while ((rdrptr == NULL) && (sid < 128));

        if (rdrptr == NULL) testfail = -1;

        /************************** 
	 * Test  
         **************************/
        rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.WriteThold = 0xFF;  
        rdrptr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold = 0xFF;  
        expected_err = SA_ERR_HPI_INTERNAL_ERROR;
	err = snmp_bc_set_sensor_thresholds((void *)h->hnd, id, sid, &thres);
        checkstatus(&err, &expected_err, &testfail);
											
	/***************************
	 * Cleanup after all tests
	 ***************************/
	err = tcleanup(&sessionid);
	return testfail;

}

#include <tsetup.c>
