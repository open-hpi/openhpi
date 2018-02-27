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
 * This file supports the functions related to HPI Sensor.
 * The file covers three general classes of function: Sensor ABI functions,
 * Build functions for creating sensor RDRs for resources and sensor specific
 * functions for generating sensor enable and thermal events
 *
 * Sensor ABI functions:
 *
 *      ov_rest_get_sensor_reading()    - Gets the sensor reading of resource
 *
 *      ov_rest_get_sensor_thresholds() - Retreives sensor's threshold values,
 *                                        if defined
 *
 *      ov_rest_get_sensor_enable()     - Retrieves sensor's boolean enablement
 *                                        status
 *
 *      ov_rest_set_sensor_enable()     - Sets sensor's boolean enablement
 *                                        status
 *
 *      ov_rest_get_sensor_event_enable()- Retrieves sensor's boolean event
 *                                         enablement status
 *
 *      ov_rest_set_sensor_event_enable()- Sets sensor's boolean event
 *                                         enablement status
 *
 *      ov_rest_get_sensor_event_masks()- Retrieves sensor's assert and
 *                                        deassert event masks
 *
 *      ov_rest_set_sensor_event_masks()- Sets sensor's assert and deassert
 *                                        event masks
 * Build functions:
 *
 *	ov_rest_build_sen_rdr()		- Builds the sensor
 *
 *	ov_rest_gen_sen_evt()		- Generates the HPI sensor event
 *
 *	ov_rest_gen_res_evt() 		- Generates the HPI resource event
 *
 *	ov_rest_proc_sen_evt()		- Processes the sensor event
 *
 * 	ov_rest_assert_sen_evt()	- Generates the assert sensor event
 *
 */

#include "ov_rest_resources.h"
#include "sahpi_wrappers.h"

/* Forward declarations of static functions */
static SaErrorT ov_rest_gen_sen_evt(struct oh_handler_state *oh_handler,
				    SaHpiRptEntryT *rpt,
				    SaHpiRdrT *rdr,
				    SaHpiInt32T sensor_status,
				    SaHpiFloat64T trigger_reading,
				    SaHpiFloat64T trigger_threshold);

static void ov_rest_gen_res_evt(struct oh_handler_state *oh_handler,
				SaHpiRptEntryT *rpt,
				SaHpiInt32T sensor_status);

/**
 * ov_rest_get_sensor_reading
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @data: Structure for receiving sensor reading values
 *      @state: Structure for receiving sensor event states
 *
 * Purpose:
 *      Gets the sensor reading
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - If the sensor is enabled for reading, then current reading is
 *        retrieved from the resource directly through REST call
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_INVALID_REQUEST - Resource has the sensor disabled
 *      SA_ERR_HPI_UNKNOWN - Invalid entity type.
 **/

SaErrorT ov_rest_get_sensor_reading(void *oh_handler,
                                    SaHpiResourceIdT resource_id,
                                    SaHpiSensorNumT rdr_num,
                                    SaHpiSensorReadingT *data,
                                    SaHpiEventStateT    *state)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        struct ov_rest_handler *ov_handler = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;

        if (oh_handler == NULL || state == NULL || data == NULL) {
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
                err("RPT is NULL for %d resource id", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability %x for resource id %d",
                                     rpt->ResourceCapabilities, resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("RDR not present for rdr number %d for resource id %d",
					rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No data for Sensor '%s' in Resource '%s' at location %d",
                       rdr->IdString.Data, rpt->ResourceTag.Data,
                       rpt->ResourceEntity.Entry[0].EntityLocation);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check whether sensor is enabled */
        if (sensor_info->sensor_enable == SAHPI_FALSE) {
                warn("Sensor '%s' is not enabled for resource '%s'" 
                     " at location %d",
                       rdr->IdString.Data, rpt->ResourceTag.Data,
                       rpt->ResourceEntity.Entry[0].EntityLocation);
                return(SA_ERR_HPI_NOT_PRESENT);
        }

	/* Check whether the reading is supported or not */
	if (rdr->RdrTypeUnion.SensorRec.DataFormat.IsSupported ==
	    SAHPI_FALSE) {
		data->IsSupported = SAHPI_FALSE;
		*state = sensor_info->current_state;
                dbg("Reading Sensor '%s' in resource '%s' at location %d is" 
                    " not supported",rdr->IdString.Data, rpt->ResourceTag.Data,
                       rpt->ResourceEntity.Entry[0].EntityLocation);
		return SA_OK;
	}

        /* Populate the return data with current sensor reading */
        data->IsSupported = sensor_info->sensor_reading.IsSupported;
        data->Type = sensor_info->sensor_reading.Type;
        data->Value = sensor_info->sensor_reading.Value;
        *state = sensor_info->current_state;

        return SA_OK;
}

/**
 * ov_rest_get_sensor_thresholds
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @threshold: Location to store sensor's threshold values
 *
 * Purpose:
 *      Retrieves sensor's threshold values, if defined
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Threshold details are returned if the event category of the sensor
 *        is set to threshold type
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_INVALID_REQUEST - Resource has the sensor disabled
 *      SA_ERR_HPI_UNKNOWN - Invalid entity type.
 **/
SaErrorT ov_rest_get_sensor_thresholds(void *oh_handler,
                                      SaHpiResourceIdT resource_id,
                                      SaHpiSensorNumT rdr_num,
                                      SaHpiSensorThresholdsT *threshold)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL || threshold == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s, for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Sensor supporting threshold shall have their event category set to
         * threshold type. Threshold of a sensor is fetched only if the
         * sensor event category value = SAHPI_EC_THRESHOLD
         */
        if (rdr->RdrTypeUnion.SensorRec.Category != SAHPI_EC_THRESHOLD ||
            rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible ==
                SAHPI_FALSE ||
            rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold == 0) {
                err("Invalid command");
                return SA_ERR_HPI_INVALID_CMD;
        }

        /* setting the return value with the threshold value from the
         * sensor info strucutre
         */
        *threshold = sensor_info->threshold;
        return SA_OK;
}

/**
 * ov_rest_set_sensor_thresholds
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @threshold: Location to store sensor's threshold values
 *
 * Purpose:
 *      Sets sensor's threshold values
 *
 * Detailed Description:
 *      - The threshold values supported by HPE Synergy for different
 *        resource such as thermal limits, fan speed limits are not enabled for
 *        modifications
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - ov_rest plugin does not support this API
 **/
SaErrorT ov_rest_set_sensor_thresholds(void *oh_handler,
                                      SaHpiResourceIdT resource_id,
                                      SaHpiSensorNumT rdr_num,
                                      const SaHpiSensorThresholdsT *threshold)
{
        err("ov_rest set sensor thresholds not supported");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_get_sensor_enable
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @enable: Location to store sensor's enablement boolean
 *
 * Purpose:
 *      Retrieves a sensor's boolean enablement status
 *
 * Detailed Description:
 *
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor enable status is returned from sensor info structure
 *        Sensor enable status determines whether the sensor is enabled for
 *        reading is retrieved from the sensor info structure
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 **/

SaErrorT ov_rest_get_sensor_enable(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiSensorNumT rdr_num,
                                  SaHpiBoolT *enable)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL || enable == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
								resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* setting the return value with the sensor enable status
         * from the sensor info strucutre
         */
        *enable = sensor_info->sensor_enable;
        return SA_OK;
}

/**
 * ov_rest_set_sensor_enable
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @enable: Location to store sensor's enablement boolean
 *
 * Purpose:
 *      Sets a sensor's boolean enablement status
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor enable status in sensor info structure is updated
 *        with enable parameter value if it is different.
 *        Sensor enable status determines whether the sensor is enabled for
 *        reading
 *      - If there is a change in sensor enable status value, then
 *        "Sensor enable change" event is generated to report the change to
 *        framework
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_INVALID_STATE - The blade is in invalid state
 **/
SaErrorT ov_rest_set_sensor_enable(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiSensorNumT rdr_num,
                                  SaHpiBoolT enable)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (rdr->RdrTypeUnion.SensorRec.EnableCtrl == SAHPI_TRUE) {
                /* Retrieve sensor_info structure from the private area
                 * of rdr
                 */
                sensor_info = (struct ov_rest_sensor_info*)
                        oh_get_rdr_data(handler->rptcache, resource_id,
                                        rdr->RecordId);
                if (sensor_info == NULL) {
                        err("No sensor data. Sensor=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }

                if (sensor_info->sensor_enable != enable) {
                        /* Update the sensor enable status with new value and
                         * report the change to the framework through the
                         * sensor enable event
                         */
                        sensor_info->sensor_enable = enable;
                        rv =  generate_sensor_enable_event(oh_handler, rdr_num,
                                                           rpt, rdr,
                                                           sensor_info);
                        if (rv != SA_OK) {
                                err("Event generation failed for resource "
					"id %d", resource_id);
                                return rv;
                        }
                }
        } else {
                err("Sensor %s does not support changing the enable status"
			" for resource id %d", rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }
        return SA_OK;
}

/**
 * ov_rest_get_sensor_event_enable
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @enable: Location to store sensor's enablement boolean
 *
 * Purpose:
 *      Retrieves a sensor's boolean event enablement status
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor event enable status is returned from sensor info structure.
 *        Sensor event enable status determines whether the sensor is enabled
 *        for raising events is retrieved from sensor info structure
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 **/
SaErrorT ov_rest_get_sensor_event_enable(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiSensorNumT rdr_num,
                                         SaHpiBoolT *enable)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL || enable == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        *enable = sensor_info->event_enable;
        return SA_OK;
}

/**
 * ov_rest_set_sensor_event_enable
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource ID
 *      @rdr_num: Sensor rdr number
 *      @enable: Location to store sensor's enablement boolean
 *
 * Purpose:
 *      Sets a sensor's boolean event enablement status
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor event enable status in sensor info structure is updated
 *        with enable parameter value if it is different
 *      - Sensor event enable status determines whether the sensor is enabled
 *        for raising events
 *      - If there is a change in sensor event enable status value, then
 *        "Sensor enable change" event is generated to report the change to
 *        framework
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_set_sensor_event_enable(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiSensorNumT rdr_num,
                                         const SaHpiBoolT enable)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_READ_ONLY) {
                err("Sensor %s does not support changing event enable status "
			"for resource id %d", rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if (sensor_info->event_enable != enable) {
                /* Update the sensor event enable status with new value
                 * and report the change to the framework through the
                 * sensor enable event
                 */
                sensor_info->event_enable = enable;
                rv =  generate_sensor_enable_event(oh_handler, rdr_num,
                                                   rpt, rdr, sensor_info);
                if (rv != SA_OK) {
                        err("Event generation failed for resource id %d",
								resource_id);
                        return rv;
                }
        }
        return SA_OK;
}

/**
 * ov_rest_get_sensor_event_masks
 *      @oh_handler: Handler data pointer.
 *      @resource_id: Resource id
 *      @sid: Sensor rdr number
 *      @assert: Location to store sensor's assert event mask.
 *      @deassert: Location to store sensor's deassert event mask.
 *
 * Purpose:
 *      Retrieves a sensor's assert and deassert event masks.
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor event mask value is returned from sensor info structure.
 *        sensor event mask determines whether the sensor is enabled for
 *        raising events if the threshold values are crossed.
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 **/
SaErrorT ov_rest_get_sensor_event_masks(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSensorNumT rdr_num,
                                       SaHpiEventStateT *assert,
                                       SaHpiEventStateT *deassert)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;

        if (oh_handler == NULL || assert == NULL || deassert == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        /* Retrieve sensor_info structure from the private area of rdr */
        sensor_info = (struct ov_rest_sensor_info*)
                oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s for resource id %d",
					rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        *assert = sensor_info->assert_mask;

        if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) {
                *deassert = sensor_info->assert_mask;
        } else {
                *deassert = sensor_info->deassert_mask;
        }
        return SA_OK;
}

/**
 * ov_rest_set_sensor_event_masks
 *      @oh_handler: Handler data pointer
 *      @resource_id: Resource id
 *      @sid: Sensor rdr number
 *      @assert: Location to store sensor's assert event mask
 *      @deassert: Location to store sensor's deassert event mask
 *
 * Purpose:
 *      Sets a sensor's assert and deassert event masks
 *
 * Detailed Description:
 *      - Fetches sensor info structure from the private data area of
 *        sensor rdr of specified rdr number
 *      - Sensor event mask value in sensor info structure is updated
 *        with assert/deassert parameter values if it is different
 *      - Sensor event mask determines whether the sensor is enabled for
 *        raising events if the threshold values are crossed
 *      - If there is a change in sensor event mask values, then
 *        "Sensor enable change" event is generated to report the change to
 *        framework
 *
 * Return values:
 *      SA_OK - Normal case
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - Invalid resource id specified
 *      SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR
 *      SA_ERR_HPI_NOT_PRESENT - RDR is not present
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error
 *      SA_ERR_HPI_INVALID_DATA - Invalid assert/deassert mask
 *      SA_ERR_HPI_READ_ONLY - The data to be operated upon is read only
 **/
SaErrorT ov_rest_set_sensor_event_masks(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSensorNumT rdr_num,
                                       SaHpiSensorEventMaskActionT action,
                                       SaHpiEventStateT assert,
                                       SaHpiEventStateT deassert)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info=NULL;
        SaHpiEventStateT orig_assert_mask = 0;
        SaHpiEventStateT orig_deassert_mask = 0;
	SaHpiEventStateT check_mask;

        if (oh_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if ((assert == 0) && (deassert == 0)) {
                err("Invalid masks for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (oh_lookup_sensoreventmaskaction(action) == NULL) {
                err("Invalid action for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;

        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
                err("No SENSOR Capability for resource id %d",
							resource_id);
                return SA_ERR_HPI_CAPABILITY;
        }

        /* Retrieve sensor rdr from framework of specified rdr number */
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 resource_id,
                                 SAHPI_SENSOR_RDR,
                                 rdr_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						rdr_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        if (rdr->RdrTypeUnion.SensorRec.EventCtrl != SAHPI_SEC_PER_EVENT) {
                err("Sensor %s do no support setting event masks for "
			"resource id %d", rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_READ_ONLY;
        }

	/* On adding new sensors with different event category or on supporting
	 * new masks for the existing sensors, update below swtich statement
 	 */
	switch (rdr->RdrTypeUnion.SensorRec.Category) {
		case SAHPI_EC_THRESHOLD:
			check_mask = OV_REST_STM_VALID_MASK;
			break;
		case SAHPI_EC_PRED_FAIL:
			check_mask = SAHPI_ES_PRED_FAILURE_DEASSERT |
					SAHPI_ES_PRED_FAILURE_ASSERT;
			break;
		case SAHPI_EC_ENABLE:
			check_mask = SAHPI_ES_DISABLED | SAHPI_ES_ENABLED;
			break;
		case SAHPI_EC_REDUNDANCY:
			check_mask = SAHPI_ES_FULLY_REDUNDANT |
					SAHPI_ES_REDUNDANCY_LOST;
			break;
		default :
			err("Un-supported event category %d detected  for "
				"resource id %d",
				rdr->RdrTypeUnion.SensorRec.Category,
				resource_id);
			return SA_ERR_HPI_INTERNAL_ERROR;
	}

	if (assert !=0 && (assert & ~(check_mask))) {
		err("Assert mask is not valid for resource id %d",
							resource_id);
		return SA_ERR_HPI_INVALID_DATA;
	}
	if (deassert != 0 && (deassert & ~(check_mask))) {
		err("Deassert mask is not valid for resource id %d",
							resource_id);
		return SA_ERR_HPI_INVALID_DATA;
	}

	/* Retrieve sensor_info structure from the private area of rdr */
	sensor_info = (struct ov_rest_sensor_info*)
		oh_get_rdr_data(handler->rptcache, resource_id, rdr->RecordId);
	if (sensor_info == NULL) {
		err("No sensor data. Sensor=%s for resource id %d",
					rdr->IdString.Data, resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	orig_assert_mask = sensor_info->assert_mask;
	orig_deassert_mask = sensor_info->deassert_mask;

	/* Based on the action type, the bits in assert/deassert mask are set
	 * or cleared
	 */
	if (action == SAHPI_SENS_ADD_EVENTS_TO_MASKS) {
		sensor_info->assert_mask = sensor_info->assert_mask | assert;

		if (rpt->ResourceCapabilities &
		    SAHPI_CAPABILITY_EVT_DEASSERTS) {
			sensor_info->deassert_mask = sensor_info->assert_mask;
		} else {
			sensor_info->deassert_mask =
				sensor_info->deassert_mask | deassert;
		}
	} else if (assert != 0 &&
		   action == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS) {
		sensor_info->assert_mask = sensor_info->assert_mask & ~(assert);
		if (rpt->ResourceCapabilities &
		    SAHPI_CAPABILITY_EVT_DEASSERTS) {
			sensor_info->deassert_mask = sensor_info->assert_mask;
		} else if (deassert != 0) {
			sensor_info->deassert_mask =
				sensor_info->deassert_mask & ~(deassert);
		}
	}

	if ((sensor_info->assert_mask != orig_assert_mask) ||
	    (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) &&
		sensor_info->deassert_mask != orig_deassert_mask)) {
		/* If the assert or deassert mask has change, raise a
		 * "sensor enable event"
		 */
		rv =  generate_sensor_enable_event(oh_handler, rdr_num, rpt,
						   rdr, sensor_info);
		if (rv != SA_OK) {
			err("Event generation failed for resource id %d",
								resource_id);
			return rv;
		}
	}

        return SA_OK;
}


/**
 * generate_sensor_enable_event
 *      @oh_handler: Handler data pointer.
 *      @rdr_num: Sensor rdr number.
 *      @rpt: Pointer to rpt structure.
 *      @rdr: Pointer to rdr structure.
 *      @sensor_info: Pointer to sensor information structure
 *
 * Purpose:
 *      Builds and generates the sensor enable event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 **/
SaErrorT generate_sensor_enable_event(void *oh_handler,
                                      SaHpiSensorNumT rdr_num,
                                      SaHpiRptEntryT *rpt,
                                      SaHpiRdrT *rdr,
                                      struct ov_rest_sensor_info *sensor_info)
{
        struct oh_handler_state *handler = NULL;
        struct oh_event event = {0};

        if (oh_handler == NULL || sensor_info == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        event.hid = handler->hid;
        event.event.EventType = SAHPI_ET_SENSOR_ENABLE_CHANGE;
        /* FIXME: map the timestamp of the OV generated event */
        oh_gettimeofday(&(event.event.Timestamp));
        event.event.Severity = SAHPI_INFORMATIONAL;
        memcpy(&event.resource, rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = rpt->ResourceId;
        event.event.EventDataUnion.SensorEnableChangeEvent.SensorNum = rdr_num;
        event.event.EventDataUnion.SensorEnableChangeEvent.SensorType =
                rdr->RdrTypeUnion.SensorRec.Type;
        event.event.EventDataUnion.SensorEnableChangeEvent.EventCategory =
                rdr->RdrTypeUnion.SensorRec.Category;
        event.event.EventDataUnion.SensorEnableChangeEvent.SensorEnable =
                sensor_info->sensor_enable;
        event.event.EventDataUnion.SensorEnableChangeEvent.SensorEventEnable =
                sensor_info->event_enable;
        event.event.EventDataUnion.SensorEnableChangeEvent.AssertEventMask =
                sensor_info->assert_mask;
        event.event.EventDataUnion.SensorEnableChangeEvent.DeassertEventMask =
                sensor_info->deassert_mask;
        event.rdrs = g_slist_append(event.rdrs,
                                    g_memdup(rdr, sizeof(SaHpiRdrT)));

        event.event.EventDataUnion.SensorEnableChangeEvent.OptionalDataPresent=
                 SAHPI_SEOD_CURRENT_STATE;

        /* If the current state is SAHPI_ES_UPPER_CRIT then the current
         * asserted event states are SAHPI_ES_UPPER_CRIT and
         * SAHPI_ES_UPPER_MAJOR.
         */
        if (rdr->RdrTypeUnion.SensorRec.Category == SAHPI_EC_THRESHOLD &&
	     sensor_info->current_state == SAHPI_ES_UPPER_CRIT) {
                event.event.EventDataUnion.SensorEnableChangeEvent.
                        CurrentState = SAHPI_ES_UPPER_CRIT |
                                       SAHPI_ES_UPPER_MAJOR;
        } else {
                event.event.EventDataUnion.SensorEnableChangeEvent.
                        CurrentState = sensor_info->current_state;
        }

        oh_evt_queue_push(handler->eventq, copy_ov_rest_event(&event));
        return SA_OK;
}


/**
 * ov_rest_build_sen_rdr
 *      @oh_handler: Pointer to openhpi handler
 *      @resource_id: Resource id
 *      @rdr: Pointer to rdr structure
 *      @sensor_info: Pointer to the sensor information
 *	@sensor_num: Sensor number
 *
 * Purpose:
 *      Build the sensor RDR
 *
 * Detailed Description:
 *	- Allocates the memory for sensor info
 *	- Copies the sensor RDR from the global sensor array
 *
 * Return values:
 *      SA_OK  - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 **/
SaErrorT ov_rest_build_sen_rdr(struct oh_handler_state *oh_handler,
			       SaHpiResourceIdT resource_id,
			       SaHpiRdrT *rdr,
			       struct ov_rest_sensor_info **sensor_info,
			       SaHpiSensorNumT sensor_num)
{
        SaHpiRptEntryT *rpt = NULL;

	if (oh_handler == NULL || rdr == NULL || sensor_info == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Get the rpt entry of the resource */
	rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
	if (rpt == NULL) {
		err("RPT is NULL for resource id %d", resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Sensor specific information is stored in this structure */
	*sensor_info =
		 g_memdup(&(ov_rest_sen_arr[sensor_num].sensor_info),
			  sizeof(struct ov_rest_sensor_info));
	if (*sensor_info == NULL) {
		err("OV_REST out of memory for resource id %d", resource_id);
		return SA_ERR_HPI_OUT_OF_MEMORY;
	}

	/* Populate the sensor rdr with default value */
	rdr->Entity = rpt->ResourceEntity;
	rdr->RdrType = SAHPI_SENSOR_RDR;
	rdr->RdrTypeUnion.SensorRec = ov_rest_sen_arr[sensor_num].sensor;

	oh_init_textbuffer(&(rdr->IdString));
	oh_append_textbuffer(&(rdr->IdString),
			     ov_rest_sen_arr[sensor_num].comment);

	return SA_OK;
}

/**
 * ov_rest_map_sen_val
 *      @sensor_info: Pointer to the sensor information structure
 *      @sensor_num: Sensor number
 *      @sensor_value: Value of the sensor
 *      @sensor_status: Pointer to the sensor status
 *
 * Purpose:
 *      Maps the sensor value got from REST call to HPI sensor state, if it is
 *	changed
 *
 * Detailed Description:
 *       NA
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - One seeing the unsupported sensor value
 **/
SaErrorT ov_rest_map_sen_val(struct ov_rest_sensor_info *sensor_info,
			     SaHpiSensorNumT sensor_num,
			     SaHpiInt32T sensor_value,
			     SaHpiInt32T *sensor_status)
{
	SaHpiInt32T sensor_class;

	if (sensor_info == NULL || sensor_status == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Get the sensor value */
	sensor_class = ov_rest_sen_arr[sensor_num].sensor_class;

	/* Check whether the sensor value is supported or not */
	if (ov_rest_sen_val_map_arr[sensor_class][sensor_value] == -1) {
		err("Not supported sensor value %d detected for sensor_num %d",
					sensor_value, sensor_num);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	/* Check whether HPI sensor value has changed or not*/
	if (sensor_info->current_state !=
	    ov_rest_sen_val_map_arr[sensor_class][sensor_value]) {
		/* Update the current sensor state */
		sensor_info->current_state =
			ov_rest_sen_val_map_arr[sensor_class][sensor_value];
		/* Get the assert state of the sensor */	
		*sensor_status =
			 ov_rest_sen_assert_map_arr[sensor_class][sensor_value];
	} else {
		/* Sensor value has not changed */  
		*sensor_status = OV_REST_SEN_NO_CHANGE;
	}

	return SA_OK;
}

/**
 * ov_rest_gen_res_evt
 *      @oh_handler: Pointer to openhpi handler
 *      @rpt: Pointer to the rpt structure
 *      @sensor_status: sensor status
 *
 * Purpose:
 *      Generates the HPI reource event
 *	changed
 *
 * Detailed Description:
 *	- If the operational status sensor state is asserted, then resource
 *	  event type is set to FAILURE and ResourceFailed field is set to TRUE.
 *	- If the operational status sensor state is deasserted, then resource
 *	  event type is set to RESTORED and ResourceFailed field is set to
 *	  FALSE.
 *	- Pushes the modified RPT entry to plugin rptcache.
 *	- Raises the HPI resource event.
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - One seeing the unsupported sensor value
 **/
static void ov_rest_gen_res_evt(struct oh_handler_state *oh_handler,
				SaHpiRptEntryT *rpt,
				SaHpiInt32T sensor_status)
{
	SaErrorT rv = SA_OK;
	struct oh_event event = {0};
	struct ovRestHotswapState *hotswap_state = NULL;

	if (oh_handler == NULL || rpt == NULL) {
		err("Invalid parameters");
		return;
	}


	if (sensor_status == OV_REST_SEN_ASSERT_TRUE &&
	    rpt->ResourceFailed != SAHPI_TRUE) {
		/* Resource failed */
		event.event.EventDataUnion.ResourceEvent.
			ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
		rpt->ResourceFailed = SAHPI_TRUE;
	} else if (sensor_status == OV_REST_SEN_ASSERT_FALSE &&
		   rpt->ResourceFailed != SAHPI_FALSE) {
		/* Resource restored */
		event.event.EventDataUnion.ResourceEvent.
			ResourceEventType = SAHPI_RESE_RESOURCE_RESTORED;
		rpt->ResourceFailed = SAHPI_FALSE;
	} else {
		/* Do not generate resource event as there is no change */
		return;
	}

        /* Update the event structure */
        event.hid = oh_handler->hid;
        oh_gettimeofday(&(event.event.Timestamp));
        event.event.Severity = SAHPI_CRITICAL;
        event.event.Source = rpt->ResourceId;
	event.event.EventType = SAHPI_ET_RESOURCE;

	/* Get the hotswap structure */
	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		hotswap_state = (struct ovRestHotswapState *)
                        oh_get_resource_data(oh_handler->rptcache,
                                             rpt->ResourceId);
	}

	/* Update the RPT entry */
        rv = oh_add_resource(oh_handler->rptcache, rpt, hotswap_state, 0);
        if (rv != SA_OK) {
                err("Adding resource failed resource id %d", rpt->ResourceId);
                return;
        }

        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));
}

/**
 * ov_rest_gen_sen_evt
 *      @oh_handler: Pointer to openhpi handler
 *      @rpt: Pointer to the rpt structure
 *      @rdr: Pointer to rdr structure
 *      @sensor_status: Status of the sensor
 *
 * Purpose:
 *      Generates the HPI sensor event
 *
 * Detailed Description:
 * 	- Obtains the sensor event structure from the global sensor array
 *	- If the trigger reading is greater than zero, then sets the trigger
 *	  reading value in the sensor event structure.
 *	- If the trigger threshold is greater than zero, then sets the trigger
 *	  threshold value in the sensor event structure.
 *	- Appends the sensor RDR to the event structure
 *	- Raises the HPI sensor event
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 **/
static SaErrorT ov_rest_gen_sen_evt(struct oh_handler_state *oh_handler,
				    SaHpiRptEntryT *rpt,
	      			    SaHpiRdrT *rdr,
		      		    SaHpiInt32T sensor_status,
				    SaHpiFloat64T trigger_reading,
				    SaHpiFloat64T trigger_threshold)
{
	struct oh_event event = {0};
	SaHpiSensorNumT sensor_num;

	if (oh_handler == NULL || rpt == NULL || rdr == NULL) {
		err("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	sensor_num = rdr->RdrTypeUnion.SensorRec.Num;

	/* Update the event structure */
	/* Get the event structure from global array */
	event.event = ov_rest_sen_arr[sensor_num].sen_evt[sensor_status];
	if (trigger_reading > 0) {
		event.event.EventDataUnion.SensorEvent.TriggerReading.Value.
		SensorFloat64
			 = trigger_reading;
	}
	if (trigger_threshold > 0) {
		event.event.EventDataUnion.SensorEvent.TriggerThreshold.Value.
		SensorFloat64
			= trigger_threshold;
	}

	memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));
        event.event.Source = event.resource.ResourceId;
        event.hid = oh_handler->hid;
        oh_gettimeofday(&(event.event.Timestamp));
	event.rdrs = g_slist_append(event.rdrs,
                                    g_memdup(rdr, sizeof(SaHpiRdrT)));
	/* Raise the HPI sensor event */
        oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

	return SA_OK;
}

/**
 * ov_rest_proc_sen_evt
 *      @oh_handler: Pointer to openhpi handler
 *      @resource_id: Resource Id
 *      @sensor_num: Sensor number
 *      @sensor_value: Sensor value
 *
 * Purpose:
 *	Processes and raises the sensor event
 *
 * Detailed Description:
 *	- Raises the sensor event on changes in the state of the sensors with
 *	  sensor class OPER, PRED_FAIL, REDUND, DIAG, ENC_AGR_OPER,
 *	  ENC_AGR_PRED_FAIL, BOOL, BOOL_RVRS, HEALTH_OPER, HEALTH_PRED_FAIL.
 *	- Raises the resource failed or restored event if there is a change in
 *	  the operational status sensor state.
 *	- Raises the sensor event on changes in the state of the sensors with
 *	  sensor class TEMP
 *
 * Return values:
 *      SA_OK  - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 *      SA_ERR_HPI_INTERNAL_ERROR - ov_rest plugin has encountered an internal
 *                                  error.
 **/
SaErrorT ov_rest_proc_sen_evt(struct oh_handler_state *oh_handler,
			      SaHpiResourceIdT resource_id,
			      SaHpiSensorNumT sensor_num,
			      SaHpiInt32T sensor_value,
			      SaHpiFloat64T trigger_reading,
			      SaHpiFloat64T trigger_threshold)
{
        SaErrorT rv = SA_OK;
	SaHpiInt32T sensor_status;
	SaHpiInt32T sensor_class;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiRdrT *rdr = NULL;
        struct ov_rest_sensor_info *sensor_info = NULL;

        if (oh_handler == NULL) {
                err("wrong parameters passed");
		return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Get the rpt entry of the resource */
        rpt = oh_get_resource_by_id(oh_handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("RPT is NULL for resource id %d", resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Get the enable sensor RDR */
        rdr = oh_get_rdr_by_type(oh_handler->rptcache, rpt->ResourceId,
                                 SAHPI_SENSOR_RDR, sensor_num);
        if (rdr == NULL) {
                err("Sensor RDR %d not present for resource id %d",
						sensor_num, resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Get the sensor information */
        sensor_info = (struct ov_rest_sensor_info*)
		oh_get_rdr_data(oh_handler->rptcache, rpt->ResourceId,
			        rdr->RecordId);
        if (sensor_info == NULL) {
                err("No sensor data. Sensor=%s for resource id %d",
				rdr->IdString.Data, resource_id);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	sensor_class = ov_rest_sen_arr[sensor_num].sensor_class;

	switch (sensor_class) {
		case OV_REST_OPER_CLASS:
			rv = ov_rest_map_sen_val(sensor_info, sensor_num,
						 sensor_value, &sensor_status);
			if (rv != SA_OK) {
				err("Setting sensor value %d has failed"
					" for resource id %d", sensor_value,
							resource_id);
				return rv;
			}

			/* If there is no change in the sensor value, ignore the
			 * OV event
			 */
			if (sensor_status == OV_REST_SEN_NO_CHANGE)
				return SA_OK;

			/* Ignore the sensor event if the sensor is disabled or
			 * sensor event is disabled
			 */
			if (sensor_info->sensor_enable == SAHPI_FALSE ||
			    sensor_info->event_enable == SAHPI_FALSE) {
				dbg ("Sensor is disabled or sensor event is "
				     "disabled");
			} else {
				/* Generate the sensor event */
				ov_rest_gen_sen_evt(oh_handler, rpt, rdr,
						    sensor_status, 0, 0);
			}

			/* Generate resource failed/restored event based on the
			 * operational status sensor state
			 */
			if (sensor_num == OV_REST_SEN_OPER_STATUS) {
				ov_rest_gen_res_evt(oh_handler, rpt,
						    sensor_status);
			}
			break;
		default:
			err("No event support for specified class %d for "
				"resource id %d", sensor_class, resource_id);
	}
	return SA_OK;
}
 

/**
 * ov_rest_assert_sen_evt
 *      @oh_handler		: Pointer to openhpi handler
 *      @rpt			: Pointer to the RPT entry
 *      @assert_sensor_list	: Pointer to the assert sensor list
 *
 * Purpose:
 *	Generates the sensor events for asserted sensors.
 *
 * Detailed Description:
 *	- Extracts the sensor RDR from the assert sensor list
 *	- Obtains the sensor number and sensor class.
 *	- Gets the appropriate sensor assert event structure from the global
 *	  sensor array
 *	- Raises the sensor assert event
 *	- If the operational status of the resource has asserted, then raises
 *	  the resource failed event
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 **/
SaErrorT ov_rest_assert_sen_evt(struct oh_handler_state *oh_handler,
				SaHpiRptEntryT *rpt,
				GSList *assert_sensor_list)
{
	GSList *node = NULL;
	SaHpiRdrT *rdr = NULL;
	SaHpiSensorNumT sensor_num;
	SaHpiInt32T sensor_class;

	if (oh_handler == NULL || rpt == NULL || assert_sensor_list == NULL) {
		err("Invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	for (node = assert_sensor_list; node; node = node->next) {
		rdr = (SaHpiRdrT *)node->data;
		sensor_num = rdr->RdrTypeUnion.SensorRec.Num;
		/* Get the sensor information */

		sensor_class = ov_rest_sen_arr[sensor_num].sensor_class;
                if ( sensor_class != OV_REST_OPER_CLASS )  {
				err("Unrecognized sensor class %d "
				    "is detected", sensor_class);
				/* Release the node->data */
				wrap_g_free(node->data);
				continue;
		 }

		/* If the operational status has failed, raise the resource
		 * failed event
		 */
		if (sensor_num == OV_REST_SEN_OPER_STATUS)
			ov_rest_gen_res_evt(oh_handler, rpt,
					    OV_REST_SEN_ASSERT_TRUE);
		/* Release the node->data */
		wrap_g_free(node->data);
	} /* End of while loop */

	/* Release the assert_sensor_list */
	g_slist_free(assert_sensor_list);

	return SA_OK;
}

/**
 * ov_rest_gen_res_event
 *      @oh_handler: Pointer to openhpi handler
 *      @rpt: Pointer to the rpt structure
 *      @res_status: resource status
 *
 * Purpose:
 *      Generates the HPI reource event.
 *
 * Detailed Description:
 *      - If the resource health status is critical/disabled/warning, then
 *        resource event type is set to FAILURE and ResourceFailed field is
 *        set to TRUE.
 *      - If the resource health status is ok, then resource event type is set
 *        to RESTORED and ResourceFailed field is set to FALSE.
 *      - Pushes the modified RPT entry to plugin rptcache.
 *      - Raises the HPI resource event.
 *
 * Return values:
 *      SA_OK - Normal case.
 *      SA_ERR_HPI_INVALID_PARAMS - On wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - One seeing the unsupported sensor value
 **/
SaErrorT ov_rest_gen_res_event(struct oh_handler_state *oh_handler,
                                SaHpiRptEntryT *rpt,
                                SaHpiInt32T res_status)
{
        SaErrorT rv = SA_OK;
        struct oh_event event = {0};
        SaHpiSeverityT severity = 0;

        if (oh_handler == NULL || rpt == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        memset(&event, 0, sizeof(struct oh_event));

        switch (res_status) {
                case OK:
                        severity = SAHPI_OK;
                        /* Resource restored */
                        rpt->ResourceFailed = SAHPI_FALSE;
                        event.event.EventDataUnion.ResourceEvent.
                              ResourceEventType = SAHPI_RESE_RESOURCE_RESTORED;
                        break;
                case Critical:
                case Disabled:
                        severity = SAHPI_CRITICAL;
                        /* Resource failed */
                        rpt->ResourceFailed = SAHPI_TRUE;
                        event.event.EventDataUnion.ResourceEvent.
                              ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
                        break;
                case Warning:
                        severity = SAHPI_MAJOR;
                        /* Resource failed */
                        rpt->ResourceFailed = SAHPI_TRUE;
                        event.event.EventDataUnion.ResourceEvent.
                              ResourceEventType = SAHPI_RESE_RESOURCE_FAILURE;
                        break;
                default:
                        err("Unknown resource status %d for resource ID %d",
                                        res_status, rpt->ResourceId);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        if (rpt->ResourceSeverity == severity) {
                dbg("Ignore the event. There is no change in resource status"
                        " of resource ID %d", rpt->ResourceId);
                return rv;
        }

        rpt->ResourceSeverity = severity;
        rv = oh_add_resource(oh_handler->rptcache, rpt, NULL, 0);
        if (rv != SA_OK) {
                err("Failed to update rpt for resource ID %d",
						rpt->ResourceId);
                return rv;
        }

        /* Update the event structure */
        event.hid = oh_handler->hid;
        oh_gettimeofday(&(event.event.Timestamp));
        event.event.Source = rpt->ResourceId;
        event.event.Severity = severity;
        event.event.EventType = SAHPI_ET_RESOURCE;
        memcpy(&(event.resource), rpt, sizeof(SaHpiRptEntryT));

        /* Raise the HPI sensor event */
        oh_evt_queue_push(oh_handler->eventq, copy_ov_rest_event(&event));

        return SA_OK;
}
void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                              SaHpiSensorNumT,
                              SaHpiSensorReadingT *,
                              SaHpiEventStateT    *)
                __attribute__ ((weak, alias("ov_rest_get_sensor_reading")));

void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("ov_rest_get_sensor_thresholds")));

void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("ov_rest_set_sensor_thresholds")));

void * oh_get_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT *)
                __attribute__ ((weak, alias("ov_rest_get_sensor_enable")));

void * oh_set_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT)
                __attribute__ ((weak, alias("ov_rest_set_sensor_enable")));

void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak,
                                alias("ov_rest_get_sensor_event_enable")));

void * oh_set_sensor_event_enables (void *,
                                    SaHpiResourceIdT id,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak,
                                alias("ov_rest_set_sensor_event_enable")));

void * oh_get_sensor_event_masks (void *,
                                  SaHpiResourceIdT,
                                  SaHpiSensorNumT,
                                  SaHpiEventStateT *,
                                  SaHpiEventStateT *)
                __attribute__ ((weak,alias("ov_rest_get_sensor_event_masks")));

void * oh_set_sensor_event_masks (void *,
                                  SaHpiResourceIdT,
                                  SaHpiSensorNumT,
                                  SaHpiSensorEventMaskActionT,
                                  SaHpiEventStateT,
                                  SaHpiEventStateT)
                __attribute__ ((weak,alias("ov_rest_set_sensor_event_masks")));


