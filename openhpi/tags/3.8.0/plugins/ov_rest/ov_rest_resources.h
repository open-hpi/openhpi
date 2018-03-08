/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

#ifndef _OV_REST_RESOURCES_H
#define _OV_REST_RESOURCES_H

/* Include files */
#include <SaHpiOvRest.h>
#include "ov_rest_sensor.h"

/* Maximum sensor classes in OV REST
 * 
 * On adding a new sensor class in ov_rest_sensor.h, please change the maximum
 * sensor class value. Accordingly, add new sensor class support in global
 * sensor enum value mapping array and global sensor event assert state mapping
 * array in ov_rest_resources.c
 */
#define OV_REST_MAX_SEN_CLASS 14

/* Maximum sensor enum values in OV REST
 *
 * If a new sensor added in include/SaHpiOvRest.h has more enum values, then
 * change the maximum enum. Accordingly, add new sensor enum values to global
 * sensor enum value mapping array and global sensor event assert state mapping
 * array in ov_rest_resources.c
 */
#define OV_REST_MAX_ENUM 21

/* Maximum sensor event array size
 *
 * Increase the event array size if a new sensor supports more number of sensor
 * event payload. Accordingly, increase the sensor_event arry in global sensor
 * array in ov_rest_resources.c
 */
#define OV_REST_MAX_SEN_EVT 4


/* Structure for storing the sensor RDR and event information  */
struct ovRestSensor {
	SaHpiSensorRecT sensor;
	struct ov_rest_sensor_info sensor_info;
	SaHpiInt32T sensor_class;
	SaHpiEventT sen_evt[OV_REST_MAX_SEN_EVT];
	const char *comment;
};

/* Structure for storing the control RDR */
struct ov_rest_control {
	SaHpiCtrlRecT control;
	const char *comment;
};

extern const SaHpiInt32T ov_rest_sen_val_map_arr[OV_REST_MAX_SEN_CLASS]
						[OV_REST_MAX_ENUM];
extern const SaHpiInt32T ov_rest_sen_assert_map_arr[OV_REST_MAX_SEN_CLASS]
						   [OV_REST_MAX_ENUM];
extern const struct ovRestSensor ov_rest_sen_arr[];

extern const struct ov_rest_control ov_rest_cntrl_arr[];

#endif


