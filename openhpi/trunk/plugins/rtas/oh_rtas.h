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
 *        Renier Morales <renierm@users.sf.net>
 */

#ifndef _OH_RTAS_H
#define _OH_RTAS_H

#include <glib.h>
#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_utils.h>
#include <oh_error.h>

void *oh_rtas_open(GHashTable *handler_config);
void oh_rtas_close(void *hnd);
SaErrorT oh_rtas_get_event(void *hnd, struct oh_event *event);
SaErrorT oh_rtas_discover_resources(void *hnd);
SaErrorT oh_rtas_discover_domain_resources(void *hnd, SaHpiDomainIdT did);
SaErrorT oh_rtas_set_resource_tag(void *hnd,
                                  SaHpiResourceIdT id,
                                  SaHpiTextBufferT *tag);
SaErrorT oh_rtas_set_resource_severity(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSeverityT sev);
SaErrorT oh_rtas_get_el_info(void *hnd,
                             SaHpiResourceIdT id,
                             SaHpiEventLogInfoT *info);
SaErrorT oh_rtas_set_el_time(void *hnd,
                             SaHpiResourceIdT id,
                             SaHpiTimeT time);
SaErrorT oh_rtas_add_el_entry(void *hnd,
                              SaHpiResourceIdT id,
                              const SaHpiEventT *Event);
SaErrorT oh_rtas_get_el_entry(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiEventLogEntryIdT current,
                              SaHpiEventLogEntryIdT *prev,
                              SaHpiEventLogEntryIdT *next,
                              SaHpiEventLogEntryT *entry,
                              SaHpiRdrT  *rdr,
                              SaHpiRptEntryT  *rptentry);
SaErrorT oh_rtas_clear_el(void *hnd, SaHpiResourceIdT id);
SaErrorT oh_rtas_set_el_state(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiBoolT e);
SaErrorT oh_rtas_reset_el_overflow(void *hnd, SaHpiResourceIdT id);
SaErrorT oh_rtas_get_sensor_reading(void *hnd,
                                    SaHpiResourceIdT id,
                                    SaHpiSensorNumT num,
                                    SaHpiSensorReadingT *reading,
                                    SaHpiEventStateT *state);
SaErrorT oh_rtas_get_sensor_thresholds(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       SaHpiSensorThresholdsT *thres);
SaErrorT oh_rtas_set_sensor_thresholds(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       const SaHpiSensorThresholdsT *thres);
SaErrorT oh_rtas_get_sensor_enable(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiSensorNumT num,
                                   SaHpiBoolT *enable);
SaErrorT oh_rtas_set_sensor_enable(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiSensorNumT num,
                                   SaHpiBoolT enable);
SaErrorT oh_rtas_get_sensor_event_enables(void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiBoolT *enables);
SaErrorT oh_rtas_set_sensor_event_enables(void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          const SaHpiBoolT enables);
SaErrorT oh_rtas_get_sensor_event_masks(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiSensorNumT  num,
                                        SaHpiEventStateT *AssertEventMask,
                                        SaHpiEventStateT *DeassertEventMask);
SaErrorT oh_rtas_set_sensor_event_masks(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiSensorNumT num,
                                        SaHpiSensorEventMaskActionT act,
                                        SaHpiEventStateT AssertEventMask,
                                        SaHpiEventStateT DeassertEventMask);
SaErrorT oh_rtas_get_control_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT *mode,
                                   SaHpiCtrlStateT *state);
SaErrorT oh_rtas_set_control_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT mode,
                                   SaHpiCtrlStateT *state);
SaErrorT oh_rtas_get_idr_info(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrInfoT *idrinfo);
SaErrorT oh_rtas_get_idr_area_header(void *hnd,
                                     SaHpiResourceIdT rid,
                                     SaHpiIdrIdT idrid,
                                     SaHpiIdrAreaTypeT areatype,
                                     SaHpiEntryIdT areaid,
                                     SaHpiEntryIdT *nextareaid,
                                     SaHpiIdrAreaHeaderT *header);
SaErrorT oh_rtas_add_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrAreaTypeT areatype,
                              SaHpiEntryIdT *areaid);
SaErrorT oh_rtas_del_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiEntryIdT areaid);
SaErrorT oh_rtas_get_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiIdrFieldTypeT fieldtype,
                               SaHpiEntryIdT fieldid,
                               SaHpiEntryIdT *nextfieldid,
                               SaHpiIdrFieldT *field);
SaErrorT oh_rtas_add_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field);
SaErrorT oh_rtas_set_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field);
SaErrorT oh_rtas_del_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiEntryIdT fieldid);
SaErrorT oh_rtas_get_watchdog_info(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt);
SaErrorT oh_rtas_set_watchdog_info(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt);
SaErrorT oh_rtas_reset_watchdog(void *hnd,
                                SaHpiResourceIdT id,
                                SaHpiWatchdogNumT num);
SaErrorT oh_rtas_get_next_announce(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiAnnunciatorNumT num,
                                   SaHpiSeverityT sev,
                                   SaHpiBoolT unack,
                                   SaHpiAnnouncementT *a);
SaErrorT oh_rtas_get_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiAnnouncementT *a);
SaErrorT oh_rtas_ack_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiSeverityT sev);
SaErrorT oh_rtas_add_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiAnnouncementT *a);
SaErrorT oh_rtas_del_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiSeverityT sev);
SaErrorT oh_rtas_get_annunc_mode(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT *mode);
SaErrorT oh_rtas_set_annunc_mode(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT mode);
SaErrorT oh_rtas_get_hotswap_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiHsStateT *state);
SaErrorT oh_rtas_set_hotswap_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiHsStateT state);
SaErrorT oh_rtas_request_hotswap_action(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiHsActionT act);
SaErrorT oh_rtas_get_power_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiPowerStateT *state);
SaErrorT oh_rtas_set_power_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiPowerStateT state);
SaErrorT oh_rtas_get_indicator_state(void *hnd,
                                     SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT *state);
SaErrorT oh_rtas_set_indicator_state(void *hnd,
                                     SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT state);
SaErrorT oh_rtas_control_parm(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiParmActionT act);
SaErrorT oh_rtas_get_reset_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiResetActionT *act);
SaErrorT oh_rtas_set_reset_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiResetActionT act);

#endif /* _OH_RTAS_H */
