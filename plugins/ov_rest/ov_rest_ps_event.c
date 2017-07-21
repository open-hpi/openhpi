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
 * This file has the implementation of the power supply unit event handling
 *
 *	process_powersupply_insertion_event()	- Process the power supply unit
 *						  insertion event
 *	process_powersupply_removed_event()	- Process the power supply unit
 *						  removal event
 *
 **/

#include "ov_rest_event.h"
#include "ov_rest_discover.h"
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * process_powersupply_insertion_event
 *      @handler: Pointer to openhpi handler structure
 *      @event:   Pointer to OV REST event response structure
 *
 * Purpose:
 *      Adds the newly inserted power supply information into RPT and
 *      RDR table
 *      Creates the hot swap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
 **/
SaErrorT process_powersupply_insertion_event( struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	struct powersupplyInfo response = {0};
	struct oh_event hs_event = {0};
	int bayNumber;
	SaHpiResourceIdT resource_id;
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	char* enclosure_doc = NULL;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue_ps = NULL, *jvalue_ps_array = NULL;
	SaHpiRptEntryT *enc_rpt = NULL;
	GSList *asserted_sensors = NULL;
	int enc_loc;

	if (handler == NULL || event == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Inserted PS Unit");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	bayNumber = ov_rest_get_baynumber(event->resourceID);
	/* FIXME: Shall we move below code in to a function as add_ps_unit to 
	 * make it reusable in case
	 */
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
	jvalue_ps_array = ov_rest_wrap_json_object_object_get(
			enclosure_response.enclosure_array,
			"powerSupplyBays");
	/* Checking for json object type, if it is not array, return */
	if (jvalue_ps_array == NULL || (json_object_get_type(jvalue_ps_array)
						!= json_type_array)) {
		CRIT("No Powersupply array for bay %d, Dropping event",
			bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	jvalue_ps = json_object_array_get_idx(jvalue_ps_array, bayNumber-1);
	if(!jvalue_ps){
		CRIT("Invalid response for the powersupply in bay %d",
								bayNumber);
		return SA_ERR_HPI_INVALID_DATA;
	}
	ov_rest_json_parse_powersupply(jvalue_ps, &response);
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
		CRIT("Enclosure data of the powersupply"
				" serial number %s is unavailable",
				response.serialNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	enc_rpt = oh_get_resource_by_id(handler->rptcache,
			enclosure->enclosure_rid);
	enc_loc = enc_rpt->ResourceEntity.Entry[0].EntityLocation;
	rv = ov_rest_build_powersupply_rpt(handler, &response, 
			&resource_id, enc_loc);
	if (rv != SA_OK) {
		err("Build rpt failed for powersupply in bay %d", bayNumber);
		wrap_free(enclosure_doc);
		return rv;
	}
	ov_rest_update_resource_status (&enclosure->ps_unit,
			response.bayNumber,
			response.serialNumber, 
			resource_id,
			RES_PRESENT,
			response.type);
	rv = ov_rest_build_powersupply_rdr(handler,
			resource_id, &response);
	if (rv != SA_OK) {
		err("Build rdr failed for powersupply in bay %d", bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for powersupply in "
				"bay %d", bayNumber);
		}
		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->ps_unit,
				response.bayNumber,
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, 
				RES_ABSENT, UNSPECIFIED_RESOURCE);

		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rv = ov_rest_populate_event(handler, resource_id, &hs_event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for powersupply in bay %d",
					bayNumber);
		wrap_free(enclosure_doc);
		return rv;
	}

	hs_event.event.EventType = SAHPI_ET_HOTSWAP;
	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* NOT_PRESENT to ACTIVE state change happened due to operator action
	 */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	/* Push the hotswap event to add the resource to OpenHPI RPTable */
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));

	/* Raise the assert sensor events */

	wrap_free(enclosure_doc);
	return SA_OK;
}

/**
 * process_powersupply_removed_event
 *      @handler: Pointer to openhpi handler structure
 *      @event:   Pointer to OV REST event response structure
 *
 * Purpose:
 *      Removes power supply information from Resource Tabel (RPT)  and
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
SaErrorT process_powersupply_removed_event( struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	struct oh_event hs_event;
	int bayNumber;
	SaHpiResourceIdT resource_id;
	SaHpiRptEntryT *rpt = NULL;
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
		dbg("Bay Number is Unknown for removed PS Unit");
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
	 * This is useful to find the enclosure related to this ps unit
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
		CRIT("Enclosure data of the powersupply in bay %d"
			" is unavailable", bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}
	if(enclosure->ps_unit.presence[bayNumber - 1] == RES_ABSENT){
		err("Extracted power supply unit may be in faulty condition"
				" in bay %d", bayNumber);
		err("The power supply unit in bay %d is absent", bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* FIXME: Shall we move below code in to a function as remove_ps_unit to
	 * make it reusable in case
	 */
	resource_id = enclosure->ps_unit.resource_id[bayNumber - 1];
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for powersupply in bay %d", bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_update_hs_event(handler, &hs_event);
	memcpy(&(hs_event.resource), rpt, sizeof(SaHpiRptEntryT));
	hs_event.event.Source = hs_event.resource.ResourceId;
	hs_event.event.Severity = hs_event.resource.ResourceSeverity;

	hs_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	hs_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	/* This state change happened due to surprise extraction */
	hs_event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
	/* Push the hotswap event to remove the resource from OpenHPI RPTable
	 */
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&hs_event));

	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(handler, resource_id);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for powersupply in bay %d",
				bayNumber);
	}
	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(handler->rptcache,
			hs_event.resource.ResourceId);
	ov_rest_update_resource_status (&enclosure->ps_unit,
			bayNumber,
			"", SAHPI_UNSPECIFIED_RESOURCE_ID,
			RES_ABSENT,
			UNSPECIFIED_RESOURCE);

	wrap_free(enclosure_doc);
	return SA_OK;
}

