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

SaErrorT snmp_bc_get_sensor_data(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiSensorNumT num,
				 SaHpiSensorReadingT *data)
{
        gchar *oid=NULL;
	SaHpiSensorReadingT working;
        struct snmp_value get_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num);
	if(rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
	struct BC_SensorInfo *s =
                (struct BC_SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
 	if(s == NULL) {
		return -1;
	}       

	if (rdr->RdrTypeUnion.SensorRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	memset(&working, 0, sizeof(SaHpiSensorReadingT));

	/* Extract index from rdr id and get the snmp of the sensor */
	if (s->mib.oid != NULL) {
		oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
		if(oid == NULL) {
			dbg("NULL SNMP OID returned for %s\n",s->mib.oid);
			return -1;
		}

		/* Read the sensor value */
		if(snmp_get(custom_handle->ss, oid, &get_value) != 0){
			dbg("SNMP could not read sensor %s. Type = %d",oid,get_value.type);
			g_free(oid);
			return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);

		/* Based on the sensor description, construct a reading to send up */
		/* format the value into the reading for each type of reading format */
		working.ValuesPresent = rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingFormats;
		if(working.ValuesPresent & SAHPI_SRF_RAW) {
			if(get_value.type != ASN_INTEGER) {
				dbg("Sensor value type mismatches reading format.");
				return -1;
			} else {
				working.Raw = (SaHpiUint32T)get_value.integer;
			}
		}

		if(working.ValuesPresent & SAHPI_SRF_INTERPRETED) {
			if(get_value.type == ASN_INTEGER) {
				working.Interpreted.Type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
				working.Interpreted.Value.SensorInt32 = get_value.integer;
			} else {
				SaHpiSensorInterpretedUnionT value;
				
				working.Interpreted.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.Interpreted.Type;
				if(rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.Interpreted.Type == SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER) {
					strncpy(working.Interpreted.Value.SensorBuffer,
						get_value.string,
                                                SAHPI_SENSOR_BUFFER_LENGTH);
				} else {
					if(s->mib.convert_snmpstr >= 0) {
						SaHpiTextBufferT buffer;
						
						oh_init_textbuffer(&buffer);
						oh_append_textbuffer(&buffer, get_value.string);
						
						if (oh_encode_sensorreading(&buffer, s->mib.convert_snmpstr, &value)) {
							dbg("Error: oh_encode_sensorreading for %s, (%s)\n",s->mib.oid, buffer.Data;
							return -1;
						}
						working.Interpreted.Value = value;
					} else {
						dbg("Sensor %s SNMP string value needs to be converted\n", s->mib.oid);
						return -1;
					}
				}
			}
		}
	}

        if (working.ValuesPresent & SAHPI_SRF_EVENT_STATE) {

		working.EventStatus.SensorStatus = s->sensor_evt_enablement.SensorStatus;

		/* Hardcoded hack for R/W LEDs */
		if (working.ValuesPresent & SAHPI_SRF_RAW) {
			if (rdr->RdrTypeUnion.SensorRec.Category == SAHPI_EC_USAGE) {
				switch (working.Raw) {
				case 0:
					working.EventStatus.EventStatus = 
						rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Min.EventStatus.EventStatus;
					break;
				case 1:
					working.EventStatus.EventStatus = 
						rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Nominal.EventStatus.EventStatus;
					break;	
				case 2:
					working.EventStatus.EventStatus = 
						rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.EventStatus.EventStatus;
					break;
				default:
					dbg("Unrecognized Raw values for LED=%s", rdr->IdString.Data);
					return -1;
				}
			}
			else {
				switch (working.Raw) {
				case 0:
					working.EventStatus.EventStatus = 
						rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Min.EventStatus.EventStatus;
					break;
				case 1:
					working.EventStatus.EventStatus = 
						rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.EventStatus.EventStatus;
					break;	
				default:
					dbg("Unrecognized Raw values for LED=%s", rdr->IdString.Data);
					return -1;
				}
			}
		}
		else { /* Non-LED sensor - normal case */
                        working.EventStatus.EventStatus = s->cur_state;
		}
        }

	memcpy(data,&working,sizeof(SaHpiSensorReadingT));
        
        return SA_OK;
}

#define get_raw_thresholds(thdmask, thdoid, thdname) \
do { \
        if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold & thdmask) { \
	        if(s->mib.threshold_oids.RawThresholds.thdoid != NULL && s->mib.threshold_oids.RawThresholds.thdoid[0] != '\0') { \
	                oid = snmp_derive_objid(rdr->Entity,s->mib.threshold_oids.RawThresholds.thdoid); \
                        if(oid == NULL) { \
                                 dbg("NULL SNMP OID returned for %s\n",s->mib.threshold_oids.RawThresholds.thdoid); \
                                 return -1; \
                        } \
	                if((snmp_get(custom_handle->ss, oid, &get_value) != 0) | \
	                   (get_value.type != ASN_INTEGER)) { \
		                dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); \
		                g_free(oid); \
		                return SA_ERR_HPI_NO_RESPONSE; \
	                } \
	                g_free(oid); \
	                found_raw = found_raw | thdmask; \
	                working.thdname.Raw = get_value.integer; \
	                working.thdname.ValuesPresent = working.thdname.ValuesPresent | SAHPI_SRF_RAW; \
	        } else { \
		        dbg("Raw threshold defined as readable but no OID defined\n"); \
	        } \
        } \
} while(0)

#define get_interpreted_thresholds(thdmask, thdoid, thdname) \
do { \
        if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold & thdmask) { \
	        if(s->mib.threshold_oids.InterpretedThresholds.thdoid != NULL && s->mib.threshold_oids.InterpretedThresholds.thdoid[0] != '\0') { \
		        oid = snmp_derive_objid(rdr->Entity,s->mib.threshold_oids.InterpretedThresholds.thdoid); \
                        if(oid == NULL) { \
                                dbg("NULL SNMP OID returned for %s\n",s->mib.threshold_oids.InterpretedThresholds.thdoid); \
                                return -1; \
                        } \
	         	if((snmp_get(custom_handle->ss, oid, &get_value) != 0) | \
	                   !((get_value.type == ASN_INTEGER) | (get_value.type == ASN_OCTET_STR))) { \
			        dbg("SNMP could not read %s; Type=%d.\n",oid,get_value.type); \
			        g_free(oid); \
			        return SA_ERR_HPI_NO_RESPONSE; \
		        } \
                        found_interpreted = found_interpreted | thdmask; \
		        /* Means we always need to define this field in bc_resources.h */ \
		        working.thdname.Interpreted.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.Interpreted.Type; \
		        working.thdname.ValuesPresent = working.thdname.ValuesPresent | SAHPI_SRF_INTERPRETED; \
		        if(get_value.type == ASN_INTEGER) { \
			         working.thdname.Interpreted.Value.SensorInt32 = get_value.integer; \
		        } else if(get_value.type == ASN_OCTET_STR && s->mib.convert_snmpstr >= 0) { \
                                SaHpiTextBufferT buffer; \
                                oh_init_textbuffer(&buffer); \
                                oh_append_textbuffer(&buffer, get_value.string); \
			        if(oh_encode_sensorreading(&buffer, s->mib.convert_snmpstr, &value)) { \
				        dbg("Error: bad return oh_encode_sensorreading for %s\n",oid); \
                                        g_free(oid); \
				        return -1; \
			        } \
			        working.thdname.Interpreted.Value = value; \
		        } else { \
			        dbg("%s threshold is string but no conversion defined\n",oid); \
                                g_free(oid); \
			        return -1; \
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
         working.thdname.Interpreted.Value.SensorFloat32 = working.thdmatch.Interpreted.Value.SensorFloat32 - \
                                                           working.thdname.Interpreted.Value.SensorFloat32; \
         if (working.thdname.Interpreted.Value.SensorFloat32 < 0) { \
                dbg("Positive Hysteresis delta is less than 0"); \
         	working.thdname.ValuesPresent = 0; \
         } \
} while(0)

#define get_low_hysteresis_value(thdname, thdmatch) \
do { \
         working.thdname.Interpreted.Value.SensorFloat32 = working.thdname.Interpreted.Value.SensorFloat32 - \
                                                           working.thdmatch.Interpreted.Value.SensorFloat32; \
         if (working.thdname.Interpreted.Value.SensorFloat32 < 0) { \
                dbg("Negitive Hysteresis delta is less than 0"); \
         	working.thdname.ValuesPresent = 0; \
         } \
} while(0)

SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       SaHpiSensorThresholdsT *thres)
{
        gchar *oid = NULL;
	SaHpiUint8T found_raw, found_interpreted;
	SaHpiSensorThresholdsT working;
	SaHpiSensorInterpretedUnionT value;
        struct snmp_value get_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_SENSOR_RDR, num);
	if(rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
        struct BC_SensorInfo *s =
                (struct BC_SensorInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
 	if(s == NULL) {
		return -1;
	}

	if (rdr->RdrTypeUnion.SensorRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

        memset(&working, 0, sizeof(SaHpiSensorThresholdsT));

	if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.IsThreshold == SAHPI_TRUE) {
		found_raw = found_interpreted = 0;
		if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_STC_RAW) {
			get_raw_thresholds(SAHPI_STM_LOW_MINOR, OidLowMinor, LowMinor);
			get_raw_thresholds(SAHPI_STM_LOW_MAJOR, OidLowMajor, LowMajor);
			get_raw_thresholds(SAHPI_STM_LOW_CRIT, OidLowCrit, LowCritical);
			get_raw_thresholds(SAHPI_STM_UP_MINOR, OidUpMinor, UpMinor);
			get_raw_thresholds(SAHPI_STM_UP_MAJOR, OidUpMajor, UpMajor);
			get_raw_thresholds(SAHPI_STM_UP_CRIT, OidUpCrit, UpCritical);
			/* Ignore any PosThdHysteresis and NegThdHysteresis for RAW
			   (going away in 1.1) */
		}

		if(rdr->RdrTypeUnion.SensorRec.ThresholdDefn.TholdCapabilities & SAHPI_STC_INTERPRETED) {
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
				dbg("Positive Hysteresis is defined but not any positive thresholds");
				working.PosThdHysteresis.ValuesPresent = 0;
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
				dbg("Negitive Hysteresis is defined but not any negitive thresholds");
				working.NegThdHysteresis.ValuesPresent = 0;
			}    
		}			    

		if (found_raw || found_interpreted) {
			memcpy(thres, &working, sizeof(SaHpiSensorThresholdsT));
			return SA_OK;
		} else {
			dbg("No threshold values found\n");
			return -1;
		}
        } else {
                dbg("Thresholds requested, but sensor does not support them.\n");
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

SaErrorT snmp_bc_get_sensor_event_enables(void *hnd,
					  SaHpiResourceIdT id,
					  SaHpiSensorNumT num,
					  SaHpiBoolT *enables)
{
	SaHpiSensorEvtEnablesT working;

	SaHpiRdrT *rdr = oh_get_rdr_by_type(((struct oh_handler_state *)hnd)->rptcache, 
					    id, SAHPI_SENSOR_RDR, num);
	if (rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
	gpointer bc_data = oh_get_rdr_data(
		((struct oh_handler_state *)hnd)->rptcache, id, rdr->RecordId);
	if (bc_data == NULL) {
		dbg("Sensor Data Pointer is NULL; RID=%x; SID=%d", id, num); 
		return -1;
	}

	if (rdr->RdrTypeUnion.SensorRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_NO_EVENTS) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	working = ((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement;

	memcpy(enables, &working, sizeof(SaHpiSensorEvtEnablesT));

        return SA_OK;
}

SaErrorT snmp_bc_set_sensor_event_enables(void *hnd,
					  SaHpiResourceIdT id,
					  SaHpiSensorNumT num,
					  const SaHpiBoolT *enables)
{
	SaHpiRdrT *rdr = oh_get_rdr_by_type(((struct oh_handler_state *)hnd)->rptcache, 
					    id, SAHPI_SENSOR_RDR, num);
	if (rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
	
	gpointer bc_data = oh_get_rdr_data(((struct oh_handler_state *)hnd)->rptcache, id, rdr->RecordId);
	if (bc_data == NULL) {
		dbg("Sensor Data Pointer is NULL; RID=%x; SID=%d", id, num); 
		return -1;
	}
		
	if (rdr->RdrTypeUnion.SensorRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	if (rdr->RdrTypeUnion.SensorRec.EventCtrl == SAHPI_SEC_NO_EVENTS) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	if ((enables->SensorStatus & SAHPI_SENSTAT_SCAN_ENABLED) ||
	    (enables->SensorStatus & SAHPI_SENSTAT_BUSY)) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	/* 
	 * BC currently does not support enabling/disabling individual events - 
         * just the entire sensor
         */

	((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement = *enables;

        return SA_OK;
}
