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
	int file, rc, sensor_state, sensor_num = 0;
        char err_buf[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiUint32T token, max_index, index = 0;

	struct oh_event * event;

	if (!handle || !parent_res_event)
	{
		return SA_ERR_HPI_INVALID_PARAMS;
	}	
	
	event = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	
	if (!event) {
		dbg("Out of memory.");
		return SA_ERR_HPI_OUT_OF_SPACE;
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
	
		        /* Now that we have the token and the index, we can
			 * grab the specific data about the sensor from the
			 * RTAS library and build the RDR.
			 */
			for (index = 0; index <= max_index; index++) {
				
				rc = rtas_get_sensor (token, index, &sensor_state);
				
				
				if (rc < SA_OK) {
					
					decode_rtas_error(rc, err_buf, SAHPI_MAX_TEXT_BUFFER_LENGTH, 
					                      token, index);
	
					dbg("Library call (rtas_get_sensor) failure for the "
					    "sensor at token %04d, index %d:\n%s\n",
					     token, index, err_buf);

				}
				
				/* This function assumes the RPT */
				
				else{
					printf("Token: %d, Index: %d, State: %d\n", token, index, sensor_state);
					
					event->type = OH_ET_RDR;
					event->did = oh_get_default_domain_id();
					event->u.rdr_event.parent = parent_res_event->u.res_event.entry.ResourceId;
					event->u.rdr_event.rdr.RdrType = SAHPI_SENSOR_RDR;
					event->u.rdr_event.rdr.Entity = parent_res_event->u.res_event.entry.ResourceEntity;
					
					/* Do entity path business */
					//rtas_modify_sensor_ep();
					
					/* For now, assume sensor number represents a count.  If we decide later to 
					 * create an RPT for each sensor type (and fill in the RDRs that consist of
					 * the sensor type), then the num will need to be reset.
					 */
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Num = sensor_num++;
					
					populate_rtas_sensor_info(token, &(event->u.rdr_event.rdr.RdrTypeUnion.SensorRec));
					 	    
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Category = SAHPI_EC_UNSPECIFIED;    
					
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.EnableCtrl = SAHPI_FALSE;
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.EventCtrl = SAHPI_SEC_READ_ONLY;  
					event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Events = SAHPI_ES_UNSPECIFIED;
					
					//event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.ThresholdDefn
					//event->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Oem 
					
					//oh_init_textbuffer(&(event->u.rdr_event.rdr.IdString));
					//oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), sensor_array[i].comment);

				}	
				
				
					
			
			}
	}
	
	return 1;
}	
