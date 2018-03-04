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

#ifndef _OV_REST_SENSOR_H
#define _OV_REST_SENSOR_H

/* Include files */
#include "ov_rest_utils.h"

/* OV_REST_STM_VALID_MASK represents masks for "up critical"
 * and "up major" thresholds
 */
#define OV_REST_STM_VALID_MASK (SaHpiSensorThdMaskT)0x30
#define OV_REST_STM_UNSPECIFED (SaHpiSensorThdMaskT)0x00

/* Sensor classes of OV REST plugin
 *
 * On adding a new sesor class, please update the maximum sensor class value in
 * ov_rest_resources.h. Accordingly, add items to sensor enum mapping and
 * event assert mapping arrays in ov_rest_resources.c
 */
#define OV_REST_OPER_CLASS			0
#define OV_REST_PRED_FAIL_CLASS			1
#define OV_REST_REDUND_CLASS			2
#define OV_REST_DIAG_CLASS			3
#define OV_REST_TEMP_CLASS      		4
#define OV_REST_PWR_STATUS_CLASS		5
#define OV_REST_FAN_SPEED_CLASS			6
#define OV_REST_PWR_SUBSYS_CLASS		7
#define OV_REST_ENC_AGR_OPER_CLASS		8
#define OV_REST_ENC_AGR_PRED_FAIL_CLASS		9
#define OV_REST_BOOL_CLASS			10
/* For some of the sensors like CPU fault, boolean false indicates sensor
 * state as enabled. BOOLEAN_REVERSE_CLASS represents the sensors whose sensor
 * value indicates reverse meaning
 */
#define OV_REST_BOOL_RVRS_CLASS		11
#define OV_REST_HEALTH_OPER_CLASS	12
#define OV_REST_HEALTH_PRED_FAIL_CLASS	13
#define OV_REST_BLADE_THERMAL_CLASS	14

/* Sensor assert states of OV REST plugin
 *
 * On adding new sensor event assert state, please update the maximum sensor
 * event array size value in ov_rest_resources.h. Accordingly, add items to
 * sensor event array in global sensor array in ov_rest_resources.c
 */
#define OV_REST_SEN_ASSERT_TRUE		0
#define OV_REST_SEN_ASSERT_FALSE	1
#define OV_REST_SEN_NO_CHANGE		2

/* Index of different thermal events event in event array.
 * These index will be used during the event generation
 */
/* OK to CAUTION */
#define OV_REST_TEMP_OK_CAUT	0
/* CAUTION to OK */
#define OV_REST_TEMP_CAUT_OK	1
/* CAUTION to CRITICAL */
#define OV_REST_TEMP_CAUT_CRIT	2
/* CRITICAL to CAUTION */
#define OV_REST_TEMP_CRIT_CAUT	3

/* The below code reduces the code size and eases code maintenance */
#define OV_REST_PROCESS_SENSOR_EVENT(sensor_num, sensor_value, reading, \
				     threshold) \
{ \
	rv = ov_rest_proc_sen_evt(oh_handler, resource_id, sensor_num, \
				 (SaHpiInt32T) sensor_value, reading, \
				 threshold); \
	if (rv != SA_OK) { \
		err("processing the sensor event for sensor %x has failed", \
		     sensor_num); \
		return rv; \
	} \
}


/* Maximum number of enum values for healthStatus field in extraData structure
 */
#define OV_REST_MAX_HEALTH_ENUM 8

/* healthStatus string. This is used for parsing the healthStatus field in
 * extraData structure
 */
#define OV_REST_HEALTH_STATUS_STR "healthStatus"

/* Different values supported by helthStatus structure in extraData */
enum ov_rest_extra_data_health {
	/* extraData healthStatus UNKNOWN */
	HEALTH_UNKNOWN,
	/* extraData healthStatus OTHER */
	HEALTH_OTHER,
	/* extraData healthStatus OK */
	HEALTH_OK,
	/* extraData healthStatus DEGRADED */
	HEALTH_DEGRAD,
	/* extraData healthStatus STRESSED */
	HEALTH_STRESSED,
	/* extraData healthStatus PREDICTIVE_FAILURE */
	HEALTH_PRED_FAIL,
	/* extraData healthStatus ERROR */
	HEALTH_ERROR,
	/* extraData healthStatus NON_RECOVERABLERROR */
	HEALTH_NRE
};

#define OV_REST_MAX_THRM_SEN	13

enum ovRestThermalSen {
	SYSTEM_ZONE,
	CPU_ZONE,
	CPU_1,
	CPU_2,
	CPU_3,
	CPU_4,
	DISK_ZONE,
	MEMORY_ZONE,
	AMBIENT_ZONE,
	STORAGE_ZONE,
	IO_BOARD_ZONE,
	POWER_SUPPLY_ZONE,
	CHASSIS_ZONE
};

/* Define the sensor number range for blade extra thermal sensors */

#define OV_REST_BLD_THRM_SEN_START	0x02e
#define OV_REST_BLD_THRM_SEN_END	0x06b

/* Structure required for building thermal sensor when server blade is off */
struct ovRestStaticThermalSensorInfo {
	SaHpiSensorNumT base_sen_num; /* Base sensor number for sensor type */
	enum ovRestThermalSen sensor; /* thermal sensor type */
	SaHpiInt32T sensor_count; /* Number of sensor to be created of 
				   * above thermal sensor type 
				   */
};

/* Structure containing thermal sensor information data*/
struct ovRestThrmSenData {
	SaHpiRdrT rdr_num;
	SaHpiSensorNumT sen_delta; /* Delta difference of the sensor rdr number
				    * from the base sensor number of particular
				    * sensor type
				    */
};
	
/* Structure for sensor reading */
struct ovRestSensorReadingData {
        SaHpiSensorReadingT data;
        SaHpiEventStateT event;
};

/* Declaration of sensor specific information structure */
struct ov_rest_sensor_info {
        SaHpiEventStateT current_state;
        SaHpiEventStateT previous_state;
        SaHpiBoolT sensor_enable;
        SaHpiBoolT event_enable;
        SaHpiEventStateT assert_mask;
        SaHpiEventStateT deassert_mask;
        SaHpiSensorReadingT sensor_reading;
        SaHpiSensorThresholdsT threshold;
};

/* Declaration of the functions related to sensor */
SaErrorT ov_rest_get_sensor_reading(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiSensorNumT num,
                                   SaHpiSensorReadingT *data,
                                   SaHpiEventStateT    *state);

SaErrorT ov_rest_get_sensor_thresholds(void *oh_handler,
                                      SaHpiResourceIdT resource_id,
                                      SaHpiSensorNumT num,
                                      SaHpiSensorThresholdsT *thres);

SaErrorT ov_rest_set_sensor_thresholds(void *oh_handler,
                                      SaHpiResourceIdT resource_id,
                                      SaHpiSensorNumT num,
                                      const SaHpiSensorThresholdsT *thres);

SaErrorT ov_rest_get_sensor_event_enable(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiSensorNumT num,
                                         SaHpiBoolT *enable);

SaErrorT ov_rest_set_sensor_event_enable(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiSensorNumT num,
                                         const SaHpiBoolT enable);

SaErrorT ov_rest_get_sensor_enable(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiSensorNumT num,
                                  SaHpiBoolT *enable);

SaErrorT ov_rest_set_sensor_enable(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiSensorNumT num,
                                  SaHpiBoolT enable);

SaErrorT ov_rest_get_sensor_event_masks(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSensorNumT num,
                                       SaHpiEventStateT *assert,
                                       SaHpiEventStateT *deassert);

SaErrorT ov_rest_set_sensor_event_masks(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSensorNumT num,
                                       SaHpiSensorEventMaskActionT act,
                                       SaHpiEventStateT assert,
                                       SaHpiEventStateT deassert);

SaErrorT update_sensor_rdr(struct oh_handler_state *oh_handler,
                           SaHpiResourceIdT resource_id,
                           SaHpiSensorNumT num,
                           SaHpiRptEntryT *rpt,
                           struct ovRestSensorReadingData *data);

SaErrorT generate_sensor_enable_event(void *oh_handler,
                                      SaHpiSensorNumT rdr_num,
                                      SaHpiRptEntryT *rpt,
                                      SaHpiRdrT *rdr,
                                      struct ov_rest_sensor_info *sensor_info);

SaErrorT generate_sensor_assert_thermal_event(void *oh_handler,
                                              SaHpiSensorNumT rdr_num,
                                              SaHpiRptEntryT *rpt,
                                              SaHpiRdrT *rdr,
                                              SaHpiSensorReadingT
                                                      current_reading,
                                              SaHpiSeverityT event_severity,
                                              struct ov_rest_sensor_info
                                                      *sensor_info);

SaErrorT generate_sensor_deassert_thermal_event(void *oh_handler,
                                                SaHpiSensorNumT rdr_num,
                                                SaHpiRptEntryT *rpt,
                                                SaHpiRdrT *rdr,
                                                SaHpiSensorReadingT
                                                        current_reading,
                                                SaHpiSeverityT event_severity,
                                                struct ov_rest_sensor_info
                                                        *sensor_info);

SaErrorT check_and_deassert_event(struct oh_handler_state *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiRdrT *rdr,
                                  struct ov_rest_sensor_info *sensor_info);

SaErrorT ov_rest_build_sen_rdr(struct oh_handler_state *oh_handler,
			       SaHpiResourceIdT resource_id,
			       SaHpiRdrT *rdr,
			       struct ov_rest_sensor_info **sensor_info,
			       SaHpiSensorNumT sensor_num);

SaErrorT ov_rest_map_sen_val(struct ov_rest_sensor_info *sensor_info,
			     SaHpiSensorNumT sensor_num,
			     SaHpiInt32T sensor_value,
			     SaHpiInt32T *sensor_status);

SaErrorT ov_rest_proc_sen_evt(struct oh_handler_state *oh_handler,
			      SaHpiResourceIdT resource_id,
			      SaHpiSensorNumT sen_num,
			      SaHpiInt32T sensor_value,
			      SaHpiFloat64T trigger_reading,
			      SaHpiFloat64T trigger_threshold);

SaErrorT ov_rest_proc_mem_evt(struct oh_handler_state *oh_handler,
			      SaHpiResourceIdT resource_id,
			      SaHpiSensorNumT sen_num,
			      char *trigger_reading,
			      SaHpiSeverityT severity);

SaErrorT ov_rest_map_thresh_resp(SaHpiRdrT *rdr,
				 void *response,
				 SaHpiBoolT event_support,
				 struct ov_rest_sensor_info
				 *sensor_info);

SaErrorT ov_rest_assert_sen_evt(struct oh_handler_state *oh_handler,
				SaHpiRptEntryT *rpt,
				GSList *assert_sensor_list);

SaErrorT ov_rest_gen_res_event(struct oh_handler_state *oh_handler,
                                SaHpiRptEntryT *rpt,
                                SaHpiInt32T res_status);

#endif
