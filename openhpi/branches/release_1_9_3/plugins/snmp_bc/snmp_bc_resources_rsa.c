/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com> 
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include  <snmp_bc_plugin.h>

/*************************************************************************
 *                   Resource Definitions
 *************************************************************************/
struct snmp_rpt snmp_bc_rpt_array_rsa[] = {
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

			/* FIXME:: SAHPI_CAPABILITY_FRU??? adapter case??? */
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVENT_LOG |
			                        SAHPI_CAPABILITY_EVT_DEASSERTS | 
			                        SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_FRU |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_CRITICAL,
			.ResourceFailed = SAHPI_FALSE,
                },
		.res_info = {
                        .mib = {
                                .OidHealth = ".1.3.6.1.4.1.2.3.51.1.2.7.1.0",
                                .HealthyValue = 255,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
				.OidUuid = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.4.0",
                        },
                        .event_array = {},
		},
                .comment = "RSA Chassis"
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
                                        .EntityLocation = SNMP_BC_HPI_LOCATION_BASE
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
			},
			/* FIXME:: SAHPI_CAPABILITY_RESET ? */
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                 },
		.res_info = {
                        .mib = {
                                .OidHealth = '\0',
                                .HealthyValue = 0,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
				.OidUuid = '\0',
                       },
                        .event_array = {},
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
                                        .EntityLocation = SNMP_BC_HPI_LOCATION_BASE
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0
                                }
			},
			/* FIXME:: SAHPI_CAPABILITY_EVT_DEASSERTS; SAHPI_CAPABILITY_RESET */
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
 			.ResourceFailed = SAHPI_FALSE,
                },
		.res_info = {
                        .mib = {
                                .OidHealth = '\0',
                                .HealthyValue = 0,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
				.OidUuid = '\0',
                       },
                        .event_array = {},
		},
                .comment = "DASD"
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
			                .EntityLocation = SNMP_BC_HPI_LOCATION_BASE
				},
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
			},
			/* FIXME:: SAHPI_CAPABILITY_RESET?? */
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
 			.ResourceFailed = SAHPI_FALSE,
                },
		.res_info = {
			.mib = {
				.OidHealth = '\0',
				.HealthyValue = 0,
				.OidReset = '\0',
				.OidPowerState = '\0',
				.OidPowerOnOff = '\0',
				.OidUuid = '\0',
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan"
        },

        {} /* Terminate array with a null element */
};

/******************************************************************************
 *                      Sensor Definitions
 ******************************************************************************/

/*****************
 * Chassis Sensors
 *****************/

struct snmp_bc_sensor snmp_bc_chassis_sensors_rsa[] = {

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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.1",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.1",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.1",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "Planar temperature sensor"
        },
        /* ASM thermal sensor on planar */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.2",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.2",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.2",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "ASM temperature sensor"
        },
        /* CPU area thermal sensor on planar */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.3.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.3",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.3",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.3",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "Planar CPU area temperature sensor"
        },
        /* I/O thermal sensor on planar */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.4",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.4",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.4",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "Planar I/O area temperature sensor"
        },
        /* System ambient thermal sensor on planar */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.5.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.5",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.5",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.5",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "Planar system ambient temperature sensor"
        },
        /* Memory thermal sensor on planar */
        {
		.index = 6,
                .sensor = {
                        .Num = 6,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.6.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.6",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.6",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.7.6",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{},
			},
		},
                .comment = "Planar memory temperature sensor"
        },
        /* 5V voltage sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.1.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.1",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.1",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.1", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.1", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.1",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.1",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "06035500", /* EN_PFA_HI_FAULT_PLANAR_5V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "06034480", /* EN_CUTOFF_HI_FAULT_PLANAR_5V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "06035800", /* EN_PFA_LO_FAULT_PLANAR_5V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "06034800", /* EN_CUTOFF_LO_FAULT_PLANAR_5V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "5 volt sensor"
        },
        /* 3.3V voltage sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.2.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.2",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.2",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.2", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.2", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.2",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.2",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "08033480", /* EN_PFA_HI_FAULT_3_35V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08032480", /* EN_CUTOFF_HI_FAULT_3_35V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08033880", /* EN_PFA_LO_FAULT_3_35V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08032880", /* EN_CUTOFF_LO_FAULT_3_35V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "3.3 volt sensor"
        },
        /* 12V voltage sensor on Chassis */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.3.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.3",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.3",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.3", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.3", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.3",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.3",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "06037500", /* EN_PFA_HI_FAULT_12V_PLANAR */
			                .event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
		                },
		                {
			                .event = "06036480", /* EN_CUTOFF_HI_FAULT_12V_PLANAR */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "06037800", /* EN_PFA_LO_FAULT_12V_PLANAR */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "06036800", /* EN_CUTOFF_LO_FAULT_12V_PLANAR */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "12 volt sensor"
        },
        /* -12V voltage sensor on Chassis */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.4.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.4",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.4",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.4", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.4", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.4",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.4",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "0803F500", /* EN_PFA_HI_FAULT_N12V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803E480", /* EN_CUTOFF_HI_FAULT_N12V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0803F800", /* EN_PFA_LO_FAULT_N12V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803E800", /* EN_CUTOFF_LO_FAULT_N12V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "-12 volt sensor"
        },
        /* -5V voltage sensor on Chassis */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.5.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.5",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.5",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.5", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.5", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.5",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.5",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.event_array = {
				{
					.event = "0803D500", /* EN_PFA_HI_FAULT_N5V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803C480", /* EN_CUTOFF_HI_FAULT_N5V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0803D800", /* EN_PFA_LO_FAULT_N5V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0803C800", /* EN_CUTOFF_LO_FAULT_N5V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "-5 volt sensor"
        },
        /* 2.5V voltage sensor on Chassis */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.6.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.6",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.6",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.6", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.6", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.6",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.6",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "08031480", /* EN_PFA_HI_FAULT_2_5V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08030480", /* EN_CUTOFF_HI_FAULT_2_5V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08031880", /* EN_PFA_LO_FAULT_2_5V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08030880", /* EN_CUTOFF_LO_FAULT_2_5V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
		.comment = "2.5 volt sensor"
	},
        /* 1.5V voltage sensor on Chassis */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.7.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.7",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.7",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.7", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.7", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.7",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.7",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
                                         SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "08041400", /* EN_PFA_HI_FAULT_1_5V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08040480", /* EN_CUTOFF_HI_FAULT_1_5V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08041800", /* EN_PFA_LO_FAULT_1_5V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08040880", /* EN_CUTOFF_LO_FAULT_1_5V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
		.comment = "1.5 volt sensor"
        },
        /* 1.25V voltage sensor on Chassis */
        {
		.index = 14,
                .sensor = {
                        .Num = 14,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.8.0",
				.threshold_oids = {
					.LowCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.8.8",
					.LowMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.8",
					.UpCritical  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.4.8", 
					.UpMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.8", 
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.8",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.8",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.sensor_enabled = SAHPI_TRUE,
			.events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
			                 SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
					.event = "08001400", /* EN_PFA_HI_FAULT_1_25V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08000480", /* EN_CUTOFF_HI_FAULT_1_25V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08001800", /* EN_PFA_LO_FAULT_1_25V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "08000880", /* EN_CUTOFF_LO_FAULT_1_25V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
		.comment = "1.25 volt sensor"
        },

        {} /* Terminate array with a null element */
};

/*************
 * CPU Sensors
 *************/

struct snmp_bc_sensor snmp_bc_cpu_thermal_sensors_rsa[] = {
        /* CPU 1 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.1",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.1",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.1",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D501", /* EN_PFA_HI_OVER_TEMP_CPU1 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C481", /* EN_CUTOFF_HI_OVER_TEMP_CPU1 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C401", /* EN_PROC_HOT_CPU1 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C081", /* EN_OVER_TEMP_CPU1 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 1 area temperature sensor"
        },
        /* CPU 2 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.2.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.2",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.2",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.2",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D502", /* EN_PFA_HI_OVER_TEMP_CPU2 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C482", /* EN_CUTOFF_HI_OVER_TEMP_CPU2 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C402", /* EN_PROC_HOT_CPU2 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421D082", /* EN_THERM_TRIP_CPU2 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 2 area temperature sensor"
        },
        /* CPU 3 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.3.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.3",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.3",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.3",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D503", /* EN_PFA_HI_OVER_TEMP_CPU3 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C483", /* EN_CUTOFF_HI_OVER_TEMP_CPU3 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C403", /* EN_PROC_HOT_CPU3 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C083", /* EN_OVER_TEMP_CPU3 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 3 area temperature sensor"
        },
        /* CPU 4 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.4.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.4",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.4",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.4",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D504", /* EN_PFA_HI_OVER_TEMP_CPU4 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C484", /* EN_CUTOFF_HI_OVER_TEMP_CPU4 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C404", /* EN_PROC_HOT_CPU4 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421D084", /* EN_THERM_TRIP_CPU4 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 4 area temperature sensor"
        },
        /* CPU 5 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.5.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.5",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.5",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.5",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D505", /* EN_PFA_HI_OVER_TEMP_CPU5 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C485", /* EN_CUTOFF_HI_OVER_TEMP_CPU5 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C405", /* EN_PROC_HOT_CPU5 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C085", /* EN_OVER_TEMP_CPU5 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 5 area temperature sensor"
        },
        /* CPU 6 area thermal sensor */
        {
		.index = 6,
                .sensor = {
                        .Num = 6,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.6.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.6",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.6",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.6",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D506", /* EN_PFA_HI_OVER_TEMP_CPU6 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C486", /* EN_CUTOFF_HI_OVER_TEMP_CPU6 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C406", /* EN_PROC_HOT_CPU6 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421D086", /* EN_THERM_TRIP_CPU6 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 6 area temperature sensor"
        },
        /* CPU 7 area thermal sensor */
        {
		.index = 7,
                .sensor = {
                        .Num = 7,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.7.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.7",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.7",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.7",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,	
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D507", /* EN_PFA_HI_OVER_TEMP_CPU7 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C487", /* EN_CUTOFF_HI_OVER_TEMP_CPU7 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C407", /* EN_PROC_HOT_CPU7 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C087", /* EN_OVER_TEMP_CPU7 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 7 area temperature sensor"
        },
        /* CPU 8 area thermal sensor */
        {
		.index = 8,
                .sensor = {
                        .Num = 8,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.8.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.8",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.8",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.7.8",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0421D508", /* EN_PFA_HI_OVER_TEMP_CPU8 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0421C488", /* EN_CUTOFF_HI_OVER_TEMP_CPU8 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421C408", /* EN_PROC_HOT_CPU8 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "0421D088", /* EN_THERM_TRIP_CPU8 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "CPU 8 area temperature sensor"
        },

        {} /* Terminate array with a null element */
};

/**************
 * DASD Sensors
 **************/

struct snmp_bc_sensor snmp_bc_dasd_thermal_sensors_rsa[] = {
        /* DASD 1 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.1.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.1",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.1",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.7.1",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0681C403", /* EN_PFA_HI_OVER_TEMP_DASD1 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0681C483", /* EN_CUTOFF_HI_OVER_TEMP_DASD1 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "DASD 1 area temperature sensor"
        },
        /* DASD 2 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.2.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.2",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.2",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.7.2",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
				{
                                        .event = "0681D491", /* EN_PFA_HI_OVER_TEMP_DASD2 */
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0681C491", /* EN_CUTOFF_HI_OVER_TEMP_DASD2 */
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR |
					               SAHPI_ES_UPPER_MINOR,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{},
			},
		},
                .comment = "DASD 2 area temperature sensor"
        },
        /* DASD 3 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.3.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.3",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.3",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.7.3",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
                                /* The MIB claims that 4 DASD units are possible
                                 * but there are events for only 2 units.
                                 */
				{},
			},
		},
                .comment = "DASD 3 area temperature sensor"
        },
        /* DASD 4 area thermal sensor */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.4.0",
				.threshold_oids = {
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.4",
					.UpCritical = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.4",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.7.4",
				},
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
                        .assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.event_array = {
                                /* The MIB claims that 4 DASD units are possible
                                 * but there are events for only 2 units.
                                 */
				{},
			},
		},
                .comment = "DASD 4 area temperature sensor"
        },

        {} /* Terminate array with a null element */
};

/*************
 * Fan Sensors
 *************/

struct snmp_bc_sensor snmp_bc_fan_sensors_rsa[] = {

	/* Fan 1 speed */
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.1.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 1 fan speed - percent of maximum RPM"
        },
	/* Fan 2 speed */
        {
		.index = 2,
                .sensor = {
                        .Num = 2,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.2.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 2 fan speed - percent of maximum RPM"
        },
	/* Fan 3 speed */
        {
		.index = 3,
                .sensor = {
                        .Num = 3,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.3.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 3 fan speed - percent of maximum RPM"
        },
	/* Fan 4 speed */
        {
		.index = 4,
                .sensor = {
                        .Num = 4,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.4.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 4 fan speed - percent of maximum RPM"
        },
	/* Fan 5 speed */
        {
		.index = 5,
                .sensor = {
                        .Num = 5,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.5.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 5 fan speed - percent of maximum RPM"
        },
	/* Fan 6 speed */
        {
		.index = 6,
                .sensor = {
                        .Num = 6,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.6.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 6 fan speed - percent of maximum RPM"
        },
	/* Fan 7 speed */
        {
		.index = 7,
                .sensor = {
                        .Num = 7,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.7.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 7 fan speed - percent of maximum RPM"
        },
	/* Fan 8 speed */
        {
		.index = 8,
                .sensor = {
                        .Num = 8,
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
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.8.0",
			},
			.cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_PRED_FAILURE_ASSERT,
			.deassert_mask = SAHPI_ES_PRED_FAILURE_ASSERT,
			.event_array = {
				{},
			},
		},
                .comment = "Blower 8 fan speed - percent of maximum RPM"
        },

        {} /* Terminate array with a null element */
};

/*************************************************************************
 *                   Control Definitions
 *************************************************************************/

/******************
 * Chassis Controls
 ******************/

/* No controls are defined */

/*************************************************************************
 *                   Inventory Definitions
 *************************************************************************/

/*************
 * Chassis VPD
 *************/

struct snmp_bc_inventory snmp_bc_chassis_inventories_rsa[] = {
        {
                .inventory = {
			.IdrId = 1,
                        .Oem = 0,
                },
		.inventory_info = {		
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = SAHPI_FALSE,
				.area_type = SAHPI_IDR_AREATYPE_CHASSIS_INFO,
				.oid = {
					.OidChassisType = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.2.0",
                                        .OidMfgDateTime = '\0',   /* Set to SAHPI_TIME_UNSPECIFIED */
                                        .OidManufacturer = '\0',
                                        .OidProductName = '\0',
                                        .OidProductVersion = ".1.3.6.1.4.1.2.3.51.1.2.21.1.1.1.0",
                                        .OidSerialNumber = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.3.0",
                                        .OidPartNumber = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.1.0",
                                        .OidFileId = '\0',
                                        .OidAssetTag = '\0',
				}
			},
		},
                .comment = "RSA VPD",
        },

        {} /* Terminate array with a null element */
};
