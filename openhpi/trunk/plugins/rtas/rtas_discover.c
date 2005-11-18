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
	int file, rc, sensor_state;
        char err_buf[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiUint32T token, max_index, index;

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
				else{
					printf("Token: %d, Index: %d, State: %d\n", token, index, sensor_state);

				}	
				
				
					
			
			}
	}
	
	return 1;
}	
