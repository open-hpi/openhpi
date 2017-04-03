
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


#**********************************************************
# HPI Utility Functions (auto-generated)
#**********************************************************

#**********************************************************
# Check Functions for HPI Complex Data Types
#**********************************************************

#**
# Check function for HPI struct SaHpiTextBufferT
#**
def checkSaHpiTextBufferT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiTextBufferT ):
        return False
    if not hasattr( x, "DataType" ):
        return False
    if not hasattr( x, "Language" ):
        return False
    if not hasattr( x, "DataLength" ):
        return False
    if not hasattr( x, "Data" ):
        return False
    if not isinstance( x.Data, str ):
        return False
    if len( x.Data ) != SAHPI_MAX_TEXT_BUFFER_LENGTH:
        return False
    return True

#**
# Check function for HPI struct SaHpiEntityT
#**
def checkSaHpiEntityT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEntityT ):
        return False
    if not hasattr( x, "EntityType" ):
        return False
    if not hasattr( x, "EntityLocation" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiEntityPathT
#**
def checkSaHpiEntityPathT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEntityPathT ):
        return False
    if not hasattr( x, "Entry" ):
        return False
    if not isinstance( x.Entry, list ):
        return False
    if len( x.Entry ) != SAHPI_MAX_ENTITY_PATH:
        return False
    for i in range( 0, SAHPI_MAX_ENTITY_PATH ):
        if not checkSaHpiEntityT( x.Entry[i] ):
            return False
    return True

#**
# Check function for HPI union SaHpiSensorReadingUnionT
#**
def checkSaHpiSensorReadingUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorReadingUnionT ):
        return False
    if mod == SAHPI_SENSOR_READING_TYPE_INT64:
        if not hasattr( x, "SensorInt64" ):
            return False
    if mod == SAHPI_SENSOR_READING_TYPE_UINT64:
        if not hasattr( x, "SensorUint64" ):
            return False
    if mod == SAHPI_SENSOR_READING_TYPE_FLOAT64:
        if not hasattr( x, "SensorFloat64" ):
            return False
    if mod == SAHPI_SENSOR_READING_TYPE_BUFFER:
        if not hasattr( x, "SensorBuffer" ):
            return False
    if mod == SAHPI_SENSOR_READING_TYPE_BUFFER:
        if not isinstance( x.SensorBuffer, str ):
            return False
    if mod == SAHPI_SENSOR_READING_TYPE_BUFFER:
        if len( x.SensorBuffer ) != SAHPI_SENSOR_BUFFER_LENGTH:
            return False
    return True

#**
# Check function for HPI struct SaHpiSensorReadingT
#**
def checkSaHpiSensorReadingT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorReadingT ):
        return False
    if not hasattr( x, "IsSupported" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "Value" ):
        return False
    if not checkSaHpiSensorReadingUnionT( x.Value, x.Type ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorThresholdsT
#**
def checkSaHpiSensorThresholdsT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorThresholdsT ):
        return False
    if not hasattr( x, "LowCritical" ):
        return False
    if not checkSaHpiSensorReadingT( x.LowCritical ):
        return False
    if not hasattr( x, "LowMajor" ):
        return False
    if not checkSaHpiSensorReadingT( x.LowMajor ):
        return False
    if not hasattr( x, "LowMinor" ):
        return False
    if not checkSaHpiSensorReadingT( x.LowMinor ):
        return False
    if not hasattr( x, "UpCritical" ):
        return False
    if not checkSaHpiSensorReadingT( x.UpCritical ):
        return False
    if not hasattr( x, "UpMajor" ):
        return False
    if not checkSaHpiSensorReadingT( x.UpMajor ):
        return False
    if not hasattr( x, "UpMinor" ):
        return False
    if not checkSaHpiSensorReadingT( x.UpMinor ):
        return False
    if not hasattr( x, "PosThdHysteresis" ):
        return False
    if not checkSaHpiSensorReadingT( x.PosThdHysteresis ):
        return False
    if not hasattr( x, "NegThdHysteresis" ):
        return False
    if not checkSaHpiSensorReadingT( x.NegThdHysteresis ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorRangeT
#**
def checkSaHpiSensorRangeT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorRangeT ):
        return False
    if not hasattr( x, "Flags" ):
        return False
    if not hasattr( x, "Max" ):
        return False
    if not checkSaHpiSensorReadingT( x.Max ):
        return False
    if not hasattr( x, "Min" ):
        return False
    if not checkSaHpiSensorReadingT( x.Min ):
        return False
    if not hasattr( x, "Nominal" ):
        return False
    if not checkSaHpiSensorReadingT( x.Nominal ):
        return False
    if not hasattr( x, "NormalMax" ):
        return False
    if not checkSaHpiSensorReadingT( x.NormalMax ):
        return False
    if not hasattr( x, "NormalMin" ):
        return False
    if not checkSaHpiSensorReadingT( x.NormalMin ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorDataFormatT
#**
def checkSaHpiSensorDataFormatT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorDataFormatT ):
        return False
    if not hasattr( x, "IsSupported" ):
        return False
    if not hasattr( x, "ReadingType" ):
        return False
    if not hasattr( x, "BaseUnits" ):
        return False
    if not hasattr( x, "ModifierUnits" ):
        return False
    if not hasattr( x, "ModifierUse" ):
        return False
    if not hasattr( x, "Percentage" ):
        return False
    if not hasattr( x, "Range" ):
        return False
    if not checkSaHpiSensorRangeT( x.Range ):
        return False
    if not hasattr( x, "AccuracyFactor" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorThdDefnT
#**
def checkSaHpiSensorThdDefnT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorThdDefnT ):
        return False
    if not hasattr( x, "IsAccessible" ):
        return False
    if not hasattr( x, "ReadThold" ):
        return False
    if not hasattr( x, "WriteThold" ):
        return False
    if not hasattr( x, "Nonlinear" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorRecT
#**
def checkSaHpiSensorRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorRecT ):
        return False
    if not hasattr( x, "Num" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "Category" ):
        return False
    if not hasattr( x, "EnableCtrl" ):
        return False
    if not hasattr( x, "EventCtrl" ):
        return False
    if not hasattr( x, "Events" ):
        return False
    if not hasattr( x, "DataFormat" ):
        return False
    if not checkSaHpiSensorDataFormatT( x.DataFormat ):
        return False
    if not hasattr( x, "ThresholdDefn" ):
        return False
    if not checkSaHpiSensorThdDefnT( x.ThresholdDefn ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlStateStreamT
#**
def checkSaHpiCtrlStateStreamT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlStateStreamT ):
        return False
    if not hasattr( x, "Repeat" ):
        return False
    if not hasattr( x, "StreamLength" ):
        return False
    if not hasattr( x, "Stream" ):
        return False
    if not isinstance( x.Stream, str ):
        return False
    if len( x.Stream ) != SAHPI_CTRL_MAX_STREAM_LENGTH:
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlStateTextT
#**
def checkSaHpiCtrlStateTextT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlStateTextT ):
        return False
    if not hasattr( x, "Line" ):
        return False
    if not hasattr( x, "Text" ):
        return False
    if not checkSaHpiTextBufferT( x.Text ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlStateOemT
#**
def checkSaHpiCtrlStateOemT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlStateOemT ):
        return False
    if not hasattr( x, "MId" ):
        return False
    if not hasattr( x, "BodyLength" ):
        return False
    if not hasattr( x, "Body" ):
        return False
    if not isinstance( x.Body, str ):
        return False
    if len( x.Body ) != SAHPI_CTRL_MAX_OEM_BODY_LENGTH:
        return False
    return True

#**
# Check function for HPI union SaHpiCtrlStateUnionT
#**
def checkSaHpiCtrlStateUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlStateUnionT ):
        return False
    if mod == SAHPI_CTRL_TYPE_DIGITAL:
        if not hasattr( x, "Digital" ):
            return False
    if mod == SAHPI_CTRL_TYPE_DISCRETE:
        if not hasattr( x, "Discrete" ):
            return False
    if mod == SAHPI_CTRL_TYPE_ANALOG:
        if not hasattr( x, "Analog" ):
            return False
    if mod == SAHPI_CTRL_TYPE_STREAM:
        if not hasattr( x, "Stream" ):
            return False
    if mod == SAHPI_CTRL_TYPE_STREAM:
        if not checkSaHpiCtrlStateStreamT( x.Stream ):
            return False
    if mod == SAHPI_CTRL_TYPE_TEXT:
        if not hasattr( x, "Text" ):
            return False
    if mod == SAHPI_CTRL_TYPE_TEXT:
        if not checkSaHpiCtrlStateTextT( x.Text ):
            return False
    if mod == SAHPI_CTRL_TYPE_OEM:
        if not hasattr( x, "Oem" ):
            return False
    if mod == SAHPI_CTRL_TYPE_OEM:
        if not checkSaHpiCtrlStateOemT( x.Oem ):
            return False
    return True

#**
# Check function for HPI struct SaHpiCtrlStateT
#**
def checkSaHpiCtrlStateT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlStateT ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "StateUnion" ):
        return False
    if not checkSaHpiCtrlStateUnionT( x.StateUnion, x.Type ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecDigitalT
#**
def checkSaHpiCtrlRecDigitalT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecDigitalT ):
        return False
    if not hasattr( x, "Default" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecDiscreteT
#**
def checkSaHpiCtrlRecDiscreteT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecDiscreteT ):
        return False
    if not hasattr( x, "Default" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecAnalogT
#**
def checkSaHpiCtrlRecAnalogT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecAnalogT ):
        return False
    if not hasattr( x, "Min" ):
        return False
    if not hasattr( x, "Max" ):
        return False
    if not hasattr( x, "Default" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecStreamT
#**
def checkSaHpiCtrlRecStreamT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecStreamT ):
        return False
    if not hasattr( x, "Default" ):
        return False
    if not checkSaHpiCtrlStateStreamT( x.Default ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecTextT
#**
def checkSaHpiCtrlRecTextT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecTextT ):
        return False
    if not hasattr( x, "MaxChars" ):
        return False
    if not hasattr( x, "MaxLines" ):
        return False
    if not hasattr( x, "Language" ):
        return False
    if not hasattr( x, "DataType" ):
        return False
    if not hasattr( x, "Default" ):
        return False
    if not checkSaHpiCtrlStateTextT( x.Default ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecOemT
#**
def checkSaHpiCtrlRecOemT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecOemT ):
        return False
    if not hasattr( x, "MId" ):
        return False
    if not hasattr( x, "ConfigData" ):
        return False
    if not isinstance( x.ConfigData, str ):
        return False
    if len( x.ConfigData ) != SAHPI_CTRL_OEM_CONFIG_LENGTH:
        return False
    if not hasattr( x, "Default" ):
        return False
    if not checkSaHpiCtrlStateOemT( x.Default ):
        return False
    return True

#**
# Check function for HPI union SaHpiCtrlRecUnionT
#**
def checkSaHpiCtrlRecUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecUnionT ):
        return False
    if mod == SAHPI_CTRL_TYPE_DIGITAL:
        if not hasattr( x, "Digital" ):
            return False
    if mod == SAHPI_CTRL_TYPE_DIGITAL:
        if not checkSaHpiCtrlRecDigitalT( x.Digital ):
            return False
    if mod == SAHPI_CTRL_TYPE_DISCRETE:
        if not hasattr( x, "Discrete" ):
            return False
    if mod == SAHPI_CTRL_TYPE_DISCRETE:
        if not checkSaHpiCtrlRecDiscreteT( x.Discrete ):
            return False
    if mod == SAHPI_CTRL_TYPE_ANALOG:
        if not hasattr( x, "Analog" ):
            return False
    if mod == SAHPI_CTRL_TYPE_ANALOG:
        if not checkSaHpiCtrlRecAnalogT( x.Analog ):
            return False
    if mod == SAHPI_CTRL_TYPE_STREAM:
        if not hasattr( x, "Stream" ):
            return False
    if mod == SAHPI_CTRL_TYPE_STREAM:
        if not checkSaHpiCtrlRecStreamT( x.Stream ):
            return False
    if mod == SAHPI_CTRL_TYPE_TEXT:
        if not hasattr( x, "Text" ):
            return False
    if mod == SAHPI_CTRL_TYPE_TEXT:
        if not checkSaHpiCtrlRecTextT( x.Text ):
            return False
    if mod == SAHPI_CTRL_TYPE_OEM:
        if not hasattr( x, "Oem" ):
            return False
    if mod == SAHPI_CTRL_TYPE_OEM:
        if not checkSaHpiCtrlRecOemT( x.Oem ):
            return False
    return True

#**
# Check function for HPI struct SaHpiCtrlDefaultModeT
#**
def checkSaHpiCtrlDefaultModeT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlDefaultModeT ):
        return False
    if not hasattr( x, "Mode" ):
        return False
    if not hasattr( x, "ReadOnly" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiCtrlRecT
#**
def checkSaHpiCtrlRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiCtrlRecT ):
        return False
    if not hasattr( x, "Num" ):
        return False
    if not hasattr( x, "OutputType" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "TypeUnion" ):
        return False
    if not checkSaHpiCtrlRecUnionT( x.TypeUnion, x.Type ):
        return False
    if not hasattr( x, "DefaultMode" ):
        return False
    if not checkSaHpiCtrlDefaultModeT( x.DefaultMode ):
        return False
    if not hasattr( x, "WriteOnly" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiIdrFieldT
#**
def checkSaHpiIdrFieldT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiIdrFieldT ):
        return False
    if not hasattr( x, "AreaId" ):
        return False
    if not hasattr( x, "FieldId" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "ReadOnly" ):
        return False
    if not hasattr( x, "Field" ):
        return False
    if not checkSaHpiTextBufferT( x.Field ):
        return False
    return True

#**
# Check function for HPI struct SaHpiIdrAreaHeaderT
#**
def checkSaHpiIdrAreaHeaderT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiIdrAreaHeaderT ):
        return False
    if not hasattr( x, "AreaId" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "ReadOnly" ):
        return False
    if not hasattr( x, "NumFields" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiIdrInfoT
#**
def checkSaHpiIdrInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiIdrInfoT ):
        return False
    if not hasattr( x, "IdrId" ):
        return False
    if not hasattr( x, "UpdateCount" ):
        return False
    if not hasattr( x, "ReadOnly" ):
        return False
    if not hasattr( x, "NumAreas" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiInventoryRecT
#**
def checkSaHpiInventoryRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiInventoryRecT ):
        return False
    if not hasattr( x, "IdrId" ):
        return False
    if not hasattr( x, "Persistent" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiWatchdogT
#**
def checkSaHpiWatchdogT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiWatchdogT ):
        return False
    if not hasattr( x, "Log" ):
        return False
    if not hasattr( x, "Running" ):
        return False
    if not hasattr( x, "TimerUse" ):
        return False
    if not hasattr( x, "TimerAction" ):
        return False
    if not hasattr( x, "PretimerInterrupt" ):
        return False
    if not hasattr( x, "PreTimeoutInterval" ):
        return False
    if not hasattr( x, "TimerUseExpFlags" ):
        return False
    if not hasattr( x, "InitialCount" ):
        return False
    if not hasattr( x, "PresentCount" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiWatchdogRecT
#**
def checkSaHpiWatchdogRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiWatchdogRecT ):
        return False
    if not hasattr( x, "WatchdogNum" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiTestAffectedEntityT
#**
def checkSaHpiDimiTestAffectedEntityT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestAffectedEntityT ):
        return False
    if not hasattr( x, "EntityImpacted" ):
        return False
    if not checkSaHpiEntityPathT( x.EntityImpacted ):
        return False
    if not hasattr( x, "ServiceImpact" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiTestResultsT
#**
def checkSaHpiDimiTestResultsT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestResultsT ):
        return False
    if not hasattr( x, "ResultTimeStamp" ):
        return False
    if not hasattr( x, "RunDuration" ):
        return False
    if not hasattr( x, "LastRunStatus" ):
        return False
    if not hasattr( x, "TestErrorCode" ):
        return False
    if not hasattr( x, "TestResultString" ):
        return False
    if not checkSaHpiTextBufferT( x.TestResultString ):
        return False
    if not hasattr( x, "TestResultStringIsURI" ):
        return False
    return True

#**
# Check function for HPI union SaHpiDimiTestParamValueT
#**
def checkSaHpiDimiTestParamValueT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestParamValueT ):
        return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
        if not hasattr( x, "paramint" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
        if not hasattr( x, "parambool" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
        if not hasattr( x, "paramfloat" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
        if not hasattr( x, "paramtext" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
        if not checkSaHpiTextBufferT( x.paramtext ):
            return False
    return True

#**
# Check function for HPI union SaHpiDimiTestParameterValueUnionT
#**
def checkSaHpiDimiTestParameterValueUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestParameterValueUnionT ):
        return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_INT32:
        if not hasattr( x, "IntValue" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
        if not hasattr( x, "FloatValue" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
        if not hasattr( x, "FloatValue" ):
            return False
    if mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
        if not hasattr( x, "FloatValue" ):
            return False
    return True

#**
# Check function for HPI struct SaHpiDimiTestParamsDefinitionT
#**
def checkSaHpiDimiTestParamsDefinitionT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestParamsDefinitionT ):
        return False
    if not hasattr( x, "ParamName" ):
        return False
    if not isinstance( x.ParamName, str ):
        return False
    if len( x.ParamName ) != SAHPI_DIMITEST_PARAM_NAME_LEN:
        return False
    if not hasattr( x, "ParamInfo" ):
        return False
    if not checkSaHpiTextBufferT( x.ParamInfo ):
        return False
    if not hasattr( x, "ParamType" ):
        return False
    if not hasattr( x, "MinValue" ):
        return False
    if not checkSaHpiDimiTestParameterValueUnionT( x.MinValue, x.ParamType ):
        return False
    if not hasattr( x, "MaxValue" ):
        return False
    if not checkSaHpiDimiTestParameterValueUnionT( x.MaxValue, x.ParamType ):
        return False
    if not hasattr( x, "DefaultParam" ):
        return False
    if not checkSaHpiDimiTestParamValueT( x.DefaultParam, x.ParamType ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiTestT
#**
def checkSaHpiDimiTestT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestT ):
        return False
    if not hasattr( x, "TestName" ):
        return False
    if not checkSaHpiTextBufferT( x.TestName ):
        return False
    if not hasattr( x, "ServiceImpact" ):
        return False
    if not hasattr( x, "EntitiesImpacted" ):
        return False
    if not isinstance( x.EntitiesImpacted, list ):
        return False
    if len( x.EntitiesImpacted ) != SAHPI_DIMITEST_MAX_ENTITIESIMPACTED:
        return False
    for i in range( 0, SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ):
        if not checkSaHpiDimiTestAffectedEntityT( x.EntitiesImpacted[i] ):
            return False
    if not hasattr( x, "NeedServiceOS" ):
        return False
    if not hasattr( x, "ServiceOS" ):
        return False
    if not checkSaHpiTextBufferT( x.ServiceOS ):
        return False
    if not hasattr( x, "ExpectedRunDuration" ):
        return False
    if not hasattr( x, "TestCapabilities" ):
        return False
    if not hasattr( x, "TestParameters" ):
        return False
    if not isinstance( x.TestParameters, list ):
        return False
    if len( x.TestParameters ) != SAHPI_DIMITEST_MAX_PARAMETERS:
        return False
    for i in range( 0, SAHPI_DIMITEST_MAX_PARAMETERS ):
        if not checkSaHpiDimiTestParamsDefinitionT( x.TestParameters[i] ):
            return False
    return True

#**
# Check function for HPI struct SaHpiDimiTestVariableParamsT
#**
def checkSaHpiDimiTestVariableParamsT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiTestVariableParamsT ):
        return False
    if not hasattr( x, "ParamName" ):
        return False
    if not isinstance( x.ParamName, str ):
        return False
    if len( x.ParamName ) != SAHPI_DIMITEST_PARAM_NAME_LEN:
        return False
    if not hasattr( x, "ParamType" ):
        return False
    if not hasattr( x, "Value" ):
        return False
    if not checkSaHpiDimiTestParamValueT( x.Value, x.ParamType ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiInfoT
#**
def checkSaHpiDimiInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiInfoT ):
        return False
    if not hasattr( x, "NumberOfTests" ):
        return False
    if not hasattr( x, "TestNumUpdateCounter" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiRecT
#**
def checkSaHpiDimiRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiRecT ):
        return False
    if not hasattr( x, "DimiNum" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiSafDefinedSpecInfoT
#**
def checkSaHpiFumiSafDefinedSpecInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiSafDefinedSpecInfoT ):
        return False
    if not hasattr( x, "SpecID" ):
        return False
    if not hasattr( x, "RevisionID" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiOemDefinedSpecInfoT
#**
def checkSaHpiFumiOemDefinedSpecInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiOemDefinedSpecInfoT ):
        return False
    if not hasattr( x, "Mid" ):
        return False
    if not hasattr( x, "BodyLength" ):
        return False
    if not hasattr( x, "Body" ):
        return False
    if not isinstance( x.Body, str ):
        return False
    if len( x.Body ) != SAHPI_FUMI_MAX_OEM_BODY_LENGTH:
        return False
    return True

#**
# Check function for HPI union SaHpiFumiSpecInfoTypeUnionT
#**
def checkSaHpiFumiSpecInfoTypeUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiSpecInfoTypeUnionT ):
        return False
    if mod == SAHPI_FUMI_SPEC_INFO_NONE:
        if not hasattr( x, "None" ):
            return False
    if mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
        if not hasattr( x, "SafDefined" ):
            return False
    if mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
        if not checkSaHpiFumiSafDefinedSpecInfoT( x.SafDefined ):
            return False
    if mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
        if not hasattr( x, "OemDefined" ):
            return False
    if mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
        if not checkSaHpiFumiOemDefinedSpecInfoT( x.OemDefined ):
            return False
    return True

#**
# Check function for HPI struct SaHpiFumiSpecInfoT
#**
def checkSaHpiFumiSpecInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiSpecInfoT ):
        return False
    if not hasattr( x, "SpecInfoType" ):
        return False
    if not hasattr( x, "SpecInfoTypeUnion" ):
        return False
    if not checkSaHpiFumiSpecInfoTypeUnionT( x.SpecInfoTypeUnion, x.SpecInfoType ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiFirmwareInstanceInfoT
#**
def checkSaHpiFumiFirmwareInstanceInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiFirmwareInstanceInfoT ):
        return False
    if not hasattr( x, "InstancePresent" ):
        return False
    if not hasattr( x, "Identifier" ):
        return False
    if not checkSaHpiTextBufferT( x.Identifier ):
        return False
    if not hasattr( x, "Description" ):
        return False
    if not checkSaHpiTextBufferT( x.Description ):
        return False
    if not hasattr( x, "DateTime" ):
        return False
    if not checkSaHpiTextBufferT( x.DateTime ):
        return False
    if not hasattr( x, "MajorVersion" ):
        return False
    if not hasattr( x, "MinorVersion" ):
        return False
    if not hasattr( x, "AuxVersion" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiImpactedEntityT
#**
def checkSaHpiFumiImpactedEntityT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiImpactedEntityT ):
        return False
    if not hasattr( x, "ImpactedEntity" ):
        return False
    if not checkSaHpiEntityPathT( x.ImpactedEntity ):
        return False
    if not hasattr( x, "ServiceImpact" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiServiceImpactDataT
#**
def checkSaHpiFumiServiceImpactDataT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiServiceImpactDataT ):
        return False
    if not hasattr( x, "NumEntities" ):
        return False
    if not hasattr( x, "ImpactedEntities" ):
        return False
    if not isinstance( x.ImpactedEntities, list ):
        return False
    if len( x.ImpactedEntities ) != SAHPI_FUMI_MAX_ENTITIES_IMPACTED:
        return False
    for i in range( 0, SAHPI_FUMI_MAX_ENTITIES_IMPACTED ):
        if not checkSaHpiFumiImpactedEntityT( x.ImpactedEntities[i] ):
            return False
    return True

#**
# Check function for HPI struct SaHpiFumiSourceInfoT
#**
def checkSaHpiFumiSourceInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiSourceInfoT ):
        return False
    if not hasattr( x, "SourceUri" ):
        return False
    if not checkSaHpiTextBufferT( x.SourceUri ):
        return False
    if not hasattr( x, "SourceStatus" ):
        return False
    if not hasattr( x, "Identifier" ):
        return False
    if not checkSaHpiTextBufferT( x.Identifier ):
        return False
    if not hasattr( x, "Description" ):
        return False
    if not checkSaHpiTextBufferT( x.Description ):
        return False
    if not hasattr( x, "DateTime" ):
        return False
    if not checkSaHpiTextBufferT( x.DateTime ):
        return False
    if not hasattr( x, "MajorVersion" ):
        return False
    if not hasattr( x, "MinorVersion" ):
        return False
    if not hasattr( x, "AuxVersion" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiComponentInfoT
#**
def checkSaHpiFumiComponentInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiComponentInfoT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "ComponentId" ):
        return False
    if not hasattr( x, "MainFwInstance" ):
        return False
    if not checkSaHpiFumiFirmwareInstanceInfoT( x.MainFwInstance ):
        return False
    if not hasattr( x, "ComponentFlags" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiBankInfoT
#**
def checkSaHpiFumiBankInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiBankInfoT ):
        return False
    if not hasattr( x, "BankId" ):
        return False
    if not hasattr( x, "BankSize" ):
        return False
    if not hasattr( x, "Position" ):
        return False
    if not hasattr( x, "BankState" ):
        return False
    if not hasattr( x, "Identifier" ):
        return False
    if not checkSaHpiTextBufferT( x.Identifier ):
        return False
    if not hasattr( x, "Description" ):
        return False
    if not checkSaHpiTextBufferT( x.Description ):
        return False
    if not hasattr( x, "DateTime" ):
        return False
    if not checkSaHpiTextBufferT( x.DateTime ):
        return False
    if not hasattr( x, "MajorVersion" ):
        return False
    if not hasattr( x, "MinorVersion" ):
        return False
    if not hasattr( x, "AuxVersion" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiLogicalBankInfoT
#**
def checkSaHpiFumiLogicalBankInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiLogicalBankInfoT ):
        return False
    if not hasattr( x, "FirmwarePersistentLocationCount" ):
        return False
    if not hasattr( x, "BankStateFlags" ):
        return False
    if not hasattr( x, "PendingFwInstance" ):
        return False
    if not checkSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance ):
        return False
    if not hasattr( x, "RollbackFwInstance" ):
        return False
    if not checkSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiLogicalComponentInfoT
#**
def checkSaHpiFumiLogicalComponentInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiLogicalComponentInfoT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "ComponentId" ):
        return False
    if not hasattr( x, "PendingFwInstance" ):
        return False
    if not checkSaHpiFumiFirmwareInstanceInfoT( x.PendingFwInstance ):
        return False
    if not hasattr( x, "RollbackFwInstance" ):
        return False
    if not checkSaHpiFumiFirmwareInstanceInfoT( x.RollbackFwInstance ):
        return False
    if not hasattr( x, "ComponentFlags" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiRecT
#**
def checkSaHpiFumiRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiRecT ):
        return False
    if not hasattr( x, "Num" ):
        return False
    if not hasattr( x, "AccessProt" ):
        return False
    if not hasattr( x, "Capability" ):
        return False
    if not hasattr( x, "NumBanks" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiResourceEventT
#**
def checkSaHpiResourceEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiResourceEventT ):
        return False
    if not hasattr( x, "ResourceEventType" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDomainEventT
#**
def checkSaHpiDomainEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDomainEventT ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "DomainId" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorEventT
#**
def checkSaHpiSensorEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorEventT ):
        return False
    if not hasattr( x, "SensorNum" ):
        return False
    if not hasattr( x, "SensorType" ):
        return False
    if not hasattr( x, "EventCategory" ):
        return False
    if not hasattr( x, "Assertion" ):
        return False
    if not hasattr( x, "EventState" ):
        return False
    if not hasattr( x, "OptionalDataPresent" ):
        return False
    if not hasattr( x, "TriggerReading" ):
        return False
    if not checkSaHpiSensorReadingT( x.TriggerReading ):
        return False
    if not hasattr( x, "TriggerThreshold" ):
        return False
    if not checkSaHpiSensorReadingT( x.TriggerThreshold ):
        return False
    if not hasattr( x, "PreviousState" ):
        return False
    if not hasattr( x, "CurrentState" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    if not hasattr( x, "SensorSpecific" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiSensorEnableChangeEventT
#**
def checkSaHpiSensorEnableChangeEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiSensorEnableChangeEventT ):
        return False
    if not hasattr( x, "SensorNum" ):
        return False
    if not hasattr( x, "SensorType" ):
        return False
    if not hasattr( x, "EventCategory" ):
        return False
    if not hasattr( x, "SensorEnable" ):
        return False
    if not hasattr( x, "SensorEventEnable" ):
        return False
    if not hasattr( x, "AssertEventMask" ):
        return False
    if not hasattr( x, "DeassertEventMask" ):
        return False
    if not hasattr( x, "OptionalDataPresent" ):
        return False
    if not hasattr( x, "CurrentState" ):
        return False
    if not hasattr( x, "CriticalAlarms" ):
        return False
    if not hasattr( x, "MajorAlarms" ):
        return False
    if not hasattr( x, "MinorAlarms" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiHotSwapEventT
#**
def checkSaHpiHotSwapEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiHotSwapEventT ):
        return False
    if not hasattr( x, "HotSwapState" ):
        return False
    if not hasattr( x, "PreviousHotSwapState" ):
        return False
    if not hasattr( x, "CauseOfStateChange" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiWatchdogEventT
#**
def checkSaHpiWatchdogEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiWatchdogEventT ):
        return False
    if not hasattr( x, "WatchdogNum" ):
        return False
    if not hasattr( x, "WatchdogAction" ):
        return False
    if not hasattr( x, "WatchdogPreTimerAction" ):
        return False
    if not hasattr( x, "WatchdogUse" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiHpiSwEventT
#**
def checkSaHpiHpiSwEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiHpiSwEventT ):
        return False
    if not hasattr( x, "MId" ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "EventData" ):
        return False
    if not checkSaHpiTextBufferT( x.EventData ):
        return False
    return True

#**
# Check function for HPI struct SaHpiOemEventT
#**
def checkSaHpiOemEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiOemEventT ):
        return False
    if not hasattr( x, "MId" ):
        return False
    if not hasattr( x, "OemEventData" ):
        return False
    if not checkSaHpiTextBufferT( x.OemEventData ):
        return False
    return True

#**
# Check function for HPI struct SaHpiUserEventT
#**
def checkSaHpiUserEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiUserEventT ):
        return False
    if not hasattr( x, "UserEventData" ):
        return False
    if not checkSaHpiTextBufferT( x.UserEventData ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiEventT
#**
def checkSaHpiDimiEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiEventT ):
        return False
    if not hasattr( x, "DimiNum" ):
        return False
    if not hasattr( x, "TestNum" ):
        return False
    if not hasattr( x, "DimiTestRunStatus" ):
        return False
    if not hasattr( x, "DimiTestPercentCompleted" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDimiUpdateEventT
#**
def checkSaHpiDimiUpdateEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDimiUpdateEventT ):
        return False
    if not hasattr( x, "DimiNum" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiFumiEventT
#**
def checkSaHpiFumiEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiFumiEventT ):
        return False
    if not hasattr( x, "FumiNum" ):
        return False
    if not hasattr( x, "BankNum" ):
        return False
    if not hasattr( x, "UpgradeStatus" ):
        return False
    return True

#**
# Check function for HPI union SaHpiEventUnionT
#**
def checkSaHpiEventUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEventUnionT ):
        return False
    if mod == SAHPI_ET_RESOURCE:
        if not hasattr( x, "ResourceEvent" ):
            return False
    if mod == SAHPI_ET_RESOURCE:
        if not checkSaHpiResourceEventT( x.ResourceEvent ):
            return False
    if mod == SAHPI_ET_DOMAIN:
        if not hasattr( x, "DomainEvent" ):
            return False
    if mod == SAHPI_ET_DOMAIN:
        if not checkSaHpiDomainEventT( x.DomainEvent ):
            return False
    if mod == SAHPI_ET_SENSOR:
        if not hasattr( x, "SensorEvent" ):
            return False
    if mod == SAHPI_ET_SENSOR:
        if not checkSaHpiSensorEventT( x.SensorEvent ):
            return False
    if mod == SAHPI_ET_SENSOR_ENABLE_CHANGE:
        if not hasattr( x, "SensorEnableChangeEvent" ):
            return False
    if mod == SAHPI_ET_SENSOR_ENABLE_CHANGE:
        if not checkSaHpiSensorEnableChangeEventT( x.SensorEnableChangeEvent ):
            return False
    if mod == SAHPI_ET_HOTSWAP:
        if not hasattr( x, "HotSwapEvent" ):
            return False
    if mod == SAHPI_ET_HOTSWAP:
        if not checkSaHpiHotSwapEventT( x.HotSwapEvent ):
            return False
    if mod == SAHPI_ET_WATCHDOG:
        if not hasattr( x, "WatchdogEvent" ):
            return False
    if mod == SAHPI_ET_WATCHDOG:
        if not checkSaHpiWatchdogEventT( x.WatchdogEvent ):
            return False
    if mod == SAHPI_ET_HPI_SW:
        if not hasattr( x, "HpiSwEvent" ):
            return False
    if mod == SAHPI_ET_HPI_SW:
        if not checkSaHpiHpiSwEventT( x.HpiSwEvent ):
            return False
    if mod == SAHPI_ET_OEM:
        if not hasattr( x, "OemEvent" ):
            return False
    if mod == SAHPI_ET_OEM:
        if not checkSaHpiOemEventT( x.OemEvent ):
            return False
    if mod == SAHPI_ET_USER:
        if not hasattr( x, "UserEvent" ):
            return False
    if mod == SAHPI_ET_USER:
        if not checkSaHpiUserEventT( x.UserEvent ):
            return False
    if mod == SAHPI_ET_DIMI:
        if not hasattr( x, "DimiEvent" ):
            return False
    if mod == SAHPI_ET_DIMI:
        if not checkSaHpiDimiEventT( x.DimiEvent ):
            return False
    if mod == SAHPI_ET_DIMI_UPDATE:
        if not hasattr( x, "DimiUpdateEvent" ):
            return False
    if mod == SAHPI_ET_DIMI_UPDATE:
        if not checkSaHpiDimiUpdateEventT( x.DimiUpdateEvent ):
            return False
    if mod == SAHPI_ET_FUMI:
        if not hasattr( x, "FumiEvent" ):
            return False
    if mod == SAHPI_ET_FUMI:
        if not checkSaHpiFumiEventT( x.FumiEvent ):
            return False
    return True

#**
# Check function for HPI struct SaHpiEventT
#**
def checkSaHpiEventT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEventT ):
        return False
    if not hasattr( x, "Source" ):
        return False
    if not hasattr( x, "EventType" ):
        return False
    if not hasattr( x, "Timestamp" ):
        return False
    if not hasattr( x, "Severity" ):
        return False
    if not hasattr( x, "EventDataUnion" ):
        return False
    if not checkSaHpiEventUnionT( x.EventDataUnion, x.EventType ):
        return False
    return True

#**
# Check function for HPI struct SaHpiNameT
#**
def checkSaHpiNameT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiNameT ):
        return False
    if not hasattr( x, "Length" ):
        return False
    if not hasattr( x, "Value" ):
        return False
    if not isinstance( x.Value, str ):
        return False
    if len( x.Value ) != SA_HPI_MAX_NAME_LENGTH:
        return False
    return True

#**
# Check function for HPI struct SaHpiConditionT
#**
def checkSaHpiConditionT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiConditionT ):
        return False
    if not hasattr( x, "Type" ):
        return False
    if not hasattr( x, "Entity" ):
        return False
    if not checkSaHpiEntityPathT( x.Entity ):
        return False
    if not hasattr( x, "DomainId" ):
        return False
    if not hasattr( x, "ResourceId" ):
        return False
    if not hasattr( x, "SensorNum" ):
        return False
    if not hasattr( x, "EventState" ):
        return False
    if not hasattr( x, "Name" ):
        return False
    if not checkSaHpiNameT( x.Name ):
        return False
    if not hasattr( x, "Mid" ):
        return False
    if not hasattr( x, "Data" ):
        return False
    if not checkSaHpiTextBufferT( x.Data ):
        return False
    return True

#**
# Check function for HPI struct SaHpiAnnouncementT
#**
def checkSaHpiAnnouncementT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiAnnouncementT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "Timestamp" ):
        return False
    if not hasattr( x, "AddedByUser" ):
        return False
    if not hasattr( x, "Severity" ):
        return False
    if not hasattr( x, "Acknowledged" ):
        return False
    if not hasattr( x, "StatusCond" ):
        return False
    if not checkSaHpiConditionT( x.StatusCond ):
        return False
    return True

#**
# Check function for HPI struct SaHpiAnnunciatorRecT
#**
def checkSaHpiAnnunciatorRecT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiAnnunciatorRecT ):
        return False
    if not hasattr( x, "AnnunciatorNum" ):
        return False
    if not hasattr( x, "AnnunciatorType" ):
        return False
    if not hasattr( x, "ModeReadOnly" ):
        return False
    if not hasattr( x, "MaxConditions" ):
        return False
    if not hasattr( x, "Oem" ):
        return False
    return True

#**
# Check function for HPI union SaHpiRdrTypeUnionT
#**
def checkSaHpiRdrTypeUnionT( x, mod ):
    if x is None:
        return False
    if not isinstance( x, SaHpiRdrTypeUnionT ):
        return False
    if mod == SAHPI_NO_RECORD:
        if not hasattr( x, "None" ):
            return False
    if mod == SAHPI_CTRL_RDR:
        if not hasattr( x, "CtrlRec" ):
            return False
    if mod == SAHPI_CTRL_RDR:
        if not checkSaHpiCtrlRecT( x.CtrlRec ):
            return False
    if mod == SAHPI_SENSOR_RDR:
        if not hasattr( x, "SensorRec" ):
            return False
    if mod == SAHPI_SENSOR_RDR:
        if not checkSaHpiSensorRecT( x.SensorRec ):
            return False
    if mod == SAHPI_INVENTORY_RDR:
        if not hasattr( x, "InventoryRec" ):
            return False
    if mod == SAHPI_INVENTORY_RDR:
        if not checkSaHpiInventoryRecT( x.InventoryRec ):
            return False
    if mod == SAHPI_WATCHDOG_RDR:
        if not hasattr( x, "WatchdogRec" ):
            return False
    if mod == SAHPI_WATCHDOG_RDR:
        if not checkSaHpiWatchdogRecT( x.WatchdogRec ):
            return False
    if mod == SAHPI_ANNUNCIATOR_RDR:
        if not hasattr( x, "AnnunciatorRec" ):
            return False
    if mod == SAHPI_ANNUNCIATOR_RDR:
        if not checkSaHpiAnnunciatorRecT( x.AnnunciatorRec ):
            return False
    if mod == SAHPI_DIMI_RDR:
        if not hasattr( x, "DimiRec" ):
            return False
    if mod == SAHPI_DIMI_RDR:
        if not checkSaHpiDimiRecT( x.DimiRec ):
            return False
    if mod == SAHPI_FUMI_RDR:
        if not hasattr( x, "FumiRec" ):
            return False
    if mod == SAHPI_FUMI_RDR:
        if not checkSaHpiFumiRecT( x.FumiRec ):
            return False
    return True

#**
# Check function for HPI struct SaHpiRdrT
#**
def checkSaHpiRdrT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiRdrT ):
        return False
    if not hasattr( x, "RecordId" ):
        return False
    if not hasattr( x, "RdrType" ):
        return False
    if not hasattr( x, "Entity" ):
        return False
    if not checkSaHpiEntityPathT( x.Entity ):
        return False
    if not hasattr( x, "IsFru" ):
        return False
    if not hasattr( x, "RdrTypeUnion" ):
        return False
    if not checkSaHpiRdrTypeUnionT( x.RdrTypeUnion, x.RdrType ):
        return False
    if not hasattr( x, "IdString" ):
        return False
    if not checkSaHpiTextBufferT( x.IdString ):
        return False
    return True

#**
# Check function for HPI struct SaHpiLoadIdT
#**
def checkSaHpiLoadIdT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiLoadIdT ):
        return False
    if not hasattr( x, "LoadNumber" ):
        return False
    if not hasattr( x, "LoadName" ):
        return False
    if not checkSaHpiTextBufferT( x.LoadName ):
        return False
    return True

#**
# Check function for HPI struct SaHpiResourceInfoT
#**
def checkSaHpiResourceInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiResourceInfoT ):
        return False
    if not hasattr( x, "ResourceRev" ):
        return False
    if not hasattr( x, "SpecificVer" ):
        return False
    if not hasattr( x, "DeviceSupport" ):
        return False
    if not hasattr( x, "ManufacturerId" ):
        return False
    if not hasattr( x, "ProductId" ):
        return False
    if not hasattr( x, "FirmwareMajorRev" ):
        return False
    if not hasattr( x, "FirmwareMinorRev" ):
        return False
    if not hasattr( x, "AuxFirmwareRev" ):
        return False
    if not hasattr( x, "Guid" ):
        return False
    if not isinstance( x.Guid, str ):
        return False
    if len( x.Guid ) != SAHPI_GUID_LENGTH:
        return False
    return True

#**
# Check function for HPI struct SaHpiRptEntryT
#**
def checkSaHpiRptEntryT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiRptEntryT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "ResourceId" ):
        return False
    if not hasattr( x, "ResourceInfo" ):
        return False
    if not checkSaHpiResourceInfoT( x.ResourceInfo ):
        return False
    if not hasattr( x, "ResourceEntity" ):
        return False
    if not checkSaHpiEntityPathT( x.ResourceEntity ):
        return False
    if not hasattr( x, "ResourceCapabilities" ):
        return False
    if not hasattr( x, "HotSwapCapabilities" ):
        return False
    if not hasattr( x, "ResourceSeverity" ):
        return False
    if not hasattr( x, "ResourceFailed" ):
        return False
    if not hasattr( x, "ResourceTag" ):
        return False
    if not checkSaHpiTextBufferT( x.ResourceTag ):
        return False
    return True

#**
# Check function for HPI struct SaHpiDomainInfoT
#**
def checkSaHpiDomainInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDomainInfoT ):
        return False
    if not hasattr( x, "DomainId" ):
        return False
    if not hasattr( x, "DomainCapabilities" ):
        return False
    if not hasattr( x, "IsPeer" ):
        return False
    if not hasattr( x, "DomainTag" ):
        return False
    if not checkSaHpiTextBufferT( x.DomainTag ):
        return False
    if not hasattr( x, "DrtUpdateCount" ):
        return False
    if not hasattr( x, "DrtUpdateTimestamp" ):
        return False
    if not hasattr( x, "RptUpdateCount" ):
        return False
    if not hasattr( x, "RptUpdateTimestamp" ):
        return False
    if not hasattr( x, "DatUpdateCount" ):
        return False
    if not hasattr( x, "DatUpdateTimestamp" ):
        return False
    if not hasattr( x, "ActiveAlarms" ):
        return False
    if not hasattr( x, "CriticalAlarms" ):
        return False
    if not hasattr( x, "MajorAlarms" ):
        return False
    if not hasattr( x, "MinorAlarms" ):
        return False
    if not hasattr( x, "DatUserAlarmLimit" ):
        return False
    if not hasattr( x, "DatOverflow" ):
        return False
    if not hasattr( x, "Guid" ):
        return False
    if not isinstance( x.Guid, str ):
        return False
    if len( x.Guid ) != SAHPI_GUID_LENGTH:
        return False
    return True

#**
# Check function for HPI struct SaHpiDrtEntryT
#**
def checkSaHpiDrtEntryT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiDrtEntryT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "DomainId" ):
        return False
    if not hasattr( x, "IsPeer" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiAlarmT
#**
def checkSaHpiAlarmT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiAlarmT ):
        return False
    if not hasattr( x, "AlarmId" ):
        return False
    if not hasattr( x, "Timestamp" ):
        return False
    if not hasattr( x, "Severity" ):
        return False
    if not hasattr( x, "Acknowledged" ):
        return False
    if not hasattr( x, "AlarmCond" ):
        return False
    if not checkSaHpiConditionT( x.AlarmCond ):
        return False
    return True

#**
# Check function for HPI struct SaHpiEventLogInfoT
#**
def checkSaHpiEventLogInfoT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEventLogInfoT ):
        return False
    if not hasattr( x, "Entries" ):
        return False
    if not hasattr( x, "Size" ):
        return False
    if not hasattr( x, "UserEventMaxSize" ):
        return False
    if not hasattr( x, "UpdateTimestamp" ):
        return False
    if not hasattr( x, "CurrentTime" ):
        return False
    if not hasattr( x, "Enabled" ):
        return False
    if not hasattr( x, "OverflowFlag" ):
        return False
    if not hasattr( x, "OverflowResetable" ):
        return False
    if not hasattr( x, "OverflowAction" ):
        return False
    return True

#**
# Check function for HPI struct SaHpiEventLogEntryT
#**
def checkSaHpiEventLogEntryT( x ):
    if x is None:
        return False
    if not isinstance( x, SaHpiEventLogEntryT ):
        return False
    if not hasattr( x, "EntryId" ):
        return False
    if not hasattr( x, "Timestamp" ):
        return False
    if not hasattr( x, "Event" ):
        return False
    if not checkSaHpiEventT( x.Event ):
        return False
    return True


#**********************************************************
# Lookups for enums, errors, event categories...
# Value -> Name (fromXXX)
# Name -> Value (toXXX)
# NB: toXXX raises FormatException if lookup fails
#**********************************************************

#**
# For SaHpiLanguageT
#**
def fromSaHpiLanguageT( x ):
    if x == SAHPI_LANG_UNDEF:
        return "UNDEF"
    if x == SAHPI_LANG_AFAR:
        return "AFAR"
    if x == SAHPI_LANG_ABKHAZIAN:
        return "ABKHAZIAN"
    if x == SAHPI_LANG_AFRIKAANS:
        return "AFRIKAANS"
    if x == SAHPI_LANG_AMHARIC:
        return "AMHARIC"
    if x == SAHPI_LANG_ARABIC:
        return "ARABIC"
    if x == SAHPI_LANG_ASSAMESE:
        return "ASSAMESE"
    if x == SAHPI_LANG_AYMARA:
        return "AYMARA"
    if x == SAHPI_LANG_AZERBAIJANI:
        return "AZERBAIJANI"
    if x == SAHPI_LANG_BASHKIR:
        return "BASHKIR"
    if x == SAHPI_LANG_BYELORUSSIAN:
        return "BYELORUSSIAN"
    if x == SAHPI_LANG_BULGARIAN:
        return "BULGARIAN"
    if x == SAHPI_LANG_BIHARI:
        return "BIHARI"
    if x == SAHPI_LANG_BISLAMA:
        return "BISLAMA"
    if x == SAHPI_LANG_BENGALI:
        return "BENGALI"
    if x == SAHPI_LANG_TIBETAN:
        return "TIBETAN"
    if x == SAHPI_LANG_BRETON:
        return "BRETON"
    if x == SAHPI_LANG_CATALAN:
        return "CATALAN"
    if x == SAHPI_LANG_CORSICAN:
        return "CORSICAN"
    if x == SAHPI_LANG_CZECH:
        return "CZECH"
    if x == SAHPI_LANG_WELSH:
        return "WELSH"
    if x == SAHPI_LANG_DANISH:
        return "DANISH"
    if x == SAHPI_LANG_GERMAN:
        return "GERMAN"
    if x == SAHPI_LANG_BHUTANI:
        return "BHUTANI"
    if x == SAHPI_LANG_GREEK:
        return "GREEK"
    if x == SAHPI_LANG_ENGLISH:
        return "ENGLISH"
    if x == SAHPI_LANG_ESPERANTO:
        return "ESPERANTO"
    if x == SAHPI_LANG_SPANISH:
        return "SPANISH"
    if x == SAHPI_LANG_ESTONIAN:
        return "ESTONIAN"
    if x == SAHPI_LANG_BASQUE:
        return "BASQUE"
    if x == SAHPI_LANG_PERSIAN:
        return "PERSIAN"
    if x == SAHPI_LANG_FINNISH:
        return "FINNISH"
    if x == SAHPI_LANG_FIJI:
        return "FIJI"
    if x == SAHPI_LANG_FAEROESE:
        return "FAEROESE"
    if x == SAHPI_LANG_FRENCH:
        return "FRENCH"
    if x == SAHPI_LANG_FRISIAN:
        return "FRISIAN"
    if x == SAHPI_LANG_IRISH:
        return "IRISH"
    if x == SAHPI_LANG_SCOTSGAELIC:
        return "SCOTSGAELIC"
    if x == SAHPI_LANG_GALICIAN:
        return "GALICIAN"
    if x == SAHPI_LANG_GUARANI:
        return "GUARANI"
    if x == SAHPI_LANG_GUJARATI:
        return "GUJARATI"
    if x == SAHPI_LANG_HAUSA:
        return "HAUSA"
    if x == SAHPI_LANG_HINDI:
        return "HINDI"
    if x == SAHPI_LANG_CROATIAN:
        return "CROATIAN"
    if x == SAHPI_LANG_HUNGARIAN:
        return "HUNGARIAN"
    if x == SAHPI_LANG_ARMENIAN:
        return "ARMENIAN"
    if x == SAHPI_LANG_INTERLINGUA:
        return "INTERLINGUA"
    if x == SAHPI_LANG_INTERLINGUE:
        return "INTERLINGUE"
    if x == SAHPI_LANG_INUPIAK:
        return "INUPIAK"
    if x == SAHPI_LANG_INDONESIAN:
        return "INDONESIAN"
    if x == SAHPI_LANG_ICELANDIC:
        return "ICELANDIC"
    if x == SAHPI_LANG_ITALIAN:
        return "ITALIAN"
    if x == SAHPI_LANG_HEBREW:
        return "HEBREW"
    if x == SAHPI_LANG_JAPANESE:
        return "JAPANESE"
    if x == SAHPI_LANG_YIDDISH:
        return "YIDDISH"
    if x == SAHPI_LANG_JAVANESE:
        return "JAVANESE"
    if x == SAHPI_LANG_GEORGIAN:
        return "GEORGIAN"
    if x == SAHPI_LANG_KAZAKH:
        return "KAZAKH"
    if x == SAHPI_LANG_GREENLANDIC:
        return "GREENLANDIC"
    if x == SAHPI_LANG_CAMBODIAN:
        return "CAMBODIAN"
    if x == SAHPI_LANG_KANNADA:
        return "KANNADA"
    if x == SAHPI_LANG_KOREAN:
        return "KOREAN"
    if x == SAHPI_LANG_KASHMIRI:
        return "KASHMIRI"
    if x == SAHPI_LANG_KURDISH:
        return "KURDISH"
    if x == SAHPI_LANG_KIRGHIZ:
        return "KIRGHIZ"
    if x == SAHPI_LANG_LATIN:
        return "LATIN"
    if x == SAHPI_LANG_LINGALA:
        return "LINGALA"
    if x == SAHPI_LANG_LAOTHIAN:
        return "LAOTHIAN"
    if x == SAHPI_LANG_LITHUANIAN:
        return "LITHUANIAN"
    if x == SAHPI_LANG_LATVIANLETTISH:
        return "LATVIANLETTISH"
    if x == SAHPI_LANG_MALAGASY:
        return "MALAGASY"
    if x == SAHPI_LANG_MAORI:
        return "MAORI"
    if x == SAHPI_LANG_MACEDONIAN:
        return "MACEDONIAN"
    if x == SAHPI_LANG_MALAYALAM:
        return "MALAYALAM"
    if x == SAHPI_LANG_MONGOLIAN:
        return "MONGOLIAN"
    if x == SAHPI_LANG_MOLDAVIAN:
        return "MOLDAVIAN"
    if x == SAHPI_LANG_MARATHI:
        return "MARATHI"
    if x == SAHPI_LANG_MALAY:
        return "MALAY"
    if x == SAHPI_LANG_MALTESE:
        return "MALTESE"
    if x == SAHPI_LANG_BURMESE:
        return "BURMESE"
    if x == SAHPI_LANG_NAURU:
        return "NAURU"
    if x == SAHPI_LANG_NEPALI:
        return "NEPALI"
    if x == SAHPI_LANG_DUTCH:
        return "DUTCH"
    if x == SAHPI_LANG_NORWEGIAN:
        return "NORWEGIAN"
    if x == SAHPI_LANG_OCCITAN:
        return "OCCITAN"
    if x == SAHPI_LANG_AFANOROMO:
        return "AFANOROMO"
    if x == SAHPI_LANG_ORIYA:
        return "ORIYA"
    if x == SAHPI_LANG_PUNJABI:
        return "PUNJABI"
    if x == SAHPI_LANG_POLISH:
        return "POLISH"
    if x == SAHPI_LANG_PASHTOPUSHTO:
        return "PASHTOPUSHTO"
    if x == SAHPI_LANG_PORTUGUESE:
        return "PORTUGUESE"
    if x == SAHPI_LANG_QUECHUA:
        return "QUECHUA"
    if x == SAHPI_LANG_RHAETOROMANCE:
        return "RHAETOROMANCE"
    if x == SAHPI_LANG_KIRUNDI:
        return "KIRUNDI"
    if x == SAHPI_LANG_ROMANIAN:
        return "ROMANIAN"
    if x == SAHPI_LANG_RUSSIAN:
        return "RUSSIAN"
    if x == SAHPI_LANG_KINYARWANDA:
        return "KINYARWANDA"
    if x == SAHPI_LANG_SANSKRIT:
        return "SANSKRIT"
    if x == SAHPI_LANG_SINDHI:
        return "SINDHI"
    if x == SAHPI_LANG_SANGRO:
        return "SANGRO"
    if x == SAHPI_LANG_SERBOCROATIAN:
        return "SERBOCROATIAN"
    if x == SAHPI_LANG_SINGHALESE:
        return "SINGHALESE"
    if x == SAHPI_LANG_SLOVAK:
        return "SLOVAK"
    if x == SAHPI_LANG_SLOVENIAN:
        return "SLOVENIAN"
    if x == SAHPI_LANG_SAMOAN:
        return "SAMOAN"
    if x == SAHPI_LANG_SHONA:
        return "SHONA"
    if x == SAHPI_LANG_SOMALI:
        return "SOMALI"
    if x == SAHPI_LANG_ALBANIAN:
        return "ALBANIAN"
    if x == SAHPI_LANG_SERBIAN:
        return "SERBIAN"
    if x == SAHPI_LANG_SISWATI:
        return "SISWATI"
    if x == SAHPI_LANG_SESOTHO:
        return "SESOTHO"
    if x == SAHPI_LANG_SUDANESE:
        return "SUDANESE"
    if x == SAHPI_LANG_SWEDISH:
        return "SWEDISH"
    if x == SAHPI_LANG_SWAHILI:
        return "SWAHILI"
    if x == SAHPI_LANG_TAMIL:
        return "TAMIL"
    if x == SAHPI_LANG_TELUGU:
        return "TELUGU"
    if x == SAHPI_LANG_TAJIK:
        return "TAJIK"
    if x == SAHPI_LANG_THAI:
        return "THAI"
    if x == SAHPI_LANG_TIGRINYA:
        return "TIGRINYA"
    if x == SAHPI_LANG_TURKMEN:
        return "TURKMEN"
    if x == SAHPI_LANG_TAGALOG:
        return "TAGALOG"
    if x == SAHPI_LANG_SETSWANA:
        return "SETSWANA"
    if x == SAHPI_LANG_TONGA:
        return "TONGA"
    if x == SAHPI_LANG_TURKISH:
        return "TURKISH"
    if x == SAHPI_LANG_TSONGA:
        return "TSONGA"
    if x == SAHPI_LANG_TATAR:
        return "TATAR"
    if x == SAHPI_LANG_TWI:
        return "TWI"
    if x == SAHPI_LANG_UKRAINIAN:
        return "UKRAINIAN"
    if x == SAHPI_LANG_URDU:
        return "URDU"
    if x == SAHPI_LANG_UZBEK:
        return "UZBEK"
    if x == SAHPI_LANG_VIETNAMESE:
        return "VIETNAMESE"
    if x == SAHPI_LANG_VOLAPUK:
        return "VOLAPUK"
    if x == SAHPI_LANG_WOLOF:
        return "WOLOF"
    if x == SAHPI_LANG_XHOSA:
        return "XHOSA"
    if x == SAHPI_LANG_YORUBA:
        return "YORUBA"
    if x == SAHPI_LANG_CHINESE:
        return "CHINESE"
    if x == SAHPI_LANG_ZULU:
        return "ZULU"
    return repr( x )

def toSaHpiLanguageT( s ):
    if s == "UNDEF":
        return SAHPI_LANG_UNDEF
    if s == "AFAR":
        return SAHPI_LANG_AFAR
    if s == "ABKHAZIAN":
        return SAHPI_LANG_ABKHAZIAN
    if s == "AFRIKAANS":
        return SAHPI_LANG_AFRIKAANS
    if s == "AMHARIC":
        return SAHPI_LANG_AMHARIC
    if s == "ARABIC":
        return SAHPI_LANG_ARABIC
    if s == "ASSAMESE":
        return SAHPI_LANG_ASSAMESE
    if s == "AYMARA":
        return SAHPI_LANG_AYMARA
    if s == "AZERBAIJANI":
        return SAHPI_LANG_AZERBAIJANI
    if s == "BASHKIR":
        return SAHPI_LANG_BASHKIR
    if s == "BYELORUSSIAN":
        return SAHPI_LANG_BYELORUSSIAN
    if s == "BULGARIAN":
        return SAHPI_LANG_BULGARIAN
    if s == "BIHARI":
        return SAHPI_LANG_BIHARI
    if s == "BISLAMA":
        return SAHPI_LANG_BISLAMA
    if s == "BENGALI":
        return SAHPI_LANG_BENGALI
    if s == "TIBETAN":
        return SAHPI_LANG_TIBETAN
    if s == "BRETON":
        return SAHPI_LANG_BRETON
    if s == "CATALAN":
        return SAHPI_LANG_CATALAN
    if s == "CORSICAN":
        return SAHPI_LANG_CORSICAN
    if s == "CZECH":
        return SAHPI_LANG_CZECH
    if s == "WELSH":
        return SAHPI_LANG_WELSH
    if s == "DANISH":
        return SAHPI_LANG_DANISH
    if s == "GERMAN":
        return SAHPI_LANG_GERMAN
    if s == "BHUTANI":
        return SAHPI_LANG_BHUTANI
    if s == "GREEK":
        return SAHPI_LANG_GREEK
    if s == "ENGLISH":
        return SAHPI_LANG_ENGLISH
    if s == "ESPERANTO":
        return SAHPI_LANG_ESPERANTO
    if s == "SPANISH":
        return SAHPI_LANG_SPANISH
    if s == "ESTONIAN":
        return SAHPI_LANG_ESTONIAN
    if s == "BASQUE":
        return SAHPI_LANG_BASQUE
    if s == "PERSIAN":
        return SAHPI_LANG_PERSIAN
    if s == "FINNISH":
        return SAHPI_LANG_FINNISH
    if s == "FIJI":
        return SAHPI_LANG_FIJI
    if s == "FAEROESE":
        return SAHPI_LANG_FAEROESE
    if s == "FRENCH":
        return SAHPI_LANG_FRENCH
    if s == "FRISIAN":
        return SAHPI_LANG_FRISIAN
    if s == "IRISH":
        return SAHPI_LANG_IRISH
    if s == "SCOTSGAELIC":
        return SAHPI_LANG_SCOTSGAELIC
    if s == "GALICIAN":
        return SAHPI_LANG_GALICIAN
    if s == "GUARANI":
        return SAHPI_LANG_GUARANI
    if s == "GUJARATI":
        return SAHPI_LANG_GUJARATI
    if s == "HAUSA":
        return SAHPI_LANG_HAUSA
    if s == "HINDI":
        return SAHPI_LANG_HINDI
    if s == "CROATIAN":
        return SAHPI_LANG_CROATIAN
    if s == "HUNGARIAN":
        return SAHPI_LANG_HUNGARIAN
    if s == "ARMENIAN":
        return SAHPI_LANG_ARMENIAN
    if s == "INTERLINGUA":
        return SAHPI_LANG_INTERLINGUA
    if s == "INTERLINGUE":
        return SAHPI_LANG_INTERLINGUE
    if s == "INUPIAK":
        return SAHPI_LANG_INUPIAK
    if s == "INDONESIAN":
        return SAHPI_LANG_INDONESIAN
    if s == "ICELANDIC":
        return SAHPI_LANG_ICELANDIC
    if s == "ITALIAN":
        return SAHPI_LANG_ITALIAN
    if s == "HEBREW":
        return SAHPI_LANG_HEBREW
    if s == "JAPANESE":
        return SAHPI_LANG_JAPANESE
    if s == "YIDDISH":
        return SAHPI_LANG_YIDDISH
    if s == "JAVANESE":
        return SAHPI_LANG_JAVANESE
    if s == "GEORGIAN":
        return SAHPI_LANG_GEORGIAN
    if s == "KAZAKH":
        return SAHPI_LANG_KAZAKH
    if s == "GREENLANDIC":
        return SAHPI_LANG_GREENLANDIC
    if s == "CAMBODIAN":
        return SAHPI_LANG_CAMBODIAN
    if s == "KANNADA":
        return SAHPI_LANG_KANNADA
    if s == "KOREAN":
        return SAHPI_LANG_KOREAN
    if s == "KASHMIRI":
        return SAHPI_LANG_KASHMIRI
    if s == "KURDISH":
        return SAHPI_LANG_KURDISH
    if s == "KIRGHIZ":
        return SAHPI_LANG_KIRGHIZ
    if s == "LATIN":
        return SAHPI_LANG_LATIN
    if s == "LINGALA":
        return SAHPI_LANG_LINGALA
    if s == "LAOTHIAN":
        return SAHPI_LANG_LAOTHIAN
    if s == "LITHUANIAN":
        return SAHPI_LANG_LITHUANIAN
    if s == "LATVIANLETTISH":
        return SAHPI_LANG_LATVIANLETTISH
    if s == "MALAGASY":
        return SAHPI_LANG_MALAGASY
    if s == "MAORI":
        return SAHPI_LANG_MAORI
    if s == "MACEDONIAN":
        return SAHPI_LANG_MACEDONIAN
    if s == "MALAYALAM":
        return SAHPI_LANG_MALAYALAM
    if s == "MONGOLIAN":
        return SAHPI_LANG_MONGOLIAN
    if s == "MOLDAVIAN":
        return SAHPI_LANG_MOLDAVIAN
    if s == "MARATHI":
        return SAHPI_LANG_MARATHI
    if s == "MALAY":
        return SAHPI_LANG_MALAY
    if s == "MALTESE":
        return SAHPI_LANG_MALTESE
    if s == "BURMESE":
        return SAHPI_LANG_BURMESE
    if s == "NAURU":
        return SAHPI_LANG_NAURU
    if s == "NEPALI":
        return SAHPI_LANG_NEPALI
    if s == "DUTCH":
        return SAHPI_LANG_DUTCH
    if s == "NORWEGIAN":
        return SAHPI_LANG_NORWEGIAN
    if s == "OCCITAN":
        return SAHPI_LANG_OCCITAN
    if s == "AFANOROMO":
        return SAHPI_LANG_AFANOROMO
    if s == "ORIYA":
        return SAHPI_LANG_ORIYA
    if s == "PUNJABI":
        return SAHPI_LANG_PUNJABI
    if s == "POLISH":
        return SAHPI_LANG_POLISH
    if s == "PASHTOPUSHTO":
        return SAHPI_LANG_PASHTOPUSHTO
    if s == "PORTUGUESE":
        return SAHPI_LANG_PORTUGUESE
    if s == "QUECHUA":
        return SAHPI_LANG_QUECHUA
    if s == "RHAETOROMANCE":
        return SAHPI_LANG_RHAETOROMANCE
    if s == "KIRUNDI":
        return SAHPI_LANG_KIRUNDI
    if s == "ROMANIAN":
        return SAHPI_LANG_ROMANIAN
    if s == "RUSSIAN":
        return SAHPI_LANG_RUSSIAN
    if s == "KINYARWANDA":
        return SAHPI_LANG_KINYARWANDA
    if s == "SANSKRIT":
        return SAHPI_LANG_SANSKRIT
    if s == "SINDHI":
        return SAHPI_LANG_SINDHI
    if s == "SANGRO":
        return SAHPI_LANG_SANGRO
    if s == "SERBOCROATIAN":
        return SAHPI_LANG_SERBOCROATIAN
    if s == "SINGHALESE":
        return SAHPI_LANG_SINGHALESE
    if s == "SLOVAK":
        return SAHPI_LANG_SLOVAK
    if s == "SLOVENIAN":
        return SAHPI_LANG_SLOVENIAN
    if s == "SAMOAN":
        return SAHPI_LANG_SAMOAN
    if s == "SHONA":
        return SAHPI_LANG_SHONA
    if s == "SOMALI":
        return SAHPI_LANG_SOMALI
    if s == "ALBANIAN":
        return SAHPI_LANG_ALBANIAN
    if s == "SERBIAN":
        return SAHPI_LANG_SERBIAN
    if s == "SISWATI":
        return SAHPI_LANG_SISWATI
    if s == "SESOTHO":
        return SAHPI_LANG_SESOTHO
    if s == "SUDANESE":
        return SAHPI_LANG_SUDANESE
    if s == "SWEDISH":
        return SAHPI_LANG_SWEDISH
    if s == "SWAHILI":
        return SAHPI_LANG_SWAHILI
    if s == "TAMIL":
        return SAHPI_LANG_TAMIL
    if s == "TELUGU":
        return SAHPI_LANG_TELUGU
    if s == "TAJIK":
        return SAHPI_LANG_TAJIK
    if s == "THAI":
        return SAHPI_LANG_THAI
    if s == "TIGRINYA":
        return SAHPI_LANG_TIGRINYA
    if s == "TURKMEN":
        return SAHPI_LANG_TURKMEN
    if s == "TAGALOG":
        return SAHPI_LANG_TAGALOG
    if s == "SETSWANA":
        return SAHPI_LANG_SETSWANA
    if s == "TONGA":
        return SAHPI_LANG_TONGA
    if s == "TURKISH":
        return SAHPI_LANG_TURKISH
    if s == "TSONGA":
        return SAHPI_LANG_TSONGA
    if s == "TATAR":
        return SAHPI_LANG_TATAR
    if s == "TWI":
        return SAHPI_LANG_TWI
    if s == "UKRAINIAN":
        return SAHPI_LANG_UKRAINIAN
    if s == "URDU":
        return SAHPI_LANG_URDU
    if s == "UZBEK":
        return SAHPI_LANG_UZBEK
    if s == "VIETNAMESE":
        return SAHPI_LANG_VIETNAMESE
    if s == "VOLAPUK":
        return SAHPI_LANG_VOLAPUK
    if s == "WOLOF":
        return SAHPI_LANG_WOLOF
    if s == "XHOSA":
        return SAHPI_LANG_XHOSA
    if s == "YORUBA":
        return SAHPI_LANG_YORUBA
    if s == "CHINESE":
        return SAHPI_LANG_CHINESE
    if s == "ZULU":
        return SAHPI_LANG_ZULU
    raise ValueError()

#**
# For SaHpiTextTypeT
#**
def fromSaHpiTextTypeT( x ):
    if x == SAHPI_TL_TYPE_UNICODE:
        return "UNICODE"
    if x == SAHPI_TL_TYPE_BCDPLUS:
        return "BCDPLUS"
    if x == SAHPI_TL_TYPE_ASCII6:
        return "ASCII6"
    if x == SAHPI_TL_TYPE_TEXT:
        return "TEXT"
    if x == SAHPI_TL_TYPE_BINARY:
        return "BINARY"
    return repr( x )

def toSaHpiTextTypeT( s ):
    if s == "UNICODE":
        return SAHPI_TL_TYPE_UNICODE
    if s == "BCDPLUS":
        return SAHPI_TL_TYPE_BCDPLUS
    if s == "ASCII6":
        return SAHPI_TL_TYPE_ASCII6
    if s == "TEXT":
        return SAHPI_TL_TYPE_TEXT
    if s == "BINARY":
        return SAHPI_TL_TYPE_BINARY
    raise ValueError()

#**
# For SaHpiEntityTypeT
#**
def fromSaHpiEntityTypeT( x ):
    if x == SAHPI_ENT_UNSPECIFIED:
        return "UNSPECIFIED"
    if x == SAHPI_ENT_OTHER:
        return "OTHER"
    if x == SAHPI_ENT_UNKNOWN:
        return "UNKNOWN"
    if x == SAHPI_ENT_PROCESSOR:
        return "PROCESSOR"
    if x == SAHPI_ENT_DISK_BAY:
        return "DISK_BAY"
    if x == SAHPI_ENT_PERIPHERAL_BAY:
        return "PERIPHERAL_BAY"
    if x == SAHPI_ENT_SYS_MGMNT_MODULE:
        return "SYS_MGMNT_MODULE"
    if x == SAHPI_ENT_SYSTEM_BOARD:
        return "SYSTEM_BOARD"
    if x == SAHPI_ENT_MEMORY_MODULE:
        return "MEMORY_MODULE"
    if x == SAHPI_ENT_PROCESSOR_MODULE:
        return "PROCESSOR_MODULE"
    if x == SAHPI_ENT_POWER_SUPPLY:
        return "POWER_SUPPLY"
    if x == SAHPI_ENT_ADD_IN_CARD:
        return "ADD_IN_CARD"
    if x == SAHPI_ENT_FRONT_PANEL_BOARD:
        return "FRONT_PANEL_BOARD"
    if x == SAHPI_ENT_BACK_PANEL_BOARD:
        return "BACK_PANEL_BOARD"
    if x == SAHPI_ENT_POWER_SYSTEM_BOARD:
        return "POWER_SYSTEM_BOARD"
    if x == SAHPI_ENT_DRIVE_BACKPLANE:
        return "DRIVE_BACKPLANE"
    if x == SAHPI_ENT_SYS_EXPANSION_BOARD:
        return "SYS_EXPANSION_BOARD"
    if x == SAHPI_ENT_OTHER_SYSTEM_BOARD:
        return "OTHER_SYSTEM_BOARD"
    if x == SAHPI_ENT_PROCESSOR_BOARD:
        return "PROCESSOR_BOARD"
    if x == SAHPI_ENT_POWER_UNIT:
        return "POWER_UNIT"
    if x == SAHPI_ENT_POWER_MODULE:
        return "POWER_MODULE"
    if x == SAHPI_ENT_POWER_MGMNT:
        return "POWER_MGMNT"
    if x == SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD:
        return "CHASSIS_BACK_PANEL_BOARD"
    if x == SAHPI_ENT_SYSTEM_CHASSIS:
        return "SYSTEM_CHASSIS"
    if x == SAHPI_ENT_SUB_CHASSIS:
        return "SUB_CHASSIS"
    if x == SAHPI_ENT_OTHER_CHASSIS_BOARD:
        return "OTHER_CHASSIS_BOARD"
    if x == SAHPI_ENT_DISK_DRIVE_BAY:
        return "DISK_DRIVE_BAY"
    if x == SAHPI_ENT_PERIPHERAL_BAY_2:
        return "PERIPHERAL_BAY_2"
    if x == SAHPI_ENT_DEVICE_BAY:
        return "DEVICE_BAY"
    if x == SAHPI_ENT_COOLING_DEVICE:
        return "COOLING_DEVICE"
    if x == SAHPI_ENT_COOLING_UNIT:
        return "COOLING_UNIT"
    if x == SAHPI_ENT_INTERCONNECT:
        return "INTERCONNECT"
    if x == SAHPI_ENT_MEMORY_DEVICE:
        return "MEMORY_DEVICE"
    if x == SAHPI_ENT_SYS_MGMNT_SOFTWARE:
        return "SYS_MGMNT_SOFTWARE"
    if x == SAHPI_ENT_BIOS:
        return "BIOS"
    if x == SAHPI_ENT_OPERATING_SYSTEM:
        return "OPERATING_SYSTEM"
    if x == SAHPI_ENT_SYSTEM_BUS:
        return "SYSTEM_BUS"
    if x == SAHPI_ENT_GROUP:
        return "GROUP"
    if x == SAHPI_ENT_REMOTE:
        return "REMOTE"
    if x == SAHPI_ENT_EXTERNAL_ENVIRONMENT:
        return "EXTERNAL_ENVIRONMENT"
    if x == SAHPI_ENT_BATTERY:
        return "BATTERY"
    if x == SAHPI_ENT_PROCESSING_BLADE:
        return "PROCESSING_BLADE"
    if x == SAHPI_ENT_CONNECTIVITY_SWITCH:
        return "CONNECTIVITY_SWITCH"
    if x == SAHPI_ENT_PROCESSOR_MEMORY_MODULE:
        return "PROCESSOR_MEMORY_MODULE"
    if x == SAHPI_ENT_IO_MODULE:
        return "IO_MODULE"
    if x == SAHPI_ENT_PROCESSOR_IO_MODULE:
        return "PROCESSOR_IO_MODULE"
    if x == SAHPI_ENT_MC_FIRMWARE:
        return "MC_FIRMWARE"
    if x == SAHPI_ENT_IPMI_CHANNEL:
        return "IPMI_CHANNEL"
    if x == SAHPI_ENT_PCI_BUS:
        return "PCI_BUS"
    if x == SAHPI_ENT_PCI_EXPRESS_BUS:
        return "PCI_EXPRESS_BUS"
    if x == SAHPI_ENT_SCSI_BUS:
        return "SCSI_BUS"
    if x == SAHPI_ENT_SATA_BUS:
        return "SATA_BUS"
    if x == SAHPI_ENT_PROC_FSB:
        return "PROC_FSB"
    if x == SAHPI_ENT_CLOCK:
        return "CLOCK"
    if x == SAHPI_ENT_SYSTEM_FIRMWARE:
        return "SYSTEM_FIRMWARE"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC:
        return "CHASSIS_SPECIFIC"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC01:
        return "CHASSIS_SPECIFIC01"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC02:
        return "CHASSIS_SPECIFIC02"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC03:
        return "CHASSIS_SPECIFIC03"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC04:
        return "CHASSIS_SPECIFIC04"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC05:
        return "CHASSIS_SPECIFIC05"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC06:
        return "CHASSIS_SPECIFIC06"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC07:
        return "CHASSIS_SPECIFIC07"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC08:
        return "CHASSIS_SPECIFIC08"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC09:
        return "CHASSIS_SPECIFIC09"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC10:
        return "CHASSIS_SPECIFIC10"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC11:
        return "CHASSIS_SPECIFIC11"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC12:
        return "CHASSIS_SPECIFIC12"
    if x == SAHPI_ENT_CHASSIS_SPECIFIC13:
        return "CHASSIS_SPECIFIC13"
    if x == SAHPI_ENT_BOARD_SET_SPECIFIC:
        return "BOARD_SET_SPECIFIC"
    if x == SAHPI_ENT_OEM_SYSINT_SPECIFIC:
        return "OEM_SYSINT_SPECIFIC"
    if x == SAHPI_ENT_ROOT:
        return "ROOT"
    if x == SAHPI_ENT_RACK:
        return "RACK"
    if x == SAHPI_ENT_SUBRACK:
        return "SUBRACK"
    if x == SAHPI_ENT_COMPACTPCI_CHASSIS:
        return "COMPACTPCI_CHASSIS"
    if x == SAHPI_ENT_ADVANCEDTCA_CHASSIS:
        return "ADVANCEDTCA_CHASSIS"
    if x == SAHPI_ENT_RACK_MOUNTED_SERVER:
        return "RACK_MOUNTED_SERVER"
    if x == SAHPI_ENT_SYSTEM_BLADE:
        return "SYSTEM_BLADE"
    if x == SAHPI_ENT_SWITCH:
        return "SWITCH"
    if x == SAHPI_ENT_SWITCH_BLADE:
        return "SWITCH_BLADE"
    if x == SAHPI_ENT_SBC_BLADE:
        return "SBC_BLADE"
    if x == SAHPI_ENT_IO_BLADE:
        return "IO_BLADE"
    if x == SAHPI_ENT_DISK_BLADE:
        return "DISK_BLADE"
    if x == SAHPI_ENT_DISK_DRIVE:
        return "DISK_DRIVE"
    if x == SAHPI_ENT_FAN:
        return "FAN"
    if x == SAHPI_ENT_POWER_DISTRIBUTION_UNIT:
        return "POWER_DISTRIBUTION_UNIT"
    if x == SAHPI_ENT_SPEC_PROC_BLADE:
        return "SPEC_PROC_BLADE"
    if x == SAHPI_ENT_IO_SUBBOARD:
        return "IO_SUBBOARD"
    if x == SAHPI_ENT_SBC_SUBBOARD:
        return "SBC_SUBBOARD"
    if x == SAHPI_ENT_ALARM_MANAGER:
        return "ALARM_MANAGER"
    if x == SAHPI_ENT_SHELF_MANAGER:
        return "SHELF_MANAGER"
    if x == SAHPI_ENT_DISPLAY_PANEL:
        return "DISPLAY_PANEL"
    if x == SAHPI_ENT_SUBBOARD_CARRIER_BLADE:
        return "SUBBOARD_CARRIER_BLADE"
    if x == SAHPI_ENT_PHYSICAL_SLOT:
        return "PHYSICAL_SLOT"
    if x == SAHPI_ENT_PICMG_FRONT_BLADE:
        return "PICMG_FRONT_BLADE"
    if x == SAHPI_ENT_SYSTEM_INVENTORY_DEVICE:
        return "SYSTEM_INVENTORY_DEVICE"
    if x == SAHPI_ENT_FILTRATION_UNIT:
        return "FILTRATION_UNIT"
    if x == SAHPI_ENT_AMC:
        return "AMC"
    if x == SAHPI_ENT_BMC:
        return "BMC"
    if x == SAHPI_ENT_IPMC:
        return "IPMC"
    if x == SAHPI_ENT_MMC:
        return "MMC"
    if x == SAHPI_ENT_SHMC:
        return "SHMC"
    if x == SAHPI_ENT_CPLD:
        return "CPLD"
    if x == SAHPI_ENT_EPLD:
        return "EPLD"
    if x == SAHPI_ENT_FPGA:
        return "FPGA"
    if x == SAHPI_ENT_DASD:
        return "DASD"
    if x == SAHPI_ENT_NIC:
        return "NIC"
    if x == SAHPI_ENT_DSP:
        return "DSP"
    if x == SAHPI_ENT_UCODE:
        return "UCODE"
    if x == SAHPI_ENT_NPU:
        return "NPU"
    if x == SAHPI_ENT_OEM:
        return "OEM"
    if x == SAHPI_ENT_INTERFACE:
        return "INTERFACE"
    if x == SAHPI_ENT_MICROTCA_CHASSIS:
        return "MICROTCA_CHASSIS"
    if x == SAHPI_ENT_CARRIER:
        return "CARRIER"
    if x == SAHPI_ENT_CARRIER_MANAGER:
        return "CARRIER_MANAGER"
    if x == SAHPI_ENT_CONFIG_DATA:
        return "CONFIG_DATA"
    if x == SAHPI_ENT_INDICATOR:
        return "INDICATOR"
    return repr( x )

def toSaHpiEntityTypeT( s ):
    if s == "UNSPECIFIED":
        return SAHPI_ENT_UNSPECIFIED
    if s == "OTHER":
        return SAHPI_ENT_OTHER
    if s == "UNKNOWN":
        return SAHPI_ENT_UNKNOWN
    if s == "PROCESSOR":
        return SAHPI_ENT_PROCESSOR
    if s == "DISK_BAY":
        return SAHPI_ENT_DISK_BAY
    if s == "PERIPHERAL_BAY":
        return SAHPI_ENT_PERIPHERAL_BAY
    if s == "SYS_MGMNT_MODULE":
        return SAHPI_ENT_SYS_MGMNT_MODULE
    if s == "SYSTEM_BOARD":
        return SAHPI_ENT_SYSTEM_BOARD
    if s == "MEMORY_MODULE":
        return SAHPI_ENT_MEMORY_MODULE
    if s == "PROCESSOR_MODULE":
        return SAHPI_ENT_PROCESSOR_MODULE
    if s == "POWER_SUPPLY":
        return SAHPI_ENT_POWER_SUPPLY
    if s == "ADD_IN_CARD":
        return SAHPI_ENT_ADD_IN_CARD
    if s == "FRONT_PANEL_BOARD":
        return SAHPI_ENT_FRONT_PANEL_BOARD
    if s == "BACK_PANEL_BOARD":
        return SAHPI_ENT_BACK_PANEL_BOARD
    if s == "POWER_SYSTEM_BOARD":
        return SAHPI_ENT_POWER_SYSTEM_BOARD
    if s == "DRIVE_BACKPLANE":
        return SAHPI_ENT_DRIVE_BACKPLANE
    if s == "SYS_EXPANSION_BOARD":
        return SAHPI_ENT_SYS_EXPANSION_BOARD
    if s == "OTHER_SYSTEM_BOARD":
        return SAHPI_ENT_OTHER_SYSTEM_BOARD
    if s == "PROCESSOR_BOARD":
        return SAHPI_ENT_PROCESSOR_BOARD
    if s == "POWER_UNIT":
        return SAHPI_ENT_POWER_UNIT
    if s == "POWER_MODULE":
        return SAHPI_ENT_POWER_MODULE
    if s == "POWER_MGMNT":
        return SAHPI_ENT_POWER_MGMNT
    if s == "CHASSIS_BACK_PANEL_BOARD":
        return SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD
    if s == "SYSTEM_CHASSIS":
        return SAHPI_ENT_SYSTEM_CHASSIS
    if s == "SUB_CHASSIS":
        return SAHPI_ENT_SUB_CHASSIS
    if s == "OTHER_CHASSIS_BOARD":
        return SAHPI_ENT_OTHER_CHASSIS_BOARD
    if s == "DISK_DRIVE_BAY":
        return SAHPI_ENT_DISK_DRIVE_BAY
    if s == "PERIPHERAL_BAY_2":
        return SAHPI_ENT_PERIPHERAL_BAY_2
    if s == "DEVICE_BAY":
        return SAHPI_ENT_DEVICE_BAY
    if s == "COOLING_DEVICE":
        return SAHPI_ENT_COOLING_DEVICE
    if s == "COOLING_UNIT":
        return SAHPI_ENT_COOLING_UNIT
    if s == "INTERCONNECT":
        return SAHPI_ENT_INTERCONNECT
    if s == "MEMORY_DEVICE":
        return SAHPI_ENT_MEMORY_DEVICE
    if s == "SYS_MGMNT_SOFTWARE":
        return SAHPI_ENT_SYS_MGMNT_SOFTWARE
    if s == "BIOS":
        return SAHPI_ENT_BIOS
    if s == "OPERATING_SYSTEM":
        return SAHPI_ENT_OPERATING_SYSTEM
    if s == "SYSTEM_BUS":
        return SAHPI_ENT_SYSTEM_BUS
    if s == "GROUP":
        return SAHPI_ENT_GROUP
    if s == "REMOTE":
        return SAHPI_ENT_REMOTE
    if s == "EXTERNAL_ENVIRONMENT":
        return SAHPI_ENT_EXTERNAL_ENVIRONMENT
    if s == "BATTERY":
        return SAHPI_ENT_BATTERY
    if s == "PROCESSING_BLADE":
        return SAHPI_ENT_PROCESSING_BLADE
    if s == "CONNECTIVITY_SWITCH":
        return SAHPI_ENT_CONNECTIVITY_SWITCH
    if s == "PROCESSOR_MEMORY_MODULE":
        return SAHPI_ENT_PROCESSOR_MEMORY_MODULE
    if s == "IO_MODULE":
        return SAHPI_ENT_IO_MODULE
    if s == "PROCESSOR_IO_MODULE":
        return SAHPI_ENT_PROCESSOR_IO_MODULE
    if s == "MC_FIRMWARE":
        return SAHPI_ENT_MC_FIRMWARE
    if s == "IPMI_CHANNEL":
        return SAHPI_ENT_IPMI_CHANNEL
    if s == "PCI_BUS":
        return SAHPI_ENT_PCI_BUS
    if s == "PCI_EXPRESS_BUS":
        return SAHPI_ENT_PCI_EXPRESS_BUS
    if s == "SCSI_BUS":
        return SAHPI_ENT_SCSI_BUS
    if s == "SATA_BUS":
        return SAHPI_ENT_SATA_BUS
    if s == "PROC_FSB":
        return SAHPI_ENT_PROC_FSB
    if s == "CLOCK":
        return SAHPI_ENT_CLOCK
    if s == "SYSTEM_FIRMWARE":
        return SAHPI_ENT_SYSTEM_FIRMWARE
    if s == "CHASSIS_SPECIFIC":
        return SAHPI_ENT_CHASSIS_SPECIFIC
    if s == "CHASSIS_SPECIFIC01":
        return SAHPI_ENT_CHASSIS_SPECIFIC01
    if s == "CHASSIS_SPECIFIC02":
        return SAHPI_ENT_CHASSIS_SPECIFIC02
    if s == "CHASSIS_SPECIFIC03":
        return SAHPI_ENT_CHASSIS_SPECIFIC03
    if s == "CHASSIS_SPECIFIC04":
        return SAHPI_ENT_CHASSIS_SPECIFIC04
    if s == "CHASSIS_SPECIFIC05":
        return SAHPI_ENT_CHASSIS_SPECIFIC05
    if s == "CHASSIS_SPECIFIC06":
        return SAHPI_ENT_CHASSIS_SPECIFIC06
    if s == "CHASSIS_SPECIFIC07":
        return SAHPI_ENT_CHASSIS_SPECIFIC07
    if s == "CHASSIS_SPECIFIC08":
        return SAHPI_ENT_CHASSIS_SPECIFIC08
    if s == "CHASSIS_SPECIFIC09":
        return SAHPI_ENT_CHASSIS_SPECIFIC09
    if s == "CHASSIS_SPECIFIC10":
        return SAHPI_ENT_CHASSIS_SPECIFIC10
    if s == "CHASSIS_SPECIFIC11":
        return SAHPI_ENT_CHASSIS_SPECIFIC11
    if s == "CHASSIS_SPECIFIC12":
        return SAHPI_ENT_CHASSIS_SPECIFIC12
    if s == "CHASSIS_SPECIFIC13":
        return SAHPI_ENT_CHASSIS_SPECIFIC13
    if s == "BOARD_SET_SPECIFIC":
        return SAHPI_ENT_BOARD_SET_SPECIFIC
    if s == "OEM_SYSINT_SPECIFIC":
        return SAHPI_ENT_OEM_SYSINT_SPECIFIC
    if s == "ROOT":
        return SAHPI_ENT_ROOT
    if s == "RACK":
        return SAHPI_ENT_RACK
    if s == "SUBRACK":
        return SAHPI_ENT_SUBRACK
    if s == "COMPACTPCI_CHASSIS":
        return SAHPI_ENT_COMPACTPCI_CHASSIS
    if s == "ADVANCEDTCA_CHASSIS":
        return SAHPI_ENT_ADVANCEDTCA_CHASSIS
    if s == "RACK_MOUNTED_SERVER":
        return SAHPI_ENT_RACK_MOUNTED_SERVER
    if s == "SYSTEM_BLADE":
        return SAHPI_ENT_SYSTEM_BLADE
    if s == "SWITCH":
        return SAHPI_ENT_SWITCH
    if s == "SWITCH_BLADE":
        return SAHPI_ENT_SWITCH_BLADE
    if s == "SBC_BLADE":
        return SAHPI_ENT_SBC_BLADE
    if s == "IO_BLADE":
        return SAHPI_ENT_IO_BLADE
    if s == "DISK_BLADE":
        return SAHPI_ENT_DISK_BLADE
    if s == "DISK_DRIVE":
        return SAHPI_ENT_DISK_DRIVE
    if s == "FAN":
        return SAHPI_ENT_FAN
    if s == "POWER_DISTRIBUTION_UNIT":
        return SAHPI_ENT_POWER_DISTRIBUTION_UNIT
    if s == "SPEC_PROC_BLADE":
        return SAHPI_ENT_SPEC_PROC_BLADE
    if s == "IO_SUBBOARD":
        return SAHPI_ENT_IO_SUBBOARD
    if s == "SBC_SUBBOARD":
        return SAHPI_ENT_SBC_SUBBOARD
    if s == "ALARM_MANAGER":
        return SAHPI_ENT_ALARM_MANAGER
    if s == "SHELF_MANAGER":
        return SAHPI_ENT_SHELF_MANAGER
    if s == "DISPLAY_PANEL":
        return SAHPI_ENT_DISPLAY_PANEL
    if s == "SUBBOARD_CARRIER_BLADE":
        return SAHPI_ENT_SUBBOARD_CARRIER_BLADE
    if s == "PHYSICAL_SLOT":
        return SAHPI_ENT_PHYSICAL_SLOT
    if s == "PICMG_FRONT_BLADE":
        return SAHPI_ENT_PICMG_FRONT_BLADE
    if s == "SYSTEM_INVENTORY_DEVICE":
        return SAHPI_ENT_SYSTEM_INVENTORY_DEVICE
    if s == "FILTRATION_UNIT":
        return SAHPI_ENT_FILTRATION_UNIT
    if s == "AMC":
        return SAHPI_ENT_AMC
    if s == "BMC":
        return SAHPI_ENT_BMC
    if s == "IPMC":
        return SAHPI_ENT_IPMC
    if s == "MMC":
        return SAHPI_ENT_MMC
    if s == "SHMC":
        return SAHPI_ENT_SHMC
    if s == "CPLD":
        return SAHPI_ENT_CPLD
    if s == "EPLD":
        return SAHPI_ENT_EPLD
    if s == "FPGA":
        return SAHPI_ENT_FPGA
    if s == "DASD":
        return SAHPI_ENT_DASD
    if s == "NIC":
        return SAHPI_ENT_NIC
    if s == "DSP":
        return SAHPI_ENT_DSP
    if s == "UCODE":
        return SAHPI_ENT_UCODE
    if s == "NPU":
        return SAHPI_ENT_NPU
    if s == "OEM":
        return SAHPI_ENT_OEM
    if s == "INTERFACE":
        return SAHPI_ENT_INTERFACE
    if s == "MICROTCA_CHASSIS":
        return SAHPI_ENT_MICROTCA_CHASSIS
    if s == "CARRIER":
        return SAHPI_ENT_CARRIER
    if s == "CARRIER_MANAGER":
        return SAHPI_ENT_CARRIER_MANAGER
    if s == "CONFIG_DATA":
        return SAHPI_ENT_CONFIG_DATA
    if s == "INDICATOR":
        return SAHPI_ENT_INDICATOR
    raise ValueError()

#**
# For SaHpiSensorTypeT
#**
def fromSaHpiSensorTypeT( x ):
    if x == SAHPI_TEMPERATURE:
        return "TEMPERATURE"
    if x == SAHPI_VOLTAGE:
        return "VOLTAGE"
    if x == SAHPI_CURRENT:
        return "CURRENT"
    if x == SAHPI_FAN:
        return "FAN"
    if x == SAHPI_PHYSICAL_SECURITY:
        return "PHYSICAL_SECURITY"
    if x == SAHPI_PLATFORM_VIOLATION:
        return "PLATFORM_VIOLATION"
    if x == SAHPI_PROCESSOR:
        return "PROCESSOR"
    if x == SAHPI_POWER_SUPPLY:
        return "POWER_SUPPLY"
    if x == SAHPI_POWER_UNIT:
        return "POWER_UNIT"
    if x == SAHPI_COOLING_DEVICE:
        return "COOLING_DEVICE"
    if x == SAHPI_OTHER_UNITS_BASED_SENSOR:
        return "OTHER_UNITS_BASED_SENSOR"
    if x == SAHPI_MEMORY:
        return "MEMORY"
    if x == SAHPI_DRIVE_SLOT:
        return "DRIVE_SLOT"
    if x == SAHPI_POST_MEMORY_RESIZE:
        return "POST_MEMORY_RESIZE"
    if x == SAHPI_SYSTEM_FW_PROGRESS:
        return "SYSTEM_FW_PROGRESS"
    if x == SAHPI_EVENT_LOGGING_DISABLED:
        return "EVENT_LOGGING_DISABLED"
    if x == SAHPI_RESERVED1:
        return "RESERVED1"
    if x == SAHPI_SYSTEM_EVENT:
        return "SYSTEM_EVENT"
    if x == SAHPI_CRITICAL_INTERRUPT:
        return "CRITICAL_INTERRUPT"
    if x == SAHPI_BUTTON:
        return "BUTTON"
    if x == SAHPI_MODULE_BOARD:
        return "MODULE_BOARD"
    if x == SAHPI_MICROCONTROLLER_COPROCESSOR:
        return "MICROCONTROLLER_COPROCESSOR"
    if x == SAHPI_ADDIN_CARD:
        return "ADDIN_CARD"
    if x == SAHPI_CHASSIS:
        return "CHASSIS"
    if x == SAHPI_CHIP_SET:
        return "CHIP_SET"
    if x == SAHPI_OTHER_FRU:
        return "OTHER_FRU"
    if x == SAHPI_CABLE_INTERCONNECT:
        return "CABLE_INTERCONNECT"
    if x == SAHPI_TERMINATOR:
        return "TERMINATOR"
    if x == SAHPI_SYSTEM_BOOT_INITIATED:
        return "SYSTEM_BOOT_INITIATED"
    if x == SAHPI_BOOT_ERROR:
        return "BOOT_ERROR"
    if x == SAHPI_OS_BOOT:
        return "OS_BOOT"
    if x == SAHPI_OS_CRITICAL_STOP:
        return "OS_CRITICAL_STOP"
    if x == SAHPI_SLOT_CONNECTOR:
        return "SLOT_CONNECTOR"
    if x == SAHPI_SYSTEM_ACPI_POWER_STATE:
        return "SYSTEM_ACPI_POWER_STATE"
    if x == SAHPI_RESERVED2:
        return "RESERVED2"
    if x == SAHPI_PLATFORM_ALERT:
        return "PLATFORM_ALERT"
    if x == SAHPI_ENTITY_PRESENCE:
        return "ENTITY_PRESENCE"
    if x == SAHPI_MONITOR_ASIC_IC:
        return "MONITOR_ASIC_IC"
    if x == SAHPI_LAN:
        return "LAN"
    if x == SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH:
        return "MANAGEMENT_SUBSYSTEM_HEALTH"
    if x == SAHPI_BATTERY:
        return "BATTERY"
    if x == SAHPI_SESSION_AUDIT:
        return "SESSION_AUDIT"
    if x == SAHPI_VERSION_CHANGE:
        return "VERSION_CHANGE"
    if x == SAHPI_OPERATIONAL:
        return "OPERATIONAL"
    if x == SAHPI_OEM_SENSOR:
        return "OEM_SENSOR"
    if x == SAHPI_COMM_CHANNEL_LINK_STATE:
        return "COMM_CHANNEL_LINK_STATE"
    if x == SAHPI_MANAGEMENT_BUS_STATE:
        return "MANAGEMENT_BUS_STATE"
    if x == SAHPI_COMM_CHANNEL_BUS_STATE:
        return "COMM_CHANNEL_BUS_STATE"
    if x == SAHPI_CONFIG_DATA:
        return "CONFIG_DATA"
    if x == SAHPI_POWER_BUDGET:
        return "POWER_BUDGET"
    return repr( x )

def toSaHpiSensorTypeT( s ):
    if s == "TEMPERATURE":
        return SAHPI_TEMPERATURE
    if s == "VOLTAGE":
        return SAHPI_VOLTAGE
    if s == "CURRENT":
        return SAHPI_CURRENT
    if s == "FAN":
        return SAHPI_FAN
    if s == "PHYSICAL_SECURITY":
        return SAHPI_PHYSICAL_SECURITY
    if s == "PLATFORM_VIOLATION":
        return SAHPI_PLATFORM_VIOLATION
    if s == "PROCESSOR":
        return SAHPI_PROCESSOR
    if s == "POWER_SUPPLY":
        return SAHPI_POWER_SUPPLY
    if s == "POWER_UNIT":
        return SAHPI_POWER_UNIT
    if s == "COOLING_DEVICE":
        return SAHPI_COOLING_DEVICE
    if s == "OTHER_UNITS_BASED_SENSOR":
        return SAHPI_OTHER_UNITS_BASED_SENSOR
    if s == "MEMORY":
        return SAHPI_MEMORY
    if s == "DRIVE_SLOT":
        return SAHPI_DRIVE_SLOT
    if s == "POST_MEMORY_RESIZE":
        return SAHPI_POST_MEMORY_RESIZE
    if s == "SYSTEM_FW_PROGRESS":
        return SAHPI_SYSTEM_FW_PROGRESS
    if s == "EVENT_LOGGING_DISABLED":
        return SAHPI_EVENT_LOGGING_DISABLED
    if s == "RESERVED1":
        return SAHPI_RESERVED1
    if s == "SYSTEM_EVENT":
        return SAHPI_SYSTEM_EVENT
    if s == "CRITICAL_INTERRUPT":
        return SAHPI_CRITICAL_INTERRUPT
    if s == "BUTTON":
        return SAHPI_BUTTON
    if s == "MODULE_BOARD":
        return SAHPI_MODULE_BOARD
    if s == "MICROCONTROLLER_COPROCESSOR":
        return SAHPI_MICROCONTROLLER_COPROCESSOR
    if s == "ADDIN_CARD":
        return SAHPI_ADDIN_CARD
    if s == "CHASSIS":
        return SAHPI_CHASSIS
    if s == "CHIP_SET":
        return SAHPI_CHIP_SET
    if s == "OTHER_FRU":
        return SAHPI_OTHER_FRU
    if s == "CABLE_INTERCONNECT":
        return SAHPI_CABLE_INTERCONNECT
    if s == "TERMINATOR":
        return SAHPI_TERMINATOR
    if s == "SYSTEM_BOOT_INITIATED":
        return SAHPI_SYSTEM_BOOT_INITIATED
    if s == "BOOT_ERROR":
        return SAHPI_BOOT_ERROR
    if s == "OS_BOOT":
        return SAHPI_OS_BOOT
    if s == "OS_CRITICAL_STOP":
        return SAHPI_OS_CRITICAL_STOP
    if s == "SLOT_CONNECTOR":
        return SAHPI_SLOT_CONNECTOR
    if s == "SYSTEM_ACPI_POWER_STATE":
        return SAHPI_SYSTEM_ACPI_POWER_STATE
    if s == "RESERVED2":
        return SAHPI_RESERVED2
    if s == "PLATFORM_ALERT":
        return SAHPI_PLATFORM_ALERT
    if s == "ENTITY_PRESENCE":
        return SAHPI_ENTITY_PRESENCE
    if s == "MONITOR_ASIC_IC":
        return SAHPI_MONITOR_ASIC_IC
    if s == "LAN":
        return SAHPI_LAN
    if s == "MANAGEMENT_SUBSYSTEM_HEALTH":
        return SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH
    if s == "BATTERY":
        return SAHPI_BATTERY
    if s == "SESSION_AUDIT":
        return SAHPI_SESSION_AUDIT
    if s == "VERSION_CHANGE":
        return SAHPI_VERSION_CHANGE
    if s == "OPERATIONAL":
        return SAHPI_OPERATIONAL
    if s == "OEM_SENSOR":
        return SAHPI_OEM_SENSOR
    if s == "COMM_CHANNEL_LINK_STATE":
        return SAHPI_COMM_CHANNEL_LINK_STATE
    if s == "MANAGEMENT_BUS_STATE":
        return SAHPI_MANAGEMENT_BUS_STATE
    if s == "COMM_CHANNEL_BUS_STATE":
        return SAHPI_COMM_CHANNEL_BUS_STATE
    if s == "CONFIG_DATA":
        return SAHPI_CONFIG_DATA
    if s == "POWER_BUDGET":
        return SAHPI_POWER_BUDGET
    raise ValueError()

#**
# For SaHpiSensorReadingTypeT
#**
def fromSaHpiSensorReadingTypeT( x ):
    if x == SAHPI_SENSOR_READING_TYPE_INT64:
        return "INT64"
    if x == SAHPI_SENSOR_READING_TYPE_UINT64:
        return "UINT64"
    if x == SAHPI_SENSOR_READING_TYPE_FLOAT64:
        return "FLOAT64"
    if x == SAHPI_SENSOR_READING_TYPE_BUFFER:
        return "BUFFER"
    return repr( x )

def toSaHpiSensorReadingTypeT( s ):
    if s == "INT64":
        return SAHPI_SENSOR_READING_TYPE_INT64
    if s == "UINT64":
        return SAHPI_SENSOR_READING_TYPE_UINT64
    if s == "FLOAT64":
        return SAHPI_SENSOR_READING_TYPE_FLOAT64
    if s == "BUFFER":
        return SAHPI_SENSOR_READING_TYPE_BUFFER
    raise ValueError()

#**
# For SaHpiSensorEventMaskActionT
#**
def fromSaHpiSensorEventMaskActionT( x ):
    if x == SAHPI_SENS_ADD_EVENTS_TO_MASKS:
        return "ADD_EVENTS_TO_MASKS"
    if x == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS:
        return "REMOVE_EVENTS_FROM_MASKS"
    return repr( x )

def toSaHpiSensorEventMaskActionT( s ):
    if s == "ADD_EVENTS_TO_MASKS":
        return SAHPI_SENS_ADD_EVENTS_TO_MASKS
    if s == "REMOVE_EVENTS_FROM_MASKS":
        return SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS
    raise ValueError()

#**
# For SaHpiSensorUnitsT
#**
def fromSaHpiSensorUnitsT( x ):
    if x == SAHPI_SU_UNSPECIFIED:
        return "UNSPECIFIED"
    if x == SAHPI_SU_DEGREES_C:
        return "DEGREES_C"
    if x == SAHPI_SU_DEGREES_F:
        return "DEGREES_F"
    if x == SAHPI_SU_DEGREES_K:
        return "DEGREES_K"
    if x == SAHPI_SU_VOLTS:
        return "VOLTS"
    if x == SAHPI_SU_AMPS:
        return "AMPS"
    if x == SAHPI_SU_WATTS:
        return "WATTS"
    if x == SAHPI_SU_JOULES:
        return "JOULES"
    if x == SAHPI_SU_COULOMBS:
        return "COULOMBS"
    if x == SAHPI_SU_VA:
        return "VA"
    if x == SAHPI_SU_NITS:
        return "NITS"
    if x == SAHPI_SU_LUMEN:
        return "LUMEN"
    if x == SAHPI_SU_LUX:
        return "LUX"
    if x == SAHPI_SU_CANDELA:
        return "CANDELA"
    if x == SAHPI_SU_KPA:
        return "KPA"
    if x == SAHPI_SU_PSI:
        return "PSI"
    if x == SAHPI_SU_NEWTON:
        return "NEWTON"
    if x == SAHPI_SU_CFM:
        return "CFM"
    if x == SAHPI_SU_RPM:
        return "RPM"
    if x == SAHPI_SU_HZ:
        return "HZ"
    if x == SAHPI_SU_MICROSECOND:
        return "MICROSECOND"
    if x == SAHPI_SU_MILLISECOND:
        return "MILLISECOND"
    if x == SAHPI_SU_SECOND:
        return "SECOND"
    if x == SAHPI_SU_MINUTE:
        return "MINUTE"
    if x == SAHPI_SU_HOUR:
        return "HOUR"
    if x == SAHPI_SU_DAY:
        return "DAY"
    if x == SAHPI_SU_WEEK:
        return "WEEK"
    if x == SAHPI_SU_MIL:
        return "MIL"
    if x == SAHPI_SU_INCHES:
        return "INCHES"
    if x == SAHPI_SU_FEET:
        return "FEET"
    if x == SAHPI_SU_CU_IN:
        return "CU_IN"
    if x == SAHPI_SU_CU_FEET:
        return "CU_FEET"
    if x == SAHPI_SU_MM:
        return "MM"
    if x == SAHPI_SU_CM:
        return "CM"
    if x == SAHPI_SU_M:
        return "M"
    if x == SAHPI_SU_CU_CM:
        return "CU_CM"
    if x == SAHPI_SU_CU_M:
        return "CU_M"
    if x == SAHPI_SU_LITERS:
        return "LITERS"
    if x == SAHPI_SU_FLUID_OUNCE:
        return "FLUID_OUNCE"
    if x == SAHPI_SU_RADIANS:
        return "RADIANS"
    if x == SAHPI_SU_STERADIANS:
        return "STERADIANS"
    if x == SAHPI_SU_REVOLUTIONS:
        return "REVOLUTIONS"
    if x == SAHPI_SU_CYCLES:
        return "CYCLES"
    if x == SAHPI_SU_GRAVITIES:
        return "GRAVITIES"
    if x == SAHPI_SU_OUNCE:
        return "OUNCE"
    if x == SAHPI_SU_POUND:
        return "POUND"
    if x == SAHPI_SU_FT_LB:
        return "FT_LB"
    if x == SAHPI_SU_OZ_IN:
        return "OZ_IN"
    if x == SAHPI_SU_GAUSS:
        return "GAUSS"
    if x == SAHPI_SU_GILBERTS:
        return "GILBERTS"
    if x == SAHPI_SU_HENRY:
        return "HENRY"
    if x == SAHPI_SU_MILLIHENRY:
        return "MILLIHENRY"
    if x == SAHPI_SU_FARAD:
        return "FARAD"
    if x == SAHPI_SU_MICROFARAD:
        return "MICROFARAD"
    if x == SAHPI_SU_OHMS:
        return "OHMS"
    if x == SAHPI_SU_SIEMENS:
        return "SIEMENS"
    if x == SAHPI_SU_MOLE:
        return "MOLE"
    if x == SAHPI_SU_BECQUEREL:
        return "BECQUEREL"
    if x == SAHPI_SU_PPM:
        return "PPM"
    if x == SAHPI_SU_RESERVED:
        return "RESERVED"
    if x == SAHPI_SU_DECIBELS:
        return "DECIBELS"
    if x == SAHPI_SU_DBA:
        return "DBA"
    if x == SAHPI_SU_DBC:
        return "DBC"
    if x == SAHPI_SU_GRAY:
        return "GRAY"
    if x == SAHPI_SU_SIEVERT:
        return "SIEVERT"
    if x == SAHPI_SU_COLOR_TEMP_DEG_K:
        return "COLOR_TEMP_DEG_K"
    if x == SAHPI_SU_BIT:
        return "BIT"
    if x == SAHPI_SU_KILOBIT:
        return "KILOBIT"
    if x == SAHPI_SU_MEGABIT:
        return "MEGABIT"
    if x == SAHPI_SU_GIGABIT:
        return "GIGABIT"
    if x == SAHPI_SU_BYTE:
        return "BYTE"
    if x == SAHPI_SU_KILOBYTE:
        return "KILOBYTE"
    if x == SAHPI_SU_MEGABYTE:
        return "MEGABYTE"
    if x == SAHPI_SU_GIGABYTE:
        return "GIGABYTE"
    if x == SAHPI_SU_WORD:
        return "WORD"
    if x == SAHPI_SU_DWORD:
        return "DWORD"
    if x == SAHPI_SU_QWORD:
        return "QWORD"
    if x == SAHPI_SU_LINE:
        return "LINE"
    if x == SAHPI_SU_HIT:
        return "HIT"
    if x == SAHPI_SU_MISS:
        return "MISS"
    if x == SAHPI_SU_RETRY:
        return "RETRY"
    if x == SAHPI_SU_RESET:
        return "RESET"
    if x == SAHPI_SU_OVERRUN:
        return "OVERRUN"
    if x == SAHPI_SU_UNDERRUN:
        return "UNDERRUN"
    if x == SAHPI_SU_COLLISION:
        return "COLLISION"
    if x == SAHPI_SU_PACKETS:
        return "PACKETS"
    if x == SAHPI_SU_MESSAGES:
        return "MESSAGES"
    if x == SAHPI_SU_CHARACTERS:
        return "CHARACTERS"
    if x == SAHPI_SU_ERRORS:
        return "ERRORS"
    if x == SAHPI_SU_CORRECTABLE_ERRORS:
        return "CORRECTABLE_ERRORS"
    if x == SAHPI_SU_UNCORRECTABLE_ERRORS:
        return "UNCORRECTABLE_ERRORS"
    return repr( x )

def toSaHpiSensorUnitsT( s ):
    if s == "UNSPECIFIED":
        return SAHPI_SU_UNSPECIFIED
    if s == "DEGREES_C":
        return SAHPI_SU_DEGREES_C
    if s == "DEGREES_F":
        return SAHPI_SU_DEGREES_F
    if s == "DEGREES_K":
        return SAHPI_SU_DEGREES_K
    if s == "VOLTS":
        return SAHPI_SU_VOLTS
    if s == "AMPS":
        return SAHPI_SU_AMPS
    if s == "WATTS":
        return SAHPI_SU_WATTS
    if s == "JOULES":
        return SAHPI_SU_JOULES
    if s == "COULOMBS":
        return SAHPI_SU_COULOMBS
    if s == "VA":
        return SAHPI_SU_VA
    if s == "NITS":
        return SAHPI_SU_NITS
    if s == "LUMEN":
        return SAHPI_SU_LUMEN
    if s == "LUX":
        return SAHPI_SU_LUX
    if s == "CANDELA":
        return SAHPI_SU_CANDELA
    if s == "KPA":
        return SAHPI_SU_KPA
    if s == "PSI":
        return SAHPI_SU_PSI
    if s == "NEWTON":
        return SAHPI_SU_NEWTON
    if s == "CFM":
        return SAHPI_SU_CFM
    if s == "RPM":
        return SAHPI_SU_RPM
    if s == "HZ":
        return SAHPI_SU_HZ
    if s == "MICROSECOND":
        return SAHPI_SU_MICROSECOND
    if s == "MILLISECOND":
        return SAHPI_SU_MILLISECOND
    if s == "SECOND":
        return SAHPI_SU_SECOND
    if s == "MINUTE":
        return SAHPI_SU_MINUTE
    if s == "HOUR":
        return SAHPI_SU_HOUR
    if s == "DAY":
        return SAHPI_SU_DAY
    if s == "WEEK":
        return SAHPI_SU_WEEK
    if s == "MIL":
        return SAHPI_SU_MIL
    if s == "INCHES":
        return SAHPI_SU_INCHES
    if s == "FEET":
        return SAHPI_SU_FEET
    if s == "CU_IN":
        return SAHPI_SU_CU_IN
    if s == "CU_FEET":
        return SAHPI_SU_CU_FEET
    if s == "MM":
        return SAHPI_SU_MM
    if s == "CM":
        return SAHPI_SU_CM
    if s == "M":
        return SAHPI_SU_M
    if s == "CU_CM":
        return SAHPI_SU_CU_CM
    if s == "CU_M":
        return SAHPI_SU_CU_M
    if s == "LITERS":
        return SAHPI_SU_LITERS
    if s == "FLUID_OUNCE":
        return SAHPI_SU_FLUID_OUNCE
    if s == "RADIANS":
        return SAHPI_SU_RADIANS
    if s == "STERADIANS":
        return SAHPI_SU_STERADIANS
    if s == "REVOLUTIONS":
        return SAHPI_SU_REVOLUTIONS
    if s == "CYCLES":
        return SAHPI_SU_CYCLES
    if s == "GRAVITIES":
        return SAHPI_SU_GRAVITIES
    if s == "OUNCE":
        return SAHPI_SU_OUNCE
    if s == "POUND":
        return SAHPI_SU_POUND
    if s == "FT_LB":
        return SAHPI_SU_FT_LB
    if s == "OZ_IN":
        return SAHPI_SU_OZ_IN
    if s == "GAUSS":
        return SAHPI_SU_GAUSS
    if s == "GILBERTS":
        return SAHPI_SU_GILBERTS
    if s == "HENRY":
        return SAHPI_SU_HENRY
    if s == "MILLIHENRY":
        return SAHPI_SU_MILLIHENRY
    if s == "FARAD":
        return SAHPI_SU_FARAD
    if s == "MICROFARAD":
        return SAHPI_SU_MICROFARAD
    if s == "OHMS":
        return SAHPI_SU_OHMS
    if s == "SIEMENS":
        return SAHPI_SU_SIEMENS
    if s == "MOLE":
        return SAHPI_SU_MOLE
    if s == "BECQUEREL":
        return SAHPI_SU_BECQUEREL
    if s == "PPM":
        return SAHPI_SU_PPM
    if s == "RESERVED":
        return SAHPI_SU_RESERVED
    if s == "DECIBELS":
        return SAHPI_SU_DECIBELS
    if s == "DBA":
        return SAHPI_SU_DBA
    if s == "DBC":
        return SAHPI_SU_DBC
    if s == "GRAY":
        return SAHPI_SU_GRAY
    if s == "SIEVERT":
        return SAHPI_SU_SIEVERT
    if s == "COLOR_TEMP_DEG_K":
        return SAHPI_SU_COLOR_TEMP_DEG_K
    if s == "BIT":
        return SAHPI_SU_BIT
    if s == "KILOBIT":
        return SAHPI_SU_KILOBIT
    if s == "MEGABIT":
        return SAHPI_SU_MEGABIT
    if s == "GIGABIT":
        return SAHPI_SU_GIGABIT
    if s == "BYTE":
        return SAHPI_SU_BYTE
    if s == "KILOBYTE":
        return SAHPI_SU_KILOBYTE
    if s == "MEGABYTE":
        return SAHPI_SU_MEGABYTE
    if s == "GIGABYTE":
        return SAHPI_SU_GIGABYTE
    if s == "WORD":
        return SAHPI_SU_WORD
    if s == "DWORD":
        return SAHPI_SU_DWORD
    if s == "QWORD":
        return SAHPI_SU_QWORD
    if s == "LINE":
        return SAHPI_SU_LINE
    if s == "HIT":
        return SAHPI_SU_HIT
    if s == "MISS":
        return SAHPI_SU_MISS
    if s == "RETRY":
        return SAHPI_SU_RETRY
    if s == "RESET":
        return SAHPI_SU_RESET
    if s == "OVERRUN":
        return SAHPI_SU_OVERRUN
    if s == "UNDERRUN":
        return SAHPI_SU_UNDERRUN
    if s == "COLLISION":
        return SAHPI_SU_COLLISION
    if s == "PACKETS":
        return SAHPI_SU_PACKETS
    if s == "MESSAGES":
        return SAHPI_SU_MESSAGES
    if s == "CHARACTERS":
        return SAHPI_SU_CHARACTERS
    if s == "ERRORS":
        return SAHPI_SU_ERRORS
    if s == "CORRECTABLE_ERRORS":
        return SAHPI_SU_CORRECTABLE_ERRORS
    if s == "UNCORRECTABLE_ERRORS":
        return SAHPI_SU_UNCORRECTABLE_ERRORS
    raise ValueError()

#**
# For SaHpiSensorModUnitUseT
#**
def fromSaHpiSensorModUnitUseT( x ):
    if x == SAHPI_SMUU_NONE:
        return "NONE"
    if x == SAHPI_SMUU_BASIC_OVER_MODIFIER:
        return "BASIC_OVER_MODIFIER"
    if x == SAHPI_SMUU_BASIC_TIMES_MODIFIER:
        return "BASIC_TIMES_MODIFIER"
    return repr( x )

def toSaHpiSensorModUnitUseT( s ):
    if s == "NONE":
        return SAHPI_SMUU_NONE
    if s == "BASIC_OVER_MODIFIER":
        return SAHPI_SMUU_BASIC_OVER_MODIFIER
    if s == "BASIC_TIMES_MODIFIER":
        return SAHPI_SMUU_BASIC_TIMES_MODIFIER
    raise ValueError()

#**
# For SaHpiSensorEventCtrlT
#**
def fromSaHpiSensorEventCtrlT( x ):
    if x == SAHPI_SEC_PER_EVENT:
        return "PER_EVENT"
    if x == SAHPI_SEC_READ_ONLY_MASKS:
        return "READ_ONLY_MASKS"
    if x == SAHPI_SEC_READ_ONLY:
        return "READ_ONLY"
    return repr( x )

def toSaHpiSensorEventCtrlT( s ):
    if s == "PER_EVENT":
        return SAHPI_SEC_PER_EVENT
    if s == "READ_ONLY_MASKS":
        return SAHPI_SEC_READ_ONLY_MASKS
    if s == "READ_ONLY":
        return SAHPI_SEC_READ_ONLY
    raise ValueError()

#**
# For SaHpiCtrlTypeT
#**
def fromSaHpiCtrlTypeT( x ):
    if x == SAHPI_CTRL_TYPE_DIGITAL:
        return "DIGITAL"
    if x == SAHPI_CTRL_TYPE_DISCRETE:
        return "DISCRETE"
    if x == SAHPI_CTRL_TYPE_ANALOG:
        return "ANALOG"
    if x == SAHPI_CTRL_TYPE_STREAM:
        return "STREAM"
    if x == SAHPI_CTRL_TYPE_TEXT:
        return "TEXT"
    if x == SAHPI_CTRL_TYPE_OEM:
        return "OEM"
    return repr( x )

def toSaHpiCtrlTypeT( s ):
    if s == "DIGITAL":
        return SAHPI_CTRL_TYPE_DIGITAL
    if s == "DISCRETE":
        return SAHPI_CTRL_TYPE_DISCRETE
    if s == "ANALOG":
        return SAHPI_CTRL_TYPE_ANALOG
    if s == "STREAM":
        return SAHPI_CTRL_TYPE_STREAM
    if s == "TEXT":
        return SAHPI_CTRL_TYPE_TEXT
    if s == "OEM":
        return SAHPI_CTRL_TYPE_OEM
    raise ValueError()

#**
# For SaHpiCtrlStateDigitalT
#**
def fromSaHpiCtrlStateDigitalT( x ):
    if x == SAHPI_CTRL_STATE_OFF:
        return "OFF"
    if x == SAHPI_CTRL_STATE_ON:
        return "ON"
    if x == SAHPI_CTRL_STATE_PULSE_OFF:
        return "PULSE_OFF"
    if x == SAHPI_CTRL_STATE_PULSE_ON:
        return "PULSE_ON"
    return repr( x )

def toSaHpiCtrlStateDigitalT( s ):
    if s == "OFF":
        return SAHPI_CTRL_STATE_OFF
    if s == "ON":
        return SAHPI_CTRL_STATE_ON
    if s == "PULSE_OFF":
        return SAHPI_CTRL_STATE_PULSE_OFF
    if s == "PULSE_ON":
        return SAHPI_CTRL_STATE_PULSE_ON
    raise ValueError()

#**
# For SaHpiCtrlModeT
#**
def fromSaHpiCtrlModeT( x ):
    if x == SAHPI_CTRL_MODE_AUTO:
        return "AUTO"
    if x == SAHPI_CTRL_MODE_MANUAL:
        return "MANUAL"
    return repr( x )

def toSaHpiCtrlModeT( s ):
    if s == "AUTO":
        return SAHPI_CTRL_MODE_AUTO
    if s == "MANUAL":
        return SAHPI_CTRL_MODE_MANUAL
    raise ValueError()

#**
# For SaHpiCtrlOutputTypeT
#**
def fromSaHpiCtrlOutputTypeT( x ):
    if x == SAHPI_CTRL_GENERIC:
        return "GENERIC"
    if x == SAHPI_CTRL_LED:
        return "LED"
    if x == SAHPI_CTRL_FAN_SPEED:
        return "FAN_SPEED"
    if x == SAHPI_CTRL_DRY_CONTACT_CLOSURE:
        return "DRY_CONTACT_CLOSURE"
    if x == SAHPI_CTRL_POWER_SUPPLY_INHIBIT:
        return "POWER_SUPPLY_INHIBIT"
    if x == SAHPI_CTRL_AUDIBLE:
        return "AUDIBLE"
    if x == SAHPI_CTRL_FRONT_PANEL_LOCKOUT:
        return "FRONT_PANEL_LOCKOUT"
    if x == SAHPI_CTRL_POWER_INTERLOCK:
        return "POWER_INTERLOCK"
    if x == SAHPI_CTRL_POWER_STATE:
        return "POWER_STATE"
    if x == SAHPI_CTRL_LCD_DISPLAY:
        return "LCD_DISPLAY"
    if x == SAHPI_CTRL_OEM:
        return "OEM"
    if x == SAHPI_CTRL_GENERIC_ADDRESS:
        return "GENERIC_ADDRESS"
    if x == SAHPI_CTRL_IP_ADDRESS:
        return "IP_ADDRESS"
    if x == SAHPI_CTRL_RESOURCE_ID:
        return "RESOURCE_ID"
    if x == SAHPI_CTRL_POWER_BUDGET:
        return "POWER_BUDGET"
    if x == SAHPI_CTRL_ACTIVATE:
        return "ACTIVATE"
    if x == SAHPI_CTRL_RESET:
        return "RESET"
    return repr( x )

def toSaHpiCtrlOutputTypeT( s ):
    if s == "GENERIC":
        return SAHPI_CTRL_GENERIC
    if s == "LED":
        return SAHPI_CTRL_LED
    if s == "FAN_SPEED":
        return SAHPI_CTRL_FAN_SPEED
    if s == "DRY_CONTACT_CLOSURE":
        return SAHPI_CTRL_DRY_CONTACT_CLOSURE
    if s == "POWER_SUPPLY_INHIBIT":
        return SAHPI_CTRL_POWER_SUPPLY_INHIBIT
    if s == "AUDIBLE":
        return SAHPI_CTRL_AUDIBLE
    if s == "FRONT_PANEL_LOCKOUT":
        return SAHPI_CTRL_FRONT_PANEL_LOCKOUT
    if s == "POWER_INTERLOCK":
        return SAHPI_CTRL_POWER_INTERLOCK
    if s == "POWER_STATE":
        return SAHPI_CTRL_POWER_STATE
    if s == "LCD_DISPLAY":
        return SAHPI_CTRL_LCD_DISPLAY
    if s == "OEM":
        return SAHPI_CTRL_OEM
    if s == "GENERIC_ADDRESS":
        return SAHPI_CTRL_GENERIC_ADDRESS
    if s == "IP_ADDRESS":
        return SAHPI_CTRL_IP_ADDRESS
    if s == "RESOURCE_ID":
        return SAHPI_CTRL_RESOURCE_ID
    if s == "POWER_BUDGET":
        return SAHPI_CTRL_POWER_BUDGET
    if s == "ACTIVATE":
        return SAHPI_CTRL_ACTIVATE
    if s == "RESET":
        return SAHPI_CTRL_RESET
    raise ValueError()

#**
# For SaHpiIdrAreaTypeT
#**
def fromSaHpiIdrAreaTypeT( x ):
    if x == SAHPI_IDR_AREATYPE_INTERNAL_USE:
        return "INTERNAL_USE"
    if x == SAHPI_IDR_AREATYPE_CHASSIS_INFO:
        return "CHASSIS_INFO"
    if x == SAHPI_IDR_AREATYPE_BOARD_INFO:
        return "BOARD_INFO"
    if x == SAHPI_IDR_AREATYPE_PRODUCT_INFO:
        return "PRODUCT_INFO"
    if x == SAHPI_IDR_AREATYPE_OEM:
        return "OEM"
    if x == SAHPI_IDR_AREATYPE_UNSPECIFIED:
        return "UNSPECIFIED"
    return repr( x )

def toSaHpiIdrAreaTypeT( s ):
    if s == "INTERNAL_USE":
        return SAHPI_IDR_AREATYPE_INTERNAL_USE
    if s == "CHASSIS_INFO":
        return SAHPI_IDR_AREATYPE_CHASSIS_INFO
    if s == "BOARD_INFO":
        return SAHPI_IDR_AREATYPE_BOARD_INFO
    if s == "PRODUCT_INFO":
        return SAHPI_IDR_AREATYPE_PRODUCT_INFO
    if s == "OEM":
        return SAHPI_IDR_AREATYPE_OEM
    if s == "UNSPECIFIED":
        return SAHPI_IDR_AREATYPE_UNSPECIFIED
    raise ValueError()

#**
# For SaHpiIdrFieldTypeT
#**
def fromSaHpiIdrFieldTypeT( x ):
    if x == SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE:
        return "CHASSIS_TYPE"
    if x == SAHPI_IDR_FIELDTYPE_MFG_DATETIME:
        return "MFG_DATETIME"
    if x == SAHPI_IDR_FIELDTYPE_MANUFACTURER:
        return "MANUFACTURER"
    if x == SAHPI_IDR_FIELDTYPE_PRODUCT_NAME:
        return "PRODUCT_NAME"
    if x == SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION:
        return "PRODUCT_VERSION"
    if x == SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER:
        return "SERIAL_NUMBER"
    if x == SAHPI_IDR_FIELDTYPE_PART_NUMBER:
        return "PART_NUMBER"
    if x == SAHPI_IDR_FIELDTYPE_FILE_ID:
        return "FILE_ID"
    if x == SAHPI_IDR_FIELDTYPE_ASSET_TAG:
        return "ASSET_TAG"
    if x == SAHPI_IDR_FIELDTYPE_CUSTOM:
        return "CUSTOM"
    if x == SAHPI_IDR_FIELDTYPE_UNSPECIFIED:
        return "UNSPECIFIED"
    return repr( x )

def toSaHpiIdrFieldTypeT( s ):
    if s == "CHASSIS_TYPE":
        return SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE
    if s == "MFG_DATETIME":
        return SAHPI_IDR_FIELDTYPE_MFG_DATETIME
    if s == "MANUFACTURER":
        return SAHPI_IDR_FIELDTYPE_MANUFACTURER
    if s == "PRODUCT_NAME":
        return SAHPI_IDR_FIELDTYPE_PRODUCT_NAME
    if s == "PRODUCT_VERSION":
        return SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION
    if s == "SERIAL_NUMBER":
        return SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER
    if s == "PART_NUMBER":
        return SAHPI_IDR_FIELDTYPE_PART_NUMBER
    if s == "FILE_ID":
        return SAHPI_IDR_FIELDTYPE_FILE_ID
    if s == "ASSET_TAG":
        return SAHPI_IDR_FIELDTYPE_ASSET_TAG
    if s == "CUSTOM":
        return SAHPI_IDR_FIELDTYPE_CUSTOM
    if s == "UNSPECIFIED":
        return SAHPI_IDR_FIELDTYPE_UNSPECIFIED
    raise ValueError()

#**
# For SaHpiWatchdogActionT
#**
def fromSaHpiWatchdogActionT( x ):
    if x == SAHPI_WA_NO_ACTION:
        return "NO_ACTION"
    if x == SAHPI_WA_RESET:
        return "RESET"
    if x == SAHPI_WA_POWER_DOWN:
        return "POWER_DOWN"
    if x == SAHPI_WA_POWER_CYCLE:
        return "POWER_CYCLE"
    return repr( x )

def toSaHpiWatchdogActionT( s ):
    if s == "NO_ACTION":
        return SAHPI_WA_NO_ACTION
    if s == "RESET":
        return SAHPI_WA_RESET
    if s == "POWER_DOWN":
        return SAHPI_WA_POWER_DOWN
    if s == "POWER_CYCLE":
        return SAHPI_WA_POWER_CYCLE
    raise ValueError()

#**
# For SaHpiWatchdogActionEventT
#**
def fromSaHpiWatchdogActionEventT( x ):
    if x == SAHPI_WAE_NO_ACTION:
        return "NO_ACTION"
    if x == SAHPI_WAE_RESET:
        return "RESET"
    if x == SAHPI_WAE_POWER_DOWN:
        return "POWER_DOWN"
    if x == SAHPI_WAE_POWER_CYCLE:
        return "POWER_CYCLE"
    if x == SAHPI_WAE_TIMER_INT:
        return "TIMER_INT"
    return repr( x )

def toSaHpiWatchdogActionEventT( s ):
    if s == "NO_ACTION":
        return SAHPI_WAE_NO_ACTION
    if s == "RESET":
        return SAHPI_WAE_RESET
    if s == "POWER_DOWN":
        return SAHPI_WAE_POWER_DOWN
    if s == "POWER_CYCLE":
        return SAHPI_WAE_POWER_CYCLE
    if s == "TIMER_INT":
        return SAHPI_WAE_TIMER_INT
    raise ValueError()

#**
# For SaHpiWatchdogPretimerInterruptT
#**
def fromSaHpiWatchdogPretimerInterruptT( x ):
    if x == SAHPI_WPI_NONE:
        return "NONE"
    if x == SAHPI_WPI_SMI:
        return "SMI"
    if x == SAHPI_WPI_NMI:
        return "NMI"
    if x == SAHPI_WPI_MESSAGE_INTERRUPT:
        return "MESSAGE_INTERRUPT"
    if x == SAHPI_WPI_OEM:
        return "OEM"
    return repr( x )

def toSaHpiWatchdogPretimerInterruptT( s ):
    if s == "NONE":
        return SAHPI_WPI_NONE
    if s == "SMI":
        return SAHPI_WPI_SMI
    if s == "NMI":
        return SAHPI_WPI_NMI
    if s == "MESSAGE_INTERRUPT":
        return SAHPI_WPI_MESSAGE_INTERRUPT
    if s == "OEM":
        return SAHPI_WPI_OEM
    raise ValueError()

#**
# For SaHpiWatchdogTimerUseT
#**
def fromSaHpiWatchdogTimerUseT( x ):
    if x == SAHPI_WTU_NONE:
        return "NONE"
    if x == SAHPI_WTU_BIOS_FRB2:
        return "BIOS_FRB2"
    if x == SAHPI_WTU_BIOS_POST:
        return "BIOS_POST"
    if x == SAHPI_WTU_OS_LOAD:
        return "OS_LOAD"
    if x == SAHPI_WTU_SMS_OS:
        return "SMS_OS"
    if x == SAHPI_WTU_OEM:
        return "OEM"
    if x == SAHPI_WTU_UNSPECIFIED:
        return "UNSPECIFIED"
    return repr( x )

def toSaHpiWatchdogTimerUseT( s ):
    if s == "NONE":
        return SAHPI_WTU_NONE
    if s == "BIOS_FRB2":
        return SAHPI_WTU_BIOS_FRB2
    if s == "BIOS_POST":
        return SAHPI_WTU_BIOS_POST
    if s == "OS_LOAD":
        return SAHPI_WTU_OS_LOAD
    if s == "SMS_OS":
        return SAHPI_WTU_SMS_OS
    if s == "OEM":
        return SAHPI_WTU_OEM
    if s == "UNSPECIFIED":
        return SAHPI_WTU_UNSPECIFIED
    raise ValueError()

#**
# For SaHpiDimiTestServiceImpactT
#**
def fromSaHpiDimiTestServiceImpactT( x ):
    if x == SAHPI_DIMITEST_NONDEGRADING:
        return "NONDEGRADING"
    if x == SAHPI_DIMITEST_DEGRADING:
        return "DEGRADING"
    if x == SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL:
        return "VENDOR_DEFINED_LEVEL"
    return repr( x )

def toSaHpiDimiTestServiceImpactT( s ):
    if s == "NONDEGRADING":
        return SAHPI_DIMITEST_NONDEGRADING
    if s == "DEGRADING":
        return SAHPI_DIMITEST_DEGRADING
    if s == "VENDOR_DEFINED_LEVEL":
        return SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL
    raise ValueError()

#**
# For SaHpiDimiTestRunStatusT
#**
def fromSaHpiDimiTestRunStatusT( x ):
    if x == SAHPI_DIMITEST_STATUS_NOT_RUN:
        return "NOT_RUN"
    if x == SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS:
        return "FINISHED_NO_ERRORS"
    if x == SAHPI_DIMITEST_STATUS_FINISHED_ERRORS:
        return "FINISHED_ERRORS"
    if x == SAHPI_DIMITEST_STATUS_CANCELED:
        return "CANCELED"
    if x == SAHPI_DIMITEST_STATUS_RUNNING:
        return "RUNNING"
    return repr( x )

def toSaHpiDimiTestRunStatusT( s ):
    if s == "NOT_RUN":
        return SAHPI_DIMITEST_STATUS_NOT_RUN
    if s == "FINISHED_NO_ERRORS":
        return SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS
    if s == "FINISHED_ERRORS":
        return SAHPI_DIMITEST_STATUS_FINISHED_ERRORS
    if s == "CANCELED":
        return SAHPI_DIMITEST_STATUS_CANCELED
    if s == "RUNNING":
        return SAHPI_DIMITEST_STATUS_RUNNING
    raise ValueError()

#**
# For SaHpiDimiTestErrCodeT
#**
def fromSaHpiDimiTestErrCodeT( x ):
    if x == SAHPI_DIMITEST_STATUSERR_NOERR:
        return "NOERR"
    if x == SAHPI_DIMITEST_STATUSERR_RUNERR:
        return "RUNERR"
    if x == SAHPI_DIMITEST_STATUSERR_UNDEF:
        return "UNDEF"
    return repr( x )

def toSaHpiDimiTestErrCodeT( s ):
    if s == "NOERR":
        return SAHPI_DIMITEST_STATUSERR_NOERR
    if s == "RUNERR":
        return SAHPI_DIMITEST_STATUSERR_RUNERR
    if s == "UNDEF":
        return SAHPI_DIMITEST_STATUSERR_UNDEF
    raise ValueError()

#**
# For SaHpiDimiTestParamTypeT
#**
def fromSaHpiDimiTestParamTypeT( x ):
    if x == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
        return "BOOLEAN"
    if x == SAHPI_DIMITEST_PARAM_TYPE_INT32:
        return "INT32"
    if x == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
        return "FLOAT64"
    if x == SAHPI_DIMITEST_PARAM_TYPE_TEXT:
        return "TEXT"
    return repr( x )

def toSaHpiDimiTestParamTypeT( s ):
    if s == "BOOLEAN":
        return SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN
    if s == "INT32":
        return SAHPI_DIMITEST_PARAM_TYPE_INT32
    if s == "FLOAT64":
        return SAHPI_DIMITEST_PARAM_TYPE_FLOAT64
    if s == "TEXT":
        return SAHPI_DIMITEST_PARAM_TYPE_TEXT
    raise ValueError()

#**
# For SaHpiDimiReadyT
#**
def fromSaHpiDimiReadyT( x ):
    if x == SAHPI_DIMI_READY:
        return "READY"
    if x == SAHPI_DIMI_WRONG_STATE:
        return "WRONG_STATE"
    if x == SAHPI_DIMI_BUSY:
        return "BUSY"
    return repr( x )

def toSaHpiDimiReadyT( s ):
    if s == "READY":
        return SAHPI_DIMI_READY
    if s == "WRONG_STATE":
        return SAHPI_DIMI_WRONG_STATE
    if s == "BUSY":
        return SAHPI_DIMI_BUSY
    raise ValueError()

#**
# For SaHpiFumiSpecInfoTypeT
#**
def fromSaHpiFumiSpecInfoTypeT( x ):
    if x == SAHPI_FUMI_SPEC_INFO_NONE:
        return "NONE"
    if x == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
        return "SAF_DEFINED"
    if x == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
        return "OEM_DEFINED"
    return repr( x )

def toSaHpiFumiSpecInfoTypeT( s ):
    if s == "NONE":
        return SAHPI_FUMI_SPEC_INFO_NONE
    if s == "SAF_DEFINED":
        return SAHPI_FUMI_SPEC_INFO_SAF_DEFINED
    if s == "OEM_DEFINED":
        return SAHPI_FUMI_SPEC_INFO_OEM_DEFINED
    raise ValueError()

#**
# For SaHpiFumiSafDefinedSpecIdT
#**
def fromSaHpiFumiSafDefinedSpecIdT( x ):
    if x == SAHPI_FUMI_SPEC_HPM1:
        return ""
    return repr( x )

def toSaHpiFumiSafDefinedSpecIdT( s ):
    if s == "":
        return SAHPI_FUMI_SPEC_HPM1
    raise ValueError()

#**
# For SaHpiFumiServiceImpactT
#**
def fromSaHpiFumiServiceImpactT( x ):
    if x == SAHPI_FUMI_PROCESS_NONDEGRADING:
        return "NONDEGRADING"
    if x == SAHPI_FUMI_PROCESS_DEGRADING:
        return "DEGRADING"
    if x == SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL:
        return "VENDOR_DEFINED_IMPACT_LEVEL"
    return repr( x )

def toSaHpiFumiServiceImpactT( s ):
    if s == "NONDEGRADING":
        return SAHPI_FUMI_PROCESS_NONDEGRADING
    if s == "DEGRADING":
        return SAHPI_FUMI_PROCESS_DEGRADING
    if s == "VENDOR_DEFINED_IMPACT_LEVEL":
        return SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL
    raise ValueError()

#**
# For SaHpiFumiSourceStatusT
#**
def fromSaHpiFumiSourceStatusT( x ):
    if x == SAHPI_FUMI_SRC_VALID:
        return "VALID"
    if x == SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED:
        return "PROTOCOL_NOT_SUPPORTED"
    if x == SAHPI_FUMI_SRC_UNREACHABLE:
        return "UNREACHABLE"
    if x == SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED:
        return "VALIDATION_NOT_STARTED"
    if x == SAHPI_FUMI_SRC_VALIDATION_INITIATED:
        return "VALIDATION_INITIATED"
    if x == SAHPI_FUMI_SRC_VALIDATION_FAIL:
        return "VALIDATION_FAIL"
    if x == SAHPI_FUMI_SRC_TYPE_MISMATCH:
        return "TYPE_MISMATCH"
    if x == SAHPI_FUMI_SRC_INVALID:
        return "INVALID"
    if x == SAHPI_FUMI_SRC_VALIDITY_UNKNOWN:
        return "VALIDITY_UNKNOWN"
    return repr( x )

def toSaHpiFumiSourceStatusT( s ):
    if s == "VALID":
        return SAHPI_FUMI_SRC_VALID
    if s == "PROTOCOL_NOT_SUPPORTED":
        return SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED
    if s == "UNREACHABLE":
        return SAHPI_FUMI_SRC_UNREACHABLE
    if s == "VALIDATION_NOT_STARTED":
        return SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED
    if s == "VALIDATION_INITIATED":
        return SAHPI_FUMI_SRC_VALIDATION_INITIATED
    if s == "VALIDATION_FAIL":
        return SAHPI_FUMI_SRC_VALIDATION_FAIL
    if s == "TYPE_MISMATCH":
        return SAHPI_FUMI_SRC_TYPE_MISMATCH
    if s == "INVALID":
        return SAHPI_FUMI_SRC_INVALID
    if s == "VALIDITY_UNKNOWN":
        return SAHPI_FUMI_SRC_VALIDITY_UNKNOWN
    raise ValueError()

#**
# For SaHpiFumiBankStateT
#**
def fromSaHpiFumiBankStateT( x ):
    if x == SAHPI_FUMI_BANK_VALID:
        return "VALID"
    if x == SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS:
        return "UPGRADE_IN_PROGRESS"
    if x == SAHPI_FUMI_BANK_CORRUPTED:
        return "CORRUPTED"
    if x == SAHPI_FUMI_BANK_ACTIVE:
        return "ACTIVE"
    if x == SAHPI_FUMI_BANK_BUSY:
        return "BUSY"
    if x == SAHPI_FUMI_BANK_UNKNOWN:
        return "UNKNOWN"
    return repr( x )

def toSaHpiFumiBankStateT( s ):
    if s == "VALID":
        return SAHPI_FUMI_BANK_VALID
    if s == "UPGRADE_IN_PROGRESS":
        return SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS
    if s == "CORRUPTED":
        return SAHPI_FUMI_BANK_CORRUPTED
    if s == "ACTIVE":
        return SAHPI_FUMI_BANK_ACTIVE
    if s == "BUSY":
        return SAHPI_FUMI_BANK_BUSY
    if s == "UNKNOWN":
        return SAHPI_FUMI_BANK_UNKNOWN
    raise ValueError()

#**
# For SaHpiFumiUpgradeStatusT
#**
def fromSaHpiFumiUpgradeStatusT( x ):
    if x == SAHPI_FUMI_OPERATION_NOTSTARTED:
        return "OPERATION_NOTSTARTED"
    if x == SAHPI_FUMI_SOURCE_VALIDATION_INITIATED:
        return "SOURCE_VALIDATION_INITIATED"
    if x == SAHPI_FUMI_SOURCE_VALIDATION_FAILED:
        return "SOURCE_VALIDATION_FAILED"
    if x == SAHPI_FUMI_SOURCE_VALIDATION_DONE:
        return "SOURCE_VALIDATION_DONE"
    if x == SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED:
        return "SOURCE_VALIDATION_CANCELLED"
    if x == SAHPI_FUMI_INSTALL_INITIATED:
        return "INSTALL_INITIATED"
    if x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED:
        return "INSTALL_FAILED_ROLLBACK_NEEDED"
    if x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED:
        return "INSTALL_FAILED_ROLLBACK_INITIATED"
    if x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE:
        return "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE"
    if x == SAHPI_FUMI_INSTALL_DONE:
        return "INSTALL_DONE"
    if x == SAHPI_FUMI_INSTALL_CANCELLED:
        return "INSTALL_CANCELLED"
    if x == SAHPI_FUMI_ROLLBACK_INITIATED:
        return "ROLLBACK_INITIATED"
    if x == SAHPI_FUMI_ROLLBACK_FAILED:
        return "ROLLBACK_FAILED"
    if x == SAHPI_FUMI_ROLLBACK_DONE:
        return "ROLLBACK_DONE"
    if x == SAHPI_FUMI_ROLLBACK_CANCELLED:
        return "ROLLBACK_CANCELLED"
    if x == SAHPI_FUMI_BACKUP_INITIATED:
        return "BACKUP_INITIATED"
    if x == SAHPI_FUMI_BACKUP_FAILED:
        return "BACKUP_FAILED"
    if x == SAHPI_FUMI_BACKUP_DONE:
        return "BACKUP_DONE"
    if x == SAHPI_FUMI_BACKUP_CANCELLED:
        return "BACKUP_CANCELLED"
    if x == SAHPI_FUMI_BANK_COPY_INITIATED:
        return "BANK_COPY_INITIATED"
    if x == SAHPI_FUMI_BANK_COPY_FAILED:
        return "BANK_COPY_FAILED"
    if x == SAHPI_FUMI_BANK_COPY_DONE:
        return "BANK_COPY_DONE"
    if x == SAHPI_FUMI_BANK_COPY_CANCELLED:
        return "BANK_COPY_CANCELLED"
    if x == SAHPI_FUMI_TARGET_VERIFY_INITIATED:
        return "TARGET_VERIFY_INITIATED"
    if x == SAHPI_FUMI_TARGET_VERIFY_FAILED:
        return "TARGET_VERIFY_FAILED"
    if x == SAHPI_FUMI_TARGET_VERIFY_DONE:
        return "TARGET_VERIFY_DONE"
    if x == SAHPI_FUMI_TARGET_VERIFY_CANCELLED:
        return "TARGET_VERIFY_CANCELLED"
    if x == SAHPI_FUMI_ACTIVATE_INITIATED:
        return "ACTIVATE_INITIATED"
    if x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED:
        return "ACTIVATE_FAILED_ROLLBACK_NEEDED"
    if x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED:
        return "ACTIVATE_FAILED_ROLLBACK_INITIATED"
    if x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE:
        return "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE"
    if x == SAHPI_FUMI_ACTIVATE_DONE:
        return "ACTIVATE_DONE"
    if x == SAHPI_FUMI_ACTIVATE_CANCELLED:
        return "ACTIVATE_CANCELLED"
    return repr( x )

def toSaHpiFumiUpgradeStatusT( s ):
    if s == "OPERATION_NOTSTARTED":
        return SAHPI_FUMI_OPERATION_NOTSTARTED
    if s == "SOURCE_VALIDATION_INITIATED":
        return SAHPI_FUMI_SOURCE_VALIDATION_INITIATED
    if s == "SOURCE_VALIDATION_FAILED":
        return SAHPI_FUMI_SOURCE_VALIDATION_FAILED
    if s == "SOURCE_VALIDATION_DONE":
        return SAHPI_FUMI_SOURCE_VALIDATION_DONE
    if s == "SOURCE_VALIDATION_CANCELLED":
        return SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED
    if s == "INSTALL_INITIATED":
        return SAHPI_FUMI_INSTALL_INITIATED
    if s == "INSTALL_FAILED_ROLLBACK_NEEDED":
        return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED
    if s == "INSTALL_FAILED_ROLLBACK_INITIATED":
        return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED
    if s == "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE":
        return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE
    if s == "INSTALL_DONE":
        return SAHPI_FUMI_INSTALL_DONE
    if s == "INSTALL_CANCELLED":
        return SAHPI_FUMI_INSTALL_CANCELLED
    if s == "ROLLBACK_INITIATED":
        return SAHPI_FUMI_ROLLBACK_INITIATED
    if s == "ROLLBACK_FAILED":
        return SAHPI_FUMI_ROLLBACK_FAILED
    if s == "ROLLBACK_DONE":
        return SAHPI_FUMI_ROLLBACK_DONE
    if s == "ROLLBACK_CANCELLED":
        return SAHPI_FUMI_ROLLBACK_CANCELLED
    if s == "BACKUP_INITIATED":
        return SAHPI_FUMI_BACKUP_INITIATED
    if s == "BACKUP_FAILED":
        return SAHPI_FUMI_BACKUP_FAILED
    if s == "BACKUP_DONE":
        return SAHPI_FUMI_BACKUP_DONE
    if s == "BACKUP_CANCELLED":
        return SAHPI_FUMI_BACKUP_CANCELLED
    if s == "BANK_COPY_INITIATED":
        return SAHPI_FUMI_BANK_COPY_INITIATED
    if s == "BANK_COPY_FAILED":
        return SAHPI_FUMI_BANK_COPY_FAILED
    if s == "BANK_COPY_DONE":
        return SAHPI_FUMI_BANK_COPY_DONE
    if s == "BANK_COPY_CANCELLED":
        return SAHPI_FUMI_BANK_COPY_CANCELLED
    if s == "TARGET_VERIFY_INITIATED":
        return SAHPI_FUMI_TARGET_VERIFY_INITIATED
    if s == "TARGET_VERIFY_FAILED":
        return SAHPI_FUMI_TARGET_VERIFY_FAILED
    if s == "TARGET_VERIFY_DONE":
        return SAHPI_FUMI_TARGET_VERIFY_DONE
    if s == "TARGET_VERIFY_CANCELLED":
        return SAHPI_FUMI_TARGET_VERIFY_CANCELLED
    if s == "ACTIVATE_INITIATED":
        return SAHPI_FUMI_ACTIVATE_INITIATED
    if s == "ACTIVATE_FAILED_ROLLBACK_NEEDED":
        return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED
    if s == "ACTIVATE_FAILED_ROLLBACK_INITIATED":
        return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED
    if s == "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE":
        return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE
    if s == "ACTIVATE_DONE":
        return SAHPI_FUMI_ACTIVATE_DONE
    if s == "ACTIVATE_CANCELLED":
        return SAHPI_FUMI_ACTIVATE_CANCELLED
    raise ValueError()

#**
# For SaHpiHsIndicatorStateT
#**
def fromSaHpiHsIndicatorStateT( x ):
    if x == SAHPI_HS_INDICATOR_OFF:
        return "FF"
    if x == SAHPI_HS_INDICATOR_ON:
        return "N"
    return repr( x )

def toSaHpiHsIndicatorStateT( s ):
    if s == "FF":
        return SAHPI_HS_INDICATOR_OFF
    if s == "N":
        return SAHPI_HS_INDICATOR_ON
    raise ValueError()

#**
# For SaHpiHsActionT
#**
def fromSaHpiHsActionT( x ):
    if x == SAHPI_HS_ACTION_INSERTION:
        return "INSERTION"
    if x == SAHPI_HS_ACTION_EXTRACTION:
        return "EXTRACTION"
    return repr( x )

def toSaHpiHsActionT( s ):
    if s == "INSERTION":
        return SAHPI_HS_ACTION_INSERTION
    if s == "EXTRACTION":
        return SAHPI_HS_ACTION_EXTRACTION
    raise ValueError()

#**
# For SaHpiHsStateT
#**
def fromSaHpiHsStateT( x ):
    if x == SAHPI_HS_STATE_INACTIVE:
        return "INACTIVE"
    if x == SAHPI_HS_STATE_INSERTION_PENDING:
        return "INSERTION_PENDING"
    if x == SAHPI_HS_STATE_ACTIVE:
        return "ACTIVE"
    if x == SAHPI_HS_STATE_EXTRACTION_PENDING:
        return "EXTRACTION_PENDING"
    if x == SAHPI_HS_STATE_NOT_PRESENT:
        return "NOT_PRESENT"
    return repr( x )

def toSaHpiHsStateT( s ):
    if s == "INACTIVE":
        return SAHPI_HS_STATE_INACTIVE
    if s == "INSERTION_PENDING":
        return SAHPI_HS_STATE_INSERTION_PENDING
    if s == "ACTIVE":
        return SAHPI_HS_STATE_ACTIVE
    if s == "EXTRACTION_PENDING":
        return SAHPI_HS_STATE_EXTRACTION_PENDING
    if s == "NOT_PRESENT":
        return SAHPI_HS_STATE_NOT_PRESENT
    raise ValueError()

#**
# For SaHpiHsCauseOfStateChangeT
#**
def fromSaHpiHsCauseOfStateChangeT( x ):
    if x == SAHPI_HS_CAUSE_AUTO_POLICY:
        return "AUTO_POLICY"
    if x == SAHPI_HS_CAUSE_EXT_SOFTWARE:
        return "EXT_SOFTWARE"
    if x == SAHPI_HS_CAUSE_OPERATOR_INIT:
        return "OPERATOR_INIT"
    if x == SAHPI_HS_CAUSE_USER_UPDATE:
        return "USER_UPDATE"
    if x == SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION:
        return "UNEXPECTED_DEACTIVATION"
    if x == SAHPI_HS_CAUSE_SURPRISE_EXTRACTION:
        return "SURPRISE_EXTRACTION"
    if x == SAHPI_HS_CAUSE_EXTRACTION_UPDATE:
        return "EXTRACTION_UPDATE"
    if x == SAHPI_HS_CAUSE_HARDWARE_FAULT:
        return "HARDWARE_FAULT"
    if x == SAHPI_HS_CAUSE_CONTAINING_FRU:
        return "CONTAINING_FRU"
    if x == SAHPI_HS_CAUSE_UNKNOWN:
        return "UNKNOWN"
    return repr( x )

def toSaHpiHsCauseOfStateChangeT( s ):
    if s == "AUTO_POLICY":
        return SAHPI_HS_CAUSE_AUTO_POLICY
    if s == "EXT_SOFTWARE":
        return SAHPI_HS_CAUSE_EXT_SOFTWARE
    if s == "OPERATOR_INIT":
        return SAHPI_HS_CAUSE_OPERATOR_INIT
    if s == "USER_UPDATE":
        return SAHPI_HS_CAUSE_USER_UPDATE
    if s == "UNEXPECTED_DEACTIVATION":
        return SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION
    if s == "SURPRISE_EXTRACTION":
        return SAHPI_HS_CAUSE_SURPRISE_EXTRACTION
    if s == "EXTRACTION_UPDATE":
        return SAHPI_HS_CAUSE_EXTRACTION_UPDATE
    if s == "HARDWARE_FAULT":
        return SAHPI_HS_CAUSE_HARDWARE_FAULT
    if s == "CONTAINING_FRU":
        return SAHPI_HS_CAUSE_CONTAINING_FRU
    if s == "UNKNOWN":
        return SAHPI_HS_CAUSE_UNKNOWN
    raise ValueError()

#**
# For SaHpiSeverityT
#**
def fromSaHpiSeverityT( x ):
    if x == SAHPI_CRITICAL:
        return "CRITICAL"
    if x == SAHPI_MAJOR:
        return "MAJOR"
    if x == SAHPI_MINOR:
        return "MINOR"
    if x == SAHPI_INFORMATIONAL:
        return "INFORMATIONAL"
    if x == SAHPI_OK:
        return "OK"
    if x == SAHPI_DEBUG:
        return "DEBUG"
    if x == SAHPI_ALL_SEVERITIES:
        return "ALL_SEVERITIES"
    return repr( x )

def toSaHpiSeverityT( s ):
    if s == "CRITICAL":
        return SAHPI_CRITICAL
    if s == "MAJOR":
        return SAHPI_MAJOR
    if s == "MINOR":
        return SAHPI_MINOR
    if s == "INFORMATIONAL":
        return SAHPI_INFORMATIONAL
    if s == "OK":
        return SAHPI_OK
    if s == "DEBUG":
        return SAHPI_DEBUG
    if s == "ALL_SEVERITIES":
        return SAHPI_ALL_SEVERITIES
    raise ValueError()

#**
# For SaHpiResourceEventTypeT
#**
def fromSaHpiResourceEventTypeT( x ):
    if x == SAHPI_RESE_RESOURCE_FAILURE:
        return "FAILURE"
    if x == SAHPI_RESE_RESOURCE_RESTORED:
        return "RESTORED"
    if x == SAHPI_RESE_RESOURCE_ADDED:
        return "ADDED"
    if x == SAHPI_RESE_RESOURCE_REMOVED:
        return "REMOVED"
    if x == SAHPI_RESE_RESOURCE_INACCESSIBLE:
        return "INACCESSIBLE"
    if x == SAHPI_RESE_RESOURCE_UPDATED:
        return "UPDATED"
    return repr( x )

def toSaHpiResourceEventTypeT( s ):
    if s == "FAILURE":
        return SAHPI_RESE_RESOURCE_FAILURE
    if s == "RESTORED":
        return SAHPI_RESE_RESOURCE_RESTORED
    if s == "ADDED":
        return SAHPI_RESE_RESOURCE_ADDED
    if s == "REMOVED":
        return SAHPI_RESE_RESOURCE_REMOVED
    if s == "INACCESSIBLE":
        return SAHPI_RESE_RESOURCE_INACCESSIBLE
    if s == "UPDATED":
        return SAHPI_RESE_RESOURCE_UPDATED
    raise ValueError()

#**
# For SaHpiDomainEventTypeT
#**
def fromSaHpiDomainEventTypeT( x ):
    if x == SAHPI_DOMAIN_REF_ADDED:
        return "ADDED"
    if x == SAHPI_DOMAIN_REF_REMOVED:
        return "REMOVED"
    return repr( x )

def toSaHpiDomainEventTypeT( s ):
    if s == "ADDED":
        return SAHPI_DOMAIN_REF_ADDED
    if s == "REMOVED":
        return SAHPI_DOMAIN_REF_REMOVED
    raise ValueError()

#**
# For SaHpiSwEventTypeT
#**
def fromSaHpiSwEventTypeT( x ):
    if x == SAHPI_HPIE_AUDIT:
        return "AUDIT"
    if x == SAHPI_HPIE_STARTUP:
        return "STARTUP"
    if x == SAHPI_HPIE_OTHER:
        return "OTHER"
    return repr( x )

def toSaHpiSwEventTypeT( s ):
    if s == "AUDIT":
        return SAHPI_HPIE_AUDIT
    if s == "STARTUP":
        return SAHPI_HPIE_STARTUP
    if s == "OTHER":
        return SAHPI_HPIE_OTHER
    raise ValueError()

#**
# For SaHpiEventTypeT
#**
def fromSaHpiEventTypeT( x ):
    if x == SAHPI_ET_RESOURCE:
        return "RESOURCE"
    if x == SAHPI_ET_DOMAIN:
        return "DOMAIN"
    if x == SAHPI_ET_SENSOR:
        return "SENSOR"
    if x == SAHPI_ET_SENSOR_ENABLE_CHANGE:
        return "SENSOR_ENABLE_CHANGE"
    if x == SAHPI_ET_HOTSWAP:
        return "HOTSWAP"
    if x == SAHPI_ET_WATCHDOG:
        return "WATCHDOG"
    if x == SAHPI_ET_HPI_SW:
        return "HPI_SW"
    if x == SAHPI_ET_OEM:
        return "OEM"
    if x == SAHPI_ET_USER:
        return "USER"
    if x == SAHPI_ET_DIMI:
        return "DIMI"
    if x == SAHPI_ET_DIMI_UPDATE:
        return "DIMI_UPDATE"
    if x == SAHPI_ET_FUMI:
        return "FUMI"
    return repr( x )

def toSaHpiEventTypeT( s ):
    if s == "RESOURCE":
        return SAHPI_ET_RESOURCE
    if s == "DOMAIN":
        return SAHPI_ET_DOMAIN
    if s == "SENSOR":
        return SAHPI_ET_SENSOR
    if s == "SENSOR_ENABLE_CHANGE":
        return SAHPI_ET_SENSOR_ENABLE_CHANGE
    if s == "HOTSWAP":
        return SAHPI_ET_HOTSWAP
    if s == "WATCHDOG":
        return SAHPI_ET_WATCHDOG
    if s == "HPI_SW":
        return SAHPI_ET_HPI_SW
    if s == "OEM":
        return SAHPI_ET_OEM
    if s == "USER":
        return SAHPI_ET_USER
    if s == "DIMI":
        return SAHPI_ET_DIMI
    if s == "DIMI_UPDATE":
        return SAHPI_ET_DIMI_UPDATE
    if s == "FUMI":
        return SAHPI_ET_FUMI
    raise ValueError()

#**
# For SaHpiStatusCondTypeT
#**
def fromSaHpiStatusCondTypeT( x ):
    if x == SAHPI_STATUS_COND_TYPE_SENSOR:
        return "SENSOR"
    if x == SAHPI_STATUS_COND_TYPE_RESOURCE:
        return "RESOURCE"
    if x == SAHPI_STATUS_COND_TYPE_OEM:
        return "OEM"
    if x == SAHPI_STATUS_COND_TYPE_USER:
        return "USER"
    return repr( x )

def toSaHpiStatusCondTypeT( s ):
    if s == "SENSOR":
        return SAHPI_STATUS_COND_TYPE_SENSOR
    if s == "RESOURCE":
        return SAHPI_STATUS_COND_TYPE_RESOURCE
    if s == "OEM":
        return SAHPI_STATUS_COND_TYPE_OEM
    if s == "USER":
        return SAHPI_STATUS_COND_TYPE_USER
    raise ValueError()

#**
# For SaHpiAnnunciatorModeT
#**
def fromSaHpiAnnunciatorModeT( x ):
    if x == SAHPI_ANNUNCIATOR_MODE_AUTO:
        return "AUTO"
    if x == SAHPI_ANNUNCIATOR_MODE_USER:
        return "USER"
    if x == SAHPI_ANNUNCIATOR_MODE_SHARED:
        return "SHARED"
    return repr( x )

def toSaHpiAnnunciatorModeT( s ):
    if s == "AUTO":
        return SAHPI_ANNUNCIATOR_MODE_AUTO
    if s == "USER":
        return SAHPI_ANNUNCIATOR_MODE_USER
    if s == "SHARED":
        return SAHPI_ANNUNCIATOR_MODE_SHARED
    raise ValueError()

#**
# For SaHpiAnnunciatorTypeT
#**
def fromSaHpiAnnunciatorTypeT( x ):
    if x == SAHPI_ANNUNCIATOR_TYPE_LED:
        return "LED"
    if x == SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE:
        return "DRY_CONTACT_CLOSURE"
    if x == SAHPI_ANNUNCIATOR_TYPE_AUDIBLE:
        return "AUDIBLE"
    if x == SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY:
        return "LCD_DISPLAY"
    if x == SAHPI_ANNUNCIATOR_TYPE_MESSAGE:
        return "MESSAGE"
    if x == SAHPI_ANNUNCIATOR_TYPE_COMPOSITE:
        return "COMPOSITE"
    if x == SAHPI_ANNUNCIATOR_TYPE_OEM:
        return "OEM"
    return repr( x )

def toSaHpiAnnunciatorTypeT( s ):
    if s == "LED":
        return SAHPI_ANNUNCIATOR_TYPE_LED
    if s == "DRY_CONTACT_CLOSURE":
        return SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE
    if s == "AUDIBLE":
        return SAHPI_ANNUNCIATOR_TYPE_AUDIBLE
    if s == "LCD_DISPLAY":
        return SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY
    if s == "MESSAGE":
        return SAHPI_ANNUNCIATOR_TYPE_MESSAGE
    if s == "COMPOSITE":
        return SAHPI_ANNUNCIATOR_TYPE_COMPOSITE
    if s == "OEM":
        return SAHPI_ANNUNCIATOR_TYPE_OEM
    raise ValueError()

#**
# For SaHpiRdrTypeT
#**
def fromSaHpiRdrTypeT( x ):
    if x == SAHPI_NO_RECORD:
        return "NO_RECORD"
    if x == SAHPI_CTRL_RDR:
        return "CTRL_RDR"
    if x == SAHPI_SENSOR_RDR:
        return "SENSOR_RDR"
    if x == SAHPI_INVENTORY_RDR:
        return "INVENTORY_RDR"
    if x == SAHPI_WATCHDOG_RDR:
        return "WATCHDOG_RDR"
    if x == SAHPI_ANNUNCIATOR_RDR:
        return "ANNUNCIATOR_RDR"
    if x == SAHPI_DIMI_RDR:
        return "DIMI_RDR"
    if x == SAHPI_FUMI_RDR:
        return "FUMI_RDR"
    return repr( x )

def toSaHpiRdrTypeT( s ):
    if s == "NO_RECORD":
        return SAHPI_NO_RECORD
    if s == "CTRL_RDR":
        return SAHPI_CTRL_RDR
    if s == "SENSOR_RDR":
        return SAHPI_SENSOR_RDR
    if s == "INVENTORY_RDR":
        return SAHPI_INVENTORY_RDR
    if s == "WATCHDOG_RDR":
        return SAHPI_WATCHDOG_RDR
    if s == "ANNUNCIATOR_RDR":
        return SAHPI_ANNUNCIATOR_RDR
    if s == "DIMI_RDR":
        return SAHPI_DIMI_RDR
    if s == "FUMI_RDR":
        return SAHPI_FUMI_RDR
    raise ValueError()

#**
# For SaHpiParmActionT
#**
def fromSaHpiParmActionT( x ):
    if x == SAHPI_DEFAULT_PARM:
        return "DEFAULT_PARM"
    if x == SAHPI_SAVE_PARM:
        return "SAVE_PARM"
    if x == SAHPI_RESTORE_PARM:
        return "RESTORE_PARM"
    return repr( x )

def toSaHpiParmActionT( s ):
    if s == "DEFAULT_PARM":
        return SAHPI_DEFAULT_PARM
    if s == "SAVE_PARM":
        return SAHPI_SAVE_PARM
    if s == "RESTORE_PARM":
        return SAHPI_RESTORE_PARM
    raise ValueError()

#**
# For SaHpiResetActionT
#**
def fromSaHpiResetActionT( x ):
    if x == SAHPI_COLD_RESET:
        return "COLD_RESET"
    if x == SAHPI_WARM_RESET:
        return "WARM_RESET"
    if x == SAHPI_RESET_ASSERT:
        return "RESET_ASSERT"
    if x == SAHPI_RESET_DEASSERT:
        return "RESET_DEASSERT"
    return repr( x )

def toSaHpiResetActionT( s ):
    if s == "COLD_RESET":
        return SAHPI_COLD_RESET
    if s == "WARM_RESET":
        return SAHPI_WARM_RESET
    if s == "RESET_ASSERT":
        return SAHPI_RESET_ASSERT
    if s == "RESET_DEASSERT":
        return SAHPI_RESET_DEASSERT
    raise ValueError()

#**
# For SaHpiPowerStateT
#**
def fromSaHpiPowerStateT( x ):
    if x == SAHPI_POWER_OFF:
        return "OFF"
    if x == SAHPI_POWER_ON:
        return "ON"
    if x == SAHPI_POWER_CYCLE:
        return "CYCLE"
    return repr( x )

def toSaHpiPowerStateT( s ):
    if s == "OFF":
        return SAHPI_POWER_OFF
    if s == "ON":
        return SAHPI_POWER_ON
    if s == "CYCLE":
        return SAHPI_POWER_CYCLE
    raise ValueError()

#**
# For SaHpiEventLogOverflowActionT
#**
def fromSaHpiEventLogOverflowActionT( x ):
    if x == SAHPI_EL_OVERFLOW_DROP:
        return "DROP"
    if x == SAHPI_EL_OVERFLOW_OVERWRITE:
        return "OVERWRITE"
    return repr( x )

def toSaHpiEventLogOverflowActionT( s ):
    if s == "DROP":
        return SAHPI_EL_OVERFLOW_DROP
    if s == "OVERWRITE":
        return SAHPI_EL_OVERFLOW_OVERWRITE
    raise ValueError()

#**
# For AtcaHpiLedColorT
#**
def fromAtcaHpiLedColorT( x ):
    if x == ATCAHPI_LED_COLOR_RESERVED:
        return "RESERVED"
    if x == ATCAHPI_LED_COLOR_BLUE:
        return "BLUE"
    if x == ATCAHPI_LED_COLOR_RED:
        return "RED"
    if x == ATCAHPI_LED_COLOR_GREEN:
        return "GREEN"
    if x == ATCAHPI_LED_COLOR_AMBER:
        return "AMBER"
    if x == ATCAHPI_LED_COLOR_ORANGE:
        return "ORANGE"
    if x == ATCAHPI_LED_COLOR_WHITE:
        return "WHITE"
    if x == ATCAHPI_LED_COLOR_NO_CHANGE:
        return "NO_CHANGE"
    if x == ATCAHPI_LED_COLOR_USE_DEFAULT:
        return "USE_DEFAULT"
    return repr( x )

def toAtcaHpiLedColorT( s ):
    if s == "RESERVED":
        return ATCAHPI_LED_COLOR_RESERVED
    if s == "BLUE":
        return ATCAHPI_LED_COLOR_BLUE
    if s == "RED":
        return ATCAHPI_LED_COLOR_RED
    if s == "GREEN":
        return ATCAHPI_LED_COLOR_GREEN
    if s == "AMBER":
        return ATCAHPI_LED_COLOR_AMBER
    if s == "ORANGE":
        return ATCAHPI_LED_COLOR_ORANGE
    if s == "WHITE":
        return ATCAHPI_LED_COLOR_WHITE
    if s == "NO_CHANGE":
        return ATCAHPI_LED_COLOR_NO_CHANGE
    if s == "USE_DEFAULT":
        return ATCAHPI_LED_COLOR_USE_DEFAULT
    raise ValueError()

#**
# For AtcaHpiResourceLedModeT
#**
def fromAtcaHpiResourceLedModeT( x ):
    if x == ATCAHPI_LED_AUTO:
        return "AUTO"
    if x == ATCAHPI_LED_MANUAL:
        return "MANUAL"
    if x == ATCAHPI_LED_LAMP_TEST:
        return "LAMP_TEST"
    return repr( x )

def toAtcaHpiResourceLedModeT( s ):
    if s == "AUTO":
        return ATCAHPI_LED_AUTO
    if s == "MANUAL":
        return ATCAHPI_LED_MANUAL
    if s == "LAMP_TEST":
        return ATCAHPI_LED_LAMP_TEST
    raise ValueError()

#**
# For AtcaHpiLedBrSupportT
#**
def fromAtcaHpiLedBrSupportT( x ):
    if x == ATCAHPI_LED_BR_SUPPORTED:
        return "SUPPORTED"
    if x == ATCAHPI_LED_BR_NOT_SUPPORTED:
        return "NOT_SUPPORTED"
    if x == ATCAHPI_LED_BR_UNKNOWN:
        return "UNKNOWN"
    return repr( x )

def toAtcaHpiLedBrSupportT( s ):
    if s == "SUPPORTED":
        return ATCAHPI_LED_BR_SUPPORTED
    if s == "NOT_SUPPORTED":
        return ATCAHPI_LED_BR_NOT_SUPPORTED
    if s == "UNKNOWN":
        return ATCAHPI_LED_BR_UNKNOWN
    raise ValueError()

#**
# For XtcaHpiLedColorT
#**
def fromXtcaHpiLedColorT( x ):
    if x == XTCAHPI_LED_COLOR_RESERVED:
        return "RESERVED"
    if x == XTCAHPI_LED_COLOR_BLUE:
        return "BLUE"
    if x == XTCAHPI_LED_COLOR_RED:
        return "RED"
    if x == XTCAHPI_LED_COLOR_GREEN:
        return "GREEN"
    if x == XTCAHPI_LED_COLOR_AMBER:
        return "AMBER"
    if x == XTCAHPI_LED_COLOR_ORANGE:
        return "ORANGE"
    if x == XTCAHPI_LED_COLOR_WHITE:
        return "WHITE"
    if x == XTCAHPI_LED_COLOR_NO_CHANGE:
        return "NO_CHANGE"
    if x == XTCAHPI_LED_COLOR_USE_DEFAULT:
        return "USE_DEFAULT"
    return repr( x )

def toXtcaHpiLedColorT( s ):
    if s == "RESERVED":
        return XTCAHPI_LED_COLOR_RESERVED
    if s == "BLUE":
        return XTCAHPI_LED_COLOR_BLUE
    if s == "RED":
        return XTCAHPI_LED_COLOR_RED
    if s == "GREEN":
        return XTCAHPI_LED_COLOR_GREEN
    if s == "AMBER":
        return XTCAHPI_LED_COLOR_AMBER
    if s == "ORANGE":
        return XTCAHPI_LED_COLOR_ORANGE
    if s == "WHITE":
        return XTCAHPI_LED_COLOR_WHITE
    if s == "NO_CHANGE":
        return XTCAHPI_LED_COLOR_NO_CHANGE
    if s == "USE_DEFAULT":
        return XTCAHPI_LED_COLOR_USE_DEFAULT
    raise ValueError()

#**
# For XtcaHpiResourceLedModeT
#**
def fromXtcaHpiResourceLedModeT( x ):
    if x == XTCAHPI_LED_AUTO:
        return "AUTO"
    if x == XTCAHPI_LED_MANUAL:
        return "MANUAL"
    if x == XTCAHPI_LED_LAMP_TEST:
        return "LAMP_TEST"
    return repr( x )

def toXtcaHpiResourceLedModeT( s ):
    if s == "AUTO":
        return XTCAHPI_LED_AUTO
    if s == "MANUAL":
        return XTCAHPI_LED_MANUAL
    if s == "LAMP_TEST":
        return XTCAHPI_LED_LAMP_TEST
    raise ValueError()

#**
# For XtcaHpiLedBrSupportT
#**
def fromXtcaHpiLedBrSupportT( x ):
    if x == XTCAHPI_LED_BR_SUPPORTED:
        return "SUPPORTED"
    if x == XTCAHPI_LED_BR_NOT_SUPPORTED:
        return "NOT_SUPPORTED"
    if x == XTCAHPI_LED_BR_UNKNOWN:
        return "UNKNOWN"
    return repr( x )

def toXtcaHpiLedBrSupportT( s ):
    if s == "SUPPORTED":
        return XTCAHPI_LED_BR_SUPPORTED
    if s == "NOT_SUPPORTED":
        return XTCAHPI_LED_BR_NOT_SUPPORTED
    if s == "UNKNOWN":
        return XTCAHPI_LED_BR_UNKNOWN
    raise ValueError()

#**
# For SaErrorT
#**
def fromSaErrorT( x ):
    if x == SA_ERR_HPI_OK:
        return "OK"
    if x == SA_ERR_HPI_ERROR:
        return "ERROR"
    if x == SA_ERR_HPI_UNSUPPORTED_API:
        return "UNSUPPORTED_API"
    if x == SA_ERR_HPI_BUSY:
        return "BUSY"
    if x == SA_ERR_HPI_INTERNAL_ERROR:
        return "INTERNAL_ERROR"
    if x == SA_ERR_HPI_INVALID_CMD:
        return "INVALID_CMD"
    if x == SA_ERR_HPI_TIMEOUT:
        return "TIMEOUT"
    if x == SA_ERR_HPI_OUT_OF_SPACE:
        return "OUT_OF_SPACE"
    if x == SA_ERR_HPI_OUT_OF_MEMORY:
        return "OUT_OF_MEMORY"
    if x == SA_ERR_HPI_INVALID_PARAMS:
        return "INVALID_PARAMS"
    if x == SA_ERR_HPI_INVALID_DATA:
        return "INVALID_DATA"
    if x == SA_ERR_HPI_NOT_PRESENT:
        return "NOT_PRESENT"
    if x == SA_ERR_HPI_NO_RESPONSE:
        return "NO_RESPONSE"
    if x == SA_ERR_HPI_DUPLICATE:
        return "DUPLICATE"
    if x == SA_ERR_HPI_INVALID_SESSION:
        return "INVALID_SESSION"
    if x == SA_ERR_HPI_INVALID_DOMAIN:
        return "INVALID_DOMAIN"
    if x == SA_ERR_HPI_INVALID_RESOURCE:
        return "INVALID_RESOURCE"
    if x == SA_ERR_HPI_INVALID_REQUEST:
        return "INVALID_REQUEST"
    if x == SA_ERR_HPI_ENTITY_NOT_PRESENT:
        return "ENTITY_NOT_PRESENT"
    if x == SA_ERR_HPI_READ_ONLY:
        return "READ_ONLY"
    if x == SA_ERR_HPI_CAPABILITY:
        return "CAPABILITY"
    if x == SA_ERR_HPI_UNKNOWN:
        return "UNKNOWN"
    if x == SA_ERR_HPI_INVALID_STATE:
        return "INVALID_STATE"
    if x == SA_ERR_HPI_UNSUPPORTED_PARAMS:
        return "UNSUPPORTED_PARAMS"
    return repr( x )

def toSaErrorT( s ):
    if s == "OK":
        return SA_ERR_HPI_OK
    if s == "ERROR":
        return SA_ERR_HPI_ERROR
    if s == "UNSUPPORTED_API":
        return SA_ERR_HPI_UNSUPPORTED_API
    if s == "BUSY":
        return SA_ERR_HPI_BUSY
    if s == "INTERNAL_ERROR":
        return SA_ERR_HPI_INTERNAL_ERROR
    if s == "INVALID_CMD":
        return SA_ERR_HPI_INVALID_CMD
    if s == "TIMEOUT":
        return SA_ERR_HPI_TIMEOUT
    if s == "OUT_OF_SPACE":
        return SA_ERR_HPI_OUT_OF_SPACE
    if s == "OUT_OF_MEMORY":
        return SA_ERR_HPI_OUT_OF_MEMORY
    if s == "INVALID_PARAMS":
        return SA_ERR_HPI_INVALID_PARAMS
    if s == "INVALID_DATA":
        return SA_ERR_HPI_INVALID_DATA
    if s == "NOT_PRESENT":
        return SA_ERR_HPI_NOT_PRESENT
    if s == "NO_RESPONSE":
        return SA_ERR_HPI_NO_RESPONSE
    if s == "DUPLICATE":
        return SA_ERR_HPI_DUPLICATE
    if s == "INVALID_SESSION":
        return SA_ERR_HPI_INVALID_SESSION
    if s == "INVALID_DOMAIN":
        return SA_ERR_HPI_INVALID_DOMAIN
    if s == "INVALID_RESOURCE":
        return SA_ERR_HPI_INVALID_RESOURCE
    if s == "INVALID_REQUEST":
        return SA_ERR_HPI_INVALID_REQUEST
    if s == "ENTITY_NOT_PRESENT":
        return SA_ERR_HPI_ENTITY_NOT_PRESENT
    if s == "READ_ONLY":
        return SA_ERR_HPI_READ_ONLY
    if s == "CAPABILITY":
        return SA_ERR_HPI_CAPABILITY
    if s == "UNKNOWN":
        return SA_ERR_HPI_UNKNOWN
    if s == "INVALID_STATE":
        return SA_ERR_HPI_INVALID_STATE
    if s == "UNSUPPORTED_PARAMS":
        return SA_ERR_HPI_UNSUPPORTED_PARAMS
    raise ValueError()

#**
# For SaHpiEventCategoryT
#**
def fromSaHpiEventCategoryT( x ):
    if x == SAHPI_EC_UNSPECIFIED:
        return "UNSPECIFIED"
    if x == SAHPI_EC_THRESHOLD:
        return "THRESHOLD"
    if x == SAHPI_EC_USAGE:
        return "USAGE"
    if x == SAHPI_EC_STATE:
        return "STATE"
    if x == SAHPI_EC_PRED_FAIL:
        return "PRED_FAIL"
    if x == SAHPI_EC_LIMIT:
        return "LIMIT"
    if x == SAHPI_EC_PERFORMANCE:
        return "PERFORMANCE"
    if x == SAHPI_EC_SEVERITY:
        return "SEVERITY"
    if x == SAHPI_EC_PRESENCE:
        return "PRESENCE"
    if x == SAHPI_EC_ENABLE:
        return "ENABLE"
    if x == SAHPI_EC_AVAILABILITY:
        return "AVAILABILITY"
    if x == SAHPI_EC_REDUNDANCY:
        return "REDUNDANCY"
    if x == SAHPI_EC_SENSOR_SPECIFIC:
        return "SENSOR_SPECIFIC"
    if x == SAHPI_EC_GENERIC:
        return "GENERIC"
    return repr( x )

def toSaHpiEventCategoryT( s ):
    if s == "UNSPECIFIED":
        return SAHPI_EC_UNSPECIFIED
    if s == "THRESHOLD":
        return SAHPI_EC_THRESHOLD
    if s == "USAGE":
        return SAHPI_EC_USAGE
    if s == "STATE":
        return SAHPI_EC_STATE
    if s == "PRED_FAIL":
        return SAHPI_EC_PRED_FAIL
    if s == "LIMIT":
        return SAHPI_EC_LIMIT
    if s == "PERFORMANCE":
        return SAHPI_EC_PERFORMANCE
    if s == "SEVERITY":
        return SAHPI_EC_SEVERITY
    if s == "PRESENCE":
        return SAHPI_EC_PRESENCE
    if s == "ENABLE":
        return SAHPI_EC_ENABLE
    if s == "AVAILABILITY":
        return SAHPI_EC_AVAILABILITY
    if s == "REDUNDANCY":
        return SAHPI_EC_REDUNDANCY
    if s == "SENSOR_SPECIFIC":
        return SAHPI_EC_SENSOR_SPECIFIC
    if s == "GENERIC":
        return SAHPI_EC_GENERIC
    raise ValueError()

