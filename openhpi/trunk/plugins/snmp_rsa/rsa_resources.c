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
 * Authors:
 *      W. David Ashley <dashley@us.ibm.com>
 */

/**************************************************************************
 * This source file defines the resource arrays declared in rsa_resources.h
 *************************************************************************/

#include <SaHpi.h>
#include <rsa_resources.h>

/*************************************************************************
 * RESTRICTIONS!!!
 *
 * - If IsThreshold=SAHPI_TRUE for an interpreted sensor, 
 *   Range.Max.Interpreted.Type must be defined for snmp_rsa.c 
 *   get_interpreted_thresholds to work.
 * - Digital controls must be integers and depend on SaHpiStateDigitalT
 *************************************************************************/

/*************************************************************************
 *                   Resource Definitions
 *************************************************************************/
struct snmp_rpt snmp_rpt_array[] = {
	/* Chassis */
        {
                .rpt = {
			.ResourceInfo = {
                                .ManufacturerId = 2,
                        },
                        .ResourceEntity = {
                                .Entry = {}
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_SEL |
                                                SAHPI_CAPABILITY_FRU |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_CRITICAL,
                },
		.rsa_res_info = {
                        .mib = {
                                .OidHealth = ".1.3.6.1.4.1.2.3.51.1.2.7.1.0",
                                .HealthyValue = 255,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
                        },
			.def_state = SAHPI_HS_STATE_ACTIVE_HEALTHY,
                        .event_array = {},
		},
                .comment = "Chassis"
        },
        /* CPUs */
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = 2,
                        },
                        .ResourceEntity = {
                                .Entry[0] = 
				{
                                        .EntityType = SAHPI_ENT_PROCESSOR,
                                        .EntityInstance = RSA_HPI_INSTANCE_BASE
                                },
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_INVENTORY_DATA |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
                 },
		.rsa_res_info = {
                        .mib = {
                                .OidHealth = '\0',
                                .HealthyValue = 0,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
                        },
			.def_state = SAHPI_HS_STATE_ACTIVE_HEALTHY,
                        .event_array = {},
		},
                .comment = "CPUs"
        },
        /* DASD */
        {
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = 2,
                        },
                        .ResourceEntity = {
                                .Entry[0] = 
				{
                                        .EntityType = SAHPI_ENT_DISK_BAY,
                                        .EntityInstance = RSA_HPI_INSTANCE_BASE
                                },
			},
                        .ResourceCapabilities = SAHPI_CAPABILITY_INVENTORY_DATA |
			                        SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
                 },
		.rsa_res_info = {
                        .mib = {
                                .OidHealth = '\0',
                                .HealthyValue = 0,
                                .OidReset = '\0',
                                .OidPowerState = '\0',
                                .OidPowerOnOff = '\0',
                        },
			.def_state = SAHPI_HS_STATE_ACTIVE_HEALTHY,
                        .event_array = {},
		},
                .comment = "DASD"
        },

        {} /* Terminate array with a null element */
};

/******************************************************************************
 *                      Sensor Definitions
 ******************************************************************************/

/*****************
 * Chassis Sensors
 *****************/

struct snmp_rsa_sensor snmp_rsa_chassis_sensors[] = {

        /* Main thermal sensor on planar */
        {
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.5.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.6",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "System ambient air temperature in degrees centigrade(C)."
        },

        {} /* Terminate array with a null element */
};

/***************
 * CPU Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_cpu_thermal_sensors[] = {
        /* CPU 1 area thermal sensor */
        {
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.1",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.1",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 1 temperature in degrees centigrade(C)."
        },
        /* CPU 2 area thermal sensor */
        {
                .sensor = {
                        .Num = 2,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.2.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.2",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.2",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 2 temperature in degrees centigrade(C)."
        },
        /* CPU 3 area thermal sensor */
        {
                .sensor = {
                        .Num = 3,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.3.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.3",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.3",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 3 temperature in degrees centigrade(C)."
        },
        /* CPU 4 area thermal sensor */
        {
                .sensor = {
                        .Num = 4,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.4.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.4",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 4 temperature in degrees centigrade(C)."
        },
        /* CPU 5 area thermal sensor */
        {
                .sensor = {
                        .Num = 5,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.5.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.5",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.5",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 5 temperature in degrees centigrade(C)."
        },
        /* CPU 6 area thermal sensor */
        {
                .sensor = {
                        .Num = 6,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.6.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.6",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.6",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 6 temperature in degrees centigrade(C)."
        },
        /* CPU 7 area thermal sensor */
        {
                .sensor = {
                        .Num = 7,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.7.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.7",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.7",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 7 temperature in degrees centigrade(C)."
        },
        /* CPU 8 area thermal sensor */
        {
                .sensor = {
                        .Num = 8,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.8.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.6.8",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.5.1.1.4.8",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU 8 temperature in degrees centigrade(C)."
        },

        {} /* Terminate array with a null element */
};

/***************
 * DASD Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_dasd_thermal_sensors[] = {
        /* DASD 1 area thermal sensor */
        {
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.1",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.1",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "DASD area temperature in degrees centigrade(C)."
        },
        /* DASD 2 area thermal sensor */
        {
                .sensor = {
                        .Num = 2,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.2.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.2",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.2",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "DASD 2 area temperature in degrees centigrade(C)."
        },
        /* DASD 3 area thermal sensor */
        {
                .sensor = {
                        .Num = 3,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.3.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.3",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.3",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "DASD 3 area temperature in degrees centigrade(C)."
        },
        /* DASD 4 area thermal sensor */
        {
                .sensor = {
                        .Num = 4,
                        .Type = SAHPI_TEMPERATURE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        /* FIXME:: Change when SNMP adds thresholds */
                        .Events = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_DEGREES_C,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_FALSE,
                                .Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 125,
							}	
						},
					},
					.Min = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 0,
							}	
						},
					},
				},
                        },
                        .ThresholdDefn = {
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
                                .IsThreshold = SAHPI_FALSE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
                                 /* FIXME:: SNMP is supposed to add thresholds soon */
				.ReadThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
				.FixedThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_CRIT,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.4.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.6.4",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.6.1.1.4.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "DASD 4 area temperature in degrees centigrade(C)."
        },

        {} /* Terminate array with a null element */
};

/*************************************************************************
 *                   Control Definitions
 *************************************************************************/

/******************
 * Chassis Controls
 ******************/

struct snmp_rsa_control snmp_rsa_chassis_controls[] = {

        {} /* Terminate array with a null element */
};

/****************
 * Power Controls
 ****************/

struct snmp_rsa_control snmp_rsa_power_controls[] = {

        {} /* Terminate array with a null element */
};
 
/*************************************************************************
 *                   Inventory Definitions
 *************************************************************************/

/*************
 * Chassis VPD
 *************/

struct snmp_rsa_inventory snmp_rsa_chassis_inventories[] = {
        {
                .inventory = {
			.EirId = 1,
                        .Oem = 0,
                },
                .mib = {
                        .not_avail_indicator_num = 0,
                        .write_only = 0,
                	.inventory_type = SAHPI_INVENT_RECTYPE_CHASSIS_INFO,
                	.chassis_type = SAHPI_INVENT_CTYP_RACKMOUNT,
			.oid = {
                        	.OidMfgDateTime = '\0',   /* Set to SAHPI_TIME_UNSPECIFIED */
                        	.OidManufacturer = '\0',
                        	.OidProductName = '\0',
                        	.OidProductVersion = ".1.3.6.1.4.1.2.3.51.1.2.21.1.1.1.0",
                        	.OidModelNumber = '\0',
                        	.OidSerialNumber = '\0',
                        	.OidPartNumber = '\0',
                        	.OidFileId = '\0',
                        	.OidAssetTag = '\0',
                	}
		},
                .comment = "Chassis VPD",
        },

        {} /* Terminate array with a null element */
};

/*********
 * Fan VPD             
 **********/

struct snmp_rsa_inventory snmp_rsa_fan_inventories[] = {

        {} /* Terminate array with a null element */
};
