/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
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

SaErrorT snmp_bc_get_sensor_reading(void *hnd,
				    SaHpiResourceIdT rid,
				    SaHpiSensorNumT sid,
				    SaHpiSensorReadingT *data,
				    SaHpiEventStateT *state)
{
	SaErrorT err = SA_OK;
        gchar *oid = NULL;
	SaHpiSensorReadingT working;
        struct snmp_value get_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;

	struct SensorInfo *s =
                (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (s == NULL) {
		dbg("Can not retrieve BC_Sensor Info from rptcache.\n");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}       

	memset(&working, 0, sizeof(SaHpiSensorReadingT));
	working.IsSupported = SAHPI_FALSE;
	
	/* Extract index from rdr id and get the snmp of the sensor */
	if ((s->mib.oid != NULL) && 
	    (rdr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE))
	{
		oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
		if (oid == NULL) {
			dbg("NULL SNMP OID returned for %s\n",s->mib.oid);
			return SA_ERR_HPI_INTERNAL_ERROR;
		}

		/* Read the sensor value */
		if (snmp_get(custom_handle->ss, oid, &get_value) != 0){
			dbg("SNMP could not read sensor %s. Type = %d", oid, get_value.type);
			g_free(oid);
			return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);

		working.IsSupported = SAHPI_TRUE;
		working.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingType;

		if (get_value.type == ASN_INTEGER) {
			working.Value.SensorUint64 = (SaHpiUint64T) get_value.integer;
		} else {
			if (s->mib.convert_snmpstr >= 0) {
				SaHpiTextBufferT buffer;
				SaHpiSensorReadingT tmpvalue;
						
				oh_init_textbuffer(&buffer);
				oh_append_textbuffer(&buffer, get_value.string);
						
				if (oh_encode_sensorreading(&buffer, s->mib.convert_snmpstr, &tmpvalue)) {
					dbg("Error: oh_encode_sensorreading for %s, (%s)\n", s->mib.oid, buffer.Data);
					return SA_ERR_HPI_INTERNAL_ERROR;
				}
				working = tmpvalue;		
			} else {
				trace("Sensor %s SNMP string value needs to be converted\n", s->mib.oid);
				working.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
				strncpy(working.Value.SensorBuffer,
					get_value.string,
					SAHPI_SENSOR_BUFFER_LENGTH);
			}

		}  		
			
	}
	
	/* NULL is a valid value for data, need to check before use */
	if (data)
		memcpy(data, &working, sizeof(SaHpiSensorReadingT));
		
	/* NUL is a valid value for state, need to check before use */
	if (state)
		err = snmp_bc_determine_sensor_eventstates(hnd, rid, sid, &working, state); 
        
        return SA_OK;
}

/*
 *
 */
SaErrorT snmp_bc_determine_sensor_eventstates(void *hnd,
					      SaHpiResourceIdT id,
					      SaHpiSensorNumT num,
					      SaHpiSensorReadingT *reading,
					      SaHpiEventStateT *state)	
{
	SaErrorT err = SA_OK;
	SaHpiEventStateT thisEventStates = 0;
	SaHpiSensorThresholdsT thres;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;	
	
	if (!hnd || !reading || !state)
		return SA_ERR_HPI_INTERNAL_ERROR;

	SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num);
	if(rdr == NULL) 
		return SA_ERR_HPI_NOT_PRESENT;
		
	memset(&thres, 0, sizeof(SaHpiSensorThresholdsT));
	
	if (rdr->RdrTypeUnion.SensorRec.Events != 0) {
		switch(rdr->RdrTypeUnion.SensorRec.Category) {
			case SAHPI_EC_THRESHOLD:
				err = snmp_bc_get_sensor_thresholds(hnd, id, num, &thres);
				if (err == SA_OK) {
					if (memcmp(&reading->Value, &thres.LowCritical.Value,
								 sizeof(SaHpiSensorReadingUnionT)) <= 0)
						thisEventStates |= SAHPI_ES_LOWER_CRIT;	 
					if (memcmp(&reading->Value, &thres.LowMajor.Value,
								 sizeof(SaHpiSensorReadingUnionT)) <= 0)
						thisEventStates |= SAHPI_ES_LOWER_MAJOR;	 
					if (memcmp(&reading->Value, &thres.LowMinor.Value,
								 sizeof(SaHpiSensorReadingUnionT)) <= 0)
						thisEventStates |= SAHPI_ES_LOWER_MINOR;	 
					if (memcmp(&reading->Value, &thres.UpCritical.Value,
								 sizeof(SaHpiSensorReadingUnionT)) >= 0)
						thisEventStates |= SAHPI_ES_UPPER_CRIT;	 
					if (memcmp(&reading->Value, &thres.UpMajor.Value,
								 sizeof(SaHpiSensorReadingUnionT)) >= 0)					
						thisEventStates |= SAHPI_ES_UPPER_MAJOR;	 
					if (memcmp(&reading->Value, &thres.UpMinor.Value,
								 sizeof(SaHpiSensorReadingUnionT)) >= 0)					
						thisEventStates |= SAHPI_ES_UPPER_MINOR;
				}				
				break;
			case SAHPI_EC_SEVERITY:
				break;
			case SAHPI_EC_PRED_FAIL:
				break;
			case SAHPI_EC_AVAILABILITY:
				break;
			case SAHPI_EC_REDUNDANCY:
				break; 
			case SAHPI_EC_USAGE:
				break;
			default:
				break;
		}
	}
	
	*state = thisEventStates;
	return(err);
}   

#define get_interpreted_thresholds(thdmask, thdoid, thdname) \
do { \
        if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold & thdmask) { \
	        if(s->mib.threshold_oids.InterpretedThresholds.thdoid != NULL && s->mib.threshold_oids.InterpretedThresholds.thdoid[0] != '\0') { \
		        oid = snmp_derive_objid(rdr->Entity,s->mib.threshold_oids.InterpretedThresholds.thdoid); \
                        if(oid == NULL) { \
                                dbg("NULL SNMP OID returned for %s\n",s->mib.threshold_oids.InterpretedThresholds.thdoid); \
                                return SA_ERR_HPI_INTERNAL_ERROR; \
                        } \
	         	if((snmp_get(custom_handle->ss, oid, &get_value) != 0) | \
	                   !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { \
			        dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); \
			        g_free(oid); \
			        return SA_ERR_HPI_NO_RESPONSE; \
		        } \
			dbg("thdmask %x\n", thdmask); \
                        found_interpreted = found_interpreted | thdmask; \
		        /* Means we always need to define this field in bc_resources.h */ \
		        working.thdname.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.Type; \
		        working.thdname.IsSupported = SAHPI_TRUE; \
		        if(get_value.type == ASN_INTEGER) { \
			         working.thdname.Value.SensorInt64 = (SaHpiInt64T) get_value.integer; \
		        } else if(get_value.type == ASN_OCTET_STR && s->mib.convert_snmpstr >= 0) { \
                                SaHpiTextBufferT buffer; \
                                oh_init_textbuffer(&buffer); \
                                oh_append_textbuffer(&buffer, get_value.string); \
			        if(oh_encode_sensorreading(&buffer, s->mib.convert_snmpstr, &value)) { \
				        dbg("Error: bad return oh_encode_sensorreading for %s\n",oid); \
                                        g_free(oid); \
				        return SA_ERR_HPI_INTERNAL_ERROR; \
			        } \
			        working.thdname = value; \
		        } else { \
			        dbg("%s threshold is string but no conversion defined\n",oid); \
                                g_free(oid); \
			        return SA_ERR_HPI_INTERNAL_ERROR; \
		        } \
                        g_free(oid); \
	        } else { \
		        dbg("Interpreted threshold defined as readable but no OID defined\n"); \
	        } \
        } \
} while(0)


/* Assuming floating point - not generic but works for BladeCenter */
#define get_up_hysteresis_value(thdname, thdmatch) \
do { \
         working.thdname.Value.SensorFloat64 = working.thdmatch.Value.SensorFloat64 - \
                                                           working.thdname.Value.SensorFloat64; \
         if (working.thdname.Value.SensorFloat64 < 0) { \
                dbg("Positive Hysteresis delta is less than 0"); \
         	working.thdname.IsSupported = SAHPI_FALSE; \
         } \
} while(0)

#define get_low_hysteresis_value(thdname, thdmatch) \
do { \
         working.thdname.Value.SensorFloat64 = working.thdname.Value.SensorFloat64 - \
                                                           working.thdmatch.Value.SensorFloat64; \
         if (working.thdname.Value.SensorFloat64 < 0) { \
                dbg("Negative Hysteresis delta is less than 0"); \
         	working.thdname.IsSupported = SAHPI_FALSE; \
         } \
} while(0)



SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       SaHpiSensorThresholdsT *thres)
{
        gchar *oid = NULL;
	SaHpiUint8T  found_interpreted;
	SaHpiSensorThresholdsT working;
	SaHpiSensorReadingT value;
        struct snmp_value get_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num);
	if (rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
        struct SensorInfo *s =
                (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
 	if (s == NULL) {
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

        memset(&working, 0, sizeof(SaHpiSensorThresholdsT));

	if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible == SAHPI_TRUE) {
		found_interpreted = 0;

		get_interpreted_thresholds(SAHPI_STM_LOW_MINOR, OidLowMinor, LowMinor);
		get_interpreted_thresholds(SAHPI_STM_LOW_MAJOR, OidLowMajor, LowMajor);
		get_interpreted_thresholds(SAHPI_STM_LOW_CRIT, OidLowCrit, LowCritical);
		get_interpreted_thresholds(SAHPI_STM_UP_MINOR, OidUpMinor, UpMinor);
		get_interpreted_thresholds(SAHPI_STM_UP_MAJOR, OidUpMajor, UpMajor);
		get_interpreted_thresholds(SAHPI_STM_UP_CRIT, OidUpCrit, UpCritical);
		get_interpreted_thresholds(SAHPI_STM_UP_HYSTERESIS, OidUpHysteresis, PosThdHysteresis);
		get_interpreted_thresholds(SAHPI_STM_LOW_HYSTERESIS, OidLowHysteresis, NegThdHysteresis);

		/* Hysteresis - BladeCenter supports a reset value for some single threshold
		 * sensors. So there may be a major threshold of 78 and a reset of 76. This
                 * extremely ugly code, calculates the delta to report hysteresis. 
		 * If multiple thresholds are defined, the most severe one is used as the 
		 * basis for the delta calculation. Also we assume all values are float32 */
		if ((found_interpreted & SAHPI_STM_UP_HYSTERESIS) && 
		    ((found_interpreted & SAHPI_STM_UP_CRIT) || 
		    (found_interpreted & SAHPI_STM_UP_MAJOR) ||
		    (found_interpreted & SAHPI_STM_UP_MINOR))) {
			if (found_interpreted & SAHPI_STM_UP_CRIT) {
				get_up_hysteresis_value(PosThdHysteresis, UpCritical);
			}
			else {
				if (found_interpreted & SAHPI_STM_UP_MAJOR) {
					get_up_hysteresis_value(PosThdHysteresis, UpMajor);	
				}
				else {
					if (found_interpreted & SAHPI_STM_UP_MINOR) {
						get_up_hysteresis_value(PosThdHysteresis, UpMinor);
					}	
				}	
			}
		}
		else {
			trace("Positive Hysteresis is defined but not any positive thresholds");
			working.PosThdHysteresis.IsSupported = SAHPI_FALSE;
		}    
			
		/* Negitive hysteresis */
		if ((found_interpreted & SAHPI_STM_LOW_HYSTERESIS) && 
			   ((found_interpreted & SAHPI_STM_LOW_CRIT) || 
			   (found_interpreted & SAHPI_STM_LOW_MAJOR) ||
			   (found_interpreted & SAHPI_STM_LOW_MINOR))) {
			if (found_interpreted & SAHPI_STM_LOW_CRIT) {
				get_low_hysteresis_value(NegThdHysteresis, LowCritical);
			}
			else {
				if (found_interpreted & SAHPI_STM_LOW_MAJOR) {
					get_low_hysteresis_value(NegThdHysteresis, LowMajor);	
				}
				else {
					if (found_interpreted & SAHPI_STM_LOW_MINOR) {
						get_low_hysteresis_value(NegThdHysteresis, LowMinor);
					}	
				}	
			}
		}
		else {
			trace("Negative Hysteresis is defined but not any negitive thresholds");
			working.NegThdHysteresis.IsSupported = SAHPI_FALSE;
		}    
			    
		if (found_interpreted) {
			memcpy(thres, &working, sizeof(SaHpiSensorThresholdsT));
			return SA_OK;
		} else {
			dbg("No threshold values found\n");
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
       } else {
		trace("Thresholds requested, but sensor does not support them.\n");
		return SA_ERR_HPI_INVALID_CMD;
       }        
}


SaErrorT snmp_bc_set_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       const SaHpiSensorThresholdsT *thres)
{
	/* BC does not support writable thresholds
	   Check Ignore bit and make sure thresholds are writable if this is added */
        return SA_ERR_HPI_INVALID_CMD;
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
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_PARAMS - if @enable NULL.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_get_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sid,
				   SaHpiBoolT *enable)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!enable) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and return enablement status */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);

	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("Cannot retrieve sensor data.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->sensor_enabled;

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
 * SA_OK - normal case.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_set_sensor_enable(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiSensorNumT sid,
				   const SaHpiBoolT enable)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and if it supports setting of sensor enablement */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	if (rdr->RdrTypeUnion.SensorRec.EnableCtrl == SAHPI_TRUE) {
#if 1
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_enable");
		return(SA_ERR_HPI_INTERNAL_ERROR);
#else /* Not tested */
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("Cannot retrieve sensor data.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		if (sinfo->sensor_enabled != enable) {
			sinfo->sensor_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
#endif
	}
	else {
		return(SA_ERR_HPI_READ_ONLY);
	}

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
 * SA_OK - normal case.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_get_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT rid,
					 SaHpiSensorNumT sid,
					 SaHpiBoolT *enable)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!enable) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and return enablement status */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);

	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("Cannot retrieve sensor data.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	*enable = sinfo->events_enabled;

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
 * SA_OK - normal case.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_set_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT rid,
					 SaHpiSensorNumT sid,
					 const SaHpiBoolT enable)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and if it supports setting of sensor event enablement */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT ||
	    rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_READ_ONLY_MASKS) {
#if 1
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_event_enable");
		return(SA_ERR_HPI_INTERNAL_ERROR);
#else /* Not tested */
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("Cannot retrieve sensor data.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		
		if (sinfo->events_enabled != enable) {
			sinfo->events_enabled = enable;
			/* FIXME:: Add SAHPI_ET_SENSOR_ENABLE_CHANGE event on IF event Q */
		}
#endif
	}
	else {
		return(SA_ERR_HPI_READ_ONLY);
	}

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
 * SA_OK - normal case.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_get_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sid,
					SaHpiEventStateT *AssertEventMask,
					SaHpiEventStateT *DeassertEventMask)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct SensorInfo *sinfo;

	if (!AssertEventMask || !DeassertEventMask) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and return enablement status */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);

	sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
 	if (sinfo == NULL) {
		dbg("Cannot retrieve sensor data.");
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	*AssertEventMask = sinfo->assert_mask;
	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) {
		*DeassertEventMask = sinfo->assert_mask;
	}
	else {
		*DeassertEventMask = sinfo->deassert_mask;	
	}

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
 * SA_OK - normal case.
 * SA_ERR_HPI_CAPABILITY - if resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_DATA - if @act not valid or @AssertEventMask/@DeassertEventMask
 *                           contain events not supported by sensor. 
 * SA_ERR_HPI_NOT_PRESENT - if sensor doesn't exist.
 **/
SaErrorT snmp_bc_set_sensor_event_masks(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSensorNumT sid,
					SaHpiSensorEventMaskActionT act,
					const SaHpiEventStateT AssertEventMask,
					const SaHpiEventStateT DeassertEventMask)
{
	SaHpiRdrT *rdr;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (oh_lookup_sensoreventmaskaction(act) == NULL) {
		return(SA_ERR_HPI_INVALID_DATA);
	}

	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) return(SA_ERR_HPI_INVALID_RESOURCE);
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) return(SA_ERR_HPI_CAPABILITY);

	/* Check if sensor exists and if it supports setting of sensor event masks */
        rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_SENSOR_RDR, sid);
	if (rdr == NULL) return(SA_ERR_HPI_NOT_PRESENT);
	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_PER_EVENT) {
#if 1
		dbg("BladeCenter/RSA do not support snmp_bc_set_sensor_event_masks");
		return(SA_ERR_HPI_INTERNAL_ERROR);
#else /* Not tested */
		struct SensorInfo *sinfo;
		sinfo = (struct SensorInfo *)oh_get_rdr_data(handle->rptcache, rid, rdr->RecordId);
		if (sinfo == NULL) {
			dbg("Cannot retrieve sensor data.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		SaHpiEventStateT orig_assert_mask = sinfo->assert_mask;
		SaHpiEventStateT orig_deassert_mask = sinfo->deassert_mask;

		/* Check for invalid data in user masks */
		if (AssertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) return(SA_ERR_HPI_INVALID_DATA);
		if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)) {
			if  (DeassertEventMask & ~(rdr->RdrTypeUnion.SensorRec.Events)) {
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
#endif
	}
	else {
		return(SA_ERR_HPI_READ_ONLY);
	}

	return(SA_OK);
}
