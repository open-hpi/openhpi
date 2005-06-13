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
 *	  Christina Hernandez <hernanc@us.ibm.com>
 *        W. David Ashley <dashley@us.ibm.com>
 */


#include <sim_init.h>
#include <rpt_utils.h>


static int sim_get_next_sensor_num(RPTable *rptcache, SaHpiResourceIdT ResId,
                                   SaHpiRdrTypeT type)
{
	int i = 0;
	SaHpiRdrT *RdrEntry;

	RdrEntry = oh_get_rdr_next(rptcache, ResId, SAHPI_FIRST_ENTRY);

	while(RdrEntry){
		if (RdrEntry->RdrType == type){
			i++;
		}
		RdrEntry = oh_get_rdr_next(rptcache, ResId, RdrEntry->RecordId);
	}

	return i;
}


static SaErrorT new_sensor(struct oh_handler_state * state, SaHpiResourceIdT ResId,
                           struct sim_sensor *mysensor) {
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;
	struct SensorInfo *info;  // our extra info

	info = (struct SensorInfo *)g_malloc0(sizeof(struct SensorInfo));

	// set up res_rdr
        res_rdr.RdrType = SAHPI_SENSOR_RDR;
        memcpy(&res_rdr.RdrTypeUnion.SensorRec, &mysensor->sensor, sizeof(SaHpiSensorRecT));
        res_rdr.IsFru = 1;
        res_rdr.RdrTypeUnion.SensorRec.Num =
         sim_get_next_sensor_num(state->rptcache, ResId,
                                 res_rdr.RdrTypeUnion.SensorRec.Type);
        res_rdr.RecordId = get_rdr_uid(res_rdr.RdrType,
                                       res_rdr.RdrTypeUnion.SensorRec.Num);
	oh_init_textbuffer(&res_rdr.IdString);
	oh_append_textbuffer(&res_rdr.IdString, mysensor->comment);

        // get the RptEntry
	RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
	if(!RptEntry){
                dbg("NULL rpt pointer during sensor add\n");
	} else {
                res_rdr.Entity = RptEntry->ResourceEntity;
	}

        // now set up our extra info for the sensor
        info->cur_state = mysensor->sensor_info.cur_state;
	info->sensor_enabled = mysensor->sensor_info.sensor_enabled;
        info->assert_mask = mysensor->sensor_info.assert_mask;
        info->deassert_mask = mysensor->sensor_info.deassert_mask;
        memcpy(&info->event_array, &mysensor->sensor_info.event_array,
               sizeof(struct sensor_event_map));
        memcpy(&info->reading2event, &mysensor->sensor_info.reading2event,
               sizeof(struct sensor_event_map));
	info->reading.IsSupported = mysensor->sensor.DataFormat.IsSupported;
	info->reading.Type = mysensor->sensor.DataFormat.ReadingType;
        switch (info->reading.Type) {
                case SAHPI_SENSOR_READING_TYPE_INT64:
                        info->reading.Value.SensorInt64 =
                         mysensor->sensor.DataFormat.Range.Nominal.Value.SensorInt64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_UINT64:
                        info->reading.Value.SensorUint64 =
                         mysensor->sensor.DataFormat.Range.Nominal.Value.SensorUint64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                        info->reading.Value.SensorFloat64 =
                         mysensor->sensor.DataFormat.Range.Nominal.Value.SensorFloat64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                default:
                        memcpy(info->reading.Value.SensorBuffer,
                               mysensor->sensor.DataFormat.Range.Nominal.Value.SensorBuffer,
                               SAHPI_SENSOR_BUFFER_LENGTH);
        }
	info->reading.Value.SensorFloat64 =
                mysensor->sensor.DataFormat.Range.Nominal.Value.SensorFloat64;
	memcpy(&info->thres, &mysensor->sensor.DataFormat.Range,
               sizeof(SaHpiSensorThresholdsT));

        // everything ready so add the rdr and extra info to the rptcache
        sim_inject_rdr(state, ResId, &res_rdr, info);

        return 0;
}


SaErrorT sim_discover_sensors(struct oh_handler_state * state)
{

        /* chassis sensors */
        int i = 0;
        while (sim_chassis_sensors[i].index != 0) {
                new_sensor(state, SIM_RPT_ENTRY_CHASSIS, &sim_chassis_sensors[i]);
                i++;
        }

        /* cpu sensors */
        i = 0;
        while (sim_cpu_sensors[i].index != 0) {
                new_sensor(state, SIM_RPT_ENTRY_CPU, &sim_cpu_sensors[i]);
                i++;
        }

        /* dasd sensors */
        i = 0;
        while (sim_dasd_sensors[i].index != 0) {
                new_sensor(state, SIM_RPT_ENTRY_DASD, &sim_dasd_sensors[i]);
                i++;
        }

        /* fan sensors */
        i = 0;
        while (sim_fan_sensors[i].index != 0) {
                new_sensor(state, SIM_RPT_ENTRY_CPU, &sim_fan_sensors[i]);
                i++;
        }

	return 0;

}

