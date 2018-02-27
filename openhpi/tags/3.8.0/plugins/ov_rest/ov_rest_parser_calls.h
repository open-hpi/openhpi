/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 *
 */

#ifndef _PARSER_CALLS_H
#define _PARSER_CALLS_H

#include<json-c/json.h>
#include "ov_rest_callsupport.h"

/* Moved these #defines from ov_rest.h - which includes this header file. */
/* Max Blade in HPE Synergy enclosure*/
#define OV_REST_C7000_MAX_BLADE 16
/* Max Blade in HPE Synergy enclosure*/
#define OV_REST_SYNERGY_MAX_BLADE 12
/* Max Blade in HPE Synergy enclosure*/
#define OV_REST_C3000_MAX_BLADE 8

OV_REST_ENUM(uidStatus,
        UID_OFF,
        UID_ON,
	UID_BLINK,
	UID_BLINKING,
	UID_FLASHING,
	UID_UNSUPPORTED,
	UID_NA)

typedef unsigned int byte;
#define MAX_256_CHARS 256
#define MAX_UUID_LENGTH 37
#define MAX_URI_LENGTH 128
#define MAX_MANUFACTURER_LENGTH 5
enum opStatus {
        OP_STATUS_UNKNOWN,
        OP_STATUS_OTHER,
        OP_STATUS_OK,
        OP_STATUS_CRITICAL,
        OP_STATUS_WARNING,
        OP_STATUS_DISABLED};
OV_REST_ENUM_STRING(opStatus,
        OP_STATUS_UNKNOWN,
        OP_STATUS_OTHER,
        OP_STATUS_OK,
	OP_STATUS_CRITICAL,
	OP_STATUS_WARNING,
	OP_STATUS_DISABLED)


OV_REST_ENUM(hpeov_boolean,
        HPEOV_FALSE,
        HPEOV_TRUE)
OV_REST_ENUM(powerState,
        Off,
        On,
        PoweringOff,
        PoweringOn,
        Restting,
        Unknown)
OV_REST_ENUM(power,
        PS_NO_OP,
        PS_UNKNOWN,
        PS_OFF,
        PS_LOW,
        PS_AUTOMATIC,
        PS_MAXIMUM)
OV_REST_ENUM(healthStatus,
	Other,
	OK,
	Disabled,
	Warning,
	Critical)
OV_REST_ENUM(presence,
        Absent,
        PresenceNoOp,
        PresenceUnknown,
        Present,
        Subsumed)
OV_REST_ENUM(changeState,
        Insert,
        Remove,
	None)
OV_REST_ENUM(bladeType,
        BLADE_TYPE_NO_OP,
        BLADE_TYPE_UNKNOWN,
        BLADE_TYPE_SERVER,
        BLADE_TYPE_STORAGE,
        BLADE_TYPE_WORKSTATION,
        BLADE_TYPE_IO)
OV_REST_ENUM(eventType,
        cpqRackServerBladeInserted2,
        BladeInserted,
        cpqRackServerBladeRemoved2,
        BladeRemoved,
        InterconnectOk,
        cpqRackNetConnectorInserted,
        InterconnectInserted,
        cpqRackNetConnectorRemoved,
        InterconnectRemoved,
        InterconnectDiscoveryComplete,
        cpqDa6AccelStatusChange,
        PowerSupplyInserted,
        PowerSupplyRemoved,
        FanInserted,
        FanRemoved,
        cpqSm2ServerPowerOn,
        cpqSm2ServerPowerOff,
        cpqSm2ServerReset,
        switchresponse,
        HealthStatusArrayCategoryStatus,
        InterconnectPowerRequestGranted,
        InterconnectManagerICMReadyFault,
        InterconnectManagerICMReadyFaultCleared,
        BladeManagerIloInternalCritical,
        BladeManagerIloInternalCriticalCleared,
        BladeManagerIloInternalWarning,
        BladeManagerIloInternalWarningCleared,
        EmIsActive,
        PowerSupplyInputLossFault,
        invalidTopology,
        neighbourIcPoweredOff16,
        neighbourIcPoweredOff46,
        cxpPortConnected,
        cxpPortMisCabled,
        cxpPortNotConnected,
        CxpPortNotConnectedToExpectedICMKind,
        failedCableRead,
        flexFabric40GbEModuleAtSideANotInFirstEnclosure,
        flexFabric40GbEModuleAtSideBNotInFirstOrSecondEnclosure,
        icNotinSameBaySet,
        InvalidInterconnectKind,
        mixedIcInSameBaySet,
        mixedIcLnSameBaySet,
        moreThanOneflexFabric40GbEModuleAtOneSideofBaySet,
        neighbourIcPoweredOff,
        noflexFabric40GbEModuleAtSideA,
        nonHpeCableInserted,
        PortStatusLinked,
        PortStatusLinkedInvalidPort,
        PortStatusUnlinked,        
        unsupportedlc,
        PortStatusLinked1,
        PortStatusLinked2,
        PortStatusLinked4,
        PortStatusUnlinked1,
        PortStatusUnlinked2,
        PortStatusUnlinked4,
        opFcoeActiveChange,
        ilt,
        PowerSupplyInternalCommFault,
        PowerSupplyInternalCommFaultCleared,
        FanElectronicFuseBlownCleared,
        ActivateStagedFirmwareSuccessful,
        BladeCommFault,
        BladeCommFaultCleared,
        BladeFruFault,
        BladeIloOperationalStatusCritical,
        BladeInsertFault,
        BladeInsertFaultCleared,
        BladeManagerICMReadyFault,
        BladeManagerICMReadyFaultCleared,
        BladePowerOn,
        ChassisAmbientThermal,
        ChassisAmbientThermalCleared,
        ChassisUidBlinking,
        CIManagerCommFault,
        CIManagerCommFaultCleared,
        CIManagerEfusePGoodFault,
        CIManagerEfusePGoodFaultCleared,
        CIManagerFruFault,
        CIManagerIloFault,
        CIManagerIloFaultCleared,
        CIManagerIloInternalCritical,
        CIManagerIloInternalCriticalCleared,
        CIManagerIloInternalWarning,
        CIManagerIloInternalWarningCleared,
        CIManagerInsertFault,
        CIManagerInsertFaultCleared,
        EmCommFault,
        EmCommFaultCleared,
        EmDataReplicationFault,
        EmDataReplicationFaultCleared,
        EmFruFault,
        EmInsertFault,
        EmInsertFaultCleared,
        EmOneViewLinkValidationPingAddressMismatch,
        EmOneViewLinkValidationPingAddressMismatch_Cleared,
        EmOneViewLinkValidationPingFailed,
        EmOneViewLinkValidationPingFailed_Cleared,
        EmOneViewLinkValidationPingFailedCleared_PingAddressChanged,
        EmOneViewLinkValidationPingFailedCleared_PingDisabled,
        EmStagedFirmwareUpdated,
        EmSwitchHardwareCritical,
        EmSwitchHardwareOk,
        FanCommFault,
        FanCommFaultCleared,
        FanEfusePGoodFault,
        FanEfusePGoodFaultCleared,
        FanFruFault,
        FanInsertFault,
        FanInsertFaultCleared,
        FanRotor1Fault,
        FanRotor1FaultCleared,
        FanRotor2Fault,
        FanRotor2FaultCleared,
        FrontPanelCommFault,
        FrontPanelCommFaultCleared,
        FrontPanelFruFault,
        FrontPanelInserted,
        FrontPanelInsertFault,
        FrontPanelInsertFaultCleared,
        InterconnectCommFault,
        InterconnectCommFaultCleared,
        InterconnectFruFault,
        InterconnectInsertFault,
        InterconnectInsertFaultCleared,
        InterconnectPowerOff,
        InterconnectPowerOn,
        MidplaneChassisIdUpdated,
        MidplaneFruFault,
        PowerAllocationCollectionChanged,
        PowerAllocationDeleted,
        PowerAllocationPatched,
        PowerAllocationPosted,
        PowerSubsystemOverLimit,
        PowerSubsystemOverLimitCleared,
        PowerSupplyCommFault,
        PowerSupplyCommFaultCleared,
        PowerSupplyFruFault,
        PowerSupplyFruManufacturedForInvalidFault,
        PowerSupplyInputFault,
        PowerSupplyInsertFault,
        PowerSupplyInsertFaultCleared,
        PowerSupplyInterposerCommFault,
        PowerSupplyInterposerCommFaultCleared,
        PowerSupplyInterposerCritical,
        PowerSupplyInterposerDiscoveryComplete,
        PowerSupplyInterposerFruContentFault,
        PowerSupplyInterposerFruFault,
        PowerSupplyInterposerFruManufacturedForInvalidFault,
        PowerSupplyInterposerFruManufacturedForMismatchFault,
        PowerSupplyInterposerInserted,
        PowerSupplyInterposerInsertFault,
        PowerSupplyInterposerInsertFaultCleared,
        PowerSupplyInterposerOk,
        PowerSupplyInterposerPSCommFault,
        PowerSupplyInterposerPSCommFaultCleared,
        PowerSupplyInterposerRemoved,
        PowerSupplyInterposerWarning,
        PowerSupplyIpduInfoChanged,
        PowerSupplyOvertempFault,
        PowerSupplyPowerCycledFault,
        PowerSupplyPsOkFault,
        PowerSupplyReinsertFault,
        PowerSupplyReplaceFault,
        RingNoActiveMgmtPort,
        RingNoActiveMgmtPortCleared,
        RingNoActiveMgmtPortCleared_OwnerChanged,
        RingNoActiveMgmtPort_RingDegraded,
        StagedFirmwareOperationSuccessful,
        BladeRedundantCommFault,
        BladeRedundantCommFaultCleared,
        ChassisEventServiceSubscriptionOverrunFault,
        ChassisEventServiceSubscriptionOverrunFaultCleared,
        ChassisRedundantCommFault,
        ChassisRedundantCommFaultCleared,
        CIManagerRedundantCommFault,
        CiManagerRedundantCommFaultCleared,
        EmHighAssuranceBootCritical,
        EmRedundantCommFault,
        EmRedundantCommFaultCleared,
        FanRedundantCommFault,
        FanRedundantCommFaultCleared,
        FrontPanelRedundantCommFault,
        FrontPanelRedundantCommFaultCleared,
        InterconnectRedundantCommFault,
        InterconnectRedundantCommFaultCleared,
        OneViewServiceEventsCommFault,
        OneViewServiceEventsCommFaultCleared,
        PowerSupplyRedundantCommFault,
        SupplyRedundantCommFaultCleared,
        coldStart,
        linkDown,
        linkUp,
        TopologyError,
        PartnerSwitchCommunicationFailure,
        PartnerSwitchVersionMismatch,
        PartnerSwitchWWIDMismatch,
        PartnerSwitchNotPresent,
        NotConfigured,
        CommunicationError,
        opModeChange,
        subPortStatusChange,
        linkStateChange,
        connectionStateChange,
        cpqHe3TemperatureDegraded,
        cpqHe3ThermalConfirmation,
        cpqSeCpuStatusChange,
        authenticationFailure,
        cpqHe3PostError,
        cpqHe3TemperatureOk,
        cpqSm2SelfTestError,
        cpqSm2ServerPowerOnFailure,
        cpqSm2IrsCommFailure,
        cpqHo2PowerThresholdTrap,
        cpqIdeAtaDiskStatusChange,
        cpqSeUSBStorageDeviceReadErrorOccurred,
        cpqSeUSBStorageDeviceWriteErrorOccurred,
        cpqSeUSBStorageDeviceRedundancyLost,
        cpqSeUSBStorageDeviceRedundancyRestored,
        cpqSeUSBStorageDeviceSyncFailed,
        cpqSePCIeDiskTemperatureFailed,
        cpqSePCIeDiskTemperatureOk,
        cpqSePCIeDiskConditionChange,
        cpqSePCIeDiskWearStatusChange,
        cpqSePciDeviceAddedOrPoweredOn,
        cpqSePciDeviceRemovedOrPoweredOff,
        cpqFca3HostCntlrStatusChange,
        cpqDa6CntlrStatusChange,
        cpqDa6LogDrvStatusChange,
        cpqDa6AccelBadDataTrap,
        cpqDa6AccelBatteryFailed,
        cpqDa7PhyDrvStatusChange,
        cpqDa7SpareStatusChange,
        cpqDaPhyDrvSSDWearStatusChange,
        cpqHe3FltTolPowerRedundancyLost,
        cpqHe3FltTolPowerSupplyInserted,
        cpqHe3FltTolPowerSupplyRemoved,
        cpqHe3FltTolFanDegraded,
        cpqHe3FltTolFanFailed,
        cpqHe3FltTolFanRedundancyLost,
        cpqHe3FltTolFanInserted,
        cpqHe3FltTolFanRemoved,
        cpqHe4FltTolPowerSupplyOk,
        cpqHe4FltTolPowerSupplyDegraded,
        cpqHe4FltTolPowerSupplyFailed,
        cpqHeResilientMemMirroredMemoryEngaged,
        cpqHe3FltTolPowerRedundancyRestored,
        cpqHe3FltTolFanRedundancyRestored,
        cpqHe5CorrMemReplaceMemModule,
        cpqHe4FltTolPowerSupplyACpowerloss,
        cpqHeSysBatteryFailed,
        cpqHeSysBatteryRemoved,
        cpqHeSysPwrAllocationNotOptimized,
        cpqHeSysPwrOnDenied,
        cpqHePowerFailureError,
        cpqHeInterlockFailureError,
        cpqSs6FanStatusChange,
        cpqSs6TempStatusChange,
        cpqSs6PwrSupplyStatusChange,
        cpqSsConnectionStatusChange,
        cpqSm2UnauthorizedLoginAttempts,
        cpqSm2SecurityOverrideEngaged,
        cpqSm2SecurityOverrideDisengaged,
        cpqHo2GenericTrap,
        cpqHoMibHealthStatusArrayChangeTrap,
        cpqSasPhyDrvStatusChange,
        cpqSasPhyDrvSSDWearStatusChange,
        cpqNic3ConnectivityRestored,
        cpqNic3ConnectivityLost,
        cpqNic3RedundancyIncreased,
        cpqNic3RedundancyReduced,
        logicalInterconnectStateChange,
        logicalInterconnectStateCritical,
        logicalInterconnectStateAbsent,
        logicalInterconnectStateMismatch,
        complianceChange,
        nameChange,
        enableTaggedLLDP,
        enableRichTLV,
        uplinkSetStateChange,
        interconnectStateChange,
        sanVlanMismatch,
        cannotCommunicateWithSwitch,
        disabledInterconnectConnectivityForSwitch,
        wrongSwitch,
        duplicateSwitch,
        invalidSwitchCredentials,
        invalidXmlReturnedFromSwitch,
        BladeConfigurationFault,
        BladeConfigurationFaultCleared,
        BladeCritical,
        BladeDiscoveryComplete,
        BladeEfusePGoodFault,
        BladeEfusePGoodFaultCleared,
        BladeFault,
        BladeFaultCleared,
        BladeFruContentFault,
        BladeFruManufacturedForInvalidFault,
        BladeFruManufacturedForMismatchFault,
        BladeIloFailedCleared,
        BladeIloFailedCritical,
        BladeIloFruFault,
        BladeIloOperationalStatusCleared,
        BladeIloOperationalStatusWarning,
        BladeInsufficientCooling,
        BladeInsufficientCoolingCleared,
        BladeInsufficientPower,
        BladeInsufficientPowerCleared,
        BladeManagerCritical,
        BladeManagerFault,
        BladeManagerFaultCleared,
        BladeManagerInternalCritical,
        BladeManagerInternalCriticalCleared,
        BladeManagerInternalWarning,
        BladeManagerInternalWarningCleared,
        BladeManagerOk,
        BladeManagerWarning,
        BladeMateDetectFault,
        BladeMateDetectFaultCleared,
        BladeMezzFruContentFault,
        BladeMezzFruFault,
        BladeMezzFruManufacturedForInvalidFault,
        BladeMezzFruManufacturedForMismatchFault,
        BladeOk,
        BladePowerAllocationChanged,
        BladePowerDeniedDueToFru,
        BladePowerDeniedDueToMating,
        BladePowerDeniedDueToMatingCleared,
        BladePowerDeniedDueToMidplane,
        BladePowerOff,
        BladePowerReleaseGranted,
        BladePowerRequestGranted,
        BladeSystemPGoodFault,
        BladeSystemPGoodFaultCleared,
        BladeThermalCritical,
        BladeThermalOk,
        BladeThermalWarning,
        BladeWarning,
        CIManagerCritical,
        CIManagerDiscoveryComplete,
        CIManagerFault,
        CIManagerFaultCleared,
        CIManagerFruContentFault,
        CIManagerFruManufacturedForInvalidFault,
        CIManagerFruManufacturedForMismatchFault,
        CIManagerInserted,
        CIManagerOk,
        CIManagerRemoved,
        CIManagerWarning,
        CIManagerWarrantySerialUpdate,
        CertificateDeleted,
        CertificateSaved,
        ChassisCritical,
        ChassisOk,
        ChassisUidOff,
        ChassisUidOn,
        ChassisWarning,
        Claimed,
        DeviceInventoryCompleted,
        DeviceManagerCanmicInfoblockUpdated,
        DeviceManagerDataChanged,
        EmBooting,
        EmClusterResourceCleared,
        EmClusterResourceFault,
        EmCritical,
        EmDiscoveryComplete,
        EmFault,
        EmFaultCleared,
        EmFruContentFault,
        EmFruManufacturedForInvalidFault,
        EmFruManufacturedForMismatchFault,
        EmFwMatch,
        EmFwMismatch,
        EmImageManagerLinkDisconnected,
        EmImageManagerLinkDisconnectedCleared_MgmtPortConnected,
        EmImageManagerLinkDisconnectedCleared_MgmtPortModeChanged,
        EmInserted,
        EmInternalLinkDisconnected,
        EmInternalLinkDisconnectedCleared_InternalLinkConnected,
        EmInternalLinkDisconnectedCleared_PeerEmRemoved,
        EmLinkConnectedIncorrectly,
        EmLinkConnectedIncorrectlyCleared_LinkLost,
        EmLinkConnectedIncorrectlyCleared_OwnersOk,
        EmLinkDisconnected,
        EmLinkDisconnectedCleared_LinkDetected,
        EmLinkDisconnectedCleared_PeerEmRemoved,
        EmLinkNeighborInfoExpired,
        EmLinkNeighborInfoExpiredCleared,
        EmLinkNeighborOwnerMismatch,
        EmLinkNeighborOwnerMismatchCleared_LinkLost,
        EmLinkedNeighborsUpdated,
        EmMgmtPortConnectedIncorrectly,
        EmMgmtPortConnectedIncorrectlyCleared_MgmtPortDisconnected,
        EmMgmtPortConnectedIncorrectlyImageManager,
        EmMissing,
        EmMissingCleared,
        EmOk,
        EmOneViewLinkDisconnected,
        EmOneViewLinkDisconnectedCleared_MgmtPortConnected,
        EmOneViewLinkDisconnectedCleared_MgmtPortModeChanged,
        EmOneViewLinkDisconnectedCleared_OneViewRemoved,
        EmPeerEmNeeded,
        EmPeerEmNeededCleared_ElinkRemoved,
        EmPeerEmNeededCleared_PeerEmInserted,
        EmRemoved,
        EmSwitchCommunicationCritical,
        EmSwitchCommunicationOk,
        EmSwitchCritical,
        EmSwitchOk,
        EmSwitchPortOk,
        EmSwitchPortUpdated,
        EmSwitchPortWarning,
        EmSwitchWarning,
        EmThermalCritical,
        EmThermalOk,
        EmThermalSensorReadOk,
        EmThermalSensorReadWarning,
        EmThermalWarning,
        EmUidLedOff,
        EmUidLedOn,
        EmWarning,
        FailedLoginAttempt,
        FanCritical,
        FanDiscoveryComplete,
        FanElectronicFuseBlown,
        FanFault,
        FanFaultCleared,
        FanFruContentFault,
        FanFruManufacturedForInvalidFault,
        FanFruManufacturedForMismatchFault,
        FanOk,
        FanWarning,
        FrontPanelAttached,
        FrontPanelCritical,
        FrontPanelDiscoveryComplete,
        FrontPanelFault,
        FrontPanelFaultCleared,
        FrontPanelFruContentFault,
        FrontPanelFruManufacturedForInvalidFault,
        FrontPanelFruManufacturedForMismatchFault,
        FrontPanelOk,
        FrontPanelRemoved,
        FrontPanelWarning,
        InsufficientFansCritical,
        InsufficientFansOk,
        InsufficientFansWarning,
        InterconnectConfigurationFault,
        InterconnectConfigurationFaultCleared,
        InterconnectCritical,
        InterconnectEfusePGoodFault,
        InterconnectEfusePGoodFaultCleared,
        InterconnectFault,
        InterconnectFaultCleared,
        InterconnectFruContentFault,
        InterconnectFruManufacturedForInvalidFault,
        InterconnectFruManufacturedForMismatchFault,
        InterconnectInsufficientCooling,
        InterconnectInsufficientCoolingCleared,
        InterconnectInsufficientPower,
        InterconnectInsufficientPowerCleared,
        InterconnectManagerFault,
        InterconnectManagerFaultCleared,
        InterconnectManagerInternalCritical,
        InterconnectManagerInternalCriticalCleared,
        InterconnectManagerInternalWarning,
        InterconnectManagerInternalWarningCleared,
        InterconnectMateDetectFault,
        InterconnectMateDetectFaultCleared,
        InterconnectPowerDeniedDueToFru,
        InterconnectPowerDeniedDueToMating,
        InterconnectPowerDeniedDueToMatingCleared,
        InterconnectPowerDeniedDueToMidplane,
        InterconnectPowerReleaseGranted,
        InterconnectSystemPGoodFault,
        InterconnectSystemPGoodFaultCleared,
        InterconnectThermalCritical,
        InterconnectThermalOk,
        InterconnectThermalWarning,
        InterconnectWarning,
        InternalFailure,
        InvalidFirmwarePackage,
        MaintenanceLoginRejected,
        MaintenanceSessionStarted,
        MidplaneDiscoveryComplete,
        MidplaneFault,
        MidplaneFruContentFault,
        MidplaneFruManufacturedForInvalidFault,
        OVERRUN,
        PasswordChanged,
        PowerAlertModeActivated,
        PowerAlertModeArmed,
        PowerAlertModeDeactivated,
        PowerAlertModeDisarmed,
        PowerCritical,
        PowerFastCappingDisabled,
        PowerFastCappingEnabled,
        PowerInputFault,
        PowerInputFaultCleared,
        PowerOk,
        PowerSubsystemChanged,
        PowerSubsystemMismatch,
        PowerSubsystemMismatchCleared,
        PowerSubsystemOverload,
        PowerSubsystemOverloadCleared,
        PowerSubsystemRedundancyLost,
        PowerSubsystemRedundancyLostCleared,
        PowerSupplyCritical,
        PowerSupplyDiscoveryComplete,
        PowerSupplyFault,
        PowerSupplyFaultCleared,
        PowerSupplyFruContentFault,
        PowerSupplyFruManufacturedFrInvalidFault,
        PowerSupplyFruManufacturedForMismatchFault,
        PowerSupplyOk,
        PowerSupplyWarning,
        PowerWarning,
        RisTestEvent,
        SessionCreated,
        SessionDeleted,
        SessionTimeout,
        StandbyUpdateFailed,
        SubscriptionCreated,
        Success,
        ThermalCritical,
        ThermalOk,
        ThermalWarning,
        TooManySessions,
        UpdateInterrupted,
        UpdateSuccessful,
        OEM_EVENT)
OV_REST_ENUM(name,
        TASK_ADD,
        TASK_REMOVE,
        TASK_POWER_ON,
        TASK_POWER_OFF,
        TASK_ACTIVATE_STANDBY_APPLIANCE,
        TASK_REFRESH,
        TASK_COLLECT_UTILIZATION_DATA,
        TASK_MANAGE_UTILIZATION_DATA,
        TASK_MANAGE_UTILIZATION,
        TASK_ASSIGN_IPV4_ADDRESS,
        TASK_INTERCONNECT_IPV4_CONSISTENCY_CHECK,
        TASK_BACKGROUND_REFRESH,
        TASK_CHECK_DEVICE_IDENTITY_AND_LOCATION,
        TASK_RESET_SYNERGY_FRAME_LINK_MODULE,
        TASK_CLEAR_ALERTS,
        TASK_UPDATE,
        TASK_CONFIGURE_BOOT_ORDER_SETTINGS_FOR_SERVER,
        TASK_ASSIGN_PROFILE,
        TASK_BACKGROUNDREPOREFRESHTASK,
        TASK_CLEAR_PROFILE,
        TASK_CONFIGURE,
        TASK_CREATE,
        TASK_DELETE,
        TASK_LOGICAL_ENCLOSURE_FIRMWARE_UPDATE,
        TASK_REAPPLY_CONFIGURATION,
        TASK_RELEASE,
        TASK_REMOVE_PROFILE,
        TASK_UPDATE_ENCLOSURE_FIRMWARE,
        TASK_VALIDATE,
        TASK_RESET)

OV_REST_ENUM( sensorStatus,
       SENSOR_STATUS_NO_OP,
       SENSOR_STATUS_UNKNOWN,
       SENSOR_STATUS_OK,
       SENSOR_STATUS_WARM,
       SENSOR_STATUS_CAUTION,
       SENSOR_STATUS_CRITICAL)

struct certificateResponse
{
	json_object *root_jobj;
	json_object *certificate;
};
struct certificates
{
	const char *SSLCert;
	const char *SSLKey;
	const char *ca;
};
struct idleTimeout
{
	int timeout;
}; 
struct applianceNodeInfoResponse
{
	json_object *root_jobj;
        json_object *applianceVersion;
        json_object *applianceStatus;
};
struct applianceVersion
{
	char serialNumber[MAX_256_CHARS];
	char name[MAX_256_CHARS];
	int build;
	char *date;
	char revision[MAX_256_CHARS];
	char softwareVersion[MAX_256_CHARS];
	char platformType[MAX_256_CHARS];
	int major;
	int minor;
	char uri[MAX_URI_LENGTH];
	char manufacturer[MAX_MANUFACTURER_LENGTH];
};

struct applianceStatus
{
	SaHpiBoolT networkConfigured;
	char cpuSpeedUnits[MAX_256_CHARS];
	int cpu;
	int cpuSpeed;
	char lanUnits[MAX_256_CHARS];
	int lan;
	char memoryUnits[MAX_256_CHARS];
	int memory;
	
};
struct applianceNodeInfo
{
	struct applianceVersion version;
	struct applianceStatus status;
};
struct applianceHaNodeInfoResponse
{
	json_object *root_jobj;
	json_object *haNode;
};
struct applianceHaNodeInfoArrayResponse
{
	json_object *root_jobj;
	json_object *haNodeArray;
};
struct applianceHaNodeInfo
{
	char version[MAX_256_CHARS];
	char name[MAX_256_CHARS];
	char role[MAX_256_CHARS]; /* This can be changed to enum */
	enum healthStatus applianceStatus;
	char modelNumber[MAX_256_CHARS];
	char applianceId[MAX_256_CHARS]; /* AKA uuid of the appliance*/
	char uri[MAX_URI_LENGTH];
	char enclosure_uri[MAX_URI_LENGTH];
	char serialNumber[MAX_256_CHARS];
	byte bayNumber;
	enum resourceCategory type;
};
struct applianceInfo
{
	char partNumber[MAX_256_CHARS];
	char sparePartNumber[MAX_256_CHARS];
	int bayNumber;
	enum powerState powerState;
	enum presence presence;
	char model[MAX_256_CHARS];
	SaHpiBoolT poweredOn;
	char serialNumber[MAX_256_CHARS];
	enum healthStatus status;
};

struct datacenterInfoArrayResponse
{
	json_object *root_jobj;
        json_object *datacenter_array;
};
struct datacenterInfo
{
        char name[MAX_256_CHARS];
        char uuid[MAX_UUID_LENGTH];
        char uri[MAX_URI_LENGTH];
        char hwVersion[MAX_256_CHARS];
        float coolingMultiplier;
	int defaultPowerLineVoltage;
        char assetTag[MAX_256_CHARS];
        char manufacturer[MAX_MANUFACTURER_LENGTH];
        int mmWidth;
        int mmDepth;
	enum healthStatus status;
};

struct enclosureStatusResponse
{
	json_object *root_jobj;
	json_object *enclosure;
	json_object *devicebay_array;
	json_object *interconnectbay_array;
};
struct enclosureInfoArrayResponse
{
	json_object *root_jobj;
	json_object *enclosure_array;
};
struct enclosureDeviceBays
{
	int bayNumber;
	char interconnectUri[MAX_URI_LENGTH];
	enum presence presence;
	enum changeState changeState;
	
};
struct enclosureInfo
{
        char rackName[MAX_256_CHARS];
        char enclosureName[MAX_256_CHARS];
        char name[MAX_256_CHARS];
	byte composerBays;
        byte bladeBays;
        char model[MAX_256_CHARS];
        char partNumber[MAX_256_CHARS];
        char serialNumber[MAX_256_CHARS];
        int bayNumber;
        enum presence presence;
        char uuid[MAX_UUID_LENGTH];
	char uri[MAX_URI_LENGTH];
        char hwVersion[MAX_256_CHARS];
	byte fanBayCount;
	/* FIXME Could we change this json_object to array? */
	byte powerSupplyBayCount;
        byte thermalSensors;
        byte interconnectTrayBays;
        char manufacturer[MAX_MANUFACTURER_LENGTH];
        int powerType;
        enum healthStatus enclosureStatus;
	char uidState[MAX_256_CHARS];
};
struct serverhardwareInfoArrayResponse
{	
	json_object *root_jobj;
	json_object *server_array;
};

struct getserverhardwareThermalInfo
{
        int bayNumber;
};
struct serverhardwareThermalInfoResponse
{	
	json_object *root_jobj;
        json_object *serverhardwareThermal_array; 
        json_object *serverhardwareFans_array;
};
struct serverhardwarePowerStatusInfoResponse
{
        byte PowerConsumedWatts;
};

struct serverhardwareThermalInfo
{
        byte sensorNumber;
        byte sensorType;
        byte entityId;
        byte entityInstance;
        byte criticalThreshold;
        byte cautionThreshold;
        byte temperatureC;
        byte oem;
        char description[MAX_256_CHARS];
	byte CurrentReading;
	byte LowerThresholdCritical;
	byte LowerThresholdNonCritical;
	char Name[MAX_256_CHARS]; //FIXME: May be it is a enum
	byte Number;
	char PhysicalContext[MAX_256_CHARS];
	char Units[MAX_256_CHARS]; //FIXME: May be it is also enum
	char Health_status[MAX_256_CHARS]; //FIXME: This one too
	char Health_state[MAX_256_CHARS]; //FIXME: This One too
};

struct serverhardwareFanInfo
{
        byte CurrentReading;
        const char *Name;
        const char *Health_status;
        const char *Health_state;
        byte Units;
};

struct serverhardwareSystemsInfoResponse
{
        const char *Memory_Status;
        const char *Processor_Status;
        const char *System_Status;
        const char *Battery_Status;
};

struct serverhardwareStorageInfoResponse
{
        const char *SmartStorage_Status;
        const char *Name;
};

struct serverhardwareNetworkAdaptersInfoResponse
{
        const char *NetworkAdapters_Status;
};

struct serverhardwareEthernetInterfacesInfoResponse
{
        const char *EthernetInterfaces_Status;
};

struct serverhardwareInfo
{
	int bayNumber;
	enum presence presence;
        char partNumber[MAX_256_CHARS];
        char serialNumber[MAX_256_CHARS];
        char model[MAX_256_CHARS];
	char name[MAX_256_CHARS];
	char manufacturer[MAX_MANUFACTURER_LENGTH];
	char fwVersion[MAX_256_CHARS];
	char uri[MAX_URI_LENGTH];
        char uuid[MAX_UUID_LENGTH];
	char locationUri[MAX_URI_LENGTH];
        enum power powered;
        enum powerState powerState;
        enum healthStatus serverStatus;
	char uidState[MAX_256_CHARS];
	enum resourceCategory type;
};

struct driveEnclosureInfoArrayResponse
{
	json_object *root_jobj;
        json_object *drive_enc_array;
};

struct driveEnclosureInfo
{
        int bayNumber;
        enum presence presence;
        char partNumber[MAX_256_CHARS];
        char serialNumber[MAX_256_CHARS];
        char enc_serialNumber[MAX_256_CHARS];
        char model[MAX_256_CHARS];
        char name[MAX_256_CHARS];
        char manufacturer[MAX_MANUFACTURER_LENGTH];
	char fwVersion[MAX_256_CHARS];
	char uri[MAX_URI_LENGTH];
        char uuid[MAX_UUID_LENGTH];
	char locationUri[MAX_URI_LENGTH];
        enum power powered;
        enum powerState powerState;
	enum healthStatus drvEncStatus;
        char uidState[MAX_256_CHARS];         
	enum resourceCategory type;
};


struct interconnectInfoArrayResponse
{
	json_object *root_jobj;
	json_object *interconnect_array;
	char next_page[MAX_256_CHARS];
};

/* Below structure is required to parse the interconnect to find the
 * bay number when the response comes as below,
 * Ex:-
             "interconnectLocation":
            {
                "locationEntries":
                [
                    {
                        "value": "4",
                        "type": "Bay"
                    },
                    {
                        "value": "/rest/enclosures/09USE7103J0K",
                        "type": "Enclosure"
                    }
                ]
            },
 */
struct locationEntries 
{
	char type[MAX_256_CHARS];
	int value;
};
struct interconnectInfo
{
        int bayNumber;
        char partNumber[MAX_256_CHARS];
        char serialNumber[MAX_256_CHARS];
        char model[MAX_256_CHARS];
	char manufacturer[MAX_MANUFACTURER_LENGTH];
	char uri[MAX_URI_LENGTH];
	char locationUri[MAX_URI_LENGTH];
	struct locationEntries location;
	enum powerState powerState;	
	char uidState[MAX_256_CHARS];
        enum healthStatus interconnectStatus;
	enum resourceCategory type;
};

struct powersupplyArrayResponse
{
	json_object *root_jobj;
	json_object *powersupplay_array;
	int powersupply_count;
};

struct powersupplyInfo
{
        int bayNumber;
        char partNumber[MAX_256_CHARS];
        char serialNumber[MAX_256_CHARS];
        char model[MAX_256_CHARS];
        enum healthStatus status;
        enum presence presence;
        int outputCapacityWatts;
	enum resourceCategory type;
};
struct fanArrayResponse
{
	json_object *root_jobj;
	json_object *fan_array;
};

struct fanInfo
{
	int bayNumber;
	SaHpiBoolT deviceRequired;
	char partNumber[MAX_256_CHARS];
	char sparePartNumber[MAX_256_CHARS];
	char serialNumber[MAX_256_CHARS];
	char model[MAX_256_CHARS];
	char fanBayType[MAX_256_CHARS];
	enum healthStatus status;
	enum presence presence;
	enum resourceCategory type;
	const char uri[MAX_URI_LENGTH];
	struct fanInfo *next;
};


struct eventArrayResponse
{
	json_object *root_jobj;
	json_object *event_array;
	char created[MAX_URI_LENGTH];
	const char *total;
};

/* ptr is ok, we will make a copy of this using copy_ov_rest_event */
struct eventInfo
{
	const char *severity;
	const char *description;
	const char *resourceUri;
	const char *resourceCategory;
	enum eventType alertTypeId;
	const char *healthCategory;
	const char *correctiveAction;
	const char *alertState;
	const char *resourceID;
	const char *created;
	const char *phyResourceType;
	char task_name[262];
	const char *category;
	enum name name;
	const char *alert_name;
	char *type;
	const char *taskState;
	int percentComplete;
};
#define MAX_EVT_SEVERITY_LEN 10
#define MAX_EVT_DESCRIPTION_LEN 1024
#define MAX_EVT_ALERTSTATE_LEN 10
#define MAX_EVT_RESOURCEID_LEN 2048
#define MAX_EVT_CREATED_LEN 32
#define MAX_TASK_STATE_LEN 20


#define OV_REST_ACCEPT \
	("Accept: application/json")
#define OV_REST_CHARSET \
	"charset: UTF-8"
#define OV_REST_CONTENT_TYPE \
	"Content-Type: application/json"
#define OV_REST_X_API_VERSION \
	"X-API-Version: 600"
#define OV_REST_AUTH \
	"Auth: %s"
#define OV_REST_SESSIONID \
	"Session-Id: %s"
#define OV_REST_X_AUTH_TOKEN \
	"X-auth-Token: %s"
#define OV_REST_LOGIN_URI \
	"https://%s/rest/login-sessions"
#define OV_REST_PATH VARPATH "/ov_rest"
#define CA_PATH OV_REST_PATH "/cert"
#define OV_CREATE_CERTIFICATE_URI \
        "https://%s/rest/certificates/client/rabbitmq"
#define OV_CERTIFICATE_REQUEST_POST \
        "{\"type\":\"RabbitMqClientCertV2\", \"commonName\":\"default\"}"
#define OV_GET_CERTIFICATES_URI \
	"https://%s/rest/certificates/client/rabbitmq/keypair/default"
#define OV_GET_CA_URI \
	"https://%s/rest/certificates/ca?filter=certType:INTERNAL"
#define OV_APPLIANCE_VERSION_URI \
	"https://%s/rest/appliance/nodeinfo/version"
#define OV_GET_IDLE_TIMEOUT_URI \
	"https://%s/rest/sessions/idle-timeout"
#define OV_APPLIANCE_STATUS_URI \
	"https://%s/rest/appliance/nodeinfo/status"
#define OV_DATACENTER_URI \
	"https://%s/rest/datacenters"
#define OV_ENCLOSURE_URI \
	"https://%s/rest/enclosures"
#define OV_SERVER_HARDWARE_URI \
	"https://%s/rest/server-hardware?start=0&count=504"
#define OV_SERVER_HARDWARE_SSO_URI \
	"https://%s%s/remoteConsoleUrl"
#define OV_SERVER_HARDWARE_THERMAL_URI \
	"https://%s/rest/v1/Chassis/1/Thermal"
#define OV_SERVER_HARDWARE_POWER_STATUS_URI \
        "https://%s/rest/v1/Chassis/1/Power"
#define OV_SERVER_HARDWARE_SYSTEMS_URI \
        "https://%s/rest/v1/Systems/1"
#define OV_SERVER_HARDWARE_SMART_STORAGE_URI \
        "https://%s/rest/v1/Systems/1/smartstorage"
#define OV_SERVER_HARDWARE_NETWORK_ADAPTERS_URI \
        "https://%s/rest/v1/Systems/1/NetworkAdapters/1"
#define OV_SERVER_HARDWARE_ETHERNET_INTERFACES_URI \
        "https://%s/rest/v1/Managers/1/EthernetInterfaces/1"
#define OV_APPLIANCE_HA_NODE_ID_URI \
	"https://%s/rest/appliance/ha-nodes/%s"
#define OV_DRIVE_ENCLOSURE_URI \
        "https://%s/rest/drive-enclosures?start=0&count=504"
#define OV_INTERCONNECT_URI \
	"https://%s/rest/interconnects?start=0&count=256"
#define OV_SAS_INTERCONNECT_URI \
	"https://%s/rest/sas-interconnects?start=0&count=256"
#define OV_ALERTS \
	"https://%s/rest/alerts"
#define OV_ACTIVE_ALERTS \
  "https://%s/rest/alerts?start=0&count=%s&filter=\"alertState=\'Active\'\""
#define OV_LOCKED_ALERTS \
  "https://%s/rest/alerts?start=0&count=%s&filter=\"alertState=\'Locked\'\""
#define OV_ALERTS_TIME_FILTER \
	"https://%s/rest/alerts?start=0&count=-1&filter=\"created+GT+'%s'\""
#define OV_REST_LOGIN_POST \
	"{\"userName\":\"%s\", \"password\":\"%s\", \"loginMsgAck\":\"%s\"}"
int ov_rest_trim_alert_string(const char* alert, struct eventInfo *response);
void ov_rest_json_parse_certificate( json_object *jobj, 
			struct certificates *response);
void ov_rest_json_parse_ca( json_object *jobj, struct certificates *response);
void ov_rest_json_parse_appliance_Ha_node( json_object *jobj,
                                struct applianceHaNodeInfo *response);
void ov_rest_json_parse_appliance_version( json_object *jarray, 
			struct applianceVersion *response);
void ov_rest_json_parse_appliance_status( json_object *jobj, 
			struct applianceStatus* response);
void ov_rest_json_parse_applianceInfo(json_object *jobj,
			struct applianceInfo *response);
void ov_rest_json_parse_server(json_object *jobj, 
			struct serverhardwareInfo *response);
void ov_rest_json_parse_drive_enclosure(json_object *jobj, 
			struct driveEnclosureInfo *response);
void ov_rest_json_parse_interconnect( json_object *jobj, 
			struct interconnectInfo *response);
void ov_rest_json_parse_enclosure( json_object *jobj, 
			struct enclosureInfo* response);
void ov_rest_json_parse_powersupply( json_object *jvalue, 
			struct powersupplyInfo* response);
void ov_rest_json_parse_fan( json_object *jvalue, struct fanInfo* response);
void ov_rest_json_parse_server_thermal_sensors(json_object *jvalue,
                        struct serverhardwareThermalInfo *response);
void ov_rest_json_parse_server_fan_sensors(json_object *jvalue,
                        struct serverhardwareFanInfo *response);
void ov_rest_json_parse_enc_device_bays( json_object *jarray, 
			struct enclosureDeviceBays* response);
void ov_rest_json_parse_enc_manager_bays( json_object *jarray,
                        struct enclosureInfo* response);
void ov_rest_json_parse_datacenter( json_object *jarray, int i, 
			struct datacenterInfo* response);
void ov_rest_json_parse_events( json_object *jobj, struct eventInfo* response);
void ov_rest_json_parse_alerts_utility( json_object *jobj, 
					struct eventInfo* response);
void ov_rest_json_parse_alerts( json_object *jobj, struct eventInfo* response);
void ov_rest_json_parse_tasks( json_object *jobj, struct eventInfo* response);
#endif /*_PARSER_CALLS_H*/

