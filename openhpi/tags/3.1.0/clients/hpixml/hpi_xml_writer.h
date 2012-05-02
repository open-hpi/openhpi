/*      -*- c++ -*-
 *
 * Copyright (C) 2011, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef HPI_XML_WRITER_H_EC5AF80F_A79B_49D7_8371_F71504C426A6
#define HPI_XML_WRITER_H_EC5AF80F_A79B_49D7_8371_F71504C426A6

#include <SaHpi.h>

#include "xml_writer.h"


/***************************************************
 * class cHpiXmlWriter
 ***************************************************/
class cHpiXmlWriter : private cXmlWriter
{
public:

    explicit cHpiXmlWriter( int indent_step, bool use_names );

    void Begin( void );
    void End( void );

    void VersionNode( const SaHpiVersionT& ver );

    void BeginDomainNode( const SaHpiDomainInfoT& di );
    void EndDomainNode();

    void BeginDrtNode( const SaHpiDomainInfoT& di );
    void EndDrtNode();
    void DrtEntryNode( const SaHpiDrtEntryT& drte );

    void BeginRptNode( const SaHpiDomainInfoT& di );
    void EndRptNode();
    void BeginResourceNode( const SaHpiRptEntryT& rpte,
                            SaHpiUint32T rdr_update_count );
    void EndResourceNode();

    void BeginInstrumentNode( const SaHpiRdrT& rdr );
    void EndInstrumentNode();

    void BeginEventLogNode( SaHpiResourceIdT rid,
                            const SaHpiEventLogInfoT& info,
                            const SaHpiEventLogCapabilitiesT& caps );
    void EndEventLogNode( SaHpiResourceIdT rid );
    void LogEntryNode( const SaHpiEventLogEntryT& entry,
                       const SaHpiRdrT& rdr,
                       const SaHpiRptEntryT& rpte );

    void BeginDatNode( const SaHpiDomainInfoT& di );
    void EndDatNode();
    void AlarmNode( const SaHpiAlarmT& a );

private:

    /****************************
     * Basic Types
     ***************************/
    void NodeSaHpiUint8T(
        const char * name,
        const SaHpiUint8T& x
    );
    void NodeSaHpiUint16T(
        const char * name,
        const SaHpiUint16T& x
    );
    void NodeSaHpiUint32T(
        const char * name,
        const SaHpiUint32T& x
    );
    void NodeSaHpiUint64T(
        const char * name,
        const SaHpiUint64T& x
    );
    void NodeSaHpiInt8T(
        const char * name,
        const SaHpiInt8T& x
    );
    void NodeSaHpiInt16T(
        const char * name,
        const SaHpiInt16T& x
    );
    void NodeSaHpiInt32T(
        const char * name,
        const SaHpiInt32T& x
    );
    void NodeSaHpiInt64T(
        const char * name,
        const SaHpiInt64T& x
    );
    void NodeSaHpiFloat64T(
        const char * name,
        const SaHpiFloat64T& x
    );

    /****************************
     * Typedefs
     ***************************/
    void NodeSaHpiBoolT(
        const char * name,
        const SaHpiBoolT& x
    );
    void NodeSaHpiManufacturerIdT(
        const char * name,
        const SaHpiManufacturerIdT& x
    );
    void NodeSaHpiVersionT(
        const char * name,
        const SaHpiVersionT& x
    );
    void NodeSaHpiDomainIdT(
        const char * name,
        const SaHpiDomainIdT& x
    );
    void NodeSaHpiSessionIdT(
        const char * name,
        const SaHpiSessionIdT& x
    );
    void NodeSaHpiResourceIdT(
        const char * name,
        const SaHpiResourceIdT& x
    );
    void NodeSaHpiEntryIdT(
        const char * name,
        const SaHpiEntryIdT& x
    );
    void NodeSaHpiTimeT(
        const char * name,
        const SaHpiTimeT& x
    );
    void NodeSaHpiTimeoutT(
        const char * name,
        const SaHpiTimeoutT& x
    );
    void NodeSaHpiInstrumentIdT(
        const char * name,
        const SaHpiInstrumentIdT& x
    );
    void NodeSaHpiEntityLocationT(
        const char * name,
        const SaHpiEntityLocationT& x
    );
    void NodeSaHpiEventStateT(
        const char * name,
        const SaHpiEventStateT& x
    );
    void NodeSaHpiSensorNumT(
        const char * name,
        const SaHpiSensorNumT& x
    );
    void NodeSaHpiSensorRangeFlagsT(
        const char * name,
        const SaHpiSensorRangeFlagsT& x
    );
    void NodeSaHpiSensorThdMaskT(
        const char * name,
        const SaHpiSensorThdMaskT& x
    );
    void NodeSaHpiCtrlNumT(
        const char * name,
        const SaHpiCtrlNumT& x
    );
    void NodeSaHpiCtrlStateDiscreteT(
        const char * name,
        const SaHpiCtrlStateDiscreteT& x
    );
    void NodeSaHpiCtrlStateAnalogT(
        const char * name,
        const SaHpiCtrlStateAnalogT& x
    );
    void NodeSaHpiTxtLineNumT(
        const char * name,
        const SaHpiTxtLineNumT& x
    );
    void NodeSaHpiIdrIdT(
        const char * name,
        const SaHpiIdrIdT& x
    );
    void NodeSaHpiWatchdogNumT(
        const char * name,
        const SaHpiWatchdogNumT& x
    );
    void NodeSaHpiWatchdogExpFlagsT(
        const char * name,
        const SaHpiWatchdogExpFlagsT& x
    );
    void NodeSaHpiDimiNumT(
        const char * name,
        const SaHpiDimiNumT& x
    );
    void NodeSaHpiDimiTestCapabilityT(
        const char * name,
        const SaHpiDimiTestCapabilityT& x
    );
    void NodeSaHpiDimiTestNumT(
        const char * name,
        const SaHpiDimiTestNumT& x
    );
    void NodeSaHpiDimiTestPercentCompletedT(
        const char * name,
        const SaHpiDimiTestPercentCompletedT& x
    );
    void NodeSaHpiFumiNumT(
        const char * name,
        const SaHpiFumiNumT& x
    );
    void NodeSaHpiBankNumT(
        const char * name,
        const SaHpiBankNumT& x
    );
    void NodeSaHpiFumiLogicalBankStateFlagsT(
        const char * name,
        const SaHpiFumiLogicalBankStateFlagsT& x
    );
    void NodeSaHpiFumiProtocolT(
        const char * name,
        const SaHpiFumiProtocolT& x
    );
    void NodeSaHpiFumiCapabilityT(
        const char * name,
        const SaHpiFumiCapabilityT& x
    );
    void NodeSaHpiSensorOptionalDataT(
        const char * name,
        const SaHpiSensorOptionalDataT& x
    );
    void NodeSaHpiSensorEnableOptDataT(
        const char * name,
        const SaHpiSensorEnableOptDataT& x
    );
    void NodeSaHpiEvtQueueStatusT(
        const char * name,
        const SaHpiEvtQueueStatusT& x
    );
    void NodeSaHpiAnnunciatorNumT(
        const char * name,
        const SaHpiAnnunciatorNumT& x
    );
    void NodeSaHpiLoadNumberT(
        const char * name,
        const SaHpiLoadNumberT& x
    );
    void NodeSaHpiCapabilitiesT(
        const char * name,
        const SaHpiCapabilitiesT& x
    );
    void NodeSaHpiHsCapabilitiesT(
        const char * name,
        const SaHpiHsCapabilitiesT& x
    );
    void NodeSaHpiDomainCapabilitiesT(
        const char * name,
        const SaHpiDomainCapabilitiesT& x
    );
    void NodeSaHpiAlarmIdT(
        const char * name,
        const SaHpiAlarmIdT& x
    );
    void NodeSaHpiEventLogCapabilitiesT(
        const char * name,
        const SaHpiEventLogCapabilitiesT& x
    );
    void NodeSaHpiEventLogEntryIdT(
        const char * name,
        const SaHpiEventLogEntryIdT& x
    );


    /****************************
     * Enums
     ***************************/
    void NodeSaHpiLanguageT(
        const char * name,
        const SaHpiLanguageT& x
    );
    void NodeSaHpiTextTypeT(
        const char * name,
        const SaHpiTextTypeT& x
    );
    void NodeSaHpiEntityTypeT(
        const char * name,
        const SaHpiEntityTypeT& x
    );
    void NodeSaHpiSensorTypeT(
        const char * name,
        const SaHpiSensorTypeT& x
    );
    void NodeSaHpiSensorReadingTypeT(
        const char * name,
        const SaHpiSensorReadingTypeT& x
    );
    void NodeSaHpiSensorEventMaskActionT(
        const char * name,
        const SaHpiSensorEventMaskActionT& x
    );
    void NodeSaHpiSensorUnitsT(
        const char * name,
        const SaHpiSensorUnitsT& x
    );
    void NodeSaHpiSensorModUnitUseT(
        const char * name,
        const SaHpiSensorModUnitUseT& x
    );
    void NodeSaHpiSensorEventCtrlT(
        const char * name,
        const SaHpiSensorEventCtrlT& x
    );
    void NodeSaHpiCtrlTypeT(
        const char * name,
        const SaHpiCtrlTypeT& x
    );
    void NodeSaHpiCtrlStateDigitalT(
        const char * name,
        const SaHpiCtrlStateDigitalT& x
    );
    void NodeSaHpiCtrlModeT(
        const char * name,
        const SaHpiCtrlModeT& x
    );
    void NodeSaHpiCtrlOutputTypeT(
        const char * name,
        const SaHpiCtrlOutputTypeT& x
    );
    void NodeSaHpiIdrAreaTypeT(
        const char * name,
        const SaHpiIdrAreaTypeT& x
    );
    void NodeSaHpiIdrFieldTypeT(
        const char * name,
        const SaHpiIdrFieldTypeT& x
    );
    void NodeSaHpiWatchdogActionT(
        const char * name,
        const SaHpiWatchdogActionT& x
    );
    void NodeSaHpiWatchdogActionEventT(
        const char * name,
        const SaHpiWatchdogActionEventT& x
    );
    void NodeSaHpiWatchdogPretimerInterruptT(
        const char * name,
        const SaHpiWatchdogPretimerInterruptT& x
    );
    void NodeSaHpiWatchdogTimerUseT(
        const char * name,
        const SaHpiWatchdogTimerUseT& x
    );
    void NodeSaHpiDimiTestServiceImpactT(
        const char * name,
        const SaHpiDimiTestServiceImpactT& x
    );
    void NodeSaHpiDimiTestRunStatusT(
        const char * name,
        const SaHpiDimiTestRunStatusT& x
    );
    void NodeSaHpiDimiTestErrCodeT(
        const char * name,
        const SaHpiDimiTestErrCodeT& x
    );
    void NodeSaHpiDimiTestParamTypeT(
        const char * name,
        const SaHpiDimiTestParamTypeT& x
    );
    void NodeSaHpiDimiReadyT(
        const char * name,
        const SaHpiDimiReadyT& x
    );
    void NodeSaHpiFumiSpecInfoTypeT(
        const char * name,
        const SaHpiFumiSpecInfoTypeT& x
    );
    void NodeSaHpiFumiSafDefinedSpecIdT(
        const char * name,
        const SaHpiFumiSafDefinedSpecIdT& x
    );
    void NodeSaHpiFumiServiceImpactT(
        const char * name,
        const SaHpiFumiServiceImpactT& x
    );
    void NodeSaHpiFumiSourceStatusT(
        const char * name,
        const SaHpiFumiSourceStatusT& x
    );
    void NodeSaHpiFumiBankStateT(
        const char * name,
        const SaHpiFumiBankStateT& x
    );
    void NodeSaHpiFumiUpgradeStatusT(
        const char * name,
        const SaHpiFumiUpgradeStatusT& x
    );
    void NodeSaHpiHsIndicatorStateT(
        const char * name,
        const SaHpiHsIndicatorStateT& x
    );
    void NodeSaHpiHsActionT(
        const char * name,
        const SaHpiHsActionT& x
    );
    void NodeSaHpiHsStateT(
        const char * name,
        const SaHpiHsStateT& x
    );
    void NodeSaHpiHsCauseOfStateChangeT(
        const char * name,
        const SaHpiHsCauseOfStateChangeT& x
    );
    void NodeSaHpiSeverityT(
        const char * name,
        const SaHpiSeverityT& x
    );
    void NodeSaHpiResourceEventTypeT(
        const char * name,
        const SaHpiResourceEventTypeT& x
    );
    void NodeSaHpiDomainEventTypeT(
        const char * name,
        const SaHpiDomainEventTypeT& x
    );
    void NodeSaHpiSwEventTypeT(
        const char * name,
        const SaHpiSwEventTypeT& x
    );
    void NodeSaHpiEventTypeT(
        const char * name,
        const SaHpiEventTypeT& x
    );
    void NodeSaHpiStatusCondTypeT(
        const char * name,
        const SaHpiStatusCondTypeT& x
    );
    void NodeSaHpiAnnunciatorModeT(
        const char * name,
        const SaHpiAnnunciatorModeT& x
    );
    void NodeSaHpiAnnunciatorTypeT(
        const char * name,
        const SaHpiAnnunciatorTypeT& x
    );
    void NodeSaHpiRdrTypeT(
        const char * name,
        const SaHpiRdrTypeT& x
    );
    void NodeSaHpiParmActionT(
        const char * name,
        const SaHpiParmActionT& x
    );
    void NodeSaHpiResetActionT(
        const char * name,
        const SaHpiResetActionT& x
    );
    void NodeSaHpiPowerStateT(
        const char * name,
        const SaHpiPowerStateT& x
    );
    void NodeSaHpiEventLogOverflowActionT(
        const char * name,
        const SaHpiEventLogOverflowActionT& x
    );


    /****************************
     * Enum-Like Types
     ***************************/
    void NodeSaErrorT(
        const char * name,
        const SaErrorT& x
    );

    void NodeSaHpiEventCategoryT(
        const char * name,
        const SaHpiEventCategoryT& x
    );

    /****************************
     * Complex Types
     ***************************/
    void NodeSaHpiTextBufferT(
        const char * name,
        const SaHpiTextBufferT& x
    );
    void NodeSaHpiEntityPathT(
        const char * name,
        const SaHpiEntityPathT& x
    );
    void NodeSaHpiNameT(
        const char * name,
        const SaHpiNameT& x
    );
    void NodeSaHpiConditionT(
        const char * name,
        const SaHpiConditionT& x
    );
    void NodeSaHpiGuidT(
        const char * name,
        const SaHpiGuidT& x
    );
    void NodeSaHpiRptEntryT(
        const char * name,
        const SaHpiRptEntryT& x
    );
    void NodeSaHpiCtrlStateStreamT(
        const char * name,
        const SaHpiCtrlStateStreamT& x
    );
    void NodeSaHpiCtrlStateTextT(
        const char * name,
        const SaHpiCtrlStateTextT& x
    );
    void NodeSaHpiCtrlStateOemT(
        const char * name,
        const SaHpiCtrlStateOemT& x
    );
    void NodeSaHpiCtrlRecDigitalT(
        const char * name,
        const SaHpiCtrlRecDigitalT& x
    );
    void NodeSaHpiCtrlRecDiscreteT(
        const char * name,
        const SaHpiCtrlRecDiscreteT& x
    );
    void NodeSaHpiCtrlRecAnalogT(
        const char * name,
        const SaHpiCtrlRecAnalogT& x
    );
    void NodeSaHpiCtrlRecStreamT(
        const char * name,
        const SaHpiCtrlRecStreamT& x
    );
    void NodeSaHpiCtrlRecTextT(
        const char * name,
        const SaHpiCtrlRecTextT& x
    );
    void NodeSaHpiCtrlRecOemT(
        const char * name,
        const SaHpiCtrlRecOemT& x
    );
    void NodeSaHpiCtrlRecT(
        const char * name,
        const SaHpiCtrlRecT& x
    );
    void NodeSaHpiSensorReadingT(
        const char * name,
        const SaHpiSensorReadingT& x
    );
    void NodeSaHpiSensorRangeT(
        const char * name,
        const SaHpiSensorRangeT& x
    );
    void NodeSaHpiSensorDataFormatT(
        const char * name,
        const SaHpiSensorDataFormatT& x
    );
    void NodeSaHpiSensorThdDefnT(
        const char * name,
        const SaHpiSensorThdDefnT& x
    );
    void NodeSaHpiSensorRecT(
        const char * name,
        const SaHpiSensorRecT& x
    );
    void NodeSaHpiInventoryRecT(
        const char * name,
        const SaHpiInventoryRecT& x
    );
    void NodeSaHpiWatchdogRecT(
        const char * name,
        const SaHpiWatchdogRecT& x
    );
    void NodeSaHpiAnnunciatorRecT(
        const char * name,
        const SaHpiAnnunciatorRecT& x
    );
    void NodeSaHpiDimiRecT(
        const char * name,
        const SaHpiDimiRecT& x
    );
    void NodeSaHpiFumiRecT(
        const char * name,
        const SaHpiFumiRecT& x
    );
    void NodeSaHpiRdrT(
        const char * name,
        const SaHpiRdrT& x
    );
    void NodeSaHpiResourceEventT(
        const char * name,
        const SaHpiResourceEventT& x
    );
    void NodeSaHpiDomainEventT(
        const char * name,
        const SaHpiDomainEventT& x
    );
    void NodeSaHpiSensorEventT(
        const char * name,
        const SaHpiSensorEventT& x
    );
    void NodeSaHpiSensorEnableChangeEventT(
        const char * name,
        const SaHpiSensorEnableChangeEventT& x
    );
    void NodeSaHpiHotSwapEventT(
        const char * name,
        const SaHpiHotSwapEventT& x
    );
    void NodeSaHpiWatchdogEventT(
        const char * name,
        const SaHpiWatchdogEventT& x
    );
    void NodeSaHpiHpiSwEventT(
        const char * name,
        const SaHpiHpiSwEventT& x
    );
    void NodeSaHpiOemEventT(
        const char * name,
        const SaHpiOemEventT& x
    );
    void NodeSaHpiUserEventT(
        const char * name,
        const SaHpiUserEventT& x
    );
    void NodeSaHpiDimiEventT(
        const char * name,
        const SaHpiDimiEventT& x
    );
    void NodeSaHpiDimiUpdateEventT(
        const char * name,
        const SaHpiDimiUpdateEventT& x
    );
    void NodeSaHpiFumiEventT(
        const char * name,
        const SaHpiFumiEventT& x
    );
    void NodeSaHpiEventT(
        const char * name,
        const SaHpiEventT& x
    );
};


#endif /* HPI_XML_WRITER_H_EC5AF80F_A79B_49D7_8371_F71504C426A6 */

