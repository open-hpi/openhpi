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
 */

#ifndef TSENSOR_H
#define TSENSOR_H

gboolean  ifobj_force_error = 0;
gboolean  ifobj_data_force_error = 0;
gboolean  snmp_force_error = 0;

gboolean  snmp_get_string_type  = 1; /* 1 implies ASN_OCTET_STR; 0 implies ASN_INTEGER */
gchar     *snmp_value_string =  "3.2 Centigrade";
long      snmp_value_integer = 0;

/* ??? What should Sensor Status be set to since 0 is invalid option ??? */
/* Event status should be set to SAHPI_ES_UNSPECIFIED if no events */
/* then sensor status doesn't matter ???? */

struct snmp_bc_sensor test_sensor = {
	.sensor = {
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
							.SensorFloat32 = 4.4,
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
		.Oem = 0,
	},
	.bc_sensor_info = {
		.cur_state = SAHPI_ES_UNSPECIFIED,
		.mib = {
			.not_avail_indicator_num = 0,
			.write_only = 0,
			.convert_snmpstr = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32,
			.oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.6.0",
			.threshold_oids = {
				.InterpretedThresholds = {
					.OidLowMinor     = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.5",
					.OidUpMinor      = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.5",
					/*  FIXME:: Hysteresis correct? */
					.OidLowHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.5",
					.OidUpHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.5",
				},
			},
		},
			.event_array = {
				{},
			},
		},
	.comment = "Plus 2.5 Volt power supply voltage reading expressed in volts(V)",
};

struct rdr_stub {
	SaHpiRdrT              Rdr;
	struct snmp_bc_sensor  Test;
} test_rdr = {
	.Rdr = {
		.RecordId = 1,
		.RdrType = SAHPI_SENSOR_RDR,
		.Entity = {
			.Entry[0] = 
			{
				.EntityType = SAHPI_ENT_SBC_BLADE,
				.EntityLocation = 1,
			},
			{
				.EntityType = SAHPI_ENT_SUB_CHASSIS,
				.EntityLocation = 1,
			},
			{
				.EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
				.EntityLocation = 1,
			},
		},
		.IdString = {
			.DataType = SAHPI_TL_TYPE_LANGUAGE,
			.Language = SAHPI_LANG_ENGLISH,
		},
	},
};

#endif
