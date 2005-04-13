/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <http://dague.net/sean>
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */


#include <sim_sensors.h>
#include <sim_init.h>
#include <sim_resources.h>
#include <rpt_utils.h>

SaErrorT sim_discover_sensors(RPTable *rpt)
{
         SaHpiRptEntryT *res;


	 /* add to first resource */
	res = oh_get_resource_next(rpt, SAHPI_FIRST_ENTRY);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	printf("I am running sim_discover_sensors");
	new_sensor(rpt, res->ResourceId, 1); /* add #1 ... */
	new_sensor(rpt, res->ResourceId, 5); /* add #5 ... */
		       
        /* add to second resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
        new_sensor(rpt, res->ResourceId, 1);
        new_sensor(rpt, res->ResourceId, 2);
        new_sensor(rpt, res->ResourceId, 3);

        /* add to third resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
        new_sensor(rpt, res->ResourceId, 2); 
        new_sensor(rpt, res->ResourceId, 7);

        /* add to fourth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
        new_sensor(rpt, res->ResourceId, 3); 
        new_sensor(rpt, res->ResourceId, 5);

        /* add to fifth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
        new_sensor(rpt, res->ResourceId, 2); 
     
        /* add to sixth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}		
        new_sensor(rpt, res->ResourceId, 1); 
        new_sensor(rpt, res->ResourceId, 2);
        new_sensor(rpt, res->ResourceId, 6);
        new_sensor(rpt, res->ResourceId, 7);

        /* add to seventh resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
        new_sensor(rpt, res->ResourceId, 4); 
        new_sensor(rpt, res->ResourceId, 2);


        return 0; 

}

SaErrorT new_sensor(RPTable *rptcache, SaHpiResourceIdT ResId, int Index){
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;

	// Copy information from rdr array to res_rdr
         res_rdr.RdrType = SAHPI_SENSOR_RDR;
         memcpy(&res_rdr.RdrTypeUnion.SensorRec, &dummy_voltage_sensors[Index].sensor, sizeof(SaHpiSensorRecT));

	 oh_init_textbuffer(&res_rdr.IdString);
	 oh_append_textbuffer(&res_rdr.IdString, dummy_voltage_sensors[Index].comment);

         res_rdr.IsFru = 1;
         res_rdr.RdrTypeUnion.SensorRec.Num = sim_get_next_sensor_num(rptcache, ResId, res_rdr.RdrTypeUnion.SensorRec.Type);
         res_rdr.RecordId = get_rdr_uid(res_rdr.RdrType, res_rdr.RdrTypeUnion.SensorRec.Num);
	 
	 RptEntry = oh_get_resource_by_id(rptcache, ResId);
	 if(!RptEntry){
		 dbg("NULL rpt pointer\n");
	 }
	 else{
	 res_rdr.Entity = RptEntry->ResourceEntity;
	 }

         oh_add_rdr(rptcache, ResId, &res_rdr, NULL, 0);

         return 0;
}

int sim_get_next_sensor_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type)
{
	int i=0;
	SaHpiRdrT *RdrEntry;
 
	RdrEntry = oh_get_rdr_next(rptcache, ResId, SAHPI_FIRST_ENTRY);

	while(RdrEntry){
		if (RdrEntry->RdrType == type){
			i++;
		}
		if (RdrEntry->RecordId != 0){
			RdrEntry = oh_get_rdr_next(rptcache, ResId, RdrEntry->RecordId);
		}
	}

	return i;
	
	if(!(oh_get_rdr_by_type(rptcache, ResId, type, i))){
		printf("I hit sim_get_next_sensor_num\n");
		return i;
	}
	else{
		while(oh_get_rdr_by_type(rptcache, ResId, type, i)){
			i++;
		}
		
		return i;
	}
}

/*************************************************************************
 *                      Sensor Definitions
 *************************************************************************/

/*****************
 * Chassis Sensors
 *****************/

struct dummy_sensor dummy_voltage_sensors[] = { 


    /* -5V voltage sensor on Management Module */
        {
	.index = 0,
        .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 1,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0]= 0,
                                               	},
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 3,
							.SensorUint64 = 0,
							.SensorFloat64 = -6.7,
							.SensorBuffer[0] = 0,
						 },
					},
										
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 2,
							.SensorFloat64 = -5,
							.SensorBuffer[0] = 0,
						},
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
                                },
				.AccuracyFactor = 45.3,
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
				.Nonlinear = 1,
                                /* Default HDW thresholds:
                                   Warning -5.50<>-4.75; Warning Reset -4.85<>-5.15 */
                         },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.5.0",
                                .threshold_oids = {
				.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.4",
				.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.4",
			       	.TotalPosThdHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.4",
				.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.4",
                                },
			.threshold_write_oids = {},
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
                                        .event = "0803C480", /* EN_CUTOFF_HI_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT| SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0803C800", /* EN_CUTOFF_LO_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "0803D500", /* EN_PFA_HI_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
					.event = "803C900", /* EN_MAJOR_LO_FAULT_N5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
                .comment = "Chassis -5 volt sensor"
        },
 
     /* Blade's 1.25V voltage sensor */
        {
		.index = 1,
		.sensor = {
			.Num = 1,
			.Type = SAHPI_VOLTAGE,
			.Category = SAHPI_EC_THRESHOLD,
			.EnableCtrl = SAHPI_FALSE,
			.EventCtrl = SAHPI_SEC_READ_ONLY,
			.Events = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR |
				SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR,
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
							.SensorInt64 = 1,
							.SensorUint64 = 3,
							.SensorFloat64 = 3.3,
							.SensorBuffer[0] = 0,
						},
					},
					.Min = {
					  	.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 2,
							.SensorUint64 = 2,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
										
					.Nominal = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 4,
							.SensorUint64 = 3,
							.SensorFloat64 = 1.25,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
				},
				.AccuracyFactor = 42.9,
			},
			.ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
				.ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |					                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
				/* Default HDW thresholds: Warning 1.10<>1.4 */
				.Nonlinear = 1,
			},
			.Oem = 0,
		},
		.sensor_info = {
			.mib = {
			.not_avail_indicator_num = 0,
				.write_only = SAHPI_FALSE,
				.oid = "discovered",
				.threshold_oids = {
					.UpMajor  = "discovered",
					.LowMajor = "discovered",
				},
				.threshold_write_oids = {},
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
				.event = "08000481", /* EN_CUTOFF_HI_FAULT_1_2V */
				.event_assertion = SAHPI_TRUE,
				.event_res_failure = SAHPI_TRUE,
				.event_res_failure_unexpected = SAHPI_TRUE,
				.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
				.event = "08000881", /* EN_CUTOFF_LO_FAULT_1_2V */
				.event_assertion = SAHPI_TRUE,
				.event_res_failure = SAHPI_TRUE,
				.event_res_failure_unexpected = SAHPI_TRUE,
				.event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
				.event = "08001401", /* EN_PFA_HI_FAULT_1_2V */
				.event_assertion = SAHPI_TRUE,
				.event_res_failure = SAHPI_FALSE,
				.event_res_failure_unexpected = SAHPI_FALSE,
				.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
				.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{
				.event = "08001801", /* EN_PFA_LO_FAULT_1_2V */
				.event_assertion = SAHPI_TRUE,
				.event_res_failure = SAHPI_FALSE,
				.event_res_failure_unexpected = SAHPI_FALSE,
				.event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
				.recovery_state = SAHPI_ES_UNSPECIFIED,
				},
				{},
			},
			.reading2event = {},
		},
	.comment = "Blade IPMI 1.25 volt sensor",
	},

/* Blade's 1.5V voltage sensor */
	{
		.index = 2,
                .sensor = {
			.Num = 1,
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
							.SensorInt64 = 2,
							.SensorUint64 = 3,
							.SensorFloat64 = 4.4,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1,
							.SensorUint64 = 2, 
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 3,
							.SensorUint64 = 2,
							.SensorFloat64 = 1.5,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
                                },
				.AccuracyFactor = 29.4,
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold  = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR,
                                .WriteThold = 0,
                                /* Default HDW thresholds: Warning 1.32<>1.68 */
				.Nonlinear = 0,
                        },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.11.x",
                                .threshold_oids = {
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.16.x",
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.17.x",
                                },
				.threshold_write_oids = {},
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
                                        .event = "0A041C00", /* EN_IO_1_5V_WARNING_HI */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
				{
                                        .event = "0A040C00", /* EN_IO_1_5V_WARNING_LOW */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "08041400", /* EN_PFA_HI_FAULT_1_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "08041800", /* EN_PFA_LO_FAULT_1_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
	.reading2event = {},
                 },
                .comment = "Blade 1.5 volt sensor"
        },
 
      /* 1.8V voltage sensor on Management Module */
        {
		.index = 3,
                .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 3,
							.SensorUint64 = -3,
							.SensorFloat64 = 4.4,
							.SensorBuffer[0] = 0,
						},
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 2,
							.SensorUint64 = 4,
							.SensorFloat64 = 2.3,
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 7,
							.SensorUint64 = 2,
							.SensorFloat64 = 1.8,
							.SensorBuffer[0] = 0,
						},
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
                              },
			      .AccuracyFactor = 87.2,
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
				.WriteThold = 0,
				.Nonlinear = 1,
                         },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.8.0",
				/* Default HDW thresholds: Warning 1.62<>1.89; Warning Reset 1.86<>1.74 */
                                .threshold_oids = {
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.6",
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.6",
					.TotalPosThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.6",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.6",
                                },
				.threshold_write_oids = {},
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
                                        .event = "0807A480", /* EN_CUTOFF_HI_FAULT_1_8V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0807A880", /* EN_CUTOFF_LO_FAULT_1_8V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "FF07A500", /* EN_MAJOR_HI_FAULT_1_8V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "FF07A900", /* EN_MAJOR_LO_FAULT_1_8V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
                .comment = "Chassis 1.8 volt sensor"
        },
   

/* 2.5V voltage sensor template*/
        {
		.index = 4,
                .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 3,
							.SensorUint64 = 2,
							.SensorFloat64 = 4.4,
							.SensorBuffer[0] = 0,
						},
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 8,
							.SensorUint64 = 1,
							.SensorFloat64 = 5.7,
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 2,
							.SensorUint64 = 5,
							.SensorFloat64 = 2.5,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
                                },
				.AccuracyFactor = 3.4,
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
					SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                                /* Default HDW thresholds:
                                   Warning 2.25<>2.63; Warning Reset 2.58<>2.42 */
				.Nonlinear = 1,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.6.0",
                                .threshold_oids = {
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.5",
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.5",
					.TotalPosThdHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.5",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.5",
                                },
				.threshold_write_oids = {},
                        
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
                                        .event = "08030480", /* EN_CUTOFF_HI_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "08030880", /* EN_CUTOFF_LO_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "FF031480", /* EN_PFA_HI_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "FF031880", /* EN_PFA_LO_FAULT_2_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
                .comment = "Chassis 2.5 volt sensor"
        },


     /* 3.3V voltage sensor on Management Module */
{
	.index = 5,
                .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 3,
							.SensorUint64 = 0,
							.SensorFloat64 = 3.6,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 2,
							.SensorUint64 = 4,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 9,
							.SensorUint64 = 8,
							.SensorFloat64 = 3.3,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
                                },
				.AccuracyFactor = 48.8,
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
					SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                                /* Default HDW thresholds:
                                   Warning 3.00<>3.47; Warning Reset 3.40<>3.20 */
				.Nonlinear = 1,
                        },
                        .Oem = 0,
                },
		.sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.2.0",
                                .threshold_oids = {
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.2",
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.2",
					.TotalPosThdHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.2",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.2",
                                },
				.threshold_write_oids = {},
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
                                        .event = "08032480", /* EN_CUTOFF_HI_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "08032880", /* EN_CUTOFF_LO_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "FF033480", /* EN_PFA_HI_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
				{
                                        .event = "FF032900", /* EN_MAJOR_LO_FAULT_3_35V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
		.comment = "Chassis 3.3 volt sensor"
},

     /* 5V voltage sensor on Management Module */
        {
		.index = 6,
                .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 4,
							.SensorUint64 = 2,
							.SensorFloat64 = 6.7,
							.SensorBuffer[0] = 0,
						},
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 7,
							.SensorUint64 = 9,
							.SensorFloat64 = 0, 
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 5,
							.SensorUint64 = 2,
							.SensorFloat64 = 5,
							.SensorBuffer[0] = 0,
						},
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
				},
				.AccuracyFactor = 78.4,
			},
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                                /* Default HDW thresholds:
                                   Warning 4.50<>5.25; Warning Reset 5.15<>4.85 */
				.Nonlinear = 1,
                         },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.1.0",
                                .threshold_oids = {
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.1",
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.1",
					.TotalPosThdHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.1",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.1",
                                },
				.threshold_write_oids = {},
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
                                        .event = "06034480", /* EN_CUTOFF_HI_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "06034800", /* EN_CUTOFF_LO_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "06034500", /* EN_MAJOR_HI_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "06035800", /* EN_PFA_LO_FAULT_PLANAR_5V */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
                .comment = "Chassis 5 volt sensor"
        },

         /* 12V voltage sensor on Management Module */
        {
		.index = 7,
                .sensor = {
                        .Num = 1,
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
							.SensorInt64 = 8,
							.SensorUint64 = 7,
							.SensorFloat64 = 16,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.Min = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 4,
							.SensorUint64 = 7,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
                                        .Nominal = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 8,
							.SensorUint64 = 0,
							.SensorFloat64 = 12,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
				},
				.AccuracyFactor = 46.5,
                        },
                        .ThresholdDefn = {
				.IsAccessible = SAHPI_TRUE,
                                .ReadThold = SAHPI_STM_LOW_MAJOR | SAHPI_STM_UP_MAJOR |
                                             SAHPI_STM_LOW_HYSTERESIS | SAHPI_STM_UP_HYSTERESIS,
                                .WriteThold = 0,
                                /* Default HDW thresholds:
                                   Warning 10.80<>12.60; Warning Reset 12.34<>11.64 */
				.Nonlinear = 0,
                        },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.2.2.1.3.0",
                                .threshold_oids = {
					.LowMajor = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.10.3",
					.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.6.3",
					.TotalPosThdHysteresis  = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.7.3",
					.TotalNegThdHysteresis = ".1.3.6.1.4.1.2.3.51.2.2.20.2.1.1.11.3",
                                },
				.threshold_write_oids = {},
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
                                        .event = "06036480", /* EN_CUTOFF_HI_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
					.event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "06036800", /* EN_CUTOFF_LO_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_LOWER_CRIT | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                },
                                {
                                        .event = "FF037500", /* EN_PFA_HI_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
					.event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {
                                        .event = "FF037800", /* EN_PFA_LO_FAULT_12V_PLANAR */
					.event_assertion = SAHPI_TRUE,
					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
			.reading2event = {},
                },
                .comment = "Chassis 12 volt sensor"
        },

	/* CPU 1 temperature sensor */
        {
		.index = 8,
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
							.SensorInt64 = 3,
							.SensorUint64 = 45,
							.SensorFloat64 = 125,
							.SensorBuffer[0] = 0,
                                                },
                                        },
                                        .Min = {
						.IsSupported = SAHPI_TRUE,
                                                .Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 4,
							.SensorUint64 = 8,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
                                                },
                                        },
					.Nominal = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 3,
							.SensorUint64 = 3,
							.SensorFloat64 = 3.3,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMax = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 1000,
							.SensorUint64 = 1000,
							.SensorFloat64 = 1000,
							.SensorBuffer[0] = 0,
						},
					},
					.NormalMin = {
						.IsSupported = SAHPI_TRUE,
						.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64,
						.Value = {
							.SensorInt64 = 0,
							.SensorUint64 = 0,
							.SensorFloat64 = 0,
							.SensorBuffer[0] = 0,
						},
					},
															
				},
				.AccuracyFactor = 45.8,
                        },
                        .ThresholdDefn = {
                                .IsAccessible = SAHPI_TRUE,
                                .ReadThold  = SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT,
                                .WriteThold = 0,
				.Nonlinear = 1,
                        },
                        .Oem = 0,
                },
                .sensor_info = {
                        .mib = {
                                .not_avail_indicator_num = 0,
                                .write_only = SAHPI_FALSE,
                                .oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.6.x",
                                .threshold_oids = {
					.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.6.x",
					.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.7.x",
                                },
				.threshold_write_oids = {},
                        },
                        .cur_state = SAHPI_ES_UNSPECIFIED,
                        .sensor_enabled = SAHPI_TRUE,
                        .events_enabled = SAHPI_TRUE,
			.assert_mask   = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
			.deassert_mask = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                        .event_array = {
                                {
                                        .event = "0421C401", /* EN_PROC_HOT_CPU1 */
					.event_assertion = SAHPI_TRUE,
 					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0421C481", /* EN_CUTOFF_HI_OVER_TEMP_CPU1 */
					.event_assertion = SAHPI_TRUE,
 					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0421D081", /* EN_THERM_TRIP_CPU1 */
					.event_assertion = SAHPI_TRUE,
 					.event_res_failure = SAHPI_TRUE,
					.event_res_failure_unexpected = SAHPI_TRUE,
                                        .event_state = SAHPI_ES_UPPER_CRIT | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                },
                                {
                                        .event = "0421D501", /* EN_PFA_HI_OVER_TEMP_CPU1 */
					.event_assertion = SAHPI_TRUE,
 					.event_res_failure = SAHPI_FALSE,
					.event_res_failure_unexpected = SAHPI_FALSE,
                                        .event_state = SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_MINOR,
                                        .recovery_state = SAHPI_ES_UNSPECIFIED,
                                },
                                {},
                        },
 			.reading2event = {},
		},
                .comment = "Blade CPU 1 temperature sensor"
        }


};

