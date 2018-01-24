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
 *
 * This file implements the re-discovery functionality. The resources of the
 * HPE Synergy are re-discovered, whenever the connection to the
 * active Composer is broken or on Composer switchover.  
 * Re-discovery is done to sync the plugin with the current state of the 
 * resources.
 *
 *      ov_rest_re_discover_resources() - Starts the re-discovery of server
 *                                        blades, interconnect blades, Composer,
 *                                        fan and power supplies.
 *
 *      re_discover_server()            - Re-discovers the server
 *                                        blades
 *
 *      re_discover_power_supply()      - Re-discovers the power supply units
 *
 *      re_discover_appliance()         - Re-discovers the aooliance/composer
 *
 *      remove_composer()              - Remove's composer from RPT.
 *
 *      add_composer()                 - Add newly re-discovered 
 *      				 composer
 *
 *      remove_server()                 - Remove server blade from RPT
 *
 *      add_inserted_server_blade()     - Add new server blade to RPT
 *
 *      re_discover_blade()             - Re-discover server blades
 *
 *      update_server_hotswap_state()   - update server hotswap state in RPT
 *
 *      re_discover_interconnect()      - Re-discovers the interconnect blades
 *
 *      update_interconnect_hotswap_state() - update hotswap state in the RPT
 *
 *      remove_interconnect()           - Removes the interconnect
 *
 *      add_inserted_interconnect()     - Adds the interconnect.
 *
 *      re_discover_fan()               - Re-discovers the fan
 *
 *      remove_fan()                    - Remove the fan from RPT
 *
 *      add_inserted_fan()              - Add fan to the RPT
 *
 *      re_discover_ps_unit()           - Re-discover the PSU
 *
 *      remove_piowersupply()           - Removes the PSU from RPT
 *
 *      add_inserted_powersupply()      - Add the PSU to RPT
 *
 *
 */

#include "ov_rest_re_discover.h"
#include "ov_rest_parser_calls.h"
#include "sahpi_wrappers.h"


/**
 * free_data
 * 	@data: gpointer to key or value in the hash table
 * Purpose:
 *		This is a call back function for the g_hash_table_destroy,
 *	initialized while creating hash table using g_hash_table_new_full.
 *	frees the memory allocated to key's and value's of the hash table.
 * Detailed Description: NA
 * 
 * Return values:
 *	Void 
 *
 * */
void free_data(gpointer data)
{
	wrap_g_free(data);
}

/**
 * ov_rest_re_discover_resources
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discover the resources.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_re_discover_resources(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;

	if (oh_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	err("Going for Re-Discovery. If you find any kind of issues during "
		"re-discovery, please re-start openhpid.");
	err("Re-discovery started");

	ov_handler = (struct ov_rest_handler *) oh_handler->data;

	/* Re-discovery is called by locking the OV handler mutex.
	 * Hence on getting request to shutdown, pass the locked mutexes
	 * for unlocking.
	 */
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_appliance(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of appliance failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_enclosure(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of enclosures failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_composer(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of composers failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_server(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of Server Blade failed");
		return rv;
	}

	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_drive_enclosure(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of Drive Enclosure failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_interconnect(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of Interconnect failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_sas_interconnect(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of SAS-Interconnect failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_powersupply(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of Power Supply failed");
		return rv;
	}
	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, ov_handler->mutex, NULL, NULL);
	rv = re_discover_fan(oh_handler);
	if (rv != SA_OK) {
		err("Re-discovery of Fan failed");
		return rv;
	}
	err("Re-discovery completed");
	err("Re-Discovery Completed. If you find any kind of issues after "
		"re-discovery, please re-start openhpid.");

	return SA_OK;
}
/*
 * re_discover_appliance
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the appliance.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_appliance(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct applianceNodeInfoResponse response = {0};
	struct applianceHaNodeInfoResponse ha_response = {0};
	struct applianceNodeInfo result = {{{0}}};
	struct applianceHaNodeInfo ha_node_result = {{0}};
	struct composer_status *composer = NULL;
	SaHpiRptEntryT *rpt = NULL;

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	composer = &ov_handler->ov_rest_resources.composer;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_APPLIANCE_VERSION_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getapplianceNodeInfo(oh_handler, &response,
			ov_handler->connection);
	if(rv != SA_OK || response.applianceVersion == NULL) {
		CRIT("Failed to get the response from ov_rest_getappliance");
		return rv;
	}
	ov_rest_json_parse_appliance_version(response.applianceVersion,
			&result.version);

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_APPLIANCE_HA_NODE_ID_URI,
			ov_handler->connection->hostname, 
			result.version.serialNumber);
	rv = ov_rest_getapplianceHaNodeInfo(&ha_response,
			ov_handler->connection);
	if(rv != SA_OK) {
		CRIT("Failed to get the response for Active HA Node");
		ov_rest_wrap_json_object_put(response.root_jobj);
		return rv;
	}
	ov_rest_json_parse_appliance_Ha_node(ha_response.haNode,
			&ha_node_result);

	ov_rest_wrap_json_object_put(response.root_jobj);
	ov_rest_wrap_json_object_put(ha_response.root_jobj);
	if(strstr(composer->serialNumber, result.version.serialNumber)){
		return SA_OK;
	}else{
		rpt = oh_get_resource_by_id(oh_handler->rptcache, composer->resource_id);
		if (rpt == NULL) {
			err("RPT is NULL for composer resource id %d", composer->resource_id);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(oh_handler, rpt->ResourceId);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for composer resource id %d",
					rpt->ResourceId);
		}
		rv = ov_rest_build_appliance_rdr(oh_handler,
                        &result, &ha_node_result, composer->resource_id);
		if (rv != SA_OK) {
			err("Build rdr failed for appliance resource id %d,"
			" Please Restart the Openhpid",	composer->resource_id);
			return rv;
		}
		strcpy(ov_handler->ov_rest_resources.composer.serialNumber,
                        result.version.serialNumber);

	}
	return SA_OK;
}

/*
 * re_discover_enclosure 
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers enclosures.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_enclosure(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo result = {{0}};
	int i = 0,arraylen = 0;
	struct enclosureStatus *enclosure = NULL, *temp = NULL;
	json_object * jvalue = NULL;
	char *match = NULL;
	GHashTable *enc_serial_hash = g_hash_table_new_full(g_str_hash, 
			g_str_equal,
			free_data,
			free_data);

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &response,
			ov_handler->connection, NULL);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_OK;
	}
	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array){
		CRIT("Enclosures may not be added as no array received");
		return SA_OK;
	}
	arraylen = json_object_array_length(response.enclosure_array);
	/* Below loop is to check if there any new enclosures
	 * and if found build the RPT.
	 *  */
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue){
			CRIT("Invalid response for the enclosure in bay %d",
					i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue,&result);
		g_hash_table_insert(enc_serial_hash, 
				g_strdup(result.serialNumber), 	
				g_strdup("TRUE"));
		enclosure = ov_handler->ov_rest_resources.enclosure;
		temp = enclosure;
		while(temp){
			if(strstr(result.serialNumber, temp->serialNumber)){
				break;
			}
			temp = temp->next;
		}
		if(temp){
			continue;
		}
		/* Add Enclsure here by calling add enclosure
		 * and continue
		 * */
		dbg("Adding the newly found enclosure with Serial number %s", 
						result.serialNumber);
		rv = add_enclosure(oh_handler, &result);
		if(rv != SA_OK){
			err("Unable to add enclosure with "
				"serial number: %s", 
				result.serialNumber);
			return rv;
		}
	}
	temp = enclosure;
	while(temp){
		match = g_hash_table_lookup(enc_serial_hash, 
				temp->serialNumber);
		if(match == NULL){
			rv =remove_enclosure(oh_handler, temp);
			if(rv != SA_OK){
				err("Unable to remove enclosure with "
					"serial number: %s", 
					temp->serialNumber);
				return rv;
			}
		}
		temp = temp->next;
	}
	g_hash_table_destroy(enc_serial_hash);
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}
/*
 * remove_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *	@enclosure: Pointer to struct enclosureStatus
 *
 * Purpose:
 *      Removes the enclosure from RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on failure.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT remove_enclosure(struct oh_handler_state *handler,
		struct enclosureStatus *enclosure)
{
	SaErrorT rv = SA_OK;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiResourceIdT resource_id = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatus *head = NULL;
	int bay;

	if(handler == NULL || enclosure == NULL){
		CRIT("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler*)handler->data;
	head = ov_handler->ov_rest_resources.enclosure;

	resource_id = enclosure->enclosure_rid;
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for enclosure id %d", resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(handler, rpt->ResourceId);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for enclosure id %d",
				rpt->ResourceId);
	}
	/* Add Code to remove all the resourece in this enclosure */
	if(head != enclosure){
		while(head){
			if(head->next == enclosure){
				head->next = enclosure->next;
				break;
			}
			head = head->next;
		}
	}
	for(bay = 1; bay <= enclosure->server.max_bays; bay++){
		if(enclosure->server.presence[bay-1] == RES_PRESENT){
			rv = remove_server_blade(handler, bay, enclosure);
			if(rv != SA_OK){
				err("Unable to remove the server blade "
						"in enclosure serial: %s and "
						"device bay: %d", 
						enclosure->serialNumber, 
						bay);
			}
		}
	}
	for(bay = 1; bay <= enclosure->interconnect.max_bays; bay++){
		if(enclosure->interconnect.presence[bay-1] == RES_PRESENT){
			rv = remove_interconnect_blade(handler, bay, enclosure);
			if(rv != SA_OK){
				err("Unable to remove the interconnect"
						"in enclosure serial: %s and "
						"device bay: %d", 
						enclosure->serialNumber, 
						bay);
			}
		}
	}
	for(bay = 1; bay <= enclosure->ps_unit.max_bays; bay++){
		if(enclosure->ps_unit.presence[bay-1] == RES_PRESENT){
			rv =remove_powersupply(handler, enclosure,bay);
			if(rv != SA_OK){
				err("Unable to remove the Powersupply Unit "
						"in enclosure serial: %s and "
						"device bay: %d", 
						enclosure->serialNumber, 
						bay);
			}
		}
	}
	for(bay = 1; bay <= enclosure->fan.max_bays; bay++){
		if(enclosure->fan.presence[bay-1] == RES_PRESENT){
			rv = ov_rest_remove_fan(handler, bay, enclosure);
			if(rv != SA_OK){
				err("Unable to remove the fan "
						"in enclosure serial: %s and "
						"fan bay: %d", 
						enclosure->serialNumber, 
						bay);
			}
		}
	}
	release_ov_rest_resources(enclosure);
	wrap_g_free(enclosure);	

	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(handler->rptcache,
			resource_id);

	return SA_OK;

}
/**
 * add_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *      @result: Pointer to struct enclosureInfo
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered enclosure to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_enclosure(struct oh_handler_state *handler,
		struct enclosureInfo *result)
{
	SaErrorT rv = SA_OK;
	struct enclosureStatus *temp = NULL;
	SaHpiResourceIdT resource_id = 0;
	struct ov_rest_handler *ov_handler = NULL;
	if(handler == NULL || result == NULL){
		CRIT("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)handler->data;
	rv = ov_rest_build_enc_info(handler, result);
	if (rv != SA_OK) {
		err("Build enclosure info failed for resource"
			" serial number %s", result->serialNumber);
		return rv;
	}
	rv = ov_rest_build_enclosure_rpt(handler, result,
			&resource_id);
	if (rv != SA_OK) {
		err("Build enclosure rpt failed for resource"
                        " serial number %s", result->serialNumber);
		return rv;
	}
	temp = ov_handler->ov_rest_resources.enclosure;
	if(temp != NULL){
		while(temp->next != NULL){
			temp = temp->next;
		}
	}else{
		return SA_ERR_HPI_ERROR;
	}
	/* Save enclosure resource id */
	temp->enclosure_rid = resource_id;
	strcpy(temp->serialNumber, result->serialNumber);

	rv = ov_rest_build_enclosure_rdr(handler,
			result, resource_id);
	if (rv != SA_OK) {
		err("Build enclosure rdr failed for resource id %d",
							resource_id);
		return rv;
	}

	return SA_OK;
}

/*
 * add_composer
 *      @oh_handler: Pointer to openhpi handler
 *      @composer_info: Pointer to struct applianceInfo
 *      @ha_node_result: Pointer to struct applianceHaNodeInfo
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered composer to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_composer(struct oh_handler_state *handler,
		struct applianceInfo *composer_info,
		struct applianceHaNodeInfo *ha_node_result)
{

        SaErrorT rv = SA_OK;
        SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	SaHpiRptEntryT *rpt = NULL;
        struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatus *enclosure = NULL;

	
        rv = ov_rest_build_composer_rpt(handler, ha_node_result,
                        &resource_id, ha_node_result->role);
        if (rv != SA_OK) {
                err("build composer rpt failed");
                return rv;
        }
	ov_handler =  (struct ov_rest_handler *) handler->data;
	enclosure = (struct enclosureStatus * )ov_handler->
					ov_rest_resources.enclosure;
	while(enclosure != NULL){
		if(strstr(ha_node_result->enclosure_uri,
					enclosure->serialNumber)){
			ov_rest_update_resource_status(
					&enclosure->composer,
					composer_info->bayNumber,
					composer_info->serialNumber,
					resource_id, RES_PRESENT,
					ha_node_result->type);
			break;
		}
		enclosure = enclosure->next;
	}
 
        rv = ov_rest_build_composer_rdr(handler,
                        composer_info, ha_node_result, resource_id);
        if (rv != SA_OK) {
                err("build appliance rdr failed");
                rv = ov_rest_free_inventory_info(handler, resource_id);
                if (rv != SA_OK) {
                        err("Inventory cleanup failed for the composer in bay "
                                " %d with resource id %d",
                                composer_info->bayNumber, resource_id);
                }

		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
                                        &enclosure->composer,
                                        composer_info->bayNumber,
                                        "",
                                        SAHPI_UNSPECIFIED_RESOURCE_ID,
					RES_ABSENT,
                                        UNSPECIFIED_RESOURCE);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        rpt = oh_get_resource_by_id (handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for server is %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        /* For composer that don't support  managed hotswap, send simple
         * hotswap event  */
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                event.event.EventType = SAHPI_ET_HOTSWAP;
                event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                        SAHPI_HS_STATE_NOT_PRESENT;
                event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                        SAHPI_HS_STATE_ACTIVE;
                event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                        SAHPI_HS_CAUSE_OPERATOR_INIT;
                oh_evt_queue_push(handler->eventq,
                                copy_ov_rest_event(&event));

                return(SA_OK);
        }
        /* Raise the hotswap event for the inserted Composer */
        event.event.EventType = SAHPI_ET_HOTSWAP;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_INSERTION_PENDING;
        /* NOT_PRESENT to INSERTION_PENDING state change happened due
         * to operator action
         */
        event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                SAHPI_HS_CAUSE_OPERATOR_INIT;
        oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

        event.rdrs = NULL;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_INSERTION_PENDING;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        /* INSERTION_PENDING to ACTIVE state change happened
         * due to auto policy of Composer 
         */
        event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
                SAHPI_HS_CAUSE_AUTO_POLICY;
        oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

        return SA_OK;

}

/*
 * remove_composer
 *      @oh_handler: Pointer to openhpi handler
 *	@enclosure: Pointer to enclosureStatus structure
 *	@bayNumber: Bay number of the composer
 *
 * Purpose:
 *      Removes the composer from RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on failure.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT remove_composer(struct oh_handler_state *handler,
		struct enclosureStatus *enclosure,
		byte bayNumber)
{

        SaErrorT rv = NULL;
        SaHpiResourceIdT resource_id = 0;
        struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
        SaHpiRptEntryT *rpt = NULL;
        if(handler == NULL){
                CRIT("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	resource_id = enclosure->composer.resource_id[bayNumber-1];
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("resource RPT is NULL, Dropping the event."
		" Enclosure serialnumber %s, baynumber %d",
						enclosure->serialNumber,
						 bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;
        event.hid = handler->hid;
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
                        oh_get_resource_data(handler->rptcache,
                                        event.resource.ResourceId);
                if (hotswap_state == NULL) {
                        err("Failed to get hotswap state of composer "
                                "in bay %d", bayNumber);
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
        oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));


        /* Free the inventory info from inventory RDR */
        rv = ov_rest_free_inventory_info(handler, rpt->ResourceId);
        if (rv != SA_OK) {
                err("Inventory cleanup failed for resource id %d",
                                rpt->ResourceId);
        }
        /* Remove the resource from plugin RPTable */
        rv = oh_remove_resource(handler->rptcache,
                        rpt->ResourceId);
	if(rv != SA_OK){
		CRIT("Failed the remove the Composer Resource with rid %d",
				rpt->ResourceId);
	}

        /* reset resource_info structure to default values */
        ov_rest_update_resource_status(
                        &enclosure->composer, bayNumber,
                        "", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
                        UNSPECIFIED_RESOURCE);
        return SA_OK;
}

/*
 * re_discover_composer
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the composer.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_composer(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo result = {{0}};
	struct applianceHaNodeInfoArrayResponse ha_node_response = {0};
	struct applianceHaNodeInfo ha_node_result = {{0}};	
	struct applianceInfo composer_info = {{0}};
	int i = 0, j = 0, arraylen = 0, comp_arraylen = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object * jvalue = NULL, *jvalue_comp_array = NULL;
	json_object *jvalue_composer = NULL;

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &response,
			ov_handler->connection, NULL);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return SA_OK;
	}
	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array){
		CRIT("Composers may not be added as no array received");
		return SA_OK;
	}
	arraylen = json_object_array_length(response.enclosure_array);
	/* Below loop is to check if there any new enclosures
	 * and if found build the RPT.
	 */
	for (i=0; i< arraylen; i++){
		memset(&result, 0, sizeof(result));
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue){
			CRIT("Invalid response for the enclosure in bay %d",
					i + 1);
			continue;
		}

		ov_rest_json_parse_enclosure(jvalue,&result);
		jvalue_comp_array = ov_rest_wrap_json_object_object_get(jvalue,
				"applianceBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_comp_array) != json_type_array) {
			CRIT("Not adding applianceBay supplied to enclosure %d,"
					" no array returned for that",i);
			continue;
		}
		comp_arraylen = json_object_array_length(jvalue_comp_array);
		for(j = 0; j < comp_arraylen; j++){
			/* Set the composer_info and ha_node_result c
 			* ontents to 0, at the begining of the loop*/
			
			memset(&composer_info, 0, sizeof(composer_info));
			memset(&ha_node_result, 0, sizeof(ha_node_result));
			jvalue_composer =
				json_object_array_get_idx(jvalue_comp_array, j);
			if (!jvalue_composer) {
				CRIT("Invalid response for the composer"
						" in bay %d", j + 1);
				continue;
			}
			ov_rest_json_parse_applianceInfo(jvalue_composer,
					&composer_info);
			if((!composer_info.serialNumber[0] == '\0')){
				WRAP_ASPRINTF(&ov_handler->connection->url, 
						OV_APPLIANCE_HA_NODE_ID_URI,
						ov_handler->connection->hostname,
						composer_info.serialNumber);
				rv = ov_rest_getapplianceHANodeArray(oh_handler, 
						&ha_node_response,
						ov_handler->connection, NULL);
				if(rv != SA_OK || ha_node_response.haNodeArray 
						== NULL) {
					CRIT("No response from "
							"ov_rest_getapplianceHANodeArray");
					return rv;
				}

				ov_rest_json_parse_appliance_Ha_node(
						ha_node_response.haNodeArray,
						&ha_node_result);

				ov_rest_wrap_json_object_put(
						ha_node_response.root_jobj);
			} else if(composer_info.presence == Present){
				CRIT("Composer serial number is NULL"
					"for the bay %d", composer_info.bayNumber);
				continue;

			}
			enclosure = ov_handler->ov_rest_resources.enclosure;
			while(enclosure){
				if(strstr(result.serialNumber, 
						enclosure->serialNumber)){
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure){
				if((enclosure->composer.presence
				[composer_info.bayNumber-1] == RES_ABSENT) && 
					(composer_info.presence == Present)){
					rv =  add_composer(oh_handler, 
							&composer_info, 
							&ha_node_result);
					if(rv != SA_OK){
						err("Unable to add composer "
							"with serial number:"
							" %s",
							result.serialNumber);
						return rv;
					}
				}else if((enclosure->composer.
					presence[composer_info.bayNumber-1]
					== RES_PRESENT) && 
					(composer_info.presence == Absent) ){

					rv = remove_composer(oh_handler, enclosure,
						 composer_info.bayNumber);
					if(rv != SA_OK){
						err("Unable to remove composer "
							"with serial number: "
							"%s",
							enclosure->serialNumber);
						return rv;
					}
				}else if((enclosure->composer.
					presence[composer_info.bayNumber-1]
					== RES_PRESENT) && 
					(composer_info.presence	== Present) ){
					if(strstr(
						enclosure->composer.serialNumber
						[ha_node_result.bayNumber-1], 
						composer_info.serialNumber)){
						continue;
					}else{
						rv = remove_composer(oh_handler,
						enclosure, 
						composer_info.bayNumber);
						if(rv != SA_OK){
							err("Unable to remove "
							"composer with"
							" serial number:"
							" %s",
							enclosure->serialNumber);
							return rv;
						}
						rv =  add_composer(oh_handler,
							&composer_info,
							&ha_node_result);
						if(rv != SA_OK){
							err("Unable to add "
							"composer with"
							" serial number: %s",
							composer_info.serialNumber);
							return rv;
						}
					}
				}
			}// end of if(enclosure)
		} // end of inner for loop
	} // end of outer for loop
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * re_discover_server
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discover the Server Blades.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_server(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureInfo enc_info = {{0}};
	char *server_doc = NULL, *enclosure_doc = NULL, *match = NULL;
	int i = 0, arraylen = 0, bay = 0;
	json_object *jvalue = NULL;
	struct enclosureStatus *enclosure = NULL;
	GHashTable *server_serial_hash = g_hash_table_new_full(g_str_hash, 
			g_str_equal,
			free_data,
			free_data);

	if (oh_handler == NULL) {
		err("Invalid parameter");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_SERVER_HARDWARE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getserverInfoArray(oh_handler, &response,
			ov_handler->connection,server_doc);
	if(rv != SA_OK || response.server_array == NULL) {
		CRIT("Server array not received. No servers added");
		return SA_OK; 
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.server_array) != json_type_array) {
		CRIT("Server array not received. No servers added");
		return SA_OK;
	}

	/*Getting the length of the array*/
	arraylen = json_object_array_length(response.server_array);
	for (i=0; i< arraylen; i++){
		if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
			dbg("shutdown_event_thread set. Returning in thread %p",
					g_thread_self());
			return SA_OK;
		}
		jvalue = json_object_array_get_idx(response.server_array, i);
		if (!jvalue) {
			CRIT("Invalid response for the serevre hardware"
					" in bay %d", i + 1);
			continue;
		}
		ov_rest_json_parse_server(jvalue,&info_result);
		g_hash_table_insert(server_serial_hash, 
				g_strdup(info_result.serialNumber), 	
				g_strdup("TRUE"));

		WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
				ov_handler->connection->hostname,
				info_result.locationUri);
		rv = ov_rest_getenclosureInfoArray(oh_handler,
				&enclosure_response, ov_handler->connection,
				enclosure_doc);
		if(rv != SA_OK ||
				enclosure_response.enclosure_array == NULL) {
			CRIT("ov_rest_getenclosureInfoArray failed");
			continue;
		}
		ov_rest_json_parse_enclosure(
				enclosure_response.enclosure_array,
				&enc_info);
		ov_rest_wrap_json_object_put(
				enclosure_response.root_jobj);

		enclosure = ov_handler->ov_rest_resources.enclosure;
		while(enclosure != NULL){
			if(strstr(enclosure->serialNumber,
						enc_info.serialNumber)){
				break;
			}
			enclosure = enclosure->next;
		}
		if(enclosure == NULL){
			CRIT("Enclosure data of the server"
					" serial number %s is unavailable",
					info_result.serialNumber);
			continue;
		}else{
			if(enclosure->server.presence[info_result.bayNumber-1]
					== RES_ABSENT){
				rv = add_inserted_blade(oh_handler, 
					&info_result, 
					enclosure);
				if(rv != SA_OK){
					err("Unable to add the server blade "
						"in enclosure serial: %s and "
						"device bay: %d",
						enclosure->serialNumber,
						info_result.bayNumber);
				}

			}else if(strstr(enclosure->server.
						serialNumber[info_result.
						bayNumber-1],
						info_result.serialNumber) ||
				!strcmp(info_result.serialNumber, "unknown")){
				continue;
			}else {

				rv = remove_server_blade(oh_handler, 
						info_result.bayNumber, 
						enclosure);
				if(rv != SA_OK){
					err("Unable to remove the server blade "
						"in enclosure serial: %s and "
						"device bay: %d",
						enclosure->serialNumber, 
						info_result.bayNumber);
				}

				rv = add_inserted_blade(oh_handler, 
						&info_result, 
						enclosure);
				if(rv != SA_OK){
					err("Unable to add the server blade "
						"in enclosure serial: %s and "
						"device bay: %d",
						enclosure->serialNumber,
						info_result.bayNumber);
				}
			}
		}
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		for(bay = 1; bay <= enclosure->server.max_bays; bay++){
			if(enclosure->server.presence[bay-1] 
			== RES_PRESENT && enclosure->server.type[bay-1] 
			== SERVER_HARDWARE){
				match = g_hash_table_lookup(server_serial_hash,
						enclosure->server.
						serialNumber[bay-1]);
				if(match == NULL){
					rv = remove_server_blade(oh_handler, 
						bay, enclosure);
					if(rv != SA_OK){
						err("Unable to remove the "
						"server blade in enclosure "
						"serial: %s and device bay: %d",
						enclosure->serialNumber,
						info_result.bayNumber);
					}
				}
			}
		}
		enclosure = enclosure->next;
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	g_hash_table_destroy(server_serial_hash);
	return SA_OK;
}
/*
 * add_inserted_blade 
 *      @oh_handler: Pointer to openhpi handler
 *      @info_result: Pointer to struct serverhardwareInfo
 *      @enclosure: Pointer to struct enclosureStatus
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered server blade to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_inserted_blade(struct oh_handler_state *handler, 
		struct serverhardwareInfo *info_result,
		struct enclosureStatus *enclosure)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	GSList *asserted_sensors = NULL;
	SaHpiRptEntryT *rpt = NULL;

	rv = build_discovered_server_rpt(handler,
			info_result, &resource_id);
	if(rv != SA_OK){
		err("Failed to Add server blade rpt for bay %d.",
				info_result->bayNumber);
		return rv;
	}
	ov_rest_update_resource_status(
			&enclosure->server,
			info_result->bayNumber,
			info_result->serialNumber,
			resource_id, RES_PRESENT,
			info_result->type);

	/* Build rdr entry for server */
	rv = ov_rest_build_server_rdr(handler, resource_id,
			info_result);
	if (rv != SA_OK) {
		err("Build RDR failed for inserted server id %d", resource_id);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for server blade in bay "
				" %d with resource id %d",
				info_result->bayNumber, resource_id);
		}
		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->server, info_result->bayNumber,
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
				UNSPECIFIED_RESOURCE);
		return rv;
	}
	rv = ov_rest_populate_event(handler, resource_id, &event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for server id %d",
							resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rpt = oh_get_resource_by_id (handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for server is %d", resource_id);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	/* For blades that don't support  managed hotswap, send simple
	 * hotswap event  */
	if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		event.event.EventType = SAHPI_ET_HOTSWAP;
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			SAHPI_HS_STATE_NOT_PRESENT;
		event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_ACTIVE;
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_OPERATOR_INIT;
		oh_evt_queue_push(handler->eventq,
				copy_ov_rest_event(&event));

		/* Raise the assert sensor events */
		if (asserted_sensors)
			ov_rest_assert_sen_evt(handler, rpt,
					asserted_sensors);

		return(SA_OK);
	}

	/* Raise the hotswap event for the inserted server blade */
	event.event.EventType = SAHPI_ET_HOTSWAP;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* NOT_PRESENT to INSERTION_PENDING state change happened due
	 *          * to operator action
	 *                   */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened
	 * due to auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	return SA_OK;
}
/**
 * re_discover_drive_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discover the Drive Enclosures.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_drive_enclosure(struct oh_handler_state *oh_handler)

{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct driveEnclosureInfoArrayResponse response = {0};
	struct driveEnclosureInfo info_result = {0};
	char *drive_enc_doc = NULL,*match = NULL;
	int i = 0, arraylen = 0, bay = 0;
	json_object *jvalue = NULL;
	struct enclosureStatus *enclosure = NULL;
	GHashTable *drive_serial_hash = g_hash_table_new_full(g_str_hash, 
			g_str_equal,
			free_data,
			free_data);

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_DRIVE_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getdriveEnclosureInfoArray(oh_handler,
			&response,
			ov_handler->connection,
			drive_enc_doc);
	if (rv != SA_OK || response.drive_enc_array == NULL) {
		CRIT("No repsonse from ov_rest_getdriveEnclosureInfoArray");
		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.drive_enc_array) != json_type_array) {
		CRIT("No drive enclosure arrays returned");
		return SA_OK;
	}
	/*Getting the length of the array*/
	arraylen = json_object_array_length(response.drive_enc_array);
	for (i=0; i< arraylen; i++){
		if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
			dbg("shutdown_event_thread set. Returning in thread %p",
					g_thread_self());
			return SA_OK;
		}
		jvalue = json_object_array_get_idx(response.drive_enc_array,i);
		if (!jvalue) {
			CRIT("Invalid response for the drive enclosure"
					" in bay %d", i + 1);
			continue;
		}
		ov_rest_json_parse_drive_enclosure(jvalue,&info_result);
		g_hash_table_insert(drive_serial_hash,
				g_strdup(info_result.serialNumber),
				g_strdup("TRUE"));
		/* Update resource_info structure with resource_id,
		 * serialNumber, and presence status
		 */
		enclosure = ov_handler->ov_rest_resources.enclosure;
		while(enclosure != NULL){
			if(strstr(enclosure->serialNumber,
						info_result.enc_serialNumber)){
				break;
			}
			enclosure = enclosure->next;
		}
		if(enclosure == NULL){
			CRIT("Enclosure data of the server"
					" serial number %s is unavailable",
					info_result.serialNumber);
			continue;
		}else {
			if(enclosure->server.presence[info_result.bayNumber-1]
					== RES_ABSENT){
				rv = add_inserted_drive_enclosure(oh_handler,
						&info_result, enclosure);
				if(rv != SA_OK){
					err("Unable to add the drive"
							"Enclosure in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							info_result.bayNumber);
				}
			}else if(strstr(enclosure->server.serialNumber
					[info_result.bayNumber-1], 
					info_result.serialNumber) || 
				!strcmp(info_result.serialNumber, "unknown")){

				continue;
			}else {
				rv =remove_drive_enclosure(oh_handler, 
						enclosure, 
						info_result.bayNumber);
				if(rv != SA_OK){
					err("Unable to remove the drive"
							"Enclosure in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							info_result.bayNumber);
				}
				rv = add_inserted_drive_enclosure(oh_handler,
						&info_result, enclosure);
				if(rv != SA_OK){
					err("Unable to add the drive"
						"Enclosure in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure->serialNumber,
						info_result.bayNumber);
				}
			}
		}
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		for(bay = 1; bay <= enclosure->server.max_bays; bay++){
			if(enclosure->server.presence[bay-1] 
			== RES_PRESENT && enclosure->server.type[bay-1] 
			== DRIVE_ENCLOSURE){
				match = g_hash_table_lookup(drive_serial_hash,
						enclosure->server.
						serialNumber[bay-1]);
				if(match == NULL){
					rv = remove_drive_enclosure(oh_handler, 
							enclosure, bay);
					if(rv != SA_OK){
						err("Unable to remove the drive"
						"Enclosure in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure-> serialNumber,
						info_result.bayNumber);
					}
				}
			}
		}
		enclosure = enclosure->next;	
	}
	g_hash_table_destroy(drive_serial_hash);
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}
/*
 * add_inserted_drive_enclosure 
 *      @oh_handler: Pointer to openhpi handler
 *	@info_result: Pointer to struct driveEnclosureInfo
 *	@enclosure: Pointer to struct enclosureStatus
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered drive 
 *	enclosure to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *	SA_ERR_HPI_INVALID_RESOURCE - on failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_inserted_drive_enclosure(struct oh_handler_state *handler,
		struct driveEnclosureInfo *info_result,
                struct enclosureStatus *enclosure)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	GSList *asserted_sensors = NULL;
	SaHpiRptEntryT *rpt = NULL;

	/* Build rpt entry for server */
	rv = build_discovered_drive_enclosure_rpt(handler,
			info_result, &resource_id);
	if(rv != SA_OK){
		err("Failed to Add Drive Enclosure rpt for bay %d.",
				info_result->bayNumber);
		return rv;
	}
	ov_rest_update_resource_status (
			&enclosure->server,
			info_result->bayNumber,
			info_result->serialNumber,
			resource_id,
			RES_PRESENT,
			info_result->type);
	/* Build rdr entry for drive enclosure */
	rv = ov_rest_build_drive_enclosure_rdr(handler,
			resource_id, info_result);
	if (rv != SA_OK) {
		err("Build RDR failed for Drive enclosure in bay %d",
					info_result->bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for drive enclosure "
					"id %d", resource_id);
		}
		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->server, info_result->bayNumber,
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
				UNSPECIFIED_RESOURCE);
		return rv;
	}
	rv = ov_rest_populate_event(handler, resource_id, &event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for drive enclosure id %d",
							resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rpt = oh_get_resource_by_id (handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for drive enclosure in bay %d",
						info_result->bayNumber);
		return SA_ERR_HPI_INVALID_RESOURCE;
	}

	/* For blades that don't support  managed hotswap, send simple
	 * hotswap event  */
	if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		event.event.EventType = SAHPI_ET_HOTSWAP;
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			SAHPI_HS_STATE_NOT_PRESENT;
		event.event.EventDataUnion.HotSwapEvent.HotSwapState =
			SAHPI_HS_STATE_ACTIVE;
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_OPERATOR_INIT;
		oh_evt_queue_push(handler->eventq,
				copy_ov_rest_event(&event));

		/* Raise the assert sensor events */
		if (asserted_sensors)
			ov_rest_assert_sen_evt(handler, rpt,
					asserted_sensors);

		return(SA_OK);
	}

	/* Raise the hotswap event for the inserted server blade */
	event.event.EventType = SAHPI_ET_HOTSWAP;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* NOT_PRESENT to INSERTION_PENDING state change happened due
	 * to operator action
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened
	 * due to auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	/* Push the Power off and power on HS event below if needed FIXME */	

#if 0
	/* Raise the assert sensor events */
	if (asserted_sensors) {
		rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
		ov_rest_assert_sen_evt(handler, rpt, asserted_sensors);
	}
#endif


	return SA_OK;
}
/*
 * remove_drive_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *	@enclosure: Pointer to struct enclosureStatus
 *	@bay_number: drive enclosure bay number
 *
 * Purpose:
 *      Removes the Drive Enclosure from RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT remove_drive_enclosure(struct oh_handler_state *handler,
			struct enclosureStatus *enclosure,
			SaHpiInt32T bay_number)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	SaHpiRptEntryT *rpt = NULL;
	if (handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	resource_id = enclosure->server.resource_id[bay_number-1];
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if(rpt == NULL){
		err("RPT is NULL for drive enclosure in bay %d", bay_number);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Push the hotswap event below */

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.event.Severity = event.resource.ResourceSeverity;

	if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
		/* Simple hotswap */
		event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
			SAHPI_HS_STATE_ACTIVE;
	} else {
		/* Managed hotswap */
		hotswap_state = (struct ovRestHotswapState *)
			oh_get_resource_data(handler->rptcache,
					event.resource.ResourceId);
		if (hotswap_state == NULL) {
			err("Failed to get hotswap state of drive enclosure"
					" in bay %d", bay_number);
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
		 * operator action
		 */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_OPERATOR_INIT;
	} else {
		/* This state change happened due to a surprise extraction */
		event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
			SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
	}

	/* Push the hotswap event to remove the resource from OpenHPI RPTable */
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));


	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(handler, rpt->ResourceId);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for drive enclosure id %d",
				rpt->ResourceId);
	}
	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(handler->rptcache,
			resource_id);

	/* reset resource_info structure to default values */
	ov_rest_update_resource_status(
			&enclosure->server, bay_number,
			"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
			UNSPECIFIED_RESOURCE);

	return SA_OK;
}
/*
 * re_discover_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the Interconnects.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
	SA_ERR_HPI_INVALID_PARAMS - on failure.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_interconnect(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enc_info = {{0}};
	char* interconnect_doc = NULL, *enclosure_doc = NULL, *match = NULL;
	int i = 0,arraylen = 0, bay = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL;
	GHashTable *interconnect_serial_hash = g_hash_table_new_full(g_str_hash,
			g_str_equal,
			free_data,
			free_data);

	if (oh_handler == NULL || oh_handler->data == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_INTERCONNECT_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
	if(rv != SA_OK || response.interconnect_array == NULL){
		CRIT("Failed to get the response from "
				"ov_rest_getinterconnectInfoArray "
				"for interconnects");

		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.interconnect_array) !=
			json_type_array) {
		CRIT("Not adding any interconnects as no array returned");
		return SA_OK;
	}

	/*Getting the length of the array*/
	arraylen = json_object_array_length(response.interconnect_array);
	while(1){
		for (i=0; i< arraylen; i++){
			if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
				dbg("shutdown_event_thread set. Returning in "
						"thread %p", g_thread_self());
				return SA_OK;
			}
			memset(&result, 0, sizeof(struct interconnectInfo));
			jvalue = json_object_array_get_idx(
					response.interconnect_array, i);
			if (!jvalue) {
				CRIT("Invalid response for the interconnect"
						" in bay %d", i + 1);
				continue;
			}
			ov_rest_json_parse_interconnect(jvalue,&result);
			g_hash_table_insert(interconnect_serial_hash,
					g_strdup(result.serialNumber),
					g_strdup("TRUE"));

			/* Find the Enclosure for this interconnect to update 
			 * the Resource matrix table */
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					result.locationUri);
			rv = ov_rest_getenclosureInfoArray(oh_handler,
					&enclosure_response, 
					ov_handler->connection,
					enclosure_doc);
			if(rv != SA_OK || enclosure_response.
					enclosure_array == NULL) {
				CRIT("Failed to get the response from "
					"ov_rest_getenclosureInfoArray\n");
				continue;
			}
			ov_rest_json_parse_enclosure(
					enclosure_response.enclosure_array,
					&enc_info);
			ov_rest_wrap_json_object_put(
					enclosure_response.root_jobj);
			enclosure = ov_handler->ov_rest_resources.enclosure;
			while(enclosure != NULL){
				if(strstr(enclosure->serialNumber,
						enc_info.serialNumber)){
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the interconnect with"
					" serial number %s is unavailable",
						result.serialNumber);
				continue;
			}else {
				if(enclosure->interconnect.
						presence[result.bayNumber-1]
						== RES_ABSENT){
					rv =add_inserted_interconnect(
							oh_handler, 
							enclosure, 
							&result);
					if(rv != SA_OK){
						err("Unable to add the inter"
							"connect in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}else if(strstr(enclosure->interconnect.
					serialNumber[result.bayNumber-1],
					result.serialNumber) ||	!strcmp(
					result.serialNumber, "unknown")){

					continue;
				}else {
					rv =remove_interconnect_blade(
							oh_handler, 
							result.bayNumber, 
							enclosure);
					if(rv != SA_OK){
						err("Unable to remove the inter"
							"connect in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
					rv = add_inserted_interconnect(
							oh_handler,
							enclosure, &result);
					if(rv != SA_OK){
						err("Unable to add the inter"
							"connect in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}
			}
		}
		ov_rest_wrap_json_object_put(response.root_jobj);
		if(response.next_page == NULL){
			break;
		}else {
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					response.next_page);
			memset(&response, 0, sizeof(response));
			rv = ov_rest_getinterconnectInfoArray(oh_handler, 
					&response, ov_handler->connection, 
					interconnect_doc);
			if(rv != SA_OK || response.interconnect_array == NULL){
				CRIT("Failed to get the response from "
					"ov_rest_getinterconnectInfoArray"
					" for interconnects");

				return SA_OK;
			}

			/* Checking for json object type, if it is not array, 
			 * return */
			if (json_object_get_type(response.interconnect_array) !=
					json_type_array) {
				CRIT("Not adding any interconnects as no array"
						" returned");
				return SA_OK;
			}

			/*Getting the length of the array*/
			arraylen = json_object_array_length(
					response.interconnect_array);

		}
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		for(bay = 1; bay <= enclosure->interconnect.max_bays; bay++){
			if(enclosure->interconnect.presence[bay-1] 
			== RES_PRESENT && enclosure->interconnect.type[bay-1] 
			== INTERCONNECT){
				match = g_hash_table_lookup(
						interconnect_serial_hash,
						enclosure->interconnect.
						serialNumber[bay-1]);
				if(match == NULL){
					rv = remove_interconnect_blade(
						oh_handler, bay, enclosure);
					if(rv != SA_OK){
						err("Unable to remove the drive"
							"Enclosure in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							bay);
					}
				}
			}
		}
		enclosure = enclosure->next;
	}
	g_hash_table_destroy(interconnect_serial_hash);
	return SA_OK;
}
/*
 * add_inserted_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *      @enclosure: Pointer to struct enclosureStatus
 *      @result: Pointer to struct interconnectInfo
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered interconnect to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_inserted_interconnect(struct oh_handler_state *handler,
				struct enclosureStatus *enclosure,
				struct interconnectInfo *result)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	GSList *asserted_sensors = NULL;

	rv = ov_rest_build_interconnect_rpt(handler,
			result, &resource_id);
	if(rv != SA_OK){
		err("Failed to Add Interconnect rpt for bay %d.",
				result->bayNumber);
		return rv;
	}
	ov_rest_update_resource_status(
			&enclosure->interconnect,
			result->bayNumber, result->serialNumber,
			resource_id, RES_PRESENT,
			result->type);

	/* Build rdr entry for interconnect */
	rv = ov_rest_build_interconnect_rdr(handler, resource_id, result);
	if (rv != SA_OK) {
		err("Failed to build inventory RDR for interconnect in bay %d",
						result->bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for interconnect id %d",
					resource_id);
		}
		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->interconnect,
				result->bayNumber,
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
				UNSPECIFIED_RESOURCE);
		return rv;
	}
	rv = ov_rest_populate_event(handler, resource_id, &event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for interconnect in "
					"bay %d", result->bayNumber);
		return rv;
	}
	/* Raise the hotswap event for the inserted interconnect blade */
	event.event.EventType = SAHPI_ET_HOTSWAP;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	/* NOT_PRESENT to INSERTION_PENDING state change happened due
	 * to operator action
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	event.rdrs = NULL;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_INSERTION_PENDING;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* INSERTION_PENDING to ACTIVE state change happened
	 * due to auto policy of server blade
	 */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_AUTO_POLICY;
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	/* Push the Power On Power oF HS event if needed later FIXME*/ 
#if 0
	/* Raise the assert sensor events */
	if (asserted_sensors) {
		rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
		ov_rest_assert_sen_evt(handler, rpt, asserted_sensors);
	}
#endif

	return SA_OK;

}
/*
 * re_discover_sas_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the Sas-Interconnects.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_sas_interconnect(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enc_info = {{0}};
	char* interconnect_doc = NULL, *enclosure_doc = NULL, *match = NULL;
	int i = 0,arraylen = 0, bay = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL;
	GHashTable *interconnect_serial_hash = g_hash_table_new_full(g_str_hash,
			g_str_equal,
			free_data,
			free_data);


	if (oh_handler == NULL || oh_handler->data == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_SAS_INTERCONNECT_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getinterconnectInfoArray(oh_handler, &response,
			ov_handler->connection, interconnect_doc);
	if(rv != SA_OK || response.interconnect_array == NULL){
		CRIT("Failed to get the response from "
				"ov_rest_getinterconnectInfoArray "
				" for SAS interconnects");

		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.interconnect_array) !=
			json_type_array) {
		CRIT("Not adding any interconnects as no array returned");
		return SA_OK;
	}

	/*Getting the length of the array*/
	arraylen = json_object_array_length(response.interconnect_array);
	while(1){
		for (i=0; i< arraylen; i++){
			if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
				dbg("shutdown_event_thread set. Returning in" 
					"thread %p", g_thread_self());
				return SA_OK;
			}
			memset(&result, 0, sizeof(struct interconnectInfo));
			jvalue = json_object_array_get_idx(
					response.interconnect_array, i);
			if (!jvalue) {
				CRIT("Invalid response for the interconnect"
						" in bay %d", i + 1);
				continue;
			}
			ov_rest_json_parse_interconnect(jvalue,&result);
			g_hash_table_insert(interconnect_serial_hash,
					g_strdup(result.serialNumber),
					g_strdup("TRUE"));

			/* Find the Enclosure for this interconnect to update 
			 * the Resource matrix table */
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					result.locationUri);
			rv = ov_rest_getenclosureInfoArray(oh_handler,
					&enclosure_response, 
					ov_handler->connection,
					enclosure_doc);
			if(rv != SA_OK || enclosure_response.enclosure_array 
				== NULL) {
				CRIT("Failed to get the response from "
					"ov_rest_getenclosureInfoArray\n");
				continue;
			}
			ov_rest_json_parse_enclosure(
					enclosure_response.enclosure_array,
					&enc_info);
			ov_rest_wrap_json_object_put(
					enclosure_response.root_jobj);
			enclosure = ov_handler->ov_rest_resources.enclosure;
			while(enclosure != NULL){
				if(strstr(enclosure->serialNumber,
						enc_info.serialNumber)){
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the interconnect with"
					" serial number %s is unavailable",
						result.serialNumber);
				continue;
			}else {
				if(enclosure->interconnect.
					presence[result.bayNumber-1]
					== RES_ABSENT){
					rv = add_inserted_interconnect(
							oh_handler, 
							enclosure, 
							&result);
					if(rv != SA_OK){
						err("Unable to add the sas"
						"-interconnect in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure->serialNumber,
						result.bayNumber);
					}
				}else if(strstr(enclosure->interconnect.
					serialNumber[result.bayNumber-1],
					result.serialNumber) ||	!strcmp(result.
					serialNumber, "unknown")){

					continue;
				}else {
					rv = remove_interconnect_blade(
							oh_handler, 
							result.bayNumber, 
							enclosure);
					if(rv != SA_OK){
						err("Unable to remove the sas"
						"-interconnect in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure->serialNumber,
						result.bayNumber);
					}
					rv = add_inserted_interconnect(
							oh_handler,
							enclosure, 
							&result);
					if(rv != SA_OK){
						err("Unable to add the sas"
						"-interconnect in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure->serialNumber,
						result.bayNumber);
					}
				}
			}
		}
		ov_rest_wrap_json_object_put(response.root_jobj);
		if(response.next_page == NULL){
			break;
		}else{
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					response.next_page);
			rv = ov_rest_getinterconnectInfoArray(oh_handler, 
					&response, ov_handler->connection, 
					interconnect_doc);
			if(rv != SA_OK || response.interconnect_array == NULL){
				CRIT("Failed to get the response from "
					"ov_rest_getinterconnectInfoArray "
					"SAS interconnects");
				return SA_OK;
			}

			/* Checking for json object type, if it is not array, 
 			 * return */
			if (json_object_get_type(response.interconnect_array) !=
					json_type_array) {
				CRIT("Not adding any interconnects as no array"
						" returned");
				return SA_OK;
			}

			/*Getting the length of the array*/
			arraylen = json_object_array_length(
					response.interconnect_array);
		}
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		for(bay = 1; bay <= enclosure->interconnect.max_bays; bay++){
			if(enclosure->interconnect.presence[bay-1]
			 == RES_PRESENT && enclosure->interconnect.type[bay-1]
				 == SAS_INTERCONNECT){
				match = g_hash_table_lookup(
						interconnect_serial_hash,
						enclosure->interconnect.
						serialNumber[bay-1]);
				if(match == NULL){
					rv = remove_interconnect_blade(
						oh_handler, bay, enclosure);
					if(rv != SA_OK){
						err("Unable to remove the sas"
						"-interconnect in enclosure"
						" serial: %s and device"
						" bay: %d",
						enclosure->serialNumber, bay);
					}
				}
			}
		}
		enclosure = enclosure->next;
	}
	g_hash_table_destroy(interconnect_serial_hash);
	return SA_OK;
}
/*
 * re_discover_powersupply
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the PowerSupply.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/


SaErrorT re_discover_powersupply(struct oh_handler_state *oh_handler)
{

	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct powersupplyInfo result = {0};
	char* enclosure_doc = NULL;
	int i = 0,j = 0,arraylen = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL, *jvalue_ps = NULL, *jvalue_ps_array = NULL;

	ov_handler = (struct ov_rest_handler *) oh_handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT("Failed to get the response from "
				"ov_rest_getenclosureInfoArray\n");
		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array){
		CRIT("Not adding power supplies, no enclosure array returned");
		return SA_OK;
	}

	arraylen = json_object_array_length(response.enclosure_array);
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue) {
			CRIT("Invalid response for the enclosure in bay %d",
					i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue, &enclosure_result);
		jvalue_ps_array = ov_rest_wrap_json_object_object_get(jvalue,
				"powerSupplyBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_ps_array) != json_type_array) {
			CRIT("Not adding power supplies to enclosure %d,"
					" no array returned for that",i);
			return rv;
		}
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
			CRIT("Enclosure data of the powersupply"
					" serial number %s is unavailable",
					result.serialNumber);
			continue;
		}

		for(j = 0; j < enclosure_result.powerSupplyBayCount; j++){
			jvalue_ps = json_object_array_get_idx(jvalue_ps_array,
					j);
			if (!jvalue_ps) {
				CRIT("Invalid response for the powersupply"
						" in bay %d", i + 1);
				continue;
			}
			ov_rest_json_parse_powersupply(jvalue_ps, &result);

			if(result.presence == Present){

				if(enclosure->ps_unit.presence
					[result.bayNumber-1] == RES_ABSENT){
					rv = add_inserted_powersupply(
						oh_handler, enclosure, &result);
				}else if(strstr(enclosure->ps_unit.serialNumber
					[result.bayNumber-1],result.
					serialNumber) || !strcmp(result.
					serialNumber, "unknown")){

					continue;
				}else {
					rv = remove_powersupply(oh_handler, 
						enclosure, result.bayNumber);
					if(rv != SA_OK){
						err("Unable to remove the power"
							"supply in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
					rv = add_inserted_powersupply(
						oh_handler, enclosure, &result);
					if(rv != SA_OK){
						err("Unable to remove the power"
							"supply in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}


			}else if(result.presence == Absent){
				if(enclosure->ps_unit.presence
						[result.bayNumber-1] 
						== RES_PRESENT){
					rv = remove_powersupply(oh_handler, 
						enclosure, result.bayNumber);
					if(rv != SA_OK){
						err("Unable to remove the power"
							" supply in enclosure"
							" serial: %s and device"
							" bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}
			}

		}
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}
/*
 * add_inserted_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *      @enclosure: Pointer to struct enclosureStatus
 *      @result: Pointer to struct powersupplyInfo
 *
 * Purpose:
 *      Build the rpt and rdrs and adds the newly discovered 
 *	powersupply unit to RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT add_inserted_powersupply(struct oh_handler_state *handler,
				struct enclosureStatus *enclosure,
				struct powersupplyInfo *result)
{
	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	struct oh_event event = {0};
	GSList *asserted_sensors = NULL;
	SaHpiRptEntryT *enc_rpt = NULL;
	int enc_loc ;

	enc_rpt = oh_get_resource_by_id(handler->rptcache,
			enclosure->enclosure_rid);
	enc_loc = enc_rpt->ResourceEntity.Entry[0].EntityLocation;

	rv = ov_rest_build_powersupply_rpt(handler,
			result, &resource_id, enc_loc);
	if(rv != SA_OK){
		err("Failed to Add powersupply rpt for bay %d.",
				result->bayNumber);
		return rv;
	}
	ov_rest_update_resource_status(
			&enclosure->ps_unit,
			result->bayNumber, result->serialNumber,
			resource_id, RES_PRESENT,
			result->type);

	/* Build rdr entry for server */
	rv = ov_rest_build_powersupply_rdr(handler, resource_id, result);
	if (rv != SA_OK) {
		err("Build RDR failed for power supply in bay %d",
						result->bayNumber);
		/* Free the inventory info from inventory RDR */
		rv = ov_rest_free_inventory_info(handler, resource_id);
		if (rv != SA_OK) {
			err("Inventory cleanup failed for powersupply id %d",
					resource_id);
		}
		oh_remove_resource(handler->rptcache, resource_id);
		/* reset resource_info structure to default values */
		ov_rest_update_resource_status(
				&enclosure->ps_unit, result->bayNumber, 
				"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
				UNSPECIFIED_RESOURCE);

		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rv = ov_rest_populate_event(handler, resource_id, &event,
			&asserted_sensors);
	if (rv != SA_OK) {
		err("Populating event struct failed for powersupply in bay %d",
						result->bayNumber);
		return rv;
	}

	event.event.EventType = SAHPI_ET_HOTSWAP;
	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	/* NOT_PRESENT to ACTIVE state change happened due to operator action */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_OPERATOR_INIT;
	/* Push the hotswap event to add the resource to OpenHPI RPTable */
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));

	/* Raise the assert sensor events */

	return SA_OK;

}
/*
 * remove_drive_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *      @enclosure: Pointer to struct enclosureStatus
 *      @bay_number: Power Supply Unit bay number
 *
 * Purpose:
 *      Removes the Power Supply Unit from RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *	SA_ERR_HPI_INVALID_PARAMS - on failure.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT remove_powersupply(struct oh_handler_state *handler,
			struct enclosureStatus *enclosure,
			SaHpiInt32T bay_number)
{

	SaErrorT rv = SA_OK;
	SaHpiResourceIdT resource_id = 0;
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event event = {0};
	if (handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	resource_id = enclosure->ps_unit.resource_id[bay_number-1];
	rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
	if(rpt == NULL){
		err("RPT is NULL for the powersupply in bay %d with "
			"resource ID %d", bay_number, resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_update_hs_event(handler, &event);
	/* Push the hotswap event below */
	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
	event.event.Source = event.resource.ResourceId;
	event.event.Severity = event.resource.ResourceSeverity;

	event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
		SAHPI_HS_STATE_ACTIVE;
	event.event.EventDataUnion.HotSwapEvent.HotSwapState =
		SAHPI_HS_STATE_NOT_PRESENT;
	/* This state change happened due to surprise extraction */
	event.event.EventDataUnion.HotSwapEvent.CauseOfStateChange =
		SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
	/* Push the hotswap event to remove the resource from OpenHPI RPTable */
	oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));


	/* Free the inventory info from inventory RDR */
	rv = ov_rest_free_inventory_info(handler, resource_id);
	if (rv != SA_OK) {
		err("Inventory cleanup failed for powersupply id %d",
				resource_id);
	}
	/* Remove the resource from plugin RPTable */
	rv = oh_remove_resource(handler->rptcache,
			event.resource.ResourceId);

	/* reset resource_info structure to default values */
	ov_rest_update_resource_status(
			&enclosure->ps_unit, bay_number,
			"", SAHPI_UNSPECIFIED_RESOURCE_ID, RES_ABSENT,
			UNSPECIFIED_RESOURCE);

	return SA_OK;

}
/*
 * re_discover_fan
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Re-discovers the Fan.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT re_discover_fan(struct oh_handler_state *oh_handler)
{

	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct fanInfo result = {0};
	char* enclosure_doc = NULL;
	int i = 0,j = 0,arraylen = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL, *jvalue_fan = NULL; 
	json_object *jvalue_fan_array = NULL;

	ov_handler = (struct ov_rest_handler *) oh_handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT("Failed to get the response from "
				"ov_rest_getenclosureInfoArray\n");
		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array){
		CRIT("Not adding fans, no enclosure array returned");
		return SA_OK;
	}

	arraylen = json_object_array_length(response.enclosure_array);
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue) {
			CRIT("Invalid response for the enclosure in bay %d",
					i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue, &enclosure_result);
		jvalue_fan_array = ov_rest_wrap_json_object_object_get(jvalue,
				"fanBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_fan_array) != json_type_array) {
			CRIT("Not adding fan to enclosure %d,"
					" no array returned for that",i);
			return SA_OK;
		}
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
			CRIT("Enclosure data of the fan"
					" serial number %s is unavailable",
					result.serialNumber);
			continue;
		}

		for(j = 0; j < enclosure_result.fanBayCount; j++){
			jvalue_fan = json_object_array_get_idx(jvalue_fan_array,
					j);
			if (!jvalue_fan) {
				CRIT("Invalid response for the fan"
						" in bay %d", i + 1);
				continue;
			}
			ov_rest_json_parse_fan(jvalue_fan, &result);
			if(result.presence == Present){

				if(enclosure->fan.presence[result.bayNumber-1] 
				== RES_ABSENT){
					rv = ov_rest_add_fan(oh_handler, 
						&result, enclosure);
				}else if(strstr(enclosure->fan.serialNumber
					[result.bayNumber-1],result.
					serialNumber) || !strcmp(result.
					serialNumber, "unknown")){

					continue;
				}else {
					rv = ov_rest_remove_fan(oh_handler, 
						result.bayNumber, enclosure);
					if(rv != SA_OK){
						err("Unable to remove the fan"
							" in enclosure serial: "
							"%s and fan bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
					rv = ov_rest_add_fan(oh_handler, 
						&result, enclosure);
					if(rv != SA_OK){
						err("Unable to add the fan"
							" in enclosure serial: "
							"%s and fan bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}


			}else if(result.presence == Absent){
				if(enclosure->fan.presence[result.bayNumber-1] 
				== RES_PRESENT){
					rv = ov_rest_remove_fan(oh_handler, 
						result.bayNumber, enclosure);
					if(rv != SA_OK){
						err("Unable to remove the fan"
							" in enclosure serial: "
							"%s and fan bay: %d",
							enclosure->serialNumber,
							result.bayNumber);
					}
				}
			}

		}
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}
