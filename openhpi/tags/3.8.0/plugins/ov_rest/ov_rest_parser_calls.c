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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#define OV_REST_CALLS_FILE
#include "sahpi_wrappers.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest_discover.h"

/**
 * parse_xsdBoolean:
 *      @str  : Character pointer.
 *
 * Purpose:
 *      Parses xsdBoolean strings, returning an hpeov_boolean.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      HPEOV_TRUE    - On Sucess.
 *      HPEOV_FALSE   - On Failure.
 *
 **/
static enum hpeov_boolean parse_xsdBoolean(char *str)
{
        if ((! strcmp(str, "true")) ||
            (! strcmp(str, "1")))
                return(HPEOV_TRUE);

        return(HPEOV_FALSE);
}

/**
 * ov_rest_json_parse_server:
 *      @jvalue  : Pointer to json_object.
 *      @servInfo: Pointer to serverhardwareInfo.
 *
 * Purpose:
 *      Parses the json response for server hardware info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_server(json_object *jvalue,
				struct serverhardwareInfo* servInfo)
{
	const char *temp = NULL;

	json_object_object_foreach(jvalue, key,val){
//		OV_REST_CHEK_STR_LEN(key,val);
		if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->serialNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"model")){
			dbg( " \n %s= %s\n", key, json_object_get_string(val));
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->model,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"mpFirmwareVersion")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->fwVersion, temp, 
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->partNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"position")){
			servInfo->bayNumber = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"powerState")){
			servInfo->powerState = rest_enum(powerState_S, 
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"uri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->uri,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"locationUri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->locationUri,temp,
					strlen(temp)+1);
			continue;
		}
                if(!strcmp(key,"status")){
                        servInfo->serverStatus = rest_enum(healthStatus_S,
                                                 json_object_get_string(val));
                        continue;
                }
		if(!strcmp(key,"uuid")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->uuid,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"uidState")){
			dbg( " \n %s= %s\n", key, json_object_get_string(val));
			temp = json_object_get_string(val);
			if(temp)
				memcpy(servInfo->uidState,temp,strlen(temp)+1);
			continue;
		}
		/* FIXME: Handle the Rackmount server information if missing any
		 * Also take the decision on type of the server(Rack/Blade) 
		 * which we are parsinn in the begining of this function
		 */
		temp = NULL;
	}
	if(servInfo->manufacturer){
		memcpy(servInfo->manufacturer,"HPE", 4);
	}
	servInfo->type = SERVER_HARDWARE;
}

/**
 * ov_rest_json_parse_drive_enclosure:
 *	@jvalue  : Pointer json_object.
 *	@drvInfo: Pointer to driveEnclosureInfo structure.
 *
 * Purpose:
 * 	Parses the json response for drive enclosure info.
 *
 * Return:
 *	None.
 *
 **/
void ov_rest_json_parse_drive_enclosure(json_object *jvalue,
					struct driveEnclosureInfo* drvInfo)
{
        const char *temp = NULL;

        json_object_object_foreach(jvalue, key, val){
		//OV_REST_CHEK_STR_LEN(key,val);
                if(!strcmp(key,"serialNumber")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(drvInfo->serialNumber,
					temp,strlen(temp)+1);
                        continue;
                }
		if(!strcmp(key,"firmwareVersion")){
                        temp = json_object_get_string(val);
                        if(temp)
				memcpy(drvInfo->fwVersion,temp,strlen(temp)+1);
			continue;
		}
                if(!strcmp(key,"enclosureName")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(drvInfo->enc_serialNumber,
					temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"model")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(drvInfo->model,temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"partNumber")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(drvInfo->partNumber,
					temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"bay")){
                        drvInfo->bayNumber = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"powerState")){
                        drvInfo->powerState = rest_enum(powerState_S, 
						json_object_get_string(val));
                        continue;
                }
                if(!strcmp(key,"status")){
                        drvInfo->drvEncStatus = 
                                        rest_enum(healthStatus_S, 
                                        json_object_get_string(val));
			continue;
                }
                if(!strcmp(key,"uri")){
                        temp = json_object_get_string(val);
                        if(temp)
                        	memcpy(drvInfo->uri, temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"enclosureUri")){
                        temp = json_object_get_string(val);
                        if(temp)
                        	memcpy(drvInfo->locationUri,
					temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"uuid")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(drvInfo->uuid,temp,strlen(temp)+1);
                        continue;
                }

                if(!strcmp(key,"uidState")){
                        dbg( " \n %s= %s\n", key, json_object_get_string(val));
                        temp = json_object_get_string(val);
                        if(temp)
                        	memcpy(drvInfo->uidState,temp,strlen(temp)+1);
                        continue;
                }

        }
	if(drvInfo->manufacturer){
		memcpy(drvInfo->manufacturer,"HPE", 4);
	}
	drvInfo->type = DRIVE_ENCLOSURE;
}

/**
 * ov_rest_trim_alert_string:
 *      @alert   : Constant character pointer.
 *      @evtinfo: Pointer to eventInfo structure.
 *
 * Purpose:
 *      Trims the alert string.
 *	Example alert strings are 
 *	hpris.emRegistry.1.0.PowerInputFault
 *	Trap.cpqDa6AccelStatusChange
 *	ERM.DEVICE_HISTORY_DATA_COLLECTION_FAILURE
 *	crm.logicalInterconnectStateChange
 *	server-hardware.data.asset.tag.invalid
 *	HACluster.NoSecondApplianceFound
 *	USB.StickPlugged
 *
 * Return:
 *      None.
 *
 **/
int ov_rest_trim_alert_string(const char* alert, struct eventInfo *evtinfo)
{
	char dest[MAX_256_CHARS];
	char trimmed_alert[MAX_256_CHARS];
	int j = 0, ret = 0, scount = 0;

	if (( alert == NULL ) ||  (evtinfo == NULL)) {
		err("Invalid parameters");
		return(-1);
	}
        memset(dest, 0, sizeof(char)*256);
        memset(trimmed_alert, 0, sizeof(char)*256);
	if (strlen(alert) >= 255 ) {
		err("Alert %s is too long %d",alert, (int)strlen(alert));
		strncpy(dest, alert, 255);
		dest[255] = '\0';
	} else
		strcpy(dest, alert);

	for (j = 0; dest[j] != '\0'; j++){
		if(dest[j] == '.') {
			dest[j] = ' ';
			scount++;
		}
	}
	if(!evtinfo->phyResourceType){
		warn("physicalResourceType is null for this alert, so setting "
						"alertTypeId to OEM_EVENT");
		evtinfo->alertTypeId = rest_enum(eventType_S,"OEM_EVENT");
		return -1;		
	}

	if (strstr(dest, "hpris ")) {
        	ret = sscanf(dest,"hpris %*s %*d %*d %s",trimmed_alert);
	} else if (strstr(dest, "Trap ")) {
		ret = sscanf(dest, "Trap %s", trimmed_alert);
	} else if (strstr(dest, "crm ")) {
		ret = sscanf(dest, "crm %s", trimmed_alert);
	} else if (strstr(dest, "swmon ")) {
		if (scount == 1)
			ret = sscanf(dest, "swmon %s", trimmed_alert);
		else if (scount == 2) 
			ret = sscanf(dest, "swmon %*s %s", trimmed_alert);
		else
			ret = sscanf(dest, "swmon %s %*s %*s", trimmed_alert);
	} else {
		warn("alert string: %s is not important as of now", alert);
		warn("Setting it as OEM_EVENT to handle generically");
		evtinfo->alertTypeId = rest_enum(eventType_S, "OEM_EVENT");
		return -1;		

	}
	if ( ret != 1 || strlen(trimmed_alert) == 0 )
		evtinfo->alertTypeId = rest_enum(eventType_S, "OEM_EVENT");
	else
		evtinfo->alertTypeId = rest_enum(eventType_S, trimmed_alert);

	/* Some alerts are too complex. Make them OEM_EVENTS */
	if ( evtinfo->alertTypeId == -1 )
		evtinfo->alertTypeId = rest_enum(eventType_S, "OEM_EVENT");

	dbg("alert=%s, trimmed=%s enum=%d",alert, trimmed_alert, 
			evtinfo->alertTypeId);
	return ret;
}
                                             
/**
 * ov_rest_json_parse_events:
 *      @jobj    : Pointer to json_object.
 *      @evtinfo: Pointer to eventInfo.
 *
 * Purpose:
 *      Parses the json response for event info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_events( json_object *jobj, struct eventInfo* evtinfo)
{
        json_object_object_foreach(jobj, key, val){
                if(!strcmp(key,"category")){
			evtinfo->category = json_object_get_string(val);
                        break;
                }
        }
}

/**
 * ov_rest_json_parse_alerts_utility:
 *      @jobj    : Pointer to json_object.
 *      @evtinfo: Pointer to eventInfo.
 *
 * Purpose:
 *      Parses the json response for event info for alerts.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_alerts_utility( json_object *jobj, 
				struct eventInfo* evtinfo)
{
//	json_object *associatedResource = NULL; 

	json_object * jvalue = jobj;
	json_object_object_foreach(jvalue, key, val){

		ov_rest_prn_json_obj(key, val);

		if(!strcmp(key,"severity")){
			evtinfo->severity = json_object_get_string(val);
			continue;
		} 
		if(!strcmp(key,"resourceUri")){
			evtinfo->resourceUri = json_object_get_string(val);
			continue;
		}
		/* We are not using the commented code below, But it is for 
 		 * future purpose*/
		/*
		if(!strcmp(key,"associatedResource")){
			associatedResource = 
				ov_rest_wrap_json_object_object_get(jobj, 
				"associatedResource");
			ov_rest_json_parse_alerts_utility(associatedResource,
								evtinfo);
			continue;
		}
		*/
		if(!strcmp(key,"physicalResourceType")){
			evtinfo->phyResourceType = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"alertTypeID") || !strcmp(key,"name")){
			evtinfo->alert_name = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"alertState")){
			evtinfo->alertState = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"resourceID")){
			evtinfo->resourceID = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"description")){
			evtinfo->description = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"healthCategory")){
			evtinfo->healthCategory = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"created")){
			evtinfo->created = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"correctiveAction")){
			evtinfo->correctiveAction = json_object_get_string(val);
			continue;
		}
	}
}

/**
 * ov_rest_json_parse_alerts:
 *      @jobj    : Pointer to json_object.
 *      @evtinfo: Pointer to eventInfo.
 *
 * Purpose:
 *      First it invokes the ov_rest_json_parse_alerts_utility function to 
 *      fully parse the received alert, and then ov_rest_trim_alert_string 
 *      function trims the alerttypeID string using parsed alert infomration.
 *
 * Return:
 *      None.
 *
 **/

void ov_rest_json_parse_alerts( json_object *jobj, struct eventInfo* evtinfo)
{
	int ret;
	ov_rest_json_parse_alerts_utility( jobj, evtinfo);
	ret = ov_rest_trim_alert_string( evtinfo->alert_name,
					evtinfo);
	if(ret != 1){
		dbg("Unknown alert.");
	}
}

/**
 * ov_rest_json_parse_tasks:
 *      @jobj    : Pointer to json_object.
 *      @evtinfo: Pointer to eventInfo.
 *
 * Purpose:
 *      Parses the json response for event info for tasks.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_tasks( json_object *jobj, struct eventInfo* evtinfo)
{
	int i = 0;
	const char* temp = NULL;
	char Name[262] = {0}, prefix[262] = "TASK_";
	json_object *associatedResource = NULL;
	json_object * jvalue = jobj;
	evtinfo->percentComplete = 0;
	if(jobj == NULL){
		return;
	}

	json_object_object_foreach(jvalue, key, val){

		ov_rest_prn_json_obj(key, val);

		if(!strcmp(key,"associatedResource")){
			associatedResource = 
				ov_rest_wrap_json_object_object_get(jobj,
				"associatedResource");
			ov_rest_json_parse_tasks(associatedResource, evtinfo);
			continue;
		}
		if(!strcmp(key,"resourceUri")) {
			evtinfo->resourceUri = 
				json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"resourceCategory")) {
			evtinfo->resourceCategory =
				json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"percentComplete")) {
			evtinfo->percentComplete = 
				atol(json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"taskState")) {
			evtinfo->taskState = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"name")){
			temp = json_object_get_string(val);
			if(temp != NULL){
				char *temp2 = NULL;
				temp2 = strdup(temp);
				ov_rest_lower_to_upper(temp2, 
						strlen(temp2), Name, 256);
				wrap_free(temp2);
				for (i = 0; i < strlen(Name); i++) {
					if (Name[i] == ' ')
						Name[i] = '_';
					if(Name[i] == '.')
						Name[i] = '\0';
				}
				Name[i] = '\0';
				strcat(prefix, Name);
				strcpy(Name, prefix);
				strcpy(evtinfo->task_name, Name);
				evtinfo->name = rest_enum(name_S, Name);
				continue;
			}
		}
	}
}

/**
 * ov_rest_json_parse_interconnect:
 *      @jvalue  : Pointer to json_object.
 *      @intinfo: Pointer to interconnectInfo.
 *
 * Purpose:
 *      Parses the json response for interconnect info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_interconnect( json_object *jvalue, 
					struct interconnectInfo *intinfo)
{
	const char *temp = NULL;
	json_object *bayLocation = NULL;
	json_object *jobj = NULL;
        int arraylen = 0, j = 0;
	if(jvalue == NULL){
		err("Invalid paramaters");
		return;
	}	

	json_object_object_foreach(jvalue, key, val){
		if(!strcmp(key,"interconnectLocation")){
			bayLocation = ov_rest_wrap_json_object_object_get(val,
						"locationEntries");
			/* Checking for json object type, if it is not array, return */
			if (bayLocation == NULL || (json_object_get_type(bayLocation)
					 != json_type_array)) {
				CRIT("The bayLocation is NULL OR no "
					"interconnect location array.");
				return;
			}
			arraylen = json_object_array_length(bayLocation);
			for(j=0;j<arraylen;j++){
				jobj =json_object_array_get_idx(bayLocation,j);
				ov_rest_json_parse_interconnect(jobj, intinfo);
				if(!strcmp(intinfo->location.type,"Bay")){
					intinfo->bayNumber = 
						intinfo->location.value;
					break;
				}
			}
			continue;
		}
		if(!strcmp(key,"value")){
			intinfo->location.value = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"type")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->location.type, temp, 
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"enclosureUri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->locationUri, temp, 
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->serialNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->partNumber, temp,
						strlen(temp)+1);
			continue;
		}
		/* Synergy and OneView fields differ-powerState/powerStatus */
		if(!strcmp(key,"powerStatus") || !strcmp(key,"powerState")){
			intinfo->powerState = rest_enum(powerState_S,
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"model")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->model,temp,strlen(temp)+1);
			continue;
		}
		/* Synergy and OneView fields differ-uri/interconnectUri */
		if(!strcmp(key,"uri")|| !strcmp(key, "interconnectUri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->uri,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"status")){
			temp = json_object_get_string(val);
			if(temp)
				intinfo->interconnectStatus =
					rest_enum(healthStatus_S, 
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"uidState")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(intinfo->uidState,temp,strlen(temp)+1);
			continue;
		}
	}
	if(strstr(intinfo->uri, "sas-interconnect")){
		intinfo->type = SAS_INTERCONNECT;
	}else{
		intinfo->type = INTERCONNECT;
	}
}

/**
 * ov_rest_json_parse_certificate:
 *      @jobj    : Pointer to json_object.
 *      @certs: Pointer to certificates structure.
 *
 * Purpose:
 *      Parses the json response for certificates.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_certificate( json_object *jobj, 
				struct certificates *certs)
{
	if(jobj == NULL){
		err("Invalid Parameters");
		return;
	}
	json_object_object_foreach(jobj, key, val){
		ov_rest_prn_json_obj(key, val);
		if(!strcmp(key,"base64SSLCertData")){
			certs->SSLCert = json_object_get_string(val);
			continue;
		}
		if(!strcmp(key,"base64SSLKeyData")){
			certs->SSLKey = json_object_get_string(val);
			continue;
		}
	}
}

/**
 * ov_rest_json_parse_ca:
 *      @jobj    : Pointer to json_object.
 *      @response: Pointer to certificates structure.
 *
 * Purpose:
 *      Parses the json response for ca certificates.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_ca( json_object *jobj, struct certificates *response)
{
	json_object *member = NULL, *jvalue = NULL, *cert = NULL;
	if(jobj == NULL){
		err("Invalid Parameters");
		return;
	}
	member = ov_rest_wrap_json_object_object_get(jobj, "members");
	if(member){
		jvalue = json_object_array_get_idx(member, 0);
		if(jvalue == NULL){
			err("Invalid Response");
			return;
		}
		cert = ov_rest_wrap_json_object_object_get(jvalue, 
						"certificateDetails");
		if(!cert){
			err("Invalid Response");
			return;
		}
		json_object_object_foreach(cert, key, val){
			if(!strcmp(key,"base64Data"))
				response->ca = json_object_get_string(val);
		}
		
	}else{
		response->ca = json_object_get_string(jobj);
	}
}

/**
 * ov_rest_json_parse_appliance_version:
 *      @jobj    : Pointer to json_object.
 *      @response: Pointer to applianceVersion structure.
 *
 * Purpose:
 *      Parses the json response for appliance version.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_appliance_version( json_object *jobj, 
				struct applianceVersion *response)
{
	const char *temp = NULL;

	json_object_object_foreach(jobj, key, val){
		ov_rest_prn_json_obj(key, val);
		if(!strcmp(key,"major")){
			response->major = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"minor")){
			response->minor = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"build")){
			response->build = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"softwareVersion")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->softwareVersion,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"platformType")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->platformType,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->serialNumber,temp, 
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"modelNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->name,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"uri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uri,temp,strlen(temp)+1);
			continue;
		}
	}
	if(response->manufacturer){
		memcpy(response->manufacturer,"HPE", 4);
	}
}

/**
 * ov_rest_json_parse_appliance_Ha_node:
 *      @jobj    : Pointer to json_object.
 *      @response: Pointer to applianceHaNodeInfo structure.
 *
 * Purpose:
 *      Parses the json response for appliance HA Node Info
 *      for Active appliance.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_appliance_Ha_node( json_object *jobj, 
				struct applianceHaNodeInfo *response)
{
	const char *temp = NULL;
	json_object * jvalue = NULL;

	json_object_object_foreach(jobj, key, val){
		ov_rest_prn_json_obj(key, val);

		if(!strcmp(key,"version")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->version,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"role")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->role,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"modelNumber")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->modelNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"status")){
			temp = json_object_get_string(val);
			if(temp)
				response->applianceStatus =
					rest_enum(healthStatus_S, 
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"name")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->name,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"uri")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				memcpy(response->uri,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key, "location")){
			jvalue = ov_rest_wrap_json_object_object_get(jobj, 
								"location");
			ov_rest_json_parse_appliance_Ha_node(jvalue, response);
			continue;
		}
		if(!strcmp(key, "bay")){
			response->bayNumber = json_object_get_int(val);
			continue;

		}
		if(!strcmp(key, "enclosure")){
			jvalue = ov_rest_wrap_json_object_object_get(jobj, 
								"enclosure");
			ov_rest_json_parse_appliance_Ha_node(jvalue, response);
			continue;
		}
		if(!strcmp(key, "resourceUri")){
			temp = json_object_get_string(val);
			if(temp != NULL)
				 memcpy(response->enclosure_uri,temp,
					strlen(temp)+1);	
			continue;
		}
	}
	response->type = SYNERGY_COMPOSER;
}
/**
 * ov_rest_json_parse_appliance_status:
 *      @jobj    : Pointer to json_object.
 *      @response: Pointer to applianceStatus structure.
 *
 * Purpose:
 *      Parses the json response for appliance status.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_appliance_status( json_object *jobj, 
					struct applianceStatus* response)
{
	const char *temp = NULL;
	json_object_object_foreach(jobj, key, val){

		ov_rest_prn_json_obj(key, val);

		if(!strcmp(key,"networkConfigured")){
			char *temp2 = strdup(json_object_get_string(val));
			response->networkConfigured = parse_xsdBoolean(temp2);
			wrap_free(temp2);
			continue;
		}
		if(!strcmp(key,"memoryUnits")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->memoryUnits,temp,
						strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"cpuSpeedUnits")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->cpuSpeedUnits,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"lanUnits")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->lanUnits,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"cpu")){
			response->cpu = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"cpuSpeed")){
			response->cpuSpeed = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"memory")){
			response->memory = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"lan")){
			response->lan = json_object_get_int(val);
			continue;
		}
	}
}

/**
 * ov_rest_json_parse_applianceInfo:
 *      @jobj    : Pointer to json_object.
 *      @response: Pointer to applianceInfo structure.
 *
 * Purpose:
 *      Parses the json response for appliance bay.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_applianceInfo(json_object *jobj,
				struct applianceInfo *response)
{

	const char *temp = NULL;
	json_object_object_foreach(jobj, key, val){

		ov_rest_prn_json_obj(key, val);

		if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->partNumber,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"sparepartNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->sparePartNumber,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"bayNumber")){
			response->bayNumber = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"powerState")){
			response->powerState = rest_enum(powerState_S, 
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"devicePresence")){
			response->presence = rest_enum(presence_S,
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"model")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->model,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"poweredOn")){
			char *temp2 = strdup(json_object_get_string(val));
			response->poweredOn = parse_xsdBoolean(temp2);
			wrap_free(temp2);
			continue;
		}
		if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->serialNumber,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"status")){
			temp = json_object_get_string(val);
			if(temp)
				response->status = rest_enum(healthStatus_S,
								temp);
			continue;
		}
	}


}


/**
 * ov_rest_json_parse_datacenter:
 *      @jarray  : Pointer to json_object.
 *      @i       : Integer variable.
 *      @response: Pointer to datacenterInfo structure.
 *
 * Purpose:
 *      Parses the json response for datacenter info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_datacenter( json_object *jarray, int i, 
				struct datacenterInfo* response)
{
	const char *temp = NULL;
	json_object * jvalue = NULL;

	/*Getting the array element at position i*/
	jvalue = json_object_array_get_idx(jarray, i); 

	json_object_object_foreach(jvalue, key, val){
		if(!strcmp(key,"id")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uuid,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"name")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->name,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"uri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uri,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"status")){
			temp = json_object_get_string(val);
			if(temp)
				response->status = rest_enum(healthStatus_S,
								temp);
			continue;
		}
	}
	if(response->manufacturer){
		memcpy(response->manufacturer,"HPE", 4);
	}
}

/**
 * ov_rest_json_parse_enc_device_bays:
 *      @jvalue  : Pointer to json_object.
 *      @response: Pointer to enclosureDeviceBays structure.
 *
 * Purpose:
 *      Parses the json response for enclosure device bays.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_enc_device_bays( json_object *jvalue, 
				struct enclosureDeviceBays *response)
{
	json_object_object_foreach(jvalue, key, val){
		if(!strcmp(key,"devicePresence")){
			response->presence = rest_enum(presence_S,
						json_object_get_string(val));
			continue;
		}
		if(!strcmp(key,"bayNumber")){
			response->bayNumber = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"changeState")){
			response->changeState = rest_enum(changeState_S, 
						json_object_get_string(val));
			continue;
		}
	}
}

/**
 * ov_rest_json_parse_enc_manager_bays:
 *      @jvalue  : Pointer to json_object.
 *      @response: Pointer to enclosureInfo structure.
 *
 * Purpose:
 *      Parses the json response for enclosure manager bays.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_enc_manager_bays( json_object *jvalue,
                                struct enclosureInfo *response)
{
        const char *temp = NULL;

        json_object_object_foreach(jvalue, key, val){
                if(!strcmp(key,"devicePresence")){
                        response->presence = rest_enum(presence_S,
                                                json_object_get_string(val));
                        continue;
                }
                if(!strcmp(key,"bayNumber")){
                        response->bayNumber = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"fwVersion")){
                        temp = json_object_get_string(val);
                        if(temp)
                                memcpy(response->hwVersion,temp,
                                        strlen(temp)+1);
                        continue;
                }
        }
}	

/**
 * ov_rest_json_parse_enclosure:
 *      @jvalue  : Pointer to json_object.
 *      @response: Pointer to enclosureInfo structure.
 *
 * Purpose:
 *      Parses the json response for enclosure info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_enclosure( json_object *jvalue, 
					struct enclosureInfo* response)
{
	const char *temp = NULL;

	json_object_object_foreach(jvalue, key, val){
		if(!strcmp(key,"uuid")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uuid,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->serialNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->partNumber,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"deviceBayCount")){
			response->bladeBays = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"interconnectBayCount")){
			response->interconnectTrayBays = json_object_get_int(
								val);
			continue;
		}
		if(!strcmp(key,"powerSupplyBayCount")){
			response->powerSupplyBayCount=json_object_get_int(val);
			continue;
		}

		if(!strcmp(key,"fanBayCount")){
			response->fanBayCount = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"name")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->name,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"uri")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uri,temp,strlen(temp)+1);
			continue;
		}
                if(!strcmp(key,"status")){
                        temp = json_object_get_string(val);
                        if(temp)
                                response->enclosureStatus =
                                       rest_enum(healthStatus_S, 
						json_object_get_string(val));
                        continue;
                }
		if(!strcmp(key,"uidState")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->uidState,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"applianceBayCount")){
			response->composerBays = json_object_get_int(val);
			continue;
		}
	}
	if(response->manufacturer){
		memcpy(response->manufacturer,"HPE", 4);
	}
}

/**
 * ov_rest_json_parse_powersupply:
 *      @jvalue  : Pointer to json_object.
 *      @response: Pointer to powersupplyInfo structure.
 *
 * Purpose:
 *      Parses the json response for powersupply info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_powersupply( json_object *jvalue, 
				struct powersupplyInfo* response)
{
	const char *temp = NULL;

        json_object_object_foreach(jvalue, key, val){
                if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->serialNumber,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->partNumber,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"model")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->model,temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"bayNumber")){
                        response->bayNumber = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"devicePresence")){
                        response->presence = rest_enum(presence_S,
						json_object_get_string(val));
                        continue;
                }
                if(!strcmp(key,"status")){
                        temp = json_object_get_string(val);
                        if(temp) 	
                                response->status = rest_enum(healthStatus_S,
                                                             temp);
                        continue;
                }
                if(!strcmp(key,"outputCapacityWatts")){
                        response->outputCapacityWatts=json_object_get_int(val);
                        continue;
                }
        }
	response->type = POWER_SUPPLY;
}

/**
 * ov_rest_json_parse_fan:
 *      @jvalue  : Pointer to json_object.
 *      @response: Pointer to fanInfo structure.
 *
 * Purpose:
 *      Parses the json response for fan info.
 *
 * Return:
 *      None.
 *
 **/
void ov_rest_json_parse_fan( json_object *jvalue, struct fanInfo* response)
{
	const char *temp = NULL;

        json_object_object_foreach(jvalue, key, val){
                if(!strcmp(key,"serialNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->serialNumber,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"partNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->partNumber,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"sparePartNumber")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->sparePartNumber,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"model")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->model,temp,strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"fanBayType")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->fanBayType,temp,
					strlen(temp)+1);
                        continue;
                }
                if(!strcmp(key,"bayNumber")){
                        response->bayNumber = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"devicePresence")){
                        response->presence = rest_enum(presence_S,
						json_object_get_string(val));
                        continue;
                }
                if(!strcmp(key,"status")){
			temp = json_object_get_string(val);
			if(temp)
                                response->status = rest_enum(healthStatus_S,
								temp);
                        continue;
                }
		temp = NULL;
        }
	response->type = FAN;
}

/**
 * ov_rest_json_parse_server_thermal_sensors:
 *	@jvalue: Pointer json_object.
 *	@response: Pointer to serverhardwareThermalInfo structure.
 *
 * Purpose:
 * 	Parses the json response of Teperatures data  and stores the data in 
 * 	serverhardwareThermalInfo strucure.
 *
 * Return:
 *	None
 *
 **/
void ov_rest_json_parse_server_thermal_sensors(json_object *jvalue, 
			struct serverhardwareThermalInfo *response)
{
	json_object *status = NULL;
	const char *temp = NULL;

	json_object_object_foreach(jvalue, key, val){
		if(!strcmp(key,"CurrentReading")){
			response->CurrentReading = json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"LowerThresholdCritical")){
			response->LowerThresholdCritical = 
						json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"LowerThresholdNonCritical")){
			response->LowerThresholdNonCritical = 
						json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"Name")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->Name,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"Number")){
			response->Number= json_object_get_int(val);
			continue;
		}
		if(!strcmp(key,"PhysicalContext")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->PhysicalContext,temp,
						strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"Units")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->Units,temp,strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"Status")){
			status =  json_object_get(val);
			if(status != NULL){
			ov_rest_json_parse_server_thermal_sensors(status, 
								response);
			json_object_put(status);
			}
			continue;
		}
		if(!strcmp(key,"Health")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->Health_status,temp,
					strlen(temp)+1);
			continue;
		}
		if(!strcmp(key,"State")){
			temp = json_object_get_string(val);
			if(temp)
				memcpy(response->Health_state,temp,
					strlen(temp)+1);
			continue;
		}
		temp = NULL;
	}
}

/**
 * ov_rest_json_parse_server_fan_sensors :
 *      @jvalue: Pointer json_object.
 *      @response: Pointer to serverhardwareFanInfo structure.
 *
 * Purpose:
 *      Parses the json response of Fans data  and stores the data in
 *      serverhardwareFanInfo strucure.
 *
 * Return:
 *      None
 *
 **/
void ov_rest_json_parse_server_fan_sensors(json_object *jvalue,
                        struct serverhardwareFanInfo *response)
{
        json_object *status = NULL;

        json_object_object_foreach(jvalue, key, val){
                if(!strcmp(key,"CurrentReading")){
                        response->CurrentReading = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"FanName")){
                        response->Name = json_object_get_string(val);
                        continue;
                }
                if(!strcmp(key,"Units")){
                        response->Units = json_object_get_int(val);
                        continue;
                }
                if(!strcmp(key,"Status")){
                        status =  json_object_get(val);
                        if(status != NULL){
                        ov_rest_json_parse_server_fan_sensors(status,
                                                                response);
			json_object_put(status);
                        }
                        continue;
                }
                if(!strcmp(key,"Health")){
                        response->Health_status = json_object_get_string(val);
                        continue;
                }
                if(!strcmp(key,"State")){
                        response->Health_state = json_object_get_string(val);
                        continue;
                }
        }
}

