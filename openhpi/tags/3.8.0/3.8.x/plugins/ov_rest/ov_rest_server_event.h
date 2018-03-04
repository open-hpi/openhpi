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
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 *
 */

#ifndef _OV_REST_SERVER_EVENT_H
#define _OV_REST_SERVER_EVENT_H
#include <SaHpiOvRest.h>

/* Include files */


SaErrorT process_server_power_on_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *event);
SaErrorT process_server_power_off_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *event);
SaErrorT process_server_reset_event(struct oh_handler_state *oh_handler,
                                       struct eventInfo *ov_event);
SaErrorT ov_rest_proc_blade_inserted( struct oh_handler_state *oh_handler,
                                      struct eventInfo* event);
SaErrorT ov_rest_proc_blade_add_complete( struct oh_handler_state *oh_handler,
                                          struct eventInfo* event);
SaErrorT ov_rest_proc_blade_removed( struct oh_handler_state *oh_handler,
                                     struct eventInfo* event);
SaErrorT build_inserted_server_rpt(struct oh_handler_state *oh_handler,
                                   struct serverhardwareInfo *response,
                                   SaHpiRptEntryT *rpt);
SaErrorT build_inserted_server_rdr(struct oh_handler_state *oh_handler,
				SaHpiResourceIdT resource_id,
				struct serverhardwareInfo *response,
				int build_sensors);
SaErrorT remove_server_blade(struct oh_handler_state *oh_handler, 
				SaHpiInt32T bay_number,
                                struct enclosureStatus *enclosure);
SaErrorT ov_rest_proc_server_status(struct oh_handler_state *oh_handler,
                                     struct eventInfo* event);
SaErrorT process_drive_enclosure_power_on_event(
				struct oh_handler_state *oh_handler,
				struct eventInfo *ov_event);
SaErrorT process_drive_enclosure_power_off_event(
				struct oh_handler_state *oh_handler,
				struct eventInfo *ov_event);
SaErrorT build_inserted_drive_enclosure_rpt(struct oh_handler_state 
				*oh_handler, struct driveEnclosureInfo 
				*response, SaHpiRptEntryT *rpt);
SaErrorT ov_rest_proc_drive_enclosure_add_complete(
                                        struct oh_handler_state *oh_handler,
                                        struct eventInfo* event);

#endif
