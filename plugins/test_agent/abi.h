/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef ABI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define ABI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <SaHpi.h>


extern "C" {


/**************************************************************
 * Test_agent plugin interface
 *************************************************************/

void * oh_open(
    GHashTable * handler_config,
    unsigned int hid,
    oh_evt_queue * eventq
);

void oh_close(
    void * hnd
);

SaErrorT oh_discover_resources(
     void * hnd
);

SaErrorT oh_set_resource_tag(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTextBufferT * tag
);

SaErrorT oh_set_resource_severity(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSeverityT sev
);

SaErrorT oh_resource_failed_remove(
    void * hnd,
    SaHpiResourceIdT rid
);

SaErrorT oh_get_el_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogInfoT * info
);

SaErrorT oh_get_el_caps(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogCapabilitiesT * caps
);

SaErrorT oh_set_el_time(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeT time
);

SaErrorT oh_add_el_entry(
    void * hnd,
    SaHpiResourceIdT rid,
    const SaHpiEventT * event
);

SaErrorT oh_get_el_entry(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogEntryIdT current,
    SaHpiEventLogEntryIdT * prev,
    SaHpiEventLogEntryIdT * next,
    SaHpiEventLogEntryT * entry,
    SaHpiRdrT * rdr,
    SaHpiRptEntryT * rpte
);

SaErrorT oh_clear_el(
    void * hnd,
    SaHpiResourceIdT rid
);

SaErrorT oh_set_el_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiBoolT e
);

SaErrorT oh_reset_el_overflow(
    void * hnd,
    SaHpiResourceIdT rid
);

SaErrorT oh_get_sensor_reading(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorReadingT * reading,
    SaHpiEventStateT * state
);

SaErrorT oh_get_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorThresholdsT * thres
);

SaErrorT oh_set_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    const SaHpiSensorThresholdsT * thres
);

SaErrorT oh_get_sensor_enable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT * enable
);

SaErrorT oh_set_sensor_enable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT enable
);

SaErrorT oh_get_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT * enables
);

SaErrorT oh_set_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    const SaHpiBoolT enables
);

SaErrorT oh_get_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiEventStateT * AssertEventMask,
    SaHpiEventStateT * DeassertEventMask
);

SaErrorT oh_set_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorEventMaskActionT act,
    SaHpiEventStateT AssertEventMask,
    SaHpiEventStateT DeassertEventMask
);

SaErrorT oh_get_control_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT * mode,
    SaHpiCtrlStateT * state
);

SaErrorT oh_set_control_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT mode,
    SaHpiCtrlStateT * state
);

SaErrorT oh_get_idr_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrInfoT * idrinfo
);

SaErrorT oh_get_idr_area_header(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT * nextareaid,
    SaHpiIdrAreaHeaderT * header
);

SaErrorT oh_add_idr_area(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT * areaid
);

SaErrorT oh_add_idr_area_id(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid
);

SaErrorT oh_del_idr_area(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid
);

SaErrorT oh_get_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiIdrFieldTypeT fieldtype,
    SaHpiEntryIdT fieldid,
    SaHpiEntryIdT * nextfieldid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_add_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_add_idr_field_id(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_set_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_del_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT fieldid
);

SaErrorT oh_get_watchdog_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt
);

SaErrorT oh_set_watchdog_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt
);

SaErrorT oh_reset_watchdog(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num
);

SaErrorT oh_get_next_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiSeverityT sev,
    SaHpiBoolT ack,
    SaHpiAnnouncementT * a
);

SaErrorT oh_get_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiAnnouncementT * a
);

SaErrorT oh_ack_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiSeverityT sev
);

SaErrorT oh_add_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnouncementT * a
);

SaErrorT oh_del_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiSeverityT sev
);

SaErrorT oh_get_annunc_mode(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT * mode
);

SaErrorT oh_set_annunc_mode(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT mode
);

SaErrorT oh_get_dimi_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiInfoT * info
);

SaErrorT oh_get_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestT * test
);

SaErrorT oh_get_dimi_test_ready(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiReadyT * ready
);

SaErrorT oh_start_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiUint8T numparams,
    SaHpiDimiTestVariableParamsT * paramslist
);

SaErrorT oh_cancel_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum
);

SaErrorT oh_get_dimi_test_status(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestPercentCompletedT * percentcompleted,
    SaHpiDimiTestRunStatusT * runstatus
);

SaErrorT oh_get_dimi_test_results(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestResultsT * testresults
);

SaErrorT oh_get_fumi_spec(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiSpecInfoT * specinfo
);

SaErrorT oh_get_fumi_service_impact(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiServiceImpactDataT * serviceimpact
);

SaErrorT oh_set_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiTextBufferT * sourceuri
);

SaErrorT oh_validate_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiSourceInfoT * sourceinfo
);

SaErrorT oh_get_fumi_source_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo
);

SaErrorT oh_get_fumi_target(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiBankInfoT * bankinfo
);

SaErrorT oh_get_fumi_target_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo
);

SaErrorT oh_get_fumi_logical_target(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiLogicalBankInfoT * bankinfo
);

SaErrorT oh_get_fumi_logical_target_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiLogicalComponentInfoT * compinfo
);

SaErrorT oh_start_fumi_backup(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num
);

SaErrorT oh_set_fumi_bank_order(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiUint32T position
);

SaErrorT oh_start_fumi_bank_copy(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT sourcebanknum,
    SaHpiBankNumT targetbanknum
);

SaErrorT oh_start_fumi_install(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_status(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiUpgradeStatusT * status
);

SaErrorT oh_start_fumi_verify(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_start_fumi_verify_main(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num
);

SaErrorT oh_cancel_fumi_upgrade(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT * disable
);

SaErrorT oh_set_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT disable
);

SaErrorT oh_start_fumi_rollback(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num
);

SaErrorT oh_activate_fumi(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num
);

SaErrorT oh_start_fumi_activate(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT logical
);

SaErrorT oh_cleanup_fumi(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_hotswap_policy_cancel(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT timeout
);

SaErrorT oh_set_autoinsert_timeout(
    void * hnd,
    SaHpiTimeoutT timeout
);

SaErrorT oh_get_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT * timeout
);

SaErrorT oh_set_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT timeout
);

SaErrorT oh_get_hotswap_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsStateT * state
);

SaErrorT oh_set_hotswap_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsStateT state
);

SaErrorT oh_request_hotswap_action(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsActionT act
);

SaErrorT oh_get_indicator_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsIndicatorStateT * state
);

SaErrorT oh_set_indicator_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsIndicatorStateT state
);

SaErrorT oh_get_power_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiPowerStateT * state
);

SaErrorT oh_set_power_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiPowerStateT state
);

SaErrorT oh_control_parm(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiParmActionT act
);

SaErrorT oh_load_id_get(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiLoadIdT * load_id
);

SaErrorT oh_load_id_set(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiLoadIdT * load_id
);

SaErrorT oh_get_reset_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiResetActionT * act
);

SaErrorT oh_set_reset_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiResetActionT act
);


}; // extern "C"


#endif // ABI_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

