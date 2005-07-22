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
 *        W. David Ashley <dashley@us.ibm.com>
 */

#include <sim_init.h>


/**************************************************************************
 *                        Resource Definitions
 *
 * These are patterned after an RSA type machine
 **************************************************************************/

/*-------------------------------------------------------------------------
  NOTE!!!!!!!!!
  The order is important here! Changing the order of these resources or
  adding additional resources means the code in the function sim_discovery()
  also needs to change!
  ------------------------------------------------------------------------*/

struct sim_rpt sim_rpt_array[] = {
	/* Chassis */
        {
                .rpt = {
			.ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
                        },

                        .ResourceCapabilities = SAHPI_CAPABILITY_EVENT_LOG |
			                        SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_SENSOR |
                                                SAHPI_CAPABILITY_ANNUNCIATOR |
                                                SAHPI_CAPABILITY_POWER |
                                                SAHPI_CAPABILITY_RESET |
                                                SAHPI_CAPABILITY_WATCHDOG |
                                                SAHPI_CAPABILITY_CONTROL,
                        .ResourceSeverity = SAHPI_CRITICAL,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Chassis"
        },
        /* CPUs */
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
				{
                                        .EntityType = SAHPI_ENT_PROCESSOR,
                                        .EntityLocation = SIM_HPI_LOCATION_BASE
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                 },
                .comment = "CPU"
        },
        /* DASD */
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
				{
                                        .EntityType = SAHPI_ENT_DISK_BAY,
                                        .EntityLocation = SIM_HPI_LOCATION_BASE
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RDR |
    			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
 			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "DASD 1"
        },
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
				{
                                        .EntityType = SAHPI_ENT_DISK_DRIVE_BAY,
                                        .EntityLocation = SIM_HPI_LOCATION_BASE
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_MANAGED_HOTSWAP |
                                                SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
 			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "HS DASD 2"
        },
        /* Fans */
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
				        .EntityType = SAHPI_ENT_FAN,
			                .EntityLocation = SIM_HPI_LOCATION_BASE
				},
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
 			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Fan"
        },

        {} /* Terminate array with a null element */
};


/******************************************************************************
 *                      Sensor Definitions
 *
 * These are patterned after an RSA type machine
 ******************************************************************************/

/*****************
 * Chassis Sensors
 *****************/

struct sim_sensor sim_chassis_sensors[] = {
        /* Thermal sensor on planar */
        {
		.index = 1,
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0601C480", /* EN_CUTOFF_HI_OVER_TEMP_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0601C080", /* EN_OVER_TEMP_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0601D500", /* EN_PFA_HI_OVER_TEMP_PLANAR */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 35,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "Planar temperature sensor"
        },
        /* CPU area thermal sensor on planar */
        {
		.index = 2,
                .sensor = {
                        .Num = 2,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0401C480", /* EN_CUTOFF_HI_OVER_TEMP_PROC */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0401D400", /* EN_PFA_HI_OVER_TEMP_PROC */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 40,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "Planar CPU area temperature sensor"
        },
        /* I/O thermal sensor on planar */
        {
		.index = 3,
                .sensor = {
                        .Num = 3,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 42,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "Planar I/O area temperature sensor"
        },
        /* System ambient thermal sensor on planar */
        {
		.index = 4,
                .sensor = {
                        .Num = 4,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "00000064", /* EN_TEMP */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0001C000", /* EN_OVER_TEMP */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0001C080", /* EN_OVER_TEMP_AMBIENT */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0001C480", /* EN_CUTOFF_HI_OVER_TEMP_AMBIENT */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0001D500", /* EN_PFA_HI_OVER_TEMP_AMBIENT */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "0000006E", /* EN_NC_TEMP */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 25,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "Planar system ambient temperature sensor"
        },
        /* Memory thermal sensor on planar */
        {
		.index = 5,
                .sensor = {
                        .Num = 5,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0501C480", /* EN_CUTOFF_HI_OVER_TEMP_MEM_AREA */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0501C481", /* EN_CUTOFF_HI_OVER_TEMP_MEMORY1 */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0501C482", /* EN_CUTOFF_HI_OVER_TEMP_MEMORY2 */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0501D400", /* EN_PFA_HI_OVER_TEMP_MEM_AREA */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "0501D501", /* EN_PFA_HI_OVER_TEMP_MEMORY1 */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "0501D502", /* EN_PFA_HI_OVER_TEMP_MEMORY2 */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 45,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "Planar memory temperature sensor"
        },
        /* 5V sensor */
        {
		.index = 6,
                .sensor = {
                        .Num = 6,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 6.7,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 5,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                         },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "06034480", /* EN_CUTOFF_HI_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "06034800", /* EN_CUTOFF_LO_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
					.event = "08034480", /* EN_CUTOFF_HI_FAULT_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08034800", /* EN_CUTOFF_LO_FAULT_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
                                        .event = "06035500", /* EN_PFA_HI_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "06035800", /* EN_PFA_LO_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 5.02,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 4,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 4.5,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 6,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 5.5,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.2,
						                      },
                                            },
             },
		},
                .comment = "5 volt sensor"
        },
        /* 3.3V sensor */
        {
		.index = 7,
                .sensor = {
                        .Num = 7,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
			.Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
				.IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 3.6,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 3.3,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                       },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "08032480", /* EN_CUTOFF_HI_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08032880", /* EN_CUTOFF_LO_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
					.event = "0002C480", /* EN_CUTOFF_HI_FAULT_3_35V_CONT */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0002C880", /* EN_CUTOFF_LO_FAULT_3_35V_CONT */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
                                        .event = "08033480", /* EN_PFA_HI_FAULT_3_35V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08033880", /* EN_PFA_LO_FAULT_3_35V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
                                        .event = "0002D400", /* EN_PFA_HI_FAULT_3_35V_CONT */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0002D800", /* EN_PFA_LO_FAULT_3_35V_CONT */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 3.31,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2.85,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 3,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 3.55,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 3.4,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.1,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.1,
						                      },
                                            },
             },
		},
                .comment = "3.3 volt sensor"
        },
        /* 12V sensor */
        {
		.index = 8,
                .sensor = {
                        .Num = 8,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 16,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 12,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
		                {
			                .event = "06036480", /* EN_CUTOFF_HI_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "06036800", /* EN_CUTOFF_LO_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
                                        .event = "06037500", /* EN_PFA_HI_FAULT_12V_PLANAR */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
			                .event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
		                },
				{
					.event = "06037800", /* EN_PFA_LO_FAULT_12V_PLANAR */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 12.10,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 10.0,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 11.0,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 14.0,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 13.0,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.25,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.25,
						                      },
                                            },
             },
		},
                .comment = "12 volt sensor"
        },
        /* -12V voltage sensor on Chassis */
        {
		.index = 9,
                .sensor = {
                        .Num = 9,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = -12,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = -16,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "0803E480", /* EN_CUTOFF_HI_FAULT_N12V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0803E800", /* EN_CUTOFF_LO_FAULT_N12V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
					.event = "0803F500", /* EN_PFA_HI_FAULT_N12V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803F800", /* EN_PFA_LO_FAULT_N12V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -11.9,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -14.0,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -13.0,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -10.0,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -11.0,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.25,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.25,
						                      },
                                            },
             },
		},
                .comment = "-12 volt sensor"
        },
        /* -5V voltage sensor on Chassis */
        {
		.index = 10,
                .sensor = {
                        .Num = 10,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = -5,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = -6.7,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "0803C480", /* EN_CUTOFF_HI_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0803C800", /* EN_CUTOFF_LO_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
					.event = "0803D500", /* EN_PFA_HI_FAULT_N5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803D800", /* EN_PFA_LO_FAULT_N5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -5.02,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -6,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -5.5,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -4,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = -4.5,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.2,
						                      },
                                            },
             },
		},
                .comment = "-5 volt sensor"
        },
        /* 2.5V voltage sensor on Chassis */
        {
		.index = 11,
                .sensor = {
                        .Num = 11,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 4.4,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 2.5,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                       },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "08030480", /* EN_CUTOFF_HI_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08030880", /* EN_CUTOFF_LO_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
                                        .event = "08031480", /* EN_PFA_HI_FAULT_2_5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08031880", /* EN_PFA_LO_FAULT_2_5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2.51,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2.25,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 3,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2.75,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.1,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.1,
						                      },
                                            },
             },
		},
		.comment = "2.5 volt sensor"
	},
        /* 1.5V voltage sensor on Chassis */
        {
		.index = 12,
                .sensor = {
                        .Num = 12,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
 			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 4.4,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 1.5,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "08040480", /* EN_CUTOFF_HI_FAULT_1_5V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08040880", /* EN_CUTOFF_LO_FAULT_1_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
                                        .event = "08041400", /* EN_PFA_HI_FAULT_1_5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08041800", /* EN_PFA_LO_FAULT_1_5V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.49,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.10,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.30,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.90,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.70,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.05,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.05,
						                      },
                                            },
             },
		},
		.comment = "1.5 volt sensor"
        },
        /* 1.25V sensor on Chassis */
        {
		.index = 13,
                .sensor = {
                        .Num = 13,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
  			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			          SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_VOLTS,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
				.Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN | SAHPI_SRF_NOMINAL,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 3.3,
						},
                                        },
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 1.25,
						},
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
				             SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                        },
                        .Oem = 0,
		},
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.sensor_enabled = SAHPI_TRUE,
			.events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "08000480", /* EN_CUTOFF_HI_FAULT_1_25V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08000880", /* EN_CUTOFF_LO_FAULT_1_25V */
                                        .event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{
					.event = "08001401", /* EN_PFA_HI_FAULT_1_25V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08001801", /* EN_PFA_LO_FAULT_1_25V */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.24,
						                      },
                                            },
            .thres = {
                        .LowCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.17,
						                      },
                                            },
                        .LowMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.21,
						                      },
                                            },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.33,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 1.29,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.03,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 0.03,
						                      },
                                            },
             },
		},
		.comment = "1.25 volt sensor"
        },

        {} /* Terminate array with a null element */
};

/*************
 * CPU Sensors
 *************/

struct sim_sensor sim_cpu_sensors[] = {
        /* CPU thermal sensor */
        {
		.index = 1,
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
  			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
				.IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
			},
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "0421C40x", /* EN_PROC_HOT_CPUx */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C48x", /* EN_CUTOFF_HI_OVER_TEMP_CPUx */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421D08x", /* EN_THERM_TRIP_CPUx */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
                                        .event = "0421D50x", /* EN_PFA_HI_OVER_TEMP_CPUx */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 50,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "CPU temperature sensor"
        },

        {} /* Terminate array with a null element */
};

/**************
 * DASD Sensors
 **************/

struct sim_sensor sim_dasd_sensors[] = {
        /* DASD thermal sensor */
        {
		.index = 1,
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
  			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .DataFormat = {
				.IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_FALSE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 125,
						},
					},
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
						},
                                        },
                                },
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT |
				             SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "0681C08x", /* EN_CUTOFF_HI_OVER_TEMP_DASD1 */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
                                        .event = "0681C40x", /* EN_PFA_HI_OVER_TEMP_DASD1 */
 					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 50,
						                      },
                                            },
            .thres = {
                        .LowCritical = { .IsSupported = SAHPI_FALSE, },
                        .LowMajor =    { .IsSupported = SAHPI_FALSE, },
                        .LowMinor =    { .IsSupported = SAHPI_FALSE, },
                        .UpCritical =  { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 80,
						                      },
                                            },
                        .UpMajor =     { .IsSupported =  SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
                        .UpMinor =     { .IsSupported = SAHPI_FALSE, },
                        .PosThdHysteresis = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 2,
						                      },
                                            },
                        .NegThdHysteresis = { .IsSupported = SAHPI_FALSE, },
             },
		},
                .comment = "DASD temperature sensor"
        },

        {} /* Terminate array with a null element */
};

/*************
 * Fan Sensors
 *************/

struct sim_sensor sim_fan_sensors[] = {
	/* Fan speed */
        {
		.index = 1,
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_PRED_FAIL,
  			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_PRED_FAILURE_ASSERT | SAHPI_ES_PRED_FAILURE_DEASSERT,
                        .DataFormat = {
                                .IsSupported = SAHPI_TRUE,
                                .ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .Percentage = SAHPI_TRUE,
                                .Range = {
                                        .Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
                                        .Max = {
  						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 100,
                                                },
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorFloat64 = 0,
                                                },
                                        },
                                },
                        },
                         .ThresholdDefn = {
                                .IsAccessible = SAHPI_FALSE,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{
                                        .event = "000A600x", /* EN_FANx_PFA */
  					.event_assertion = SAHPI_TRUE,
       					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_PRED_FAILURE_ASSERT,
                                        .recovery_state = SAHPI_ES_PRED_FAILURE_DEASSERT,
				},
				{},
			},
            .reading = { .IsSupported = SAHPI_TRUE,
                                              .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
                                              .Value = {
							                        .SensorFloat64 = 60,
						                      },
                                            },
		},
                .comment = "Blower fan speed - percent of maximum RPM"
        },

        /* Blower's global operational sensor - event-only */
        {
		.index = 2,
                .sensor = {
                        .Num = 2,
                        .Type = SAHPI_OPERATIONAL,
                        .Category = SAHPI_EC_AVAILABILITY,
			.EnableCtrl = SAHPI_FALSE,
                        .EventCtrl = SAHPI_SEC_READ_ONLY,
                        .Events = SAHPI_ES_RUNNING | SAHPI_ES_OFF_LINE,
                        .DataFormat = {
                                .IsSupported = SAHPI_FALSE,
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_FALSE,
                        },
                        .Oem = 0,
                },
                .sensor_info = {
                        .cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_OFF_LINE,
			.deassert_mask = SAHPI_ES_OFF_LINE,
                        .event_array = {
                                {
                                        .event = "0002680x", /* EN_FAN1_SPEED */
  					.event_assertion = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_OFF_LINE,
       					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .recovery_state = SAHPI_ES_RUNNING,
                                },
                                {
                                        .event = "0A02600x", /* EN_FAULT_FAN1 */
  					.event_assertion = SAHPI_TRUE,
       					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_OFF_LINE,
                                        .recovery_state = SAHPI_ES_RUNNING,
                                },
                                {
                                        .event = "0602600x", /* EN_FAN_x_NOT_PRESENT */
  					.event_assertion = SAHPI_TRUE,
       					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_OFF_LINE,
                                        .recovery_state = SAHPI_ES_RUNNING,
                                },
                                {},
                        },
   			.reading2event = {},
                },
                .comment = "Blower global operational sensor"
        },

        {} /* Terminate array with a null element */
};


/******************************************************************************
 *                      Control Definitions
 *
 * These are patterned after an RSA type machine
 ******************************************************************************/

struct sim_control sim_chassis_controls[] = {
        /* Front Panel Identify LED. User controlled. */
  	/* 0 is Off; 1 is solid on; 2 is blinking */
	{
                .index = 1,
                .control = {
                        .Num = 1,
                        .OutputType = SAHPI_CTRL_LED,
                        .Type = SAHPI_CTRL_TYPE_DISCRETE,
                        .TypeUnion.Discrete.Default = 0,
			.DefaultMode = {
				.Mode = SAHPI_CTRL_MODE_MANUAL,
				.ReadOnly = SAHPI_TRUE,
			},
			.WriteOnly = SAHPI_FALSE,
                        .Oem = 0,
                },
		.mode = SAHPI_CTRL_MODE_MANUAL,
                .comment = "Front Panel Identify LED"
        },

        {} /* Terminate array with a null element */
};

struct sim_control sim_cpu_controls[] = {

        {} /* Terminate array with a null element */
};

struct sim_control sim_dasd_controls[] = {

        {} /* Terminate array with a null element */
};

struct sim_control sim_fan_controls[] = {

        {} /* Terminate array with a null element */
};


/******************************************************************************
 *                      Annunciator Definitions
 *
 * These are completely made up as RSA has no annunciators
 ******************************************************************************/

struct sim_annunciator sim_chassis_annunciators[] = {
        {
                .index = 1,
                .annun = {
                        .AnnunciatorNum = 1,
                        .AnnunciatorType = SAHPI_ANNUNCIATOR_TYPE_AUDIBLE,
                        .ModeReadOnly = SAHPI_FALSE,
                        .MaxConditions = 2,
                        .Oem = 0,
                },
                .announs[0] = {
                        .EntryId = 1,
                        .Timestamp = 0,
                        .AddedByUser = SAHPI_FALSE,
                        .Severity = SAHPI_MAJOR,
                        .Acknowledged = SAHPI_FALSE,
                        .StatusCond = {
                                .Type = SAHPI_STATUS_COND_TYPE_SENSOR,
                                .Entity   = {
                                        .Entry = {
                                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                                {SAHPI_ENT_ROOT, 0}
                                        },
                                },
                                .DomainId = 1,
                                .ResourceId = 1,
                                .SensorNum = 1,
                                .EventState = SAHPI_ES_UNSPECIFIED,
                                .Name = {
                                        .Length = 5,
                                        .Value = "announ"
                                },
                                .Mid = 123,
                        },
                },
                .announs[1] = {
                        .EntryId = 2,
                        .Timestamp = 0,
                        .AddedByUser = SAHPI_FALSE,
                        .Severity = SAHPI_MINOR,
                        .Acknowledged = SAHPI_FALSE,
                        .StatusCond = {
                                .Type = SAHPI_STATUS_COND_TYPE_SENSOR,
                                .Entity   = {
                                        .Entry = {
                                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                                {SAHPI_ENT_ROOT, 0}
                                        },
                                },
                                .DomainId = 1,
                                .ResourceId = 1,
                                .SensorNum = 1,
                                .EventState = SAHPI_ES_UNSPECIFIED,
                                .Name = {
                                        .Length = 5,
                                        .Value = "announ"
                                },
                                .Mid = 123,
                        },
                },
                .announs[2] = {
                        .EntryId = 3,
                        .Timestamp = 0,
                        .AddedByUser = SAHPI_FALSE,
                        .Severity = SAHPI_INFORMATIONAL,
                        .Acknowledged = SAHPI_FALSE,
                        .StatusCond = {
                                .Type = SAHPI_STATUS_COND_TYPE_SENSOR,
                                .Entity   = {
                                        .Entry = {
                                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                                {SAHPI_ENT_ROOT, 0}
                                        },
                                },
                                .DomainId = 1,
                                .ResourceId = 1,
                                .SensorNum = 1,
                                .EventState = SAHPI_ES_UNSPECIFIED,
                                .Name = {
                                        .Length = 5,
                                        .Value = "announ"
                                },
                                .Mid = 123,
                        },
                },
                .comment = "Annunciator 1"
        },

        {} /* Terminate array with a null element */
};

struct sim_annunciator sim_cpu_annunciators[] = {

        {} /* Terminate array with a null element */
};

struct sim_annunciator sim_dasd_annunciators[] = {

        {} /* Terminate array with a null element */
};

struct sim_annunciator sim_fan_annunciators[] = {

        {} /* Terminate array with a null element */
};


/******************************************************************************
 *                      Watchdog Definitions
 *
 * These are completely made up as RSA has no watchdogs
 ******************************************************************************/

struct sim_watchdog sim_chassis_watchdogs[] = {
        {
                .watchdogrec = {
                        .WatchdogNum = 1,
                        .Oem = 0,
                },
                .wd = {
                        .Log = SAHPI_TRUE,
                        .Running = SAHPI_FALSE,
                        .TimerUse = SAHPI_WTU_NONE,
                        .TimerAction = SAHPI_WA_NO_ACTION,
                        .PretimerInterrupt = SAHPI_WPI_NONE,
                        .PreTimeoutInterval = 0,
                        .TimerUseExpFlags = SAHPI_WTU_NONE,
                        .InitialCount = 0,
                        .PresentCount = 0,
                },
                .comment = "Watchdog 1"
        },

        {} /* Terminate array with a null element */
};

struct sim_watchdog sim_cpu_watchdogs[] = {

        {} /* Terminate array with a null element */
};

struct sim_watchdog sim_dasd_watchdogs[] = {

        {} /* Terminate array with a null element */
};

struct sim_watchdog sim_fan_watchdogs[] = {

        {} /* Terminate array with a null element */
};



/*************************************************************************
 *                   Inventory Definitions
 *************************************************************************/

struct sim_inventory sim_chassis_inventory[] = {
        {
                .invrec = {
                        .IdrId = 1,
                        .Persistent = SAHPI_FALSE,
                        .Oem = 0,
                },
                .info = {
                        .nextareaid = 2, // change if you add more areas below
                        .idrinfo = {
                                .IdrId = 1,
                                .UpdateCount = 0,
                                .ReadOnly = SAHPI_TRUE,
                                .NumAreas = 1, // change if you want more areas below
                        },
                        .area[0] = {
                                .nextfieldid = 2, // change if you add more fields below
                                .idrareahead = {
                                        .AreaId = 1,
                                        .Type = SAHPI_IDR_AREATYPE_CHASSIS_INFO,
                                        .ReadOnly = SAHPI_TRUE,
                                        .NumFields = 1, //change if you add more fields below
                                },
                                .field[0] = {
                                        .AreaId = 1,
                                        .FieldId = 1,
                                        .Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER,
                                        .ReadOnly = SAHPI_TRUE,
                                        .Field = {
                                                .DataType = SAHPI_TL_TYPE_TEXT,
                                                .Language = SAHPI_LANG_ENGLISH,
                                                .DataLength = 6,
                                                .Data[0] = 'I',
                                                .Data[1] = 'B',
                                                .Data[2] = 'M',
                                                .Data[3] = 'X',
                                                .Data[4] = 'X',
                                                .Data[5] = 'X',
                                                .Data[6] = '\0',
                                        },
                                },
                        },
                },
                .comment = "Simulator Inv 1",
        },

        {} /* Terminate array with a null element */
};

struct sim_inventory sim_cpu_inventory[] = {

        {} /* Terminate array with a null element */
};

struct sim_inventory sim_dasd_inventory[] = {

        {} /* Terminate array with a null element */
};

struct sim_inventory sim_fan_inventory[] = {

        {} /* Terminate array with a null element */
};

