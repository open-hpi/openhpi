/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
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
 * Neither the name of the Hewlett-Packard Corporation, nor the names
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
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 *      Vivek Kumar <vivek.kumar2@hp.com>
 *      Raghavendra M.S. <raghavendra.ms@hp.com>
 *
 * This file implements the re-discovery functionality. The resources of the
 * HP BladeSystem c-Class are re-discovered, whenever the connection to the
 * active OA is broken or on OA switchover.  Re-discovery is done to sync the
 * plugin with the current state of the resources.
 *
 *      oa_soap_re_discover_resources() - Starts the re-discovery of server
 *                                        blades, interconnect blades, OA,
 *                                        fan and power supplies.
 *
 *      re_discover_server()            - Re-discovers the ProLiant server
 *                                        blades
 *
 *      re_discover_interconnect()      - Re-discovers the interconnect blades
 *
 *      re_discover_fan()               - Re-discovers the fan
 *
 *      re_discover_power_supply()      - Re-discovers the power supply units
 *
 *      re_discover_oa()                - Re-discovers the onboard administrator
 *
 */

#include <oa_soap_plugin.h>

/**
 * oa_soap_re_discover_resources
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
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
SaErrorT oa_soap_re_discover_resources(struct oh_handler_state *oh_handler,
                                       SOAP_CON *con)
{
        SaErrorT rv = SA_OK;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        err("Re-discovery started");
        rv = re_discover_blade(oh_handler, con);
        if (rv != SA_OK) {
                err("Re-discovery of server blade failed");
                return rv;
        }

        rv = re_discover_interconnect(oh_handler, con);
        if (rv != SA_OK) {
                err("Re-discovery of interconnect failed");
                return rv;
        }

        rv = re_discover_fan(oh_handler, con);
        if (rv != SA_OK) {
                err("Re-discovery of fan failed");
                return rv;
        }

        rv = re_discover_ps_unit(oh_handler, con);
        if (rv != SA_OK) {
                err("Re-discovery of power supply unit failed");
                return rv;
        }

        rv = re_discover_oa(oh_handler, con);
        if (rv != SA_OK) {
                err("Re-discovery of OA failed");
                return rv;
        }

        err("Re-discovery completed");
        return rv;
}

/**
 * re_discover_oa
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *
 * Purpose:
 *      Re-discover the OAs.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_oa(struct oh_handler_state *oh_handler,
                        SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
        struct getOaStatus request;
        struct oaStatus response;
        SaHpiInt32T i;
        enum resource_presence_status state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1; i <= oa_handler->oa_soap_resources.oa.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getOaStatus(con, &request, &response);
                if (rv != SOAP_OK) {
                        err("get OA status failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                /* Sometimes, if the OA is absent, then OA status is shown as
                 * STANDBY in getOaStatus response.  As workaround, if OA
                 * status is STANDBY and oaRedudancy state is set to false,
                 * Then, it is considered as ABSENT.
                 *
                 * But, if the OA is recently inserted, then oaRedudancy state
                 * will be set to false.  In this scenario, the OA state will
                 * be wrongly considered as ABSENT.  This is a known limitation.
                 * TODO: Remove this workaround once the fix is available in
                 * OA firmware
                 */
                if ((response.oaRole == OA_ABSENT) ||
                    (response.oaRole == STANDBY &&
                     response.oaRedundancy == HPOA_FALSE)) {
                        /* The OA is absent, check OA is absent in presence
                         * matrix
                         */
                        if (oa_handler->oa_soap_resources.oa.presence[i - 1] ==
                            RES_ABSENT)
                                continue;
                        else
                                state = RES_ABSENT;
                } else {
                        /* The OA is present, check OA is present in presence
                         * matrix
                         */
                        if (oa_handler->oa_soap_resources.oa.presence[i - 1] ==
                            RES_PRESENT)
                                continue;
                        else
                                state = RES_PRESENT;
                }

                if (state == RES_ABSENT) {
                        /* The OA is present according OA presence matrix, but
                         * OA is removed.  Remove the OA resource from rptcache.
                         */
                        rv = remove_oa(oh_handler, i);
                        if (rv != SA_OK) {
                                err("OA <%d> removal failed", i);
                                return rv;
                        } else {
                                err("OA in slot <%d> is removed", i);
                                oa_handler->
                                        oa_soap_resources.oa.presence[i - 1] =
                                        RES_ABSENT;
                        }
                } else {
                        /* The OA is absent according OA presence matrix, but
                         * OA is present.  Add the OA resource to rptcache.
                         */
                        rv = add_oa(oh_handler, con, i);
                        if (rv != SA_OK) {
                                err("OA <%d> add failed", i);
                                return rv;
                        } else {
                                err("OA in slot <%d> is added", i);
                                oa_handler->
                                        oa_soap_resources.oa.presence[i - 1] =
                                        RES_PRESENT;
                        }
                }
        }
        return SA_OK;
}

/**
 * remove_oa
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the extracted OA
 *
 * Purpose:
 *      Removes the OA information from the OpenHPI data store
 *      Updates the status of the OA in OA data structure as absent
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_oa(struct oh_handler_state *oh_handler,
                   SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        char* entity_root = NULL;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event event;
        struct oa_soap_handler *oa_handler = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        update_hotswap_event(oh_handler, &event);
        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType=SAHPI_ENT_SYS_MGMNT_MODULE;
        entity_path.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource rpt is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;

        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;

        /* Push the hotswap event to remove the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));
        /* Remove the resource from plugin rptcache */
        rv = oh_remove_resource(oh_handler->rptcache,
                                event.resource.ResourceId);

        return SA_OK;
}

/**
 * add_oa
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *      @bay_number: Bay number of the extracted OA
 *
 * Purpose:
 *      Re-discover the newly added OA.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT add_oa(struct oh_handler_state *oh_handler,
                SOAP_CON *con,
                SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        struct getOaInfo request;
        struct oaInfo response;
        struct getOaNetworkInfo network_info;
        struct oaNetworkInfo network_info_response;
        struct oa_soap_handler *oa_handler = NULL;
        struct oh_event event;
        struct oa_info *temp = NULL;
        char *user_name = NULL, *password = NULL;
        char url[MAX_URL_LEN];

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the oa_info structure of the inserted OA */
        switch (bay_number) {
                case 1:
                        temp = oa_handler->oa_1;
                        break;
                case 2:
                        temp = oa_handler->oa_2;
                        break;
        }

        /* Get the IP address of the newly inserted OA */
        network_info.bayNumber = bay_number;
        rv = soap_getOaNetworkInfo(con, &network_info, &network_info_response);
        if (rv != SOAP_OK) {
                err("Get OA network info failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the OA user name and password from conf file */
        user_name = (char *) g_hash_table_lookup(oh_handler->config,
                                                 "OA_User_Name");
        password = (char *) g_hash_table_lookup(oh_handler->config,
                                                "OA_Password");

        /* Create the OA URL */
        memset(url, 0, MAX_URL_LEN);
        snprintf(url,
                 strlen(network_info_response.ipAddress) + strlen(PORT) + 1,
                 "%s" PORT, network_info_response.ipAddress);

        /* Lock the inserted OA mutex */
        g_mutex_lock(temp->mutex);
        /* Create the SOAP_CON for inserted OA */
        temp->hpi_con = soap_open(url, user_name, password, HPI_CALL_TIMEOUT);
        if (temp->hpi_con == NULL) {
                err("soap_open for <%s> has failed", url);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        temp->event_con = soap_open(url, user_name, password,
                                    EVENT_CALL_TIMEOUT);
        if (temp->event_con == NULL) {
                err("soap_open for <%s> has failed", url);
                soap_close(temp->hpi_con);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        g_mutex_unlock(temp->mutex);

        update_hotswap_event(oh_handler, &event);
        /* Build the rpt entry */
        rv = build_oa_rpt(oh_handler, bay_number, &(event.resource));
        if (rv != SA_OK) {
                err("Failed to build OA RPT");
                return rv;
        }
        request.bayNumber = bay_number;

        rv = soap_getOaInfo(con, &request, &response);
        if (rv != SOAP_OK) {
                err("Get OA info failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Build the RDRs */
        rv = build_oa_rdr(oh_handler, con, &response, &event);
        if (rv != SA_OK) {
                err("Failed to build OA RDR");
                oh_remove_resource(oh_handler->rptcache,
                                   event.resource.ResourceId);
                return rv;
        }

        event.event.Source = event.resource.ResourceId;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        /* Push the hotswap event to add the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        return SA_OK;
}

/**
 * re_discover_blade
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
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
SaErrorT re_discover_blade(struct oh_handler_state *oh_handler,
                           SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
        struct getBladeInfo request;
        struct bladeInfo response;
        SaHpiInt32T i;
        enum resource_presence_status state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1; i <= oa_handler->oa_soap_resources.server.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getBladeInfo(con, &request, &response);
                if (rv != SOAP_OK) {
                        err("Get blade info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                if (response.presence != PRESENT ||
                   response.bladeType != BLADE_TYPE_SERVER) {
                        /* The server blade is absent, check server is absent
                         * in presence matrix
                         */
                        if (oa_handler->oa_soap_resources.server.presence[i - 1]
                            == RES_ABSENT)
                                continue;
                        else
                                state = RES_ABSENT;
                        /* The server blade is present, check server is present
                         * in presence matrix
                         */
                } else if (oa_handler->oa_soap_resources.server.presence[i - 1]
                           == RES_PRESENT) {
                                /* Check and update the hotswap state of the
                                 * server blade
                                 */
                                rv = update_server_hotswap_state(oh_handler,
                                                                 con, i);
                                if (rv != SA_OK) {
                                        err("Update server hot swap state "
                                            "failed");
                                        return rv;
                                }
                                continue;
                } else
                        state = RES_PRESENT;

                if (state == RES_ABSENT) {
                        /* The server blade is present according OA presence
                         * matrix, but server is removed.  Remove the server
                         * resource from rptcache.
                         */
                        rv = remove_server_blade(oh_handler, i);
                        if (rv != SA_OK) {
                                err("Server blade <%d> removal failed", i);
                                return rv;
                        } else {
                                err("Server in slot <%d> is removed", i);
                                oa_handler->
                                        oa_soap_resources.server.presence[i - 1]
                                        = RES_ABSENT;
                        }
                } else if (state == RES_PRESENT) {
                        /* The server blade is absent according OA presence
                         * matrix, but server is present.  Add the server
                         * resource to rptcache.
                         */
                        rv = add_server_blade(oh_handler, con, &response);
                        if (rv != SA_OK) {
                                err("Server blade <%d> add failed", i);
                                return rv;
                        } else {
                                err("Server in slot <%d> is added", i);
                                oa_handler->
                                        oa_soap_resources.server.presence[i - 1]
                                        = RES_PRESENT;
                        }
                }
        }
        return SA_OK;
}

/**
 * update_server_hotswap_state
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to soap client handler
 *      @bay_number: Bay number of the removed blade
 *
 * Purpose:
 *      Updates the server blade hot swap state in OpenHPI data store
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT update_server_hotswap_state(struct oh_handler_state *oh_handler,
                                     SOAP_CON *con,
                                     SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oa_soap_hotswap_state *hotswap_state = NULL;
        char* entity_root = NULL;
        struct oh_event event;
        SaHpiPowerStateT state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType = SAHPI_ENT_SYSTEM_BLADE;
        entity_path.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource RPT is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        hotswap_state = (struct oa_soap_hotswap_state *)
                oh_get_resource_data(oh_handler->rptcache, rpt->ResourceId);
        if (hotswap_state == NULL) {
                err("Unable to get the resource private data");
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        rv = get_server_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power state");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check whether current hotswap state of the server is same as
         * in hotswap structure in rpt entry
         */
        if ((state == SAHPI_POWER_ON &&
             hotswap_state->currentHsState == SAHPI_HS_STATE_ACTIVE) ||
            (state == SAHPI_POWER_OFF &&
             hotswap_state->currentHsState == SAHPI_HS_STATE_INACTIVE)) {
                return SA_OK;
        }

        /* Hotswap structure in rpt entry is not reflecting the current state
         * Update the hotswap structure and raise hotswap event
         */
        update_hotswap_event(oh_handler, &event);
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;
        switch (state) {
                case SAHPI_POWER_ON:
                        /* Server got powered on.  Update the hotswap
                         * structure.
                         */
                        hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
                        event.rdrs = NULL;
                        /* Raise the hotswap events */
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_INACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_INSERTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_INSERTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                                     SAHPI_HS_STATE_ACTIVE;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;

                case SAHPI_POWER_OFF:
                        /* Server got powered off.  Update the hotswap
                         * structure.
                         */
                        hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;
                        event.rdrs = NULL;
                        /* Raise the hotswap events */
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                                     SAHPI_HS_STATE_INACTIVE;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;

                default :
                        err("unknown power status");
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return rv;
}

/**
 * remove_server_blade
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the removed blade
 *
 * Purpose:
 *      Remove the Server Blade from the OpenHPI infrastructure data store
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_server_blade(struct oh_handler_state *oh_handler,
                             SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        char* entity_root = NULL;
        struct oa_soap_hotswap_state *hotswap_state;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType=SAHPI_ENT_SYSTEM_BLADE;
        entity_path.Entry[0].EntityLocation= bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource rpt is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;

        hotswap_state = (struct oa_soap_hotswap_state *)
                oh_get_resource_data(oh_handler->rptcache,
                                     event.resource.ResourceId);
        if (hotswap_state == NULL) {
                err("Failed to get hotswap state of server blade");
                event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                        SAHPI_HS_STATE_INACTIVE;
        }

        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                hotswap_state->currentHsState;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;

        /* Push the hotswap event to remove the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));
        /* Remove the resource from plugin rptcache */
        rv = oh_remove_resource(oh_handler->rptcache,
                                event.resource.ResourceId);

        return SA_OK;
}

/**
 * add_server_blade
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *      @info:       Pointer to the get blade info response structure
 *
 * Purpose:
 *      Remove the Server Blade from the OpenHPI infrastructure data store
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT add_server_blade(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          struct bladeInfo *info)
{
        SaErrorT rv = SA_OK;
        struct oh_event event;
        SaHpiPowerStateT state;
        SaHpiInt32T bay_number;

        if (oh_handler == NULL || info == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);
        bay_number = info->bayNumber;

        /* Build the server RPR entry */
        rv = build_server_rpt(oh_handler, con, info->name,
                              bay_number, &(event.resource));

        if (rv != SA_OK) {
                err("build inserted server rpt failed");
                return rv;
        }

        /* Build the server RDR */
        rv = build_server_rdr(oh_handler, con, bay_number, &event);
        if (rv != SA_OK) {
                err("build inserted server RDR failed");
                oh_remove_resource(oh_handler->rptcache,
                                   event.resource.ResourceId);
                return rv;
        }

        rv = get_server_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power status");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Raise the hotswap event for the inserted server blade */
        event.event.Source = event.resource.ResourceId;
        event.rdrs = NULL;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_INSERTION_PENDING;
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        event.rdrs = NULL;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_INSERTION_PENDING;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        /* Check the power state of the server.  If the power state is off,
         * may be server got powered off after inserting.  Inserting the
         * server makes to power on automatically.  Hence raise the hotswap
         * events for power off.
         */
        switch (state) {
                case SAHPI_POWER_ON:
                        break;

                case SAHPI_POWER_OFF:
                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_INACTIVE;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;

                default :
                        err("unknown power status");
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }
        return rv;
}

/**
 * re_discover_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *
 * Purpose:
 *      Re-discover the interconnects.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_interconnect(struct oh_handler_state *oh_handler,
                                  SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
        struct getInterconnectTrayStatus request;
        struct interconnectTrayStatus response;
        SaHpiInt32T i;
        enum resource_presence_status state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1;
             i <= oa_handler->oa_soap_resources.interconnect.max_bays;
             i++) {
                request.bayNumber = i;
                rv = soap_getInterconnectTrayStatus(con, &request, &response);
                if (rv != SOAP_OK) {
                        err("Get interconnect tray status failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                if (response.presence != PRESENT) {
                        /* The interconnect is absent,
                         * check interconnect is absent in presence matrix
                         */
                        if (oa_handler->
                            oa_soap_resources.interconnect.presence[i - 1] ==
                            RES_ABSENT)
                                continue;
                        else
                                state = RES_ABSENT;

                        /* The interconnect is present,
                         * check interconnect is present in presence matrix
                         */
                } else if (oa_handler->
                           oa_soap_resources.interconnect.presence[i - 1] ==
                           RES_PRESENT) {
                        /* Check and update the hotswap state of the server
                         * blade
                         */
                        rv = update_interconnect_hotswap_state(oh_handler,
                                                               con, i);
                        if (rv != SA_OK) {
                                err("update interconnect hot state failed");
                                return rv;
                        }
                        continue;
                 } else
                        state = RES_PRESENT;

                if (state == RES_ABSENT) {
                        /* The interconnect is present according OA presence
                         * matrix, but interconnect is removed.  Remove the
                         * interconnect resource from rptcache.
                         */
                        rv = remove_interconnect(oh_handler, i);
                        if (rv != SA_OK) {
                                err("Interconnect blade <%d> removal failed",
                                    i);
                                return rv;
                        } else {
                                err("Interconnect blade <%d> removed", i);
                                oa_handler->oa_soap_resources.interconnect.
                                        presence[i - 1] = RES_ABSENT;
                        }
                } else if (state == RES_PRESENT) {
                        /* The interconnect is absent according OA presence
                         * matrix, but interconnect is added.  Add the
                         * interconnect resource to rptcache.
                         */
                        rv = add_interconnect(oh_handler, con, i);
                        if (rv != SA_OK) {
                                err("Interconnect blade <%d> add failed", i);
                                return rv;
                        } else {
                                err("Interconnect blade <%d> added", i);
                                oa_handler->oa_soap_resources.interconnect.
                                        presence[i - 1] = RES_PRESENT;
                        }
                }
        }
        return SA_OK;
}

/**
 * update_interconnect_hotswap_state
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *      @bay_number: Bay number of the removed blade
 *
 * Purpose:
 *      Updates the interconnect hot swap state in OpenHPI data store
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT update_interconnect_hotswap_state(struct oh_handler_state *oh_handler,
                                           SOAP_CON *con,
                                           SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oa_soap_hotswap_state *hotswap_state = NULL;
        char* entity_root = NULL;
        struct oh_event event;
        SaHpiPowerStateT state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType = SAHPI_ENT_SWITCH_BLADE;
        entity_path.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource RPT is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        hotswap_state = (struct oa_soap_hotswap_state *)
                oh_get_resource_data(oh_handler->rptcache, rpt->ResourceId);
        if (hotswap_state == NULL) {
                err("Unable to get the resource private data");
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        rv = get_interconnect_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power status");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check whether current hotswap state of the interconnect is same as
         * in hotswap structure in rpt entry
         */
        if ((state == SAHPI_POWER_ON &&
             hotswap_state->currentHsState == SAHPI_HS_STATE_ACTIVE) ||
            (state == SAHPI_POWER_OFF &&
             hotswap_state->currentHsState == SAHPI_HS_STATE_INACTIVE)) {
                return SA_OK;
        }

        update_hotswap_event(oh_handler, &event);
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;
        switch (state) {
                case SAHPI_POWER_ON:
                        /* Interconnect got powered on.  Update the hotswap
                         * structure.
                         */
                        hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
                        event.rdrs = NULL;
                        /* Raise the hotswap events */
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_INACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_INSERTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_INSERTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_ACTIVE;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;

                case SAHPI_POWER_OFF:
                        /* Interconnect got powered off.  Update the hotswap
                         * structure.
                         */
                        hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;
                        event.rdrs = NULL;
                        /* Raise the hotswap events */
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_INACTIVE;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;

                default :
                        err("unknown power status");
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return rv;
}

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
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_interconnect(struct oh_handler_state *oh_handler,
                             SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        char* entity_root = NULL;
        struct oh_event event;
        struct oa_soap_hotswap_state *hotswap_state;
        SaHpiEntityPathT root_entity_path;
        SaHpiEntityPathT entity_path;
        SaHpiRptEntryT *rpt = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType = SAHPI_ENT_SWITCH_BLADE;
        entity_path.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource rpt is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;

        hotswap_state = (struct oa_soap_hotswap_state *)
                oh_get_resource_data(oh_handler->rptcache,
                                     event.resource.ResourceId);
        if (hotswap_state == NULL) {
                err("Failed to get hotswap state");
                event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                        SAHPI_HS_STATE_INACTIVE;
        }
        else
                event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                        hotswap_state->currentHsState;

        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;

        /* Push the hotswap event to remove the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));
        /* Remove the resource from plugin rptcache */
        rv = oh_remove_resource(oh_handler->rptcache,
                                event.resource.ResourceId);

        return SA_OK;
}

/**
 * add_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *      @bay_number: Bay number of the removed interconnect
 *
 * Purpose:
 *      Adds the interconnect.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT add_interconnect(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        struct getInterconnectTrayInfo info;
        struct interconnectTrayInfo response;
        struct oh_event event;
        SaHpiPowerStateT state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);

        info.bayNumber = bay_number;
        rv = soap_getInterconnectTrayInfo(con, &info, &response);
        if (rv != SOAP_OK) {
                err("Get Interconnect tray info failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Build the rpt entry */
        rv = build_interconnect_rpt(oh_handler, con,
                                    response.name,
                                    bay_number, &(event.resource));
        if (rv != SA_OK) {
                err("Failed to get interconnect inventory RPT");
                return rv;
        }

        /* Build the RDRs */
        rv = build_interconnect_rdr(oh_handler, con,
                                    bay_number, &event);
        if (rv != SA_OK) {
                err("Failed to get interconnect inventory RDR");
                oh_remove_resource(oh_handler->rptcache,
                                   event.resource.ResourceId);
                return rv;
        }

        event.event.Source = event.resource.ResourceId;

        /* Raise the hotswap event for the inserted interconnect blade */
        event.rdrs = NULL;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                               SAHPI_HS_STATE_INSERTION_PENDING;
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        event.rdrs = NULL;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_INSERTION_PENDING;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                                      SAHPI_HS_STATE_ACTIVE;
        /* Check the power state of the interconnect.  If the power state is
         * off, may be interconnect got powered off after inserting.
         * Inserting the interconnect makes to power on automatically.
         * Hence raise the hotswap events for power off.
         */
        rv = get_interconnect_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power status");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        switch (state) {
                case SAHPI_POWER_ON:
                        break;

                case SAHPI_POWER_OFF:
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));

                        event.rdrs = NULL;
                        event.event.EventDataUnion.HotSwapEvent.
                                PreviousHotSwapState =
                                SAHPI_HS_STATE_EXTRACTION_PENDING;
                        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                                SAHPI_HS_STATE_INACTIVE;
                        break;

                default :
                        err("unknown power status");
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        return SA_OK;
}

/**
 * re_discover_fan
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the SOAP_CON structure
 *
 * Purpose:
 *      Re-discover the Fans.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_fan(struct oh_handler_state *oh_handler,
                         SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
        struct getFanInfo request;
        struct fanInfo response;
        SaHpiInt32T i;
        enum resource_presence_status state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1; i <= oa_handler->oa_soap_resources.fan.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getFanInfo(con, &request, &response);
                if (rv != SOAP_OK) {
                        err("Get fan info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                if (response.presence != PRESENT) {
                        /* The Fan is absent, check Fan is absent in presence
                         * matrix
                         */
                        if (oa_handler->oa_soap_resources.fan.presence[i - 1] ==
                            RES_ABSENT)
                                continue;
                        else
                                state = RES_ABSENT;
                } else {
                        /* The Fan is present, check Fan is present in presence
                         * matrix
                         */
                        if (oa_handler->oa_soap_resources.fan.presence[i - 1] ==
                            RES_PRESENT)
                                continue;
                        else
                                state = RES_PRESENT;
                }

                if (state == RES_ABSENT) {
                        /* The Fan is present according Fan presence matrix,
                         * but Fan is removed.  Remove the Fan resource from
                         * rptcache.
                         */
                        rv = remove_fan(oh_handler, i);
                        if (rv != SA_OK) {
                                err("Fan <%d> removal failed", i);
                                return rv;
                        } else {
                                err("Fan <%d> removed", i);
                                oa_handler->
                                        oa_soap_resources.fan.presence[i - 1] =
                                        RES_ABSENT;
                        }
                } else if (state == RES_PRESENT) {
                        /* The Fan is absent according Fan presence matrix,
                         * but Fan is present.  Add the Fan resource from
                         * rptcache.
                         */
                        rv = add_fan(oh_handler, con, &response);
                        if (rv != SA_OK) {
                                err("Fan <%d> add failed", i);
                                return rv;
                        } else {
                                err("Fan <%d> added", i);
                                oa_handler->
                                        oa_soap_resources.fan.presence[i - 1] =
                                        RES_PRESENT;
                        }
                }
        }
        return SA_OK;
}

/**
 * remove_fan
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the fan
 *
 * Purpose:
 *      Remove the Fan from OpenHPI infrastructure.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_fan(struct oh_handler_state *oh_handler,
                    SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        char* entity_root = NULL;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[1].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[1].EntityLocation = 0;
        entity_path.Entry[0].EntityType=SAHPI_ENT_COOLING_DEVICE;
        entity_path.Entry[0].EntityLocation= bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("Failed to get rpt for fan");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.rdrs = NULL;
        event.event.Source = event.resource.ResourceId;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;

        /* Push the hotswap event to remove the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));
        /* Remove the resource from plugin rptcache */
        rv = oh_remove_resource(oh_handler->rptcache,
                                event.resource.ResourceId);

        return rv;
}

/**
 * add_fan
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to SOAP_CON structure
 *      @info:       Pointer to the get fan info response structure
 *
 * Purpose:
 *      Add the fan information to OpenHPI infrastructure.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT add_fan(struct oh_handler_state *oh_handler,
                 SOAP_CON *con,
                 struct fanInfo *info)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        struct oh_event event;
        SaHpiInt32T bay_number;

        if (oh_handler == NULL || con == NULL || info == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        update_hotswap_event(oh_handler, &event);

        bay_number = info->bayNumber;
        /* Build the rpt entry */
        rv = build_fan_rpt(oh_handler, info->name, info->bayNumber,
                           &(event.resource));
        if (rv != SA_OK) {
                err("Failed to poplate fan RPT");
                return rv;
        }

        /* Build the RDRs */
        rv = build_fan_rdr(oh_handler, con, info, &event);
        if (rv != SA_OK) {
                err("Failed to poplate fan RDR");
                oh_remove_resource(oh_handler->rptcache,
                                   event.resource.ResourceId);
                return rv;
        }

        event.event.Source = event.resource.ResourceId;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        /* Push the hotswap event to add the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        return rv;
}

/**
 * re_discover_ps_unit
 *      @oh_handler: Pointer to openhpi handler
 *      @con: Pointer to the SOAP_CON structure
 *
 * Purpose:
 *      Re-discover the Power Supply Units.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT re_discover_ps_unit(struct oh_handler_state *oh_handler,
                             SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
        struct getPowerSupplyInfo request;
        struct powerSupplyInfo response;
        SaHpiInt32T i;
        enum resource_presence_status state;

        if (oh_handler == NULL || con == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1; i <= oa_handler->oa_soap_resources.ps_unit.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getPowerSupplyInfo(con, &request, &response);
                if (rv != SOAP_OK) {
                        err("Get power supply info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                if (response.presence != PRESENT) {
                        /* The power supply unit is absent,
                         * check power supply unit is absent in presence matrix
                         */
                        if (oa_handler->
                                oa_soap_resources.ps_unit.presence[i - 1] ==
                            RES_ABSENT)
                                continue;
                        else
                                state = RES_ABSENT;
                } else {
                        /* The power supply unit is present,
                         * check power supply unit is present in presence matrix
                         */
                        if (oa_handler->
                                oa_soap_resources.ps_unit.presence[i - 1] ==
                            RES_PRESENT)
                                continue;
                        else
                                state = RES_PRESENT;
                }

                if (state == RES_ABSENT) {
                        /* The power supply unit is present according power
                         * supply presence matrix, but power supply unit is
                         * removed.  Remove the power supply unit resource
                         * from rptcache.
                         */
                        rv = remove_ps_unit(oh_handler, i);
                        if (rv != SA_OK) {
                                err("Power Supply Unit <%d> removal failed", i);
                                return rv;
                        } else {
                                err("Power Supply Unit <%d> removed", i);
                                oa_handler->oa_soap_resources.ps_unit.
                                        presence[i - 1] = RES_ABSENT;
                        }
                } else if (state == RES_PRESENT) {
                        /* The power supply unit is absent according power
                         * supply presence matrix, but power supply unit is
                         * added.  Add the power supply unit resource from
                         * rptcache.
                         */
                        rv = add_ps_unit(oh_handler, con, &response);
                        if (rv != SA_OK) {
                                err("Power Supply Unit <%d> add failed", i);
                                return rv;
                        } else {
                                err("Power Supply Unit <%d> added", i);
                                oa_handler->oa_soap_resources.ps_unit.
                                        presence[i - 1] = RES_PRESENT;
                        }
                }
        }
        return SA_OK;
}

/**
 * remove_ps_unit
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the fan
 *
 * Purpose:
 *      Remove the Power Supply Unit.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT remove_ps_unit(struct oh_handler_state *oh_handler,
                        SaHpiInt32T bay_number)
{
        SaErrorT rv = SA_OK;
        char* entityRoot = NULL;
        SaHpiEntityPathT entity_path;
        SaHpiEntityPathT root_entity_path;
        SaHpiRptEntryT *rpt = NULL;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        update_hotswap_event(oh_handler, &event);

        entityRoot = (char *)g_hash_table_lookup(oh_handler->config,
                                                 "entity_root");
        rv = oh_encode_entitypath(entityRoot, &root_entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        entity_path.Entry[2].EntityType = SAHPI_ENT_ROOT;
        entity_path.Entry[2].EntityLocation = 0;
        entity_path.Entry[1].EntityType= SAHPI_ENT_POWER_MGMNT;
        entity_path.Entry[1].EntityLocation= 1;
        entity_path.Entry[0].EntityType= SAHPI_ENT_POWER_SUPPLY;
        entity_path.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&entity_path, &root_entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt = oh_get_resource_by_ep(oh_handler->rptcache, &entity_path);
        if (rpt == NULL) {
                err("resource rpt is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;

        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        /* Push the hotswap event to remove the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));
        /* Remove the resource from plugin rptcache */
        rv = oh_remove_resource(oh_handler->rptcache,
                                event.resource.ResourceId);

        return rv;
}

/**
 * add_ps_unit
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer SOAP_CON structure
 *      @info:       Pointer to the get power supply info response structure
 *
 * Purpose:
 *      Add the Power Supply Unit information to OpenHPI data store.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT add_ps_unit(struct oh_handler_state *oh_handler,
                     SOAP_CON *con,
                     struct powerSupplyInfo *info)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        char power_supply_disp[] = POWER_SUPPLY_NAME;
        struct oh_event event;

        if (oh_handler == NULL || con == NULL || info == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        update_hotswap_event(oh_handler, &event);

        /* Build the rpt entry */
        rv = build_power_supply_rpt(oh_handler, power_supply_disp,
                                    info->bayNumber, &(event.resource));
        if (rv != SA_OK) {
                err("build power supply rpt failed");
                return rv;
        }

        /* Build the RDRs */
        rv = build_power_supply_rdr(oh_handler, con, info, &event);
        if (rv != SA_OK) {
                err("build power supply RDR failed");
                oh_remove_resource(oh_handler->rptcache,
                                   event.resource.ResourceId);
                return rv;
        }

        event.event.Source = event.resource.ResourceId;
        event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
                SAHPI_HS_STATE_NOT_PRESENT;
        event.event.EventDataUnion.HotSwapEvent.HotSwapState =
                SAHPI_HS_STATE_ACTIVE;
        /* Push the hotswap event to add the resource from OpenHPI
         * infrastructure rptcache
         */
        oh_evt_queue_push(oh_handler->eventq, copy_oa_soap_event(&event));

        return rv;
}
