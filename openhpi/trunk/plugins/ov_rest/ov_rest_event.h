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
 */

#ifndef _OV_REST_EVENT_H
#define _OV_REST_EVENT_H

/* Include files */
#include "ov_rest.h"
#include "ov_rest_re_discover.h"
#include "ov_rest_parser_calls.h"
#include "ov_rest_server_event.h"
#include "ov_rest_composer_event.h"
#include "ov_rest_interconnect_event.h"
#include "ov_rest_ps_event.h"
#include "ov_rest_fan_event.h"

#define AMQP_USER "scmbadmin"
#define AMQP_EXTERNAL_USER "guest"
#define AMQP_EXTERNAL_PASSWORD "guest"
#define AMQP_PORT 5671
#define AMQP_EXCHANGE "scmb"
#define AMQP_ALERTS_BINDINGKEY "scmb.alerts.#"
#define AMQP_TASKS_BINDINGKEY "scmb.tasks.#"
#define AMQP_CONSUME_TIMEOUT_SEC 5
#define AMQP_CONSUME_TIMEOUT_USEC 0

int ov_rest_get_event(void *oh_handler);
int ov_rest_get_baynumber(const char *path);
gpointer ov_rest_event_thread(gpointer ov_handler);
void process_ov_events(struct oh_handler_state *oh_handler,
                       json_object *scmb_resource);
void ov_rest_process_alerts(struct oh_handler_state *oh_handler,
		json_object * scmb_resource, struct eventInfo* event);
void ov_rest_process_tasks(struct oh_handler_state *oh_handler,
		json_object * scmb_resource, struct eventInfo* event);
SaErrorT ov_rest_setuplistner(struct oh_handler_state *handler);
SaErrorT ov_rest_scmb_listner(struct oh_handler_state *handler);
SaErrorT  ov_rest_proc_add_task( struct oh_handler_state *oh_handler,
                                  struct eventInfo* event);
SaErrorT  ov_rest_proc_power_on_task( struct oh_handler_state *oh_handler,
                                  struct eventInfo* event);
SaErrorT  ov_rest_proc_power_off_task( struct oh_handler_state *oh_handler,
                                  struct eventInfo* event);
SaErrorT  ov_rest_proc_reset_task( struct oh_handler_state *oh_handler,
                                  struct eventInfo* event);
SaErrorT ov_rest_amqp_err_handling(struct oh_handler_state *handler, 
				int library_error);
SaErrorT ov_rest_re_discover(struct oh_handler_state *handler);
SaErrorT ov_rest_getActiveLockedEventArray(REST_CON *connection,
                struct eventArrayResponse *response);
SaErrorT process_active_and_locked_alerts(struct oh_handler_state *handler,
                struct eventArrayResponse *event_response);
SaErrorT ov_rest_proc_activate_standby_composer(
					struct oh_handler_state *oh_handler,
					struct eventInfo* event);
SaErrorT oem_event_handler(struct oh_handler_state *oh_handler,
                        struct eventInfo* event);
SaErrorT oem_event_to_file(struct oh_handler_state *handler,
                        struct eventInfo* ov_event,
                        struct oh_event* oem_event);

#endif
