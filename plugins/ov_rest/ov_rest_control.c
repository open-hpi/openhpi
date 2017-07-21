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
 *
 *  This file handles all the control functionality related apis.
 *
 *      ov_rest_get_control_state       - Control ABI to return the control
 *                                        state and mode of the resource
 *
 *      ov_rest_set_control_state       - Control ABI to set the control
 *                                        state the resource
 *
 *      ov_rest_build_control_rdr        - Creates and adds the control rdr
 *
 *	ov_rest_set_pwr_cntrl()		- Sets the power control
 *
 *	ov_rest_get_pwr_cntrl()		- Gets the power control
 *
 *	ov_rest_set_uid_cntrl()		- Sets the UID control
 *
 *	ov_rest_get_uid_cntrl()		- Gets the UID control
 *
 *
 */

#include <stdio.h>
#include "sahpi_wrappers.h"
#include "ov_rest_control.h"

/* Forward declaraction for static functions */
static SaErrorT ov_rest_set_pwr_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiResourceIdT resource_id,
                                      SaHpiCtrlStateDigitalT control_state);
static SaErrorT ov_rest_get_pwr_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiResourceIdT resource_id,
                                      SaHpiCtrlStateDigitalT *control_state);
static SaErrorT ov_rest_set_uid_cntrl(struct oh_handler_state *oh_handler,
                               	      SaHpiRptEntryT *rpt,
			              SaHpiCtrlStateDigitalT control_state);
static SaErrorT ov_rest_get_uid_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiRptEntryT *rpt,
				      SaHpiCtrlStateDigitalT *control_state);
const char* uid_state[] = { "Off","On","Blink","Blinking",
                            "Flashing","Unsupported","NA"
                          };

/**
 * valToIndex:
 *      @arrStr: Pointer to pointer to constant character.
 *      @str   : Pointer to constant character.
 *
 * Purpose:
 *      Returns index of a string in an array.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      0    - On Sucess.
 *      -1   - On Failure.
 **/
static int valToIndex(const char**arrStr, const char* str)
{
  int ret = 0;

  if (arrStr && str) {
    while (*arrStr) {
      if (strcmp(*arrStr, str)==0)
          return ret;
      arrStr++; ret++;
    }
  }

  ret = -1;
  return ret;
}



/**
 * ov_rest_get_control_state
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Control rdr number
 *      @mode: Mode of the control
 *      @state: State of the control
 *
 * Purpose:
 *      Gets the current state and the mode of the control object
 *
 * Detailed Description:
 *      - Gets the current state and the default mode of control object
 *        of either server blade or interconnect
 *      - To determine the control state, power state of the resource is
 *        retrieved and is appropriately mapped to control state
 *      - Plug-in does not support changing the control mode
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_CONTROL
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INVALID_RESOURCE - Resource does not exist
 *      SA_ERR_HPI_NOT_PRESENT - Control not present
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/
SaErrorT ov_rest_get_control_state(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiCtrlNumT rdr_num,
                                   SaHpiCtrlModeT *mode,
                                   SaHpiCtrlStateT *state)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
	struct ov_rest_handler *ov_handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        SaHpiCtrlTypeT type;
        SaHpiCtrlModeT ctrl_mode;
        SaHpiCtrlStateT ctrl_state;
        SaHpiCtrlRecT *ctrl = NULL;
	SaHpiCtrlStateDigitalT control_digital_state = 0;

	char *url =NULL;

        if (oh_handler == NULL || mode == NULL || state == NULL) {
                err("Invalid parameter.");
                return (SA_ERR_HPI_INVALID_PARAMS);
        }

        handler = (struct oh_handler_state *) oh_handler;
	ov_handler = (struct ov_rest_handler *) handler->data;

        rpt = oh_get_resource_by_id (handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("Resource RPT is NULL for resource id %d",
                                            resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                err("Invalid resource capability for resource id %d",
                                            resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type(handler->rptcache, resource_id,
                                 SAHPI_CTRL_RDR, rdr_num);
        if (rdr == NULL) {
                err("Resource RDR %d is NULL for resource id %d",
                                            rdr_num, resource_id);
               return (SA_ERR_HPI_NOT_PRESENT);
        }

	/* Set connection */
        REST_CON *conn = ov_handler->connection;
        rv = get_url_from_idr(handler, resource_id, &url);
        if (rv != SA_OK) {
                err("Error in getting url from idr for %d resource id",
                                                             resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        WRAP_ASPRINTF(&conn->url, "https://%s%s", conn->hostname, url);
        free (url);

        ctrl = &(rdr->RdrTypeUnion.CtrlRec);
        ctrl_mode = ctrl->DefaultMode.Mode;

        /* Set control mode of return parameter to Manual mode
         * Manual mode is the only mode supported by plug-in
         */
        *mode = ctrl_mode;
        type = ctrl->Type;
        ctrl_state.Type = type;

        switch (rdr_num){
                case OV_REST_PWR_CNTRL:
                        rv = ov_rest_get_pwr_cntrl(handler, resource_id, 
						   &control_digital_state);
        	        ctrl_state.StateUnion.Digital = control_digital_state;
                        if (rv != SA_OK) {
                          err("Failed to get the power state RDR for"
                              " resource id %d", resource_id);
                          return rv;
                        }
			break;
                case OV_REST_UID_CNTRL:
                        rv = ov_rest_get_uid_cntrl(handler, rpt,
                                                   &control_digital_state);
        	        ctrl_state.StateUnion.Digital = control_digital_state;
                        if (rv != SA_OK) {
                          err("Failed to get the uid status for"
                              " resource id %d", resource_id);
                          return rv;
                        }
			break;
                default:
                        err("Invalid control rdr num %d for the resource id "
				"%d", rdr_num, resource_id);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Return the appropriately mapped control state */
        *state = ctrl_state;
        return rv;
}

/**
 * ov_rest_set_control_state:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Control rdr number
 *      @mode: Mode of the control
 *      @state: State of the control
 *
 * Purpose:
 *      Sets the current state of the control object. Mode setting is not
 *      allowed
 *
 * Detailed Description:
 *      - Validates the state parameter and sets the control state of resource
 *        to the specified value
 *      - the current state and the default mode of control object
 *        of either server blade or interconnect
 *      - To determine the control state, power state of the resource is
 *        retrieved and is appropriately mapped to control state
 *      - Plug-in does not support changing the control mode
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_CONTROL
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INVALID_RESOURCE - Resource does not exist
 *      SA_ERR_HPI_NOT_PRESENT - Control not present
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 *      SA_ERR_HPI_INVALID_DATA - Invalid Control Mode/State specified
 *      SA_ERR_HPI_UNSUPPORTED_PARAMS - Setting the control mode
 **/
SaErrorT ov_rest_set_control_state(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiCtrlNumT rdr_num,
                                   SaHpiCtrlModeT mode,
                                   SaHpiCtrlStateT *state)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
	struct ov_rest_handler *ov_handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        SaHpiCtrlRecT *ctrl = NULL;
	char *url =NULL;

        if (oh_handler == NULL || state == NULL) {
                err("Invalid parameter.");
                return (SA_ERR_HPI_INVALID_PARAMS);
        }

        handler = (struct oh_handler_state *) oh_handler;
	ov_handler = (struct ov_rest_handler *) handler->data;

        rpt = oh_get_resource_by_id (handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("Resource RPT is NULL for resource id %d",
                                               resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
                err("Invalid resource capability for resource id %d",
                                               resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        rdr = oh_get_rdr_by_type (handler->rptcache, resource_id, 
				  SAHPI_CTRL_RDR, rdr_num);
        if (rdr == NULL) {
                err("Resource RDR %d is NULL for resource id %d",
                                     rdr_num, resource_id);
                return (SA_ERR_HPI_NOT_PRESENT);
        }
        ctrl = &(rdr->RdrTypeUnion.CtrlRec);

        /* Validate the state specified in the parameter list */
        rv = oh_valid_ctrl_state_mode ((ctrl), mode, state);
        if (rv != SA_OK) {
                err("Control state specified is invalid for resource"
                    " id %d", resource_id);
                return (rv);
        }

        /* Auto mode is not supported */
        if (mode == SAHPI_CTRL_MODE_AUTO) {
                err( "AUTO CONTROL MODE is not supported for resource"
                    " id %d", resource_id);
                return SA_ERR_HPI_UNSUPPORTED_PARAMS;
        }

	/* Return error for Non digital type control request */
	if ((state->Type != SAHPI_CTRL_TYPE_DIGITAL) &&
	    (state->Type != SAHPI_CTRL_TYPE_DISCRETE) &&
	    (state->Type != SAHPI_CTRL_TYPE_ANALOG)) {
		err("Control type %d not supported for resource id %d",
                       state->Type, resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
// Fix This Below code is duplicated in few functions called after this Block
//  Hemanth Identified this as a Bug
        /* Set connection */
        REST_CON *conn = ov_handler->connection;
        rv = get_url_from_idr(handler, resource_id, &url);
        if (rv != SA_OK) {
                err("Error in getting url from idr for %d resource id",
								resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        WRAP_ASPRINTF(&conn->url, "https://%s%s", conn->hostname, url);
        free (url);
	
        /* If control mode is MANUAL and specified state could be digital,
         * discrete, or analog type, then the control state is updated
         * with specified state value
         */
	if (state->Type == SAHPI_CTRL_TYPE_DIGITAL) {
		ctrl->TypeUnion.Digital.Default = state->StateUnion.Digital;
	}
	else {
	        if (state->Type == SAHPI_CTRL_TYPE_DISCRETE) {
		        ctrl->TypeUnion.Discrete.Default =
                          state->StateUnion.Discrete;
                }
                else {
		        ctrl->TypeUnion.Analog.Default =
                          state->StateUnion.Analog;
                }
	}
	switch (rdr_num) {
		case OV_REST_PWR_CNTRL:
			rv = ov_rest_set_pwr_cntrl(handler, resource_id,
						   state->StateUnion.Digital);
			if (rv != SA_OK) {
			  err("Set power state failed for resource id %d",
                                                                resource_id);
			  return rv;
			}
			break;
		case OV_REST_UID_CNTRL:
			rv = ov_rest_set_uid_cntrl(handler, rpt,
						   state->StateUnion.Digital);
			if (rv != SA_OK) {
			  err("Set uid state failed for resource id %d",
                                                                resource_id);
			  return rv;
			}
			break;
		default:
			err("Invalid control rdr num %d for resource id %d",
                                                       rdr_num, resource_id);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
        return SA_OK;
}


/**
 * ov_rest_build_control_rdr:
 *      @oh_handler: Handler data pointer
 *      @rdr: Pointer to the rdr structure
 *      @resource_id: Resource ID
 *      @control_num: Control rdr number
 *      @analogLimitLow: RDR lower limit for analog control
 *      @analogLimitHigh: RDR upper limit for analog control
 *
 * Purpose:
 *      Builds the control rdr.
 *
 * Detailed Description:
 *      - Retrieves and populates the control rdr contents from global
 *	  controls array based in control num. 
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/
SaErrorT ov_rest_build_control_rdr(struct oh_handler_state *oh_handler,
				   SaHpiRdrT *rdr,
				   SaHpiResourceIdT resource_id,
				   SaHpiCtrlNumT control_num,
                                   int analogLimitLow,
                                   int analogLimitHigh)
{
        SaHpiRptEntryT *rpt = NULL;

        if (oh_handler == NULL || rdr == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
        if (!rpt) {
                err("Could not find rpt for resource id %d", resource_id);
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }

        /* Set the control rdr with default values */
        rdr->Entity = rpt->ResourceEntity;
        rdr->RdrType = SAHPI_CTRL_RDR;
	rdr->RdrTypeUnion.CtrlRec = ov_rest_cntrl_arr[control_num].control;


	oh_init_textbuffer(&(rdr->IdString));
	oh_append_textbuffer(&(rdr->IdString),
			ov_rest_cntrl_arr[control_num].comment);

	if (rdr->RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_ANALOG) {
                rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Min =
                  analogLimitLow;
                rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Max =
                  analogLimitHigh;
		rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Default = 
		  analogLimitLow;
        } 
        return SA_OK;
}

/**
 * ov_rest_get_uid_cntrl
 *      @oh_handler: Handler data pointer
 *      @rpt: Pointer to the rpt structure
 *      @control_state: Pointer to digital control state
 *
 * Purpose:
 *      Retrieves the UID status of the resource 
 *
 * Detailed Description:
 *      - Retrieves the UID status of different resources based on entity type.
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/
static SaErrorT ov_rest_get_uid_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiRptEntryT *rpt,
				      SaHpiCtrlStateDigitalT *control_state)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;
        struct enclosureInfo encInfo = {{0}}; 
	struct serverhardwareInfo servInfo = {0};
	struct interconnectInfo intconInfo = {0};
	enum uidStatus  uid_status;
	OV_STRING response = {0};

        if (oh_handler == NULL || rpt == NULL || control_state == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;
	REST_CON *conn = ov_handler->connection;

        /* Check whether the re-discovery is started by trying to lock the
	 * ov_handler mutex
	 */
        rv = lock_ov_rest_handler(ov_handler);
        if (rv != SA_OK) {
                err("OV REST handler is locked while calling __func__ "
			"for resource id %d", rpt->ResourceId);
                return rv;
        }

        switch (rpt->ResourceEntity.Entry[0].EntityType) {
                case (SAHPI_ENT_SYSTEM_CHASSIS):
			rv = rest_get_request(conn, &response);
			if (rv != SA_OK || response.jobj == NULL) {
				CRIT("Get enclosure status failed "
					"for resource id %d",
						rpt->ResourceId);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}	
			ov_rest_json_parse_enclosure(response.jobj, &encInfo);
  	                uid_status = valToIndex(uid_state, encInfo.uidState);
			ov_rest_wrap_json_object_put(response.jobj);
                        break;
                case (SAHPI_ENT_SYSTEM_BLADE):
                case (SAHPI_ENT_IO_BLADE):
                case (SAHPI_ENT_DISK_BLADE):
			rv = rest_get_request(conn, &response);
			if (rv != SA_OK || response.jobj == NULL) {
				CRIT("Get Blade status failed "
					"for resource id %d",
						rpt->ResourceId);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			ov_rest_json_parse_server(response.jobj, &servInfo);
                        uid_status = valToIndex(uid_state, servInfo.uidState);
			ov_rest_wrap_json_object_put(response.jobj);
                        break;

                case (SAHPI_ENT_SWITCH_BLADE):
			rv = rest_get_request(conn, &response);
			if (rv != SA_OK || response.jobj == NULL) {
				CRIT("Get Interconnect status failed "
					"for resource id %d",
						rpt->ResourceId);
				return SA_ERR_HPI_INTERNAL_ERROR;
			}		
			ov_rest_json_parse_interconnect(response.jobj, 
							&intconInfo);
                        uid_status = valToIndex(uid_state,intconInfo.uidState);
			ov_rest_wrap_json_object_put(response.jobj);
                        break;

                default:
                        CRIT("Invalid Resource Type %d for resource id %d",
                                rpt->ResourceEntity.Entry[0].EntityType,
				rpt->ResourceId);
                        return (SA_ERR_HPI_INTERNAL_ERROR);
        }

	switch (uid_status) {
		case UID_ON:
		case UID_BLINK:
		case UID_BLINKING:
		case UID_FLASHING:
			*control_state = SAHPI_CTRL_STATE_ON;
			break;
		case UID_OFF:
		case UID_NA:
		case UID_UNSUPPORTED:	//FIXME: Need verify how to deel?
			*control_state = SAHPI_CTRL_STATE_OFF;
			break;
		default:
			err("Invalid uid status %d for Resource Type %d "
				"of id %d", uid_status,
				rpt->ResourceEntity.Entry[0].EntityType,
				rpt->ResourceId);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}
	return SA_OK;
}

/**
 * ov_rest_set_uid_cntrl
 *      @oh_handler: Handler data pointer
 *      @rpt: Pointer to the rpt structure
 *      @control_state: Digital control state
 *
 * Purpose:
 *      Sets the UID status of the resource 
 *
 * Detailed Description:
 *      - Sets the UID status of different resources based on entity type.
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/

static SaErrorT ov_rest_set_uid_cntrl(struct oh_handler_state *oh_handler,
                               	      SaHpiRptEntryT *rpt,
			              SaHpiCtrlStateDigitalT control_state)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;
        OV_STRING response = {0};
	char *postField = NULL;

        if (oh_handler == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;
	REST_CON *conn = ov_handler->connection;

        /* Check whether the re-discovery is started by trying to lock the
	 * ov_handler mutex
	 */
        rv = lock_ov_rest_handler(ov_handler);
        if (rv != SA_OK) {
                err("OV REST handler is locked while calling __func__ "
			"for resource id %d", rpt->ResourceId);
                return rv;
        }

	/* Return error if the control state is PULSE_ON or PULSE_OFF */
	if ((control_state == SAHPI_CTRL_STATE_PULSE_ON) ||
	    (control_state == SAHPI_CTRL_STATE_PULSE_OFF)) {
		err("Setting the control state to PULSE ON/OFF not supported "
			"for resource id %d", rpt->ResourceId);
		return SA_ERR_HPI_INVALID_DATA;
	}

	/* Map the control state to uid status */
	if (control_state == SAHPI_CTRL_STATE_ON) {
		WRAP_ASPRINTF(&postField,"[{ \"op\":\"replace\","
			"\"path\":\"/uidState\","
			"\"value\":\"%s\"}]","On");
	} else {
		WRAP_ASPRINTF(&postField,"[{ \"op\":\"replace\","
			"\"path\":\"/uidState\","
			"\"value\":\"%s\"}]","Off");
	}
        
        switch (rpt->ResourceEntity.Entry[0].EntityType) {
                case (SAHPI_ENT_SYSTEM_CHASSIS):
			rv = rest_patch_request (conn, &response, postField);
                        if (rv != SA_OK) {
                                err("Set enclosure UID failed for resource "
					"id %d", rpt->ResourceId);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        break;
                case (SAHPI_ENT_SYSTEM_BLADE):
                case (SAHPI_ENT_IO_BLADE):
                case (SAHPI_ENT_DISK_BLADE):
			rv = rest_patch_request (conn, &response, postField);
                        if (rv != SA_OK) {
                                err("Set Blade UID failed for resource id %d",
							rpt->ResourceId);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        break;
                case (SAHPI_ENT_SWITCH_BLADE):
			rv = rest_patch_request (conn, &response, postField);
                        if (rv != SA_OK) {
                                err("Set Interconnect UID failed for resource"
					" id %d", rpt->ResourceId);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        break;
                default:
                        err("Invalid Resource Type %d for resource id %d",
                              rpt->ResourceEntity.Entry[0].EntityType,
				rpt->ResourceId);
                        return (SA_ERR_HPI_INTERNAL_ERROR);
        }
        return SA_OK;
}


/**
 * ov_rest_get_pwr_cntrl:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource id
 *      @control state: Pointer to digital control state
 *
 * Purpose:
 *      Gets the control state of power controls on resource
 *
 * Detailed Description:
 *      - Gets the power state of resource and maps it to control state
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/
static SaErrorT ov_rest_get_pwr_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiResourceIdT resource_id,
                                      SaHpiCtrlStateDigitalT *control_state)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT power_state;

	if (oh_handler == NULL || control_state == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	
	rv = ov_rest_get_power_state(oh_handler, resource_id,
				     &power_state);
	if (rv != SA_OK) {
		err("Failed to get the power state RDR for resource id %d",
                                         resource_id);
		return rv;
	}

	switch (power_state) {
		case SAHPI_POWER_ON:
			*control_state = SAHPI_CTRL_STATE_ON;
			break;
		case SAHPI_POWER_OFF:
			*control_state = SAHPI_CTRL_STATE_OFF;
			break;
		default:
			err("Invalid power state %d detected for "
				"Resource ID %d", power_state,resource_id);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}

	return SA_OK;
}

/**
 * ov_rest_set_pwr_cntrl:
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource id
 *      @control state: Digital control state
 *
 * Purpose:
 *      Sets the control state of power controls on resource
 *
 * Detailed Description:
 *      - Sets the power state of resource after mapping the control state
 *	  appropriate power state
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameter
 *      SA_ERR_HPI_INTERNAL_ERROR - Internal error encountered
 **/
static SaErrorT ov_rest_set_pwr_cntrl(struct oh_handler_state *oh_handler,
				      SaHpiResourceIdT resource_id,
                                      SaHpiCtrlStateDigitalT control_state)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT power_state;

	if (oh_handler == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	
	/* Return error if the control state is PULSE_ON or PULSE_OFF */
	if ((control_state == SAHPI_CTRL_STATE_PULSE_ON) ||
	    (control_state == SAHPI_CTRL_STATE_PULSE_OFF)) {
		err("Setting the control state to PULSE ON/OFF not supported"
			" for resource id %d", resource_id);
		return SA_ERR_HPI_INVALID_DATA;
	}

	if (control_state == SAHPI_CTRL_STATE_ON) {
		power_state = SAHPI_POWER_ON;
	} else {
		power_state = SAHPI_POWER_OFF;
	}

	rv = ov_rest_set_power_state(oh_handler, resource_id,
				     power_state);
	if (rv != SA_OK) {
		err("Failed to set the power state of resource id %d",
                                                      resource_id);
		return rv;
	}
	return SA_OK;
}


void *oh_get_control_state (void *,
                            SaHpiResourceIdT,
                            SaHpiCtrlNumT,
                            SaHpiCtrlModeT *,
                            SaHpiCtrlStateT *)
               __attribute__ ((weak, alias ("ov_rest_get_control_state")));

void *oh_set_control_state (void *,
                            SaHpiResourceIdT,
                            SaHpiCtrlNumT,
                            SaHpiCtrlModeT,
                            SaHpiCtrlStateT *)
               __attribute__ ((weak, alias ("ov_rest_set_control_state")));

