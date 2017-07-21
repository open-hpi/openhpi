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

#include "sahpi_wrappers.h"
#include "ov_rest_power.h"
#include "rpt_utils.h"

/**
 * do_server_op:
 *      @conn:    Pointer to the REST_CON.
 *      @state:   Power state of the server-hardware.
 *      @control: Action/operation to be performed.
 *
 * Purpose:
 *      Performs the action/operation requested.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - on success.
 **/
static SaErrorT do_server_op (REST_CON *conn,
			      const char *state,
			      const char *control)
{
	OV_STRING response = {0};
	SaErrorT rv = SA_OK;
	char *uri = NULL, *postField=NULL;
	char *prevUrl = NULL;

	/* setup required REST url */
	prevUrl = conn->url;
	WRAP_ASPRINTF(&uri,"%s/powerState",conn->url);
	conn->url = uri;

	WRAP_ASPRINTF(&postField,"{\"powerState\":\"%s\",\"powerControl\":\"%s\"}", 
				state, control);
	rv = rest_put_request(conn, &response, postField);

	conn->url = prevUrl;
	ov_rest_wrap_json_object_put(response.jobj);
	wrap_free(uri);
	wrap_free(postField);
	return rv;
}

/**
 * do_interconnect_op:
 *      @conn:    Pointer to the REST_CON.
 *      @state:   Power state of the interconnect.
 *
 * Purpose:
 *      Performs the action/operation requested.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - on success.
 **/
static SaErrorT do_interconnect_op (REST_CON *conn,
				    const char *state)
{

	OV_STRING response = {0};
	SaErrorT rv = SA_OK;
	char *postField=NULL;

	WRAP_ASPRINTF(&postField,"[{\"op\": \"replace\","
			" \"path\": \"/powerState\","
			" \"value\": \"%s\"}]", state);
	rv = rest_patch_request(conn, &response, postField);
	ov_rest_wrap_json_object_put(response.jobj);
	wrap_free(postField);
	return rv;
}

/**
 * ov_rest_get_power_state:
 *      @oh_handler:  Pointer to openhpi handler
 *      @resource_id: Resource id
 *      @state:       Pointer to power state
 *
 * Purpose:
 *      Gets the power state of the resource.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                       - on success.
 *      SA_ERR_HPI_INVALID_PARAMS   - on wrong parameters
 *      SA_ERR_HPI_CAPABILITY       - on power capability is not set for
 *                                    the given resource
 *      SA_ERR_HPI_INVALID_RESOURCE - on not able to find the resource
 *      SA_ERR_HPI_INTERNAL_ERROR   - on failure.
 **/
SaErrorT ov_rest_get_power_state(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiPowerStateT *state)
{
        SaErrorT retVal = SA_OK;
        SaHpiRptEntryT *rpt = NULL;
        struct ov_rest_handler *ov_handler = NULL;
        struct oh_handler_state *handler = NULL;
	char *url = NULL;

        if (oh_handler == NULL || state == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        ov_handler = (struct ov_rest_handler *) handler->data;

//FIXME: 
// Need to check functionality of "lock_ov_rest_handler"
        retVal = lock_ov_rest_handler(ov_handler); 
        if (retVal != SA_OK) {
                err("OV REST handler is locked while calling __func__"
			" for resource id %d", resource_id);
                return retVal;
        }

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                err("No POWER Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

	REST_CON *conn = ov_handler->connection;

	retVal = get_url_from_idr(handler, resource_id, &url);
	if (retVal != SA_OK) {
		err("Error in getting url from idr for %d resource id",
								resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	WRAP_ASPRINTF(&conn->url, "https://%s%s", conn->hostname, url);
	wrap_free (url);

        switch (rpt->ResourceEntity.Entry[0].EntityType) {
                case (SAHPI_ENT_SYSTEM_BLADE):
                case (SAHPI_ENT_IO_BLADE):
                case (SAHPI_ENT_DISK_BLADE):
                        retVal = get_server_power_state(conn, state);
                        break;

                case (SAHPI_ENT_SWITCH_BLADE):
                        retVal = get_interconnect_power_state(conn, state);
                        break;

                default:
                        err("Invalid Resource Type %d for resource id %d",
			rpt->ResourceEntity.Entry[0].EntityType, resource_id);
                        retVal = SA_ERR_HPI_INTERNAL_ERROR;
        }

        return retVal;
}

/**
 * ov_rest_set_power_state:
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @state:       Power state
 *
 * Purpose:
 *      Sets the power state of the resource.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_set_power_state(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiPowerStateT state)
{
        SaErrorT rv = SA_OK;
        SaHpiRptEntryT *rpt = NULL;
        struct ov_rest_handler *ov_handler = NULL;
        struct oh_handler_state *handler = NULL;
	char *url = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        ov_handler = (struct ov_rest_handler *) handler->data;

        rv = lock_ov_rest_handler(ov_handler);
        if (rv != SA_OK) {
                err("OV REST handler is locked while calling __func__ "
			"for resource id %d", resource_id);
                return rv;
        }

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("Failed to get RPT for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_POWER)) {
                err("No POWER Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

	REST_CON *conn = ov_handler->connection;
        wrap_free(conn->url);

        rv = get_url_from_idr(handler, resource_id, &url);
        if (rv != SA_OK) {
                err("Error in getting url from idr for %d resource id",
								resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        WRAP_ASPRINTF(&conn->url, "https://%s%s", conn->hostname, url);
        wrap_free (url);

        switch (rpt->ResourceEntity.Entry[0].EntityType) {
                 case (SAHPI_ENT_SYSTEM_BLADE):
                        rv = set_server_power_state(conn, state);
                        break;

                 case (SAHPI_ENT_IO_BLADE):
                 case (SAHPI_ENT_DISK_BLADE):
                        return(SA_ERR_HPI_UNSUPPORTED_API);

                 case (SAHPI_ENT_SWITCH_BLADE):
                        rv = set_interconnect_power_state(conn, state);
                        break;

                 default:
                        err("Invalid Resource Type %d for resource id %d",
			rpt->ResourceEntity.Entry[0].EntityType, resource_id);
                        return SA_ERR_HPI_UNKNOWN;
        }

        return rv;
}

/**
 * get_server_power_state:
 *      @con:        Pointer to REST_CON.
 *      @state:      Pointer to power state of the server-hardware.
 *
 * Purpose:
 *      Gets the server-hardware power state.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT get_server_power_state(REST_CON *conn,
                                SaHpiPowerStateT *state)
{
        SaErrorT rv = SA_OK;
	const char *powerState = NULL;

        if (conn == NULL || state == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

	OV_STRING response = {0};
	rv = rest_get_request(conn, &response);
        if (rv != SA_OK) {
                err("Get blade status failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	json_object *jObj = response.jobj;
	if(!jObj){
		err("Invalid Response");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	jObj = ov_rest_wrap_json_object_object_get(jObj, "powerState");
	if(!jObj){
		err("Invalid Response");
		ov_rest_wrap_json_object_put(response.jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	powerState = json_object_get_string(jObj);
	if(!powerState){
		err("Invalid PowerState Null");
		ov_rest_wrap_json_object_put(response.jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	if (strcmp(powerState, "On") ==0)
		*state = SAHPI_POWER_ON;
	else if (strcmp(powerState, "Off") ==0)
		*state = SAHPI_POWER_OFF;
	else{
		err("Wrong (REBOOT) or Unknown Power State detected for "
							"Server");
		ov_rest_wrap_json_object_put(response.jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
//FIXME:
//	Do we need to handle "PoweringOn","PoweringOff", "Resetting" state?
        ov_rest_wrap_json_object_put(response.jobj);
        return SA_OK;
}

/**
 * get_interconnect_power_state
 *      @con:        Pointer to REST_CON
 *      @state:      Pointer to power state of the interconnect blade
 *
 * Purpose:
 *      Gets the interconnect power state.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT get_interconnect_power_state(REST_CON *conn,
                                      SaHpiPowerStateT *state)
{
        SaErrorT rv = SA_OK;
	const char *powerState = NULL;

        if (conn == NULL || state == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        OV_STRING response = {0};
        rv = rest_get_request(conn, &response);
        if (rv != SA_OK) {
                err("Get interconnect tray status failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        json_object *jObj = response.jobj;
	if(!jObj){
		err("Invalid Response");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
        jObj = ov_rest_wrap_json_object_object_get(jObj, "powerState");
	if(!jObj){
		err("Invalid Response");
		ov_rest_wrap_json_object_put(response.jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	powerState = json_object_get_string(jObj);
	if(!powerState){
		err("Invalid PowerState NULL");
		ov_rest_wrap_json_object_put(response.jobj);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
        if (strcmp(powerState, "On") ==0)
                *state = SAHPI_POWER_ON;
        else if (strcmp(powerState, "Off") ==0)
                *state = SAHPI_POWER_OFF;
        else if (strcmp(powerState, "Unknown") ==0)
                *state = SAHPI_POWER_OFF;
        else{
                err("Wrong (REBOOT) or Unknown Power State detected for "
						"Interconnect");
		ov_rest_wrap_json_object_put(response.jobj);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        ov_rest_wrap_json_object_put(response.jobj);
        return SA_OK;
}

/**
 * set_server_power_state:
 *      @con:        Pointer to the REST_CON.
 *      @state:      Power state of the server-hardware.
 *
 * Purpose:
 *      Sets the power state of the server-hardware,
 *      if the current state is not same as requested state.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - on success.
 *      SA_ERR_HPI_INVALID_PARAMS  - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR  - on failure.
 **/
SaErrorT set_server_power_state(REST_CON *conn,
                                SaHpiPowerStateT state)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT tmp;

        if (conn == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = get_server_power_state(conn, &tmp);
        if (rv != SA_OK) {
                err("Get server power state failed");
                return rv;
        }

        if (state == tmp) {
                err("Nothing to be done. Blade is in the requested state");
                return SA_OK;
        }
	
        switch (state) {
                case (SAHPI_POWER_ON):
			rv = do_server_op(conn, "On", "MomentaryPress");
                        if (rv != SA_OK) {
                                err("Set blade power to power on failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
			
                       break;

                case (SAHPI_POWER_OFF):
			rv = do_server_op(conn, "Off", "MomentaryPress");
                        if (rv != SA_OK) {
                                err("Set blade power to power off failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
		        
                        break;

                case (SAHPI_POWER_CYCLE):
                        if (tmp != SAHPI_POWER_OFF) {
				rv = do_server_op(conn, "Off", "PressAndHold");
                                if (rv != SA_OK) {
                                        err("Set blade power to power off "
                                            "failed");
                                        return SA_ERR_HPI_INTERNAL_ERROR;
                                }

				sleep(OV_SERVER_POWER_OFF_WAIT_PERIOD);
			}

			rv = do_server_op(conn, "On", "MomentaryPress");
                        if (rv != SA_OK) {
                                err("Set blade power to power on failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                       break;

                default:
                        err("Invalid power state %d", state);
                        return SA_ERR_HPI_INVALID_PARAMS;
        }

        return SA_OK;
}

/**
 * set_interconnect_power_state:
 *      @con:        Pointer to the REST_CON.
 *      @state:      Power state of the interconnect blade.
 *
 * Purpose:
 *      Sets the power state of the interconnect blade,
 *      if the current state is not same as requested state.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                      - on success.
 *      SA_ERR_HPI_INVALID_PARAMS  - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR  - on failure.
 **/
SaErrorT set_interconnect_power_state(REST_CON *conn,
                                      SaHpiPowerStateT state)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT tmp;

        if (conn == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rv = get_interconnect_power_state(conn, &tmp);
        if (rv != SA_OK) {
                err("Get interconnect power state failed");
                return rv;
        }

        if (state == tmp) {
                err("Nothing to be done. Interconnect is in requested state");
                return SA_OK;
        }

        switch (state) {
                case (SAHPI_POWER_ON):
			rv = do_interconnect_op(conn, "On");
                        if (rv != SA_OK) {
                                err("Set interconnect power to power on "
                                    "failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }

                        break;
                case (SAHPI_POWER_OFF):
			rv = do_interconnect_op(conn, "Off");
                        if (rv != SA_OK) {
                                err("Set interconnect power to power on "
                                    "failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }

                        break;

                case (SAHPI_POWER_CYCLE):
                        if (tmp != SAHPI_POWER_OFF) {
				rv = do_interconnect_op(conn, "Off");
                                if (rv != SA_OK) {
                                        err("Set interconnect power to "
                                            "power on failed");
                                        return SA_ERR_HPI_INTERNAL_ERROR;
                                }
				//FIXME:
				// Do we need delay at this point ?
                        }

			rv = do_interconnect_op(conn, "On");
                        if (rv != SA_OK) {
                                err("Set interconnect power to power on "
                                    "failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        break;

                default:
                        err("Invalid power state %d", state);
                        return SA_ERR_HPI_INVALID_PARAMS;
        }

        return SA_OK;
}


void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
                __attribute__ ((weak, alias("ov_rest_get_power_state")));

void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
                __attribute__ ((weak, alias("ov_rest_set_power_state")));

