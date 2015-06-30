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
 *
 * This file has the implementation of the power supply unit event handling
 *
 *      process_ps_insertion_event()  - Processes the power supply unit
 *                                      insertion event
 *
 *      process_ps_extraction_event() - Processes the power supply unit
 *                                      extraction event
 *
 *	oa_soap_proc_ps_subsys_info() - Processes the power subsystem info event
 *
 *	oa_soap_proc_ps_status()      - Processes the power supply status event
 *
 *	oa_soap_proc_ps_info()      - Processes the power supply info event
 */

#include "oa_soap_ps_event.h"
#include "sahpi_wrappers.h"

/**
 * process_ps_insertion_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @con:        Pointer to SOAP_CON structure
 *      @oa_event:   Pointer to oa event response structure
 *
 * Purpose:
 *      Adds the newly inserted power supply information into RPT and RDR table
 *      Creates the hot swap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 *      SA_ERR_HPI_OUT_OF_MEMORY  - on out of memory.
 **/
SaErrorT process_ps_insertion_event(struct oh_handler_state *oh_handler,
                                    SOAP_CON *con,
                                    struct eventInfo *oa_event)
{
        struct getPowerSupplyInfo info;
        struct powerSupplyInfo *response = NULL;
        SaErrorT rv = SA_OK;

        if (oh_handler == NULL || con == NULL || oa_event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        info.bayNumber = oa_event->eventData.powerSupplyStatus.bayNumber;
        response =
          (struct powerSupplyInfo *)g_malloc0(sizeof(struct powerSupplyInfo));
        if( response == NULL){
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        response->presence = PRESENCE_NO_OP;
        response->modelNumber[0] = '\0';
        response->sparePartNumber[0] = '\0';
        response->serialNumber[0] = '\0';
        response->productName[0] = '\0';

        rv = soap_getPowerSupplyInfo(con, &info, response);
        if (rv != SOAP_OK) {
                err("Get power supply info failed");
                wrap_g_free(response);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* If the power supply unit does not have the power cord plugged in,
         * then power supply unit will be in faulty condition. If the power
         * supply is reported as PRESENT by the OA, add the power supply in 
         * a ResourceFailed state.
         */


        if (response->serialNumber[0] == '\0') {
                err("No Serial Number for PSU at slot %d. Please check", 
                        info.bayNumber);
        }

        rv = add_ps_unit(oh_handler, con, response);
        if (rv != SA_OK) {
                err("Add power supply %d failed", response->bayNumber);
                wrap_g_free(response);
                return rv;
        }

        wrap_g_free(response);
        return SA_OK;
}

/**
 * process_ps_extraction_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @oa_event:   Pointer to oa event response structure
 *
 * Purpose:
 *      Gets the power extraction event.
 *      Removes the extracted power supply information from RPT
 *      Creates the hot swap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_ps_extraction_event(struct oh_handler_state *oh_handler,
                                     struct eventInfo *oa_event)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T bay_number;
        struct oa_soap_handler *oa_handler;

        if (oh_handler == NULL || oa_event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        bay_number = oa_event->eventData.powerSupplyStatus.bayNumber;

       /* If the power supply unit does not have the power cord
        * plugged in, then power supply unit will be in faulty
        * condition. In this case, all the information in the
        * response structure is NULL. Then the faulty power supply
        * unit is considered as ABSENT. Ignore the extraction event.
        */
        if (oa_handler->oa_soap_resources.ps_unit.presence[bay_number - 1] ==
            RES_ABSENT) {
                err("Extracted power supply unit may be in faulty condition");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = remove_ps_unit(oh_handler, bay_number);
        if (rv != SA_OK) {
                err("Remove power supply unit failed");
        }

        return SA_OK;
}

/**
 * oa_soap_push_power_events
 *      @oh_handler     : Pointer to openhpi handler structure
 *      @info           : Pointer to power subsystem info structure
 *      @resource_id    : Power subsystem resource id
 *
 * Purpose:
 *      Pushes changes to oa_power controls upstream to HPI domain
 *
 * Detailed Description: NA
 *
 * Return values:
 *      NONE
 **/
void oa_soap_push_power_events(struct oh_handler_state *oh_handler,
                               struct powerSubsystemInfo *info,
                               SaHpiResourceIdT resource_id)
{
        struct oa_soap_handler *oa_handler;
        SaErrorT rv = SA_OK;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct powerConfigInfo *power_config_info;
        struct powerCapConfig *power_cap_config;
        xmlNode *extra_data;
        struct extraDataInfo extra_data_info;
        struct oh_event event;
        int oldMin = 0, oldMax = 0;
        int oldMinDerated = 0, oldMaxDerated = 0;
        int oldMinRated = 0, oldMaxRated = 0;
        int newMin = 0, newMax = 0;
        
        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        power_config_info = &(oa_handler->power_config_info);
        power_cap_config = &(oa_handler->power_cap_config);

        extra_data = info->extraData;
        while (extra_data) {
                soap_getExtraData(extra_data, &extra_data_info);
                if (!(strcmp(extra_data_info.name, "ACLimitLow"))) {
                  newMin = atoi(extra_data_info.value);
                }
                if (!(strcmp(extra_data_info.name, "ACLimitHigh"))) {
                  newMax = atoi(extra_data_info.value);
                }
                extra_data = soap_next_node(extra_data);
        }

        /* If we have a new ACLimitLow or a new ACLimitHigh, then we need to */
        /* push an oh_event                                                  */
        if ((power_config_info->ACLimitLow != newMin) ||
            (power_config_info->ACLimitHigh != newMax)) {
            /* Need to re-build static power limit control for enclosure */
            dbg("Static power limits have changed\n");
            dbg("New static power limit low: %d\n", newMin);
            dbg("New static power limit high: %d\n", newMax);

            rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
            rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
                                  SAHPI_FIRST_ENTRY);

            while (rdr) {
                if (rdr->RdrType == SAHPI_CTRL_RDR) {
                    /* Test to see if this is the static power limit  */
                    /* control rdr                                    */
                    if (rdr->RdrTypeUnion.CtrlRec.Num ==
                        OA_SOAP_STATIC_PWR_LIMIT_CNTRL) {
                        power_config_info->ACLimitLow = newMin;
                        power_config_info->ACLimitHigh = newMax;
                        rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Min = newMin;
                        rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Max = newMax;

                        /* We need to constuct/send an oh_event to update the */
                        /* HPI domain                                         */
                        memset(&event, 0, sizeof(struct oh_event));
                        event.event.EventType = SAHPI_ET_RESOURCE;
                        memcpy(&event.resource, rpt, sizeof(SaHpiRptEntryT));
                        event.event.Severity = SAHPI_INFORMATIONAL;
                        event.event.Source = event.resource.ResourceId;
                        if (oh_gettimeofday(&(event.event.Timestamp))
                            != SA_OK) {
                            event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
                        }
                        event.event.EventDataUnion.ResourceEvent.
                          ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
                        event.rdrs = g_slist_append(event.rdrs, g_memdup(rdr,
                                                    sizeof(SaHpiRdrT)));
                        event.hid = oh_handler->hid;

                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;
                    }
                }
                rdr = oh_get_rdr_next(oh_handler->rptcache,
                                  rpt->ResourceId,
                                  rdr->RecordId);
                if (rdr == NULL) {
                        err("Did not find static power limit control rdr");
                }
            }
        }

        /* Since we do not get the EVENT_ENC_GRP_CAP event - which is  */
        /* supposed to contain new dynamic power cap limits, we will   */
        /* instead query for the latest powerCapConfig, and and        */
        /* inspect the returned data to see if the dynamic power cap   */
        /* limits have changed. We also inspect the derated and rated  */
        /* circuit caps for OA firmware 3.00 and higher. If the limits */
        /* have changed, then construct oh_event and push it to the    */
        /* HPI domain.                                                 */

        /* Save the old limit values for comparison later              */
        oldMin = power_cap_config->enclosurePowerCapLowerBound;
        oldMax = power_cap_config->enclosurePowerCapUpperBound;
        oldMinDerated = power_cap_config->deratedCircuitCapLowerBound;
        oldMaxDerated = power_cap_config->deratedCircuitCapUpperBound;
        oldMinRated = power_cap_config->ratedCircuitCapLowerBound;
        oldMaxRated = power_cap_config->ratedCircuitCapUpperBound;

        /* Make a soap call to get the enclosure power cap config which */
        /* may contain new dynamic power cap limits                     */
        dbg("\n Threadid=%p \n",g_thread_self());

        rv = soap_getPowerCapConfig(oa_handler->active_con,
                                    power_cap_config,
                                    &(oa_handler->desired_dynamic_pwr_cap),
                                    &(oa_handler->desired_derated_circuit_cap),
                                    &(oa_handler->desired_rated_circuit_cap));

        if (rv != SOAP_OK) {
                err("Getting the power cap config failed");
                return;
        }

        newMin = power_cap_config->enclosurePowerCapLowerBound;
        newMax = power_cap_config->enclosurePowerCapUpperBound;

        /* If we have a new enclosurePowerCapLowerBound or a new         */
        /* enclosurePowerCapUpperBound then we need to push an oh_event. */
        if ((newMin != oldMin) || (newMax != oldMax)) {
            /* Need to re-build dynamic power cap control for enclosure */
            dbg("Dynamic power cap has changed\n");
            dbg("New dynamic power cap low: %d\n", newMin);
            dbg("New dynamic power cap high: %d\n", newMax);

            rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
            rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
                  SAHPI_FIRST_ENTRY);

            while (rdr) {
                if (rdr->RdrType == SAHPI_CTRL_RDR) {
                    /* Test to see if this is the dynamic power cap control  */
                    /* rdr                                                   */
                    if (rdr->RdrTypeUnion.CtrlRec.Num ==
                        OA_SOAP_DYNAMIC_PWR_CAP_CNTRL) {
                        rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Min = newMin;
                        rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Max = newMax;

                        /* We need to constuct/send an oh_event to update the */
                        /* HPI domain                                         */
                        memset(&event, 0, sizeof(struct oh_event));
                        event.event.EventType = SAHPI_ET_RESOURCE;
                        memcpy(&event.resource, rpt, sizeof(SaHpiRptEntryT));
                        event.event.Severity = SAHPI_INFORMATIONAL;
                        event.event.Source = event.resource.ResourceId;
                        if (oh_gettimeofday(&(event.event.Timestamp))
                            != SA_OK) {
                            event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
                        }
                        event.event.EventDataUnion.ResourceEvent.
                          ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
                        event.rdrs = g_slist_append(event.rdrs, g_memdup(rdr,
                                                    sizeof(SaHpiRdrT)));
                        event.hid = oh_handler->hid;

                        oh_evt_queue_push(oh_handler->eventq,
                                          copy_oa_soap_event(&event));
                        break;
                    }
                }
                rdr = oh_get_rdr_next(oh_handler->rptcache,
                                      rpt->ResourceId,
                                      rdr->RecordId);
                if (rdr == NULL) {
                        err("Did not find dynamic power cap control rdr");
                }
            }
        }

        /* Check the OA firmware version.  For OA firmware 3.00 and higher, */
        /* we have to check if the limits for derated and rated circuit     */
        /* caps have changed.                                               */
        if (oa_handler->active_fm_ver >= 3.0) {
            /* Check derated circuit cap limits for changes. */
            newMin = power_cap_config->deratedCircuitCapLowerBound;
            newMax = power_cap_config->deratedCircuitCapUpperBound;
            if ((newMin != oldMinDerated) || (newMax != oldMaxDerated)) {
                /* Need to re-build derated circuit cap control for enclosure */
                dbg("Derated circuit cap has changed\n");
                dbg("New derated circuit cap low: %d\n", newMin);
                dbg("New derated circuit cap high: %d\n", newMax);

                rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
                rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
                                      SAHPI_FIRST_ENTRY);

                while (rdr) {
                    if (rdr->RdrType == SAHPI_CTRL_RDR) {
                        /* Test to see if this is the derated circuit cap   */
                        /* control rdr                                      */
                        if (rdr->RdrTypeUnion.CtrlRec.Num ==
                            OA_SOAP_DERATED_CIRCUIT_CAP_CNTRL) {
                            rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Min =
                              newMin;
                            rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Max =
                              newMax;

                            /* We need to constuct/send an oh_event to update */
                            /* the HPI domain                                 */
                            memset(&event, 0, sizeof(struct oh_event));
                            event.event.EventType = SAHPI_ET_RESOURCE;
                            memcpy(&event.resource, rpt,
                                   sizeof(SaHpiRptEntryT));
                            event.event.Severity = SAHPI_INFORMATIONAL;
                            event.event.Source = event.resource.ResourceId;
                            if (oh_gettimeofday(&(event.event.Timestamp))
                                != SA_OK) {
                                    event.event.Timestamp =
                                      SAHPI_TIME_UNSPECIFIED;
                            }
                            event.event.EventDataUnion.ResourceEvent.
                              ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
                            event.rdrs = g_slist_append(event.rdrs,
                                                        g_memdup(rdr,
                                                        sizeof(SaHpiRdrT)));
                            event.hid = oh_handler->hid;

                            oh_evt_queue_push(oh_handler->eventq,
                              copy_oa_soap_event(&event));
                            break;
                        }
                    }
                    rdr = oh_get_rdr_next(oh_handler->rptcache,
                                          rpt->ResourceId,
                                          rdr->RecordId);
                    if (rdr == NULL) {
                            err("Did not find derated circuit cap control rdr");
                    }
                }
            }

            /* Check rated circuit cap limits for changes. */
            newMin = power_cap_config->ratedCircuitCapLowerBound;
            newMax = power_cap_config->ratedCircuitCapUpperBound;
            if ((newMin != oldMinRated) || (newMax != oldMaxRated)) {
                /* Need to re-build rated circuit cap control for enclosure */
                dbg("Rated circuit cap has changed\n");
                dbg("New rated circuit cap low: %d\n", newMin);
                dbg("New rated circuit cap high: %d\n", newMax);

                rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
                rdr = oh_get_rdr_next(oh_handler->rptcache, rpt->ResourceId,
                                      SAHPI_FIRST_ENTRY);

                while (rdr) {
                    if (rdr->RdrType == SAHPI_CTRL_RDR) {
                        /* Test to see if this is the rated circuit cap   */
                        /* control rdr                                    */
                        if (rdr->RdrTypeUnion.CtrlRec.Num ==
                            OA_SOAP_RATED_CIRCUIT_CAP_CNTRL) {
                            rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Min =
                              newMin;
                            rdr->RdrTypeUnion.CtrlRec.TypeUnion.Analog.Max =
                              newMax;

                            /* We need to constuct/send an oh_event to    */
                            /* update the HPI domain                      */
                            memset(&event, 0, sizeof(struct oh_event));
                            event.event.EventType = SAHPI_ET_RESOURCE;
                            memcpy(&event.resource, rpt,
                                   sizeof(SaHpiRptEntryT));
                            event.event.Severity = SAHPI_INFORMATIONAL;
                            event.event.Source = event.resource.ResourceId;
                            if (oh_gettimeofday(&(event.event.Timestamp))
                                != SA_OK) {
                                    event.event.Timestamp =
                                      SAHPI_TIME_UNSPECIFIED;
                            }
                            event.event.EventDataUnion.ResourceEvent.
                              ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;
                            event.rdrs = g_slist_append(event.rdrs,
                                                        g_memdup(rdr,
                                                        sizeof(SaHpiRdrT)));
                            event.hid = oh_handler->hid;

                            oh_evt_queue_push(oh_handler->eventq,
                                              copy_oa_soap_event(&event));
                            break;
                        }
                    }
                    rdr = oh_get_rdr_next(oh_handler->rptcache,
                                          rpt->ResourceId,
                                          rdr->RecordId);
                    if (rdr == NULL) {
                            err("Did not find rated circuit cap control rdr");
                    }
                }
            }
        }

        return;
}

/**
 * oa_soap_proc_ps_subsys_info
 *      @oh_handler	: Pointer to openhpi handler structure
 *      @info		: Pointer to power subsystem info structure
 *
 * Purpose:
 *	Processes the power subsystem info event
 *
 * Detailed Description: NA
 *
 * Return values:
 *	NONE
 **/
void oa_soap_proc_ps_subsys_info(struct oh_handler_state *oh_handler,
				 struct powerSubsystemInfo *info)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
	SaHpiResourceIdT resource_id;

        if (oh_handler == NULL || info == NULL) {
                err("Invalid parameters");
                return;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        resource_id = oa_handler->oa_soap_resources.power_subsystem_rid;

	/* Process the operational status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_OPER_STATUS,
				     info->operationalStatus, 0, 0);

	/* Process the predictive failure status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_PRED_FAIL,
				     info->operationalStatus, 0, 0);

	/* Process the redundancy sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_REDUND,
				     info->redundancy, 0, 0);

        /* Push power events upstream to HPI domain if needed */
        oa_soap_push_power_events(oh_handler, info, resource_id);

	return;
}

/**
 * oa_soap_proc_ps_status
 *      @oh_handler	: Pointer to openhpi handler structure
 *      @status		: Pointer to power supply status structure
 *
 * Purpose:
 *	Processes the power supply status event
 *
 * Detailed Description: NA
 *
 * Return values:
 *	NONE
 **/
void oa_soap_proc_ps_status(struct oh_handler_state *oh_handler,
			    struct powerSupplyStatus *status)
{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler;
	SaHpiResourceIdT resource_id;
	enum diagnosticStatus diag_ex_status[OA_SOAP_MAX_DIAG_EX];

        if (oh_handler == NULL || status == NULL) {
                err("Invalid parameters");
                return;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        resource_id = oa_handler->oa_soap_resources.ps_unit.
			resource_id[status->bayNumber - 1];

	/* Process the operational status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_OPER_STATUS,
				     status->operationalStatus, 0, 0);

	/* Process the predictive failure status sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_PRED_FAIL,
				     status->operationalStatus, 0, 0);

	/* Process the redundancy sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_INT_DATA_ERR,
				    status->diagnosticChecks.
					internalDataError, 0, 0)

	/* Process the device location error sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_LOC_ERR,
				     status->diagnosticChecks.
					deviceLocationError, 0, 0)

	/* Process the device failure error sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_FAIL,
				     status->diagnosticChecks.deviceFailure,
				     0, 0)

	/* Process the device degraded error sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_DEGRAD,
				     status->diagnosticChecks.deviceDegraded,
				     0, 0)

	/* Process the AC failure sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_AC_FAIL,
				     status->diagnosticChecks.acFailure, 0, 0)

	oa_soap_parse_diag_ex(status->diagnosticChecksEx, diag_ex_status);

	/* Process device not supported sensor rdr */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_NOT_SUPPORT,
				     diag_ex_status[DIAG_EX_DEV_NOT_SUPPORT],
				     0, 0)

	/* Process Device mix match sensor */
	OA_SOAP_PROCESS_SENSOR_EVENT(OA_SOAP_SEN_DEV_MIX_MATCH,
				     diag_ex_status[DIAG_EX_DEV_MIX_MATCH],
				     0, 0)

	return;
}

/**
 * oa_soap_proc_ps_info
 *      @oh_handler	: Pointer to openhpi handler structure
 *      @con            : Pointer to the SOAP_CON structure
 *      @oa_event	: Pointer to OA event structure
 *
 * Purpose:
 *	Processes the power supply info event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - success.
 *      SA_ERR_HPI_INVALID_PARAMS - Invalid parameters
 *      SA_ERR_HPI_OUT_OF_MEMORY  - Out of memory
 **/
SaErrorT oa_soap_proc_ps_info(struct oh_handler_state *oh_handler,
                                  SOAP_CON *con,
                                  struct eventInfo *oa_event)

{
        SaErrorT rv = SA_OK;
        struct oa_soap_handler *oa_handler = NULL;
        SaHpiInt32T bay_number, len;
        char *serial_number = NULL;
        xmlNode *extra_data;
        struct extraDataInfo extra_data_info;
        char name[MAX_PRODUCT_NAME_LENGTH+1];
	SaHpiResourceIdT resource_id;

        if (oh_handler == NULL || oa_event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        bay_number = oa_event->eventData.powerSupplyInfo.bayNumber;

        if(!oa_event->eventData.powerSupplyInfo.serialNumber){
                err("Serial # of PSU at %d is NULL", bay_number);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if( oa_event->eventData.powerSupplyInfo.presence != PRESENT ) {
                err("Serial # of PSU at %d is NOT PRESENT", bay_number);
                return SA_ERR_HPI_INVALID_PARAMS;
        }


        len = strlen(oa_event->eventData.powerSupplyInfo.serialNumber);
        serial_number = (char *)g_malloc0(sizeof(char) * len + 1);
        strcpy(serial_number, oa_event->eventData.powerSupplyInfo.serialNumber);
        serial_number[len]='\0';
        if (strcmp(serial_number,"[Unknown]") == 0 )  {
                err("Serial # of PSU at %d is [Unknown]", bay_number);
                wrap_g_free(serial_number);
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }

        name[0] = '\0';
        extra_data = oa_event->eventData.powerSupplyInfo.extraData;
        while (extra_data) {
                soap_getExtraData(extra_data, &extra_data_info);
                if (!(strcmp(extra_data_info.name, "productName"))) {
                   strncpy(name,extra_data_info.value,MAX_PRODUCT_NAME_LENGTH);
                   name[MAX_PRODUCT_NAME_LENGTH] = '\0';
                }
                extra_data = soap_next_node(extra_data);
        }

        resource_id = oa_handler->
                oa_soap_resources.ps_unit.resource_id[bay_number - 1];
        /* Build the inserted ps_unit RPT entry */
        rv = build_power_supply_rpt(oh_handler, name,
                                    bay_number, &resource_id);
        if (rv != SA_OK) {
                err("Failed to build the ps_unit RPT for PSU at %d", bay_number);
                wrap_g_free(serial_number);
                return rv;
        }

        /* Update resource_status structure with resource_id,
         * serial_number, and presence status
         */
        oa_soap_update_resource_status(
                 &oa_handler->oa_soap_resources.ps_unit, bay_number,
                 serial_number, resource_id, RES_PRESENT);

        /* The RDR already exist, but the relevant data is available only now
         * So just go ahead and correct it. When building the RDR the code does
         * take care of already existing RDR.
         */
        rv = build_power_supply_rdr(oh_handler, con,
                                    &(oa_event->eventData.powerSupplyInfo), resource_id);

        wrap_g_free(serial_number);
        return SA_OK;

}
