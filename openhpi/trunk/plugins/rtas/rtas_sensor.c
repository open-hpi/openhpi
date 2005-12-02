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
 *        Daniel de Araujo <ddearauj@us.ibm.com>
 *        Renier Morales <renierm@users.sf.net>
 *
 */
 

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <rtas_sensor.h>
#include <rtas_utils.h>


/**
 * rtas_get_sensor_reading:
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
 * SA_ERR_HPI_CAPABILITY      - Resource doesn't have SAHPI_CAPABILITY_SENSOR.
 * SA_ERR_HPI_INVALID_REQUEST - Sensor is disabled.
 * SA_ERR_HPI_NOT_PRESENT     - Sensor doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS  - Pointer parameter(s) are NULL.
 **/
 
SaErrorT rtas_get_sensor_reading(void                *handler,
				 SaHpiResourceIdT     resourceid,
				 SaHpiSensorNumT      sensornum,
				 SaHpiSensorReadingT *reading,
				 SaHpiEventStateT    *e_state)
{
	SaHpiSensorReadingT sensor_reading;
	SaHpiEventStateT event_state;
	struct SensorInfo *sensor_info;
	SaHpiInt32T state, status;
	char err_buf[150];
	size_t sizebuf = sizeof(err_buf);
	
	if (!handler) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        struct oh_handler_state *handler_state = (struct oh_handler_state *)handler;

	//do some locking here......
	
	/* Check if resource exists and has sensor capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handler_state->rptcache, resourceid);
        
	if (!rpt) {	
		//unlock it
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)) {
		//unlock it
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Check if sensor exists and is enabled */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handler_state->rptcache, resourceid, SAHPI_SENSOR_RDR, sensornum);
	
	if (rdr == NULL) {
		//unlock it
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	sensor_info = (struct SensorInfo *)oh_get_rdr_data(handler_state->rptcache, resourceid, rdr->RecordId);
 	
	if (sensor_info == NULL) {
		//unlock it
		dbg("No sensor data. Sensor=%s", rdr->IdString.Data);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       
	
	memset(&sensor_reading, 0, sizeof(SaHpiSensorReadingT));
	event_state = SAHPI_ES_UNSPECIFIED;

	trace("Sensor Reading: Resource=%s; RDR=%s", rpt->ResourceTag.Data, rdr->IdString.Data);
	
	/************************************************************
	 * Get sensor's reading.
         * Need to go through this logic, since user may request just
         * the event state for a readable sensor. Need to translate
         * sensor reading to event in this case.
         ************************************************************/
	if (rdr->RdrTypeUnion.SensorRec.DataFormat.IsSupported == SAHPI_TRUE) {
		
		status = rtas_get_sensor(sensor_info->token, sensor_info->index, &state);
		
		if (status < 0) {
			decode_rtas_error(status, err_buf, sizebuf, sensor_info->token, sensor_info->index);
					
			dbg("Cannot determine sensor's reading. Error=%s", err_buf);
			//unlock it
			return(status);
		}
		
		else {
			
			/* if the return code is 990x. we must sleep for 10^x milliseconds before
			 * trying to query the rtas sensor again
			 */
			if ((status & TOKEN_MASK) == 9900) {
				sleep((exp10(status & TIME_MASK)));
				
				status = rtas_get_sensor(sensor_info->token, sensor_info->index, &state);
		
				if (status < 0) {
					decode_rtas_error(status, err_buf, sizebuf, sensor_info->token, sensor_info->index);
					
					dbg("Cannot determine sensor's reading. Error=%s", err_buf);
					//unlock it
					return(status);
				}
			}
			else {
				sensor_reading.IsSupported = SAHPI_TRUE;
				sensor_reading.Type = rdr->RdrTypeUnion.SensorRec.DataFormat.ReadingType;
				
				switch (sensor_reading.Type) {
					
					case SAHPI_SENSOR_READING_TYPE_INT64:
					
						sensor_reading.Value.SensorInt64 = state;
						
						break;
					case SAHPI_SENSOR_READING_TYPE_UINT64:
					
						sensor_reading.Value.SensorUint64 = state;
					
						break;
					case SAHPI_SENSOR_READING_TYPE_FLOAT64:
						
						sensor_reading.Value.SensorFloat64 = (SaHpiFloat64T)state;
						
						break;
					case SAHPI_SENSOR_READING_TYPE_BUFFER:
					
						memcpy(sensor_reading.Value.SensorBuffer, &state, sizeof(state)); 
						
						break;
						
					default:
					
						break;	
				}
			}			
						
		}					
		
	}
	else {
		sensor_reading.IsSupported = SAHPI_FALSE;
	}

	/******************************************************************
	 * Get sensor's event state.
         * Always get the event state, to reset the sensor's current state,
         * whether caller wants to know event state or not.
	 * NEED TO FIND OUT IF THIS IS IMPLEMENTED
	 ******************************************************************/
#if 0		
	err = snmp_bc_get_sensor_eventstate(hnd, rid, sid, &working_reading, &working_state);
	if (err) {
		dbg("Cannot determine sensor's event state. Error=%s", decode_rtas_error(err, 
											 sensor_info->token, 
											 sensor_info->index));
		//unlock it
		return(err);
	}

#endif
	/* sinfo->cur_state = working_state; */
	if (reading) memcpy(reading, &sensor_reading, sizeof(SaHpiSensorReadingT));
//	if (e_state) memcpy(state, &sensor_state, sizeof(SaHpiEventStateT));

	//unlock it
        
	return(SA_OK);
}

/**
 * rtas_get_sensor_location_code
 * 
 * @token  - the sensor type.
 * @index  - index of the sensor.
 * @buffer - buffer to store the location code of the sensor.
 *
 * @return - TBD
 */

int rtas_get_sensor_location_code(int token, int index, char *buffer)
{
	int filehandle, i, len;
	char filename[45], temp[4096], *pos;

	if (buffer == NULL)
		return 0;

	buffer[0] = '\0';
	snprintf(filename, MAX_SENSOR_LOCATION_STRING_SIZE, 
	                          "%s%04d", RTAS_SENSOR_LOCATION, token);

	filehandle = open(filename, O_RDONLY);
	if (filehandle < 0)
		return 0;

	if ((len = read(filehandle, temp, 4096)) < 0)
		return 0;

	pos = temp;

	for (i=0; i<index; i++) {
		pos += strlen(pos)+1;
		if (pos >= temp + len) {
			close(filehandle);
			return 0;
		}
	}

	strncpy(buffer, pos, MAX_SENSOR_LOCATION_STRING_SIZE);
	close(filehandle);

	return 1;
}

