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


#include <rtas_utils.h>

/**
 * librtas_error
 * @brief check for a librtas specific return code
 *
 * @param error librtas return code
 * @param buf buffer to write librtas error message to
 * @param size size of "buffer"
 */
void decode_rtas_error (int error, char *buf, size_t size, int token, int index) 
{
	switch (error) {
		case -1:
			snprintf(buf, size, "Hardware error retrieving a sensor: token %04d, "
				            "index %d\n", token, index);
			break;
		case -3:
			snprintf(buf, size,"The sensor at token %04d, index %d is not "
						    "implemented.\n", token, index);
			break;	
		case RTAS_KERNEL_INT:
			snprintf(buf, size, "No kernel interface to firmware");
			break;
		case RTAS_KERNEL_IMP:
			snprintf(buf, size, "No kernel implementation of function");
			break;
		case RTAS_PERM:
			snprintf(buf, size, "Non-root caller");
			break;
		case RTAS_NO_MEM:
			snprintf(buf, size, "Out of heap memory");
			break;
		case RTAS_NO_LOWMEM:
			snprintf(buf, size, "Kernel out of low memory");
			break;
		case RTAS_FREE_ERR:
			snprintf(buf, size, "Attempt to free nonexistant RMO buffer");
			break;
		case RTAS_TIMEOUT:
			snprintf(buf, size, "RTAS delay exceeded specified timeout");
			break;
		case RTAS_IO_ASSERT:
			snprintf(buf, size, "Unexpected I/O error");
			break;
		case RTAS_UNKNOWN_OP:
			snprintf(buf, size, "No firmware implementation of function");
			break;
		default:
			snprintf(buf, size, "Unknown librtas error %d", error);
	}

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
void populate_rtas_sensor_info(int token, SaHpiSensorRecT *sensor_info)
{

	if (!sensor_info)
		return;
	
	
      	sensor_info->DataFormat.IsSupported = SAHPI_TRUE;
		
	switch (token) {
		
		case 1:
		
			sensor_info->Type = SAHPI_PHYSICAL_SECURITY;
			
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

		case 3:
		
			sensor_info->Type = SAHPI_TEMPERATURE;
			
			sensor_info->DataFormat.ReadingType    = SAHPI_SENSOR_READING_TYPE_INT64;
			sensor_info->DataFormat.BaseUnits      = SAHPI_SU_DEGREES_C;
			sensor_info->DataFormat.ModifierUnits  = SAHPI_SU_UNSPECIFIED;
   			sensor_info->DataFormat.ModifierUse    = SAHPI_SMUU_NONE;
	      		sensor_info->DataFormat.Percentage     = SAHPI_FALSE;
    			
			
			//?????????????????????????????????????????????????????????????????????????????
			sensor_info->DataFormat.Range.Flags    = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
			
			sensor_info->DataFormat.Range.Max.IsSupported        = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Max.Type               = SAHPI_SENSOR_READING_TYPE_INT64;
			
			sensor_info->DataFormat.Range.Min.IsSupported 	     = SAHPI_TRUE;
			sensor_info->DataFormat.Range.Min.Type 	   	     = SAHPI_SENSOR_READING_TYPE_UINT64;
			
			break;
		case 9:
		
			sensor_info->Type = SAHPI_SYSTEM_ACPI_POWER_STATE;
			
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
		
		
		case 9001:
			
			sensor_info->Type = SAHPI_FAN;
			
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
		case 9002:
			
			sensor_info->Type = SAHPI_VOLTAGE;
			
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
		case 9004:
			
			sensor_info->Type = SAHPI_POWER_SUPPLY;
			
			break;
		case 9006:
		
			sensor_info->Type = SAHPI_PLATFORM_ALERT;
			
			break;
		case 9009:
		
			sensor_info->Type = SAHPI_OPERATIONAL;
			
			break;
		
		default:
			
			/* OUTSTANDING TOKENS:
			 *	9000, 9003, 9005, 9007
			 */
			
			sensor_info->Type = SAHPI_OEM_SENSOR;
			
			break;

	}
}				
