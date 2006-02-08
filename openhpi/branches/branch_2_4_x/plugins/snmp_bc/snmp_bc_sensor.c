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
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

/**
 * snmp_bc_get_sensor_reading:
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
SaErrorT snmp_bc_get_sensor_reading(void *hnd,
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
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {	
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and is enabled */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	if (sinfo->sensor_enabled == SAHPI_FALSE) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_REQUEST);
	}

	memset(&working_reading, 0, sizeof(SaHpiSensorReadingT));
	working_state = SAHPI_ES_UNSPECIFIED;

	trace("Sensor Reading: Resource=%s; RDR=%s", rpt->ResourceTag.Data, rdr->IdString.Data);
	
	/************************************************************
	 * Get sensor's reading.
         * Need to go through this logic, since user may request just
         * the event state for a readable sensor. Need to translate
         * sensor reading to event in this case.
         ************************************************************/
	if (rdr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
		err = snmp_bc_get_sensor_oid_reading(hnd, rid, sid, sinfo->mib.oid, &working_reading);
		if (err) {
			dbg("Cannot determine sensor's reading. Error=%s", oh_lookup_error(err));
			snmp_bc_unlock_handler(custom_handle);
			return(err);
		}
	}
	else {
		working_reading.IsSupported = SAHPI_FALSE;
	}

	/******************************************************************
	 * Get sensor's event state.
         * Always get the event state, to reset the sensor's current state,
         * whether caller wants to know event state or not.
	 ******************************************************************/
	err = snmp_bc_get_sensor_eventstate(hnd, rid, sid, &working_reading, &working_state);
	if (err) {
		dbg("Cannot determine sensor's event state. Error=%s", oh_lookup_error(err));
		snmp_bc_unlock_handler(custom_handle);
		return(err);
	}

#if 0
	{       /* Debug section */
		SaHpiTextBufferT buffer;

		dbg("Sensor=%s", rdr->IdString.Data);
		oh_decode_sensorreading(working_reading, rdr->RdrTypeUnion.SensorRec.DataFormat, &buffer);
		dbg("  Reading: %s.", buffer.Data);
		
		oh_decode_eventstate(working_state, rdr->RdrTypeUnion.SensorRec.Category, &buffer);
		dbg("  Event State: %s\n", buffer.Data);
	}
#endif

	/* sinfo->cur_state = working_state; */
	if (reading) memcpy(reading, &working_reading, sizeof(SaHpiSensorReadingT));
	if (state) memcpy(state, &working_state, sizeof(SaHpiEventStateT));

	snmp_bc_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * snmp_bc_get_sensor_eventstate:
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
SaErrorT snmp_bc_get_sensor_eventstate(void *hnd,
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
	if (sinfo->sensor_enabled == SAHPI_FALSE) return(SA_ERR_HPI_INVALID_REQUEST);

	/* If sensor is not readable, return current event state */
	if (rdr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_FALSE) {
		*state = sinfo->cur_state;
		return(SA_OK);
	}
	
	/***************************************************************************
	 * Translate reading into event state. Algorithm is:
	 * - If sensor is a threshold and has readable thresholds.
         *   - If so, check from most severe to least
	 * - If not found or not a threshold value, search reading2event array.
	 *   - Check for Ranges supported; return after first match.
	 *   - Nominal only - reading must match nominal value
	 *   - Max && Min - min value <= reading <= max value
	 *   - Max only - reading > max value 
	 *   - Min only - reading < min value
	 * - else SAHPI_ES_UNSPECIFIED
	 ***************************************************************************/
	if (rdr->RdrTypeUnion.SensorRec.Category == SAHPI_EC_THRESHOLD &&
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold != 0) {
		SaHpiSensorThresholdsT thres;		
		memset(&thres, 0, sizeof(SaHpiSensorThresholdsT));

		SaErrorT err = snmp_bc_get_sensor_thresholds(hnd, rid, sid, &thres);
		if (err) {
			dbg("Cannot get sensor thresholds for Sensor=%s. Error=%s", 
			    rdr->IdString.Data, oh_lookup_error(err));
			return(err);
		}
		if (thres.LowCritical.IsSupported == SAHPI_TRUE) {
			if (oh_compare_sensorreading(reading->Type, reading, &thres.LowCritical) <= 0) {
				*state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR;
				return(SA_OK);
			}
		}
		if (thres.LowMajor.IsSupported == SAHPI_TRUE) {
			if (oh_compare_sensorreading(reading->Type, reading, &thres.LowMajor) <= 0) {
				*state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR;
				return(SA_OK);
			}
		}
		if (thres.LowMinor.IsSupported == SAHPI_TRUE) {
			if (oh_compare_sensorreading(reading->Type, reading, &thres.LowMinor) <= 0) {
				*state = SAHPI_ES_LOWER_MINOR;
				return(SA_OK);
			}
		}
		if (thres.UpCritical.IsSupported == SAHPI_TRUE) {
			if (oh_compare_sensorreading(reading->Type, reading, &thres.UpCritical) >= 0) {
				*state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR;
				return(SA_OK);
			}
		}
		if (thres.UpMajor.IsSupported == SAHPI_TRUE) {
			if (oh_compare_sensorreading(reading->Type, reading, &thres.UpMajor) >= 0) {
				*state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR;
				return(SA_OK);
			}
		}
		if (thres.UpMinor.IsSupported == SAHPI_TRUE) {		
			if (oh_compare_sensorreading(reading->Type, reading, &thres.UpMinor) >= 0) {
				*state = SAHPI_ES_UPPER_MINOR;
				return(SA_OK);
			}
		}
	}		
	
	/* Check reading2event array */
	for (i=0; i < SNMP_BC_MAX_SENSOR_READING_MAP_ARRAY_SIZE &&
		     sinfo->reading2event[i].num != 0; i++) {

		/* reading == nominal */
		if (sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_NOMINAL) {
			if (oh_compare_sensorreading(reading->Type, reading, 
						     &sinfo->reading2event[i].rangemap.Nominal) == 0) {
				*state = sinfo->reading2event[i].state;
				return(SA_OK);
			}
		}
		/* min <= reading <= max */
		if (sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MAX &&
		    sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MIN) {
			if (oh_compare_sensorreading(reading->Type, reading, 
						     &sinfo->reading2event[i].rangemap.Min) >= 0 &&
			    oh_compare_sensorreading(reading->Type, reading, 
						     &sinfo->reading2event[i].rangemap.Max) <= 0) {
				*state = sinfo->reading2event[i].state;
				return(SA_OK);
			}
		}
		/* reading > max */
		if (sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MAX &&
		    !(sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MIN)) {
			if (oh_compare_sensorreading(reading->Type, reading, 
						     &sinfo->reading2event[i].rangemap.Max) > 0) {
				*state = sinfo->reading2event[i].state;
				return(SA_OK);
			}
		}
		/* reading < min */
		if (!(sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MAX) &&
		    sinfo->reading2event[i].rangemap.Flags & SAHPI_SRF_MIN) {
			if (oh_compare_sensorreading(reading->Type, reading, 
						     &sinfo->reading2event[i].rangemap.Min) < 0) {
				*state = sinfo->reading2event[i].state;
				return(SA_OK);
			}
		}
	}

        /* Unfortunately for thresholds, this also means normal */
	*state = SAHPI_ES_UNSPECIFIED; 

	return(SA_OK);
}

#define get_threshold(thdmask, thdname) \
do { \
        if (rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold & thdmask) { \
		if (sinfo->mib.threshold_oids.thdname == NULL || \
		    sinfo->mib.threshold_oids.thdname[0] == '\0') { \
			dbg("No OID defined for readable threshold. Sensor=%s", rdr->IdString.Data); \
			snmp_bc_unlock_handler(custom_handle); \
			return(SA_ERR_HPI_INTERNAL_ERROR); \
		} \
		SaErrorT err = snmp_bc_get_sensor_oid_reading(hnd, rid, sid, \
							      sinfo->mib.threshold_oids.thdname, \
							      &(working.thdname)); \
		if (err) { \
			snmp_bc_unlock_handler(custom_handle); \
			return(err); \
		} \
		if (working.thdname.Type == SAHPI_SENSOR_READING_TYPE_BUFFER) { \
			dbg("Sensor type SAHPI_SENSOR_READING_TYPE_BUFFER cannot have thresholds. Sensor=%s", \
			    rdr->IdString.Data); \
			snmp_bc_unlock_handler(custom_handle); \
			return(SA_ERR_HPI_INTERNAL_ERROR); \
		} \
		found_thresholds = found_thresholds | thdmask; \
	} \
	else { \
		working.thdname.IsSupported = SAHPI_FALSE; \
	} \
} while(0)

/**
 * snmp_bc_get_sensor_thresholds:
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
SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       SaHpiSensorThresholdsT *thres)
{
	int upper_thresholds, lower_thresholds;
	SaHpiSensorThdMaskT  found_thresholds;
	SaHpiSensorThresholdsT working;
        struct SensorInfo *sinfo;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	
	if (!hnd || !thres) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {	
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exits and has readable thresholds */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL){
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
        sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.Category != SAHPI_EC_THRESHOLD ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible == SAHPI_FALSE ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold == 0) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_CMD);
	}

        memset(&working, 0, sizeof(SaHpiSensorThresholdsT));
	found_thresholds = lower_thresholds = upper_thresholds = 0;
	
	get_threshold(SAHPI_STM_LOW_MINOR, LowMinor);
	if (found_thresholds & SAHPI_STM_LOW_MINOR) lower_thresholds++;
	get_threshold(SAHPI_STM_LOW_MAJOR, LowMajor);
	if (found_thresholds & SAHPI_STM_LOW_MAJOR) lower_thresholds++;
	get_threshold(SAHPI_STM_LOW_CRIT, LowCritical);
	if (found_thresholds & SAHPI_STM_LOW_CRIT) lower_thresholds++;
	get_threshold(SAHPI_STM_UP_MINOR, UpMinor);
	if (found_thresholds & SAHPI_STM_UP_MINOR) upper_thresholds++;
	get_threshold(SAHPI_STM_UP_MAJOR, UpMajor);
	if (found_thresholds & SAHPI_STM_UP_MAJOR) upper_thresholds++;
	get_threshold(SAHPI_STM_UP_CRIT, UpCritical);
	if (found_thresholds & SAHPI_STM_UP_CRIT) upper_thresholds++;

	/************************************************************************
	 * Find Hysteresis Values
	 *
         * Hardware can define hysteresis values two ways:
         * 
         * - As delta values as defined in the spec. In this case, 
         *   PosThdHysteresis and/or NegThdHysteresis values are defined
         *   and this routine just returns those values.
         *
         * - Total values - as in threshold is 80 degrees; positive hysteresis is
         *   defined to be 78 degrees. In this case, TotalPosThdHysteresis and/or
         *   TotalNegThdHysteresis are defined and this routine needs to make 
         *   the required calculations to return to the user a delta value. Total
         *   values can only be used if:
         *   1) if there is more thanone upper/lower threshold defined, the least
         *      critical threshold is used as the base for calculating delta values.
         *   2) Total values cannot be of type SAHPI_SENSOR_READING_TYPE_UINT64 or 
         *      SAHPI_SENSOR_READING_TYPE_BUFFER.
         *
         *   Code can support a delta value for one set of thresholds (upper or 
         *   lower) and a total value for the opposite set.
         *************************************************************************/

	if (sinfo->mib.threshold_oids.NegThdHysteresis) {
		get_threshold(SAHPI_STM_LOW_HYSTERESIS, NegThdHysteresis);
	}
	if (sinfo->mib.threshold_oids.PosThdHysteresis) {
		get_threshold(SAHPI_STM_UP_HYSTERESIS, PosThdHysteresis);
	}

	/* Negative Total Hysteresis - applies to lower thresholds */
	if (sinfo->mib.threshold_oids.TotalNegThdHysteresis) {
		SaHpiSensorReadingT reading;

		if (found_thresholds & SAHPI_STM_LOW_HYSTERESIS) {
			dbg("Cannot define both delta and total negative hysteresis. Sensor=%s",
			    rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		if (lower_thresholds == 0) {
			dbg("No lower thresholds are defined for total negative hysteresis. Sensor=%s",
			    rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		/* Get negative hysteresis value */
		SaErrorT err = snmp_bc_get_sensor_oid_reading(hnd, rid, sid,
							      sinfo->mib.threshold_oids.TotalNegThdHysteresis,
							      &reading);
		if (err) {
			snmp_bc_unlock_handler(custom_handle);
			return(err);
		}
		
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
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}
	
	/* Positive Total Hysteresis - applies to upper thresholds */
	if (sinfo->mib.threshold_oids.TotalPosThdHysteresis) {
		SaHpiSensorReadingT reading;

		if (found_thresholds & SAHPI_STM_UP_HYSTERESIS) {
			dbg("Cannot define both delta and total positive hysteresis. Sensor=%s",
			    rdr->IdString.Data);			    
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		if (upper_thresholds == 0) {
			dbg("No upper thresholds are defined for total positive hysteresis. Sensor=%s",
			    rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		/* Get positive hysteresis value */
		SaErrorT err = snmp_bc_get_sensor_oid_reading(hnd, rid, sid,
							      sinfo->mib.threshold_oids.TotalPosThdHysteresis,
							      &reading);
		if (err) {
			snmp_bc_unlock_handler(custom_handle);
			return(err);
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
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

	if (found_thresholds == 0) {
		dbg("No readable thresholds found. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	memcpy(thres, &working, sizeof(SaHpiSensorThresholdsT));
	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

#define merge_threshold(thdname) \
do { \
        if (thres->thdname.IsSupported) { \
                working.thdname.IsSupported = SAHPI_TRUE; \
                working.thdname.Type = thres->thdname.Type; \
        	switch(thres->thdname.Type) { \
        	case SAHPI_SENSOR_READING_TYPE_INT64: \
        		working.thdname.Value.SensorInt64 = thres->thdname.Value.SensorInt64; \
        		break; \
        	case SAHPI_SENSOR_READING_TYPE_FLOAT64: \
        		working.thdname.Value.SensorFloat64 = thres->thdname.Value.SensorFloat64; \
        		break; \
        	case SAHPI_SENSOR_READING_TYPE_UINT64: \
        		working.thdname.Value.SensorUint64 = thres->thdname.Value.SensorUint64; \
		        break; \
        	case SAHPI_SENSOR_READING_TYPE_BUFFER: \
        	default: \
        		dbg("Invalid threshold reading type."); \
			snmp_bc_unlock_handler(custom_handle); \
        		return(SA_ERR_HPI_INVALID_CMD); \
        	} \
        } \
} while(0)

#define write_valid_threshold(thdname) \
do { \
	if (thres->thdname.IsSupported) { \
		if (sinfo->mib.threshold_write_oids.thdname == NULL || \
		    sinfo->mib.threshold_oids.thdname[0] == '\0') { \
			dbg("No writable threshold OID defined for thdname."); \
			snmp_bc_unlock_handler(custom_handle); \
			return(SA_ERR_HPI_INTERNAL_ERROR); \
		} \
		err = snmp_bc_set_threshold_reading(hnd, rid, sid, \
					            sinfo->mib.threshold_write_oids.thdname, \
						    &(working.thdname)); \
		if (err) { \
			snmp_bc_unlock_handler(custom_handle); \
			return(err); \
		} \
	} \
} while(0)

/**
 * snmp_bc_set_sensor_thresholds:
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
SaErrorT snmp_bc_set_sensor_thresholds(void *hnd,
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
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and has writable thresholds */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	if (rdr->RdrTypeUnion.SensorRec.Category != SAHPI_EC_THRESHOLD ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible == SAHPI_FALSE ||
	    rdr->RdrTypeUnion.SensorRec.ThresholdDefn.WriteThold == 0) {
	    	snmp_bc_unlock_handler(custom_handle);
	    	return(SA_ERR_HPI_INVALID_CMD);
	}
  
	/* Overlay proposed thresholds on top of existing ones and validate */
	err = snmp_bc_get_sensor_thresholds(hnd, rid, sid, &working);
	if (err) {
		snmp_bc_unlock_handler(custom_handle);
		return(err);
	}
	
	merge_threshold(LowCritical);
	merge_threshold(LowMajor);
	merge_threshold(LowMinor);
	merge_threshold(UpCritical);
	merge_threshold(UpMajor);
	merge_threshold(UpMinor);
	merge_threshold(PosThdHysteresis);
	merge_threshold(NegThdHysteresis);
	
	err = oh_valid_thresholds(&working, rdr);
	if (err) {
		snmp_bc_unlock_handler(custom_handle);
		return(err);
	}
	
	/************************ 
	 * Write valid thresholds
         ************************/
	write_valid_threshold(UpCritical);
	write_valid_threshold(UpMajor);
	write_valid_threshold(UpMinor);
	write_valid_threshold(LowCritical);
	write_valid_threshold(LowMajor);
	write_valid_threshold(LowMinor);

	/* We don't support writing total value hysteresis only deltas */
	write_valid_threshold(NegThdHysteresis);
	write_valid_threshold(PosThdHysteresis);

	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

SaErrorT snmp_bc_get_sensor_oid_reading(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sid,
					const char *raw_oid,
					SaHpiSensorReadingT *reading)
{
	SaHpiSensorReadingT working;
	SaErrorT err;
	struct SensorInfo *sinfo;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_value get_value;
	SaHpiEntityPathT valEntity;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	/* Normalize and read sensor's raw SNMP OID */
	err = snmp_bc_validate_ep(&(rdr->Entity), &valEntity);
	err = snmp_bc_oid_snmp_get(custom_handle, &valEntity, raw_oid, &get_value, SAHPI_TRUE);
	if (err) {
		dbg("SNMP cannot read sensor OID=%s. Type=%d", raw_oid, get_value.type);
		return(err);
	}
		
	/* Convert SNMP value to HPI reading value */
	working.IsSupported = SAHPI_TRUE;
	if (get_value.type == ASN_INTEGER) {
		working.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		working.Value.SensorInt64 = (SaHpiInt64T)get_value.integer;
	} 
	else {
		SaHpiTextBufferT buffer;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, get_value.string);
		
		err = oh_encode_sensorreading(&buffer,
					      rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingType,
					      &working);
		if (err) {
			dbg("Cannot convert sensor OID=%s value=%s. Error=%s",
			    sinfo->mib.oid, buffer.Data, oh_lookup_error(err));
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}
	
	memcpy(reading, &working, sizeof(SaHpiSensorReadingT));

	return(SA_OK);
}

SaErrorT snmp_bc_set_threshold_reading(void *hnd,
				       SaHpiResourceIdT rid,
				       SaHpiSensorNumT sid,
				       const char *raw_oid,
				       const SaHpiSensorReadingT *reading)
{
	SaErrorT err;
	SaHpiTextBufferT buffer;
	SaHpiFloat64T tmp_num;
	struct SensorInfo *sinfo;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_value set_value;
	SaHpiEntityPathT valEntity;

	if (!hnd || !reading || !raw_oid) {
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

	/* Convert reading into SNMP string structure */
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

	/*************************************************************
	 * NOTE! Assuming max format for writable thresholds is ddd.dd
         *************************************************************/
 	snprintf((char *)buffer.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH, "%'+3.2f", tmp_num);

	/* Copy string to SNMP structure */
	set_value.type = ASN_OCTET_STR;
	strncpy(set_value.string, (char *)buffer.Data, buffer.DataLength);

	/* Normalize and read sensor's raw SNMP OID */
	err = snmp_bc_validate_ep(&(rdr->Entity), &valEntity);
	err = snmp_bc_oid_snmp_set(custom_handle, &valEntity, raw_oid, set_value);
	if (err) {
		dbg("SNMP cannot set sensor OID=%s.", raw_oid);
		return(err);
	}
		
	return(SA_OK);
}

/**
 * snmp_bc_get_sensor_enable:
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
SaErrorT snmp_bc_get_sensor_enable(void *hnd,
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
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->sensor_enabled;

	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * snmp_bc_set_sensor_enable:
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
SaErrorT snmp_bc_set_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sid,
				   const SaHpiBoolT enable)
{

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor enablement */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EnableCtrl == SAHPI_TRUE) {
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_enable");
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		if (sinfo->sensor_enabled != enable) {
			/* Probably need to drive an OID, if hardware supported it */
			sinfo->sensor_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
	}
	else {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * snmp_bc_get_sensor_event_enable:
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
SaErrorT snmp_bc_get_sensor_event_enable(void *hnd,
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
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->events_enabled;

	snmp_bc_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * snmp_bc_set_sensor_event_enable:
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
SaErrorT snmp_bc_set_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT rid,
					 SaHpiSensorNumT sid,
					 const SaHpiBoolT enable)
{
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!hnd ) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor event enablement */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT ||
	    rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_READ_ONLY_MASKS) {
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_event_enable\n");    
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		
		if (sinfo->events_enabled != enable) {
			/* Probably need to drive an OID, if hardware supported it */
			sinfo->events_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
	}
	else {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * snmp_bc_get_sensor_event_masks:
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
SaErrorT snmp_bc_get_sensor_event_masks(void *hnd,
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

	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}
	
	/* Check if sensor exists and return enablement status */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	} 
	
	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	if (AssertEventMask) *AssertEventMask = sinfo->assert_mask;
	
	if (DeassertEventMask) {
	        if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) {
		        *DeassertEventMask = sinfo->assert_mask;
	        } else {
		        *DeassertEventMask = sinfo->deassert_mask;	
	        }
	}

	snmp_bc_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * snmp_bc_set_sensor_event_masks:
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
SaErrorT snmp_bc_set_sensor_event_masks(void *hnd,
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
	
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and if it supports setting of sensor event masks */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT) {
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_event_masks");
                /* Probably need to drive an OID, if hardware supported it */
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		SaHpiEventStateT orig_assert_mask = sinfo->assert_mask;
		SaHpiEventStateT orig_deassert_mask = sinfo->deassert_mask;

		/* Check for invalid data in user masks */
		if ( (AssertEventMask != SAHPI_ALL_EVENT_STATES) &&
		     (AssertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) ) { 
			snmp_bc_unlock_handler(custom_handle);
			return(SA_ERR_HPI_INVALID_DATA);
		}
		if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)) {
			if  ( (DeassertEventMask != SAHPI_ALL_EVENT_STATES) &&
				(DeassertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) ) {
				snmp_bc_unlock_handler(custom_handle);
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
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_READ_ONLY);
	}

	snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}


void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiSensorReadingT *,
                             SaHpiEventStateT    *)
                __attribute__ ((weak, alias("snmp_bc_get_sensor_reading")));

void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("snmp_bc_get_sensor_thresholds")));

void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("snmp_bc_set_sensor_thresholds")));

void * oh_get_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT *)
                __attribute__ ((weak, alias("snmp_bc_get_sensor_enable")));

void * oh_set_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT)
                __attribute__ ((weak, alias("snmp_bc_set_sensor_enable")));

void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("snmp_bc_get_sensor_event_enable")));

void * oh_set_sensor_event_enables (void *, SaHpiResourceIdT id, SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("snmp_bc_set_sensor_event_enable")));

void * oh_get_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiEventStateT *, SaHpiEventStateT *)
                __attribute__ ((weak, alias("snmp_bc_get_sensor_event_masks")));

void * oh_set_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiSensorEventMaskActionT,
                                  SaHpiEventStateT,
                                  SaHpiEventStateT)
                __attribute__ ((weak, alias("snmp_bc_set_sensor_event_masks")));


