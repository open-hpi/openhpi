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
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#ifndef __OH_ALARM_H
#define __OH_ALARM_H

#include <SaHpi.h>
#include <oh_event.h>

SaErrorT oh_detect_event_alarm(struct oh_event *e);
SaErrorT oh_detect_resource_alarm(SaHpiRptEntryT *res);
SaErrorT oh_detect_sensor_alarm(SaHpiRdrT *rdr, SaHpiBoolT enable);
SaErrorT oh_detect_sensor_mask_alarm(SaHpiRdrT *rdr, SaHpiEventStateT state);

#endif /* __OH_ALARM_H */

