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
 *
 * This file handles all the resource reset states related apis.
 *
 *      ov_rest_get_reset_state()   - API to return the reset state of chassis
 *                                    components
 *
 *      ov_rest_set_reset_state()   - API to set the reset state for all the
 *                                    chassis components
 **/

#include "sahpi_wrappers.h"
#include "ov_rest_reset.h"

/**
 * ov_rest_wait_for_action_completion 
 *      @oh_handler:  Pointer to openhpi handler
 *      @resource_id: Resource id
 *      @action:      Pointer to reset action
 *
 * Purpose:
 *      waits for the requested action completion
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INVALID_STATE  - on invalid power state.
 **/
static SaErrorT ov_rest_wait_for_action_completion(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiResetActionT action)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT state, expected = SAHPI_POWER_ON;
        int polls = 0;

        if (oh_handler == NULL) {
                err("Invalid parameter oh_handler");
                return SA_ERR_HPI_INVALID_PARAMS;
        }	

        if ( action == SAHPI_RESET_DEASSERT )
                expected = SAHPI_POWER_ON;
	else if ( action == SAHPI_RESET_ASSERT )
                expected = SAHPI_POWER_OFF;
        else {
                err("Invalid power state %d passed", action);
                return( SA_ERR_HPI_INVALID_STATE);
        }

        for (polls=0; polls < OV_MAX_POWER_POLLS; polls++) {
                ov_rest_get_power_state(oh_handler, resource_id, &state);
                if (state == expected)
                        break; 
                sleep(OV_POWER_POLL_SLEEP_SECONDS);
        }

        if( polls == OV_MAX_POWER_POLLS){
                err("Failed to get the requested state even after %d seconds",
                              OV_MAX_POWER_POLLS*OV_POWER_POLL_SLEEP_SECONDS);
                return( SA_ERR_HPI_INVALID_STATE);
        }

        return rv;
}

/**
 * ov_rest_get_reset_state
 *      @oh_handler:  Pointer to openhpi handler
 *      @resource_id: Resource id
 *      @action:      Pointer to reset action
 *
 * Purpose:
 *      gets the reset state of the chassis resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_get_reset_state(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiResetActionT *action)
{
        SaErrorT rv = SA_OK;
        SaHpiPowerStateT state;

        if (oh_handler == NULL || action == NULL) {
                err("Invalid parameters oh_handler/action");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the current power state of the resource */
        rv = ov_rest_get_power_state(oh_handler, resource_id, &state);
        if (rv != SA_OK) {
                err("Get power state failed for server id %d", resource_id);
                return rv;
        }

        switch (state) {
                case (SAHPI_POWER_ON):
                        *action = SAHPI_RESET_DEASSERT;
                        break;

                case (SAHPI_POWER_OFF):
                        *action = SAHPI_RESET_ASSERT;
                        break;

                /* Power cycle is a momentary state
                 * Hence, resource should never give the 'power cycle' as
                 * its current power state
                 */
                case (SAHPI_POWER_CYCLE):
                        err("Wrong reset state (Power cycle) detected"
				" for server id %d", resource_id);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                        break;
                default:
                        err("Wrong reset state %d detected for server id %d",
						state, resource_id);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * ov_rest_set_reset_state
 *      @oh_handler:  Pointer to openhpi handler
 *      @resource_id: Resource id
 *      @action:      Reset action
 *
 * Purpose:
 *      sets the reset state of the resource in the chassis
 *
 * Detailed Description:
 *      - Resource capability will be checked based on the resource id
 *      - and then based on the action and type of the entity, different
 *        interface api is used for resetting the resource component.
 *
 * Return values:
 *      SA_OK                      - on success.
 *      SA_ERR_HPI_INVALID_PARAMS  - on wrong parameters
 *      SA_ERR_HPI_INVALID_REQUEST - on request to reset a resource
 *                                   which is powered off.
 *                                   Or on wrong reset request
 *      SA_ERR_HPI_INTERNAL_ERROR  - on failure.
 **/
SaErrorT ov_rest_set_reset_state(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiResetActionT action)
{
        SaErrorT rv = SA_OK, int_err_ret = SA_ERR_HPI_INTERNAL_ERROR;
        SaHpiPowerStateT tmp;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_handler_state *handler = NULL;
        struct ov_rest_handler *ov_handler = NULL;
        char *uri=NULL;
        char *url = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter oh_handler");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        ov_handler = (struct ov_rest_handler *) handler->data;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("There is no resource for %d resource id", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        /* Check whether resource has reset capability */
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_RESET)) {
                err("No RESET Capability for resource id %d",
						resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        switch (action) {
                case SAHPI_RESET_DEASSERT:
                        /* RESET_DEASSERT is equivalent to power on
                         * Call the set power state function with power on
                         */
                        rv = ov_rest_set_power_state(oh_handler, resource_id,
                                                              SAHPI_POWER_ON);
                        if (rv != SA_OK) {
                                err("Set power ON failed for resource id %d",
							resource_id);
                        	return rv;
			}
                        ov_rest_wait_for_action_completion(oh_handler,
					resource_id, SAHPI_RESET_DEASSERT);
                        break;

                case SAHPI_RESET_ASSERT:
                        /* RESET_ASSERT is equivalent to power off
                         * Call the set power state function with power off
                         */
                        rv = ov_rest_set_power_state(oh_handler, resource_id,
                                                     SAHPI_POWER_OFF);
                        if (rv != SA_OK) {
                                err("Set power OFF failed for resource id %d",
								resource_id);
                        	return rv;
			}
                        ov_rest_wait_for_action_completion(oh_handler,
					resource_id, SAHPI_RESET_ASSERT);
                        break;

                case SAHPI_COLD_RESET:
                case SAHPI_WARM_RESET:
                        /* Get the current power state */
                        rv = ov_rest_get_power_state(oh_handler, resource_id,
                                                     &tmp);
                        if (rv != SA_OK) {
                                err("Get power state failed for resource id %d",
								resource_id);
                                return rv;
                        }

                        /* Reset can not be done when resource is in power
                         * off state
                         */
                        if (tmp == SAHPI_POWER_OFF) {
                                return SA_ERR_HPI_INVALID_REQUEST;
                        }

                        /* Check whether ov_handler mutex is locked or not */
                        rv = lock_ov_rest_handler(ov_handler);
                        if (rv != SA_OK) {
                                err("OV REST handler is locked while calling "
					"__func__ for resource id %d",
					resource_id);
                                return rv;
                        }
			
                        REST_CON *conn = ov_handler->connection;
                        wrap_free(conn->url);

                        rv = get_url_from_idr(handler, resource_id, &url);
                        if (rv != SA_OK) {
                                err("Error in getting url from idr for %d"
                                        " resource id", resource_id);
                                return int_err_ret;
                        }
                        WRAP_ASPRINTF(&conn->url, "https://%s%s", conn->hostname,
                                                                        url);
                        wrap_free (url);

                        OV_STRING response = {0};
                        char * postField=NULL;

                        /* Check the resource entity type */
                        switch (rpt->ResourceEntity.Entry[0].EntityType) {
                                case SAHPI_ENT_SYSTEM_BLADE:
                                        /* Resource type is server blade.
                                         * Reset the server blade
                                         */
                                        if (action == SAHPI_COLD_RESET){
                                                WRAP_ASPRINTF(&postField,"%s",
                                                 "{\"powerState\":\"On\",\
                                         \"powerControl\":\"ColdBoot\"}");
                                        }
                                        else{
                                                WRAP_ASPRINTF(&postField,"%s",
                                                 "{\"powerState\":\"On\",\
                                            \"powerControl\":\"Reset\"}");
                                        }

                                        WRAP_ASPRINTF(&uri,"%s/powerState",
                                                                conn->url);
                                        wrap_free(conn->url);
                                        WRAP_ASPRINTF(&conn->url,"%s",uri);
                                        wrap_free(uri);

                                        rv = rest_put_request(conn, &response,
                                                                postField);
                                        wrap_free(postField);

                                        if (rv != SA_OK) {
                                                err("Set blade power to power "
                                                    "reset failed for resource "
							"id %d", resource_id);
                                                return int_err_ret;
                                        }

                                        ov_rest_wait_for_action_completion(
						oh_handler, resource_id,
						SAHPI_RESET_DEASSERT);
                                        break;

                                case SAHPI_ENT_IO_BLADE:
                                case SAHPI_ENT_DISK_BLADE:
                                        return(SA_ERR_HPI_UNSUPPORTED_API);

                                case SAHPI_ENT_SWITCH_BLADE:
                                        /* Resource type is interconnect blade.
                                         * Reset the interconnect blade
                                         */
                                        WRAP_ASPRINTF(&postField,"%s",
                                                 "[{ \"op\": \"replace\",\
                                         \"path\": \"/deviceResetState\",\
                                                  \"value\": \"Reset\" }]");
                                       
                                        rv = rest_patch_request(conn,&response,
                                                                 postField);
                                        wrap_free(postField);

                                        if (rv != SA_OK) {
                                                err("Set interconnect reset "
                                                    "failed for resource id %d",
								resource_id);
                                                return int_err_ret;
                                        }

                                        ov_rest_wait_for_action_completion(
						oh_handler, resource_id,
						SAHPI_RESET_DEASSERT);
                                        break;

                                default:
                                        err("Invalid Resource Type %x",
                                      rpt->ResourceEntity.Entry[0].EntityType);
                                        return int_err_ret;
                        }
                        break;

                default:
                        err("Invalid reset state %d requested", action);
                        return SA_ERR_HPI_INVALID_REQUEST;
        }
        return SA_OK;
}

void * oh_get_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT *)
               __attribute__ ((weak, alias("ov_rest_get_reset_state")));

void * oh_set_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT)
               __attribute__ ((weak, alias("ov_rest_set_reset_state")));

