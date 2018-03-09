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
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

/* Include libraries */
#include "ov_rest_control.h"
#include "ov_rest_discover.h"
#include "ov_rest_inventory.h"
#include "sahpi_wrappers.h"
#include "ov_rest_utils.h"
#include "ov_rest_event.h"
#include <SaHpiOvRest.h>

/* Global variable to hold total temperature sensors */
int ov_rest_Total_Temp_Sensors = 0;

static void ov_rest_push_disc_res(struct oh_handler_state *oh_handler);

/**
 * ov_rest_getapplianceNodeInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to the appliance info response structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine makes the request call to retrive the appliance
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getapplianceNodeInfo(struct oh_handler_state *oh_handler,
                struct applianceNodeInfoResponse *response,
                REST_CON *connection)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		response->applianceVersion = s.jobj;
	}

	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}
/**
 * ov_rest_getapplianceHaNodeInfo:
 *      @response:   Pointer to the  applianceHaNodeInfoResponse structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine makes the request call to retrive the appliance
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getapplianceHaNodeInfo(
				struct applianceHaNodeInfoResponse *response,
		                REST_CON *connection)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		response->haNode = s.jobj;
	}

	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getdatacenterInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to datacenter info array response structure.
 *      @connection: Pointer to connection structure.
 *      @datacenter_doc: Char pointer to hold the datacenter response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the datacenter
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getdatacenterInfo(struct oh_handler_state *oh_handler,
                struct datacenterInfoArrayResponse *response,
                REST_CON *connection,
                char* datacenter_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	enum json_type type;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		type = json_object_get_type(s.jobj);
		if (type == json_type_array) {
			response->datacenter_array = s.jobj;
		}else {
			response->datacenter_array = 
				ov_rest_wrap_json_object_object_get(s.jobj, 
						"members"); 
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getenclosureStatus:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to enclosure status response structure.
 *      @connection: Pointer to connection structure.
 *      @enclosure_doc: Char pointer to hold the enclosure response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the enclosure
 *      status information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getenclosureStatus(struct oh_handler_state *oh_handler,
                struct enclosureStatusResponse* response,
                REST_CON *connection,
                char* enclosure_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init (CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL *curl = curl_easy_init ();
	rv = ov_rest_curl_get_request (connection, chunk, curl, &s);
	if (s.jobj== NULL)
	{
		return rv;
	}
	else
	{
		response->root_jobj = s.jobj;
		response->devicebay_array = 
			ov_rest_wrap_json_object_object_get (s.jobj, 
				"deviceBays");	
		response->interconnectbay_array = 
			ov_rest_wrap_json_object_object_get (s.jobj, 
				"interconnectBays");
		response->enclosure = s.jobj;
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getenclosureInfoArray:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to enclosure info array response structure.
 *      @connection: Pointer to connection structure.
 *      @enclosure_doc: Char pointer to hold the enclosure response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the enclosure
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR                    - on failure
 **/
SaErrorT ov_rest_getenclosureInfoArray(struct oh_handler_state *oh_handler,
		struct enclosureInfoArrayResponse* response,
		REST_CON *connection,
		char* enclosure_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		/*Getting the array if it is a key value pair*/
		response->enclosure_array = 
			ov_rest_wrap_json_object_object_get(s.jobj, "members");
		if (!response->enclosure_array) {
			response->enclosure_array = s.jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getapplianceHANodeArray:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to appliance HA node info array response.
 *      @connection: Pointer to connection structure.
 *      @appliance_ha_info: Char pointer to hold the appliance response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the composer HA Node 
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR                    - on failure
 **/
SaErrorT ov_rest_getapplianceHANodeArray(struct oh_handler_state *oh_handler,
		struct applianceHaNodeInfoArrayResponse* response,
		REST_CON *connection,
		char* appliance_ha_info)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		/*Getting the array if it is a key value pair*/
		response->haNodeArray = 
			ov_rest_wrap_json_object_object_get(s.jobj, "members");
		if (!response->haNodeArray) {
			response->haNodeArray = s.jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getserverInfoArray:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to server hardware info array response structure.
 *      @connection: Pointer to connection structure.
 *      @server_doc: Char pointer to hold the server hardware response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the server hardware
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      SA_ERR_HPI_TIMEOUT - on failure
 **/
SaErrorT ov_rest_getserverInfoArray(struct oh_handler_state *oh_handler, 
			struct serverhardwareInfoArrayResponse *response, 
			REST_CON *connection,
			char* server_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		/*Getting the array if it is a key value pair*/
		response->server_array = 
			ov_rest_wrap_json_object_object_get(s.jobj, "members"); 
		if (!response->server_array) {
			response->server_array = s.jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}
  
/**
 * ov_rest_ov_rest_getserverConsoleUrl:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the remote consoleUrl 
 *      for server hardware. And then parses the iLO IP and Sessionkey
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_getserverConsoleUrl(struct oh_handler_state *oh_handler, 
			REST_CON *connection)
{
	OV_STRING s = {0};
	const char *console_url = NULL;
	char sso_url[300];
	int i = 0;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		err("Invalid Response from getserverConsoleUrl");
		wrap_g_free(connection->url);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return SA_ERR_HPI_TIMEOUT;
	}else
        {
                json_object * jobj = s.jobj;
		
		/*Getting the array if it is a key value pair*/
	        jobj = ov_rest_wrap_json_object_object_get(jobj, 
						"remoteConsoleUrl");
		if(!jobj){
			err("Invalid Response from getserverConsoleUrl"
                                   "for remoteConsoleUrl");
			wrap_free(s.ptr);
			ov_rest_wrap_json_object_put(s.jobj);
			wrap_g_free(connection->url);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
	        console_url = json_object_get_string(jobj);
		if(console_url == NULL){
			err("Console url is NULL");
			wrap_free(s.ptr);
			ov_rest_wrap_json_object_put(s.jobj);
			wrap_g_free(connection->url);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			return SA_ERR_HPI_INVALID_SESSION;
		}
		strcpy(sso_url, console_url);
		/* Replace all special charactors in sso url with Space*/
		for(i = 0; sso_url[i] !='\0';i++){
			if(sso_url[i]== '=' || sso_url[i]== '&')
				sso_url[i] = ' ';
		}
		sscanf(sso_url, "hplocons://addr %s %*s %s", 
						connection->serverIlo,
						connection->xAuthToken);
        }
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
        wrap_g_free(connection->url);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return SA_OK;
}

/**
 * ov_rest_getserverThermalInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to the serverhardwareThermalInfoResponse
 *      	     structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      thermal(temperatures and fans) sesors information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT 	  - on timeout reached
 **/
SaErrorT ov_rest_getserverThermalInfo(struct oh_handler_state *oh_handler,
			struct serverhardwareThermalInfoResponse *response, 
			REST_CON *connection)
{
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		wrap_g_free(connection->url);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return SA_ERR_HPI_TIMEOUT;
	}else
	{
		response->root_jobj = s.jobj;
		/*Getting the array if it is a key value pair*/
		response->serverhardwareThermal_array = 
			ov_rest_wrap_json_object_object_get(s.jobj, 
					"Temperatures");
		if (!response->serverhardwareThermal_array) {
			response->serverhardwareThermal_array = s.jobj;
		}

		response->serverhardwareFans_array =
			ov_rest_wrap_json_object_object_get(s.jobj,
					"Fans");
		if (!response->serverhardwareFans_array) {
			response->serverhardwareFans_array = s.jobj;
		}
	}

	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getserverPowerStatusInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to the serverhardwarePowerStatusInfoResponse
 *                   structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      power status sensor information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT        - on timeout reached
 **/
SaErrorT ov_rest_getserverPowerStatusInfo(struct oh_handler_state *oh_handler,
                        struct serverhardwarePowerStatusInfoResponse *response,
                        REST_CON *connection)
{
        OV_STRING s = {0};
        struct curl_slist *chunk = NULL;
        curl_global_init(CURL_GLOBAL_ALL);
        /* Get a curl handle */
        CURL* curl = curl_easy_init();
        ov_rest_curl_get_request(connection, chunk, curl, &s);
        if(s.jobj == NULL || s.len == 0) {
                wrap_g_free(connection->url);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return SA_ERR_HPI_TIMEOUT;
        }else
        {
                json_object *jobj = ov_rest_wrap_json_object_object_get(s.jobj,
                                                "PowerConsumedWatts");
                response->PowerConsumedWatts = json_object_get_int(jobj);
        }
	wrap_free(s.ptr);
        ov_rest_wrap_json_object_put(s.jobj);
        wrap_g_free(connection->url);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return SA_OK;
}

/**
 * ov_rest_getserverSystemsInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to the serverhardwareSystemsInfoResponse
 *                   structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      systems(memory, processor, hardware health and battery status)
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT        - on timeout reached
 **/
SaErrorT ov_rest_getserverSystemsInfo(struct oh_handler_state *oh_handler,
                        struct serverhardwareSystemsInfoResponse *response,
                        REST_CON *connection)
{
	OV_STRING s = {0};
	json_object *memory = NULL, *mem_status = NULL, *mem_health = NULL;
	json_object *processor = NULL, *proc_status = NULL, *proc_health =NULL;
	json_object *system_status = NULL, *system_health = NULL;
	json_object *oem = NULL, *hpe = NULL, *battery = NULL;
	json_object *battery_obj = NULL, *condition = NULL;
	int arraylen = 0, i;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		wrap_g_free(connection->url);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return SA_ERR_HPI_TIMEOUT;
	}else
	{
		memory = ov_rest_wrap_json_object_object_get(s.jobj, "Memory");
		mem_status = ov_rest_wrap_json_object_object_get(memory,
				"Status");
		mem_health = ov_rest_wrap_json_object_object_get(mem_status,
				"HealthRollUp");
		response->Memory_Status = json_object_get_string(mem_health);

		processor = ov_rest_wrap_json_object_object_get(s.jobj,
				"Processors");
		proc_status = ov_rest_wrap_json_object_object_get(processor,
				"Status");
		proc_health = ov_rest_wrap_json_object_object_get(proc_status,
				"HealthRollUp");
		response->Processor_Status =
			json_object_get_string(proc_health);

		system_status = ov_rest_wrap_json_object_object_get(s.jobj,
				"Status");
		system_health = ov_rest_wrap_json_object_object_get(
				system_status, "Health");
		response->System_Status =
			json_object_get_string(system_health);

		oem = ov_rest_wrap_json_object_object_get(s.jobj, "Oem");
		hpe = ov_rest_wrap_json_object_object_get(oem, "Hp");
		battery = ov_rest_wrap_json_object_object_get(hpe, "Battery");
		if (battery != NULL && 
				(json_object_get_type(battery) == json_type_array)) {
			arraylen = json_object_array_length(battery);
			for (i = 0; i < arraylen; i++) {
				battery_obj =
					json_object_array_get_idx(battery, i);
				condition =ov_rest_wrap_json_object_object_get(
						battery_obj, "Condition");
				response->Battery_Status =
					json_object_get_string(condition);
			}
		}
		else
			response->Battery_Status =
				json_object_get_string(condition);
	}
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getserverStorageInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to the serverhardwareStorageInfoResponse
 *                   structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      smartstorage status sensor information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT        - on timeout reached
 **/
SaErrorT ov_rest_getserverStorageInfo(struct oh_handler_state *oh_handler,
                        struct serverhardwareStorageInfoResponse *response,
                        REST_CON *connection)
{
	OV_STRING s = {0};
	json_object *name = NULL, *status = NULL, *health = NULL;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		wrap_g_free(connection->url);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return(SA_ERR_HPI_TIMEOUT);
	}else
	{
		name = ov_rest_wrap_json_object_object_get(s.jobj, "Name");
		response->Name = json_object_get_string(name);

		status = ov_rest_wrap_json_object_object_get(s.jobj, "Status");
		health = ov_rest_wrap_json_object_object_get(status, "Health");
		response->SmartStorage_Status = json_object_get_string(health);
	}
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getserverNetworkAdaptersInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to the serverhardwareNetworkAdaptersInfoResponse
 *                   structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      network adapters status sensor information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT        - on timeout reached
 **/
SaErrorT ov_rest_getserverNetworkAdaptersInfo(
                struct oh_handler_state *oh_handler,
                struct serverhardwareNetworkAdaptersInfoResponse *response,
                REST_CON *connection)
{
	OV_STRING s = {0};
	json_object *status = NULL, *health = NULL;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		wrap_g_free(connection->url);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return SA_ERR_HPI_TIMEOUT;
	}else
	{
		status = ov_rest_wrap_json_object_object_get(s.jobj, "Status");
		health = ov_rest_wrap_json_object_object_get(status, "Health");
		response->NetworkAdapters_Status =
			json_object_get_string(health);
	}
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getserverEthernetInterfacesInfo:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response  : Pointer to serverhardwareEthernetInterfacesInfoResponse
 *                   structure.
 *      @connection: Pointer to connection structure.
 *
 * Purpose:
 *      This routine make the request call to retrive the server hardware
 *      ethernet interfaces status sensor information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_TIMEOUT        - on timeout reached
 **/
SaErrorT ov_rest_getserverEthernetInterfacesInfo(
                struct oh_handler_state *oh_handler,
                struct serverhardwareEthernetInterfacesInfoResponse *response,
                REST_CON *connection)
{
        OV_STRING s = {0};
        json_object *status = NULL, *health = NULL;
        struct curl_slist *chunk = NULL;
        curl_global_init(CURL_GLOBAL_ALL);
        /* Get a curl handle */
        CURL* curl = curl_easy_init();
        ov_rest_curl_get_request(connection, chunk, curl, &s);
        if(s.jobj == NULL || s.len == 0){
                wrap_g_free(connection->url);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return(SA_ERR_HPI_TIMEOUT);
        }else
        {
                status = ov_rest_wrap_json_object_object_get(s.jobj, "Status");
                health = ov_rest_wrap_json_object_object_get(status, "Health");
                response->EthernetInterfaces_Status =
                                        json_object_get_string(health);
        }
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
        wrap_g_free(connection->url);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return SA_OK;
}

/**
 * ov_rest_getdriveEnclosureInfoArray:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to the drive Enclosure Info structure.
 *      @connection: Pointer to connection structure.
 *      @drive_enc_doc: Char pointer to hold the driven enclosure response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the array Drive
 *      Enclosures information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getdriveEnclosureInfoArray(struct oh_handler_state 
		*oh_handler, struct driveEnclosureInfoArrayResponse *response,
		REST_CON *connection, char* drive_enc_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		/*Getting the array if it is a key value pair*/
		response->drive_enc_array = 
			ov_rest_wrap_json_object_object_get(s.jobj, "members");
		if (!response->drive_enc_array) {
			response->drive_enc_array = s.jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return SA_OK;
}

/**
 * ov_rest_getinterconnectInfoArray:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to the interconnect info array structure.
 *      @connection: Pointer to connection structure.
 *      @interconnect_doc: Char pointer to hold the interconnect response doc.
 *
 * Purpose:
 *      This routine makes the request call to retrive the interconnect
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      ERROR                        - on failure
 **/
SaErrorT ov_rest_getinterconnectInfoArray(struct oh_handler_state *oh_handler, 
			struct interconnectInfoArrayResponse *response, 
			REST_CON *connection,
			char* interconnect_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	const char *temp = NULL;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		temp = json_object_get_string(
				ov_rest_wrap_json_object_object_get(
					s.jobj, "nextPageUri"));
		memset(response->next_page,0,MAX_256_CHARS);
		if(temp != NULL){
			memcpy(response->next_page, temp, strlen(temp)+1);
		}
		/*Getting the array if it is a key value pair*/
		response->interconnect_array = 
			ov_rest_wrap_json_object_object_get(s.jobj, "members");
		if (!response->interconnect_array) {
			response->interconnect_array = s.jobj;
		}
	}
	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_build_serverThermalRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the thermal sensors of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverThermalRdr(struct oh_handler_state *oh_handler,
                                        REST_CON *connection,
                                        struct serverhardwareInfo *response,
                                        SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwareThermalInfoResponse thermal_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server"
                          " in bay %d", response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo) &&
                                     strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_THERMAL_URI,
                                           connection->serverIlo);
                rv = ov_rest_getserverThermalInfo(oh_handler,
                                                  &thermal_response,
                                                  connection);
                if(rv == SA_OK){
                        /* Build the thermal sensors here */
                        rv = ov_rest_build_server_thermal_rdr(oh_handler,
                                        &thermal_response, rpt);
                        if (rv != SA_OK) {
                                err("Building thermal sensor rdr failed"
                                    " for server in bay %d", response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Thermal Info for server in bay"
                                                   " %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }
	ov_rest_wrap_json_object_put(thermal_response.root_jobj);
        return rv;
}

/**
 * ov_rest_build_serverPowerStatusRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the power status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverPowerStatusRdr(struct oh_handler_state 
					*oh_handler, REST_CON *connection,
					struct serverhardwareInfo *response, 
					SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwarePowerStatusInfoResponse power_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server"
                    " in bay %d", response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo)&&
                                    strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_POWER_STATUS_URI,
                                           connection->serverIlo);
                rv = ov_rest_getserverPowerStatusInfo(oh_handler,
                                                      &power_response,
                                                      connection);
                if(rv == SA_OK){
                        /* Build the power status sensor here */
                        rv = ov_rest_build_server_power_status_rdr(oh_handler,
                                                &power_response, rpt);
                        if (rv != SA_OK) {
                                err("Building power status sensor rdr failed"
                                    " for server in bay %d", response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Power Status Info for server in"
                                               " bay %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }

        return rv;
}

/**
 * ov_rest_build_serverSystemsRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the systems sensor rdr of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverSystemsRdr(struct oh_handler_state *oh_handler,
                                        REST_CON *connection,
                                        struct serverhardwareInfo *response,
                                        SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwareSystemsInfoResponse system_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server in"
                    " bay %d", response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo)&&
                                     strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SYSTEMS_URI,
                                           connection->serverIlo);
                rv = ov_rest_getserverSystemsInfo(oh_handler,
                                                  &system_response,
                                                  connection);
                if(rv == SA_OK){
                        /* Build the Memory, Processor, Hardware Health and
                        Battery sensor rdrs here */
                        rv = ov_rest_build_server_systems_rdr(oh_handler,
                                                &system_response, rpt);
                        if (rv != SA_OK) {
                                err("Building system sensor rdr failed for"
                                    " server in bay %d", response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Systems Info for server in bay"
                                                   " %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }

        return rv;
}

/**
 * ov_rest_build_serverStorageRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the smart storage status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverStorageRdr(struct oh_handler_state *oh_handler,
                                        REST_CON *connection,
                                        struct serverhardwareInfo *response,
                                        SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwareStorageInfoResponse storage_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server in bay %d",
                               response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo)&&
                                     strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url,OV_SERVER_HARDWARE_SMART_STORAGE_URI,
                                           connection->serverIlo);
                rv = ov_rest_getserverStorageInfo(oh_handler,
                                                  &storage_response,
                                                  connection);
                if(rv == SA_OK){
                        /* Build the SmartStorage sensor rdr here */
                        rv = ov_rest_build_server_storage_rdr(oh_handler,
                                                &storage_response, rpt);
                        if (rv != SA_OK) {
                                err("Build smart storage sensor rdr failed for"
                                    " server in bay %d", response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Storage Info for server in bay"
                                                   " %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }

        return rv;
}

/**
 * ov_rest_build_serverNetworkAdaptersRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the network adapater sensor rdr of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverNetworkAdaptersRdr(
                                        struct oh_handler_state *oh_handler,
                                        REST_CON *connection,
                                        struct serverhardwareInfo *response,
                                        SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwareNetworkAdaptersInfoResponse
                                               network_adapter_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server in bay %d",
                           response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo)&&
                                     strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url,
                                OV_SERVER_HARDWARE_NETWORK_ADAPTERS_URI,
                                connection->serverIlo);
                rv = ov_rest_getserverNetworkAdaptersInfo(oh_handler,
                                &network_adapter_response, connection);
                if(rv == SA_OK){
                        /* Build the NetworkAdapters sensor rdr here */
                        rv = ov_rest_build_server_network_adapters_rdr(
				oh_handler, &network_adapter_response, rpt);
                        if (rv != SA_OK) {
                                err("Build network adapter sensor rdr failed "
                                    		"for server in bay %d",
						response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Network Adapter Info for server"
                                            " in bay %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }

        return rv;
}

/**
 * ov_rest_build_serverEthernetInterfacesRdr:
 *      @oh_handler: Pointer to openhpi handler.
 *      @connection: Pointer to connection structure.
 *      @response:   Pointer to serverhardwareInfo structure.
 *      @rpt:        Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the ethernet interface sensor rdr of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *
 **/
SaErrorT ov_rest_build_serverEthernetInterfacesRdr(
                                        struct oh_handler_state *oh_handler,
                                        REST_CON *connection,
                                        struct serverhardwareInfo *response,
                                        SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        struct serverhardwareEthernetInterfacesInfoResponse
                                            ethernet_inetrface_response = {0};

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_free(connection->url);
        WRAP_ASPRINTF(&connection->url, OV_SERVER_HARDWARE_SSO_URI,
                                   connection->hostname, response->uri);
        rv = ov_rest_getserverConsoleUrl(oh_handler, connection);
        if (rv != SA_OK) {
                err("Error in getting server Console Url for server in bay %d",
                             response->bayNumber);
                wrap_free(connection->url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if(connection->serverIlo && strlen(connection->serverIlo)&&
                                     strcmp(connection->serverIlo,"0.0.0.0")){
                wrap_free(connection->url);
                WRAP_ASPRINTF(&connection->url,
                                OV_SERVER_HARDWARE_ETHERNET_INTERFACES_URI,
                                connection->serverIlo);
                rv = ov_rest_getserverEthernetInterfacesInfo(oh_handler,
                                &ethernet_inetrface_response, connection);
                if(rv == SA_OK){
                        /* Build the EthernetInterfaces sensor rdr here */
                        rv = ov_rest_build_server_ethernet_inetrfaces_rdr(
                                                oh_handler,
                                                &ethernet_inetrface_response,
                                                rpt);
                        if (rv != SA_OK) {
                                err("Building ethernet interface sensor rdr"
                                    " failed for server in bay %d",
                                              response->bayNumber);
                        }
                }
                else
                        warn("Error in getting Ethernet Interfaces Info for"
                                     " server in bay %d", response->bayNumber);
        } else {
                err("Server iLO IP in bay number %d is NULL or Invalid.",
                                                          response->bayNumber);
        }

        return rv;
}

/**
 * ov_rest_getAllEvents:
 *      @response:   Pointer to the event array response structure.
 *      @connection: Pointer to connection structure.
 *      @getallevents_doc: Char pointer to hold the event array response doc.
 *
 * Purpose:
 *      This routine makes the request call to get all the events'
 *      information in json object.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      -1                        - on failure
 **/
SaErrorT ov_rest_getAllEvents(struct eventArrayResponse *response,
                REST_CON *connection,
                char* getallevents_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	enum json_type type;
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);

	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		json_object * jobj = s.jobj;
		response->root_jobj = jobj;
		type = json_object_get_type(jobj);
		if (type == json_type_array) {
			response->event_array = jobj;
		}else {
			response->event_array = 
				ov_rest_wrap_json_object_object_get(jobj, "members"); 
		}
		json_object_object_foreach(jobj, key, val){
			if(!strcmp(key,"created")){
				memset(response->created,0,MAX_URI_LENGTH);
				memcpy(response->created,
						json_object_get_string(val),
						strlen(json_object_get_string(val))+1);
				break;
			}
		}
	}
	wrap_free(s.ptr);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_create_certificate:
 *      @connection: OV REST connection provided by ov_rest_open().
 *      @postfields: Specify data to POST.
 *
 * Purpose:
 *      Generates a certificate for RabbitMQ clients.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK - On Success.
 *      -1    - On Failure.
 **/
SaErrorT ov_rest_create_certificate(REST_CON *connection,
                                    char* postfields)
{
        OV_STRING s = {0};
        SaErrorT rv = SA_OK;
        struct curl_slist *chunk = NULL;
        curl_global_init(CURL_GLOBAL_ALL);
        /* Get a curl handle */
        CURL* curlHandle = curl_easy_init();

        rv = ov_rest_curl_put_request(connection, chunk, curlHandle,
                                         postfields, &s);

        //FIXME : Could not check "Certification generation Failure" 
        //except "Conflict".
        if(s.jobj){
                dbg("\nCertifcate:   %s\n",json_object_get_string(s.jobj));
        }
        else {
                //FIXME
                //Give time to generate cert.
                sleep(GEN_CERT_WAIT_TIME);
        }
	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
        wrap_g_free(connection->url);
        curl_easy_cleanup(curlHandle);
        curl_global_cleanup();
        wrap_free(postfields);
        return rv;
}

/**
 * ov_rest_getcertificates:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to certificateResponse structure.
 *      @connection: OV REST connection provided by ov_rest_open().
 *      @certificate_doc: Char pointer to hold the certificate response doc.
 *
 * Purpose:
 *      Gets a certificate for RabbitMQ clients.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK 			 - On Success.
 *      SA_ERR_INTERNAL_ERROR    - On Failure.
 **/
SaErrorT ov_rest_getcertificates(struct oh_handler_state *oh_handler,
                struct certificateResponse *response,
                REST_CON *connection,
                char* certificate_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		response->certificate = s.jobj;
	}

	wrap_free(s.ptr);
	wrap_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_getca:
 *      @oh_handler: Pointer to openhpi handler.
 *      @response:   Pointer to certificateResponse structure.
 *      @connection: OV REST connection provided by ov_rest_open().
 *      @ca_doc: Char pointer to hold the certificate response doc.
 *
 * Purpose:
 *      Gets a ca certificate.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK 			- On Success.
 *     SA_ERR_HPI_INTERNAL_ERROR     - On Failure.
 *     SA_ERR_HPI_TIMEOUT	     - On Failure.
 **/
SaErrorT ov_rest_getca(struct oh_handler_state *oh_handler,
                struct certificateResponse *response,
                REST_CON *connection,
                char* ca_doc)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	rv = ov_rest_curl_get_request(connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0){
		return rv;
	}else
	{
		response->root_jobj = s.jobj;
		response->certificate = s.jobj;
	}

	wrap_free(s.ptr);
	wrap_g_free(connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_session_timeout:
 *      @ov_handler: Pointer to ov_rest plugin handler.
 *      @to: Pointer to idleTimeout structure.
 *
 * Purpose:
 *      Gets the timeout value to keep the session alive
 *
 * Detailed Description: 
 *      This was created to keep the session alive by getting the 
 *      idleTimeout value of the session.
 *
 * Return values:
 *     SA_OK 			- On Success.
 *     SA_ERR_HPI_INVALID_SESSION	- No AuthToken for SSO
 *     SA_ERR_HPI_TIMEOUT	     - CURLE TIMEOUT
 *     SA_ERR_HPI_INTERNAL_ERROR     - On all other CURL errors.
 **/
SaErrorT ov_rest_session_timeout(struct ov_rest_handler *ov_handler, 
		struct idleTimeout *to)
{
	SaErrorT rv = SA_OK;
	OV_STRING s = {0};
	const char *ecode = NULL;
	/* struct timeoutResponse response = {0}; */
	/* json_object *timeoutResponse = NULL;  */
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curl = curl_easy_init();
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_GET_IDLE_TIMEOUT_URI,
				ov_handler->connection->hostname);
	rv = ov_rest_curl_get_request(ov_handler->connection, chunk, curl, &s);
	if(s.jobj == NULL || s.len == 0 || rv != SA_OK) {
		err("Get session idleTimeout failed");
                return rv;
        } 
	json_object_object_foreach(s.jobj, key, val) {
		ov_rest_prn_json_obj(key, val);
		if(!strcmp(key,"idleTimeout")) 
			to->timeout = json_object_get_int(val);
		if(!strcmp(key,"errorCode"))
			ecode = json_object_get_string(val);
	}

	if (ecode != NULL || to->timeout == 0) {
		CRIT("Session error %s or timeout=%d", ecode, to->timeout);
		rv = SA_ERR_HPI_INTERNAL_ERROR;
	}

	wrap_free(s.ptr);
	ov_rest_wrap_json_object_put(s.jobj);
	wrap_g_free(ov_handler->connection->url);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return rv;
}
	
/*
 * ov_rest_discover_resources
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the resources.
 *      This ABI is called from the OpenHPI framework for
 *      discovering HPE Synergy resources
 *
 * Detailed Description:
 *      - Checks the plugin initialization completion.
 *        If the plugin initialization has failed, then it tries to do the
 *        plugin initialization and then discovery.
 *      - OpenHPI framework calls this ABI every 3 minutes.
 *        If this function is called after the initial discovery,
 *        then call is ignored and no discovery is done again.
 *      - If the discovery is called for the 1st time (after plugin
 *        initialazation), then, starts the event thread.
 *      - If the discovery is faild for the 1st time (after plugin
 *        initialazation), then, it retries discovery nextime when
 *        OpenHPI framework calls this ABI after 3 minutes.
 *
 * Return values:
 *      SA_OK                     - on success or on shutdown_event_thread
 *                                  set to TRUE or discovery already completed
 *      SA_ERR_HPI_INTERNAL_ERROR - Wrong ov_rest handler state detected
 *
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 */
SaErrorT ov_rest_discover_resources(void *oh_handler)
{
	int rv = SA_OK;
	GError **error = NULL;
	struct oh_handler_state *handler = NULL;
	struct ov_rest_handler *ov_handler = NULL;
	struct idleTimeout sess_timeout = {0};
	static int mtl = 480; /* Max timeout loops 86400000/(1000*60*3) */

	handler = (struct oh_handler_state *) oh_handler;
	ov_handler = (struct ov_rest_handler *) handler->data;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	/* Check the event thread shutdown status
         * If TRUE, return SA_OK
         */
        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }

	wrap_g_mutex_lock(ov_handler->mutex);
	switch(ov_handler->status) {
		case PRE_DISCOVERY:
			dbg("First discovery");
			break;
		case PLUGIN_NOT_INITIALIZED:
			rv = build_ov_rest_custom_handler(handler);
			if (rv != SA_OK) {
				err("Plugin initialization failed");
				wrap_g_mutex_unlock(ov_handler->mutex);
				return rv;
			}
			break;
		case DISCOVERY_FAIL:
			err("Re-discovery, after failure, for Synergy %s", 
				ov_handler->connection->hostname);
			rv = ov_rest_connection_init(handler);
			if(rv != SA_OK) {
				err("Please check whether the Synergy "
					"Composer %s is accessible",
					ov_handler->connection->hostname);
				wrap_g_mutex_unlock(ov_handler->mutex);
				return rv;
			}
			rv = ov_rest_re_discover_resources(handler);
			if (rv != SA_OK) {
				err("Re-discovery failed ");
				wrap_g_mutex_unlock(ov_handler->mutex);
				return rv;
			}
			ov_handler->status = DISCOVERY_COMPLETED;
			wrap_g_mutex_unlock(ov_handler->mutex);
			err("Re-discovery successfull");
			return rv;

		case DISCOVERY_COMPLETED:
			dbg("Discovery already done");
			/* Call a function to keep the session alive */
			rv = ov_rest_session_timeout(ov_handler,&sess_timeout);
			if (( ov_handler->discover_called_count == 0 ) &&
				( sess_timeout.timeout != 0 )) {
				mtl = sess_timeout.timeout/(1000*60*3);
				dbg("idleTimeout=%d and max loops=%d",
					sess_timeout.timeout, mtl);
			}
			if (( rv == SA_OK ) &&
				( ov_handler->discover_called_count < mtl-3 )) {
				ov_handler->discover_called_count++;
                                wrap_g_mutex_unlock(ov_handler->mutex);
                                return rv;
                        }
			rv = ov_rest_connection_init(handler);
			if(rv != SA_OK){
				err("Please check whether the Synergy "
					"Composer %s is accessible",
					ov_handler->connection->hostname);
				wrap_g_mutex_unlock(ov_handler->mutex);
				return rv;
			}
			wrap_g_mutex_unlock(ov_handler->mutex);
			return rv;

		default:
			err("Wrong ov_rest handler state %d detected",
                              ov_handler->status);
			wrap_g_mutex_unlock(ov_handler->mutex);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	if(ov_handler->thread_handler == NULL) {
		ov_handler->thread_handler = 
			wrap_g_thread_create_new("ov_rest_event_thread",
					ov_rest_event_thread,
					handler,
					TRUE, error);
		if(ov_handler->thread_handler == NULL) {
			err("wrap_g_thread_create_new failed");
			wrap_g_mutex_unlock(ov_handler->mutex);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
	}

	rv = discover_ov_rest_system(handler);
	if(rv != SA_OK) {
		err("Discovery Failed");
		ov_handler->status = DISCOVERY_FAIL;
		wrap_g_mutex_unlock(ov_handler->mutex);
                return rv;
	}
	ov_handler->status = DISCOVERY_COMPLETED;
	err("OV_REST Discovery Completed");
	wrap_g_mutex_unlock(ov_handler->mutex);
	return SA_OK;
}

void func_t (gpointer key, gpointer value, gpointer udata)
{
       dbg("\nkey: %s value: %s\n",(char*)key ,(char*)value);
}

/**
 * discover_ov_rest_system
 *      @handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the OV REST resources.
 *      Discovers all the resources of Synergy system
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK        - on success.
 *      Error        - on failure.
 **/
SaErrorT discover_ov_rest_system(struct oh_handler_state *handler)
{
	SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;

        if (handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) handler->data;

	/* Discover Data Center here*/

	dbg(" Discovering HPE Synergy Appliance ......................");
	rv = ov_rest_discover_appliance(handler);	
        if (rv != SA_OK) {
                err("Failed to discover Appliance");
                return rv;
        }

        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
        dbg(" Discovering Enclosure ......................");
        rv = ov_rest_discover_enclosure(handler);
        if (rv != SA_OK) {
                err("Failed to discover Enclosure");
                return rv;
        }

        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
	dbg(" Discovering Composers ...................");
	rv = ov_rest_discover_composer(handler);
	if (rv != SA_OK) {
		err("Failed to discover Composers");
		return rv;
	}
        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
        dbg(" Discovering Blades ...................");
        rv = ov_rest_discover_server(handler);
        if (rv != SA_OK) {
                err("Failed to discover Server Blade");
                return rv;
        }
        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }

        dbg(" Discovering Drive Enclosures ...................");
        rv = ov_rest_discover_drive_enclosure(handler);
        if (rv != SA_OK) {
                err("Failed to discover Drive Enclosure");
                return rv;
        }

        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
        dbg(" Discovering InterConnect ...................");
        rv = ov_rest_discover_interconnect(handler);
        if (rv != SA_OK) {
                err("Failed to discover InterConnects");
		if(rv != SA_ERR_HPI_TIMEOUT)
	                return rv;
        }

        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
        dbg(" Discovering SAS InterConnect ...................");
        rv = ov_rest_discover_sas_interconnect(handler);
        if (rv != SA_OK) {
                err("Failed to discover SAS-InterConnects");
                return rv;
        }

        dbg(" Discovering Power Supply ...................");
	rv = ov_rest_discover_powersupply(handler);
        if(rv != SA_OK) {
                err("Failed to discover PowerSupply");
                return rv;
        }

        dbg(" Discovering Fan ...................");
	rv = ov_rest_discover_fan(handler);
        if(rv != SA_OK) {
                err("Failed to discover Fan");
                return rv;
        }

        if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
                dbg("shutdown_event_thread set. Returning in thread %p",
                     g_thread_self());
                return SA_OK;
        }
	ov_rest_push_disc_res(handler);

        g_hash_table_foreach(ov_handler->uri_rid, func_t,NULL); 

	return rv;
}

/**
 * ov_rest_discover_appliance:
 *      @handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the appliance.
 *      We want a top level entity path that is not removable.
 *      Appliance is a top level Synergy ring representation.
 *      Active composer represents Synergy ring
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK    - on success.
 *      Error    - on failure.
 **/
SaErrorT ov_rest_discover_appliance(struct oh_handler_state *handler)
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct applianceNodeInfoResponse response = {0};
	struct applianceHaNodeInfoArrayResponse ha_array_response = {0};
	struct applianceNodeInfo result = {{{0}}};
	struct applianceHaNodeInfo ha_node_result = {{0}};
	SaHpiResourceIdT resource_id;
	char* s = NULL;
	char *appliance_array_doc = NULL;
	char active_sno[MAX_256_CHARS] = "\0";

	ov_handler = (struct ov_rest_handler *) handler->data;

	memset(&response,0, sizeof(struct applianceNodeInfoResponse));
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_APPLIANCE_VERSION_URI, 
			ov_handler->connection->hostname);
	rv = ov_rest_getapplianceNodeInfo(handler, &response, 
			ov_handler->connection);
	if(rv != SA_OK || response.applianceVersion == NULL) {
		CRIT("Failed to get the response from ov_rest_getappliance\n");
		return rv;
	}
	ov_rest_json_parse_appliance_version(response.applianceVersion,
			&result.version);

	ov_rest_wrap_json_object_put(response.root_jobj);

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_APPLIANCE_HA_NODE_ID_URI, 
			ov_handler->connection->hostname, 
			result.version.serialNumber);
	strncpy(active_sno, result.version.serialNumber, MAX_256_CHARS-1);
	active_sno[MAX_256_CHARS-1] = '\0';

	rv = ov_rest_getapplianceHANodeArray(handler, &ha_array_response, 
			ov_handler->connection, appliance_array_doc); 

	if(rv != SA_OK || ha_array_response.haNodeArray == NULL) {
		CRIT("No response from ov_rest_getapplianceHANodeArray");
		return rv;
	}
	ov_rest_json_parse_appliance_Ha_node(
			ha_array_response.haNodeArray,
			&ha_node_result);

	ov_rest_wrap_json_object_put(ha_array_response.root_jobj);

	if (strcmp(ha_node_result.role, "Active") == 0) {
		if(strstr(ha_node_result.uri, active_sno) == NULL) {  
			CRIT("Active composer uri %s and sno %s differ",
					ha_node_result.uri, active_sno);
		}
		rv = ov_rest_build_appliance_rpt(handler, 
				&ha_node_result, &resource_id, "Active");
		if (rv != SA_OK) {
			err("Build appliance rpt failed for resource id %d",
							resource_id);
			return rv;
		}
		/* Save appliance resource id */
		ov_handler->ov_rest_resources.composer.resource_id = 
			resource_id;
		strcpy(ov_handler->ov_rest_resources.composer.serialNumber,
				result.version.serialNumber);
	}
	itostr(resource_id ,&s);
	g_hash_table_insert(ov_handler->uri_rid, 
			g_strdup(result.version.uri), g_strdup(s));
	wrap_free(s);
	rv = ov_rest_build_appliance_rdr(handler, 
			&result, &ha_node_result, resource_id);
	if (rv != SA_OK) {
		err("Build appliance rdr failed for resource id %d",
							resource_id);
		wrap_free(s);
		return rv;
	}
	return SA_OK;
}

/**
 * ov_rest_build_appliance_inv_rdr:
 *      @oh_handler: Handler data pointer.
 *      @response: Pointer to appliance Info structure.
 *      @rdr: Rdr Structure for inventory data.
 *      @inventory: Rdr private data structure.
 *
 * Purpose:
 *      Creates an inventory rdr for appliance.
 *
 * Detailed Description:
 *      - Populates the enclosure inventory rdr with default values.
 *      - Inventory data repository is created and associated in the private
 *        data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - Request failed due to insufficient memory.
 **/
SaErrorT ov_rest_build_appliance_inv_rdr(struct oh_handler_state *oh_handler,
                                 struct applianceNodeInfo *response,
                                 struct applianceHaNodeInfo *ha_response,
                                 SaHpiRdrT *rdr,
                                 struct ov_rest_inventory **inventory)
{
	SaErrorT rv = SA_OK;
	SaHpiIdrFieldT hpi_field;
	char appliance_inv_str[] = APPLIANCE_INVENTORY_STRING, *tmp = NULL;
	struct ov_rest_inventory *local_inventory = NULL;
	struct ovRestArea *head_area = NULL;
	SaHpiInt32T add_success_flag = 0;
	SaHpiInt32T product_area_success_flag = 0;
	SaHpiInt32T area_count = 0;
	struct ov_rest_handler *ov_handler = NULL;
	SaHpiResourceIdT resource_id;
	SaHpiRptEntryT *rpt = NULL;
	SaHpiFloat64T fm_version;
	SaHpiInt32T major;

	if (oh_handler == NULL || response == NULL || rdr == NULL ||
			inventory == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	resource_id = ov_handler->ov_rest_resources.composer.resource_id;
	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("Appliance RPT is NULL for resource id %d", resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Populating inventory rdr with default values and resource name */
	rdr->Entity = rpt->ResourceEntity;
	rdr->RecordId = 0;
	rdr->RdrType  = SAHPI_INVENTORY_RDR;
	rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
	rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->version.name);
	rdr->IdString.DataLength = strlen(response->version.name);
	snprintf((char *)rdr->IdString.Data,
			strlen(response->version.name) + 1,
			"%s", response->version.name);

	/* Create inventory IDR and populate the IDR header */
	local_inventory = (struct ov_rest_inventory*)
		g_malloc0(sizeof(struct ov_rest_inventory));
	if (!local_inventory) {
		err("OV REST out of memory while building appliance inventory"
			" rdr for resource id %d", resource_id);
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.IdrId =
		rdr->RdrTypeUnion.InventoryRec.IdrId;
	local_inventory->info.idr_info.UpdateCount = 1;
	local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
	local_inventory->info.idr_info.NumAreas = 0;
	local_inventory->info.area_list = NULL;
	local_inventory->comment =
		(char *)g_malloc0(strlen(appliance_inv_str) + 1);
	strcpy(local_inventory->comment, appliance_inv_str);

	/* Create and add product area if resource name and/or manufacturer
	 * information exist
	 */
	rv = ov_rest_add_product_area(&local_inventory->info.area_list,
			response->version.name,
			"HPE",
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add product area failed for appliance id %d",
							resource_id);
		return rv;
	}

	/* add_success_flag will be true if product area is added,
	 * if this is the first successful creation of IDR area, then have
	 * area pointer stored as the head node for area list
	 */
	if (add_success_flag != SAHPI_FALSE) {
		product_area_success_flag = SAHPI_TRUE;
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add chassis area if resource part number and/or
	 * serial number exist
	 */
	rv = ov_rest_add_chassis_area(&local_inventory->info.area_list,
			NULL,
			response->version.serialNumber,
			&add_success_flag);
	if (rv != SA_OK) {
		err("Add chassis area failed  for the appliance id %d",
							resource_id);
		return rv;
	}
	if (add_success_flag != SAHPI_FALSE) {
		(local_inventory->info.idr_info.NumAreas)++;
		if (area_count == 0) {
			head_area = local_inventory->info.area_list;
		}
		++area_count;
	}

	/* Create and add internal area if all/atleast one of required
	 * information of resource for internal is available
	 */
	local_inventory->info.area_list = head_area;
	*inventory = local_inventory;

	/* Adding the product version in IDR product area.  It is added at
	 * the end of the field list.
	 */
	if (product_area_success_flag == SAHPI_TRUE) {
		/* Add the product version field if the appliance hardware info
		 * is available
		 */
		if (response->version.softwareVersion != NULL) {
			memset(&hpi_field, 0, sizeof(SaHpiIdrFieldT));
			hpi_field.AreaId = local_inventory->info.area_list->
				idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
			strcpy ((char *)hpi_field.Field.Data,
					response->version.softwareVersion);

			rv = ov_rest_idr_field_add(&(local_inventory
					->info.area_list->field_list),
					&hpi_field);
			if (rv != SA_OK) {
				err("Add idr softwareVersion field failed for "
					"the appliance id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
			
			/* Check whether Firmware version is NULL. */
			if (!response->version.softwareVersion) {
				err("Firmware version is not vailable for the"
					"resource %d", resource_id);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			/* Store Firmware MajorRev & MinorRev data in rpt */
			fm_version = atof(response->version.softwareVersion);
			rpt->ResourceInfo.FirmwareMajorRev = major =
					(SaHpiUint8T)floor(fm_version);
			rpt->ResourceInfo.FirmwareMinorRev = rintf((
					fm_version - major) * 100);
		}
		if (ha_response->uri != NULL) {
			hpi_field.AreaId = local_inventory->info.area_list->
					idr_area_head.AreaId;
			hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
			WRAP_ASPRINTF(&tmp,"URI = %s",ha_response->uri);
			strcpy ((char *)hpi_field.Field.Data, tmp);
			wrap_free(tmp);

			rv = ov_rest_idr_field_add(&(
				local_inventory->info.area_list->field_list),
				&hpi_field);
			if (rv != SA_OK) {
				err("Add idr uri field failed for the "
                                         "appliance id %d", resource_id);
				return rv;
			}

			/* Increment the field counter */
			local_inventory->info.area_list->idr_area_head.
				NumFields++;
		}
	}
	return SA_OK;
}

/**
 * ov_rest_build_appliance_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to appliance info response structure.
 *      @ha_response: Pointer to appliance ha_node info response structure.
 *      @resource_id: Resource id
 *
 * Purpose:
 *      Populate the appliance RDR.
 *      Pushes the RDR entry to plugin RPTable
 *
 * Detailed Description:
 *         - Creates the appliance RDR
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_appliance_rdr(struct oh_handler_state *oh_handler,
                             struct applianceNodeInfo *response,
                             struct applianceHaNodeInfo *ha_response,
                             SaHpiResourceIdT resource_id)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        struct ov_rest_inventory *inventory = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        SaHpiInt32T sensor_status = 0;
        SaHpiInt32T sensor_val = 0;

        if (oh_handler == NULL || response == NULL || ha_response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Build inventory rdr for the appliance */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = ov_rest_build_appliance_inv_rdr(oh_handler, response, ha_response,
				&rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to Add appliance inventory RDR for the resource "
				"id %d", resource_id);
                return rv;
        }

        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for appliance resource id %d",
								resource_id);
                return rv;
        }

        /* Build operational status sensor rdr */
        switch (ha_response->applianceStatus ) {
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

        return SA_OK;
}

/**
 * ov_rest_build_appliance_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @name:        Pointer to the name of the appliance.
 *      @resource_id: Pointer to the resource id.
 *      @type:  Active / Standby.
 *
 * Purpose:
 *      Builds the appliance RPT entry.
 *      All non-active appliances are added under the respective enclisures.
 *      Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_appliance_rpt(struct oh_handler_state *oh_handler,
                             struct applianceHaNodeInfo *response,
                             SaHpiResourceIdT *resource_id, char *type)
{
	SaErrorT rv = SA_OK;
	SaHpiEntityPathT entity_path = {{{0}}};
	struct ov_rest_handler *ov_handler = NULL;
	char *entity_root = NULL, *enclosure_doc = NULL;
	struct ovRestHotswapState * hotswap_state = NULL;
	SaHpiPowerStateT state = {0};
	struct applianceInfo appliancebay_info = {{0}};
	struct enclosureInfoArrayResponse enc_response = {0};
	json_object* jvalue_comp_array = NULL, *jvalue_composer = NULL;
	SaHpiRptEntryT rpt = {0};

	if (oh_handler == NULL || response == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	if(ov_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			response->enclosure_uri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enc_response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || enc_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return rv;
	}
	jvalue_comp_array = ov_rest_wrap_json_object_object_get(
			enc_response.enclosure_array,
			"applianceBays");
	jvalue_composer = 
		json_object_array_get_idx(jvalue_comp_array, 
				response->bayNumber-1);
	if (!jvalue_composer) {
		CRIT("Invalid response for the composer"
				" in bay %d", response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;	
	}
	ov_rest_json_parse_applianceInfo(jvalue_composer,
			&appliancebay_info);
	ov_rest_wrap_json_object_put(enc_response.root_jobj);
	/* Fetch and encode the entity path required for the rpt field */
	entity_root = (char *) g_hash_table_lookup(oh_handler->config,
			"entity_root");
	memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
	rv = oh_encode_entitypath(entity_root, &entity_path);
	if (rv != SA_OK) {
		err("Encoding entity path failed for the appliance");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Populate the rpt with the details of the appliance */
	memset(&rpt, 0, sizeof(SaHpiRptEntryT));
	rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
		SAHPI_CAPABILITY_RESOURCE |
		SAHPI_CAPABILITY_SENSOR |
		SAHPI_CAPABILITY_INVENTORY_DATA |
		SAHPI_CAPABILITY_CONTROL ;
	rpt.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_ROOT;
	rpt.ResourceEntity.Entry[0].EntityLocation = 0;
	rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
	if (rv != SA_OK) {
		err("Concat of entity path failed for the appliance");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	switch(response->applianceStatus){
		case OK:
			rpt.ResourceSeverity = SAHPI_OK;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Disabled:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_TRUE;
			break;
		case Warning:
			rpt.ResourceSeverity = SAHPI_MINOR;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Critical:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		default:
			rpt.ResourceSeverity = SAHPI_MAJOR;
			rpt.ResourceFailed = SAHPI_TRUE;
	}
	rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
	rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->name);
	rpt.ResourceTag.DataLength = strlen("ACTIVE_COMPOSER");
	memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	snprintf((char *) (rpt.ResourceTag.Data),
			rpt.ResourceTag.DataLength+1, "%s", "ACTIVE_COMPOSER");
	rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));
	/* set default hotswap capability */
	if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		rpt.HotSwapCapabilities =
			SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
	} else {
		rpt.HotSwapCapabilities = 0;
	}
	if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		switch (appliancebay_info.powerState){
			case Off:
				state = SAHPI_POWER_OFF;
				break;
			case On:
				state = SAHPI_POWER_ON;
				break;
			case PoweringOff:
				state = SAHPI_POWER_OFF;
				break;
			case PoweringOn:
				state = SAHPI_POWER_ON;
				break;
			case Restting:
				err("Wrong Power State (REBOOT) detected"
						" for appliance in bay %d",
						appliancebay_info.bayNumber);
				return SA_ERR_HPI_INTERNAL_ERROR;
			default:
				err("Unknown Power State %d detected"
						" for the server in bay %d",
						appliancebay_info.powerState,
						appliancebay_info.bayNumber);
				return SA_ERR_HPI_INTERNAL_ERROR;
		}
	}
	if(appliancebay_info.poweredOn){
		state = SAHPI_POWER_ON;
	}
	hotswap_state = (struct ovRestHotswapState *)
		g_malloc0(sizeof(struct ovRestHotswapState));
	if(hotswap_state == NULL){
		err("Out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	switch (state) {
		case SAHPI_POWER_ON:
		case SAHPI_POWER_CYCLE:
			hotswap_state->currentHsState =
				SAHPI_HS_STATE_ACTIVE;
			break;
		case SAHPI_POWER_OFF:
			hotswap_state->currentHsState =
				SAHPI_HS_STATE_INACTIVE;
			break;
		default:
			err("Unknown power status %d for server"
					" in bay %d", state, 
					appliancebay_info.bayNumber);
			wrap_g_free(hotswap_state);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Add the Appliance rpt to the plugin RPTable */
	rv = oh_add_resource(oh_handler->rptcache, &rpt, hotswap_state , 0);
	if (rv != SA_OK) {
		err("Failed to Add Appliance Resource");
		wrap_g_free(hotswap_state);
		return rv;
	}
	*resource_id = rpt.ResourceId;
	return SA_OK;
}

/**
 * ov_rest_build_composer_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @name:        Pointer to the name of the appliance.
 *      @resource_id: Pointer to the resource id.
 *      @type:  Active / Standby.
 *
 * Purpose:
 *      Builds the appliance RPT entry.
 *     All non-active appliances are added under root
 *      Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_composer_rpt(struct oh_handler_state *oh_handler,
                             struct applianceHaNodeInfo *response,
                             SaHpiResourceIdT *resource_id, char *type)
{
	SaErrorT rv = SA_OK;
	SaHpiEntityPathT entity_path = {{{0}}};
	struct ov_rest_handler *ov_handler = NULL;
	char *entity_root = NULL, *enclosure_doc = NULL;
	SaHpiRptEntryT rpt = {0}, *enc_rpt = NULL;
	struct enclosureStatus *enclosure = NULL;
	struct ovRestHotswapState * hotswap_state = NULL;
	SaHpiPowerStateT state = {0};
	struct applianceInfo appliancebay_info = {{0}};
	struct enclosureInfoArrayResponse enc_response = {0};
	json_object* jvalue_comp_array = NULL, *jvalue_composer = NULL;

	if (oh_handler == NULL || response == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	if(ov_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
			ov_handler->connection->hostname,
			response->enclosure_uri);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &enc_response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || enc_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return rv;
	}
	jvalue_comp_array = ov_rest_wrap_json_object_object_get(
			enc_response.enclosure_array,
			"applianceBays");
	jvalue_composer =
		json_object_array_get_idx(jvalue_comp_array,
				response->bayNumber-1);
	if (!jvalue_composer) {
		CRIT("Invalid response for the composer"
				" in bay %d", response->bayNumber);
		ov_rest_wrap_json_object_put(enc_response.root_jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	ov_rest_json_parse_applianceInfo(jvalue_composer,
			&appliancebay_info);
	ov_rest_wrap_json_object_put(enc_response.root_jobj);
	/* Fetch and encode the entity path required for the rpt field */
	entity_root = (char *) g_hash_table_lookup(oh_handler->config,
			"entity_root");
	memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
	rv = oh_encode_entitypath(entity_root, &entity_path);
	if (rv != SA_OK) {
		err("Encoding entity path failed for the appliance");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Populate the rpt with the details of the appliance */
	memset(&rpt, 0, sizeof(SaHpiRptEntryT));
	rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
		SAHPI_CAPABILITY_RESOURCE |
		SAHPI_CAPABILITY_SENSOR |
		SAHPI_CAPABILITY_INVENTORY_DATA |
		SAHPI_CAPABILITY_CONTROL ;
	rpt.ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
	rpt.ResourceEntity.Entry[2].EntityLocation = 0;
	rpt.ResourceEntity.Entry[1].EntityType =
		SAHPI_ENT_SYSTEM_CHASSIS;
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		if(strstr(response->enclosure_uri, enclosure->serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure){
		enc_rpt = oh_get_resource_by_id(oh_handler->rptcache,
				enclosure->enclosure_rid);
	}else{
		err("Could not find the associated enclosure"
				" for the composer in bay %d, parent location uri"
				" %s",response->bayNumber, response->enclosure_uri);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rpt.ResourceEntity.Entry[1].EntityLocation =
		enc_rpt->ResourceEntity.Entry[0].EntityLocation;
	rpt.ResourceEntity.Entry[0].EntityType =
		SAHPI_ENT_SYS_MGMNT_MODULE;
	rpt.ResourceEntity.Entry[0].EntityLocation =
		response->bayNumber;
	rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
	if (rv != SA_OK) {
		err("Concat of entity path failed for the appliance");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	switch(response->applianceStatus){
		case OK:
			rpt.ResourceSeverity = SAHPI_OK;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Disabled:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_TRUE;
			break;
		case Warning:
			rpt.ResourceSeverity = SAHPI_MINOR;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Critical:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		default:
			rpt.ResourceSeverity = SAHPI_MAJOR;
			rpt.ResourceFailed = SAHPI_TRUE;
	}
	rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
	rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->name);
	rpt.ResourceTag.DataLength = strlen(response->name);
	memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	snprintf((char *) (rpt.ResourceTag.Data),
			strlen(response->name) + 1, "%s", response->name);
	rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));
	/* set default hotswap capability */
	if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		rpt.HotSwapCapabilities =
			SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
	} else {
		rpt.HotSwapCapabilities = 0;
	}
	if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		switch (appliancebay_info.powerState){
			case Off:
				state = SAHPI_POWER_OFF;
				break;
			case On:
				state = SAHPI_POWER_ON;
				break;
			case PoweringOff:
				state = SAHPI_POWER_OFF;
				break;
			case PoweringOn:
				state = SAHPI_POWER_ON;
				break;
			case Restting:
				err("Wrong Power State (REBOOT) detected"
						" for appliance in bay %d",
						appliancebay_info.bayNumber);
				return SA_ERR_HPI_INTERNAL_ERROR;
			default:
				err("Unknown Power State %d detected"
						" for the server in bay %d",
						appliancebay_info.powerState,
						appliancebay_info.bayNumber);
				return SA_ERR_HPI_INTERNAL_ERROR;
		}
	}
	if(appliancebay_info.poweredOn){
		state = SAHPI_POWER_ON;
	}
	hotswap_state = (struct ovRestHotswapState *)
		g_malloc0(sizeof(struct ovRestHotswapState));
	if(hotswap_state == NULL){
		err("Out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	switch (state) {
		case SAHPI_POWER_ON:
		case SAHPI_POWER_CYCLE:
			hotswap_state->currentHsState =
				SAHPI_HS_STATE_ACTIVE;
			break;
		case SAHPI_POWER_OFF:
			hotswap_state->currentHsState =
				SAHPI_HS_STATE_INACTIVE;
			break;
		default:
			err("Unknown power status %d for server"
					" in bay %d", state,
					appliancebay_info.bayNumber);
			wrap_g_free(hotswap_state);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	/* Add the Appliance rpt to the plugin RPTable */
	rv = oh_add_resource(oh_handler->rptcache, &rpt, hotswap_state , 0);
	if (rv != SA_OK) {
		err("Failed to Add Appliance Resource");
		wrap_g_free(hotswap_state);
		return rv;
	}
	*resource_id = rpt.ResourceId;
	return SA_OK;
}



/**
 * ov_rest_build_enc_info:
 *      @oh_handler: Pointer to openhpi handler.
 *      @info:       Pointer to enclosure info structure.
 *
 * Purpose:
 *      Builds the enclosure info.
 *
 * Detailed Description:
 *         - NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure.
 **/
SaErrorT ov_rest_build_enc_info(struct oh_handler_state *oh_handler,
                                       struct enclosureInfo *info)
{
	
	struct ov_rest_handler *ov_handler = NULL;
	SaHpiInt32T i = 0;;
	struct enclosureStatus *temp_enclosure = NULL;
	struct enclosureStatus *enclosure = NULL, *temp = NULL;

	if (oh_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	if(ov_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	enclosure = ov_handler->ov_rest_resources.enclosure;
	temp_enclosure = (struct enclosureStatus* )
				g_malloc0(sizeof(struct enclosureStatus ));

	temp_enclosure->enclosure_rid =
		SAHPI_UNSPECIFIED_RESOURCE_ID;
	temp_enclosure->power_subsystem_rid =
		SAHPI_UNSPECIFIED_RESOURCE_ID;
	temp_enclosure->thermal_subsystem_rid =
		SAHPI_UNSPECIFIED_RESOURCE_ID;
	temp_enclosure->lcd_rid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	temp_enclosure->serialNumber = (char *) g_malloc0(sizeof(char *) *
                                        MAX_256_CHARS);
	if (temp_enclosure->serialNumber == NULL) {
		err("Out of memory");
		wrap_g_free(temp_enclosure);
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}
	strcpy(temp_enclosure->serialNumber, info->serialNumber);
	

	/* Create the resource presence matrix for
         * Composer, server, interconnect,power supply and fan unit.
         * We need the resource presence matrix for re-discovery to sync
         * with current states of the resources
         */

	/* Build resource presence matrix for Composers */
	temp_enclosure->composer.max_bays = info->composerBays;
	if(temp_enclosure->composer.max_bays){
		temp_enclosure->composer.presence = 
			(enum resource_presence *)
			g_malloc0((sizeof(enum resource_presence)) *
					temp_enclosure->composer.max_bays);
		if(temp_enclosure->composer.presence == NULL){
			err("Out of memory");
			wrap_g_free(temp_enclosure->serialNumber);
			wrap_g_free(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* allocate memory for resource_id matrix for composers */
		 temp_enclosure->composer.resource_id = 
			(SaHpiResourceIdT *) g_malloc0((
				sizeof(SaHpiResourceIdT ) *
				temp_enclosure->composer.max_bays));
		if (temp_enclosure->composer.resource_id == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		temp_enclosure->composer.type = 
			(enum resourceCategory *)
			g_malloc0((sizeof(enum resourceCategory)) *
					temp_enclosure->composer.max_bays);
		if(temp_enclosure->composer.type == NULL){
			
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		temp_enclosure->composer.serialNumber = (char **)
			g_malloc0(sizeof(char **) *
					temp_enclosure->composer.max_bays);
		if(temp_enclosure->composer.serialNumber == NULL){
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		for(i = 0; i < temp_enclosure->composer.max_bays; i++){
			temp_enclosure->composer.presence[i] = RES_ABSENT;
			temp_enclosure->composer.resource_id[i] =
				SAHPI_UNSPECIFIED_RESOURCE_ID;
			temp_enclosure->composer.serialNumber[i] = (char *)
				g_malloc0(sizeof(char) * MAX_256_CHARS);
			if (temp_enclosure->composer.serialNumber[i] ==
					NULL) {
				err("Out of memory");
				release_ov_rest_resources(temp_enclosure);
				return SA_ERR_HPI_OUT_OF_MEMORY;
			}
		}
	}	

	/* Build resource presence matrix for servers */
	temp_enclosure->server.max_bays = info->bladeBays;
	if(temp_enclosure->server.max_bays){
		temp_enclosure->server.presence =
			(enum resource_presence *)
			g_malloc0((sizeof(enum resource_presence)) *
					temp_enclosure->server.max_bays);
		if (temp_enclosure->server.presence == NULL) {
			err("Out of memory");	
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* allocate memory for resource_id matrix server blades */
		temp_enclosure->server.resource_id =
			(SaHpiResourceIdT *) g_malloc0((
				sizeof(SaHpiResourceIdT ) * 
				temp_enclosure->server.max_bays));
		if (temp_enclosure->server.resource_id == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		temp_enclosure->server.type =
			(enum resourceCategory *)
			g_malloc0((sizeof(enum resourceCategory)) *
					temp_enclosure->server.max_bays);
		if (temp_enclosure->server.type == NULL) {
			err("Out of memory");	
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* Create the placeholder for serial number
		 * If the gets replaced during the switchover or when OV 
		 * is not reachable, we can detect this change by comparing 
		 * the serial numbers of the old and new blade.
		 */
		temp_enclosure->server.serialNumber = (char **)
			g_malloc0(sizeof(char **) *
					temp_enclosure->server.max_bays);
		if (temp_enclosure->server.serialNumber == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		for (i = 0; i < temp_enclosure->server.max_bays; i++) {
			temp_enclosure->server.presence[i] = RES_ABSENT;
			temp_enclosure->server.resource_id[i] =
				SAHPI_UNSPECIFIED_RESOURCE_ID;
			temp_enclosure->server.serialNumber[i] = (char *)
				g_malloc0(sizeof(char *) * MAX_256_CHARS);
			if (temp_enclosure->server.serialNumber[i] ==
					NULL) {
				err("Out of memory");
				release_ov_rest_resources(temp_enclosure);
				return SA_ERR_HPI_OUT_OF_MEMORY;
			}
		}
	}

	/* Build resource presence matrix for interconnects */
	temp_enclosure->interconnect.max_bays =
		info->interconnectTrayBays;
	if(temp_enclosure->interconnect.max_bays){

		temp_enclosure->interconnect.presence =
			(enum resource_presence *)
			g_malloc0((sizeof(enum resource_presence)) *
					temp_enclosure->interconnect.max_bays);
		if (temp_enclosure->interconnect.presence == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* allocate memory for resource_id matrix interconnects */
		temp_enclosure->interconnect.resource_id =
			(SaHpiResourceIdT *)
			g_malloc0((sizeof(SaHpiResourceIdT ) *
				temp_enclosure->interconnect.max_bays));
		if (temp_enclosure->interconnect.resource_id == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		temp_enclosure->interconnect.type =
			(enum resourceCategory *)
			g_malloc0((sizeof(enum resourceCategory)) *
					temp_enclosure->interconnect.max_bays);
		if (temp_enclosure->interconnect.type == NULL) {
			err("Out of memory");	
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		temp_enclosure->interconnect.serialNumber = (char **)
			g_malloc0(sizeof(char **) *
					temp_enclosure->interconnect.max_bays);
		if (temp_enclosure->interconnect.serialNumber == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		for (i = 0; i < temp_enclosure->interconnect.max_bays;
				i++) {
			temp_enclosure->interconnect.presence[i] =
				RES_ABSENT;
			temp_enclosure->interconnect.resource_id[i] =
				SAHPI_UNSPECIFIED_RESOURCE_ID;

			temp_enclosure->interconnect.serialNumber[i] =
				(char *) g_malloc0(sizeof(char *) *
						MAX_256_CHARS);
			if (temp_enclosure->interconnect.serialNumber[i]
					== NULL) {
				err("Out of memory");
				release_ov_rest_resources(temp_enclosure);
				return SA_ERR_HPI_OUT_OF_MEMORY;
			}
		}
	}

	/* Build resource presence matrix for fans */
	temp_enclosure->fan.max_bays = info->fanBayCount;
	if(temp_enclosure->fan.max_bays){

		temp_enclosure->fan.presence =
			(enum resource_presence *)
			g_malloc0((sizeof(enum resource_presence)) *
					temp_enclosure->fan.max_bays);
		if (temp_enclosure->fan.presence == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* allocate memory for fans resource_id array */
		temp_enclosure->fan.resource_id =
			(SaHpiResourceIdT *)
			g_malloc0((sizeof(SaHpiResourceIdT ) *
						temp_enclosure->fan.max_bays));
		if (temp_enclosure->fan.resource_id == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		temp_enclosure->fan.type =
			(enum resourceCategory *)
			g_malloc0((sizeof(enum resourceCategory)) *
					temp_enclosure->fan.max_bays);
		if (temp_enclosure->fan.type == NULL) {
			err("Out of memory");	
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

                temp_enclosure->fan.serialNumber = (char **)
                        g_malloc0(sizeof(char **) *
                                        temp_enclosure->fan.max_bays);
                if (temp_enclosure->fan.serialNumber == NULL) {
                        err("Out of memory");
                        release_ov_rest_resources(temp_enclosure);
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }

		/* Fans do not have serial number. Therefore, the serial number
		 * array is not constructed.
		 */
		for (i = 0; i < temp_enclosure->fan.max_bays; i++) {
			temp_enclosure->fan.presence[i] = RES_ABSENT;
			temp_enclosure->fan.resource_id[i] =
				SAHPI_UNSPECIFIED_RESOURCE_ID;

                        temp_enclosure->fan.serialNumber[i] =
                                (char *) g_malloc0(sizeof(char *) *
                                                MAX_256_CHARS);
                        if (temp_enclosure->fan.serialNumber[i] ==
                                        NULL) {
                                err("Out of memory");
                                release_ov_rest_resources(temp_enclosure);
                                return SA_ERR_HPI_OUT_OF_MEMORY;
			}
		}
	}

	/* Build resource presence matrix for power supply units */
	temp_enclosure->ps_unit.max_bays = info->powerSupplyBayCount;
	if(temp_enclosure->ps_unit.max_bays){
		temp_enclosure->ps_unit.presence =
			(enum resource_presence *)
			g_malloc0((sizeof(enum resource_presence)) *
					temp_enclosure->ps_unit.max_bays);
		if (temp_enclosure->ps_unit.presence == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		/* allocate memory for power supplies resource_id array */
		temp_enclosure->ps_unit.resource_id =
			(SaHpiResourceIdT *)
			g_malloc0((sizeof(SaHpiResourceIdT ) *
					temp_enclosure->ps_unit.max_bays));
		if (temp_enclosure->ps_unit.resource_id == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		temp_enclosure->ps_unit.type =
			(enum resourceCategory *)
			g_malloc0((sizeof(enum resourceCategory)) *
					temp_enclosure->ps_unit.max_bays);
		if (temp_enclosure->ps_unit.type == NULL) {
			err("Out of memory");	
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}
		temp_enclosure->ps_unit.serialNumber = (char **)
			g_malloc0(sizeof(char **) *
					temp_enclosure->ps_unit.max_bays);
		if (temp_enclosure->ps_unit.serialNumber == NULL) {
			err("Out of memory");
			release_ov_rest_resources(temp_enclosure);
			return SA_ERR_HPI_OUT_OF_MEMORY;
		}

		for (i = 0; i < temp_enclosure->ps_unit.max_bays; i++) {
			temp_enclosure->ps_unit.presence[i] = RES_ABSENT;
			temp_enclosure->ps_unit.resource_id[i] =
				SAHPI_UNSPECIFIED_RESOURCE_ID;

			temp_enclosure->ps_unit.serialNumber[i] =
				(char *) g_malloc0(sizeof(char *) *
						MAX_256_CHARS);
			if (temp_enclosure->ps_unit.serialNumber[i] ==
					NULL) {
				err("Out of memory");
				release_ov_rest_resources(temp_enclosure);
				return SA_ERR_HPI_OUT_OF_MEMORY;
			}
		}
	}
	temp = enclosure;
	if(enclosure == NULL){
		enclosure = temp_enclosure;
		enclosure->next = NULL;
		ov_handler->ov_rest_resources.enclosure = enclosure;
	}else { 
		temp = enclosure;
		while(enclosure != NULL){
			if(enclosure->next == NULL){
				enclosure->next = temp_enclosure;
				enclosure = enclosure->next;
				enclosure->next = NULL;
				break;
			}
			enclosure = enclosure->next;
		}
		ov_handler->ov_rest_resources.enclosure = temp;
	}
	dbg("interconnet bays = %d",
			temp_enclosure->interconnect.max_bays);

	return SA_OK;
}

/**
 * ov_rest_build_enclosure_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @name:        Pointer to the name of the enclosure.
 *      @resource_id: Pointer to the resource id.
 *
 * Purpose:
 *      Builds the enclosure RPT entry.
 *      Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_enclosure_rpt(struct oh_handler_state *oh_handler,
                             struct enclosureInfo *response,
                             SaHpiResourceIdT *resource_id)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path = {{{0}}};
        struct ov_rest_handler *ov_handler = NULL;
        char *entity_root = NULL;
        SaHpiRptEntryT rpt = {0};

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;
        if(ov_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Fetch and encode the entity path required for the rpt field */
        entity_root = (char *) g_hash_table_lookup(oh_handler->config,
                                                   "entity_root");
        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed for the enclosure"
			" with serial number %s", response->serialNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        /* Populate the rpt with the details of the enclosure */
        memset(&rpt, 0, sizeof(SaHpiRptEntryT));
        rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                   SAHPI_CAPABILITY_RESOURCE |
                                   SAHPI_CAPABILITY_SENSOR |
                                   SAHPI_CAPABILITY_INVENTORY_DATA |
                                   SAHPI_CAPABILITY_CONTROL ;
        rpt.ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt.ResourceEntity.Entry[1].EntityLocation = 0;
	rpt.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_SYSTEM_CHASSIS; 
        rpt.ResourceEntity.Entry[0].EntityLocation = 
		           ++ov_handler->current_enclosure_count;
	ov_handler->max_current_enclosure_count++;
	
        rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
        if (rv != SA_OK) {
                err("Concat of entity path failed for the enclosure"
			" with serial number %s", response->serialNumber); 
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        switch(response->enclosureStatus){
                case OK:
                        rpt.ResourceSeverity = SAHPI_OK;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Disabled:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_TRUE;
                        break;
                case Warning:
                        rpt.ResourceSeverity = SAHPI_MINOR;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Critical:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                default:
                        rpt.ResourceSeverity = SAHPI_MAJOR;
                        rpt.ResourceFailed = SAHPI_TRUE;
        }
        rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
        rpt.HotSwapCapabilities = 0x0;
        rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(response->name);
        rpt.ResourceTag.DataLength = strlen(response->name);
        memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *) (rpt.ResourceTag.Data),
                 strlen(response->name) + 1, "%s", response->name);
        rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));

        /* Add the enclosure rpt to the plugin RPTable */
        rv = oh_add_resource(oh_handler->rptcache, &rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to Add Enclosure with serial number %s",
						response->serialNumber);
                return rv;
        }
        *resource_id = rpt.ResourceId;
        return SA_OK;
}

/**
 * ov_rest_build_enclosure_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to enclosure info response structure.
 *      @resource_id: Resource id.
 *
 * Purpose:
 *      Populate the enclosure RDR.
 *      Pushes the RDR entry to plugin RPTable
 *
 * Detailed Description:
 *         - Creates the enclosure inventory RDR.
 *         - Creates the temperature, operational status sensor RDR.
 *         - Creates UID control RDR
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_enclosure_rdr(struct oh_handler_state *oh_handler,
                             struct enclosureInfo *response,
                             SaHpiResourceIdT resource_id)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
	struct ov_rest_inventory *inventory = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Build inventory rdr for the enclosure */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = ov_rest_build_enclosure_inv_rdr(oh_handler, response, &rdr, 
						&inventory);
        if (rv != SA_OK) {
                err("Failed to Add enclosure inventory RDR for resource"
				" id %d", resource_id);
                return rv;
        }

        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for enclosure resource id %d",
							resource_id);
                return rv;
        }

        /* Build UID control rdr for Enclosure */ 
        OV_REST_BUILD_CONTROL_RDR(OV_REST_UID_CNTRL, 0, 0);

        /* Build operational status sensor rdr */
	switch (response->enclosureStatus ) {
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

        return SA_OK;
}

/**
 * ov_rest_discover_enclosure:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the enclosure.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK            - on success.
 *      SA_ERR_HPI_ERROR - on failure.
 **/
SaErrorT ov_rest_discover_enclosure(struct oh_handler_state *handler)
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo result = {{0}};
        SaHpiResourceIdT resource_id;
	char* enclosure_doc = NULL, *s = NULL;
	int i = 0, j = 0, arraylen = 0;
	struct enclosureStatus *enclosure = NULL, *temp = NULL;
	json_object *jvalue = NULL, *jvalue_enc = NULL, *jvalue_enc_array=NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI, 
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(handler, &response, 
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return rv;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array){
		CRIT("Enclosures may not be added as no array received");
		return SA_OK; 
	}

	arraylen = json_object_array_length(response.enclosure_array);
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue){
			CRIT("Invalid response for the enclosure %d",
				i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue,&result);
		jvalue_enc_array = ov_rest_wrap_json_object_object_get(jvalue,
		                        "managerBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_enc_array) != json_type_array) {
		        CRIT("Not adding managerBay supplied to enclosure %d,"
		                " no array returned for that",i);
			continue;
		}

		for(j = 0; j < arraylen; j++){
		        jvalue_enc = json_object_array_get_idx(jvalue_enc_array,
		                        j);
		        if (!jvalue_enc) {
		                CRIT("Invalid response for the enclosure"
		                        " %d", i + 1);
		                continue;
		        }
		        ov_rest_json_parse_enc_manager_bays(jvalue_enc, &result);
			if(result.presence == Absent)
		                continue;
		}
		enclosure = ov_handler->ov_rest_resources.enclosure;
		temp = enclosure;
		while(temp){
			if(strstr(result.serialNumber, temp->serialNumber)){
				break;		
			}
			temp = temp->next;
		}
		if(temp){
			if(strstr(result.serialNumber, temp->serialNumber)){
				continue;
			}
		}
		
		rv = ov_rest_build_enc_info(handler, &result);
		if (rv != SA_OK) {
			err("Build enclosure info failed for the enclosure "
				"with serial number %s", result.serialNumber);
			ov_rest_wrap_json_object_put(response.root_jobj);
			return rv;
		}
		rv = ov_rest_build_enclosure_rpt(handler, &result, 
			&resource_id);
		if (rv != SA_OK) {
			err("Build enclosure rpt failed for the enclosure "
				"with serial number %s", result.serialNumber);
			ov_rest_wrap_json_object_put(response.root_jobj);
			return rv;
		}
		itostr(resource_id ,&s);
	        g_hash_table_insert(ov_handler->uri_rid, g_strdup(result.uri), 
					g_strdup(s));
		wrap_free(s);
		enclosure = ov_handler->ov_rest_resources.enclosure;
		temp = enclosure;
		if(temp != NULL){
			while(temp->next != NULL){
				temp = temp->next;
			}
		}else{
			ov_rest_wrap_json_object_put(response.root_jobj);
			return SA_ERR_HPI_ERROR;
		}
		/* Save enclosure resource id */
		temp->enclosure_rid = resource_id;
		strcpy(temp->serialNumber, result.serialNumber);

		rv = ov_rest_build_enclosure_rdr(handler,
				&result, resource_id);
		if (rv != SA_OK) {
			err("Build enclosure rdr failed for the enclosure "
				"with serial number %s", result.serialNumber);
			ov_rest_wrap_json_object_put(response.root_jobj);
			return rv;
		}

	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * ov_rest_build_composer_inv_rdr:
 *      @oh_handler: Handler data pointer.
 *      @response: Pointer to appliance Info structure.
 *	@ha_response: Pointer to struct applianceHaNodeInfo structure
 *      @rdr: Rdr Structure for inventory data.
 *      @inventory: Rdr private data structure.
 *
 * Purpose:
 *      Creates an inventory rdr for composer.
 *
 * Detailed Description:
 *      - Populates the composer inventory rdr with default values.
 *      - Inventory data repository is created and associated in the private
 *        data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - Request failed due to insufficient memory.
 **/

SaErrorT ov_rest_build_composer_inv_rdr(struct oh_handler_state *oh_handler,
				struct applianceInfo *response,
				struct applianceHaNodeInfo *ha_response,
				SaHpiResourceIdT resource_id,
				SaHpiRdrT *rdr,
				struct ov_rest_inventory **inventory)
{

        SaErrorT rv = SA_OK;
        SaHpiIdrFieldT hpi_field = {0};
        char appliance_inv_str[] = APPLIANCE_INVENTORY_STRING, *tmp = NULL;
        struct ov_rest_inventory *local_inventory = NULL;
        struct ovRestArea *head_area = NULL;
        SaHpiInt32T add_success_flag = 0;
        SaHpiInt32T product_area_success_flag = 0;
        SaHpiInt32T area_count = 0;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiFloat64T fm_version = 0;
        SaHpiInt32T major = 0;

        if (oh_handler == NULL || response == NULL || rdr == NULL ||
                        inventory == NULL) {
                err("Invalid parameter.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the rpt entry of the resource */
        rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("resource RPT is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Populating inventory rdr with default values and resource name */
        rdr->Entity = rpt->ResourceEntity;
        rdr->RecordId = 0;
        rdr->RdrType  = SAHPI_INVENTORY_RDR;
        rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
        rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
        rdr->IdString.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(ha_response->name);
        rdr->IdString.DataLength = strlen(ha_response->name);
        snprintf((char *)rdr->IdString.Data,
                        strlen(ha_response->name) + 1,
                        "%s", ha_response->name);

        /* Create inventory IDR and populate the IDR header */
        local_inventory = (struct ov_rest_inventory*)
                g_malloc0(sizeof(struct ov_rest_inventory));
        if (!local_inventory) {
                err("OV REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
        local_inventory->info.idr_info.IdrId =
                rdr->RdrTypeUnion.InventoryRec.IdrId;
        local_inventory->info.idr_info.UpdateCount = 1;
        local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
        local_inventory->info.idr_info.NumAreas = 0;
        local_inventory->info.area_list = NULL;
        local_inventory->comment =
                (char *)g_malloc0(strlen(appliance_inv_str) + 1);
        strcpy(local_inventory->comment, appliance_inv_str);

        /* Create and add product area if resource name and/or manufacturer
         * information exist
         */
        rv = ov_rest_add_product_area(&local_inventory->info.area_list,
                        ha_response->name,
                        "HPE",
                        &add_success_flag);
        if (rv != SA_OK) {
                err("Add product area failed");
                return rv;
        }

        /* add_success_flag will be true if product area is added,
         * if this is the first successful creation of IDR area, then have
         * area pointer stored as the head node for area list
         */
        if (add_success_flag != SAHPI_FALSE) {
                product_area_success_flag = SAHPI_TRUE;
                (local_inventory->info.idr_info.NumAreas)++;
                if (area_count == 0) {
                        head_area = local_inventory->info.area_list;
                }
                ++area_count;
        }

        /* Create and add chassis area if resource part number and/or
         * serial number exist
         */
        rv = ov_rest_add_chassis_area(&local_inventory->info.area_list,
                        NULL,
                        response->serialNumber,
                        &add_success_flag);
        if (rv != SA_OK) {
                err("Add chassis area failed");
                return rv;
        }
        if (add_success_flag != SAHPI_FALSE) {
                (local_inventory->info.idr_info.NumAreas)++;
                if (area_count == 0) {
                        head_area = local_inventory->info.area_list;
                }
                ++area_count;
        }

        /* Create and add internal area if all/atleast one of required
         * information of resource for internal is available
         */
        local_inventory->info.area_list = head_area;
        *inventory = local_inventory;

        /* Adding the product version in IDR product area.  It is added at
         * the end of the field list.
         */
        if (product_area_success_flag == SAHPI_TRUE) {
                /* Add the product version field if the appliance hardware info
                 * is available
                 */
                if (ha_response->version != NULL) {
                        memset(&hpi_field, 0, sizeof(SaHpiIdrFieldT));
                        hpi_field.AreaId = local_inventory->info.area_list->
                                idr_area_head.AreaId;
                        hpi_field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
                        strcpy ((char *)hpi_field.Field.Data,
                                        ha_response->version);

                        rv = ov_rest_idr_field_add(&(local_inventory
                                        ->info.area_list->field_list),
                                        &hpi_field);
                        if (rv != SA_OK) {
                                err("Add idr field failed");
                                return rv;
                        }

                        /* Increment the field counter */
                        local_inventory->info.area_list->idr_area_head.
                                NumFields++;

                        /* Check whether Firmware version is NULL. */
                        if (!ha_response->version) {
                                err("Firmware version is not vailable for the"
                                        "resource %d", resource_id);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        /* Store Firmware MajorRev & MinorRev data in rpt */
                        fm_version = atof(ha_response->version);
                        rpt->ResourceInfo.FirmwareMajorRev = major =
                                        (SaHpiUint8T)floor(fm_version);
                        rpt->ResourceInfo.FirmwareMinorRev = rintf((
                                        fm_version - major) * 100);
                }
                if (ha_response->uri != NULL) {
                        hpi_field.AreaId = local_inventory->info.area_list->
                                        idr_area_head.AreaId;
                        hpi_field.Type = SAHPI_IDR_FIELDTYPE_CUSTOM;
                        WRAP_ASPRINTF(&tmp,"URI = %s",ha_response->uri);
                        strcpy ((char *)hpi_field.Field.Data, tmp);
                        wrap_free(tmp);

                        rv = ov_rest_idr_field_add(&(
                                local_inventory->info.area_list->field_list),
                                &hpi_field);
                        if (rv != SA_OK) {
                                err("Add idr field failed");
                                return rv;
                        }

                        /* Increment the field counter */
                        local_inventory->info.area_list->idr_area_head.
                                NumFields++;
                }
        }
        return SA_OK;

}

/**
 * ov_rest_build_composer_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @composer_info:    Pointer to applianceInfo structure.
 *      @ha_response: Pointer to appliance ha_node info response structure.
 *      @resource_id: Resource id
 *
 * Purpose:
 *      Populate the composer RDR.
 *      Pushes the RDR entry to plugin RPTable
 *
 * Detailed Description:
 *         - Creates the composer RDR
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT ov_rest_build_composer_rdr( struct oh_handler_state *oh_handler,
			struct applianceInfo *composer_info,
			struct applianceHaNodeInfo *ha_response,
			SaHpiResourceIdT resource_id)
{	
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        struct ov_rest_inventory *inventory = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        SaHpiInt32T sensor_status = 0;
        SaHpiInt32T sensor_val = 0;

        if (oh_handler == NULL || composer_info == NULL || ha_response == NULL){
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Build inventory rdr for the appliance */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = ov_rest_build_composer_inv_rdr(oh_handler, composer_info, 
				ha_response, resource_id, &rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to Add appliance inventory RDR");
                return rv;
        }

        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                return rv;
        }

        /* Build operational status sensor rdr */
        switch (ha_response->applianceStatus ) {
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

        return SA_OK;
}

/**
 * ov_rest_discover_composer:
 *      @handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the composers.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK    - on success.
 *      Error    - on failure.
 **/

SaErrorT ov_rest_discover_composer(struct oh_handler_state *handler)
{

	SaErrorT rv = 0;
	SaHpiResourceIdT resource_id;
	struct enclosureInfoArrayResponse enc_response = {0};
	struct applianceInfo composer_info = {{0}};
	struct applianceHaNodeInfo ha_node_result = {{0}};
	struct applianceHaNodeInfoArrayResponse ha_node_response = {0};
	struct enclosureInfo enc_result = {{0}};
	char *enclosure_doc = NULL;
	/* this Pointer must be freed at the end of this function */
	int i = 0, j = 0, arraylen = 0, comp_arraylen = 0;
	json_object *jvalue = NULL, *jvalue_comp_array = NULL;
	json_object *jvalue_composer = NULL;
	struct enclosureStatus *enclosure = NULL;
	struct ov_rest_handler *ov_handler = NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(handler, &enc_response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || enc_response.enclosure_array == NULL) {
		CRIT("No response from ov_rest_getenclosureInfoArray");
		return rv;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(enc_response.enclosure_array) 
			!= json_type_array){
		CRIT("Enclosures may not be added as no array received");
		ov_rest_wrap_json_object_put(enc_response.root_jobj);
		return SA_OK;
	}

	arraylen = json_object_array_length(enc_response.enclosure_array);
	for (i=0; i< arraylen; i++){
		memset(&enc_result, 0, sizeof(enc_result));
		jvalue = json_object_array_get_idx(
				enc_response.enclosure_array,i);
		if (!jvalue){
			CRIT("Invalid response for the enclosure in bay %d",
					i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue,&enc_result);
		jvalue_comp_array = ov_rest_wrap_json_object_object_get(jvalue,
				"applianceBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_comp_array) != json_type_array){
			CRIT("Not adding applianceBay supplied to enclosure %d,"
					" no array returned for that",i);
			continue;
		}
		comp_arraylen = json_object_array_length(jvalue_comp_array);
		for(j = 0; j < comp_arraylen; j++){
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
			/* Check if the composer presence, if Absent ignore it 
 			* and go for next composer. 
 			* */
			if(composer_info.presence == Absent){
				continue;
			}
			/* Check if the composer is present but serial number 
 			*  is null then ignore and continue.
 			* */
			if(composer_info.serialNumber == '\0'){
				CRIT("Composer Serial Number is NULL in bay %d"
					" of Enclosure rerial number %s",
						j+1, enc_result.serialNumber);
				continue;
			}
			
			WRAP_ASPRINTF(&ov_handler->connection->url, 
					OV_APPLIANCE_HA_NODE_ID_URI,
					ov_handler->connection->hostname, 
					composer_info.serialNumber);
		        rv = ov_rest_getapplianceHANodeArray(handler, 
					 &ha_node_response,
       				         ov_handler->connection, NULL);
			if(rv != SA_OK || ha_node_response.haNodeArray 
				== NULL){
				CRIT("No response from "
					"ov_rest_getapplianceHANodeArray");
				ov_rest_wrap_json_object_put(
						enc_response.root_jobj);
				return rv;
			}

			ov_rest_json_parse_appliance_Ha_node(
					ha_node_response.haNodeArray,
					&ha_node_result);

			ov_rest_wrap_json_object_put(
						ha_node_response.root_jobj);

			/* Build rpt entry for composer */
			rv = ov_rest_build_composer_rpt(handler,
					&ha_node_result, &resource_id, 
					ha_node_result.role);
			if(rv != SA_OK){
				err("Failed to Add Composer rpt for bay %d.",
						composer_info.bayNumber);
				continue;
			}

			enclosure = ov_handler->ov_rest_resources.enclosure;
			while(enclosure != NULL){
				if(strstr(enclosure->serialNumber,
						enc_result.serialNumber)){
					ov_rest_update_resource_status(
						&enclosure->composer,
						composer_info.bayNumber,
						composer_info.serialNumber,
						resource_id, RES_PRESENT,
						ha_node_result.type);
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the Composer"
					" serial number %s is unavailable",
					composer_info.serialNumber);
			}
			/* Build rdr entry for server */
			rv = ov_rest_build_composer_rdr(handler, &composer_info,
					&ha_node_result, resource_id);
			if(rv != SA_OK){
				err("build composer rdr failed");
				rv = ov_rest_free_inventory_info(handler, 
								resource_id);
				if (rv != SA_OK) {
					err("Inventory cleanup failed for the "
					"composer in bay %d with resource id "
					"%d", composer_info.bayNumber, 
					resource_id);
				}

				oh_remove_resource(handler->rptcache, resource_id);
				/* reset resource_info structure to default 
 				 * values */
				ov_rest_update_resource_status(
						&enclosure->composer,
						composer_info.bayNumber,
						"",
						SAHPI_UNSPECIFIED_RESOURCE_ID,
						RES_ABSENT,
						UNSPECIFIED_RESOURCE);
				ov_rest_wrap_json_object_put(
							enc_response.root_jobj);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
		}
	}

	ov_rest_wrap_json_object_put(enc_response.root_jobj);
	return SA_OK;
}

/**
 * build_discovered_server_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to the serverhardwareInfo structure.
 *      @resource_id: Pointer to the resource id.
 *
 * Purpose:
 *      - Populate server hardware RPT with aid of ov_rest_build_server_rpt()
 *        and add hotswap state information to the returned rpt.
 *      - Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_discovered_server_rpt(struct oh_handler_state *oh_handler,
                                     struct serverhardwareInfo *response,
                                     SaHpiResourceIdT *resource_id)
{
        SaErrorT rv = SA_OK;
	SaHpiPowerStateT state = {0};
        struct ovRestHotswapState *hotswap_state = NULL;
        SaHpiRptEntryT rpt = {0};

        if (oh_handler == NULL || response == NULL ||
            resource_id == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if(ov_rest_build_server_rpt(oh_handler, response, &rpt) != SA_OK) {
                err("Building Server Rpt failed during discovery"
			" in bay %d", response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the power state of the server blade to determine the
         * hotswap state.  The hotswap state of the server will be
         * maintained in the private data area of the server RPT.
         */
        if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
                switch (response->powerState) {
                        case Off:
                                state = SAHPI_POWER_OFF;
                                break;
                        case On:
                                state = SAHPI_POWER_ON;
                                break;
                        case PoweringOff:
				state = SAHPI_POWER_OFF;
				break;
			case PoweringOn:
				state = SAHPI_POWER_ON;
				break;
			case Restting:
                                err("Wrong Power State (REBOOT) detected"
					" for server in bay %d",
						response->bayNumber);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                                break;
                        default:
                                err("Unknown Power State %d detected"
					" for the server in bay %d",
					response->powerState,
					response->bayNumber);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                }

                hotswap_state = (struct ovRestHotswapState *)
                        g_malloc0(sizeof(struct ovRestHotswapState));
                if (hotswap_state == NULL) {
                        err("Out of memory");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }

                switch (state) {
                        case SAHPI_POWER_ON:
                        case SAHPI_POWER_CYCLE:
                                hotswap_state->currentHsState =
                                        SAHPI_HS_STATE_ACTIVE;
                                break;
                        case SAHPI_POWER_OFF:
                                hotswap_state->currentHsState =
                                        SAHPI_HS_STATE_INACTIVE;
                                break;
                        default:
                                err("Unknown power status %d for server"
				" in bay %d", state, response->bayNumber);
                                wrap_g_free(hotswap_state);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }
        /* Add the server rpt to the plugin RPTable */
        rv = oh_add_resource(oh_handler->rptcache, &rpt, hotswap_state, 0);
        if (rv != SA_OK) {
                err("Failed to add Server rpt in bay %d", response->bayNumber);
                wrap_g_free(hotswap_state);
                return rv;
        }

        *resource_id = rpt.ResourceId;
        return SA_OK;
}

/**
 * ov_rest_build_server_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to the serverhardwareInfo structure.
 *      @rpt:         Pointer to rpt to be filled.
 *
 * Purpose:
 *      This routine should be called to during discovery/re-discovery phase
 *      and when a new server-hardware gets inserted. It populates the
 *      server-hardware RPT information common to discovered and insterted
 *      server-hardwares. The caller will fill in the information specific to
 *      the manner in which the server-hardware was found.
 *      For example, the hotswap state could be different in the case
 *      of discovered vs. inserted server-hardwares, because an inserted
 *      server-hardware goes through the pending state, while a discovered
 *      server-hardware doesn't.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_build_server_rpt(struct oh_handler_state *oh_handler,
                          struct serverhardwareInfo *response,
                          SaHpiRptEntryT *rpt)
{
	SaErrorT rv = SA_OK;
	SaHpiEntityPathT entity_path = {{{0}}};
	char *entity_root = NULL;
	struct enclosureStatus *enclosure = NULL;
	SaHpiResourceIdT enc_rid;
	SaHpiRptEntryT *enc_rpt = NULL;
	struct ov_rest_handler* ov_handler = NULL;

	if (oh_handler == NULL || response == NULL || rpt == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	ov_handler = (struct ov_rest_handler *)oh_handler->data;
	entity_root = (char *) g_hash_table_lookup(oh_handler->config,
			"entity_root");
	rv = oh_encode_entitypath(entity_root, &entity_path);
	if (rv != SA_OK) {
		err("Encoding entity path failed for server in bay %d",
						response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}


	/* Populate the rpt with the details of the server */
	memset(rpt, 0, sizeof(SaHpiRptEntryT));
	rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
		SAHPI_CAPABILITY_RESET |
		SAHPI_CAPABILITY_RESOURCE |
		SAHPI_CAPABILITY_POWER |
		SAHPI_CAPABILITY_FRU |
		SAHPI_CAPABILITY_MANAGED_HOTSWAP |
		SAHPI_CAPABILITY_SENSOR |
		SAHPI_CAPABILITY_CONTROL |
		SAHPI_CAPABILITY_INVENTORY_DATA;

	if(response->bayNumber){

		rpt->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
		rpt->ResourceEntity.Entry[2].EntityLocation = 0;
		rpt->ResourceEntity.Entry[1].EntityType = 
			SAHPI_ENT_SYSTEM_CHASSIS;
		enclosure = ov_handler->ov_rest_resources.enclosure;
		while(enclosure){
			if(strstr(response->locationUri, enclosure->serialNumber)){
				break;
			}
			enclosure = enclosure->next;
		}
		if(enclosure){
			enc_rid = enclosure->enclosure_rid; 
			enc_rpt = oh_get_resource_by_id(oh_handler->rptcache, 
						enc_rid);
		}else{
			err("Could not find the associated enclosure"
				" for the server in bay %d, parent location "
				" uri %s", response->bayNumber,
					response->locationUri);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
		rpt->ResourceEntity.Entry[1].EntityLocation = 
			enc_rpt->ResourceEntity.Entry[0].EntityLocation; 

		rpt->ResourceEntity.Entry[0].EntityType =
			SAHPI_ENT_SYSTEM_BLADE;
		rpt->ResourceEntity.Entry[0].EntityLocation = 
			response->bayNumber;

	}else{
	/*Below commented code is useful in future when we add support for RMS*/
/*
		rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
		rpt->ResourceEntity.Entry[1].EntityLocation = 0; 
		rpt->ResourceEntity.Entry[0].EntityType =
			SAHPI_ENT_RACK_MOUNTED_SERVER;
		rpt->ResourceEntity.Entry[0].EntityLocation =
			++ov_handler->current_rms_count;
*/
		err("The server-hardware %s bayNumber is %d."
		"Incomplete information, so this resource can't be added"
			,response->model, response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
	if (rv != SA_OK) {
		err("Internal error (oh_concat_ep call) for server in bay %d",
						response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
	rpt->ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
        switch(response->serverStatus){
                case OK:
                        rpt->ResourceSeverity = SAHPI_OK;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                case Disabled:
                        rpt->ResourceSeverity = SAHPI_CRITICAL;
                        rpt->ResourceFailed = SAHPI_TRUE;
                        break;
                case Warning:
                        rpt->ResourceSeverity = SAHPI_MINOR;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                case Critical:
                        rpt->ResourceSeverity = SAHPI_CRITICAL;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                default:
                        rpt->ResourceSeverity = SAHPI_MAJOR;
                        rpt->ResourceFailed = SAHPI_TRUE;
        }
	rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->model);
	rpt->ResourceTag.DataLength = strlen(response->model);
	memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	snprintf((char *) (rpt->ResourceTag.Data),
			rpt->ResourceTag.DataLength+1, "%s", response->model);

	/* set default hotswap capability */
	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		rpt->HotSwapCapabilities =
			SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
	} else {
		rpt->HotSwapCapabilities = 0;
	}
	return SA_OK;
}

/**
* ov_rest_build_server_rdr:
*      @oh_handler:  Pointer to openhpi handler.
*      @resource_id: Resource id.
*      @response:    Server-hardware info response structure.
*
* Purpose:
*      Populate the server-hardware RDR.
*      Pushes the RDR entry to plugin RPTable.
*
* Detailed Description:
*      - Creates the inventory RDR.
*      - Creates the temperature, fan, power status, memory status,
*        processor status, hardware health, battery status, storage status,
*        network adapter status, ethernet interface status and
*        operational status sensor RDR.
*      - Creates UID and power control RDR.
*
* Return values:
*      SA_OK                        - on success.
*      SA_ERR_HPI_INVALID_PARAMS    - on wrong parameters.
*      SA_ERR_HPI_INVALID_RESOURCE  - on invalid resource.
*      SA_ERR_HPI_INTERNAL_ERROR    - on failure.
**/
SaErrorT ov_rest_build_server_rdr(struct oh_handler_state *oh_handler,
                          SaHpiResourceIdT resource_id,
                          struct serverhardwareInfo *response)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiRptEntryT *rpt = NULL;
        struct ov_rest_inventory *inventory = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val;

        if (oh_handler == NULL ) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rpt = oh_get_resource_by_id (oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("There is no server with the resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        /* Build inventory rdr for server */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = ov_rest_build_server_inv_rdr(oh_handler, resource_id,
                                  &rdr, &inventory, response);
        if (rv != SA_OK) {
                err("Failed to get server inventory RDR for resource id %d",
						resource_id);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for server resource id %d",
							resource_id);
                return rv;
        }

	/* The following code goes out to iLO to get the details. Synergy
	 * will not provide the details at the top level anytime soon. 
	 * Synergy access to iLO is not that great as of now. So this code
	 * is commented out for now 
         */
	/*
        struct ov_rest_handler *ov_handler = NULL;
        ov_handler = (struct ov_rest_handler *) oh_handler->data;
        rv = ov_rest_build_serverThermalRdr(oh_handler, ov_handler->connection,
                                                        response, rpt);
        if (rv != SA_OK) {
                warn("Building thermal rdr failed for server in bay %d",
                                                          response->bayNumber);
        }

        rv = ov_rest_build_serverPowerStatusRdr(oh_handler,
                                                ov_handler->connection,
                                                response, rpt);
        if (rv != SA_OK) {
                warn("Building power status rdr failed for server in bay %d",
                                                          response->bayNumber);
        }

        rv = ov_rest_build_serverSystemsRdr(oh_handler, ov_handler->connection,
                                                        response, rpt);
        if (rv != SA_OK) {
                warn("Building system sensor rdr failed for server in bay %d",
                                                          response->bayNumber);
        }

        rv = ov_rest_build_serverStorageRdr(oh_handler, ov_handler->connection,
                                                        response, rpt);
        if (rv != SA_OK) {
                warn("Building storage sensor rdr failed for server in bay %d",
                                                          response->bayNumber);
        }

        rv = ov_rest_build_serverNetworkAdaptersRdr(oh_handler,
                                                    ov_handler->connection,
                                                    response, rpt);
        if (rv != SA_OK) {
                warn("Build network adapter rdr failed for server in bay %d",
                                                          response->bayNumber);
        }

        rv = ov_rest_build_serverEthernetInterfacesRdr(oh_handler,
                                                       ov_handler->connection,
                                                       response, rpt);
        if (rv != SA_OK) {
                warn("Building ethernet interface sensor rdr failed for server"
                                             "in bay %d", response->bayNumber);
        }

        ov_rest_Total_Temp_Sensors = 0;
	*/


        /* Build power control rdr for server */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_PWR_CNTRL, 0, 0);

        /* Build UID control rdr for server */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_UID_CNTRL, 0, 0);

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
 * ov_rest_discover_server:
 *      @handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the server-hardware.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 **/
SaErrorT ov_rest_discover_server(struct oh_handler_state *handler)
{
	SaErrorT rv = 0;
	SaHpiResourceIdT resource_id;
	struct ov_rest_handler *ov_handler = NULL;
	struct serverhardwareInfoArrayResponse response = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct serverhardwareInfo info_result = {0};
	struct enclosureInfo enc_info = {{0}};
	char*  server_doc = NULL, *enclosure_doc = NULL, *s = NULL;        
	/* this Pointer must be freed at the end of this funtion */
	int i = 0, arraylen = 0;
	char* blade_name = NULL;
	json_object *jvalue = NULL;
	struct enclosureStatus *enclosure = NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_SERVER_HARDWARE_URI, 
			ov_handler->connection->hostname);
	rv = ov_rest_getserverInfoArray(handler, &response, 
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
		/* Copy the blade name from response for future processing */
			ov_rest_lower_to_upper(info_result.model,
					strlen(info_result.model), blade_name,
					MAX_256_CHARS);
			/* Build rpt entry for server */
			rv = build_discovered_server_rpt(handler,
					&info_result, &resource_id);
			if(rv != SA_OK){
				err("Failed to Add Server rpt for bay %d.",
					info_result.bayNumber);
				continue;
			}
			itostr(resource_id, &s);
			
			g_hash_table_insert(ov_handler->uri_rid, g_strdup
					(info_result.uri), g_strdup(s));
			/* Update resource_info structure with resource_id,
			 * serialNumber, and presence status
			 */
			wrap_free(s);
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					info_result.locationUri);
			rv = ov_rest_getenclosureInfoArray(handler, 
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
                                        ov_rest_update_resource_status(
						&enclosure->server,
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
                        }

			/* Build rdr entry for server */
			ov_rest_build_server_rdr(handler, resource_id, 
				&info_result);	

	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	wrap_free(server_doc);
	return SA_OK;	
}

/**
 * ov_rest_build_drive_enclosure_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to the drive Enclosure Info structure.
 *      @rpt:         Pointer to rpt to be filled.
 *
 * Purpose:
 *      This routine should be called to during discovery/re-discovery phase
 *      and when a new blade gets inserted.  It populates the Drive Enclosure
 *	/Storage Blade  RPT information common to discovered and insterted 
 *	blades.  The caller will fill in the information specific to the manner
 *	in which the blade was found. For example, the hotswap state could be 
 *	different in the case of discovered vs. inserted blades, because an 
 *	inserted blade goes through the pending state, while a discovered blade 
 *	doesn't.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT ov_rest_build_drive_enclosure_rpt(struct oh_handler_state *oh_handler,
                          struct driveEnclosureInfo *response,
                          SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path = {{{0}}};
        char *entity_root = NULL;
        SaHpiResourceIdT enc_rid;
        SaHpiRptEntryT *enc_rpt = NULL;
        struct enclosureStatus *enclosure = NULL;
        struct ov_rest_handler* ov_handler = NULL;

        if (oh_handler == NULL || response == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        ov_handler = (struct ov_rest_handler *)oh_handler->data;
        entity_root = (char *) g_hash_table_lookup(oh_handler->config,
                        "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed for the drive enclosure"
			" in bay %d", response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }


        /* Populate the rpt with the details of the server */
        memset(rpt, 0, sizeof(SaHpiRptEntryT));
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                SAHPI_CAPABILITY_RESET |
                SAHPI_CAPABILITY_MANAGED_HOTSWAP |
                SAHPI_CAPABILITY_RESOURCE |
                SAHPI_CAPABILITY_FRU |
                SAHPI_CAPABILITY_SENSOR |
                SAHPI_CAPABILITY_CONTROL |
                SAHPI_CAPABILITY_INVENTORY_DATA;

        if(response->bayNumber){

                rpt->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
                rpt->ResourceEntity.Entry[2].EntityLocation = 0;
                rpt->ResourceEntity.Entry[1].EntityType = 
					SAHPI_ENT_SYSTEM_CHASSIS;
		enclosure = ov_handler->ov_rest_resources.enclosure;
		while(enclosure){
			if(strstr(response->locationUri, enclosure->serialNumber)){
				break;
			}
			enclosure = enclosure->next;
		}
		if(enclosure){
			enc_rid = enclosure->enclosure_rid; 
			enc_rpt = oh_get_resource_by_id(oh_handler->rptcache, 
						enc_rid);
		}else{
			err("Could not find the associated enclosure"
				" for the drive enclosure in bay %d, parent"
				" location uri %s",
				response->bayNumber,
				response->locationUri);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
                rpt->ResourceEntity.Entry[1].EntityLocation = 
			enc_rpt->ResourceEntity.Entry[0].EntityLocation;

                rpt->ResourceEntity.Entry[0].EntityType =
                        SAHPI_ENT_DISK_BLADE;
                rpt->ResourceEntity.Entry[0].EntityLocation= 
						response->bayNumber;
        }
        else{
                rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
                rpt->ResourceEntity.Entry[1].EntityLocation = 0;
                rpt->ResourceEntity.Entry[0].EntityType =
                        SAHPI_ENT_RACK_MOUNTED_SERVER;
                rpt->ResourceEntity.Entry[0].EntityLocation =
                        ++ov_handler->current_rms_count;

        }

        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("Internal error (oh_concat_ep call) for drive enclosure"
			" in bay %d", response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
        switch(response->drvEncStatus){
                case OK:
                        rpt->ResourceSeverity = SAHPI_OK;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                case Disabled:
                        rpt->ResourceSeverity = SAHPI_CRITICAL;
                        rpt->ResourceFailed = SAHPI_TRUE;
                        break;
                case Warning:
                        rpt->ResourceSeverity = SAHPI_MINOR;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                case Critical:
                        rpt->ResourceSeverity = SAHPI_CRITICAL;
                        rpt->ResourceFailed = SAHPI_FALSE;
                        break;
                default:
                        rpt->ResourceSeverity = SAHPI_MAJOR;
                        rpt->ResourceFailed = SAHPI_TRUE;
        }
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(response->model);
        rpt->ResourceTag.DataLength = strlen(response->model);
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *) (rpt->ResourceTag.Data),
                       rpt->ResourceTag.DataLength + 1, "%s", response->model);

        /* set default hotswap capability */
        if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
                rpt->HotSwapCapabilities =
                        SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
        } else {
                rpt->HotSwapCapabilities = 0;
        }
        return SA_OK;
}

/**
 * build_discovered_drive_enclosure_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to the driveEnclosureInfo structure.
 *      @resource_id: Pointer to the resource id.
 *
 * Purpose:
 *      Populate the Drive enclosure/Storage blade RPT with aid of 
 *      ov_rest_build_drive_enclosure_rpt() and add hotswap state information 
 *      to the returned rpt. Pushes the RPT entry to plugin RPTable
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_discovered_drive_enclosure_rpt(
                                     struct oh_handler_state *oh_handler,
                                     struct driveEnclosureInfo *response,
                                     SaHpiResourceIdT *resource_id)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT state;
        struct ovRestHotswapState *hotswap_state = NULL;
        SaHpiRptEntryT rpt = {0};

        if (oh_handler == NULL || response == NULL ||
            resource_id == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if(ov_rest_build_drive_enclosure_rpt(oh_handler, response, &rpt) 
								!= SA_OK) {
                err("Building Rpt failed during discovery for drive enclosure"
				" in bay %d", response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the power state of the Drive Enclosure/ Storage blade state.
 	 * The hotswap state of the server will be
         * maintained in the private data area of the server RPT.
         */
        if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
                switch (response->powerState) {
                        case Off:
                                state = SAHPI_POWER_OFF;
                                break;
                        case On:
                                state = SAHPI_POWER_ON;
                                break;
                        case PoweringOff:
                                state = SAHPI_POWER_OFF;
                                break;
                        case PoweringOn:
                                state = SAHPI_POWER_ON;
                                break;
                        default:
                                err("Unknown Power State %d detected"
					" for drive enclosure in bay"
					" %d", response->powerState,
						response->bayNumber);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                }

                hotswap_state = (struct ovRestHotswapState *)
                        g_malloc0(sizeof(struct ovRestHotswapState));
                if (hotswap_state == NULL) {
                        err("Out of memory");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }

                switch (state) {
                        case SAHPI_POWER_ON:
                        case SAHPI_POWER_CYCLE:
                                hotswap_state->currentHsState =
                                        SAHPI_HS_STATE_ACTIVE;
                                break;
                        case SAHPI_POWER_OFF:
                                hotswap_state->currentHsState =
                                        SAHPI_HS_STATE_INACTIVE;
                                break;
                        default:
                                err("Unknown power status %d for"
					" drive enclosure in bay %d",
					state, response->bayNumber);
                                wrap_g_free(hotswap_state);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }
        /* Add the Drive Enclosure rpt to the plugin RPTable */
        rv = oh_add_resource(oh_handler->rptcache, &rpt, hotswap_state, 0);
        if (rv != SA_OK) {
                err("Failed to add drive enclosure rpt in bay %d",
						response->bayNumber);
                wrap_g_free(hotswap_state);
                return rv;
        }

        *resource_id = rpt.ResourceId;
        return SA_OK;
}

/**
* ov_rest_build_drive_enclosure_rdr:
*      @oh_handler:  Pointer to openhpi handler.
*      @resource_id: Resource id.
*      @response:    Drive enclosure info response structure.
*
* Purpose:
*      Populate the Drive Enclosure RDR.
*      Pushes the RDR entry to plugin RPTable.
*
* Detailed Description:
*      - Creates the inventory RDR
*
* Return values:
*      SA_OK                     - on success.
*      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
*      SA_ERR_HPI_INVALID_RESOURCE - on wrong resource id
**/
SaErrorT ov_rest_build_drive_enclosure_rdr(struct oh_handler_state *oh_handler,
                          SaHpiResourceIdT resource_id,
                          struct driveEnclosureInfo *response)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiRptEntryT *rpt = NULL;
        struct ov_rest_inventory *inventory = NULL;
	char *thermal_doc = NULL;

        if (oh_handler == NULL ) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        rpt = oh_get_resource_by_id (oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for drive enclosure in bay %d with "
			"resource id %d", response->bayNumber, resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        /* Build inventory rdr for the Drive Enclosure */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = ov_rest_build_drive_enclosure_inv_rdr(oh_handler, resource_id,
                                  &rdr, &inventory, response);
        if (rv != SA_OK) {
                err("Failed to get inventory RDR for Drive Enclosure in bay %d"
			" with resource id %d",
			response->bayNumber, resource_id);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for drive enclosure in bay %d with "
			"resource id %d", response->bayNumber, resource_id);
                return rv;
        }

        /* Build power control rdr for server */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_PWR_CNTRL, 0, 0);

        /* Build UID control rdr for server */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_UID_CNTRL, 0, 0);
	wrap_g_free(thermal_doc);
        return SA_OK;
}

/** ov_rest_discover_drive_enclosure
 * 	@handler: Pointer to openhpi handler
 *
 * Purpose:
 * 	Discover the Drive Enclosure/Storage Blade.
 *
 * Return values:
 * 	SA_OK			- on success.
 *
 **/
SaErrorT ov_rest_discover_drive_enclosure(struct oh_handler_state *handler)
{
	SaErrorT rv = 0;
	SaHpiResourceIdT resource_id;
	struct ov_rest_handler *ov_handler = NULL;
	struct driveEnclosureInfoArrayResponse response = {0};
	struct driveEnclosureInfo info_result = {0};
	char *drive_enc_doc = NULL,*s = NULL;
	int i = 0, arraylen = 0;
	char* blade_name = NULL;
	json_object *jvalue = NULL;
	struct enclosureStatus *enclosure = NULL;

	ov_handler = (struct ov_rest_handler *) handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_DRIVE_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getdriveEnclosureInfoArray(handler, 
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
		/* Copy the blade name from response for future processing */
		ov_rest_lower_to_upper(info_result.model,
				strlen(info_result.model), blade_name,
				MAX_256_CHARS);
		/* Build rpt entry for server */
		rv = build_discovered_drive_enclosure_rpt(handler,
				&info_result, &resource_id);
		if(rv != SA_OK){
			err("Failed to add drive enclosure rpt for bay %d.",
					info_result.bayNumber);
			continue;
		}
		itostr(resource_id, &s);

		dbg("Uri = %s resource_id = %s, %d",
				info_result.uri,s, resource_id);
		g_hash_table_insert(ov_handler->uri_rid, 
				g_strdup(info_result.uri), g_strdup(s));
		wrap_free(s);
		/* Update resource_info structure with resource_id,
		 * serialNumber, and presence status
		 */
		enclosure = ov_handler->ov_rest_resources.enclosure;
		while(enclosure != NULL){
			if(strstr(enclosure->serialNumber,
						info_result.enc_serialNumber)){
				ov_rest_update_resource_status (
						&enclosure->server,
						info_result.bayNumber,
						info_result.serialNumber, 
						resource_id,
						RES_PRESENT,
						info_result.type);
				break;
			}
			enclosure = enclosure->next;
		}
		if(enclosure == NULL){
			CRIT("Enclosure data of the server"
					" serial number %s is unavailable",
					info_result.serialNumber);
		}

		/* Build rdr entry for server */
		rv = ov_rest_build_drive_enclosure_rdr(handler, 
				resource_id, &info_result);
		if(rv != SA_OK){
			err("Failed to add drive enclosure rdr for bay %d.",
				info_result.bayNumber);
			continue;
		}
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * ov_rest_build_interconnect_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response: Pointer to interconnectInfo response structure.
 *      @resource_id: Pointer to the resource id.
 *
 * Purpose:
 *      - Populate the (re)discovered interconnect RPT. This routine gets 
 *        called only when a switch  blade is (re)discovered.
 *      - Pushes the RPT entry to plugin RPTable
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_interconnect_rpt(struct oh_handler_state *oh_handler,
                                struct interconnectInfo *response,
                                SaHpiResourceIdT *resource_id)
{
	SaErrorT rv = SA_OK;
	SaHpiEntityPathT entity_path = {{{0}}};
	char *entity_root = NULL;
	struct ovRestHotswapState *hotswap_state = NULL;
	SaHpiRptEntryT rpt = {0};
	char temp[256];
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatus *enclosure = NULL;
	SaHpiResourceIdT enc_rid;
	SaHpiRptEntryT *enc_rpt = NULL;

	if (oh_handler == NULL || response->model == NULL ||
			resource_id == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	if(ov_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	entity_root = (char *) g_hash_table_lookup(oh_handler->config,
			"entity_root");
	rv = oh_encode_entitypath(entity_root, &entity_path);
	if (rv != SA_OK) {
		err("Encoding entity path failed for interconnect in"
				" bay %d", response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Populate the rpt with the details of the interconnect */
	memset(&rpt, 0, sizeof(SaHpiRptEntryT));
	rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
		SAHPI_CAPABILITY_RESET |
		SAHPI_CAPABILITY_RESOURCE |
		SAHPI_CAPABILITY_POWER |
		SAHPI_CAPABILITY_FRU |
		SAHPI_CAPABILITY_MANAGED_HOTSWAP |
		SAHPI_CAPABILITY_SENSOR |
		SAHPI_CAPABILITY_CONTROL |
		SAHPI_CAPABILITY_INVENTORY_DATA;
	rpt.ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
	rpt.ResourceEntity.Entry[2].EntityLocation = 0;
	rpt.ResourceEntity.Entry[1].EntityType = SAHPI_ENT_SYSTEM_CHASSIS;
	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure){
		if(strstr(response->locationUri, enclosure->serialNumber)){
			break;
		}
		enclosure = enclosure->next;
	}
	if(enclosure){
		enc_rid = enclosure->enclosure_rid; 
		enc_rpt = oh_get_resource_by_id(oh_handler->rptcache, 
				enc_rid);
	}else{
		err("Could not find the associated enclosure"
				" for the interconnect in bay %d, "
				" parent location uri %s",
				response->bayNumber,
				response->locationUri);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rpt.ResourceEntity.Entry[1].EntityLocation = 
		enc_rpt->ResourceEntity.Entry[0].EntityLocation;
	rpt.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_SWITCH_BLADE;
	rpt.ResourceEntity.Entry[0].EntityLocation = response->bayNumber;
	rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
	if (rv != SA_OK) {
		err("Concat of entity path failed for the interconnec"
			" in bay %d", response->bayNumber);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));

	/* Check whether the interconnect blade is from Cisco Systems
	 * FIXME: Cisco interconnect blades have model starting with "Cisco"
	 *       If this format gets changed for any reason,
	 *       then Cisco interconnect blades will have manufacture id as 
	 *       HPE in ManufacturerId field of rpt entry.
	 *       If the interconnect model format changes,
	 *       please change the logic accordingly.
	 */
	ov_rest_lower_to_upper(response->model, strlen(response->model), temp, 
			256);
	if (strstr(temp, "CISCO") != NULL)
		rpt.ResourceInfo.ManufacturerId = CISCO_MANUFACTURING_ID;
	else
		rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;

	switch(response->interconnectStatus){
		case OK:
			rpt.ResourceSeverity = SAHPI_OK;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Disabled:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_TRUE;
			break;
		case Warning:
			rpt.ResourceSeverity = SAHPI_MINOR;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		case Critical:
			rpt.ResourceSeverity = SAHPI_CRITICAL;
			rpt.ResourceFailed = SAHPI_FALSE;
			break;
		default:
			rpt.ResourceSeverity = SAHPI_MAJOR;
			rpt.ResourceFailed = SAHPI_TRUE;
	}
	rpt.HotSwapCapabilities = SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
	rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
	rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	ov_rest_trim_whitespace(response->model);
	rpt.ResourceTag.DataLength = strlen(response->model);
	memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	snprintf((char *) (rpt.ResourceTag.Data),
			rpt.ResourceTag.DataLength + 1, "%s", response->model);

	hotswap_state = (struct ovRestHotswapState *)
		g_malloc0(sizeof(struct ovRestHotswapState));
	if (hotswap_state == NULL) {
		err("Out of memory");
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}

	/* Get the power state of the interconnect blade to determine
	 * the hotswap state.  The hotswap state of the interconnect
	 * shall be maintained in a private data area of the
	 * interconnect RPT.
	 */
	if (rpt.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {

		switch (response->powerState) {
			case (On):
				hotswap_state->currentHsState = 
				SAHPI_HS_STATE_ACTIVE;
				break;
			case (Off):
			case (Unknown):
				hotswap_state->currentHsState =
				SAHPI_HS_STATE_INACTIVE;
				break;
			default:
				err("Unknown Power State %d detected"
					" for interconnect in bay %d",
					response->powerState,
					response->bayNumber);
				wrap_g_free(hotswap_state);
				return SA_ERR_HPI_INTERNAL_ERROR;
		}
	}
	/* Add the interconnect rpt to the plugin RPTable */
	rv = oh_add_resource(oh_handler->rptcache, &rpt, hotswap_state, 0);
	if (rv != SA_OK) {
		err("Failed to add Interconnect RPT in bay %d",
						response->bayNumber);
		wrap_g_free(hotswap_state);
		return rv;
	}

	*resource_id = rpt.ResourceId;
	return SA_OK;
}

/**
 * ov_rest_build_interconnect_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource id.
 *      @response:    Pointer to interconnectInfo response structure.
 *
 * Purpose:
 *      Populate the interconnect blade RDR.
 *      Pushes the RDRs to plugin RPTable.
 *
 * Detailed Description:
 *         - Creates the inventory RDR.
 *         - Creates the temperature, operational status sensor RDR.
 *         - Creates UID and power control RDR.
 *         - This funtion callers
 *           discover_interconnect
 *           process_interconnect_insertion_event
 *           process_interconnect_info_event
 *           add_interconnect
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_interconnect_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct interconnectInfo *response)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_inventory *inventory = NULL;
        SaHpiRdrT rdr = {0};
	struct ov_rest_sensor_info *sensor_info=NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val = 0;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Build inventory rdr for interconnect */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = build_interconnect_inv_rdr(oh_handler, resource_id,
                                        &rdr, &inventory, response);
        if (rv != SA_OK) {
                err("Failed to get interconnect inventory RDR for resource "
				"id %d", resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for interconnect resource id %d",
								resource_id);
                return rv;
        }

        /* Build power control rdr for interconnect */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_PWR_CNTRL, 0, 0);

        /* Build UID control rdr for interconnect */
        OV_REST_BUILD_CONTROL_RDR(OV_REST_UID_CNTRL, 0, 0);

        /* Build operational status sensor rdr */
        switch (response->interconnectStatus ) {
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
 * ov_rest_discover_sas_interconnect:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the sas-interconnect blades.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT ov_rest_discover_sas_interconnect(struct oh_handler_state *handler) 
{
	SaErrorT rv = 0;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo result = {0};
	SaHpiResourceIdT resource_id;
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enc_info = {{0}};
	struct enclosureStatus *enclosure = NULL;
	char *interconnect_doc = NULL, *enclosure_doc = NULL, *s = NULL;
	json_object *jvalue = NULL;
	int i = 0, arraylen = 0;

	if (handler == NULL || handler->data == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_SAS_INTERCONNECT_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getinterconnectInfoArray(handler, &response,
			ov_handler->connection,
			interconnect_doc);
	if (rv != SA_OK || response.interconnect_array == NULL) {
		CRIT("No response from ov_rest_getinterconnectInfoArray for "
						"SAS interconnects");
		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.interconnect_array) !=
			json_type_array) {
		CRIT("No sas-interconnects arrays returned");
		return SA_OK;
	}
	arraylen = json_object_array_length(response.interconnect_array);
	while(1){
		for (i=0; i< arraylen; i++){
			if (ov_handler->shutdown_event_thread == SAHPI_TRUE) {
				dbg("shutdown_event_thread set. Returning in "
					"thread %p", g_thread_self());
				return SA_OK;
			}
			memset(&result, 0, sizeof(struct interconnectInfo));
			jvalue = json_object_array_get_idx (
					response.interconnect_array, i);
			if (!jvalue) {
				CRIT("Invalid response for the sas-interconnect"
						" in bay %d", i + 1);
				continue;
			}
			ov_rest_json_parse_interconnect(jvalue,&result);
			rv = ov_rest_build_interconnect_rpt(handler,
					&result, &resource_id);
			if(rv != SA_OK){
				err("Failed to Add sas-interconnect rpt for "
					"bay %d.", result.bayNumber);
				continue;
			}
			itostr(resource_id ,&s);
			dbg("Uri = %s resource_id = %s, %d",
					result.uri,s,resource_id);
			g_hash_table_insert(ov_handler->uri_rid, 
					g_strdup(result.uri),
					g_strdup(s));
			wrap_free(s);
			/* Update resource_info structure with resource_id,
			 * serialNumber, and presence status
			 */
			/* Find the Enclosure for this interconnect to update 
 			 * the Resource matrix table */
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					result.locationUri);
			rv = ov_rest_getenclosureInfoArray(handler, 
					&enclosure_response, 
					ov_handler->connection,
					enclosure_doc);
			if(rv != SA_OK || enclosure_response.
						enclosure_array == NULL) {
				CRIT("Faild to get the response from "
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
					ov_rest_update_resource_status (
						&enclosure->interconnect,
						result.bayNumber,
							result.serialNumber,
							resource_id,
							RES_PRESENT,
							result.type);
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the sas interconnect"
					" serial number %s is unavailable",
						result.serialNumber);
				continue;
			}

			/* Build rdr entry for interconnect */
			ov_rest_build_interconnect_rdr(handler, 
						resource_id, &result);
			wrap_g_free(enclosure_doc);
			wrap_g_free(s);
		}
		ov_rest_wrap_json_object_put(response.root_jobj);
		if(response.next_page[0] == '\0'){
			break;
		}
		else{
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					response.next_page);
			memset(&response, 0, sizeof(response));
			rv = ov_rest_getinterconnectInfoArray(handler, 
					&response,
					ov_handler->connection,
					interconnect_doc);
			if (rv != SA_OK || response.interconnect_array == NULL){
				CRIT("No response from "
					"ov_rest_getinterconnectInfoArray "
					"for sas-interconnects");
				return SA_OK;
			}
			/* Checking for json object type, if it is not array, 
 			* return */
			if (json_object_get_type(response.interconnect_array) !=
					json_type_array) {
				CRIT("No sas-interconnects arrays returned");
				return SA_OK;
			}
			arraylen = json_object_array_length(
						response.interconnect_array);
		}
	}
	return rv;	
}

/**
 * ov_rest_discover_interconnect:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the interconnect blades.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT ov_rest_discover_interconnect(struct oh_handler_state *handler)
{
	SaErrorT rv = 0;
	SaHpiResourceIdT resource_id;
	struct ov_rest_handler *ov_handler = NULL;
	struct interconnectInfoArrayResponse response = {0};
	struct interconnectInfo result = {0};
	struct enclosureInfoArrayResponse enclosure_response = {0};
	struct enclosureInfo enc_info = {{0}};
	char* interconnect_doc = NULL, *enclosure_doc = NULL, *s = NULL;   
	int i = 0,arraylen = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL;

	if (handler == NULL || handler->data == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	ov_handler = (struct ov_rest_handler *) handler->data;
	WRAP_ASPRINTF(&ov_handler->connection->url, OV_INTERCONNECT_URI, 
			ov_handler->connection->hostname);
	rv = ov_rest_getinterconnectInfoArray(handler, &response, 
			ov_handler->connection, interconnect_doc);
	if(rv != SA_OK || response.interconnect_array == NULL){
		CRIT("Failed to get the response from "
			"ov_rest_getinterconnectInfoArray for "
				"interconnects");

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
			rv = ov_rest_build_interconnect_rpt(handler,
					&result, &resource_id);
			if(rv != SA_OK){
				err("Failed to Add Interconnect rpt for "
						"bay %d.", result.bayNumber);
				continue;
			}
			itostr(resource_id ,&s);
			dbg("Uri = %s resource_id = %s, %d",result.uri,s,
								resource_id);
			g_hash_table_insert(ov_handler->uri_rid, 
					g_strdup(result.uri),
					g_strdup(s));
			wrap_free(s);
			/* Update resource_info structure with resource_id,
			 * serialNumber, and presence status
			 */
			/* Find the Enclosure for this interconnect to update 
 			 * the Resource matrix table */
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					result.locationUri);
			rv = ov_rest_getenclosureInfoArray(handler, 
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
					ov_rest_update_resource_status(
							&enclosure->interconnect,
							result.bayNumber, 
							result.serialNumber, 
							resource_id, RES_PRESENT,
							result.type);
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the interconnect with"
					" serial number %s is unavailable",
						result.serialNumber);
				continue;
			}

			/* Build rdr entry for server */
			ov_rest_build_interconnect_rdr(handler, 
							resource_id, &result);
			wrap_g_free(enclosure_doc);
			wrap_g_free(s);
		}
		ov_rest_wrap_json_object_put(response.root_jobj);
		if(response.next_page[0] == '\0'){
			break;
		}else {
			WRAP_ASPRINTF(&ov_handler->connection->url, "https://%s%s",
					ov_handler->connection->hostname,
					response.next_page);
			memset(&response, 0, sizeof(response));
			rv = ov_rest_getinterconnectInfoArray(handler, 
					&response,
					ov_handler->connection, 
					interconnect_doc);
			if(rv != SA_OK || response.interconnect_array == NULL){
				CRIT("Failed to get the response from "
					"ov_rest_getinterconnectInfoArray "
					"for interconnects");

				return SA_OK;
			}

			/* Checking for json object type, if it is not array, 
 			 * return */
			if (json_object_get_type(response.interconnect_array) !=
					json_type_array) {
				CRIT("Not adding any interconnects as no "
					"array returned");
				return SA_OK;
			}

			/*Getting the length of the array*/
			arraylen = json_object_array_length(
					response.interconnect_array);

		}
	}
	return rv;
}

/**
 * build_powersupply_inv_rdr:
 *      @oh_handler: Handler data pointer.
 *      @resource_id: Resource id.
 *      @rdr: Rdr Structure for inventory data.
 *      @inventory: Rdr private data structure.
 *      @response: Pointer to the power sypply info response.
 *
 * Purpose:
 *      Creates an inventory rdr for power supply.
 *
 * Detailed Description:
 *      - Populates the power supply inventory rdr with default values.
 *      - Inventory data repository is created and associated as part of the
 *        private data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK - On Success.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory
 **/
SaErrorT build_powersupply_inv_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
                                struct powersupplyInfo *response)
{
       SaErrorT rv = SA_OK;
       char power_inv_str[] = POWER_SUPPLY_INVENTORY_STRING;
       struct ov_rest_inventory *local_inventory = NULL;
       struct ovRestArea *head_area = NULL;
       SaHpiInt32T add_success_flag = 0;
       SaHpiInt32T area_count = 0;
       SaHpiRptEntryT *rpt = NULL;

       if (oh_handler == NULL || rdr == NULL || response == NULL ||
                       inventory == NULL) {
               err("Invalid parameter.");
               return SA_ERR_HPI_INVALID_PARAMS;
       }

       /* Get the rpt entry of the resource */
       rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
       if (rpt == NULL) {
               err("RPT is NULL for powersupply in bay %d with resource id %d",
					response->bayNumber, resource_id);
               return SA_ERR_HPI_INTERNAL_ERROR;
       }

       /* Populating the inventory rdr with rpt values for the resource */
       rdr->Entity = rpt->ResourceEntity;
       rdr->RecordId = 0;
       rdr->RdrType  = SAHPI_INVENTORY_RDR;
       rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
       rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
       rdr->IdString.Language = SAHPI_LANG_ENGLISH;
       ov_rest_trim_whitespace(response->model);
       rdr->IdString.DataLength = strlen(response->model);
       snprintf((char *)rdr->IdString.Data,
                       strlen(response->model)+ 1,
                       "%s",response->model );

       /* Create inventory IDR and populate the IDR header */
       local_inventory = (struct ov_rest_inventory*)
               g_malloc0(sizeof(struct ov_rest_inventory));
       if (!local_inventory) {
               err("OV REST out of memory");
               return SA_ERR_HPI_OUT_OF_MEMORY;
       }
       local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
       local_inventory->info.idr_info.IdrId =
               rdr->RdrTypeUnion.InventoryRec.IdrId;
       local_inventory->info.idr_info.UpdateCount = 1;
       local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
       local_inventory->info.idr_info.NumAreas = 0;
       local_inventory->info.area_list = NULL;
       local_inventory->comment =
               (char *)g_malloc0(strlen(power_inv_str) + 1);
       strcpy(local_inventory->comment, power_inv_str);

       /* Create and add product area if resource name and/or manufacturer
        * information exist
        */
       rv = ov_rest_add_product_area(&local_inventory->info.area_list,
                       response->model,
                       "HPE",
                       &add_success_flag);
       if (rv != SA_OK) {
               err("Add product area failed for powersupply resource id %d",
							resource_id);
               return rv;
       }

       /* add_success_flag will be true if product area is added,
        * if this is the first successful creation of IDR area, then have
        * area pointer stored as the head node for area list
        */
       if (add_success_flag != SAHPI_FALSE) {
               (local_inventory->info.idr_info.NumAreas)++;
               if (area_count == 0) {
                       head_area = local_inventory->info.area_list;
               }
               ++area_count;
       }

       /* Create and add board area if resource part number and/or
        * serial number exist
        */
       rv = ov_rest_add_board_area(&local_inventory->info.area_list,
                       response->partNumber,
                       response->serialNumber,
                       &add_success_flag);
       if (rv != SA_OK) {
               err("Add board area failed for powersupply resource id %d",
						resource_id);
               return rv;
       }
       if (add_success_flag != SAHPI_FALSE) {
               (local_inventory->info.idr_info.NumAreas)++;
               if (area_count == 0) {
                       head_area = local_inventory->info.area_list;
               }
               ++area_count;
       }

       local_inventory->info.area_list = head_area;
       *inventory = local_inventory;
       return SA_OK;
}

/**
 * ov_rest_build_powersupply_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource id.
 *      @response:    Power supply info response structure.
 *
 * Purpose:
 *      Populate the power supply unit RDR.
 *      Pushes the RDR entry to plugin RPTable.
 *
 * Detailed Description:
 *         - Creates the inventory RDR.
 *         - Creates the power status, operational status sensor RDR.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_powersupply_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct powersupplyInfo *response)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_inventory *inventory = NULL;
        SaHpiRdrT rdr = {0};
        struct ov_rest_sensor_info *sensor_info=NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Build inventory rdr for interconnect */
        memset(&rdr, 0, sizeof(SaHpiRdrT));
        rv = build_powersupply_inv_rdr(oh_handler, resource_id,
                                        &rdr, &inventory, response);
        if (rv != SA_OK) {
                err("Failed to build powersupply inventory RDR of resource "
						"id %d", resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr for powersupply resource id %d",
								resource_id);
                return rv;
        }

	switch (response->status) {
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
 
        /* Build operational status sensor rdr */
        OV_REST_BUILD_ENABLE_SENSOR_RDR(OV_REST_SEN_OPER_STATUS, sensor_val);
 
        return SA_OK;
}

/**
 * ov_rest_build_powersupply_rpt:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response: Power supply info response structure.
 *      @resource_id: Pointer to resource id.
 *      @enclosure_location: Enclosure location of Integer type.
 *
 * Purpose:
 *      Populate the power supply unit RPT.
 *      Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_powersupply_rpt(struct oh_handler_state *oh_handler,
                                struct powersupplyInfo *response,
                               SaHpiResourceIdT *resource_id,
                                int enclosure_location)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path = {{{0}}};
        char *entity_root = NULL;
        struct ovRestHotswapState *hotswap_state = NULL;
        SaHpiRptEntryT rpt = {0};
        struct ov_rest_handler *ov_handler = NULL;


        if (oh_handler == NULL || response->model == NULL ||
                        resource_id == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;
        if(ov_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *) g_hash_table_lookup(oh_handler->config,
                        "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed for powersupply in bay %d",
							response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Populate the rpt with the details of the interconnect */
        memset(&rpt, 0, sizeof(SaHpiRptEntryT));
        rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                   SAHPI_CAPABILITY_RESOURCE |
                                   SAHPI_CAPABILITY_FRU |
                                   SAHPI_CAPABILITY_SENSOR |
                                   SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt.ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
        rpt.ResourceEntity.Entry[2].EntityLocation = 0;
        rpt.ResourceEntity.Entry[1].EntityType = SAHPI_ENT_SYSTEM_CHASSIS;
        rpt.ResourceEntity.Entry[1].EntityLocation = enclosure_location;
        rpt.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_POWER_SUPPLY;
        rpt.ResourceEntity.Entry[0].EntityLocation = response->bayNumber;
        rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
        if (rv != SA_OK) {
                err("Concat of entity path failed for powersupply in "
			"bay %d", response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));
        switch(response->status){
                case OK:
                        rpt.ResourceSeverity = SAHPI_OK;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Disabled:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_TRUE;
                        break;
                case Warning:
                        rpt.ResourceSeverity = SAHPI_MINOR;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Critical:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                default:
                        rpt.ResourceSeverity = SAHPI_MAJOR;
                        rpt.ResourceFailed = SAHPI_TRUE;
        }
        rpt.HotSwapCapabilities = 0x0;
        rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
        rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(response->model);
        rpt.ResourceTag.DataLength = strlen(response->model);
        memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *) (rpt.ResourceTag.Data),
                        rpt.ResourceTag.DataLength + 1, "%s", response->model);

        /* Add the interconnect rpt to the plugin RPTable */
        rv = oh_add_resource(oh_handler->rptcache, &rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add PowerSupply RPT in bay %d",
						response->bayNumber);
                wrap_g_free(hotswap_state);
                return rv;
        }

        *resource_id = rpt.ResourceId;
        return SA_OK;
}

/**
 * ov_rest_discover_powersupply:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discover the power supply.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK    - on success.
 *      Error    - on failure.                 
 **/
SaErrorT ov_rest_discover_powersupply(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct powersupplyInfo result = {0};
	SaHpiResourceIdT resource_id;
	char* enclosure_doc = NULL, *s = NULL;
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
		return rv;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array) {
		CRIT("Not adding power supplies, no enclosure array returned");
		return SA_OK;
	}

	arraylen = json_object_array_length(response.enclosure_array);
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue) {
			CRIT("Invalid response for the enclosure %d",
				i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue, &enclosure_result);
		jvalue_ps_array = ov_rest_wrap_json_object_object_get(jvalue,
					"powerSupplyBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_ps_array) != json_type_array) {
			CRIT("Not adding power supplied to enclosure %d,"
				" no array returned for that",i);
			return SA_OK;
		}		

		for(j = 0; j < enclosure_result.powerSupplyBayCount; j++){
			jvalue_ps = json_object_array_get_idx(jvalue_ps_array, 
					j);
			if (!jvalue_ps) {
				CRIT("Invalid response for the powersupply"
					" in bay %d", j + 1);
				continue;
			}
			ov_rest_json_parse_powersupply(jvalue_ps, &result);
			if(result.presence == Absent)
				continue;
			rv = ov_rest_build_powersupply_rpt(oh_handler, &result,
					&resource_id, i+1);
			if (rv != SA_OK) {
				err("Build PowerSupply rpt failed placed in "
					"bay %d", j + 1);
				return rv;
			}
			enclosure = (struct enclosureStatus *)
				ov_handler->ov_rest_resources.enclosure;
			while(enclosure != NULL){
				if(!strcmp(enclosure->serialNumber,
					enclosure_result.serialNumber)){
					ov_rest_update_resource_status(
						&enclosure->ps_unit,
						result.bayNumber,
						result.serialNumber, 
						resource_id, RES_PRESENT,
						result.type);
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the powersupply"
					" serial number %s is unavailable",
					result.serialNumber);
			}

			rv = ov_rest_build_powersupply_rdr(oh_handler,
					resource_id, &result);
			if (rv != SA_OK) {
				err("Build PowerSupply rdr failed placed in "
					"bay %d", j + 1);
				wrap_free(s);
				ov_rest_wrap_json_object_put(
						response.root_jobj);
				return rv;
			}
		}
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * ov_rest_build_fan_inv_rdr:
 *      @oh_handler: Handler data pointer.
 *      @resource_id: Resource id.
 *      @rdr: Rdr Structure for inventory data.
 *      @inventory: Rdr private data structure.
 *      @response: Pointer to the fan info response.
 *
 * Purpose:
 *      Creates an inventory rdr for fan
 *
 * Detailed Description:
 *      - Populates the fan inventory rdr with default values.
 *      - Inventory data repository is created and associated as part of the
 *        private data area of the Inventory RDR.
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - On internal error.
 *      SA_ERR_HPI_OUT_OF_MEMORY - Request failed due to insufficient memory.
 **/
SaErrorT ov_rest_build_fan_inv_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                SaHpiRdrT *rdr,
                                struct ov_rest_inventory **inventory,
                                struct fanInfo *response)
{
       SaErrorT rv = SA_OK;
       char fan_inv_str[] = FAN_INVENTORY_STRING;
       struct ov_rest_inventory *local_inventory = NULL;
       struct ovRestArea *head_area = NULL;
       SaHpiInt32T add_success_flag = 0;
       SaHpiInt32T area_count = 0;
       SaHpiRptEntryT *rpt = NULL;

       if (oh_handler == NULL || rdr == NULL || response == NULL ||
                       inventory == NULL) {
               err("Invalid parameter.");
               return SA_ERR_HPI_INVALID_PARAMS;
       }

       /* Get the rpt entry of the resource */
       rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
       if (rpt == NULL) {
               err("RPT is NULL for fan in bay %d with resource id %d",
					response->bayNumber, resource_id);
               return SA_ERR_HPI_INTERNAL_ERROR;
       }

       /* Populating the inventory rdr with rpt values for the resource */
       rdr->Entity = rpt->ResourceEntity;
       rdr->RecordId = 0;
       rdr->RdrType  = SAHPI_INVENTORY_RDR;
       rdr->RdrTypeUnion.InventoryRec.IdrId = SAHPI_DEFAULT_INVENTORY_ID;
       rdr->IdString.DataType = SAHPI_TL_TYPE_TEXT;
       rdr->IdString.Language = SAHPI_LANG_ENGLISH;
       ov_rest_trim_whitespace(response->model);
       rdr->IdString.DataLength = strlen(response->model);
       snprintf((char *)rdr->IdString.Data,
                       strlen(response->model)+ 1,
                       "%s",response->model );

       /* Create inventory IDR and populate the IDR header */
       local_inventory = (struct ov_rest_inventory*)
               g_malloc0(sizeof(struct ov_rest_inventory));
       if (!local_inventory) {
               err("OV REST out of memory");
               return SA_ERR_HPI_OUT_OF_MEMORY;
       }
       local_inventory->inv_rec.IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;
       local_inventory->info.idr_info.IdrId =
               rdr->RdrTypeUnion.InventoryRec.IdrId;
       local_inventory->info.idr_info.UpdateCount = 1;
       local_inventory->info.idr_info.ReadOnly = SAHPI_FALSE;
       local_inventory->info.idr_info.NumAreas = 0;
       local_inventory->info.area_list = NULL;
       local_inventory->comment =
               (char *)g_malloc0(strlen(fan_inv_str) + 1);
       strcpy(local_inventory->comment, fan_inv_str);

       /* Create and add product area if resource name and/or manufacturer
        * information exist
        */
       rv = ov_rest_add_product_area(&local_inventory->info.area_list,
                       response->model,
                       "HPE",
                       &add_success_flag);
       if (rv != SA_OK) {
               err("Add product area failed for fan resource id %d",
							resource_id);
               return rv;
       }

       /* add_success_flag will be true if product area is added,
        * if this is the first successful creation of IDR area, then have
        * area pointer stored as the head node for area list
        */
       if (add_success_flag != SAHPI_FALSE) {
               (local_inventory->info.idr_info.NumAreas)++;
               if (area_count == 0) {
                       head_area = local_inventory->info.area_list;
               }
               ++area_count;
       }

       /* Create and add board area if resource part number and/or
        * serial number exist
        */
       rv = ov_rest_add_board_area(&local_inventory->info.area_list,
                       response->partNumber,
                       response->serialNumber,
                       &add_success_flag);
       if (rv != SA_OK) {
               err("Add board area failed for fan resource id %d",
							resource_id);
               return rv;
       }
       if (add_success_flag != SAHPI_FALSE) {
               (local_inventory->info.idr_info.NumAreas)++;
               if (area_count == 0) {
                       head_area = local_inventory->info.area_list;
               }
               ++area_count;
       }
       local_inventory->info.area_list = head_area;
       *inventory = local_inventory;
       return SA_OK;
}

/**
 * ov_rest_build_fan_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource id.
 *      @response:    Fan info response structure.
 *
 * Purpose:
 *      - Creates the inventory RDR.
 *      - Creates operational status sensor RDR.
 *      - Pushes the RDRs to plugin RPTable.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_fan_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct fanInfo *response)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_inventory *inventory = NULL;
	SaHpiRdrT rdr = {0};
        struct ov_rest_sensor_info *sensor_info=NULL;
        SaHpiInt32T sensor_status;
        SaHpiInt32T sensor_val;

	if (oh_handler == NULL || response == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Build inventory rdr for interconnect */
	memset(&rdr, 0, sizeof(SaHpiRdrT));
	rv = ov_rest_build_fan_inv_rdr(oh_handler, resource_id,
			&rdr, &inventory, response);
	if (rv != SA_OK) {
		err("Failed to build Fan inventory RDR for resource id %d",
						resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, &rdr, inventory, 0);
	if (rv != SA_OK) {
		err("Failed to add rdr for fan resource id %d", resource_id);
		return rv;
	}

        switch (response->status) {
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

        /* Build operational status sensor rdr */
        OV_REST_BUILD_ENABLE_SENSOR_RDR(OV_REST_SEN_OPER_STATUS,
                                        sensor_val)
	return SA_OK;
}

/**
 * ov_rest_build_fan_rpt:
 *      @oh_handler         : Pointer to openhpi handler.
 *      @response           : Fan info response structure.
 *      @resource_id        : Pointer to the resource id.
 *      @enclosure_location : Enclosure location of Integer type.
 *
 * Purpose:
 *      Populate the fan RPT.
 *      Pushes the RPT entry to plugin RPTable.
 *
 * Detailed Description:
 *      - Creates the Fan RPT entry.
 *      - Pushes the RPT entry to plugin RPTable.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_build_fan_rpt(struct oh_handler_state *oh_handler,
                                struct fanInfo *response,
                               SaHpiResourceIdT *resource_id,
                                int enclosure_location)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path = {{{0}}};
        char *entity_root = NULL;
        SaHpiRptEntryT rpt = {0};
        struct ov_rest_handler *ov_handler = NULL;

        if (oh_handler == NULL || response == NULL ||
                        resource_id == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;
        if(ov_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *) g_hash_table_lookup(oh_handler->config,
                        "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed for fan in bay %d",
							response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Populate the rpt with the details of the interconnect */
        memset(&rpt, 0, sizeof(SaHpiRptEntryT));
        rpt.ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                   SAHPI_CAPABILITY_RESOURCE |
                                   SAHPI_CAPABILITY_FRU |
                                   SAHPI_CAPABILITY_SENSOR |
                                   SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt.ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
        rpt.ResourceEntity.Entry[2].EntityLocation = 0;
        rpt.ResourceEntity.Entry[1].EntityType = SAHPI_ENT_SYSTEM_CHASSIS;
        rpt.ResourceEntity.Entry[1].EntityLocation = enclosure_location;
        rpt.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_FAN;
        rpt.ResourceEntity.Entry[0].EntityLocation = response->bayNumber;
        rv = oh_concat_ep(&(rpt.ResourceEntity), &entity_path);
        if (rv != SA_OK) {
                err("Concat of entity path failed for fan in bay %d",
					response->bayNumber);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        switch(response->status){
                case OK:
                        rpt.ResourceSeverity = SAHPI_OK;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Disabled:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_TRUE;
                        break;
                case Warning:
                        rpt.ResourceSeverity = SAHPI_MINOR;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                case Critical:
                        rpt.ResourceSeverity = SAHPI_CRITICAL;
                        rpt.ResourceFailed = SAHPI_FALSE;
                        break;
                default:
                        rpt.ResourceSeverity = SAHPI_MAJOR;
                        rpt.ResourceFailed = SAHPI_TRUE;
        }
        rpt.ResourceId = oh_uid_from_entity_path(&(rpt.ResourceEntity));
        rpt.HotSwapCapabilities = 0x0;
        rpt.ResourceInfo.ManufacturerId = HPE_MANUFACTURING_ID;
        rpt.ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt.ResourceTag.Language = SAHPI_LANG_ENGLISH;
        ov_rest_trim_whitespace(response->model);
        rpt.ResourceTag.DataLength = strlen(response->model);
        memset(rpt.ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *) (rpt.ResourceTag.Data),
                        rpt.ResourceTag.DataLength + 1, "%s", response->model);

        /* Add the interconnect rpt to the plugin RPTable */
        rv = oh_add_resource(oh_handler->rptcache, &rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add Fan %d RPT", response->bayNumber);
                return rv;
        }

        *resource_id = rpt.ResourceId;
        return SA_OK;
}

/**
 * ov_rest_discover_fan:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      Discovers the fan.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK     - on success.
 *      Error     - on failure.
 **/
SaErrorT ov_rest_discover_fan(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK; 
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureInfoArrayResponse response = {0};
	struct enclosureInfo enclosure_result = {{0}};
	struct fanInfo result = {0};
	SaHpiResourceIdT resource_id;
	char* enclosure_doc = NULL, *s = NULL;
	int i = 0,j = 0,arraylen = 0;
	struct enclosureStatus *enclosure = NULL;
	json_object *jvalue = NULL, *jvalue_fan = NULL, 
	*jvalue_fan_array = NULL;

	ov_handler = (struct ov_rest_handler *) oh_handler->data;

	WRAP_ASPRINTF(&ov_handler->connection->url, OV_ENCLOSURE_URI,
			ov_handler->connection->hostname);
	rv = ov_rest_getenclosureInfoArray(oh_handler, &response,
			ov_handler->connection, enclosure_doc);
	if(rv != SA_OK || response.enclosure_array == NULL) {
		CRIT(" No response from ov_rest_getenclosureInfoArray");
		return SA_OK;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response.enclosure_array) != json_type_array) {
		CRIT("Not adding fans as no enclosure arrays returned");
		return SA_ERR_HPI_INVALID_DATA;
	}

	arraylen = json_object_array_length(response.enclosure_array);
	for (i=0; i< arraylen; i++){
		jvalue = json_object_array_get_idx(response.enclosure_array,i);
		if (!jvalue) {
			CRIT("Invalid response for the enclosure in list %d",
				i + 1);
			continue;
		}
		ov_rest_json_parse_enclosure(jvalue, &enclosure_result);
		jvalue_fan_array = ov_rest_wrap_json_object_object_get(jvalue,
				"fanBays");
		/* Checking for json object type, if it is not array, return */
		if (json_object_get_type(jvalue_fan_array) != json_type_array) {
			CRIT("Fan array is not returned for enclosure %d",i+1); 
			return SA_OK;
		}

		for(j = 0; j < enclosure_result.fanBayCount; j++){
			jvalue_fan = json_object_array_get_idx(
					jvalue_fan_array, j);
			if (!jvalue_fan) {
				CRIT("Invalid response for the fan in bay %d",
					j + 1);
				continue;
			}	
			ov_rest_json_parse_fan(jvalue_fan, &result);
			if(result.presence == Absent)
				continue;
			rv = ov_rest_build_fan_rpt(oh_handler, &result,
					&resource_id, i+1);
			if (rv != SA_OK) {
				err("Build Fan rpt failed in bay %d", j + 1);
				ov_rest_wrap_json_object_put(
							response.root_jobj);
				return rv;
			}
			enclosure = (struct enclosureStatus *)
				ov_handler->ov_rest_resources.enclosure;
			while(enclosure != NULL){
				if(!strcmp(enclosure->serialNumber,
					enclosure_result.serialNumber)){
					ov_rest_update_resource_status(
						&enclosure->fan,
						result.bayNumber,
						result.serialNumber, 
						resource_id, RES_PRESENT,
						result.type);
					break;
				}
				enclosure = enclosure->next;
			}
			if(enclosure == NULL){
				CRIT("Enclosure data of the fan"
					" serial number %s is unavailable",
						result.serialNumber);
			}

			rv = ov_rest_build_fan_rdr(oh_handler,
					resource_id, &result);
			if (rv != SA_OK) {
				err("Build Fan rdr failed in bay %d", j + 1);
				wrap_free(s);
				ov_rest_wrap_json_object_put(
						response.root_jobj);
				return rv;
			}
		}
	}
	ov_rest_wrap_json_object_put(response.root_jobj);
	return SA_OK;
}

/**
 * ov_rest_build_temperature_sensor_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareThermalInfo structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the temperature sensors of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_temperature_sensor_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareThermalInfo server_therm_info,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;

        if (strcmp(server_therm_info.Health_state, "Absent")) {
                memset(&rdr, 0, sizeof(SaHpiRdrT));
                sensor = &(rdr.RdrTypeUnion.SensorRec);
                /* Populate the sensor rdr with default value */
                rdr.Entity = rpt->ResourceEntity;
                rdr.RdrType = SAHPI_SENSOR_RDR;
                sensor->Num = server_therm_info.Number;
                sensor->Type = SAHPI_TEMPERATURE;
                sensor->Category = SAHPI_EC_THRESHOLD;
                sensor->EnableCtrl = SAHPI_TRUE;
                sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
                sensor->Events = SAHPI_ES_UNSPECIFIED;
                sensor->DataFormat.IsSupported = SAHPI_TRUE;
                sensor->DataFormat.ReadingType =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor->DataFormat.BaseUnits = SAHPI_SU_DEGREES_C;
                sensor->DataFormat.ModifierUnits = SAHPI_SU_UNSPECIFIED;
                sensor->DataFormat.Percentage = SAHPI_FALSE;
                sensor->DataFormat.Range.Flags =
                                        SAHPI_SRF_MAX | SAHPI_SRF_NORMAL_MAX;
                sensor->DataFormat.Range.Max.IsSupported = SAHPI_TRUE;
                sensor->DataFormat.Range.Max.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor->DataFormat.Range.Max.Value.SensorFloat64 =
                                server_therm_info.LowerThresholdCritical;
                sensor->DataFormat.Range.NormalMax.IsSupported = SAHPI_TRUE;
                sensor->DataFormat.Range.NormalMax.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor->DataFormat.Range.NormalMax.Value.SensorFloat64 =
                                server_therm_info.LowerThresholdNonCritical;
                sensor->DataFormat.AccuracyFactor = 0;
                sensor->ThresholdDefn.IsAccessible = SAHPI_TRUE;
                sensor->ThresholdDefn.ReadThold =
                                SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR;
                sensor->ThresholdDefn.WriteThold = 0x0;
                sensor->Oem = 0;

                oh_init_textbuffer(&(rdr.IdString));
                oh_append_textbuffer(&(rdr.IdString), server_therm_info.Name);

                /* Sensor specific information is stored in this structure */
                sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
                if (sensor_info == NULL) {
                        err("OV_REST out of memory");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
                sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
                sensor_info->sensor_enable = SAHPI_TRUE;
                sensor_info->event_enable = SAHPI_FALSE;
                sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
                sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
                sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
                sensor_info->sensor_reading.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor_info->sensor_reading.Value.SensorFloat64 =
                                        server_therm_info.CurrentReading;
                sensor_info->threshold.UpCritical.IsSupported = SAHPI_TRUE;
                sensor_info->threshold.UpCritical.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor_info->threshold.UpCritical.Value.SensorFloat64 =
                                server_therm_info.LowerThresholdCritical;
                sensor_info->threshold.UpMajor.IsSupported = SAHPI_TRUE;
                sensor_info->threshold.UpMajor.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor_info->threshold.UpMajor.Value.SensorFloat64 =
                                server_therm_info.LowerThresholdNonCritical;

                rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
                if (rv != SA_OK) {
                        err("Failed to add temperature sensor rdr for "
					"resource id %d", rpt->ResourceId);
                        return rv;
                }
        }

        return SA_OK;
}

/**
 * ov_rest_build_fan_sensor_rdr_info:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareFanInfo structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the fan sensors of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_fan_sensor_rdr_info(struct oh_handler_state *oh_handler,
                struct serverhardwareFanInfo server_fan_info,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
	if(server_fan_info.Health_state == NULL){
		return SA_ERR_HPI_INVALID_PARAMS;
	}
        if (strcmp(server_fan_info.Health_state, "Absent")) {
                memset(&rdr, 0, sizeof(SaHpiRdrT));
                sensor = &(rdr.RdrTypeUnion.SensorRec);
                /* Populate the sensor rdr with default value */
                rdr.Entity = rpt->ResourceEntity;
                rdr.RdrType = SAHPI_SENSOR_RDR;
                sensor->Num = ov_rest_Total_Temp_Sensors + 1;
                ov_rest_Total_Temp_Sensors++;
                sensor->Type = SAHPI_COOLING_DEVICE;
                sensor->Category = SAHPI_EC_THRESHOLD;
                sensor->EnableCtrl = SAHPI_TRUE;
                sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
                sensor->Events = SAHPI_ES_UNSPECIFIED;
                sensor->DataFormat.IsSupported = SAHPI_TRUE;
                sensor->DataFormat.ReadingType =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor->DataFormat.BaseUnits = SAHPI_SU_UNSPECIFIED;
                sensor->DataFormat.ModifierUnits = SAHPI_SU_UNSPECIFIED;
                sensor->DataFormat.Percentage = SAHPI_TRUE;
                sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

                oh_init_textbuffer(&(rdr.IdString));
                oh_append_textbuffer(&(rdr.IdString), server_fan_info.Name);

                /* Sensor specific information is stored in this structure */
                sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
                if (sensor_info == NULL) {
                        err("OV_REST out of memory");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
                sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
                sensor_info->sensor_enable = SAHPI_TRUE;
                sensor_info->event_enable = SAHPI_FALSE;
                sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
                sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
                sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
                sensor_info->sensor_reading.Type =
                                        SAHPI_SENSOR_READING_TYPE_FLOAT64;
                sensor_info->sensor_reading.Value.SensorFloat64 =
                                                server_fan_info.CurrentReading;

                rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
                if (rv != SA_OK) {
                        err("Failed to add fan sensor rdr for resource id %d",
						rpt->ResourceId);
                        return rv;
                }
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_thermal_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareThermalInfoResponse structure .
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the thermal sensors of blade resource.
 *
 * Detailed Description:
 *      - Parses the serverhardwareThermalInfoResponse responses.
 *      - Builds temperature and fan sensors for server hardware.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 *
 **/
SaErrorT ov_rest_build_server_thermal_rdr(struct oh_handler_state *oh_handler,
		struct serverhardwareThermalInfoResponse *response,
		SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        int i = 0, arraylen = 0;
        json_object *jvalue = NULL, *jval = NULL;
        struct serverhardwareThermalInfo server_therm_info = {0};
        struct serverhardwareFanInfo server_fan_info = {0};

	if (oh_handler == NULL ||
			response->serverhardwareThermal_array == NULL) {
		CRIT("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Checking for json object type, if it is not array, return */
	if (json_object_get_type(response->serverhardwareThermal_array) !=
							json_type_array) {
		CRIT("Unable to obtain thermal sensors array"
			"Can not build server thermal sensors");
		return SA_ERR_HPI_INVALID_DATA;
	}

	arraylen = json_object_array_length(
				response->serverhardwareThermal_array);
	for(i = 0; i < arraylen; i++){
		jvalue = json_object_array_get_idx(
				response->serverhardwareThermal_array, i);
		if(!jvalue){
			CRIT("Invalid serverhardwareThermal Response "
						"for the bay %d", i+1);
			continue;
		}
		ov_rest_json_parse_server_thermal_sensors(jvalue, 
							&server_therm_info);
                rv = ov_rest_build_temperature_sensor_rdr(oh_handler,
                                                server_therm_info, rpt);
                if (rv != SA_OK) {
                        err("Error in building temperature sensors for server"
				" in bay %d", i+1);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }
        ov_rest_Total_Temp_Sensors = arraylen;

        /* Checking for json object type, if it is not array, return */
        if (json_object_get_type(response->serverhardwareFans_array) !=
                                                         json_type_array) {
                /* Do not know if blades will have fan array so skip this
			message for now
                CRIT("Json object type is not of json_type_array for"
                        " the server hardware Fan sensors"); */
                return SA_ERR_HPI_INVALID_DATA;
        }

        arraylen = json_object_array_length(
                                response->serverhardwareFans_array);
        for(i=0; i<arraylen; i++){
                jval = json_object_array_get_idx(
                                response->serverhardwareFans_array, i);
		if(!jval){
			CRIT("Invalid ServerhardwareFans response "
						"for the bay %d", i+1);
			continue;
		}
                ov_rest_json_parse_server_fan_sensors(jval,
                                                        &server_fan_info);
                rv = ov_rest_build_fan_sensor_rdr_info(oh_handler, 
				server_fan_info, rpt);
                if (rv != SA_OK) {
                        err("Error in building fan sensors for server in"
				" bay %d", i + 1);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }

	return SA_OK;
}

/**
 * ov_rest_build_server_power_status_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwarePowerStatusInfoResponse 
 *			structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the power status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_power_status_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwarePowerStatusInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_POWER_SUPPLY;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64;
        sensor->DataFormat.BaseUnits = SAHPI_SU_WATTS;
        sensor->DataFormat.ModifierUnits = SAHPI_SU_UNSPECIFIED;
        sensor->DataFormat.ModifierUse = SAHPI_SMUU_NONE;
        sensor->DataFormat.Percentage = SAHPI_FALSE;
        sensor->DataFormat.Range.Flags = 0;
        sensor->DataFormat.AccuracyFactor = 0;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "Power status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
        sensor_info->sensor_reading.Value.SensorFloat64 =
                                                response->PowerConsumedWatts;

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server power status rdr for resource id %d",
							rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_memory_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareSystemsInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the memory status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_memory_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_MEMORY;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "Memory Status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->Memory_Status) {
                len = strlen(response->Memory_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
				response->Memory_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server memory rdr for resource id %d",
							rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_processor_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareSystemsInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the processor status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_processor_rdr(struct oh_handler_state 
		*oh_handler,struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_PROCESSOR;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "Processor Status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->Processor_Status) {
                len = strlen(response->Processor_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char*)sensor_info->sensor_reading.Value.SensorBuffer,
                                        response->Processor_Status, len);
        }
        else
                strcpy((char*)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server processor rdr for resource id %d",
							rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_health_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareSystemsInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the hardware health status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_health_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_OPERATIONAL;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "Hardware Health");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->System_Status) {
                len = strlen(response->System_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                response->System_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server health rdr for resource id %d",
							rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_battery_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareSystemsInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the battery status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_battery_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_OPERATIONAL;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "Battery Status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->Battery_Status) {
                len = strlen(response->Battery_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                response->Battery_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server battery rdr for resource id %d",
							rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_systems_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareSystemsInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the system sensors of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                      - On Success.
 *      SA_ERR_HPI_INTERNAL_ERROR  - On Failure.
 *
 **/
SaErrorT ov_rest_build_server_systems_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;

        rv = ov_rest_build_server_memory_rdr(oh_handler, response, rpt);
        if (rv != SA_OK) {
                err("Failed to build server Memory rdr");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = ov_rest_build_server_processor_rdr(oh_handler, response, rpt);
        if (rv != SA_OK) {
                err("Failed to build server Processor rdr");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = ov_rest_build_server_health_rdr(oh_handler, response, rpt);
        if (rv != SA_OK) {
                err("Failed to build server Health rdr");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = ov_rest_build_server_battery_rdr(oh_handler, response, rpt);
        if (rv != SA_OK) {
                err("Failed to build server Battery rdr");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return rv;
}

/**
 * ov_rest_build_server_storage_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareStorageInfoResponse structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the storage status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_storage_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareStorageInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_OPERATIONAL;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), response->Name);

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->SmartStorage_Status) {
                len = strlen(response->SmartStorage_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                        response->SmartStorage_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server storage rdr for resource id %d",
						rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_network_adapters_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareNetworkAdaptersInfoResponse 
 *			structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the network adapter status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_network_adapters_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareNetworkAdaptersInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_OPERATIONAL;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "NetworkAdapters Status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->NetworkAdapters_Status) {
                len = strlen(response->NetworkAdapters_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                        response->NetworkAdapters_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server network adapter rdr"
				" for resource id %d", rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_build_server_ethernet_inetrfaces_rdr:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @response:    Pointer to serverhardwareEthernetInterfacesInfoResponse 
 *			structure.
 *      @rpt: Pointer to SaHpiRptEntryT structure.
 *
 * Purpose:
 *      Builds the ethernet interface status sensor of blade resource.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - On Out Of Memory.
 *
 **/
SaErrorT ov_rest_build_server_ethernet_inetrfaces_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareEthernetInterfacesInfoResponse *response,
                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT rdr = {0};
        SaHpiSensorRecT *sensor = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        int len = 0;

        memset(&rdr, 0, sizeof(SaHpiRdrT));
        sensor = &(rdr.RdrTypeUnion.SensorRec);
        /* Populate the sensor rdr */
        rdr.Entity = rpt->ResourceEntity;
        rdr.RdrType = SAHPI_SENSOR_RDR;
        sensor->Num = ov_rest_Total_Temp_Sensors + 1;
        ov_rest_Total_Temp_Sensors++;
        sensor->Type = SAHPI_OPERATIONAL;
        sensor->Category = SAHPI_EC_UNSPECIFIED;
        sensor->EnableCtrl = SAHPI_TRUE;
        sensor->EventCtrl = SAHPI_SEC_READ_ONLY;
        sensor->Events = SAHPI_ES_UNSPECIFIED;
        sensor->DataFormat.IsSupported = SAHPI_TRUE;
        sensor->DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
        sensor->ThresholdDefn.IsAccessible = SAHPI_FALSE;

        oh_init_textbuffer(&(rdr.IdString));
        oh_append_textbuffer(&(rdr.IdString), "EthernetInterfaces Status");

        /* Sensor specific information is stored in this structure */
        sensor_info = g_malloc0(sizeof(struct ov_rest_sensor_info));
        if (sensor_info == NULL) {
                err("OV_REST out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        sensor_info->current_state = SAHPI_ES_UNSPECIFIED;
        sensor_info->sensor_enable = SAHPI_TRUE;
        sensor_info->event_enable = SAHPI_FALSE;
        sensor_info->assert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->deassert_mask = OV_REST_STM_UNSPECIFED;
        sensor_info->sensor_reading.IsSupported = SAHPI_TRUE;
        sensor_info->sensor_reading.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
        if (response->EthernetInterfaces_Status) {
                len = strlen(response->EthernetInterfaces_Status);
                if (len >= SAHPI_SENSOR_BUFFER_LENGTH)
                        len = SAHPI_SENSOR_BUFFER_LENGTH - 1;
                strncpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                response->EthernetInterfaces_Status, len);
        }
        else
                strcpy((char *)sensor_info->sensor_reading.Value.SensorBuffer,
                                                        "NOT AVAILABLE");

        rv = oh_add_rdr(oh_handler->rptcache, rpt->ResourceId, &rdr,
                                                        sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add server ethernet interface rdr"
			" for resource id %d", rpt->ResourceId);
                return rv;
        }

        return SA_OK;
}

/*
 * ov_rest_populate_event:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @event:       Pointer to event structure.
 *      @assert_sensors: Pointer to GSList.
 *
 * Purpose:
 *      - Populates the event structure with default values of the resource.
 *      - If sensor is in assert state, then pushes the asserted sensor RDR
 *        to list.
 *
 * Detailed Description:
 *      - Populates the event structure with default values of the resource.
 *      - If sensor is in assert state, then pushes the asserted sensor RDR to
 *        assert sensor list. This list is used for generating the sensor 
 *        assert events.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT ov_rest_populate_event(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct oh_event *event,
                                GSList **assert_sensors)
{
	SaHpiRptEntryT *rpt = NULL;
	SaHpiRdrT *rdr = NULL;
	struct ov_rest_sensor_info *sensor_info = NULL;
	SaHpiEventStateT state;
	SaHpiEventCategoryT evt_catg;

	if (oh_handler == NULL || event == NULL || assert_sensors == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);

	memset(event, 0, sizeof(struct oh_event));
	event->event.Source = rpt->ResourceId;
	oh_gettimeofday(&event->event.Timestamp);
	event->event.Severity = rpt->ResourceSeverity;
	event->resource = *rpt;
	event->hid = oh_handler->hid;

	/* Get the first RDR */
	rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
			SAHPI_FIRST_ENTRY);
	while (rdr) {
		/* Push the rdr to event rdrs list */
		event->rdrs =
			g_slist_append(event->rdrs, g_memdup(rdr,
						sizeof(SaHpiRdrT)));

		/* Check whether the RDR is of type sensor */
		if (rdr->RdrType == SAHPI_SENSOR_RDR) {
			sensor_info = (struct ov_rest_sensor_info*)
				oh_get_rdr_data(oh_handler->rptcache,
						resource_id,
						rdr->RecordId);

			/* Check whether the event is enabled or not */
			if (sensor_info->event_enable == SAHPI_TRUE) {
				state = sensor_info->current_state;
				evt_catg = rdr->RdrTypeUnion.SensorRec.Category;
				/* Check whether the sensor current state is
				 * asserted or not. Sensor is considered to be
				 * in assert state, if any one of the following
				 * 3 conditions are met:
				 *
				 * 1. event category = ENABLE and state =
				 * DISABLED
				 * 2. event category = PRED_FAIL and state =
				 * PRED_FAILURE_ASSERT
				 * 3. event category = THRESHOLD and state =
				 * UPPER_MAJOR or _UPPER_CRIT
				 */
				if ( (evt_catg == SAHPI_EC_ENABLE &&
					state == SAHPI_ES_DISABLED) ||
					(evt_catg == SAHPI_EC_PRED_FAIL &&
					state == SAHPI_ES_PRED_FAILURE_ASSERT) 
					|| (evt_catg == SAHPI_EC_REDUNDANCY &&
					state == SAHPI_ES_REDUNDANCY_LOST) ||
					(evt_catg == SAHPI_EC_THRESHOLD &&
					(state == SAHPI_ES_UPPER_MAJOR ||
					state == SAHPI_ES_UPPER_CRIT)) ) {
					/* Push the sensor rdr to assert sensor
					 * list
					 */
					*assert_sensors =
						g_slist_append(*assert_sensors,
						g_memdup(rdr, 
						sizeof(SaHpiRdrT)));
				}
			}
		}
		/* Get the next RDR */
		rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
				rdr->RecordId);
	}

	return SA_OK;
}


/*
 * ov_rest_push_disc_res:
 *      @oh_handler: Pointer to openhpi handler.
 *
 * Purpose:
 *      @Pushes the discovered resource entries to openhpi infrastructure.
 *
 * Detailed Description:
 *      - Get the rpt entry of the resources one by one.
 *      - Creates the resource or hotswap event depending on the resource
 *        hotswap capability.
 *      - Pushes the events to openhpi infrastructure.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
static void ov_rest_push_disc_res(struct oh_handler_state *oh_handler)
{
	SaHpiRptEntryT *rpt = NULL;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;
	GSList *assert_sensor_list = NULL;

	if (oh_handler == NULL) {
		err("Invalid parameter");
		return;
	}

	/* Get the first resource */
	rpt = oh_get_resource_next(oh_handler->rptcache, SAHPI_FIRST_ENTRY);
	while (rpt) {
		/* Populate the event structure with default values and get the
		 * asserted sensor list
		 */
		ov_rest_populate_event(oh_handler, rpt->ResourceId, &event,
				&assert_sensor_list);

		/* Check whether the resource has hotswap capability */
		if (event.resource.ResourceCapabilities &
				SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
			/* Get the hotswap state and fill the current
			 * hotswap state
			 */
			hotswap_state = (struct ovRestHotswapState *)
				oh_get_resource_data(oh_handler->rptcache,
						event.resource.ResourceId);
			if (hotswap_state == NULL) {
				err("Failed to get server hotswap state for "
					"resource id %d", rpt->ResourceId);
				return;
			}
			event.event.EventType = SAHPI_ET_HOTSWAP;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				hotswap_state->currentHsState;
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange = SAHPI_HS_CAUSE_UNKNOWN;
		} else if (event.resource.ResourceCapabilities &
				SAHPI_CAPABILITY_FRU) {
			/* The resource is FRU, but does not have hotswap
			 * capability Fill the current hotswap state as ACTIVE.
			 */
			event.event.EventType = SAHPI_ET_HOTSWAP;
			event.event.EventDataUnion.HotSwapEvent.HotSwapState =
				SAHPI_HS_STATE_ACTIVE;
			event.event.EventDataUnion.HotSwapEvent.
				CauseOfStateChange = SAHPI_HS_CAUSE_UNKNOWN;
		} else {
			/* The resource does not have FRU and hotswap
			 * capabilities. Raise the resrouce event.
			 */
			event.event.EventType = SAHPI_ET_RESOURCE;
			event.event.EventDataUnion.ResourceEvent.
				ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
		}
		/* Push the event to OpenHPI */
		oh_evt_queue_push(oh_handler->eventq,
				copy_ov_rest_event(&event));

		/* Get the next resource */
		rpt = oh_get_resource_next(oh_handler->rptcache,
				rpt->ResourceId);
	} /* End of while loop */

	return;
}


void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("ov_rest_discover_resources")));

