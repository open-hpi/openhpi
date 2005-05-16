/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *	  Christina Hernandez <hernanc@us.ibm.com>
 */

#include <sim_plugin.h>

/**
 * sim_get_sensor_reading:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @data: Location to store sensor's value (may be NULL).
 * @state: Location to store sensor's state (may be NULL).
 *
 * Retrieves a sensor's value and/or state. Both @data and @state
 * may be NULL, in which case this function can be used to test for
 * sensor presence.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_REQUEST - Sensor is disabled.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT sim_get_sensor_reading(void *hnd,
				    SaHpiResourceIdT rid,
				    SaHpiSensorNumT sid,
				    SaHpiSensorReadingT *reading,
				    SaHpiEventStateT *state)
{
	SaErrorT err;
	SaHpiSensorReadingT working_reading;
	SaHpiEventStateT working_state;
	struct SensorInfo *sinfo;
	
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	sim_lock_handler(custom_handle);

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {	
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and is enabled */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		sim_unlock_handler(custom_handle);
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	if (sinfo->sensor_enabled == SAHPI_FALSE) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

	memset(&working_reading, 0, sizeof(SaHpiSensorReadingT));
	working_state = SAHPI_ES_UNSPECIFIED;

	trace("Sensor Reading: Resource=%s; RDR=%s", rpt->ResourceTag.Data, rdr->IdString.Data);

	/* Get sensor's event state.*/
	err = sim_get_sensor_eventstate(hnd, rid, sid, &working_reading, &working_state);
	if (err) {
		dbg("Cannot determine sensor's event state. Error=%s", oh_lookup_error(err));
		sim_unlock_handler(custom_handle);
		return(err);
	}

	if (reading) memcpy(reading, &working_reading, sizeof(SaHpiSensorReadingT));
	if (state) memcpy(state, &working_state, sizeof(SaHpiEventStateT));

	sim_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * sim_get_sensor_eventstate:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @reading: Location of sensor's reading
 * @state: Location to store sensor's state.
 *
 * Translates and sensor's reading into an event state(s).
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_REQUEST - Sensor is disabled.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_get_sensor_eventstate(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       SaHpiSensorReadingT *reading,
				       SaHpiEventStateT *state)
{
	int i;
	struct SensorInfo *sinfo;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd || !reading || !state) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);
	
	/* Check if sensor exist and is enabled */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}  

	/*If sensor is enabled, set event state to cur_state*/     
	if (sinfo->sensor_enabled == SAHPI_FALSE){
		return(SA_ERR_HPI_INVALID_REQUEST);
	}
	else{
		*state = sinfo->cur_state;
		return(SA_OK);
	}		

	return(SA_OK);
}

/* End of sim_get_sensor_eventstate() */


/**
 * sim_get_sensor_thresholds:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @thres: Location to store sensor's threshold values.
 *
 * Retreives sensor's threshold values, if defined.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_CMD - Sensor is not threshold type, has accessible or readable thresholds.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       SaHpiSensorThresholdsT *thres)
{
	int upper_thresholds, lower_thresholds;
      struct SensorInfo *sinfo;
      struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	SaHpiThresholdsT working;
	
	if (!hnd || !thres) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;
	
	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {	
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and has readable thresholds */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL){
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
        sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* switch - case statements used to set delta*/		
	switch (rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingType) {
		
	case SAHPI_SENSOR_READING_TYPE_INT64:
	{
		SaHpiInt64T delta;
		if (found_thresholds & SAHPI_STM_LOW_MINOR) {
			delta = reading.Value.SensorInt64 - working.LowMinor.Value.SensorInt64;
		}
		else {
			if (found_thresholds & SAHPI_STM_LOW_MAJOR) {
				delta = reading.Value.SensorInt64 - working.LowMajor.Value.SensorInt64;
			}
			else {
				delta = reading.Value.SensorInt64 - working.LowCritical.Value.SensorInt64;
			}
		}

		if (delta < 0) {
			dbg("Negative hysteresis delta is less than 0");
			working.NegThdHysteresis.IsSupported = SAHPI_FALSE;
		}
		else {
			working.NegThdHysteresis.IsSupported = SAHPI_TRUE;
			working.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_INT64;
			working.NegThdHysteresis.Value.SensorInt64 = delta;
		}
		break;
	}
		
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
	{
		SaHpiFloat64T delta;
		if (found_thresholds & SAHPI_STM_LOW_MINOR) {
			delta = reading.Value.SensorFloat64 - working.LowMinor.Value.SensorFloat64;
		}
		else {
			if (found_thresholds & SAHPI_STM_LOW_MAJOR) {
				delta = reading.Value.SensorFloat64 - working.LowMajor.Value.SensorFloat64;
			}
			else {
				delta = reading.Value.SensorFloat64 - working.LowCritical.Value.SensorFloat64;
			}
		}

		if (delta < 0) {
			dbg("Negative hysteresis delta is less than 0");
			working.NegThdHysteresis.IsSupported = SAHPI_FALSE;
		}
		else {
			working.NegThdHysteresis.IsSupported = SAHPI_TRUE;
			working.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
			working.NegThdHysteresis.Value.SensorFloat64 = delta;
		}
		break;
	}
	
	case SAHPI_SENSOR_READING_TYPE_UINT64:
	
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
	
	default:
		dbg("Invalid reading type for threshold. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
}
		
	switch (rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingType) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
	{
		SaHpiInt64T delta;
		if (found_thresholds & SAHPI_STM_UP_MINOR) {
			delta = working.UpMinor.Value.SensorInt64 - reading.Value.SensorInt64;
		}
		else {
			if (found_thresholds & SAHPI_STM_UP_MAJOR) {
				delta = working.UpMajor.Value.SensorInt64 - reading.Value.SensorInt64;
			}
			else {
				delta = working.UpCritical.Value.SensorInt64 - reading.Value.SensorInt64;
			}
		}

		if (delta < 0) {
			dbg("Positive hysteresis delta is less than 0");
			working.PosThdHysteresis.IsSupported = SAHPI_FALSE;
		}
		else {
			working.PosThdHysteresis.IsSupported = SAHPI_TRUE;
			working.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_INT64;
			working.PosThdHysteresis.Value.SensorInt64 = delta;
		}
		break;
	}
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
	{
		SaHpiFloat64T delta;
		if (found_thresholds & SAHPI_STM_UP_MINOR) {
			delta = working.UpMinor.Value.SensorFloat64 - reading.Value.SensorFloat64;
		}
		else {
			if (found_thresholds & SAHPI_STM_UP_MAJOR) {
				delta = working.UpMajor.Value.SensorFloat64 - reading.Value.SensorFloat64;
			}
			else {
				delta = working.UpCritical.Value.SensorFloat64 - reading.Value.SensorFloat64;
			}
		}

		if (delta < 0) {
			dbg("Positive hysteresis delta is less than 0");
			working.PosThdHysteresis.IsSupported = SAHPI_FALSE;
		}
		else {
			working.PosThdHysteresis.IsSupported = SAHPI_TRUE;
			working.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
			working.PosThdHysteresis.Value.SensorFloat64 = delta;
		}
		break;
	}

	case SAHPI_SENSOR_READING_TYPE_UINT64:
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
	default:
			dbg("Invalid reading type for threshold. Sensor=%s", rdr->IdString.Data);
			sim_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

	if (found_thresholds == 0) {
		dbg("No readable thresholds found. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	memcpy(thres, &working, sizeof(SaHpiSensorThresholdsT));
	sim_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * sim_set_sensor_thresholds:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @thres: Location of sensor's settable threshold values.
 *
 * Sets sensor's threshold values.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_CMD - Non-writable thresholds or invalid thresholds.
 * SA_ERR_HPI_INVALID_DATA - Threshold values out of order; negative hysteresis
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_set_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       const SaHpiSensorThresholdsT *thres)
{
	SaErrorT err;
	SaHpiSensorThresholdsT working;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!hnd || !thres) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;
	
	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and has writable thresholds */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	if (rdr->RdrTypeUnion.SensorRec.Category != SAHPI_EC_THRESHOLD ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible == SAHPI_FALSE ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.WriteThold == 0) {
	    	sim_unlock_handler(custom_handle);
	    	return(SA_ERR_HPI_INVALID_CMD);
	}


	if (thres->thdname.IsSupported) {
		working.thdname.IsSupported = SAHPI_TRUE;
		working.thdname.Type = thres->thdname.Type; 
		
		switch(thres->thdname.Type) { 
		
		case SAHPI_SENSOR_READING_TYPE_INT64: 
		working.thdname.Value.SensorInt64 = thres->thdname.Value.SensorInt64; 
		break; 

		case SAHPI_SENSOR_READING_TYPE_FLOAT64: 
		working.thdname.Value.SensorFloat64 = thres->thdname.Value.SensorFloat64; 
		break; 

		case SAHPI_SENSOR_READING_TYPE_UINT64: 
		working.thdname.Value.SensorUint64 = thres->thdname.Value.SensorUint64; 
		break; 

		case SAHPI_SENSOR_READING_TYPE_BUFFER: 
		default: 
		dbg("Invalid threshold reading type."); 
		sim_unlock_handler(custom_handle); 
		
		return(SA_ERR_HPI_INVALID_CMD); 
	} 
	
	return(SA_OK);
}

SaErrorT sim_set_threshold_reading(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       const SaHpiSensorReadingT *reading)
{
	SaErrorT err;
	SaHpiTextBufferT buffer;
	SaHpiFloat64T tmp_num;
	struct SensorInfo *sinfo;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;
	struct sim_value set_value;

	if (!hnd || !reading) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* Convert reading into SIM string structure */
	err = oh_init_textbuffer(&buffer);
	if (err) return(err);

	switch (reading->Type) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
		tmp_num = (SaHpiFloat64T)reading->Value.SensorInt64;
		break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
		tmp_num = reading->Value.SensorFloat64;
		break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
		tmp_num = (SaHpiFloat64T)reading->Value.SensorUint64;
		break;
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
		default:
			dbg("Invalid type for threshold. Sensor=%s", rdr->IdString.Data);
			return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	return(SA_OK);
}

/**
 * sim_get_sensor_enable:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @enable: Location to store sensor's enablement boolean.
 *
 * Retrieves a sensor's boolean enablement status.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @enable is NULL.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_get_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sid,
				   SaHpiBoolT *enable)
{

	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!hnd || !enable) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->sensor_enabled;

	sim_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * sim_set_sensor_enable:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @enable: Enable/disable sensor.
 *
 * Sets a sensor's boolean enablement status.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_set_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sid,
				   const SaHpiBoolT enable)
{

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor enablement */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EnableCtrl == SAHPI_TRUE) {
		dbg("BladeCenter/RSA do not support sim_set_sensor_enable");
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			sim_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		if (sinfo->sensor_enabled != enable) {
			/* Probably need to drive an OID, if hardware supported it */
			sinfo->sensor_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
	}
	else {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	sim_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * sim_get_sensor_event_enable:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @enable: Location to store sensor event enablement boolean.
 *
 * Retrieves a sensor's boolean event enablement status.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_get_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT rid,
					 SaHpiSensorNumT sid,
					 SaHpiBoolT *enable)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!hnd || !enable) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->events_enabled;

	sim_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * sim_set_sensor_event_enable:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @enable: Enable/disable sensor.
 *
 * Sets a sensor's boolean event enablement status.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_set_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT rid,
					 SaHpiSensorNumT sid,
					 const SaHpiBoolT enable)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	sim_lock_handler(custom_handle);

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor event enablement */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT ||
	    rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_READ_ONLY_MASKS) {
		dbg("BladeCenter/RSA do not support sim_set_sensor_event_enable\n");    
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			sim_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		
		if (sinfo->events_enabled != enable) {
			/* Probably need to drive an OID, if hardware supported it */
			sinfo->events_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
	}
	else {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	sim_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * sim_get_sensor_event_masks:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @AssertEventMask: Location to store sensor's assert event mask.
 * @DeassertEventMask: Location to store sensor's deassert event mask.
 *
 * Retrieves a sensor's assert and deassert event masks.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_get_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sid,
					SaHpiEventStateT *AssertEventMask,
					SaHpiEventStateT *DeassertEventMask)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (!AssertEventMask || !DeassertEventMask) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;
	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}
	
	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	} 
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	*AssertEventMask = sinfo->assert_mask;
	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) {
		*DeassertEventMask = sinfo->assert_mask;
	}
	else {
		*DeassertEventMask = sinfo->deassert_mask;	
	}

	sim_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * sim_set_sensor_event_masks:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @sid: Sensor ID.
 * @act: Add/Remove action to perform on event masks.
 * @AssertEventMask: Sensor's assert event mask.
 * @DeassertEventMask: sensor's deassert event mask.
 *
 * Sets a sensor's assert and deassert event masks.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_DATA - @act not valid or @AssertEventMask/@DeassertEventMask
 *                           contain events not supported by sensor. 
 * SA_ERR_HPI_NOT_PRESENT - Sensor doesn't exist.
 **/
SaErrorT sim_set_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sid,
					SaHpiSensorEventMaskActionT act,
					const SaHpiEventStateT AssertEventMask,
					const SaHpiEventStateT DeassertEventMask)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	if (oh_lookup_sensoreventmaskaction(act) == NULL) {
		return(SA_ERR_HPI_INVALID_DATA);
	}
	
	struct sim_hnd *custom_handle = (struct sim_hnd *)handle->data;

	sim_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor event masks */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT) {
		dbg("BladeCenter/RSA do not support sim_set_sensor_event_masks");
                /* Probably need to drive an OID, if hardware supported it */
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			sim_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		SaHpiEventStateT orig_assert_mask = sinfo->assert_mask;
		SaHpiEventStateT orig_deassert_mask = sinfo->deassert_mask;

		/* Check for invalid data in user masks */
		if ( (AssertEventMask != SAHPI_ALL_EVENT_STATES) &&
		     (AssertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) ) { 
			sim_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INVALID_DATA);
		}
		if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)) {
			if  ( (DeassertEventMask != SAHPI_ALL_EVENT_STATES) &&
				(DeassertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) ) {
				sim_unlock_handler(custom_handle);
				return(SA_ERR_HPI_INVALID_DATA);
			}
		}

		/* Add to event masks */
		if (act == SAHPI_SENS_ADD_EVENTS_TO_MASKS) {
			if (AssertEventMask == SAHPI_ALL_EVENT_STATES) {
				sinfo->assert_mask = rdr->RdrTypeUnion.SensorRec.Events;
			}
			else {
				sinfo->assert_mask = sinfo->assert_mask | AssertEventMask;
			}
			if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)) {
				if (DeassertEventMask == SAHPI_ALL_EVENT_STATES) {
					sinfo->deassert_mask = rdr->RdrTypeUnion.SensorRec.Events;
				}
				else {
					sinfo->deassert_mask = sinfo->deassert_mask | DeassertEventMask;
				}
			}
		}
		else { /* Remove from event masks */
			if (AssertEventMask == SAHPI_ALL_EVENT_STATES) {
				sinfo->assert_mask = 0;
			}
			else {
				sinfo->assert_mask = sinfo->assert_mask & ~AssertEventMask;
			}
			if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)) {
				if (DeassertEventMask == SAHPI_ALL_EVENT_STATES) {
					sinfo->deassert_mask = 0;
				}
				else {
					sinfo->deassert_mask = sinfo->deassert_mask & ~DeassertEventMask;
				}
			}
		}
		
		/* Generate event, if needed */
		if (sinfo->assert_mask != orig_assert_mask) {
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
		else {
			if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) &&
			    sinfo->deassert_mask != orig_deassert_mask) {
				/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
			}
		}
	}
	else {
		sim_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	sim_unlock_handler(custom_handle);
	return(SA_OK);
}
