/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Renier Morales <renierm@users.sf.net>
 *        Daniel de Araujo <ddearauj@us.ibm.com>
 */
 

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <rtas_discover.h>
#include <rtas_utils.h>



SaErrorT rtas_discover_resources(void *hnd)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_discover_domain_resources(void *hnd, SaHpiDomainIdT did)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

/**
 * rtas_discover_sensors: 
 * @handler: Pointer to handler's data.
 * @parent_res_event: Pointer to resource's event structure.
 *
 * Discovers resource's available sensors and its events.
 *
 * Return values:
 * Adds sensor RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - incoming parameters are NULL.
 **/
SaErrorT rtas_discover_sensors(struct oh_handler_state *handle,
			       struct oh_event *parent_res_event)

{
	int file, sensor_num = 0;
        //char err_buf[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiUint32T token, max_index, index = 0;
	SaHpiInt32T val, state;
	struct SensorInfo *sensor_info;

	struct oh_event * event;

	if (!handle || !parent_res_event)
	{
		return SA_ERR_HPI_INVALID_PARAMS;
	}	
	
	/* open the binary file and read the indexes */
	file = open(RTAS_SENSORS_PATH, O_RDONLY);
	
	/* The rtas-sensors file is layed out in the following fashion :
	 * 
	 * 	32-bit-integer-token 32-bit-integer-index ...
	 *
	 * As a result, we read the first 32-bit chunk to establish 
	 * the type of sensor is in the system.  We then read the next
	 * 32-bit chunk to determine how many sensors exist of type
	 * token.  This process repeats until we've reached EOL. 
	 * Note, sensor indices are not necessarily contiguous.
	 */
	while (read(file, (char *)&token, 
	                     sizeof(SaHpiUint32T)) == sizeof(SaHpiUint32T)) {
		
		if (read(file, (char *)&max_index, 
		             sizeof(SaHpiUint32T)) != sizeof(SaHpiUint32T));
			{
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
	
		        
		/* Make sure we didn't get bogus tokens */
		if (    token != RTAS_RESERVED_SENSOR_2  &&
			token != RTAS_RESERVED_SENSOR_4  &&
			token != RTAS_RESERVED_SENSOR_5  &&
			token != RTAS_RESERVED_SENSOR_6  &&
			token != RTAS_RESERVED_SENSOR_7  &&
			token != RTAS_RESERVED_SENSOR_8  &&
			token != RTAS_RESERVED_SENSOR_10 &&
			token != RTAS_RESERVED_SENSOR_11 &&
			token != RTAS_RESERVED_SENSOR_9008 )
		{
			/* Now that we have the token and the index, we can
			 * grab the specific data about the sensor  and build the RDR.
			 */
			for (index = 0; index <= max_index; index++) {
				
				
				/* We cannot assume that there are sensors in each index leading up
				 * up the maxIndex.  We must determine whether the sensor actually
				 * exists.  We do this by calling rtas_get_sensor.  Note: A return
				 * value of "-3" means the sensor does not exist. 
				 */
				 	 
				state = rtas_get_sensor(token, index, &val);
				
				if (state != -3) {
				
					event = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
					sensor_info = (struct SensorInfo*)g_malloc0(sizeof(struct SensorInfo));
		
					if (!event || !sensor_info) {
						dbg("Out of memory.");
						return SA_ERR_HPI_OUT_OF_SPACE;
					}
				
					event->type = OH_ET_RDR;
					event->did  = oh_get_default_domain_id();
					
					event->u.rdr_event.parent      = parent_res_event->u.res_event.entry.ResourceId;
					event->u.rdr_event.rdr.RdrType = SAHPI_SENSOR_RDR;
					event->u.rdr_event.rdr.Entity  = parent_res_event->u.res_event.entry.ResourceEntity;
				
					/* Do entity path business */
					//rtas_modify_sensor_ep(); //needs more research
				
					/* For now, assume sensor number represents a count.  If we decide later to 
					 * create an RPT for each sensor type (and fill in the RDRs that consist of
					 * the sensor type), then the num will need to be reset.
					 */
         				event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Num = sensor_num++;
				
					populate_rtas_sensor_rec_info(token, &(event->u.rdr_event.rdr.RdrTypeUnion.SensorRec));
			 	     
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Category   = SAHPI_EC_THRESHOLD | SAHPI_EC_SEVERITY;					 
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.EventCtrl  = SAHPI_SEC_READ_ONLY;  
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Events	 = SAHPI_ES_OK | SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_CRIT | SAHPI_ES_UPPER_MINOR |SAHPI_ES_UPPER_CRIT;
				    				
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.ThresholdDefn.IsAccessible = SAHPI_FALSE;
					
					sensor_info->token = token;
					sensor_info->index = index;

					sensor_info->enabled = SAHPI_TRUE;
					
					if ( (token == RTAS_SECURITY_SENSOR) ||
					     (token == RTAS_SURVEILLANCE_SENSOR) ) {
					     
					     	if (val == 0) { sensor_info->enabled = SAHPI_FALSE; }
					
					}
							
					sensor_info->current_val = val;
				
					switch ((rtasSensorState)state) {
		
						case SENSOR_OK: 
							sensor_info->current_state = SAHPI_ES_UNSPECIFIED; 
							break; 
				
						case SENSOR_CRITICAL_LOW:
							sensor_info->current_state = SAHPI_ES_LOWER_CRIT;
							break; 	
			
						case SENSOR_WARNING_LOW:
							sensor_info->current_state = SAHPI_ES_LOWER_MINOR;
							break; 
				
						case SENSOR_NORMAL:
							sensor_info->current_state = SAHPI_ES_OK; 
							break; 
				
						case SENSOR_WARNING_HIGH: 
							sensor_info->current_state = SAHPI_ES_UPPER_MINOR; 
							break; 
				
						case SENSOR_CRITICAL_HIGH:	
							sensor_info->current_state = SAHPI_ES_UPPER_CRIT;
							break; 
				
						default:
							sensor_info->current_state = SAHPI_ES_UNSPECIFIED; 
							break; 
					}	
				
				
					rtas_get_sensor_location_code(token, index, sensor_info->sensor_location);
	
					oh_add_rdr(handle->rptcache, parent_res_event->u.res_event.entry.ResourceId,
					           &(event->u.rdr_event.rdr), sensor_info, 0);
				}	
			}
		}
	}
	
	close(file);
	
	return SA_OK;
}	

/**
 * populate_rtas_sensor_info
 * 
 * @data - pointer to the location of the SaHpiSensorRecT.
 * @token - the sensor type.
 *
 * @return - writes the specific sensor info based on the type
 *           of sensor.
 */
void populate_rtas_sensor_rec_info(int token, SaHpiSensorRecT *sensor_info)
{
			
	if (!sensor_info)
		return;
	
	sensor_info->DataFormat.IsSupported = SAHPI_TRUE;
	
	switch ((rtasSensorToken)token) {
		
		case RTAS_SECURITY_SENSOR:
				
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_PHYSICAL_SECURITY;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_CHARACTERS;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;
    			
			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 3;
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;
			
			
				
			break;

		case RTAS_THERMAL_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_TEMPERATURE;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_INT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_DEGREES_C;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_INT64;
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_INT64;
			
			
			break;
			
		case RTAS_POWER_STATE_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_SYSTEM_ACPI_POWER_STATE;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_CHARACTERS;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 7;			
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;
						
			break;
		
		case RTAS_SURVEILLANCE_SENSOR:
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH;
			
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_MINUTE;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 255;			
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;			

			break;
			
		case RTAS_FAN_SENSOR:
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_FAN;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_RPM;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;			
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
	
			break;
		
		case RTAS_VOLTAGE_SENSOR:
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_VOLTAGE;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_INT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_VOLTS;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_INT64;			
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_INT64;	

					
			break;
		
		
		case RTAS_CONNECTOR_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_SLOT_CONNECTOR;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 4;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;
			
			break;
		
		case RTAS_POWER_SUPPLY_SENSOR:
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_POWER_SUPPLY;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 3;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;

			break;

		case RTAS_GIQ_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_TRUE;
			
			sensor_info->Type = SAHPI_CRITICAL_INTERRUPT;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 1;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;			
			
			break;
		
		case RTAS_SYSTEM_ATTENTION_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_PLATFORM_ALERT;
			
			sensor_info->DataFormat.IsSupported    = SAHPI_TRUE;
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 1;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;
									
			break;
		
		case RTAS_IDENTIFY_INDICATOR_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_OTHER_FRU;
			
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 1;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;			

			break;
				
		case RTAS_COMPONENT_RESET_STATE_SENSOR:
		
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_OPERATIONAL;
			
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Max.Value.SensorUint64 = 2;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.Range.Min.Value.SensorUint64 = 0;			
			
			break;
		
		default:
			
			sensor_info->EnableCtrl = SAHPI_FALSE;
			
			sensor_info->Type = SAHPI_OEM_SENSOR;
			
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_UINT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_UNSPECIFIED;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;

			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_FALSE;						
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_FALSE;
			
			break;

	}
}

void * oh_discover_resources (void *)
        __attribute__ ((weak, alias("rtas_discover_resources")));
void * oh_discover_domain_resource (void *, SaHpiDomainIdT)
        __attribute__ ((weak, alias("rtas_discover_domain_resources")));

