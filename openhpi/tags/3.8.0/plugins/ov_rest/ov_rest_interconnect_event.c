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
 * This file has the interconnect blade related events handling
 *
 *	ov_rest_proc_interconnect_inserted	- Process the interconnect
 *						  insertion event
 *	ov_rest_proc_interconnect_add_complete	- Process the interconnect
 *						  add task
 *	ov_rest_proc_interconnect_removed	- Process the interconnect
 *						  removal event
 *	remove_interconnect_blade		- Removes the Interconnect
 *						  resource entry from the RPT
 *	process_interconnect_reset_task		- Process the interconeect
 *						  reset event.
 **/

#include "ov_rest_event.h"
#include "ov_rest_interconnect_event.h"
#include "ov_rest_discover.h"
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * remove_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the removed interconnect
 *
 * Purpose:
 *      Removes the interconnect.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_interconnect_blade(struct oh_handler_state *oh_handler,
                             SaHpiInt32T bay_number,
			     struct enclosureStatus *enclosure)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiResourceIdT resource_id = 0;

	if (oh_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_rest_update_hs_event(oh_handler, &event);

	resource_id = enclosure->interconnect.resource_id[bay_number - 1];

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect removed from "
				"bay %d in enclosure rid %d",bay_number, 
						enclosure->enclosure_rid);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.event.Severity = event.resource.ResourceSeverity;

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(oh_handler->rptcache,
				event.resource.ResourceId);
	if (hotswap_state == NULL) {
		err("Failed to get hotswap state for the interconnect %d in "
			"enclosure rid %d",
			bay_number,enclosure->enclosure_rid);
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			SAHPI_HS_STATE_INACTIVE;
	}
	else
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			hotswap_state->currentHsState;

	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;

	if ((hotswap_state) &&
			(hotswap_state->currentHsState == SAHPI_HS_STATE_INACTIVE)) {
		/* INACTIVE to NOT_PRESENT state change happened due to
		 * operator action
		 */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_OPERATOR_INIT;
	} else {
		/* This state change happened due to surprise extraction */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
	}

	/* Push the hotswap event to remove the resource from OpenHPI RPTable
	 */
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(oh_handler, rpt->ResourceId);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for resource id %d",
				rpt->ResourceId);
	}
	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(oh_handler->rptcache,
			event.resource.ResourceId);

	/* reset resource_info structure to default values */
	ov_rest_update_resource_status(
			&enclosure->interconnect, bay_number,
			"", SAHPI_UNSPECIFIED_RESOURCE_ID, 
                        RES_ABSENT, UNSPECIFIED_RESOURCE);
	return SA_OK;
}

/**
 * ov_rest_proc_interconnect_inserted
 *      @handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Alert/Event Handler for the interconnect inserted alert/event.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid Resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_proc_interconnect_inserted( struct oh_handler_state *handler, 
						struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	SaHpiInt32T bayNumber = 0;
	SaHpiResourceIdT resource_id = 0;
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event hs_event = {0};
	struct ovRestHotswapState * hotswap_state = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo info_result = {0};
	GSList *asserted_sensors = NULL;
	json_object *jvalue_interconnect_array = NULL, *jvalue = NULL;
	char *enclosure_doc = NULL, *interconnect_doc = NULL;
	char *blade_name = NULL;

	if (handler == NULL || event == NULL) {
		err("Wrong parameters passed");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Inserted Interconnect");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getenclosureInfoArray(handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Parse enclosure information to get enclosure serial number*/
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array, 
			&enclosure_result);
	jvalue_interconnect_array = ov_rest_wrap_json_object_object_get(
			enclosure_response.enclosure_array,
			"interconnectBays");
	/* Checking for json object type, if it is not array, return */
	if (jvalue_interconnect_array == NULL ||
		(json_object_get_type(jvalue_interconnect_array) !=
						json_type_array)) {
		CRIT("No Interconnect array for bay %d. Dropping Event",
			bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}

	jvalue = json_object_array_get_idx (jvalue_interconnect_array, 
			bayNumber-1);
	if (!jvalue) {
		CRIT("Invalid response for the interconnect in bay %d",
								bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	/* At this moment interconnect JSON object is collected from enclosure
	 * response, So it contains very minimal and fortunatly we get the 
	 * interconnect resource uri if the interconnect is present in that
	 * particular slot 
	 */
	ov_rest_json_parse_interconnect (jvalue, &info_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Take the interconnect URI and issue the GET call on it to get full
	 * details of the interconnect this time
	 */
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			info_result.uri);
	rv = ov_rest_getinterconnectInfoArray(handler, &response,
			ov_handler->connection, interconnect_doc);
	if (rv != SA_OK || response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray for "
						"interconnects");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_interconnect(response.interconnect_array,
			&info_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);

	ov_rest_lower_to_upper (info_result.model,
			strlen (info_result.model), blade_name,
			MAX_256_CHARS);
	/* Add the Interconnect in to the RPT */
	/* Build rpt entry for server */

	/* FIXME: Add new funtion to build RPT when interconnect inserted*/
	rv = ov_rest_build_interconnect_rpt (handler,
			&info_result, &resource_id);
	if (rv != SA_OK)
	{
		err ("Failed to Add interconnect rpt for bay %d.",
				info_result.bayNumber);
		wrap_free(enclosure_doc);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while (enclosure != NULL)
	{
		if (strstr (enclosure->serialNumber, 
					enclosure_result.serialNumber))
		{
			ov_rest_update_resource_status(&enclosure->interconnect,
					info_result.bayNumber,
					info_result.serialNumber,
					resource_id,
					RES_PRESENT,
					info_result.type);
			break;
		}
		enclosure = enclosure->next;
	}
	if (enclosure == NULL)
	{
		CRIT ("Enclosure location of the interconnect"
				" serial number %s is unavailable",
				info_result.serialNumber);
		wrap_free(enclosure_doc);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	/* Build rdr entry for server */
	ov_rest_build_interconnect_rdr (handler, resource_id, &info_result);
	rv = ov_rest_populate_event(handler, resource_id, &hs_event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Creating hotswap event failed for the interconnect "
			"inserted in bay %d,in enclosure rid %d",bayNumber, 
						enclosure->enclosure_rid);
		wrap_free(enclosure_doc);
		wrap_free(interconnect_doc);
		return rv;
	}

	hs_event.event.EventType = SAHPI_ET_HOTSWAP;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* NOT_PRESENT to INSERTION_PENDING state change happened due
	 * to operator action
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));

	/* Get the rpt entry of the server */
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for the inserted interconnect in "
				"bay %d in enclosure rid %d", bayNumber, 
						enclosure->enclosure_rid);
		wrap_free(enclosure_doc);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(handler->rptcache, resource_id);
	if (hotswap_state == NULL) {
		err("Failed to get hotswap state of interconnect blade "
			"in bay %d,in enclosure rid %d",bayNumber, 
						enclosure->enclosure_rid);
		wrap_free(enclosure_doc);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;

	ov_rest_update_hs_event(handler, &hs_event);
	memcpy(&(hs_event.resource), rpt, sizeof(SaHpiRptEntryT));
	hs_event.event.Source = hs_event.resource.ResourceId;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened due
	 * to Auto policy of server blade
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));
	/* Raise the assert sensor events */

	wrap_free(enclosure_doc);
	wrap_free(interconnect_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_interconnect_add_complete
 *      @handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Task/Event Handler for the interconnect add task. 
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid Resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_proc_interconnect_add_complete(
				struct oh_handler_state *handler,
				struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event hs_event = {0};
	struct ovRestHotswapState * hotswap_state = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo info_result = {0};
	struct enclosureStatus *enclosure = NULL;
	GSList *asserted_sensors = NULL;
	char *interconnect_doc = NULL;

	if (handler == NULL || event == NULL) {
		err("wrong parameters passed");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)handler->data;
	enclosure = ov_handler->ov_rest_resources.enclosure;
        if (event->resourceUri == NULL) {
                 err ("resourceUri is NULL, failed to add interconnect");
		return SA_ERR_HPI_INVALID_PARAMS;
        }

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			event->resourceUri);
	rv = ov_rest_getinterconnectInfoArray(handler, &response,
			ov_handler->connection, interconnect_doc);
	if (rv != SA_OK || response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray"
				" for interconnects");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_interconnect(response.interconnect_array,
			&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);

	/* Add the Interconnect into the RPT */
	rv = ov_rest_build_interconnect_rpt (handler,
			&info_result, &resource_id);
	if (rv != SA_OK)
	{
		err ("Failed to Add interconnect rpt for bay %d.",
				info_result.bayNumber);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Build rdr entry for server */
	rv = ov_rest_build_interconnect_rdr (handler, resource_id, &info_result);
        if (rv != SA_OK) {
                err("Failed to build the interconnect RDR in bay %d",
					info_result.bayNumber);
                rv = oh_remove_resource(handler->rptcache,
                                        (int)atoi(event->resourceID));
                return rv;
        }
	while(enclosure != NULL){
                if(strstr(enclosure->serialNumber,
                                        info_result.locationUri)){
                        ov_rest_update_resource_status(&enclosure->server,
                                        info_result.bayNumber,
                                        info_result.serialNumber,
                                        resource_id, RES_PRESENT,
                                        info_result.type);
                        break;
                }
                enclosure = enclosure->next;
        }

	rv = ov_rest_populate_event(handler, resource_id, &hs_event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Creating hotswap event failed for the interconnect"
			" in bay %d", info_result.bayNumber);
		wrap_free(interconnect_doc);
		return rv;
	}

	hs_event.event.EventType = SAHPI_ET_HOTSWAP;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* NOT_PRESENT to INSERTION_PENDING state change happened due
	 * to operator action
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));

	/* Get the rpt entry of the interconnect */
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect in bay %d",
					 info_result.bayNumber);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(handler->rptcache, resource_id);
	if (hotswap_state == NULL) {
		err("Failed to get hotswap state of interconnect blade"
			" in bay %d", info_result.bayNumber);
		wrap_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;

	ov_rest_update_hs_event(handler, &hs_event);
	memcpy(&(hs_event.resource), rpt, sizeof(SaHpiRptEntryT));
	hs_event.event.Source = hs_event.resource.ResourceId;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened due
	 * to Auto policy of server blade
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));
	/* Raise the assert sensor events */

	wrap_free(interconnect_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_interconnect_removed
 *      @handler: Pointer to openhpi handler
 *      @event: Ponter to the eventInfo structure
 * Purpose:
 *      Event/Alert Handler fot the Interconnect Extractions alert/event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *	SA_ERR_HPI_INVALID_RESOURCE - on invalid resource
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_interconnect_removed( struct oh_handler_state *handler,
					 struct eventInfo* event)
{
	/* As we dont get either Bay number or resource uri of the extracted
	 * resource, we have to depend on the location uri and the Resource
	 * presence Matrix*/
	SaErrorT rv = 0;
	SaHpiInt32T bayNumber = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatusResponse response = {0};
	struct enclosureInfo enc_info = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char *enclosure_doc = NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Interconnect to Remove");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);

	WRAP_ASPRINTF (&ov_handler->connection->url,"https://%s%s" ,
			ov_handler->connection->hostname,event->resourceUri);
	rv = ov_rest_getenclosureStatus(handler, &response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || response.enclosure == NULL) {
		CRIT("No response from ov_rest_getenclosureStatus");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}	
	ov_rest_json_parse_enclosure(response.enclosure, &enc_info);
	ov_rest_wrap_json_object_put(response.root_jobj);
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(strstr(enclosure->serialNumber,enc_info.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure is not identified to remove the "
				"blade from bay %d",bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	if(enclosure->interconnect.presence[bayNumber-1] != RES_ABSENT){
		rv = remove_interconnect_blade(handler, bayNumber,
				enclosure);
		if (rv != SA_OK) {
			err("Interconnect blade %d removal failed",
					bayNumber);
			wrap_free(enclosure_doc);
			return rv;
		} else
			err("Interconnect in slot %d is removed from "
				"enclosure rid %d", bayNumber, 
						enclosure->enclosure_rid);
	}

	wrap_free(enclosure_doc);
	return SA_OK;
}

/**
 * process_interconnect_power_off_task
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      - Creates the Interconnect Power Off hotswap events.
 *
 * Detailed Description:
 *      - On powering off interconnect, it will take approximately
 *        30 seconds to get correct power state. So its done in loops
 *        with sleep of 10 seconds every poll.
 *      - Gets enclosure information where interconnect resides.
 *      - Gets hotswap state of interconnect.
 *      - Gets rpt entry for a resource.
 *      - Creates the Interconnect Power Off hotswap events,
 *        ACTIVE->EXTRACTION_PENDING and EXTRACTION_PENDING->INACTIVE.
 *      
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT process_interconnect_power_off_task(
                                        struct oh_handler_state *oh_handler,
                                        struct eventInfo* ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *interconnect_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;
	int polls = 0;

	if(oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;

	for (polls=0; polls < OV_MAX_POWER_POLLS; polls++) {
		WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
				ov_handler->connection->hostname,
				ov_event->resourceUri);
		rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
		if (rv != SA_OK || response.interconnect_array == NULL) {
			CRIT("Failed to get Interconnect Info Array");
			continue;
		}
		ov_rest_json_parse_interconnect(response.interconnect_array,
								&info_result);
		ov_rest_wrap_json_object_put(response.root_jobj);
		if (info_result.powerState == Off)
			break;

		wrap_g_free(interconnect_doc);
		sleep(OV_POWER_POLL_SLEEP_SECONDS);
	}

	if( polls == OV_MAX_POWER_POLLS){
		err("Failed to get the requested state even after %d seconds",
			OV_MAX_POWER_POLLS*OV_POWER_POLL_SLEEP_SECONDS);
		return( SA_ERR_HPI_INVALID_STATE);
	}
	
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("Failed to get Enclosure Info Array");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the interconnect Resourceid by looking at the enclosure
	 * linked list */
	enclosure = (struct enclosureStatus *)
				ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
				enclosure_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the interconnect in bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of Interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	memset(&event, 0, sizeof(struct oh_event));
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.resource.ResourceSeverity = SAHPI_CRITICAL;
	event.event.Severity = event.resource.ResourceSeverity;
	hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;

	/* Raise the power off hotswap event */
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
				SAHPI_HS_STATE_ACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_EXTRACTION_PENDING;
	/* ACTIVE to EXTRACTION_PENDING state change happened
	 * due power off event. The deactivation can not be
	 * stopped.
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
				SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
				SAHPI_HS_STATE_EXTRACTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_INACTIVE;
	/* EXTRACTION_PENDING to INACTIVE state change happens
	 * due to auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
				SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	wrap_g_free(enclosure_doc);
	wrap_g_free(interconnect_doc);

	return SA_OK;
}

/**
 * process_interconnect_power_on_task
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the Interconnect Power On hotswap events.
 *
 * Detailed Description: NA
 *      - On powering on interconnect, it will take approximately
 *        10 seconds to get correct power state. So its done in loops
 *        with sleep of 10 seconds every poll.
 *      - Gets enclosure information where interconnect resides.
 *      - Gets hotswap state of interconnect.
 *      - Gets rpt entry for a resource.
 *      - Creates the Interconnect Power On hotswap events,
 *        INACTIVE->INSERTION_PENDING and INSERTION_PENDING->ACTIVE.
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT process_interconnect_power_on_task(
                                        struct oh_handler_state *oh_handler,
                                        struct eventInfo* ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *interconnect_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;
	int polls = 0;

	if(oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;

	for (polls=0; polls < OV_MAX_POWER_POLLS; polls++) {
		WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
				ov_handler->connection->hostname,
				ov_event->resourceUri);
		rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
		if (rv != SA_OK || response.interconnect_array == NULL) {
			CRIT("Failed to get Interconnect Info Array");
			continue;
		}
		ov_rest_json_parse_interconnect(response.interconnect_array,
								&info_result);
		ov_rest_wrap_json_object_put(response.root_jobj);
		if (info_result.powerState == On)
			break;

		wrap_g_free(interconnect_doc);
		sleep(OV_POWER_POLL_SLEEP_SECONDS);
	}

	if( polls == OV_MAX_POWER_POLLS){
		err("Failed to get the requested state even after %d seconds",
			OV_MAX_POWER_POLLS*OV_POWER_POLL_SLEEP_SECONDS);
		return( SA_ERR_HPI_INVALID_STATE);
	}	

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("Failed to get Enclosure Info Array");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
	/* Find the interconnect Resourceid by looking at the enclosure
	 * linked list.
	 */
	enclosure = (struct enclosureStatus *)
				ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
					enclosure_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the interconnect in bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of Interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	memset(&event, 0, sizeof(struct oh_event));
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.resource.ResourceSeverity = SAHPI_OK;
	event.event.Severity = event.resource.ResourceSeverity;
	/* Update the current hot swap state to ACTIVE */
	hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
	/* Check for any thermal sensor state
	 * and raise the deassert events. But currently we dont have any
	 */

	/* Raise the power on hotswap event*/
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
					SAHPI_HS_STATE_INACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
					SAHPI_HS_STATE_INSERTION_PENDING;
	/* The cause of the state change is unknown */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
					SAHPI_HS_CAUSE_UNKNOWN;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
					SAHPI_HS_STATE_INSERTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
					SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened
	 * Auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
					SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	wrap_g_free(enclosure_doc);
	wrap_g_free(interconnect_doc);

	return SA_OK;
}

/**
 * ov_rest_proc_switch_status_change 
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates Interconnect/Switch status change (Power Off/On) hotswap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *	SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_switch_status_change( struct oh_handler_state 
				*oh_handler, struct eventInfo* ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo info_result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *interconnect_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if(oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
	if (rv != SA_OK || response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray"
				" for interconnects");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_interconnect(response.interconnect_array, 
			&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
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
	/* Find the interconnect Resourceid by using the enclosure linked list*/
	enclosure = (struct enclosureStatus *)
			ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
				enclosure_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the interconnect in bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of Interconnect blade in bay"
			" %d", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber, 
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	memset(&event, 0, sizeof(struct oh_event));
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	switch(info_result.powerState) {
		case (Off):
			event.resource.ResourceSeverity = SAHPI_CRITICAL;
			hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;
			if (rv != SA_OK) {
				err("Add rpt entry failed for the "
					"interconnect in bay %d",
					info_result.bayNumber);
				wrap_g_free(enclosure_doc);
				wrap_g_free(interconnect_doc);
				return rv;
			}
			/* Raise the power off hotswap event*/
			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_EXTRACTION_PENDING;
			/* ACTIVE to EXTRACTION_PENDING state change happened
			 * due power off event. The deactivation can not be
			 * stopped.
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange =
				SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
			oh_evt_queue_push(oh_handler->eventq,
					copy_ov_rest_event(&event));

			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState =
				SAHPI_HS_STATE_EXTRACTION_PENDING;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_INACTIVE;
			/* EXTRACTION_PENDING to INACTIVE state change happens
			 * due to auto policy of server blade
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange = SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push(oh_handler->eventq,
					copy_ov_rest_event(&event));
			break;

		case (On):
			event.resource.ResourceSeverity = SAHPI_OK;
			/* Update the current hot swap state to ACTIVE */
			hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
			/* Check for any thermal sensor state
			 * and raise the deassert events. But currently we 
			 * don't have any
			 */


			/* Raise the power on hotswap event*/
			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState = SAHPI_HS_STATE_INACTIVE;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			/* The cause of the state change is unknown */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange = SAHPI_HS_CAUSE_UNKNOWN;
			oh_evt_queue_push(oh_handler->eventq,
					copy_ov_rest_event(&event));

			event.rdrs = NULL;
			event.event.EventDataUnion.HotSwapEvent.
				PreviousHotSwapState =
				SAHPI_HS_STATE_INSERTION_PENDING;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			/* INSERTION_PENDING to ACTIVE state change happened
			 * to Auto policy of server blade
			 */
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange = SAHPI_HS_CAUSE_AUTO_POLICY;
			oh_evt_queue_push(oh_handler->eventq,
					copy_ov_rest_event(&event));
			break;

		default:
			err("Wrong power state %d for the interconnect in "
				"bay %d", info_result.powerState,
						info_result.bayNumber);
			wrap_g_free(enclosure_doc);
			wrap_g_free(interconnect_doc);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	wrap_g_free(enclosure_doc);
	wrap_g_free(interconnect_doc);

	return SA_OK;
}

/**
 * ov_rest_proc_interconnect_fault
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the Interconnect/Switch fault OEM event and
 *      OV_REST_SEN_OPER_STATUS sensor event.
 *      pushes the corrective action to syslog messages
 *      when the alert state is cleared, then the OEM event Desciption
 *      will have "Is Cleared" at the end.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *	SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_interconnect_fault(struct oh_handler_state *oh_handler,
                                        struct eventInfo* oh_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event oem_event = {0};
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse int_response = {0};
	struct interconnectInfo int_info_result = {0};
	struct enclosureInfoArrayResponse enc_response = {0};
	struct enclosureInfo enc_result = {{0}};
	SaHpiResourceIdT resource_id;
	struct enclosureStatus *enclosure = NULL;
	char *enc_doc = NULL, *interconnect_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || oh_handler->data == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			oh_event->resourceUri);
	rv = ov_rest_getinterconnectInfoArray(oh_handler, &int_response,
			ov_handler->connection, interconnect_doc);
	if (rv != SA_OK || int_response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray"
				" for interconnects");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_interconnect(
			int_response.interconnect_array, &int_info_result);
	ov_rest_wrap_json_object_put(int_response.root_jobj);
	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			int_info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enc_response,
			ov_handler->connection,
			enc_doc);
	if (rv != SA_OK || enc_response.enclosure_array== NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(
			enc_response.enclosure_array,
			&enc_result);
	ov_rest_wrap_json_object_put(enc_response.root_jobj);

	enclosure = (struct enclosureStatus *)ov_handler->
		ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
					enc_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the Interconnect"
				" serial number %s is unavailable",
				int_info_result.serialNumber);
		dbg("Skipping the event\n");
		wrap_g_free(enc_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	resource_id = enclosure->interconnect.resource_id
		[int_info_result.bayNumber - 1];
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);

	if (rpt == NULL) {
		err("Resource RPT is NULL for the "
				"Interconnect at bay number %d",
				int_info_result.bayNumber);
		dbg("Skipping the event\n");
		wrap_g_free(enc_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	memset(&oem_event, 0, sizeof(struct oh_event));
	memcpy(&(oem_event.resource), rpt, sizeof(SaHpiRptEntryT));
	oem_event.event.Source = oem_event.resource.ResourceId;
	oem_event.hid = oh_handler->hid;
	oem_event.event.EventType = SAHPI_ET_OEM;
	/* Coverting event.created time from string to nanoseconds */
	struct tm tm;
	strptime(oh_event->created, "%Y-%m-%dT%H:%M:%S.%NZ", &tm);
	time_t t = mktime(&tm);
	oem_event.event.Timestamp = (SaHpiTimeT)t * 1000000000;
	if (!strcmp(oh_event->severity, "Critical"))
		oem_event.event.Severity = SAHPI_CRITICAL;
	else if (!strcmp(oh_event->severity, "Warning"))
		oem_event.event.Severity = SAHPI_MAJOR;
	else
		err("Unknown Event Severity %s", oh_event->severity);
	oem_event.event.EventDataUnion.OemEvent.MId =
		oem_event.resource.ResourceInfo.ManufacturerId;
	oem_event.event.EventDataUnion.OemEvent.OemEventData.DataType =
		SAHPI_TL_TYPE_TEXT;
	oem_event.event.EventDataUnion.OemEvent.OemEventData.Language =
		SAHPI_LANG_ENGLISH;
	if (strlen(oh_event->description) > SAHPI_MAX_TEXT_BUFFER_LENGTH)
		oem_event.event.EventDataUnion.OemEvent.OemEventData.
			DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
	else

		if(!strcmp(oh_event->alertState, "Cleared"))
			oem_event.event.EventDataUnion.OemEvent.OemEventData.
				DataLength = strlen(oh_event->description)+
				sizeof(" Is Cleared.");
		else
			oem_event.event.EventDataUnion.OemEvent.OemEventData.
				DataLength =strlen(oh_event->description)+1;

	snprintf((char *) oem_event.event.EventDataUnion.OemEvent.
			OemEventData.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
			"%s", oh_event->description);
	if(!strcmp(oh_event->alertState, "Cleared")){
		if(oem_event.event.EventDataUnion.OemEvent.OemEventData.
				DataLength == SAHPI_MAX_TEXT_BUFFER_LENGTH ||
				(oem_event.event.EventDataUnion.OemEvent.
				 OemEventData.DataLength+sizeof(" Is Cleared."))
				> SAHPI_MAX_TEXT_BUFFER_LENGTH){

			strcpy((char*)&oem_event.event.EventDataUnion.OemEvent.
				OemEventData.Data[
				SAHPI_MAX_TEXT_BUFFER_LENGTH - 
				(int)sizeof(" Is Cleared.")], " Is Cleared");
		} else {
			strcat((char*)oem_event.event.EventDataUnion.OemEvent.
					OemEventData.Data, " Is Cleared.");
		}
	}
	oem_event_to_file(oh_handler, oh_event, &oem_event);
	oh_evt_queue_push (oh_handler->eventq,
			copy_ov_rest_event(&oem_event));

	OV_REST_PROCESS_SENSOR_EVENT(OV_REST_SEN_OPER_STATUS,
					HEALTH_ERROR, 0, 0);

	wrap_g_free(enc_doc);
	wrap_g_free(interconnect_doc);
	return SA_OK;
}

/**
 * ov_rest_proc_int_status
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the interconnect status event.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_proc_int_status(struct oh_handler_state *oh_handler,
                                  struct eventInfo* ov_event)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;
        struct interconnectInfoArrayResponse response = {0};
        struct interconnectInfo info_result = {0};
        struct enclosureInfoArrayResponse enclosure_response = {0};
        struct enclosureInfo enclosure_result = {{0}};
        struct enclosureStatus *enclosure = NULL;
        char* enclosure_doc = NULL, *int_doc = NULL;
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
        rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
                        ov_handler->connection, int_doc);
        if (rv != SA_OK || response.interconnect_array == NULL) {
                CRIT("Failed to get Interconnect Info Array");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
	/* Parse the Interconnect json response*/
        ov_rest_json_parse_interconnect(response.interconnect_array,
                                                        &info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);
        /* Now we have to get the enclosure serial number*/
        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                        ov_handler->connection->hostname,
                        info_result.locationUri);
        rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
                        ov_handler->connection, enclosure_doc);
        if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
                CRIT("Failed to get Enclosure Info Array");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
                        &enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);
        /* Find the interconnect Resourceid by looking at the enclosure
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
                CRIT("Enclosure data of interconnect in bay %d is unavailable",
                                info_result.bayNumber);
                wrap_g_free(enclosure_doc);
                wrap_g_free(int_doc);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }


        /* Get the rpt entry of the resource */
        rpt = oh_get_resource_by_id(oh_handler->rptcache,
                        enclosure->
                        interconnect.resource_id[info_result.bayNumber - 1]);
        if (rpt == NULL) {
                err("RPT is NULL for Interconnect in bay %d",
                                        info_result.bayNumber);
                wrap_g_free(enclosure_doc);
                wrap_g_free(int_doc);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = ov_rest_gen_res_event(oh_handler, rpt,
                                        info_result.interconnectStatus);
        if (rv != SA_OK) {
                err("Failed to generate resource event for interconnect"
                        " in bay %d", info_result.bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * process_interconnect_reset_task:
 *      @oh_handler: Pointer to openhpi handler structure
 *      @ov_event:   Pointer to the eventInfo structure
 *
 * Purpose:
 *      Creates the Interconnect reset hpi hotswap events.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT process_interconnect_reset_task(
                                        struct oh_handler_state *oh_handler,
                                        struct eventInfo* ov_event)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfo info_result = {0};
	struct interconnectInfoArrayResponse response = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char* enclosure_doc = NULL, *interconnect_doc = NULL;
	SaHpiRptEntryT *rpt = NULL;

	if(oh_handler == NULL || ov_event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			ov_event->resourceUri);
	rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
	if (rv != SA_OK || response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray"
			" for interconnects");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_interconnect(response.interconnect_array,
					&info_result);
	ov_rest_wrap_json_object_put(response.root_jobj);

	WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			info_result.locationUri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enclosure_response,
			ov_handler->connection, enclosure_doc);
	if (rv != SA_OK || enclosure_response.enclosure_array == NULL) {
		CRIT("Failed to get Enclosure Info Array");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_enclosure(enclosure_response.enclosure_array,
			&enclosure_result);
	ov_rest_wrap_json_object_put(enclosure_response.root_jobj);

	/* Find the interconnect Resourceid by looking at the enclosure
	 * linked list.
	 */
	enclosure = (struct enclosureStatus *)
				ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(!strcmp(enclosure->serialNumber,
					enclosure_result.serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure == NULL){
		CRIT("Enclosure data of the interconnect in bay %d"
			" is unavailable", info_result.bayNumber);
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	hotswap_state = (struct ovRestHotswapState *)
		oh_get_resource_data(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (hotswap_state == NULL)
	{
		err ("Failed to get hotswap state of Interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache,
		enclosure->interconnect.resource_id[info_result.bayNumber - 1]);
	if (rpt == NULL) {
		err("RPT is NULL for the interconnect in bay %d, "
				"in enclosure rid %d ",info_result.bayNumber,
						enclosure->enclosure_rid );
		wrap_g_free(enclosure_doc);
		wrap_g_free(interconnect_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	memset(&event, 0, sizeof(struct oh_event));
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.hid = oh_handler->hid;
	event.event.EventType = SAHPI_ET_HOTSWAP;
	oh_gettimeofday(&(event.event.Timestamp));
	event.resource.ResourceSeverity = SAHPI_OK;
	event.event.Severity = event.resource.ResourceSeverity;
	/* Update the current hot swap state to ACTIVE */
	hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;

	/* On reset of interconnect, it has powered off and powered on
	 * Raise 2 hoswap events for power off
	 * ACTIVE -> EXTRACTION_PENDING and EXTRACTION_PENDING -> INACTIVE
	 * Then, raise 2 hoswap events for power on
	 * INACTIVE -> INSERTION_PENDING and INSERTION_PENDING -> ACTIVE
	 */
	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	/* ACTIVE to EXTRACTION_PENDING state change happened due power off
	 *  event. The deactivation can not be stopped.
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_EXTRACTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INACTIVE;
	/* EXTRACTION_PENDING to INACTIVE state change happened due
	 * to Auto policy of the server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* The cause of the state change is unknown */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_UNKNOWN;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened due
	 * to auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	wrap_g_free(enclosure_doc);
	wrap_g_free(interconnect_doc);

	return SA_OK;
}
