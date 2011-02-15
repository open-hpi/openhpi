/*      -*- linux-c++ -*-
 *
 *(C) Copyright Pigeon Point Systems. 2010
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

#include <dlfcn.h>
#include <string.h>

#include <string>

#include <oh_error.h>

#include "baselib.h"


namespace Slave {


/**************************************************************
 * class cBaseLib
 *************************************************************/
cBaseLib::cBaseLib()
    : m_handle( 0 )
{
    // empty
    memset( &m_abi, 0, sizeof(m_abi) );
}

cBaseLib::~cBaseLib()
{
    if ( m_handle ) {
        m_abi.saHpiFinalize();
        dlclose( m_handle );
    }
}


template <typename F>
void GetF( void * handle, const char * name, F& f, std::string& error )
{
    dlerror();
    f = 0;
    void * ptr = dlsym( handle, name );
    const char * dlerr = dlerror();
    if ( dlerr ) {
        error += dlerr;
        error.push_back( '\n' );
    }
    f = reinterpret_cast<F>(ptr);
}

bool cBaseLib::LoadBaseLib()
{
    m_handle = dlopen( "libopenhpi.so", RTLD_NOW | RTLD_LOCAL );
    if ( !m_handle ) {
        return false;
    }

    std::string error;

    GetF( m_handle,
          "saHpiVersionGet",
          m_abi.saHpiVersionGet,
          error );
    GetF( m_handle,
          "saHpiInitialize",
          m_abi.saHpiInitialize,
          error );
    GetF( m_handle,
          "saHpiFinalize",
          m_abi.saHpiFinalize,
          error );
    GetF( m_handle,
          "saHpiSessionOpen",
          m_abi.saHpiSessionOpen,
          error );
    GetF( m_handle,
          "saHpiSessionClose",
          m_abi.saHpiSessionClose,
          error );
    GetF( m_handle,
          "saHpiDiscover",
          m_abi.saHpiDiscover,
          error );
    GetF( m_handle,
          "saHpiDomainInfoGet",
          m_abi.saHpiDomainInfoGet,
          error );
    GetF( m_handle,
          "saHpiDrtEntryGet",
          m_abi.saHpiDrtEntryGet,
          error );
    GetF( m_handle,
          "saHpiDomainTagSet",
          m_abi.saHpiDomainTagSet,
          error );
    GetF( m_handle,
          "saHpiRptEntryGet",
          m_abi.saHpiRptEntryGet,
          error );
    GetF( m_handle,
          "saHpiRptEntryGetByResourceId",
          m_abi.saHpiRptEntryGetByResourceId,
          error );
    GetF( m_handle,
          "saHpiResourceSeveritySet",
          m_abi.saHpiResourceSeveritySet,
          error );
    GetF( m_handle,
          "saHpiResourceTagSet",
          m_abi.saHpiResourceTagSet,
          error );
    GetF( m_handle,
          "saHpiMyEntityPathGet",
          m_abi.saHpiMyEntityPathGet,
          error );
    GetF( m_handle,
          "saHpiResourceIdGet",
          m_abi.saHpiResourceIdGet,
          error );
    GetF( m_handle,
          "saHpiGetIdByEntityPath",
          m_abi.saHpiGetIdByEntityPath,
          error );
    GetF( m_handle,
          "saHpiGetChildEntityPath",
          m_abi.saHpiGetChildEntityPath,
          error );
    GetF( m_handle,
          "saHpiResourceFailedRemove",
          m_abi.saHpiResourceFailedRemove,
          error );
    GetF( m_handle,
          "saHpiEventLogInfoGet",
          m_abi.saHpiEventLogInfoGet,
          error );
    GetF( m_handle,
          "saHpiEventLogCapabilitiesGet",
          m_abi.saHpiEventLogCapabilitiesGet,
          error );
    GetF( m_handle,
          "saHpiEventLogEntryGet",
          m_abi.saHpiEventLogEntryGet,
          error );
    GetF( m_handle,
          "saHpiEventLogEntryAdd",
          m_abi.saHpiEventLogEntryAdd,
          error );
    GetF( m_handle,
          "saHpiEventLogClear",
          m_abi.saHpiEventLogClear,
          error );
    GetF( m_handle,
          "saHpiEventLogTimeGet",
          m_abi.saHpiEventLogTimeGet,
          error );
    GetF( m_handle,
          "saHpiEventLogTimeSet",
          m_abi.saHpiEventLogTimeSet,
          error );
    GetF( m_handle,
          "saHpiEventLogStateGet",
          m_abi.saHpiEventLogStateGet,
          error );
    GetF( m_handle,
          "saHpiEventLogStateSet",
          m_abi.saHpiEventLogStateSet,
          error );
    GetF( m_handle,
          "saHpiEventLogOverflowReset",
          m_abi.saHpiEventLogOverflowReset,
          error );
    GetF( m_handle,
          "saHpiSubscribe",
          m_abi.saHpiSubscribe,
          error );
    GetF( m_handle,
          "saHpiUnsubscribe",
          m_abi.saHpiUnsubscribe,
          error );
    GetF( m_handle,
          "saHpiEventGet",
          m_abi.saHpiEventGet,
          error );
    GetF( m_handle,
          "saHpiEventAdd",
          m_abi.saHpiEventAdd,
          error );
    GetF( m_handle,
          "saHpiAlarmGetNext",
          m_abi.saHpiAlarmGetNext,
          error );
    GetF( m_handle,
          "saHpiAlarmGet",
          m_abi.saHpiAlarmGet,
          error );
    GetF( m_handle,
          "saHpiAlarmAcknowledge",
          m_abi.saHpiAlarmAcknowledge,
          error );
    GetF( m_handle,
          "saHpiAlarmAdd",
          m_abi.saHpiAlarmAdd,
          error );
    GetF( m_handle,
          "saHpiAlarmDelete",
          m_abi.saHpiAlarmDelete,
          error );
    GetF( m_handle,
          "saHpiRdrGet",
          m_abi.saHpiRdrGet,
          error );
    GetF( m_handle,
          "saHpiRdrGetByInstrumentId",
          m_abi.saHpiRdrGetByInstrumentId,
          error );
    GetF( m_handle,
          "saHpiRdrUpdateCountGet",
          m_abi.saHpiRdrUpdateCountGet,
          error );
    GetF( m_handle,
          "saHpiSensorReadingGet",
          m_abi.saHpiSensorReadingGet,
          error );
    GetF( m_handle,
          "saHpiSensorThresholdsGet",
          m_abi.saHpiSensorThresholdsGet,
          error );
    GetF( m_handle,
          "saHpiSensorThresholdsSet",
          m_abi.saHpiSensorThresholdsSet,
          error );
    GetF( m_handle,
          "saHpiSensorTypeGet",
          m_abi.saHpiSensorTypeGet,
          error );
    GetF( m_handle,
          "saHpiSensorEnableGet",
          m_abi.saHpiSensorEnableGet,
          error );
    GetF( m_handle,
          "saHpiSensorEnableSet",
          m_abi.saHpiSensorEnableSet,
          error );
    GetF( m_handle,
          "saHpiSensorEventEnableGet",
          m_abi.saHpiSensorEventEnableGet,
          error );
    GetF( m_handle,
          "saHpiSensorEventEnableSet",
          m_abi.saHpiSensorEventEnableSet,
          error );
    GetF( m_handle,
          "saHpiSensorEventMasksGet",
          m_abi.saHpiSensorEventMasksGet,
          error );
    GetF( m_handle,
          "saHpiSensorEventMasksSet",
          m_abi.saHpiSensorEventMasksSet,
          error );
    GetF( m_handle,
          "saHpiControlTypeGet",
          m_abi.saHpiControlTypeGet,
          error );
    GetF( m_handle,
          "saHpiControlGet",
          m_abi.saHpiControlGet,
          error );
    GetF( m_handle,
          "saHpiControlSet",
          m_abi.saHpiControlSet,
          error );
    GetF( m_handle,
          "saHpiIdrInfoGet",
          m_abi.saHpiIdrInfoGet,
          error );
    GetF( m_handle,
          "saHpiIdrAreaHeaderGet",
          m_abi.saHpiIdrAreaHeaderGet,
          error );
    GetF( m_handle,
          "saHpiIdrAreaAdd",
          m_abi.saHpiIdrAreaAdd,
          error );
    GetF( m_handle,
          "saHpiIdrAreaAddById",
          m_abi.saHpiIdrAreaAddById,
          error );
    GetF( m_handle,
          "saHpiIdrAreaDelete",
          m_abi.saHpiIdrAreaDelete,
          error );
    GetF( m_handle,
          "saHpiIdrFieldGet",
          m_abi.saHpiIdrFieldGet,
          error );
    GetF( m_handle,
          "saHpiIdrFieldAdd",
          m_abi.saHpiIdrFieldAdd,
          error );
    GetF( m_handle,
          "saHpiIdrFieldAddById",
          m_abi.saHpiIdrFieldAddById,
          error );
    GetF( m_handle,
          "saHpiIdrFieldSet",
          m_abi.saHpiIdrFieldSet,
          error );
    GetF( m_handle,
          "saHpiIdrFieldDelete",
          m_abi.saHpiIdrFieldDelete,
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerGet",
          m_abi.saHpiWatchdogTimerGet,
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerSet",
          m_abi.saHpiWatchdogTimerSet,
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerReset",
          m_abi.saHpiWatchdogTimerReset,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorGetNext",
          m_abi.saHpiAnnunciatorGetNext,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorGet",
          m_abi.saHpiAnnunciatorGet,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorAcknowledge",
          m_abi.saHpiAnnunciatorAcknowledge,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorAdd",
          m_abi.saHpiAnnunciatorAdd,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorDelete",
          m_abi.saHpiAnnunciatorDelete,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorModeGet",
          m_abi.saHpiAnnunciatorModeGet,
          error );
    GetF( m_handle,
          "saHpiAnnunciatorModeSet",
          m_abi.saHpiAnnunciatorModeSet,
          error );
    GetF( m_handle,
          "saHpiDimiInfoGet",
          m_abi.saHpiDimiInfoGet,
          error );
    GetF( m_handle,
          "saHpiDimiTestInfoGet",
          m_abi.saHpiDimiTestInfoGet,
          error );
    GetF( m_handle,
          "saHpiDimiTestReadinessGet",
          m_abi.saHpiDimiTestReadinessGet,
          error );
    GetF( m_handle,
          "saHpiDimiTestStart",
          m_abi.saHpiDimiTestStart,
          error );
    GetF( m_handle,
          "saHpiDimiTestCancel",
          m_abi.saHpiDimiTestCancel,
          error );
    GetF( m_handle,
          "saHpiDimiTestStatusGet",
          m_abi.saHpiDimiTestStatusGet,
          error );
    GetF( m_handle,
          "saHpiDimiTestResultsGet",
          m_abi.saHpiDimiTestResultsGet,
          error );
    GetF( m_handle,
          "saHpiFumiSpecInfoGet",
          m_abi.saHpiFumiSpecInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiServiceImpactGet",
          m_abi.saHpiFumiServiceImpactGet,
          error );
    GetF( m_handle,
          "saHpiFumiSourceSet",
          m_abi.saHpiFumiSourceSet,
          error );
    GetF( m_handle,
          "saHpiFumiSourceInfoValidateStart",
          m_abi.saHpiFumiSourceInfoValidateStart,
          error );
    GetF( m_handle,
          "saHpiFumiSourceInfoGet",
          m_abi.saHpiFumiSourceInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiSourceComponentInfoGet",
          m_abi.saHpiFumiSourceComponentInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiTargetInfoGet",
          m_abi.saHpiFumiTargetInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiTargetComponentInfoGet",
          m_abi.saHpiFumiTargetComponentInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiLogicalTargetInfoGet",
          m_abi.saHpiFumiLogicalTargetInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiLogicalTargetComponentInfoGet",
          m_abi.saHpiFumiLogicalTargetComponentInfoGet,
          error );
    GetF( m_handle,
          "saHpiFumiBackupStart",
          m_abi.saHpiFumiBackupStart,
          error );
    GetF( m_handle,
          "saHpiFumiBankBootOrderSet",
          m_abi.saHpiFumiBankBootOrderSet,
          error );
    GetF( m_handle,
          "saHpiFumiBankCopyStart",
          m_abi.saHpiFumiBankCopyStart,
          error );
    GetF( m_handle,
          "saHpiFumiInstallStart",
          m_abi.saHpiFumiInstallStart,
          error );
    GetF( m_handle,
          "saHpiFumiUpgradeStatusGet",
          m_abi.saHpiFumiUpgradeStatusGet,
          error );
    GetF( m_handle,
          "saHpiFumiTargetVerifyStart",
          m_abi.saHpiFumiTargetVerifyStart,
          error );
    GetF( m_handle,
          "saHpiFumiTargetVerifyMainStart",
          m_abi.saHpiFumiTargetVerifyMainStart,
          error );
    GetF( m_handle,
          "saHpiFumiUpgradeCancel",
          m_abi.saHpiFumiUpgradeCancel,
          error );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableGet",
          m_abi.saHpiFumiAutoRollbackDisableGet,
          error );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableSet",
          m_abi.saHpiFumiAutoRollbackDisableSet,
          error );
    GetF( m_handle,
          "saHpiFumiRollbackStart",
          m_abi.saHpiFumiRollbackStart,
          error );
    GetF( m_handle,
          "saHpiFumiActivate",
          m_abi.saHpiFumiActivate,
          error );
    GetF( m_handle,
          "saHpiFumiActivateStart",
          m_abi.saHpiFumiActivateStart,
          error );
    GetF( m_handle,
          "saHpiFumiCleanup",
          m_abi.saHpiFumiCleanup,
          error );
    GetF( m_handle,
          "saHpiHotSwapPolicyCancel",
          m_abi.saHpiHotSwapPolicyCancel,
          error );
    GetF( m_handle,
          "saHpiResourceActiveSet",
          m_abi.saHpiResourceActiveSet,
          error );
    GetF( m_handle,
          "saHpiResourceInactiveSet",
          m_abi.saHpiResourceInactiveSet,
          error );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutGet",
          m_abi.saHpiAutoInsertTimeoutGet,
          error );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutSet",
          m_abi.saHpiAutoInsertTimeoutSet,
          error );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutGet",
          m_abi.saHpiAutoExtractTimeoutGet,
          error );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutSet",
          m_abi.saHpiAutoExtractTimeoutSet,
          error );
    GetF( m_handle,
          "saHpiHotSwapStateGet",
          m_abi.saHpiHotSwapStateGet,
          error );
    GetF( m_handle,
          "saHpiHotSwapActionRequest",
          m_abi.saHpiHotSwapActionRequest,
          error );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateGet",
          m_abi.saHpiHotSwapIndicatorStateGet,
          error );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateSet",
          m_abi.saHpiHotSwapIndicatorStateSet,
          error );
    GetF( m_handle,
          "saHpiParmControl",
          m_abi.saHpiParmControl,
          error );
    GetF( m_handle,
          "saHpiResourceLoadIdGet",
          m_abi.saHpiResourceLoadIdGet,
          error );
    GetF( m_handle,
          "saHpiResourceLoadIdSet",
          m_abi.saHpiResourceLoadIdSet,
          error );
    GetF( m_handle,
          "saHpiResourceResetStateGet",
          m_abi.saHpiResourceResetStateGet,
          error );
    GetF( m_handle,
          "saHpiResourceResetStateSet",
          m_abi.saHpiResourceResetStateSet,
          error );
    GetF( m_handle,
          "saHpiResourcePowerStateGet",
          m_abi.saHpiResourcePowerStateGet,
          error );
    GetF( m_handle,
          "saHpiResourcePowerStateSet",
          m_abi.saHpiResourcePowerStateSet,
          error );

    GetF( m_handle,
          "oHpiDomainAdd",
          m_abi.oHpiDomainAdd,
          error );

    if ( !error.empty() ) {
        dlclose( m_handle );
        m_handle = 0;
        err( "Slave: %s\n", error.c_str() );
        return false;
    }

    SaErrorT rv = m_abi.saHpiInitialize( SAHPI_INTERFACE_VERSION, 0, 0, 0, 0 );
    if ( rv != SA_OK ) {
        dlclose( m_handle );
        m_handle = 0;
        err( "Slave: saHpiInitialize failed with rv = %d\n", rv );
        return false;
    }

    return true;
}


}; // namespace Slave

