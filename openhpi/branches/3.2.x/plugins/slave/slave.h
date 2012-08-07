/*      -*- linux-c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
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

#include <SaHpi.h>


extern "C" {


/**************************************************************
 * Slave plugin interface
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
    SaHpiResourceIdT id,
    SaHpiTextBufferT * tag
);

SaErrorT oh_set_resource_severity(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSeverityT sev
);

SaErrorT oh_resource_failed_remove(
    void * hnd,
    SaHpiResourceIdT id
);

SaErrorT oh_get_el_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogInfoT * info
);

SaErrorT oh_get_el_caps(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogCapabilitiesT * caps
);

SaErrorT oh_set_el_time(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeT time
);

SaErrorT oh_add_el_entry(
    void * hnd,
    SaHpiResourceIdT id,
    const SaHpiEventT * event
);

SaErrorT oh_get_el_entry(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogEntryIdT current,
    SaHpiEventLogEntryIdT * prev,
    SaHpiEventLogEntryIdT * next,
    SaHpiEventLogEntryT * entry,
    SaHpiRdrT * rdr,
    SaHpiRptEntryT * rpte
);

SaErrorT oh_clear_el(
    void * hnd,
    SaHpiResourceIdT id
);

SaErrorT oh_set_el_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiBoolT e
);

SaErrorT oh_reset_el_overflow(
    void * hnd,
    SaHpiResourceIdT id
);

SaErrorT oh_get_sensor_reading(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorReadingT * reading,
    SaHpiEventStateT * state
);

SaErrorT oh_get_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorThresholdsT * thres
);

SaErrorT oh_set_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    const SaHpiSensorThresholdsT * thres
);

SaErrorT oh_get_sensor_enable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT * enable
);

SaErrorT oh_set_sensor_enable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT enable
);

SaErrorT oh_get_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT * enables
);

SaErrorT oh_set_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    const SaHpiBoolT enables
);

SaErrorT oh_get_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiEventStateT * AssertEventMask,
    SaHpiEventStateT * DeassertEventMask
);

SaErrorT oh_set_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorEventMaskActionT act,
    SaHpiEventStateT AssertEventMask,
    SaHpiEventStateT DeassertEventMask
);

SaErrorT oh_get_control_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT * mode,
    SaHpiCtrlStateT * state
);

SaErrorT oh_set_control_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT mode,
    SaHpiCtrlStateT * state
);

SaErrorT oh_get_idr_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrInfoT * idrinfo
);

SaErrorT oh_get_idr_area_header(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT * nextareaid,
    SaHpiIdrAreaHeaderT * header
);

SaErrorT oh_add_idr_area(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT * areaid
);

SaErrorT oh_add_idr_area_id(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid
);

SaErrorT oh_del_idr_area(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid
);

SaErrorT oh_get_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiIdrFieldTypeT fieldtype,
    SaHpiEntryIdT fieldid,
    SaHpiEntryIdT * nextfieldid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_add_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_add_idr_field_id(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_set_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field
);

SaErrorT oh_del_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT fieldid
);

SaErrorT oh_get_watchdog_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt
);

SaErrorT oh_set_watchdog_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt
);

SaErrorT oh_reset_watchdog(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num
);

SaErrorT oh_get_next_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiSeverityT sev,
    SaHpiBoolT ack,
    SaHpiAnnouncementT * ann
);

SaErrorT oh_get_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiAnnouncementT * ann
);

SaErrorT oh_ack_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiSeverityT sev
);

SaErrorT oh_add_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnouncementT * ann
);

SaErrorT oh_del_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiSeverityT sev
);

SaErrorT oh_get_annunc_mode(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT * mode
);

SaErrorT oh_set_annunc_mode(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT mode
);

SaErrorT oh_get_dimi_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiInfoT * info
);

SaErrorT oh_get_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestT * test
);

SaErrorT oh_get_dimi_test_ready(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiReadyT * ready
);

SaErrorT oh_start_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiUint8T numparams,
    SaHpiDimiTestVariableParamsT * paramslist
);

SaErrorT oh_cancel_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum
);

SaErrorT oh_get_dimi_test_status(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestPercentCompletedT * percentcompleted,
    SaHpiDimiTestRunStatusT * runstatus
);

SaErrorT oh_get_dimi_test_results(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestResultsT * testresults
);

SaErrorT oh_get_fumi_spec(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiSpecInfoT * specinfo
);

SaErrorT oh_get_fumi_service_impact(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiServiceImpactDataT * serviceimpact
);

SaErrorT oh_set_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiTextBufferT * sourceuri
);

SaErrorT oh_validate_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiSourceInfoT * sourceinfo
);

SaErrorT oh_get_fumi_source_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo
);

SaErrorT oh_get_fumi_target(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiBankInfoT * bankinfo
);

SaErrorT oh_get_fumi_target_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo
);

SaErrorT oh_get_fumi_logical_target(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiLogicalBankInfoT * bankinfo
);

SaErrorT oh_get_fumi_logical_target_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiLogicalComponentInfoT * compinfo
);

SaErrorT oh_start_fumi_backup(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num
);

SaErrorT oh_set_fumi_bank_order(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiUint32T position
);

SaErrorT oh_start_fumi_bank_copy(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT sourcebanknum,
    SaHpiBankNumT targetbanknum
);

SaErrorT oh_start_fumi_install(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_status(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiUpgradeStatusT * status
);

SaErrorT oh_start_fumi_verify(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_start_fumi_verify_main(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num
);

SaErrorT oh_cancel_fumi_upgrade(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_get_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT * disable
);

SaErrorT oh_set_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT disable
);

SaErrorT oh_start_fumi_rollback(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num
);

SaErrorT oh_activate_fumi(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num
);

SaErrorT oh_start_fumi_activate(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT logical
);

SaErrorT oh_cleanup_fumi(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum
);

SaErrorT oh_hotswap_policy_cancel(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT timeout
);

SaErrorT oh_set_autoinsert_timeout(
    void * hnd,
    SaHpiTimeoutT timeout
);

SaErrorT oh_get_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT * timeout
);

SaErrorT oh_set_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT timeout
);

SaErrorT oh_get_hotswap_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsStateT * state
);

SaErrorT oh_set_hotswap_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsStateT state
);

SaErrorT oh_request_hotswap_action(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsActionT act
);

SaErrorT oh_get_indicator_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsIndicatorStateT * state
);

SaErrorT oh_set_indicator_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsIndicatorStateT state
);

SaErrorT oh_get_power_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiPowerStateT * state
);

SaErrorT oh_set_power_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiPowerStateT state
);

SaErrorT oh_control_parm(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiParmActionT act
);

SaErrorT oh_load_id_get(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiLoadIdT * load_id
);

SaErrorT oh_load_id_set(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiLoadIdT * load_id
);

SaErrorT oh_get_reset_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiResetActionT * act
);

SaErrorT oh_set_reset_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiResetActionT act
);


}; // extern "C"

