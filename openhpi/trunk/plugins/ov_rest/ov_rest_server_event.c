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
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 *
 * This file has the server blade related events handling
 *
 *      process_server_power_on_event()   - Processes the server power on event
 *
 *     	process_server_power_off_event()  - Processes the server power off
 *					    event
 *      process_server_reset_event()      - Processes the server reset event
 
 *      ov_rest_proc_blade_inserted()     - Processes the server
 *                                          insert completed event
 *      ov_rest_proc_blade_add_complete() - Processes the server
 *                                          add completed task 
 *      ov_rest_proc_blade_removed()      - Processes the server extraction
 *                                          event
 *      build_inserted_server_rpt()       - Builds the rpt entry for inserted
 *                                          server
 *      build_inserted_server_rdr()       - Builds the rdr enty for inserted 
 *      				    server
 **/

#include "ov_rest_event.h"
#include "ov_rest_server_event.h"
#include "ov_rest_discover.h"
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest_control.h"
#include "ov_rest.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * process_server_power_on_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event: Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the server power on hotswap event.  If the sever blade
 *      was powered on after insertion, then the INSERTION_PENDING to
 *      ACTIVE hot swap event is generated.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_server_power_on_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *server_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getserverInfoArray(oh_handler, &response,
			ov_handler->connection, server_doc);
	if (rv != SA_OK || response.server_array == NULL) {
		CRIT("No response from ov_rest_getserverInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse the Server json response*/
	ov_rest_json_parse_server (response.server_array, &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
	/* Now we have to get the enclosure serial number*/
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the server Resourceid by looking at the enclosure linked list*/
	/* FIXME : We could make below code as a funtion to get the resource id
	 * by using enclosure serial number */
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
		CRIT("Enclosure data of the server in bay %d is unavailable",
			info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, 
		enclosure->server.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for server blade in bay %d," 
				"in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data (oh_handler->rptcache, 
				enclosure->server.
				resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of server blade in bay %d, "
				"in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Severity = SAHPI_CRITICAL;

	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		hotswap_state->currentHsState;
	/* Check whether blade is inserted and then powered on */
	switch (hotswap_state->currentHsState)
	{
		case (SAHPI_HS_STATE_INSERTION_PENDING):
			hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			/* INSERTION_PENDING to ACTIVE state change happens due
			 * to auto policy of server blade
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange =
				SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push (oh_handler->eventq, 
					copy_ov_rest_event(&event));
			break;

		case (SAHPI_HS_STATE_INACTIVE):
			event.resource.ResourceSeverity = SAHPI_OK;
			/* The previous state of the server was power off
			 * Update the current hotswap state to ACTIVE
			 */
			hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;

			/* Raise the server power on hotswap event */
			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			/* The cause of the state change is unknown */
			event.event.EventDataUnion.
				HotSwapEvent.CauseOfStateChange =
				SAHPI_HS_CAUSE_UNKNOWN;
			oh_evt_queue_push (oh_handler->eventq, 
					copy_ov_rest_event(&event));

			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			/* INSERTION_PENDING to ACTIVE state change happens due
			 * to Auto policy of server blade
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange =
				SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push (oh_handler->eventq,
					copy_ov_rest_event(&event));
			break;

		default:
			err ("wrong state %d detected for Server Blade"
				" in bay %d", hotswap_state->currentHsState,
						info_result.bayNumber);
			wrap_g_free(enclosure_doc);
			wrap_g_free(server_doc);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	wrap_g_free(enclosure_doc);
	wrap_g_free(server_doc);

	return SA_OK;
}

/**
 * process_server_power_off_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the server power off hotswap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_server_power_off_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *server_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getserverInfoArray(oh_handler, &response,
			ov_handler->connection, server_doc);
	if (rv != SA_OK || response.server_array == NULL) {
		CRIT("No response from ov_rest_getserverInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse the Server json response*/
	ov_rest_json_parse_server (response.server_array, &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
	/* Now we have to get the enclosure serial number*/
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the server Resourceid by looking at the enclosure linked list*/
	/* FIXME : We could make below code as a funtion to get the resource id
	 * by using enclosure serial number */
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
		CRIT("Enclosure data of the server in bay %d is unavailable",
			info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data (oh_handler->rptcache,
				enclosure->
				server.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of server blade in bay %d, "
				"in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, 
			enclosure->
			server.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for server in bay %d", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Severity = SAHPI_CRITICAL;

	if (hotswap_state->currentHsState != SAHPI_HS_STATE_ACTIVE) {
		dbg("Blade is not in proper state");
		dbg("Ignoring the power off event");
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Raise the server power off hotswap event */
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	/* ACTIVE to EXTRACTION_PENDING state change can not be stopped.
	 * Hence, this is unexpected deactivation
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INACTIVE;
	/* EXTRACTION_PENDING to INACTIVE state change happens due to auto
	 * policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.resource.ResourceSeverity = SAHPI_CRITICAL;
	hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;

	wrap_g_free(enclosure_doc);
	wrap_g_free(server_doc);
	return SA_OK;
}

/**
 * process_server_reset_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the server reset hotswap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_server_reset_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *server_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getserverInfoArray(oh_handler, &response,
			ov_handler->connection, server_doc);
	if (rv != SA_OK || response.server_array == NULL) {
		CRIT("No response from ov_rest_getserverInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse the Server json response*/
	ov_rest_json_parse_server (response.server_array, &info_result);
	/* Now we have to get the enclosure serial number*/
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the server Resourceid by looking at the enclosure linked list*/
	/* FIXME : We could make below code as a funtion to get the resource id
	 * by using enclosure serial number */
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
		CRIT("Enclosure data of the server in bay %d is unavailable",
			info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data (oh_handler->rptcache,
				enclosure->server.
				resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of server blade in bay %d, "
				"in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, 
			enclosure->server.
			resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for server in bay %d", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	if (hotswap_state->currentHsState == SAHPI_HS_STATE_INSERTION_PENDING) {
		memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
		event.event.Source = event.resource.ResourceId;
		event.hid = oh_handler->hid;
		event.event.EventType = SAHPI_ET_HOTSWAP;
		oh_gettimeofday(&(event.event.Timestamp));
		event.event.Severity = SAHPI_CRITICAL;
		event.event.EventDataUnion.HotSwapEvent.
			PreviousHotSwapState =
			SAHPI_HS_STATE_INSERTION_PENDING;
		event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_ACTIVE;
		/* INSERTION_PENDING to ACTIVE state change happens due
		 * to Auto policy of server blade
		 */
		event.event.EventDataUnion.HotSwapEvent.
			CauseOfStateChange = SAHPI_HS_CAUSE_AUTO_POLICY;
		oh_evt_queue_push(oh_handler->eventq,
				copy_ov_rest_event(&event));
	}
	wrap_g_free(enclosure_doc);
	wrap_g_free(server_doc);
	return SA_OK;
}

/**
 * build_inserted_server_rpt 
 *      @oh_handler: Pointer to openhpi handler structure
 *      @response:      Pointer to the serverhardwareInfo structure
 *	@rpt:		Ponter to the SaHpiRptEntryT structure
 *
 * Purpose:
 *      Creates and adds the inserted server RPT to the RPT table.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *	SA_ERR_HPI_OUT_OF_MEMORY - Out of Memory
 **/
SaErrorT build_inserted_server_rpt(struct oh_handler_state *oh_handler,
                                   struct serverhardwareInfo *response,
                                   SaHpiRptEntryT *rpt)
{
	SaErrorT rv = SA_OK;
	struct ovRestHotswapState *hotswap_state = NULL;

	if (oh_handler == NULL || response == NULL || rpt == NULL) {
		err("invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	if (ov_rest_build_server_rpt(oh_handler, response, rpt) != SA_OK) {
		err("Building Server RPT failed for inserted blade in bay %d",
							response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		hotswap_state = (struct ovRestHotswapState *)
			g_malloc0(sizeof(struct ovRestHotswapState));
		if (hotswap_state == NULL) {
			err("Out of memory for server blade in bay %d",
							response->bayNumber);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* Inserted server needs some time to stabilize.  Put the
		 * server hotswap state to INSERTION_PENDING.  Once the
		 * server stabilizes, put the hotswap state to ACTIVE
		 * (handled in power on event).
		 */
		hotswap_state->currentHsState =
			SAHPI_HS_STATE_INSERTION_PENDING;
	}

	rv = oh_add_resource(oh_handler->rptcache, rpt, hotswap_state, 0);
	if (rv != SA_OK) {
		err("Failed to add Server rpt in bay %d", response->bayNumber);
		wrap_g_free(hotswap_state);
		return rv;
	}
	wrap_g_free(hotswap_state);
	return SA_OK;
}

/**
 * build_inserted_server_rdr 
 *      @oh_handler: Pointer to openhpi handler
 *      @resource_id: resource id of the Server H/W resource
 *      @response: Pointer to the serverhardwareInfo Struct
 * Purpose:
 *      Build the RDR's for the inserted server H/W
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *	SA_ERR_HPI_INVALID_RESOURCE - Resource is invalid
 **/
SaErrorT build_inserted_server_rdr(struct oh_handler_state *oh_handler,
                          SaHpiResourceIdT resource_id,
                          struct serverhardwareInfo *response,
                          int build_sensors)
{
	SaErrorT rv = SA_OK;
	SaHpiRdrT rdr = {0};
	SaHpiRptEntryT *rpt = NULL;
	struct ov_rest_inventory *inventory = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val;
	/*      enum diagnosticStatus diag_ex_status[OV_REST_MAX_DIAG_EX]; */

	if (oh_handler == NULL || resource_id == 0 || response == NULL) {
		err("Invalid parameter");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	rpt = oh_get_resource_by_id (oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for server in bay %d", response->bayNumber);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	/* Build inventory rdr for server */
	rv = ov_rest_build_server_inv_rdr(oh_handler,resource_id,
			&rdr, &inventory, response);
	if (rv != SA_OK) {
		err("Failed to build server inventory RDR "
				"in slot %d", response->bayNumber);
		return rv;
	}
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
	if (rv != SA_OK) {
		err("Failed to add rdr for server in bay %d",
						response->bayNumber);
		return rv;
	}

        if (rpt->ResourceEntity.Entry[0].EntityType ==
                SAHPI_ENT_SYSTEM_BLADE) {
            /* Build power control rdr for server */
            OV_REST_BUILD_CONTROL_RDR(OV_REST_PWR_CNTRL, 0, 0);
        }
        /* Build UID control rdr for server */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_UID_CNTRL, 0, 0);

        if(build_sensors != TRUE)
                return SA_OK;

         /* Build operational status sensor rdr */
	switch (response->serverStatus ) {
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
 
        OV_REST_BUILD_ENABLE_SENSOR_RDR(OV_REST_SEN_OPER_STATUS,sensor_val); 

	return SA_OK;
}

/**
 * ov_rest_proc_blade_inserted 
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Alert/Event Handler for the server inserted alert.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *	SA_ERR_HPI_INVALID_RESOURCE - Invalid Resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_proc_blade_inserted( struct oh_handler_state *oh_handler, 
					struct eventInfo* event)
{
	SaErrorT rv = 0;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event hs_event = {0};
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureStatusResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	char *server_doc = NULL, *enclosure_doc = NULL;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL;
	char *blade_name = NULL;
	int bayNumber;
	GSList *asserted_sensors = NULL;

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	if(!event->resourceID){
		dbg("Baynumber is unknown for inserted server blade");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getenclosureStatus(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure == NULL) {
		CRIT("No response from ov_rest_getenclosureStatus");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse enclosure information to get enclosure serial number*/
	ov_rest_json_parse_enclosure(enclosure_response.enclosure, 
				&enclosure_result);

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(enclosure_response.devicebay_array) 
						!= json_type_array) {
		CRIT("No server array for bay %d. Dropping event."
			" Server not added", bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}

	jvalue = json_object_array_get_idx (enclosure_response.devicebay_array,
				 bayNumber-1);
	if (!jvalue) {
		CRIT("Invalid response for the enclosue devicebay %d",
							bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	/* With Below Call we dont get much information as it is the object of
	 * enclosure response that we are parsing in below funtion. So we have
	 * to call again with server uri
	 */
	ov_rest_json_parse_server (jvalue, &info_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	if(info_result.uri == NULL){
		err("Resource URI is NULL for inserted server blade in bay %d",
							bayNumber);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			info_result.uri);
	rv = ov_rest_getserverInfoArray(oh_handler, &response,
			ov_handler->connection, server_doc);
	if (rv != SA_OK || response.server_array == NULL) {
		CRIT("No response from ov_rest_getserverInfoArray for "
			"server blade in bay %d", bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_server(response.server_array, &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);

	/* Add the server Blade in to the RPT */
	ov_rest_lower_to_upper (info_result.model,
			strlen (info_result.model), blade_name,
			MAX_256_CHARS);

	/* Build the server RPT entry */
	rv = build_discovered_server_rpt(oh_handler, &info_result, 
					&resource_id);
	if (rv != SA_OK) {
		err("Build rpt failed for inserted server blade in bay %d",
							bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return rv;
	}
	/* Update resource_info structure with resource_id,
	 * serialNumber, and presence status
	 */
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(strstr(enclosure->serialNumber,
				enclosure_result.serialNumber)){
			ov_rest_update_resource_status (&enclosure->server,
					info_result.bayNumber,
					info_result.serialNumber, 
					resource_id, RES_PRESENT,
					info_result.type);
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the server"
				" serial number %s is unavailable",
				info_result.serialNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	/* Build the server RDR */
	rv = build_inserted_server_rdr(oh_handler, resource_id,
			&info_result, TRUE);
	if (rv != SA_OK) {
		err("Build RDR failed for inserted server blade in bay %d",
							bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(oh_handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for server id %d",
					resource_id);
		}
		oh_remove_resource(oh_handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->server, bayNumber,
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, 
				RES_ABSENT, UNSPECIFIED_RESOURCE);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return rv;
	}
	rv = ov_rest_populate_event(oh_handler, resource_id, &hs_event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for server in bay %d",
								bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(server_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for server in bay %d", bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	hs_event.event.EventType = SAHPI_ET_HOTSWAP;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	/* For blades that do not support managed hotswap, current hotswap
	 * state is ACTIVE
	 */
	if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_ACTIVE;
	} else {
		hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_INSERTION_PENDING;
	}
	/* NOT_PRESENT to INSERTION_PENDING/ACTIVE state change happened due
	 * to operator action of blade insertion
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	/* Raise the hotswap event for the inserted server blade */
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&hs_event));
	/* FIXME - For now Do not know how to get the sensors, so skip it*/
	/* Raise the assert sensor events */
#if 0
	if (asserted_sensors)
		ov_rest_assert_sen_evt(oh_handler, &rpt, asserted_sensors);
#endif
	wrap_g_free(enclosure_doc);
	wrap_g_free(server_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_blade_add_complete
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Task/Event Handler for the server add task.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid Resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_proc_blade_add_complete( struct oh_handler_state *oh_handler,
                                          struct eventInfo* event)
{
        SaErrorT rv = SA_OK;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiResourceIdT resource_id = 0;
        struct oh_event hs_event = {0};
        struct ov_rest_handler *ov_handler = NULL;
        struct serverhardwareInfoArrayResponse response = {0};
        struct serverhardwareInfo info_result = {0};
        char *server_doc = NULL;
	struct enclosureStatus *enclosure = NULL;
        GSList *asserted_sensors = NULL;

        if (oh_handler == NULL || event == NULL) {
                err ("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (event->resourceUri == NULL) {
                err ("Resource uri is NULL, failed to add the server blade");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        ov_handler = (struct ov_rest_handler *)oh_handler->data;
	enclosure = ov_handler->ov_rest_resources.enclosure;
        WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
                        ov_handler->connection->hostname,
                        event->resourceUri);
        rv = ov_rest_getserverInfoArray(oh_handler, &response,
                        ov_handler->connection, server_doc);
        if (rv != SA_OK || response.server_array == NULL) {
                CRIT("No response from ov_rest_getserverInfoArray");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        ov_rest_json_parse_server(response.server_array, &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);

        /* Build the server RPT entry */
	rv = build_discovered_server_rpt(oh_handler, &info_result, 
					&resource_id);
        if (rv != SA_OK) {
                err("Build rpt failed for inserted server in bay %d",
						info_result.bayNumber);
                wrap_g_free(server_doc);
                return rv;
        }

        /* Build the server RDR */
        rv = build_inserted_server_rdr(oh_handler, resource_id,
                        &info_result, TRUE);
        if (rv != SA_OK) {
                err("Build RDR failed for inserted server id %d", resource_id);
                /* Free the inventory info from inventory RDR */
                rv = ov_rest_free_inventory_info(oh_handler, resource_id);
                if (rv != SA_OK) {
                        err("Inventory cleanup failed for server id %d",
                                        resource_id);
                }
                oh_remove_resource(oh_handler->rptcache, resource_id);
                wrap_g_free(server_doc);
                return rv;
        }
	while(enclosure != NULL){
		if(strstr(info_result.locationUri,
					enclosure->serialNumber)){
			ov_rest_update_resource_status(&enclosure->server,
					info_result.bayNumber,
					info_result.serialNumber,
					resource_id, RES_PRESENT,
					info_result.type);
			break;
		}
		enclosure = enclosure->next;
	}
        rv = ov_rest_populate_event(oh_handler, resource_id, &hs_event,
                        &asserted_sensors);
        if (rv != SA_OK) {
                err("Populating event struct failed for server id %d",
								resource_id);
                wrap_g_free(server_doc);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);

        hs_event.event.EventType = SAHPI_ET_HOTSWAP;
        hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        /* For blades that do not support managed hotswap, current hotswap
         * state is ACTIVE
         */
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                        SAHPI_HS_STATE_ACTIVE;
        } else {
                hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                        SAHPI_HS_STATE_INSERTION_PENDING;
        }
        /* NOT_PRESENT to INSERTION_PENDING/ACTIVE state change happened due
         * to operator action of blade insertion
         */
        hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                SAHPI_HS_CAUSE_OPERATOR_INIT;
        /* Raise the hotswap event for the inserted server blade */
        oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&hs_event));

        /* FIXME - For now Do not know how to get the sensors, so skip it*/
        /* Raise the assert sensor events */
#if 0
        if (asserted_sensors)
                ov_rest_assert_sen_evt(oh_handler, &rpt, asserted_sensors);
#endif
        wrap_g_free(server_doc);
        return SA_OK;
}

/**
 *   remove_server_blade
 *    @oh_handler: Pointer to openhpi handler
 *    @bay_number: Bay number of the removed blade
 * Purpose:
 *      Remove the Server Blade from the RPTable
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_server_blade(struct oh_handler_state *oh_handler,
                                SaHpiInt32T bay_number,
                                struct enclosureStatus *enclosure)
{
	SaErrorT rv = SA_OK;
	struct ovRestHotswapState *hotswap_state = NULL;
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event event = {0};
	SaHpiResourceIdT resource_id;

	if (oh_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	resource_id = enclosure->server.resource_id[bay_number-1];

	ov_rest_update_hs_event(oh_handler, &event);

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for server blade in bay %d", bay_number);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Severity = SAHPI_CRITICAL;

	if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		/* Simple hotswap */
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			SAHPI_HS_STATE_ACTIVE;
	} else {
		/* Managed hotswap */
		hotswap_state = (struct ovRestHotswapState *)
			oh_get_resource_data(oh_handler->rptcache,
					event.resource.ResourceId);
		if (hotswap_state == NULL) {
			err("Failed to get hotswap state of server blade "
				"in bay %d", bay_number);
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState = SAHPI_HS_STATE_INACTIVE;
		} else {
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState =
				hotswap_state->currentHsState;
		}
	}
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;

	if (event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState ==
			SAHPI_HS_STATE_INACTIVE) {
		/* INACTIVE to NOT_PRESENT state change happened due to
		 * operator action */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_OPERATOR_INIT;
	} else {
		/* This state change happened due to a surprise extraction */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
	}

	/* Push the hotswap event to remove the resource from OpenHPI RPTable
	*/
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(oh_handler, rpt->ResourceId);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for server id %d",
				rpt->ResourceId);
	}
	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(oh_handler->rptcache,
				rpt->ResourceId);

	/* reset resource_info structure to default values */
	ov_rest_update_resource_status(
			&enclosure->server, bay_number,
			"", SAHPI_UNSPECIFIED_RESOURCE_ID, 
			RES_ABSENT, UNSPECIFIED_RESOURCE);

	return SA_OK;
}

/**
 *  ov_rest_proc_blade_removed 
 *    @handler: Pointer to openhpi handler
 *    @event: Ponter to the eventInfo structure
 * Purpose:
 *      Event/Alert Handler fot the Server Extractions alert/event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_blade_removed( struct oh_handler_state *handler, 
					struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatusResponse enclosure_response = {0};
	struct enclosureDeviceBays result = {0};
	struct enclosureInfo enc_info = {{0}};
	struct enclosureStatus *enclosure = NULL;
	int bayNumber = 0;
	char *enclosure_doc = NULL;
	json_object * jvalue = NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Baynumber is unknown for inserted server blade");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);

	WRAP_ASPRINTF (&ov_handler->connection->url,"https://%s%s" ,
			ov_handler->connection->hostname,event->resourceUri);
	rv = ov_rest_getenclosureStatus(handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure == NULL) {
		CRIT("No response from ov_rest_getenclosureStatus");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure,
			&enc_info);

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(enclosure_response.devicebay_array) !=
					json_type_array) {
		CRIT("No server array for bay %d, dropping event."
			" Server not removed", bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}

	jvalue = json_object_array_get_idx(enclosure_response.devicebay_array, 
				bayNumber-1);
	if (!jvalue) {
		CRIT("Invalid response for the enclosure devicebay %d",
								bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	ov_rest_json_parse_enc_device_bays (jvalue, &result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(strstr(enclosure->serialNumber,enc_info.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure is not identified to remove blade in bay %d",
			bayNumber);
		wrap_g_free(enclosure_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	if(result.changeState == Remove || result.presence == Absent){
		if(enclosure->server.presence[bayNumber - 1] != RES_ABSENT){
			rv = remove_server_blade(handler, bayNumber, enclosure);
			if (rv != SA_OK) {
				err("Server blade %d removal failed",
						bayNumber);
				wrap_g_free(enclosure_doc);
				return rv;
			} else
				CRIT("Server in slot %d is removed", bayNumber);
		}else {
			err("Server in slot %d is already removed or empty",
					bayNumber);
			wrap_g_free(enclosure_doc);
			return SA_OK;
		}
	}

	wrap_g_free(enclosure_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_server_status
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the server status event.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_server_status(struct oh_handler_state *oh_handler,
                                        struct eventInfo* ov_event)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;
        struct serverhardwareInfoArrayResponse response = {0};
        struct serverhardwareInfo info_result = {0};
        struct enclosureInfoArrayResponse enclosure_response = {0};
        struct enclosureInfo enclosure_result = {{0}};
        struct enclosureStatus *enclosure = NULL;
        char* enclosure_doc = NULL, *server_doc = NULL;
        SaHpiRptEntryT *rpt = NULL;

        if (oh_handler == NULL || ov_event == NULL)
        {
                err ("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        ov_handler = (struct ov_rest_handler *)oh_handler->data;
        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                        ov_handler->connection->hostname,
                        ov_event->resourceUri);
        rv = ov_rest_getserverInfoArray(oh_handler, &response,
                        ov_handler->connection, server_doc);
        if (rv != SA_OK || response.server_array == NULL) {
                CRIT("No response from ov_rest_getserverInfoArray");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        /* Parse the Server json response*/
        ov_rest_json_parse_server (response.server_array, &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
        /* Now we have to get the enclosure serial number*/
        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                        ov_handler->connection->hostname,
                        info_result.locationUri);
        rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
                        ov_handler->connection, enclosure_doc);
        if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
                CRIT("No response from ov_rest_getenclosureInfoArray");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
                        &enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
        /* Find the server Resourceid by looking at the enclosure linked list*/
        /* FIXME : We could make below code as a funtion to get the resource id
         * by using enclosure serial number */
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
                CRIT("Enclosure data of the server in bay %d is unavailable",
                        info_result.bayNumber);
                wrap_g_free(enclosure_doc);
                wrap_g_free(server_doc);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }


        /* Get the rpt entry of the resource */
        rpt = oh_get_resource_by_id(oh_handler->rptcache,
                        enclosure->
                        server.resource_id[info_result.bayNumber - 1]);
        if (rpt == NULL) {
                err("RPT is NULL for server in bay %d", info_result.bayNumber);
                wrap_g_free(enclosure_doc);
                wrap_g_free(server_doc);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = ov_rest_gen_res_event(oh_handler, rpt, info_result.serverStatus);
        if (rv != SA_OK) {
                err("Failed to generate resource event for server in bay %d",
                        info_result.bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * process_drive_enclosure_power_on_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event: Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the drive enclosure power on hotswap event.
 *      If the drive enclosure was powered on after insertion, then the 
 *      INSERTION_PENDING to ACTIVE hot swap event is generated.
 *
 *      If The Drive enclosure was in INACTIVE state then
 *      INACTIVE to INSERTION_PENDING
 *      INSERTION_PENDING to ACTIVE hot swap events gets generated.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_drive_enclosure_power_on_event(
					struct oh_handler_state *oh_handler,
					struct eventInfo *ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct driveEnclosureInfoArrayResponse response = {0};
	struct driveEnclosureInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *drive_enc_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getdriveEnclosureInfoArray(oh_handler, &response,
			ov_handler->connection, drive_enc_doc);
	if (rv != SA_OK || response.drive_enc_array == NULL) {
		CRIT("No response from ov_rest_getdriveEnclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}	
	/* Parse the drive enclosure json response*/
	ov_rest_json_parse_drive_enclosure(response.drive_enc_array, 
			&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
	/* Now we have to get the enclosure serial number*/
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the Drive Enclosure Resourceid by looking at the enclosure 
	 * linked list*/
	/* FIXME : We could make below code as a funtion to get the resource id
	 * by using enclosure serial number */
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
		CRIT("Enclosure data of the drive enclosure in bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, 
			enclosure->server.
			resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for drive enclosure in bay %d", 
						info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data (oh_handler->rptcache, 
				enclosure->server.
				resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of server blade in bay %d, "
				"in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Severity = SAHPI_CRITICAL;

	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		hotswap_state->currentHsState;
	/* Check whether Drive Enclosure is inserted and then powered on */
	switch (hotswap_state->currentHsState)
	{
		case (SAHPI_HS_STATE_INSERTION_PENDING):
			hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			/* INSERTION_PENDING to ACTIVE state change happens due
			 * to auto policy of Drive Enclosure 
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange =
				SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push (oh_handler->eventq, 
					copy_ov_rest_event(&event));
			break;

		case (SAHPI_HS_STATE_INACTIVE):
			event.resource.ResourceSeverity = SAHPI_OK;
			/* The previous state of the Drive Enclosure was power 
			 * off Update the current hotswap state to ACTIVE
			 */
			hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;

			/* Raise the drive enclosure power on hotswap event */
			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			/* The cause of the state change is unknown */
			event.event.EventDataUnion.
				HotSwapEvent.CauseOfStateChange =
				SAHPI_HS_CAUSE_UNKNOWN;
			oh_evt_queue_push (oh_handler->eventq, 
					copy_ov_rest_event(&event));

			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			/* INSERTION_PENDING to ACTIVE state change happens due
			 * to Auto policy of drive enclosure
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange =
				SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push (oh_handler->eventq,
					copy_ov_rest_event(&event));
			break;

		default:
			err ("Wrong hotswap state %d detected for server blade "
				"in bay %d", hotswap_state->currentHsState,
						info_result.bayNumber);
			wrap_g_free(enclosure_doc);
			wrap_g_free(drive_enc_doc);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	wrap_g_free(enclosure_doc);
	wrap_g_free(drive_enc_doc);

	return SA_OK;
}


/**
 * process_drive_enclosure_power_off_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the drive enclosure power off hotswap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_drive_enclosure_power_off_event(
					struct oh_handler_state *oh_handler,
					struct eventInfo *ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct driveEnclosureInfoArrayResponse response = {0};
	struct driveEnclosureInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *drive_enc_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getdriveEnclosureInfoArray(oh_handler, &response,
			ov_handler->connection, drive_enc_doc);
	if (rv != SA_OK || response.drive_enc_array == NULL) {
		CRIT("No response from ov_rest_getdriveEnclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse the drive enclosure json response*/
	ov_rest_json_parse_drive_enclosure(response.drive_enc_array, 
			&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
	/* Now we have to get the enclosure serial number*/
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s", 
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array  == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArrayy");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the drive enclosure Resourceid by looking at the enclosure 
 	 * linked list*/
	/* FIXME : We could make below code as a funtion to get the resource id
	 * by using enclosure serial number */
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
		CRIT("Enclosure data of the drive enclosure n bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data (oh_handler->rptcache,
				enclosure->
				server.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of drive enclosure in bay %d"
				", in enclosure rid %d", info_result.bayNumber,
						enclosure->enclosure_rid);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, 
			enclosure->
			server.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for drive enclosure in bay %d",
						info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.event.Severity = SAHPI_CRITICAL;

	if (hotswap_state->currentHsState != SAHPI_HS_STATE_ACTIVE) {
		dbg("Drive enclosure is not in proper state");
		dbg("Ignoring the power off event");
		wrap_g_free(enclosure_doc);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Raise the drive enclosure power off hotswap event */
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	/* ACTIVE to EXTRACTION_PENDING state change can not be stopped.
	 * Hence, this is unexpected deactivation
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INACTIVE;
	/* EXTRACTION_PENDING to INACTIVE state change happens due to auto
	 * policy of drive enclosure
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.resource.ResourceSeverity = SAHPI_CRITICAL;
	hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;

	wrap_g_free(enclosure_doc);
	wrap_g_free(drive_enc_doc);
	return SA_OK;
}


/**
 * build_inserted_drive_enclosure_rpt 
 *      @oh_handler: Pointer to openhpi handler structure
 *      @response:      Pointer to the driveEnclosureInfo structure
 *	@rpt:		Ponter to the SaHpiRptEntryT structure
 *
 * Purpose:
 *      Creates and adds the inserted drive enclosure RPT to the RPT table.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *	SA_ERR_HPI_OUT_OF_MEMORY - Out of Memory
 **/
SaErrorT build_inserted_drive_enclosure_rpt(struct oh_handler_state 
			*oh_handler, struct driveEnclosureInfo *response,
			SaHpiRptEntryT *rpt)
{
	SaErrorT rv = SA_OK;
	struct ovRestHotswapState *hotswap_state = NULL;

	if (oh_handler == NULL || response == NULL || rpt == NULL) {
		err("invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	if (ov_rest_build_drive_enclosure_rpt(oh_handler, response, rpt) 
		!= SA_OK) {
		err("Building RPT failed for the inserted drive enclosure"
			" in bay %d", response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		hotswap_state = (struct ovRestHotswapState *)
			g_malloc0(sizeof(struct ovRestHotswapState));
		if (hotswap_state == NULL) {
			err("Out of memory for drive enclosure in bay %d",
						response->bayNumber);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* Inserted ve enclosure needs some time to stabilize.  Put the
		 * server hotswap state to INSERTION_PENDING.  Once the
		 * drive enclosure stabilizes, put the hotswap state to ACTIVE
		 * (handled in power on event).
		 */
		hotswap_state->currentHsState =
			SAHPI_HS_STATE_INSERTION_PENDING;
	}

	rv = oh_add_resource(oh_handler->rptcache, rpt, hotswap_state, 0);
	if (rv != SA_OK) {
		err("Failed to add RPT for drive enclosure in bay %d",
							response->bayNumber);
		wrap_g_free(hotswap_state);
		return rv;
	}
	wrap_g_free(hotswap_state);
	return SA_OK;
}

/**
 * ov_rest_proc_drive_enclosure_add_complete
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Task/Event Handler for the drive-enclosure add task.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid Resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_proc_drive_enclosure_add_complete( 
					struct oh_handler_state *oh_handler,
                                        struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	SaHpiRptEntryT rpt = {0};
	struct oh_event hs_event = {0};
	struct ov_rest_handler *ov_handler = NULL;
	struct driveEnclosureInfoArrayResponse response = {0};
	struct driveEnclosureInfo info_result = {0};
	struct enclosureStatus *enclosure = NULL;
	char *drive_enc_doc = NULL;
	GSList *asserted_sensors = NULL;

	if (oh_handler == NULL || oh_handler->data == NULL || event == NULL) {
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	if (event->resourceUri == NULL) {
		err("Resource uri is NULL, failed to add the drive enclosure");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	enclosure = ov_handler->ov_rest_resources.enclosure;
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getdriveEnclosureInfoArray(oh_handler, &response,
			ov_handler->connection, drive_enc_doc);
	if (rv != SA_OK || response.drive_enc_array == NULL) {
		CRIT("No response from ov_rest_getdriveEnclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_drive_enclosure(response.drive_enc_array, 
			&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);

	/* Build the drive enclosure RPT entry */
	rv = build_inserted_drive_enclosure_rpt(oh_handler, &info_result, &rpt);
	if (rv != SA_OK) {
		err("Build rpt failed for inserted drive enclosure in bay %d",
							info_result.bayNumber);
		wrap_g_free(drive_enc_doc);
		return rv;
	}

	/* Build the drive enclosure RDR */
	rv = ov_rest_build_drive_enclosure_rdr(oh_handler,rpt.ResourceId,
			&info_result);
	if (rv != SA_OK) {
		err("Build RDR failed for inserted drive enclosure in bay %d",
							info_result.bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(oh_handler, rpt.ResourceId);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for drive enclosure "
					"id %d", rpt.ResourceId);
		}
		oh_remove_resource(oh_handler->rptcache, rpt.ResourceId);
		wrap_g_free(drive_enc_doc);
		return rv;
	}
	while(enclosure != NULL){
		if(strstr(info_result.locationUri,
					enclosure->serialNumber)){
			ov_rest_update_resource_status(&enclosure->server,
					info_result.bayNumber,
					info_result.serialNumber,
					rpt.ResourceId, RES_PRESENT,
					info_result.type);
			break;
		}
		enclosure = enclosure->next;
	}

	rv = ov_rest_populate_event(oh_handler, rpt.ResourceId, &hs_event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for drive enclosure "
				"in bay %d", info_result.bayNumber);
		wrap_g_free(drive_enc_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	hs_event.event.EventType = SAHPI_ET_HOTSWAP;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	/* For blades that do not support managed hotswap, current hotswap
	 * state is ACTIVE
	 */
	if (!(rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_ACTIVE;
	} else {
		hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_INSERTION_PENDING;
	}
	/* NOT_PRESENT to INSERTION_PENDING/ACTIVE state change happened due
	 * to operator action of blade insertion
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	/* Raise the hotswap event for the inserted server blade */
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&hs_event));

	wrap_g_free(drive_enc_doc);
	return SA_OK;
}

