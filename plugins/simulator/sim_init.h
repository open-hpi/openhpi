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
 *      Sean Dague
 *
 */
#ifndef _SIM_INIT_H
#define _SIM_INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_domain.h>
#include <oh_utils.h>
#include <oh_error.h>

void *sim_open(GHashTable *handler_config);
SaErrorT sim_discover(void *hnd);
int sim_get_event(void *hnd, struct oh_event *event);
void sim_close(void *hnd);
int build_rptcache(RPTable *rptcache, SaHpiEntityPathT *root_ep);
struct oh_event *eventdup(const struct oh_event *event);
SaErrorT new_sensor(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiSensorNumT Index);
int sim_get_next_sensor_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type);

#endif
