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
 **/

#include "ov_rest_resources.h"

/* Array for mapping the OV enums to HPI values. -1 is assigned if the OV is 
 * not supporting the enum value.
 * 
 * Please modify the array on adding new sensor class and/or changing the 
 * sensor enum values defined in ov_rest_resources.h
 */

const SaHpiInt32T ov_rest_sen_val_map_arr[OV_REST_MAX_SEN_CLASS]
					 [OV_REST_MAX_ENUM] = {
	/* OV_REST_OPER_CLASS. This uses the enum opStatus values as index */
	{
		SAHPI_ES_DISABLED, /* OP_STATUS_UNKNOWN */
		SAHPI_ES_DISABLED, /* OP_STATUS_OTHER */
		SAHPI_ES_ENABLED, /* OP_STATUS_OK */
		SAHPI_ES_ENABLED, /* OP_STATUS_CRITICAL */
		SAHPI_ES_ENABLED, /* OP_STATUS_WARNING */
		SAHPI_ES_DISABLED, /* OP_STATUS_DISABLED */
	},
};

/* Array for mapping the OV enums to OV REST plugin sensor assert states. 
 * -1 is assigned, if the OV is not supporting the enum value.
 * 
 * Please modify the array on adding new sensor class and/or changing the 
 * sensor enum values defined in ov_rest_resources.h
 */
const SaHpiInt32T ov_rest_sen_assert_map_arr[OV_REST_MAX_SEN_CLASS]
					    [OV_REST_MAX_ENUM] = {
	/* OV_REST_OPER_CLASS. This uses the enum opStatus values
	 * as index
	 */
	{
		/* uses the enum opStatus values as index */
		OV_REST_SEN_ASSERT_TRUE, /* OP_STATUS_UNKNOWN */
		OV_REST_SEN_ASSERT_TRUE, /* OP_STATUS_OTHER */
		OV_REST_SEN_ASSERT_FALSE, /* OP_STATUS_OK */
		OV_REST_SEN_ASSERT_TRUE, /* OP_STATUS_CRITICAL */
		OV_REST_SEN_ASSERT_FALSE, /* OP_STATUS_WARNING */
		OV_REST_SEN_ASSERT_TRUE, /* OP_STATUS_DISABLED */
	},
};

/* Array for creating the sensor RDRs
 * 
 * Please add new entries to the array on adding a new sensor
 */
const struct ovRestSensor ov_rest_sen_arr[] = {
	/* operational status sensor */
	{
		.sensor = {
			.Num = OV_REST_SEN_OPER_STATUS,
			.Type = SAHPI_OPERATIONAL,
			.Category = SAHPI_EC_ENABLE,
			.EnableCtrl = SAHPI_TRUE,
			.EventCtrl = SAHPI_SEC_PER_EVENT,
			.Events = SAHPI_ES_ENABLED | SAHPI_ES_DISABLED,
			.DataFormat = {
				.IsSupported = SAHPI_FALSE,
			},
			.ThresholdDefn = {
				.IsAccessible = SAHPI_FALSE,
			},
			.Oem = 0,
		},
		.sensor_info = {
			.current_state = SAHPI_ES_ENABLED,
			.sensor_enable = SAHPI_TRUE,
			.event_enable = SAHPI_TRUE,
			.assert_mask = SAHPI_ES_DISABLED,
			.deassert_mask = SAHPI_ES_ENABLED,
		},
		.sensor_class = OV_REST_OPER_CLASS,
		.sen_evt = {
			/* Assert event */
			{
				.EventType = SAHPI_ET_SENSOR,
				.Severity = SAHPI_CRITICAL,
				.EventDataUnion.SensorEvent = {
					.SensorNum = OV_REST_SEN_OPER_STATUS,
					.SensorType = SAHPI_OPERATIONAL,
					.EventCategory = SAHPI_EC_ENABLE,
					.Assertion = SAHPI_TRUE,
					.EventState = SAHPI_ES_DISABLED,
					.OptionalDataPresent =
						SAHPI_SOD_CURRENT_STATE |
						SAHPI_SOD_PREVIOUS_STATE,
					.TriggerReading = {
						.IsSupported = SAHPI_FALSE,
					},
					.TriggerThreshold = {
						.IsSupported = SAHPI_FALSE,
					},
					.PreviousState =  SAHPI_ES_ENABLED,
					.CurrentState = SAHPI_ES_DISABLED,
					.Oem = 0,
					.SensorSpecific = 0,
				},
			},
			/* Deassert event */
			{
				.EventType = SAHPI_ET_SENSOR,
				.Severity = SAHPI_CRITICAL,
				.EventDataUnion.SensorEvent = {
					.SensorNum = OV_REST_SEN_OPER_STATUS,
					.SensorType = SAHPI_OPERATIONAL,
					.EventCategory = SAHPI_EC_ENABLE,
					.Assertion = SAHPI_FALSE,
					.EventState = SAHPI_ES_ENABLED,
					.OptionalDataPresent =
						SAHPI_SOD_CURRENT_STATE |
						SAHPI_SOD_PREVIOUS_STATE,
					.TriggerReading = {
						.IsSupported = SAHPI_FALSE,
					},
					.TriggerThreshold = {
						.IsSupported = SAHPI_FALSE,
					},
					.PreviousState =  SAHPI_ES_DISABLED,
					.CurrentState = SAHPI_ES_ENABLED,
					.Oem = 0,
					.SensorSpecific = 0,
				},
			},
			{},
			{},
		},
		.comment = "Operational status",
	},
};



/* Global array containing the details of all control rdr structure details 
 *
 * Please add new entries to the array on supporting new control in OV REST
 */
const struct ov_rest_control ov_rest_cntrl_arr[] = {
	/* UID status */
	{
		.control = {
			.Num = OV_REST_UID_CNTRL,
			.OutputType = SAHPI_CTRL_LED,
			.Type = SAHPI_CTRL_TYPE_DIGITAL,
			.TypeUnion.Digital.Default = SAHPI_CTRL_STATE_OFF,
			.DefaultMode = {
				.Mode = SAHPI_CTRL_MODE_MANUAL,
				.ReadOnly = SAHPI_TRUE,
			},
			.WriteOnly = SAHPI_FALSE,
			.Oem = 0,
		},
		.comment = "UID LED state",
	},
	/* Power status */
	{
		.control = {
			.Num = OV_REST_PWR_CNTRL,
			.OutputType = SAHPI_CTRL_POWER_STATE,
			.Type = SAHPI_CTRL_TYPE_DIGITAL,
			.TypeUnion.Digital.Default = SAHPI_CTRL_STATE_ON,
			.DefaultMode = {
				.Mode = SAHPI_CTRL_MODE_MANUAL,
				.ReadOnly = SAHPI_TRUE,
			},
			.WriteOnly = SAHPI_FALSE,
			.Oem = 0,
		},
		.comment = "power state",
	},
	{} /* Terminate array with a null element */
};



