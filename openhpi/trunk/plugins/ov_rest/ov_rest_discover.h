/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
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
 * Neither the name of the Hewlett Packard Enterprise, nor the names
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
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 *
 */

#ifndef _OV_REST_DISCOVER_H
#define _OV_REST_DISCOVER_H

/* Include libraries */
#include <stdio.h>
#include <oh_error.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "ov_rest_parser_calls.h"
#include "ov_rest_resources.h"

#define GEN_CERT_WAIT_TIME 10
/* Builds the sensor rdr and gets the current value of sensor */
#define OV_REST_BUILD_ENABLE_SENSOR_RDR(sensor_num, sensor_value) \
{ \
        memset(&rdr, 0, sizeof(SaHpiRdrT)); \
        rv = ov_rest_build_sen_rdr(oh_handler, resource_id, &rdr, \
                                   &sensor_info, sensor_num); \
        if (rv != SA_OK) { \
                err("Failed to create sensor rdr for sensor %x", sensor_num); \
                return rv; \
        } \
 \
        rv = ov_rest_map_sen_val(sensor_info, sensor_num, \
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

/* Builds the sensor rdr which does not events based on sensor number */
#define OV_REST_BUILD_SENSOR_RDR(sensor_num) \
{ \
        memset(&rdr, 0, sizeof(SaHpiRdrT)); \
        rv = ov_rest_build_sen_rdr(oh_handler, resource_id, &rdr, \
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

#define OV_REST_BUILD_CONTROL_RDR(control_num, analogLimitLow, \
                                  analogLimitHigh) \
{ \
        memset(&rdr, 0, sizeof(SaHpiRdrT)); \
        rv = ov_rest_build_control_rdr(oh_handler, &rdr, resource_id, \
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

SaErrorT ov_rest_getapplianceHaNodeInfo(
                struct applianceHaNodeInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getapplianceHANodeArray(struct oh_handler_state *oh_handler,
		struct applianceHaNodeInfoArrayResponse* response,
		REST_CON *connection,
		char* appliance_ha_info);

SaErrorT ov_rest_getapplianceNodeInfo(struct oh_handler_state *oh_handler,
                struct applianceNodeInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getdatacenterInfo(struct oh_handler_state *oh_handler,
		struct datacenterInfoArrayResponse *response,
		REST_CON *connection,
		char* datacenter_doc);

SaErrorT ov_rest_getenclosureInfoArray(struct oh_handler_state *oh_handler,
		struct enclosureInfoArrayResponse* response,
		REST_CON *connection,
		char* enclosure_doc);
SaErrorT ov_rest_getenclosureStatus(struct oh_handler_state *oh_handler,
                struct enclosureStatusResponse* response,
                REST_CON *connection,
                char* enclosure_doc);

SaErrorT ov_rest_getserverInfoArray(struct oh_handler_state *oh_handler,
		struct serverhardwareInfoArrayResponse *response,
		REST_CON *connection,
		char* server_doc);

SaErrorT ov_rest_getdriveEnclosureInfoArray(struct oh_handler_state 
		*oh_handler, struct driveEnclosureInfoArrayResponse 
		*response, REST_CON *connection,
                char* drive_enc_doc);

SaErrorT ov_rest_getinterconnectInfoArray(struct oh_handler_state *oh_handler,
		struct interconnectInfoArrayResponse *response,
		REST_CON *connection,
		char* interconnect_doc);

SaErrorT ov_rest_build_serverThermalRdr(struct oh_handler_state *oh_handler,
                REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_serverPowerStatusRdr(struct oh_handler_state 
		*oh_handler, REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_serverSystemsRdr(struct oh_handler_state *oh_handler,
                REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_serverStorageRdr(struct oh_handler_state *oh_handler,
                REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_serverNetworkAdaptersRdr(
                struct oh_handler_state *oh_handler,
                REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_serverEthernetInterfacesRdr(
                struct oh_handler_state *oh_handler,
                REST_CON *connection,
                struct serverhardwareInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_getAllEvents(struct eventArrayResponse *response,
                REST_CON *connection,
                char* getallevents_doc);

SaErrorT ov_rest_create_certificate (REST_CON *connection,
		char* postfields);

SaErrorT ov_rest_getcertificates (struct oh_handler_state *oh_handler,
		struct certificateResponse *response,
		REST_CON *connection,
		char *certificate_doc);

SaErrorT ov_rest_getserverConsoleUrl(struct oh_handler_state *oh_handler,
                        REST_CON *connection);
SaErrorT ov_rest_getserverThermalInfo(struct oh_handler_state *oh_handler,
                struct serverhardwareThermalInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getserverPowerStatusInfo(struct oh_handler_state *oh_handler,
                struct serverhardwarePowerStatusInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getserverSystemsInfo(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getserverStorageInfo(struct oh_handler_state *oh_handler,
                struct serverhardwareStorageInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getserverNetworkAdaptersInfo(
                struct oh_handler_state *oh_handler,
                struct serverhardwareNetworkAdaptersInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getserverEthernetInterfacesInfo(
                struct oh_handler_state *oh_handler,
                struct serverhardwareEthernetInterfacesInfoResponse *response,
                REST_CON *connection);

SaErrorT ov_rest_getca(struct oh_handler_state *oh_handler,
                struct certificateResponse *response,
                REST_CON *connection,
                char* ca_doc);

SaErrorT ov_rest_session_timeout(struct ov_rest_handler *ov_handler, 
 		struct idleTimeout *to);

SaErrorT ov_rest_build_appliance_rdr(struct oh_handler_state *oh_handler,
                struct applianceNodeInfo *response,
                struct applianceHaNodeInfo *ha_response,
                SaHpiResourceIdT resource_id);

SaErrorT ov_rest_build_enclosure_rdr(struct oh_handler_state *oh_handler,
		struct enclosureInfo *response,
		SaHpiResourceIdT resource_id);

SaErrorT ov_rest_build_appliance_rpt(struct oh_handler_state *oh_handler,
		struct applianceHaNodeInfo *response,
		SaHpiResourceIdT *resource_id, char *type);

SaErrorT ov_rest_build_composer_rpt(struct oh_handler_state *oh_handler,
                             struct applianceHaNodeInfo *response,
                             SaHpiResourceIdT *resource_id, char *type);

SaErrorT ov_rest_build_enclosure_rpt(struct oh_handler_state *oh_handler,
		struct enclosureInfo *response,
		SaHpiResourceIdT *resource_id);

SaErrorT ov_rest_build_composer_rdr( struct oh_handler_state *oh_handler,
                        struct applianceInfo *composer_info,
                        struct applianceHaNodeInfo *ha_response,
                        SaHpiResourceIdT resource_id);
SaErrorT ov_rest_discover_composer(struct oh_handler_state *handler);

SaErrorT build_discovered_server_rpt(struct oh_handler_state *oh_handler, 
		struct serverhardwareInfo *info_result, 
		SaHpiResourceIdT *resource_id);

SaErrorT ov_rest_build_server_rpt(struct oh_handler_state *oh_handler,
		struct serverhardwareInfo *response,
		SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_rdr(struct oh_handler_state *oh_handler,
		SaHpiResourceIdT resource_id,
		struct serverhardwareInfo *response);

SaErrorT ov_rest_build_server_thermal_rdr(struct oh_handler_state *oh_handler,
		struct serverhardwareThermalInfoResponse* response,
		SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_temperature_sensor_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareThermalInfo server_therm_info,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_fan_sensor_rdr_info(struct oh_handler_state *oh_handler,
                struct serverhardwareFanInfo server_fan_info,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_power_status_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwarePowerStatusInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_memory_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_processor_rdr(struct oh_handler_state 
		*oh_handler, struct serverhardwareSystemsInfoResponse 
		*response, SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_health_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_battery_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_systems_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareSystemsInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_storage_rdr(struct oh_handler_state *oh_handler,
                struct serverhardwareStorageInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_network_adapters_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareNetworkAdaptersInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_server_ethernet_inetrfaces_rdr(
                struct oh_handler_state *oh_handler,
                struct serverhardwareEthernetInterfacesInfoResponse *response,
                SaHpiRptEntryT *rpt);

SaErrorT build_discovered_drive_enclosure_rpt(
		struct oh_handler_state *oh_handler,
                struct driveEnclosureInfo *info_result,
                SaHpiResourceIdT *resource_id);

SaErrorT ov_rest_build_drive_enclosure_rpt(struct oh_handler_state *oh_handler,
                struct driveEnclosureInfo *response,
                SaHpiRptEntryT *rpt);

SaErrorT ov_rest_build_drive_enclosure_rdr(struct oh_handler_state *oh_handler,
                SaHpiResourceIdT resource_id,
                struct driveEnclosureInfo *response);

SaErrorT ov_rest_build_interconnect_rpt(struct oh_handler_state *oh_handler,
		struct interconnectInfo *response,
		SaHpiResourceIdT *resource_id);

SaErrorT ov_rest_build_interconnect_rdr(struct oh_handler_state *oh_handler,
		SaHpiResourceIdT resource_id,
		struct interconnectInfo *response);

SaErrorT ov_rest_build_powersupply_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct powersupplyInfo *response);

SaErrorT ov_rest_build_powersupply_rpt(struct oh_handler_state *oh_handler,
                                struct powersupplyInfo *response,
                                SaHpiResourceIdT *resource_id,
                                int enclosure_location);

SaErrorT ov_rest_build_fan_rdr(struct oh_handler_state *oh_handler,
                                SaHpiResourceIdT resource_id,
                                struct fanInfo *response);

SaErrorT ov_rest_build_fan_rpt(struct oh_handler_state *oh_handler,
                                struct fanInfo *response,
                                SaHpiResourceIdT *resource_id,
                                int enclosure_location);

SaErrorT ov_rest_build_enc_info(struct oh_handler_state *oh_handler,
                                       struct enclosureInfo *info);
SaErrorT ov_rest_discover_resources(void *oh_handler);

void release_ov_rest_resources(struct enclosureStatus *enclosure);

SaErrorT discover_ov_rest_system(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_appliance(struct oh_handler_state *handler);

SaErrorT ov_rest_discover_datacenter(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_enclosure(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_server(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_drive_enclosure(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_interconnect(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_sas_interconnect(struct oh_handler_state 
				*oh_handler);

SaErrorT ov_rest_discover_powersupply(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_discover_fan(struct oh_handler_state *oh_handler);

SaErrorT ov_rest_populate_event(struct oh_handler_state *oh_handler,
		SaHpiResourceIdT resource_id,
		struct oh_event *event,
		GSList **assert_sensors);
void func_t (gpointer key, gpointer value, gpointer udata);

#endif
