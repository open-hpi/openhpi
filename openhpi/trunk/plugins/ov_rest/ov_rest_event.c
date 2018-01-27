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
 * This file is having the code for handling the events which
 * are coming from Oneview Synergy.
 *
 *      ov_rest_get_event()            - this is not required.
 *      			  	 we will think about it and decide.
 *
 *      ov_rest_proc_add_task()        - Call the Task/Event Handler for
 *                                       resource add task, if taskState is
 *                                       completed and percentComplete is 100%.
 *
 *      ov_rest_getActiveLockedEventArray() - Get Active / Locked Event Array
 *                                            through REST GET call.
 *
 *      process_active_and_locked_alerts() - Processing Active and Locked alert
 *                                           and pushing them to event queue.
 *
 *      ov_rest_setuplistner()          - Create and download the AMQP client
 *                                        certificate, private key and root
 *                                        CA certificate.
 *
 *      ov_rest_event_thread()          - handles the OV events and pushes the
 *                                        same into the framework queue
 *
 *      process_ov_events()             - handles the ov events and calls
 *                                        correct handler function for
 *                                        different events
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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <amqp_ssl_socket.h>
#include <amqp_framing.h>
#include <assert.h>
extern void ov_die_on_amqp_error(amqp_rpc_reply_t x, char const *context);

/**
 * ov_rest_get_event
 *      @oh_handler: Pointer to openhpi handler structure
 *
 * Purpose:
 *      oh_get_event standards infrastructure call.
 *      Just a dummy function as SCMB thread handles events
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK - Always returns SA_OK
 **/

int ov_rest_get_event(void *oh_handler)
{
        /* Since OV sends the events on any changes to resources
         * Using this function, OV need not to be polled for resource state
         * changes.  This method always returns 0
         *
         * No events for infra-structure to process
         */
        dbg("oh_get_event is a dummy function as SCMB thread handles events");
        return SA_OK;
}

/**
 * oem_event_to_file
 *      @ov_handler: Pointer to ov_rest_handler structure
 *      @ov_event: Pointer to eventInfo structure
 *      @oem_event: Pointer to oh_event structure
 *
 * Purpose:
 * 	writes the oem event to a file called oem_event.log
 * 	
 * Detailed Description: NA
 *
 * Return values:
 * 	SA_ERR_HPI_ERROR 	- on Error
 * 	SA_OK 			- on Success
 **/
SaErrorT oem_event_to_file(struct oh_handler_state *handler,
                        struct eventInfo* ov_event,
                        struct oh_event* oem_event)
{
        FILE *oemfile = NULL;
        char *event = NULL, *temp = NULL, *oem_file_path = NULL;
        oh_big_textbuffer bigbuf;
        int len = 0, num = 0;
	
        temp = (char *)g_hash_table_lookup(handler->config, "entity_root");
        sscanf(temp, "%*[^0-9]%d", &num);
        WRAP_ASPRINTF(&oem_file_path, "%s/%s%s%d%s",
        			OV_REST_PATH, "oem_event", "_", num, ".log");
        oemfile = fopen(oem_file_path,"a+");
        if(oemfile == NULL)
        {
                CRIT("Error opening OEM_EVENT_FILE file: %s",oem_file_path);
		wrap_free(oem_file_path);
                return SA_ERR_HPI_ERROR;
        }
        wrap_free(oem_file_path);
        oem_file_path = NULL;
        oh_decode_entitypath(&oem_event->resource.ResourceEntity, &bigbuf);
        if(!strcmp(ov_event->alertState, "Cleared")){
               WRAP_ASPRINTF(&event,"ResourceID: %d \nTime Stamp: %s "
        		"\nEntity Path: %s"
                        "\nSeverity: %s \nAlert State: %s "
                        "\nEvent Description: %s. "
        		"\nCorrective Action: %s. "
                        "\n             ************            \n",
                        oem_event->event.Source,
                        ov_event->created, 
        		bigbuf.Data,
			ov_event->severity, ov_event->alertState,
                        ov_event->description, ov_event->correctiveAction );
        } else {
               WRAP_ASPRINTF(&event, "ResourceID: %d " 
        		"\nTime Stamp: %s "
        		"\nEntity Path: %s"
                        "\nSeverity: %s \nEvent Description: %s "
                        "\nCorrective Action: %s "
                        "\n             ************            \n",
                        oem_event->event.Source, 
        		ov_event->created,
        		bigbuf.Data,
                        ov_event->severity, 
        		ov_event->description,
                        ov_event->correctiveAction);

        }
        len = strlen(event);
        if(len != fwrite(event, sizeof(char), len, oemfile))
        {
                CRIT("\nError in Writing to OEM_EVENT_FILE file\n");
                fclose(oemfile);
                wrap_free(event);
                return SA_ERR_HPI_ERROR;
        }

        fclose(oemfile);
        wrap_free(event);
        return SA_OK;
}


/* ov_rest_get_baynumber
 * 	@resourceID: Pointer to the resourceID String
 *
 * Purpose:
 * 	Takes the ResourceID string (Ex: "/rest/v1/Blade/1") and parses it to
 * 	get the bay number of the resource.
 *
 * Detailed Description: NA
 *
 *  Return values:
 *  	1 to 12 (Bay Number) - Success
 *  	0  - When resource is not in RPT 
 */

int ov_rest_get_baynumber(const char *resourceID)
{
	int bayNumber = 0, len = 0;

	if(resourceID){
		len = strlen(resourceID)-1;
		while(len){
			/* Search for the last '/' in the resource ID string*/
			if(resourceID[len]=='/'){
				/* Get the baynumber string starting address 
 				 * location.
 				 */
				bayNumber = atoi(resourceID + len+1);
				if(bayNumber<=12&&bayNumber>0){
					return bayNumber;
				}else {
					dbg("Baynumber %d not in range(1-12)",
						bayNumber );
					return 0;
				}
			}	
			len--;
		}
		warn("Baynumber not found for %s", resourceID);
		return 0;
	}else {
		err("resoureID passed in is NULL");
		return 0;
	}
}

/*
 * ov_rest_getActiveLockedEventArray
 *      @connection: Pointer to oneview handler connection
 *      @response  : Pointer to the eventArrayResponse structure
 *
 * Purpose:
 *      Get Active / Locked Event Array through REST GET call.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - Success.
 *      SA_ERR_HPI_INTERNAL_ERROR - On Internal Error.
 *      SA_ERR_HPI_INVALID_PARAMS - On Invalid Parameters.
 */
SaErrorT ov_rest_getActiveLockedEventArray(REST_CON *connection,
                        struct eventArrayResponse *response)
{
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);

	if (connection == NULL || response == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	/* Get a curl handle */
	CURL* curl = curl_easy_init();

	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		err("Get Active or Locked Event Array Failed");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}else
	{
		json_object * jobj = s.jobj;
		response->root_jobj = jobj;
		if(!jobj){
			err("Get Active or Locked Event Array Failed");
			return SA_ERR_HPI_INVALID_DATA;
		}
		json_object_object_foreach(jobj, key,val){
			if(!strcmp(key,"total")){
				response->total = json_object_get_string(val);
				break;
			}
		}
		/*Getting the array if it is a key value pair*/
		response->event_array =
			ov_rest_wrap_json_object_object_get(jobj, "members");
		if (!response->event_array) {
			response->event_array = jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/*
 * process_active_and_locked_alerts
 *      @handler       : Pointer to openhpi handler structure.
 *      @event_response: Pointer to the eventArrayResponse structure.
 *
 * Purpose:
 *      Processing Active and Locked alerts and pushing them to event queue.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - On Success.
 */
SaErrorT process_active_and_locked_alerts(struct oh_handler_state *handler,
                        struct eventArrayResponse *event_response )
{
        SaErrorT rv = SA_OK;
        struct eventInfo event = {0};
	int i = 0, arraylen = 0;
        json_object *jvalue = NULL;
        struct serverhardwareInfoArrayResponse response = {0};
        struct serverhardwareInfo info_result = {0};
        struct driveEnclosureInfoArrayResponse drive_enc_response = {0};
        struct driveEnclosureInfo drive_enc_info_result = {0};
        struct interconnectInfoArrayResponse int_response = {0};
        struct interconnectInfo int_info_result = {0};
        struct enclosureInfoArrayResponse enc_response = {0};
        struct enclosureInfo enc_result = {{0}};
        struct enclosureStatus *enclosure = NULL;
        char *enc_doc = NULL, *server_doc = NULL, *interconnect_doc = NULL;
        char *drive_enc_doc = NULL;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event oem_event = {0};
        struct ov_rest_handler *ov_handler = NULL;

        if ( handler == NULL || event_response == NULL ) { 
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        ov_handler = handler->data;

        /* Checking for json object type, if it is not array, return */
        if (json_object_get_type(event_response->event_array) !=
                                                       json_type_array) {
                CRIT("No array for active/locked alerts.");
                return SA_ERR_HPI_INVALID_DATA;
        } 

        /*Getting the length of the array*/
        arraylen = json_object_array_length(event_response->event_array);
        for (i=0; i< arraylen; i++){
                jvalue =
                    json_object_array_get_idx(event_response->event_array, i);
                if (!jvalue) {
                        CRIT("Invalid response for the event %d", i + 1);
                        continue;
                }
                ov_rest_json_parse_alerts (jvalue, &event);
                wrap_free(ov_handler->connection->url);
                if (!strcmp((event.phyResourceType), "enclosures")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event.resourceUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                      &enc_response,
                                                      ov_handler->connection,
                                                      enc_doc);
                        if (rv != SA_OK ||
                                       enc_response.enclosure_array == NULL) {
                                CRIT("Failed to get Enclosure Info Array");
                                continue;
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
                                CRIT("Enclosure data of enclosure"
                                          " serial number %s is unavailable",
                                           enc_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                continue;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->enclosure_rid);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the Enclosure"
					" id %d", enclosure->enclosure_rid);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                continue;
                        }
                }
                else if (!strcmp((event.phyResourceType),"server-hardware")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event.resourceUri);
                        rv = ov_rest_getserverInfoArray(handler, &response,
                                      ov_handler->connection, server_doc);
                        if (rv != SA_OK || response.server_array == NULL) {
                                CRIT("Failed to get Server Info Array");
                                continue;
                        }
                        ov_rest_json_parse_server (response.server_array,
                                                                &info_result);
			ov_rest_wrap_json_object_put(response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                       enc_response.enclosure_array == NULL) {
                                CRIT("Failed to get Enclosure Info Array");
                                continue;
                        }
                        ov_rest_json_parse_enclosure(
                                           enc_response.enclosure_array,
                                           &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the server Resourceid by looking at the
                         * enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                CRIT("Enclosure data of the Server"
                                          " serial number %s is unavailable",
                                           info_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(server_doc);
                                continue;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->server.resource_id
                                                 [info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the server at"
                                        " bay number %d",
                                       info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(server_doc);
                                continue;
                        }
                }
                else if (!strcmp((event.phyResourceType),"drive-enclosures")){
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event.resourceUri);
                        rv = ov_rest_getdriveEnclosureInfoArray(handler,
                                               &drive_enc_response,
                                            ov_handler->connection,
                                                    drive_enc_doc);
                        if (rv != SA_OK ||
                                  drive_enc_response.drive_enc_array == NULL) {
                                CRIT("No response from"
                                       " ov_rest_getdriveEnclosureInfoArray");
                                continue;
                        }
                        /* Parse the drive enclosure json response*/
                        ov_rest_json_parse_drive_enclosure(
                                          drive_enc_response.drive_enc_array,
                                          &drive_enc_info_result);
			ov_rest_wrap_json_object_put(
						drive_enc_response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        drive_enc_info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler, &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                       enc_response.enclosure_array == NULL) {
                                CRIT("Failed to get Enclosure Info Array");
                                continue;
                        }
                        ov_rest_json_parse_enclosure(
                                           enc_response.enclosure_array,
                                           &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the Drive Enclosure Resourceid by looking at
                         * the enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                CRIT("Enclosure data of the Drive Enclosure"
                                          " serial number %s is unavailable",
                                           info_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(drive_enc_doc);
                                continue;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->server.resource_id
                                        [drive_enc_info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the"
                                    " Drive Enclosure at bay number %d",
                                       drive_enc_info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(drive_enc_doc);
                                continue;
                        }
                }
                else if (!strcmp((event.phyResourceType), "interconnects")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event.resourceUri);
                        rv = ov_rest_getinterconnectInfoArray(handler,
                                                              &int_response,
                                      ov_handler->connection,interconnect_doc);
                        if (rv != SA_OK ||
                                  int_response.interconnect_array == NULL) {
                                CRIT("No response from"
                                        " ov_rest_getinterconnectInfoArray "
					" for interconnects");
                                continue;
                        }
                        ov_rest_json_parse_interconnect(
                                               int_response.interconnect_array,
                                               &int_info_result);
			ov_rest_wrap_json_object_put(int_response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        int_info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                       enc_response.enclosure_array == NULL) {
                                CRIT("Failed to get Enclosure Info Array"
					" from interconnect location");
                                continue;
                        }
                        ov_rest_json_parse_enclosure(
                                                 enc_response.enclosure_array,
                                                 &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the Interconnect Resourceid by looking at the
                         * enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(interconnect_doc);
                                continue;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->interconnect.resource_id
                                             [int_info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the "
                                        "Interconnect at bay number %d",
                                       int_info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_free(enc_doc);
                                wrap_free(interconnect_doc);
                                continue;
                        }
                }
                else {
                        dbg("The Active/Locked event for the %s not processed",
                                                 event.phyResourceType); 
                        continue;
                }

                memset(&oem_event, 0, sizeof(struct oh_event));
                memcpy(&(oem_event.resource), rpt, sizeof(SaHpiRptEntryT));
                oem_event.event.Source = oem_event.resource.ResourceId;
                oem_event.hid = handler->hid;
                oem_event.event.EventType = SAHPI_ET_OEM;
                /* Coverting event.created time from string to nanoseconds */
                struct tm tm;
                strptime(event.created, "%Y-%m-%dT%H:%M:%S.%NZ", &tm);
                time_t t = mktime(&tm);
                oem_event.event.Timestamp = (SaHpiTimeT)t * 1000000000;
                if (!strcmp(event.severity, "Critical"))
                        oem_event.event.Severity = SAHPI_CRITICAL;
                else if (!strcmp(event.severity, "Warning"))
                        oem_event.event.Severity = SAHPI_MAJOR;
                else
                        err("Unknown Event Severity %s", event.severity);
                oem_event.event.EventDataUnion.OemEvent.MId =
                          oem_event.resource.ResourceInfo.ManufacturerId;
                oem_event.event.EventDataUnion.OemEvent.OemEventData.DataType =
                                                      SAHPI_TL_TYPE_TEXT;
                oem_event.event.EventDataUnion.OemEvent.OemEventData.Language =
                                                      SAHPI_LANG_ENGLISH;
                if (strlen(event.description) > SAHPI_MAX_TEXT_BUFFER_LENGTH)
                        oem_event.event.EventDataUnion.OemEvent.OemEventData.
                           DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
                else
                        oem_event.event.EventDataUnion.OemEvent.OemEventData.
                           DataLength = strlen(event.description);
                snprintf((char *) oem_event.event.EventDataUnion.OemEvent.
                         OemEventData.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
                                                 "%s", event.description);
                oh_evt_queue_push (handler->eventq,
                               copy_ov_rest_event(&oem_event));
                /* Fix for #1932 */
                //err("Active/Locked alert found during discovery.");
                //err("Descripton: %s", event.description);
                //err("CorrectiveAction: %s", event.correctiveAction);
		oem_event_to_file(handler, &event, &oem_event);	
        }
        return SA_OK;
}

/*
 * oem_event_handler
 *      @handler       : Pointer to openhpi handler structure.
 *      @event          : Pointer to the eventInfo structure.
 *
 * Purpose:
 *      Handler for OEM Events/Alerts pushing them to event queue.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - On Success.
 */

SaErrorT oem_event_handler(struct oh_handler_state *handler,
                        struct eventInfo* event)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;
        struct serverhardwareInfoArrayResponse response = {0};
        struct serverhardwareInfo info_result = {0};
        struct driveEnclosureInfoArrayResponse drive_enc_response = {0};
        struct driveEnclosureInfo drive_enc_info_result = {0};
        struct interconnectInfoArrayResponse int_response = {0};
        struct interconnectInfo int_info_result = {0};
        struct enclosureInfoArrayResponse enc_response = {0};
        struct enclosureInfo enc_result = {{0}};
        struct enclosureStatus *enclosure = NULL;
        char *enc_doc = NULL, *server_doc = NULL, *interconnect_doc = NULL;
        char *drive_enc_doc = NULL;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event oem_event = {0};

        ov_handler = (struct ov_rest_handler *)handler->data;
        if(!strcmp(event->severity,"Critical")){
                if (!strcmp((event->phyResourceType), "enclosures")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event->resourceUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                                      ov_handler->connection,
                                                      enc_doc);
                        if (rv != SA_OK ||
                                  enc_response.enclosure_array == NULL) {
                                CRIT("No response from"
                                          " ov_rest_getenclosureInfoArray");
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
                                CRIT("Enclosure data of enclosure"
                                          " serial number %s is unavailable",
                                           enc_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
			/* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->enclosure_rid);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the Enclosure"
					" serial number %s is unavailable",
						enc_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                }
                else if (!strcmp((event->phyResourceType),"server-hardware")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event->resourceUri);
                        rv = ov_rest_getserverInfoArray(handler, &response,
                                      ov_handler->connection, server_doc);
                        if (rv != SA_OK || response.server_array == NULL) {
                                CRIT("No response from"
                                       " ov_rest_getserverInfoArray");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        ov_rest_json_parse_server (response.server_array,
                                                                &info_result);
			ov_rest_wrap_json_object_put(response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                  enc_response.enclosure_array == NULL) {
                                CRIT("No response from"
                                        " ov_rest_getenclosureInfoArray");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        ov_rest_json_parse_enclosure(
                                           enc_response.enclosure_array,
                                           &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the server Resourceid by looking at the
                         * enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                CRIT("Enclosure data of the Server"
                                          " serial number %s is unavailable",
                                           info_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(server_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->server.resource_id
                                                 [info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the server at"
                                        " bay number %d",
                                       info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(server_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                }
                else if (!strcmp((event->phyResourceType),"drive-enclosures")){
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event->resourceUri);
                        rv = ov_rest_getdriveEnclosureInfoArray(handler,
                                               &drive_enc_response,
                                            ov_handler->connection,
                                                    drive_enc_doc);
                        if (rv != SA_OK ||
                                  drive_enc_response.drive_enc_array == NULL) {
                                CRIT("No response from"
                                     " ov_rest_getdriveEnclosureInfoArray");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        /* Parse the drive enclosure json response*/
                        ov_rest_json_parse_drive_enclosure(
                                          drive_enc_response.drive_enc_array,
                                          &drive_enc_info_result);
			ov_rest_wrap_json_object_put(drive_enc_response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        drive_enc_info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                        enc_response.enclosure_array == NULL) {
                                CRIT("No response from"
                                        " ov_rest_getenclosureInfoArray");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        ov_rest_json_parse_enclosure(
                                           enc_response.enclosure_array,
                                           &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the Drive Enclosure Resourceid by looking at
                         * the enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                CRIT("Enclosure data of the Drive Enclosure"
                                          " serial number %s is unavailable",
                                           info_result.serialNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(drive_enc_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->server.resource_id
                                        [drive_enc_info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the"
                                    " Drive Enclosure at bay number %d",
                                       drive_enc_info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(drive_enc_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                }
                else if (!strcmp((event->phyResourceType), "interconnects")) {
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                      ov_handler->connection->hostname,
                                      event->resourceUri);
                        rv = ov_rest_getinterconnectInfoArray(handler,
                                                              &int_response,
                                      ov_handler->connection,interconnect_doc);
                        if (rv !=SA_OK ||
                                int_response.interconnect_array == NULL) {
                                CRIT("No response from"
                                        " ov_rest_getinterconnectInfoArray "
					"for interconnects");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        ov_rest_json_parse_interconnect(
                                               int_response.interconnect_array,
                                               &int_info_result);
			ov_rest_wrap_json_object_put(int_response.root_jobj);
                        /* Now we have to get the enclosure serial number*/
                        WRAP_ASPRINTF (&ov_handler->connection->url, "https://%s%s",
                                        ov_handler->connection->hostname,
                                        int_info_result.locationUri);
                        rv = ov_rest_getenclosureInfoArray(handler,
                                                           &enc_response,
                                        ov_handler->connection, enc_doc);
                        if (rv != SA_OK ||
                                        enc_response.enclosure_array == NULL) {
                                CRIT("No response from"
                                        " ov_rest_getenclosureInfoArray");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        ov_rest_json_parse_enclosure(
                                                 enc_response.enclosure_array,
                                                 &enc_result);
			ov_rest_wrap_json_object_put(enc_response.root_jobj);
                        /* Find the Interconnect Resourceid by looking at the
                         * enclosure linked list*/
                        /* FIXME : We could make below code as a funtion to get
                         * the resource id by using enclosure serial number */
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
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(interconnect_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        /* Get the rpt entry of the resource */
                        rpt = oh_get_resource_by_id(handler->rptcache,
                                        enclosure->interconnect.resource_id
                                             [int_info_result.bayNumber - 1]);
                        if (rpt == NULL) {
                                err("Resource RPT is NULL for the "
                                        "Interconnect at bay number %d",
                                       int_info_result.bayNumber);
                                dbg("Skipping the event");
                                wrap_g_free(enc_doc);
                                wrap_g_free(interconnect_doc);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                }
                else {
                        dbg("The Active/Locked event for the %s not processed",
                                                 event->phyResourceType);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                memset(&oem_event, 0, sizeof(struct oh_event));
                memcpy(&(oem_event.resource), rpt, sizeof(SaHpiRptEntryT));
                oem_event.event.Source = oem_event.resource.ResourceId;
                oem_event.hid = handler->hid;
                oem_event.event.EventType = SAHPI_ET_OEM;
                /* Coverting event.created time from string to nanoseconds */
                struct tm tm;
                strptime(event->created, "%Y-%m-%dT%H:%M:%S.%NZ", &tm);
                time_t t = mktime(&tm);
                oem_event.event.Timestamp = (SaHpiTimeT)t * 1000000000;
                if (!strcmp(event->severity, "Critical"))
                        oem_event.event.Severity = SAHPI_CRITICAL;
                else if (!strcmp(event->severity, "Warning"))
                        oem_event.event.Severity = SAHPI_MAJOR;
                else
                        err("Unknown Event Severity %s", event->severity);
                oem_event.event.EventDataUnion.OemEvent.MId =
                          oem_event.resource.ResourceInfo.ManufacturerId;
                oem_event.event.EventDataUnion.OemEvent.OemEventData.DataType =
                                                      SAHPI_TL_TYPE_TEXT;
                oem_event.event.EventDataUnion.OemEvent.OemEventData.Language =
                                                      SAHPI_LANG_ENGLISH;
                if (strlen(event->description) > SAHPI_MAX_TEXT_BUFFER_LENGTH)
                        oem_event.event.EventDataUnion.OemEvent.OemEventData.
                           DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
                else
                        oem_event.event.EventDataUnion.OemEvent.OemEventData.
                           DataLength = strlen(event->description);
                snprintf((char *) oem_event.event.EventDataUnion.OemEvent.
                         OemEventData.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
                                                 "%s", event->description);
                oh_evt_queue_push (handler->eventq,
                               copy_ov_rest_event(&oem_event));
                /* Fix for #1932 */
                //err("Active/Locked alert found during discovery.");
                //err("Descripton: %s", event->description);
                //err("CorrectiveAction: %s", event->correctiveAction);
                oem_event_to_file(handler, event, &oem_event);
        }
        return rv;
}

/**
 * ov_rest_proc_addptask
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Call the Task/Event Handler for resource add task, if taskState
 *      is "completed" and percentComplete is "100%".
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 **/
SaErrorT  ov_rest_proc_add_task( struct oh_handler_state *oh_handler,
                                                       struct eventInfo* event)
{
        if (oh_handler == NULL || event == NULL)
        {
                err ("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if(event->taskState == NULL){
		return SA_ERR_HPI_INVALID_PARAMS;
	}
        if ((!strcmp(event->taskState, "Completed")) &&
                                   (event->percentComplete == 100)) {
		if(!strcmp(event->resourceCategory, "server-hardware")) {
			ov_rest_proc_blade_add_complete(oh_handler, event);
			dbg("TASK_ADD_SERVER");
		} else if (!strcmp(event->resourceCategory, 
						"drive-enclosures")){
			ov_rest_proc_drive_enclosure_add_complete(oh_handler,
									event);
			dbg("TASK_ADD_DRIVE_ENCLOSURE");
		} else if (!strcmp(event->resourceCategory,"interconnects")) {
			ov_rest_proc_interconnect_add_complete(oh_handler,
								event);
			dbg("TASK_ADD_INTERCONNECT");
		} else if (!strcmp(event->resourceCategory,
				"sas-interconnects")) {
			ov_rest_proc_interconnect_add_complete(oh_handler,
								event);
			dbg("TASK_ADD_INTERCONNECT");
		} else if (!strcmp(event->resourceCategory, 
				"logical-interconnects")) {
			ov_rest_proc_interconnect_add_complete(oh_handler,
								event);
			dbg("TASK_ADD_INTERCONNECT");
		}else if(!strcmp(event->resourceCategory, "ha_node")){
			ov_rest_proc_composer_insertion_event(oh_handler,
								event);
			dbg("TASK_ADD_COMPOSER");
		} else {
			err("Unknown resourceCategory %s",
						event->resourceCategory);
		}
        }
        return SA_OK;
}

/**
 * ov_rest_proc_power_on_task
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Call the Task/Event Handler for resource power on task, if taskState
 *      is "completed" and percentComplete is "100%".
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 **/
SaErrorT  ov_rest_proc_power_on_task( struct oh_handler_state *oh_handler,
					struct eventInfo* event)
{
	if (oh_handler == NULL || event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	if ((!strcmp(event->taskState, "Completed")) &&
			(event->percentComplete == 100)) {
		if (!strcmp(event->resourceCategory, "drive-enclosures")){
			process_drive_enclosure_power_on_event(oh_handler,
					event);
			dbg("TASK_POWER_ON for DRIVE_ENCLOSURE");
		} else if (!strcmp(event->resourceCategory, "interconnects")){
			process_interconnect_power_on_task(oh_handler,
					event);
			dbg("TASK_POWER_ON for INTERCONNECT");
		} else
			warn("Not handling power on for %s category", 
                                                     event->resourceCategory);
	}
	return SA_OK;
}

/**
 * ov_rest_proc_power_off_task
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Call the Task/Event Handler for resource power off task, if taskState
 *      is "completed" and percentComplete is "100%".
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 **/
SaErrorT  ov_rest_proc_power_off_task( struct oh_handler_state *oh_handler,
                                                       struct eventInfo* event)
{
        if (oh_handler == NULL || event == NULL)
        {
                err ("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if ((!strcmp(event->taskState, "Completed")) &&
                                   (event->percentComplete == 100)) {
		if (!strcmp(event->resourceCategory, "drive-enclosures")){
			process_drive_enclosure_power_off_event(oh_handler,
								event);
			dbg("TASK_POWER_OFF for DRIVE_ENCLOSURE");
		} else if (!strcmp(event->resourceCategory, "interconnects")){
			process_interconnect_power_off_task(oh_handler,
					event);
			dbg("TASK_POWER_OFF for INTERCONNECT");
		} else 
			warn("Not handling power off for %s category", 
                                                     event->resourceCategory);

        }
        return SA_OK;
}

/**
 * ov_rest_proc_reset_task
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *      Call the Task/Event Handler for resource reset task, if taskState
 *      is "completed" and percentComplete is "100%".
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 **/
SaErrorT  ov_rest_proc_reset_task( struct oh_handler_state *oh_handler,
                                                       struct eventInfo* event)
{
        if (oh_handler == NULL || event == NULL)
        {
                err ("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if ((!strcmp(event->taskState, "Completed")) &&
                                   (event->percentComplete == 100)) {
		if (!strcmp(event->resourceCategory, "interconnects")){
			process_interconnect_reset_task(oh_handler, event);
			dbg("TASK_RESET for INTERCONNECT");
		} else 
			warn("Not handling reset task for %s category", 
                                                     event->resourceCategory);

        }
        return SA_OK;
}

/**
 * ov_rest_proc_activate_standby_composer
 *      @oh_handler: Pointer to openhpi handler structure
 *      @event:      Pointer to the eventInfo structure
 *
 * Purpose:
 *     Task/Event handler for the TASK_ACTIVATE_STANDBY_APPLIANCE
 *     when the task is 100% complete this function initiates the
 *     Re-Discovery of the resources.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 **/

SaErrorT ov_rest_proc_activate_standby_composer(
					struct oh_handler_state *oh_handler,
					struct eventInfo* event)
{
	SaErrorT rv = SA_OK;

	if (oh_handler == NULL || event == NULL)
	{
		err ("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	if ((!strcmp(event->taskState, "Completed")) &&
			(event->percentComplete == 100)) {

		rv = ov_rest_re_discover(oh_handler);
		if(rv != SA_OK){
			CRIT("Re-Discovery faild.");
			return rv;
		}

	}
	return SA_OK;
}

/**
 * ov_rest_setuplistner
 *      @handler   : Pointer to openhpi handler structure.
 *
 * Purpose:
 *      Create and download the AMQP client certificate, private key and
 *      root CA certificate.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_INTERNAL_ERROR - On Internal Error.
 *      SA_ERR_HPI_INVALID_DATA   - On Invalid Data.
 **/
SaErrorT ov_rest_setuplistner(struct oh_handler_state *handler)
{
	SaErrorT rv = SA_OK;
	struct certificateResponse response = {0};
	struct certificates result = {0};
	int SSLCert_len = 0, SSLKey_len = 0, ca_len = 0, num = 0;
	char * postfields = NULL, *temp = NULL;
	FILE* fp = NULL;
	struct ov_rest_handler *ov_handler = NULL;

	char* certificate_doc = NULL, *ca_doc = NULL;
	struct stat st = {0};

	ov_handler = (struct ov_rest_handler *)handler->data;

	if (stat(OV_REST_PATH, &st) == -1) {
		if(mkdir(OV_REST_PATH, 0600)== -1){
			CRIT("Failed to create the directory %s, %s", 
					OV_REST_PATH, strerror(errno));
		}
	}
	if (stat(CA_PATH, &st) == -1) {
		if(mkdir(CA_PATH, 0600)== -1){
			CRIT("Failed to create the directory %s, %s", 
					CA_PATH, strerror(errno));
		}
	}
	memset(&response,0, sizeof(struct certificateResponse));
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_CREATE_CERTIFICATE_URI,
					ov_handler->connection->hostname);
	WRAP_ASPRINTF(&postfields, OV_CERTIFICATE_REQUEST_POST);
	ov_rest_create_certificate(ov_handler->connection, postfields);

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_GET_CERTIFICATES_URI,
					ov_handler->connection->hostname);
	rv = ov_rest_getcertificates(NULL, &response, ov_handler->connection,
							certificate_doc);
	if (rv != SA_OK || response.certificate == NULL) {
		CRIT("No response from ov_rest_getcertificates");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_certificate(response.certificate,&result);
	if(chdir(CA_PATH) == -1){
		CRIT("Failed to change the dir to %s, %s", CA_PATH, 
						strerror(errno));
	}
	temp = (char *)g_hash_table_lookup(handler->config, "entity_root");
	sscanf(temp, "%*[^0-9]%d", &num);
	if (num >= 100) {
		CRIT("Out of array size %d", num);
		return SA_ERR_HPI_INVALID_DATA;
	}

	memset(ov_handler->cert_t.fSslCert, 0, 
					sizeof(ov_handler->cert_t.fSslCert));
	sprintf(ov_handler->cert_t.fSslCert, "%s%s%d%s", "SSLCert", "_",
								num, ".pem");
	fp = fopen(ov_handler->cert_t.fSslCert,"w+");
	if(fp == NULL)
	{
		CRIT("Error opening the file %s", ov_handler->cert_t.fSslCert);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
        SSLCert_len = strlen(result.SSLCert);
	if(SSLCert_len != fwrite(result.SSLCert, sizeof(char),SSLCert_len, fp))
	{
		CRIT("Error in Writing the file %s",
						ov_handler->cert_t.fSslCert);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	fclose(fp);

	memset(ov_handler->cert_t.fSslKey, 0, 
					sizeof(ov_handler->cert_t.fSslKey));
	sprintf(ov_handler->cert_t.fSslKey, "%s%s%d%s", "SSLKey", "_",
								num, ".pem");
	fp = fopen(ov_handler->cert_t.fSslKey,"w+");
	if(fp == NULL)
        {
                CRIT("Error opening the file %s", ov_handler->cert_t.fSslKey);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	SSLKey_len = strlen(result.SSLKey);
	if(SSLKey_len != fwrite(result.SSLKey, sizeof(char), SSLKey_len, fp))
	{
		CRIT("Error in Writing the file %s",
						ov_handler->cert_t.fSslKey);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	fclose(fp);
	ov_rest_wrap_json_object_put(response.root_jobj);

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_GET_CA_URI,
                                       ov_handler->connection->hostname);
	rv = ov_rest_getca(NULL, &response, ov_handler->connection, ca_doc);
	if (rv != SA_OK || response.certificate == NULL) {
		CRIT("No response from ov_rest_getcertificates");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_ca(response.certificate, &result);
	memset(ov_handler->cert_t.fCaRoot, 0, 
					sizeof(ov_handler->cert_t.fCaRoot));
	sprintf(ov_handler->cert_t.fCaRoot, "%s%s%d%s", "caroot", "_",
								num, ".pem");
	fp = fopen(ov_handler->cert_t.fCaRoot,"w+");
	if(fp == NULL)
        {
                CRIT("Error opening the file %s", ov_handler->cert_t.fCaRoot);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
	ca_len = strlen(result.ca);
	if(ca_len != fwrite(result.ca, sizeof(char), ca_len, fp))
	{
		dbg("ca_len = %d",ca_len);
		CRIT("Error in Writing the file %s",
						ov_handler->cert_t.fCaRoot);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	fclose(fp);
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * ov_die_on_amqp_error
 *      @x      : Variable to amqp_rpc_reply_t.
 *      @context: Ponter to character constant.
 *
 * Description:
 *      This function handles the AMQP response based on the return type.
 *
 * Returns:
 *      None.
 **/
void ov_die_on_amqp_error(amqp_rpc_reply_t x, char const *context)
{
  switch (x.reply_type) {
  case AMQP_RESPONSE_NORMAL:
    return;

  case AMQP_RESPONSE_NONE:
    err("%s: missing RPC reply type!", context);
    break;

  case AMQP_RESPONSE_LIBRARY_EXCEPTION:
    err("%s: %s", context, amqp_error_string2(x.library_error));
    break;

  case AMQP_RESPONSE_SERVER_EXCEPTION:
    switch (x.reply.id) {
    case AMQP_CONNECTION_CLOSE_METHOD: {
      amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
      err("%s: server connection error %d, message: %.*s",
              context,
              m->reply_code,
              (int) m->reply_text.len, (char *) m->reply_text.bytes);
      break;
    }
    case AMQP_CHANNEL_CLOSE_METHOD: {
      amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
      err("%s: server channel error %d, message: %.*s",
              context,
              m->reply_code,
              (int) m->reply_text.len, (char *) m->reply_text.bytes);
      break;
    }
    default:
      err("%s: unknown server error, method id 0x%08X", context, x.reply.id);
      break;
    }
    break;
  }

}

/** ov_rest_scmb_listner 
 * @handler: Pointer to the oh_handler_state structure.
 *
 * Purpose:
 *      establish the connection with SCMB to trive the alerts/tasks
 *      in to single queue.And then these messages will get parsed and
 *      appropriate handler will be called to handle the alert/task.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 *     	SA_ERR_HPI_OUT_OF_MEMORY  - on Out of memory
 **/
SaErrorT ov_rest_scmb_listner(struct oh_handler_state *handler)
{
	struct ov_rest_handler *ov_handler = NULL;
	json_object *jobj = NULL, *scmb_resource = NULL;
	int status = 0;
	char *messages = NULL;
	amqp_socket_t *socket = NULL;
	amqp_connection_state_t conn = {0};
	amqp_bytes_t queuename;
	struct timeval timeout = {0};
	SaErrorT rv = SA_OK;

	ov_handler = (struct ov_rest_handler *)handler->data;
	conn = amqp_new_connection();
	if(!conn){
		err("Error creating connection");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	socket = amqp_ssl_socket_new(conn);
	if (!socket) {
		err("Error in creating SSL/TLS socket");
		amqp_destroy_connection(conn);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	if(chdir(CA_PATH) == -1){
		CRIT("Failed to change the dir to %s, %s", CA_PATH, 
						strerror(errno));
	}
	status = amqp_ssl_socket_set_cacert(socket,ov_handler->cert_t.fCaRoot);
	if (status) {
		err("Error in setting CA certificate");
		amqp_destroy_connection(conn);
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	/* Path to client.pem, key.pem */
	status = amqp_ssl_socket_set_key(socket, ov_handler->cert_t.fSslCert,
			ov_handler->cert_t.fSslKey); 
	if (status) {
		err("Error in setting client cert");
		amqp_destroy_connection(conn);
		return SA_ERR_HPI_ERROR;
	}
	/* Set port to 5671 and hostname from handler */
	status = amqp_socket_open(socket, 
			ov_handler->connection->hostname, AMQP_PORT); 
	if (status) {
		err("Error in opening SSL/TLS connection");
		amqp_destroy_connection(conn);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	ov_die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 3, 
				AMQP_SASL_METHOD_EXTERNAL,
				AMQP_EXTERNAL_USER, 
				AMQP_EXTERNAL_PASSWORD), "Logging in");
	amqp_channel_open(conn, 1);
	ov_die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");
	{
		amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, 
				amqp_empty_bytes, 
				0, 0, 0, 1,
				amqp_empty_table);
		ov_die_on_amqp_error(amqp_get_rpc_reply(conn), 
				"Declaring queue");
		queuename = amqp_bytes_malloc_dup(r->queue);
		if (queuename.bytes == NULL) {
			err("Out of memory while copying queue name");
			amqp_destroy_connection(conn);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
	}
	amqp_queue_bind(conn, 1, queuename, 
			amqp_cstring_bytes(AMQP_EXCHANGE), 
			amqp_cstring_bytes(AMQP_ALERTS_BINDINGKEY),
			amqp_empty_table);
	amqp_queue_bind(conn,1,	queuename, 
			amqp_cstring_bytes(AMQP_EXCHANGE), 
			amqp_cstring_bytes(AMQP_TASKS_BINDINGKEY),
			amqp_empty_table);
	ov_die_on_amqp_error(amqp_get_rpc_reply(conn), "Binding queue");
	amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, 
			amqp_empty_table);
	amqp_get_rpc_reply(conn);
	while (1) {
		amqp_rpc_reply_t res = {0};
		amqp_envelope_t envelope = {0};
		amqp_maybe_release_buffers(conn);
		OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);

		/* FIXME: res */
		timeout.tv_sec = AMQP_CONSUME_TIMEOUT_SEC;
		timeout.tv_usec = AMQP_CONSUME_TIMEOUT_USEC;
		res = amqp_consume_message(conn, &envelope, &timeout, 0);
		switch(res.reply_type){
			case AMQP_RESPONSE_NORMAL:
				dbg("AMQP_RESPONSE_NORMAL");
				break;
			case AMQP_RESPONSE_SERVER_EXCEPTION:
				err("AMQP_RESPONSE_SERVER_EXCEPTION");
				continue;
			case AMQP_RESPONSE_LIBRARY_EXCEPTION:
				rv = ov_rest_amqp_err_handling(handler, 
						res.library_error);
				if(rv != SA_OK){
					amqp_bytes_free(queuename);
					amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
					amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
					amqp_destroy_connection(conn);
					return rv;
				}
				continue;
			default:
				err("Unknown AMQP response %d",res.reply_type);
				break;
		}

		dbg("Delivery %u, exchange %.*s routingkey %.*s",
				(unsigned) envelope.delivery_tag,
				(int) envelope.exchange.len, 
				(char *) envelope.exchange.bytes,
				(int) envelope.routing_key.len, 
				(char *) envelope.routing_key.bytes);

		if (envelope.message.properties._flags & 
				AMQP_BASIC_CONTENT_TYPE_FLAG) {
			dbg("Content-type: %.*s",
					(int) envelope.message.
					properties.content_type.len,
					(char *) envelope.message.
					properties.content_type.bytes);
		}
		messages = (char *)g_malloc0( envelope.message.body.len + 1);
		memcpy(messages, (char *)envelope.message.body.bytes, 
				envelope.message.body.len);
		jobj = json_tokener_parse(messages);
		scmb_resource = ov_rest_wrap_json_object_object_get(jobj, 
				"resource");
		process_ov_events(handler, scmb_resource);
		ov_rest_wrap_json_object_put(jobj);
		wrap_g_free(messages);
		amqp_destroy_envelope(&envelope);
	}
	amqp_bytes_free(queuename);
	amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
	amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(conn);

	return SA_OK;
}

/**
 * ov_rest_amqp_err_handling
 * 	@handler: Pointer to the oh_handler_state structure.
 * 	@res:	Ponter to amqp_rpc_reply_t
 * Description:
 *	This function handles the library exception by calling 
 * appropriate funtions in Switch statement.
 *
 * Returns:
 *
 * SA_OK			- On Success
 * SA_ERR_HPI_OUT_OF_MEMORY	- On Out of Memory
 * SA_ERR_HPI_INVALID_DATA	- On Invalid Data
 * SA_ERR_HPI_ERROR		- On Error
 * SA_ERR_HPI_INVALID_PARAMS	- On Invalid Parameters
 * SA_ERR_HPI_UNSUPPORTED_API	- On Unsupported API
 * SA_ERR_HPI_INTERNAL_ERROR	- On Inernal Error
 * SA_ERR_HPI_INVALID_STATE	- On Invalid state
 *
 **/
SaErrorT ov_rest_amqp_err_handling(struct oh_handler_state *handler, 
				int library_error)
{
	switch(library_error){

		case AMQP_STATUS_OK: 
					dbg("AMQP_STATUS_OK");
					return SA_OK; 
		case AMQP_STATUS_NO_MEMORY: 
					err("AMQP_STATUS_NO_MEMORY");
					return SA_ERR_HPI_OUT_OF_MEMORY;
		case AMQP_STATUS_BAD_AMQP_DATA: 
					err("AMQP_STATUS_BAD_AMQP_DATA");
					return SA_ERR_HPI_INVALID_DATA;
		case AMQP_STATUS_UNKNOWN_CLASS: 
					err("AMQP_STATUS_UNKNOWN_CLASS");
					return SA_ERR_HPI_UNKNOWN;
		case AMQP_STATUS_UNKNOWN_METHOD:
					err("AMQP_STATUS_UNKNOWN_METHOD");
					return SA_ERR_HPI_UNKNOWN;
		case AMQP_STATUS_HOSTNAME_RESOLUTION_FAILED:
					err("AMQP_STATUS_HOSTNAME_" 
							"RESOLUTION_FAILED");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_INCOMPATIBLE_AMQP_VERSION:
					err("AMQP_STATUS_INCOMPATIBLE_"
							"AMQP_VERSION");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_CONNECTION_CLOSED:
					err("AMQP_STATUS_CONNECTION_CLOSED");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_BAD_URL:
					err("AMQP_STATUS_BAD_URL");
					return SA_ERR_HPI_INVALID_DATA;
						
		case AMQP_STATUS_SOCKET_ERROR:
					err("AMQP_STATUS_SOCKET_ERROR");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_INVALID_PARAMETER: 
					err("AMQP_STATUS_INVALID_PARAMETER");
					return SA_ERR_HPI_INVALID_PARAMS;
		case AMQP_STATUS_TABLE_TOO_BIG:
					err("AMQP_STATUS_TABLE_TOO_BIG");
					return SA_ERR_HPI_INVALID_DATA;
		case AMQP_STATUS_WRONG_METHOD: 
					err("AMQP_STATUS_WRONG_METHOD");
					return SA_ERR_HPI_UNSUPPORTED_API;
		case AMQP_STATUS_TIMEOUT:
					/* This error is expected when no 
 					 * alerts received before timeout 
 					 * expires, So its not a critical  
 					 * error setting it to debug level. 
 					 */
					dbg("AMQP_STATUS_TIMEOUT");
					return SA_OK;
		case AMQP_STATUS_TIMER_FAILURE:
					err("AMQP_STATUS_TIMER_FAILURE");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_HEARTBEAT_TIMEOUT:
					err("AMQP_STATUS_HEARTBEAT_TIMEOUT");
					return SA_ERR_HPI_TIMEOUT;
		case AMQP_STATUS_UNEXPECTED_STATE:
					err("AMQP_STATUS_UNEXPECTED_STATE");
					return SA_ERR_HPI_INVALID_STATE;
		case AMQP_STATUS_SOCKET_CLOSED:
					err("AMQP_STATUS_SOCKET_CLOSED");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_SOCKET_INUSE:
					err("AMQP_STATUS_SOCKET_INUSE");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_BROKER_UNSUPPORTED_SASL_METHOD:
					err("AMQP_STATUS_BROKER_UNSUPPORTED_"
								"SASL_METHOD");
					return SA_ERR_HPI_UNSUPPORTED_API;
		case AMQP_STATUS_UNSUPPORTED:
					err("AMQP_STATUS_UNSUPPORTED");
					return SA_ERR_HPI_UNSUPPORTED_PARAMS;
		case _AMQP_STATUS_NEXT_VALUE:
					err("_AMQP_STATUS_NEXT_VALUE");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_TCP_ERROR:
					err("AMQP_STATUS_TCP_ERROR");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_TCP_SOCKETLIB_INIT_ERROR:
					err("AMQP_STATUS_TCP_SOCKETLIB_"
								"INIT_ERROR");
					return SA_ERR_HPI_ERROR;
		case _AMQP_STATUS_TCP_NEXT_VALUE:
					err("_AMQP_STATUS_TCP_NEXT_VALUE");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_SSL_ERROR:
					err("AMQP_STATUS_SSL_ERROR");
					return SA_ERR_HPI_ERROR;
		case AMQP_STATUS_SSL_HOSTNAME_VERIFY_FAILED:
					err("AMQP_STATUS_SSL_HOSTNAME_"
							"VERIFY_FAILED");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_SSL_PEER_VERIFY_FAILED:
					err("AMQP_STATUS_SSL_PEER_"
							"VERIFY_FAILED");
					return SA_ERR_HPI_INTERNAL_ERROR;
		case AMQP_STATUS_SSL_CONNECTION_FAILED:
					err("AMQP_STATUS_SSL_CONNECTION_"
								"FAILED");
					return SA_ERR_HPI_ERROR;
		case _AMQP_STATUS_SSL_NEXT_VALUE:
					err("_AMQP_STATUS_SSL_NEXT_VALUE");
					return SA_ERR_HPI_INTERNAL_ERROR;
		default:				
			break; 
	}

	return SA_OK;
}

/**
 * ov_rest_re_discover
 * 	@handler: Pointer to the oh_handler_state structure.
 * Description:
 * 	This function removes all existing RPT entries and re builds it by 
 * 	re-discovering all the resources, to recover from the connection 
 * 	failure etc..
 * Returns:
 *	SA_OK  - On Success.
 **/
SaErrorT ov_rest_re_discover(struct oh_handler_state *handler)
{
	SaErrorT rv = SA_OK;
	SaHpiBoolT is_ov_accessible = SAHPI_FALSE;
	struct ov_rest_handler *ov_handler = NULL;

	if (handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) handler->data;

	/* This loop ends when the One View is accessible */
	while (is_ov_accessible == SAHPI_FALSE) {
		OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);
		/* SCMB may not be reachable, try to establish the
		 * connection
		 */
		rv = ov_rest_connection_init(handler);
		if (rv != SA_OK) {
			/* waiting for the network connection to restore*/
			err("Please check whether the Synergy Composer"
							" is accessible");
			sleep(4);
			continue;
		} else {
			rv = ov_rest_setuplistner(handler);
			if(rv != SA_OK){
				/* waiting for the network connection to restore*/
				err("Please check whether the Synergy Composer"
							" is accessible");
				sleep(4);
				continue;
			} 
		}

		is_ov_accessible = SAHPI_TRUE;
		/* Synergy Composer is Accessible. Clean up the Old RPT cache,
 		*  discover all the resources and build the RPT cache */
		wrap_g_mutex_lock(ov_handler->mutex);
		rv = ov_rest_re_discover_resources(handler);
		if (rv != SA_OK) {
			is_ov_accessible = SAHPI_FALSE;
			err("Re-discovery failed ");
			wrap_g_mutex_unlock(ov_handler->mutex);
			/* waiting for the network connection to restore*/
			sleep(4);   
		}
	}
	wrap_g_mutex_unlock(ov_handler->mutex);
	return SA_OK;
}

/**
 * event_thread
 *      @ov_pointer: Pointer to the oh_handler_state structure for this thread.
 *
 * Purpose:
 *      Gets the event from the OV.
 *      Processes the OV event and pushes the event to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      (gpointer *) SA_OK                     - on success.
 *      (gpointer *) SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      (gpointer *) SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
gpointer ov_rest_event_thread(gpointer ov_pointer)
{
	struct oh_handler_state *handler = NULL;
	int ret_code = SA_ERR_HPI_INVALID_PARAMS;
	struct ov_rest_handler *ov_handler = NULL;
	SaHpiBoolT is_plugin_initialized = SAHPI_FALSE;
	SaHpiBoolT is_discovery_completed = SAHPI_FALSE;
	struct applianceNodeInfoResponse response = {0};
	SaErrorT rv = SA_OK;
	FILE *oemfile = NULL;
	struct eventArrayResponse event_response = {0};
	char* getallevents_doc = NULL, *temp = NULL, *oem_file_path = NULL;
	int num = 0;

	if (ov_pointer == NULL) {
		err("Invalid parameter");
		g_thread_exit(&ret_code);
	}

	/* Extract oh_handler and ov_rest_handler structure from ov_pointer */
	handler = (struct oh_handler_state *)ov_pointer;
	ov_handler = (struct ov_rest_handler *)handler->data;

	/* Check whether the plugin is initialized.
	 * If not, wait till plugin gets initialized
	 */
	while (is_plugin_initialized == SAHPI_FALSE) {
		OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);
		wrap_g_mutex_lock(ov_handler->ov_mutex);
		if (ov_handler->status == PRE_DISCOVERY ||
				ov_handler->status == DISCOVERY_COMPLETED) {
			wrap_g_mutex_unlock(ov_handler->ov_mutex);
			is_plugin_initialized = SAHPI_TRUE;
		} else {
			wrap_g_mutex_unlock(ov_handler->ov_mutex);
			dbg("Waiting for the plugin initialization "
					"to complete.");
			sleep(2);
		}
	}

	/* Check whether the discovery is over.
	 * If not, wait till discovery gets completed
	 */
	while (is_discovery_completed == SAHPI_FALSE) {
		OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);
		wrap_g_mutex_lock(ov_handler->ov_mutex);
		if (ov_handler->status == DISCOVERY_COMPLETED) {
			wrap_g_mutex_unlock(ov_handler->ov_mutex);
			is_discovery_completed = SAHPI_TRUE;
		} else {
			wrap_g_mutex_unlock(ov_handler->ov_mutex);
			dbg("Waiting for the discovery to complete.");
			sleep(2);
		}
	}

	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);
	ov_rest_setuplistner(handler);
	
	/* if the file is already exist, then flush all of its contents by 
 	 * opening in "w" or "w+" mode */
	temp = (char *)g_hash_table_lookup(handler->config, "entity_root");
	sscanf(temp, "%*[^0-9]%d", &num);
	WRAP_ASPRINTF(&oem_file_path, "%s/%s%s%d%s",
                        OV_REST_PATH, "oem_event", "_", num, ".log");
	oemfile = fopen(oem_file_path,"w");
	if(oemfile == NULL)
	{
		CRIT("Error opening OEM_EVENT_FILE file: %s",oem_file_path);
		wrap_free(oem_file_path);
		return (gpointer* )SA_ERR_HPI_ERROR;
	}
	wrap_free(oem_file_path);
	oem_file_path = NULL;
	fclose(oemfile);

	/** Handling Active alerts dring discovery **/
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ACTIVE_ALERTS,
				ov_handler->connection->hostname, "-1");
	rv = ov_rest_getActiveLockedEventArray(ov_handler->connection,
							&event_response);
	if(rv == SA_OK){
		WRAP_ASPRINTF(&ov_handler->connection->url, OV_ACTIVE_ALERTS,
				ov_handler->connection->hostname,
				event_response.total);
		ov_rest_wrap_json_object_put(event_response.root_jobj);
	}
	rv = ov_rest_getActiveLockedEventArray(ov_handler->connection,
						&event_response);
	if(rv == SA_OK){
		process_active_and_locked_alerts(handler, &event_response);
		err("Active alerts are found and events are added "
						"to logs/oem event file.");
		err("Please login to the composer to get complete details.");
		ov_rest_wrap_json_object_put(event_response.root_jobj);
	}
	
	/** Handling Locked alerts dring discovery **/
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_LOCKED_ALERTS,
				ov_handler->connection->hostname, "-1");
	rv = ov_rest_getActiveLockedEventArray(ov_handler->connection,
							&event_response);
	if(rv == SA_OK){
		WRAP_ASPRINTF(&ov_handler->connection->url, OV_LOCKED_ALERTS,
				ov_handler->connection->hostname,
				event_response.total);
	}
	ov_rest_getActiveLockedEventArray(ov_handler->connection,
							&event_response);
	if(rv == SA_OK){
		process_active_and_locked_alerts(handler, &event_response);
		err("Locked alerts are found and events are added "
						"to logs/oem event file.");
		err("Please login to the composer to get complete details.");
	}

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ALERTS, 
			ov_handler->connection->hostname);	
	ov_rest_getAllEvents(&event_response, ov_handler->connection, 
			getallevents_doc);
	ov_rest_wrap_json_object_put(event_response.root_jobj);
	wrap_free(ov_handler->connection->url);
	/* Listen for the events from OneView Synergy SCMB messages */
	while(1){
		OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);
		rv = ov_rest_scmb_listner(handler);
		if(rv != SA_OK){
			sleep(5);
			/* Check if session id is valid. if session id valid, 
			 * then go for next loop.
			 * if sesion id is not valid then go for re-discovery.
			 * */
			WRAP_ASPRINTF(&ov_handler->connection->url,
					OV_APPLIANCE_VERSION_URI,
					ov_handler->connection->hostname);
			rv = ov_rest_getapplianceNodeInfo(
					handler, &response, 
					ov_handler->connection);
			ov_rest_wrap_json_object_put(response.root_jobj);
			if(rv == SA_OK){
				err("Composer is Accessible, "
						"SCMB is not working");
				continue;
			} else {

				ov_rest_re_discover(handler);
			}

		}
	}
	return (gpointer *) SA_OK;
}

/**
 * process_ov_events
 *      @oh_handler: Pointer to the openhpi handler structure
 *      @scmb_resource:  json_object Pointer to "resource" object inside
 *      json response message from scmb listen.
 *
 * Purpose:
 *      Get the event and call alerts/tasks function based on event category.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      NONE - void return, as this function processes the events
 **/
void process_ov_events(struct oh_handler_state *oh_handler,
                       json_object *scmb_resource)
{
	struct eventInfo event = {0};
	struct ov_rest_handler *ov_handler = NULL;

	if (scmb_resource == NULL || oh_handler == NULL) {
		err("Invalid parameter");
		return;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;	

	OV_REST_CHEK_SHUTDOWN_REQ(ov_handler, NULL, NULL, NULL);

	ov_rest_json_parse_events(scmb_resource, &event);
        if (!strcmp(event.category, "alerts")) {
		ov_rest_process_alerts(oh_handler,scmb_resource,&event);
	}
        else if (!strcmp(event.category, "tasks")) {
		ov_rest_process_tasks(oh_handler,scmb_resource,&event);
	} else {
                err("Unexpected event category %s",event.category);
	}

	return;
}

/**
 * ov_rest_process_alerts
 * 	@oh_handler: Pointer to the openhpi handler structure
 *      @scmb_resource:  json_object Pointer to "resource" object inside
 *	@event: Pointer to the event structure
 *
 * Purpose:
 *      Process the alerts by calling appropriate functions
 *
 * Detailed Description:
 *	This function is called by process_ov_events for processing
 *	alerts. 
 *
 * Return values:
 *      NONE - void return, as this function processes the alerts
 **/
void ov_rest_process_alerts(struct oh_handler_state *oh_handler,
		json_object * scmb_resource, struct eventInfo* event)
{

	ov_rest_json_parse_alerts(scmb_resource, event);
	dbg("%s alert received mapped to alertTypeId=%d", event->alert_name, 
			event->alertTypeId);
	switch (event->alertTypeId) {
		case cpqRackServerBladeRemoved2:
		case BladeRemoved:
			ov_rest_proc_blade_removed(oh_handler,event);
			dbg("EVENT_BLADE_REMOVED");
			break;
		case cpqRackNetConnectorRemoved:
		case InterconnectRemoved:
			dbg("EVENT_INTERCONNECT_REMOVED");
			ov_rest_proc_interconnect_removed(oh_handler,
					event);
			break;
		case PowerSupplyInserted:
			process_powersupply_insertion_event(oh_handler,
					event);
			dbg("PowerSupplyInserted");
			break;
		case PowerSupplyRemoved:
			process_powersupply_removed_event(oh_handler,
					event);
			dbg("PowerSupplyRemoved");
			break;
		case FanInserted:
			process_fan_inserted_event(oh_handler,
					event);
			dbg("FanInserted");
			break;
		case FanRemoved:
			process_fan_removed_event(oh_handler,
					event);
			dbg("FanRemoved");
			break;
		case cpqSm2ServerPowerOn:
			process_server_power_on_event(oh_handler,
					event);
			dbg("ServerPowerOn");
			break;
		case cpqSm2ServerPowerOff:
			process_server_power_off_event(oh_handler,
					event);
			dbg("ServerPowerOff");
			break;
		case cpqSm2ServerReset:
			process_server_reset_event(oh_handler,
					event);
			dbg("ServerReset");
			break;
		case switchresponse:
			//ov_rest_proc_switch_status_change(oh_handler,
			//		event);
			dbg("Switch/Int. Response status Change");
			break;
		case HealthStatusArrayCategoryStatus:
			ov_rest_proc_server_status(oh_handler, event);
			dbg("HealthStatusArrayCategoryStatus");
			break;
		case connectionStateChange:
		case interconnectStateChange:
			ov_rest_proc_int_status(oh_handler, event);
			dbg("%s", event->alert_name);
			break;
		case CIManagerOk:
			ov_rest_proc_composer_status(oh_handler, event,
					OK);
			dbg("CIManagerOk");
			break;
		case CIManagerWarning:
			ov_rest_proc_composer_status(oh_handler, event,
					Warning);
			dbg("CIManagerWarning");
			break;
		case CIManagerCritical:
			ov_rest_proc_composer_status(oh_handler, event,
					Critical);
			dbg("CIManagerCritical");
			break;
		case CIManagerInserted:
			ov_rest_proc_composer_insertion_event(oh_handler,event);
			dbg("CIManagerInserted");
			break;
		case CIManagerRemoved:
			ov_rest_proc_composer_removed_event(oh_handler, event);
			dbg("CIManagerRemoved");
			break;
		case CommunicationError:
			ov_rest_proc_interconnect_fault(oh_handler,
					event);
			dbg("%s", event->alert_name);
			break;
		case InterconnectPowerRequestGranted:
		case InterconnectManagerICMReadyFault:
		case InterconnectManagerICMReadyFaultCleared:
		case BladeManagerIloInternalCritical:
		case BladeManagerIloInternalCriticalCleared:
		case BladeManagerIloInternalWarning:
		case BladeManagerIloInternalWarningCleared:
		case EmIsActive:
		case PowerSupplyInputLossFault:
		case invalidTopology:
		case neighbourIcPoweredOff16:
		case neighbourIcPoweredOff46:
		case cxpPortConnected:
		case cxpPortMisCabled:
		case cxpPortNotConnected:
		case CxpPortNotConnectedToExpectedICMKind:
		case failedCableRead:
		case flexFabric40GbEModuleAtSideANotInFirstEnclosure:
		case flexFabric40GbEModuleAtSideBNotInFirstOrSecondEnclosure:
		case icNotinSameBaySet:
		case InvalidInterconnectKind:
		case mixedIcInSameBaySet:
		case mixedIcLnSameBaySet:
		case moreThanOneflexFabric40GbEModuleAtOneSideofBaySet:
		case neighbourIcPoweredOff:
		case noflexFabric40GbEModuleAtSideA:
		case nonHpeCableInserted:
		case PortStatusLinked:
		case PortStatusLinkedInvalidPort:
		case PortStatusUnlinked:	
		case unsupportedlc:
		case PortStatusLinked1:
		case PortStatusLinked2:
		case PortStatusLinked4:
		case PortStatusUnlinked1:
		case PortStatusUnlinked2:
		case PortStatusUnlinked4:
		case opFcoeActiveChange:
		case ilt:
		case PowerSupplyInternalCommFault:
		case PowerSupplyInternalCommFaultCleared:
                case FanElectronicFuseBlownCleared:
                case ActivateStagedFirmwareSuccessful:
                case BladeCommFault:
                case BladeCommFaultCleared:
                case BladeFruFault:
                case BladeIloOperationalStatusCritical:
                case BladeInsertFault:
                case BladeInsertFaultCleared:
                case BladeManagerICMReadyFault:
                case BladeManagerICMReadyFaultCleared:
                case BladePowerOn:
                case ChassisAmbientThermal:
                case ChassisAmbientThermalCleared:
                case ChassisUidBlinking:
                case CIManagerCommFault:
                case CIManagerCommFaultCleared:
                case CIManagerEfusePGoodFault:
                case CIManagerEfusePGoodFaultCleared:
                case CIManagerFruFault:
                case CIManagerIloFault:
                case CIManagerIloFaultCleared:
                case CIManagerIloInternalCritical:
                case CIManagerIloInternalCriticalCleared:
                case CIManagerIloInternalWarning:
                case CIManagerIloInternalWarningCleared:
                case CIManagerInsertFault:
                case CIManagerInsertFaultCleared:
                case EmCommFault:
                case EmCommFaultCleared:
                case EmDataReplicationFault:
                case EmDataReplicationFaultCleared:
                case EmFruFault:
                case EmInsertFault:
                case EmInsertFaultCleared:
                case EmOneViewLinkValidationPingAddressMismatch:
                case EmOneViewLinkValidationPingAddressMismatch_Cleared:
                case EmOneViewLinkValidationPingFailed:
                case EmOneViewLinkValidationPingFailed_Cleared:
                case EmOneViewLinkValidationPingFailedCleared_PingAddressChanged:
                case EmOneViewLinkValidationPingFailedCleared_PingDisabled:
                case EmStagedFirmwareUpdated:
                case EmSwitchHardwareCritical:
                case EmSwitchHardwareOk:
                case FanCommFault:
                case FanCommFaultCleared:
                case FanEfusePGoodFault:
                case FanEfusePGoodFaultCleared:
                case FanFruFault:
                case FanInsertFault:
                case FanInsertFaultCleared:
                case FanRotor1Fault:
                case FanRotor1FaultCleared:
                case FanRotor2Fault:
                case FanRotor2FaultCleared:
                case FrontPanelCommFault:
                case FrontPanelCommFaultCleared:
                case FrontPanelFruFault:
                case FrontPanelInserted:
                case FrontPanelInsertFault:
                case FrontPanelInsertFaultCleared:
                case InterconnectCommFault:
                case InterconnectCommFaultCleared:
                case InterconnectFruFault:
                case InterconnectInsertFault:
                case InterconnectInsertFaultCleared:
                case InterconnectPowerOff:
                case InterconnectPowerOn:
                case MidplaneChassisIdUpdated:
                case MidplaneFruFault:
                case PowerAllocationCollectionChanged:
                case PowerAllocationDeleted:
                case PowerAllocationPatched:
                case PowerAllocationPosted:
                case PowerSubsystemOverLimit:
                case PowerSubsystemOverLimitCleared:
                case PowerSupplyCommFault:
                case PowerSupplyCommFaultCleared:
                case PowerSupplyFruFault:
                case PowerSupplyFruManufacturedForInvalidFault:
                case PowerSupplyInputFault:
                case PowerSupplyInsertFault:
                case PowerSupplyInsertFaultCleared:
                case PowerSupplyInterposerCommFault:
                case PowerSupplyInterposerCommFaultCleared:
                case PowerSupplyInterposerCritical:
                case PowerSupplyInterposerDiscoveryComplete:
                case PowerSupplyInterposerFruContentFault:
                case PowerSupplyInterposerFruFault:
                case PowerSupplyInterposerFruManufacturedForInvalidFault:
                case PowerSupplyInterposerFruManufacturedForMismatchFault:
                case PowerSupplyInterposerInserted:
                case PowerSupplyInterposerInsertFault:
                case PowerSupplyInterposerInsertFaultCleared:
                case PowerSupplyInterposerOk:
                case PowerSupplyInterposerPSCommFault:
                case PowerSupplyInterposerPSCommFaultCleared:
                case PowerSupplyInterposerRemoved:
                case PowerSupplyInterposerWarning:
                case PowerSupplyIpduInfoChanged:
                case PowerSupplyOvertempFault:
                case PowerSupplyPowerCycledFault:
                case PowerSupplyPsOkFault:
                case PowerSupplyReinsertFault:
                case PowerSupplyReplaceFault:
                case RingNoActiveMgmtPort:
                case RingNoActiveMgmtPortCleared:
                case RingNoActiveMgmtPortCleared_OwnerChanged:
                case RingNoActiveMgmtPort_RingDegraded:
                case StagedFirmwareOperationSuccessful:
                case BladeRedundantCommFault:
                case BladeRedundantCommFaultCleared:
                case ChassisEventServiceSubscriptionOverrunFault:
                case ChassisEventServiceSubscriptionOverrunFaultCleared:
                case ChassisRedundantCommFault:
                case ChassisRedundantCommFaultCleared:
                case CIManagerRedundantCommFault:
                case CiManagerRedundantCommFaultCleared:
                case EmHighAssuranceBootCritical:
                case EmRedundantCommFault:
                case EmRedundantCommFaultCleared:
                case FanRedundantCommFault:
                case FanRedundantCommFaultCleared:
                case FrontPanelRedundantCommFault:
                case FrontPanelRedundantCommFaultCleared:
                case InterconnectRedundantCommFault:
                case InterconnectRedundantCommFaultCleared:
                case OneViewServiceEventsCommFault:
                case OneViewServiceEventsCommFaultCleared:
                case PowerSupplyRedundantCommFault:
                case SupplyRedundantCommFaultCleared:
		case TopologyError:
		case PartnerSwitchCommunicationFailure:
		case PartnerSwitchVersionMismatch: 
		case PartnerSwitchWWIDMismatch: 
		case PartnerSwitchNotPresent:
		case NotConfigured:
		case cpqRackServerBladeInserted2:
		case BladeInserted:
		case cpqRackNetConnectorInserted:
		case InterconnectInserted:
		case coldStart:
		case linkDown:
		case linkUp:
		case opModeChange:
		case subPortStatusChange:
		case linkStateChange:
		case cpqHe3TemperatureDegraded:
		case cpqHe3ThermalConfirmation:
		case cpqSeCpuStatusChange:
		case authenticationFailure:
		case cpqHe3PostError:
		case cpqHe3TemperatureOk: 
		case cpqSm2SelfTestError:
		case cpqSm2ServerPowerOnFailure:
		case cpqSm2IrsCommFailure:
		case cpqHo2PowerThresholdTrap:
		case cpqIdeAtaDiskStatusChange:
		case cpqSeUSBStorageDeviceReadErrorOccurred:
		case cpqSeUSBStorageDeviceWriteErrorOccurred:
		case cpqSeUSBStorageDeviceRedundancyLost:
		case cpqSeUSBStorageDeviceRedundancyRestored:
		case cpqSeUSBStorageDeviceSyncFailed:
		case cpqSePCIeDiskTemperatureFailed:
		case cpqSePCIeDiskTemperatureOk:
		case cpqSePCIeDiskConditionChange:
		case cpqSePCIeDiskWearStatusChange:
		case cpqSePciDeviceAddedOrPoweredOn:
		case cpqSePciDeviceRemovedOrPoweredOff:
		case cpqFca3HostCntlrStatusChange:
		case cpqDa6CntlrStatusChange:
		case cpqDa6LogDrvStatusChange:
		case cpqDa6AccelStatusChange:
		case cpqDa6AccelBadDataTrap:
		case cpqDa6AccelBatteryFailed:
		case cpqDa7PhyDrvStatusChange:
		case cpqDa7SpareStatusChange:
		case cpqDaPhyDrvSSDWearStatusChange:
		case cpqHe3FltTolPowerRedundancyLost:
		case cpqHe3FltTolPowerSupplyInserted:
		case cpqHe3FltTolPowerSupplyRemoved:
		case cpqHe3FltTolFanDegraded: 
		case cpqHe3FltTolFanFailed: 
		case cpqHe3FltTolFanRedundancyLost:
		case cpqHe3FltTolFanInserted:
		case cpqHe3FltTolFanRemoved:
		case cpqHe4FltTolPowerSupplyOk:
		case cpqHe4FltTolPowerSupplyDegraded:
		case cpqHe4FltTolPowerSupplyFailed:
		case cpqHeResilientMemMirroredMemoryEngaged:
		case cpqHe3FltTolPowerRedundancyRestored:
		case cpqHe3FltTolFanRedundancyRestored:
		case cpqHe5CorrMemReplaceMemModule:
		case cpqHe4FltTolPowerSupplyACpowerloss:
		case cpqHeSysBatteryFailed:
		case cpqHeSysBatteryRemoved:
		case cpqHeSysPwrAllocationNotOptimized:
		case cpqHeSysPwrOnDenied:
		case cpqHePowerFailureError:
		case cpqHeInterlockFailureError:
		case cpqSs6FanStatusChange:
		case cpqSs6TempStatusChange:
		case cpqSs6PwrSupplyStatusChange:
		case cpqSsConnectionStatusChange:
		case cpqSm2UnauthorizedLoginAttempts:
		case cpqSm2SecurityOverrideEngaged:
		case cpqSm2SecurityOverrideDisengaged:
		case cpqHo2GenericTrap: 
		case cpqHoMibHealthStatusArrayChangeTrap:
		case cpqSasPhyDrvStatusChange:
		case cpqSasPhyDrvSSDWearStatusChange:
		case cpqNic3ConnectivityRestored:
		case cpqNic3ConnectivityLost:
		case cpqNic3RedundancyIncreased:
		case cpqNic3RedundancyReduced:     

		/* crm. alerts starts from here below */

		case logicalInterconnectStateChange:  
		case logicalInterconnectStateCritical:  
		case logicalInterconnectStateAbsent:  
		case logicalInterconnectStateMismatch:  
		case complianceChange:  
		case nameChange:  
		case enableTaggedLLDP:
		case enableRichTLV:  
		case uplinkSetStateChange:  
//		case connection.bandwidthChange:  
		case sanVlanMismatch:  
		case cannotCommunicateWithSwitch:  
		case disabledInterconnectConnectivityForSwitch:  
		case wrongSwitch:  
		case duplicateSwitch:  
		case invalidSwitchCredentials:  
		case invalidXmlReturnedFromSwitch:  

		/* missing hpris alerts starts from here below*/

		case BladeConfigurationFault:  
		case BladeConfigurationFaultCleared:  
		case BladeCritical:  
		case BladeDiscoveryComplete:  
		case BladeEfusePGoodFault:  
		case BladeEfusePGoodFaultCleared:  
		case BladeFault:  
		case BladeFaultCleared:  
		case BladeFruContentFault:  
		case BladeFruManufacturedForInvalidFault:  
		case BladeFruManufacturedForMismatchFault:
		case BladeIloFailedCleared:
		case BladeIloFailedCritical:
		case BladeIloFruFault:
		case BladeIloOperationalStatusCleared:
		case BladeIloOperationalStatusWarning:
		case BladeInsufficientCooling:
		case BladeInsufficientCoolingCleared:
		case BladeInsufficientPower:
		case BladeInsufficientPowerCleared:
		case BladeManagerCritical:
		case BladeManagerFault:
		case BladeManagerFaultCleared:
		case BladeManagerInternalCritical:
		case BladeManagerInternalCriticalCleared:
		case BladeManagerInternalWarning:
		case BladeManagerInternalWarningCleared:
		case BladeManagerOk:
		case BladeManagerWarning:
		case BladeMateDetectFault:
		case BladeMateDetectFaultCleared:
		case BladeMezzFruContentFault:
		case BladeMezzFruFault:
		case BladeMezzFruManufacturedForInvalidFault:
		case BladeMezzFruManufacturedForMismatchFault:
		case BladeOk:
		case BladePowerAllocationChanged:
		case BladePowerDeniedDueToFru:
		case BladePowerDeniedDueToMating:
		case BladePowerDeniedDueToMatingCleared:
		case BladePowerDeniedDueToMidplane:
		case BladePowerOff:
		case BladePowerReleaseGranted:
		case BladePowerRequestGranted:
		case BladeSystemPGoodFault:
		case BladeSystemPGoodFaultCleared:
		case BladeThermalCritical:
		case BladeThermalOk:
		case BladeThermalWarning:
		case BladeWarning:
		case CIManagerDiscoveryComplete:
		case CIManagerFault:
		case CIManagerFaultCleared:
		case CIManagerFruContentFault:
		case CIManagerFruManufacturedForInvalidFault:
		case CIManagerFruManufacturedForMismatchFault:
		case CIManagerWarrantySerialUpdate:
		case CertificateDeleted:
		case CertificateSaved:
		case ChassisCritical:
		case ChassisOk:
		case ChassisUidOff:
		case ChassisUidOn:
		case ChassisWarning:
		case Claimed:
		case DeviceInventoryCompleted:
		case DeviceManagerCanmicInfoblockUpdated:
		case DeviceManagerDataChanged:
		case EmBooting:
		case EmClusterResourceCleared:
		case EmClusterResourceFault:
		case EmCritical:
		case EmDiscoveryComplete:
		case EmFault:
		case EmFaultCleared:
		case EmFruContentFault:
		case EmFruManufacturedForInvalidFault:
		case EmFruManufacturedForMismatchFault:
		case EmFwMatch:
		case EmFwMismatch:
		case EmImageManagerLinkDisconnected:
		case EmImageManagerLinkDisconnectedCleared_MgmtPortConnected:
		case EmImageManagerLinkDisconnectedCleared_MgmtPortModeChanged:
		case EmInserted:
		case EmInternalLinkDisconnected:
		case EmInternalLinkDisconnectedCleared_InternalLinkConnected:
		case EmInternalLinkDisconnectedCleared_PeerEmRemoved:
		case EmLinkConnectedIncorrectly:
		case EmLinkConnectedIncorrectlyCleared_LinkLost:
		case EmLinkConnectedIncorrectlyCleared_OwnersOk:
		case EmLinkDisconnected:
		case EmLinkDisconnectedCleared_LinkDetected:
		case EmLinkDisconnectedCleared_PeerEmRemoved:
		case EmLinkNeighborInfoExpired:
		case EmLinkNeighborInfoExpiredCleared:
		case EmLinkNeighborOwnerMismatch:
		case EmLinkNeighborOwnerMismatchCleared_LinkLost:
		case EmLinkedNeighborsUpdated:
		case EmMgmtPortConnectedIncorrectly:
		case EmMgmtPortConnectedIncorrectlyCleared_MgmtPortDisconnected:
		case EmMgmtPortConnectedIncorrectlyImageManager:
		case EmMissing:
		case EmMissingCleared:
		case EmOk:
		case EmOneViewLinkDisconnected:
		case EmOneViewLinkDisconnectedCleared_MgmtPortConnected:
		case EmOneViewLinkDisconnectedCleared_MgmtPortModeChanged:
		case EmOneViewLinkDisconnectedCleared_OneViewRemoved:
		case EmPeerEmNeeded:
		case EmPeerEmNeededCleared_ElinkRemoved:
		case EmPeerEmNeededCleared_PeerEmInserted:
		case EmRemoved:
		case EmSwitchCommunicationCritical:
		case EmSwitchCommunicationOk:
		case EmSwitchCritical:
		case EmSwitchOk:
		case EmSwitchPortOk:
		case EmSwitchPortUpdated:
		case EmSwitchPortWarning:
		case EmSwitchWarning:
		case EmThermalCritical:
		case EmThermalOk:
		case EmThermalSensorReadOk:
		case EmThermalSensorReadWarning:
		case EmThermalWarning:
		case EmUidLedOff:
		case EmUidLedOn:
		case EmWarning:
		case FailedLoginAttempt:
		case FanCritical:
		case FanDiscoveryComplete:
		case FanElectronicFuseBlown:
		case FanFault:
		case FanFaultCleared:
		case FanFruContentFault:
		case FanFruManufacturedForInvalidFault:
		case FanFruManufacturedForMismatchFault:
		case FanOk:
		case FanWarning:
		case FrontPanelAttached:
		case FrontPanelCritical:
		case FrontPanelDiscoveryComplete:
		case FrontPanelFault:
		case FrontPanelFaultCleared:
		case FrontPanelFruContentFault:
		case FrontPanelFruManufacturedForInvalidFault:
		case FrontPanelFruManufacturedForMismatchFault:
		case FrontPanelOk:
		case FrontPanelRemoved:
		case FrontPanelWarning:
		case InsufficientFansCritical:
		case InsufficientFansOk:
		case InsufficientFansWarning:
		case InterconnectConfigurationFault:
		case InterconnectConfigurationFaultCleared:
		case InterconnectCritical:
		case InterconnectDiscoveryComplete:
		case InterconnectEfusePGoodFault:
		case InterconnectEfusePGoodFaultCleared:
		case InterconnectFault:
		case InterconnectFaultCleared:
		case InterconnectFruContentFault:
		case InterconnectFruManufacturedForInvalidFault:
		case InterconnectFruManufacturedForMismatchFault:
		case InterconnectInsufficientCooling:
		case InterconnectInsufficientCoolingCleared:
		case InterconnectInsufficientPower:
		case InterconnectInsufficientPowerCleared:
		case InterconnectManagerFault:
		case InterconnectManagerFaultCleared:
		case InterconnectManagerInternalCritical:
		case InterconnectManagerInternalCriticalCleared:
		case InterconnectManagerInternalWarning:
		case InterconnectManagerInternalWarningCleared:
		case InterconnectMateDetectFault:
		case InterconnectMateDetectFaultCleared:
		case InterconnectOk:
		case InterconnectPowerDeniedDueToFru:
		case InterconnectPowerDeniedDueToMating:
		case InterconnectPowerDeniedDueToMatingCleared:
		case InterconnectPowerDeniedDueToMidplane:
		case InterconnectPowerReleaseGranted:
		case InterconnectSystemPGoodFault:
		case InterconnectSystemPGoodFaultCleared:
		case InterconnectThermalCritical:
		case InterconnectThermalOk:
		case InterconnectThermalWarning:
		case InterconnectWarning:
		case InternalFailure:
		case InvalidFirmwarePackage:
		case MaintenanceLoginRejected:
		case MaintenanceSessionStarted:
		case MidplaneDiscoveryComplete:
		case MidplaneFault:
		case MidplaneFruContentFault:
		case MidplaneFruManufacturedForInvalidFault:
		case OVERRUN:
		case PasswordChanged:
		case PowerAlertModeActivated:
		case PowerAlertModeArmed:
		case PowerAlertModeDeactivated:
		case PowerAlertModeDisarmed:
		case PowerCritical:
		case PowerFastCappingDisabled:
		case PowerFastCappingEnabled:
		case PowerInputFault:
		case PowerInputFaultCleared:
		case PowerOk:
		case PowerSubsystemChanged:
		case PowerSubsystemMismatch:
		case PowerSubsystemMismatchCleared:
		case PowerSubsystemOverload:
		case PowerSubsystemOverloadCleared:
		case PowerSubsystemRedundancyLost:
		case PowerSubsystemRedundancyLostCleared:
		case PowerSupplyCritical:
		case PowerSupplyDiscoveryComplete:
		case PowerSupplyFault:
		case PowerSupplyFaultCleared:
		case PowerSupplyFruContentFault:
		case PowerSupplyFruManufacturedFrInvalidFault:
		case PowerSupplyFruManufacturedForMismatchFault:
		case PowerSupplyOk:
		case PowerSupplyWarning:
		case PowerWarning:
		case RisTestEvent:
		case SessionCreated:
		case SessionDeleted:
		case SessionTimeout:
		case StandbyUpdateFailed:
		case SubscriptionCreated:
		case Success:
		case ThermalCritical:
		case ThermalOk:
		case ThermalWarning:
		case TooManySessions:
		case UpdateInterrupted:
		case UpdateSuccessful:
			dbg("%s -- Not processed", event->alert_name);
			break;
		case OEM_EVENT:
			oem_event_handler(oh_handler, event);
			dbg("OEM_EVENT");
			break;
		default:
			err("ALERT %s IS NOT REGISTERED", event->alert_name);
	}
	return;
}

/**
 * ov_rest_process_tasks
 * 	@oh_handler: Pointer to the openhpi handler structure
 *      @scmb_resource:  json_object Pointer to "resource" object inside
 *	@event: Pointer to the event structure
 *
 * Purpose:
 *      Process the tasks by calling appropriate functions
 *
 * Detailed Description:
 *	This function is called by process_ov_events for processing
 *	alerts. 
 *
 * Return values:
 *      NONE - void return, as this function processes the tasks
 **/
void ov_rest_process_tasks(struct oh_handler_state *oh_handler,
		json_object * scmb_resource, struct eventInfo* event)
{

	ov_rest_json_parse_tasks(scmb_resource, event);
	dbg("%s task received", event->task_name);
	switch (event->name) {
		case TASK_ADD:
			ov_rest_proc_add_task(oh_handler,event);
			dbg("TASK_ADD");
			break;
		case TASK_POWER_ON:
			ov_rest_proc_power_on_task(oh_handler,event);
			dbg("TASK_POWER_ON");
			break;
		case TASK_POWER_OFF:
			ov_rest_proc_power_off_task(oh_handler,event);
			dbg("TASK_POWER_OFF");
			break;
		case TASK_RESET:
			ov_rest_proc_reset_task(oh_handler,event);
			dbg("TASK_RESET");
			break;
		case TASK_ACTIVATE_STANDBY_APPLIANCE:
			ov_rest_proc_activate_standby_composer(oh_handler, event);
			dbg("TASK_ACTIVATE_STANDBY_APPLIANCE");
			break;
			
		case TASK_REMOVE:
		case TASK_REFRESH:
		case TASK_COLLECT_UTILIZATION_DATA:
		case TASK_MANAGE_UTILIZATION_DATA:
		case TASK_MANAGE_UTILIZATION:
		case TASK_ASSIGN_IPV4_ADDRESS:
		case TASK_INTERCONNECT_IPV4_CONSISTENCY_CHECK:
		case TASK_BACKGROUND_REFRESH:
		case TASK_CHECK_DEVICE_IDENTITY_AND_LOCATION:
		case TASK_RESET_SYNERGY_FRAME_LINK_MODULE: 
		case TASK_CLEAR_ALERTS:
		case TASK_UPDATE:
		case TASK_CONFIGURE_BOOT_ORDER_SETTINGS_FOR_SERVER:
		case TASK_ASSIGN_PROFILE:
		case TASK_BACKGROUNDREPOREFRESHTASK:
		case TASK_CLEAR_PROFILE:
		case TASK_CONFIGURE:
		case TASK_CREATE:
		case TASK_DELETE:
		case TASK_LOGICAL_ENCLOSURE_FIRMWARE_UPDATE:
		case TASK_REAPPLY_CONFIGURATION:
		case TASK_RELEASE:
		case TASK_REMOVE_PROFILE:
		case TASK_UPDATE_ENCLOSURE_FIRMWARE:
		case TASK_VALIDATE:
			dbg("%s -- Not processed", event->task_name);
			break;
		default:
			warn("TASK %s IS NOT REGISTERED", event->task_name);
	}
	return;
}

void * oh_get_event (void *)
                __attribute__ ((weak, alias("ov_rest_get_event")));
