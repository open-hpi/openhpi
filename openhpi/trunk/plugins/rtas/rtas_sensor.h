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
 
#ifndef RTAS_SENSOR_H
#define RTAS_SENSOR_H

#include <glib.h>
#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <oh_domain.h>
#include <librtas.h>

#define RTAS_SENSORS_PATH	"/proc/device-tree/rtas/rtas-sensors"
#define RTAS_SENSOR_LOCATION    "/proc/device-tree/rtas/ibm,sensor-"
#define MAX_SENSOR_LOCATION_STRING_SIZE 24

#define MILLISECONDS_PER_MINUTE 1000
#define TOKEN_MASK 0xFFFFFFF0
#define TIME_MASK ~TOKEN_MASK


struct SensorInfo {
	SaHpiUint32T	token;
	SaHpiUint32T	index;
	char 		sensor_location[MAX_SENSOR_LOCATION_STRING_SIZE];
};


int rtas_get_sensor_location_code(int token, int index, char *buffer);  

SaErrorT rtas_get_sensor_reading(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiSensorNumT sid,
				 SaHpiSensorReadingT *reading,
				 SaHpiEventStateT *state);

#endif
