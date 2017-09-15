/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
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
 * Neither the name of the Hewlett-Packard Corporation, nor the names
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
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 *      Shuah Khan <shuah.khan@hp.com>
 *      Raghavendra M.S. <raghavendra.ms@hp.com>
 */

#ifndef _OA_SOAP_DISCOVER_H
#define _OA_SOAP_DISCOVER_H

/* Include files */
#include "oa_soap_sensor.h"
#include "oa_soap_control.h"
#include "oa_soap_event.h"
#include "oa_soap_resources.h"

/* The OA does not gives the resource names for power supply, power subsystem
 * and OA. The names will be used for creating the tag in RPT entry
 */
#define POWER_SUPPLY_NAME "Power Supply Unit"
#define POWER_SUBSYSTEM_NAME "Power Subsystem"
#define OA_NAME "Onboard Administrator"
#define MAX_NAME_LEN 64

#define CISCO "CISCO"                   /* Identifies the Cisco interconnects */

/* Builds the sensor rdr and gets the current value of sensor */
#define OA_SOAP_BUILD_ENABLE_SENSOR_RDR(sensor_num, sensor_value) \
{ \
	memset(&rdr, 0, sizeof(SaHpiRdrT)); \
	rv = oa_soap_build_sen_rdr(oh_handler, resource_id, &rdr, \
				   &sensor_info, sensor_num); \
	if (rv != SA_OK) { \
		err("Failed to create sensor rdr for sensor %x", sensor_num); \
		return rv; \
	} \
 \
	rv = oa_soap_map_sen_val(sensor_info, sensor_num, \
				(SaHpiInt32T) sensor_value, &sensor_status); \
        if (rv != SA_OK) { \
                err("Setting sensor state failed"); \
                wrap_g_free(sensor_info); \
                return rv; \
        } \
 \
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, \
			   &rdr, sensor_info, 0); \
	if (rv != SA_OK) { \
		err("Failed to add rdr"); \
		return rv; \
	} \
}

/* Builds the threshold sensor rdr and puts threshold information provided by OA
 */
#define OA_SOAP_BUILD_THRESHOLD_SENSOR_RDR(sensor_num, response) \
{ \
	memset(&rdr, 0, sizeof(SaHpiRdrT)); \
	rv = oa_soap_build_sen_rdr(oh_handler, resource_id, &rdr, \
				   &sensor_info, sensor_num); \
	if (rv != SA_OK) { \
		err("Failed to create sensor rdr for sensor %x", sensor_num); \
		return rv; \
	} \
 \
 	/* the thresholds values updated in the rdr from the global sensor \
	 * array contains the dummy values, these values needs to be replaced \
	 * with actual threshold values provided by OA  \
	 */ \
 	rv = oa_soap_map_thresh_resp(&rdr, (void *)&response, \
	                             event_support, sensor_info);  \
        if (rv != SA_OK) { \
                err("Updating rdr with threshold failed"); \
                wrap_g_free(sensor_info); \
                return rv; \
        } \
 \
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, \
			   &rdr, sensor_info, 0); \
	if (rv != SA_OK) { \
		err("Failed to add rdr"); \
		return rv; \
	} \
}

/* Builds the sensor rdr which does not events based on sensor number */
#define OA_SOAP_BUILD_SENSOR_RDR(sensor_num) \
{ \
	memset(&rdr, 0, sizeof(SaHpiRdrT)); \
	rv = oa_soap_build_sen_rdr(oh_handler, resource_id, &rdr, \
				   &sensor_info, sensor_num); \
	if (rv != SA_OK) { \
		err("Failed to create sensor rdr for sensor %x", sensor_num); \
		return rv; \
	} \
 \
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, \
			   &rdr, sensor_info, 0); \
	if (rv != SA_OK) { \
		err("Failed to add rdr"); \
		return rv; \
	} \
}

/* Builds the control rdr.                                  */
/* Set analog limits to 0 if building a non-analog control. */
#define OA_SOAP_BUILD_CONTROL_RDR(control_num, analogLimitLow, \
                                  analogLimitHigh) \
{ \
	memset(&rdr, 0, sizeof(SaHpiRdrT)); \
	rv = oa_soap_build_control_rdr(oh_handler, &rdr, resource_id, \
                                       control_num, analogLimitLow, \
                                       analogLimitHigh); \
	if (rv != SA_OK) { \
		err("Failed to create rdr for control %x", control_num);\
		return rv; \
	} \
 \
	rv = oh_add_rdr(oh_handler->rptcache, resource_id, \
			   &rdr, 0, 0); \
	if (rv != SA_OK) { \
		err("Failed to add rdr"); \
		return rv; \
	} \
}

/* Function prototypes */

SaErrorT oa_soap_discover_resources(void *oh_handler);

SaErrorT discover_oa_soap_system(struct oh_handler_state *oh_handler);

SaErrorT build_enclosure_rpt(struct oh_handler_state *oh_handler,
                             char *name,
                             SaHpiResourceIdT *resource_id);

SaErrorT build_enclosure_rdr(struct oh_handler_state *oh_handler,
                             SOAP_CON *con,
                             struct enclosureInfo *response,
                             SaHpiResourceIdT resource_id);

SaErrorT discover_enclosure(struct oh_handler_state *oh_handler);

SaErrorT build_oa_rpt(struct oh_handler_state *oh_handler,
                      SaHpiInt32T bay_number,
                      SaHpiResourceIdT *resource_id);

SaErrorT build_oa_rdr(struct oh_handler_state *oh_handler,
                      SOAP_CON *con,
                      SaHpiInt32T bay_number,
                      struct oaInfo *response,
                      SaHpiResourceIdT resource_id);

SaErrorT discover_oa(struct oh_handler_state *oh_handler);

SaErrorT build_discovered_server_rpt(struct oh_handler_state *oh_handler,
                                     struct bladeInfo *response,
                                     SaHpiResourceIdT *resource_id,
                                     struct bladeStatus *);

SaErrorT build_server_rpt(struct oh_handler_state *oh_handler,
                          struct bladeInfo *response,
                          SaHpiRptEntryT *rpt);

SaErrorT build_inserted_server_rdr(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          SaHpiInt32T bay_number,
                          SaHpiResourceIdT resource_id,
                          char *name,
                          int build_sensors);

SaErrorT build_discovered_server_rdr_arr(struct oh_handler_state *oh_handler,
                          SOAP_CON *con,
                          SaHpiInt32T bay_number,
                          SaHpiResourceIdT resource_id,
                          char *name,
                          int build_sensors,
                          struct bladeInfo *, struct bladeStatus *,
                          struct bladePortMap *);

SaErrorT discover_server(struct oh_handler_state *oh_handler);

SaErrorT build_inserted_intr_rpt(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                char *name,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT *resource_id,
                                int inserted);

SaErrorT build_discovered_intr_rpt(struct oh_handler_state *oh_handler,
                                char *name,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT *resource_id,
                                struct interconnectTrayStatus *);

SaErrorT build_inserted_interconnect_rdr(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT resource_id,
                                int build_sensors);

SaErrorT build_discovered_intr_rdr_arr(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT resource_id,
                                int build_sensors,
                                struct interconnectTrayInfo *,
                                struct interconnectTrayStatus *,
                                struct interconnectTrayPortMap *);

SaErrorT discover_interconnect(struct oh_handler_state *oh_handler);

SaErrorT build_fan_rpt(struct oh_handler_state *oh_handler,
                       char *name,
                       SaHpiInt32T bay_number,
                       SaHpiResourceIdT *resource_id);

SaErrorT build_fan_rdr(struct oh_handler_state *oh_handler,
                       SOAP_CON *con,
                       struct fanInfo *response,
                       SaHpiResourceIdT resource_id);

SaErrorT discover_fan(struct oh_handler_state *oh_handler);

SaErrorT build_power_subsystem_rpt(struct oh_handler_state *oh_handler,
                                   char *name,
                                   SaHpiResourceIdT *resource_id);

SaErrorT build_power_subsystem_rdr(struct oh_handler_state *oh_handler,
                                   SaHpiResourceIdT resource_id);

SaErrorT discover_power_subsystem(struct oh_handler_state *oh_handler);

SaErrorT build_power_supply_rpt(struct oh_handler_state *oh_handler,
                                char *name,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT *resource_id);

SaErrorT build_power_supply_rdr(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                struct powerSupplyInfo *response,
                                SaHpiResourceIdT resource_id);

SaErrorT build_discovered_ps_rdr_arr(struct oh_handler_state *oh_handler,
                                struct powerSupplyInfo *response,
                                SaHpiResourceIdT resource_id,
                                struct powerSupplyStatus *status_response);

SaErrorT discover_power_supply(struct oh_handler_state *oh_handler);

void oa_soap_get_health_val(xmlNode *extra_data,
			    enum oa_soap_extra_data_health *status);

void oa_soap_parse_diag_ex(xmlNode *diag_ex,
			   enum diagnosticStatus *diag_status_arr);

SaErrorT oa_soap_build_rpt(struct oh_handler_state *oh_handler,
                           SaHpiInt32T resourece_type,
                           SaHpiInt32T location,
                           SaHpiRptEntryT *rpt);

SaErrorT oa_soap_get_fz_arr(struct oa_soap_handler *oa_handler,
                             SaHpiInt32T max_fz,
                             struct getFanZoneArrayResponse *response);

SaErrorT oa_soap_build_fan_rpt(struct oh_handler_state *oh_handler,
                                SaHpiInt32T bay_number,
                                SaHpiResourceIdT *resource_id);

SaErrorT oa_soap_build_fan_rdr(struct oh_handler_state *oh_handler,
                                SOAP_CON *con,
                                struct fanInfo *response,
                                SaHpiResourceIdT resource_id);

SaErrorT oa_soap_populate_event(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct oh_event *event,
                                GSList **assert_sensors);

SaErrorT oa_soap_build_blade_thermal_rdr(
                          struct oh_handler_state *oh_handler,
                          struct bladeThermalInfoArrayResponse thermal_response,
                          SaHpiRptEntryT *rpt,
                          char *name);

SaErrorT oa_soap_modify_blade_thermal_rdr(
                        struct oh_handler_state *oh_handler,
                        struct bladeThermalInfoArrayResponse thermal_response,
                        SaHpiRptEntryT *rpt);

SaErrorT oa_soap_get_ps_info_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getPowerSupplyInfoArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_ps_sts_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getPowerSupplyStsArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_fan_info_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getFanInfoArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_bladeinfo_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getBladeInfoArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_interconct_trayinfo_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct interconnectTrayInfoArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_interconct_traysts_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct interconnectTrayStsArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_interconct_traypm_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct interconnectTrayPmArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_oa_info_arr(SOAP_CON *con,
                            SaHpiInt32T max_bays,
                            struct getOaInfoArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_oa_sts_arr(SOAP_CON *con,
                            SaHpiInt32T max_bays,
                            struct getOaStatusArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_bladests_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getBladeStsArrayResponse *response,
                            xmlDocPtr *);

SaErrorT oa_soap_get_portmap_arr(struct oa_soap_handler *oa_handler,
                            SaHpiInt32T max_bays,
                            struct getBladePortMapArrayResponse *response,
                            xmlDocPtr *);


#endif
