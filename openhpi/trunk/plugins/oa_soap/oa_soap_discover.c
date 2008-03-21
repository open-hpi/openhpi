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
 * This file implements the discovery functionality. The resources of the
 * HP BladeSystem c-Class are discovered. The c-Class Enclosure, ProLian server
 * blades, interconnect blades, fans, power sub-system and power supplies are
 * discovered.
 *
 *      oa_soap_discover_resources()    - Checks the plugin initialization
 *                                        completion.  Starts the event threads
 *                                        for active and standby OAs. Starts
 *                                        the discovery.
 *
 *      discover_oa_soap_system()       - Discovers all the resources in HP
 *                                        BladeSystem c-Class
 *
 *      discover_enclosure()            - Discovers the enclosure along with
 *                                        its capabilities
 *
 *      discover_server()               - Discovers the ProLiant server blades
 *                                        along with its capabilities
 *
 *      discover_interconnect()         - Discovers the interconnect blades
 *                                        along with its capabilities
 *
 *      discover_fan()                  - Discovers the fan along with
 *                                        its capabilities
 *
 *      discover_power_subsystem()      - Discovers the power subsystem
 *                                        along with its capabilities
 *
 *      discover_power_supply()         - Discovers the power supplies
 *                                        along with its capabilities
 *
 *      discover_oa()                  - Discovers the onboard administrator
 *                                       along with its capabilities
 *
 */

#include <oa_soap_plugin.h>

/**
 * oa_soap_discover_resources
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the resources.
 *      This ABI is called from the OpenHPI framework for
 *      discovering HP BladeSystem cClass resources
 *
 * Detailed Description:
 *      - Checks the plugin initialization completion.
 *        If the plugin initialization has failed, then it tries to do the
 *        plugin initialization and then discovery.
 *      - OpenHPI framework calls this ABI every 3 minutes.
 *        If this function is called after the initial discovery,
 *        then call is ignored and no discovery is done again.
 *      - If the discovery is called for the 1st time (after plugin
 *        initialazation), then, starts the event threads for active and
 *        standby OAs.
 *      - If the discovery is called for the 1st time (after plugin
 *        initialazation), then, starts the discovery
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT oa_soap_discover_resources(void *oh_handler)
{
        struct oh_handler_state *handler;
        struct oa_soap_handler *oa_handler = NULL;
        struct event_handler oa1_event_handler, oa2_event_handler;
        SaErrorT rv = SA_OK;
        GError **error = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Retrieve the oa soap handler from oh_handler */
        handler = (struct oh_handler_state *) oh_handler;
        oa_handler = (struct oa_soap_handler *) handler->data;
        /* Check whether the plugin is initialized or not
         * If not, initialize the plugin
         */
        if (oa_handler == NULL) {
                rv = build_oa_soap_custom_handler(handler);
                if (rv != SA_OK) {
                        err("Plugin initialization failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
        }

        g_mutex_lock(oa_handler->mutex);
        /* Check the status of the plugin */
        switch (oa_handler->status) {
                case DISCOVERY_COMPLETED:
                        /* OpenHPI framework calls the discovery every 3 minutes
                         * OA SOAP plugin gets the changes to resource as
                         * part of event handling, hence re-discovery
                         * is not required. If the discovery is already
                         * done once, ignore and return success
                         */
                        g_mutex_unlock(oa_handler->mutex);
                        dbg("Discovery already done");
                        return SA_OK;
                        break;

                case PLUGIN_NOT_INITIALIZED:
                        /* The plugin has encountered a problem while
                         * initializing, the configured OA may not be reachable.
                         * Try to initialize the plugin again.
                         */
                        g_mutex_unlock(oa_handler->mutex);
                        rv = build_oa_soap_custom_handler(handler);
                        if (rv != SA_OK) {
                                err("Plugin initialization failed");
                                return SA_ERR_HPI_INTERNAL_ERROR;
                        }
                        break;

                case DISCOVERY_FAIL:
                        /* The last discovery has failed.
                         * May be due to active OA is not reachable or
                         * due to OA switchover during discovery.
                         * Try to recover from the problem
                         */
                        g_mutex_unlock(oa_handler->mutex);
                        rv = check_discovery_failure(oh_handler);
                        if (rv != SA_OK) {
                                g_mutex_lock(oa_handler->mutex);
                                oa_handler->status = DISCOVERY_FAIL;
                                g_mutex_unlock(oa_handler->mutex);
                                err("Discovery failed for OA %s",
                                    oa_handler->active_con->server);
                                return SA_ERR_HPI_INTERNAL_ERROR;
                         }
                        break;

                case PRE_DISCOVERY:
                        /* This is the first call for discovery */
                        g_mutex_unlock(oa_handler->mutex);
                        dbg("First discovery");
                        break;

                /* This code should never get executed */
                default:
                        g_mutex_unlock(oa_handler->mutex);
                        err("Wrong oa_soap handler state detected");
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Create the event thread for the OA
         * If the thread_handler is not NULL, then the event threads are
         * already created and skip the event thread creation part
         */
        g_mutex_lock(oa_handler->mutex);
        if (oa_handler->oa_1->thread_handler == NULL) {
                oa1_event_handler.oh_handler = handler;
                oa1_event_handler.oa = oa_handler->oa_1;
                /* Subscribe for events, so that we don't lose any changes
                 * which may happen during discovery
                 */
                rv = create_event_session(oa_handler->oa_1);
                if (rv != SOAP_OK) {
                        /* If the subscription for events fails,
                         * then discovery will fail.
                         * Discovery method has the recovery mechanism.
                         * Hence the error is not handled here
                         */
                        dbg("Subscribe for events failed for OA %s",
                            oa_handler->oa_1->server);
                }

                oa_handler->oa_1->thread_handler =
                        g_thread_create(oa_soap_event_thread,
                                        &oa1_event_handler,
                                        TRUE, error);
                if (oa_handler->oa_1->thread_handler == NULL) {
                        g_mutex_unlock(oa_handler->mutex);
                        err("g_thread_create failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
               }
        }
        else
               dbg("OA %s event thread is already started",
                    oa_handler->oa_1->server);

        /* Create the event thread for OA in slot 2 */
        if (oa_handler->oa_2->thread_handler == NULL) {
                oa2_event_handler.oh_handler = handler;
                oa2_event_handler.oa = oa_handler->oa_2;
                rv = create_event_session(oa_handler->oa_2);
                if (rv != SOAP_OK) {
                        dbg("Subscribe for events failed OA %s",
                            oa_handler->oa_2->server);
                }

                oa_handler->oa_2->thread_handler =
                        g_thread_create(oa_soap_event_thread,
                                        &oa2_event_handler,
                                        TRUE, error);
                if (oa_handler->oa_2->thread_handler == NULL) {
                        g_mutex_unlock(oa_handler->mutex);
                        err("g_thread_create failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
               }
        }
        else
               dbg("OA %s event thread is already started",
                   oa_handler->oa_2->server);

        /* Plug-in intialization is successfully done.
         * Start the discovery of the cClass resources
         */
        rv = discover_oa_soap_system(handler);
        if (rv != SA_OK) {
                oa_handler->status = DISCOVERY_FAIL;
                g_mutex_unlock(oa_handler->mutex);
                err("Discovery failed for active OA %s",
                    oa_handler->active_con->server);
                return rv;
        }

        oa_handler->status = DISCOVERY_COMPLETED;
        g_mutex_unlock(oa_handler->mutex);

        err("Discovery completed for active OA %s",
            oa_handler->active_con->server);
        return SA_OK;
}

/**
 * discover_oa_soap_system
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the OA SOAP resources.
 *      Discovers all the resources of cClass system
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_oa_soap_system(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        dbg("Discovering HP BladeSystem c-Class");
        dbg(" Discovering Enclosure ......................");
        rv = discover_enclosure(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover Enclosure");
                return rv;
        }

        dbg(" Discovering Server Blade ...................");
        rv = discover_server(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover Server Blade");
                return rv;
        }

        dbg(" Discovering InterConnect ...................");
        rv = discover_interconnect(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover InterConnect");
                return rv;
        }

        dbg(" Discovering Fan ............................");
        rv = discover_fan(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover Fan ");
                return rv;
        }

        dbg(" Discovering Power Subsystem ................");
        rv = discover_power_subsystem(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover Power Subsystem ");
                return rv;
        }

        dbg(" Discovering Power Supply Unit ..............");
        rv = discover_power_supply(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover Power Supply Unit");
                return rv;
        }

        dbg(" Discovering OA .............................");
        rv = discover_oa(oh_handler);
        if (rv != SA_OK) {
                err("Failed to discover OA");
                return rv;
        }

        dbg("Discovery completed");
        return SA_OK;
}

/**
 * build_enclosure_info
 *      @oh_handler: Pointer to openhpi handler
 *      @info:       Pointer to enclosure info structure
 *
 * Purpose:
 *      Gets the enclosure info and fills the max bays available for the
 *      enclosure
 *
 * Detailed Description:
 *      - Gets the maximum number bays for server blades, interconnect,
 *        OA, fans and power supply
 *      - Creates the presence matrix for the server blades, interconnect,
 *        OA, fans and power supply
 *      - Initialize the presence matrix to ABSENT
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_enclosure_info(struct oh_handler_state *oh_handler,
                              struct enclosureInfo *info)
{
        struct oa_soap_handler *oa_handler = NULL;
        SaHpiInt32T i;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        /* Create the resource presence matrix for
         * server, interconnect, OA, power supply and fan unit.
         * We need the resource presence matrix for re-discovery to sync
         * with current states of the resources
         */

        /* Build resource presence matrix for servers */
        oa_handler->oa_soap_resources.server.max_bays = info->bladeBays;
        oa_handler->oa_soap_resources.server.presence =
                (enum resource_presence_status *)
                g_malloc0((sizeof(enum resource_presence_status)) *
                          oa_handler->oa_soap_resources.server.max_bays);
        if (oa_handler->oa_soap_resources.server.presence == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        for (i = 0; i < oa_handler->oa_soap_resources.server.max_bays; i++)
                oa_handler->oa_soap_resources.server.presence[i] = RES_ABSENT;

        /* Build resource presence matrix for interconnects */
        oa_handler->oa_soap_resources.interconnect.max_bays =
                info->interconnectTrayBays;
        oa_handler->oa_soap_resources.interconnect.presence =
                (enum resource_presence_status *)
                g_malloc0((sizeof(enum resource_presence_status)) *
                          oa_handler->oa_soap_resources.interconnect.max_bays);
        if (oa_handler->oa_soap_resources.interconnect.presence == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        for (i = 0;
             i < oa_handler->oa_soap_resources.interconnect.max_bays;
             i++)
                oa_handler->oa_soap_resources.interconnect.presence[i] =
                        RES_ABSENT;

        /* Build resource presence matrix for OAs */
        oa_handler->oa_soap_resources.oa.max_bays = info->oaBays;
        oa_handler->oa_soap_resources.oa.presence =
                (enum resource_presence_status *)
                g_malloc0((sizeof(enum resource_presence_status)) *
                          oa_handler->oa_soap_resources.oa.max_bays);
        if (oa_handler->oa_soap_resources.oa.presence == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        for (i = 0; i < oa_handler->oa_soap_resources.oa.max_bays; i++)
                oa_handler->oa_soap_resources.oa.presence[i] = RES_ABSENT;

        /* Build resource presence matrix for fans */
        oa_handler->oa_soap_resources.fan.max_bays = info->fanBays;
        oa_handler->oa_soap_resources.fan.presence =
                (enum resource_presence_status *)
                g_malloc0((sizeof(enum resource_presence_status)) *
                          oa_handler->oa_soap_resources.fan.max_bays);
        if (oa_handler->oa_soap_resources.fan.presence == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        for (i = 0; i < oa_handler->oa_soap_resources.fan.max_bays; i++)
                oa_handler->oa_soap_resources.fan.presence[i] = RES_ABSENT;

        /* Build resource presence matrix for power supply units */
        oa_handler->oa_soap_resources.ps_unit.max_bays = info->powerSupplyBays;
        oa_handler->oa_soap_resources.ps_unit.presence =
                (enum resource_presence_status *)
                g_malloc0((sizeof(enum resource_presence_status)) *
                          oa_handler->oa_soap_resources.ps_unit.max_bays);
        if (oa_handler->oa_soap_resources.ps_unit.presence == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        for (i = 0; i < oa_handler->oa_soap_resources.ps_unit.max_bays; i++)
                oa_handler->oa_soap_resources.ps_unit.presence[i] = RES_ABSENT;

        dbg("server_bays = <%d>",oa_handler->oa_soap_resources.server.max_bays);
        dbg("intercennet_bays = <%d>",
            oa_handler->oa_soap_resources.interconnect.max_bays);
        dbg("oa_bays = <%d>",oa_handler->oa_soap_resources.oa.max_bays);
        dbg("fan_bays = <%d>",oa_handler->oa_soap_resources.fan.max_bays);
        dbg("power_supply_bays = <%d>",
            oa_handler->oa_soap_resources.ps_unit.max_bays);

        return SA_OK;
}

/**
 * build_enclosure_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @name:       Pointer to the name of the enclosure
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Builds the enclosure RPT entry.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_enclosure_rpt(struct oh_handler_state *oh_handler,
                             char *name,
                             SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        char *entity_root = NULL;

        if (oh_handler == NULL || name == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Fetch and encode the entity path required for the rpt field */
        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        memset(&entity_path, 0, sizeof(SaHpiEntityPathT));
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));
        /* Populate the rpt with the details of the enclosure */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[0].EntityLocation = 0;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                 strlen(name) + 1,"%s", name);
        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));

        /* Add the enclosure rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to Add Enclosure Resource");
        }
        return rv;
}

/**
 * build_enclosure_rdr
 *      @oh_handler: Pointer to openhpi handler.
 *      @con:        Pointer to the soap client handler.
 *      @response:   Pointer to enclosure info response structure.
 *      @event:      Pointer to event structure.
 *
 * Purpose:
 *      Populate the enclosure RDR.
 *      Pushes the RDR entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_enclosure_rdr(struct oh_handler_state *oh_handler,
                             SOAP_CON *con,
                             struct enclosureInfo *response,
                             struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_info = NULL;

        if (oh_handler == NULL || event == NULL || con == NULL ||
           response == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for the enclosure */
        rv = build_enclosure_inv_rdr(oh_handler, response, rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to Add enclosure inventory RDR");
                g_free(rdr);
                return rv;
        }

        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        /* Append the rdr to the rdr list to be updated in framework */
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build thermal sensor rdr for the enclosure */
        rv = build_enclosure_thermal_sensor_rdr(oh_handler, con,
                                                1, rdr, &sensor_info);
        if (rv != SA_OK) {
                err("Failed to get sensor rdr of enclosure");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }

        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }

        /* Append the rdr to the rdr list to be updated in framework */
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_enclosure
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discovers the enclosure.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_enclosure(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        struct enclosureInfo response;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        /* Make a soap call to OA requesting for the enclosure information */
        rv = soap_getEnclosureInfo(oa_handler->active_con,
                                   &response);

        if (rv != SOAP_OK) {
                err("Get enclosure info failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Retrieve the enclosure information */
        rv = build_enclosure_info(oh_handler, &response);
        if (rv != SA_OK) {
                err("build enclosure info failed");
                return rv;
        }

        memset(&event, 0, sizeof(struct oh_event));
        event.rdrs = NULL;
        rv = build_enclosure_rpt(oh_handler, response.name,
                                 &(event.resource));
        if (rv != SA_OK) {
                err("build enclosure rpt failed");
                return rv;
        }

        rv = build_enclosure_rdr(oh_handler, oa_handler->active_con,
                                 &response, &event);
        if (rv != SA_OK) {
                err("build enclosure rdr failed");
                return rv;
        }

        /* Push enclosure resource information to OpenHPI
         * framework through a resource event
         */
        push_event_to_queue(oh_handler, &event);

        dbg("Done");
        return rv;
}

/**
 * build_oa_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @bay_number: Bay number of the OA
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the OA RPT entry.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_oa_rpt(struct oh_handler_state *oh_handler,
                      SaHpiInt32T bay_number,
                      SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        char *entity_root = NULL;

        if (oh_handler == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));

        /* Populate the rpt with the details of the enclosure */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_FRU |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 0;
        rpt->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_SYS_MGMNT_MODULE;
        rpt->ResourceEntity.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = 0x0;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(OA_NAME) + 1;
        memset(rpt->ResourceTag.Data,0,SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                 strlen(OA_NAME) + 1, OA_NAME);

        /* Add the OA rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to Add OA RPT");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        return rv;
}

/**
 * build_oa_rdr
 *      @oh_handler: Pointer to openhpi handler.
 *      @con:        Pointer to the soap client handler.
 *      @response:   Pointer OA info response structure.
 *      @event:      Pointer to event structure.
 *
 * Purpose:
 *      Populates the OA RDRs and pushes the RDRs to the infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_oa_rdr(struct oh_handler_state *oh_handler,
                      SOAP_CON *con,
                      struct oaInfo *response,
                      struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_info=NULL;
        SaHpiInt32T bay_number;

        if (oh_handler == NULL || con == NULL || response == NULL ||
            event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        bay_number = response->bayNumber;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for OA */
        rv = build_oa_inv_rdr(oh_handler, response, rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to build OA inventory RDR");
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build thermal sensor rdr for OA */
        rv = build_oa_thermal_sensor_rdr(oh_handler, con,
                                         bay_number, rdr, &sensor_info);
        if (rv != SA_OK) {
                err("Failed to get sensor rdr for OA");
                del_rdr_from_event(event);
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                del_rdr_from_event(event);
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_oa
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the OA.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_oa(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        struct getOaInfo request;
        struct oaInfo response;
        struct getOaStatus status_request;
        struct oaStatus status_response;
        SaHpiInt32T i = 0;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *)oh_handler->data;
        if (oa_handler == NULL) {
                err("OA SOAP handler is NULL");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (i = 1; i <= oa_handler->oa_soap_resources.oa.max_bays; i++) {
                status_request.bayNumber = i;
                rv = soap_getOaStatus(oa_handler->active_con, &status_request,
                                      &status_response);
                if (rv != SOAP_OK) {
                        err("Get OA status failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                /* Sometimes, if the OA is absent, then OA status is shown as
                 * STANDBY in getOaStatus response.  As workaround, if OA
                 * status is STANDBY and oaRedudancy state is set to false,
                 * Then, it is considered as ABSENT.
                 *
                 * But, if the OA is recently inserted, then oaRedudancy state
                 * will be set to false.  In this scenario, the OA state will
                 * be wrongly considered as ABSENT.  This is a known limitation
                 * TODO: Remove this workaround once the fix is available in OA
                 * firmware
                 *
                 * If resource not present, continue checking for next bay
                 */
                if ((status_response.oaRole == OA_ABSENT) ||
                    (status_response.oaRole == STANDBY &&
                     status_response.oaRedundancy == HPOA_FALSE)) {
                        dbg("OA %d is not present", i);
                        continue;
                }

                request.bayNumber = i;
                rv = soap_getOaInfo(oa_handler->active_con, &request,
                                    &response);
                if (rv != SOAP_OK) {
                        err("Get OA Info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                memset(&event, 0, sizeof(struct oh_event));
                event.rdrs = NULL;

                /* Build rpt entry for OA */
                rv = build_oa_rpt(oh_handler, i, &(event.resource));
                if (rv != SA_OK) {
                        err("Failed to build OA RPT");
                        return rv;
                }

                /* Build rdr entry for OA */
                rv = build_oa_rdr(oh_handler, oa_handler->active_con,
                                  &response, &event);
                if (rv != SA_OK) {
                        err("Failed to build OA RDR");
                        return rv;
                }

                /* Push the hotswap event to OpenHPI framework */
                push_event_to_queue(oh_handler, &event);

                /* Update the resource presence matrix with PRESENT state */
                oa_handler->oa_soap_resources.oa.presence[i - 1] = RES_PRESENT;
        }

        dbg("Done");
        return SA_OK;
}

/**
 * build_server_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the soap client handler
 *      @name:       Pointer to the name of the server blade
 *      @bay_number: Bay number of the server blade
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the server blade RPT.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_server_rpt(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          char *name,
                          SaHpiInt32T bay_number,
                          SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        SaHpiPowerStateT state;
        char *entity_root = NULL;
        struct oa_soap_hotswap_state *hotswap_state = NULL;

        if (oh_handler == NULL || con == NULL || name == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));

        /* Populate the rpt with the details of the server */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESET |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_POWER |
                                    SAHPI_CAPABILITY_FRU |
                                    SAHPI_CAPABILITY_MANAGED_HOTSWAP |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_CONTROL |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 0;
        rpt->ResourceEntity.Entry[0].EntityType=SAHPI_ENT_SYSTEM_BLADE;
        rpt->ResourceEntity.Entry[0].EntityLocation= bay_number;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data,0,SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                 rpt->ResourceTag.DataLength,"%s", name);

        /* Get the power state of the server blade to determine the hotswap
         * state.  Hotswap state of the server shall be maintained in private
         * data area of server rpt
         */
        rv = get_server_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power status");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        hotswap_state = (struct oa_soap_hotswap_state *)
                g_malloc0(sizeof(struct oa_soap_hotswap_state));
        if (hotswap_state == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        switch (state) {
                case SAHPI_POWER_ON:
                case SAHPI_POWER_CYCLE:
                        hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
                        break;

                case SAHPI_POWER_OFF:
                        hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;
                        break;

                default :
                        err("unknown power status");
                        if (hotswap_state != NULL)
                                g_free(hotswap_state);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Add the server rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, hotswap_state, 0);
        if (rv != SA_OK) {
                err("Failed to add Server rpt");
                if (hotswap_state != NULL)
                        g_free(hotswap_state);
                return rv;
        }
        return rv;
}

/**
 * build_server_rdr
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the soap client handler.
 *      @response:   Server blade info response structure
 *      @event:      Pointer to the event structure
 *
 * Purpose:
 *      Populate the server blade RDR.
 *      Pushes the RDR entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_server_rdr(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          SaHpiInt32T bay_number,
                          struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_thermal_info = NULL;
        struct oa_soap_sensor_info *sensor_power_info = NULL;

        if (oh_handler == NULL || con == NULL || event == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for server */
        rv = build_server_inv_rdr(oh_handler, con, bay_number, rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to get server inventory RDR");
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build thermal sensor rdr for server */
        rv = build_server_thermal_sensor_rdr(oh_handler, con,
                                             bay_number, rdr,
                                             &sensor_thermal_info);
        if (rv != SA_OK) {
                err("Failed to get server thermal sensor RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId, rdr,
                        sensor_thermal_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build power sensor rdr for server */
        rv = build_server_power_sensor_rdr(oh_handler, con,
                                           bay_number, rdr,
                                           &sensor_power_info);
        if (rv != SA_OK) {
                err("Failed to get server power sensor RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_power_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build control rdr for server */
        rv = build_server_control_rdr(oh_handler,
                                      bay_number, rdr);
        if (rv != SA_OK) {
                err("Failed to get server control RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_server
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the server.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_server(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T i, bay_number = 0;
        struct oa_soap_handler *oa_handler = NULL;
        struct getBladeInfo request;
        struct bladeInfo response;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        /* Discover the ProLiant blades present in server bays */
        for (i = 1; i <= oa_handler->oa_soap_resources.server.max_bays; i++) {
                request.bayNumber = i;
                /* Make a soap call to get the information of blade in the
                 * current bay
                 */
                rv = soap_getBladeInfo(oa_handler->active_con,
                                       &request, &response);
                if (rv != SOAP_OK) {
                        err("Get blade info failed");
                        return rv;
                }

                /* Only ProLiant server blade shall be discovered.
                 * Storage, IO and workstation blades are ignored
                 * and considered as ABSENT
                 * If resource not present, continue checking for next bay
                 */
                if (response.presence != PRESENT ||
                   response.bladeType != BLADE_TYPE_SERVER) {
                        continue;
                }

                memset(&event, 0, sizeof(struct oh_event));
                event.rdrs = NULL;
                bay_number = response.bayNumber;

                /* Build rpt entry for server */
                rv = build_server_rpt(oh_handler, oa_handler->active_con,
                                      response.name, bay_number,
                                      &(event.resource));
                if (rv != SA_OK) {
                        err("Failed to get Server rpt");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                /* Build rdr entry for server */
                rv = build_server_rdr(oh_handler, oa_handler->active_con,
                                      bay_number, &event);
                if (rv != SA_OK) {
                        err("Failed to add Server rdr");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                push_event_to_queue(oh_handler, &event);
                oa_handler->oa_soap_resources.server.presence[i - 1] =
                        RES_PRESENT;
        }
        dbg("Done");
        return SA_OK;
}

/**
 * build_interconnect_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the soap client handler.
 *      @name:       Pointer to the name of the interconnect blade
 *      @bay_number: Bay number of the interconnect blade
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the interconnect RPT.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on malloc failure
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_interconnect_rpt(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                char *name,
                                SaHpiInt32T bay_number,
                                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        SaHpiPowerStateT state;
        char *entity_root = NULL;
        struct oa_soap_hotswap_state *hotswap_state = NULL;

        if (oh_handler == NULL || con == NULL || name == NULL ||
           rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));

        /* Populate the rpt with the details of the interconnect */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESET |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_POWER |
                                    SAHPI_CAPABILITY_FRU |
                                    SAHPI_CAPABILITY_MANAGED_HOTSWAP |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_CONTROL |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 0;
        rpt->ResourceEntity.Entry[0].EntityType= SAHPI_ENT_SWITCH_BLADE;
        rpt->ResourceEntity.Entry[0].EntityLocation= bay_number;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                  rpt->ResourceTag.DataLength, "%s", name);

        /* Get the power state of the interconnect blade to determine the
         * hotswap state. Hotswap state of the interconnect shall be maintained
         * in private data area of interconnect rpt
         */
        rv = get_interconnect_power_state(con, bay_number, &state);
        if (rv != SA_OK) {
                err("Unable to get power status");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        hotswap_state = (struct oa_soap_hotswap_state *)
                g_malloc0(sizeof(struct oa_soap_hotswap_state));
        if (hotswap_state == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        switch (state) {
                case SAHPI_POWER_ON:
                case SAHPI_POWER_CYCLE:
                        hotswap_state->currentHsState = SAHPI_HS_STATE_ACTIVE;
                        break;

                case SAHPI_POWER_OFF:
                        hotswap_state->currentHsState = SAHPI_HS_STATE_INACTIVE;
                        break;

                default :
                        err("unknow power status");
                        if (hotswap_state != NULL)
                                g_free(hotswap_state);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Add the interconnect rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, hotswap_state, 0);
        if (rv != SA_OK) {
                err("Failed to add Interconnect RPT");
                if (hotswap_state != NULL)
                        g_free(hotswap_state);
                return rv;
        }

        return rv;
}

/**
 * build_interconnect_rdr
 *      @oh_handler:    Pointer to openhpi handler
 *      @con:           Pointer to the soap client handler.
 *      @info_response: Interconnect info response structure
 *      @event:         Pointer to the event structure
 *
 * Purpose:
 *      Populate the interconnect blade RDR.
 *      Pushes the RDRs to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_interconnect_rdr(struct oh_handler_state *oh_handler,
                                SOAP_CON* con,
                                SaHpiInt32T bay_number,
                                struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_info = NULL;
        SaHpiRdrT *rdr = NULL;

        if (oh_handler == NULL || con == NULL || event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for interconnect */
        rv = build_interconnect_inv_rdr(oh_handler, con, bay_number,
                                        rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to get interconnect inventory RDR");
                g_free(rdr);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build thermal sensor rdr for interconnect */
        rv = build_interconnect_thermal_sensor_rdr(oh_handler, con, bay_number,
                                                   rdr, &sensor_info);
        if (rv != SA_OK) {
                err("Failed to get interconnect thermal sensor RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return SA_ERR_HPI_INVALID_DATA;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build control rdr for interconnect */
        rv = build_interconnect_control_rdr(oh_handler, bay_number, rdr);
        if (rv != SA_OK) {
                err("Failed to get interconnect control RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return SA_ERR_HPI_INVALID_DATA;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_interconnect
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the interconnect blade.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_interconnect(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T bay_number, i;
        struct getInterconnectTrayInfo info_request;
        struct interconnectTrayInfo info_response;
        struct getInterconnectTrayStatus status_request;
        struct interconnectTrayStatus status_response;
        struct oa_soap_handler *oa_handler = NULL;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        for (i = 1;
             i <= oa_handler->oa_soap_resources.interconnect.max_bays;
             i++) {
                status_request.bayNumber = i;
                rv = soap_getInterconnectTrayStatus(oa_handler->active_con,
                                                    &status_request,
                                                    &status_response);
                if (rv != SOAP_OK) {
                        err("Get Interconnect tray status failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                 /* If resource not present, continue checking for next bay */
                if (status_response.presence != PRESENT) {
                        continue;
                }

                info_request.bayNumber = i;
                rv = soap_getInterconnectTrayInfo(oa_handler->active_con,
                                                  &info_request,
                                                  &info_response);
                if (rv != SOAP_OK) {
                        err("Get Interconnect tray info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                memset(&event, 0, sizeof(struct oh_event));
                event.rdrs = NULL;
                bay_number = i;

                /* Build rpt entry for interconnect */
                rv = build_interconnect_rpt(oh_handler, oa_handler->active_con,
                                       info_response.name, bay_number,
                                       &(event.resource));
                if (rv != SA_OK) {
                       err("Failed to get interconnect RPT");
                       return rv;
                }

                /* Build rdr entry for interconnect */
                rv = build_interconnect_rdr(oh_handler, oa_handler->active_con,
                                            bay_number, &event);
                if (rv != SA_OK) {
                       err("Failed to get interconnect RDR");
                       return rv;
                }
                push_event_to_queue(oh_handler, &event);
                oa_handler->oa_soap_resources.interconnect.presence[i - 1] =
                        RES_PRESENT;
        }
        dbg("Done");
        return rv;
}

/**
 * build_fan_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @name:       Pointer to the name of the Fan
 *      @bay_number: Bay number of the Fan
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the fan RPT.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_fan_rpt(struct oh_handler_state *oh_handler,
                       char *name,
                       SaHpiInt32T bay_number,
                       SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        char *entity_root = NULL;

        if (oh_handler == NULL || name == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));

        /* Populate the rpt with the details of the fan */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_FRU |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 0;
        rpt->ResourceEntity.Entry[0].EntityType= SAHPI_ENT_COOLING_DEVICE;
        rpt->ResourceEntity.Entry[0].EntityLocation= bay_number;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = 0x0;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *) (rpt->ResourceTag.Data),
                 rpt->ResourceTag.DataLength, "%s", name);

        /* Add the fan rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add Fan RPT ");
                return rv;
        }
        return rv;
}

/**
 * build_fan_rdr
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the soap client handler.
 *      @response:   Fan info response structure
 *      @event:      Pointer to the event structure
 *
 * Purpose:
 *      Populate the oa_soap fan RDR.
 *      Pushes the RDR entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_fan_rdr(struct oh_handler_state *oh_handler,
                       SOAP_CON *con,
                       struct fanInfo *response,
                       struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_speed_info = NULL;
        struct oa_soap_sensor_info *sensor_power_info = NULL;
        SaHpiInt32T bay_number;

        if (oh_handler == NULL || con == NULL || response == NULL ||
           event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        bay_number = response->bayNumber;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for fan */
        rv = build_fan_inv_rdr(oh_handler, response, rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to get fan inventory RDR");
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build fan speed sensor rdr for fan */
        rv = build_fan_speed_sensor_rdr(oh_handler, con,
                                        bay_number, rdr, &sensor_speed_info);
        if (rv != SA_OK) {
                err("Failed to get fan speed sensor RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_speed_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build power sensor rdr for fan */
        rv = build_fan_power_sensor_rdr(oh_handler, con,
                                        bay_number, rdr, &sensor_power_info);
        if (rv != SA_OK) {
                err("Failed to get fan power sensor RDR");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_power_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_fan
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the fan.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_fan(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        SaHpiInt32T i;
        struct getFanInfo request;
        struct fanInfo response;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        for (i = 1; i <= oa_handler->oa_soap_resources.fan.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getFanInfo(oa_handler->active_con,
                                     &request, &response);
                if (rv != SOAP_OK) {
                        err("Get Fan Info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                 /* If resource not present, continue checking for next bay */
                if (response.presence != PRESENT)
                        continue;

                memset(&event, 0, sizeof(struct oh_event));
                event.rdrs = NULL;

                /* Build rpt entry for fan */
                rv = build_fan_rpt(oh_handler, response.name,
                                   i, &(event.resource));
                if (rv != SA_OK) {
                        err("Failed to populate Fan RPT");
                        return rv;
                }
                rv = build_fan_rdr(oh_handler, oa_handler->active_con,
                                   &response, &event);
                if (rv != SA_OK) {
                        err("Failed to populate Fan RDR");
                        return rv;
                }
                push_event_to_queue(oh_handler, &event);
                oa_handler->oa_soap_resources.fan.presence[i - 1] = RES_PRESENT;
        }
        dbg("Done");
        return SA_OK;
}

/**
 * build_power_subsystem_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @name:       Pointer to the name of the Power subsystem
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the power supply RPT.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_power_subsystem_rpt(struct oh_handler_state *oh_handler,
                                   char *name,
                                   SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        char *entity_root = NULL;

        if (oh_handler == NULL || name == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Fetch and encode the entity path required for the rpt field */
        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));
        /* Populate the rpt with the details of the power subsystem */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_SENSOR;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 0;
        rpt->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_POWER_MGMNT;
        rpt->ResourceEntity.Entry[0].EntityLocation = 1;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = 0x0;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                 rpt->ResourceTag.DataLength, "%s", name);

        /* Add the power subsystem rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add Power Subsystem RPT");
                return rv;
        }
        return rv;
}

/**
 * build_power_subsystem_rdr
 *      @oh_handler: Pointer to openhpi handler
 *      @event:      Pointer to the event structure
 *
 * Purpose:
 *      Populate the power supply RDR.
 *      Pushes the RDR entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_power_subsystem_rdr(struct oh_handler_state *oh_handler,
                                   struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_sensor_info *sensor_input_power_info = NULL;
        struct oa_soap_sensor_info *sensor_output_power_info = NULL;
        struct oa_soap_sensor_info *sensor_power_consumed_info = NULL;
        struct oa_soap_sensor_info *sensor_power_capacity_info = NULL;

        if (oh_handler == NULL || event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        /* Build the input power sensor RDR */
        rv = build_ps_subsystem_input_power_sensor_rdr( oh_handler,
                rdr, &sensor_input_power_info);
        if (rv != SA_OK) {
                err("Failed to get input power sensor RDR for power subsystem");
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_input_power_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        /* Build the ouput power sensor RDR */
        rv = build_ps_subsystem_output_power_sensor_rdr(oh_handler,
                rdr, &sensor_output_power_info);
        if (rv != SA_OK) {
                err("Failed to get output power sensor RDR "
                    "for power subsystem");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_output_power_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        /* Build the power consumed sensor RDR */
        rv = build_ps_subsystem_power_consumed_sensor_rdr(oh_handler,
                rdr, &sensor_power_consumed_info);
        if (rv != SA_OK) {
                err("Failed to get power consumed sensor RDR "
                    "for power subsystem");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_power_consumed_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        /* Build the power capacity sensor RDR */
        rv = build_ps_subsystem_power_capacity_sensor_rdr(oh_handler,
                rdr, &sensor_power_capacity_info);
        if (rv != SA_OK) {
                err("Failed to get power capacity sensor RDR "
                    "for power subsystem");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_power_capacity_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_power_subsystem
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the power supply.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_power_subsystem(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        char power_subsystem[] = POWER_SUBSYSTEM_NAME;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        memset(&event, 0, sizeof(struct oh_event));
        event.rdrs = NULL;

        /* Build rpt entry for power sub system */
        rv = build_power_subsystem_rpt(oh_handler, power_subsystem,
                                       &(event.resource));
        if (rv != SA_OK) {
                err("build power subsystem rpt failed");
                return rv;
        }

        /* Build rdr entry for power sub system*/
        rv = build_power_subsystem_rdr(oh_handler, &event);
        if (rv != SA_OK) {
                err("build power subsystem RDR failed");
                return rv;
        }

        push_event_to_queue(oh_handler, &event);

        dbg("Done");
        return rv;
}

/**
 * build_power_supply_rpt
 *      @oh_handler: Pointer to openhpi handler
 *      @name:       Pointer to the name of the Power supply
 *      @bay_number: Bay number of the Power supply
 *      @rpt:        Pointer to the rpt entry
 *
 * Purpose:
 *      Populate the power supply unit RPT.
 *      Pushes the RPT entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_power_supply_rpt(struct oh_handler_state *oh_handler,
                                char *name,
                                SaHpiInt32T bay_number,
                                SaHpiRptEntryT *rpt)
{
        SaErrorT rv = SA_OK;
        SaHpiEntityPathT entity_path;
        char *entity_root = NULL;

        if (oh_handler == NULL || name == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entity_root = (char *)g_hash_table_lookup(oh_handler->config,
                                                  "entity_root");
        rv = oh_encode_entitypath(entity_root, &entity_path);
        if (rv != SA_OK) {
                err("Encoding entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(rpt, 0, sizeof(SaHpiRptEntryT));

        /* Populate the rpt with the details of the power supply */
        rpt->ResourceCapabilities = SAHPI_CAPABILITY_RDR |
                                    SAHPI_CAPABILITY_RESOURCE |
                                    SAHPI_CAPABILITY_FRU |
                                    SAHPI_CAPABILITY_SENSOR |
                                    SAHPI_CAPABILITY_INVENTORY_DATA;
        rpt->ResourceEntity.Entry[2].EntityType = SAHPI_ENT_ROOT;
        rpt->ResourceEntity.Entry[2].EntityLocation = 0;
        rpt->ResourceEntity.Entry[1].EntityType = SAHPI_ENT_POWER_MGMNT;
        rpt->ResourceEntity.Entry[1].EntityLocation = 1;
        rpt->ResourceEntity.Entry[0].EntityType = SAHPI_ENT_POWER_SUPPLY;
        rpt->ResourceEntity.Entry[0].EntityLocation = bay_number;
        rv = oh_concat_ep(&rpt->ResourceEntity, &entity_path);
        if (rv != SA_OK) {
                err("concat of entity path failed");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rpt->ResourceId = oh_uid_from_entity_path(&(rpt->ResourceEntity));
        rpt->ResourceSeverity = SAHPI_OK;
        rpt->ResourceInfo.ResourceRev = 0;
        rpt->ResourceFailed = SAHPI_FALSE;
        rpt->HotSwapCapabilities = 0x0;
        rpt->ResourceTag.DataType = SAHPI_TL_TYPE_TEXT;
        rpt->ResourceTag.Language = SAHPI_LANG_ENGLISH;
        rpt->ResourceTag.DataLength = strlen(name) + 1;
        memset(rpt->ResourceTag.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        snprintf((char *)rpt->ResourceTag.Data,
                 rpt->ResourceTag.DataLength, "%s", name);

        /* Add the power supply rpt to the plugin rptcache */
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to add power supply unit RPT");
                return rv;
        }
        return rv;
}

/**
 * build_power_supply_rdr
 *      @oh_handler: Pointer to openhpi handler
 *      @con:        Pointer to the soap client handler.
 *      @response:   Power supply info response structure
 *      @event:      Pointer to the event structure
 *
 * Purpose:
 *      Populate the power supply unit RDR.
 *      Pushes the RDR entry to infrastructure
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT build_power_supply_rdr(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                struct powerSupplyInfo *response,
                                struct oh_event *event)
{
        SaErrorT rv = SA_OK;
        SaHpiRdrT *rdr = NULL;
        struct oa_soap_inventory *inventory = NULL;
        struct oa_soap_sensor_info *sensor_info = NULL;

        if (oh_handler == NULL || con == NULL ||
            response == NULL || event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build inventory rdr for power supply */
        rv = build_power_inv_rdr(oh_handler, response, rdr, &inventory);
        if (rv != SA_OK) {
                err("Failed to get power supply unit inventory RDR");
                g_free(rdr);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, inventory, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        rdr = NULL;
        rdr = (SaHpiRdrT *)g_malloc0(sizeof(SaHpiRdrT));
        if (rdr == NULL) {
                err("Out of memory");
                del_rdr_from_event(event);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        /* Build power sensor rdr for power supply */
        rv = build_ps_power_sensor_rdr(oh_handler, con,
                                       response->bayNumber, rdr, &sensor_info);
        if (rv != SA_OK) {
                err("Failed to get power sensor RDR for power supply unit");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        rv = oh_add_rdr(oh_handler->rptcache, event->resource.ResourceId,
                        rdr, sensor_info, 0);
        if (rv != SA_OK) {
                err("Failed to add rdr");
                g_free(rdr);
                del_rdr_from_event(event);
                return rv;
        }
        event->rdrs = g_slist_append(event->rdrs, rdr);

        return rv;
}

/**
 * discover_power_supply
 *      @oh_handler: Pointer to openhpi handler
 *
 * Purpose:
 *      Discover the power supply.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT discover_power_supply(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        SaHpiInt32T i;
        struct getPowerSupplyInfo request;
        struct powerSupplyInfo response;
        char power_supply[] = POWER_SUPPLY_NAME;
        struct oh_event event;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        for (i = 1; i <= oa_handler->oa_soap_resources.ps_unit.max_bays; i++) {
                request.bayNumber = i;
                rv = soap_getPowerSupplyInfo(oa_handler->active_con,
                                             &request, &response);
                if (rv != SOAP_OK) {
                        err("Get power supply info failed");
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                /* If resource not present, continue checking for next bay */
                if (response.presence != PRESENT)
                        continue;

                memset(&event, 0, sizeof(struct oh_event));
                event.rdrs = NULL;

                /* Build the rpt entry for power supply unit */
                rv = build_power_supply_rpt(oh_handler, power_supply,
                                            i, &(event.resource));
                if (rv != SA_OK) {
                        err("build power supply unit rpt failed");
                        return rv;
                }

                /* Build the rdr entry for power supply */
                rv = build_power_supply_rdr(oh_handler, oa_handler->active_con,
                                            &response, &event);
                if (rv != SA_OK) {
                        err("build power supply unit RDR failed");
                        return rv;
                }

                push_event_to_queue(oh_handler, &event);
                oa_handler->oa_soap_resources.ps_unit.presence[i - 1] =
                        RES_PRESENT;
        }
        dbg("Done");
        return rv;
}

void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("oa_soap_discover_resources")));
