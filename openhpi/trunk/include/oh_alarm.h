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

struct oh_dat {
        SaHpiAlarmIdT next_id;
        GList *list;        
};

SaErrorT oh_detect_event_alarm(SaHpiDomainIdT did, struct oh_event *e);
SaErrorT oh_detect_res_sev_alarm(SaHpiDomainIdT did,
                                 SaHpiRptEntryT *res,
                                 SaHpiSeverityT sev);
SaErrorT oh_detect_sensor_enable_alarm(SaHpiDomainIdT did,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num,
                                       SaHpiBoolT enable);
SaErrorT oh_detect_sensor_mask_alarm(SaHpiDomainIdT did,
                                     SaHpiRdrT *rdr,
                                     SaHpiEventStateT state);
SaErrorT oh_detect_sensor_enable_alarm(SaHpiDomainIdT did,
                                       SaHpiResourceIdT rid,
                                       SaHpiSensorNumT num,
                                       SaHpiBoolT enable);

#endif /* __OH_ALARM_H */

