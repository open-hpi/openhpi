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
 * This file has the implementation of the thermal subsystem, fan zone and fan
 * event handling
 *
 *	process_fan_inserted_event()	- Processes the fan insertion event
 *
 *	process_fan_removed_event()	- Processes the fan extraction event
 *
 *	ov_rest_add_fan()		- Adds the fan resource entry to the
 *					  RPT
 *	ov_rest_remove_fan()		- Removes the fan resource entry from
 *					  the RPT
 *
 **/

#include "ov_rest_event.h"
#include "ov_rest_fan_event.h"
#include "ov_rest_discover.h"
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/**
  * ov_rest_add_fan
  *      @handler: Pointer to openhpi handler structure
  *      @response:   pointer fanInfo structure
  *	 @enclosure: Ponter to enclosureStatus structure
  *
  *
  * Purpose:
  *      Adds the newly inserted FAN information into RPT and
  *      RDR table
  *
  * Detailed Description: NA
  *
  * Return values:
  *      SA_OK                     - success.
  *      SA_ERR_HPI_INTERNAL_ERROR - on failure
  */
SaErrorT ov_rest_add_fan(struct oh_handler_state *handler, 
			struct fanInfo *response,
			struct enclosureStatus *enclosure)
{
	int enc_loc;
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id;
	SaHpiRptEntryT *enc_rpt = NULL;
	struct oh_event event = {0};
	GSList *asserted_sensors = NULL;

        enc_rpt = oh_get_resource_by_id(handler->rptcache,
					enclosure->enclosure_rid);
        enc_loc = enc_rpt->ResourceEntity.Entry[0].EntityLocation;
        rv = ov_rest_build_fan_rpt(handler, response,
                                                &resource_id, enc_loc);
        if (rv != SA_OK) {
                err("Build Fan rpt failed for the fan bay %d in "
				"enclosure resource id %d ", 
				response->bayNumber, enclosure->enclosure_rid);
                return rv;
        }
        ov_rest_update_resource_status (&enclosure->fan,
                                        response->bayNumber,
                                        response->serialNumber,
					resource_id,
                                        RES_PRESENT,
					response->type);
        rv = ov_rest_build_fan_rdr(handler,
                                        resource_id, response);
        if (rv != SA_OK) {
                err("build Fan rdr failed  the fan bay %d in " 
				"enclosure resource id %d ",
				response->bayNumber, enclosure->enclosure_rid);
                /* Free the inventory info from inventory RDR */
                rv = ov_rest_free_inventory_info(handler, resource_id);
                if (rv != SA_OK) {
                        err("Inventory cleanup failed for fan in bay %d "
				"with resource id %d", response->bayNumber,
							resource_id);
                }
                oh_remove_resource(handler->rptcache, resource_id);
                /* reset resource_info structure to default values */
                ov_rest_update_resource_status(
                              &enclosure->fan,
                              response->bayNumber,
                              "", SAHPI_UNSPECIFIED_RESOURCE_ID, 
                              RES_ABSENT, UNSPECIFIED_RESOURCE);

                return SA_ERR_HPI_INTERNAL_ERROR;
        }
	rv = ov_rest_populate_event(handler, resource_id, &event, 
				&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for the fan bay %d in "
			"enclosure resource id %d", response->bayNumber,
						enclosure->enclosure_rid);
		return rv;
	}
        event.event.EventType = SAHPI_ET_HOTSWAP;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        /* NOT_PRESENT to ACTIVE state change happened due to operator action*/
        event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                SAHPI_HS_CAUSE_OPERATOR_INIT;
        /* Push the hotswap event to add the resource to OpenHPI RPTable */
        oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));


	return SA_OK;
}

/**
  * process_fan_inserted_event
  *      @handler: Pointer to openhpi handler structure
  *	 @event: Ponter eventInfo Strucure
  *
  * Purpose:
  *      Handler fot the newly inserted FAN event
  *      
  *
  * Detailed Description: NA
  *
  * Return values:
  *      SA_OK                     - success.
  *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
  *      SA_ERR_HPI_INTERNAL_ERROR - on failure
  *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
  */
SaErrorT process_fan_inserted_event( struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	int bayNumber;
	struct ov_rest_handler *ov_handler = NULL;
	struct fanInfo response = {0};
	struct enclosureStatus *enclosure = NULL;
	struct enclosureInfo enclosure_result = {{0}};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	char* enclosure_doc = NULL;
	json_object *jvalue_fan_array = NULL, *jvalue_fan = NULL;
	
	if (handler == NULL || event == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Inserted Fan");
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
        jvalue_fan_array = ov_rest_wrap_json_object_object_get(
				enclosure_response.enclosure_array, "fanBays");
        /* Checking for json object type, if it is not array, return */
        if (jvalue_fan_array == NULL || (json_object_get_type(jvalue_fan_array)
                                                         != json_type_array)) {
                CRIT("No Fan array in enclosure for fan bay %d. "
			"Not adding Fan", bayNumber);
                return SA_ERR_HPI_INVALID_DATA;
        }

        jvalue_fan = json_object_array_get_idx(jvalue_fan_array, bayNumber-1);
        if (!jvalue_fan) {
                CRIT("Invalid response for the fan in bay %d", bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        ov_rest_json_parse_fan(jvalue_fan, &response);
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
                CRIT("Enclosure data of the Fan"
                                " serial number %s is unavailable",
                                response.serialNumber);
        }
	

	rv = ov_rest_add_fan(handler, &response, enclosure);
	if (rv != SA_OK) {
		err("Adding fan %d failed",
				response.bayNumber);
		wrap_free(enclosure_doc);
		return rv;
	}
	wrap_free(enclosure_doc);
	return SA_OK;
}

/**
 * ov_rest_remove_fan
 *      @handler: Pointer to openhpi handler structure
 *      @bayNumber: BayNumber/slot number of the fan
 *      @enclosure: Ponter to enclosureStatus structure
 *
 *
 * Purpose:
 *      Removes the FAN information into RPT and
 *      RDR table
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_remove_fan(struct oh_handler_state *handler,
		SaHpiInt32T bayNumber,
		struct enclosureStatus *enclosure)
{
	SaHpiResourceIdT resource_id;
	SaErrorT rv = SA_OK;
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event hs_event = {0};
	
        resource_id = enclosure->fan.resource_id[bayNumber - 1];
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for the fan in bay %d with resocuce ID %d",
					bayNumber, resource_id);
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
                err("Inventory cleanup failed for fan in bay %d with "
			"resource id %d", bayNumber, rpt->ResourceId);
        }
        /* Remove the resource from plugin RPTable */
        rv = oh_remove_resource(handler->rptcache,
                                hs_event.resource.ResourceId);
        ov_rest_update_resource_status (&enclosure->fan,
                                        bayNumber,
                                        "", SAHPI_UNSPECIFIED_RESOURCE_ID,
                                        RES_ABSENT,
                                        UNSPECIFIED_RESOURCE);
        return SA_OK;
}

/**
 * process_fan_removed_event
 *      @handler: Pointer to openhpi handler structure
 *      @event: Ponter to eventInfo structure
 *
 *
 * Purpose:
 *      Handler for the  FAN Remoced event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
 **/
SaErrorT process_fan_removed_event( struct oh_handler_state *handler,
                                        struct eventInfo* event)
{
	SaErrorT rv = SA_OK;
	SaHpiInt32T bayNumber;
	struct ov_rest_handler *ov_handler = NULL;
	struct fanInfo response = {0};
        struct enclosureStatus *enclosure = NULL;
        struct enclosureInfo enclosure_result = {{0}};
        struct enclosureInfoArrayResponse enclosure_response = {0};
        char* enclosure_doc = NULL;

	if (handler == NULL || event == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) handler->data;
	if(!event->resourceID){
		dbg("Bay Number is Unknown for Removed Fan");
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
                CRIT("Enclosure data of the Fan"
                                " serial number %s is unavailable",
                                response.serialNumber);
        }
	
	if(enclosure->fan.presence[bayNumber - 1] == RES_ABSENT){
		err("Extracted Fan in bay %d  may be in faulty condition", 
			bayNumber);
		wrap_free(enclosure_doc);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	rv = ov_rest_remove_fan(handler, bayNumber, enclosure);
	if (rv != SA_OK) {
		err("Remove fan %d failed",
				bayNumber);
		wrap_free(enclosure_doc);
		return rv;
	}

	wrap_free(enclosure_doc);
	return SA_OK;
}

