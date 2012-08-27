/*      -*- java -*-
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

package org.openhpi;

import static org.openhpi.HpiDataTypes.*;


/**********************************************************
 * HPI Marshal (auto-generated)
 *********************************************************/
class HpiMarshalGen extends HpiMarshalCore
{
    /**********************************************************
     * Marshal: For HPI Simple Data Types
     *********************************************************/
    public void marshalSaHpiBoolT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiManufacturerIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiVersionT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaErrorT( long x )
    {
        marshalSaHpiInt32T( x );
    }

    public void marshalSaHpiDomainIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiSessionIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiResourceIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiEntryIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiTimeT( long x )
    {
        marshalSaHpiInt64T( x );
    }

    public void marshalSaHpiTimeoutT( long x )
    {
        marshalSaHpiInt64T( x );
    }

    public void marshalSaHpiInstrumentIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiEntityLocationT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiEventCategoryT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiEventStateT( long x )
    {
        marshalSaHpiUint16T( x );
    }

    public void marshalSaHpiSensorNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiSensorRangeFlagsT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiSensorThdMaskT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiCtrlNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiCtrlStateDiscreteT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiCtrlStateAnalogT( long x )
    {
        marshalSaHpiInt32T( x );
    }

    public void marshalSaHpiTxtLineNumT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiIdrIdT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiWatchdogNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiWatchdogExpFlagsT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiDimiNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiDimiTestCapabilityT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiDimiTestNumT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiDimiTestPercentCompletedT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiFumiNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiBankNumT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiFumiLogicalBankStateFlagsT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiFumiProtocolT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiFumiCapabilityT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiSensorOptionalDataT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiSensorEnableOptDataT( long x )
    {
        marshalSaHpiUint8T( x );
    }

    public void marshalSaHpiEvtQueueStatusT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiAnnunciatorNumT( long x )
    {
        marshalSaHpiInstrumentIdT( x );
    }

    public void marshalSaHpiLoadNumberT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiCapabilitiesT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiHsCapabilitiesT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiDomainCapabilitiesT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiAlarmIdT( long x )
    {
        marshalSaHpiEntryIdT( x );
    }

    public void marshalSaHpiEventLogCapabilitiesT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiEventLogEntryIdT( long x )
    {
        marshalSaHpiUint32T( x );
    }

    public void marshalSaHpiLanguageT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiTextTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiEntityTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorReadingTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorEventMaskActionT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorUnitsT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorModUnitUseT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSensorEventCtrlT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiCtrlTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiCtrlStateDigitalT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiCtrlModeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiCtrlOutputTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiIdrAreaTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiIdrFieldTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiWatchdogActionT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiWatchdogActionEventT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiWatchdogPretimerInterruptT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiWatchdogTimerUseT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDimiTestServiceImpactT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDimiTestRunStatusT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDimiTestErrCodeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDimiTestParamTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDimiReadyT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiSpecInfoTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiSafDefinedSpecIdT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiServiceImpactT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiSourceStatusT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiBankStateT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiFumiUpgradeStatusT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiHsIndicatorStateT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiHsActionT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiHsStateT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiHsCauseOfStateChangeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSeverityT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiResourceEventTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiDomainEventTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiSwEventTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiEventTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiStatusCondTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiAnnunciatorModeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiAnnunciatorTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiRdrTypeT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiParmActionT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiResetActionT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiPowerStateT( long x )
    {
        marshalEnum( x );
    }

    public void marshalSaHpiEventLogOverflowActionT( long x )
    {
        marshalEnum( x );
    }

    /**********************************************************
     * Marshal: For HPI Structs and Unions
     *********************************************************/
    public void marshalSaHpiTextBufferT( SaHpiTextBufferT x )
    {
        marshalSaHpiTextTypeT( x.DataType );
        marshalSaHpiLanguageT( x.Language );
        marshalSaHpiUint8T( x.DataLength );
        marshalByteArray( x.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH );
    }

    public void marshalSaHpiEntityT( SaHpiEntityT x )
    {
        marshalSaHpiEntityTypeT( x.EntityType );
        marshalSaHpiEntityLocationT( x.EntityLocation );
    }

    public void marshalSaHpiEntityPathT( SaHpiEntityPathT x )
    {
        for ( int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            marshalSaHpiEntityT( x.Entry[i] );
        }
    }

    public void marshalSaHpiSensorReadingUnionT( SaHpiSensorReadingUnionT x, long mod )
    {
        if ( mod == SAHPI_SENSOR_READING_TYPE_INT64 ) {
            marshalSaHpiInt64T( x.SensorInt64 );
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_UINT64 ) {
            marshalSaHpiUint64T( x.SensorUint64 );
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
            marshalSaHpiFloat64T( x.SensorFloat64 );
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            marshalByteArray( x.SensorBuffer, SAHPI_SENSOR_BUFFER_LENGTH );
        }
    }

    public void marshalSaHpiSensorReadingT( SaHpiSensorReadingT x )
    {
        marshalSaHpiBoolT( x.IsSupported );
        marshalSaHpiSensorReadingTypeT( x.Type );
        marshalSaHpiSensorReadingUnionT( x.Value, x.Type );
    }

    public void marshalSaHpiSensorThresholdsT( SaHpiSensorThresholdsT x )
    {
        marshalSaHpiSensorReadingT( x.LowCritical );
        marshalSaHpiSensorReadingT( x.LowMajor );
        marshalSaHpiSensorReadingT( x.LowMinor );
        marshalSaHpiSensorReadingT( x.UpCritical );
        marshalSaHpiSensorReadingT( x.UpMajor );
        marshalSaHpiSensorReadingT( x.UpMinor );
        marshalSaHpiSensorReadingT( x.PosThdHysteresis );
        marshalSaHpiSensorReadingT( x.NegThdHysteresis );
    }

    public void marshalSaHpiSensorRangeT( SaHpiSensorRangeT x )
    {
        marshalSaHpiSensorRangeFlagsT( x.Flags );
        marshalSaHpiSensorReadingT( x.Max );
        marshalSaHpiSensorReadingT( x.Min );
        marshalSaHpiSensorReadingT( x.Nominal );
        marshalSaHpiSensorReadingT( x.NormalMax );
        marshalSaHpiSensorReadingT( x.NormalMin );
    }

    public void marshalSaHpiSensorDataFormatT( SaHpiSensorDataFormatT x )
    {
        marshalSaHpiBoolT( x.IsSupported );
        marshalSaHpiSensorReadingTypeT( x.ReadingType );
        marshalSaHpiSensorUnitsT( x.BaseUnits );
        marshalSaHpiSensorUnitsT( x.ModifierUnits );
        marshalSaHpiSensorModUnitUseT( x.ModifierUse );
        marshalSaHpiBoolT( x.Percentage );
        marshalSaHpiSensorRangeT( x.Range );
        marshalSaHpiFloat64T( x.AccuracyFactor );
    }

    public void marshalSaHpiSensorThdDefnT( SaHpiSensorThdDefnT x )
    {
        marshalSaHpiBoolT( x.IsAccessible );
        marshalSaHpiSensorThdMaskT( x.ReadThold );
        marshalSaHpiSensorThdMaskT( x.WriteThold );
        marshalSaHpiBoolT( x.Nonlinear );
    }

    public void marshalSaHpiSensorRecT( SaHpiSensorRecT x )
    {
        marshalSaHpiSensorNumT( x.Num );
        marshalSaHpiSensorTypeT( x.Type );
        marshalSaHpiEventCategoryT( x.Category );
        marshalSaHpiBoolT( x.EnableCtrl );
        marshalSaHpiSensorEventCtrlT( x.EventCtrl );
        marshalSaHpiEventStateT( x.Events );
        marshalSaHpiSensorDataFormatT( x.DataFormat );
        marshalSaHpiSensorThdDefnT( x.ThresholdDefn );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiCtrlStateStreamT( SaHpiCtrlStateStreamT x )
    {
        marshalSaHpiBoolT( x.Repeat );
        marshalSaHpiUint32T( x.StreamLength );
        marshalByteArray( x.Stream, SAHPI_CTRL_MAX_STREAM_LENGTH );
    }

    public void marshalSaHpiCtrlStateTextT( SaHpiCtrlStateTextT x )
    {
        marshalSaHpiTxtLineNumT( x.Line );
        marshalSaHpiTextBufferT( x.Text );
    }

    public void marshalSaHpiCtrlStateOemT( SaHpiCtrlStateOemT x )
    {
        marshalSaHpiManufacturerIdT( x.MId );
        marshalSaHpiUint8T( x.BodyLength );
        marshalByteArray( x.Body, SAHPI_CTRL_MAX_OEM_BODY_LENGTH );
    }

    public void marshalSaHpiCtrlStateUnionT( SaHpiCtrlStateUnionT x, long mod )
    {
        if ( mod == SAHPI_CTRL_TYPE_DIGITAL ) {
            marshalSaHpiCtrlStateDigitalT( x.Digital );
        }
        if ( mod == SAHPI_CTRL_TYPE_DISCRETE ) {
            marshalSaHpiCtrlStateDiscreteT( x.Discrete );
        }
        if ( mod == SAHPI_CTRL_TYPE_ANALOG ) {
            marshalSaHpiCtrlStateAnalogT( x.Analog );
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            marshalSaHpiCtrlStateStreamT( x.Stream );
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            marshalSaHpiCtrlStateTextT( x.Text );
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            marshalSaHpiCtrlStateOemT( x.Oem );
        }
    }

    public void marshalSaHpiCtrlStateT( SaHpiCtrlStateT x )
    {
        marshalSaHpiCtrlTypeT( x.Type );
        marshalSaHpiCtrlStateUnionT( x.StateUnion, x.Type );
    }

    public void marshalSaHpiCtrlRecDigitalT( SaHpiCtrlRecDigitalT x )
    {
        marshalSaHpiCtrlStateDigitalT( x.Default );
    }

    public void marshalSaHpiCtrlRecDiscreteT( SaHpiCtrlRecDiscreteT x )
    {
        marshalSaHpiCtrlStateDiscreteT( x.Default );
    }

    public void marshalSaHpiCtrlRecAnalogT( SaHpiCtrlRecAnalogT x )
    {
        marshalSaHpiCtrlStateAnalogT( x.Min );
        marshalSaHpiCtrlStateAnalogT( x.Max );
        marshalSaHpiCtrlStateAnalogT( x.Default );
    }

    public void marshalSaHpiCtrlRecStreamT( SaHpiCtrlRecStreamT x )
    {
        marshalSaHpiCtrlStateStreamT( x.Default );
    }

    public void marshalSaHpiCtrlRecTextT( SaHpiCtrlRecTextT x )
    {
        marshalSaHpiUint8T( x.MaxChars );
        marshalSaHpiUint8T( x.MaxLines );
        marshalSaHpiLanguageT( x.Language );
        marshalSaHpiTextTypeT( x.DataType );
        marshalSaHpiCtrlStateTextT( x.Default );
    }

    public void marshalSaHpiCtrlRecOemT( SaHpiCtrlRecOemT x )
    {
        marshalSaHpiManufacturerIdT( x.MId );
        marshalByteArray( x.ConfigData, SAHPI_CTRL_OEM_CONFIG_LENGTH );
        marshalSaHpiCtrlStateOemT( x.Default );
    }

    public void marshalSaHpiCtrlRecUnionT( SaHpiCtrlRecUnionT x, long mod )
    {
        if ( mod == SAHPI_CTRL_TYPE_DIGITAL ) {
            marshalSaHpiCtrlRecDigitalT( x.Digital );
        }
        if ( mod == SAHPI_CTRL_TYPE_DISCRETE ) {
            marshalSaHpiCtrlRecDiscreteT( x.Discrete );
        }
        if ( mod == SAHPI_CTRL_TYPE_ANALOG ) {
            marshalSaHpiCtrlRecAnalogT( x.Analog );
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            marshalSaHpiCtrlRecStreamT( x.Stream );
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            marshalSaHpiCtrlRecTextT( x.Text );
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            marshalSaHpiCtrlRecOemT( x.Oem );
        }
    }

    public void marshalSaHpiCtrlDefaultModeT( SaHpiCtrlDefaultModeT x )
    {
        marshalSaHpiCtrlModeT( x.Mode );
        marshalSaHpiBoolT( x.ReadOnly );
    }

    public void marshalSaHpiCtrlRecT( SaHpiCtrlRecT x )
    {
        marshalSaHpiCtrlNumT( x.Num );
        marshalSaHpiCtrlOutputTypeT( x.OutputType );
        marshalSaHpiCtrlTypeT( x.Type );
        marshalSaHpiCtrlRecUnionT( x.TypeUnion, x.Type );
        marshalSaHpiCtrlDefaultModeT( x.DefaultMode );
        marshalSaHpiBoolT( x.WriteOnly );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiIdrFieldT( SaHpiIdrFieldT x )
    {
        marshalSaHpiEntryIdT( x.AreaId );
        marshalSaHpiEntryIdT( x.FieldId );
        marshalSaHpiIdrFieldTypeT( x.Type );
        marshalSaHpiBoolT( x.ReadOnly );
        marshalSaHpiTextBufferT( x.Field );
    }

    public void marshalSaHpiIdrAreaHeaderT( SaHpiIdrAreaHeaderT x )
    {
        marshalSaHpiEntryIdT( x.AreaId );
        marshalSaHpiIdrAreaTypeT( x.Type );
        marshalSaHpiBoolT( x.ReadOnly );
        marshalSaHpiUint32T( x.NumFields );
    }

    public void marshalSaHpiIdrInfoT( SaHpiIdrInfoT x )
    {
        marshalSaHpiIdrIdT( x.IdrId );
        marshalSaHpiUint32T( x.UpdateCount );
        marshalSaHpiBoolT( x.ReadOnly );
        marshalSaHpiUint32T( x.NumAreas );
    }

    public void marshalSaHpiInventoryRecT( SaHpiInventoryRecT x )
    {
        marshalSaHpiIdrIdT( x.IdrId );
        marshalSaHpiBoolT( x.Persistent );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiWatchdogT( SaHpiWatchdogT x )
    {
        marshalSaHpiBoolT( x.Log );
        marshalSaHpiBoolT( x.Running );
        marshalSaHpiWatchdogTimerUseT( x.TimerUse );
        marshalSaHpiWatchdogActionT( x.TimerAction );
        marshalSaHpiWatchdogPretimerInterruptT( x.PretimerInterrupt );
        marshalSaHpiUint32T( x.PreTimeoutInterval );
        marshalSaHpiWatchdogExpFlagsT( x.TimerUseExpFlags );
        marshalSaHpiUint32T( x.InitialCount );
        marshalSaHpiUint32T( x.PresentCount );
    }

    public void marshalSaHpiWatchdogRecT( SaHpiWatchdogRecT x )
    {
        marshalSaHpiWatchdogNumT( x.WatchdogNum );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiDimiTestAffectedEntityT( SaHpiDimiTestAffectedEntityT x )
    {
        marshalSaHpiEntityPathT( x.EntityImpacted );
        marshalSaHpiDimiTestServiceImpactT( x.ServiceImpact );
    }

    public void marshalSaHpiDimiTestResultsT( SaHpiDimiTestResultsT x )
    {
        marshalSaHpiTimeT( x.ResultTimeStamp );
        marshalSaHpiTimeoutT( x.RunDuration );
        marshalSaHpiDimiTestRunStatusT( x.LastRunStatus );
        marshalSaHpiDimiTestErrCodeT( x.TestErrorCode );
        marshalSaHpiTextBufferT( x.TestResultString );
        marshalSaHpiBoolT( x.TestResultStringIsURI );
    }

    public void marshalSaHpiDimiTestParamValueT( SaHpiDimiTestParamValueT x, long mod )
    {
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            marshalSaHpiInt32T( x.paramint );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            marshalSaHpiBoolT( x.parambool );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            marshalSaHpiFloat64T( x.paramfloat );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            marshalSaHpiTextBufferT( x.paramtext );
        }
    }

    public void marshalSaHpiDimiTestParameterValueUnionT( SaHpiDimiTestParameterValueUnionT x, long mod )
    {
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            marshalSaHpiInt32T( x.IntValue );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            marshalSaHpiFloat64T( x.FloatValue );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            marshalSaHpiFloat64T( x.FloatValue );
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            marshalSaHpiFloat64T( x.FloatValue );
        }
    }

    public void marshalSaHpiDimiTestParamsDefinitionT( SaHpiDimiTestParamsDefinitionT x )
    {
        marshalByteArray( x.ParamName, SAHPI_DIMITEST_PARAM_NAME_LEN );
        marshalSaHpiTextBufferT( x.ParamInfo );
        marshalSaHpiDimiTestParamTypeT( x.ParamType );
        marshalSaHpiDimiTestParameterValueUnionT( x.MinValue, x.ParamType );
        marshalSaHpiDimiTestParameterValueUnionT( x.MaxValue, x.ParamType );
        marshalSaHpiDimiTestParamValueT( x.DefaultParam, x.ParamType );
    }

    public void marshalSaHpiDimiTestT( SaHpiDimiTestT x )
    {
        marshalSaHpiTextBufferT( x.TestName );
        marshalSaHpiDimiTestServiceImpactT( x.ServiceImpact );
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            marshalSaHpiDimiTestAffectedEntityT( x.EntitiesImpacted[i] );
        }
        marshalSaHpiBoolT( x.NeedServiceOS );
        marshalSaHpiTextBufferT( x.ServiceOS );
        marshalSaHpiTimeT( x.ExpectedRunDuration );
        marshalSaHpiDimiTestCapabilityT( x.TestCapabilities );
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            marshalSaHpiDimiTestParamsDefinitionT( x.TestParameters[i] );
        }
    }

    public void marshalSaHpiDimiTestVariableParamsT( SaHpiDimiTestVariableParamsT x )
    {
        marshalByteArray( x.ParamName, SAHPI_DIMITEST_PARAM_NAME_LEN );
        marshalSaHpiDimiTestParamTypeT( x.ParamType );
        marshalSaHpiDimiTestParamValueT( x.Value, x.ParamType );
    }

    public void marshalSaHpiDimiInfoT( SaHpiDimiInfoT x )
    {
        marshalSaHpiUint32T( x.NumberOfTests );
        marshalSaHpiUint32T( x.TestNumUpdateCounter );
    }

    public void marshalSaHpiDimiRecT( SaHpiDimiRecT x )
    {
        marshalSaHpiDimiNumT( x.DimiNum );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiFumiSafDefinedSpecInfoT( SaHpiFumiSafDefinedSpecInfoT x )
    {
        marshalSaHpiFumiSafDefinedSpecIdT( x.SpecID );
        marshalSaHpiUint32T( x.RevisionID );
    }

    public void marshalSaHpiFumiOemDefinedSpecInfoT( SaHpiFumiOemDefinedSpecInfoT x )
    {
        marshalSaHpiManufacturerIdT( x.Mid );
        marshalSaHpiUint8T( x.BodyLength );
        marshalByteArray( x.Body, SAHPI_FUMI_MAX_OEM_BODY_LENGTH );
    }

    public void marshalSaHpiFumiSpecInfoTypeUnionT( SaHpiFumiSpecInfoTypeUnionT x, long mod )
    {
        if ( mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            marshalSaHpiFumiSafDefinedSpecInfoT( x.SafDefined );
        }
        if ( mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            marshalSaHpiFumiOemDefinedSpecInfoT( x.OemDefined );
        }
    }

    public void marshalSaHpiFumiSpecInfoT( SaHpiFumiSpecInfoT x )
    {
        marshalSaHpiFumiSpecInfoTypeT( x.SpecInfoType );
        marshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoTypeUnion, x.SpecInfoType );
    }

    public void marshalSaHpiFumiFirmwareInstanceInfoT( SaHpiFumiFirmwareInstanceInfoT x )
    {
        marshalSaHpiBoolT( x.InstancePresent );
        marshalSaHpiTextBufferT( x.Identifier );
        marshalSaHpiTextBufferT( x.Description );
        marshalSaHpiTextBufferT( x.DateTime );
        marshalSaHpiUint32T( x.MajorVersion );
        marshalSaHpiUint32T( x.MinorVersion );
        marshalSaHpiUint32T( x.AuxVersion );
    }

    public void marshalSaHpiFumiImpactedEntityT( SaHpiFumiImpactedEntityT x )
    {
        marshalSaHpiEntityPathT( x.ImpactedEntity );
        marshalSaHpiFumiServiceImpactT( x.ServiceImpact );
    }

    public void marshalSaHpiFumiServiceImpactDataT( SaHpiFumiServiceImpactDataT x )
    {
        marshalSaHpiUint32T( x.NumEntities );
        for ( int i = 0; i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            marshalSaHpiFumiImpactedEntityT( x.ImpactedEntities[i] );
        }
    }

    public void marshalSaHpiFumiSourceInfoT( SaHpiFumiSourceInfoT x )
    {
        marshalSaHpiTextBufferT( x.SourceUri );
        marshalSaHpiFumiSourceStatusT( x.SourceStatus );
        marshalSaHpiTextBufferT( x.Identifier );
        marshalSaHpiTextBufferT( x.Description );
        marshalSaHpiTextBufferT( x.DateTime );
        marshalSaHpiUint32T( x.MajorVersion );
        marshalSaHpiUint32T( x.MinorVersion );
        marshalSaHpiUint32T( x.AuxVersion );
    }

    public void marshalSaHpiFumiComponentInfoT( SaHpiFumiComponentInfoT x )
    {
        marshalSaHpiEntryIdT( x.EntryId );
        marshalSaHpiUint32T( x.ComponentId );
        marshalSaHpiFumiFirmwareInstanceInfoT( x.MainFwInstance );
        marshalSaHpiUint32T( x.ComponentFlags );
    }

    public void marshalSaHpiFumiBankInfoT( SaHpiFumiBankInfoT x )
    {
        marshalSaHpiUint8T( x.BankId );
        marshalSaHpiUint32T( x.BankSize );
        marshalSaHpiUint32T( x.Position );
        marshalSaHpiFumiBankStateT( x.BankState );
        marshalSaHpiTextBufferT( x.Identifier );
        marshalSaHpiTextBufferT( x.Description );
        marshalSaHpiTextBufferT( x.DateTime );
        marshalSaHpiUint32T( x.MajorVersion );
        marshalSaHpiUint32T( x.MinorVersion );
        marshalSaHpiUint32T( x.AuxVersion );
    }

    public void marshalSaHpiFumiLogicalBankInfoT( SaHpiFumiLogicalBankInfoT x )
    {
        marshalSaHpiUint8T( x.FirmwarePersistentLocationCount );
        marshalSaHpiFumiLogicalBankStateFlagsT( x.BankStateFlags );
        marshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance );
        marshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance );
    }

    public void marshalSaHpiFumiLogicalComponentInfoT( SaHpiFumiLogicalComponentInfoT x )
    {
        marshalSaHpiEntryIdT( x.EntryId );
        marshalSaHpiUint32T( x.ComponentId );
        marshalSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance );
        marshalSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance );
        marshalSaHpiUint32T( x.ComponentFlags );
    }

    public void marshalSaHpiFumiRecT( SaHpiFumiRecT x )
    {
        marshalSaHpiFumiNumT( x.Num );
        marshalSaHpiFumiProtocolT( x.AccessProt );
        marshalSaHpiFumiCapabilityT( x.Capability );
        marshalSaHpiUint8T( x.NumBanks );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiResourceEventT( SaHpiResourceEventT x )
    {
        marshalSaHpiResourceEventTypeT( x.ResourceEventType );
    }

    public void marshalSaHpiDomainEventT( SaHpiDomainEventT x )
    {
        marshalSaHpiDomainEventTypeT( x.Type );
        marshalSaHpiDomainIdT( x.DomainId );
    }

    public void marshalSaHpiSensorEventT( SaHpiSensorEventT x )
    {
        marshalSaHpiSensorNumT( x.SensorNum );
        marshalSaHpiSensorTypeT( x.SensorType );
        marshalSaHpiEventCategoryT( x.EventCategory );
        marshalSaHpiBoolT( x.Assertion );
        marshalSaHpiEventStateT( x.EventState );
        marshalSaHpiSensorOptionalDataT( x.OptionalDataPresent );
        marshalSaHpiSensorReadingT( x.TriggerReading );
        marshalSaHpiSensorReadingT( x.TriggerThreshold );
        marshalSaHpiEventStateT( x.PreviousState );
        marshalSaHpiEventStateT( x.CurrentState );
        marshalSaHpiUint32T( x.Oem );
        marshalSaHpiUint32T( x.SensorSpecific );
    }

    public void marshalSaHpiSensorEnableChangeEventT( SaHpiSensorEnableChangeEventT x )
    {
        marshalSaHpiSensorNumT( x.SensorNum );
        marshalSaHpiSensorTypeT( x.SensorType );
        marshalSaHpiEventCategoryT( x.EventCategory );
        marshalSaHpiBoolT( x.SensorEnable );
        marshalSaHpiBoolT( x.SensorEventEnable );
        marshalSaHpiEventStateT( x.AssertEventMask );
        marshalSaHpiEventStateT( x.DeassertEventMask );
        marshalSaHpiSensorEnableOptDataT( x.OptionalDataPresent );
        marshalSaHpiEventStateT( x.CurrentState );
        marshalSaHpiEventStateT( x.CriticalAlarms );
        marshalSaHpiEventStateT( x.MajorAlarms );
        marshalSaHpiEventStateT( x.MinorAlarms );
    }

    public void marshalSaHpiHotSwapEventT( SaHpiHotSwapEventT x )
    {
        marshalSaHpiHsStateT( x.HotSwapState );
        marshalSaHpiHsStateT( x.PreviousHotSwapState );
        marshalSaHpiHsCauseOfStateChangeT( x.CauseOfStateChange );
    }

    public void marshalSaHpiWatchdogEventT( SaHpiWatchdogEventT x )
    {
        marshalSaHpiWatchdogNumT( x.WatchdogNum );
        marshalSaHpiWatchdogActionEventT( x.WatchdogAction );
        marshalSaHpiWatchdogPretimerInterruptT( x.WatchdogPreTimerAction );
        marshalSaHpiWatchdogTimerUseT( x.WatchdogUse );
    }

    public void marshalSaHpiHpiSwEventT( SaHpiHpiSwEventT x )
    {
        marshalSaHpiManufacturerIdT( x.MId );
        marshalSaHpiSwEventTypeT( x.Type );
        marshalSaHpiTextBufferT( x.EventData );
    }

    public void marshalSaHpiOemEventT( SaHpiOemEventT x )
    {
        marshalSaHpiManufacturerIdT( x.MId );
        marshalSaHpiTextBufferT( x.OemEventData );
    }

    public void marshalSaHpiUserEventT( SaHpiUserEventT x )
    {
        marshalSaHpiTextBufferT( x.UserEventData );
    }

    public void marshalSaHpiDimiEventT( SaHpiDimiEventT x )
    {
        marshalSaHpiDimiNumT( x.DimiNum );
        marshalSaHpiDimiTestNumT( x.TestNum );
        marshalSaHpiDimiTestRunStatusT( x.DimiTestRunStatus );
        marshalSaHpiDimiTestPercentCompletedT( x.DimiTestPercentCompleted );
    }

    public void marshalSaHpiDimiUpdateEventT( SaHpiDimiUpdateEventT x )
    {
        marshalSaHpiDimiNumT( x.DimiNum );
    }

    public void marshalSaHpiFumiEventT( SaHpiFumiEventT x )
    {
        marshalSaHpiFumiNumT( x.FumiNum );
        marshalSaHpiUint8T( x.BankNum );
        marshalSaHpiFumiUpgradeStatusT( x.UpgradeStatus );
    }

    public void marshalSaHpiEventUnionT( SaHpiEventUnionT x, long mod )
    {
        if ( mod == SAHPI_ET_RESOURCE ) {
            marshalSaHpiResourceEventT( x.ResourceEvent );
        }
        if ( mod == SAHPI_ET_DOMAIN ) {
            marshalSaHpiDomainEventT( x.DomainEvent );
        }
        if ( mod == SAHPI_ET_SENSOR ) {
            marshalSaHpiSensorEventT( x.SensorEvent );
        }
        if ( mod == SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            marshalSaHpiSensorEnableChangeEventT( x.SensorEnableChangeEvent );
        }
        if ( mod == SAHPI_ET_HOTSWAP ) {
            marshalSaHpiHotSwapEventT( x.HotSwapEvent );
        }
        if ( mod == SAHPI_ET_WATCHDOG ) {
            marshalSaHpiWatchdogEventT( x.WatchdogEvent );
        }
        if ( mod == SAHPI_ET_HPI_SW ) {
            marshalSaHpiHpiSwEventT( x.HpiSwEvent );
        }
        if ( mod == SAHPI_ET_OEM ) {
            marshalSaHpiOemEventT( x.OemEvent );
        }
        if ( mod == SAHPI_ET_USER ) {
            marshalSaHpiUserEventT( x.UserEvent );
        }
        if ( mod == SAHPI_ET_DIMI ) {
            marshalSaHpiDimiEventT( x.DimiEvent );
        }
        if ( mod == SAHPI_ET_DIMI_UPDATE ) {
            marshalSaHpiDimiUpdateEventT( x.DimiUpdateEvent );
        }
        if ( mod == SAHPI_ET_FUMI ) {
            marshalSaHpiFumiEventT( x.FumiEvent );
        }
    }

    public void marshalSaHpiEventT( SaHpiEventT x )
    {
        marshalSaHpiResourceIdT( x.Source );
        marshalSaHpiEventTypeT( x.EventType );
        marshalSaHpiTimeT( x.Timestamp );
        marshalSaHpiSeverityT( x.Severity );
        marshalSaHpiEventUnionT( x.EventDataUnion, x.EventType );
    }

    public void marshalSaHpiNameT( SaHpiNameT x )
    {
        marshalSaHpiUint16T( x.Length );
        marshalByteArray( x.Value, SA_HPI_MAX_NAME_LENGTH );
    }

    public void marshalSaHpiConditionT( SaHpiConditionT x )
    {
        marshalSaHpiStatusCondTypeT( x.Type );
        marshalSaHpiEntityPathT( x.Entity );
        marshalSaHpiDomainIdT( x.DomainId );
        marshalSaHpiResourceIdT( x.ResourceId );
        marshalSaHpiSensorNumT( x.SensorNum );
        marshalSaHpiEventStateT( x.EventState );
        marshalSaHpiNameT( x.Name );
        marshalSaHpiManufacturerIdT( x.Mid );
        marshalSaHpiTextBufferT( x.Data );
    }

    public void marshalSaHpiAnnouncementT( SaHpiAnnouncementT x )
    {
        marshalSaHpiEntryIdT( x.EntryId );
        marshalSaHpiTimeT( x.Timestamp );
        marshalSaHpiBoolT( x.AddedByUser );
        marshalSaHpiSeverityT( x.Severity );
        marshalSaHpiBoolT( x.Acknowledged );
        marshalSaHpiConditionT( x.StatusCond );
    }

    public void marshalSaHpiAnnunciatorRecT( SaHpiAnnunciatorRecT x )
    {
        marshalSaHpiAnnunciatorNumT( x.AnnunciatorNum );
        marshalSaHpiAnnunciatorTypeT( x.AnnunciatorType );
        marshalSaHpiBoolT( x.ModeReadOnly );
        marshalSaHpiUint32T( x.MaxConditions );
        marshalSaHpiUint32T( x.Oem );
    }

    public void marshalSaHpiRdrTypeUnionT( SaHpiRdrTypeUnionT x, long mod )
    {
        if ( mod == SAHPI_CTRL_RDR ) {
            marshalSaHpiCtrlRecT( x.CtrlRec );
        }
        if ( mod == SAHPI_SENSOR_RDR ) {
            marshalSaHpiSensorRecT( x.SensorRec );
        }
        if ( mod == SAHPI_INVENTORY_RDR ) {
            marshalSaHpiInventoryRecT( x.InventoryRec );
        }
        if ( mod == SAHPI_WATCHDOG_RDR ) {
            marshalSaHpiWatchdogRecT( x.WatchdogRec );
        }
        if ( mod == SAHPI_ANNUNCIATOR_RDR ) {
            marshalSaHpiAnnunciatorRecT( x.AnnunciatorRec );
        }
        if ( mod == SAHPI_DIMI_RDR ) {
            marshalSaHpiDimiRecT( x.DimiRec );
        }
        if ( mod == SAHPI_FUMI_RDR ) {
            marshalSaHpiFumiRecT( x.FumiRec );
        }
    }

    public void marshalSaHpiRdrT( SaHpiRdrT x )
    {
        marshalSaHpiEntryIdT( x.RecordId );
        marshalSaHpiRdrTypeT( x.RdrType );
        marshalSaHpiEntityPathT( x.Entity );
        marshalSaHpiBoolT( x.IsFru );
        marshalSaHpiRdrTypeUnionT( x.RdrTypeUnion, x.RdrType );
        marshalSaHpiTextBufferT( x.IdString );
    }

    public void marshalSaHpiLoadIdT( SaHpiLoadIdT x )
    {
        marshalSaHpiLoadNumberT( x.LoadNumber );
        marshalSaHpiTextBufferT( x.LoadName );
    }

    public void marshalSaHpiResourceInfoT( SaHpiResourceInfoT x )
    {
        marshalSaHpiUint8T( x.ResourceRev );
        marshalSaHpiUint8T( x.SpecificVer );
        marshalSaHpiUint8T( x.DeviceSupport );
        marshalSaHpiManufacturerIdT( x.ManufacturerId );
        marshalSaHpiUint16T( x.ProductId );
        marshalSaHpiUint8T( x.FirmwareMajorRev );
        marshalSaHpiUint8T( x.FirmwareMinorRev );
        marshalSaHpiUint8T( x.AuxFirmwareRev );
        marshalByteArray( x.Guid, SAHPI_GUID_LENGTH );
    }

    public void marshalSaHpiRptEntryT( SaHpiRptEntryT x )
    {
        marshalSaHpiEntryIdT( x.EntryId );
        marshalSaHpiResourceIdT( x.ResourceId );
        marshalSaHpiResourceInfoT( x.ResourceInfo );
        marshalSaHpiEntityPathT( x.ResourceEntity );
        marshalSaHpiCapabilitiesT( x.ResourceCapabilities );
        marshalSaHpiHsCapabilitiesT( x.HotSwapCapabilities );
        marshalSaHpiSeverityT( x.ResourceSeverity );
        marshalSaHpiBoolT( x.ResourceFailed );
        marshalSaHpiTextBufferT( x.ResourceTag );
    }

    public void marshalSaHpiDomainInfoT( SaHpiDomainInfoT x )
    {
        marshalSaHpiDomainIdT( x.DomainId );
        marshalSaHpiDomainCapabilitiesT( x.DomainCapabilities );
        marshalSaHpiBoolT( x.IsPeer );
        marshalSaHpiTextBufferT( x.DomainTag );
        marshalSaHpiUint32T( x.DrtUpdateCount );
        marshalSaHpiTimeT( x.DrtUpdateTimestamp );
        marshalSaHpiUint32T( x.RptUpdateCount );
        marshalSaHpiTimeT( x.RptUpdateTimestamp );
        marshalSaHpiUint32T( x.DatUpdateCount );
        marshalSaHpiTimeT( x.DatUpdateTimestamp );
        marshalSaHpiUint32T( x.ActiveAlarms );
        marshalSaHpiUint32T( x.CriticalAlarms );
        marshalSaHpiUint32T( x.MajorAlarms );
        marshalSaHpiUint32T( x.MinorAlarms );
        marshalSaHpiUint32T( x.DatUserAlarmLimit );
        marshalSaHpiBoolT( x.DatOverflow );
        marshalByteArray( x.Guid, SAHPI_GUID_LENGTH );
    }

    public void marshalSaHpiDrtEntryT( SaHpiDrtEntryT x )
    {
        marshalSaHpiEntryIdT( x.EntryId );
        marshalSaHpiDomainIdT( x.DomainId );
        marshalSaHpiBoolT( x.IsPeer );
    }

    public void marshalSaHpiAlarmT( SaHpiAlarmT x )
    {
        marshalSaHpiAlarmIdT( x.AlarmId );
        marshalSaHpiTimeT( x.Timestamp );
        marshalSaHpiSeverityT( x.Severity );
        marshalSaHpiBoolT( x.Acknowledged );
        marshalSaHpiConditionT( x.AlarmCond );
    }

    public void marshalSaHpiEventLogInfoT( SaHpiEventLogInfoT x )
    {
        marshalSaHpiUint32T( x.Entries );
        marshalSaHpiUint32T( x.Size );
        marshalSaHpiUint32T( x.UserEventMaxSize );
        marshalSaHpiTimeT( x.UpdateTimestamp );
        marshalSaHpiTimeT( x.CurrentTime );
        marshalSaHpiBoolT( x.Enabled );
        marshalSaHpiBoolT( x.OverflowFlag );
        marshalSaHpiBoolT( x.OverflowResetable );
        marshalSaHpiEventLogOverflowActionT( x.OverflowAction );
    }

    public void marshalSaHpiEventLogEntryT( SaHpiEventLogEntryT x )
    {
        marshalSaHpiEventLogEntryIdT( x.EntryId );
        marshalSaHpiTimeT( x.Timestamp );
        marshalSaHpiEventT( x.Event );
    }

    /**********************************************************
     * Demarshal: For HPI Simple Data Types
     *********************************************************/
    public long demarshalSaHpiBoolT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiManufacturerIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiVersionT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaErrorT()
        throws HpiException
    {
        return demarshalSaHpiInt32T();
    }

    public long demarshalSaHpiDomainIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiSessionIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiResourceIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiEntryIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiTimeT()
        throws HpiException
    {
        return demarshalSaHpiInt64T();
    }

    public long demarshalSaHpiTimeoutT()
        throws HpiException
    {
        return demarshalSaHpiInt64T();
    }

    public long demarshalSaHpiInstrumentIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiEntityLocationT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiEventCategoryT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiEventStateT()
        throws HpiException
    {
        return demarshalSaHpiUint16T();
    }

    public long demarshalSaHpiSensorNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiSensorRangeFlagsT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiSensorThdMaskT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiCtrlNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiCtrlStateDiscreteT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiCtrlStateAnalogT()
        throws HpiException
    {
        return demarshalSaHpiInt32T();
    }

    public long demarshalSaHpiTxtLineNumT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiIdrIdT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiWatchdogNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiWatchdogExpFlagsT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiDimiNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiDimiTestCapabilityT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiDimiTestNumT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiDimiTestPercentCompletedT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiFumiNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiBankNumT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiFumiLogicalBankStateFlagsT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiFumiProtocolT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiFumiCapabilityT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiSensorOptionalDataT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiSensorEnableOptDataT()
        throws HpiException
    {
        return demarshalSaHpiUint8T();
    }

    public long demarshalSaHpiEvtQueueStatusT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiAnnunciatorNumT()
        throws HpiException
    {
        return demarshalSaHpiInstrumentIdT();
    }

    public long demarshalSaHpiLoadNumberT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiCapabilitiesT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiHsCapabilitiesT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiDomainCapabilitiesT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiAlarmIdT()
        throws HpiException
    {
        return demarshalSaHpiEntryIdT();
    }

    public long demarshalSaHpiEventLogCapabilitiesT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiEventLogEntryIdT()
        throws HpiException
    {
        return demarshalSaHpiUint32T();
    }

    public long demarshalSaHpiLanguageT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiTextTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiEntityTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorReadingTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorEventMaskActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorUnitsT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorModUnitUseT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSensorEventCtrlT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiCtrlTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiCtrlStateDigitalT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiCtrlModeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiCtrlOutputTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiIdrAreaTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiIdrFieldTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiWatchdogActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiWatchdogActionEventT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiWatchdogPretimerInterruptT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiWatchdogTimerUseT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDimiTestServiceImpactT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDimiTestRunStatusT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDimiTestErrCodeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDimiTestParamTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDimiReadyT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiSpecInfoTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiSafDefinedSpecIdT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiServiceImpactT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiSourceStatusT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiBankStateT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiFumiUpgradeStatusT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiHsIndicatorStateT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiHsActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiHsStateT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiHsCauseOfStateChangeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSeverityT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiResourceEventTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiDomainEventTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiSwEventTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiEventTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiStatusCondTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiAnnunciatorModeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiAnnunciatorTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiRdrTypeT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiParmActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiResetActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiPowerStateT()
        throws HpiException
    {
        return demarshalEnum();
    }

    public long demarshalSaHpiEventLogOverflowActionT()
        throws HpiException
    {
        return demarshalEnum();
    }

    /**********************************************************
     * Demarshal: For HPI Structs and Unions
     *********************************************************/
    public SaHpiTextBufferT demarshalSaHpiTextBufferT()
        throws HpiException
    {
        SaHpiTextBufferT x = new SaHpiTextBufferT();

        x.DataType = demarshalSaHpiTextTypeT();
        x.Language = demarshalSaHpiLanguageT();
        x.DataLength = demarshalSaHpiUint8T();
        x.Data = demarshalByteArray( SAHPI_MAX_TEXT_BUFFER_LENGTH );

        return x;
    }

    public SaHpiEntityT demarshalSaHpiEntityT()
        throws HpiException
    {
        SaHpiEntityT x = new SaHpiEntityT();

        x.EntityType = demarshalSaHpiEntityTypeT();
        x.EntityLocation = demarshalSaHpiEntityLocationT();

        return x;
    }

    public SaHpiEntityPathT demarshalSaHpiEntityPathT()
        throws HpiException
    {
        SaHpiEntityPathT x = new SaHpiEntityPathT();

        x.Entry = new SaHpiEntityT[SAHPI_MAX_ENTITY_PATH];
        for ( int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            x.Entry[i] = demarshalSaHpiEntityT();
        }

        return x;
    }

    public SaHpiSensorReadingUnionT demarshalSaHpiSensorReadingUnionT( long mod )
        throws HpiException
    {
        SaHpiSensorReadingUnionT x = new SaHpiSensorReadingUnionT();

        if ( mod == SAHPI_SENSOR_READING_TYPE_INT64 ) {
            x.SensorInt64 = demarshalSaHpiInt64T();
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_UINT64 ) {
            x.SensorUint64 = demarshalSaHpiUint64T();
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
            x.SensorFloat64 = demarshalSaHpiFloat64T();
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            x.SensorBuffer = demarshalByteArray( SAHPI_SENSOR_BUFFER_LENGTH );
        }

        return x;
    }

    public SaHpiSensorReadingT demarshalSaHpiSensorReadingT()
        throws HpiException
    {
        SaHpiSensorReadingT x = new SaHpiSensorReadingT();

        x.IsSupported = demarshalSaHpiBoolT();
        x.Type = demarshalSaHpiSensorReadingTypeT();
        x.Value = demarshalSaHpiSensorReadingUnionT( x.Type );

        return x;
    }

    public SaHpiSensorThresholdsT demarshalSaHpiSensorThresholdsT()
        throws HpiException
    {
        SaHpiSensorThresholdsT x = new SaHpiSensorThresholdsT();

        x.LowCritical = demarshalSaHpiSensorReadingT();
        x.LowMajor = demarshalSaHpiSensorReadingT();
        x.LowMinor = demarshalSaHpiSensorReadingT();
        x.UpCritical = demarshalSaHpiSensorReadingT();
        x.UpMajor = demarshalSaHpiSensorReadingT();
        x.UpMinor = demarshalSaHpiSensorReadingT();
        x.PosThdHysteresis = demarshalSaHpiSensorReadingT();
        x.NegThdHysteresis = demarshalSaHpiSensorReadingT();

        return x;
    }

    public SaHpiSensorRangeT demarshalSaHpiSensorRangeT()
        throws HpiException
    {
        SaHpiSensorRangeT x = new SaHpiSensorRangeT();

        x.Flags = demarshalSaHpiSensorRangeFlagsT();
        x.Max = demarshalSaHpiSensorReadingT();
        x.Min = demarshalSaHpiSensorReadingT();
        x.Nominal = demarshalSaHpiSensorReadingT();
        x.NormalMax = demarshalSaHpiSensorReadingT();
        x.NormalMin = demarshalSaHpiSensorReadingT();

        return x;
    }

    public SaHpiSensorDataFormatT demarshalSaHpiSensorDataFormatT()
        throws HpiException
    {
        SaHpiSensorDataFormatT x = new SaHpiSensorDataFormatT();

        x.IsSupported = demarshalSaHpiBoolT();
        x.ReadingType = demarshalSaHpiSensorReadingTypeT();
        x.BaseUnits = demarshalSaHpiSensorUnitsT();
        x.ModifierUnits = demarshalSaHpiSensorUnitsT();
        x.ModifierUse = demarshalSaHpiSensorModUnitUseT();
        x.Percentage = demarshalSaHpiBoolT();
        x.Range = demarshalSaHpiSensorRangeT();
        x.AccuracyFactor = demarshalSaHpiFloat64T();

        return x;
    }

    public SaHpiSensorThdDefnT demarshalSaHpiSensorThdDefnT()
        throws HpiException
    {
        SaHpiSensorThdDefnT x = new SaHpiSensorThdDefnT();

        x.IsAccessible = demarshalSaHpiBoolT();
        x.ReadThold = demarshalSaHpiSensorThdMaskT();
        x.WriteThold = demarshalSaHpiSensorThdMaskT();
        x.Nonlinear = demarshalSaHpiBoolT();

        return x;
    }

    public SaHpiSensorRecT demarshalSaHpiSensorRecT()
        throws HpiException
    {
        SaHpiSensorRecT x = new SaHpiSensorRecT();

        x.Num = demarshalSaHpiSensorNumT();
        x.Type = demarshalSaHpiSensorTypeT();
        x.Category = demarshalSaHpiEventCategoryT();
        x.EnableCtrl = demarshalSaHpiBoolT();
        x.EventCtrl = demarshalSaHpiSensorEventCtrlT();
        x.Events = demarshalSaHpiEventStateT();
        x.DataFormat = demarshalSaHpiSensorDataFormatT();
        x.ThresholdDefn = demarshalSaHpiSensorThdDefnT();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiCtrlStateStreamT demarshalSaHpiCtrlStateStreamT()
        throws HpiException
    {
        SaHpiCtrlStateStreamT x = new SaHpiCtrlStateStreamT();

        x.Repeat = demarshalSaHpiBoolT();
        x.StreamLength = demarshalSaHpiUint32T();
        x.Stream = demarshalByteArray( SAHPI_CTRL_MAX_STREAM_LENGTH );

        return x;
    }

    public SaHpiCtrlStateTextT demarshalSaHpiCtrlStateTextT()
        throws HpiException
    {
        SaHpiCtrlStateTextT x = new SaHpiCtrlStateTextT();

        x.Line = demarshalSaHpiTxtLineNumT();
        x.Text = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiCtrlStateOemT demarshalSaHpiCtrlStateOemT()
        throws HpiException
    {
        SaHpiCtrlStateOemT x = new SaHpiCtrlStateOemT();

        x.MId = demarshalSaHpiManufacturerIdT();
        x.BodyLength = demarshalSaHpiUint8T();
        x.Body = demarshalByteArray( SAHPI_CTRL_MAX_OEM_BODY_LENGTH );

        return x;
    }

    public SaHpiCtrlStateUnionT demarshalSaHpiCtrlStateUnionT( long mod )
        throws HpiException
    {
        SaHpiCtrlStateUnionT x = new SaHpiCtrlStateUnionT();

        if ( mod == SAHPI_CTRL_TYPE_DIGITAL ) {
            x.Digital = demarshalSaHpiCtrlStateDigitalT();
        }
        if ( mod == SAHPI_CTRL_TYPE_DISCRETE ) {
            x.Discrete = demarshalSaHpiCtrlStateDiscreteT();
        }
        if ( mod == SAHPI_CTRL_TYPE_ANALOG ) {
            x.Analog = demarshalSaHpiCtrlStateAnalogT();
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            x.Stream = demarshalSaHpiCtrlStateStreamT();
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            x.Text = demarshalSaHpiCtrlStateTextT();
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            x.Oem = demarshalSaHpiCtrlStateOemT();
        }

        return x;
    }

    public SaHpiCtrlStateT demarshalSaHpiCtrlStateT()
        throws HpiException
    {
        SaHpiCtrlStateT x = new SaHpiCtrlStateT();

        x.Type = demarshalSaHpiCtrlTypeT();
        x.StateUnion = demarshalSaHpiCtrlStateUnionT( x.Type );

        return x;
    }

    public SaHpiCtrlRecDigitalT demarshalSaHpiCtrlRecDigitalT()
        throws HpiException
    {
        SaHpiCtrlRecDigitalT x = new SaHpiCtrlRecDigitalT();

        x.Default = demarshalSaHpiCtrlStateDigitalT();

        return x;
    }

    public SaHpiCtrlRecDiscreteT demarshalSaHpiCtrlRecDiscreteT()
        throws HpiException
    {
        SaHpiCtrlRecDiscreteT x = new SaHpiCtrlRecDiscreteT();

        x.Default = demarshalSaHpiCtrlStateDiscreteT();

        return x;
    }

    public SaHpiCtrlRecAnalogT demarshalSaHpiCtrlRecAnalogT()
        throws HpiException
    {
        SaHpiCtrlRecAnalogT x = new SaHpiCtrlRecAnalogT();

        x.Min = demarshalSaHpiCtrlStateAnalogT();
        x.Max = demarshalSaHpiCtrlStateAnalogT();
        x.Default = demarshalSaHpiCtrlStateAnalogT();

        return x;
    }

    public SaHpiCtrlRecStreamT demarshalSaHpiCtrlRecStreamT()
        throws HpiException
    {
        SaHpiCtrlRecStreamT x = new SaHpiCtrlRecStreamT();

        x.Default = demarshalSaHpiCtrlStateStreamT();

        return x;
    }

    public SaHpiCtrlRecTextT demarshalSaHpiCtrlRecTextT()
        throws HpiException
    {
        SaHpiCtrlRecTextT x = new SaHpiCtrlRecTextT();

        x.MaxChars = demarshalSaHpiUint8T();
        x.MaxLines = demarshalSaHpiUint8T();
        x.Language = demarshalSaHpiLanguageT();
        x.DataType = demarshalSaHpiTextTypeT();
        x.Default = demarshalSaHpiCtrlStateTextT();

        return x;
    }

    public SaHpiCtrlRecOemT demarshalSaHpiCtrlRecOemT()
        throws HpiException
    {
        SaHpiCtrlRecOemT x = new SaHpiCtrlRecOemT();

        x.MId = demarshalSaHpiManufacturerIdT();
        x.ConfigData = demarshalByteArray( SAHPI_CTRL_OEM_CONFIG_LENGTH );
        x.Default = demarshalSaHpiCtrlStateOemT();

        return x;
    }

    public SaHpiCtrlRecUnionT demarshalSaHpiCtrlRecUnionT( long mod )
        throws HpiException
    {
        SaHpiCtrlRecUnionT x = new SaHpiCtrlRecUnionT();

        if ( mod == SAHPI_CTRL_TYPE_DIGITAL ) {
            x.Digital = demarshalSaHpiCtrlRecDigitalT();
        }
        if ( mod == SAHPI_CTRL_TYPE_DISCRETE ) {
            x.Discrete = demarshalSaHpiCtrlRecDiscreteT();
        }
        if ( mod == SAHPI_CTRL_TYPE_ANALOG ) {
            x.Analog = demarshalSaHpiCtrlRecAnalogT();
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            x.Stream = demarshalSaHpiCtrlRecStreamT();
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            x.Text = demarshalSaHpiCtrlRecTextT();
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            x.Oem = demarshalSaHpiCtrlRecOemT();
        }

        return x;
    }

    public SaHpiCtrlDefaultModeT demarshalSaHpiCtrlDefaultModeT()
        throws HpiException
    {
        SaHpiCtrlDefaultModeT x = new SaHpiCtrlDefaultModeT();

        x.Mode = demarshalSaHpiCtrlModeT();
        x.ReadOnly = demarshalSaHpiBoolT();

        return x;
    }

    public SaHpiCtrlRecT demarshalSaHpiCtrlRecT()
        throws HpiException
    {
        SaHpiCtrlRecT x = new SaHpiCtrlRecT();

        x.Num = demarshalSaHpiCtrlNumT();
        x.OutputType = demarshalSaHpiCtrlOutputTypeT();
        x.Type = demarshalSaHpiCtrlTypeT();
        x.TypeUnion = demarshalSaHpiCtrlRecUnionT( x.Type );
        x.DefaultMode = demarshalSaHpiCtrlDefaultModeT();
        x.WriteOnly = demarshalSaHpiBoolT();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiIdrFieldT demarshalSaHpiIdrFieldT()
        throws HpiException
    {
        SaHpiIdrFieldT x = new SaHpiIdrFieldT();

        x.AreaId = demarshalSaHpiEntryIdT();
        x.FieldId = demarshalSaHpiEntryIdT();
        x.Type = demarshalSaHpiIdrFieldTypeT();
        x.ReadOnly = demarshalSaHpiBoolT();
        x.Field = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiIdrAreaHeaderT demarshalSaHpiIdrAreaHeaderT()
        throws HpiException
    {
        SaHpiIdrAreaHeaderT x = new SaHpiIdrAreaHeaderT();

        x.AreaId = demarshalSaHpiEntryIdT();
        x.Type = demarshalSaHpiIdrAreaTypeT();
        x.ReadOnly = demarshalSaHpiBoolT();
        x.NumFields = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiIdrInfoT demarshalSaHpiIdrInfoT()
        throws HpiException
    {
        SaHpiIdrInfoT x = new SaHpiIdrInfoT();

        x.IdrId = demarshalSaHpiIdrIdT();
        x.UpdateCount = demarshalSaHpiUint32T();
        x.ReadOnly = demarshalSaHpiBoolT();
        x.NumAreas = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiInventoryRecT demarshalSaHpiInventoryRecT()
        throws HpiException
    {
        SaHpiInventoryRecT x = new SaHpiInventoryRecT();

        x.IdrId = demarshalSaHpiIdrIdT();
        x.Persistent = demarshalSaHpiBoolT();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiWatchdogT demarshalSaHpiWatchdogT()
        throws HpiException
    {
        SaHpiWatchdogT x = new SaHpiWatchdogT();

        x.Log = demarshalSaHpiBoolT();
        x.Running = demarshalSaHpiBoolT();
        x.TimerUse = demarshalSaHpiWatchdogTimerUseT();
        x.TimerAction = demarshalSaHpiWatchdogActionT();
        x.PretimerInterrupt = demarshalSaHpiWatchdogPretimerInterruptT();
        x.PreTimeoutInterval = demarshalSaHpiUint32T();
        x.TimerUseExpFlags = demarshalSaHpiWatchdogExpFlagsT();
        x.InitialCount = demarshalSaHpiUint32T();
        x.PresentCount = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiWatchdogRecT demarshalSaHpiWatchdogRecT()
        throws HpiException
    {
        SaHpiWatchdogRecT x = new SaHpiWatchdogRecT();

        x.WatchdogNum = demarshalSaHpiWatchdogNumT();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiDimiTestAffectedEntityT demarshalSaHpiDimiTestAffectedEntityT()
        throws HpiException
    {
        SaHpiDimiTestAffectedEntityT x = new SaHpiDimiTestAffectedEntityT();

        x.EntityImpacted = demarshalSaHpiEntityPathT();
        x.ServiceImpact = demarshalSaHpiDimiTestServiceImpactT();

        return x;
    }

    public SaHpiDimiTestResultsT demarshalSaHpiDimiTestResultsT()
        throws HpiException
    {
        SaHpiDimiTestResultsT x = new SaHpiDimiTestResultsT();

        x.ResultTimeStamp = demarshalSaHpiTimeT();
        x.RunDuration = demarshalSaHpiTimeoutT();
        x.LastRunStatus = demarshalSaHpiDimiTestRunStatusT();
        x.TestErrorCode = demarshalSaHpiDimiTestErrCodeT();
        x.TestResultString = demarshalSaHpiTextBufferT();
        x.TestResultStringIsURI = demarshalSaHpiBoolT();

        return x;
    }

    public SaHpiDimiTestParamValueT demarshalSaHpiDimiTestParamValueT( long mod )
        throws HpiException
    {
        SaHpiDimiTestParamValueT x = new SaHpiDimiTestParamValueT();

        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            x.paramint = demarshalSaHpiInt32T();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            x.parambool = demarshalSaHpiBoolT();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            x.paramfloat = demarshalSaHpiFloat64T();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            x.paramtext = demarshalSaHpiTextBufferT();
        }

        return x;
    }

    public SaHpiDimiTestParameterValueUnionT demarshalSaHpiDimiTestParameterValueUnionT( long mod )
        throws HpiException
    {
        SaHpiDimiTestParameterValueUnionT x = new SaHpiDimiTestParameterValueUnionT();

        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            x.IntValue = demarshalSaHpiInt32T();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            x.FloatValue = demarshalSaHpiFloat64T();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            x.FloatValue = demarshalSaHpiFloat64T();
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            x.FloatValue = demarshalSaHpiFloat64T();
        }

        return x;
    }

    public SaHpiDimiTestParamsDefinitionT demarshalSaHpiDimiTestParamsDefinitionT()
        throws HpiException
    {
        SaHpiDimiTestParamsDefinitionT x = new SaHpiDimiTestParamsDefinitionT();

        x.ParamName = demarshalByteArray( SAHPI_DIMITEST_PARAM_NAME_LEN );
        x.ParamInfo = demarshalSaHpiTextBufferT();
        x.ParamType = demarshalSaHpiDimiTestParamTypeT();
        x.MinValue = demarshalSaHpiDimiTestParameterValueUnionT( x.ParamType );
        x.MaxValue = demarshalSaHpiDimiTestParameterValueUnionT( x.ParamType );
        x.DefaultParam = demarshalSaHpiDimiTestParamValueT( x.ParamType );

        return x;
    }

    public SaHpiDimiTestT demarshalSaHpiDimiTestT()
        throws HpiException
    {
        SaHpiDimiTestT x = new SaHpiDimiTestT();

        x.TestName = demarshalSaHpiTextBufferT();
        x.ServiceImpact = demarshalSaHpiDimiTestServiceImpactT();
        x.EntitiesImpacted = new SaHpiDimiTestAffectedEntityT[SAHPI_DIMITEST_MAX_ENTITIESIMPACTED];
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            x.EntitiesImpacted[i] = demarshalSaHpiDimiTestAffectedEntityT();
        }
        x.NeedServiceOS = demarshalSaHpiBoolT();
        x.ServiceOS = demarshalSaHpiTextBufferT();
        x.ExpectedRunDuration = demarshalSaHpiTimeT();
        x.TestCapabilities = demarshalSaHpiDimiTestCapabilityT();
        x.TestParameters = new SaHpiDimiTestParamsDefinitionT[SAHPI_DIMITEST_MAX_PARAMETERS];
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            x.TestParameters[i] = demarshalSaHpiDimiTestParamsDefinitionT();
        }

        return x;
    }

    public SaHpiDimiTestVariableParamsT demarshalSaHpiDimiTestVariableParamsT()
        throws HpiException
    {
        SaHpiDimiTestVariableParamsT x = new SaHpiDimiTestVariableParamsT();

        x.ParamName = demarshalByteArray( SAHPI_DIMITEST_PARAM_NAME_LEN );
        x.ParamType = demarshalSaHpiDimiTestParamTypeT();
        x.Value = demarshalSaHpiDimiTestParamValueT( x.ParamType );

        return x;
    }

    public SaHpiDimiInfoT demarshalSaHpiDimiInfoT()
        throws HpiException
    {
        SaHpiDimiInfoT x = new SaHpiDimiInfoT();

        x.NumberOfTests = demarshalSaHpiUint32T();
        x.TestNumUpdateCounter = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiDimiRecT demarshalSaHpiDimiRecT()
        throws HpiException
    {
        SaHpiDimiRecT x = new SaHpiDimiRecT();

        x.DimiNum = demarshalSaHpiDimiNumT();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiSafDefinedSpecInfoT demarshalSaHpiFumiSafDefinedSpecInfoT()
        throws HpiException
    {
        SaHpiFumiSafDefinedSpecInfoT x = new SaHpiFumiSafDefinedSpecInfoT();

        x.SpecID = demarshalSaHpiFumiSafDefinedSpecIdT();
        x.RevisionID = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiOemDefinedSpecInfoT demarshalSaHpiFumiOemDefinedSpecInfoT()
        throws HpiException
    {
        SaHpiFumiOemDefinedSpecInfoT x = new SaHpiFumiOemDefinedSpecInfoT();

        x.Mid = demarshalSaHpiManufacturerIdT();
        x.BodyLength = demarshalSaHpiUint8T();
        x.Body = demarshalByteArray( SAHPI_FUMI_MAX_OEM_BODY_LENGTH );

        return x;
    }

    public SaHpiFumiSpecInfoTypeUnionT demarshalSaHpiFumiSpecInfoTypeUnionT( long mod )
        throws HpiException
    {
        SaHpiFumiSpecInfoTypeUnionT x = new SaHpiFumiSpecInfoTypeUnionT();

        if ( mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            x.SafDefined = demarshalSaHpiFumiSafDefinedSpecInfoT();
        }
        if ( mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            x.OemDefined = demarshalSaHpiFumiOemDefinedSpecInfoT();
        }

        return x;
    }

    public SaHpiFumiSpecInfoT demarshalSaHpiFumiSpecInfoT()
        throws HpiException
    {
        SaHpiFumiSpecInfoT x = new SaHpiFumiSpecInfoT();

        x.SpecInfoType = demarshalSaHpiFumiSpecInfoTypeT();
        x.SpecInfoTypeUnion = demarshalSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoType );

        return x;
    }

    public SaHpiFumiFirmwareInstanceInfoT demarshalSaHpiFumiFirmwareInstanceInfoT()
        throws HpiException
    {
        SaHpiFumiFirmwareInstanceInfoT x = new SaHpiFumiFirmwareInstanceInfoT();

        x.InstancePresent = demarshalSaHpiBoolT();
        x.Identifier = demarshalSaHpiTextBufferT();
        x.Description = demarshalSaHpiTextBufferT();
        x.DateTime = demarshalSaHpiTextBufferT();
        x.MajorVersion = demarshalSaHpiUint32T();
        x.MinorVersion = demarshalSaHpiUint32T();
        x.AuxVersion = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiImpactedEntityT demarshalSaHpiFumiImpactedEntityT()
        throws HpiException
    {
        SaHpiFumiImpactedEntityT x = new SaHpiFumiImpactedEntityT();

        x.ImpactedEntity = demarshalSaHpiEntityPathT();
        x.ServiceImpact = demarshalSaHpiFumiServiceImpactT();

        return x;
    }

    public SaHpiFumiServiceImpactDataT demarshalSaHpiFumiServiceImpactDataT()
        throws HpiException
    {
        SaHpiFumiServiceImpactDataT x = new SaHpiFumiServiceImpactDataT();

        x.NumEntities = demarshalSaHpiUint32T();
        x.ImpactedEntities = new SaHpiFumiImpactedEntityT[SAHPI_FUMI_MAX_ENTITIES_IMPACTED];
        for ( int i = 0; i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            x.ImpactedEntities[i] = demarshalSaHpiFumiImpactedEntityT();
        }

        return x;
    }

    public SaHpiFumiSourceInfoT demarshalSaHpiFumiSourceInfoT()
        throws HpiException
    {
        SaHpiFumiSourceInfoT x = new SaHpiFumiSourceInfoT();

        x.SourceUri = demarshalSaHpiTextBufferT();
        x.SourceStatus = demarshalSaHpiFumiSourceStatusT();
        x.Identifier = demarshalSaHpiTextBufferT();
        x.Description = demarshalSaHpiTextBufferT();
        x.DateTime = demarshalSaHpiTextBufferT();
        x.MajorVersion = demarshalSaHpiUint32T();
        x.MinorVersion = demarshalSaHpiUint32T();
        x.AuxVersion = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiComponentInfoT demarshalSaHpiFumiComponentInfoT()
        throws HpiException
    {
        SaHpiFumiComponentInfoT x = new SaHpiFumiComponentInfoT();

        x.EntryId = demarshalSaHpiEntryIdT();
        x.ComponentId = demarshalSaHpiUint32T();
        x.MainFwInstance = demarshalSaHpiFumiFirmwareInstanceInfoT();
        x.ComponentFlags = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiBankInfoT demarshalSaHpiFumiBankInfoT()
        throws HpiException
    {
        SaHpiFumiBankInfoT x = new SaHpiFumiBankInfoT();

        x.BankId = demarshalSaHpiUint8T();
        x.BankSize = demarshalSaHpiUint32T();
        x.Position = demarshalSaHpiUint32T();
        x.BankState = demarshalSaHpiFumiBankStateT();
        x.Identifier = demarshalSaHpiTextBufferT();
        x.Description = demarshalSaHpiTextBufferT();
        x.DateTime = demarshalSaHpiTextBufferT();
        x.MajorVersion = demarshalSaHpiUint32T();
        x.MinorVersion = demarshalSaHpiUint32T();
        x.AuxVersion = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiLogicalBankInfoT demarshalSaHpiFumiLogicalBankInfoT()
        throws HpiException
    {
        SaHpiFumiLogicalBankInfoT x = new SaHpiFumiLogicalBankInfoT();

        x.FirmwarePersistentLocationCount = demarshalSaHpiUint8T();
        x.BankStateFlags = demarshalSaHpiFumiLogicalBankStateFlagsT();
        x.PendingFwInstance = demarshalSaHpiFumiFirmwareInstanceInfoT();
        x.RollbackFwInstance = demarshalSaHpiFumiFirmwareInstanceInfoT();

        return x;
    }

    public SaHpiFumiLogicalComponentInfoT demarshalSaHpiFumiLogicalComponentInfoT()
        throws HpiException
    {
        SaHpiFumiLogicalComponentInfoT x = new SaHpiFumiLogicalComponentInfoT();

        x.EntryId = demarshalSaHpiEntryIdT();
        x.ComponentId = demarshalSaHpiUint32T();
        x.PendingFwInstance = demarshalSaHpiFumiFirmwareInstanceInfoT();
        x.RollbackFwInstance = demarshalSaHpiFumiFirmwareInstanceInfoT();
        x.ComponentFlags = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiFumiRecT demarshalSaHpiFumiRecT()
        throws HpiException
    {
        SaHpiFumiRecT x = new SaHpiFumiRecT();

        x.Num = demarshalSaHpiFumiNumT();
        x.AccessProt = demarshalSaHpiFumiProtocolT();
        x.Capability = demarshalSaHpiFumiCapabilityT();
        x.NumBanks = demarshalSaHpiUint8T();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiResourceEventT demarshalSaHpiResourceEventT()
        throws HpiException
    {
        SaHpiResourceEventT x = new SaHpiResourceEventT();

        x.ResourceEventType = demarshalSaHpiResourceEventTypeT();

        return x;
    }

    public SaHpiDomainEventT demarshalSaHpiDomainEventT()
        throws HpiException
    {
        SaHpiDomainEventT x = new SaHpiDomainEventT();

        x.Type = demarshalSaHpiDomainEventTypeT();
        x.DomainId = demarshalSaHpiDomainIdT();

        return x;
    }

    public SaHpiSensorEventT demarshalSaHpiSensorEventT()
        throws HpiException
    {
        SaHpiSensorEventT x = new SaHpiSensorEventT();

        x.SensorNum = demarshalSaHpiSensorNumT();
        x.SensorType = demarshalSaHpiSensorTypeT();
        x.EventCategory = demarshalSaHpiEventCategoryT();
        x.Assertion = demarshalSaHpiBoolT();
        x.EventState = demarshalSaHpiEventStateT();
        x.OptionalDataPresent = demarshalSaHpiSensorOptionalDataT();
        x.TriggerReading = demarshalSaHpiSensorReadingT();
        x.TriggerThreshold = demarshalSaHpiSensorReadingT();
        x.PreviousState = demarshalSaHpiEventStateT();
        x.CurrentState = demarshalSaHpiEventStateT();
        x.Oem = demarshalSaHpiUint32T();
        x.SensorSpecific = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiSensorEnableChangeEventT demarshalSaHpiSensorEnableChangeEventT()
        throws HpiException
    {
        SaHpiSensorEnableChangeEventT x = new SaHpiSensorEnableChangeEventT();

        x.SensorNum = demarshalSaHpiSensorNumT();
        x.SensorType = demarshalSaHpiSensorTypeT();
        x.EventCategory = demarshalSaHpiEventCategoryT();
        x.SensorEnable = demarshalSaHpiBoolT();
        x.SensorEventEnable = demarshalSaHpiBoolT();
        x.AssertEventMask = demarshalSaHpiEventStateT();
        x.DeassertEventMask = demarshalSaHpiEventStateT();
        x.OptionalDataPresent = demarshalSaHpiSensorEnableOptDataT();
        x.CurrentState = demarshalSaHpiEventStateT();
        x.CriticalAlarms = demarshalSaHpiEventStateT();
        x.MajorAlarms = demarshalSaHpiEventStateT();
        x.MinorAlarms = demarshalSaHpiEventStateT();

        return x;
    }

    public SaHpiHotSwapEventT demarshalSaHpiHotSwapEventT()
        throws HpiException
    {
        SaHpiHotSwapEventT x = new SaHpiHotSwapEventT();

        x.HotSwapState = demarshalSaHpiHsStateT();
        x.PreviousHotSwapState = demarshalSaHpiHsStateT();
        x.CauseOfStateChange = demarshalSaHpiHsCauseOfStateChangeT();

        return x;
    }

    public SaHpiWatchdogEventT demarshalSaHpiWatchdogEventT()
        throws HpiException
    {
        SaHpiWatchdogEventT x = new SaHpiWatchdogEventT();

        x.WatchdogNum = demarshalSaHpiWatchdogNumT();
        x.WatchdogAction = demarshalSaHpiWatchdogActionEventT();
        x.WatchdogPreTimerAction = demarshalSaHpiWatchdogPretimerInterruptT();
        x.WatchdogUse = demarshalSaHpiWatchdogTimerUseT();

        return x;
    }

    public SaHpiHpiSwEventT demarshalSaHpiHpiSwEventT()
        throws HpiException
    {
        SaHpiHpiSwEventT x = new SaHpiHpiSwEventT();

        x.MId = demarshalSaHpiManufacturerIdT();
        x.Type = demarshalSaHpiSwEventTypeT();
        x.EventData = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiOemEventT demarshalSaHpiOemEventT()
        throws HpiException
    {
        SaHpiOemEventT x = new SaHpiOemEventT();

        x.MId = demarshalSaHpiManufacturerIdT();
        x.OemEventData = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiUserEventT demarshalSaHpiUserEventT()
        throws HpiException
    {
        SaHpiUserEventT x = new SaHpiUserEventT();

        x.UserEventData = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiDimiEventT demarshalSaHpiDimiEventT()
        throws HpiException
    {
        SaHpiDimiEventT x = new SaHpiDimiEventT();

        x.DimiNum = demarshalSaHpiDimiNumT();
        x.TestNum = demarshalSaHpiDimiTestNumT();
        x.DimiTestRunStatus = demarshalSaHpiDimiTestRunStatusT();
        x.DimiTestPercentCompleted = demarshalSaHpiDimiTestPercentCompletedT();

        return x;
    }

    public SaHpiDimiUpdateEventT demarshalSaHpiDimiUpdateEventT()
        throws HpiException
    {
        SaHpiDimiUpdateEventT x = new SaHpiDimiUpdateEventT();

        x.DimiNum = demarshalSaHpiDimiNumT();

        return x;
    }

    public SaHpiFumiEventT demarshalSaHpiFumiEventT()
        throws HpiException
    {
        SaHpiFumiEventT x = new SaHpiFumiEventT();

        x.FumiNum = demarshalSaHpiFumiNumT();
        x.BankNum = demarshalSaHpiUint8T();
        x.UpgradeStatus = demarshalSaHpiFumiUpgradeStatusT();

        return x;
    }

    public SaHpiEventUnionT demarshalSaHpiEventUnionT( long mod )
        throws HpiException
    {
        SaHpiEventUnionT x = new SaHpiEventUnionT();

        if ( mod == SAHPI_ET_RESOURCE ) {
            x.ResourceEvent = demarshalSaHpiResourceEventT();
        }
        if ( mod == SAHPI_ET_DOMAIN ) {
            x.DomainEvent = demarshalSaHpiDomainEventT();
        }
        if ( mod == SAHPI_ET_SENSOR ) {
            x.SensorEvent = demarshalSaHpiSensorEventT();
        }
        if ( mod == SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            x.SensorEnableChangeEvent = demarshalSaHpiSensorEnableChangeEventT();
        }
        if ( mod == SAHPI_ET_HOTSWAP ) {
            x.HotSwapEvent = demarshalSaHpiHotSwapEventT();
        }
        if ( mod == SAHPI_ET_WATCHDOG ) {
            x.WatchdogEvent = demarshalSaHpiWatchdogEventT();
        }
        if ( mod == SAHPI_ET_HPI_SW ) {
            x.HpiSwEvent = demarshalSaHpiHpiSwEventT();
        }
        if ( mod == SAHPI_ET_OEM ) {
            x.OemEvent = demarshalSaHpiOemEventT();
        }
        if ( mod == SAHPI_ET_USER ) {
            x.UserEvent = demarshalSaHpiUserEventT();
        }
        if ( mod == SAHPI_ET_DIMI ) {
            x.DimiEvent = demarshalSaHpiDimiEventT();
        }
        if ( mod == SAHPI_ET_DIMI_UPDATE ) {
            x.DimiUpdateEvent = demarshalSaHpiDimiUpdateEventT();
        }
        if ( mod == SAHPI_ET_FUMI ) {
            x.FumiEvent = demarshalSaHpiFumiEventT();
        }

        return x;
    }

    public SaHpiEventT demarshalSaHpiEventT()
        throws HpiException
    {
        SaHpiEventT x = new SaHpiEventT();

        x.Source = demarshalSaHpiResourceIdT();
        x.EventType = demarshalSaHpiEventTypeT();
        x.Timestamp = demarshalSaHpiTimeT();
        x.Severity = demarshalSaHpiSeverityT();
        x.EventDataUnion = demarshalSaHpiEventUnionT( x.EventType );

        return x;
    }

    public SaHpiNameT demarshalSaHpiNameT()
        throws HpiException
    {
        SaHpiNameT x = new SaHpiNameT();

        x.Length = demarshalSaHpiUint16T();
        x.Value = demarshalByteArray( SA_HPI_MAX_NAME_LENGTH );

        return x;
    }

    public SaHpiConditionT demarshalSaHpiConditionT()
        throws HpiException
    {
        SaHpiConditionT x = new SaHpiConditionT();

        x.Type = demarshalSaHpiStatusCondTypeT();
        x.Entity = demarshalSaHpiEntityPathT();
        x.DomainId = demarshalSaHpiDomainIdT();
        x.ResourceId = demarshalSaHpiResourceIdT();
        x.SensorNum = demarshalSaHpiSensorNumT();
        x.EventState = demarshalSaHpiEventStateT();
        x.Name = demarshalSaHpiNameT();
        x.Mid = demarshalSaHpiManufacturerIdT();
        x.Data = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiAnnouncementT demarshalSaHpiAnnouncementT()
        throws HpiException
    {
        SaHpiAnnouncementT x = new SaHpiAnnouncementT();

        x.EntryId = demarshalSaHpiEntryIdT();
        x.Timestamp = demarshalSaHpiTimeT();
        x.AddedByUser = demarshalSaHpiBoolT();
        x.Severity = demarshalSaHpiSeverityT();
        x.Acknowledged = demarshalSaHpiBoolT();
        x.StatusCond = demarshalSaHpiConditionT();

        return x;
    }

    public SaHpiAnnunciatorRecT demarshalSaHpiAnnunciatorRecT()
        throws HpiException
    {
        SaHpiAnnunciatorRecT x = new SaHpiAnnunciatorRecT();

        x.AnnunciatorNum = demarshalSaHpiAnnunciatorNumT();
        x.AnnunciatorType = demarshalSaHpiAnnunciatorTypeT();
        x.ModeReadOnly = demarshalSaHpiBoolT();
        x.MaxConditions = demarshalSaHpiUint32T();
        x.Oem = demarshalSaHpiUint32T();

        return x;
    }

    public SaHpiRdrTypeUnionT demarshalSaHpiRdrTypeUnionT( long mod )
        throws HpiException
    {
        SaHpiRdrTypeUnionT x = new SaHpiRdrTypeUnionT();

        if ( mod == SAHPI_CTRL_RDR ) {
            x.CtrlRec = demarshalSaHpiCtrlRecT();
        }
        if ( mod == SAHPI_SENSOR_RDR ) {
            x.SensorRec = demarshalSaHpiSensorRecT();
        }
        if ( mod == SAHPI_INVENTORY_RDR ) {
            x.InventoryRec = demarshalSaHpiInventoryRecT();
        }
        if ( mod == SAHPI_WATCHDOG_RDR ) {
            x.WatchdogRec = demarshalSaHpiWatchdogRecT();
        }
        if ( mod == SAHPI_ANNUNCIATOR_RDR ) {
            x.AnnunciatorRec = demarshalSaHpiAnnunciatorRecT();
        }
        if ( mod == SAHPI_DIMI_RDR ) {
            x.DimiRec = demarshalSaHpiDimiRecT();
        }
        if ( mod == SAHPI_FUMI_RDR ) {
            x.FumiRec = demarshalSaHpiFumiRecT();
        }

        return x;
    }

    public SaHpiRdrT demarshalSaHpiRdrT()
        throws HpiException
    {
        SaHpiRdrT x = new SaHpiRdrT();

        x.RecordId = demarshalSaHpiEntryIdT();
        x.RdrType = demarshalSaHpiRdrTypeT();
        x.Entity = demarshalSaHpiEntityPathT();
        x.IsFru = demarshalSaHpiBoolT();
        x.RdrTypeUnion = demarshalSaHpiRdrTypeUnionT( x.RdrType );
        x.IdString = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiLoadIdT demarshalSaHpiLoadIdT()
        throws HpiException
    {
        SaHpiLoadIdT x = new SaHpiLoadIdT();

        x.LoadNumber = demarshalSaHpiLoadNumberT();
        x.LoadName = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiResourceInfoT demarshalSaHpiResourceInfoT()
        throws HpiException
    {
        SaHpiResourceInfoT x = new SaHpiResourceInfoT();

        x.ResourceRev = demarshalSaHpiUint8T();
        x.SpecificVer = demarshalSaHpiUint8T();
        x.DeviceSupport = demarshalSaHpiUint8T();
        x.ManufacturerId = demarshalSaHpiManufacturerIdT();
        x.ProductId = demarshalSaHpiUint16T();
        x.FirmwareMajorRev = demarshalSaHpiUint8T();
        x.FirmwareMinorRev = demarshalSaHpiUint8T();
        x.AuxFirmwareRev = demarshalSaHpiUint8T();
        x.Guid = demarshalByteArray( SAHPI_GUID_LENGTH );

        return x;
    }

    public SaHpiRptEntryT demarshalSaHpiRptEntryT()
        throws HpiException
    {
        SaHpiRptEntryT x = new SaHpiRptEntryT();

        x.EntryId = demarshalSaHpiEntryIdT();
        x.ResourceId = demarshalSaHpiResourceIdT();
        x.ResourceInfo = demarshalSaHpiResourceInfoT();
        x.ResourceEntity = demarshalSaHpiEntityPathT();
        x.ResourceCapabilities = demarshalSaHpiCapabilitiesT();
        x.HotSwapCapabilities = demarshalSaHpiHsCapabilitiesT();
        x.ResourceSeverity = demarshalSaHpiSeverityT();
        x.ResourceFailed = demarshalSaHpiBoolT();
        x.ResourceTag = demarshalSaHpiTextBufferT();

        return x;
    }

    public SaHpiDomainInfoT demarshalSaHpiDomainInfoT()
        throws HpiException
    {
        SaHpiDomainInfoT x = new SaHpiDomainInfoT();

        x.DomainId = demarshalSaHpiDomainIdT();
        x.DomainCapabilities = demarshalSaHpiDomainCapabilitiesT();
        x.IsPeer = demarshalSaHpiBoolT();
        x.DomainTag = demarshalSaHpiTextBufferT();
        x.DrtUpdateCount = demarshalSaHpiUint32T();
        x.DrtUpdateTimestamp = demarshalSaHpiTimeT();
        x.RptUpdateCount = demarshalSaHpiUint32T();
        x.RptUpdateTimestamp = demarshalSaHpiTimeT();
        x.DatUpdateCount = demarshalSaHpiUint32T();
        x.DatUpdateTimestamp = demarshalSaHpiTimeT();
        x.ActiveAlarms = demarshalSaHpiUint32T();
        x.CriticalAlarms = demarshalSaHpiUint32T();
        x.MajorAlarms = demarshalSaHpiUint32T();
        x.MinorAlarms = demarshalSaHpiUint32T();
        x.DatUserAlarmLimit = demarshalSaHpiUint32T();
        x.DatOverflow = demarshalSaHpiBoolT();
        x.Guid = demarshalByteArray( SAHPI_GUID_LENGTH );

        return x;
    }

    public SaHpiDrtEntryT demarshalSaHpiDrtEntryT()
        throws HpiException
    {
        SaHpiDrtEntryT x = new SaHpiDrtEntryT();

        x.EntryId = demarshalSaHpiEntryIdT();
        x.DomainId = demarshalSaHpiDomainIdT();
        x.IsPeer = demarshalSaHpiBoolT();

        return x;
    }

    public SaHpiAlarmT demarshalSaHpiAlarmT()
        throws HpiException
    {
        SaHpiAlarmT x = new SaHpiAlarmT();

        x.AlarmId = demarshalSaHpiAlarmIdT();
        x.Timestamp = demarshalSaHpiTimeT();
        x.Severity = demarshalSaHpiSeverityT();
        x.Acknowledged = demarshalSaHpiBoolT();
        x.AlarmCond = demarshalSaHpiConditionT();

        return x;
    }

    public SaHpiEventLogInfoT demarshalSaHpiEventLogInfoT()
        throws HpiException
    {
        SaHpiEventLogInfoT x = new SaHpiEventLogInfoT();

        x.Entries = demarshalSaHpiUint32T();
        x.Size = demarshalSaHpiUint32T();
        x.UserEventMaxSize = demarshalSaHpiUint32T();
        x.UpdateTimestamp = demarshalSaHpiTimeT();
        x.CurrentTime = demarshalSaHpiTimeT();
        x.Enabled = demarshalSaHpiBoolT();
        x.OverflowFlag = demarshalSaHpiBoolT();
        x.OverflowResetable = demarshalSaHpiBoolT();
        x.OverflowAction = demarshalSaHpiEventLogOverflowActionT();

        return x;
    }

    public SaHpiEventLogEntryT demarshalSaHpiEventLogEntryT()
        throws HpiException
    {
        SaHpiEventLogEntryT x = new SaHpiEventLogEntryT();

        x.EntryId = demarshalSaHpiEventLogEntryIdT();
        x.Timestamp = demarshalSaHpiTimeT();
        x.Event = demarshalSaHpiEventT();

        return x;
    }

};

