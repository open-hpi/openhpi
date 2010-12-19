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


void GetF( GModule * handle, const char * name, gpointer * pf, std::string& error )
{
    gboolean rc = g_module_symbol( handle, name, pf );
    if ( rc == FALSE ) {
        *pf = 0;
        error += g_module_error();
        error.push_back( '\n' );
    }
}

bool cBaseLib::LoadBaseLib()
{
    if ( g_module_supported() == FALSE ) {
        err( "ERROR. GModule is not supported. Cannot load base lib." );
        return false;
    }

    m_handle = g_module_open( "libopenhpi", G_MODULE_BIND_LOCAL );
    if ( !m_handle ) {
        return false;
    }

    std::string error;

    GetF( m_handle,
          "saHpiVersionGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiVersionGet ),
          error );
    GetF( m_handle,
          "saHpiInitialize",
          reinterpret_cast<gpointer *>( &m_abi.saHpiInitialize ),
          error );
    GetF( m_handle,
          "saHpiFinalize",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFinalize ),
          error );
    GetF( m_handle,
          "saHpiSessionOpen",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSessionOpen ),
          error );
    GetF( m_handle,
          "saHpiSessionClose",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSessionClose ),
          error );
    GetF( m_handle,
          "saHpiDiscover",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDiscover ),
          error );
    GetF( m_handle,
          "saHpiDomainInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDomainInfoGet ),
          error );
    GetF( m_handle,
          "saHpiDrtEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDrtEntryGet ),
          error );
    GetF( m_handle,
          "saHpiDomainTagSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDomainTagSet ),
          error );
    GetF( m_handle,
          "saHpiRptEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRptEntryGet ),
          error );
    GetF( m_handle,
          "saHpiRptEntryGetByResourceId",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRptEntryGetByResourceId ),
          error );
    GetF( m_handle,
          "saHpiResourceSeveritySet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceSeveritySet ),
          error );
    GetF( m_handle,
          "saHpiResourceTagSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceTagSet ),
          error );
    GetF( m_handle,
          "saHpiMyEntityPathGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiMyEntityPathGet ),
          error );
    GetF( m_handle,
          "saHpiResourceIdGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceIdGet ),
          error );
    GetF( m_handle,
          "saHpiGetIdByEntityPath",
          reinterpret_cast<gpointer *>( &m_abi.saHpiGetIdByEntityPath ),
          error );
    GetF( m_handle,
          "saHpiGetChildEntityPath",
          reinterpret_cast<gpointer *>( &m_abi.saHpiGetChildEntityPath ),
          error );
    GetF( m_handle,
          "saHpiResourceFailedRemove",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceFailedRemove ),
          error );
    GetF( m_handle,
          "saHpiEventLogInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogInfoGet ),
          error );
    GetF( m_handle,
          "saHpiEventLogCapabilitiesGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogCapabilitiesGet ),
          error );
    GetF( m_handle,
          "saHpiEventLogEntryGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogEntryGet ),
          error );
    GetF( m_handle,
          "saHpiEventLogEntryAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogEntryAdd ),
          error );
    GetF( m_handle,
          "saHpiEventLogClear",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogClear ),
          error );
    GetF( m_handle,
          "saHpiEventLogTimeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogTimeGet ),
          error );
    GetF( m_handle,
          "saHpiEventLogTimeSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogTimeSet ),
          error );
    GetF( m_handle,
          "saHpiEventLogStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogStateGet ),
          error );
    GetF( m_handle,
          "saHpiEventLogStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogStateSet ),
          error );
    GetF( m_handle,
          "saHpiEventLogOverflowReset",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventLogOverflowReset ),
          error );
    GetF( m_handle,
          "saHpiSubscribe",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSubscribe ),
          error );
    GetF( m_handle,
          "saHpiUnsubscribe",
          reinterpret_cast<gpointer *>( &m_abi.saHpiUnsubscribe ),
          error );
    GetF( m_handle,
          "saHpiEventGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventGet ),
          error );
    GetF( m_handle,
          "saHpiEventAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiEventAdd ),
          error );
    GetF( m_handle,
          "saHpiAlarmGetNext",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmGetNext ),
          error );
    GetF( m_handle,
          "saHpiAlarmGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmGet ),
          error );
    GetF( m_handle,
          "saHpiAlarmAcknowledge",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmAcknowledge ),
          error );
    GetF( m_handle,
          "saHpiAlarmAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmAdd ),
          error );
    GetF( m_handle,
          "saHpiAlarmDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAlarmDelete ),
          error );
    GetF( m_handle,
          "saHpiRdrGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrGet ),
          error );
    GetF( m_handle,
          "saHpiRdrGetByInstrumentId",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrGetByInstrumentId ),
          error );
    GetF( m_handle,
          "saHpiRdrUpdateCountGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiRdrUpdateCountGet ),
          error );
    GetF( m_handle,
          "saHpiSensorReadingGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorReadingGet ),
          error );
    GetF( m_handle,
          "saHpiSensorThresholdsGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorThresholdsGet ),
          error );
    GetF( m_handle,
          "saHpiSensorThresholdsSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorThresholdsSet ),
          error );
    GetF( m_handle,
          "saHpiSensorTypeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorTypeGet ),
          error );
    GetF( m_handle,
          "saHpiSensorEnableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEnableGet ),
          error );
    GetF( m_handle,
          "saHpiSensorEnableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEnableSet ),
          error );
    GetF( m_handle,
          "saHpiSensorEventEnableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventEnableGet ),
          error );
    GetF( m_handle,
          "saHpiSensorEventEnableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventEnableSet ),
          error );
    GetF( m_handle,
          "saHpiSensorEventMasksGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventMasksGet ),
          error );
    GetF( m_handle,
          "saHpiSensorEventMasksSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiSensorEventMasksSet ),
          error );
    GetF( m_handle,
          "saHpiControlTypeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlTypeGet ),
          error );
    GetF( m_handle,
          "saHpiControlGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlGet ),
          error );
    GetF( m_handle,
          "saHpiControlSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiControlSet ),
          error );
    GetF( m_handle,
          "saHpiIdrInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrInfoGet ),
          error );
    GetF( m_handle,
          "saHpiIdrAreaHeaderGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaHeaderGet ),
          error );
    GetF( m_handle,
          "saHpiIdrAreaAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaAdd ),
          error );
    GetF( m_handle,
          "saHpiIdrAreaAddById",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaAddById ),
          error );
    GetF( m_handle,
          "saHpiIdrAreaDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrAreaDelete ),
          error );
    GetF( m_handle,
          "saHpiIdrFieldGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldGet ),
          error );
    GetF( m_handle,
          "saHpiIdrFieldAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldAdd ),
          error );
    GetF( m_handle,
          "saHpiIdrFieldAddById",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldAddById ),
          error );
    GetF( m_handle,
          "saHpiIdrFieldSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldSet ),
          error );
    GetF( m_handle,
          "saHpiIdrFieldDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiIdrFieldDelete ),
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerGet ),
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerSet ),
          error );
    GetF( m_handle,
          "saHpiWatchdogTimerReset",
          reinterpret_cast<gpointer *>( &m_abi.saHpiWatchdogTimerReset ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorGetNext",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorGetNext ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorGet ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorAcknowledge",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorAcknowledge ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorAdd",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorAdd ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorDelete",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorDelete ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorModeGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorModeGet ),
          error );
    GetF( m_handle,
          "saHpiAnnunciatorModeSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAnnunciatorModeSet ),
          error );
    GetF( m_handle,
          "saHpiDimiInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiInfoGet ),
          error );
    GetF( m_handle,
          "saHpiDimiTestInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestInfoGet ),
          error );
    GetF( m_handle,
          "saHpiDimiTestReadinessGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestReadinessGet ),
          error );
    GetF( m_handle,
          "saHpiDimiTestStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestStart ),
          error );
    GetF( m_handle,
          "saHpiDimiTestCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestCancel ),
          error );
    GetF( m_handle,
          "saHpiDimiTestStatusGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestStatusGet ),
          error );
    GetF( m_handle,
          "saHpiDimiTestResultsGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiDimiTestResultsGet ),
          error );
    GetF( m_handle,
          "saHpiFumiSpecInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSpecInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiServiceImpactGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiServiceImpactGet ),
          error );
    GetF( m_handle,
          "saHpiFumiSourceSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceSet ),
          error );
    GetF( m_handle,
          "saHpiFumiSourceInfoValidateStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceInfoValidateStart ),
          error );
    GetF( m_handle,
          "saHpiFumiSourceInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiSourceComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiSourceComponentInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiTargetInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiTargetComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetComponentInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiLogicalTargetInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiLogicalTargetInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiLogicalTargetComponentInfoGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiLogicalTargetComponentInfoGet ),
          error );
    GetF( m_handle,
          "saHpiFumiBackupStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBackupStart ),
          error );
    GetF( m_handle,
          "saHpiFumiBankBootOrderSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBankBootOrderSet ),
          error );
    GetF( m_handle,
          "saHpiFumiBankCopyStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiBankCopyStart ),
          error );
    GetF( m_handle,
          "saHpiFumiInstallStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiInstallStart ),
          error );
    GetF( m_handle,
          "saHpiFumiUpgradeStatusGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiUpgradeStatusGet ),
          error );
    GetF( m_handle,
          "saHpiFumiTargetVerifyStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetVerifyStart ),
          error );
    GetF( m_handle,
          "saHpiFumiTargetVerifyMainStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiTargetVerifyMainStart ),
          error );
    GetF( m_handle,
          "saHpiFumiUpgradeCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiUpgradeCancel ),
          error );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiAutoRollbackDisableGet ),
          error );
    GetF( m_handle,
          "saHpiFumiAutoRollbackDisableSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiAutoRollbackDisableSet ),
          error );
    GetF( m_handle,
          "saHpiFumiRollbackStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiRollbackStart ),
          error );
    GetF( m_handle,
          "saHpiFumiActivate",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiActivate ),
          error );
    GetF( m_handle,
          "saHpiFumiActivateStart",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiActivateStart ),
          error );
    GetF( m_handle,
          "saHpiFumiCleanup",
          reinterpret_cast<gpointer *>( &m_abi.saHpiFumiCleanup ),
          error );
    GetF( m_handle,
          "saHpiHotSwapPolicyCancel",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapPolicyCancel ),
          error );
    GetF( m_handle,
          "saHpiResourceActiveSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceActiveSet ),
          error );
    GetF( m_handle,
          "saHpiResourceInactiveSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceInactiveSet ),
          error );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoInsertTimeoutGet ),
          error );
    GetF( m_handle,
          "saHpiAutoInsertTimeoutSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoInsertTimeoutSet ),
          error );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoExtractTimeoutGet ),
          error );
    GetF( m_handle,
          "saHpiAutoExtractTimeoutSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiAutoExtractTimeoutSet ),
          error );
    GetF( m_handle,
          "saHpiHotSwapStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapStateGet ),
          error );
    GetF( m_handle,
          "saHpiHotSwapActionRequest",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapActionRequest ),
          error );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapIndicatorStateGet ),
          error );
    GetF( m_handle,
          "saHpiHotSwapIndicatorStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiHotSwapIndicatorStateSet ),
          error );
    GetF( m_handle,
          "saHpiParmControl",
          reinterpret_cast<gpointer *>( &m_abi.saHpiParmControl ),
          error );
    GetF( m_handle,
          "saHpiResourceLoadIdGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceLoadIdGet ),
          error );
    GetF( m_handle,
          "saHpiResourceLoadIdSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceLoadIdSet ),
          error );
    GetF( m_handle,
          "saHpiResourceResetStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceResetStateGet ),
          error );
    GetF( m_handle,
          "saHpiResourceResetStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourceResetStateSet ),
          error );
    GetF( m_handle,
          "saHpiResourcePowerStateGet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourcePowerStateGet ),
          error );
    GetF( m_handle,
          "saHpiResourcePowerStateSet",
          reinterpret_cast<gpointer *>( &m_abi.saHpiResourcePowerStateSet ),
          error );

    GetF( m_handle,
          "oHpiDomainAdd",
          reinterpret_cast<gpointer *>( &m_abi.oHpiDomainAdd ),
          error );

    if ( !error.empty() ) {
        g_module_close( m_handle );
        m_handle = 0;
        err( "Slave: %s\n", error.c_str() );
        return false;
    }

    SaErrorT rv = m_abi.saHpiInitialize( SAHPI_INTERFACE_VERSION, 0, 0, 0, 0 );
    if ( rv != SA_OK ) {
        g_module_close( m_handle );
        m_handle = 0;
        err( "Slave: saHpiInitialize failed with rv = %d\n", rv );
        return false;
    }

    return true;
}


}; // namespace Slave

