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
#include <oh_domain.h>
#include <oh_event.h>

/* Alarm Handling */
SaHpiAlarmT *oh_add_alarm(struct oh_domain *d, SaHpiAlarmT *alarm);
SaHpiAlarmT *oh_get_alarm(struct oh_domain *d,
                          SaHpiAlarmIdT *aid,
                          SaHpiSeverityT *severity,
                          SaHpiStatusCondTypeT *type,
                          SaHpiResourceIdT *rid,
                          SaHpiManufacturerIdT *mid,
                          SaHpiSensorNumT *num,
                          SaHpiEventStateT *state,
                          SaHpiBoolT unacknowledged,                          
                          int get_next);
SaErrorT oh_remove_alarm(struct oh_domain *d,
                         SaHpiSeverityT *severity,
                         SaHpiStatusCondTypeT *type,
                         SaHpiResourceIdT *rid,
                         SaHpiManufacturerIdT *mid,
                         SaHpiSensorNumT *num,
                         SaHpiEventStateT *state,
                         SaHpiEventStateT *deassert_mask,
                         int multi);
SaErrorT oh_close_alarmtable(struct oh_domain *d);

/* Alarm Triggers */
SaErrorT oh_detect_event_alarm(struct oh_event *e);
SaErrorT oh_detect_res_sev_alarm(SaHpiDomainIdT did,
                                 SaHpiResourceIdT rid,
                                 SaHpiSeverityT new_sev);
SaErrorT oh_detect_sensor_enable_alarm(SaHpiDomainIdT did,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num,
                                       SaHpiBoolT enable);
SaErrorT oh_detect_sensor_mask_alarm(SaHpiDomainIdT did,
                                     SaHpiResourceIdT rid,
                                     SaHpiSensorNumT num,
                                     SaHpiSensorEventMaskActionT action,
                                     SaHpiEventStateT deassert_mask);

#endif /* __OH_ALARM_H */

