/*      -*- c# -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
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

using System;


namespace org {
namespace openhpi {


/**********************************************************
 * HPI Marshal (auto-generated)
 *********************************************************/
internal class HpiMarshalGen: HpiMarshalCore
{
    /**********************************************************
     * Marshal: For HPI Simple Data Types
     *********************************************************/
    public void MarshalSaHpiBoolT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiManufacturerIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiVersionT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaErrorT( long x )
    {
        MarshalSaHpiInt32T( x );
    }

    public void MarshalSaHpiDomainIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiSessionIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiResourceIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiEntryIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiTimeT( long x )
    {
        MarshalSaHpiInt64T( x );
    }

    public void MarshalSaHpiTimeoutT( long x )
    {
        MarshalSaHpiInt64T( x );
    }

    public void MarshalSaHpiInstrumentIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiEntityLocationT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiEventCategoryT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiEventStateT( long x )
    {
        MarshalSaHpiUint16T( x );
    }

    public void MarshalSaHpiSensorNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiSensorRangeFlagsT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiSensorThdMaskT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiCtrlNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiCtrlStateDiscreteT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiCtrlStateAnalogT( long x )
    {
        MarshalSaHpiInt32T( x );
    }

    public void MarshalSaHpiTxtLineNumT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiIdrIdT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiWatchdogNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiWatchdogExpFlagsT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiDimiNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiDimiTestCapabilityT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiDimiTestNumT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiDimiTestPercentCompletedT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiFumiNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiBankNumT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiFumiLogicalBankStateFlagsT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiFumiProtocolT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiFumiCapabilityT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiSensorOptionalDataT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiSensorEnableOptDataT( long x )
    {
        MarshalSaHpiUint8T( x );
    }

    public void MarshalSaHpiEvtQueueStatusT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiAnnunciatorNumT( long x )
    {
        MarshalSaHpiInstrumentIdT( x );
    }

    public void MarshalSaHpiLoadNumberT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiCapabilitiesT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiHsCapabilitiesT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiDomainCapabilitiesT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiAlarmIdT( long x )
    {
        MarshalSaHpiEntryIdT( x );
    }

    public void MarshalSaHpiEventLogCapabilitiesT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiEventLogEntryIdT( long x )
    {
        MarshalSaHpiUint32T( x );
    }

    public void MarshalSaHpiLanguageT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiTextTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiEntityTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorReadingTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorEventMaskActionT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorUnitsT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorModUnitUseT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSensorEventCtrlT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiCtrlTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiCtrlStateDigitalT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiCtrlModeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiCtrlOutputTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiIdrAreaTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiIdrFieldTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiWatchdogActionT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiWatchdogActionEventT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiWatchdogPretimerInterruptT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiWatchdogTimerUseT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDimiTestServiceImpactT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDimiTestRunStatusT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDimiTestErrCodeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDimiTestParamTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDimiReadyT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiSpecInfoTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiSafDefinedSpecIdT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiServiceImpactT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiSourceStatusT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiBankStateT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiFumiUpgradeStatusT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiHsIndicatorStateT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiHsActionT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiHsStateT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiHsCauseOfStateChangeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSeverityT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiResourceEventTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiDomainEventTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiSwEventTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiEventTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiStatusCondTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiAnnunciatorModeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiAnnunciatorTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiRdrTypeT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiParmActionT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiResetActionT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiPowerStateT( long x )
    {
        MarshalEnum( x );
    }

    public void MarshalSaHpiEventLogOverflowActionT( long x )
    {
        MarshalEnum( x );
    }

    /**********************************************************
     * Marshal: For HPI Structs and Unions
     *********************************************************/
    public void MarshalSaHpiTextBufferT( SaHpiTextBufferT x )
    {
        MarshalSaHpiTextTypeT( x.DataType );
        MarshalSaHpiLanguageT( x.Language );
        MarshalSaHpiUint8T( x.DataLength );
        MarshalByteArray( x.Data, HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );
    }

    public void MarshalSaHpiEntityT( SaHpiEntityT x )
    {
        MarshalSaHpiEntityTypeT( x.EntityType );
        MarshalSaHpiEntityLocationT( x.EntityLocation );
    }

    public void MarshalSaHpiEntityPathT( SaHpiEntityPathT x )
    {
        for ( int i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            MarshalSaHpiEntityT( x.Entry[i] );
        }
    }

    public void MarshalSaHpiSensorReadingUnionT( SaHpiSensorReadingUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_INT64 ) {
            MarshalSaHpiInt64T( x.SensorInt64 );
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_UINT64 ) {
            MarshalSaHpiUint64T( x.SensorUint64 );
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
            MarshalSaHpiFloat64T( x.SensorFloat64 );
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            MarshalByteArray( x.SensorBuffer, HpiConst.SAHPI_SENSOR_BUFFER_LENGTH );
        }
    }

    public void MarshalSaHpiSensorReadingT( SaHpiSensorReadingT x )
    {
        MarshalSaHpiBoolT( x.IsSupported );
        MarshalSaHpiSensorReadingTypeT( x.Type );
        MarshalSaHpiSensorReadingUnionT( x.Value, x.Type );
    }

    public void MarshalSaHpiSensorThresholdsT( SaHpiSensorThresholdsT x )
    {
        MarshalSaHpiSensorReadingT( x.LowCritical );
        MarshalSaHpiSensorReadingT( x.LowMajor );
        MarshalSaHpiSensorReadingT( x.LowMinor );
        MarshalSaHpiSensorReadingT( x.UpCritical );
        MarshalSaHpiSensorReadingT( x.UpMajor );
        MarshalSaHpiSensorReadingT( x.UpMinor );
        MarshalSaHpiSensorReadingT( x.PosThdHysteresis );
        MarshalSaHpiSensorReadingT( x.NegThdHysteresis );
    }

    public void MarshalSaHpiSensorRangeT( SaHpiSensorRangeT x )
    {
        MarshalSaHpiSensorRangeFlagsT( x.Flags );
        MarshalSaHpiSensorReadingT( x.Max );
        MarshalSaHpiSensorReadingT( x.Min );
        MarshalSaHpiSensorReadingT( x.Nominal );
        MarshalSaHpiSensorReadingT( x.NormalMax );
        MarshalSaHpiSensorReadingT( x.NormalMin );
    }

    public void MarshalSaHpiSensorDataFormatT( SaHpiSensorDataFormatT x )
    {
        MarshalSaHpiBoolT( x.IsSupported );
        MarshalSaHpiSensorReadingTypeT( x.ReadingType );
        MarshalSaHpiSensorUnitsT( x.BaseUnits );
        MarshalSaHpiSensorUnitsT( x.ModifierUnits );
        MarshalSaHpiSensorModUnitUseT( x.ModifierUse );
        MarshalSaHpiBoolT( x.Percentage );
        MarshalSaHpiSensorRangeT( x.Range );
        MarshalSaHpiFloat64T( x.AccuracyFactor );
    }

    public void MarshalSaHpiSensorThdDefnT( SaHpiSensorThdDefnT x )
    {
        MarshalSaHpiBoolT( x.IsAccessible );
        MarshalSaHpiSensorThdMaskT( x.ReadThold );
        MarshalSaHpiSensorThdMaskT( x.WriteThold );
        MarshalSaHpiBoolT( x.Nonlinear );
    }

    public void MarshalSaHpiSensorRecT( SaHpiSensorRecT x )
    {
        MarshalSaHpiSensorNumT( x.Num );
        MarshalSaHpiSensorTypeT( x.Type );
        MarshalSaHpiEventCategoryT( x.Category );
        MarshalSaHpiBoolT( x.EnableCtrl );
        MarshalSaHpiSensorEventCtrlT( x.EventCtrl );
        MarshalSaHpiEventStateT( x.Events );
        MarshalSaHpiSensorDataFormatT( x.DataFormat );
        MarshalSaHpiSensorThdDefnT( x.ThresholdDefn );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiCtrlStateStreamT( SaHpiCtrlStateStreamT x )
    {
        MarshalSaHpiBoolT( x.Repeat );
        MarshalSaHpiUint32T( x.StreamLength );
        MarshalByteArray( x.Stream, HpiConst.SAHPI_CTRL_MAX_STREAM_LENGTH );
    }

    public void MarshalSaHpiCtrlStateTextT( SaHpiCtrlStateTextT x )
    {
        MarshalSaHpiTxtLineNumT( x.Line );
        MarshalSaHpiTextBufferT( x.Text );
    }

    public void MarshalSaHpiCtrlStateOemT( SaHpiCtrlStateOemT x )
    {
        MarshalSaHpiManufacturerIdT( x.MId );
        MarshalSaHpiUint8T( x.BodyLength );
        MarshalByteArray( x.Body, HpiConst.SAHPI_CTRL_MAX_OEM_BODY_LENGTH );
    }

    public void MarshalSaHpiCtrlStateUnionT( SaHpiCtrlStateUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DIGITAL ) {
            MarshalSaHpiCtrlStateDigitalT( x.Digital );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DISCRETE ) {
            MarshalSaHpiCtrlStateDiscreteT( x.Discrete );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_ANALOG ) {
            MarshalSaHpiCtrlStateAnalogT( x.Analog );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            MarshalSaHpiCtrlStateStreamT( x.Stream );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            MarshalSaHpiCtrlStateTextT( x.Text );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            MarshalSaHpiCtrlStateOemT( x.Oem );
        }
    }

    public void MarshalSaHpiCtrlStateT( SaHpiCtrlStateT x )
    {
        MarshalSaHpiCtrlTypeT( x.Type );
        MarshalSaHpiCtrlStateUnionT( x.StateUnion, x.Type );
    }

    public void MarshalSaHpiCtrlRecDigitalT( SaHpiCtrlRecDigitalT x )
    {
        MarshalSaHpiCtrlStateDigitalT( x.Default );
    }

    public void MarshalSaHpiCtrlRecDiscreteT( SaHpiCtrlRecDiscreteT x )
    {
        MarshalSaHpiCtrlStateDiscreteT( x.Default );
    }

    public void MarshalSaHpiCtrlRecAnalogT( SaHpiCtrlRecAnalogT x )
    {
        MarshalSaHpiCtrlStateAnalogT( x.Min );
        MarshalSaHpiCtrlStateAnalogT( x.Max );
        MarshalSaHpiCtrlStateAnalogT( x.Default );
    }

    public void MarshalSaHpiCtrlRecStreamT( SaHpiCtrlRecStreamT x )
    {
        MarshalSaHpiCtrlStateStreamT( x.Default );
    }

    public void MarshalSaHpiCtrlRecTextT( SaHpiCtrlRecTextT x )
    {
        MarshalSaHpiUint8T( x.MaxChars );
        MarshalSaHpiUint8T( x.MaxLines );
        MarshalSaHpiLanguageT( x.Language );
        MarshalSaHpiTextTypeT( x.DataType );
        MarshalSaHpiCtrlStateTextT( x.Default );
    }

    public void MarshalSaHpiCtrlRecOemT( SaHpiCtrlRecOemT x )
    {
        MarshalSaHpiManufacturerIdT( x.MId );
        MarshalByteArray( x.ConfigData, HpiConst.SAHPI_CTRL_OEM_CONFIG_LENGTH );
        MarshalSaHpiCtrlStateOemT( x.Default );
    }

    public void MarshalSaHpiCtrlRecUnionT( SaHpiCtrlRecUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DIGITAL ) {
            MarshalSaHpiCtrlRecDigitalT( x.Digital );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DISCRETE ) {
            MarshalSaHpiCtrlRecDiscreteT( x.Discrete );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_ANALOG ) {
            MarshalSaHpiCtrlRecAnalogT( x.Analog );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            MarshalSaHpiCtrlRecStreamT( x.Stream );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            MarshalSaHpiCtrlRecTextT( x.Text );
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            MarshalSaHpiCtrlRecOemT( x.Oem );
        }
    }

    public void MarshalSaHpiCtrlDefaultModeT( SaHpiCtrlDefaultModeT x )
    {
        MarshalSaHpiCtrlModeT( x.Mode );
        MarshalSaHpiBoolT( x.ReadOnly );
    }

    public void MarshalSaHpiCtrlRecT( SaHpiCtrlRecT x )
    {
        MarshalSaHpiCtrlNumT( x.Num );
        MarshalSaHpiCtrlOutputTypeT( x.OutputType );
        MarshalSaHpiCtrlTypeT( x.Type );
        MarshalSaHpiCtrlRecUnionT( x.TypeUnion, x.Type );
        MarshalSaHpiCtrlDefaultModeT( x.DefaultMode );
        MarshalSaHpiBoolT( x.WriteOnly );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiIdrFieldT( SaHpiIdrFieldT x )
    {
        MarshalSaHpiEntryIdT( x.AreaId );
        MarshalSaHpiEntryIdT( x.FieldId );
        MarshalSaHpiIdrFieldTypeT( x.Type );
        MarshalSaHpiBoolT( x.ReadOnly );
        MarshalSaHpiTextBufferT( x.Field );
    }

    public void MarshalSaHpiIdrAreaHeaderT( SaHpiIdrAreaHeaderT x )
    {
        MarshalSaHpiEntryIdT( x.AreaId );
        MarshalSaHpiIdrAreaTypeT( x.Type );
        MarshalSaHpiBoolT( x.ReadOnly );
        MarshalSaHpiUint32T( x.NumFields );
    }

    public void MarshalSaHpiIdrInfoT( SaHpiIdrInfoT x )
    {
        MarshalSaHpiIdrIdT( x.IdrId );
        MarshalSaHpiUint32T( x.UpdateCount );
        MarshalSaHpiBoolT( x.ReadOnly );
        MarshalSaHpiUint32T( x.NumAreas );
    }

    public void MarshalSaHpiInventoryRecT( SaHpiInventoryRecT x )
    {
        MarshalSaHpiIdrIdT( x.IdrId );
        MarshalSaHpiBoolT( x.Persistent );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiWatchdogT( SaHpiWatchdogT x )
    {
        MarshalSaHpiBoolT( x.Log );
        MarshalSaHpiBoolT( x.Running );
        MarshalSaHpiWatchdogTimerUseT( x.TimerUse );
        MarshalSaHpiWatchdogActionT( x.TimerAction );
        MarshalSaHpiWatchdogPretimerInterruptT( x.PretimerInterrupt );
        MarshalSaHpiUint32T( x.PreTimeoutInterval );
        MarshalSaHpiWatchdogExpFlagsT( x.TimerUseExpFlags );
        MarshalSaHpiUint32T( x.InitialCount );
        MarshalSaHpiUint32T( x.PresentCount );
    }

    public void MarshalSaHpiWatchdogRecT( SaHpiWatchdogRecT x )
    {
        MarshalSaHpiWatchdogNumT( x.WatchdogNum );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiDimiTestAffectedEntityT( SaHpiDimiTestAffectedEntityT x )
    {
        MarshalSaHpiEntityPathT( x.EntityImpacted );
        MarshalSaHpiDimiTestServiceImpactT( x.ServiceImpact );
    }

    public void MarshalSaHpiDimiTestResultsT( SaHpiDimiTestResultsT x )
    {
        MarshalSaHpiTimeT( x.ResultTimeStamp );
        MarshalSaHpiTimeoutT( x.RunDuration );
        MarshalSaHpiDimiTestRunStatusT( x.LastRunStatus );
        MarshalSaHpiDimiTestErrCodeT( x.TestErrorCode );
        MarshalSaHpiTextBufferT( x.TestResultString );
        MarshalSaHpiBoolT( x.TestResultStringIsURI );
    }

    public void MarshalSaHpiDimiTestParamValueT( SaHpiDimiTestParamValueT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            MarshalSaHpiInt32T( x.paramint );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            MarshalSaHpiBoolT( x.parambool );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            MarshalSaHpiFloat64T( x.paramfloat );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            MarshalSaHpiTextBufferT( x.paramtext );
        }
    }

    public void MarshalSaHpiDimiTestParameterValueUnionT( SaHpiDimiTestParameterValueUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            MarshalSaHpiInt32T( x.IntValue );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            MarshalSaHpiFloat64T( x.FloatValue );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            MarshalSaHpiFloat64T( x.FloatValue );
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            MarshalSaHpiFloat64T( x.FloatValue );
        }
    }

    public void MarshalSaHpiDimiTestParamsDefinitionT( SaHpiDimiTestParamsDefinitionT x )
    {
        MarshalByteArray( x.ParamName, HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN );
        MarshalSaHpiTextBufferT( x.ParamInfo );
        MarshalSaHpiDimiTestParamTypeT( x.ParamType );
        MarshalSaHpiDimiTestParameterValueUnionT( x.MinValue, x.ParamType );
        MarshalSaHpiDimiTestParameterValueUnionT( x.MaxValue, x.ParamType );
        MarshalSaHpiDimiTestParamValueT( x.DefaultParam, x.ParamType );
    }

    public void MarshalSaHpiDimiTestT( SaHpiDimiTestT x )
    {
        MarshalSaHpiTextBufferT( x.TestName );
        MarshalSaHpiDimiTestServiceImpactT( x.ServiceImpact );
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            MarshalSaHpiDimiTestAffectedEntityT( x.EntitiesImpacted[i] );
        }
        MarshalSaHpiBoolT( x.NeedServiceOS );
        MarshalSaHpiTextBufferT( x.ServiceOS );
        MarshalSaHpiTimeT( x.ExpectedRunDuration );
        MarshalSaHpiDimiTestCapabilityT( x.TestCapabilities );
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            MarshalSaHpiDimiTestParamsDefinitionT( x.TestParameters[i] );
        }
    }

    public void MarshalSaHpiDimiTestVariableParamsT( SaHpiDimiTestVariableParamsT x )
    {
        MarshalByteArray( x.ParamName, HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN );
        MarshalSaHpiDimiTestParamTypeT( x.ParamType );
        MarshalSaHpiDimiTestParamValueT( x.Value, x.ParamType );
    }

    public void MarshalSaHpiDimiInfoT( SaHpiDimiInfoT x )
    {
        MarshalSaHpiUint32T( x.NumberOfTests );
        MarshalSaHpiUint32T( x.TestNumUpdateCounter );
    }

    public void MarshalSaHpiDimiRecT( SaHpiDimiRecT x )
    {
        MarshalSaHpiDimiNumT( x.DimiNum );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiFumiSafDefinedSpecInfoT( SaHpiFumiSafDefinedSpecInfoT x )
    {
        MarshalSaHpiFumiSafDefinedSpecIdT( x.SpecID );
        MarshalSaHpiUint32T( x.RevisionID );
    }

    public void MarshalSaHpiFumiOemDefinedSpecInfoT( SaHpiFumiOemDefinedSpecInfoT x )
    {
        MarshalSaHpiManufacturerIdT( x.Mid );
        MarshalSaHpiUint8T( x.BodyLength );
        MarshalByteArray( x.Body, HpiConst.SAHPI_FUMI_MAX_OEM_BODY_LENGTH );
    }

    public void MarshalSaHpiFumiSpecInfoTypeUnionT( SaHpiFumiSpecInfoTypeUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            MarshalSaHpiFumiSafDefinedSpecInfoT( x.SafDefined );
        }
        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            MarshalSaHpiFumiOemDefinedSpecInfoT( x.OemDefined );
        }
    }

    public void MarshalSaHpiFumiSpecInfoT( SaHpiFumiSpecInfoT x )
    {
        MarshalSaHpiFumiSpecInfoTypeT( x.SpecInfoType );
        MarshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoTypeUnion, x.SpecInfoType );
    }

    public void MarshalSaHpiFumiFirmwareInstanceInfoT( SaHpiFumiFirmwareInstanceInfoT x )
    {
        MarshalSaHpiBoolT( x.InstancePresent );
        MarshalSaHpiTextBufferT( x.Identifier );
        MarshalSaHpiTextBufferT( x.Description );
        MarshalSaHpiTextBufferT( x.DateTime );
        MarshalSaHpiUint32T( x.MajorVersion );
        MarshalSaHpiUint32T( x.MinorVersion );
        MarshalSaHpiUint32T( x.AuxVersion );
    }

    public void MarshalSaHpiFumiImpactedEntityT( SaHpiFumiImpactedEntityT x )
    {
        MarshalSaHpiEntityPathT( x.ImpactedEntity );
        MarshalSaHpiFumiServiceImpactT( x.ServiceImpact );
    }

    public void MarshalSaHpiFumiServiceImpactDataT( SaHpiFumiServiceImpactDataT x )
    {
        MarshalSaHpiUint32T( x.NumEntities );
        for ( int i = 0; i < HpiConst.SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            MarshalSaHpiFumiImpactedEntityT( x.ImpactedEntities[i] );
        }
    }

    public void MarshalSaHpiFumiSourceInfoT( SaHpiFumiSourceInfoT x )
    {
        MarshalSaHpiTextBufferT( x.SourceUri );
        MarshalSaHpiFumiSourceStatusT( x.SourceStatus );
        MarshalSaHpiTextBufferT( x.Identifier );
        MarshalSaHpiTextBufferT( x.Description );
        MarshalSaHpiTextBufferT( x.DateTime );
        MarshalSaHpiUint32T( x.MajorVersion );
        MarshalSaHpiUint32T( x.MinorVersion );
        MarshalSaHpiUint32T( x.AuxVersion );
    }

    public void MarshalSaHpiFumiComponentInfoT( SaHpiFumiComponentInfoT x )
    {
        MarshalSaHpiEntryIdT( x.EntryId );
        MarshalSaHpiUint32T( x.ComponentId );
        MarshalSaHpiFumiFirmwareInstanceInfoT( x.MainFwInstance );
        MarshalSaHpiUint32T( x.ComponentFlags );
    }

    public void MarshalSaHpiFumiBankInfoT( SaHpiFumiBankInfoT x )
    {
        MarshalSaHpiUint8T( x.BankId );
        MarshalSaHpiUint32T( x.BankSize );
        MarshalSaHpiUint32T( x.Position );
        MarshalSaHpiFumiBankStateT( x.BankState );
        MarshalSaHpiTextBufferT( x.Identifier );
        MarshalSaHpiTextBufferT( x.Description );
        MarshalSaHpiTextBufferT( x.DateTime );
        MarshalSaHpiUint32T( x.MajorVersion );
        MarshalSaHpiUint32T( x.MinorVersion );
        MarshalSaHpiUint32T( x.AuxVersion );
    }

    public void MarshalSaHpiFumiLogicalBankInfoT( SaHpiFumiLogicalBankInfoT x )
    {
        MarshalSaHpiUint8T( x.FirmwarePersistentLocationCount );
        MarshalSaHpiFumiLogicalBankStateFlagsT( x.BankStateFlags );
        MarshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance );
        MarshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance );
    }

    public void MarshalSaHpiFumiLogicalComponentInfoT( SaHpiFumiLogicalComponentInfoT x )
    {
        MarshalSaHpiEntryIdT( x.EntryId );
        MarshalSaHpiUint32T( x.ComponentId );
        MarshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance );
        MarshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance );
        MarshalSaHpiUint32T( x.ComponentFlags );
    }

    public void MarshalSaHpiFumiRecT( SaHpiFumiRecT x )
    {
        MarshalSaHpiFumiNumT( x.Num );
        MarshalSaHpiFumiProtocolT( x.AccessProt );
        MarshalSaHpiFumiCapabilityT( x.Capability );
        MarshalSaHpiUint8T( x.NumBanks );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiResourceEventT( SaHpiResourceEventT x )
    {
        MarshalSaHpiResourceEventTypeT( x.ResourceEventType );
    }

    public void MarshalSaHpiDomainEventT( SaHpiDomainEventT x )
    {
        MarshalSaHpiDomainEventTypeT( x.Type );
        MarshalSaHpiDomainIdT( x.DomainId );
    }

    public void MarshalSaHpiSensorEventT( SaHpiSensorEventT x )
    {
        MarshalSaHpiSensorNumT( x.SensorNum );
        MarshalSaHpiSensorTypeT( x.SensorType );
        MarshalSaHpiEventCategoryT( x.EventCategory );
        MarshalSaHpiBoolT( x.Assertion );
        MarshalSaHpiEventStateT( x.EventState );
        MarshalSaHpiSensorOptionalDataT( x.OptionalDataPresent );
        MarshalSaHpiSensorReadingT( x.TriggerReading );
        MarshalSaHpiSensorReadingT( x.TriggerThreshold );
        MarshalSaHpiEventStateT( x.PreviousState );
        MarshalSaHpiEventStateT( x.CurrentState );
        MarshalSaHpiUint32T( x.Oem );
        MarshalSaHpiUint32T( x.SensorSpecific );
    }

    public void MarshalSaHpiSensorEnableChangeEventT( SaHpiSensorEnableChangeEventT x )
    {
        MarshalSaHpiSensorNumT( x.SensorNum );
        MarshalSaHpiSensorTypeT( x.SensorType );
        MarshalSaHpiEventCategoryT( x.EventCategory );
        MarshalSaHpiBoolT( x.SensorEnable );
        MarshalSaHpiBoolT( x.SensorEventEnable );
        MarshalSaHpiEventStateT( x.AssertEventMask );
        MarshalSaHpiEventStateT( x.DeassertEventMask );
        MarshalSaHpiSensorEnableOptDataT( x.OptionalDataPresent );
        MarshalSaHpiEventStateT( x.CurrentState );
        MarshalSaHpiEventStateT( x.CriticalAlarms );
        MarshalSaHpiEventStateT( x.MajorAlarms );
        MarshalSaHpiEventStateT( x.MinorAlarms );
    }

    public void MarshalSaHpiHotSwapEventT( SaHpiHotSwapEventT x )
    {
        MarshalSaHpiHsStateT( x.HotSwapState );
        MarshalSaHpiHsStateT( x.PreviousHotSwapState );
        MarshalSaHpiHsCauseOfStateChangeT( x.CauseOfStateChange );
    }

    public void MarshalSaHpiWatchdogEventT( SaHpiWatchdogEventT x )
    {
        MarshalSaHpiWatchdogNumT( x.WatchdogNum );
        MarshalSaHpiWatchdogActionEventT( x.WatchdogAction );
        MarshalSaHpiWatchdogPretimerInterruptT( x.WatchdogPreTimerAction );
        MarshalSaHpiWatchdogTimerUseT( x.WatchdogUse );
    }

    public void MarshalSaHpiHpiSwEventT( SaHpiHpiSwEventT x )
    {
        MarshalSaHpiManufacturerIdT( x.MId );
        MarshalSaHpiSwEventTypeT( x.Type );
        MarshalSaHpiTextBufferT( x.EventData );
    }

    public void MarshalSaHpiOemEventT( SaHpiOemEventT x )
    {
        MarshalSaHpiManufacturerIdT( x.MId );
        MarshalSaHpiTextBufferT( x.OemEventData );
    }

    public void MarshalSaHpiUserEventT( SaHpiUserEventT x )
    {
        MarshalSaHpiTextBufferT( x.UserEventData );
    }

    public void MarshalSaHpiDimiEventT( SaHpiDimiEventT x )
    {
        MarshalSaHpiDimiNumT( x.DimiNum );
        MarshalSaHpiDimiTestNumT( x.TestNum );
        MarshalSaHpiDimiTestRunStatusT( x.DimiTestRunStatus );
        MarshalSaHpiDimiTestPercentCompletedT( x.DimiTestPercentCompleted );
    }

    public void MarshalSaHpiDimiUpdateEventT( SaHpiDimiUpdateEventT x )
    {
        MarshalSaHpiDimiNumT( x.DimiNum );
    }

    public void MarshalSaHpiFumiEventT( SaHpiFumiEventT x )
    {
        MarshalSaHpiFumiNumT( x.FumiNum );
        MarshalSaHpiUint8T( x.BankNum );
        MarshalSaHpiFumiUpgradeStatusT( x.UpgradeStatus );
    }

    public void MarshalSaHpiEventUnionT( SaHpiEventUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_ET_RESOURCE ) {
            MarshalSaHpiResourceEventT( x.ResourceEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_DOMAIN ) {
            MarshalSaHpiDomainEventT( x.DomainEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR ) {
            MarshalSaHpiSensorEventT( x.SensorEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            MarshalSaHpiSensorEnableChangeEventT( x.SensorEnableChangeEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_HOTSWAP ) {
            MarshalSaHpiHotSwapEventT( x.HotSwapEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_WATCHDOG ) {
            MarshalSaHpiWatchdogEventT( x.WatchdogEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_HPI_SW ) {
            MarshalSaHpiHpiSwEventT( x.HpiSwEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_OEM ) {
            MarshalSaHpiOemEventT( x.OemEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_USER ) {
            MarshalSaHpiUserEventT( x.UserEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI ) {
            MarshalSaHpiDimiEventT( x.DimiEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI_UPDATE ) {
            MarshalSaHpiDimiUpdateEventT( x.DimiUpdateEvent );
        }
        if ( mod == HpiConst.SAHPI_ET_FUMI ) {
            MarshalSaHpiFumiEventT( x.FumiEvent );
        }
    }

    public void MarshalSaHpiEventT( SaHpiEventT x )
    {
        MarshalSaHpiResourceIdT( x.Source );
        MarshalSaHpiEventTypeT( x.EventType );
        MarshalSaHpiTimeT( x.Timestamp );
        MarshalSaHpiSeverityT( x.Severity );
        MarshalSaHpiEventUnionT( x.EventDataUnion, x.EventType );
    }

    public void MarshalSaHpiNameT( SaHpiNameT x )
    {
        MarshalSaHpiUint16T( x.Length );
        MarshalByteArray( x.Value, HpiConst.SA_HPI_MAX_NAME_LENGTH );
    }

    public void MarshalSaHpiConditionT( SaHpiConditionT x )
    {
        MarshalSaHpiStatusCondTypeT( x.Type );
        MarshalSaHpiEntityPathT( x.Entity );
        MarshalSaHpiDomainIdT( x.DomainId );
        MarshalSaHpiResourceIdT( x.ResourceId );
        MarshalSaHpiSensorNumT( x.SensorNum );
        MarshalSaHpiEventStateT( x.EventState );
        MarshalSaHpiNameT( x.Name );
        MarshalSaHpiManufacturerIdT( x.Mid );
        MarshalSaHpiTextBufferT( x.Data );
    }

    public void MarshalSaHpiAnnouncementT( SaHpiAnnouncementT x )
    {
        MarshalSaHpiEntryIdT( x.EntryId );
        MarshalSaHpiTimeT( x.Timestamp );
        MarshalSaHpiBoolT( x.AddedByUser );
        MarshalSaHpiSeverityT( x.Severity );
        MarshalSaHpiBoolT( x.Acknowledged );
        MarshalSaHpiConditionT( x.StatusCond );
    }

    public void MarshalSaHpiAnnunciatorRecT( SaHpiAnnunciatorRecT x )
    {
        MarshalSaHpiAnnunciatorNumT( x.AnnunciatorNum );
        MarshalSaHpiAnnunciatorTypeT( x.AnnunciatorType );
        MarshalSaHpiBoolT( x.ModeReadOnly );
        MarshalSaHpiUint32T( x.MaxConditions );
        MarshalSaHpiUint32T( x.Oem );
    }

    public void MarshalSaHpiRdrTypeUnionT( SaHpiRdrTypeUnionT x, long mod )
    {
        if ( mod == HpiConst.SAHPI_CTRL_RDR ) {
            MarshalSaHpiCtrlRecT( x.CtrlRec );
        }
        if ( mod == HpiConst.SAHPI_SENSOR_RDR ) {
            MarshalSaHpiSensorRecT( x.SensorRec );
        }
        if ( mod == HpiConst.SAHPI_INVENTORY_RDR ) {
            MarshalSaHpiInventoryRecT( x.InventoryRec );
        }
        if ( mod == HpiConst.SAHPI_WATCHDOG_RDR ) {
            MarshalSaHpiWatchdogRecT( x.WatchdogRec );
        }
        if ( mod == HpiConst.SAHPI_ANNUNCIATOR_RDR ) {
            MarshalSaHpiAnnunciatorRecT( x.AnnunciatorRec );
        }
        if ( mod == HpiConst.SAHPI_DIMI_RDR ) {
            MarshalSaHpiDimiRecT( x.DimiRec );
        }
        if ( mod == HpiConst.SAHPI_FUMI_RDR ) {
            MarshalSaHpiFumiRecT( x.FumiRec );
        }
    }

    public void MarshalSaHpiRdrT( SaHpiRdrT x )
    {
        MarshalSaHpiEntryIdT( x.RecordId );
        MarshalSaHpiRdrTypeT( x.RdrType );
        MarshalSaHpiEntityPathT( x.Entity );
        MarshalSaHpiBoolT( x.IsFru );
        MarshalSaHpiRdrTypeUnionT( x.RdrTypeUnion, x.RdrType );
        MarshalSaHpiTextBufferT( x.IdString );
    }

    public void MarshalSaHpiLoadIdT( SaHpiLoadIdT x )
    {
        MarshalSaHpiLoadNumberT( x.LoadNumber );
        MarshalSaHpiTextBufferT( x.LoadName );
    }

    public void MarshalSaHpiResourceInfoT( SaHpiResourceInfoT x )
    {
        MarshalSaHpiUint8T( x.ResourceRev );
        MarshalSaHpiUint8T( x.SpecificVer );
        MarshalSaHpiUint8T( x.DeviceSupport );
        MarshalSaHpiManufacturerIdT( x.ManufacturerId );
        MarshalSaHpiUint16T( x.ProductId );
        MarshalSaHpiUint8T( x.FirmwareMajorRev );
        MarshalSaHpiUint8T( x.FirmwareMinorRev );
        MarshalSaHpiUint8T( x.AuxFirmwareRev );
        MarshalByteArray( x.Guid, HpiConst.SAHPI_GUID_LENGTH );
    }

    public void MarshalSaHpiRptEntryT( SaHpiRptEntryT x )
    {
        MarshalSaHpiEntryIdT( x.EntryId );
        MarshalSaHpiResourceIdT( x.ResourceId );
        MarshalSaHpiResourceInfoT( x.ResourceInfo );
        MarshalSaHpiEntityPathT( x.ResourceEntity );
        MarshalSaHpiCapabilitiesT( x.ResourceCapabilities );
        MarshalSaHpiHsCapabilitiesT( x.HotSwapCapabilities );
        MarshalSaHpiSeverityT( x.ResourceSeverity );
        MarshalSaHpiBoolT( x.ResourceFailed );
        MarshalSaHpiTextBufferT( x.ResourceTag );
    }

    public void MarshalSaHpiDomainInfoT( SaHpiDomainInfoT x )
    {
        MarshalSaHpiDomainIdT( x.DomainId );
        MarshalSaHpiDomainCapabilitiesT( x.DomainCapabilities );
        MarshalSaHpiBoolT( x.IsPeer );
        MarshalSaHpiTextBufferT( x.DomainTag );
        MarshalSaHpiUint32T( x.DrtUpdateCount );
        MarshalSaHpiTimeT( x.DrtUpdateTimestamp );
        MarshalSaHpiUint32T( x.RptUpdateCount );
        MarshalSaHpiTimeT( x.RptUpdateTimestamp );
        MarshalSaHpiUint32T( x.DatUpdateCount );
        MarshalSaHpiTimeT( x.DatUpdateTimestamp );
        MarshalSaHpiUint32T( x.ActiveAlarms );
        MarshalSaHpiUint32T( x.CriticalAlarms );
        MarshalSaHpiUint32T( x.MajorAlarms );
        MarshalSaHpiUint32T( x.MinorAlarms );
        MarshalSaHpiUint32T( x.DatUserAlarmLimit );
        MarshalSaHpiBoolT( x.DatOverflow );
        MarshalByteArray( x.Guid, HpiConst.SAHPI_GUID_LENGTH );
    }

    public void MarshalSaHpiDrtEntryT( SaHpiDrtEntryT x )
    {
        MarshalSaHpiEntryIdT( x.EntryId );
        MarshalSaHpiDomainIdT( x.DomainId );
        MarshalSaHpiBoolT( x.IsPeer );
    }

    public void MarshalSaHpiAlarmT( SaHpiAlarmT x )
    {
        MarshalSaHpiAlarmIdT( x.AlarmId );
        MarshalSaHpiTimeT( x.Timestamp );
        MarshalSaHpiSeverityT( x.Severity );
        MarshalSaHpiBoolT( x.Acknowledged );
        MarshalSaHpiConditionT( x.AlarmCond );
    }

    public void MarshalSaHpiEventLogInfoT( SaHpiEventLogInfoT x )
    {
        MarshalSaHpiUint32T( x.Entries );
        MarshalSaHpiUint32T( x.Size );
        MarshalSaHpiUint32T( x.UserEventMaxSize );
        MarshalSaHpiTimeT( x.UpdateTimestamp );
        MarshalSaHpiTimeT( x.CurrentTime );
        MarshalSaHpiBoolT( x.Enabled );
        MarshalSaHpiBoolT( x.OverflowFlag );
        MarshalSaHpiBoolT( x.OverflowResetable );
        MarshalSaHpiEventLogOverflowActionT( x.OverflowAction );
    }

    public void MarshalSaHpiEventLogEntryT( SaHpiEventLogEntryT x )
    {
        MarshalSaHpiEventLogEntryIdT( x.EntryId );
        MarshalSaHpiTimeT( x.Timestamp );
        MarshalSaHpiEventT( x.Event );
    }

    /**********************************************************
     * Demarshal: For HPI Simple Data Types
     *********************************************************/
    public long DemarshalSaHpiBoolT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiManufacturerIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiVersionT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaErrorT()
    {
        return DemarshalSaHpiInt32T();
    }

    public long DemarshalSaHpiDomainIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiSessionIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiResourceIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiEntryIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiTimeT()
    {
        return DemarshalSaHpiInt64T();
    }

    public long DemarshalSaHpiTimeoutT()
    {
        return DemarshalSaHpiInt64T();
    }

    public long DemarshalSaHpiInstrumentIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiEntityLocationT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiEventCategoryT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiEventStateT()
    {
        return DemarshalSaHpiUint16T();
    }

    public long DemarshalSaHpiSensorNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiSensorRangeFlagsT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiSensorThdMaskT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiCtrlNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiCtrlStateDiscreteT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiCtrlStateAnalogT()
    {
        return DemarshalSaHpiInt32T();
    }

    public long DemarshalSaHpiTxtLineNumT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiIdrIdT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiWatchdogNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiWatchdogExpFlagsT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiDimiNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiDimiTestCapabilityT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiDimiTestNumT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiDimiTestPercentCompletedT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiFumiNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiBankNumT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiFumiLogicalBankStateFlagsT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiFumiProtocolT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiFumiCapabilityT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiSensorOptionalDataT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiSensorEnableOptDataT()
    {
        return DemarshalSaHpiUint8T();
    }

    public long DemarshalSaHpiEvtQueueStatusT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiAnnunciatorNumT()
    {
        return DemarshalSaHpiInstrumentIdT();
    }

    public long DemarshalSaHpiLoadNumberT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiCapabilitiesT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiHsCapabilitiesT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiDomainCapabilitiesT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiAlarmIdT()
    {
        return DemarshalSaHpiEntryIdT();
    }

    public long DemarshalSaHpiEventLogCapabilitiesT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiEventLogEntryIdT()
    {
        return DemarshalSaHpiUint32T();
    }

    public long DemarshalSaHpiLanguageT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiTextTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiEntityTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorReadingTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorEventMaskActionT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorUnitsT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorModUnitUseT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSensorEventCtrlT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiCtrlTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiCtrlStateDigitalT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiCtrlModeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiCtrlOutputTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiIdrAreaTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiIdrFieldTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiWatchdogActionT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiWatchdogActionEventT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiWatchdogPretimerInterruptT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiWatchdogTimerUseT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDimiTestServiceImpactT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDimiTestRunStatusT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDimiTestErrCodeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDimiTestParamTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDimiReadyT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiSpecInfoTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiSafDefinedSpecIdT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiServiceImpactT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiSourceStatusT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiBankStateT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiFumiUpgradeStatusT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiHsIndicatorStateT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiHsActionT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiHsStateT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiHsCauseOfStateChangeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSeverityT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiResourceEventTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiDomainEventTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiSwEventTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiEventTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiStatusCondTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiAnnunciatorModeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiAnnunciatorTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiRdrTypeT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiParmActionT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiResetActionT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiPowerStateT()
    {
        return DemarshalEnum();
    }

    public long DemarshalSaHpiEventLogOverflowActionT()
    {
        return DemarshalEnum();
    }

    /**********************************************************
     * Demarshal: For HPI Structs and Unions
     *********************************************************/
    public SaHpiTextBufferT DemarshalSaHpiTextBufferT()
    {
        SaHpiTextBufferT x = new SaHpiTextBufferT();

        x.DataType = DemarshalSaHpiTextTypeT();
        x.Language = DemarshalSaHpiLanguageT();
        x.DataLength = DemarshalSaHpiUint8T();
        x.Data = DemarshalByteArray( HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH );

        return x;
    }

    public SaHpiEntityT DemarshalSaHpiEntityT()
    {
        SaHpiEntityT x = new SaHpiEntityT();

        x.EntityType = DemarshalSaHpiEntityTypeT();
        x.EntityLocation = DemarshalSaHpiEntityLocationT();

        return x;
    }

    public SaHpiEntityPathT DemarshalSaHpiEntityPathT()
    {
        SaHpiEntityPathT x = new SaHpiEntityPathT();

        x.Entry = new SaHpiEntityT[HpiConst.SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            x.Entry[i] = DemarshalSaHpiEntityT();
        }

        return x;
    }

    public SaHpiSensorReadingUnionT DemarshalSaHpiSensorReadingUnionT( long mod )
    {
        SaHpiSensorReadingUnionT x = new SaHpiSensorReadingUnionT();

        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_INT64 ) {
            x.SensorInt64 = DemarshalSaHpiInt64T();
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_UINT64 ) {
            x.SensorUint64 = DemarshalSaHpiUint64T();
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
            x.SensorFloat64 = DemarshalSaHpiFloat64T();
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            x.SensorBuffer = DemarshalByteArray( HpiConst.SAHPI_SENSOR_BUFFER_LENGTH );
        }

        return x;
    }

    public SaHpiSensorReadingT DemarshalSaHpiSensorReadingT()
    {
        SaHpiSensorReadingT x = new SaHpiSensorReadingT();

        x.IsSupported = DemarshalSaHpiBoolT();
        x.Type = DemarshalSaHpiSensorReadingTypeT();
        x.Value = DemarshalSaHpiSensorReadingUnionT( x.Type );

        return x;
    }

    public SaHpiSensorThresholdsT DemarshalSaHpiSensorThresholdsT()
    {
        SaHpiSensorThresholdsT x = new SaHpiSensorThresholdsT();

        x.LowCritical = DemarshalSaHpiSensorReadingT();
        x.LowMajor = DemarshalSaHpiSensorReadingT();
        x.LowMinor = DemarshalSaHpiSensorReadingT();
        x.UpCritical = DemarshalSaHpiSensorReadingT();
        x.UpMajor = DemarshalSaHpiSensorReadingT();
        x.UpMinor = DemarshalSaHpiSensorReadingT();
        x.PosThdHysteresis = DemarshalSaHpiSensorReadingT();
        x.NegThdHysteresis = DemarshalSaHpiSensorReadingT();

        return x;
    }

    public SaHpiSensorRangeT DemarshalSaHpiSensorRangeT()
    {
        SaHpiSensorRangeT x = new SaHpiSensorRangeT();

        x.Flags = DemarshalSaHpiSensorRangeFlagsT();
        x.Max = DemarshalSaHpiSensorReadingT();
        x.Min = DemarshalSaHpiSensorReadingT();
        x.Nominal = DemarshalSaHpiSensorReadingT();
        x.NormalMax = DemarshalSaHpiSensorReadingT();
        x.NormalMin = DemarshalSaHpiSensorReadingT();

        return x;
    }

    public SaHpiSensorDataFormatT DemarshalSaHpiSensorDataFormatT()
    {
        SaHpiSensorDataFormatT x = new SaHpiSensorDataFormatT();

        x.IsSupported = DemarshalSaHpiBoolT();
        x.ReadingType = DemarshalSaHpiSensorReadingTypeT();
        x.BaseUnits = DemarshalSaHpiSensorUnitsT();
        x.ModifierUnits = DemarshalSaHpiSensorUnitsT();
        x.ModifierUse = DemarshalSaHpiSensorModUnitUseT();
        x.Percentage = DemarshalSaHpiBoolT();
        x.Range = DemarshalSaHpiSensorRangeT();
        x.AccuracyFactor = DemarshalSaHpiFloat64T();

        return x;
    }

    public SaHpiSensorThdDefnT DemarshalSaHpiSensorThdDefnT()
    {
        SaHpiSensorThdDefnT x = new SaHpiSensorThdDefnT();

        x.IsAccessible = DemarshalSaHpiBoolT();
        x.ReadThold = DemarshalSaHpiSensorThdMaskT();
        x.WriteThold = DemarshalSaHpiSensorThdMaskT();
        x.Nonlinear = DemarshalSaHpiBoolT();

        return x;
    }

    public SaHpiSensorRecT DemarshalSaHpiSensorRecT()
    {
        SaHpiSensorRecT x = new SaHpiSensorRecT();

        x.Num = DemarshalSaHpiSensorNumT();
        x.Type = DemarshalSaHpiSensorTypeT();
        x.Category = DemarshalSaHpiEventCategoryT();
        x.EnableCtrl = DemarshalSaHpiBoolT();
        x.EventCtrl = DemarshalSaHpiSensorEventCtrlT();
        x.Events = DemarshalSaHpiEventStateT();
        x.DataFormat = DemarshalSaHpiSensorDataFormatT();
        x.ThresholdDefn = DemarshalSaHpiSensorThdDefnT();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiCtrlStateStreamT DemarshalSaHpiCtrlStateStreamT()
    {
        SaHpiCtrlStateStreamT x = new SaHpiCtrlStateStreamT();

        x.Repeat = DemarshalSaHpiBoolT();
        x.StreamLength = DemarshalSaHpiUint32T();
        x.Stream = DemarshalByteArray( HpiConst.SAHPI_CTRL_MAX_STREAM_LENGTH );

        return x;
    }

    public SaHpiCtrlStateTextT DemarshalSaHpiCtrlStateTextT()
    {
        SaHpiCtrlStateTextT x = new SaHpiCtrlStateTextT();

        x.Line = DemarshalSaHpiTxtLineNumT();
        x.Text = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiCtrlStateOemT DemarshalSaHpiCtrlStateOemT()
    {
        SaHpiCtrlStateOemT x = new SaHpiCtrlStateOemT();

        x.MId = DemarshalSaHpiManufacturerIdT();
        x.BodyLength = DemarshalSaHpiUint8T();
        x.Body = DemarshalByteArray( HpiConst.SAHPI_CTRL_MAX_OEM_BODY_LENGTH );

        return x;
    }

    public SaHpiCtrlStateUnionT DemarshalSaHpiCtrlStateUnionT( long mod )
    {
        SaHpiCtrlStateUnionT x = new SaHpiCtrlStateUnionT();

        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DIGITAL ) {
            x.Digital = DemarshalSaHpiCtrlStateDigitalT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DISCRETE ) {
            x.Discrete = DemarshalSaHpiCtrlStateDiscreteT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_ANALOG ) {
            x.Analog = DemarshalSaHpiCtrlStateAnalogT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            x.Stream = DemarshalSaHpiCtrlStateStreamT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            x.Text = DemarshalSaHpiCtrlStateTextT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            x.Oem = DemarshalSaHpiCtrlStateOemT();
        }

        return x;
    }

    public SaHpiCtrlStateT DemarshalSaHpiCtrlStateT()
    {
        SaHpiCtrlStateT x = new SaHpiCtrlStateT();

        x.Type = DemarshalSaHpiCtrlTypeT();
        x.StateUnion = DemarshalSaHpiCtrlStateUnionT( x.Type );

        return x;
    }

    public SaHpiCtrlRecDigitalT DemarshalSaHpiCtrlRecDigitalT()
    {
        SaHpiCtrlRecDigitalT x = new SaHpiCtrlRecDigitalT();

        x.Default = DemarshalSaHpiCtrlStateDigitalT();

        return x;
    }

    public SaHpiCtrlRecDiscreteT DemarshalSaHpiCtrlRecDiscreteT()
    {
        SaHpiCtrlRecDiscreteT x = new SaHpiCtrlRecDiscreteT();

        x.Default = DemarshalSaHpiCtrlStateDiscreteT();

        return x;
    }

    public SaHpiCtrlRecAnalogT DemarshalSaHpiCtrlRecAnalogT()
    {
        SaHpiCtrlRecAnalogT x = new SaHpiCtrlRecAnalogT();

        x.Min = DemarshalSaHpiCtrlStateAnalogT();
        x.Max = DemarshalSaHpiCtrlStateAnalogT();
        x.Default = DemarshalSaHpiCtrlStateAnalogT();

        return x;
    }

    public SaHpiCtrlRecStreamT DemarshalSaHpiCtrlRecStreamT()
    {
        SaHpiCtrlRecStreamT x = new SaHpiCtrlRecStreamT();

        x.Default = DemarshalSaHpiCtrlStateStreamT();

        return x;
    }

    public SaHpiCtrlRecTextT DemarshalSaHpiCtrlRecTextT()
    {
        SaHpiCtrlRecTextT x = new SaHpiCtrlRecTextT();

        x.MaxChars = DemarshalSaHpiUint8T();
        x.MaxLines = DemarshalSaHpiUint8T();
        x.Language = DemarshalSaHpiLanguageT();
        x.DataType = DemarshalSaHpiTextTypeT();
        x.Default = DemarshalSaHpiCtrlStateTextT();

        return x;
    }

    public SaHpiCtrlRecOemT DemarshalSaHpiCtrlRecOemT()
    {
        SaHpiCtrlRecOemT x = new SaHpiCtrlRecOemT();

        x.MId = DemarshalSaHpiManufacturerIdT();
        x.ConfigData = DemarshalByteArray( HpiConst.SAHPI_CTRL_OEM_CONFIG_LENGTH );
        x.Default = DemarshalSaHpiCtrlStateOemT();

        return x;
    }

    public SaHpiCtrlRecUnionT DemarshalSaHpiCtrlRecUnionT( long mod )
    {
        SaHpiCtrlRecUnionT x = new SaHpiCtrlRecUnionT();

        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DIGITAL ) {
            x.Digital = DemarshalSaHpiCtrlRecDigitalT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DISCRETE ) {
            x.Discrete = DemarshalSaHpiCtrlRecDiscreteT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_ANALOG ) {
            x.Analog = DemarshalSaHpiCtrlRecAnalogT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            x.Stream = DemarshalSaHpiCtrlRecStreamT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            x.Text = DemarshalSaHpiCtrlRecTextT();
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            x.Oem = DemarshalSaHpiCtrlRecOemT();
        }

        return x;
    }

    public SaHpiCtrlDefaultModeT DemarshalSaHpiCtrlDefaultModeT()
    {
        SaHpiCtrlDefaultModeT x = new SaHpiCtrlDefaultModeT();

        x.Mode = DemarshalSaHpiCtrlModeT();
        x.ReadOnly = DemarshalSaHpiBoolT();

        return x;
    }

    public SaHpiCtrlRecT DemarshalSaHpiCtrlRecT()
    {
        SaHpiCtrlRecT x = new SaHpiCtrlRecT();

        x.Num = DemarshalSaHpiCtrlNumT();
        x.OutputType = DemarshalSaHpiCtrlOutputTypeT();
        x.Type = DemarshalSaHpiCtrlTypeT();
        x.TypeUnion = DemarshalSaHpiCtrlRecUnionT( x.Type );
        x.DefaultMode = DemarshalSaHpiCtrlDefaultModeT();
        x.WriteOnly = DemarshalSaHpiBoolT();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiIdrFieldT DemarshalSaHpiIdrFieldT()
    {
        SaHpiIdrFieldT x = new SaHpiIdrFieldT();

        x.AreaId = DemarshalSaHpiEntryIdT();
        x.FieldId = DemarshalSaHpiEntryIdT();
        x.Type = DemarshalSaHpiIdrFieldTypeT();
        x.ReadOnly = DemarshalSaHpiBoolT();
        x.Field = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiIdrAreaHeaderT DemarshalSaHpiIdrAreaHeaderT()
    {
        SaHpiIdrAreaHeaderT x = new SaHpiIdrAreaHeaderT();

        x.AreaId = DemarshalSaHpiEntryIdT();
        x.Type = DemarshalSaHpiIdrAreaTypeT();
        x.ReadOnly = DemarshalSaHpiBoolT();
        x.NumFields = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiIdrInfoT DemarshalSaHpiIdrInfoT()
    {
        SaHpiIdrInfoT x = new SaHpiIdrInfoT();

        x.IdrId = DemarshalSaHpiIdrIdT();
        x.UpdateCount = DemarshalSaHpiUint32T();
        x.ReadOnly = DemarshalSaHpiBoolT();
        x.NumAreas = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiInventoryRecT DemarshalSaHpiInventoryRecT()
    {
        SaHpiInventoryRecT x = new SaHpiInventoryRecT();

        x.IdrId = DemarshalSaHpiIdrIdT();
        x.Persistent = DemarshalSaHpiBoolT();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiWatchdogT DemarshalSaHpiWatchdogT()
    {
        SaHpiWatchdogT x = new SaHpiWatchdogT();

        x.Log = DemarshalSaHpiBoolT();
        x.Running = DemarshalSaHpiBoolT();
        x.TimerUse = DemarshalSaHpiWatchdogTimerUseT();
        x.TimerAction = DemarshalSaHpiWatchdogActionT();
        x.PretimerInterrupt = DemarshalSaHpiWatchdogPretimerInterruptT();
        x.PreTimeoutInterval = DemarshalSaHpiUint32T();
        x.TimerUseExpFlags = DemarshalSaHpiWatchdogExpFlagsT();
        x.InitialCount = DemarshalSaHpiUint32T();
        x.PresentCount = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiWatchdogRecT DemarshalSaHpiWatchdogRecT()
    {
        SaHpiWatchdogRecT x = new SaHpiWatchdogRecT();

        x.WatchdogNum = DemarshalSaHpiWatchdogNumT();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiDimiTestAffectedEntityT DemarshalSaHpiDimiTestAffectedEntityT()
    {
        SaHpiDimiTestAffectedEntityT x = new SaHpiDimiTestAffectedEntityT();

        x.EntityImpacted = DemarshalSaHpiEntityPathT();
        x.ServiceImpact = DemarshalSaHpiDimiTestServiceImpactT();

        return x;
    }

    public SaHpiDimiTestResultsT DemarshalSaHpiDimiTestResultsT()
    {
        SaHpiDimiTestResultsT x = new SaHpiDimiTestResultsT();

        x.ResultTimeStamp = DemarshalSaHpiTimeT();
        x.RunDuration = DemarshalSaHpiTimeoutT();
        x.LastRunStatus = DemarshalSaHpiDimiTestRunStatusT();
        x.TestErrorCode = DemarshalSaHpiDimiTestErrCodeT();
        x.TestResultString = DemarshalSaHpiTextBufferT();
        x.TestResultStringIsURI = DemarshalSaHpiBoolT();

        return x;
    }

    public SaHpiDimiTestParamValueT DemarshalSaHpiDimiTestParamValueT( long mod )
    {
        SaHpiDimiTestParamValueT x = new SaHpiDimiTestParamValueT();

        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            x.paramint = DemarshalSaHpiInt32T();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            x.parambool = DemarshalSaHpiBoolT();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            x.paramfloat = DemarshalSaHpiFloat64T();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            x.paramtext = DemarshalSaHpiTextBufferT();
        }

        return x;
    }

    public SaHpiDimiTestParameterValueUnionT DemarshalSaHpiDimiTestParameterValueUnionT( long mod )
    {
        SaHpiDimiTestParameterValueUnionT x = new SaHpiDimiTestParameterValueUnionT();

        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            x.IntValue = DemarshalSaHpiInt32T();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            x.FloatValue = DemarshalSaHpiFloat64T();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            x.FloatValue = DemarshalSaHpiFloat64T();
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            x.FloatValue = DemarshalSaHpiFloat64T();
        }

        return x;
    }

    public SaHpiDimiTestParamsDefinitionT DemarshalSaHpiDimiTestParamsDefinitionT()
    {
        SaHpiDimiTestParamsDefinitionT x = new SaHpiDimiTestParamsDefinitionT();

        x.ParamName = DemarshalByteArray( HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN );
        x.ParamInfo = DemarshalSaHpiTextBufferT();
        x.ParamType = DemarshalSaHpiDimiTestParamTypeT();
        x.MinValue = DemarshalSaHpiDimiTestParameterValueUnionT( x.ParamType );
        x.MaxValue = DemarshalSaHpiDimiTestParameterValueUnionT( x.ParamType );
        x.DefaultParam = DemarshalSaHpiDimiTestParamValueT( x.ParamType );

        return x;
    }

    public SaHpiDimiTestT DemarshalSaHpiDimiTestT()
    {
        SaHpiDimiTestT x = new SaHpiDimiTestT();

        x.TestName = DemarshalSaHpiTextBufferT();
        x.ServiceImpact = DemarshalSaHpiDimiTestServiceImpactT();
        x.EntitiesImpacted = new SaHpiDimiTestAffectedEntityT[HpiConst.SAHPI_DIMITEST_MAX_ENTITIESIMPACTED];
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            x.EntitiesImpacted[i] = DemarshalSaHpiDimiTestAffectedEntityT();
        }
        x.NeedServiceOS = DemarshalSaHpiBoolT();
        x.ServiceOS = DemarshalSaHpiTextBufferT();
        x.ExpectedRunDuration = DemarshalSaHpiTimeT();
        x.TestCapabilities = DemarshalSaHpiDimiTestCapabilityT();
        x.TestParameters = new SaHpiDimiTestParamsDefinitionT[HpiConst.SAHPI_DIMITEST_MAX_PARAMETERS];
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            x.TestParameters[i] = DemarshalSaHpiDimiTestParamsDefinitionT();
        }

        return x;
    }

    public SaHpiDimiTestVariableParamsT DemarshalSaHpiDimiTestVariableParamsT()
    {
        SaHpiDimiTestVariableParamsT x = new SaHpiDimiTestVariableParamsT();

        x.ParamName = DemarshalByteArray( HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN );
        x.ParamType = DemarshalSaHpiDimiTestParamTypeT();
        x.Value = DemarshalSaHpiDimiTestParamValueT( x.ParamType );

        return x;
    }

    public SaHpiDimiInfoT DemarshalSaHpiDimiInfoT()
    {
        SaHpiDimiInfoT x = new SaHpiDimiInfoT();

        x.NumberOfTests = DemarshalSaHpiUint32T();
        x.TestNumUpdateCounter = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiDimiRecT DemarshalSaHpiDimiRecT()
    {
        SaHpiDimiRecT x = new SaHpiDimiRecT();

        x.DimiNum = DemarshalSaHpiDimiNumT();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiSafDefinedSpecInfoT DemarshalSaHpiFumiSafDefinedSpecInfoT()
    {
        SaHpiFumiSafDefinedSpecInfoT x = new SaHpiFumiSafDefinedSpecInfoT();

        x.SpecID = DemarshalSaHpiFumiSafDefinedSpecIdT();
        x.RevisionID = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiOemDefinedSpecInfoT DemarshalSaHpiFumiOemDefinedSpecInfoT()
    {
        SaHpiFumiOemDefinedSpecInfoT x = new SaHpiFumiOemDefinedSpecInfoT();

        x.Mid = DemarshalSaHpiManufacturerIdT();
        x.BodyLength = DemarshalSaHpiUint8T();
        x.Body = DemarshalByteArray( HpiConst.SAHPI_FUMI_MAX_OEM_BODY_LENGTH );

        return x;
    }

    public SaHpiFumiSpecInfoTypeUnionT DemarshalSaHpiFumiSpecInfoTypeUnionT( long mod )
    {
        SaHpiFumiSpecInfoTypeUnionT x = new SaHpiFumiSpecInfoTypeUnionT();

        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            x.SafDefined = DemarshalSaHpiFumiSafDefinedSpecInfoT();
        }
        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            x.OemDefined = DemarshalSaHpiFumiOemDefinedSpecInfoT();
        }

        return x;
    }

    public SaHpiFumiSpecInfoT DemarshalSaHpiFumiSpecInfoT()
    {
        SaHpiFumiSpecInfoT x = new SaHpiFumiSpecInfoT();

        x.SpecInfoType = DemarshalSaHpiFumiSpecInfoTypeT();
        x.SpecInfoTypeUnion = DemarshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoType );

        return x;
    }

    public SaHpiFumiFirmwareInstanceInfoT DemarshalSaHpiFumiFirmwareInstanceInfoT()
    {
        SaHpiFumiFirmwareInstanceInfoT x = new SaHpiFumiFirmwareInstanceInfoT();

        x.InstancePresent = DemarshalSaHpiBoolT();
        x.Identifier = DemarshalSaHpiTextBufferT();
        x.Description = DemarshalSaHpiTextBufferT();
        x.DateTime = DemarshalSaHpiTextBufferT();
        x.MajorVersion = DemarshalSaHpiUint32T();
        x.MinorVersion = DemarshalSaHpiUint32T();
        x.AuxVersion = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiImpactedEntityT DemarshalSaHpiFumiImpactedEntityT()
    {
        SaHpiFumiImpactedEntityT x = new SaHpiFumiImpactedEntityT();

        x.ImpactedEntity = DemarshalSaHpiEntityPathT();
        x.ServiceImpact = DemarshalSaHpiFumiServiceImpactT();

        return x;
    }

    public SaHpiFumiServiceImpactDataT DemarshalSaHpiFumiServiceImpactDataT()
    {
        SaHpiFumiServiceImpactDataT x = new SaHpiFumiServiceImpactDataT();

        x.NumEntities = DemarshalSaHpiUint32T();
        x.ImpactedEntities = new SaHpiFumiImpactedEntityT[HpiConst.SAHPI_FUMI_MAX_ENTITIES_IMPACTED];
        for ( int i = 0; i < HpiConst.SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            x.ImpactedEntities[i] = DemarshalSaHpiFumiImpactedEntityT();
        }

        return x;
    }

    public SaHpiFumiSourceInfoT DemarshalSaHpiFumiSourceInfoT()
    {
        SaHpiFumiSourceInfoT x = new SaHpiFumiSourceInfoT();

        x.SourceUri = DemarshalSaHpiTextBufferT();
        x.SourceStatus = DemarshalSaHpiFumiSourceStatusT();
        x.Identifier = DemarshalSaHpiTextBufferT();
        x.Description = DemarshalSaHpiTextBufferT();
        x.DateTime = DemarshalSaHpiTextBufferT();
        x.MajorVersion = DemarshalSaHpiUint32T();
        x.MinorVersion = DemarshalSaHpiUint32T();
        x.AuxVersion = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiComponentInfoT DemarshalSaHpiFumiComponentInfoT()
    {
        SaHpiFumiComponentInfoT x = new SaHpiFumiComponentInfoT();

        x.EntryId = DemarshalSaHpiEntryIdT();
        x.ComponentId = DemarshalSaHpiUint32T();
        x.MainFwInstance = DemarshalSaHpiFumiFirmwareInstanceInfoT();
        x.ComponentFlags = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiBankInfoT DemarshalSaHpiFumiBankInfoT()
    {
        SaHpiFumiBankInfoT x = new SaHpiFumiBankInfoT();

        x.BankId = DemarshalSaHpiUint8T();
        x.BankSize = DemarshalSaHpiUint32T();
        x.Position = DemarshalSaHpiUint32T();
        x.BankState = DemarshalSaHpiFumiBankStateT();
        x.Identifier = DemarshalSaHpiTextBufferT();
        x.Description = DemarshalSaHpiTextBufferT();
        x.DateTime = DemarshalSaHpiTextBufferT();
        x.MajorVersion = DemarshalSaHpiUint32T();
        x.MinorVersion = DemarshalSaHpiUint32T();
        x.AuxVersion = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiLogicalBankInfoT DemarshalSaHpiFumiLogicalBankInfoT()
    {
        SaHpiFumiLogicalBankInfoT x = new SaHpiFumiLogicalBankInfoT();

        x.FirmwarePersistentLocationCount = DemarshalSaHpiUint8T();
        x.BankStateFlags = DemarshalSaHpiFumiLogicalBankStateFlagsT();
        x.PendingFwInstance = DemarshalSaHpiFumiFirmwareInstanceInfoT();
        x.RollbackFwInstance = DemarshalSaHpiFumiFirmwareInstanceInfoT();

        return x;
    }

    public SaHpiFumiLogicalComponentInfoT DemarshalSaHpiFumiLogicalComponentInfoT()
    {
        SaHpiFumiLogicalComponentInfoT x = new SaHpiFumiLogicalComponentInfoT();

        x.EntryId = DemarshalSaHpiEntryIdT();
        x.ComponentId = DemarshalSaHpiUint32T();
        x.PendingFwInstance = DemarshalSaHpiFumiFirmwareInstanceInfoT();
        x.RollbackFwInstance = DemarshalSaHpiFumiFirmwareInstanceInfoT();
        x.ComponentFlags = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiRecT DemarshalSaHpiFumiRecT()
    {
        SaHpiFumiRecT x = new SaHpiFumiRecT();

        x.Num = DemarshalSaHpiFumiNumT();
        x.AccessProt = DemarshalSaHpiFumiProtocolT();
        x.Capability = DemarshalSaHpiFumiCapabilityT();
        x.NumBanks = DemarshalSaHpiUint8T();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiResourceEventT DemarshalSaHpiResourceEventT()
    {
        SaHpiResourceEventT x = new SaHpiResourceEventT();

        x.ResourceEventType = DemarshalSaHpiResourceEventTypeT();

        return x;
    }

    public SaHpiDomainEventT DemarshalSaHpiDomainEventT()
    {
        SaHpiDomainEventT x = new SaHpiDomainEventT();

        x.Type = DemarshalSaHpiDomainEventTypeT();
        x.DomainId = DemarshalSaHpiDomainIdT();

        return x;
    }

    public SaHpiSensorEventT DemarshalSaHpiSensorEventT()
    {
        SaHpiSensorEventT x = new SaHpiSensorEventT();

        x.SensorNum = DemarshalSaHpiSensorNumT();
        x.SensorType = DemarshalSaHpiSensorTypeT();
        x.EventCategory = DemarshalSaHpiEventCategoryT();
        x.Assertion = DemarshalSaHpiBoolT();
        x.EventState = DemarshalSaHpiEventStateT();
        x.OptionalDataPresent = DemarshalSaHpiSensorOptionalDataT();
        x.TriggerReading = DemarshalSaHpiSensorReadingT();
        x.TriggerThreshold = DemarshalSaHpiSensorReadingT();
        x.PreviousState = DemarshalSaHpiEventStateT();
        x.CurrentState = DemarshalSaHpiEventStateT();
        x.Oem = DemarshalSaHpiUint32T();
        x.SensorSpecific = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiSensorEnableChangeEventT DemarshalSaHpiSensorEnableChangeEventT()
    {
        SaHpiSensorEnableChangeEventT x = new SaHpiSensorEnableChangeEventT();

        x.SensorNum = DemarshalSaHpiSensorNumT();
        x.SensorType = DemarshalSaHpiSensorTypeT();
        x.EventCategory = DemarshalSaHpiEventCategoryT();
        x.SensorEnable = DemarshalSaHpiBoolT();
        x.SensorEventEnable = DemarshalSaHpiBoolT();
        x.AssertEventMask = DemarshalSaHpiEventStateT();
        x.DeassertEventMask = DemarshalSaHpiEventStateT();
        x.OptionalDataPresent = DemarshalSaHpiSensorEnableOptDataT();
        x.CurrentState = DemarshalSaHpiEventStateT();
        x.CriticalAlarms = DemarshalSaHpiEventStateT();
        x.MajorAlarms = DemarshalSaHpiEventStateT();
        x.MinorAlarms = DemarshalSaHpiEventStateT();

        return x;
    }

    public SaHpiHotSwapEventT DemarshalSaHpiHotSwapEventT()
    {
        SaHpiHotSwapEventT x = new SaHpiHotSwapEventT();

        x.HotSwapState = DemarshalSaHpiHsStateT();
        x.PreviousHotSwapState = DemarshalSaHpiHsStateT();
        x.CauseOfStateChange = DemarshalSaHpiHsCauseOfStateChangeT();

        return x;
    }

    public SaHpiWatchdogEventT DemarshalSaHpiWatchdogEventT()
    {
        SaHpiWatchdogEventT x = new SaHpiWatchdogEventT();

        x.WatchdogNum = DemarshalSaHpiWatchdogNumT();
        x.WatchdogAction = DemarshalSaHpiWatchdogActionEventT();
        x.WatchdogPreTimerAction = DemarshalSaHpiWatchdogPretimerInterruptT();
        x.WatchdogUse = DemarshalSaHpiWatchdogTimerUseT();

        return x;
    }

    public SaHpiHpiSwEventT DemarshalSaHpiHpiSwEventT()
    {
        SaHpiHpiSwEventT x = new SaHpiHpiSwEventT();

        x.MId = DemarshalSaHpiManufacturerIdT();
        x.Type = DemarshalSaHpiSwEventTypeT();
        x.EventData = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiOemEventT DemarshalSaHpiOemEventT()
    {
        SaHpiOemEventT x = new SaHpiOemEventT();

        x.MId = DemarshalSaHpiManufacturerIdT();
        x.OemEventData = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiUserEventT DemarshalSaHpiUserEventT()
    {
        SaHpiUserEventT x = new SaHpiUserEventT();

        x.UserEventData = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiDimiEventT DemarshalSaHpiDimiEventT()
    {
        SaHpiDimiEventT x = new SaHpiDimiEventT();

        x.DimiNum = DemarshalSaHpiDimiNumT();
        x.TestNum = DemarshalSaHpiDimiTestNumT();
        x.DimiTestRunStatus = DemarshalSaHpiDimiTestRunStatusT();
        x.DimiTestPercentCompleted = DemarshalSaHpiDimiTestPercentCompletedT();

        return x;
    }

    public SaHpiDimiUpdateEventT DemarshalSaHpiDimiUpdateEventT()
    {
        SaHpiDimiUpdateEventT x = new SaHpiDimiUpdateEventT();

        x.DimiNum = DemarshalSaHpiDimiNumT();

        return x;
    }

    public SaHpiFumiEventT DemarshalSaHpiFumiEventT()
    {
        SaHpiFumiEventT x = new SaHpiFumiEventT();

        x.FumiNum = DemarshalSaHpiFumiNumT();
        x.BankNum = DemarshalSaHpiUint8T();
        x.UpgradeStatus = DemarshalSaHpiFumiUpgradeStatusT();

        return x;
    }

    public SaHpiEventUnionT DemarshalSaHpiEventUnionT( long mod )
    {
        SaHpiEventUnionT x = new SaHpiEventUnionT();

        if ( mod == HpiConst.SAHPI_ET_RESOURCE ) {
            x.ResourceEvent = DemarshalSaHpiResourceEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_DOMAIN ) {
            x.DomainEvent = DemarshalSaHpiDomainEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR ) {
            x.SensorEvent = DemarshalSaHpiSensorEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            x.SensorEnableChangeEvent = DemarshalSaHpiSensorEnableChangeEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_HOTSWAP ) {
            x.HotSwapEvent = DemarshalSaHpiHotSwapEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_WATCHDOG ) {
            x.WatchdogEvent = DemarshalSaHpiWatchdogEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_HPI_SW ) {
            x.HpiSwEvent = DemarshalSaHpiHpiSwEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_OEM ) {
            x.OemEvent = DemarshalSaHpiOemEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_USER ) {
            x.UserEvent = DemarshalSaHpiUserEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI ) {
            x.DimiEvent = DemarshalSaHpiDimiEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI_UPDATE ) {
            x.DimiUpdateEvent = DemarshalSaHpiDimiUpdateEventT();
        }
        if ( mod == HpiConst.SAHPI_ET_FUMI ) {
            x.FumiEvent = DemarshalSaHpiFumiEventT();
        }

        return x;
    }

    public SaHpiEventT DemarshalSaHpiEventT()
    {
        SaHpiEventT x = new SaHpiEventT();

        x.Source = DemarshalSaHpiResourceIdT();
        x.EventType = DemarshalSaHpiEventTypeT();
        x.Timestamp = DemarshalSaHpiTimeT();
        x.Severity = DemarshalSaHpiSeverityT();
        x.EventDataUnion = DemarshalSaHpiEventUnionT( x.EventType );

        return x;
    }

    public SaHpiNameT DemarshalSaHpiNameT()
    {
        SaHpiNameT x = new SaHpiNameT();

        x.Length = DemarshalSaHpiUint16T();
        x.Value = DemarshalByteArray( HpiConst.SA_HPI_MAX_NAME_LENGTH );

        return x;
    }

    public SaHpiConditionT DemarshalSaHpiConditionT()
    {
        SaHpiConditionT x = new SaHpiConditionT();

        x.Type = DemarshalSaHpiStatusCondTypeT();
        x.Entity = DemarshalSaHpiEntityPathT();
        x.DomainId = DemarshalSaHpiDomainIdT();
        x.ResourceId = DemarshalSaHpiResourceIdT();
        x.SensorNum = DemarshalSaHpiSensorNumT();
        x.EventState = DemarshalSaHpiEventStateT();
        x.Name = DemarshalSaHpiNameT();
        x.Mid = DemarshalSaHpiManufacturerIdT();
        x.Data = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiAnnouncementT DemarshalSaHpiAnnouncementT()
    {
        SaHpiAnnouncementT x = new SaHpiAnnouncementT();

        x.EntryId = DemarshalSaHpiEntryIdT();
        x.Timestamp = DemarshalSaHpiTimeT();
        x.AddedByUser = DemarshalSaHpiBoolT();
        x.Severity = DemarshalSaHpiSeverityT();
        x.Acknowledged = DemarshalSaHpiBoolT();
        x.StatusCond = DemarshalSaHpiConditionT();

        return x;
    }

    public SaHpiAnnunciatorRecT DemarshalSaHpiAnnunciatorRecT()
    {
        SaHpiAnnunciatorRecT x = new SaHpiAnnunciatorRecT();

        x.AnnunciatorNum = DemarshalSaHpiAnnunciatorNumT();
        x.AnnunciatorType = DemarshalSaHpiAnnunciatorTypeT();
        x.ModeReadOnly = DemarshalSaHpiBoolT();
        x.MaxConditions = DemarshalSaHpiUint32T();
        x.Oem = DemarshalSaHpiUint32T();

        return x;
    }

    public SaHpiRdrTypeUnionT DemarshalSaHpiRdrTypeUnionT( long mod )
    {
        SaHpiRdrTypeUnionT x = new SaHpiRdrTypeUnionT();

        if ( mod == HpiConst.SAHPI_CTRL_RDR ) {
            x.CtrlRec = DemarshalSaHpiCtrlRecT();
        }
        if ( mod == HpiConst.SAHPI_SENSOR_RDR ) {
            x.SensorRec = DemarshalSaHpiSensorRecT();
        }
        if ( mod == HpiConst.SAHPI_INVENTORY_RDR ) {
            x.InventoryRec = DemarshalSaHpiInventoryRecT();
        }
        if ( mod == HpiConst.SAHPI_WATCHDOG_RDR ) {
            x.WatchdogRec = DemarshalSaHpiWatchdogRecT();
        }
        if ( mod == HpiConst.SAHPI_ANNUNCIATOR_RDR ) {
            x.AnnunciatorRec = DemarshalSaHpiAnnunciatorRecT();
        }
        if ( mod == HpiConst.SAHPI_DIMI_RDR ) {
            x.DimiRec = DemarshalSaHpiDimiRecT();
        }
        if ( mod == HpiConst.SAHPI_FUMI_RDR ) {
            x.FumiRec = DemarshalSaHpiFumiRecT();
        }

        return x;
    }

    public SaHpiRdrT DemarshalSaHpiRdrT()
    {
        SaHpiRdrT x = new SaHpiRdrT();

        x.RecordId = DemarshalSaHpiEntryIdT();
        x.RdrType = DemarshalSaHpiRdrTypeT();
        x.Entity = DemarshalSaHpiEntityPathT();
        x.IsFru = DemarshalSaHpiBoolT();
        x.RdrTypeUnion = DemarshalSaHpiRdrTypeUnionT( x.RdrType );
        x.IdString = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiLoadIdT DemarshalSaHpiLoadIdT()
    {
        SaHpiLoadIdT x = new SaHpiLoadIdT();

        x.LoadNumber = DemarshalSaHpiLoadNumberT();
        x.LoadName = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiResourceInfoT DemarshalSaHpiResourceInfoT()
    {
        SaHpiResourceInfoT x = new SaHpiResourceInfoT();

        x.ResourceRev = DemarshalSaHpiUint8T();
        x.SpecificVer = DemarshalSaHpiUint8T();
        x.DeviceSupport = DemarshalSaHpiUint8T();
        x.ManufacturerId = DemarshalSaHpiManufacturerIdT();
        x.ProductId = DemarshalSaHpiUint16T();
        x.FirmwareMajorRev = DemarshalSaHpiUint8T();
        x.FirmwareMinorRev = DemarshalSaHpiUint8T();
        x.AuxFirmwareRev = DemarshalSaHpiUint8T();
        x.Guid = DemarshalByteArray( HpiConst.SAHPI_GUID_LENGTH );

        return x;
    }

    public SaHpiRptEntryT DemarshalSaHpiRptEntryT()
    {
        SaHpiRptEntryT x = new SaHpiRptEntryT();

        x.EntryId = DemarshalSaHpiEntryIdT();
        x.ResourceId = DemarshalSaHpiResourceIdT();
        x.ResourceInfo = DemarshalSaHpiResourceInfoT();
        x.ResourceEntity = DemarshalSaHpiEntityPathT();
        x.ResourceCapabilities = DemarshalSaHpiCapabilitiesT();
        x.HotSwapCapabilities = DemarshalSaHpiHsCapabilitiesT();
        x.ResourceSeverity = DemarshalSaHpiSeverityT();
        x.ResourceFailed = DemarshalSaHpiBoolT();
        x.ResourceTag = DemarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiDomainInfoT DemarshalSaHpiDomainInfoT()
    {
        SaHpiDomainInfoT x = new SaHpiDomainInfoT();

        x.DomainId = DemarshalSaHpiDomainIdT();
        x.DomainCapabilities = DemarshalSaHpiDomainCapabilitiesT();
        x.IsPeer = DemarshalSaHpiBoolT();
        x.DomainTag = DemarshalSaHpiTextBufferT();
        x.DrtUpdateCount = DemarshalSaHpiUint32T();
        x.DrtUpdateTimestamp = DemarshalSaHpiTimeT();
        x.RptUpdateCount = DemarshalSaHpiUint32T();
        x.RptUpdateTimestamp = DemarshalSaHpiTimeT();
        x.DatUpdateCount = DemarshalSaHpiUint32T();
        x.DatUpdateTimestamp = DemarshalSaHpiTimeT();
        x.ActiveAlarms = DemarshalSaHpiUint32T();
        x.CriticalAlarms = DemarshalSaHpiUint32T();
        x.MajorAlarms = DemarshalSaHpiUint32T();
        x.MinorAlarms = DemarshalSaHpiUint32T();
        x.DatUserAlarmLimit = DemarshalSaHpiUint32T();
        x.DatOverflow = DemarshalSaHpiBoolT();
        x.Guid = DemarshalByteArray( HpiConst.SAHPI_GUID_LENGTH );

        return x;
    }

    public SaHpiDrtEntryT DemarshalSaHpiDrtEntryT()
    {
        SaHpiDrtEntryT x = new SaHpiDrtEntryT();

        x.EntryId = DemarshalSaHpiEntryIdT();
        x.DomainId = DemarshalSaHpiDomainIdT();
        x.IsPeer = DemarshalSaHpiBoolT();

        return x;
    }

    public SaHpiAlarmT DemarshalSaHpiAlarmT()
    {
        SaHpiAlarmT x = new SaHpiAlarmT();

        x.AlarmId = DemarshalSaHpiAlarmIdT();
        x.Timestamp = DemarshalSaHpiTimeT();
        x.Severity = DemarshalSaHpiSeverityT();
        x.Acknowledged = DemarshalSaHpiBoolT();
        x.AlarmCond = DemarshalSaHpiConditionT();

        return x;
    }

    public SaHpiEventLogInfoT DemarshalSaHpiEventLogInfoT()
    {
        SaHpiEventLogInfoT x = new SaHpiEventLogInfoT();

        x.Entries = DemarshalSaHpiUint32T();
        x.Size = DemarshalSaHpiUint32T();
        x.UserEventMaxSize = DemarshalSaHpiUint32T();
        x.UpdateTimestamp = DemarshalSaHpiTimeT();
        x.CurrentTime = DemarshalSaHpiTimeT();
        x.Enabled = DemarshalSaHpiBoolT();
        x.OverflowFlag = DemarshalSaHpiBoolT();
        x.OverflowResetable = DemarshalSaHpiBoolT();
        x.OverflowAction = DemarshalSaHpiEventLogOverflowActionT();

        return x;
    }

    public SaHpiEventLogEntryT DemarshalSaHpiEventLogEntryT()
    {
        SaHpiEventLogEntryT x = new SaHpiEventLogEntryT();

        x.EntryId = DemarshalSaHpiEventLogEntryIdT();
        x.Timestamp = DemarshalSaHpiTimeT();
        x.Event = DemarshalSaHpiEventT();

        return x;
    }

};


}; // namespace openhpi
}; // namespace org

