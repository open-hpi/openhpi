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
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 */

#ifndef _OV_REST_RE_DISCOVER_H
#define _OV_REST_RE_DISCOVER_H

/* Include files */
#include "ov_rest_discover.h"
#include "ov_rest_server_event.h"
#include "ov_rest_interconnect_event.h"
#include "ov_rest_ps_event.h"
#include "ov_rest_fan_event.h"

void free_data(gpointer data);

SaErrorT ov_rest_re_discover_resources(struct oh_handler_state *oh_handler);
SaErrorT re_discover_appliance(struct oh_handler_state *oh_handler);
SaErrorT re_discover_enclosure(struct oh_handler_state *oh_handler);
SaErrorT remove_enclosure(struct oh_handler_state *handler,
                struct enclosureStatus *enclosure);
SaErrorT add_enclosure(struct oh_handler_state *handler,
                struct enclosureInfo *result);
SaErrorT re_discover_composer(struct oh_handler_state *oh_handler);
SaErrorT add_composer(struct oh_handler_state *handler,
                struct applianceInfo *composer_info,
                struct applianceHaNodeInfo *ha_node_result);
SaErrorT remove_composer(struct oh_handler_state *handler,
                struct enclosureStatus *enclosure,
                byte bayNumber);
SaErrorT re_discover_server(struct oh_handler_state *oh_handler);
SaErrorT add_inserted_blade(struct oh_handler_state *handler,
                struct serverhardwareInfo *info_result,
                struct enclosureStatus *enclosure);
SaErrorT remove_server(struct oh_handler_state *handler,
                struct enclosureStatus *enclosure,
                SaHpiInt32T bay_number);
SaErrorT re_discover_drive_enclosure(struct oh_handler_state *oh_handler);
SaErrorT add_inserted_drive_enclosure(struct oh_handler_state *handler,
                struct driveEnclosureInfo *info_result,
                struct enclosureStatus *enclosure);
SaErrorT remove_drive_enclosure(struct oh_handler_state *handler,
                        struct enclosureStatus *enclosure,
                        SaHpiInt32T bay_number);
SaErrorT re_discover_interconnect(struct oh_handler_state *oh_handler);
SaErrorT add_inserted_interconnect(struct oh_handler_state *handler,
                                struct enclosureStatus *enclosure,
                                struct interconnectInfo *result);
SaErrorT remove_interconnect(struct oh_handler_state *handler,
                        struct enclosureStatus *enclosure,
                        SaHpiInt32T bay_number);
SaErrorT re_discover_sas_interconnect(struct oh_handler_state *oh_handler);
SaErrorT re_discover_powersupply(struct oh_handler_state *oh_handler);
SaErrorT add_inserted_powersupply(struct oh_handler_state *handler,
                                struct enclosureStatus *enclosure,
                                struct powersupplyInfo *result);
SaErrorT remove_powersupply(struct oh_handler_state *handler,
                        struct enclosureStatus *enclosure,
                        SaHpiInt32T bay_number);
SaErrorT re_discover_fan(struct oh_handler_state *oh_handler);
SaErrorT add_inserted_fan(struct oh_handler_state *handler,
                        struct enclosureStatus *enclosure,
                        struct fanInfo *result);
SaErrorT remove_fan(struct oh_handler_state *handler,
                struct enclosureStatus *enclosure,
                SaHpiInt32T bay_number);
/*
SaErrorT update_server_hotswap_state(struct oh_handler_state *oh_handler,
                                     SOAP_CON *con,
                                     SaHpiInt32T bay_number);
SaErrorT update_interconnect_hotswap_state(struct oh_handler_state *oh_handler,
                                           SOAP_CON *con,
                                           SaHpiInt32T bay_number);
*/

#endif
