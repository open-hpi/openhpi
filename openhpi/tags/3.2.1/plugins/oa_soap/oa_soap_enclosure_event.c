/*
 * Copyright (C) 2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra M.S. <raghavendra.ms@hp.com>
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 *
 * This file has the enclosure related events handling
 *
 * 	oa_soap_proc_enc_status()	- Processes the enclosure status event
 *
 */
#include "oa_soap_enclosure_event.h"

/**
 * oa_soap_proc_enc_status
 *      @oh_handler: Pointer to openhpi handler structure
 *      @con: Pointer to soap client con object
 *      @event: Pointer to the openhpi event structure
 *
 * Purpose:
 *     
 *
 * Detailed Description: NA
 *
 * Return values:
 *	NONE
 **/
void oa_soap_proc_enc_status(struct oh_handler_state *oh_handler,
			     struct enclosureStatus *status)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id;
	struct oa_soap_handler *oa_handler;
	enum diagnosticStatus diag_ex_status[OA_SOAP_MAX_DIAG_EX];

	if (oh_handler == NULL || status == NULL) {
		err("wrong parameters passed");
		return;
	}

	oa_handler = (struct oa_soap_handler *) oh_handler->data;
	resource_id = oa_handler->oa_soap_resources.enclosure_rid;

	/* Process the operational status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_OPER_STATUS,
				     status->operationalStatus, 0, 0)

	/* Process the predictive failure status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_PRED_FAIL,
				     status->operationalStatus, 0, 0)

	/* Process the internal data error sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_INT_DATA_ERR,
				     status->diagnosticChecks.internalDataError,
				     0, 0)

	/* Process the device failure sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_FAIL,
				     status->diagnosticChecks.deviceFailure,
				     0, 0)

	/* Process the device degraded sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_DEGRAD,
				     status->diagnosticChecks.deviceDegraded,
				     0, 0)

	/* Process the redundancy error sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_REDUND_ERR,
				     status->diagnosticChecks.redundancy, 0, 0)

	oa_soap_parse_diag_ex(status->diagnosticChecksEx, diag_ex_status);

	/* Build device not supported sensor rdr */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_NOT_SUPPORT,
				     diag_ex_status[DIAG_EX_DEV_NOT_SUPPORT],
				     0, 0)

	return;
}

/**
 * process_enc_thermal_event
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @con:         Pointer to the SOAP_CON structure
 *      @response:    Pointer to the Enclosure thermal Info structure
 *
 * Purpose:
 *      Processes and creates enclosure / chassis sensor thermal events
 *
 * Detailed Description: NA
 *
 * Return values: 
 *      NONE
 **/
void oa_soap_proc_enc_thermal(struct oh_handler_state *oh_handler,
                                       SOAP_CON *con,
					struct thermalInfo *response)
{
        SaErrorT rv = SA_OK;
        SaHpiResourceIdT resource_id;
        struct oa_soap_handler *oa_handler = NULL;
        SaHpiFloat64T trigger_reading;
        SaHpiFloat64T trigger_threshold;
        struct getThermalInfo thermal_request;
        struct thermalInfo thermal_response;
        struct oa_soap_sensor_info *sensor_info = NULL;
        SaHpiRdrT *rdr = NULL;
 
        if (oh_handler == NULL || con== NULL || response == NULL) {
                err("Invalid parameters");
                return;
        }
 
        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        resource_id = oa_handler->
                oa_soap_resources.enclosure_rid;
 
        rdr = oh_get_rdr_by_type(oh_handler->rptcache, resource_id,
                                 SAHPI_SENSOR_RDR, OA_SOAP_SEN_TEMP_STATUS);
        if (rdr)
	        sensor_info = (struct oa_soap_sensor_info *)
                        oh_get_rdr_data(oh_handler->rptcache, resource_id,
                                        rdr->RecordId);
 
        
        /* Based on the sensor status,
         * determine the threshold which triggered the thermal event from 
	 * Enclosure.
         * Event with SENSOR_STATUS_CAUTION or SENSOR_STATUS_OK is
         * generated only if CAUTION threshold is crossed.
         * Event with SENSOR_STATUS_CRITICAL is generated only when CRITICAL
         * threshold is crossed.
         * Sensor current reading and trigger threshold are required for event
         * generation. Sensor current reading is not provided by the event,
         * hence make soap call to get the reading
         */
        thermal_request.bayNumber = 1;
        thermal_request.sensorType = SENSOR_TYPE_ENC;
 
        rv = soap_getThermalInfo(con, &thermal_request, &thermal_response);
        if (rv != SOAP_OK) {
                err("soap_getThermalInfo soap call returns error");
                return;
        }
 
        trigger_reading = (SaHpiInt32T)thermal_response.temperatureC;
 
        if ((response->sensorStatus == SENSOR_STATUS_CAUTION &&
             sensor_info->current_state != SAHPI_ES_UPPER_MAJOR) ||
            (response->sensorStatus == SENSOR_STATUS_OK &&
              sensor_info->current_state !=  SAHPI_ES_UNSPECIFIED)) {
                /* Trigger for this event is caution threshold */
                trigger_threshold = thermal_response.cautionThreshold;
        } else if (response->sensorStatus == SENSOR_STATUS_CRITICAL  &&
                   sensor_info->current_state != SAHPI_ES_UPPER_CRIT) { 
                /* Trigger for this event is critical threshold */
                trigger_threshold = thermal_response.criticalThreshold;
        } else {
                dbg("Ignore the event. There is no change in the sensor state");
                return;
        }
 
        /* Process the thermal event from Enclosure and generate appropriate 
	 *  HPI event
         */
        OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_TEMP_STATUS,
                                response->sensorStatus,
                                trigger_reading,trigger_threshold)
 
        return;
}
/** 
 * oa_soap_proc_enc_network_info_changed 
 * 	@oh_handler:  Pointer to openhpi handler structure
 * 	@response:    Pointer to the Enclosure thermal Info structure	
 *
 * Purpose:
 * 	Precesses and updates the ipswap(Enclosure IP Mode) Status
 *
 * Detailed Description: NA
 *
 * Retunr values:
 * 	NONE
 */ 	
void oa_soap_proc_enc_network_info_changed(struct oh_handler_state *oh_handler,
                                    struct  enclosureNetworkInfo *response)    
{
        struct extraDataInfo extra_data_info;
        xmlNode *extra_data = NULL;
        struct oa_soap_handler *oa_handler = NULL;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameters");
                return;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        extra_data = response->extraData;
        while (extra_data) {
                soap_getExtraData(extra_data, &extra_data_info);
                if ((!(strcmp(extra_data_info.name, "IpSwap"))) &&
                         (extra_data_info.value != NULL)) {
                          if(!(strcasecmp(extra_data_info.value,
                                                  "true"))){
                                    oa_handler->ipswap = HPOA_TRUE;
                                    dbg("Enclosure IP Mode is Enabled");
                          } else {
                                    oa_handler->ipswap = HPOA_FALSE;
                                    dbg("Enclosure IP Mode is Disabled");
                          }
                          break;
                }
                extra_data = soap_next_node(extra_data);
        }

        return;
}
