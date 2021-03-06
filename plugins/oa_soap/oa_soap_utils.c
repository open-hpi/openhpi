/*
 * Copyright (C) 2007-2009, Hewlett-Packard Development Company, LLP
 * (C) Copyright 2015-2018 Hewlett Packard Enterprise Development LP
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
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 *      Vivek Kumar <vivek.kumar2@hp.com>
 *      Shuah Khan <shuah.khan@hp.com>
 *      Mohan Devarajulu <mohan@fc.hp.com>
 *
 * This file implements all the utility functions which will be useful of oa
 * soap functioning. Majority of the functions are helper functions for
 * different modules.
 *
 *      get_oa_soap_info()              - Get active and stand by oa information
 *                                        including IP address
 *
 *      get_oa_state()                  - Get the both oa states and initializes
 *                                        the SOAP_CON objects
 *
 *      update_hotswap_event()          - Updates the Hotswap event structure
 *
 *      copy_oa_soap_event()            - Copies the event data from the event
 *                                        received from oa into the
 *                                        allocated event structure
 *
 *      push_event_to_queue()           - Pushes events into the infrastructure
 *                                        event queue
 *
 *      del_rdr_from_event()            - Delete RDRs from rdrlist of the
 *                                        event structure
 *
 *      check_oa_status()               - Check the oa status and update the oa
 *                                        handler with active OA SOAP_CON
 *
 *      check_oa_user_permissions()     - Check the OA with user permissions
 *                                        level and makes sure plug-in can
 *                                        access all the resources using the
 *                                        supplied user credentials in the
 *                                        conf file
 *
 *      check_discovery_failure()       - Checks reason for discovery failure
 *                                        If the failure is due to 'insufficient
 *                                        priveleges' then creates the fresh
 *                                        oa session
 *
 *      lock_oa_soap_handler()          - Tries to lock the oa_handler mutex.
 *                                        If mutex is already locked earlier,
 *                                        returns error
 *
 *      check_config_parameters()       - Checks whether all the parameters are
 *                                        present in the config file
 *
 *      create_event_session()          - Creates a event session with OA
 *
 *      create_oa_connection()          - Create OA connection after closing the
 *                                        earlier soap_con structures
 *
 *      initialize_oa_con()             - Initialize the hpi_con and event_con
 *
 *      delete_all_inventory_info()     - Frees the memory allocated for
 *                                        inventory areas for all the resources
 *
 *      cleanup_plugin_rptable()        - Frees the memory allocated for
 *                                        plugin RPTable
 *
 *      release_oa_soap_resources()     - Frees the memory allocated for
 *                                        resources presence matrix and serial
 *                                        number array
 *
 *      get_oa_fw_version()             - Gets the Active OA firmware version
 *
 *      update_oa_info()                - Updates the RPT entry with OA
 *                                        firmware version. Updates the serial
 *                                        number array with OA serial number.
 *
 *      convert_lower_to_upper          - Converts the lower case to upper case
 *
 *      update_oa_fw_version()          - Updates the RPT entry and IDR entry
 *                                        with OA firmware version
 *
 *      oa_soap_check_serial_number()   - Check the serial_number and
 *                                        give a proper message
 *
 *      oa_soap_sleep_in_loop()   	- Sleep in 3 second intervals so that
 *                                        thread could catch the signal and exit
 *
 **/

#include "oa_soap_utils.h"
#include "sahpi_wrappers.h"

/**
 * get_oa_soap_info
 *      @oh_handler: Pointer to the openhpi handler
 *
 * Purpose:
 *      Gets the Active/standby OA hostname/IP address from the config file
 *      and calls the get_oa_info fucntion
 *
 * Detailed Description:
 *      - Will take the active oa ip from configuration file and will check the
 *        oa state. If active oa is not accessible, we would try the standby oa
 *      - Same above operation will be repeated for the standby oa. If standby
 *        oa is also not accessible, then return error
 *      - Checks whether OA hostname/IP address is not empty string
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - if wrong parameters passed
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT get_oa_soap_info(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T len = 0;
        char *server = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the Active OA hostname/IP address and check whether it is NULL */
        server = (char *) g_hash_table_lookup(oh_handler->config, "ACTIVE_OA");
        /* Check whether server string is not empty */
        len = strlen(server);
        if (len != 0) {
                /* Get the OA states and initialize the SOAP_CON structures */
                rv = get_oa_state(oh_handler, server);
                if (rv == SA_OK)
                        return SA_OK;
        } else {
                err("ACTIVE_OA is not provided by the user");
        }

        /* May be user has supplied wrong hostname/IP address
         * or OA is not reachable. Ignore and try with standby OA
         * Get the Standby OA hostname/IP address and check whether it is NULL
         */
        server = (char *) g_hash_table_lookup(oh_handler->config, "STANDBY_OA");
        if (server == NULL) {
                err("STANDBY_OA entry is not present in conf file");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check whether server string is not empty */
        len = strlen(server);
        if (len == 0) {
                err("STANDBY_OA is not provided by the user");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the OA states and initialize the SOAP_CON structures */
        rv = get_oa_state(oh_handler, server);
        if (rv != SA_OK) {
                err("Standby OA - %s may not be accessible", server);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * get_oa_state
 *      @oa_handler: Pointer to OA SOAP handler structure
 *      @server:     Pointer to hostname/IP address of the OA
 *
 * Purpose:
 *      Gets the details of active and standby OA
 *
 * Detailed Description:
 *      - Initializes the SOAP_CON structures
 *      - Using the supplied user credentials from config file, soap connection
 *        is made using soap_open api for both hpi and events receiving
 *      - Will check the user permission on OA using check_user_permission api
 *      - Will get the oa information. Using these info, this api will decide
 *        which oa is active and which one is standby
 *
 * Return values:
 *      SA_OK  - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT get_oa_state(struct oh_handler_state *oh_handler,
                      char *server)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        struct getOaStatus status;
        struct oaStatus status_response;
        struct getOaInfo info;
        struct oaInfo info_response;
        struct getOaNetworkInfo network_info;
        struct oaNetworkInfo network_info_response;
        enum oaRole oa_role;
        char active_ip[MAX_URL_LEN], standby_ip[MAX_URL_LEN], *url = NULL;  
        char active_fm[MAX_BUF_SIZE], standby_fm[MAX_BUF_SIZE];
        char firmware[MAX_BUF_SIZE];
        char *user_name = NULL, *password = NULL;
        SaHpiInt32T i, bay = 0, active_bay = 0, standby_bay = 0;
        SOAP_CON *hpi_con = NULL, *event_con = NULL;
        struct oa_info *this_oa = NULL, *other_oa = NULL;
        char *endptr = NULL;
        struct extraDataInfo extra_data_info;
        xmlNode *extra_data = NULL;

        if (oh_handler == NULL|| server == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

	if (!strcmp(server,"0.0.0.0")) {
                err("Invalid OA IP 0.0.0.0");
                return SA_ERR_HPI_INVALID_PARAMS;
	}

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        /* Create the OA URL */
        rv = asprintf(&url, "%s" PORT, server);			   
        if(rv == -1){
                wrap_free(url);
                err("Failed to allocate memory for buffer to        \
                                           hold OA credentials");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Get the user_name and password from config file */
        user_name = (char *) g_hash_table_lookup(oh_handler->config,
                                                 "OA_User_Name");
        password = (char *) g_hash_table_lookup(oh_handler->config,
                                                "OA_Password");

        /* Estabish the connection with OA */
        hpi_con = soap_open(url, user_name, password, HPI_CALL_TIMEOUT);
        if (hpi_con == NULL) {
                wrap_free(url);
                err("hpi_con intialization for OA - %s has failed", server);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        event_con = soap_open(url, user_name, password, EVENT_CALL_TIMEOUT);
        if (event_con == NULL) {
                wrap_free(url);
                err("event_con intialization for OA - %s has failed", server);
                soap_close(hpi_con);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        wrap_free(url);
        url = NULL;
        /* Check whether user_name has admin rights */
        rv = check_oa_user_permissions(oa_handler, hpi_con, user_name);
        if (rv != SA_OK) {
                soap_close(hpi_con);
                soap_close(event_con);
                return rv;
        }

        /* Get the 2 OAs information */
        for (i = 1; i <= MAX_OA_BAYS; i++) {
                status.bayNumber = i;
                rv = soap_getOaStatus(hpi_con, &status, &status_response);
                if (rv != SOAP_OK) {
                        err("Get OA status failed");
                        soap_close(hpi_con);
                        soap_close(event_con);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                oa_role = status_response.oaRole;
                /* Sometimes, if the OA is absent, then OA status is shown as
                 * STANDBY in getOaStatus response.  As workaround, if OA
                 * status is STANDBY and oaRedudancy state is set to false,
                 * then, it is considered as ABSENT.
                 *
                 * But, if the OA is recently inserted, then oaRedudancy state
                 * will be set to false.  In this scenario, the OA state will
                 * be wrongly considered as ABSENT.  This is a known limitation.
                 * TODO: Remove this workaround once the fix is available in
                 * OA firmware.
                 */
                if ((oa_role == OA_ABSENT) ||
                    (oa_role == STANDBY &&
                     status_response.oaRedundancy == HPOA_FALSE))
                        continue;

                info.bayNumber = i;
                rv = soap_getOaInfo(hpi_con, &info, &info_response);
                if (rv != SOAP_OK) {
                        err("Get OA info failed");
                        soap_close(hpi_con);
                        soap_close(event_con);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                /* 'youAreHere' true indicates that we are talking to this OA.
                 * This helps to find the bay number of the active and standby
                 * OA.
                 */
                if (info_response.youAreHere == HPOA_TRUE) {
                        bay = i;
                       /* Find the oa_info structure for this OA (to which we
                        * are talking to) and the other OA.
                        */
                        switch (bay) {
                                case 1:
                                        this_oa = oa_handler->oa_1;
                                        other_oa = oa_handler->oa_2;
                                        break;
                                case 2:
                                        this_oa = oa_handler->oa_2;
                                        other_oa = oa_handler->oa_1;
                                        break;
                        }
                }

                /* Store the firmware version.
                 * Firmware version is not used.
                 * We may require when we support multiple OA firmwares.
                 */
                memset(firmware, 0, MAX_BUF_SIZE);
                strncpy(firmware, info_response.fwVersion,
                        strlen(info_response.fwVersion));

                network_info.bayNumber = i;
                rv = soap_getOaNetworkInfo(hpi_con, &network_info,
                                           &network_info_response);
                if (rv != SOAP_OK) {
                        err("Get OA network info failed");
                        soap_close(hpi_con);
                        soap_close(event_con);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                extra_data = network_info_response.extraData;
                while (extra_data) {
                        soap_getExtraData(extra_data, &extra_data_info);
                        if ((!(strcmp(extra_data_info.name, "IpSwap"))) &&
                                    (extra_data_info.value != NULL)) {
                                    if(!(strcasecmp(extra_data_info.value,
                                                  "true"))) {
                                             oa_handler->ipswap = HPOA_TRUE;
                                    } else {
                                             oa_handler->ipswap = HPOA_FALSE;
                                    }
                                    break;
                        }
                        extra_data = soap_next_node(extra_data);
                }


                /* Find the active and standby bay number, IP address
                 * and firmware version */
                switch (oa_role) {
                        case ACTIVE:
                                active_bay = i;
                                memset(active_ip, 0, MAX_URL_LEN);
                                rv = oa_soap_get_oa_ip(server,
                                                       network_info_response,
                                                       active_ip);
                                if (rv != SOAP_OK) {
                                        err("Get Active OA IP failed");
                                        soap_close(hpi_con);
                                        soap_close(event_con);
                                        return SA_ERR_HPI_INTERNAL_ERROR;
                                }

                                memset(active_fm, 0, MAX_BUF_SIZE);
                                strncpy(active_fm, firmware, strlen(firmware));
                                /* Check that string to float conversion */
                                /* proceeds without error.               */
                                errno = 0;
                                oa_handler->active_fm_ver =
                                  strtod(firmware, &endptr);
                                if ((errno != 0) || (firmware == endptr)) {
                                  /* Float conversion did not work */
                                  err("could not convert OA firmware version \
%s to a floating point value\n", firmware);
                                }
                                break;
                        case STANDBY:
                                standby_bay = i;
                                memset(standby_ip, 0, MAX_URL_LEN);
                                rv = oa_soap_get_oa_ip(server,
                                                       network_info_response,
                                                       standby_ip);
                                if (rv != SOAP_OK) {
                                        err("Get Standby OA IP failed");
                                        soap_close(hpi_con);
                                        soap_close(event_con);
                                        return SA_ERR_HPI_INTERNAL_ERROR;
                                }

                                memset(standby_fm, 0, MAX_BUF_SIZE);
                                strncpy(standby_fm, firmware, strlen(firmware));
                                break;
                        default:
                                err("wrong oa state detected for bay %d", i);
                                soap_close(hpi_con);
                                soap_close(event_con);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }

        if (active_bay != 0) {
                dbg("\tActive OA bay number - %d", active_bay);
                dbg("\tActive OA ip address - %s", active_ip);
                dbg("\tActive OA firmware version - %s", active_fm);
        }
        if (standby_bay != 0) {
                dbg("\tStandby OA bay number - %d", standby_bay);
                dbg("\tStandby OA ip address - %s", standby_ip);
                dbg("\tStandby OA firmware version - %s", standby_fm);
        }

        /* Get the status and firmware version of the OA which we are
         * talking to
         */
        if (bay == active_bay) {
                this_oa->oa_status = ACTIVE;
                this_oa->fm_version = atof(active_fm);
        } else { /* bay == standby_bay */
                this_oa->oa_status = STANDBY;
                this_oa->fm_version = atof(standby_fm);
        }

        /* Initialize the hpi_con and event_con structures */
        this_oa->hpi_con = hpi_con;
        this_oa->event_con = event_con;
        memset(this_oa->server, 0, MAX_URL_LEN);
        strncpy(this_oa->server, server, strlen(server));

        /* Check whether 2 OAs are present or not */
        if (active_bay == 0 || standby_bay == 0)
                return SA_OK;

        memset(other_oa->server, 0, MAX_URL_LEN);
        /* Construct the other OA url and copy the IP address to oa_info
         * structure
         */
        if (bay == standby_bay) {
                other_oa->oa_status = ACTIVE;
                other_oa->fm_version = atof(active_fm);
                strncpy(other_oa->server, active_ip, strlen(active_ip));
                rv = asprintf(&url, "%s" PORT, active_ip);		      
                if (rv == -1){
                        wrap_free(url);
                        err("Failed to allocate memory for buffer to        \
                                                 hold OA credentials");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }

        } else {
                other_oa->oa_status = STANDBY;
                other_oa->fm_version = atof(standby_fm);
                strncpy(other_oa->server, standby_ip, strlen(standby_ip));
                rv = asprintf(&url, "%s" PORT, standby_ip);		      
                if(rv == -1){
                       wrap_free(url);
                       err("Failed to allocate memory for buffer to        \
                                                 hold OA credentials");

                       return SA_ERR_HPI_OUT_OF_MEMORY;
                }

        }

        /* Initialize the soap_con for hpi and event thread */
        other_oa->hpi_con = soap_open(url, user_name,
                                      password, HPI_CALL_TIMEOUT);
        if (other_oa->hpi_con == NULL) {
                err("Initializing the hpi_con for OA %s failed", url);
                wrap_free(url);
                /* If this OA status is ACTIVE, then return error, else ignore
                 * If standby OA is not accessible, then the recovery from
                 * this problem will be done by the event thread.
                 * Since we have access to Active OA, we should ignore this
                 * for time being
                 */
                if (other_oa->oa_status == ACTIVE) {
                        soap_close(this_oa->hpi_con);
                        soap_close(this_oa->event_con);
                        this_oa->hpi_con = NULL;
                        this_oa->event_con = NULL;
                        err("Active OA - %s may not be accessible",
                            other_oa->server);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                } else
                        return SA_OK;
            }

        other_oa->event_con = soap_open(url, user_name,
                                        password, EVENT_CALL_TIMEOUT);
        if (other_oa->event_con == NULL) {
                err("Initializing the event_con for OA %s failed", url);
                wrap_free(url);
                /* If this OA status is ACTIVE, then return error, else
                 * ignore
                 */
                if (other_oa->oa_status != ACTIVE) {
                        soap_close(this_oa->hpi_con);
                        soap_close(this_oa->event_con);
                        this_oa->hpi_con = NULL;
                        this_oa->event_con = NULL;
                        soap_close(other_oa->hpi_con);
                        other_oa->hpi_con = NULL;
                        err("Active OA - %s may not be accessible",
                            other_oa->server);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                } else {
                        soap_close(other_oa->hpi_con);
                        other_oa->hpi_con = NULL;
                        return SA_OK;
                }
        }
        wrap_free(url);
        return SA_OK;
}

/**
 * update_hotswap_event
 *      @event:      Pointer to openhpi event structure
 *      @oh_handler: Pointer to the openhpi handler
 *
 * Purpose:
 *      this api updates the event structure with hotswap details
 *
 * Detailed Description: NA
 *
 * Return values:
 *      NONE
 **/

void update_hotswap_event(struct oh_handler_state *oh_handler,
                          struct oh_event *event)
{
        if (oh_handler == NULL || event == NULL) {
                err("Invalid parameters");
                return;
        }

        memset(event, 0, sizeof(struct oh_event));
        event->hid = oh_handler->hid;
        event->event.EventType = SAHPI_ET_HOTSWAP;
        /* TODO: map the timestamp of the OA generated event */
        oh_gettimeofday(&(event->event.Timestamp));
        event->event.Severity = SAHPI_CRITICAL;
}

/**
 * copy_oa_soap_event
 *      @event: Pointer to the openhpi event structure
 *
 * Purpose:
 *      makes a copy of the received event
 *
 * Detailed Description:
 *      - Allocates the memory to new event structure.
 *      - Copies the hpi event to the newly created event structure
 *      - Returns the newly created event structure
 *
 * Return values:
 *      Pointer to copied oh_event structure - on success
 *      NULL                                 - if wrong parameters passed
 *                                           - if the memory allocation failed.
 **/

struct oh_event *copy_oa_soap_event(struct oh_event *event)
{
        struct oh_event *e;

        if (event == NULL) {
                err("Invalid parameter");
                return NULL;
        }

        e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
        if (e == NULL) {
                err("Out of memory!");
                return NULL;
        }
        memcpy(e, event, sizeof(struct oh_event));
        return e;
}

/**
 * del_rdr_from_event
 *      @event: Pointer to the openhpi event structure
 *
 * Purpose:
 *      deletes the RDR from the event.
 *
 * Detailed Description:
 *      - traverse the event.rdrs list and delete the RDRs one by one.
 *      - This function will be called if the discovery fails for a resource
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - if wrong parameters passed
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT del_rdr_from_event(struct oh_event *event)
{
        GSList *node = NULL;
        SaHpiRdrT *rdr = NULL;

        if (event == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (event->rdrs == NULL)
                return SA_OK;

        /* Traverse the RDRs list and delete the RDR */
        node = event->rdrs;
        do {
                rdr = NULL;
                rdr = (SaHpiRdrT *)node->data;
                if (rdr == NULL) {
                        err("Wrong node detected in the GSList");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                /* Pop out the RDR from the RDRs list  */
                event->rdrs = g_slist_remove(event->rdrs, (gpointer)rdr);
                wrap_g_free(rdr);
                /* Get the next RDR */
                node = event->rdrs;
        } while (node != NULL);

        return SA_OK;
}

/**
 * check_oa_status
 *      @oa_handler: Pointer to the OA SOAP plug-in handler
 *      @oa:         Pointer to the OA data structure
 *      @con:        Pointer to the SOAP_CON structure
 *
 * Purpose:
 *      checks the status of the oa
 *
 * Detailed Description:
 *      - gets the status of the OA
 *      - Updates the OA handler with active OA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - if wrong parameters passed
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT check_oa_status(struct oa_soap_handler *oa_handler,
                         struct oa_info *oa,
                         SOAP_CON *con)
{
        SaErrorT rv = SA_OK;
        struct getOaStatus status;
        struct oaStatus status_response;

        if (oa_handler == NULL || oa == NULL || con == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the bay number of the OA */
        if (oa == oa_handler->oa_1)
                status.bayNumber = 1;
        else
                status.bayNumber = 2;

        wrap_g_mutex_lock(oa->mutex);
        rv = soap_getOaStatus(con, &status, &status_response);
        if (rv != SOAP_OK) {
                err("Get OA status call failed for OA IP %s ", oa->server);
                wrap_g_mutex_unlock(oa->mutex);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        if (status_response.oaRole == TRANSITION) {
                /* OA is in transitioning state to become active.
                 * This is a very rare scenario.
                 *
                 * Wait till it it becomes Active
                 */
                err("OA is in transition state");
                oa_soap_sleep_in_loop(oa_handler, OA_STABILIZE_MAX_TIME);
                rv = soap_getOaStatus(con, &status, &status_response);
                if (rv != SOAP_OK) {
                        err("Get OA status call failed for OA IP %s ", oa->server);
                        wrap_g_mutex_unlock(oa->mutex);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                /* Check OA is still in TRANSITION state
                 * Ideally, OA should be out of TRANSITION state
                 */
                if (status_response.oaRole == TRANSITION) {
                        err("OA is in TRANSITION for a long time");
                        err("Please correct the OA");
                        wrap_g_mutex_unlock(oa->mutex);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                 }
        }

       /* If Enclosure IP mode is enabled then ipswap becomes true, 
          then Active OA IP is always same. So do not change the Role */

        if(!oa_handler->ipswap) {
                oa->oa_status = status_response.oaRole;
        }

        if (oa->oa_status == ACTIVE) {
                wrap_g_mutex_unlock(oa->mutex);
                /* Always lock the oa_handler mutex and then oa_info mutex
                 * This is to avoid the deadlock
                 */
                wrap_g_mutex_lock(oa_handler->mutex);
                wrap_g_mutex_lock(oa->mutex);
                /* Point the active_con to Active OA's hpi_con */
                if (oa_handler->active_con != oa->hpi_con) {
                        oa_handler->active_con = oa->hpi_con;
                        err("OA %s has become Active", oa->server);
                }
                wrap_g_mutex_unlock(oa->mutex);
                wrap_g_mutex_unlock(oa_handler->mutex);
        } else
                wrap_g_mutex_unlock(oa->mutex);

        return SA_OK;
}

/**
 * check_oa_user_permissions
 *      @oa_handler: Pointer to the oa_handler structure
 *      @con:        Pointer to the SOAP CON
 *      @user_name:  Pointer to the user name string
 *
 * Purpose:
 *      check oa user permissions, even for the oa administrator.
 *
 * Detailed Description:
 *      - checks whether OA user has ADMINISTRATOR or OPERATOR
 *        permission and can access all the resources in HP 
 *        BladeSystem c-Class
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - if wrong parameters passed
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT check_oa_user_permissions(struct oa_soap_handler *oa_handler,
                                   SOAP_CON *con,
                                   char *user_name)
{
        SaErrorT rv = SA_OK;
        struct getUserInfo request;
        struct userInfo response;
        struct bayAccess bay_access;

        if (oa_handler == NULL || con == NULL || user_name == NULL) {
                err("Invalid Parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        memset(&request, 0, sizeof(struct getUserInfo));
        request.username = user_name;

        rv = soap_getUserInfo(con, &request, &response);
        if (rv != SOAP_OK) {
                err("Get user info call failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check user name is enabled or not
         * This should ideally never fail.
         * We can not have a user name which is disabled and session is created
         */
        if (response.isEnabled != HPOA_TRUE) {
                err("User - %s is not enabled for OA %s",
                    user_name, con->server);
                err("Please give full permissions to user - %s", user_name);
                oa_handler->status = PLUGIN_NOT_INITIALIZED;
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for the ADMINISTRATOR or OPERATOR rights */
        if ((response.acl != ADMINISTRATOR) && (response.acl != OPERATOR))  {
                err("User - %s is not Administrator or Operator on OA %s", 
                     user_name, con->server);
                err("Please give full permissions to user - %s", user_name);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check the permissions for accessing OA */
        if (response.bayPermissions.oaAccess != HPOA_TRUE) {
                err("User - %s does not have access rights to OA bay(s) "
                    "for OA %s", user_name, con->server);
                err("Please give full permissions to user - %s", user_name);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check the permissions for accessing server lades */
        while (response.bayPermissions.bladeBays) {
                soap_getBayAccess(response.bayPermissions.bladeBays,
                                  &bay_access);
                if (bay_access.access != HPOA_TRUE) {
                        err("User - %s does not have access rights to "
                            "server bay(s) for OA - %s",
                            user_name, con->server);
                        err("Please give full permissions to user - %s",
                            user_name);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                response.bayPermissions.bladeBays =
                          soap_next_node(response.bayPermissions.bladeBays);
        }

        /* Check the permissions for accessing interconnect */
        while (response.bayPermissions.interconnectTrayBays) {
                soap_getBayAccess(response.bayPermissions.interconnectTrayBays,
                                   &bay_access);
                if (bay_access.access != HPOA_TRUE) {
                        err("User - %s does not have access rights to "
                            "interconnect bay(s) for OA %s",
                            user_name, con->server);
                        err("Please give full permissions to user - %s",
                            user_name);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                response.bayPermissions.interconnectTrayBays =
                        soap_next_node(response.bayPermissions.
                                               interconnectTrayBays);
        }

        return SA_OK;
}

/**
 * check_discovery_failure
 *      @oh_handler: Pointer to the openhpi handler
 *
 * Purpose:
 *      checks the oa status after the discovery failure.
 *
 * Detailed Description:
 *      - this api will be called only during the discovery failure.
 *      - checks the reason for discovery failure.
 *      - if the failure is due to 'insufficient priveleges',
 *        then creates the fresh oa session
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - if wrong parameters passed
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT check_discovery_failure(struct oh_handler_state *oh_handler)
{
        SaErrorT oa1_rv, oa2_rv, rv;
        struct oa_soap_handler *oa_handler = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        /* Initialize the return values with failure case */
        oa1_rv = SA_ERR_HPI_INTERNAL_ERROR;
        oa2_rv = SA_ERR_HPI_INTERNAL_ERROR;

        /* If the hpi_con is NULL, then OA is not reachable */
        rv = get_oa_soap_info(oh_handler);
       	if (rv != SA_OK) {
                oa_handler->status = PLUGIN_NOT_INITIALIZED;
                err("Get OA SOAP info failed");
                return rv;
        }
        if (oa_handler->oa_1->hpi_con != NULL) {
                /* Get the status of the OA in slot 1 */
                oa1_rv = check_oa_status(oa_handler, oa_handler->oa_1,
                                         oa_handler->oa_1->hpi_con);
                if (oa1_rv != SA_OK)
                        err("check oa_status has failed for - %s",
                             oa_handler->oa_1->server);
        }

        if (oa_handler->oa_2->hpi_con != NULL) {
                /* Get the status of the OA in slot 2 */
                oa2_rv = check_oa_status(oa_handler, oa_handler->oa_2,
                                         oa_handler->oa_2->hpi_con);
                if (oa2_rv != SA_OK)
                        err("check oa_status has failed for OA - %s",
                             oa_handler->oa_2->server);
        }

        /* If the OA is reachable (check_oa_status call succeeded)
         * and OA STATUS is ACTIVE, then return sucess, else return failure.
         */
        if (oa1_rv == SA_OK && oa_handler->oa_1->oa_status == ACTIVE)
                return SA_OK;
        else if (oa2_rv == SA_OK && oa_handler->oa_2->oa_status == ACTIVE)
                return SA_OK;
        else
                return SA_ERR_HPI_INTERNAL_ERROR;
}

/**
 * lock_oa_soap_handler
 *      @oa_handler: Pointer to the oa_handler
 *
 * Purpose:
 *      Tries to lock the oa_handler mutex. If mutex is already locked earlier,
 *      returns error
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT lock_oa_soap_handler(struct oa_soap_handler *oa_handler)
{
        gboolean lock_state = TRUE;

        if (oa_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Try to lock the oa_handler mutex */
        lock_state = wrap_g_mutex_trylock(oa_handler->mutex);
        if (lock_state == FALSE) {
                err("OA SOAP Handler is locked.");
                err("No operation is allowed in this state");
                err("Please try after some time");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Unlock the oa_handler mutex */
        wrap_g_mutex_unlock(oa_handler->mutex);
        return SA_OK;
}

/**
 * check_config_parameters
 *      @handler_config: Pointer to the config handler
 *
 * Purpose:
 *      Checks whether all the parameters are present in the config file
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/

SaErrorT check_config_parameters(GHashTable *handler_config)
{
        char *temp = NULL;

        if (handler_config == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check for entity_root entry */
        temp = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if (temp == NULL) {
                err("entity_root is missing in the config file.");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for OA user name entry */
        temp = (char *) g_hash_table_lookup(handler_config, "OA_User_Name");
        if (temp == NULL) {
                err("Failed to find attribute OA_User_Name in openhpi.conf ");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for OA_Password entry */
        temp = (char *) g_hash_table_lookup(handler_config, "OA_Password");
        if (temp == NULL) {
                err("Failed to find attribute OA_Password in openhpi.conf ");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for Active OA hostname/IP address entry
         * STANDBY_OA is an optional parameter and hence not checked
         */
        temp = (char *) g_hash_table_lookup(handler_config, "ACTIVE_OA");
        if (temp == NULL) {
                err("Failed to find attribute ACTIVE_OA in openhpi.conf ");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * create_event_session
 *      @oa: Pointer to the oa info structure
 *
 * Purpose:
 *      creates the fresh event session with OA
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on unsupported oa events.
 **/
SaErrorT create_event_session(struct oa_info *oa)
{
        SaErrorT rv = SOAP_OK;
        struct eventPid pid;

        if (oa == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_g_mutex_lock(oa->mutex);
        if (oa->event_con == NULL) {
                dbg("OA may not be accessible");
                wrap_g_mutex_unlock(oa->mutex);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = soap_subscribeForEvents(oa->event_con, &pid);
        wrap_g_mutex_unlock(oa->mutex);
        if (rv != SOAP_OK) {
                err("Subscribe for events failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Store the event pid in oa_info structure */
        oa->event_pid=pid.pid;

        return SA_OK;
}

/**
 * create_oa_connection
 *      @oa:        Pointer to the oa info structure
 *      @user_name: Pointer the to the OA user name
 *      @password:  Pointer the to the OA password
 *
 * Purpose:
 *      Creates the connection with the OA
 *      This function will not return until the connection is established
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      NONE
 **/
void create_oa_connection(struct oa_soap_handler *oa_handler,
                          struct oa_info *oa,
                          char *user_name,
                          char *password)
{
        SaErrorT rv = SA_OK;
        SaHpiBoolT is_oa_present = SAHPI_FALSE;
        SaHpiBoolT is_oa_accessible = SAHPI_FALSE;

        if (oa == NULL || user_name == NULL || password == NULL) {
                err("Invalid parameters");
                return;
        }

        while (is_oa_accessible == SAHPI_FALSE) {
                OA_SOAP_CHEK_SHUTDOWN_REQ(oa_handler, NULL, NULL, NULL);
                /* Check whether the OA is present.
                 * If not, wait till the OA is inserted
                 */
                 is_oa_present = SAHPI_FALSE;
                 while (is_oa_present == SAHPI_FALSE) {
                	OA_SOAP_CHEK_SHUTDOWN_REQ(oa_handler, NULL, NULL, NULL);
                        wrap_g_mutex_lock(oa->mutex);
                        if (oa->oa_status != OA_ABSENT) {
                                wrap_g_mutex_unlock(oa->mutex);
                                is_oa_present = SAHPI_TRUE;
                        } else {
                                wrap_g_mutex_unlock(oa->mutex);
                                /* OA is not present,
                                 * wait for 30 seconds and check again
                                 */
                                oa_soap_sleep_in_loop(oa_handler, 30);
                        }
                }

                wrap_g_mutex_lock(oa->mutex);
                /* Close the soap_con strctures */
                if (oa->hpi_con != NULL) {
                        soap_close(oa->hpi_con);
                        oa->hpi_con = NULL;
                }
                if (oa->event_con != NULL) {
                        soap_close(oa->event_con);
                        oa->event_con = NULL;
                }
                wrap_g_mutex_unlock(oa->mutex);

                rv = initialize_oa_con(oa, user_name, password);
                if ((rv != SA_OK) && (oa->oa_status != OA_ABSENT)) {
                        /* OA may not be reachable
                         * wait for 2 seconds and check again
                         */
                        sleep(2);
                        continue;
                }

                /* hpi_con and event_con successfully created */
                is_oa_accessible = SAHPI_TRUE;
        }

        return;
}

/**
 * initialize_oa_con
 *      @oa:        Pointer to the oa info structure
 *      @user_name: Pointer the to the OA user name
 *      @password:  Pointer the to the OA password
 *
 * Purpose:
 *      Initializes the hpi_con and event_con
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      NONE
 **/
SaErrorT initialize_oa_con(struct oa_info *oa,
                           char *user_name,
                           char *password)
{       
        SaErrorT rv = SA_OK; 
        char *url = NULL;    		    
        
        if (oa == NULL || user_name == NULL || password == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

	if (!strcmp(oa->server,"0.0.0.0")) {
                err("Invalid OA IP  0.0.0.0");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        wrap_g_mutex_lock(oa->mutex);
        rv = asprintf(&url, "%s" PORT, oa->server);			
        if(rv == -1){
                wrap_free(url);
                err("Failed to allocate memory for buffer to        \
                                             hold OA credentials");
                wrap_g_mutex_unlock(oa->mutex);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }


        oa->hpi_con = soap_open(url, user_name, password,
                                HPI_CALL_TIMEOUT);
        if (oa->hpi_con == NULL) {
                wrap_free(url);
                /* OA may not be reachable */
                wrap_g_mutex_unlock(oa->mutex);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Try to create event_con connection
         * Ideally, this call should not fail
         */
        oa->event_con = soap_open(url, user_name, password,
                                  EVENT_CALL_TIMEOUT);
        if (oa->event_con == NULL) {
                wrap_free(url);
                /* OA may not be reachable */
                wrap_g_mutex_unlock(oa->mutex);
                soap_close(oa->hpi_con);
                oa->hpi_con = NULL;
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        wrap_g_mutex_unlock(oa->mutex);
        wrap_free(url);
        return SA_OK;

}

/**
 * delete_all_inventory_info
 *      @oh_handler: Pointer to the plugin handler
 *
 * Purpose:
 *      Traverses through all resources and extracts the inventory RDR
 *      Frees up the memory allocated for inventory information
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT delete_all_inventory_info(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        SaHpiRptEntryT  *rpt = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_next(oh_handler->rptcache, SAHPI_FIRST_ENTRY);
        while (rpt) {
                if (rpt->ResourceCapabilities 
                    & SAHPI_CAPABILITY_INVENTORY_DATA) {
                        /* Free the inventory info from inventory RDR */
                        rv = free_inventory_info(oh_handler, rpt->ResourceId);
                        if (rv != SA_OK) 
                                err("Inventory cleanup failed for resource %d",
                                    rpt->ResourceId);
                }
                /* Get the next resource */
                rpt = oh_get_resource_next(oh_handler->rptcache,
                                           rpt->ResourceId);
        }

        return SA_OK;
}

/**
 * cleanup_plugin_rptable
 *      @oh_handler: Pointer to the plugin handler
 *
 * Purpose:
 *      Frees up the memory allocated for RPT and RDR entries
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      NONE
 **/
void cleanup_plugin_rptable(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        if(oa_handler == NULL) {
                err("Invalid parameter");
                return;
        }

        rv = delete_all_inventory_info(oh_handler);
        if (rv != SA_OK) {
                err("Deleting all inventory information failed");
        }

        release_oa_soap_resources(oa_handler);

        rv = oh_flush_rpt(oh_handler->rptcache);
        if (rv != SA_OK) {
                err("Plugin RPTable flush failed");
        }

        return;
}

/**
 * release_oa_soap_resources
 *      @oa_handler: Pointer to oa soap handler
 *
 * Purpose:
 *      To free the memory allocated for resource presence and serial number
 *      for OA, interconnect, server, fan and power supply
 *
 * Detailed Description: NA
 *
 * Return values:
 *      None
 **/
void release_oa_soap_resources(struct oa_soap_handler *oa_handler)
{
        SaHpiInt32T i;

        /* Release memory of blade presence, resource id and blade
         * serial number arrays
         */
        wrap_g_free(oa_handler->oa_soap_resources.server.presence);
        wrap_g_free(oa_handler->oa_soap_resources.server.resource_id);

	if(oa_handler->oa_soap_resources.server.serial_number != NULL) {
	    for (i = 0; i < oa_handler->oa_soap_resources.server.max_bays; i++)
	    {
                wrap_g_free(oa_handler->oa_soap_resources.server.serial_number[i]);
            }
            wrap_g_free(oa_handler->oa_soap_resources.server.serial_number);
	}

        /* Release memory of interconnect presence and serial number array */
        wrap_g_free(oa_handler->oa_soap_resources.interconnect.presence);
        wrap_g_free(oa_handler->oa_soap_resources.interconnect.resource_id);

	if(oa_handler->oa_soap_resources.interconnect.serial_number != NULL) {
            for (i = 0; i < oa_handler->oa_soap_resources.interconnect.max_bays;
             i++) {
                      wrap_g_free(oa_handler->oa_soap_resources.interconnect.
                          serial_number[i]);
            }
            wrap_g_free(oa_handler->oa_soap_resources.interconnect.serial_number);
	}

        /* Release memory of OA presence and serial number array */
        wrap_g_free(oa_handler->oa_soap_resources.oa.presence);
        wrap_g_free(oa_handler->oa_soap_resources.oa.resource_id);

	if(oa_handler->oa_soap_resources.oa.serial_number != NULL) {
            for (i = 0; i < oa_handler->oa_soap_resources.oa.max_bays; i++) {
                wrap_g_free(oa_handler->oa_soap_resources.oa.
                    serial_number[i]);
            }
            wrap_g_free(oa_handler->oa_soap_resources.oa.serial_number);
	}

        /* Release memory of fan presence.  Since fans do not have serial
         * numbers, a serial numbers array does not need to be released.
         */
        wrap_g_free(oa_handler->oa_soap_resources.fan.presence);
        wrap_g_free(oa_handler->oa_soap_resources.fan.resource_id);

        /* Release memory of fan zone resource id */
        wrap_g_free(oa_handler->oa_soap_resources.fan_zone.resource_id);

        /* Release memory of power supply presence and serial number array */
        wrap_g_free(oa_handler->oa_soap_resources.ps_unit.presence);
        wrap_g_free(oa_handler->oa_soap_resources.ps_unit.resource_id);

	if(oa_handler->oa_soap_resources.ps_unit.serial_number != NULL) {
            for (i = 0; i < oa_handler->oa_soap_resources.ps_unit.max_bays; i++)
	    {
                wrap_g_free(oa_handler->oa_soap_resources.
                            ps_unit.serial_number[i]);
            }
            wrap_g_free(oa_handler->oa_soap_resources.ps_unit.serial_number);
	}
}

/**
 * get_oa_fw_version
 *      @oh_handler: Pointer to the plugin handler
 *
 * Purpose:
 *      Returns the OA firmware version of the active OA
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      Active OA firmware version - on success
 *      0.0                        - on failure
 **/
SaHpiFloat64T get_oa_fw_version(struct oh_handler_state *oh_handler)
{
        struct oa_soap_handler *oa_handler;

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return 0.0;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        if (oa_handler->active_con == oa_handler->oa_1->hpi_con)
                return oa_handler->oa_1->fm_version;
        else if (oa_handler->active_con == oa_handler->oa_2->hpi_con)
                return oa_handler->oa_2->fm_version;
        else
                return 0.0;
}

/**
 * update_oa_info
 *      @oh_handler:  Pointer to the plugin handler
 *      @response:    Pointer to the oaInfo structure
 *      @resource_id: Resource Id
 *
 * Purpose:
 *      Returns the OA firmware version of the active OA
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_HPI_ERR_INAVLID_PARAMS - on invalid parametersfailure
 *      SA_HPI_ERR_INTERNAL_ERROR - on failure
 *      SA_OK                     - on success
 **/
SaErrorT update_oa_info(struct oh_handler_state *oh_handler,
                        struct oaInfo *response,
                        SaHpiResourceIdT resource_id)
{
        SaHpiRptEntryT *rpt = NULL;
        SaHpiFloat64T fm_version;
        SaHpiInt32T major;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("OA rpt is not present");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        if (strlen(response->fwVersion) == 0) {
                err("Firmware version is null string");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Firmware version is in the format of x.yy. 'x' is the major version
         * 'yy' is the minor version
         */
        fm_version = atof(response->fwVersion);
        rpt->ResourceInfo.FirmwareMajorRev = major = 
				(SaHpiUint8T)floor(fm_version);
        rpt->ResourceInfo.FirmwareMinorRev = rintf((fm_version - major) * 100);

        return SA_OK;
}

/**
 * convert_lower_to_upper
 *      @src:      Pointer to the source string handler
 *      @src_len:  String length of the source string
 *      @dest:     Pointer to destination string
 *      @dest_len: Length of the destination string
 *
 * Purpose:
 *      Converts the lower case characters to upper case
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_HPI_ERR_INAVLID_PARAMS - on invalid parametersfailure
 *      SA_HPI_ERR_INTERNAL_ERROR - on failure
 *      SA_OK                     - on success
 **/

SaErrorT convert_lower_to_upper(char *src,
                                SaHpiInt32T src_len,
                                char *dest,
                                SaHpiInt32T dest_len)
{
        SaHpiInt32T i;

        if (src == NULL || dest == NULL) {
                dbg("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (dest_len < src_len) {
                err("Source string is longer than destination string");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(dest, 0, dest_len);
        for (i = 0; i < src_len; i++)
                dest[i] = toupper(src[i]);

        return SA_OK;
}

/**
 * update_reseource_status()
 *      @res_status     pointer to resource_status_t
 *      @index          index into the resource info fields in res_status
 *      @serial_number  serial_number string to be copied into res_status
 *      @resource_id    resource id to be updated to res_status
 *      @presence       presence status
 *
 *      Description:
 *      This routine updates the resource status entry with passed in
 *      serial_number, resource_id, and presence.  This routine should be
 *      called to set and reset the resource status fields that change
 *      when a a resource gets added and removed.
 *
 *      Return value: none
**/
void oa_soap_update_resource_status(resource_status_t *res_status,
                                    SaHpiInt32T index,
                                    char *serial_number,
                                    SaHpiResourceIdT resource_id,
                                    resource_presence_status_t presence)
{
        if (index <= 0) {
                err("Invalid index value %d - returning without update\n",
                    index);
                return;
        }
        if ((serial_number != NULL) && (serial_number[0] != '\0')) {
                size_t len;
                len = strlen(serial_number);
                strncpy(res_status->serial_number[index-1], serial_number, len);
                res_status->serial_number[index-1][len] = '\0';
        }
        res_status->resource_id[index-1] = resource_id;
        res_status->presence[index-1] = presence;

        return;
}

char * oa_soap_trim_whitespace(char *s) {
  int i, len = strlen(s);

  for (i=(len-1); i>=0; i--) {
    if ((s[i] == ' ') || (s[i] == '\t')) {
      s[i] = 0;
    }
    else
      break;
  }
  return(s);
}

/**
 * update_oa_fw_version
 *      @oh_handler:  Pointer to the plugin handler
 *      @response:    Pointer to the oaInfo structure
 *      @resource_id: Resource Id
 *
 * Purpose:
 *      Updates the RPT entry and IDR entry with OA firmware version
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_HPI_ERR_INAVLID_PARAMS - on invalid parameters
 *      SA_HPI_ERR_INTERNAL_ERROR - on failure
 *      SA_OK                     - on success
 **/

SaErrorT update_oa_fw_version(struct oh_handler_state *oh_handler,
                              struct oaInfo *response,
                              SaHpiResourceIdT resource_id)
{
        SaErrorT rv = SA_OK;
        SaHpiFloat64T fm_version;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        SaHpiIdrIdT IdrId = 0;
        SaHpiIdrFieldT field;
        struct oh_event event;
        SaHpiInt32T major;
        SaHpiInt32T minor;

        if (oh_handler == NULL || response == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("OA rpt is not present");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        if (strlen(response->fwVersion) == 0) {
                err("Firmware version is null string");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        fm_version = atof(response->fwVersion);
        major = (SaHpiUint8T)floor(fm_version);
        minor = rintf((fm_version - major) * 100);

        if(rpt->ResourceInfo.FirmwareMajorRev == major &&
                rpt->ResourceInfo.FirmwareMinorRev == minor){
                return rv;
        }

        if(( major == rpt->ResourceInfo.FirmwareMajorRev &&
        minor < rpt->ResourceInfo.FirmwareMinorRev ) ||
        major < rpt->ResourceInfo.FirmwareMajorRev ) {
                WARN("OA Firmware Version downgraded");
        }
	
        rpt->ResourceInfo.FirmwareMajorRev = major;
        rpt->ResourceInfo.FirmwareMinorRev = minor;

        /* Get the inventory RDR */
        rdr = oh_get_rdr_by_type(oh_handler->rptcache, resource_id,
                                SAHPI_INVENTORY_RDR,
                                SAHPI_DEFAULT_INVENTORY_ID);
        if (rdr == NULL) {
                err("Inventory RDR is not found");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        IdrId = rdr->RdrTypeUnion.InventoryRec.IdrId;

        memset(&field, 0, sizeof(SaHpiIdrFieldT));
        field.Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
        field.Field.DataType = SAHPI_TL_TYPE_TEXT;
        field.Field.Language = SAHPI_LANG_ENGLISH;
        oa_soap_trim_whitespace(response->fwVersion);
        field.Field.DataLength = strlen(response->fwVersion);
        field.AreaId = 1;
        field.FieldId = 3;
        snprintf((char *)field.Field.Data,
                        strlen(response->fwVersion)+ 1,
                        "%s",response->fwVersion);

        rv = oa_soap_set_idr_field(oh_handler, resource_id, IdrId,
                                                &field);
        if (rv != SOAP_OK) {
                err("oa_soap_set_idr_field failed");
                return rv;
        }

        /* Event Handling */
        memset(&event, 0, sizeof(struct oh_event));
        event.event.EventType = SAHPI_ET_RESOURCE;
        memcpy(&event.resource,
                    rpt,
                    sizeof(SaHpiRptEntryT));
        event.event.Severity = SAHPI_INFORMATIONAL;
        event.event.Source = event.resource.ResourceId;
        if (oh_gettimeofday(&(event.event.Timestamp)) != SA_OK) {
                event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
        }
        event.event.EventDataUnion.ResourceEvent.
        ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
        event.rdrs = g_slist_append(event.rdrs, g_memdup(rdr,
                     sizeof(SaHpiRdrT)));
        event.hid = oh_handler->hid;
        oh_evt_queue_push(oh_handler->eventq,
        copy_oa_soap_event(&event));

        return SA_OK;
}

/**
 * oa_soap_get_oa_ip
 *      @server:     Pointer to hostname/IP address of the OA
 *      @network_info_response: Pointer to oaNetworkInfo response structure
 *      @oa_ip:     Pointer to Active/Standby OA IP address
 *
 * Purpose:
 *      Gets the active/standby OA IP address
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK  - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT oa_soap_get_oa_ip(char *server, 
                           struct oaNetworkInfo network_info_response,
                           char *oa_ip)
{
        int ipv6, local_ipv6;
        char *interface_name;
        struct extraDataInfo extra_data_info;
        xmlNode *extra_data = NULL;

        if (server == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* intialize the flags for ipv6  */
        ipv6 = strstr(server, ":") == NULL ? 0 : 1;
        local_ipv6 = (strstr(server, "fe80")- server) == 0 ? 1 : 0;

        extra_data = network_info_response.extraData;
        while (extra_data) {
                soap_getExtraData(extra_data, &extra_data_info);
                if ((!(strcmp(extra_data_info.name, "Ipv6Address0"))) 
                       && (extra_data_info.value != NULL)) {
                        memcpy(network_info_response.ipv6Address0,
                               extra_data_info.value,
                               strlen(extra_data_info.value) -
                               strlen(strchr(extra_data_info.value, '/')));
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6Address1")))
                       && (extra_data_info.value != NULL)) {
                        memcpy(network_info_response.ipv6Address1,
                               extra_data_info.value,
                               strlen(extra_data_info.value) -
                               strlen(strchr(extra_data_info.value, '/')));
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6Address2")))
                       && (extra_data_info.value != NULL)) {
                        memcpy(network_info_response.ipv6Address2,
                               extra_data_info.value,
                               strlen(extra_data_info.value) -
                               strlen(strchr(extra_data_info.value, '/')));
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6Address3")))
                       && (extra_data_info.value != NULL)) {
                        memcpy(network_info_response.ipv6Address3,
                               extra_data_info.value,
                               strlen(extra_data_info.value) -
                               strlen(strchr(extra_data_info.value, '/')));
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6AddressType0"))) &&
                                           (extra_data_info.value != NULL)) {
                        network_info_response.ipv6AddressType0 = 
                                                 extra_data_info.value;
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6AddressType1"))) &&
                                           (extra_data_info.value != NULL)) {
                        network_info_response.ipv6AddressType1 =
                                                extra_data_info.value;
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6AddressType2"))) &&
                                           (extra_data_info.value != NULL)) {
                        network_info_response.ipv6AddressType2 =
                                                extra_data_info.value;
                }
                if ((!(strcmp(extra_data_info.name, "Ipv6AddressType3"))) &&
                                           (extra_data_info.value != NULL)) {
                        network_info_response.ipv6AddressType3 =
                                                extra_data_info.value;
                }
                extra_data = soap_next_node(extra_data);
        }

        if(!ipv6) {
                strncpy(oa_ip, network_info_response.ipAddress,
                        strlen(network_info_response.ipAddress));
        } else if(!local_ipv6) {
                if ((!(strcmp(network_info_response.ipv6AddressType0,
                                                              "STATIC")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address0,
                                strlen(network_info_response.ipv6Address0));
                } else if ((!(strcmp(network_info_response.ipv6AddressType1,
                                                              "STATIC")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address1,
                                strlen(network_info_response.ipv6Address1));
                } else if ((!(strcmp(network_info_response.ipv6AddressType2,
                                                              "STATIC")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address2,
                                strlen(network_info_response.ipv6Address2));
                } else if ((!(strcmp(network_info_response.ipv6AddressType3,
                                                              "STATIC")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address3,
                                strlen(network_info_response.ipv6Address3));
                }
        } else {
                if ((!(strcmp(network_info_response.ipv6AddressType0,
                                                           "LINKLOCAL")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address0,
                                strlen(network_info_response.ipv6Address0));
                } else if ((!(strcmp(network_info_response.ipv6AddressType1,
                                                           "LINKLOCAL")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address1,
                                strlen(network_info_response.ipv6Address1));
                } else if ((!(strcmp(network_info_response.ipv6AddressType2,
                                                           "LINKLOCAL")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address2,
                                strlen(network_info_response.ipv6Address2));
                } else if ((!(strcmp(network_info_response.ipv6AddressType3,
                                                           "LINKLOCAL")))) {
                        strncpy(oa_ip, network_info_response.ipv6Address3,
                                strlen(network_info_response.ipv6Address3));
                }
                interface_name = strchr(server, '%');
                strcat(oa_ip, interface_name);
        }
        return SA_OK;
}

/**
 * oa_soap_check_serial_number()
 *      @slot:              slot number of the blade
 *      @serial_number:     Pointer to serial_number string
 *
 * Purpose:
 *      Just prints out a message. OA sends out information from iLO as it
 *      becomes available. So empty, "[Unknown]" serial numbers are common
 *      During a re-discovery after a switchover a blade with a good serial
 *      number could be replaced with a blade with a bad serial number. So 
 *      only thing we could do is give a warning message. Nothing more. It 
 *      is users responsility to correct it using RBSU/boot utility
 *
 * Detailed Description: NA
 *
 * Return values:
 *                Void
 **/
void oa_soap_check_serial_number(int slot, char *serial_number)
{
        int j=0, len=0;

        if (serial_number == 0 ) { 
               WARN("Blade(%d) serialNumber is NULL",slot);
        } else if ((len = strlen(serial_number)) == 0) { 
               WARN("Blade(%d) serialNumber is empty",slot);
        } else if (strcmp(serial_number,"[Unknown]")) {
               if (len >= 9) 
                    len = 9;
               for (j =0; j < len; j++){
                     if (isalnum(serial_number[j]))
                         continue;
                     else {
                         CRIT("Blade(%d) serialNumber %s is "
                              "invalid",slot,serial_number);
                         break;
                     }
               }
        } else {
               dbg("Blade(%d) serialNumber is [Unknown]",slot);
        }
}

/**
 * oa_soap_sleep_in_loop()
 *      @oa_handler: 	Pointer to OA SOAP handler structure
 *      @secs:		Total time to sleep in seconds
 * 
 * Purpose:
 *	OA takes a long time to switchover and there are many situations
 *	where we need to wait for more than 3 seconds. In all thse situations
 * 	if a signal arrives, it is better to get out asap. So we check for the
 *	shutdown_event_thread after every 3 seconds and get out if it is true
 *	This will enable full, proper shutdown of the plugin in case of kill
 *
 * Detailed Description: NA
 *
 * Return values:
 *                Exit or OK
 **/
SaErrorT oa_soap_sleep_in_loop(struct oa_soap_handler *oa_handler, int secs)
{
        int i=3, j=0; 
        GThread *my_id;
        if (oa_handler == NULL || oa_handler->oa_1 == NULL || 
                 oa_handler->oa_2 == NULL || secs <= 0 ) {
                err("Wrong parameters\n");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if (secs < 4) {
                sleep(secs);
                return SA_OK;
        } 
        my_id = g_thread_self();
        while (j < secs) {

               /* Exit only from event threads, owned by plugin */
               if (my_id == oa_handler->oa_1->thread_handler ||
                   my_id == oa_handler->oa_2->thread_handler ) {
                        OA_SOAP_CHEK_SHUTDOWN_REQ(oa_handler, NULL, NULL, NULL);
               } else {
                        /* In case of infrastructure threads just return */
                        if( oa_handler->shutdown_event_thread )
                                return SA_OK;
               } 
               if (j+i > secs)
                       i=secs-j;         
               if ( i > 0 )
                       sleep(i);
               j = j+i;
                
        }
        return SA_OK;
}
