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


static SaErrorT new_sensor(struct oh_handler_state * state,
                           SaHpiResourceIdT ResId, struct sim_sensor *mysensor) {
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;
	struct SensorInfo *info;  // our extra info

	info = (struct SensorInfo *)g_malloc0(sizeof(struct SensorInfo));

	// set up res_rdr
        res_rdr.RdrType = SAHPI_SENSOR_RDR;
        memcpy(&res_rdr.RdrTypeUnion.SensorRec, &mysensor->sensor, sizeof(SaHpiSensorRecT));
        res_rdr.IsFru = 1;
        res_rdr.RecordId = get_rdr_uid(res_rdr.RdrType,
                                       res_rdr.RdrTypeUnion.SensorRec.Num);
	oh_init_textbuffer(&res_rdr.IdString);
	oh_append_textbuffer(&res_rdr.IdString, mysensor->comment);

        // get the RptEntry
	RptEntry = oh_get_resource_by_id(state->rptcache, ResId);
	if (!RptEntry){
                dbg("NULL rpt pointer during sensor add\n");
                return SA_ERR_HPI_INVALID_RESOURCE;
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
               sizeof(struct sensor_reading_map));
        memcpy(&info->reading, &mysensor->sensor_info.reading,
               sizeof(SaHpiSensorReadingT));
        memcpy(&info->thres, &mysensor->sensor_info.thres,
               sizeof(SaHpiSensorThresholdsT));

        // everything ready so add the rdr and extra info to the rptcache
        sim_inject_rdr(state, ResId, &res_rdr, info);

        return SA_OK;
}


SaErrorT sim_discover_chassis_sensors(struct oh_handler_state * state,
                                      SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_chassis_sensors[i].index != 0) {
                rc = new_sensor(state, resid, &sim_chassis_sensors[i]);
                if (rc) {
                        dbg("Error %d returned when adding chassis sensor", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d chassis sensors injected", j, i);

	return 0;

}


SaErrorT sim_discover_cpu_sensors(struct oh_handler_state * state,
                                  SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_cpu_sensors[i].index != 0) {
                rc = new_sensor(state, resid, &sim_cpu_sensors[i]);
                if (rc) {
                        dbg("Error %d returned when adding cpu sensor", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d cpu sensors injected", j, i);

	return 0;

}


SaErrorT sim_discover_dasd_sensors(struct oh_handler_state * state,
                                   SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_dasd_sensors[i].index != 0) {
                rc = new_sensor(state, resid, &sim_dasd_sensors[i]);
                if (rc) {
                        dbg("Error %d returned when adding dasd sensor", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d dasd sensors injected", j, i);

	return 0;

}


SaErrorT sim_discover_hs_dasd_sensors(struct oh_handler_state * state,
                                      SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_hs_dasd_sensors[i].index != 0) {
                rc = new_sensor(state, resid, &sim_hs_dasd_sensors[i]);
                if (rc) {
                        dbg("Error %d returned when adding hs dasd sensor", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d hs dasd sensors injected", j, i);

	return 0;

}


SaErrorT sim_discover_fan_sensors(struct oh_handler_state * state,
                                  SaHpiResourceIdT resid) {
        SaErrorT rc;
        int i = 0;
        int j = 0;

        while (sim_fan_sensors[i].index != 0) {
                rc = new_sensor(state, resid, &sim_fan_sensors[i]);
                if (rc) {
                        dbg("Error %d returned when adding fan sensor", rc);
                } else {
                        j++;
                }
                i++;
        }
        dbg("%d of %d fan sensors injected", j, i);

	return 0;

}

