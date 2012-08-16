
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#

from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib.HpiMarshalCore import HpiMarshalCore


#**********************************************************
# HPI Marshal (auto-generated)
#**********************************************************
class HpiMarshalGen( HpiMarshalCore ):

    #**********************************************************
    # Marshal: For HPI Simple Data Types
    #**********************************************************
    def marshalSaHpiBoolT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiManufacturerIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiVersionT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaErrorT( self, x ):
        self.marshalSaHpiInt32T( x )

    def marshalSaHpiDomainIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiSessionIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiResourceIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiEntryIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiTimeT( self, x ):
        self.marshalSaHpiInt64T( x )

    def marshalSaHpiTimeoutT( self, x ):
        self.marshalSaHpiInt64T( x )

    def marshalSaHpiInstrumentIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiEntityLocationT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiEventCategoryT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiEventStateT( self, x ):
        self.marshalSaHpiUint16T( x )

    def marshalSaHpiSensorNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiSensorRangeFlagsT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiSensorThdMaskT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiCtrlNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiCtrlStateDiscreteT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiCtrlStateAnalogT( self, x ):
        self.marshalSaHpiInt32T( x )

    def marshalSaHpiTxtLineNumT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiIdrIdT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiWatchdogNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiWatchdogExpFlagsT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiDimiNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiDimiTestCapabilityT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiDimiTestNumT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiDimiTestPercentCompletedT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiFumiNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiBankNumT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiFumiLogicalBankStateFlagsT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiFumiProtocolT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiFumiCapabilityT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiSensorOptionalDataT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiSensorEnableOptDataT( self, x ):
        self.marshalSaHpiUint8T( x )

    def marshalSaHpiEvtQueueStatusT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiAnnunciatorNumT( self, x ):
        self.marshalSaHpiInstrumentIdT( x )

    def marshalSaHpiLoadNumberT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiCapabilitiesT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiHsCapabilitiesT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiDomainCapabilitiesT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiAlarmIdT( self, x ):
        self.marshalSaHpiEntryIdT( x )

    def marshalSaHpiEventLogCapabilitiesT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiEventLogEntryIdT( self, x ):
        self.marshalSaHpiUint32T( x )

    def marshalSaHpiLanguageT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiTextTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiEntityTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorReadingTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorEventMaskActionT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorUnitsT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorModUnitUseT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSensorEventCtrlT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiCtrlTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiCtrlStateDigitalT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiCtrlModeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiCtrlOutputTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiIdrAreaTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiIdrFieldTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiWatchdogActionT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiWatchdogActionEventT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiWatchdogPretimerInterruptT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiWatchdogTimerUseT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDimiTestServiceImpactT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDimiTestRunStatusT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDimiTestErrCodeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDimiTestParamTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDimiReadyT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiSpecInfoTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiSafDefinedSpecIdT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiServiceImpactT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiSourceStatusT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiBankStateT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiFumiUpgradeStatusT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiHsIndicatorStateT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiHsActionT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiHsStateT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiHsCauseOfStateChangeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSeverityT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiResourceEventTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiDomainEventTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiSwEventTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiEventTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiStatusCondTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiAnnunciatorModeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiAnnunciatorTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiRdrTypeT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiParmActionT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiResetActionT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiPowerStateT( self, x ):
        self.marshalEnum( x )

    def marshalSaHpiEventLogOverflowActionT( self, x ):
        self.marshalEnum( x )


    #**********************************************************
    # Marshal: For HPI Structs and Unions
    #**********************************************************
    def marshalSaHpiTextBufferT( self, x ):
        self.marshalSaHpiTextTypeT( x.DataType )
        self.marshalSaHpiLanguageT( x.Language )
        self.marshalSaHpiUint8T( x.DataLength )
        self.marshalByteArray( x.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH )

    def marshalSaHpiEntityT( self, x ):
        self.marshalSaHpiEntityTypeT( x.EntityType )
        self.marshalSaHpiEntityLocationT( x.EntityLocation )

    def marshalSaHpiEntityPathT( self, x ):
        for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
            self.marshalSaHpiEntityT( x.Entry[i] )

    def marshalSaHpiSensorReadingUnionT( self, x, mod ):
        if mod == SAHPI_SENSOR_READING_TYPE_INT64:
            self.marshalSaHpiInt64T( x.SensorInt64 )
        if mod == SAHPI_SENSOR_READING_TYPE_UINT64:
            self.marshalSaHpiUint64T( x.SensorUint64 )
        if mod == SAHPI_SENSOR_READING_TYPE_FLOAT64:
            self.marshalSaHpiFloat64T( x.SensorFloat64 )
        if mod == SAHPI_SENSOR_READING_TYPE_BUFFER:
            self.marshalByteArray( x.SensorBuffer, SAHPI_SENSOR_BUFFER_LENGTH )

    def marshalSaHpiSensorReadingT( self, x ):
        self.marshalSaHpiBoolT( x.IsSupported )
        self.marshalSaHpiSensorReadingTypeT( x.Type )
        self.marshalSaHpiSensorReadingUnionT( x.Value, x.Type )

    def marshalSaHpiSensorThresholdsT( self, x ):
        self.marshalSaHpiSensorReadingT( x.LowCritical )
        self.marshalSaHpiSensorReadingT( x.LowMajor )
        self.marshalSaHpiSensorReadingT( x.LowMinor )
        self.marshalSaHpiSensorReadingT( x.UpCritical )
        self.marshalSaHpiSensorReadingT( x.UpMajor )
        self.marshalSaHpiSensorReadingT( x.UpMinor )
        self.marshalSaHpiSensorReadingT( x.PosThdHysteresis )
        self.marshalSaHpiSensorReadingT( x.NegThdHysteresis )

    def marshalSaHpiSensorRangeT( self, x ):
        self.marshalSaHpiSensorRangeFlagsT( x.Flags )
        self.marshalSaHpiSensorReadingT( x.Max )
        self.marshalSaHpiSensorReadingT( x.Min )
        self.marshalSaHpiSensorReadingT( x.Nominal )
        self.marshalSaHpiSensorReadingT( x.NormalMax )
        self.marshalSaHpiSensorReadingT( x.NormalMin )

    def marshalSaHpiSensorDataFormatT( self, x ):
        self.marshalSaHpiBoolT( x.IsSupported )
        self.marshalSaHpiSensorReadingTypeT( x.ReadingType )
        self.marshalSaHpiSensorUnitsT( x.BaseUnits )
        self.marshalSaHpiSensorUnitsT( x.ModifierUnits )
        self.marshalSaHpiSensorModUnitUseT( x.ModifierUse )
        self.marshalSaHpiBoolT( x.Percentage )
        self.marshalSaHpiSensorRangeT( x.Range )
        self.marshalSaHpiFloat64T( x.AccuracyFactor )

    def marshalSaHpiSensorThdDefnT( self, x ):
        self.marshalSaHpiBoolT( x.IsAccessible )
        self.marshalSaHpiSensorThdMaskT( x.ReadThold )
        self.marshalSaHpiSensorThdMaskT( x.WriteThold )
        self.marshalSaHpiBoolT( x.Nonlinear )

    def marshalSaHpiSensorRecT( self, x ):
        self.marshalSaHpiSensorNumT( x.Num )
        self.marshalSaHpiSensorTypeT( x.Type )
        self.marshalSaHpiEventCategoryT( x.Category )
        self.marshalSaHpiBoolT( x.EnableCtrl )
        self.marshalSaHpiSensorEventCtrlT( x.EventCtrl )
        self.marshalSaHpiEventStateT( x.Events )
        self.marshalSaHpiSensorDataFormatT( x.DataFormat )
        self.marshalSaHpiSensorThdDefnT( x.ThresholdDefn )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiCtrlStateStreamT( self, x ):
        self.marshalSaHpiBoolT( x.Repeat )
        self.marshalSaHpiUint32T( x.StreamLength )
        self.marshalByteArray( x.Stream, SAHPI_CTRL_MAX_STREAM_LENGTH )

    def marshalSaHpiCtrlStateTextT( self, x ):
        self.marshalSaHpiTxtLineNumT( x.Line )
        self.marshalSaHpiTextBufferT( x.Text )

    def marshalSaHpiCtrlStateOemT( self, x ):
        self.marshalSaHpiManufacturerIdT( x.MId )
        self.marshalSaHpiUint8T( x.BodyLength )
        self.marshalByteArray( x.Body, SAHPI_CTRL_MAX_OEM_BODY_LENGTH )

    def marshalSaHpiCtrlStateUnionT( self, x, mod ):
        if mod == SAHPI_CTRL_TYPE_DIGITAL:
            self.marshalSaHpiCtrlStateDigitalT( x.Digital )
        if mod == SAHPI_CTRL_TYPE_DISCRETE:
            self.marshalSaHpiCtrlStateDiscreteT( x.Discrete )
        if mod == SAHPI_CTRL_TYPE_ANALOG:
            self.marshalSaHpiCtrlStateAnalogT( x.Analog )
        if mod == SAHPI_CTRL_TYPE_STREAM:
            self.marshalSaHpiCtrlStateStreamT( x.Stream )
        if mod == SAHPI_CTRL_TYPE_TEXT:
            self.marshalSaHpiCtrlStateTextT( x.Text )
        if mod == SAHPI_CTRL_TYPE_OEM:
            self.marshalSaHpiCtrlStateOemT( x.Oem )

    def marshalSaHpiCtrlStateT( self, x ):
        self.marshalSaHpiCtrlTypeT( x.Type )
        self.marshalSaHpiCtrlStateUnionT( x.StateUnion, x.Type )

    def marshalSaHpiCtrlRecDigitalT( self, x ):
        self.marshalSaHpiCtrlStateDigitalT( x.Default )

    def marshalSaHpiCtrlRecDiscreteT( self, x ):
        self.marshalSaHpiCtrlStateDiscreteT( x.Default )

    def marshalSaHpiCtrlRecAnalogT( self, x ):
        self.marshalSaHpiCtrlStateAnalogT( x.Min )
        self.marshalSaHpiCtrlStateAnalogT( x.Max )
        self.marshalSaHpiCtrlStateAnalogT( x.Default )

    def marshalSaHpiCtrlRecStreamT( self, x ):
        self.marshalSaHpiCtrlStateStreamT( x.Default )

    def marshalSaHpiCtrlRecTextT( self, x ):
        self.marshalSaHpiUint8T( x.MaxChars )
        self.marshalSaHpiUint8T( x.MaxLines )
        self.marshalSaHpiLanguageT( x.Language )
        self.marshalSaHpiTextTypeT( x.DataType )
        self.marshalSaHpiCtrlStateTextT( x.Default )

    def marshalSaHpiCtrlRecOemT( self, x ):
        self.marshalSaHpiManufacturerIdT( x.MId )
        self.marshalByteArray( x.ConfigData, SAHPI_CTRL_OEM_CONFIG_LENGTH )
        self.marshalSaHpiCtrlStateOemT( x.Default )

    def marshalSaHpiCtrlRecUnionT( self, x, mod ):
        if mod == SAHPI_CTRL_TYPE_DIGITAL:
            self.marshalSaHpiCtrlRecDigitalT( x.Digital )
        if mod == SAHPI_CTRL_TYPE_DISCRETE:
            self.marshalSaHpiCtrlRecDiscreteT( x.Discrete )
        if mod == SAHPI_CTRL_TYPE_ANALOG:
            self.marshalSaHpiCtrlRecAnalogT( x.Analog )
        if mod == SAHPI_CTRL_TYPE_STREAM:
            self.marshalSaHpiCtrlRecStreamT( x.Stream )
        if mod == SAHPI_CTRL_TYPE_TEXT:
            self.marshalSaHpiCtrlRecTextT( x.Text )
        if mod == SAHPI_CTRL_TYPE_OEM:
            self.marshalSaHpiCtrlRecOemT( x.Oem )

    def marshalSaHpiCtrlDefaultModeT( self, x ):
        self.marshalSaHpiCtrlModeT( x.Mode )
        self.marshalSaHpiBoolT( x.ReadOnly )

    def marshalSaHpiCtrlRecT( self, x ):
        self.marshalSaHpiCtrlNumT( x.Num )
        self.marshalSaHpiCtrlOutputTypeT( x.OutputType )
        self.marshalSaHpiCtrlTypeT( x.Type )
        self.marshalSaHpiCtrlRecUnionT( x.TypeUnion, x.Type )
        self.marshalSaHpiCtrlDefaultModeT( x.DefaultMode )
        self.marshalSaHpiBoolT( x.WriteOnly )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiIdrFieldT( self, x ):
        self.marshalSaHpiEntryIdT( x.AreaId )
        self.marshalSaHpiEntryIdT( x.FieldId )
        self.marshalSaHpiIdrFieldTypeT( x.Type )
        self.marshalSaHpiBoolT( x.ReadOnly )
        self.marshalSaHpiTextBufferT( x.Field )

    def marshalSaHpiIdrAreaHeaderT( self, x ):
        self.marshalSaHpiEntryIdT( x.AreaId )
        self.marshalSaHpiIdrAreaTypeT( x.Type )
        self.marshalSaHpiBoolT( x.ReadOnly )
        self.marshalSaHpiUint32T( x.NumFields )

    def marshalSaHpiIdrInfoT( self, x ):
        self.marshalSaHpiIdrIdT( x.IdrId )
        self.marshalSaHpiUint32T( x.UpdateCount )
        self.marshalSaHpiBoolT( x.ReadOnly )
        self.marshalSaHpiUint32T( x.NumAreas )

    def marshalSaHpiInventoryRecT( self, x ):
        self.marshalSaHpiIdrIdT( x.IdrId )
        self.marshalSaHpiBoolT( x.Persistent )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiWatchdogT( self, x ):
        self.marshalSaHpiBoolT( x.Log )
        self.marshalSaHpiBoolT( x.Running )
        self.marshalSaHpiWatchdogTimerUseT( x.TimerUse )
        self.marshalSaHpiWatchdogActionT( x.TimerAction )
        self.marshalSaHpiWatchdogPretimerInterruptT( x.PretimerInterrupt )
        self.marshalSaHpiUint32T( x.PreTimeoutInterval )
        self.marshalSaHpiWatchdogExpFlagsT( x.TimerUseExpFlags )
        self.marshalSaHpiUint32T( x.InitialCount )
        self.marshalSaHpiUint32T( x.PresentCount )

    def marshalSaHpiWatchdogRecT( self, x ):
        self.marshalSaHpiWatchdogNumT( x.WatchdogNum )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiDimiTestAffectedEntityT( self, x ):
        self.marshalSaHpiEntityPathT( x.EntityImpacted )
        self.marshalSaHpiDimiTestServiceImpactT( x.ServiceImpact )

    def marshalSaHpiDimiTestResultsT( self, x ):
        self.marshalSaHpiTimeT( x.ResultTimeStamp )
        self.marshalSaHpiTimeoutT( x.RunDuration )
        self.marshalSaHpiDimiTestRunStatusT( x.LastRunStatus )
        self.marshalSaHpiDimiTestErrCodeT( x.TestErrorCode )
        self.marshalSaHpiTextBufferT( x.TestResultString )
        self.marshalSaHpiBoolT( x.TestResultStringIsURI )

    def marshalSaHpiDimiTestParamValueT( self, x, mod ):
        if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
            self.marshalSaHpiInt32T( x.paramint )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
            self.marshalSaHpiBoolT( x.parambool )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
            self.marshalSaHpiFloat64T( x.paramfloat )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
            self.marshalSaHpiTextBufferT( x.paramtext )

    def marshalSaHpiDimiTestParameterValueUnionT( self, x, mod ):
        if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
            self.marshalSaHpiInt32T( x.IntValue )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
            self.marshalSaHpiFloat64T( x.FloatValue )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
            self.marshalSaHpiFloat64T( x.FloatValue )
        if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
            self.marshalSaHpiFloat64T( x.FloatValue )

    def marshalSaHpiDimiTestParamsDefinitionT( self, x ):
        self.marshalByteArray( x.ParamName, SAHPI_DIMITEST_PARAM_NAME_LEN )
        self.marshalSaHpiTextBufferT( x.ParamInfo )
        self.marshalSaHpiDimiTestParamTypeT( x.ParamType )
        self.marshalSaHpiDimiTestParameterValueUnionT( x.MinValue, x.ParamType )
        self.marshalSaHpiDimiTestParameterValueUnionT( x.MaxValue, x.ParamType )
        self.marshalSaHpiDimiTestParamValueT( x.DefaultParam, x.ParamType )

    def marshalSaHpiDimiTestT( self, x ):
        self.marshalSaHpiTextBufferT( x.TestName )
        self.marshalSaHpiDimiTestServiceImpactT( x.ServiceImpact )
        for i in range( 0, SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ):
            self.marshalSaHpiDimiTestAffectedEntityT( x.EntitiesImpacted[i] )
        self.marshalSaHpiBoolT( x.NeedServiceOS )
        self.marshalSaHpiTextBufferT( x.ServiceOS )
        self.marshalSaHpiTimeT( x.ExpectedRunDuration )
        self.marshalSaHpiDimiTestCapabilityT( x.TestCapabilities )
        for i in range( 0, SAHPI_DIMITEST_MAX_PARAMETERS ):
            self.marshalSaHpiDimiTestParamsDefinitionT( x.TestParameters[i] )

    def marshalSaHpiDimiTestVariableParamsT( self, x ):
        self.marshalByteArray( x.ParamName, SAHPI_DIMITEST_PARAM_NAME_LEN )
        self.marshalSaHpiDimiTestParamTypeT( x.ParamType )
        self.marshalSaHpiDimiTestParamValueT( x.Value, x.ParamType )

    def marshalSaHpiDimiInfoT( self, x ):
        self.marshalSaHpiUint32T( x.NumberOfTests )
        self.marshalSaHpiUint32T( x.TestNumUpdateCounter )

    def marshalSaHpiDimiRecT( self, x ):
        self.marshalSaHpiDimiNumT( x.DimiNum )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiFumiSafDefinedSpecInfoT( self, x ):
        self.marshalSaHpiFumiSafDefinedSpecIdT( x.SpecID )
        self.marshalSaHpiUint32T( x.RevisionID )

    def marshalSaHpiFumiOemDefinedSpecInfoT( self, x ):
        self.marshalSaHpiManufacturerIdT( x.Mid )
        self.marshalSaHpiUint8T( x.BodyLength )
        self.marshalByteArray( x.Body, SAHPI_FUMI_MAX_OEM_BODY_LENGTH )

    def marshalSaHpiFumiSpecInfoTypeUnionT( self, x, mod ):
        if mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
            self.marshalSaHpiFumiSafDefinedSpecInfoT( x.SafDefined )
        if mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
            self.marshalSaHpiFumiOemDefinedSpecInfoT( x.OemDefined )

    def marshalSaHpiFumiSpecInfoT( self, x ):
        self.marshalSaHpiFumiSpecInfoTypeT( x.SpecInfoType )
        self.marshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoTypeUnion, x.SpecInfoType )

    def marshalSaHpiFumiFirmwareInstanceInfoT( self, x ):
        self.marshalSaHpiBoolT( x.InstancePresent )
        self.marshalSaHpiTextBufferT( x.Identifier )
        self.marshalSaHpiTextBufferT( x.Description )
        self.marshalSaHpiTextBufferT( x.DateTime )
        self.marshalSaHpiUint32T( x.MajorVersion )
        self.marshalSaHpiUint32T( x.MinorVersion )
        self.marshalSaHpiUint32T( x.AuxVersion )

    def marshalSaHpiFumiImpactedEntityT( self, x ):
        self.marshalSaHpiEntityPathT( x.ImpactedEntity )
        self.marshalSaHpiFumiServiceImpactT( x.ServiceImpact )

    def marshalSaHpiFumiServiceImpactDataT( self, x ):
        self.marshalSaHpiUint32T( x.NumEntities )
        for i in range( 0, SAHPI_FUMI_MAX_ENTITIES_IMPACTED ):
            self.marshalSaHpiFumiImpactedEntityT( x.ImpactedEntities[i] )

    def marshalSaHpiFumiSourceInfoT( self, x ):
        self.marshalSaHpiTextBufferT( x.SourceUri )
        self.marshalSaHpiFumiSourceStatusT( x.SourceStatus )
        self.marshalSaHpiTextBufferT( x.Identifier )
        self.marshalSaHpiTextBufferT( x.Description )
        self.marshalSaHpiTextBufferT( x.DateTime )
        self.marshalSaHpiUint32T( x.MajorVersion )
        self.marshalSaHpiUint32T( x.MinorVersion )
        self.marshalSaHpiUint32T( x.AuxVersion )

    def marshalSaHpiFumiComponentInfoT( self, x ):
        self.marshalSaHpiEntryIdT( x.EntryId )
        self.marshalSaHpiUint32T( x.ComponentId )
        self.marshalSaHpiFumiFirmwareInstanceInfoT( x.MainFwInstance )
        self.marshalSaHpiUint32T( x.ComponentFlags )

    def marshalSaHpiFumiBankInfoT( self, x ):
        self.marshalSaHpiUint8T( x.BankId )
        self.marshalSaHpiUint32T( x.BankSize )
        self.marshalSaHpiUint32T( x.Position )
        self.marshalSaHpiFumiBankStateT( x.BankState )
        self.marshalSaHpiTextBufferT( x.Identifier )
        self.marshalSaHpiTextBufferT( x.Description )
        self.marshalSaHpiTextBufferT( x.DateTime )
        self.marshalSaHpiUint32T( x.MajorVersion )
        self.marshalSaHpiUint32T( x.MinorVersion )
        self.marshalSaHpiUint32T( x.AuxVersion )

    def marshalSaHpiFumiLogicalBankInfoT( self, x ):
        self.marshalSaHpiUint8T( x.FirmwarePersistentLocationCount )
        self.marshalSaHpiFumiLogicalBankStateFlagsT( x.BankStateFlags )
        self.marshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance )
        self.marshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance )

    def marshalSaHpiFumiLogicalComponentInfoT( self, x ):
        self.marshalSaHpiEntryIdT( x.EntryId )
        self.marshalSaHpiUint32T( x.ComponentId )
        self.marshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance )
        self.marshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance )
        self.marshalSaHpiUint32T( x.ComponentFlags )

    def marshalSaHpiFumiRecT( self, x ):
        self.marshalSaHpiFumiNumT( x.Num )
        self.marshalSaHpiFumiProtocolT( x.AccessProt )
        self.marshalSaHpiFumiCapabilityT( x.Capability )
        self.marshalSaHpiUint8T( x.NumBanks )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiResourceEventT( self, x ):
        self.marshalSaHpiResourceEventTypeT( x.ResourceEventType )

    def marshalSaHpiDomainEventT( self, x ):
        self.marshalSaHpiDomainEventTypeT( x.Type )
        self.marshalSaHpiDomainIdT( x.DomainId )

    def marshalSaHpiSensorEventT( self, x ):
        self.marshalSaHpiSensorNumT( x.SensorNum )
        self.marshalSaHpiSensorTypeT( x.SensorType )
        self.marshalSaHpiEventCategoryT( x.EventCategory )
        self.marshalSaHpiBoolT( x.Assertion )
        self.marshalSaHpiEventStateT( x.EventState )
        self.marshalSaHpiSensorOptionalDataT( x.OptionalDataPresent )
        self.marshalSaHpiSensorReadingT( x.TriggerReading )
        self.marshalSaHpiSensorReadingT( x.TriggerThreshold )
        self.marshalSaHpiEventStateT( x.PreviousState )
        self.marshalSaHpiEventStateT( x.CurrentState )
        self.marshalSaHpiUint32T( x.Oem )
        self.marshalSaHpiUint32T( x.SensorSpecific )

    def marshalSaHpiSensorEnableChangeEventT( self, x ):
        self.marshalSaHpiSensorNumT( x.SensorNum )
        self.marshalSaHpiSensorTypeT( x.SensorType )
        self.marshalSaHpiEventCategoryT( x.EventCategory )
        self.marshalSaHpiBoolT( x.SensorEnable )
        self.marshalSaHpiBoolT( x.SensorEventEnable )
        self.marshalSaHpiEventStateT( x.AssertEventMask )
        self.marshalSaHpiEventStateT( x.DeassertEventMask )
        self.marshalSaHpiSensorEnableOptDataT( x.OptionalDataPresent )
        self.marshalSaHpiEventStateT( x.CurrentState )
        self.marshalSaHpiEventStateT( x.CriticalAlarms )
        self.marshalSaHpiEventStateT( x.MajorAlarms )
        self.marshalSaHpiEventStateT( x.MinorAlarms )

    def marshalSaHpiHotSwapEventT( self, x ):
        self.marshalSaHpiHsStateT( x.HotSwapState )
        self.marshalSaHpiHsStateT( x.PreviousHotSwapState )
        self.marshalSaHpiHsCauseOfStateChangeT( x.CauseOfStateChange )

    def marshalSaHpiWatchdogEventT( self, x ):
        self.marshalSaHpiWatchdogNumT( x.WatchdogNum )
        self.marshalSaHpiWatchdogActionEventT( x.WatchdogAction )
        self.marshalSaHpiWatchdogPretimerInterruptT( x.WatchdogPreTimerAction )
        self.marshalSaHpiWatchdogTimerUseT( x.WatchdogUse )

    def marshalSaHpiHpiSwEventT( self, x ):
        self.marshalSaHpiManufacturerIdT( x.MId )
        self.marshalSaHpiSwEventTypeT( x.Type )
        self.marshalSaHpiTextBufferT( x.EventData )

    def marshalSaHpiOemEventT( self, x ):
        self.marshalSaHpiManufacturerIdT( x.MId )
        self.marshalSaHpiTextBufferT( x.OemEventData )

    def marshalSaHpiUserEventT( self, x ):
        self.marshalSaHpiTextBufferT( x.UserEventData )

    def marshalSaHpiDimiEventT( self, x ):
        self.marshalSaHpiDimiNumT( x.DimiNum )
        self.marshalSaHpiDimiTestNumT( x.TestNum )
        self.marshalSaHpiDimiTestRunStatusT( x.DimiTestRunStatus )
        self.marshalSaHpiDimiTestPercentCompletedT( x.DimiTestPercentCompleted )

    def marshalSaHpiDimiUpdateEventT( self, x ):
        self.marshalSaHpiDimiNumT( x.DimiNum )

    def marshalSaHpiFumiEventT( self, x ):
        self.marshalSaHpiFumiNumT( x.FumiNum )
        self.marshalSaHpiUint8T( x.BankNum )
        self.marshalSaHpiFumiUpgradeStatusT( x.UpgradeStatus )

    def marshalSaHpiEventUnionT( self, x, mod ):
        if mod == SAHPI_ET_RESOURCE:
            self.marshalSaHpiResourceEventT( x.ResourceEvent )
        if mod == SAHPI_ET_DOMAIN:
            self.marshalSaHpiDomainEventT( x.DomainEvent )
        if mod == SAHPI_ET_SENSOR:
            self.marshalSaHpiSensorEventT( x.SensorEvent )
        if mod == SAHPI_ET_SENSOR_ENABLE_CHANGE:
            self.marshalSaHpiSensorEnableChangeEventT( x.SensorEnableChangeEvent )
        if mod == SAHPI_ET_HOTSWAP:
            self.marshalSaHpiHotSwapEventT( x.HotSwapEvent )
        if mod == SAHPI_ET_WATCHDOG:
            self.marshalSaHpiWatchdogEventT( x.WatchdogEvent )
        if mod == SAHPI_ET_HPI_SW:
            self.marshalSaHpiHpiSwEventT( x.HpiSwEvent )
        if mod == SAHPI_ET_OEM:
            self.marshalSaHpiOemEventT( x.OemEvent )
        if mod == SAHPI_ET_USER:
            self.marshalSaHpiUserEventT( x.UserEvent )
        if mod == SAHPI_ET_DIMI:
            self.marshalSaHpiDimiEventT( x.DimiEvent )
        if mod == SAHPI_ET_DIMI_UPDATE:
            self.marshalSaHpiDimiUpdateEventT( x.DimiUpdateEvent )
        if mod == SAHPI_ET_FUMI:
            self.marshalSaHpiFumiEventT( x.FumiEvent )

    def marshalSaHpiEventT( self, x ):
        self.marshalSaHpiResourceIdT( x.Source )
        self.marshalSaHpiEventTypeT( x.EventType )
        self.marshalSaHpiTimeT( x.Timestamp )
        self.marshalSaHpiSeverityT( x.Severity )
        self.marshalSaHpiEventUnionT( x.EventDataUnion, x.EventType )

    def marshalSaHpiNameT( self, x ):
        self.marshalSaHpiUint16T( x.Length )
        self.marshalByteArray( x.Value, SA_HPI_MAX_NAME_LENGTH )

    def marshalSaHpiConditionT( self, x ):
        self.marshalSaHpiStatusCondTypeT( x.Type )
        self.marshalSaHpiEntityPathT( x.Entity )
        self.marshalSaHpiDomainIdT( x.DomainId )
        self.marshalSaHpiResourceIdT( x.ResourceId )
        self.marshalSaHpiSensorNumT( x.SensorNum )
        self.marshalSaHpiEventStateT( x.EventState )
        self.marshalSaHpiNameT( x.Name )
        self.marshalSaHpiManufacturerIdT( x.Mid )
        self.marshalSaHpiTextBufferT( x.Data )

    def marshalSaHpiAnnouncementT( self, x ):
        self.marshalSaHpiEntryIdT( x.EntryId )
        self.marshalSaHpiTimeT( x.Timestamp )
        self.marshalSaHpiBoolT( x.AddedByUser )
        self.marshalSaHpiSeverityT( x.Severity )
        self.marshalSaHpiBoolT( x.Acknowledged )
        self.marshalSaHpiConditionT( x.StatusCond )

    def marshalSaHpiAnnunciatorRecT( self, x ):
        self.marshalSaHpiAnnunciatorNumT( x.AnnunciatorNum )
        self.marshalSaHpiAnnunciatorTypeT( x.AnnunciatorType )
        self.marshalSaHpiBoolT( x.ModeReadOnly )
        self.marshalSaHpiUint32T( x.MaxConditions )
        self.marshalSaHpiUint32T( x.Oem )

    def marshalSaHpiRdrTypeUnionT( self, x, mod ):
        if mod == SAHPI_CTRL_RDR:
            self.marshalSaHpiCtrlRecT( x.CtrlRec )
        if mod == SAHPI_SENSOR_RDR:
            self.marshalSaHpiSensorRecT( x.SensorRec )
        if mod == SAHPI_INVENTORY_RDR:
            self.marshalSaHpiInventoryRecT( x.InventoryRec )
        if mod == SAHPI_WATCHDOG_RDR:
            self.marshalSaHpiWatchdogRecT( x.WatchdogRec )
        if mod == SAHPI_ANNUNCIATOR_RDR:
            self.marshalSaHpiAnnunciatorRecT( x.AnnunciatorRec )
        if mod == SAHPI_DIMI_RDR:
            self.marshalSaHpiDimiRecT( x.DimiRec )
        if mod == SAHPI_FUMI_RDR:
            self.marshalSaHpiFumiRecT( x.FumiRec )

    def marshalSaHpiRdrT( self, x ):
        self.marshalSaHpiEntryIdT( x.RecordId )
        self.marshalSaHpiRdrTypeT( x.RdrType )
        self.marshalSaHpiEntityPathT( x.Entity )
        self.marshalSaHpiBoolT( x.IsFru )
        self.marshalSaHpiRdrTypeUnionT( x.RdrTypeUnion, x.RdrType )
        self.marshalSaHpiTextBufferT( x.IdString )

    def marshalSaHpiLoadIdT( self, x ):
        self.marshalSaHpiLoadNumberT( x.LoadNumber )
        self.marshalSaHpiTextBufferT( x.LoadName )

    def marshalSaHpiResourceInfoT( self, x ):
        self.marshalSaHpiUint8T( x.ResourceRev )
        self.marshalSaHpiUint8T( x.SpecificVer )
        self.marshalSaHpiUint8T( x.DeviceSupport )
        self.marshalSaHpiManufacturerIdT( x.ManufacturerId )
        self.marshalSaHpiUint16T( x.ProductId )
        self.marshalSaHpiUint8T( x.FirmwareMajorRev )
        self.marshalSaHpiUint8T( x.FirmwareMinorRev )
        self.marshalSaHpiUint8T( x.AuxFirmwareRev )
        self.marshalByteArray( x.Guid, SAHPI_GUID_LENGTH )

    def marshalSaHpiRptEntryT( self, x ):
        self.marshalSaHpiEntryIdT( x.EntryId )
        self.marshalSaHpiResourceIdT( x.ResourceId )
        self.marshalSaHpiResourceInfoT( x.ResourceInfo )
        self.marshalSaHpiEntityPathT( x.ResourceEntity )
        self.marshalSaHpiCapabilitiesT( x.ResourceCapabilities )
        self.marshalSaHpiHsCapabilitiesT( x.HotSwapCapabilities )
        self.marshalSaHpiSeverityT( x.ResourceSeverity )
        self.marshalSaHpiBoolT( x.ResourceFailed )
        self.marshalSaHpiTextBufferT( x.ResourceTag )

    def marshalSaHpiDomainInfoT( self, x ):
        self.marshalSaHpiDomainIdT( x.DomainId )
        self.marshalSaHpiDomainCapabilitiesT( x.DomainCapabilities )
        self.marshalSaHpiBoolT( x.IsPeer )
        self.marshalSaHpiTextBufferT( x.DomainTag )
        self.marshalSaHpiUint32T( x.DrtUpdateCount )
        self.marshalSaHpiTimeT( x.DrtUpdateTimestamp )
        self.marshalSaHpiUint32T( x.RptUpdateCount )
        self.marshalSaHpiTimeT( x.RptUpdateTimestamp )
        self.marshalSaHpiUint32T( x.DatUpdateCount )
        self.marshalSaHpiTimeT( x.DatUpdateTimestamp )
        self.marshalSaHpiUint32T( x.ActiveAlarms )
        self.marshalSaHpiUint32T( x.CriticalAlarms )
        self.marshalSaHpiUint32T( x.MajorAlarms )
        self.marshalSaHpiUint32T( x.MinorAlarms )
        self.marshalSaHpiUint32T( x.DatUserAlarmLimit )
        self.marshalSaHpiBoolT( x.DatOverflow )
        self.marshalByteArray( x.Guid, SAHPI_GUID_LENGTH )

    def marshalSaHpiDrtEntryT( self, x ):
        self.marshalSaHpiEntryIdT( x.EntryId )
        self.marshalSaHpiDomainIdT( x.DomainId )
        self.marshalSaHpiBoolT( x.IsPeer )

    def marshalSaHpiAlarmT( self, x ):
        self.marshalSaHpiAlarmIdT( x.AlarmId )
        self.marshalSaHpiTimeT( x.Timestamp )
        self.marshalSaHpiSeverityT( x.Severity )
        self.marshalSaHpiBoolT( x.Acknowledged )
        self.marshalSaHpiConditionT( x.AlarmCond )

    def marshalSaHpiEventLogInfoT( self, x ):
        self.marshalSaHpiUint32T( x.Entries )
        self.marshalSaHpiUint32T( x.Size )
        self.marshalSaHpiUint32T( x.UserEventMaxSize )
        self.marshalSaHpiTimeT( x.UpdateTimestamp )
        self.marshalSaHpiTimeT( x.CurrentTime )
        self.marshalSaHpiBoolT( x.Enabled )
        self.marshalSaHpiBoolT( x.OverflowFlag )
        self.marshalSaHpiBoolT( x.OverflowResetable )
        self.marshalSaHpiEventLogOverflowActionT( x.OverflowAction )

    def marshalSaHpiEventLogEntryT( self, x ):
        self.marshalSaHpiEventLogEntryIdT( x.EntryId )
        self.marshalSaHpiTimeT( x.Timestamp )
        self.marshalSaHpiEventT( x.Event )

    #**********************************************************
    # Demarshal: For HPI Simple Data Types
    #**********************************************************
    def demarshalSaHpiBoolT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiManufacturerIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiVersionT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaErrorT( self ):
        return self.demarshalSaHpiInt32T()

    def demarshalSaHpiDomainIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiSessionIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiResourceIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiEntryIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiTimeT( self ):
        return self.demarshalSaHpiInt64T()

    def demarshalSaHpiTimeoutT( self ):
        return self.demarshalSaHpiInt64T()

    def demarshalSaHpiInstrumentIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiEntityLocationT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiEventCategoryT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiEventStateT( self ):
        return self.demarshalSaHpiUint16T()

    def demarshalSaHpiSensorNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiSensorRangeFlagsT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiSensorThdMaskT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiCtrlNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiCtrlStateDiscreteT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiCtrlStateAnalogT( self ):
        return self.demarshalSaHpiInt32T()

    def demarshalSaHpiTxtLineNumT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiIdrIdT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiWatchdogNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiWatchdogExpFlagsT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiDimiNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiDimiTestCapabilityT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiDimiTestNumT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiDimiTestPercentCompletedT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiFumiNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiBankNumT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiFumiLogicalBankStateFlagsT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiFumiProtocolT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiFumiCapabilityT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiSensorOptionalDataT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiSensorEnableOptDataT( self ):
        return self.demarshalSaHpiUint8T()

    def demarshalSaHpiEvtQueueStatusT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiAnnunciatorNumT( self ):
        return self.demarshalSaHpiInstrumentIdT()

    def demarshalSaHpiLoadNumberT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiCapabilitiesT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiHsCapabilitiesT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiDomainCapabilitiesT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiAlarmIdT( self ):
        return self.demarshalSaHpiEntryIdT()

    def demarshalSaHpiEventLogCapabilitiesT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiEventLogEntryIdT( self ):
        return self.demarshalSaHpiUint32T()

    def demarshalSaHpiLanguageT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiTextTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiEntityTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorReadingTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorEventMaskActionT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorUnitsT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorModUnitUseT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSensorEventCtrlT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiCtrlTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiCtrlStateDigitalT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiCtrlModeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiCtrlOutputTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiIdrAreaTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiIdrFieldTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiWatchdogActionT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiWatchdogActionEventT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiWatchdogPretimerInterruptT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiWatchdogTimerUseT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDimiTestServiceImpactT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDimiTestRunStatusT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDimiTestErrCodeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDimiTestParamTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDimiReadyT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiSpecInfoTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiSafDefinedSpecIdT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiServiceImpactT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiSourceStatusT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiBankStateT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiFumiUpgradeStatusT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiHsIndicatorStateT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiHsActionT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiHsStateT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiHsCauseOfStateChangeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSeverityT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiResourceEventTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiDomainEventTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiSwEventTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiEventTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiStatusCondTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiAnnunciatorModeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiAnnunciatorTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiRdrTypeT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiParmActionT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiResetActionT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiPowerStateT( self ):
        return self.demarshalEnum()

    def demarshalSaHpiEventLogOverflowActionT( self ):
        return self.demarshalEnum()


    #**********************************************************
    # Demarshal: For HPI Structs and Unions
    #**********************************************************
    def demarshalSaHpiTextBufferT( self ):
        x = SaHpiTextBufferT()
        x.DataType = self.demarshalSaHpiTextTypeT()
        x.Language = self.demarshalSaHpiLanguageT()
        x.DataLength = self.demarshalSaHpiUint8T()
        x.Data = self.demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH )
        return x

    def demarshalSaHpiEntityT( self ):
        x = SaHpiEntityT()
        x.EntityType = self.demarshalSaHpiEntityTypeT()
        x.EntityLocation = self.demarshalSaHpiEntityLocationT()
        return x

    def demarshalSaHpiEntityPathT( self ):
        x = SaHpiEntityPathT()
        x.Entry = []
        for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
            x.Entry.append( self.demarshalSaHpiEntityT() )
        return x

    def demarshalSaHpiSensorReadingUnionT( self, mod ):
        x = SaHpiSensorReadingUnionT()
        if mod == SAHPI_SENSOR_READING_TYPE_INT64:
            x.SensorInt64 = self.demarshalSaHpiInt64T()
        if mod == SAHPI_SENSOR_READING_TYPE_UINT64:
            x.SensorUint64 = self.demarshalSaHpiUint64T()
        if mod == SAHPI_SENSOR_READING_TYPE_FLOAT64:
            x.SensorFloat64 = self.demarshalSaHpiFloat64T()
        if mod == SAHPI_SENSOR_READING_TYPE_BUFFER:
            x.SensorBuffer = self.demarshalByteArray( SAHPI_SENSOR_BUFFER_LENGTH )
        return x

    def demarshalSaHpiSensorReadingT( self ):
        x = SaHpiSensorReadingT()
        x.IsSupported = self.demarshalSaHpiBoolT()
        x.Type = self.demarshalSaHpiSensorReadingTypeT()
        x.Value = self.demarshalSaHpiSensorReadingUnionT( x.Type )
        return x

    def demarshalSaHpiSensorThresholdsT( self ):
        x = SaHpiSensorThresholdsT()
        x.LowCritical = self.demarshalSaHpiSensorReadingT()
        x.LowMajor = self.demarshalSaHpiSensorReadingT()
        x.LowMinor = self.demarshalSaHpiSensorReadingT()
        x.UpCritical = self.demarshalSaHpiSensorReadingT()
        x.UpMajor = self.demarshalSaHpiSensorReadingT()
        x.UpMinor = self.demarshalSaHpiSensorReadingT()
        x.PosThdHysteresis = self.demarshalSaHpiSensorReadingT()
        x.NegThdHysteresis = self.demarshalSaHpiSensorReadingT()
        return x

    def demarshalSaHpiSensorRangeT( self ):
        x = SaHpiSensorRangeT()
        x.Flags = self.demarshalSaHpiSensorRangeFlagsT()
        x.Max = self.demarshalSaHpiSensorReadingT()
        x.Min = self.demarshalSaHpiSensorReadingT()
        x.Nominal = self.demarshalSaHpiSensorReadingT()
        x.NormalMax = self.demarshalSaHpiSensorReadingT()
        x.NormalMin = self.demarshalSaHpiSensorReadingT()
        return x

    def demarshalSaHpiSensorDataFormatT( self ):
        x = SaHpiSensorDataFormatT()
        x.IsSupported = self.demarshalSaHpiBoolT()
        x.ReadingType = self.demarshalSaHpiSensorReadingTypeT()
        x.BaseUnits = self.demarshalSaHpiSensorUnitsT()
        x.ModifierUnits = self.demarshalSaHpiSensorUnitsT()
        x.ModifierUse = self.demarshalSaHpiSensorModUnitUseT()
        x.Percentage = self.demarshalSaHpiBoolT()
        x.Range = self.demarshalSaHpiSensorRangeT()
        x.AccuracyFactor = self.demarshalSaHpiFloat64T()
        return x

    def demarshalSaHpiSensorThdDefnT( self ):
        x = SaHpiSensorThdDefnT()
        x.IsAccessible = self.demarshalSaHpiBoolT()
        x.ReadThold = self.demarshalSaHpiSensorThdMaskT()
        x.WriteThold = self.demarshalSaHpiSensorThdMaskT()
        x.Nonlinear = self.demarshalSaHpiBoolT()
        return x

    def demarshalSaHpiSensorRecT( self ):
        x = SaHpiSensorRecT()
        x.Num = self.demarshalSaHpiSensorNumT()
        x.Type = self.demarshalSaHpiSensorTypeT()
        x.Category = self.demarshalSaHpiEventCategoryT()
        x.EnableCtrl = self.demarshalSaHpiBoolT()
        x.EventCtrl = self.demarshalSaHpiSensorEventCtrlT()
        x.Events = self.demarshalSaHpiEventStateT()
        x.DataFormat = self.demarshalSaHpiSensorDataFormatT()
        x.ThresholdDefn = self.demarshalSaHpiSensorThdDefnT()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiCtrlStateStreamT( self ):
        x = SaHpiCtrlStateStreamT()
        x.Repeat = self.demarshalSaHpiBoolT()
        x.StreamLength = self.demarshalSaHpiUint32T()
        x.Stream = self.demarshalByteArray( SAHPI_CTRL_MAX_STREAM_LENGTH )
        return x

    def demarshalSaHpiCtrlStateTextT( self ):
        x = SaHpiCtrlStateTextT()
        x.Line = self.demarshalSaHpiTxtLineNumT()
        x.Text = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiCtrlStateOemT( self ):
        x = SaHpiCtrlStateOemT()
        x.MId = self.demarshalSaHpiManufacturerIdT()
        x.BodyLength = self.demarshalSaHpiUint8T()
        x.Body = self.demarshalByteArray( SAHPI_CTRL_MAX_OEM_BODY_LENGTH )
        return x

    def demarshalSaHpiCtrlStateUnionT( self, mod ):
        x = SaHpiCtrlStateUnionT()
        if mod == SAHPI_CTRL_TYPE_DIGITAL:
            x.Digital = self.demarshalSaHpiCtrlStateDigitalT()
        if mod == SAHPI_CTRL_TYPE_DISCRETE:
            x.Discrete = self.demarshalSaHpiCtrlStateDiscreteT()
        if mod == SAHPI_CTRL_TYPE_ANALOG:
            x.Analog = self.demarshalSaHpiCtrlStateAnalogT()
        if mod == SAHPI_CTRL_TYPE_STREAM:
            x.Stream = self.demarshalSaHpiCtrlStateStreamT()
        if mod == SAHPI_CTRL_TYPE_TEXT:
            x.Text = self.demarshalSaHpiCtrlStateTextT()
        if mod == SAHPI_CTRL_TYPE_OEM:
            x.Oem = self.demarshalSaHpiCtrlStateOemT()
        return x

    def demarshalSaHpiCtrlStateT( self ):
        x = SaHpiCtrlStateT()
        x.Type = self.demarshalSaHpiCtrlTypeT()
        x.StateUnion = self.demarshalSaHpiCtrlStateUnionT( x.Type )
        return x

    def demarshalSaHpiCtrlRecDigitalT( self ):
        x = SaHpiCtrlRecDigitalT()
        x.Default = self.demarshalSaHpiCtrlStateDigitalT()
        return x

    def demarshalSaHpiCtrlRecDiscreteT( self ):
        x = SaHpiCtrlRecDiscreteT()
        x.Default = self.demarshalSaHpiCtrlStateDiscreteT()
        return x

    def demarshalSaHpiCtrlRecAnalogT( self ):
        x = SaHpiCtrlRecAnalogT()
        x.Min = self.demarshalSaHpiCtrlStateAnalogT()
        x.Max = self.demarshalSaHpiCtrlStateAnalogT()
        x.Default = self.demarshalSaHpiCtrlStateAnalogT()
        return x

    def demarshalSaHpiCtrlRecStreamT( self ):
        x = SaHpiCtrlRecStreamT()
        x.Default = self.demarshalSaHpiCtrlStateStreamT()
        return x

    def demarshalSaHpiCtrlRecTextT( self ):
        x = SaHpiCtrlRecTextT()
        x.MaxChars = self.demarshalSaHpiUint8T()
        x.MaxLines = self.demarshalSaHpiUint8T()
        x.Language = self.demarshalSaHpiLanguageT()
        x.DataType = self.demarshalSaHpiTextTypeT()
        x.Default = self.demarshalSaHpiCtrlStateTextT()
        return x

    def demarshalSaHpiCtrlRecOemT( self ):
        x = SaHpiCtrlRecOemT()
        x.MId = self.demarshalSaHpiManufacturerIdT()
        x.ConfigData = self.demarshalByteArray( SAHPI_CTRL_OEM_CONFIG_LENGTH )
        x.Default = self.demarshalSaHpiCtrlStateOemT()
        return x

    def demarshalSaHpiCtrlRecUnionT( self, mod ):
        x = SaHpiCtrlRecUnionT()
        if mod == SAHPI_CTRL_TYPE_DIGITAL:
            x.Digital = self.demarshalSaHpiCtrlRecDigitalT()
        if mod == SAHPI_CTRL_TYPE_DISCRETE:
            x.Discrete = self.demarshalSaHpiCtrlRecDiscreteT()
        if mod == SAHPI_CTRL_TYPE_ANALOG:
            x.Analog = self.demarshalSaHpiCtrlRecAnalogT()
        if mod == SAHPI_CTRL_TYPE_STREAM:
            x.Stream = self.demarshalSaHpiCtrlRecStreamT()
        if mod == SAHPI_CTRL_TYPE_TEXT:
            x.Text = self.demarshalSaHpiCtrlRecTextT()
        if mod == SAHPI_CTRL_TYPE_OEM:
            x.Oem = self.demarshalSaHpiCtrlRecOemT()
        return x

    def demarshalSaHpiCtrlDefaultModeT( self ):
        x = SaHpiCtrlDefaultModeT()
        x.Mode = self.demarshalSaHpiCtrlModeT()
        x.ReadOnly = self.demarshalSaHpiBoolT()
        return x

    def demarshalSaHpiCtrlRecT( self ):
        x = SaHpiCtrlRecT()
        x.Num = self.demarshalSaHpiCtrlNumT()
        x.OutputType = self.demarshalSaHpiCtrlOutputTypeT()
        x.Type = self.demarshalSaHpiCtrlTypeT()
        x.TypeUnion = self.demarshalSaHpiCtrlRecUnionT( x.Type )
        x.DefaultMode = self.demarshalSaHpiCtrlDefaultModeT()
        x.WriteOnly = self.demarshalSaHpiBoolT()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiIdrFieldT( self ):
        x = SaHpiIdrFieldT()
        x.AreaId = self.demarshalSaHpiEntryIdT()
        x.FieldId = self.demarshalSaHpiEntryIdT()
        x.Type = self.demarshalSaHpiIdrFieldTypeT()
        x.ReadOnly = self.demarshalSaHpiBoolT()
        x.Field = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiIdrAreaHeaderT( self ):
        x = SaHpiIdrAreaHeaderT()
        x.AreaId = self.demarshalSaHpiEntryIdT()
        x.Type = self.demarshalSaHpiIdrAreaTypeT()
        x.ReadOnly = self.demarshalSaHpiBoolT()
        x.NumFields = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiIdrInfoT( self ):
        x = SaHpiIdrInfoT()
        x.IdrId = self.demarshalSaHpiIdrIdT()
        x.UpdateCount = self.demarshalSaHpiUint32T()
        x.ReadOnly = self.demarshalSaHpiBoolT()
        x.NumAreas = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiInventoryRecT( self ):
        x = SaHpiInventoryRecT()
        x.IdrId = self.demarshalSaHpiIdrIdT()
        x.Persistent = self.demarshalSaHpiBoolT()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiWatchdogT( self ):
        x = SaHpiWatchdogT()
        x.Log = self.demarshalSaHpiBoolT()
        x.Running = self.demarshalSaHpiBoolT()
        x.TimerUse = self.demarshalSaHpiWatchdogTimerUseT()
        x.TimerAction = self.demarshalSaHpiWatchdogActionT()
        x.PretimerInterrupt = self.demarshalSaHpiWatchdogPretimerInterruptT()
        x.PreTimeoutInterval = self.demarshalSaHpiUint32T()
        x.TimerUseExpFlags = self.demarshalSaHpiWatchdogExpFlagsT()
        x.InitialCount = self.demarshalSaHpiUint32T()
        x.PresentCount = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiWatchdogRecT( self ):
        x = SaHpiWatchdogRecT()
        x.WatchdogNum = self.demarshalSaHpiWatchdogNumT()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiDimiTestAffectedEntityT( self ):
        x = SaHpiDimiTestAffectedEntityT()
        x.EntityImpacted = self.demarshalSaHpiEntityPathT()
        x.ServiceImpact = self.demarshalSaHpiDimiTestServiceImpactT()
        return x

    def demarshalSaHpiDimiTestResultsT( self ):
        x = SaHpiDimiTestResultsT()
        x.ResultTimeStamp = self.demarshalSaHpiTimeT()
        x.RunDuration = self.demarshalSaHpiTimeoutT()
        x.LastRunStatus = self.demarshalSaHpiDimiTestRunStatusT()
        x.TestErrorCode = self.demarshalSaHpiDimiTestErrCodeT()
        x.TestResultString = self.demarshalSaHpiTextBufferT()
        x.TestResultStringIsURI = self.demarshalSaHpiBoolT()
        return x

    def demarshalSaHpiDimiTestParamValueT( self, mod ):
        x = SaHpiDimiTestParamValueT()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
            x.paramint = self.demarshalSaHpiInt32T()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
            x.parambool = self.demarshalSaHpiBoolT()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
            x.paramfloat = self.demarshalSaHpiFloat64T()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
            x.paramtext = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiDimiTestParameterValueUnionT( self, mod ):
        x = SaHpiDimiTestParameterValueUnionT()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
            x.IntValue = self.demarshalSaHpiInt32T()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
            x.FloatValue = self.demarshalSaHpiFloat64T()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
            x.FloatValue = self.demarshalSaHpiFloat64T()
        if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
            x.FloatValue = self.demarshalSaHpiFloat64T()
        return x

    def demarshalSaHpiDimiTestParamsDefinitionT( self ):
        x = SaHpiDimiTestParamsDefinitionT()
        x.ParamName = self.demarshalByteArray( SAHPI_DIMITEST_PARAM_NAME_LEN )
        x.ParamInfo = self.demarshalSaHpiTextBufferT()
        x.ParamType = self.demarshalSaHpiDimiTestParamTypeT()
        x.MinValue = self.demarshalSaHpiDimiTestParameterValueUnionT( x.ParamType )
        x.MaxValue = self.demarshalSaHpiDimiTestParameterValueUnionT( x.ParamType )
        x.DefaultParam = self.demarshalSaHpiDimiTestParamValueT( x.ParamType )
        return x

    def demarshalSaHpiDimiTestT( self ):
        x = SaHpiDimiTestT()
        x.TestName = self.demarshalSaHpiTextBufferT()
        x.ServiceImpact = self.demarshalSaHpiDimiTestServiceImpactT()
        x.EntitiesImpacted = []
        for i in range( 0, SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ):
            x.EntitiesImpacted.append( self.demarshalSaHpiDimiTestAffectedEntityT() )
        x.NeedServiceOS = self.demarshalSaHpiBoolT()
        x.ServiceOS = self.demarshalSaHpiTextBufferT()
        x.ExpectedRunDuration = self.demarshalSaHpiTimeT()
        x.TestCapabilities = self.demarshalSaHpiDimiTestCapabilityT()
        x.TestParameters = []
        for i in range( 0, SAHPI_DIMITEST_MAX_PARAMETERS ):
            x.TestParameters.append( self.demarshalSaHpiDimiTestParamsDefinitionT() )
        return x

    def demarshalSaHpiDimiTestVariableParamsT( self ):
        x = SaHpiDimiTestVariableParamsT()
        x.ParamName = self.demarshalByteArray( SAHPI_DIMITEST_PARAM_NAME_LEN )
        x.ParamType = self.demarshalSaHpiDimiTestParamTypeT()
        x.Value = self.demarshalSaHpiDimiTestParamValueT( x.ParamType )
        return x

    def demarshalSaHpiDimiInfoT( self ):
        x = SaHpiDimiInfoT()
        x.NumberOfTests = self.demarshalSaHpiUint32T()
        x.TestNumUpdateCounter = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiDimiRecT( self ):
        x = SaHpiDimiRecT()
        x.DimiNum = self.demarshalSaHpiDimiNumT()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiSafDefinedSpecInfoT( self ):
        x = SaHpiFumiSafDefinedSpecInfoT()
        x.SpecID = self.demarshalSaHpiFumiSafDefinedSpecIdT()
        x.RevisionID = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiOemDefinedSpecInfoT( self ):
        x = SaHpiFumiOemDefinedSpecInfoT()
        x.Mid = self.demarshalSaHpiManufacturerIdT()
        x.BodyLength = self.demarshalSaHpiUint8T()
        x.Body = self.demarshalByteArray( SAHPI_FUMI_MAX_OEM_BODY_LENGTH )
        return x

    def demarshalSaHpiFumiSpecInfoTypeUnionT( self, mod ):
        x = SaHpiFumiSpecInfoTypeUnionT()
        if mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
            x.SafDefined = self.demarshalSaHpiFumiSafDefinedSpecInfoT()
        if mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
            x.OemDefined = self.demarshalSaHpiFumiOemDefinedSpecInfoT()
        return x

    def demarshalSaHpiFumiSpecInfoT( self ):
        x = SaHpiFumiSpecInfoT()
        x.SpecInfoType = self.demarshalSaHpiFumiSpecInfoTypeT()
        x.SpecInfoTypeUnion = self.demarshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoType )
        return x

    def demarshalSaHpiFumiFirmwareInstanceInfoT( self ):
        x = SaHpiFumiFirmwareInstanceInfoT()
        x.InstancePresent = self.demarshalSaHpiBoolT()
        x.Identifier = self.demarshalSaHpiTextBufferT()
        x.Description = self.demarshalSaHpiTextBufferT()
        x.DateTime = self.demarshalSaHpiTextBufferT()
        x.MajorVersion = self.demarshalSaHpiUint32T()
        x.MinorVersion = self.demarshalSaHpiUint32T()
        x.AuxVersion = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiImpactedEntityT( self ):
        x = SaHpiFumiImpactedEntityT()
        x.ImpactedEntity = self.demarshalSaHpiEntityPathT()
        x.ServiceImpact = self.demarshalSaHpiFumiServiceImpactT()
        return x

    def demarshalSaHpiFumiServiceImpactDataT( self ):
        x = SaHpiFumiServiceImpactDataT()
        x.NumEntities = self.demarshalSaHpiUint32T()
        x.ImpactedEntities = []
        for i in range( 0, SAHPI_FUMI_MAX_ENTITIES_IMPACTED ):
            x.ImpactedEntities.append( self.demarshalSaHpiFumiImpactedEntityT() )
        return x

    def demarshalSaHpiFumiSourceInfoT( self ):
        x = SaHpiFumiSourceInfoT()
        x.SourceUri = self.demarshalSaHpiTextBufferT()
        x.SourceStatus = self.demarshalSaHpiFumiSourceStatusT()
        x.Identifier = self.demarshalSaHpiTextBufferT()
        x.Description = self.demarshalSaHpiTextBufferT()
        x.DateTime = self.demarshalSaHpiTextBufferT()
        x.MajorVersion = self.demarshalSaHpiUint32T()
        x.MinorVersion = self.demarshalSaHpiUint32T()
        x.AuxVersion = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiComponentInfoT( self ):
        x = SaHpiFumiComponentInfoT()
        x.EntryId = self.demarshalSaHpiEntryIdT()
        x.ComponentId = self.demarshalSaHpiUint32T()
        x.MainFwInstance = self.demarshalSaHpiFumiFirmwareInstanceInfoT()
        x.ComponentFlags = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiBankInfoT( self ):
        x = SaHpiFumiBankInfoT()
        x.BankId = self.demarshalSaHpiUint8T()
        x.BankSize = self.demarshalSaHpiUint32T()
        x.Position = self.demarshalSaHpiUint32T()
        x.BankState = self.demarshalSaHpiFumiBankStateT()
        x.Identifier = self.demarshalSaHpiTextBufferT()
        x.Description = self.demarshalSaHpiTextBufferT()
        x.DateTime = self.demarshalSaHpiTextBufferT()
        x.MajorVersion = self.demarshalSaHpiUint32T()
        x.MinorVersion = self.demarshalSaHpiUint32T()
        x.AuxVersion = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiLogicalBankInfoT( self ):
        x = SaHpiFumiLogicalBankInfoT()
        x.FirmwarePersistentLocationCount = self.demarshalSaHpiUint8T()
        x.BankStateFlags = self.demarshalSaHpiFumiLogicalBankStateFlagsT()
        x.PendingFwInstance = self.demarshalSaHpiFumiFirmwareInstanceInfoT()
        x.RollbackFwInstance = self.demarshalSaHpiFumiFirmwareInstanceInfoT()
        return x

    def demarshalSaHpiFumiLogicalComponentInfoT( self ):
        x = SaHpiFumiLogicalComponentInfoT()
        x.EntryId = self.demarshalSaHpiEntryIdT()
        x.ComponentId = self.demarshalSaHpiUint32T()
        x.PendingFwInstance = self.demarshalSaHpiFumiFirmwareInstanceInfoT()
        x.RollbackFwInstance = self.demarshalSaHpiFumiFirmwareInstanceInfoT()
        x.ComponentFlags = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiFumiRecT( self ):
        x = SaHpiFumiRecT()
        x.Num = self.demarshalSaHpiFumiNumT()
        x.AccessProt = self.demarshalSaHpiFumiProtocolT()
        x.Capability = self.demarshalSaHpiFumiCapabilityT()
        x.NumBanks = self.demarshalSaHpiUint8T()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiResourceEventT( self ):
        x = SaHpiResourceEventT()
        x.ResourceEventType = self.demarshalSaHpiResourceEventTypeT()
        return x

    def demarshalSaHpiDomainEventT( self ):
        x = SaHpiDomainEventT()
        x.Type = self.demarshalSaHpiDomainEventTypeT()
        x.DomainId = self.demarshalSaHpiDomainIdT()
        return x

    def demarshalSaHpiSensorEventT( self ):
        x = SaHpiSensorEventT()
        x.SensorNum = self.demarshalSaHpiSensorNumT()
        x.SensorType = self.demarshalSaHpiSensorTypeT()
        x.EventCategory = self.demarshalSaHpiEventCategoryT()
        x.Assertion = self.demarshalSaHpiBoolT()
        x.EventState = self.demarshalSaHpiEventStateT()
        x.OptionalDataPresent = self.demarshalSaHpiSensorOptionalDataT()
        x.TriggerReading = self.demarshalSaHpiSensorReadingT()
        x.TriggerThreshold = self.demarshalSaHpiSensorReadingT()
        x.PreviousState = self.demarshalSaHpiEventStateT()
        x.CurrentState = self.demarshalSaHpiEventStateT()
        x.Oem = self.demarshalSaHpiUint32T()
        x.SensorSpecific = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiSensorEnableChangeEventT( self ):
        x = SaHpiSensorEnableChangeEventT()
        x.SensorNum = self.demarshalSaHpiSensorNumT()
        x.SensorType = self.demarshalSaHpiSensorTypeT()
        x.EventCategory = self.demarshalSaHpiEventCategoryT()
        x.SensorEnable = self.demarshalSaHpiBoolT()
        x.SensorEventEnable = self.demarshalSaHpiBoolT()
        x.AssertEventMask = self.demarshalSaHpiEventStateT()
        x.DeassertEventMask = self.demarshalSaHpiEventStateT()
        x.OptionalDataPresent = self.demarshalSaHpiSensorEnableOptDataT()
        x.CurrentState = self.demarshalSaHpiEventStateT()
        x.CriticalAlarms = self.demarshalSaHpiEventStateT()
        x.MajorAlarms = self.demarshalSaHpiEventStateT()
        x.MinorAlarms = self.demarshalSaHpiEventStateT()
        return x

    def demarshalSaHpiHotSwapEventT( self ):
        x = SaHpiHotSwapEventT()
        x.HotSwapState = self.demarshalSaHpiHsStateT()
        x.PreviousHotSwapState = self.demarshalSaHpiHsStateT()
        x.CauseOfStateChange = self.demarshalSaHpiHsCauseOfStateChangeT()
        return x

    def demarshalSaHpiWatchdogEventT( self ):
        x = SaHpiWatchdogEventT()
        x.WatchdogNum = self.demarshalSaHpiWatchdogNumT()
        x.WatchdogAction = self.demarshalSaHpiWatchdogActionEventT()
        x.WatchdogPreTimerAction = self.demarshalSaHpiWatchdogPretimerInterruptT()
        x.WatchdogUse = self.demarshalSaHpiWatchdogTimerUseT()
        return x

    def demarshalSaHpiHpiSwEventT( self ):
        x = SaHpiHpiSwEventT()
        x.MId = self.demarshalSaHpiManufacturerIdT()
        x.Type = self.demarshalSaHpiSwEventTypeT()
        x.EventData = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiOemEventT( self ):
        x = SaHpiOemEventT()
        x.MId = self.demarshalSaHpiManufacturerIdT()
        x.OemEventData = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiUserEventT( self ):
        x = SaHpiUserEventT()
        x.UserEventData = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiDimiEventT( self ):
        x = SaHpiDimiEventT()
        x.DimiNum = self.demarshalSaHpiDimiNumT()
        x.TestNum = self.demarshalSaHpiDimiTestNumT()
        x.DimiTestRunStatus = self.demarshalSaHpiDimiTestRunStatusT()
        x.DimiTestPercentCompleted = self.demarshalSaHpiDimiTestPercentCompletedT()
        return x

    def demarshalSaHpiDimiUpdateEventT( self ):
        x = SaHpiDimiUpdateEventT()
        x.DimiNum = self.demarshalSaHpiDimiNumT()
        return x

    def demarshalSaHpiFumiEventT( self ):
        x = SaHpiFumiEventT()
        x.FumiNum = self.demarshalSaHpiFumiNumT()
        x.BankNum = self.demarshalSaHpiUint8T()
        x.UpgradeStatus = self.demarshalSaHpiFumiUpgradeStatusT()
        return x

    def demarshalSaHpiEventUnionT( self, mod ):
        x = SaHpiEventUnionT()
        if mod == SAHPI_ET_RESOURCE:
            x.ResourceEvent = self.demarshalSaHpiResourceEventT()
        if mod == SAHPI_ET_DOMAIN:
            x.DomainEvent = self.demarshalSaHpiDomainEventT()
        if mod == SAHPI_ET_SENSOR:
            x.SensorEvent = self.demarshalSaHpiSensorEventT()
        if mod == SAHPI_ET_SENSOR_ENABLE_CHANGE:
            x.SensorEnableChangeEvent = self.demarshalSaHpiSensorEnableChangeEventT()
        if mod == SAHPI_ET_HOTSWAP:
            x.HotSwapEvent = self.demarshalSaHpiHotSwapEventT()
        if mod == SAHPI_ET_WATCHDOG:
            x.WatchdogEvent = self.demarshalSaHpiWatchdogEventT()
        if mod == SAHPI_ET_HPI_SW:
            x.HpiSwEvent = self.demarshalSaHpiHpiSwEventT()
        if mod == SAHPI_ET_OEM:
            x.OemEvent = self.demarshalSaHpiOemEventT()
        if mod == SAHPI_ET_USER:
            x.UserEvent = self.demarshalSaHpiUserEventT()
        if mod == SAHPI_ET_DIMI:
            x.DimiEvent = self.demarshalSaHpiDimiEventT()
        if mod == SAHPI_ET_DIMI_UPDATE:
            x.DimiUpdateEvent = self.demarshalSaHpiDimiUpdateEventT()
        if mod == SAHPI_ET_FUMI:
            x.FumiEvent = self.demarshalSaHpiFumiEventT()
        return x

    def demarshalSaHpiEventT( self ):
        x = SaHpiEventT()
        x.Source = self.demarshalSaHpiResourceIdT()
        x.EventType = self.demarshalSaHpiEventTypeT()
        x.Timestamp = self.demarshalSaHpiTimeT()
        x.Severity = self.demarshalSaHpiSeverityT()
        x.EventDataUnion = self.demarshalSaHpiEventUnionT( x.EventType )
        return x

    def demarshalSaHpiNameT( self ):
        x = SaHpiNameT()
        x.Length = self.demarshalSaHpiUint16T()
        x.Value = self.demarshalByteArray( SA_HPI_MAX_NAME_LENGTH )
        return x

    def demarshalSaHpiConditionT( self ):
        x = SaHpiConditionT()
        x.Type = self.demarshalSaHpiStatusCondTypeT()
        x.Entity = self.demarshalSaHpiEntityPathT()
        x.DomainId = self.demarshalSaHpiDomainIdT()
        x.ResourceId = self.demarshalSaHpiResourceIdT()
        x.SensorNum = self.demarshalSaHpiSensorNumT()
        x.EventState = self.demarshalSaHpiEventStateT()
        x.Name = self.demarshalSaHpiNameT()
        x.Mid = self.demarshalSaHpiManufacturerIdT()
        x.Data = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiAnnouncementT( self ):
        x = SaHpiAnnouncementT()
        x.EntryId = self.demarshalSaHpiEntryIdT()
        x.Timestamp = self.demarshalSaHpiTimeT()
        x.AddedByUser = self.demarshalSaHpiBoolT()
        x.Severity = self.demarshalSaHpiSeverityT()
        x.Acknowledged = self.demarshalSaHpiBoolT()
        x.StatusCond = self.demarshalSaHpiConditionT()
        return x

    def demarshalSaHpiAnnunciatorRecT( self ):
        x = SaHpiAnnunciatorRecT()
        x.AnnunciatorNum = self.demarshalSaHpiAnnunciatorNumT()
        x.AnnunciatorType = self.demarshalSaHpiAnnunciatorTypeT()
        x.ModeReadOnly = self.demarshalSaHpiBoolT()
        x.MaxConditions = self.demarshalSaHpiUint32T()
        x.Oem = self.demarshalSaHpiUint32T()
        return x

    def demarshalSaHpiRdrTypeUnionT( self, mod ):
        x = SaHpiRdrTypeUnionT()
        if mod == SAHPI_CTRL_RDR:
            x.CtrlRec = self.demarshalSaHpiCtrlRecT()
        if mod == SAHPI_SENSOR_RDR:
            x.SensorRec = self.demarshalSaHpiSensorRecT()
        if mod == SAHPI_INVENTORY_RDR:
            x.InventoryRec = self.demarshalSaHpiInventoryRecT()
        if mod == SAHPI_WATCHDOG_RDR:
            x.WatchdogRec = self.demarshalSaHpiWatchdogRecT()
        if mod == SAHPI_ANNUNCIATOR_RDR:
            x.AnnunciatorRec = self.demarshalSaHpiAnnunciatorRecT()
        if mod == SAHPI_DIMI_RDR:
            x.DimiRec = self.demarshalSaHpiDimiRecT()
        if mod == SAHPI_FUMI_RDR:
            x.FumiRec = self.demarshalSaHpiFumiRecT()
        return x

    def demarshalSaHpiRdrT( self ):
        x = SaHpiRdrT()
        x.RecordId = self.demarshalSaHpiEntryIdT()
        x.RdrType = self.demarshalSaHpiRdrTypeT()
        x.Entity = self.demarshalSaHpiEntityPathT()
        x.IsFru = self.demarshalSaHpiBoolT()
        x.RdrTypeUnion = self.demarshalSaHpiRdrTypeUnionT( x.RdrType )
        x.IdString = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiLoadIdT( self ):
        x = SaHpiLoadIdT()
        x.LoadNumber = self.demarshalSaHpiLoadNumberT()
        x.LoadName = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiResourceInfoT( self ):
        x = SaHpiResourceInfoT()
        x.ResourceRev = self.demarshalSaHpiUint8T()
        x.SpecificVer = self.demarshalSaHpiUint8T()
        x.DeviceSupport = self.demarshalSaHpiUint8T()
        x.ManufacturerId = self.demarshalSaHpiManufacturerIdT()
        x.ProductId = self.demarshalSaHpiUint16T()
        x.FirmwareMajorRev = self.demarshalSaHpiUint8T()
        x.FirmwareMinorRev = self.demarshalSaHpiUint8T()
        x.AuxFirmwareRev = self.demarshalSaHpiUint8T()
        x.Guid = self.demarshalByteArray( SAHPI_GUID_LENGTH )
        return x

    def demarshalSaHpiRptEntryT( self ):
        x = SaHpiRptEntryT()
        x.EntryId = self.demarshalSaHpiEntryIdT()
        x.ResourceId = self.demarshalSaHpiResourceIdT()
        x.ResourceInfo = self.demarshalSaHpiResourceInfoT()
        x.ResourceEntity = self.demarshalSaHpiEntityPathT()
        x.ResourceCapabilities = self.demarshalSaHpiCapabilitiesT()
        x.HotSwapCapabilities = self.demarshalSaHpiHsCapabilitiesT()
        x.ResourceSeverity = self.demarshalSaHpiSeverityT()
        x.ResourceFailed = self.demarshalSaHpiBoolT()
        x.ResourceTag = self.demarshalSaHpiTextBufferT()
        return x

    def demarshalSaHpiDomainInfoT( self ):
        x = SaHpiDomainInfoT()
        x.DomainId = self.demarshalSaHpiDomainIdT()
        x.DomainCapabilities = self.demarshalSaHpiDomainCapabilitiesT()
        x.IsPeer = self.demarshalSaHpiBoolT()
        x.DomainTag = self.demarshalSaHpiTextBufferT()
        x.DrtUpdateCount = self.demarshalSaHpiUint32T()
        x.DrtUpdateTimestamp = self.demarshalSaHpiTimeT()
        x.RptUpdateCount = self.demarshalSaHpiUint32T()
        x.RptUpdateTimestamp = self.demarshalSaHpiTimeT()
        x.DatUpdateCount = self.demarshalSaHpiUint32T()
        x.DatUpdateTimestamp = self.demarshalSaHpiTimeT()
        x.ActiveAlarms = self.demarshalSaHpiUint32T()
        x.CriticalAlarms = self.demarshalSaHpiUint32T()
        x.MajorAlarms = self.demarshalSaHpiUint32T()
        x.MinorAlarms = self.demarshalSaHpiUint32T()
        x.DatUserAlarmLimit = self.demarshalSaHpiUint32T()
        x.DatOverflow = self.demarshalSaHpiBoolT()
        x.Guid = self.demarshalByteArray( SAHPI_GUID_LENGTH )
        return x

    def demarshalSaHpiDrtEntryT( self ):
        x = SaHpiDrtEntryT()
        x.EntryId = self.demarshalSaHpiEntryIdT()
        x.DomainId = self.demarshalSaHpiDomainIdT()
        x.IsPeer = self.demarshalSaHpiBoolT()
        return x

    def demarshalSaHpiAlarmT( self ):
        x = SaHpiAlarmT()
        x.AlarmId = self.demarshalSaHpiAlarmIdT()
        x.Timestamp = self.demarshalSaHpiTimeT()
        x.Severity = self.demarshalSaHpiSeverityT()
        x.Acknowledged = self.demarshalSaHpiBoolT()
        x.AlarmCond = self.demarshalSaHpiConditionT()
        return x

    def demarshalSaHpiEventLogInfoT( self ):
        x = SaHpiEventLogInfoT()
        x.Entries = self.demarshalSaHpiUint32T()
        x.Size = self.demarshalSaHpiUint32T()
        x.UserEventMaxSize = self.demarshalSaHpiUint32T()
        x.UpdateTimestamp = self.demarshalSaHpiTimeT()
        x.CurrentTime = self.demarshalSaHpiTimeT()
        x.Enabled = self.demarshalSaHpiBoolT()
        x.OverflowFlag = self.demarshalSaHpiBoolT()
        x.OverflowResetable = self.demarshalSaHpiBoolT()
        x.OverflowAction = self.demarshalSaHpiEventLogOverflowActionT()
        return x

    def demarshalSaHpiEventLogEntryT( self ):
        x = SaHpiEventLogEntryT()
        x.EntryId = self.demarshalSaHpiEventLogEntryIdT()
        x.Timestamp = self.demarshalSaHpiTimeT()
        x.Event = self.demarshalSaHpiEventT()
        return x

