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
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityInstance = 0
                                }
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
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityInstance = 0
                                }
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
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityInstance = 0
                                }
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.1",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.1",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "Planar ambient temperature in degrees centigrade(C)."
        },

        /* ASM thermal sensor on planar */
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.2.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.2",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.4.2",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "ASM ambient temperature in degrees centigrade(C)."
        },

        /* CPU area thermal sensor on planar */
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.3.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.3.6",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.3.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "CPU ambient area temperature in degrees centigrade(C)."
        },

        /* I/O thermal sensor on planar */
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.4.1.0",
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

        /* System ambient thermal sensor on planar */
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.5.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.5.6",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.5.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "System ambient air temperature in degrees centigrade(C)."
        },

        /* Memory thermal sensor on planar */
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
                                .oid = ".1.3.6.1.4.1.2.3.51.1.2.1.6.1.0",
				/* FIXME:: SNMP is supposed to add thresholds soon */
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.6",
						.OidUpCrit       = ".1.3.6.1.4.1.2.3.51.1.2.20.1.1.1.1.6.4",
					},
				},
			},
			.event_array = {
				{},
			},
		},
                .comment = "I/O ambient air temperature in degrees centigrade(C)."
        },
        /* 5V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 7,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.1.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.1",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.1",
					},
				},
			},
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
                .comment = "Plus 5 Volt power supply voltage reading expressed in volts(V)"
        },
        /* 3.3V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 8,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.2.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.2",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.2",
					},
				},
			},
			.event_array = {
				{
                                        .event = "0002D400", /* EN_PFA_HI_FAULT_3_35V */
					.event_state = SAHPI_ES_UPPER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0002C480", /* EN_CUTOFF_HI_FAULT_3_35V */
					.event_state = SAHPI_ES_UPPER_CRIT,
					.recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				},
				{
					.event = "08033880", /* EN_PFA_LO_FAULT_3_35V */
					.event_state = SAHPI_ES_LOWER_MAJOR,
					.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
					.event = "0002C880", /* EN_CUTOFF_LO_FAULT_3_35V */
					.event_state = SAHPI_ES_LOWER_CRIT,
					.recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				},
				{},
			},
		},
                .comment = "Plus 3.3 Volt power supply voltage reading expressed in volts(V)"
        },
        /* 12V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 9,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.3.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.3",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.3",
					},
				},
			},
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
                .comment = "Plus 12 Volt power supply voltage reading expressed in volts(V)"
        },
        /* -12V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 10,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.4.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.4",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.4",
					},
				},
			},
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
                .comment = "Plus -12 Volt power supply voltage reading expressed in volts(V)"
        },
        /* -5V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 11,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.5.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.5",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.5",
					},
				},
			},
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
                .comment = "Plus -5 Volt power supply voltage reading expressed in volts(V)"
        },
        /* 2.5V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 12,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.6.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.6",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.6",
					},
				},
			},
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
                .comment = "Plus 2.5 Volt power supply voltage reading expressed in volts(V)"
        },
        /* 1.5V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 13,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.7.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.7",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.7",
					},
				},
			},
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
                .comment = "Plus 1.5 Volt power supply voltage reading expressed in volts(V)"
        },
        /* 1.25V voltage sensor on Chassis */
        {
                .sensor = {
                        .Num = 14,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT |
				  SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_VOLTS,
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
								.SensorFloat32 = 16,
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
                                .IsThreshold = SAHPI_TRUE,
				.TholdCapabilities = SAHPI_STC_INTERPRETED,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.8.0",
				.threshold_oids = {
					.InterpretedThresholds = {
						.OidLowMajor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.8",
						.OidUpMajor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.8",
					},
				},
			},
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
                .comment = "Plus 1.25 Volt power supply voltage reading expressed in volts(V)"
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
                                /* The MIB claims that 4 DASD units are possible
                                 * but there are events for only 2 units.
                                 */
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
                                /* The MIB claims that 4 DASD units are possible
                                 * but there are events for only 2 units.
                                 */
				{},
			},
		},
                .comment = "DASD 4 area temperature in degrees centigrade(C)."
        },

        {} /* Terminate array with a null element */
};

/***************
 * Fan Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_fan_sensors[] = {

	/* Fan 1 speed */
        {
                .sensor = {
                        .Num = 1,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.1.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 1 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 2 speed */
        {
                .sensor = {
                        .Num = 2,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.2.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 2 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 3 speed */
        {
                .sensor = {
                        .Num = 3,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.3.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 3 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 4 speed */
        {
                .sensor = {
                        .Num = 4,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.4.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 4 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 5 speed */
        {
                .sensor = {
                        .Num = 5,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.5.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 5 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 6 speed */
        {
                .sensor = {
                        .Num = 6,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.6.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 6 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 7 speed */
        {
                .sensor = {
                        .Num = 7,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.7.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 7 speed expressed in percent(%) of maximum RPM."
        },

	/* Fan 8 speed */
        {
                .sensor = {
                        .Num = 8,
                        .Type = SAHPI_FAN,
                        .Category = SAHPI_EC_UNSPECIFIED,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_UNSPECIFIED,
                        .Ignore = SAHPI_FALSE,
                        .DataFormat = {
                                .ReadingFormats = SAHPI_SRF_INTERPRETED,
                                .IsNumeric = SAHPI_TRUE,
                                .SignFormat = SAHPI_SDF_UNSIGNED,
                                .BaseUnits = SAHPI_SU_RPM,
                                .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                .ModifierUse = SAHPI_SMUU_NONE,
                                .FactorsStatic = SAHPI_TRUE,
                                .Factors = {
                                        .Linearization = SAHPI_SL_LINEAR,
                                },
                                .Percentage = SAHPI_TRUE,
				.Range = {
					.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN,
					.Max = {
						.ValuesPresent = SAHPI_SRF_INTERPRETED,
						.Interpreted = {
							.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
							.Value = {
								.SensorFloat32 = 100,
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
                                .IsThreshold = SAHPI_FALSE
                        },
                        .Oem = 0
                },
		.rsa_sensor_info = {
			.cur_state = SAHPI_ES_UNSPECIFIED,
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
				.oid = ".1.3.6.1.4.1.2.3.51.1.2.3.8.0",
			},
			.event_array = {
				{},
			},
		},
                .comment = "Fan 8 speed expressed in percent(%) of maximum RPM."
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
		.rsa_inventory_info = {		
			.mib = {
				.not_avail_indicator_num = 0,
				.write_only = 0,
				.inventory_type = SAHPI_INVENT_RECTYPE_CHASSIS_INFO,
				.chassis_type = SAHPI_INVENT_CTYP_TOWER,
				.oid = {
                                        .OidMfgDateTime = '\0',   /* Set to SAHPI_TIME_UNSPECIFIED */
                                        .OidManufacturer = '\0',
                                        .OidProductName = '\0',
                                        .OidProductVersion = ".1.3.6.1.4.1.2.3.51.1.2.21.1.1.1.0",
                                        .OidModelNumber = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.2.0",
                                        .OidSerialNumber = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.3.0",
                                        .OidPartNumber = ".1.3.6.1.4.1.2.3.51.1.2.21.2.1.1.0",
                                        .OidFileId = '\0',
                                        .OidAssetTag = '\0',
					/* UUID .1.3.6.1.4.1.2.3.51.1.2.21.2.1.4.0  */
				}
			},
		},
                .comment = "Chassis VPD",
        },

        {} /* Terminate array with a null element */
};
