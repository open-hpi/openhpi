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

#include <string.h>

#include <string>

#include <glib.h>

#include <oh_error.h>

#include "baselib.h"


namespace Slave {


/**************************************************************
 * class cBaseLib
 *************************************************************/
cBaseLib::cBaseLib()
    : m_handle( 0 )
{
    memset( &m_abi, 0, sizeof(m_abi) );
}

cBaseLib::~cBaseLib()
{
    if ( m_handle ) {
        m_abi.saHpiFinalize();
        g_module_close( m_handle );
    }
}


void GetF( GModule * handle, const char * name, gpointer * pf, size_t& nerrors )
{
    gboolean rc = g_module_symbol( handle, name, pf );
    if ( rc == FALSE ) {
        *pf = 0;
        CRIT( "Problem with symbol %s in base library.", name );
        CRIT( "%s", g_module_error() );
        ++nerrors;
    }
}

bool cBaseLib::LoadBaseLib()
{
    if ( g_module_supported() == FALSE ) {
        CRIT( "GModule is not supported. Cannot load base library." );
        return false;
    }
    m_handle = g_module_open( "libopenhpi", G_MODULE_BIND_LOCAL );
    if ( !m_handle ) {
        CRIT( "Cannot load base library." );
        return false;
    }

    size_t nerrors = 0;

    GetF( m_handle,
          "saHpiVersionGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiVersionGet ),
          nerrors );
    GetF( m_handle,
          "saHpiInitialize",
          reinterpret_cast<gpointer *>( &m_abi.saHpiInitialize ),
          nerrors );
    GetF( m_handle,
          "saHpiFinalize",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFinalize ),
          nerrors );
    GetF( m_handle,
          "saHpiSessionOpen",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSessionOpen ),
          nerrors );
    GetF( m_handle,
          "saHpiSessionClose",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSessionClose ),
          nerrors );
    GetF( m_handle,
          "saHpiDiscover",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDiscover ),
          nerrors );
    GetF( m_handle,
          "saHpiDomainInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDomainInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDrtEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDrtEntryGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDomainTagSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDomainTagSet ),
          nerrors );
    GetF( m_handle,
          "saHpiRptEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRptEntryGet ),
          nerrors );
    GetF( m_handle,
          "saHpiRptEntryGetByResourceId",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRptEntryGetByResourceId ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceSeveritySet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceSeveritySet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceTagSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceTagSet ),
          nerrors );
    GetF( m_handle,
          "saHpiMyEntityPathGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiMyEntityPathGet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceIdGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceIdGet ),
          nerrors );
    GetF( m_handle,
          "saHpiGetIdByEntityPath",
          reinterpret_cast<gpointer *>( &m_abi.saHpiGetIdByEntityPath ),
          nerrors );
    GetF( m_handle,
          "saHpiGetChildEntityPath",
          reinterpret_cast<gpointer *>( &m_abi.saHpiGetChildEntityPath ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceFailedRemove",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceFailedRemove ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogCapabilitiesGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogCapabilitiesGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogEntryGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogEntryAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogEntryAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogClear",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogClear ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogTimeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogTimeGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogTimeSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogTimeSet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogStateGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogStateSet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventLogOverflowReset",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogOverflowReset ),
          nerrors );
    GetF( m_handle,
          "saHpiSubscribe",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSubscribe ),
          nerrors );
    GetF( m_handle,
          "saHpiUnsubscribe",
          reinterpret_cast<gpointer *>( &m_abi.saHpiUnsubscribe ),
          nerrors );
    GetF( m_handle,
          "saHpiEventGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventGet ),
          nerrors );
    GetF( m_handle,
          "saHpiEventAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiAlarmGetNext",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmGetNext ),
          nerrors );
    GetF( m_handle,
          "saHpiAlarmGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmGet ),
          nerrors );
    GetF( m_handle,
          "saHpiAlarmAcknowledge",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmAcknowledge ),
          nerrors );
    GetF( m_handle,
          "saHpiAlarmAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiAlarmDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmDelete ),
          nerrors );
    GetF( m_handle,
          "saHpiRdrGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrGet ),
          nerrors );
    GetF( m_handle,
          "saHpiRdrGetByInstrumentId",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrGetByInstrumentId ),
          nerrors );
    GetF( m_handle,
          "saHpiRdrUpdateCountGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrUpdateCountGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorReadingGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorReadingGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorThresholdsGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorThresholdsGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorThresholdsSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorThresholdsSet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorTypeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorTypeGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEnableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEnableGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEnableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEnableSet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEventEnableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventEnableGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEventEnableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventEnableSet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEventMasksGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventMasksGet ),
          nerrors );
    GetF( m_handle,
          "saHpiSensorEventMasksSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventMasksSet ),
          nerrors );
    GetF( m_handle,
          "saHpiControlTypeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlTypeGet ),
          nerrors );
    GetF( m_handle,
          "saHpiControlGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlGet ),
          nerrors );
    GetF( m_handle,
          "saHpiControlSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlSet ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrAreaHeaderGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaHeaderGet ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrAreaAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrAreaAddById",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaAddById ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrAreaDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaDelete ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrFieldGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldGet ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrFieldAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrFieldAddById",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldAddById ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrFieldSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldSet ),
          nerrors );
    GetF( m_handle,
          "saHpiIdrFieldDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldDelete ),
          nerrors );
    GetF( m_handle,
          "saHpiWatchdogTimerGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerGet ),
          nerrors );
    GetF( m_handle,
          "saHpiWatchdogTimerSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerSet ),
          nerrors );
    GetF( m_handle,
          "saHpiWatchdogTimerReset",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerReset ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorGetNext",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorGetNext ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorGet ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorAcknowledge",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorAcknowledge ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorAdd ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorDelete ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorModeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorModeGet ),
          nerrors );
    GetF( m_handle,
          "saHpiAnnunciatorModeSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorModeSet ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestReadinessGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestReadinessGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestStart ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestCancel ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestStatusGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestStatusGet ),
          nerrors );
    GetF( m_handle,
          "saHpiDimiTestResultsGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestResultsGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiSpecInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSpecInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiServiceImpactGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiServiceImpactGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiSourceSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceSet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiSourceInfoValidateStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceInfoValidateStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiSourceInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiSourceComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceComponentInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiTargetInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiTargetComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetComponentInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiLogicalTargetInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiLogicalTargetInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiLogicalTargetComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiLogicalTargetComponentInfoGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiBackupStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBackupStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiBankBootOrderSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBankBootOrderSet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiBankCopyStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBankCopyStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiInstallStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiInstallStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiUpgradeStatusGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiUpgradeStatusGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiTargetVerifyStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetVerifyStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiTargetVerifyMainStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetVerifyMainStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiUpgradeCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiUpgradeCancel ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiAutoRollbackDisableGet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiAutoRollbackDisableSet ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiRollbackStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiRollbackStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiActivate",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiActivate ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiActivateStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiActivateStart ),
          nerrors );
    GetF( m_handle,
          "saHpiFumiCleanup",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiCleanup ),
          nerrors );
    GetF( m_handle,
          "saHpiHotSwapPolicyCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapPolicyCancel ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceActiveSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceActiveSet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceInactiveSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceInactiveSet ),
          nerrors );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoInsertTimeoutGet ),
          nerrors );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoInsertTimeoutSet ),
          nerrors );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoExtractTimeoutGet ),
          nerrors );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoExtractTimeoutSet ),
          nerrors );
    GetF( m_handle,
          "saHpiHotSwapStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapStateGet ),
          nerrors );
    GetF( m_handle,
          "saHpiHotSwapActionRequest",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapActionRequest ),
          nerrors );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapIndicatorStateGet ),
          nerrors );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapIndicatorStateSet ),
          nerrors );
    GetF( m_handle,
          "saHpiParmControl",
          reinterpret_cast<gpointer *>( &m_abi.saHpiParmControl ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceLoadIdGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceLoadIdGet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceLoadIdSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceLoadIdSet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceResetStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceResetStateGet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourceResetStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceResetStateSet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourcePowerStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourcePowerStateGet ),
          nerrors );
    GetF( m_handle,
          "saHpiResourcePowerStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourcePowerStateSet ),
          nerrors );

    GetF( m_handle,
          "oHpiDomainAdd",
          reinterpret_cast<gpointer *>( &m_abi.oHpiDomainAdd ),
          nerrors );
    GetF( m_handle,
          "oHpiDomainAdd",
          reinterpret_cast<gpointer *>( &m_abi.oHpiDomainAdd ),
          nerrors );

    if ( nerrors != 0 ) {
        g_module_close( m_handle );
        m_handle = 0;
        return false;
    }

    SaErrorT rv = m_abi.saHpiInitialize( SAHPI_INTERFACE_VERSION, 0, 0, 0, 0 );
    if ( rv != SA_OK ) {
        g_module_close( m_handle );
        m_handle = 0;
        CRIT( "saHpiInitialize failed with rv = %d", rv );
        return false;
    }

    return true;
}


}; // namespace Slave

