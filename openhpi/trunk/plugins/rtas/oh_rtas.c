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

#include <oh_rtas.h>

void *oh_rtas_open(GHashTable *handler_config)
{
        return NULL;
}

void oh_rtas_close(void *hnd)
{
        return;
}

SaErrorT oh_rtas_get_event(void *hnd, struct oh_event *event)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_discover_resources(void *hnd)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_discover_domain_resources(void *hnd, SaHpiDomainIdT did)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_resource_tag(void *hnd,
                                  SaHpiResourceIdT id,
                                  SaHpiTextBufferT *tag)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_resource_severity(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSeverityT sev)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_el_info(void *hnd,
                             SaHpiResourceIdT id,
                             SaHpiEventLogInfoT *info)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_el_time(void *hnd,
                             SaHpiResourceIdT id,
                             SaHpiTimeT time)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_add_el_entry(void *hnd,
                              SaHpiResourceIdT id,
                              const SaHpiEventT *Event)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_el_entry(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiEventLogEntryIdT current,
                              SaHpiEventLogEntryIdT *prev,
                              SaHpiEventLogEntryIdT *next,
                              SaHpiEventLogEntryT *entry,
                              SaHpiRdrT  *rdr,
                              SaHpiRptEntryT  *rptentry)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_clear_el(void *hnd, SaHpiResourceIdT id)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_el_state(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiBoolT e)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_reset_el_overflow(void *hnd, SaHpiResourceIdT id)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_reading(void *hnd,
                                    SaHpiResourceIdT id,
                                    SaHpiSensorNumT num,
                                    SaHpiSensorReadingT *reading,
                                    SaHpiEventStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_thresholds(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       SaHpiSensorThresholdsT *thres)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_sensor_thresholds(void *hnd,
                                       SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       const SaHpiSensorThresholdsT *thres)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_enable(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiSensorNumT num,
                                   SaHpiBoolT *enable)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_sensor_enable(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiSensorNumT num,
                                   SaHpiBoolT enable)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_event_enables(void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiBoolT *enables)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_sensor_event_enables(void *hnd,
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          const SaHpiBoolT enables)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_event_masks(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiSensorNumT  num,
                                        SaHpiEventStateT *AssertEventMask,
                                        SaHpiEventStateT *DeassertEventMask)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_sensor_event_masks(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiSensorNumT num,
                                        SaHpiSensorEventMaskActionT act,
                                        SaHpiEventStateT AssertEventMask,
                                        SaHpiEventStateT DeassertEventMask)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_control_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT *mode,
                                   SaHpiCtrlStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_control_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT mode,
                                   SaHpiCtrlStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_idr_info(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrInfoT *idrinfo)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_idr_area_header(void *hnd,
                                     SaHpiResourceIdT rid,
                                     SaHpiIdrIdT idrid,
                                     SaHpiIdrAreaTypeT areatype,
                                     SaHpiEntryIdT areaid,
                                     SaHpiEntryIdT *nextareaid,
                                     SaHpiIdrAreaHeaderT *header)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_add_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrAreaTypeT areatype,
                              SaHpiEntryIdT *areaid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_del_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiEntryIdT areaid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiIdrFieldTypeT fieldtype,
                               SaHpiEntryIdT fieldid,
                               SaHpiEntryIdT *nextfieldid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_add_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_del_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiEntryIdT fieldid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_watchdog_info(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_watchdog_info(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_reset_watchdog(void *hnd,
                                SaHpiResourceIdT id,
                                SaHpiWatchdogNumT num)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_next_announce(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiAnnunciatorNumT num,
                                   SaHpiSeverityT sev,
                                   SaHpiBoolT unack,
                                   SaHpiAnnouncementT *a)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiAnnouncementT *a)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_ack_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiSeverityT sev)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_add_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiAnnouncementT *a)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_del_announce(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT aid,
                              SaHpiSeverityT sev)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_annunc_mode(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT *mode)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_annunc_mode(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT mode)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_hotswap_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiHsStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_hotswap_state(void *hnd,
                                   SaHpiResourceIdT id,
                                   SaHpiHsStateT state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_request_hotswap_action(void *hnd,
                                        SaHpiResourceIdT id,
                                        SaHpiHsActionT act)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_power_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiPowerStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_power_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiPowerStateT state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_indicator_state(void *hnd,
                                     SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT *state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_indicator_state(void *hnd,
                                     SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT state)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_control_parm(void *hnd,
                              SaHpiResourceIdT id,
                              SaHpiParmActionT act)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_reset_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiResetActionT *act)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_reset_state(void *hnd,
                                 SaHpiResourceIdT id,
                                 SaHpiResetActionT act)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sel_info (void *hnd, 
                               SaHpiResourceIdT id, 
			       SaHpiEventLogInfoT *evtlog)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}			       

SaErrorT oh_rtas_set_sel_time (void *hnd, 
                             SaHpiResourceIdT id, 
			     const SaHpiEventT *evt)
{
	return SA_ERR_HPI_INTERNAL_ERROR;
}	

SaErrorT oh_rtas_add_sel_entry (void *hnd, 
                                SaHpiResourceIdT id, 
				const SaHpiEventT *evt)
{
	return SA_ERR_HPI_INTERNAL_ERROR;
}
		     
SaErrorT oh_rtas_get_sel_entry (void *hnd, 
                                SaHpiResourceIdT id, 
				SaHpiEventLogEntryIdT current,
                                SaHpiEventLogEntryIdT *prev, 
				SaHpiEventLogEntryIdT *next,
                                SaHpiEventLogEntryT *entry, 
				SaHpiRdrT *rdr, 
				SaHpiRptEntryT  *rdtentry) 
{
	return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_get_sensor_event_enabled(void *hnd, 
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT sensornum,
                                          SaHpiBoolT *enable)
{
	return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT oh_rtas_set_sensor_event_enabled(void *hnd, 
                                          SaHpiResourceIdT id,
                                          SaHpiSensorNumT sensornum,
                                          SaHpiBoolT *enable)
{
	return SA_ERR_HPI_INTERNAL_ERROR;
}

					  			
/* Function ABI aliases */
void * oh_open (GHashTable *) __attribute__ ((weak, alias("oh_rtas_open")));
void * oh_close (void *) __attribute__ ((weak, alias("oh_rtas_close")));
void * oh_get_event (void *, struct oh_event *)
        __attribute__ ((weak, alias("oh_rtas_get_event")));
void * oh_discover_resources (void *)
        __attribute__ ((weak, alias("oh_rtas_discover_resources")));
void * oh_discover_domain_resource (void *, SaHpiDomainIdT)
        __attribute__ ((weak, alias("oh_rtas_discover_domain_resources")));
void * oh_set_resource_tag (void *, SaHpiResourceIdT, SaHpiTextBufferT *)
        __attribute__ ((weak, alias("oh_rtas_set_resource_tag")));
void * oh_set_resource_severity (void *, SaHpiResourceIdT, SaHpiSeverityT)
        __attribute__ ((weak, alias("oh_rtas_set_resource_severity")));
void * oh_get_el_info (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *)
        __attribute__ ((weak, alias("oh_rtas_get_sel_info")));
void * oh_set_el_time (void *, SaHpiResourceIdT, const SaHpiEventT *)
        __attribute__ ((weak, alias("oh_rtas_set_sel_time")));
void * oh_add_el_entry (void *, SaHpiResourceIdT, const SaHpiEventT *)
        __attribute__ ((weak, alias("oh_rtas_add_sel_entry")));
void * oh_get_el_entry (void *, SaHpiResourceIdT, SaHpiEventLogEntryIdT,
                        SaHpiEventLogEntryIdT *, SaHpiEventLogEntryIdT *,
                        SaHpiEventLogEntryT *, SaHpiRdrT *, SaHpiRptEntryT  *)
        __attribute__ ((weak, alias("oh_rtas_get_sel_entry")));
void * oh_clear_el (void *, SaHpiResourceIdT)
        __attribute__ ((weak, alias("oh_rtas_clear_el")));
void * oh_set_el_state (void *, SaHpiResourceIdT)
        __attribute__ ((weak, alias("oh_rtas_set_el_state")));
void * oh_reset_el_overflow (void *, SaHpiResourceIdT)
        __attribute__ ((weak, alias("oh_rtas_reset_el_overflow")));
void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                              SaHpiSensorNumT,
                              SaHpiSensorReadingT *,
                              SaHpiEventStateT    *)
        __attribute__ ((weak, alias("oh_rtas_get_sensor_reading")));
void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *)
        __attribute__ ((weak, alias("oh_rtas_get_sensor_thresholds")));
void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *)
        __attribute__ ((weak, alias("oh_rtas_set_sensor_thresholds")));
void * oh_get_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT *)
        __attribute__ ((weak, alias("oh_rtas_get_sensor_enable")));
void * oh_set_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT)
        __attribute__ ((weak, alias("oh_rtas_set_sensor_enable")));
void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
        __attribute__ ((weak, alias("oh_rtas_get_sensor_event_enabled")));
void * oh_set_sensor_event_enables (void *, SaHpiResourceIdT id, SaHpiSensorNumT,
                                    SaHpiBoolT *)
        __attribute__ ((weak, alias("oh_rtas_set_sensor_event_enabled")));
void * oh_get_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiEventStateT *, SaHpiEventStateT *)
        __attribute__ ((weak, alias("oh_rtas_get_sensor_event_masks")));
void * oh_set_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiSensorEventMaskActionT,
                                  SaHpiEventStateT,
                                  SaHpiEventStateT)
        __attribute__ ((weak, alias("oh_rtas_set_sensor_event_masks")));
void * oh_get_control_state (void *, SaHpiResourceIdT, SaHpiCtrlNumT,
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *)
        __attribute__ ((weak, alias("oh_rtas_get_control_state")));
void * oh_set_control_state (void *, SaHpiResourceIdT,SaHpiCtrlNumT,
                             SaHpiCtrlModeT, SaHpiCtrlStateT *)
        __attribute__ ((weak, alias("oh_rtas_set_control_state")));
void * oh_get_idr_info (void *hnd, SaHpiResourceIdT, SaHpiIdrIdT,SaHpiIdrInfoT)
        __attribute__ ((weak, alias("oh_rtas_get_idr_info")));
void * oh_get_idr_area_header (void *, SaHpiResourceIdT, SaHpiIdrIdT,
                               SaHpiIdrAreaTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                               SaHpiIdrAreaHeaderT)
        __attribute__ ((weak, alias("oh_rtas_get_idr_area_header")));
void * oh_add_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT)
        __attribute__ ((weak, alias("oh_rtas_add_idr_area")));
void * oh_del_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT)
        __attribute__ ((weak, alias("oh_rtas_del_idr_area")));
void * oh_get_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldT)
        __attribute__ ((weak, alias("oh_rtas_get_idr_field")));
void * oh_add_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
        __attribute__ ((weak, alias("oh_rtas_add_idr_field")));
void * oh_set_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
        __attribute__ ((weak, alias("oh_rtas_set_idr_field")));
void * oh_del_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiEntryIdT)
        __attribute__ ((weak, alias("oh_rtas_del_idr_field")));
void * oh_get_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
        __attribute__ ((weak, alias("oh_rtas_get_watchdog_info")));
void * oh_set_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
        __attribute__ ((weak, alias("oh_rtas_set_watchdog_info")));
void * oh_get_next_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                             SaHpiSeverityT, SaHpiBoolT, SaHpiAnnouncementT)
        __attribute__ ((weak, alias("oh_rtas_get_next_announce")));
void * oh_get_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiAnnouncementT *)
        __attribute__ ((weak, alias("oh_rtas_get_announce")));
void * oh_ack_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiSeverityT)
        __attribute__ ((weak, alias("oh_rtas_ack_announce")));
void * oh_add_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiAnnouncementT *)
        __attribute__ ((weak, alias("oh_rtas_add_announce")));
void * oh_del_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                       SaHpiEntryIdT, SaHpiSeverityT)
        __attribute__ ((weak, alias("oh_rtas_del_announce")));
void * oh_get_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT *)
        __attribute__ ((weak, alias("oh_rtas_get_annunc_mode")));
void * oh_set_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT)
        __attribute__ ((weak, alias("oh_rtas_set_annunc_mode")));
void * oh_reset_watchdog (void *, SaHpiResourceIdT, SaHpiWatchdogNumT)
        __attribute__ ((weak, alias("oh_rtas_reset_watchdog")));
void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
        __attribute__ ((weak, alias("oh_rtas_get_hotswap_state")));
void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
        __attribute__ ((weak, alias("oh_rtas_set_hotswap_state")));
void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
        __attribute__ ((weak, alias("oh_rtas_request_hotswap_action")));
void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
        __attribute__ ((weak, alias("oh_rtas_get_power_state")));
void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
        __attribute__ ((weak, alias("oh_rtas_set_power_state")));
void * oh_get_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT *)
        __attribute__ ((weak, alias("oh_rtas_get_indicator_state")));
void * oh_set_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT)
        __attribute__ ((weak, alias("oh_rtas_set_indicator_state")));
void * oh_control_parm (void *, SaHpiResourceIdT, SaHpiParmActionT)
        __attribute__ ((weak, alias("oh_rtas_control_parm")));
void * oh_get_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT *)
        __attribute__ ((weak, alias("oh_rtas_get_reset_state")));
void * oh_set_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT)
        __attribute__ ((weak, alias("oh_rtas_set_reset_state")));
