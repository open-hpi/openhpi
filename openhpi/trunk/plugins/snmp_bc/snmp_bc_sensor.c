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

#include <glib.h>
#include <SaHpi.h>

#include <openhpi.h>
#include <oh_plugin.h>
#include <snmp_util.h>

#include <bc_resources.h>
#include <snmp_bc.h>
#include <snmp_bc_utils.h>
#include <snmp_bc_sensor.h>

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
						if(get_interpreted_value(get_value.string,s->mib.convert_snmpstr,&value)) {
							dbg("Error: get_interpreted_value for %s, (%s)\n",s->mib.oid,get_value.string);
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
	                found_raw++; \
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
		        found_interpreted++; \
		        /* Means we always need to define this field in bc_resources.h */ \
		        working.thdname.Interpreted.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.Range.Max.Interpreted.Type; \
		        working.thdname.ValuesPresent = working.thdname.ValuesPresent | SAHPI_SRF_INTERPRETED; \
		        if(get_value.type == ASN_INTEGER) { \
			         working.thdname.Interpreted.Value.SensorInt32 = get_value.integer; \
		        } else if(get_value.type == ASN_OCTET_STR && s->mib.convert_snmpstr >= 0) { \
			        if(get_interpreted_value(get_value.string,s->mib.convert_snmpstr,&value)) { \
				        dbg("Error: bad return from get_interpreted_value for %s\n",oid); \
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

SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       SaHpiSensorThresholdsT *thres)
{
        gchar *oid = NULL;
	int  found_raw, found_interpreted;
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

			/* FIXME:: Add PosThdHysteresis and NegThdHysteresis */			 	
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
					  SaHpiSensorEvtEnablesT *enables)
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
					  const SaHpiSensorEvtEnablesT *enables)
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
	 * Ignore the assertion/deassert event states in enables, since BC does not
         * support enabling/disabling individual events - just the entire sensor
         */

	if (!(enables->SensorStatus & SAHPI_SENSTAT_EVENTS_ENABLED)) {
		/* turn off events in RDR's data */
		((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement.SensorStatus =
			((struct BC_SensorInfo *)bc_data)->sensor_evt_enablement.SensorStatus
			& ~SAHPI_SENSTAT_EVENTS_ENABLED;
	}

        return SA_OK;
}
