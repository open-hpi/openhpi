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

#ifndef __BASELIB_H__
#define __BASELIB_H__

#include <string>

#include <gmodule.h>

#include <SaHpi.h>


/**************************************************************
 * Pointers to Base Lib API functions (SAI-HPI-B.03.02)
 *************************************************************/

typedef
SaHpiVersionT SAHPI_API (*saHpiVersionGetPtr)( void );

typedef
SaErrorT SAHPI_API (*saHpiInitializePtr) (
    SAHPI_IN    SaHpiVersionT        RequestedVersion,
    SAHPI_IN    SaHpiUint32T         NumOptions,
    SAHPI_INOUT SaHpiInitOptionT     *Options,
    SAHPI_OUTNN SaHpiUint32T         *FailedOption,
    SAHPI_OUTNN SaErrorT             *OptionError
);

typedef
SaErrorT SAHPI_API (*saHpiFinalizePtr)( void );

typedef
SaErrorT SAHPI_API (*saHpiSessionOpenPtr)(
    SAHPI_IN  SaHpiDomainIdT      DomainId,
    SAHPI_OUT SaHpiSessionIdT     *SessionId,
    SAHPI_IN  void                *SecurityParams
);

typedef
SaErrorT SAHPI_API (*saHpiSessionClosePtr)(
    SAHPI_IN SaHpiSessionIdT SessionId
);

typedef
SaErrorT SAHPI_API (*saHpiDiscoverPtr)(
    SAHPI_IN SaHpiSessionIdT SessionId
);

typedef
SaErrorT SAHPI_API (*saHpiDomainInfoGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_OUT SaHpiDomainInfoT     *DomainInfo
);

typedef
SaErrorT SAHPI_API (*saHpiDrtEntryGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiEntryIdT       EntryId,
    SAHPI_OUT SaHpiEntryIdT       *NextEntryId,
    SAHPI_OUT SaHpiDrtEntryT      *DrtEntry
);

typedef
SaErrorT SAHPI_API (*saHpiDomainTagSetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiTextBufferT     *DomainTag
);

typedef
SaErrorT SAHPI_API (*saHpiRptEntryGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiEntryIdT       EntryId,
    SAHPI_OUT SaHpiEntryIdT       *NextEntryId,
    SAHPI_OUT SaHpiRptEntryT      *RptEntry
);

typedef
SaErrorT SAHPI_API (*saHpiRptEntryGetByResourceIdPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_OUT SaHpiRptEntryT       *RptEntry
);

typedef
SaErrorT SAHPI_API (*saHpiResourceSeveritySetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_IN  SaHpiSeverityT      Severity
);

typedef
SaErrorT SAHPI_API (*saHpiResourceTagSetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_IN  SaHpiTextBufferT    *ResourceTag
);

typedef
SaErrorT SAHPI_API (*saHpiMyEntityPathGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_OUT SaHpiEntityPathT     *EntityPath
);

typedef
SaErrorT SAHPI_API (*saHpiResourceIdGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_OUT SaHpiResourceIdT     *ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiGetIdByEntityPathPtr)(
        SAHPI_IN      SaHpiSessionIdT    SessionId,
        SAHPI_IN    SaHpiEntityPathT     EntityPath,
        SAHPI_IN    SaHpiRdrTypeT        InstrumentType,
        SAHPI_INOUT    SaHpiUint32T      *InstanceId,
        SAHPI_OUT    SaHpiResourceIdT    *ResourceId,
        SAHPI_OUT    SaHpiInstrumentIdT  *InstrumentId,
        SAHPI_OUT    SaHpiUint32T        *RptUpdateCount
);

typedef
SaErrorT SAHPI_API (*saHpiGetChildEntityPathPtr)(
    SAHPI_IN      SaHpiSessionIdT   SessionId,
    SAHPI_IN    SaHpiEntityPathT    ParentEntityPath,
    SAHPI_INOUT    SaHpiUint32T     *InstanceId,
    SAHPI_OUT    SaHpiEntityPathT   *ChildEntityPath,
    SAHPI_OUT    SaHpiUint32T       *RptUpdateCount
);

typedef
SaErrorT SAHPI_API (*saHpiResourceFailedRemovePtr)(
    SAHPI_IN    SaHpiSessionIdT        SessionId,
    SAHPI_IN    SaHpiResourceIdT       ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogInfoGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_OUT SaHpiEventLogInfoT  *Info
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogCapabilitiesGetPtr)(
    SAHPI_IN  SaHpiSessionIdT             SessionId,
    SAHPI_IN  SaHpiResourceIdT            ResourceId,
    SAHPI_OUT SaHpiEventLogCapabilitiesT  *EventLogCapabilities
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogEntryGetPtr)(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiEventLogEntryIdT    EntryId,
    SAHPI_OUT   SaHpiEventLogEntryIdT    *PrevEntryId,
    SAHPI_OUT   SaHpiEventLogEntryIdT    *NextEntryId,
    SAHPI_OUT   SaHpiEventLogEntryT      *EventLogEntry,
    SAHPI_INOUT SaHpiRdrT                *Rdr,
    SAHPI_INOUT SaHpiRptEntryT           *RptEntry
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogEntryAddPtr)(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId,
    SAHPI_IN SaHpiEventT         *EvtEntry
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogClearPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogTimeGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_OUT SaHpiTimeT           *Time
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogTimeSetPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiTimeT           Time
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogStateGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_OUT SaHpiBoolT          *EnableState
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogStateSetPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiBoolT           EnableState
);

typedef
SaErrorT SAHPI_API (*saHpiEventLogOverflowResetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiSubscribePtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId
);

typedef
SaErrorT SAHPI_API (*saHpiUnsubscribePtr)(
    SAHPI_IN SaHpiSessionIdT  SessionId
);

typedef
SaErrorT SAHPI_API (*saHpiEventGetPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiTimeoutT              Timeout,
    SAHPI_OUT SaHpiEventT               *Event,
    SAHPI_INOUT SaHpiRdrT               *Rdr,
    SAHPI_INOUT SaHpiRptEntryT          *RptEntry,
    SAHPI_INOUT SaHpiEvtQueueStatusT    *EventQueueStatus
);

typedef
SaErrorT SAHPI_API (*saHpiEventAddPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiEventT          *EvtEntry
);

typedef
SaErrorT SAHPI_API (*saHpiAlarmGetNextPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiSeverityT             Severity,
    SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
    SAHPI_INOUT SaHpiAlarmT             *Alarm
);

typedef
SaErrorT SAHPI_API (*saHpiAlarmGetPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_OUT SaHpiAlarmT               *Alarm
);

typedef
SaErrorT SAHPI_API (*saHpiAlarmAcknowledgePtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity
);

typedef
SaErrorT SAHPI_API (*saHpiAlarmAddPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_INOUT SaHpiAlarmT             *Alarm
);

typedef
SaErrorT SAHPI_API (*saHpiAlarmDeletePtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiAlarmIdT              AlarmId,
    SAHPI_IN SaHpiSeverityT             Severity
);

typedef
SaErrorT SAHPI_API (*saHpiRdrGetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_IN  SaHpiEntryIdT         EntryId,
    SAHPI_OUT SaHpiEntryIdT         *NextEntryId,
    SAHPI_OUT SaHpiRdrT             *Rdr
);

typedef
SaErrorT SAHPI_API (*saHpiRdrGetByInstrumentIdPtr)(
    SAHPI_IN  SaHpiSessionIdT        SessionId,
    SAHPI_IN  SaHpiResourceIdT       ResourceId,
    SAHPI_IN  SaHpiRdrTypeT          RdrType,
    SAHPI_IN  SaHpiInstrumentIdT     InstrumentId,
    SAHPI_OUT SaHpiRdrT              *Rdr
);

typedef
SaErrorT SAHPI_API (*saHpiRdrUpdateCountGetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_OUT SaHpiUint32T          *UpdateCount
);

typedef
SaErrorT SAHPI_API (*saHpiSensorReadingGetPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiSensorNumT       SensorNum,
    SAHPI_INOUT SaHpiSensorReadingT   *Reading,
    SAHPI_INOUT SaHpiEventStateT      *EventState
);

typedef
SaErrorT SAHPI_API (*saHpiSensorThresholdsGetPtr)(
    SAHPI_IN  SaHpiSessionIdT            SessionId,
    SAHPI_IN  SaHpiResourceIdT           ResourceId,
    SAHPI_IN  SaHpiSensorNumT            SensorNum,
    SAHPI_OUT SaHpiSensorThresholdsT     *SensorThresholds
);

typedef
SaErrorT SAHPI_API (*saHpiSensorThresholdsSetPtr)(
    SAHPI_IN  SaHpiSessionIdT          SessionId,
    SAHPI_IN  SaHpiResourceIdT         ResourceId,
    SAHPI_IN  SaHpiSensorNumT          SensorNum,
    SAHPI_IN  SaHpiSensorThresholdsT   *SensorThresholds
);

typedef
SaErrorT SAHPI_API (*saHpiSensorTypeGetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_IN  SaHpiSensorNumT       SensorNum,
    SAHPI_OUT SaHpiSensorTypeT      *Type,
    SAHPI_OUT SaHpiEventCategoryT   *Category
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEnableGetPtr)(
    SAHPI_IN  SaHpiSessionIdT         SessionId,
    SAHPI_IN  SaHpiResourceIdT        ResourceId,
    SAHPI_IN  SaHpiSensorNumT         SensorNum,
    SAHPI_OUT SaHpiBoolT              *SensorEnabled
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEnableSetPtr)(
    SAHPI_IN  SaHpiSessionIdT         SessionId,
    SAHPI_IN  SaHpiResourceIdT        ResourceId,
    SAHPI_IN  SaHpiSensorNumT         SensorNum,
    SAHPI_IN  SaHpiBoolT              SensorEnabled
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEventEnableGetPtr)(
    SAHPI_IN  SaHpiSessionIdT         SessionId,
    SAHPI_IN  SaHpiResourceIdT        ResourceId,
    SAHPI_IN  SaHpiSensorNumT         SensorNum,
    SAHPI_OUT SaHpiBoolT              *SensorEventsEnabled
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEventEnableSetPtr)(
    SAHPI_IN  SaHpiSessionIdT         SessionId,
    SAHPI_IN  SaHpiResourceIdT        ResourceId,
    SAHPI_IN  SaHpiSensorNumT         SensorNum,
    SAHPI_IN  SaHpiBoolT              SensorEventsEnabled
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEventMasksGetPtr)(
    SAHPI_IN  SaHpiSessionIdT         SessionId,
    SAHPI_IN  SaHpiResourceIdT        ResourceId,
    SAHPI_IN  SaHpiSensorNumT         SensorNum,
    SAHPI_INOUT SaHpiEventStateT      *AssertEventMask,
    SAHPI_INOUT SaHpiEventStateT      *DeassertEventMask
);

typedef
SaErrorT SAHPI_API (*saHpiSensorEventMasksSetPtr)(
    SAHPI_IN  SaHpiSessionIdT                 SessionId,
    SAHPI_IN  SaHpiResourceIdT                ResourceId,
    SAHPI_IN  SaHpiSensorNumT                 SensorNum,
    SAHPI_IN  SaHpiSensorEventMaskActionT     Action,
    SAHPI_IN  SaHpiEventStateT                AssertEventMask,
    SAHPI_IN  SaHpiEventStateT                DeassertEventMask
);

typedef
SaErrorT SAHPI_API (*saHpiControlTypeGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_IN  SaHpiResourceIdT    ResourceId,
    SAHPI_IN  SaHpiCtrlNumT       CtrlNum,
    SAHPI_OUT SaHpiCtrlTypeT      *Type
);

typedef
SaErrorT SAHPI_API (*saHpiControlGetPtr)(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiCtrlNumT        CtrlNum,
    SAHPI_OUT   SaHpiCtrlModeT       *CtrlMode,
    SAHPI_INOUT SaHpiCtrlStateT      *CtrlState
);

typedef
SaErrorT SAHPI_API (*saHpiControlSetPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiCtrlNumT        CtrlNum,
    SAHPI_IN SaHpiCtrlModeT       CtrlMode,
    SAHPI_IN SaHpiCtrlStateT      *CtrlState
);

typedef
SaErrorT SAHPI_API (*saHpiIdrInfoGetPtr)(
    SAHPI_IN SaHpiSessionIdT         SessionId,
    SAHPI_IN SaHpiResourceIdT        ResourceId,
    SAHPI_IN SaHpiIdrIdT             IdrId,
    SAHPI_OUT SaHpiIdrInfoT          *IdrInfo
);

typedef
SaErrorT SAHPI_API (*saHpiIdrAreaHeaderGetPtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
    SAHPI_IN SaHpiEntryIdT            AreaId,
    SAHPI_OUT SaHpiEntryIdT           *NextAreaId,
    SAHPI_OUT SaHpiIdrAreaHeaderT     *Header
);

typedef
SaErrorT SAHPI_API (*saHpiIdrAreaAddPtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_IN SaHpiIdrAreaTypeT        AreaType,
    SAHPI_OUT SaHpiEntryIdT           *AreaId
);

typedef
SaErrorT SAHPI_API (*saHpiIdrAreaAddByIdPtr)(
    SAHPI_IN SaHpiSessionIdT    SessionId,
    SAHPI_IN SaHpiResourceIdT   ResourceId,
    SAHPI_IN SaHpiIdrIdT        IdrId,
    SAHPI_IN SaHpiIdrAreaTypeT  AreaType,
    SAHPI_IN SaHpiEntryIdT      AreaId
);

typedef
SaErrorT SAHPI_API (*saHpiIdrAreaDeletePtr)(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_IN SaHpiIdrIdT            IdrId,
    SAHPI_IN SaHpiEntryIdT          AreaId
);

typedef
SaErrorT SAHPI_API (*saHpiIdrFieldGetPtr)(
    SAHPI_IN SaHpiSessionIdT         SessionId,
    SAHPI_IN SaHpiResourceIdT        ResourceId,
    SAHPI_IN SaHpiIdrIdT             IdrId,
    SAHPI_IN SaHpiEntryIdT           AreaId,
    SAHPI_IN SaHpiIdrFieldTypeT      FieldType,
    SAHPI_IN SaHpiEntryIdT           FieldId,
    SAHPI_OUT SaHpiEntryIdT          *NextFieldId,
    SAHPI_OUT SaHpiIdrFieldT         *Field
);

typedef
SaErrorT SAHPI_API (*saHpiIdrFieldAddPtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_INOUT SaHpiIdrFieldT        *Field
);

typedef
SaErrorT SAHPI_API (*saHpiIdrFieldAddByIdPtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_INOUT SaHpiIdrFieldT        *Field
);

typedef
SaErrorT SAHPI_API (*saHpiIdrFieldSetPtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_IN SaHpiIdrFieldT           *Field
);

typedef
SaErrorT SAHPI_API (*saHpiIdrFieldDeletePtr)(
    SAHPI_IN SaHpiSessionIdT          SessionId,
    SAHPI_IN SaHpiResourceIdT         ResourceId,
    SAHPI_IN SaHpiIdrIdT              IdrId,
    SAHPI_IN SaHpiEntryIdT            AreaId,
    SAHPI_IN SaHpiEntryIdT            FieldId
);

typedef
SaErrorT SAHPI_API (*saHpiWatchdogTimerGetPtr)(
    SAHPI_IN  SaHpiSessionIdT        SessionId,
    SAHPI_IN  SaHpiResourceIdT       ResourceId,
    SAHPI_IN  SaHpiWatchdogNumT      WatchdogNum,
    SAHPI_OUT SaHpiWatchdogT         *Watchdog
);

typedef
SaErrorT SAHPI_API (*saHpiWatchdogTimerSetPtr)(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiWatchdogNumT     WatchdogNum,
    SAHPI_IN SaHpiWatchdogT        *Watchdog
);

typedef
SaErrorT SAHPI_API (*saHpiWatchdogTimerResetPtr)(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiWatchdogNumT     WatchdogNum
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorGetNextPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_IN SaHpiSeverityT             Severity,
    SAHPI_IN SaHpiBoolT                 UnacknowledgedOnly,
    SAHPI_INOUT SaHpiAnnouncementT      *Announcement
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorGetPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_IN SaHpiEntryIdT              EntryId,
    SAHPI_OUT SaHpiAnnouncementT        *Announcement
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorAcknowledgePtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_IN SaHpiEntryIdT              EntryId,
    SAHPI_IN SaHpiSeverityT             Severity
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorAddPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_INOUT SaHpiAnnouncementT      *Announcement
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorDeletePtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_IN SaHpiEntryIdT              EntryId,
    SAHPI_IN SaHpiSeverityT             Severity
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorModeGetPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_OUT SaHpiAnnunciatorModeT     *Mode
);

typedef
SaErrorT SAHPI_API (*saHpiAnnunciatorModeSetPtr)(
    SAHPI_IN SaHpiSessionIdT            SessionId,
    SAHPI_IN SaHpiResourceIdT           ResourceId,
    SAHPI_IN SaHpiAnnunciatorNumT       AnnunciatorNum,
    SAHPI_IN SaHpiAnnunciatorModeT      Mode
);

typedef
SaErrorT SAHPI_API (*saHpiDimiInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiDimiNumT       DimiNum,
    SAHPI_OUT   SaHpiDimiInfoT      *DimiInfo
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiTestT       *DimiTest
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestReadinessGetPtr)(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum,
    SAHPI_OUT   SaHpiDimiReadyT      *DimiReady
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestStartPtr)(
    SAHPI_IN    SaHpiSessionIdT                SessionId,
    SAHPI_IN    SaHpiResourceIdT               ResourceId,
    SAHPI_IN    SaHpiDimiNumT                  DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT              TestNum,
    SAHPI_IN    SaHpiUint8T                    NumberOfParams,
    SAHPI_IN    SaHpiDimiTestVariableParamsT   *ParamsList
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestCancelPtr)(
    SAHPI_IN    SaHpiSessionIdT      SessionId,
    SAHPI_IN    SaHpiResourceIdT     ResourceId,
    SAHPI_IN    SaHpiDimiNumT        DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT    TestNum
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestStatusGetPtr)(
    SAHPI_IN    SaHpiSessionIdT                 SessionId,
    SAHPI_IN    SaHpiResourceIdT                ResourceId,
    SAHPI_IN    SaHpiDimiNumT                   DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT               TestNum,
    SAHPI_OUT   SaHpiDimiTestPercentCompletedT  *PercentCompleted,
    SAHPI_OUT   SaHpiDimiTestRunStatusT         *RunStatus
);

typedef
SaErrorT SAHPI_API (*saHpiDimiTestResultsGetPtr)(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiDimiNumT            DimiNum,
    SAHPI_IN    SaHpiDimiTestNumT        TestNum,
    SAHPI_OUT   SaHpiDimiTestResultsT    *TestResults
);

typedef
SaErrorT SAHPI_API (*saHpiFumiSpecInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum,
    SAHPI_OUT   SaHpiFumiSpecInfoT  *SpecInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiServiceImpactGetPtr)(
    SAHPI_IN   SaHpiSessionIdT              SessionId,
    SAHPI_IN   SaHpiResourceIdT             ResourceId,
    SAHPI_IN   SaHpiFumiNumT                FumiNum,
    SAHPI_OUT  SaHpiFumiServiceImpactDataT  *ServiceImpact
);

typedef
SaErrorT SAHPI_API (*saHpiFumiSourceSetPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum,
    SAHPI_IN    SaHpiBankNumT       BankNum,
    SAHPI_IN    SaHpiTextBufferT    *SourceUri
);

typedef
SaErrorT SAHPI_API (*saHpiFumiSourceInfoValidateStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiSourceInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiSourceInfoT  *SourceInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiSourceComponentInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiFumiNumT            FumiNum,
    SAHPI_IN    SaHpiBankNumT            BankNum,
    SAHPI_IN    SaHpiEntryIdT            ComponentEntryId,
    SAHPI_OUT   SaHpiEntryIdT            *NextComponentEntryId,
    SAHPI_OUT   SaHpiFumiComponentInfoT  *ComponentInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiTargetInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_OUT   SaHpiFumiBankInfoT    *BankInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiTargetComponentInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiFumiNumT            FumiNum,
    SAHPI_IN    SaHpiBankNumT            BankNum,
    SAHPI_IN    SaHpiEntryIdT            ComponentEntryId,
    SAHPI_OUT   SaHpiEntryIdT            *NextComponentEntryId,
    SAHPI_OUT   SaHpiFumiComponentInfoT  *ComponentInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiLogicalTargetInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT             SessionId,
    SAHPI_IN    SaHpiResourceIdT            ResourceId,
    SAHPI_IN    SaHpiFumiNumT               FumiNum,
    SAHPI_OUT   SaHpiFumiLogicalBankInfoT   *BankInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiLogicalTargetComponentInfoGetPtr)(
    SAHPI_IN    SaHpiSessionIdT                 SessionId,
    SAHPI_IN    SaHpiResourceIdT                ResourceId,
    SAHPI_IN    SaHpiFumiNumT                   FumiNum,
    SAHPI_IN    SaHpiEntryIdT                   ComponentEntryId,
    SAHPI_OUT   SaHpiEntryIdT                   *NextComponentEntryId,
    SAHPI_OUT   SaHpiFumiLogicalComponentInfoT  *ComponentInfo
);

typedef
SaErrorT SAHPI_API (*saHpiFumiBackupStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiBankBootOrderSetPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum,
    SAHPI_IN    SaHpiUint32T          Position
);

typedef
SaErrorT SAHPI_API (*saHpiFumiBankCopyStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         SourceBankNum,
    SAHPI_IN    SaHpiBankNumT         TargetBankNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiInstallStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiUpgradeStatusGetPtr)(
    SAHPI_IN    SaHpiSessionIdT          SessionId,
    SAHPI_IN    SaHpiResourceIdT         ResourceId,
    SAHPI_IN    SaHpiFumiNumT            FumiNum,
    SAHPI_IN    SaHpiBankNumT            BankNum,
    SAHPI_OUT   SaHpiFumiUpgradeStatusT  *UpgradeStatus
);

typedef
SaErrorT SAHPI_API (*saHpiFumiTargetVerifyStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiTargetVerifyMainStartPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiUpgradeCancelPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum,
    SAHPI_IN    SaHpiBankNumT         BankNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiAutoRollbackDisableGetPtr)(
    SAHPI_IN    SaHpiSessionIdT   SessionId,
    SAHPI_IN    SaHpiResourceIdT  ResourceId,
    SAHPI_IN    SaHpiFumiNumT     FumiNum,
    SAHPI_OUT   SaHpiBoolT        *Disable
);

typedef
SaErrorT SAHPI_API (*saHpiFumiAutoRollbackDisableSetPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum,
    SAHPI_IN    SaHpiBoolT          Disable
);

typedef
SaErrorT SAHPI_API (*saHpiFumiRollbackStartPtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiActivatePtr)(
    SAHPI_IN    SaHpiSessionIdT       SessionId,
    SAHPI_IN    SaHpiResourceIdT      ResourceId,
    SAHPI_IN    SaHpiFumiNumT         FumiNum
);

typedef
SaErrorT SAHPI_API (*saHpiFumiActivateStartPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum,
    SAHPI_IN    SaHpiBoolT          Logical
);

typedef
SaErrorT SAHPI_API (*saHpiFumiCleanupPtr)(
    SAHPI_IN    SaHpiSessionIdT     SessionId,
    SAHPI_IN    SaHpiResourceIdT    ResourceId,
    SAHPI_IN    SaHpiFumiNumT       FumiNum,
    SAHPI_IN    SaHpiBankNumT       BankNum
);

typedef
SaErrorT SAHPI_API (*saHpiHotSwapPolicyCancelPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiResourceActiveSetPtr)(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiResourceInactiveSetPtr)(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId
);

typedef
SaErrorT SAHPI_API (*saHpiAutoInsertTimeoutGetPtr)(
    SAHPI_IN  SaHpiSessionIdT     SessionId,
    SAHPI_OUT SaHpiTimeoutT       *Timeout
);

typedef
SaErrorT SAHPI_API (*saHpiAutoInsertTimeoutSetPtr)(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiTimeoutT          Timeout
);

typedef
SaErrorT SAHPI_API (*saHpiAutoExtractTimeoutGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_OUT SaHpiTimeoutT        *Timeout
);

typedef
SaErrorT SAHPI_API (*saHpiAutoExtractTimeoutSetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_IN  SaHpiTimeoutT        Timeout
);

typedef
SaErrorT SAHPI_API (*saHpiHotSwapStateGetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_OUT SaHpiHsStateT         *State
);

typedef
SaErrorT SAHPI_API (*saHpiHotSwapActionRequestPtr)(
    SAHPI_IN SaHpiSessionIdT     SessionId,
    SAHPI_IN SaHpiResourceIdT    ResourceId,
    SAHPI_IN SaHpiHsActionT      Action
);

typedef
SaErrorT SAHPI_API (*saHpiHotSwapIndicatorStateGetPtr)(
    SAHPI_IN  SaHpiSessionIdT            SessionId,
    SAHPI_IN  SaHpiResourceIdT           ResourceId,
    SAHPI_OUT SaHpiHsIndicatorStateT     *State
);

typedef
SaErrorT SAHPI_API (*saHpiHotSwapIndicatorStateSetPtr)(
    SAHPI_IN SaHpiSessionIdT           SessionId,
    SAHPI_IN SaHpiResourceIdT          ResourceId,
    SAHPI_IN SaHpiHsIndicatorStateT    State
);

typedef
SaErrorT SAHPI_API (*saHpiParmControlPtr)(
    SAHPI_IN SaHpiSessionIdT      SessionId,
    SAHPI_IN SaHpiResourceIdT     ResourceId,
    SAHPI_IN SaHpiParmActionT     Action
);

typedef
SaErrorT SAHPI_API (*saHpiResourceLoadIdGetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_OUT SaHpiLoadIdT         *LoadId
);

typedef
SaErrorT SAHPI_API (*saHpiResourceLoadIdSetPtr)(
    SAHPI_IN  SaHpiSessionIdT      SessionId,
    SAHPI_IN  SaHpiResourceIdT     ResourceId,
    SAHPI_IN  SaHpiLoadIdT         *LoadId
);

typedef
SaErrorT SAHPI_API (*saHpiResourceResetStateGetPtr)(
    SAHPI_IN SaHpiSessionIdT        SessionId,
    SAHPI_IN SaHpiResourceIdT       ResourceId,
    SAHPI_OUT SaHpiResetActionT     *ResetAction
);

typedef
SaErrorT SAHPI_API (*saHpiResourceResetStateSetPtr)(
    SAHPI_IN SaHpiSessionIdT       SessionId,
    SAHPI_IN SaHpiResourceIdT      ResourceId,
    SAHPI_IN SaHpiResetActionT     ResetAction
);

typedef
SaErrorT SAHPI_API (*saHpiResourcePowerStateGetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_OUT SaHpiPowerStateT      *State
);

typedef
SaErrorT SAHPI_API (*saHpiResourcePowerStateSetPtr)(
    SAHPI_IN  SaHpiSessionIdT       SessionId,
    SAHPI_IN  SaHpiResourceIdT      ResourceId,
    SAHPI_IN  SaHpiPowerStateT      State
);


/**************************************************************
 * Pointers to oHpi* functions
 *************************************************************/
typedef
SaErrorT SAHPI_API (*oHpiDomainAddPtr)(
    const SaHpiTextBufferT *host,
    SaHpiUint16T port,
    const SaHpiEntityPathT *entity_root,
    SaHpiDomainIdT *domain_id
);


namespace Slave {


/**************************************************************
 * struct BaseLibAbi
 *************************************************************/
struct BaseLibAbi
{
    saHpiVersionGetPtr                        saHpiVersionGet;
    saHpiInitializePtr                        saHpiInitialize;
    saHpiFinalizePtr                          saHpiFinalize;
    saHpiSessionOpenPtr                       saHpiSessionOpen;
    saHpiSessionClosePtr                      saHpiSessionClose;
    saHpiDiscoverPtr                          saHpiDiscover;
    saHpiDomainInfoGetPtr                     saHpiDomainInfoGet;
    saHpiDrtEntryGetPtr                       saHpiDrtEntryGet;
    saHpiDomainTagSetPtr                      saHpiDomainTagSet;
    saHpiRptEntryGetPtr                       saHpiRptEntryGet;
    saHpiRptEntryGetByResourceIdPtr           saHpiRptEntryGetByResourceId;
    saHpiResourceSeveritySetPtr               saHpiResourceSeveritySet;
    saHpiResourceTagSetPtr                    saHpiResourceTagSet;
    saHpiMyEntityPathGetPtr                   saHpiMyEntityPathGet;
    saHpiResourceIdGetPtr                     saHpiResourceIdGet;
    saHpiGetIdByEntityPathPtr                 saHpiGetIdByEntityPath;
    saHpiGetChildEntityPathPtr                saHpiGetChildEntityPath;
    saHpiResourceFailedRemovePtr              saHpiResourceFailedRemove;
    saHpiEventLogInfoGetPtr                   saHpiEventLogInfoGet;
    saHpiEventLogCapabilitiesGetPtr           saHpiEventLogCapabilitiesGet;
    saHpiEventLogEntryGetPtr                  saHpiEventLogEntryGet;
    saHpiEventLogEntryAddPtr                  saHpiEventLogEntryAdd;
    saHpiEventLogClearPtr                     saHpiEventLogClear;
    saHpiEventLogTimeGetPtr                   saHpiEventLogTimeGet;
    saHpiEventLogTimeSetPtr                   saHpiEventLogTimeSet;
    saHpiEventLogStateGetPtr                  saHpiEventLogStateGet;
    saHpiEventLogStateSetPtr                  saHpiEventLogStateSet;
    saHpiEventLogOverflowResetPtr             saHpiEventLogOverflowReset;
    saHpiSubscribePtr                         saHpiSubscribe;
    saHpiUnsubscribePtr                       saHpiUnsubscribe;
    saHpiEventGetPtr                          saHpiEventGet;
    saHpiEventAddPtr                          saHpiEventAdd;
    saHpiAlarmGetNextPtr                      saHpiAlarmGetNext;
    saHpiAlarmGetPtr                          saHpiAlarmGet;
    saHpiAlarmAcknowledgePtr                  saHpiAlarmAcknowledge;
    saHpiAlarmAddPtr                          saHpiAlarmAdd;
    saHpiAlarmDeletePtr                       saHpiAlarmDelete;
    saHpiRdrGetPtr                            saHpiRdrGet;
    saHpiRdrGetByInstrumentIdPtr              saHpiRdrGetByInstrumentId;
    saHpiRdrUpdateCountGetPtr                 saHpiRdrUpdateCountGet;
    saHpiSensorReadingGetPtr                  saHpiSensorReadingGet;
    saHpiSensorThresholdsGetPtr               saHpiSensorThresholdsGet;
    saHpiSensorThresholdsSetPtr               saHpiSensorThresholdsSet;
    saHpiSensorTypeGetPtr                     saHpiSensorTypeGet;
    saHpiSensorEnableGetPtr                   saHpiSensorEnableGet;
    saHpiSensorEnableSetPtr                   saHpiSensorEnableSet;
    saHpiSensorEventEnableGetPtr              saHpiSensorEventEnableGet;
    saHpiSensorEventEnableSetPtr              saHpiSensorEventEnableSet;
    saHpiSensorEventMasksGetPtr               saHpiSensorEventMasksGet;
    saHpiSensorEventMasksSetPtr               saHpiSensorEventMasksSet;
    saHpiControlTypeGetPtr                    saHpiControlTypeGet;
    saHpiControlGetPtr                        saHpiControlGet;
    saHpiControlSetPtr                        saHpiControlSet;
    saHpiIdrInfoGetPtr                        saHpiIdrInfoGet;
    saHpiIdrAreaHeaderGetPtr                  saHpiIdrAreaHeaderGet;
    saHpiIdrAreaAddPtr                        saHpiIdrAreaAdd;
    saHpiIdrAreaAddByIdPtr                    saHpiIdrAreaAddById;
    saHpiIdrAreaDeletePtr                     saHpiIdrAreaDelete;
    saHpiIdrFieldGetPtr                       saHpiIdrFieldGet;
    saHpiIdrFieldAddPtr                       saHpiIdrFieldAdd;
    saHpiIdrFieldAddByIdPtr                   saHpiIdrFieldAddById;
    saHpiIdrFieldSetPtr                       saHpiIdrFieldSet;
    saHpiIdrFieldDeletePtr                    saHpiIdrFieldDelete;
    saHpiWatchdogTimerGetPtr                  saHpiWatchdogTimerGet;
    saHpiWatchdogTimerSetPtr                  saHpiWatchdogTimerSet;
    saHpiWatchdogTimerResetPtr                saHpiWatchdogTimerReset;
    saHpiAnnunciatorGetNextPtr                saHpiAnnunciatorGetNext;
    saHpiAnnunciatorGetPtr                    saHpiAnnunciatorGet;
    saHpiAnnunciatorAcknowledgePtr            saHpiAnnunciatorAcknowledge;
    saHpiAnnunciatorAddPtr                    saHpiAnnunciatorAdd;
    saHpiAnnunciatorDeletePtr                 saHpiAnnunciatorDelete;
    saHpiAnnunciatorModeGetPtr                saHpiAnnunciatorModeGet;
    saHpiAnnunciatorModeSetPtr                saHpiAnnunciatorModeSet;
    saHpiDimiInfoGetPtr                       saHpiDimiInfoGet;
    saHpiDimiTestInfoGetPtr                   saHpiDimiTestInfoGet;
    saHpiDimiTestReadinessGetPtr              saHpiDimiTestReadinessGet;
    saHpiDimiTestStartPtr                     saHpiDimiTestStart;
    saHpiDimiTestCancelPtr                    saHpiDimiTestCancel;
    saHpiDimiTestStatusGetPtr                 saHpiDimiTestStatusGet;
    saHpiDimiTestResultsGetPtr                saHpiDimiTestResultsGet;
    saHpiFumiSpecInfoGetPtr                   saHpiFumiSpecInfoGet;
    saHpiFumiServiceImpactGetPtr              saHpiFumiServiceImpactGet;
    saHpiFumiSourceSetPtr                     saHpiFumiSourceSet;
    saHpiFumiSourceInfoValidateStartPtr       saHpiFumiSourceInfoValidateStart;
    saHpiFumiSourceInfoGetPtr                 saHpiFumiSourceInfoGet;
    saHpiFumiSourceComponentInfoGetPtr        saHpiFumiSourceComponentInfoGet;
    saHpiFumiTargetInfoGetPtr                 saHpiFumiTargetInfoGet;
    saHpiFumiTargetComponentInfoGetPtr        saHpiFumiTargetComponentInfoGet;
    saHpiFumiLogicalTargetInfoGetPtr          saHpiFumiLogicalTargetInfoGet;
    saHpiFumiLogicalTargetComponentInfoGetPtr saHpiFumiLogicalTargetComponentInfoGet;
    saHpiFumiBackupStartPtr                   saHpiFumiBackupStart;
    saHpiFumiBankBootOrderSetPtr              saHpiFumiBankBootOrderSet;
    saHpiFumiBankCopyStartPtr                 saHpiFumiBankCopyStart;
    saHpiFumiInstallStartPtr                  saHpiFumiInstallStart;
    saHpiFumiUpgradeStatusGetPtr              saHpiFumiUpgradeStatusGet;
    saHpiFumiTargetVerifyStartPtr             saHpiFumiTargetVerifyStart;
    saHpiFumiTargetVerifyMainStartPtr         saHpiFumiTargetVerifyMainStart;
    saHpiFumiUpgradeCancelPtr                 saHpiFumiUpgradeCancel;
    saHpiFumiAutoRollbackDisableGetPtr        saHpiFumiAutoRollbackDisableGet;
    saHpiFumiAutoRollbackDisableSetPtr        saHpiFumiAutoRollbackDisableSet;
    saHpiFumiRollbackStartPtr                 saHpiFumiRollbackStart;
    saHpiFumiActivatePtr                      saHpiFumiActivate;
    saHpiFumiActivateStartPtr                 saHpiFumiActivateStart;
    saHpiFumiCleanupPtr                       saHpiFumiCleanup;
    saHpiHotSwapPolicyCancelPtr               saHpiHotSwapPolicyCancel;
    saHpiResourceActiveSetPtr                 saHpiResourceActiveSet;
    saHpiResourceInactiveSetPtr               saHpiResourceInactiveSet;
    saHpiAutoInsertTimeoutGetPtr              saHpiAutoInsertTimeoutGet;
    saHpiAutoInsertTimeoutSetPtr              saHpiAutoInsertTimeoutSet;
    saHpiAutoExtractTimeoutGetPtr             saHpiAutoExtractTimeoutGet;
    saHpiAutoExtractTimeoutSetPtr             saHpiAutoExtractTimeoutSet;
    saHpiHotSwapStateGetPtr                   saHpiHotSwapStateGet;
    saHpiHotSwapActionRequestPtr              saHpiHotSwapActionRequest;
    saHpiHotSwapIndicatorStateGetPtr          saHpiHotSwapIndicatorStateGet;
    saHpiHotSwapIndicatorStateSetPtr          saHpiHotSwapIndicatorStateSet;
    saHpiParmControlPtr                       saHpiParmControl;
    saHpiResourceLoadIdGetPtr                 saHpiResourceLoadIdGet;
    saHpiResourceLoadIdSetPtr                 saHpiResourceLoadIdSet;
    saHpiResourceResetStateGetPtr             saHpiResourceResetStateGet;
    saHpiResourceResetStateSetPtr             saHpiResourceResetStateSet;
    saHpiResourcePowerStateGetPtr             saHpiResourcePowerStateGet;
    saHpiResourcePowerStateSetPtr             saHpiResourcePowerStateSet;
    oHpiDomainAddPtr                          oHpiDomainAdd;
};


/**************************************************************
 * class cBaseLib
 *************************************************************/
class cBaseLib
{
public:

    const BaseLibAbi * Abi() const
    {
        return &m_abi;
    }

protected:

    explicit cBaseLib();

    ~cBaseLib();

    bool LoadBaseLib();

private:

    GModule * m_handle;
    BaseLibAbi m_abi;
};


}; // namespace Slave


#endif // __BASELIB_H__

