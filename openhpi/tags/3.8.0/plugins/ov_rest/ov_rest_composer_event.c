/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
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
 * Neither the name of the Hewlett Packard Enterprise, nor the names
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
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 */

/* Include libraries */
#include "ov_rest_event.h"
#include "ov_rest_composer_event.h"
#include "ov_rest_discover.h"
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

SaErrorT ov_rest_proc_composer_status(struct oh_handler_state *oh_handler,
 					 struct eventInfo *ov_event,
					 enum healthStatus composer_health)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct composer_status *composer = {0};
	struct applianceInfo appliance = {{0}};
	SaHpiRptEntryT *rpt = NULL;
	SaHpiSeverityT severity = 0;
	struct oh_event event;
	struct enclosureStatusResponse enclosure_response = {0};
	char *enclosure_doc = NULL;
	int bayNumber = 0;
	json_object * jvalue = NULL, *appliance_array = NULL;
	struct ov_rest_sensor_info *sensor_info = NULL;
	SaHpiInt32T sensor_status = 0;
	SaHpiInt32T sensor_val = 0;
	SaHpiRdrT rdr = {0};
	SaHpiResourceIdT resource_id = 0;

	if (oh_handler == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	composer = (struct composer_status*)&ov_handler->
					ov_rest_resources.composer;
	
	/* Check whether this alerts is coming from Active Appliance/Composer */
	bayNumber = ov_rest_get_baynumber(ov_event->resourceID);
	WRAP_ASPRINTF (&ov_handler->connection->url,"https://%s%s" ,
			ov_handler->connection->hostname,ov_event->resourceUri);
	rv = ov_rest_getenclosureStatus(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK){
		return rv;
	}
	if(!enclosure_response.enclosure){
		err("No Response for enclosure status call for composer in "
							"bay %d", bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	appliance_array = ov_rest_wrap_json_object_object_get(
					enclosure_response.enclosure,
					"applianceBays");
	if(!appliance_array ||
		(json_object_get_type(appliance_array) != json_type_array)){
		err("Invalid Response for appliance bay %d", bayNumber);
		ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
		return SA_ERR_HPI_INVALID_DATA;
	}
	jvalue = json_object_array_get_idx(appliance_array, bayNumber-1);
	if(!jvalue){
		err("Invalid Response for appliance bay %d", bayNumber);
		ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
		return SA_ERR_HPI_INVALID_DATA;
	}
	ov_rest_json_parse_applianceInfo(jvalue, &appliance);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	if(!appliance.serialNumber){
		err("No serial number at appliance bay %d", bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	if(strcmp(appliance.serialNumber, composer->serialNumber)){
		warn("This alert is not from Active Appliance, ignore");
		wrap_g_free(enclosure_doc);
		return SA_OK;	
	}	

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache,
			composer->resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for composer in bay %d with resource id %d",
					bayNumber, composer->resource_id);
		wrap_g_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memset(&event, 0, sizeof(struct oh_event));

	switch (composer_health) {
		case OK:
			severity = SAHPI_OK;
			/* Resource restored */
			rpt->ResourceFailed = SAHPI_FALSE;
			event.event.EventDataUnion.ResourceEvent.
				ResourceEventType = SAHPI_RESE_RESOURCE_RESTORED;
			break;
		case Critical:
		case Disabled:
			severity = SAHPI_CRITICAL;
			/* Resource failed */
			rpt->ResourceFailed = SAHPI_TRUE;
			event.event.EventDataUnion.ResourceEvent.
				ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
			break;
		case Warning:
			severity = SAHPI_MAJOR;
			/* Resource failed */
			rpt->ResourceFailed = SAHPI_TRUE;
			event.event.EventDataUnion.ResourceEvent.
				ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
			break;
		default:
			err("Unknown status %d for Composer in bay %d",
						composer_health, bayNumber);
			wrap_g_free(enclosure_doc);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}

	if (rpt->ResourceSeverity == severity) {
		dbg("Ignore the event. There is no change in composer status");
		wrap_g_free(enclosure_doc);
		return rv;
	}

	rpt->ResourceSeverity = severity;
	rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
	if (rv != SA_OK) {
		err("Failed to update rpt for composer in bay %d with "
			" resource id %d", bayNumber, composer->resource_id);
		wrap_g_free(enclosure_doc);
		return rv;
	}

	/* Update the event structure */
	event.hid = oh_handler->hid;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Source =
		composer->resource_id;
	event.event.Severity = severity;
	event.event.EventType = SAHPI_ET_RESOURCE;
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));

	/* Raise the HPI sensor event */
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	/* Build operational status sensor rdr */
	switch (composer_health) {
		case OK:
			sensor_val = OP_STATUS_OK;
			break;
		case Critical:
			sensor_val = OP_STATUS_CRITICAL;
			break;
		case Warning:
			sensor_val = OP_STATUS_WARNING;
			break;
		case Disabled:
			sensor_val = OP_STATUS_DISABLED;
			break;
		default :
			sensor_val = OP_STATUS_UNKNOWN;
	}

	OV_REST_BUILD_ENABLE_SENSOR_RDR(OV_REST_SEN_OPER_STATUS, sensor_val);
		
	wrap_g_free(enclosure_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_composer_insertion_event
 *      @handler: Pointer to openhpi handler structure
 *      @event:   Pointer to OV REST event response structure
 *
 * Purpose:
 *      Adds the newly inserted Composer information into RPT and
 *      RDR table.
 *      Creates the hot swap event.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 *      SA_ERR_HPI_INVALID_DATA   - On Invalid data.
 *      SA_ERR_HPI_INVALID_RESOURCE - On Invalid Resource.
 **/
SaErrorT ov_rest_proc_composer_insertion_event( 
					struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	int bayNumber;
	struct applianceInfo response = {{0}};
	struct applianceHaNodeInfoArrayResponse ha_node_response = {0};
	struct applianceHaNodeInfo ha_node_result = {{0}};
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	char* enclosure_doc = NULL;
	json_object *jvalue_cim = NULL, *jvalue_cim_array = NULL;

	if (handler == NULL || event == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Inserted Composer,"
			" Please Restart the Openhpid");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getenclosureInfoArray(handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	jvalue_cim_array = ov_rest_wrap_json_object_object_get(
			enclosure_response.enclosure_array,
			"applianceBays");
	/* Checking for json object type, if it is not array, return */
	if (jvalue_cim_array == NULL || (json_object_get_type(jvalue_cim_array)
				!= json_type_array)) {
		CRIT("No appliance array for bay %d, Dropping event",
				bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	jvalue_cim = json_object_array_get_idx(jvalue_cim_array, bayNumber-1);
	if(!jvalue_cim){
		CRIT("Invalid response for the appliance in bay %d",
				bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	ov_rest_json_parse_applianceInfo(jvalue_cim, &response);
	/* Make call to the ha-node  */
	if(!response.serialNumber || !strcmp(response.serialNumber, "")){
		CRIT("serial number is NULL in appliance bay %d .\n"
		"Please Restart the Openhpid.", bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_APPLIANCE_HA_NODE_ID_URI,
			ov_handler->connection->hostname,
			response.serialNumber);
	rv = ov_rest_getapplianceHANodeArray(handler,&ha_node_response,
			ov_handler->connection, NULL);
	if(rv != SA_OK){
		CRIT("ov_rest_getapplianceHANodeArray call Failed");
		return rv;
	}
	ov_rest_json_parse_appliance_Ha_node(ha_node_response.haNodeArray,
			&ha_node_result);

	ov_rest_wrap_json_object_put(ha_node_response.root_jobj);
	
	rv  = add_composer(handler, &response, &ha_node_result);
	if(rv != SA_OK){
		CRIT("Failed to add the inserted Composer");
		return rv;
	}
	wrap_free(enclosure_doc);
	return SA_OK;
}


/**
 * ov_rest_proc_composer_removed_event
 *      @handler: Pointer to openhpi handler structure
 *      @event:   Pointer to OV REST event response structure
 *
 * Purpose:
 *      Removes Composer information from Resource Table (RPT) and
 *      Creates the hot swap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid are unknow resource
 **/
SaErrorT ov_rest_proc_composer_removed_event( struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	int bayNumber;
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	char* enclosure_doc = NULL;
	struct enclosureStatus *enclosure = NULL;

	if (handler == NULL || event == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for removed Composer");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getenclosureInfoArray(handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* we are making this call to get just enclosure serial number
	 * This is useful to find the enclosure related to this composer
	 */
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);

	enclosure = (struct enclosureStatus *)ov_handler->
		ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
					enclosure_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the composer is unavailable");
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	if(enclosure->composer.presence[bayNumber - 1] == RES_ABSENT){
		err("Composer does not exist dropping the event, enclosure "
		"serial number %s in bay number %d", enclosure->serialNumber,
						  bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	rv = remove_composer(handler, enclosure, bayNumber);
	if(rv != SA_OK){
		CRIT("Failed to Remove the Composer");
		return rv;
	}
	return SA_OK;
}


