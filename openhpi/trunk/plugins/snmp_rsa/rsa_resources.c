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
                                .Entry = {}
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
                                .Entry = {}
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
								.SensorFloat32 =125,
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
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.1.0",
			/* FIXME:: SNMP is supposed to add thresholds soon */
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidUpMinor      = "\0",
					.OidUpCrit       = "\0",
				},
			},
		},
                .comment = "Planar temperature in degrees centigrade(C)."
        },
        /* ASM card thermal sensor on planar */
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
								.SensorFloat32 =125,
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
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.2.0",
			/* FIXME:: SNMP is supposed to add thresholds soon */
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidUpMinor      = "\0",
					.OidUpCrit       = "\0",
				},
			},
		},
                .comment = "ASM card temperature in degrees centigrade(C)."
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
								.SensorFloat32 =125,
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
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.3.0",
			/* FIXME:: SNMP is supposed to add thresholds soon */
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidUpMinor      = "\0",
					.OidUpCrit       = "\0",
				},
			},
		},
                .comment = "CPU area temperature in degrees centigrade(C)."
        },
        /* I/O area thermal sensor on planar */
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
								.SensorFloat32 =125,
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
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.5.0",
			/* FIXME:: SNMP is supposed to add thresholds soon */
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidUpMinor      = "\0",
					.OidUpCrit       = "\0",
				},
			},
		},
                .comment = "I/O area temperature in degrees centigrade(C)."
        },
        /* Memory area thermal sensor on planar */
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
								.SensorFloat32 =125,
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
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.1.1.6.0",
			/* FIXME:: SNMP is supposed to add thresholds soon */
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidUpMinor      = "\0",
					.OidUpCrit       = "\0",
				},
			},
		},
                .comment = "Memory area temperature in degrees centigrade(C)."
        },
        /* +5V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 6,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 5,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.1.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.1",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.1",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.1",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.1"
				},
			},
		},
                .comment = "Plus 5 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* +3.3V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 7,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 3.3,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.2.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.2",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.2",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.2",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.2"
				},
			},
		},
                .comment = "Plus 3.3 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* +12V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 8,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 12,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.3.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.3",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.3",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.3",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.3"
				},
			},
		},
                .comment = "Plus 12 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* -12V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 9,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = -12,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.4.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.4",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.4",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.4",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.4"
				},
			},
		},
                .comment = "Minus 12 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* -5V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 10,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = -5,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.5.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.5",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.5",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.5",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.5"
				},
			},
		},
                .comment = "Minus 5 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* +2.5V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 11,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 2.5,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.6.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.6",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.6",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.6",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.6"
				},
			},
		},
                .comment = "Plus 2.5 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* +1.5V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 12,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 1.5,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.7.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.7",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.7",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.7",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.7"
				},
			},
		},
                .comment = "Plus 1.5 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },
        /* +1.25V voltage sensor on the planar */
        {
                .sensor = {
                        .Num = 13,
                        .Type = SAHPI_VOLTAGE,
                        .Category = SAHPI_EC_THRESHOLD,
                        .EventCtrl = SAHPI_SEC_GLOBAL_DISABLE,
                        .Events = SAHPI_ES_LOWER_MINOR | SAHPI_ES_UPPER_MINOR,
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
								.SensorFloat32 = 1.25,
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
				.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
 				             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.FixedThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_UP_MINOR |
				              SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                         },
                        .Oem = 0
                },
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.1.2.2.1.13.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.10.8",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.6.8",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.11.8",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.1.2.20.2.1.1.7.8"
				},
			},
		},
                .comment = "Plus 1.25 Volt sensor on the planar, voltage reading expressed in volts(V)"
        },

        {} /* Terminate array with a null element */
};

/***************
 * Power Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_power_sensors[] = {

        {} /* Terminate array with a null element */
};

/***************
 * CPU Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_cpu_sensors[] = {

        {} /* Terminate array with a null element */
};

/***************
 * DASD Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_dasd_sensors[] = {

        {} /* Terminate array with a null element */
};

/***************
 * Fan Sensors
 ***************/

struct snmp_rsa_sensor snmp_rsa_fan_sensors[] = {

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
