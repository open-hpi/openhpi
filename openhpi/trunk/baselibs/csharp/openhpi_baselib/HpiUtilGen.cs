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
 * HPI Utility Functions (auto-generated)
 *********************************************************/
public static partial class HpiUtil
{
    /**********************************************************
     * Check Functions for HPI Complex Data Types
     *********************************************************/

    /**
     * Check function for HPI struct SaHpiTextBufferT
     */
    public static bool Check( SaHpiTextBufferT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Data == null ) {
            return false;
        }
        if ( x.Data.Length != HpiConst.SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiEntityT
     */
    public static bool Check( SaHpiEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiEntityPathT
     */
    public static bool Check( SaHpiEntityPathT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Entry == null ) {
            return false;
        }
        if ( x.Entry.Length != HpiConst.SAHPI_MAX_ENTITY_PATH ) {
            return false;
        }
        for ( int i = 0; i < HpiConst.SAHPI_MAX_ENTITY_PATH; ++i ) {
            if ( !Check( x.Entry[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiSensorReadingUnionT
     */
    public static bool Check( SaHpiSensorReadingUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            if ( x.SensorBuffer == null ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            if ( x.SensorBuffer.Length != HpiConst.SAHPI_SENSOR_BUFFER_LENGTH ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorReadingT
     */
    public static bool Check( SaHpiSensorReadingT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Value, x.Type ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorThresholdsT
     */
    public static bool Check( SaHpiSensorThresholdsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.LowCritical ) ) {
            return false;
        }
        if ( !Check( x.LowMajor ) ) {
            return false;
        }
        if ( !Check( x.LowMinor ) ) {
            return false;
        }
        if ( !Check( x.UpCritical ) ) {
            return false;
        }
        if ( !Check( x.UpMajor ) ) {
            return false;
        }
        if ( !Check( x.UpMinor ) ) {
            return false;
        }
        if ( !Check( x.PosThdHysteresis ) ) {
            return false;
        }
        if ( !Check( x.NegThdHysteresis ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorRangeT
     */
    public static bool Check( SaHpiSensorRangeT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Max ) ) {
            return false;
        }
        if ( !Check( x.Min ) ) {
            return false;
        }
        if ( !Check( x.Nominal ) ) {
            return false;
        }
        if ( !Check( x.NormalMax ) ) {
            return false;
        }
        if ( !Check( x.NormalMin ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorDataFormatT
     */
    public static bool Check( SaHpiSensorDataFormatT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Range ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorThdDefnT
     */
    public static bool Check( SaHpiSensorThdDefnT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorRecT
     */
    public static bool Check( SaHpiSensorRecT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.DataFormat ) ) {
            return false;
        }
        if ( !Check( x.ThresholdDefn ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlStateStreamT
     */
    public static bool Check( SaHpiCtrlStateStreamT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Stream == null ) {
            return false;
        }
        if ( x.Stream.Length != HpiConst.SAHPI_CTRL_MAX_STREAM_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlStateTextT
     */
    public static bool Check( SaHpiCtrlStateTextT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Text ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlStateOemT
     */
    public static bool Check( SaHpiCtrlStateOemT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Body == null ) {
            return false;
        }
        if ( x.Body.Length != HpiConst.SAHPI_CTRL_MAX_OEM_BODY_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiCtrlStateUnionT
     */
    public static bool Check( SaHpiCtrlStateUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            if ( !Check( x.Stream ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            if ( !Check( x.Text ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            if ( !Check( x.Oem ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlStateT
     */
    public static bool Check( SaHpiCtrlStateT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.StateUnion, x.Type ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecDigitalT
     */
    public static bool Check( SaHpiCtrlRecDigitalT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecDiscreteT
     */
    public static bool Check( SaHpiCtrlRecDiscreteT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecAnalogT
     */
    public static bool Check( SaHpiCtrlRecAnalogT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecStreamT
     */
    public static bool Check( SaHpiCtrlRecStreamT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecTextT
     */
    public static bool Check( SaHpiCtrlRecTextT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecOemT
     */
    public static bool Check( SaHpiCtrlRecOemT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ConfigData == null ) {
            return false;
        }
        if ( x.ConfigData.Length != HpiConst.SAHPI_CTRL_OEM_CONFIG_LENGTH ) {
            return false;
        }
        if ( !Check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiCtrlRecUnionT
     */
    public static bool Check( SaHpiCtrlRecUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DIGITAL ) {
            if ( !Check( x.Digital ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_DISCRETE ) {
            if ( !Check( x.Discrete ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_ANALOG ) {
            if ( !Check( x.Analog ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_STREAM ) {
            if ( !Check( x.Stream ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_TEXT ) {
            if ( !Check( x.Text ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_CTRL_TYPE_OEM ) {
            if ( !Check( x.Oem ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlDefaultModeT
     */
    public static bool Check( SaHpiCtrlDefaultModeT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiCtrlRecT
     */
    public static bool Check( SaHpiCtrlRecT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.TypeUnion, x.Type ) ) {
            return false;
        }
        if ( !Check( x.DefaultMode ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiIdrFieldT
     */
    public static bool Check( SaHpiIdrFieldT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Field ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiIdrAreaHeaderT
     */
    public static bool Check( SaHpiIdrAreaHeaderT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiIdrInfoT
     */
    public static bool Check( SaHpiIdrInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiInventoryRecT
     */
    public static bool Check( SaHpiInventoryRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiWatchdogT
     */
    public static bool Check( SaHpiWatchdogT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiWatchdogRecT
     */
    public static bool Check( SaHpiWatchdogRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiTestAffectedEntityT
     */
    public static bool Check( SaHpiDimiTestAffectedEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.EntityImpacted ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiTestResultsT
     */
    public static bool Check( SaHpiDimiTestResultsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.TestResultString ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiDimiTestParamValueT
     */
    public static bool Check( SaHpiDimiTestParamValueT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            if ( !Check( x.paramtext ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiDimiTestParameterValueUnionT
     */
    public static bool Check( SaHpiDimiTestParameterValueUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiTestParamsDefinitionT
     */
    public static bool Check( SaHpiDimiTestParamsDefinitionT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ParamName == null ) {
            return false;
        }
        if ( x.ParamName.Length != HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN ) {
            return false;
        }
        if ( !Check( x.ParamInfo ) ) {
            return false;
        }
        if ( !Check( x.MinValue, x.ParamType ) ) {
            return false;
        }
        if ( !Check( x.MaxValue, x.ParamType ) ) {
            return false;
        }
        if ( !Check( x.DefaultParam, x.ParamType ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiTestT
     */
    public static bool Check( SaHpiDimiTestT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.TestName ) ) {
            return false;
        }
        if ( x.EntitiesImpacted == null ) {
            return false;
        }
        if ( x.EntitiesImpacted.Length != HpiConst.SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ) {
            return false;
        }
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            if ( !Check( x.EntitiesImpacted[i] ) ) {
                return false;
            }
        }
        if ( !Check( x.ServiceOS ) ) {
            return false;
        }
        if ( x.TestParameters == null ) {
            return false;
        }
        if ( x.TestParameters.Length != HpiConst.SAHPI_DIMITEST_MAX_PARAMETERS ) {
            return false;
        }
        for ( int i = 0; i < HpiConst.SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            if ( !Check( x.TestParameters[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiTestVariableParamsT
     */
    public static bool Check( SaHpiDimiTestVariableParamsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ParamName == null ) {
            return false;
        }
        if ( x.ParamName.Length != HpiConst.SAHPI_DIMITEST_PARAM_NAME_LEN ) {
            return false;
        }
        if ( !Check( x.Value, x.ParamType ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiInfoT
     */
    public static bool Check( SaHpiDimiInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiRecT
     */
    public static bool Check( SaHpiDimiRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiSafDefinedSpecInfoT
     */
    public static bool Check( SaHpiFumiSafDefinedSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiOemDefinedSpecInfoT
     */
    public static bool Check( SaHpiFumiOemDefinedSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Body == null ) {
            return false;
        }
        if ( x.Body.Length != HpiConst.SAHPI_FUMI_MAX_OEM_BODY_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiFumiSpecInfoTypeUnionT
     */
    public static bool Check( SaHpiFumiSpecInfoTypeUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            if ( !Check( x.SafDefined ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            if ( !Check( x.OemDefined ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiSpecInfoT
     */
    public static bool Check( SaHpiFumiSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.SpecInfoTypeUnion, x.SpecInfoType ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiFirmwareInstanceInfoT
     */
    public static bool Check( SaHpiFumiFirmwareInstanceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Identifier ) ) {
            return false;
        }
        if ( !Check( x.Description ) ) {
            return false;
        }
        if ( !Check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiImpactedEntityT
     */
    public static bool Check( SaHpiFumiImpactedEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.ImpactedEntity ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiServiceImpactDataT
     */
    public static bool Check( SaHpiFumiServiceImpactDataT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ImpactedEntities == null ) {
            return false;
        }
        if ( x.ImpactedEntities.Length != HpiConst.SAHPI_FUMI_MAX_ENTITIES_IMPACTED ) {
            return false;
        }
        for ( int i = 0; i < HpiConst.SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            if ( !Check( x.ImpactedEntities[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiSourceInfoT
     */
    public static bool Check( SaHpiFumiSourceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.SourceUri ) ) {
            return false;
        }
        if ( !Check( x.Identifier ) ) {
            return false;
        }
        if ( !Check( x.Description ) ) {
            return false;
        }
        if ( !Check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiComponentInfoT
     */
    public static bool Check( SaHpiFumiComponentInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.MainFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiBankInfoT
     */
    public static bool Check( SaHpiFumiBankInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Identifier ) ) {
            return false;
        }
        if ( !Check( x.Description ) ) {
            return false;
        }
        if ( !Check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiLogicalBankInfoT
     */
    public static bool Check( SaHpiFumiLogicalBankInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.PendingFwInstance ) ) {
            return false;
        }
        if ( !Check( x.RollbackFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiLogicalComponentInfoT
     */
    public static bool Check( SaHpiFumiLogicalComponentInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.PendingFwInstance ) ) {
            return false;
        }
        if ( !Check( x.RollbackFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiRecT
     */
    public static bool Check( SaHpiFumiRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiResourceEventT
     */
    public static bool Check( SaHpiResourceEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDomainEventT
     */
    public static bool Check( SaHpiDomainEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorEventT
     */
    public static bool Check( SaHpiSensorEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.TriggerReading ) ) {
            return false;
        }
        if ( !Check( x.TriggerThreshold ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiSensorEnableChangeEventT
     */
    public static bool Check( SaHpiSensorEnableChangeEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiHotSwapEventT
     */
    public static bool Check( SaHpiHotSwapEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiWatchdogEventT
     */
    public static bool Check( SaHpiWatchdogEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiHpiSwEventT
     */
    public static bool Check( SaHpiHpiSwEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.EventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiOemEventT
     */
    public static bool Check( SaHpiOemEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.OemEventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiUserEventT
     */
    public static bool Check( SaHpiUserEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.UserEventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiEventT
     */
    public static bool Check( SaHpiDimiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDimiUpdateEventT
     */
    public static bool Check( SaHpiDimiUpdateEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiFumiEventT
     */
    public static bool Check( SaHpiFumiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiEventUnionT
     */
    public static bool Check( SaHpiEventUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_ET_RESOURCE ) {
            if ( !Check( x.ResourceEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_DOMAIN ) {
            if ( !Check( x.DomainEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR ) {
            if ( !Check( x.SensorEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            if ( !Check( x.SensorEnableChangeEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_HOTSWAP ) {
            if ( !Check( x.HotSwapEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_WATCHDOG ) {
            if ( !Check( x.WatchdogEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_HPI_SW ) {
            if ( !Check( x.HpiSwEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_OEM ) {
            if ( !Check( x.OemEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_USER ) {
            if ( !Check( x.UserEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI ) {
            if ( !Check( x.DimiEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_DIMI_UPDATE ) {
            if ( !Check( x.DimiUpdateEvent ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ET_FUMI ) {
            if ( !Check( x.FumiEvent ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiEventT
     */
    public static bool Check( SaHpiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.EventDataUnion, x.EventType ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiNameT
     */
    public static bool Check( SaHpiNameT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Value == null ) {
            return false;
        }
        if ( x.Value.Length != HpiConst.SA_HPI_MAX_NAME_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiConditionT
     */
    public static bool Check( SaHpiConditionT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Entity ) ) {
            return false;
        }
        if ( !Check( x.Name ) ) {
            return false;
        }
        if ( !Check( x.Data ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiAnnouncementT
     */
    public static bool Check( SaHpiAnnouncementT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.StatusCond ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiAnnunciatorRecT
     */
    public static bool Check( SaHpiAnnunciatorRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI union SaHpiRdrTypeUnionT
     */
    public static bool Check( SaHpiRdrTypeUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == HpiConst.SAHPI_CTRL_RDR ) {
            if ( !Check( x.CtrlRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_SENSOR_RDR ) {
            if ( !Check( x.SensorRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_INVENTORY_RDR ) {
            if ( !Check( x.InventoryRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_WATCHDOG_RDR ) {
            if ( !Check( x.WatchdogRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_ANNUNCIATOR_RDR ) {
            if ( !Check( x.AnnunciatorRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_DIMI_RDR ) {
            if ( !Check( x.DimiRec ) ) {
                return false;
            }
        }
        if ( mod == HpiConst.SAHPI_FUMI_RDR ) {
            if ( !Check( x.FumiRec ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiRdrT
     */
    public static bool Check( SaHpiRdrT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Entity ) ) {
            return false;
        }
        if ( !Check( x.RdrTypeUnion, x.RdrType ) ) {
            return false;
        }
        if ( !Check( x.IdString ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiLoadIdT
     */
    public static bool Check( SaHpiLoadIdT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.LoadName ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiResourceInfoT
     */
    public static bool Check( SaHpiResourceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Guid == null ) {
            return false;
        }
        if ( x.Guid.Length != HpiConst.SAHPI_GUID_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiRptEntryT
     */
    public static bool Check( SaHpiRptEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.ResourceInfo ) ) {
            return false;
        }
        if ( !Check( x.ResourceEntity ) ) {
            return false;
        }
        if ( !Check( x.ResourceTag ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDomainInfoT
     */
    public static bool Check( SaHpiDomainInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.DomainTag ) ) {
            return false;
        }
        if ( x.Guid == null ) {
            return false;
        }
        if ( x.Guid.Length != HpiConst.SAHPI_GUID_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiDrtEntryT
     */
    public static bool Check( SaHpiDrtEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiAlarmT
     */
    public static bool Check( SaHpiAlarmT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.AlarmCond ) ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiEventLogInfoT
     */
    public static bool Check( SaHpiEventLogInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * Check function for HPI struct SaHpiEventLogEntryT
     */
    public static bool Check( SaHpiEventLogEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !Check( x.Event ) ) {
            return false;
        }
        return true;
    }


    /**********************************************************
     * Lookups for enums, errors, event categories...
     * Value -> Name (FromXXX)
     * Name -> Value (ToXXX)
     * NB: ToXXX throws FormatException if lookup fails
     *********************************************************/
    /**
     * For SaHpiLanguageT
     */
    public static string FromSaHpiLanguageT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_LANG_UNDEF:
                return "UNDEF";
            case HpiConst.SAHPI_LANG_AFAR:
                return "AFAR";
            case HpiConst.SAHPI_LANG_ABKHAZIAN:
                return "ABKHAZIAN";
            case HpiConst.SAHPI_LANG_AFRIKAANS:
                return "AFRIKAANS";
            case HpiConst.SAHPI_LANG_AMHARIC:
                return "AMHARIC";
            case HpiConst.SAHPI_LANG_ARABIC:
                return "ARABIC";
            case HpiConst.SAHPI_LANG_ASSAMESE:
                return "ASSAMESE";
            case HpiConst.SAHPI_LANG_AYMARA:
                return "AYMARA";
            case HpiConst.SAHPI_LANG_AZERBAIJANI:
                return "AZERBAIJANI";
            case HpiConst.SAHPI_LANG_BASHKIR:
                return "BASHKIR";
            case HpiConst.SAHPI_LANG_BYELORUSSIAN:
                return "BYELORUSSIAN";
            case HpiConst.SAHPI_LANG_BULGARIAN:
                return "BULGARIAN";
            case HpiConst.SAHPI_LANG_BIHARI:
                return "BIHARI";
            case HpiConst.SAHPI_LANG_BISLAMA:
                return "BISLAMA";
            case HpiConst.SAHPI_LANG_BENGALI:
                return "BENGALI";
            case HpiConst.SAHPI_LANG_TIBETAN:
                return "TIBETAN";
            case HpiConst.SAHPI_LANG_BRETON:
                return "BRETON";
            case HpiConst.SAHPI_LANG_CATALAN:
                return "CATALAN";
            case HpiConst.SAHPI_LANG_CORSICAN:
                return "CORSICAN";
            case HpiConst.SAHPI_LANG_CZECH:
                return "CZECH";
            case HpiConst.SAHPI_LANG_WELSH:
                return "WELSH";
            case HpiConst.SAHPI_LANG_DANISH:
                return "DANISH";
            case HpiConst.SAHPI_LANG_GERMAN:
                return "GERMAN";
            case HpiConst.SAHPI_LANG_BHUTANI:
                return "BHUTANI";
            case HpiConst.SAHPI_LANG_GREEK:
                return "GREEK";
            case HpiConst.SAHPI_LANG_ENGLISH:
                return "ENGLISH";
            case HpiConst.SAHPI_LANG_ESPERANTO:
                return "ESPERANTO";
            case HpiConst.SAHPI_LANG_SPANISH:
                return "SPANISH";
            case HpiConst.SAHPI_LANG_ESTONIAN:
                return "ESTONIAN";
            case HpiConst.SAHPI_LANG_BASQUE:
                return "BASQUE";
            case HpiConst.SAHPI_LANG_PERSIAN:
                return "PERSIAN";
            case HpiConst.SAHPI_LANG_FINNISH:
                return "FINNISH";
            case HpiConst.SAHPI_LANG_FIJI:
                return "FIJI";
            case HpiConst.SAHPI_LANG_FAEROESE:
                return "FAEROESE";
            case HpiConst.SAHPI_LANG_FRENCH:
                return "FRENCH";
            case HpiConst.SAHPI_LANG_FRISIAN:
                return "FRISIAN";
            case HpiConst.SAHPI_LANG_IRISH:
                return "IRISH";
            case HpiConst.SAHPI_LANG_SCOTSGAELIC:
                return "SCOTSGAELIC";
            case HpiConst.SAHPI_LANG_GALICIAN:
                return "GALICIAN";
            case HpiConst.SAHPI_LANG_GUARANI:
                return "GUARANI";
            case HpiConst.SAHPI_LANG_GUJARATI:
                return "GUJARATI";
            case HpiConst.SAHPI_LANG_HAUSA:
                return "HAUSA";
            case HpiConst.SAHPI_LANG_HINDI:
                return "HINDI";
            case HpiConst.SAHPI_LANG_CROATIAN:
                return "CROATIAN";
            case HpiConst.SAHPI_LANG_HUNGARIAN:
                return "HUNGARIAN";
            case HpiConst.SAHPI_LANG_ARMENIAN:
                return "ARMENIAN";
            case HpiConst.SAHPI_LANG_INTERLINGUA:
                return "INTERLINGUA";
            case HpiConst.SAHPI_LANG_INTERLINGUE:
                return "INTERLINGUE";
            case HpiConst.SAHPI_LANG_INUPIAK:
                return "INUPIAK";
            case HpiConst.SAHPI_LANG_INDONESIAN:
                return "INDONESIAN";
            case HpiConst.SAHPI_LANG_ICELANDIC:
                return "ICELANDIC";
            case HpiConst.SAHPI_LANG_ITALIAN:
                return "ITALIAN";
            case HpiConst.SAHPI_LANG_HEBREW:
                return "HEBREW";
            case HpiConst.SAHPI_LANG_JAPANESE:
                return "JAPANESE";
            case HpiConst.SAHPI_LANG_YIDDISH:
                return "YIDDISH";
            case HpiConst.SAHPI_LANG_JAVANESE:
                return "JAVANESE";
            case HpiConst.SAHPI_LANG_GEORGIAN:
                return "GEORGIAN";
            case HpiConst.SAHPI_LANG_KAZAKH:
                return "KAZAKH";
            case HpiConst.SAHPI_LANG_GREENLANDIC:
                return "GREENLANDIC";
            case HpiConst.SAHPI_LANG_CAMBODIAN:
                return "CAMBODIAN";
            case HpiConst.SAHPI_LANG_KANNADA:
                return "KANNADA";
            case HpiConst.SAHPI_LANG_KOREAN:
                return "KOREAN";
            case HpiConst.SAHPI_LANG_KASHMIRI:
                return "KASHMIRI";
            case HpiConst.SAHPI_LANG_KURDISH:
                return "KURDISH";
            case HpiConst.SAHPI_LANG_KIRGHIZ:
                return "KIRGHIZ";
            case HpiConst.SAHPI_LANG_LATIN:
                return "LATIN";
            case HpiConst.SAHPI_LANG_LINGALA:
                return "LINGALA";
            case HpiConst.SAHPI_LANG_LAOTHIAN:
                return "LAOTHIAN";
            case HpiConst.SAHPI_LANG_LITHUANIAN:
                return "LITHUANIAN";
            case HpiConst.SAHPI_LANG_LATVIANLETTISH:
                return "LATVIANLETTISH";
            case HpiConst.SAHPI_LANG_MALAGASY:
                return "MALAGASY";
            case HpiConst.SAHPI_LANG_MAORI:
                return "MAORI";
            case HpiConst.SAHPI_LANG_MACEDONIAN:
                return "MACEDONIAN";
            case HpiConst.SAHPI_LANG_MALAYALAM:
                return "MALAYALAM";
            case HpiConst.SAHPI_LANG_MONGOLIAN:
                return "MONGOLIAN";
            case HpiConst.SAHPI_LANG_MOLDAVIAN:
                return "MOLDAVIAN";
            case HpiConst.SAHPI_LANG_MARATHI:
                return "MARATHI";
            case HpiConst.SAHPI_LANG_MALAY:
                return "MALAY";
            case HpiConst.SAHPI_LANG_MALTESE:
                return "MALTESE";
            case HpiConst.SAHPI_LANG_BURMESE:
                return "BURMESE";
            case HpiConst.SAHPI_LANG_NAURU:
                return "NAURU";
            case HpiConst.SAHPI_LANG_NEPALI:
                return "NEPALI";
            case HpiConst.SAHPI_LANG_DUTCH:
                return "DUTCH";
            case HpiConst.SAHPI_LANG_NORWEGIAN:
                return "NORWEGIAN";
            case HpiConst.SAHPI_LANG_OCCITAN:
                return "OCCITAN";
            case HpiConst.SAHPI_LANG_AFANOROMO:
                return "AFANOROMO";
            case HpiConst.SAHPI_LANG_ORIYA:
                return "ORIYA";
            case HpiConst.SAHPI_LANG_PUNJABI:
                return "PUNJABI";
            case HpiConst.SAHPI_LANG_POLISH:
                return "POLISH";
            case HpiConst.SAHPI_LANG_PASHTOPUSHTO:
                return "PASHTOPUSHTO";
            case HpiConst.SAHPI_LANG_PORTUGUESE:
                return "PORTUGUESE";
            case HpiConst.SAHPI_LANG_QUECHUA:
                return "QUECHUA";
            case HpiConst.SAHPI_LANG_RHAETOROMANCE:
                return "RHAETOROMANCE";
            case HpiConst.SAHPI_LANG_KIRUNDI:
                return "KIRUNDI";
            case HpiConst.SAHPI_LANG_ROMANIAN:
                return "ROMANIAN";
            case HpiConst.SAHPI_LANG_RUSSIAN:
                return "RUSSIAN";
            case HpiConst.SAHPI_LANG_KINYARWANDA:
                return "KINYARWANDA";
            case HpiConst.SAHPI_LANG_SANSKRIT:
                return "SANSKRIT";
            case HpiConst.SAHPI_LANG_SINDHI:
                return "SINDHI";
            case HpiConst.SAHPI_LANG_SANGRO:
                return "SANGRO";
            case HpiConst.SAHPI_LANG_SERBOCROATIAN:
                return "SERBOCROATIAN";
            case HpiConst.SAHPI_LANG_SINGHALESE:
                return "SINGHALESE";
            case HpiConst.SAHPI_LANG_SLOVAK:
                return "SLOVAK";
            case HpiConst.SAHPI_LANG_SLOVENIAN:
                return "SLOVENIAN";
            case HpiConst.SAHPI_LANG_SAMOAN:
                return "SAMOAN";
            case HpiConst.SAHPI_LANG_SHONA:
                return "SHONA";
            case HpiConst.SAHPI_LANG_SOMALI:
                return "SOMALI";
            case HpiConst.SAHPI_LANG_ALBANIAN:
                return "ALBANIAN";
            case HpiConst.SAHPI_LANG_SERBIAN:
                return "SERBIAN";
            case HpiConst.SAHPI_LANG_SISWATI:
                return "SISWATI";
            case HpiConst.SAHPI_LANG_SESOTHO:
                return "SESOTHO";
            case HpiConst.SAHPI_LANG_SUDANESE:
                return "SUDANESE";
            case HpiConst.SAHPI_LANG_SWEDISH:
                return "SWEDISH";
            case HpiConst.SAHPI_LANG_SWAHILI:
                return "SWAHILI";
            case HpiConst.SAHPI_LANG_TAMIL:
                return "TAMIL";
            case HpiConst.SAHPI_LANG_TELUGU:
                return "TELUGU";
            case HpiConst.SAHPI_LANG_TAJIK:
                return "TAJIK";
            case HpiConst.SAHPI_LANG_THAI:
                return "THAI";
            case HpiConst.SAHPI_LANG_TIGRINYA:
                return "TIGRINYA";
            case HpiConst.SAHPI_LANG_TURKMEN:
                return "TURKMEN";
            case HpiConst.SAHPI_LANG_TAGALOG:
                return "TAGALOG";
            case HpiConst.SAHPI_LANG_SETSWANA:
                return "SETSWANA";
            case HpiConst.SAHPI_LANG_TONGA:
                return "TONGA";
            case HpiConst.SAHPI_LANG_TURKISH:
                return "TURKISH";
            case HpiConst.SAHPI_LANG_TSONGA:
                return "TSONGA";
            case HpiConst.SAHPI_LANG_TATAR:
                return "TATAR";
            case HpiConst.SAHPI_LANG_TWI:
                return "TWI";
            case HpiConst.SAHPI_LANG_UKRAINIAN:
                return "UKRAINIAN";
            case HpiConst.SAHPI_LANG_URDU:
                return "URDU";
            case HpiConst.SAHPI_LANG_UZBEK:
                return "UZBEK";
            case HpiConst.SAHPI_LANG_VIETNAMESE:
                return "VIETNAMESE";
            case HpiConst.SAHPI_LANG_VOLAPUK:
                return "VOLAPUK";
            case HpiConst.SAHPI_LANG_WOLOF:
                return "WOLOF";
            case HpiConst.SAHPI_LANG_XHOSA:
                return "XHOSA";
            case HpiConst.SAHPI_LANG_YORUBA:
                return "YORUBA";
            case HpiConst.SAHPI_LANG_CHINESE:
                return "CHINESE";
            case HpiConst.SAHPI_LANG_ZULU:
                return "ZULU";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiLanguageT( string s )
    {
        if ( s == "UNDEF" ) {
            return HpiConst.SAHPI_LANG_UNDEF;
        }
        if ( s == "AFAR" ) {
            return HpiConst.SAHPI_LANG_AFAR;
        }
        if ( s == "ABKHAZIAN" ) {
            return HpiConst.SAHPI_LANG_ABKHAZIAN;
        }
        if ( s == "AFRIKAANS" ) {
            return HpiConst.SAHPI_LANG_AFRIKAANS;
        }
        if ( s == "AMHARIC" ) {
            return HpiConst.SAHPI_LANG_AMHARIC;
        }
        if ( s == "ARABIC" ) {
            return HpiConst.SAHPI_LANG_ARABIC;
        }
        if ( s == "ASSAMESE" ) {
            return HpiConst.SAHPI_LANG_ASSAMESE;
        }
        if ( s == "AYMARA" ) {
            return HpiConst.SAHPI_LANG_AYMARA;
        }
        if ( s == "AZERBAIJANI" ) {
            return HpiConst.SAHPI_LANG_AZERBAIJANI;
        }
        if ( s == "BASHKIR" ) {
            return HpiConst.SAHPI_LANG_BASHKIR;
        }
        if ( s == "BYELORUSSIAN" ) {
            return HpiConst.SAHPI_LANG_BYELORUSSIAN;
        }
        if ( s == "BULGARIAN" ) {
            return HpiConst.SAHPI_LANG_BULGARIAN;
        }
        if ( s == "BIHARI" ) {
            return HpiConst.SAHPI_LANG_BIHARI;
        }
        if ( s == "BISLAMA" ) {
            return HpiConst.SAHPI_LANG_BISLAMA;
        }
        if ( s == "BENGALI" ) {
            return HpiConst.SAHPI_LANG_BENGALI;
        }
        if ( s == "TIBETAN" ) {
            return HpiConst.SAHPI_LANG_TIBETAN;
        }
        if ( s == "BRETON" ) {
            return HpiConst.SAHPI_LANG_BRETON;
        }
        if ( s == "CATALAN" ) {
            return HpiConst.SAHPI_LANG_CATALAN;
        }
        if ( s == "CORSICAN" ) {
            return HpiConst.SAHPI_LANG_CORSICAN;
        }
        if ( s == "CZECH" ) {
            return HpiConst.SAHPI_LANG_CZECH;
        }
        if ( s == "WELSH" ) {
            return HpiConst.SAHPI_LANG_WELSH;
        }
        if ( s == "DANISH" ) {
            return HpiConst.SAHPI_LANG_DANISH;
        }
        if ( s == "GERMAN" ) {
            return HpiConst.SAHPI_LANG_GERMAN;
        }
        if ( s == "BHUTANI" ) {
            return HpiConst.SAHPI_LANG_BHUTANI;
        }
        if ( s == "GREEK" ) {
            return HpiConst.SAHPI_LANG_GREEK;
        }
        if ( s == "ENGLISH" ) {
            return HpiConst.SAHPI_LANG_ENGLISH;
        }
        if ( s == "ESPERANTO" ) {
            return HpiConst.SAHPI_LANG_ESPERANTO;
        }
        if ( s == "SPANISH" ) {
            return HpiConst.SAHPI_LANG_SPANISH;
        }
        if ( s == "ESTONIAN" ) {
            return HpiConst.SAHPI_LANG_ESTONIAN;
        }
        if ( s == "BASQUE" ) {
            return HpiConst.SAHPI_LANG_BASQUE;
        }
        if ( s == "PERSIAN" ) {
            return HpiConst.SAHPI_LANG_PERSIAN;
        }
        if ( s == "FINNISH" ) {
            return HpiConst.SAHPI_LANG_FINNISH;
        }
        if ( s == "FIJI" ) {
            return HpiConst.SAHPI_LANG_FIJI;
        }
        if ( s == "FAEROESE" ) {
            return HpiConst.SAHPI_LANG_FAEROESE;
        }
        if ( s == "FRENCH" ) {
            return HpiConst.SAHPI_LANG_FRENCH;
        }
        if ( s == "FRISIAN" ) {
            return HpiConst.SAHPI_LANG_FRISIAN;
        }
        if ( s == "IRISH" ) {
            return HpiConst.SAHPI_LANG_IRISH;
        }
        if ( s == "SCOTSGAELIC" ) {
            return HpiConst.SAHPI_LANG_SCOTSGAELIC;
        }
        if ( s == "GALICIAN" ) {
            return HpiConst.SAHPI_LANG_GALICIAN;
        }
        if ( s == "GUARANI" ) {
            return HpiConst.SAHPI_LANG_GUARANI;
        }
        if ( s == "GUJARATI" ) {
            return HpiConst.SAHPI_LANG_GUJARATI;
        }
        if ( s == "HAUSA" ) {
            return HpiConst.SAHPI_LANG_HAUSA;
        }
        if ( s == "HINDI" ) {
            return HpiConst.SAHPI_LANG_HINDI;
        }
        if ( s == "CROATIAN" ) {
            return HpiConst.SAHPI_LANG_CROATIAN;
        }
        if ( s == "HUNGARIAN" ) {
            return HpiConst.SAHPI_LANG_HUNGARIAN;
        }
        if ( s == "ARMENIAN" ) {
            return HpiConst.SAHPI_LANG_ARMENIAN;
        }
        if ( s == "INTERLINGUA" ) {
            return HpiConst.SAHPI_LANG_INTERLINGUA;
        }
        if ( s == "INTERLINGUE" ) {
            return HpiConst.SAHPI_LANG_INTERLINGUE;
        }
        if ( s == "INUPIAK" ) {
            return HpiConst.SAHPI_LANG_INUPIAK;
        }
        if ( s == "INDONESIAN" ) {
            return HpiConst.SAHPI_LANG_INDONESIAN;
        }
        if ( s == "ICELANDIC" ) {
            return HpiConst.SAHPI_LANG_ICELANDIC;
        }
        if ( s == "ITALIAN" ) {
            return HpiConst.SAHPI_LANG_ITALIAN;
        }
        if ( s == "HEBREW" ) {
            return HpiConst.SAHPI_LANG_HEBREW;
        }
        if ( s == "JAPANESE" ) {
            return HpiConst.SAHPI_LANG_JAPANESE;
        }
        if ( s == "YIDDISH" ) {
            return HpiConst.SAHPI_LANG_YIDDISH;
        }
        if ( s == "JAVANESE" ) {
            return HpiConst.SAHPI_LANG_JAVANESE;
        }
        if ( s == "GEORGIAN" ) {
            return HpiConst.SAHPI_LANG_GEORGIAN;
        }
        if ( s == "KAZAKH" ) {
            return HpiConst.SAHPI_LANG_KAZAKH;
        }
        if ( s == "GREENLANDIC" ) {
            return HpiConst.SAHPI_LANG_GREENLANDIC;
        }
        if ( s == "CAMBODIAN" ) {
            return HpiConst.SAHPI_LANG_CAMBODIAN;
        }
        if ( s == "KANNADA" ) {
            return HpiConst.SAHPI_LANG_KANNADA;
        }
        if ( s == "KOREAN" ) {
            return HpiConst.SAHPI_LANG_KOREAN;
        }
        if ( s == "KASHMIRI" ) {
            return HpiConst.SAHPI_LANG_KASHMIRI;
        }
        if ( s == "KURDISH" ) {
            return HpiConst.SAHPI_LANG_KURDISH;
        }
        if ( s == "KIRGHIZ" ) {
            return HpiConst.SAHPI_LANG_KIRGHIZ;
        }
        if ( s == "LATIN" ) {
            return HpiConst.SAHPI_LANG_LATIN;
        }
        if ( s == "LINGALA" ) {
            return HpiConst.SAHPI_LANG_LINGALA;
        }
        if ( s == "LAOTHIAN" ) {
            return HpiConst.SAHPI_LANG_LAOTHIAN;
        }
        if ( s == "LITHUANIAN" ) {
            return HpiConst.SAHPI_LANG_LITHUANIAN;
        }
        if ( s == "LATVIANLETTISH" ) {
            return HpiConst.SAHPI_LANG_LATVIANLETTISH;
        }
        if ( s == "MALAGASY" ) {
            return HpiConst.SAHPI_LANG_MALAGASY;
        }
        if ( s == "MAORI" ) {
            return HpiConst.SAHPI_LANG_MAORI;
        }
        if ( s == "MACEDONIAN" ) {
            return HpiConst.SAHPI_LANG_MACEDONIAN;
        }
        if ( s == "MALAYALAM" ) {
            return HpiConst.SAHPI_LANG_MALAYALAM;
        }
        if ( s == "MONGOLIAN" ) {
            return HpiConst.SAHPI_LANG_MONGOLIAN;
        }
        if ( s == "MOLDAVIAN" ) {
            return HpiConst.SAHPI_LANG_MOLDAVIAN;
        }
        if ( s == "MARATHI" ) {
            return HpiConst.SAHPI_LANG_MARATHI;
        }
        if ( s == "MALAY" ) {
            return HpiConst.SAHPI_LANG_MALAY;
        }
        if ( s == "MALTESE" ) {
            return HpiConst.SAHPI_LANG_MALTESE;
        }
        if ( s == "BURMESE" ) {
            return HpiConst.SAHPI_LANG_BURMESE;
        }
        if ( s == "NAURU" ) {
            return HpiConst.SAHPI_LANG_NAURU;
        }
        if ( s == "NEPALI" ) {
            return HpiConst.SAHPI_LANG_NEPALI;
        }
        if ( s == "DUTCH" ) {
            return HpiConst.SAHPI_LANG_DUTCH;
        }
        if ( s == "NORWEGIAN" ) {
            return HpiConst.SAHPI_LANG_NORWEGIAN;
        }
        if ( s == "OCCITAN" ) {
            return HpiConst.SAHPI_LANG_OCCITAN;
        }
        if ( s == "AFANOROMO" ) {
            return HpiConst.SAHPI_LANG_AFANOROMO;
        }
        if ( s == "ORIYA" ) {
            return HpiConst.SAHPI_LANG_ORIYA;
        }
        if ( s == "PUNJABI" ) {
            return HpiConst.SAHPI_LANG_PUNJABI;
        }
        if ( s == "POLISH" ) {
            return HpiConst.SAHPI_LANG_POLISH;
        }
        if ( s == "PASHTOPUSHTO" ) {
            return HpiConst.SAHPI_LANG_PASHTOPUSHTO;
        }
        if ( s == "PORTUGUESE" ) {
            return HpiConst.SAHPI_LANG_PORTUGUESE;
        }
        if ( s == "QUECHUA" ) {
            return HpiConst.SAHPI_LANG_QUECHUA;
        }
        if ( s == "RHAETOROMANCE" ) {
            return HpiConst.SAHPI_LANG_RHAETOROMANCE;
        }
        if ( s == "KIRUNDI" ) {
            return HpiConst.SAHPI_LANG_KIRUNDI;
        }
        if ( s == "ROMANIAN" ) {
            return HpiConst.SAHPI_LANG_ROMANIAN;
        }
        if ( s == "RUSSIAN" ) {
            return HpiConst.SAHPI_LANG_RUSSIAN;
        }
        if ( s == "KINYARWANDA" ) {
            return HpiConst.SAHPI_LANG_KINYARWANDA;
        }
        if ( s == "SANSKRIT" ) {
            return HpiConst.SAHPI_LANG_SANSKRIT;
        }
        if ( s == "SINDHI" ) {
            return HpiConst.SAHPI_LANG_SINDHI;
        }
        if ( s == "SANGRO" ) {
            return HpiConst.SAHPI_LANG_SANGRO;
        }
        if ( s == "SERBOCROATIAN" ) {
            return HpiConst.SAHPI_LANG_SERBOCROATIAN;
        }
        if ( s == "SINGHALESE" ) {
            return HpiConst.SAHPI_LANG_SINGHALESE;
        }
        if ( s == "SLOVAK" ) {
            return HpiConst.SAHPI_LANG_SLOVAK;
        }
        if ( s == "SLOVENIAN" ) {
            return HpiConst.SAHPI_LANG_SLOVENIAN;
        }
        if ( s == "SAMOAN" ) {
            return HpiConst.SAHPI_LANG_SAMOAN;
        }
        if ( s == "SHONA" ) {
            return HpiConst.SAHPI_LANG_SHONA;
        }
        if ( s == "SOMALI" ) {
            return HpiConst.SAHPI_LANG_SOMALI;
        }
        if ( s == "ALBANIAN" ) {
            return HpiConst.SAHPI_LANG_ALBANIAN;
        }
        if ( s == "SERBIAN" ) {
            return HpiConst.SAHPI_LANG_SERBIAN;
        }
        if ( s == "SISWATI" ) {
            return HpiConst.SAHPI_LANG_SISWATI;
        }
        if ( s == "SESOTHO" ) {
            return HpiConst.SAHPI_LANG_SESOTHO;
        }
        if ( s == "SUDANESE" ) {
            return HpiConst.SAHPI_LANG_SUDANESE;
        }
        if ( s == "SWEDISH" ) {
            return HpiConst.SAHPI_LANG_SWEDISH;
        }
        if ( s == "SWAHILI" ) {
            return HpiConst.SAHPI_LANG_SWAHILI;
        }
        if ( s == "TAMIL" ) {
            return HpiConst.SAHPI_LANG_TAMIL;
        }
        if ( s == "TELUGU" ) {
            return HpiConst.SAHPI_LANG_TELUGU;
        }
        if ( s == "TAJIK" ) {
            return HpiConst.SAHPI_LANG_TAJIK;
        }
        if ( s == "THAI" ) {
            return HpiConst.SAHPI_LANG_THAI;
        }
        if ( s == "TIGRINYA" ) {
            return HpiConst.SAHPI_LANG_TIGRINYA;
        }
        if ( s == "TURKMEN" ) {
            return HpiConst.SAHPI_LANG_TURKMEN;
        }
        if ( s == "TAGALOG" ) {
            return HpiConst.SAHPI_LANG_TAGALOG;
        }
        if ( s == "SETSWANA" ) {
            return HpiConst.SAHPI_LANG_SETSWANA;
        }
        if ( s == "TONGA" ) {
            return HpiConst.SAHPI_LANG_TONGA;
        }
        if ( s == "TURKISH" ) {
            return HpiConst.SAHPI_LANG_TURKISH;
        }
        if ( s == "TSONGA" ) {
            return HpiConst.SAHPI_LANG_TSONGA;
        }
        if ( s == "TATAR" ) {
            return HpiConst.SAHPI_LANG_TATAR;
        }
        if ( s == "TWI" ) {
            return HpiConst.SAHPI_LANG_TWI;
        }
        if ( s == "UKRAINIAN" ) {
            return HpiConst.SAHPI_LANG_UKRAINIAN;
        }
        if ( s == "URDU" ) {
            return HpiConst.SAHPI_LANG_URDU;
        }
        if ( s == "UZBEK" ) {
            return HpiConst.SAHPI_LANG_UZBEK;
        }
        if ( s == "VIETNAMESE" ) {
            return HpiConst.SAHPI_LANG_VIETNAMESE;
        }
        if ( s == "VOLAPUK" ) {
            return HpiConst.SAHPI_LANG_VOLAPUK;
        }
        if ( s == "WOLOF" ) {
            return HpiConst.SAHPI_LANG_WOLOF;
        }
        if ( s == "XHOSA" ) {
            return HpiConst.SAHPI_LANG_XHOSA;
        }
        if ( s == "YORUBA" ) {
            return HpiConst.SAHPI_LANG_YORUBA;
        }
        if ( s == "CHINESE" ) {
            return HpiConst.SAHPI_LANG_CHINESE;
        }
        if ( s == "ZULU" ) {
            return HpiConst.SAHPI_LANG_ZULU;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiTextTypeT
     */
    public static string FromSaHpiTextTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_TL_TYPE_UNICODE:
                return "UNICODE";
            case HpiConst.SAHPI_TL_TYPE_BCDPLUS:
                return "BCDPLUS";
            case HpiConst.SAHPI_TL_TYPE_ASCII6:
                return "ASCII6";
            case HpiConst.SAHPI_TL_TYPE_TEXT:
                return "TEXT";
            case HpiConst.SAHPI_TL_TYPE_BINARY:
                return "BINARY";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiTextTypeT( string s )
    {
        if ( s == "UNICODE" ) {
            return HpiConst.SAHPI_TL_TYPE_UNICODE;
        }
        if ( s == "BCDPLUS" ) {
            return HpiConst.SAHPI_TL_TYPE_BCDPLUS;
        }
        if ( s == "ASCII6" ) {
            return HpiConst.SAHPI_TL_TYPE_ASCII6;
        }
        if ( s == "TEXT" ) {
            return HpiConst.SAHPI_TL_TYPE_TEXT;
        }
        if ( s == "BINARY" ) {
            return HpiConst.SAHPI_TL_TYPE_BINARY;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiEntityTypeT
     */
    public static string FromSaHpiEntityTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_ENT_UNSPECIFIED:
                return "UNSPECIFIED";
            case HpiConst.SAHPI_ENT_OTHER:
                return "OTHER";
            case HpiConst.SAHPI_ENT_UNKNOWN:
                return "UNKNOWN";
            case HpiConst.SAHPI_ENT_PROCESSOR:
                return "PROCESSOR";
            case HpiConst.SAHPI_ENT_DISK_BAY:
                return "DISK_BAY";
            case HpiConst.SAHPI_ENT_PERIPHERAL_BAY:
                return "PERIPHERAL_BAY";
            case HpiConst.SAHPI_ENT_SYS_MGMNT_MODULE:
                return "SYS_MGMNT_MODULE";
            case HpiConst.SAHPI_ENT_SYSTEM_BOARD:
                return "SYSTEM_BOARD";
            case HpiConst.SAHPI_ENT_MEMORY_MODULE:
                return "MEMORY_MODULE";
            case HpiConst.SAHPI_ENT_PROCESSOR_MODULE:
                return "PROCESSOR_MODULE";
            case HpiConst.SAHPI_ENT_POWER_SUPPLY:
                return "POWER_SUPPLY";
            case HpiConst.SAHPI_ENT_ADD_IN_CARD:
                return "ADD_IN_CARD";
            case HpiConst.SAHPI_ENT_FRONT_PANEL_BOARD:
                return "FRONT_PANEL_BOARD";
            case HpiConst.SAHPI_ENT_BACK_PANEL_BOARD:
                return "BACK_PANEL_BOARD";
            case HpiConst.SAHPI_ENT_POWER_SYSTEM_BOARD:
                return "POWER_SYSTEM_BOARD";
            case HpiConst.SAHPI_ENT_DRIVE_BACKPLANE:
                return "DRIVE_BACKPLANE";
            case HpiConst.SAHPI_ENT_SYS_EXPANSION_BOARD:
                return "SYS_EXPANSION_BOARD";
            case HpiConst.SAHPI_ENT_OTHER_SYSTEM_BOARD:
                return "OTHER_SYSTEM_BOARD";
            case HpiConst.SAHPI_ENT_PROCESSOR_BOARD:
                return "PROCESSOR_BOARD";
            case HpiConst.SAHPI_ENT_POWER_UNIT:
                return "POWER_UNIT";
            case HpiConst.SAHPI_ENT_POWER_MODULE:
                return "POWER_MODULE";
            case HpiConst.SAHPI_ENT_POWER_MGMNT:
                return "POWER_MGMNT";
            case HpiConst.SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD:
                return "CHASSIS_BACK_PANEL_BOARD";
            case HpiConst.SAHPI_ENT_SYSTEM_CHASSIS:
                return "SYSTEM_CHASSIS";
            case HpiConst.SAHPI_ENT_SUB_CHASSIS:
                return "SUB_CHASSIS";
            case HpiConst.SAHPI_ENT_OTHER_CHASSIS_BOARD:
                return "OTHER_CHASSIS_BOARD";
            case HpiConst.SAHPI_ENT_DISK_DRIVE_BAY:
                return "DISK_DRIVE_BAY";
            case HpiConst.SAHPI_ENT_PERIPHERAL_BAY_2:
                return "PERIPHERAL_BAY_2";
            case HpiConst.SAHPI_ENT_DEVICE_BAY:
                return "DEVICE_BAY";
            case HpiConst.SAHPI_ENT_COOLING_DEVICE:
                return "COOLING_DEVICE";
            case HpiConst.SAHPI_ENT_COOLING_UNIT:
                return "COOLING_UNIT";
            case HpiConst.SAHPI_ENT_INTERCONNECT:
                return "INTERCONNECT";
            case HpiConst.SAHPI_ENT_MEMORY_DEVICE:
                return "MEMORY_DEVICE";
            case HpiConst.SAHPI_ENT_SYS_MGMNT_SOFTWARE:
                return "SYS_MGMNT_SOFTWARE";
            case HpiConst.SAHPI_ENT_BIOS:
                return "BIOS";
            case HpiConst.SAHPI_ENT_OPERATING_SYSTEM:
                return "OPERATING_SYSTEM";
            case HpiConst.SAHPI_ENT_SYSTEM_BUS:
                return "SYSTEM_BUS";
            case HpiConst.SAHPI_ENT_GROUP:
                return "GROUP";
            case HpiConst.SAHPI_ENT_REMOTE:
                return "REMOTE";
            case HpiConst.SAHPI_ENT_EXTERNAL_ENVIRONMENT:
                return "EXTERNAL_ENVIRONMENT";
            case HpiConst.SAHPI_ENT_BATTERY:
                return "BATTERY";
            case HpiConst.SAHPI_ENT_PROCESSING_BLADE:
                return "PROCESSING_BLADE";
            case HpiConst.SAHPI_ENT_CONNECTIVITY_SWITCH:
                return "CONNECTIVITY_SWITCH";
            case HpiConst.SAHPI_ENT_PROCESSOR_MEMORY_MODULE:
                return "PROCESSOR_MEMORY_MODULE";
            case HpiConst.SAHPI_ENT_IO_MODULE:
                return "IO_MODULE";
            case HpiConst.SAHPI_ENT_PROCESSOR_IO_MODULE:
                return "PROCESSOR_IO_MODULE";
            case HpiConst.SAHPI_ENT_MC_FIRMWARE:
                return "MC_FIRMWARE";
            case HpiConst.SAHPI_ENT_IPMI_CHANNEL:
                return "IPMI_CHANNEL";
            case HpiConst.SAHPI_ENT_PCI_BUS:
                return "PCI_BUS";
            case HpiConst.SAHPI_ENT_PCI_EXPRESS_BUS:
                return "PCI_EXPRESS_BUS";
            case HpiConst.SAHPI_ENT_SCSI_BUS:
                return "SCSI_BUS";
            case HpiConst.SAHPI_ENT_SATA_BUS:
                return "SATA_BUS";
            case HpiConst.SAHPI_ENT_PROC_FSB:
                return "PROC_FSB";
            case HpiConst.SAHPI_ENT_CLOCK:
                return "CLOCK";
            case HpiConst.SAHPI_ENT_SYSTEM_FIRMWARE:
                return "SYSTEM_FIRMWARE";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC:
                return "CHASSIS_SPECIFIC";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC01:
                return "CHASSIS_SPECIFIC01";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC02:
                return "CHASSIS_SPECIFIC02";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC03:
                return "CHASSIS_SPECIFIC03";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC04:
                return "CHASSIS_SPECIFIC04";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC05:
                return "CHASSIS_SPECIFIC05";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC06:
                return "CHASSIS_SPECIFIC06";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC07:
                return "CHASSIS_SPECIFIC07";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC08:
                return "CHASSIS_SPECIFIC08";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC09:
                return "CHASSIS_SPECIFIC09";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC10:
                return "CHASSIS_SPECIFIC10";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC11:
                return "CHASSIS_SPECIFIC11";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC12:
                return "CHASSIS_SPECIFIC12";
            case HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC13:
                return "CHASSIS_SPECIFIC13";
            case HpiConst.SAHPI_ENT_BOARD_SET_SPECIFIC:
                return "BOARD_SET_SPECIFIC";
            case HpiConst.SAHPI_ENT_OEM_SYSINT_SPECIFIC:
                return "OEM_SYSINT_SPECIFIC";
            case HpiConst.SAHPI_ENT_ROOT:
                return "ROOT";
            case HpiConst.SAHPI_ENT_RACK:
                return "RACK";
            case HpiConst.SAHPI_ENT_SUBRACK:
                return "SUBRACK";
            case HpiConst.SAHPI_ENT_COMPACTPCI_CHASSIS:
                return "COMPACTPCI_CHASSIS";
            case HpiConst.SAHPI_ENT_ADVANCEDTCA_CHASSIS:
                return "ADVANCEDTCA_CHASSIS";
            case HpiConst.SAHPI_ENT_RACK_MOUNTED_SERVER:
                return "RACK_MOUNTED_SERVER";
            case HpiConst.SAHPI_ENT_SYSTEM_BLADE:
                return "SYSTEM_BLADE";
            case HpiConst.SAHPI_ENT_SWITCH:
                return "SWITCH";
            case HpiConst.SAHPI_ENT_SWITCH_BLADE:
                return "SWITCH_BLADE";
            case HpiConst.SAHPI_ENT_SBC_BLADE:
                return "SBC_BLADE";
            case HpiConst.SAHPI_ENT_IO_BLADE:
                return "IO_BLADE";
            case HpiConst.SAHPI_ENT_DISK_BLADE:
                return "DISK_BLADE";
            case HpiConst.SAHPI_ENT_DISK_DRIVE:
                return "DISK_DRIVE";
            case HpiConst.SAHPI_ENT_FAN:
                return "FAN";
            case HpiConst.SAHPI_ENT_POWER_DISTRIBUTION_UNIT:
                return "POWER_DISTRIBUTION_UNIT";
            case HpiConst.SAHPI_ENT_SPEC_PROC_BLADE:
                return "SPEC_PROC_BLADE";
            case HpiConst.SAHPI_ENT_IO_SUBBOARD:
                return "IO_SUBBOARD";
            case HpiConst.SAHPI_ENT_SBC_SUBBOARD:
                return "SBC_SUBBOARD";
            case HpiConst.SAHPI_ENT_ALARM_MANAGER:
                return "ALARM_MANAGER";
            case HpiConst.SAHPI_ENT_SHELF_MANAGER:
                return "SHELF_MANAGER";
            case HpiConst.SAHPI_ENT_DISPLAY_PANEL:
                return "DISPLAY_PANEL";
            case HpiConst.SAHPI_ENT_SUBBOARD_CARRIER_BLADE:
                return "SUBBOARD_CARRIER_BLADE";
            case HpiConst.SAHPI_ENT_PHYSICAL_SLOT:
                return "PHYSICAL_SLOT";
            case HpiConst.SAHPI_ENT_PICMG_FRONT_BLADE:
                return "PICMG_FRONT_BLADE";
            case HpiConst.SAHPI_ENT_SYSTEM_INVENTORY_DEVICE:
                return "SYSTEM_INVENTORY_DEVICE";
            case HpiConst.SAHPI_ENT_FILTRATION_UNIT:
                return "FILTRATION_UNIT";
            case HpiConst.SAHPI_ENT_AMC:
                return "AMC";
            case HpiConst.SAHPI_ENT_BMC:
                return "BMC";
            case HpiConst.SAHPI_ENT_IPMC:
                return "IPMC";
            case HpiConst.SAHPI_ENT_MMC:
                return "MMC";
            case HpiConst.SAHPI_ENT_SHMC:
                return "SHMC";
            case HpiConst.SAHPI_ENT_CPLD:
                return "CPLD";
            case HpiConst.SAHPI_ENT_EPLD:
                return "EPLD";
            case HpiConst.SAHPI_ENT_FPGA:
                return "FPGA";
            case HpiConst.SAHPI_ENT_DASD:
                return "DASD";
            case HpiConst.SAHPI_ENT_NIC:
                return "NIC";
            case HpiConst.SAHPI_ENT_DSP:
                return "DSP";
            case HpiConst.SAHPI_ENT_UCODE:
                return "UCODE";
            case HpiConst.SAHPI_ENT_NPU:
                return "NPU";
            case HpiConst.SAHPI_ENT_OEM:
                return "OEM";
            case HpiConst.SAHPI_ENT_INTERFACE:
                return "INTERFACE";
            case HpiConst.SAHPI_ENT_MICROTCA_CHASSIS:
                return "MICROTCA_CHASSIS";
            case HpiConst.SAHPI_ENT_CARRIER:
                return "CARRIER";
            case HpiConst.SAHPI_ENT_CARRIER_MANAGER:
                return "CARRIER_MANAGER";
            case HpiConst.SAHPI_ENT_CONFIG_DATA:
                return "CONFIG_DATA";
            case HpiConst.SAHPI_ENT_INDICATOR:
                return "INDICATOR";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiEntityTypeT( string s )
    {
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_ENT_UNSPECIFIED;
        }
        if ( s == "OTHER" ) {
            return HpiConst.SAHPI_ENT_OTHER;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.SAHPI_ENT_UNKNOWN;
        }
        if ( s == "PROCESSOR" ) {
            return HpiConst.SAHPI_ENT_PROCESSOR;
        }
        if ( s == "DISK_BAY" ) {
            return HpiConst.SAHPI_ENT_DISK_BAY;
        }
        if ( s == "PERIPHERAL_BAY" ) {
            return HpiConst.SAHPI_ENT_PERIPHERAL_BAY;
        }
        if ( s == "SYS_MGMNT_MODULE" ) {
            return HpiConst.SAHPI_ENT_SYS_MGMNT_MODULE;
        }
        if ( s == "SYSTEM_BOARD" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_BOARD;
        }
        if ( s == "MEMORY_MODULE" ) {
            return HpiConst.SAHPI_ENT_MEMORY_MODULE;
        }
        if ( s == "PROCESSOR_MODULE" ) {
            return HpiConst.SAHPI_ENT_PROCESSOR_MODULE;
        }
        if ( s == "POWER_SUPPLY" ) {
            return HpiConst.SAHPI_ENT_POWER_SUPPLY;
        }
        if ( s == "ADD_IN_CARD" ) {
            return HpiConst.SAHPI_ENT_ADD_IN_CARD;
        }
        if ( s == "FRONT_PANEL_BOARD" ) {
            return HpiConst.SAHPI_ENT_FRONT_PANEL_BOARD;
        }
        if ( s == "BACK_PANEL_BOARD" ) {
            return HpiConst.SAHPI_ENT_BACK_PANEL_BOARD;
        }
        if ( s == "POWER_SYSTEM_BOARD" ) {
            return HpiConst.SAHPI_ENT_POWER_SYSTEM_BOARD;
        }
        if ( s == "DRIVE_BACKPLANE" ) {
            return HpiConst.SAHPI_ENT_DRIVE_BACKPLANE;
        }
        if ( s == "SYS_EXPANSION_BOARD" ) {
            return HpiConst.SAHPI_ENT_SYS_EXPANSION_BOARD;
        }
        if ( s == "OTHER_SYSTEM_BOARD" ) {
            return HpiConst.SAHPI_ENT_OTHER_SYSTEM_BOARD;
        }
        if ( s == "PROCESSOR_BOARD" ) {
            return HpiConst.SAHPI_ENT_PROCESSOR_BOARD;
        }
        if ( s == "POWER_UNIT" ) {
            return HpiConst.SAHPI_ENT_POWER_UNIT;
        }
        if ( s == "POWER_MODULE" ) {
            return HpiConst.SAHPI_ENT_POWER_MODULE;
        }
        if ( s == "POWER_MGMNT" ) {
            return HpiConst.SAHPI_ENT_POWER_MGMNT;
        }
        if ( s == "CHASSIS_BACK_PANEL_BOARD" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD;
        }
        if ( s == "SYSTEM_CHASSIS" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_CHASSIS;
        }
        if ( s == "SUB_CHASSIS" ) {
            return HpiConst.SAHPI_ENT_SUB_CHASSIS;
        }
        if ( s == "OTHER_CHASSIS_BOARD" ) {
            return HpiConst.SAHPI_ENT_OTHER_CHASSIS_BOARD;
        }
        if ( s == "DISK_DRIVE_BAY" ) {
            return HpiConst.SAHPI_ENT_DISK_DRIVE_BAY;
        }
        if ( s == "PERIPHERAL_BAY_2" ) {
            return HpiConst.SAHPI_ENT_PERIPHERAL_BAY_2;
        }
        if ( s == "DEVICE_BAY" ) {
            return HpiConst.SAHPI_ENT_DEVICE_BAY;
        }
        if ( s == "COOLING_DEVICE" ) {
            return HpiConst.SAHPI_ENT_COOLING_DEVICE;
        }
        if ( s == "COOLING_UNIT" ) {
            return HpiConst.SAHPI_ENT_COOLING_UNIT;
        }
        if ( s == "INTERCONNECT" ) {
            return HpiConst.SAHPI_ENT_INTERCONNECT;
        }
        if ( s == "MEMORY_DEVICE" ) {
            return HpiConst.SAHPI_ENT_MEMORY_DEVICE;
        }
        if ( s == "SYS_MGMNT_SOFTWARE" ) {
            return HpiConst.SAHPI_ENT_SYS_MGMNT_SOFTWARE;
        }
        if ( s == "BIOS" ) {
            return HpiConst.SAHPI_ENT_BIOS;
        }
        if ( s == "OPERATING_SYSTEM" ) {
            return HpiConst.SAHPI_ENT_OPERATING_SYSTEM;
        }
        if ( s == "SYSTEM_BUS" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_BUS;
        }
        if ( s == "GROUP" ) {
            return HpiConst.SAHPI_ENT_GROUP;
        }
        if ( s == "REMOTE" ) {
            return HpiConst.SAHPI_ENT_REMOTE;
        }
        if ( s == "EXTERNAL_ENVIRONMENT" ) {
            return HpiConst.SAHPI_ENT_EXTERNAL_ENVIRONMENT;
        }
        if ( s == "BATTERY" ) {
            return HpiConst.SAHPI_ENT_BATTERY;
        }
        if ( s == "PROCESSING_BLADE" ) {
            return HpiConst.SAHPI_ENT_PROCESSING_BLADE;
        }
        if ( s == "CONNECTIVITY_SWITCH" ) {
            return HpiConst.SAHPI_ENT_CONNECTIVITY_SWITCH;
        }
        if ( s == "PROCESSOR_MEMORY_MODULE" ) {
            return HpiConst.SAHPI_ENT_PROCESSOR_MEMORY_MODULE;
        }
        if ( s == "IO_MODULE" ) {
            return HpiConst.SAHPI_ENT_IO_MODULE;
        }
        if ( s == "PROCESSOR_IO_MODULE" ) {
            return HpiConst.SAHPI_ENT_PROCESSOR_IO_MODULE;
        }
        if ( s == "MC_FIRMWARE" ) {
            return HpiConst.SAHPI_ENT_MC_FIRMWARE;
        }
        if ( s == "IPMI_CHANNEL" ) {
            return HpiConst.SAHPI_ENT_IPMI_CHANNEL;
        }
        if ( s == "PCI_BUS" ) {
            return HpiConst.SAHPI_ENT_PCI_BUS;
        }
        if ( s == "PCI_EXPRESS_BUS" ) {
            return HpiConst.SAHPI_ENT_PCI_EXPRESS_BUS;
        }
        if ( s == "SCSI_BUS" ) {
            return HpiConst.SAHPI_ENT_SCSI_BUS;
        }
        if ( s == "SATA_BUS" ) {
            return HpiConst.SAHPI_ENT_SATA_BUS;
        }
        if ( s == "PROC_FSB" ) {
            return HpiConst.SAHPI_ENT_PROC_FSB;
        }
        if ( s == "CLOCK" ) {
            return HpiConst.SAHPI_ENT_CLOCK;
        }
        if ( s == "SYSTEM_FIRMWARE" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_FIRMWARE;
        }
        if ( s == "CHASSIS_SPECIFIC" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC;
        }
        if ( s == "CHASSIS_SPECIFIC01" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC01;
        }
        if ( s == "CHASSIS_SPECIFIC02" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC02;
        }
        if ( s == "CHASSIS_SPECIFIC03" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC03;
        }
        if ( s == "CHASSIS_SPECIFIC04" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC04;
        }
        if ( s == "CHASSIS_SPECIFIC05" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC05;
        }
        if ( s == "CHASSIS_SPECIFIC06" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC06;
        }
        if ( s == "CHASSIS_SPECIFIC07" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC07;
        }
        if ( s == "CHASSIS_SPECIFIC08" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC08;
        }
        if ( s == "CHASSIS_SPECIFIC09" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC09;
        }
        if ( s == "CHASSIS_SPECIFIC10" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC10;
        }
        if ( s == "CHASSIS_SPECIFIC11" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC11;
        }
        if ( s == "CHASSIS_SPECIFIC12" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC12;
        }
        if ( s == "CHASSIS_SPECIFIC13" ) {
            return HpiConst.SAHPI_ENT_CHASSIS_SPECIFIC13;
        }
        if ( s == "BOARD_SET_SPECIFIC" ) {
            return HpiConst.SAHPI_ENT_BOARD_SET_SPECIFIC;
        }
        if ( s == "OEM_SYSINT_SPECIFIC" ) {
            return HpiConst.SAHPI_ENT_OEM_SYSINT_SPECIFIC;
        }
        if ( s == "ROOT" ) {
            return HpiConst.SAHPI_ENT_ROOT;
        }
        if ( s == "RACK" ) {
            return HpiConst.SAHPI_ENT_RACK;
        }
        if ( s == "SUBRACK" ) {
            return HpiConst.SAHPI_ENT_SUBRACK;
        }
        if ( s == "COMPACTPCI_CHASSIS" ) {
            return HpiConst.SAHPI_ENT_COMPACTPCI_CHASSIS;
        }
        if ( s == "ADVANCEDTCA_CHASSIS" ) {
            return HpiConst.SAHPI_ENT_ADVANCEDTCA_CHASSIS;
        }
        if ( s == "RACK_MOUNTED_SERVER" ) {
            return HpiConst.SAHPI_ENT_RACK_MOUNTED_SERVER;
        }
        if ( s == "SYSTEM_BLADE" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_BLADE;
        }
        if ( s == "SWITCH" ) {
            return HpiConst.SAHPI_ENT_SWITCH;
        }
        if ( s == "SWITCH_BLADE" ) {
            return HpiConst.SAHPI_ENT_SWITCH_BLADE;
        }
        if ( s == "SBC_BLADE" ) {
            return HpiConst.SAHPI_ENT_SBC_BLADE;
        }
        if ( s == "IO_BLADE" ) {
            return HpiConst.SAHPI_ENT_IO_BLADE;
        }
        if ( s == "DISK_BLADE" ) {
            return HpiConst.SAHPI_ENT_DISK_BLADE;
        }
        if ( s == "DISK_DRIVE" ) {
            return HpiConst.SAHPI_ENT_DISK_DRIVE;
        }
        if ( s == "FAN" ) {
            return HpiConst.SAHPI_ENT_FAN;
        }
        if ( s == "POWER_DISTRIBUTION_UNIT" ) {
            return HpiConst.SAHPI_ENT_POWER_DISTRIBUTION_UNIT;
        }
        if ( s == "SPEC_PROC_BLADE" ) {
            return HpiConst.SAHPI_ENT_SPEC_PROC_BLADE;
        }
        if ( s == "IO_SUBBOARD" ) {
            return HpiConst.SAHPI_ENT_IO_SUBBOARD;
        }
        if ( s == "SBC_SUBBOARD" ) {
            return HpiConst.SAHPI_ENT_SBC_SUBBOARD;
        }
        if ( s == "ALARM_MANAGER" ) {
            return HpiConst.SAHPI_ENT_ALARM_MANAGER;
        }
        if ( s == "SHELF_MANAGER" ) {
            return HpiConst.SAHPI_ENT_SHELF_MANAGER;
        }
        if ( s == "DISPLAY_PANEL" ) {
            return HpiConst.SAHPI_ENT_DISPLAY_PANEL;
        }
        if ( s == "SUBBOARD_CARRIER_BLADE" ) {
            return HpiConst.SAHPI_ENT_SUBBOARD_CARRIER_BLADE;
        }
        if ( s == "PHYSICAL_SLOT" ) {
            return HpiConst.SAHPI_ENT_PHYSICAL_SLOT;
        }
        if ( s == "PICMG_FRONT_BLADE" ) {
            return HpiConst.SAHPI_ENT_PICMG_FRONT_BLADE;
        }
        if ( s == "SYSTEM_INVENTORY_DEVICE" ) {
            return HpiConst.SAHPI_ENT_SYSTEM_INVENTORY_DEVICE;
        }
        if ( s == "FILTRATION_UNIT" ) {
            return HpiConst.SAHPI_ENT_FILTRATION_UNIT;
        }
        if ( s == "AMC" ) {
            return HpiConst.SAHPI_ENT_AMC;
        }
        if ( s == "BMC" ) {
            return HpiConst.SAHPI_ENT_BMC;
        }
        if ( s == "IPMC" ) {
            return HpiConst.SAHPI_ENT_IPMC;
        }
        if ( s == "MMC" ) {
            return HpiConst.SAHPI_ENT_MMC;
        }
        if ( s == "SHMC" ) {
            return HpiConst.SAHPI_ENT_SHMC;
        }
        if ( s == "CPLD" ) {
            return HpiConst.SAHPI_ENT_CPLD;
        }
        if ( s == "EPLD" ) {
            return HpiConst.SAHPI_ENT_EPLD;
        }
        if ( s == "FPGA" ) {
            return HpiConst.SAHPI_ENT_FPGA;
        }
        if ( s == "DASD" ) {
            return HpiConst.SAHPI_ENT_DASD;
        }
        if ( s == "NIC" ) {
            return HpiConst.SAHPI_ENT_NIC;
        }
        if ( s == "DSP" ) {
            return HpiConst.SAHPI_ENT_DSP;
        }
        if ( s == "UCODE" ) {
            return HpiConst.SAHPI_ENT_UCODE;
        }
        if ( s == "NPU" ) {
            return HpiConst.SAHPI_ENT_NPU;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_ENT_OEM;
        }
        if ( s == "INTERFACE" ) {
            return HpiConst.SAHPI_ENT_INTERFACE;
        }
        if ( s == "MICROTCA_CHASSIS" ) {
            return HpiConst.SAHPI_ENT_MICROTCA_CHASSIS;
        }
        if ( s == "CARRIER" ) {
            return HpiConst.SAHPI_ENT_CARRIER;
        }
        if ( s == "CARRIER_MANAGER" ) {
            return HpiConst.SAHPI_ENT_CARRIER_MANAGER;
        }
        if ( s == "CONFIG_DATA" ) {
            return HpiConst.SAHPI_ENT_CONFIG_DATA;
        }
        if ( s == "INDICATOR" ) {
            return HpiConst.SAHPI_ENT_INDICATOR;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorTypeT
     */
    public static string FromSaHpiSensorTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_TEMPERATURE:
                return "TEMPERATURE";
            case HpiConst.SAHPI_VOLTAGE:
                return "VOLTAGE";
            case HpiConst.SAHPI_CURRENT:
                return "CURRENT";
            case HpiConst.SAHPI_FAN:
                return "FAN";
            case HpiConst.SAHPI_PHYSICAL_SECURITY:
                return "PHYSICAL_SECURITY";
            case HpiConst.SAHPI_PLATFORM_VIOLATION:
                return "PLATFORM_VIOLATION";
            case HpiConst.SAHPI_PROCESSOR:
                return "PROCESSOR";
            case HpiConst.SAHPI_POWER_SUPPLY:
                return "POWER_SUPPLY";
            case HpiConst.SAHPI_POWER_UNIT:
                return "POWER_UNIT";
            case HpiConst.SAHPI_COOLING_DEVICE:
                return "COOLING_DEVICE";
            case HpiConst.SAHPI_OTHER_UNITS_BASED_SENSOR:
                return "OTHER_UNITS_BASED_SENSOR";
            case HpiConst.SAHPI_MEMORY:
                return "MEMORY";
            case HpiConst.SAHPI_DRIVE_SLOT:
                return "DRIVE_SLOT";
            case HpiConst.SAHPI_POST_MEMORY_RESIZE:
                return "POST_MEMORY_RESIZE";
            case HpiConst.SAHPI_SYSTEM_FW_PROGRESS:
                return "SYSTEM_FW_PROGRESS";
            case HpiConst.SAHPI_EVENT_LOGGING_DISABLED:
                return "EVENT_LOGGING_DISABLED";
            case HpiConst.SAHPI_RESERVED1:
                return "RESERVED1";
            case HpiConst.SAHPI_SYSTEM_EVENT:
                return "SYSTEM_EVENT";
            case HpiConst.SAHPI_CRITICAL_INTERRUPT:
                return "CRITICAL_INTERRUPT";
            case HpiConst.SAHPI_BUTTON:
                return "BUTTON";
            case HpiConst.SAHPI_MODULE_BOARD:
                return "MODULE_BOARD";
            case HpiConst.SAHPI_MICROCONTROLLER_COPROCESSOR:
                return "MICROCONTROLLER_COPROCESSOR";
            case HpiConst.SAHPI_ADDIN_CARD:
                return "ADDIN_CARD";
            case HpiConst.SAHPI_CHASSIS:
                return "CHASSIS";
            case HpiConst.SAHPI_CHIP_SET:
                return "CHIP_SET";
            case HpiConst.SAHPI_OTHER_FRU:
                return "OTHER_FRU";
            case HpiConst.SAHPI_CABLE_INTERCONNECT:
                return "CABLE_INTERCONNECT";
            case HpiConst.SAHPI_TERMINATOR:
                return "TERMINATOR";
            case HpiConst.SAHPI_SYSTEM_BOOT_INITIATED:
                return "SYSTEM_BOOT_INITIATED";
            case HpiConst.SAHPI_BOOT_ERROR:
                return "BOOT_ERROR";
            case HpiConst.SAHPI_OS_BOOT:
                return "OS_BOOT";
            case HpiConst.SAHPI_OS_CRITICAL_STOP:
                return "OS_CRITICAL_STOP";
            case HpiConst.SAHPI_SLOT_CONNECTOR:
                return "SLOT_CONNECTOR";
            case HpiConst.SAHPI_SYSTEM_ACPI_POWER_STATE:
                return "SYSTEM_ACPI_POWER_STATE";
            case HpiConst.SAHPI_RESERVED2:
                return "RESERVED2";
            case HpiConst.SAHPI_PLATFORM_ALERT:
                return "PLATFORM_ALERT";
            case HpiConst.SAHPI_ENTITY_PRESENCE:
                return "ENTITY_PRESENCE";
            case HpiConst.SAHPI_MONITOR_ASIC_IC:
                return "MONITOR_ASIC_IC";
            case HpiConst.SAHPI_LAN:
                return "LAN";
            case HpiConst.SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH:
                return "MANAGEMENT_SUBSYSTEM_HEALTH";
            case HpiConst.SAHPI_BATTERY:
                return "BATTERY";
            case HpiConst.SAHPI_SESSION_AUDIT:
                return "SESSION_AUDIT";
            case HpiConst.SAHPI_VERSION_CHANGE:
                return "VERSION_CHANGE";
            case HpiConst.SAHPI_OPERATIONAL:
                return "OPERATIONAL";
            case HpiConst.SAHPI_OEM_SENSOR:
                return "OEM_SENSOR";
            case HpiConst.SAHPI_COMM_CHANNEL_LINK_STATE:
                return "COMM_CHANNEL_LINK_STATE";
            case HpiConst.SAHPI_MANAGEMENT_BUS_STATE:
                return "MANAGEMENT_BUS_STATE";
            case HpiConst.SAHPI_COMM_CHANNEL_BUS_STATE:
                return "COMM_CHANNEL_BUS_STATE";
            case HpiConst.SAHPI_CONFIG_DATA:
                return "CONFIG_DATA";
            case HpiConst.SAHPI_POWER_BUDGET:
                return "POWER_BUDGET";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorTypeT( string s )
    {
        if ( s == "TEMPERATURE" ) {
            return HpiConst.SAHPI_TEMPERATURE;
        }
        if ( s == "VOLTAGE" ) {
            return HpiConst.SAHPI_VOLTAGE;
        }
        if ( s == "CURRENT" ) {
            return HpiConst.SAHPI_CURRENT;
        }
        if ( s == "FAN" ) {
            return HpiConst.SAHPI_FAN;
        }
        if ( s == "PHYSICAL_SECURITY" ) {
            return HpiConst.SAHPI_PHYSICAL_SECURITY;
        }
        if ( s == "PLATFORM_VIOLATION" ) {
            return HpiConst.SAHPI_PLATFORM_VIOLATION;
        }
        if ( s == "PROCESSOR" ) {
            return HpiConst.SAHPI_PROCESSOR;
        }
        if ( s == "POWER_SUPPLY" ) {
            return HpiConst.SAHPI_POWER_SUPPLY;
        }
        if ( s == "POWER_UNIT" ) {
            return HpiConst.SAHPI_POWER_UNIT;
        }
        if ( s == "COOLING_DEVICE" ) {
            return HpiConst.SAHPI_COOLING_DEVICE;
        }
        if ( s == "OTHER_UNITS_BASED_SENSOR" ) {
            return HpiConst.SAHPI_OTHER_UNITS_BASED_SENSOR;
        }
        if ( s == "MEMORY" ) {
            return HpiConst.SAHPI_MEMORY;
        }
        if ( s == "DRIVE_SLOT" ) {
            return HpiConst.SAHPI_DRIVE_SLOT;
        }
        if ( s == "POST_MEMORY_RESIZE" ) {
            return HpiConst.SAHPI_POST_MEMORY_RESIZE;
        }
        if ( s == "SYSTEM_FW_PROGRESS" ) {
            return HpiConst.SAHPI_SYSTEM_FW_PROGRESS;
        }
        if ( s == "EVENT_LOGGING_DISABLED" ) {
            return HpiConst.SAHPI_EVENT_LOGGING_DISABLED;
        }
        if ( s == "RESERVED1" ) {
            return HpiConst.SAHPI_RESERVED1;
        }
        if ( s == "SYSTEM_EVENT" ) {
            return HpiConst.SAHPI_SYSTEM_EVENT;
        }
        if ( s == "CRITICAL_INTERRUPT" ) {
            return HpiConst.SAHPI_CRITICAL_INTERRUPT;
        }
        if ( s == "BUTTON" ) {
            return HpiConst.SAHPI_BUTTON;
        }
        if ( s == "MODULE_BOARD" ) {
            return HpiConst.SAHPI_MODULE_BOARD;
        }
        if ( s == "MICROCONTROLLER_COPROCESSOR" ) {
            return HpiConst.SAHPI_MICROCONTROLLER_COPROCESSOR;
        }
        if ( s == "ADDIN_CARD" ) {
            return HpiConst.SAHPI_ADDIN_CARD;
        }
        if ( s == "CHASSIS" ) {
            return HpiConst.SAHPI_CHASSIS;
        }
        if ( s == "CHIP_SET" ) {
            return HpiConst.SAHPI_CHIP_SET;
        }
        if ( s == "OTHER_FRU" ) {
            return HpiConst.SAHPI_OTHER_FRU;
        }
        if ( s == "CABLE_INTERCONNECT" ) {
            return HpiConst.SAHPI_CABLE_INTERCONNECT;
        }
        if ( s == "TERMINATOR" ) {
            return HpiConst.SAHPI_TERMINATOR;
        }
        if ( s == "SYSTEM_BOOT_INITIATED" ) {
            return HpiConst.SAHPI_SYSTEM_BOOT_INITIATED;
        }
        if ( s == "BOOT_ERROR" ) {
            return HpiConst.SAHPI_BOOT_ERROR;
        }
        if ( s == "OS_BOOT" ) {
            return HpiConst.SAHPI_OS_BOOT;
        }
        if ( s == "OS_CRITICAL_STOP" ) {
            return HpiConst.SAHPI_OS_CRITICAL_STOP;
        }
        if ( s == "SLOT_CONNECTOR" ) {
            return HpiConst.SAHPI_SLOT_CONNECTOR;
        }
        if ( s == "SYSTEM_ACPI_POWER_STATE" ) {
            return HpiConst.SAHPI_SYSTEM_ACPI_POWER_STATE;
        }
        if ( s == "RESERVED2" ) {
            return HpiConst.SAHPI_RESERVED2;
        }
        if ( s == "PLATFORM_ALERT" ) {
            return HpiConst.SAHPI_PLATFORM_ALERT;
        }
        if ( s == "ENTITY_PRESENCE" ) {
            return HpiConst.SAHPI_ENTITY_PRESENCE;
        }
        if ( s == "MONITOR_ASIC_IC" ) {
            return HpiConst.SAHPI_MONITOR_ASIC_IC;
        }
        if ( s == "LAN" ) {
            return HpiConst.SAHPI_LAN;
        }
        if ( s == "MANAGEMENT_SUBSYSTEM_HEALTH" ) {
            return HpiConst.SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH;
        }
        if ( s == "BATTERY" ) {
            return HpiConst.SAHPI_BATTERY;
        }
        if ( s == "SESSION_AUDIT" ) {
            return HpiConst.SAHPI_SESSION_AUDIT;
        }
        if ( s == "VERSION_CHANGE" ) {
            return HpiConst.SAHPI_VERSION_CHANGE;
        }
        if ( s == "OPERATIONAL" ) {
            return HpiConst.SAHPI_OPERATIONAL;
        }
        if ( s == "OEM_SENSOR" ) {
            return HpiConst.SAHPI_OEM_SENSOR;
        }
        if ( s == "COMM_CHANNEL_LINK_STATE" ) {
            return HpiConst.SAHPI_COMM_CHANNEL_LINK_STATE;
        }
        if ( s == "MANAGEMENT_BUS_STATE" ) {
            return HpiConst.SAHPI_MANAGEMENT_BUS_STATE;
        }
        if ( s == "COMM_CHANNEL_BUS_STATE" ) {
            return HpiConst.SAHPI_COMM_CHANNEL_BUS_STATE;
        }
        if ( s == "CONFIG_DATA" ) {
            return HpiConst.SAHPI_CONFIG_DATA;
        }
        if ( s == "POWER_BUDGET" ) {
            return HpiConst.SAHPI_POWER_BUDGET;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorReadingTypeT
     */
    public static string FromSaHpiSensorReadingTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_SENSOR_READING_TYPE_INT64:
                return "INT64";
            case HpiConst.SAHPI_SENSOR_READING_TYPE_UINT64:
                return "UINT64";
            case HpiConst.SAHPI_SENSOR_READING_TYPE_FLOAT64:
                return "FLOAT64";
            case HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER:
                return "BUFFER";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorReadingTypeT( string s )
    {
        if ( s == "INT64" ) {
            return HpiConst.SAHPI_SENSOR_READING_TYPE_INT64;
        }
        if ( s == "UINT64" ) {
            return HpiConst.SAHPI_SENSOR_READING_TYPE_UINT64;
        }
        if ( s == "FLOAT64" ) {
            return HpiConst.SAHPI_SENSOR_READING_TYPE_FLOAT64;
        }
        if ( s == "BUFFER" ) {
            return HpiConst.SAHPI_SENSOR_READING_TYPE_BUFFER;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorEventMaskActionT
     */
    public static string FromSaHpiSensorEventMaskActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_SENS_ADD_EVENTS_TO_MASKS:
                return "ADD_EVENTS_TO_MASKS";
            case HpiConst.SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS:
                return "REMOVE_EVENTS_FROM_MASKS";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorEventMaskActionT( string s )
    {
        if ( s == "ADD_EVENTS_TO_MASKS" ) {
            return HpiConst.SAHPI_SENS_ADD_EVENTS_TO_MASKS;
        }
        if ( s == "REMOVE_EVENTS_FROM_MASKS" ) {
            return HpiConst.SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorUnitsT
     */
    public static string FromSaHpiSensorUnitsT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_SU_UNSPECIFIED:
                return "UNSPECIFIED";
            case HpiConst.SAHPI_SU_DEGREES_C:
                return "DEGREES_C";
            case HpiConst.SAHPI_SU_DEGREES_F:
                return "DEGREES_F";
            case HpiConst.SAHPI_SU_DEGREES_K:
                return "DEGREES_K";
            case HpiConst.SAHPI_SU_VOLTS:
                return "VOLTS";
            case HpiConst.SAHPI_SU_AMPS:
                return "AMPS";
            case HpiConst.SAHPI_SU_WATTS:
                return "WATTS";
            case HpiConst.SAHPI_SU_JOULES:
                return "JOULES";
            case HpiConst.SAHPI_SU_COULOMBS:
                return "COULOMBS";
            case HpiConst.SAHPI_SU_VA:
                return "VA";
            case HpiConst.SAHPI_SU_NITS:
                return "NITS";
            case HpiConst.SAHPI_SU_LUMEN:
                return "LUMEN";
            case HpiConst.SAHPI_SU_LUX:
                return "LUX";
            case HpiConst.SAHPI_SU_CANDELA:
                return "CANDELA";
            case HpiConst.SAHPI_SU_KPA:
                return "KPA";
            case HpiConst.SAHPI_SU_PSI:
                return "PSI";
            case HpiConst.SAHPI_SU_NEWTON:
                return "NEWTON";
            case HpiConst.SAHPI_SU_CFM:
                return "CFM";
            case HpiConst.SAHPI_SU_RPM:
                return "RPM";
            case HpiConst.SAHPI_SU_HZ:
                return "HZ";
            case HpiConst.SAHPI_SU_MICROSECOND:
                return "MICROSECOND";
            case HpiConst.SAHPI_SU_MILLISECOND:
                return "MILLISECOND";
            case HpiConst.SAHPI_SU_SECOND:
                return "SECOND";
            case HpiConst.SAHPI_SU_MINUTE:
                return "MINUTE";
            case HpiConst.SAHPI_SU_HOUR:
                return "HOUR";
            case HpiConst.SAHPI_SU_DAY:
                return "DAY";
            case HpiConst.SAHPI_SU_WEEK:
                return "WEEK";
            case HpiConst.SAHPI_SU_MIL:
                return "MIL";
            case HpiConst.SAHPI_SU_INCHES:
                return "INCHES";
            case HpiConst.SAHPI_SU_FEET:
                return "FEET";
            case HpiConst.SAHPI_SU_CU_IN:
                return "CU_IN";
            case HpiConst.SAHPI_SU_CU_FEET:
                return "CU_FEET";
            case HpiConst.SAHPI_SU_MM:
                return "MM";
            case HpiConst.SAHPI_SU_CM:
                return "CM";
            case HpiConst.SAHPI_SU_M:
                return "M";
            case HpiConst.SAHPI_SU_CU_CM:
                return "CU_CM";
            case HpiConst.SAHPI_SU_CU_M:
                return "CU_M";
            case HpiConst.SAHPI_SU_LITERS:
                return "LITERS";
            case HpiConst.SAHPI_SU_FLUID_OUNCE:
                return "FLUID_OUNCE";
            case HpiConst.SAHPI_SU_RADIANS:
                return "RADIANS";
            case HpiConst.SAHPI_SU_STERADIANS:
                return "STERADIANS";
            case HpiConst.SAHPI_SU_REVOLUTIONS:
                return "REVOLUTIONS";
            case HpiConst.SAHPI_SU_CYCLES:
                return "CYCLES";
            case HpiConst.SAHPI_SU_GRAVITIES:
                return "GRAVITIES";
            case HpiConst.SAHPI_SU_OUNCE:
                return "OUNCE";
            case HpiConst.SAHPI_SU_POUND:
                return "POUND";
            case HpiConst.SAHPI_SU_FT_LB:
                return "FT_LB";
            case HpiConst.SAHPI_SU_OZ_IN:
                return "OZ_IN";
            case HpiConst.SAHPI_SU_GAUSS:
                return "GAUSS";
            case HpiConst.SAHPI_SU_GILBERTS:
                return "GILBERTS";
            case HpiConst.SAHPI_SU_HENRY:
                return "HENRY";
            case HpiConst.SAHPI_SU_MILLIHENRY:
                return "MILLIHENRY";
            case HpiConst.SAHPI_SU_FARAD:
                return "FARAD";
            case HpiConst.SAHPI_SU_MICROFARAD:
                return "MICROFARAD";
            case HpiConst.SAHPI_SU_OHMS:
                return "OHMS";
            case HpiConst.SAHPI_SU_SIEMENS:
                return "SIEMENS";
            case HpiConst.SAHPI_SU_MOLE:
                return "MOLE";
            case HpiConst.SAHPI_SU_BECQUEREL:
                return "BECQUEREL";
            case HpiConst.SAHPI_SU_PPM:
                return "PPM";
            case HpiConst.SAHPI_SU_RESERVED:
                return "RESERVED";
            case HpiConst.SAHPI_SU_DECIBELS:
                return "DECIBELS";
            case HpiConst.SAHPI_SU_DBA:
                return "DBA";
            case HpiConst.SAHPI_SU_DBC:
                return "DBC";
            case HpiConst.SAHPI_SU_GRAY:
                return "GRAY";
            case HpiConst.SAHPI_SU_SIEVERT:
                return "SIEVERT";
            case HpiConst.SAHPI_SU_COLOR_TEMP_DEG_K:
                return "COLOR_TEMP_DEG_K";
            case HpiConst.SAHPI_SU_BIT:
                return "BIT";
            case HpiConst.SAHPI_SU_KILOBIT:
                return "KILOBIT";
            case HpiConst.SAHPI_SU_MEGABIT:
                return "MEGABIT";
            case HpiConst.SAHPI_SU_GIGABIT:
                return "GIGABIT";
            case HpiConst.SAHPI_SU_BYTE:
                return "BYTE";
            case HpiConst.SAHPI_SU_KILOBYTE:
                return "KILOBYTE";
            case HpiConst.SAHPI_SU_MEGABYTE:
                return "MEGABYTE";
            case HpiConst.SAHPI_SU_GIGABYTE:
                return "GIGABYTE";
            case HpiConst.SAHPI_SU_WORD:
                return "WORD";
            case HpiConst.SAHPI_SU_DWORD:
                return "DWORD";
            case HpiConst.SAHPI_SU_QWORD:
                return "QWORD";
            case HpiConst.SAHPI_SU_LINE:
                return "LINE";
            case HpiConst.SAHPI_SU_HIT:
                return "HIT";
            case HpiConst.SAHPI_SU_MISS:
                return "MISS";
            case HpiConst.SAHPI_SU_RETRY:
                return "RETRY";
            case HpiConst.SAHPI_SU_RESET:
                return "RESET";
            case HpiConst.SAHPI_SU_OVERRUN:
                return "OVERRUN";
            case HpiConst.SAHPI_SU_UNDERRUN:
                return "UNDERRUN";
            case HpiConst.SAHPI_SU_COLLISION:
                return "COLLISION";
            case HpiConst.SAHPI_SU_PACKETS:
                return "PACKETS";
            case HpiConst.SAHPI_SU_MESSAGES:
                return "MESSAGES";
            case HpiConst.SAHPI_SU_CHARACTERS:
                return "CHARACTERS";
            case HpiConst.SAHPI_SU_ERRORS:
                return "ERRORS";
            case HpiConst.SAHPI_SU_CORRECTABLE_ERRORS:
                return "CORRECTABLE_ERRORS";
            case HpiConst.SAHPI_SU_UNCORRECTABLE_ERRORS:
                return "UNCORRECTABLE_ERRORS";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorUnitsT( string s )
    {
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_SU_UNSPECIFIED;
        }
        if ( s == "DEGREES_C" ) {
            return HpiConst.SAHPI_SU_DEGREES_C;
        }
        if ( s == "DEGREES_F" ) {
            return HpiConst.SAHPI_SU_DEGREES_F;
        }
        if ( s == "DEGREES_K" ) {
            return HpiConst.SAHPI_SU_DEGREES_K;
        }
        if ( s == "VOLTS" ) {
            return HpiConst.SAHPI_SU_VOLTS;
        }
        if ( s == "AMPS" ) {
            return HpiConst.SAHPI_SU_AMPS;
        }
        if ( s == "WATTS" ) {
            return HpiConst.SAHPI_SU_WATTS;
        }
        if ( s == "JOULES" ) {
            return HpiConst.SAHPI_SU_JOULES;
        }
        if ( s == "COULOMBS" ) {
            return HpiConst.SAHPI_SU_COULOMBS;
        }
        if ( s == "VA" ) {
            return HpiConst.SAHPI_SU_VA;
        }
        if ( s == "NITS" ) {
            return HpiConst.SAHPI_SU_NITS;
        }
        if ( s == "LUMEN" ) {
            return HpiConst.SAHPI_SU_LUMEN;
        }
        if ( s == "LUX" ) {
            return HpiConst.SAHPI_SU_LUX;
        }
        if ( s == "CANDELA" ) {
            return HpiConst.SAHPI_SU_CANDELA;
        }
        if ( s == "KPA" ) {
            return HpiConst.SAHPI_SU_KPA;
        }
        if ( s == "PSI" ) {
            return HpiConst.SAHPI_SU_PSI;
        }
        if ( s == "NEWTON" ) {
            return HpiConst.SAHPI_SU_NEWTON;
        }
        if ( s == "CFM" ) {
            return HpiConst.SAHPI_SU_CFM;
        }
        if ( s == "RPM" ) {
            return HpiConst.SAHPI_SU_RPM;
        }
        if ( s == "HZ" ) {
            return HpiConst.SAHPI_SU_HZ;
        }
        if ( s == "MICROSECOND" ) {
            return HpiConst.SAHPI_SU_MICROSECOND;
        }
        if ( s == "MILLISECOND" ) {
            return HpiConst.SAHPI_SU_MILLISECOND;
        }
        if ( s == "SECOND" ) {
            return HpiConst.SAHPI_SU_SECOND;
        }
        if ( s == "MINUTE" ) {
            return HpiConst.SAHPI_SU_MINUTE;
        }
        if ( s == "HOUR" ) {
            return HpiConst.SAHPI_SU_HOUR;
        }
        if ( s == "DAY" ) {
            return HpiConst.SAHPI_SU_DAY;
        }
        if ( s == "WEEK" ) {
            return HpiConst.SAHPI_SU_WEEK;
        }
        if ( s == "MIL" ) {
            return HpiConst.SAHPI_SU_MIL;
        }
        if ( s == "INCHES" ) {
            return HpiConst.SAHPI_SU_INCHES;
        }
        if ( s == "FEET" ) {
            return HpiConst.SAHPI_SU_FEET;
        }
        if ( s == "CU_IN" ) {
            return HpiConst.SAHPI_SU_CU_IN;
        }
        if ( s == "CU_FEET" ) {
            return HpiConst.SAHPI_SU_CU_FEET;
        }
        if ( s == "MM" ) {
            return HpiConst.SAHPI_SU_MM;
        }
        if ( s == "CM" ) {
            return HpiConst.SAHPI_SU_CM;
        }
        if ( s == "M" ) {
            return HpiConst.SAHPI_SU_M;
        }
        if ( s == "CU_CM" ) {
            return HpiConst.SAHPI_SU_CU_CM;
        }
        if ( s == "CU_M" ) {
            return HpiConst.SAHPI_SU_CU_M;
        }
        if ( s == "LITERS" ) {
            return HpiConst.SAHPI_SU_LITERS;
        }
        if ( s == "FLUID_OUNCE" ) {
            return HpiConst.SAHPI_SU_FLUID_OUNCE;
        }
        if ( s == "RADIANS" ) {
            return HpiConst.SAHPI_SU_RADIANS;
        }
        if ( s == "STERADIANS" ) {
            return HpiConst.SAHPI_SU_STERADIANS;
        }
        if ( s == "REVOLUTIONS" ) {
            return HpiConst.SAHPI_SU_REVOLUTIONS;
        }
        if ( s == "CYCLES" ) {
            return HpiConst.SAHPI_SU_CYCLES;
        }
        if ( s == "GRAVITIES" ) {
            return HpiConst.SAHPI_SU_GRAVITIES;
        }
        if ( s == "OUNCE" ) {
            return HpiConst.SAHPI_SU_OUNCE;
        }
        if ( s == "POUND" ) {
            return HpiConst.SAHPI_SU_POUND;
        }
        if ( s == "FT_LB" ) {
            return HpiConst.SAHPI_SU_FT_LB;
        }
        if ( s == "OZ_IN" ) {
            return HpiConst.SAHPI_SU_OZ_IN;
        }
        if ( s == "GAUSS" ) {
            return HpiConst.SAHPI_SU_GAUSS;
        }
        if ( s == "GILBERTS" ) {
            return HpiConst.SAHPI_SU_GILBERTS;
        }
        if ( s == "HENRY" ) {
            return HpiConst.SAHPI_SU_HENRY;
        }
        if ( s == "MILLIHENRY" ) {
            return HpiConst.SAHPI_SU_MILLIHENRY;
        }
        if ( s == "FARAD" ) {
            return HpiConst.SAHPI_SU_FARAD;
        }
        if ( s == "MICROFARAD" ) {
            return HpiConst.SAHPI_SU_MICROFARAD;
        }
        if ( s == "OHMS" ) {
            return HpiConst.SAHPI_SU_OHMS;
        }
        if ( s == "SIEMENS" ) {
            return HpiConst.SAHPI_SU_SIEMENS;
        }
        if ( s == "MOLE" ) {
            return HpiConst.SAHPI_SU_MOLE;
        }
        if ( s == "BECQUEREL" ) {
            return HpiConst.SAHPI_SU_BECQUEREL;
        }
        if ( s == "PPM" ) {
            return HpiConst.SAHPI_SU_PPM;
        }
        if ( s == "RESERVED" ) {
            return HpiConst.SAHPI_SU_RESERVED;
        }
        if ( s == "DECIBELS" ) {
            return HpiConst.SAHPI_SU_DECIBELS;
        }
        if ( s == "DBA" ) {
            return HpiConst.SAHPI_SU_DBA;
        }
        if ( s == "DBC" ) {
            return HpiConst.SAHPI_SU_DBC;
        }
        if ( s == "GRAY" ) {
            return HpiConst.SAHPI_SU_GRAY;
        }
        if ( s == "SIEVERT" ) {
            return HpiConst.SAHPI_SU_SIEVERT;
        }
        if ( s == "COLOR_TEMP_DEG_K" ) {
            return HpiConst.SAHPI_SU_COLOR_TEMP_DEG_K;
        }
        if ( s == "BIT" ) {
            return HpiConst.SAHPI_SU_BIT;
        }
        if ( s == "KILOBIT" ) {
            return HpiConst.SAHPI_SU_KILOBIT;
        }
        if ( s == "MEGABIT" ) {
            return HpiConst.SAHPI_SU_MEGABIT;
        }
        if ( s == "GIGABIT" ) {
            return HpiConst.SAHPI_SU_GIGABIT;
        }
        if ( s == "BYTE" ) {
            return HpiConst.SAHPI_SU_BYTE;
        }
        if ( s == "KILOBYTE" ) {
            return HpiConst.SAHPI_SU_KILOBYTE;
        }
        if ( s == "MEGABYTE" ) {
            return HpiConst.SAHPI_SU_MEGABYTE;
        }
        if ( s == "GIGABYTE" ) {
            return HpiConst.SAHPI_SU_GIGABYTE;
        }
        if ( s == "WORD" ) {
            return HpiConst.SAHPI_SU_WORD;
        }
        if ( s == "DWORD" ) {
            return HpiConst.SAHPI_SU_DWORD;
        }
        if ( s == "QWORD" ) {
            return HpiConst.SAHPI_SU_QWORD;
        }
        if ( s == "LINE" ) {
            return HpiConst.SAHPI_SU_LINE;
        }
        if ( s == "HIT" ) {
            return HpiConst.SAHPI_SU_HIT;
        }
        if ( s == "MISS" ) {
            return HpiConst.SAHPI_SU_MISS;
        }
        if ( s == "RETRY" ) {
            return HpiConst.SAHPI_SU_RETRY;
        }
        if ( s == "RESET" ) {
            return HpiConst.SAHPI_SU_RESET;
        }
        if ( s == "OVERRUN" ) {
            return HpiConst.SAHPI_SU_OVERRUN;
        }
        if ( s == "UNDERRUN" ) {
            return HpiConst.SAHPI_SU_UNDERRUN;
        }
        if ( s == "COLLISION" ) {
            return HpiConst.SAHPI_SU_COLLISION;
        }
        if ( s == "PACKETS" ) {
            return HpiConst.SAHPI_SU_PACKETS;
        }
        if ( s == "MESSAGES" ) {
            return HpiConst.SAHPI_SU_MESSAGES;
        }
        if ( s == "CHARACTERS" ) {
            return HpiConst.SAHPI_SU_CHARACTERS;
        }
        if ( s == "ERRORS" ) {
            return HpiConst.SAHPI_SU_ERRORS;
        }
        if ( s == "CORRECTABLE_ERRORS" ) {
            return HpiConst.SAHPI_SU_CORRECTABLE_ERRORS;
        }
        if ( s == "UNCORRECTABLE_ERRORS" ) {
            return HpiConst.SAHPI_SU_UNCORRECTABLE_ERRORS;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorModUnitUseT
     */
    public static string FromSaHpiSensorModUnitUseT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_SMUU_NONE:
                return "NONE";
            case HpiConst.SAHPI_SMUU_BASIC_OVER_MODIFIER:
                return "BASIC_OVER_MODIFIER";
            case HpiConst.SAHPI_SMUU_BASIC_TIMES_MODIFIER:
                return "BASIC_TIMES_MODIFIER";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorModUnitUseT( string s )
    {
        if ( s == "NONE" ) {
            return HpiConst.SAHPI_SMUU_NONE;
        }
        if ( s == "BASIC_OVER_MODIFIER" ) {
            return HpiConst.SAHPI_SMUU_BASIC_OVER_MODIFIER;
        }
        if ( s == "BASIC_TIMES_MODIFIER" ) {
            return HpiConst.SAHPI_SMUU_BASIC_TIMES_MODIFIER;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSensorEventCtrlT
     */
    public static string FromSaHpiSensorEventCtrlT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_SEC_PER_EVENT:
                return "PER_EVENT";
            case HpiConst.SAHPI_SEC_READ_ONLY_MASKS:
                return "READ_ONLY_MASKS";
            case HpiConst.SAHPI_SEC_READ_ONLY:
                return "READ_ONLY";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSensorEventCtrlT( string s )
    {
        if ( s == "PER_EVENT" ) {
            return HpiConst.SAHPI_SEC_PER_EVENT;
        }
        if ( s == "READ_ONLY_MASKS" ) {
            return HpiConst.SAHPI_SEC_READ_ONLY_MASKS;
        }
        if ( s == "READ_ONLY" ) {
            return HpiConst.SAHPI_SEC_READ_ONLY;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiCtrlTypeT
     */
    public static string FromSaHpiCtrlTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_CTRL_TYPE_DIGITAL:
                return "DIGITAL";
            case HpiConst.SAHPI_CTRL_TYPE_DISCRETE:
                return "DISCRETE";
            case HpiConst.SAHPI_CTRL_TYPE_ANALOG:
                return "ANALOG";
            case HpiConst.SAHPI_CTRL_TYPE_STREAM:
                return "STREAM";
            case HpiConst.SAHPI_CTRL_TYPE_TEXT:
                return "TEXT";
            case HpiConst.SAHPI_CTRL_TYPE_OEM:
                return "OEM";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiCtrlTypeT( string s )
    {
        if ( s == "DIGITAL" ) {
            return HpiConst.SAHPI_CTRL_TYPE_DIGITAL;
        }
        if ( s == "DISCRETE" ) {
            return HpiConst.SAHPI_CTRL_TYPE_DISCRETE;
        }
        if ( s == "ANALOG" ) {
            return HpiConst.SAHPI_CTRL_TYPE_ANALOG;
        }
        if ( s == "STREAM" ) {
            return HpiConst.SAHPI_CTRL_TYPE_STREAM;
        }
        if ( s == "TEXT" ) {
            return HpiConst.SAHPI_CTRL_TYPE_TEXT;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_CTRL_TYPE_OEM;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiCtrlStateDigitalT
     */
    public static string FromSaHpiCtrlStateDigitalT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_CTRL_STATE_OFF:
                return "OFF";
            case HpiConst.SAHPI_CTRL_STATE_ON:
                return "ON";
            case HpiConst.SAHPI_CTRL_STATE_PULSE_OFF:
                return "PULSE_OFF";
            case HpiConst.SAHPI_CTRL_STATE_PULSE_ON:
                return "PULSE_ON";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiCtrlStateDigitalT( string s )
    {
        if ( s == "OFF" ) {
            return HpiConst.SAHPI_CTRL_STATE_OFF;
        }
        if ( s == "ON" ) {
            return HpiConst.SAHPI_CTRL_STATE_ON;
        }
        if ( s == "PULSE_OFF" ) {
            return HpiConst.SAHPI_CTRL_STATE_PULSE_OFF;
        }
        if ( s == "PULSE_ON" ) {
            return HpiConst.SAHPI_CTRL_STATE_PULSE_ON;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiCtrlModeT
     */
    public static string FromSaHpiCtrlModeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_CTRL_MODE_AUTO:
                return "AUTO";
            case HpiConst.SAHPI_CTRL_MODE_MANUAL:
                return "MANUAL";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiCtrlModeT( string s )
    {
        if ( s == "AUTO" ) {
            return HpiConst.SAHPI_CTRL_MODE_AUTO;
        }
        if ( s == "MANUAL" ) {
            return HpiConst.SAHPI_CTRL_MODE_MANUAL;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiCtrlOutputTypeT
     */
    public static string FromSaHpiCtrlOutputTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_CTRL_GENERIC:
                return "GENERIC";
            case HpiConst.SAHPI_CTRL_LED:
                return "LED";
            case HpiConst.SAHPI_CTRL_FAN_SPEED:
                return "FAN_SPEED";
            case HpiConst.SAHPI_CTRL_DRY_CONTACT_CLOSURE:
                return "DRY_CONTACT_CLOSURE";
            case HpiConst.SAHPI_CTRL_POWER_SUPPLY_INHIBIT:
                return "POWER_SUPPLY_INHIBIT";
            case HpiConst.SAHPI_CTRL_AUDIBLE:
                return "AUDIBLE";
            case HpiConst.SAHPI_CTRL_FRONT_PANEL_LOCKOUT:
                return "FRONT_PANEL_LOCKOUT";
            case HpiConst.SAHPI_CTRL_POWER_INTERLOCK:
                return "POWER_INTERLOCK";
            case HpiConst.SAHPI_CTRL_POWER_STATE:
                return "POWER_STATE";
            case HpiConst.SAHPI_CTRL_LCD_DISPLAY:
                return "LCD_DISPLAY";
            case HpiConst.SAHPI_CTRL_OEM:
                return "OEM";
            case HpiConst.SAHPI_CTRL_GENERIC_ADDRESS:
                return "GENERIC_ADDRESS";
            case HpiConst.SAHPI_CTRL_IP_ADDRESS:
                return "IP_ADDRESS";
            case HpiConst.SAHPI_CTRL_RESOURCE_ID:
                return "RESOURCE_ID";
            case HpiConst.SAHPI_CTRL_POWER_BUDGET:
                return "POWER_BUDGET";
            case HpiConst.SAHPI_CTRL_ACTIVATE:
                return "ACTIVATE";
            case HpiConst.SAHPI_CTRL_RESET:
                return "RESET";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiCtrlOutputTypeT( string s )
    {
        if ( s == "GENERIC" ) {
            return HpiConst.SAHPI_CTRL_GENERIC;
        }
        if ( s == "LED" ) {
            return HpiConst.SAHPI_CTRL_LED;
        }
        if ( s == "FAN_SPEED" ) {
            return HpiConst.SAHPI_CTRL_FAN_SPEED;
        }
        if ( s == "DRY_CONTACT_CLOSURE" ) {
            return HpiConst.SAHPI_CTRL_DRY_CONTACT_CLOSURE;
        }
        if ( s == "POWER_SUPPLY_INHIBIT" ) {
            return HpiConst.SAHPI_CTRL_POWER_SUPPLY_INHIBIT;
        }
        if ( s == "AUDIBLE" ) {
            return HpiConst.SAHPI_CTRL_AUDIBLE;
        }
        if ( s == "FRONT_PANEL_LOCKOUT" ) {
            return HpiConst.SAHPI_CTRL_FRONT_PANEL_LOCKOUT;
        }
        if ( s == "POWER_INTERLOCK" ) {
            return HpiConst.SAHPI_CTRL_POWER_INTERLOCK;
        }
        if ( s == "POWER_STATE" ) {
            return HpiConst.SAHPI_CTRL_POWER_STATE;
        }
        if ( s == "LCD_DISPLAY" ) {
            return HpiConst.SAHPI_CTRL_LCD_DISPLAY;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_CTRL_OEM;
        }
        if ( s == "GENERIC_ADDRESS" ) {
            return HpiConst.SAHPI_CTRL_GENERIC_ADDRESS;
        }
        if ( s == "IP_ADDRESS" ) {
            return HpiConst.SAHPI_CTRL_IP_ADDRESS;
        }
        if ( s == "RESOURCE_ID" ) {
            return HpiConst.SAHPI_CTRL_RESOURCE_ID;
        }
        if ( s == "POWER_BUDGET" ) {
            return HpiConst.SAHPI_CTRL_POWER_BUDGET;
        }
        if ( s == "ACTIVATE" ) {
            return HpiConst.SAHPI_CTRL_ACTIVATE;
        }
        if ( s == "RESET" ) {
            return HpiConst.SAHPI_CTRL_RESET;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiIdrAreaTypeT
     */
    public static string FromSaHpiIdrAreaTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_IDR_AREATYPE_INTERNAL_USE:
                return "INTERNAL_USE";
            case HpiConst.SAHPI_IDR_AREATYPE_CHASSIS_INFO:
                return "CHASSIS_INFO";
            case HpiConst.SAHPI_IDR_AREATYPE_BOARD_INFO:
                return "BOARD_INFO";
            case HpiConst.SAHPI_IDR_AREATYPE_PRODUCT_INFO:
                return "PRODUCT_INFO";
            case HpiConst.SAHPI_IDR_AREATYPE_OEM:
                return "OEM";
            case HpiConst.SAHPI_IDR_AREATYPE_UNSPECIFIED:
                return "UNSPECIFIED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiIdrAreaTypeT( string s )
    {
        if ( s == "INTERNAL_USE" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_INTERNAL_USE;
        }
        if ( s == "CHASSIS_INFO" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_CHASSIS_INFO;
        }
        if ( s == "BOARD_INFO" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_BOARD_INFO;
        }
        if ( s == "PRODUCT_INFO" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_PRODUCT_INFO;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_OEM;
        }
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_IDR_AREATYPE_UNSPECIFIED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiIdrFieldTypeT
     */
    public static string FromSaHpiIdrFieldTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE:
                return "CHASSIS_TYPE";
            case HpiConst.SAHPI_IDR_FIELDTYPE_MFG_DATETIME:
                return "MFG_DATETIME";
            case HpiConst.SAHPI_IDR_FIELDTYPE_MANUFACTURER:
                return "MANUFACTURER";
            case HpiConst.SAHPI_IDR_FIELDTYPE_PRODUCT_NAME:
                return "PRODUCT_NAME";
            case HpiConst.SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION:
                return "PRODUCT_VERSION";
            case HpiConst.SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER:
                return "SERIAL_NUMBER";
            case HpiConst.SAHPI_IDR_FIELDTYPE_PART_NUMBER:
                return "PART_NUMBER";
            case HpiConst.SAHPI_IDR_FIELDTYPE_FILE_ID:
                return "FILE_ID";
            case HpiConst.SAHPI_IDR_FIELDTYPE_ASSET_TAG:
                return "ASSET_TAG";
            case HpiConst.SAHPI_IDR_FIELDTYPE_CUSTOM:
                return "CUSTOM";
            case HpiConst.SAHPI_IDR_FIELDTYPE_UNSPECIFIED:
                return "UNSPECIFIED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiIdrFieldTypeT( string s )
    {
        if ( s == "CHASSIS_TYPE" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE;
        }
        if ( s == "MFG_DATETIME" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_MFG_DATETIME;
        }
        if ( s == "MANUFACTURER" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_MANUFACTURER;
        }
        if ( s == "PRODUCT_NAME" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
        }
        if ( s == "PRODUCT_VERSION" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
        }
        if ( s == "SERIAL_NUMBER" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
        }
        if ( s == "PART_NUMBER" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_PART_NUMBER;
        }
        if ( s == "FILE_ID" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_FILE_ID;
        }
        if ( s == "ASSET_TAG" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_ASSET_TAG;
        }
        if ( s == "CUSTOM" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_CUSTOM;
        }
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiWatchdogActionT
     */
    public static string FromSaHpiWatchdogActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_WA_NO_ACTION:
                return "NO_ACTION";
            case HpiConst.SAHPI_WA_RESET:
                return "RESET";
            case HpiConst.SAHPI_WA_POWER_DOWN:
                return "POWER_DOWN";
            case HpiConst.SAHPI_WA_POWER_CYCLE:
                return "POWER_CYCLE";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiWatchdogActionT( string s )
    {
        if ( s == "NO_ACTION" ) {
            return HpiConst.SAHPI_WA_NO_ACTION;
        }
        if ( s == "RESET" ) {
            return HpiConst.SAHPI_WA_RESET;
        }
        if ( s == "POWER_DOWN" ) {
            return HpiConst.SAHPI_WA_POWER_DOWN;
        }
        if ( s == "POWER_CYCLE" ) {
            return HpiConst.SAHPI_WA_POWER_CYCLE;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiWatchdogActionEventT
     */
    public static string FromSaHpiWatchdogActionEventT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_WAE_NO_ACTION:
                return "NO_ACTION";
            case HpiConst.SAHPI_WAE_RESET:
                return "RESET";
            case HpiConst.SAHPI_WAE_POWER_DOWN:
                return "POWER_DOWN";
            case HpiConst.SAHPI_WAE_POWER_CYCLE:
                return "POWER_CYCLE";
            case HpiConst.SAHPI_WAE_TIMER_INT:
                return "TIMER_INT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiWatchdogActionEventT( string s )
    {
        if ( s == "NO_ACTION" ) {
            return HpiConst.SAHPI_WAE_NO_ACTION;
        }
        if ( s == "RESET" ) {
            return HpiConst.SAHPI_WAE_RESET;
        }
        if ( s == "POWER_DOWN" ) {
            return HpiConst.SAHPI_WAE_POWER_DOWN;
        }
        if ( s == "POWER_CYCLE" ) {
            return HpiConst.SAHPI_WAE_POWER_CYCLE;
        }
        if ( s == "TIMER_INT" ) {
            return HpiConst.SAHPI_WAE_TIMER_INT;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiWatchdogPretimerInterruptT
     */
    public static string FromSaHpiWatchdogPretimerInterruptT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_WPI_NONE:
                return "NONE";
            case HpiConst.SAHPI_WPI_SMI:
                return "SMI";
            case HpiConst.SAHPI_WPI_NMI:
                return "NMI";
            case HpiConst.SAHPI_WPI_MESSAGE_INTERRUPT:
                return "MESSAGE_INTERRUPT";
            case HpiConst.SAHPI_WPI_OEM:
                return "OEM";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiWatchdogPretimerInterruptT( string s )
    {
        if ( s == "NONE" ) {
            return HpiConst.SAHPI_WPI_NONE;
        }
        if ( s == "SMI" ) {
            return HpiConst.SAHPI_WPI_SMI;
        }
        if ( s == "NMI" ) {
            return HpiConst.SAHPI_WPI_NMI;
        }
        if ( s == "MESSAGE_INTERRUPT" ) {
            return HpiConst.SAHPI_WPI_MESSAGE_INTERRUPT;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_WPI_OEM;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiWatchdogTimerUseT
     */
    public static string FromSaHpiWatchdogTimerUseT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_WTU_NONE:
                return "NONE";
            case HpiConst.SAHPI_WTU_BIOS_FRB2:
                return "BIOS_FRB2";
            case HpiConst.SAHPI_WTU_BIOS_POST:
                return "BIOS_POST";
            case HpiConst.SAHPI_WTU_OS_LOAD:
                return "OS_LOAD";
            case HpiConst.SAHPI_WTU_SMS_OS:
                return "SMS_OS";
            case HpiConst.SAHPI_WTU_OEM:
                return "OEM";
            case HpiConst.SAHPI_WTU_UNSPECIFIED:
                return "UNSPECIFIED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiWatchdogTimerUseT( string s )
    {
        if ( s == "NONE" ) {
            return HpiConst.SAHPI_WTU_NONE;
        }
        if ( s == "BIOS_FRB2" ) {
            return HpiConst.SAHPI_WTU_BIOS_FRB2;
        }
        if ( s == "BIOS_POST" ) {
            return HpiConst.SAHPI_WTU_BIOS_POST;
        }
        if ( s == "OS_LOAD" ) {
            return HpiConst.SAHPI_WTU_OS_LOAD;
        }
        if ( s == "SMS_OS" ) {
            return HpiConst.SAHPI_WTU_SMS_OS;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_WTU_OEM;
        }
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_WTU_UNSPECIFIED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDimiTestServiceImpactT
     */
    public static string FromSaHpiDimiTestServiceImpactT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DIMITEST_NONDEGRADING:
                return "NONDEGRADING";
            case HpiConst.SAHPI_DIMITEST_DEGRADING:
                return "DEGRADING";
            case HpiConst.SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL:
                return "VENDOR_DEFINED_LEVEL";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDimiTestServiceImpactT( string s )
    {
        if ( s == "NONDEGRADING" ) {
            return HpiConst.SAHPI_DIMITEST_NONDEGRADING;
        }
        if ( s == "DEGRADING" ) {
            return HpiConst.SAHPI_DIMITEST_DEGRADING;
        }
        if ( s == "VENDOR_DEFINED_LEVEL" ) {
            return HpiConst.SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDimiTestRunStatusT
     */
    public static string FromSaHpiDimiTestRunStatusT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DIMITEST_STATUS_NOT_RUN:
                return "NOT_RUN";
            case HpiConst.SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS:
                return "FINISHED_NO_ERRORS";
            case HpiConst.SAHPI_DIMITEST_STATUS_FINISHED_ERRORS:
                return "FINISHED_ERRORS";
            case HpiConst.SAHPI_DIMITEST_STATUS_CANCELED:
                return "CANCELED";
            case HpiConst.SAHPI_DIMITEST_STATUS_RUNNING:
                return "RUNNING";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDimiTestRunStatusT( string s )
    {
        if ( s == "NOT_RUN" ) {
            return HpiConst.SAHPI_DIMITEST_STATUS_NOT_RUN;
        }
        if ( s == "FINISHED_NO_ERRORS" ) {
            return HpiConst.SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS;
        }
        if ( s == "FINISHED_ERRORS" ) {
            return HpiConst.SAHPI_DIMITEST_STATUS_FINISHED_ERRORS;
        }
        if ( s == "CANCELED" ) {
            return HpiConst.SAHPI_DIMITEST_STATUS_CANCELED;
        }
        if ( s == "RUNNING" ) {
            return HpiConst.SAHPI_DIMITEST_STATUS_RUNNING;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDimiTestErrCodeT
     */
    public static string FromSaHpiDimiTestErrCodeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DIMITEST_STATUSERR_NOERR:
                return "NOERR";
            case HpiConst.SAHPI_DIMITEST_STATUSERR_RUNERR:
                return "RUNERR";
            case HpiConst.SAHPI_DIMITEST_STATUSERR_UNDEF:
                return "UNDEF";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDimiTestErrCodeT( string s )
    {
        if ( s == "NOERR" ) {
            return HpiConst.SAHPI_DIMITEST_STATUSERR_NOERR;
        }
        if ( s == "RUNERR" ) {
            return HpiConst.SAHPI_DIMITEST_STATUSERR_RUNERR;
        }
        if ( s == "UNDEF" ) {
            return HpiConst.SAHPI_DIMITEST_STATUSERR_UNDEF;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDimiTestParamTypeT
     */
    public static string FromSaHpiDimiTestParamTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
                return "BOOLEAN";
            case HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32:
                return "INT32";
            case HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
                return "FLOAT64";
            case HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT:
                return "TEXT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDimiTestParamTypeT( string s )
    {
        if ( s == "BOOLEAN" ) {
            return HpiConst.SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN;
        }
        if ( s == "INT32" ) {
            return HpiConst.SAHPI_DIMITEST_PARAM_TYPE_INT32;
        }
        if ( s == "FLOAT64" ) {
            return HpiConst.SAHPI_DIMITEST_PARAM_TYPE_FLOAT64;
        }
        if ( s == "TEXT" ) {
            return HpiConst.SAHPI_DIMITEST_PARAM_TYPE_TEXT;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDimiReadyT
     */
    public static string FromSaHpiDimiReadyT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DIMI_READY:
                return "READY";
            case HpiConst.SAHPI_DIMI_WRONG_STATE:
                return "WRONG_STATE";
            case HpiConst.SAHPI_DIMI_BUSY:
                return "BUSY";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDimiReadyT( string s )
    {
        if ( s == "READY" ) {
            return HpiConst.SAHPI_DIMI_READY;
        }
        if ( s == "WRONG_STATE" ) {
            return HpiConst.SAHPI_DIMI_WRONG_STATE;
        }
        if ( s == "BUSY" ) {
            return HpiConst.SAHPI_DIMI_BUSY;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiSpecInfoTypeT
     */
    public static string FromSaHpiFumiSpecInfoTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_SPEC_INFO_NONE:
                return "NONE";
            case HpiConst.SAHPI_FUMI_SPEC_INFO_SAF_DEFINED:
                return "SAF_DEFINED";
            case HpiConst.SAHPI_FUMI_SPEC_INFO_OEM_DEFINED:
                return "OEM_DEFINED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiSpecInfoTypeT( string s )
    {
        if ( s == "NONE" ) {
            return HpiConst.SAHPI_FUMI_SPEC_INFO_NONE;
        }
        if ( s == "SAF_DEFINED" ) {
            return HpiConst.SAHPI_FUMI_SPEC_INFO_SAF_DEFINED;
        }
        if ( s == "OEM_DEFINED" ) {
            return HpiConst.SAHPI_FUMI_SPEC_INFO_OEM_DEFINED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiSafDefinedSpecIdT
     */
    public static string FromSaHpiFumiSafDefinedSpecIdT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_SPEC_HPM1:
                return "";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiSafDefinedSpecIdT( string s )
    {
        if ( s == "" ) {
            return HpiConst.SAHPI_FUMI_SPEC_HPM1;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiServiceImpactT
     */
    public static string FromSaHpiFumiServiceImpactT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_PROCESS_NONDEGRADING:
                return "NONDEGRADING";
            case HpiConst.SAHPI_FUMI_PROCESS_DEGRADING:
                return "DEGRADING";
            case HpiConst.SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL:
                return "VENDOR_DEFINED_IMPACT_LEVEL";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiServiceImpactT( string s )
    {
        if ( s == "NONDEGRADING" ) {
            return HpiConst.SAHPI_FUMI_PROCESS_NONDEGRADING;
        }
        if ( s == "DEGRADING" ) {
            return HpiConst.SAHPI_FUMI_PROCESS_DEGRADING;
        }
        if ( s == "VENDOR_DEFINED_IMPACT_LEVEL" ) {
            return HpiConst.SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiSourceStatusT
     */
    public static string FromSaHpiFumiSourceStatusT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_SRC_VALID:
                return "VALID";
            case HpiConst.SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED:
                return "PROTOCOL_NOT_SUPPORTED";
            case HpiConst.SAHPI_FUMI_SRC_UNREACHABLE:
                return "UNREACHABLE";
            case HpiConst.SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED:
                return "VALIDATION_NOT_STARTED";
            case HpiConst.SAHPI_FUMI_SRC_VALIDATION_INITIATED:
                return "VALIDATION_INITIATED";
            case HpiConst.SAHPI_FUMI_SRC_VALIDATION_FAIL:
                return "VALIDATION_FAIL";
            case HpiConst.SAHPI_FUMI_SRC_TYPE_MISMATCH:
                return "TYPE_MISMATCH";
            case HpiConst.SAHPI_FUMI_SRC_INVALID:
                return "INVALID";
            case HpiConst.SAHPI_FUMI_SRC_VALIDITY_UNKNOWN:
                return "VALIDITY_UNKNOWN";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiSourceStatusT( string s )
    {
        if ( s == "VALID" ) {
            return HpiConst.SAHPI_FUMI_SRC_VALID;
        }
        if ( s == "PROTOCOL_NOT_SUPPORTED" ) {
            return HpiConst.SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED;
        }
        if ( s == "UNREACHABLE" ) {
            return HpiConst.SAHPI_FUMI_SRC_UNREACHABLE;
        }
        if ( s == "VALIDATION_NOT_STARTED" ) {
            return HpiConst.SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED;
        }
        if ( s == "VALIDATION_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_SRC_VALIDATION_INITIATED;
        }
        if ( s == "VALIDATION_FAIL" ) {
            return HpiConst.SAHPI_FUMI_SRC_VALIDATION_FAIL;
        }
        if ( s == "TYPE_MISMATCH" ) {
            return HpiConst.SAHPI_FUMI_SRC_TYPE_MISMATCH;
        }
        if ( s == "INVALID" ) {
            return HpiConst.SAHPI_FUMI_SRC_INVALID;
        }
        if ( s == "VALIDITY_UNKNOWN" ) {
            return HpiConst.SAHPI_FUMI_SRC_VALIDITY_UNKNOWN;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiBankStateT
     */
    public static string FromSaHpiFumiBankStateT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_BANK_VALID:
                return "VALID";
            case HpiConst.SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS:
                return "UPGRADE_IN_PROGRESS";
            case HpiConst.SAHPI_FUMI_BANK_CORRUPTED:
                return "CORRUPTED";
            case HpiConst.SAHPI_FUMI_BANK_ACTIVE:
                return "ACTIVE";
            case HpiConst.SAHPI_FUMI_BANK_BUSY:
                return "BUSY";
            case HpiConst.SAHPI_FUMI_BANK_UNKNOWN:
                return "UNKNOWN";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiBankStateT( string s )
    {
        if ( s == "VALID" ) {
            return HpiConst.SAHPI_FUMI_BANK_VALID;
        }
        if ( s == "UPGRADE_IN_PROGRESS" ) {
            return HpiConst.SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS;
        }
        if ( s == "CORRUPTED" ) {
            return HpiConst.SAHPI_FUMI_BANK_CORRUPTED;
        }
        if ( s == "ACTIVE" ) {
            return HpiConst.SAHPI_FUMI_BANK_ACTIVE;
        }
        if ( s == "BUSY" ) {
            return HpiConst.SAHPI_FUMI_BANK_BUSY;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.SAHPI_FUMI_BANK_UNKNOWN;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiFumiUpgradeStatusT
     */
    public static string FromSaHpiFumiUpgradeStatusT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_FUMI_OPERATION_NOTSTARTED:
                return "OPERATION_NOTSTARTED";
            case HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_INITIATED:
                return "SOURCE_VALIDATION_INITIATED";
            case HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_FAILED:
                return "SOURCE_VALIDATION_FAILED";
            case HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_DONE:
                return "SOURCE_VALIDATION_DONE";
            case HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED:
                return "SOURCE_VALIDATION_CANCELLED";
            case HpiConst.SAHPI_FUMI_INSTALL_INITIATED:
                return "INSTALL_INITIATED";
            case HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED:
                return "INSTALL_FAILED_ROLLBACK_NEEDED";
            case HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED:
                return "INSTALL_FAILED_ROLLBACK_INITIATED";
            case HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE:
                return "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE";
            case HpiConst.SAHPI_FUMI_INSTALL_DONE:
                return "INSTALL_DONE";
            case HpiConst.SAHPI_FUMI_INSTALL_CANCELLED:
                return "INSTALL_CANCELLED";
            case HpiConst.SAHPI_FUMI_ROLLBACK_INITIATED:
                return "ROLLBACK_INITIATED";
            case HpiConst.SAHPI_FUMI_ROLLBACK_FAILED:
                return "ROLLBACK_FAILED";
            case HpiConst.SAHPI_FUMI_ROLLBACK_DONE:
                return "ROLLBACK_DONE";
            case HpiConst.SAHPI_FUMI_ROLLBACK_CANCELLED:
                return "ROLLBACK_CANCELLED";
            case HpiConst.SAHPI_FUMI_BACKUP_INITIATED:
                return "BACKUP_INITIATED";
            case HpiConst.SAHPI_FUMI_BACKUP_FAILED:
                return "BACKUP_FAILED";
            case HpiConst.SAHPI_FUMI_BACKUP_DONE:
                return "BACKUP_DONE";
            case HpiConst.SAHPI_FUMI_BACKUP_CANCELLED:
                return "BACKUP_CANCELLED";
            case HpiConst.SAHPI_FUMI_BANK_COPY_INITIATED:
                return "BANK_COPY_INITIATED";
            case HpiConst.SAHPI_FUMI_BANK_COPY_FAILED:
                return "BANK_COPY_FAILED";
            case HpiConst.SAHPI_FUMI_BANK_COPY_DONE:
                return "BANK_COPY_DONE";
            case HpiConst.SAHPI_FUMI_BANK_COPY_CANCELLED:
                return "BANK_COPY_CANCELLED";
            case HpiConst.SAHPI_FUMI_TARGET_VERIFY_INITIATED:
                return "TARGET_VERIFY_INITIATED";
            case HpiConst.SAHPI_FUMI_TARGET_VERIFY_FAILED:
                return "TARGET_VERIFY_FAILED";
            case HpiConst.SAHPI_FUMI_TARGET_VERIFY_DONE:
                return "TARGET_VERIFY_DONE";
            case HpiConst.SAHPI_FUMI_TARGET_VERIFY_CANCELLED:
                return "TARGET_VERIFY_CANCELLED";
            case HpiConst.SAHPI_FUMI_ACTIVATE_INITIATED:
                return "ACTIVATE_INITIATED";
            case HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED:
                return "ACTIVATE_FAILED_ROLLBACK_NEEDED";
            case HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED:
                return "ACTIVATE_FAILED_ROLLBACK_INITIATED";
            case HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE:
                return "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE";
            case HpiConst.SAHPI_FUMI_ACTIVATE_DONE:
                return "ACTIVATE_DONE";
            case HpiConst.SAHPI_FUMI_ACTIVATE_CANCELLED:
                return "ACTIVATE_CANCELLED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiFumiUpgradeStatusT( string s )
    {
        if ( s == "OPERATION_NOTSTARTED" ) {
            return HpiConst.SAHPI_FUMI_OPERATION_NOTSTARTED;
        }
        if ( s == "SOURCE_VALIDATION_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_INITIATED;
        }
        if ( s == "SOURCE_VALIDATION_FAILED" ) {
            return HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_FAILED;
        }
        if ( s == "SOURCE_VALIDATION_DONE" ) {
            return HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_DONE;
        }
        if ( s == "SOURCE_VALIDATION_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED;
        }
        if ( s == "INSTALL_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_INITIATED;
        }
        if ( s == "INSTALL_FAILED_ROLLBACK_NEEDED" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED;
        }
        if ( s == "INSTALL_FAILED_ROLLBACK_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED;
        }
        if ( s == "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE;
        }
        if ( s == "INSTALL_DONE" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_DONE;
        }
        if ( s == "INSTALL_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_INSTALL_CANCELLED;
        }
        if ( s == "ROLLBACK_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_ROLLBACK_INITIATED;
        }
        if ( s == "ROLLBACK_FAILED" ) {
            return HpiConst.SAHPI_FUMI_ROLLBACK_FAILED;
        }
        if ( s == "ROLLBACK_DONE" ) {
            return HpiConst.SAHPI_FUMI_ROLLBACK_DONE;
        }
        if ( s == "ROLLBACK_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_ROLLBACK_CANCELLED;
        }
        if ( s == "BACKUP_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_BACKUP_INITIATED;
        }
        if ( s == "BACKUP_FAILED" ) {
            return HpiConst.SAHPI_FUMI_BACKUP_FAILED;
        }
        if ( s == "BACKUP_DONE" ) {
            return HpiConst.SAHPI_FUMI_BACKUP_DONE;
        }
        if ( s == "BACKUP_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_BACKUP_CANCELLED;
        }
        if ( s == "BANK_COPY_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_BANK_COPY_INITIATED;
        }
        if ( s == "BANK_COPY_FAILED" ) {
            return HpiConst.SAHPI_FUMI_BANK_COPY_FAILED;
        }
        if ( s == "BANK_COPY_DONE" ) {
            return HpiConst.SAHPI_FUMI_BANK_COPY_DONE;
        }
        if ( s == "BANK_COPY_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_BANK_COPY_CANCELLED;
        }
        if ( s == "TARGET_VERIFY_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_TARGET_VERIFY_INITIATED;
        }
        if ( s == "TARGET_VERIFY_FAILED" ) {
            return HpiConst.SAHPI_FUMI_TARGET_VERIFY_FAILED;
        }
        if ( s == "TARGET_VERIFY_DONE" ) {
            return HpiConst.SAHPI_FUMI_TARGET_VERIFY_DONE;
        }
        if ( s == "TARGET_VERIFY_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_TARGET_VERIFY_CANCELLED;
        }
        if ( s == "ACTIVATE_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_INITIATED;
        }
        if ( s == "ACTIVATE_FAILED_ROLLBACK_NEEDED" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED;
        }
        if ( s == "ACTIVATE_FAILED_ROLLBACK_INITIATED" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED;
        }
        if ( s == "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE;
        }
        if ( s == "ACTIVATE_DONE" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_DONE;
        }
        if ( s == "ACTIVATE_CANCELLED" ) {
            return HpiConst.SAHPI_FUMI_ACTIVATE_CANCELLED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiHsIndicatorStateT
     */
    public static string FromSaHpiHsIndicatorStateT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_HS_INDICATOR_OFF:
                return "FF";
            case HpiConst.SAHPI_HS_INDICATOR_ON:
                return "N";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiHsIndicatorStateT( string s )
    {
        if ( s == "FF" ) {
            return HpiConst.SAHPI_HS_INDICATOR_OFF;
        }
        if ( s == "N" ) {
            return HpiConst.SAHPI_HS_INDICATOR_ON;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiHsActionT
     */
    public static string FromSaHpiHsActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_HS_ACTION_INSERTION:
                return "INSERTION";
            case HpiConst.SAHPI_HS_ACTION_EXTRACTION:
                return "EXTRACTION";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiHsActionT( string s )
    {
        if ( s == "INSERTION" ) {
            return HpiConst.SAHPI_HS_ACTION_INSERTION;
        }
        if ( s == "EXTRACTION" ) {
            return HpiConst.SAHPI_HS_ACTION_EXTRACTION;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiHsStateT
     */
    public static string FromSaHpiHsStateT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_HS_STATE_INACTIVE:
                return "INACTIVE";
            case HpiConst.SAHPI_HS_STATE_INSERTION_PENDING:
                return "INSERTION_PENDING";
            case HpiConst.SAHPI_HS_STATE_ACTIVE:
                return "ACTIVE";
            case HpiConst.SAHPI_HS_STATE_EXTRACTION_PENDING:
                return "EXTRACTION_PENDING";
            case HpiConst.SAHPI_HS_STATE_NOT_PRESENT:
                return "NOT_PRESENT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiHsStateT( string s )
    {
        if ( s == "INACTIVE" ) {
            return HpiConst.SAHPI_HS_STATE_INACTIVE;
        }
        if ( s == "INSERTION_PENDING" ) {
            return HpiConst.SAHPI_HS_STATE_INSERTION_PENDING;
        }
        if ( s == "ACTIVE" ) {
            return HpiConst.SAHPI_HS_STATE_ACTIVE;
        }
        if ( s == "EXTRACTION_PENDING" ) {
            return HpiConst.SAHPI_HS_STATE_EXTRACTION_PENDING;
        }
        if ( s == "NOT_PRESENT" ) {
            return HpiConst.SAHPI_HS_STATE_NOT_PRESENT;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiHsCauseOfStateChangeT
     */
    public static string FromSaHpiHsCauseOfStateChangeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_HS_CAUSE_AUTO_POLICY:
                return "AUTO_POLICY";
            case HpiConst.SAHPI_HS_CAUSE_EXT_SOFTWARE:
                return "EXT_SOFTWARE";
            case HpiConst.SAHPI_HS_CAUSE_OPERATOR_INIT:
                return "OPERATOR_INIT";
            case HpiConst.SAHPI_HS_CAUSE_USER_UPDATE:
                return "USER_UPDATE";
            case HpiConst.SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION:
                return "UNEXPECTED_DEACTIVATION";
            case HpiConst.SAHPI_HS_CAUSE_SURPRISE_EXTRACTION:
                return "SURPRISE_EXTRACTION";
            case HpiConst.SAHPI_HS_CAUSE_EXTRACTION_UPDATE:
                return "EXTRACTION_UPDATE";
            case HpiConst.SAHPI_HS_CAUSE_HARDWARE_FAULT:
                return "HARDWARE_FAULT";
            case HpiConst.SAHPI_HS_CAUSE_CONTAINING_FRU:
                return "CONTAINING_FRU";
            case HpiConst.SAHPI_HS_CAUSE_UNKNOWN:
                return "UNKNOWN";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiHsCauseOfStateChangeT( string s )
    {
        if ( s == "AUTO_POLICY" ) {
            return HpiConst.SAHPI_HS_CAUSE_AUTO_POLICY;
        }
        if ( s == "EXT_SOFTWARE" ) {
            return HpiConst.SAHPI_HS_CAUSE_EXT_SOFTWARE;
        }
        if ( s == "OPERATOR_INIT" ) {
            return HpiConst.SAHPI_HS_CAUSE_OPERATOR_INIT;
        }
        if ( s == "USER_UPDATE" ) {
            return HpiConst.SAHPI_HS_CAUSE_USER_UPDATE;
        }
        if ( s == "UNEXPECTED_DEACTIVATION" ) {
            return HpiConst.SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
        }
        if ( s == "SURPRISE_EXTRACTION" ) {
            return HpiConst.SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
        }
        if ( s == "EXTRACTION_UPDATE" ) {
            return HpiConst.SAHPI_HS_CAUSE_EXTRACTION_UPDATE;
        }
        if ( s == "HARDWARE_FAULT" ) {
            return HpiConst.SAHPI_HS_CAUSE_HARDWARE_FAULT;
        }
        if ( s == "CONTAINING_FRU" ) {
            return HpiConst.SAHPI_HS_CAUSE_CONTAINING_FRU;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.SAHPI_HS_CAUSE_UNKNOWN;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSeverityT
     */
    public static string FromSaHpiSeverityT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_CRITICAL:
                return "CRITICAL";
            case HpiConst.SAHPI_MAJOR:
                return "MAJOR";
            case HpiConst.SAHPI_MINOR:
                return "MINOR";
            case HpiConst.SAHPI_INFORMATIONAL:
                return "INFORMATIONAL";
            case HpiConst.SAHPI_OK:
                return "OK";
            case HpiConst.SAHPI_DEBUG:
                return "DEBUG";
            case HpiConst.SAHPI_ALL_SEVERITIES:
                return "ALL_SEVERITIES";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSeverityT( string s )
    {
        if ( s == "CRITICAL" ) {
            return HpiConst.SAHPI_CRITICAL;
        }
        if ( s == "MAJOR" ) {
            return HpiConst.SAHPI_MAJOR;
        }
        if ( s == "MINOR" ) {
            return HpiConst.SAHPI_MINOR;
        }
        if ( s == "INFORMATIONAL" ) {
            return HpiConst.SAHPI_INFORMATIONAL;
        }
        if ( s == "OK" ) {
            return HpiConst.SAHPI_OK;
        }
        if ( s == "DEBUG" ) {
            return HpiConst.SAHPI_DEBUG;
        }
        if ( s == "ALL_SEVERITIES" ) {
            return HpiConst.SAHPI_ALL_SEVERITIES;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiResourceEventTypeT
     */
    public static string FromSaHpiResourceEventTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_RESE_RESOURCE_FAILURE:
                return "FAILURE";
            case HpiConst.SAHPI_RESE_RESOURCE_RESTORED:
                return "RESTORED";
            case HpiConst.SAHPI_RESE_RESOURCE_ADDED:
                return "ADDED";
            case HpiConst.SAHPI_RESE_RESOURCE_REMOVED:
                return "REMOVED";
            case HpiConst.SAHPI_RESE_RESOURCE_INACCESSIBLE:
                return "INACCESSIBLE";
            case HpiConst.SAHPI_RESE_RESOURCE_UPDATED:
                return "UPDATED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiResourceEventTypeT( string s )
    {
        if ( s == "FAILURE" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_FAILURE;
        }
        if ( s == "RESTORED" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_RESTORED;
        }
        if ( s == "ADDED" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_ADDED;
        }
        if ( s == "REMOVED" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_REMOVED;
        }
        if ( s == "INACCESSIBLE" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_INACCESSIBLE;
        }
        if ( s == "UPDATED" ) {
            return HpiConst.SAHPI_RESE_RESOURCE_UPDATED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiDomainEventTypeT
     */
    public static string FromSaHpiDomainEventTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DOMAIN_REF_ADDED:
                return "ADDED";
            case HpiConst.SAHPI_DOMAIN_REF_REMOVED:
                return "REMOVED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiDomainEventTypeT( string s )
    {
        if ( s == "ADDED" ) {
            return HpiConst.SAHPI_DOMAIN_REF_ADDED;
        }
        if ( s == "REMOVED" ) {
            return HpiConst.SAHPI_DOMAIN_REF_REMOVED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiSwEventTypeT
     */
    public static string FromSaHpiSwEventTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_HPIE_AUDIT:
                return "AUDIT";
            case HpiConst.SAHPI_HPIE_STARTUP:
                return "STARTUP";
            case HpiConst.SAHPI_HPIE_OTHER:
                return "OTHER";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiSwEventTypeT( string s )
    {
        if ( s == "AUDIT" ) {
            return HpiConst.SAHPI_HPIE_AUDIT;
        }
        if ( s == "STARTUP" ) {
            return HpiConst.SAHPI_HPIE_STARTUP;
        }
        if ( s == "OTHER" ) {
            return HpiConst.SAHPI_HPIE_OTHER;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiEventTypeT
     */
    public static string FromSaHpiEventTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_ET_RESOURCE:
                return "RESOURCE";
            case HpiConst.SAHPI_ET_DOMAIN:
                return "DOMAIN";
            case HpiConst.SAHPI_ET_SENSOR:
                return "SENSOR";
            case HpiConst.SAHPI_ET_SENSOR_ENABLE_CHANGE:
                return "SENSOR_ENABLE_CHANGE";
            case HpiConst.SAHPI_ET_HOTSWAP:
                return "HOTSWAP";
            case HpiConst.SAHPI_ET_WATCHDOG:
                return "WATCHDOG";
            case HpiConst.SAHPI_ET_HPI_SW:
                return "HPI_SW";
            case HpiConst.SAHPI_ET_OEM:
                return "OEM";
            case HpiConst.SAHPI_ET_USER:
                return "USER";
            case HpiConst.SAHPI_ET_DIMI:
                return "DIMI";
            case HpiConst.SAHPI_ET_DIMI_UPDATE:
                return "DIMI_UPDATE";
            case HpiConst.SAHPI_ET_FUMI:
                return "FUMI";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiEventTypeT( string s )
    {
        if ( s == "RESOURCE" ) {
            return HpiConst.SAHPI_ET_RESOURCE;
        }
        if ( s == "DOMAIN" ) {
            return HpiConst.SAHPI_ET_DOMAIN;
        }
        if ( s == "SENSOR" ) {
            return HpiConst.SAHPI_ET_SENSOR;
        }
        if ( s == "SENSOR_ENABLE_CHANGE" ) {
            return HpiConst.SAHPI_ET_SENSOR_ENABLE_CHANGE;
        }
        if ( s == "HOTSWAP" ) {
            return HpiConst.SAHPI_ET_HOTSWAP;
        }
        if ( s == "WATCHDOG" ) {
            return HpiConst.SAHPI_ET_WATCHDOG;
        }
        if ( s == "HPI_SW" ) {
            return HpiConst.SAHPI_ET_HPI_SW;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_ET_OEM;
        }
        if ( s == "USER" ) {
            return HpiConst.SAHPI_ET_USER;
        }
        if ( s == "DIMI" ) {
            return HpiConst.SAHPI_ET_DIMI;
        }
        if ( s == "DIMI_UPDATE" ) {
            return HpiConst.SAHPI_ET_DIMI_UPDATE;
        }
        if ( s == "FUMI" ) {
            return HpiConst.SAHPI_ET_FUMI;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiStatusCondTypeT
     */
    public static string FromSaHpiStatusCondTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_STATUS_COND_TYPE_SENSOR:
                return "SENSOR";
            case HpiConst.SAHPI_STATUS_COND_TYPE_RESOURCE:
                return "RESOURCE";
            case HpiConst.SAHPI_STATUS_COND_TYPE_OEM:
                return "OEM";
            case HpiConst.SAHPI_STATUS_COND_TYPE_USER:
                return "USER";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiStatusCondTypeT( string s )
    {
        if ( s == "SENSOR" ) {
            return HpiConst.SAHPI_STATUS_COND_TYPE_SENSOR;
        }
        if ( s == "RESOURCE" ) {
            return HpiConst.SAHPI_STATUS_COND_TYPE_RESOURCE;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_STATUS_COND_TYPE_OEM;
        }
        if ( s == "USER" ) {
            return HpiConst.SAHPI_STATUS_COND_TYPE_USER;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiAnnunciatorModeT
     */
    public static string FromSaHpiAnnunciatorModeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_ANNUNCIATOR_MODE_AUTO:
                return "AUTO";
            case HpiConst.SAHPI_ANNUNCIATOR_MODE_USER:
                return "USER";
            case HpiConst.SAHPI_ANNUNCIATOR_MODE_SHARED:
                return "SHARED";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiAnnunciatorModeT( string s )
    {
        if ( s == "AUTO" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_MODE_AUTO;
        }
        if ( s == "USER" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_MODE_USER;
        }
        if ( s == "SHARED" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_MODE_SHARED;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiAnnunciatorTypeT
     */
    public static string FromSaHpiAnnunciatorTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_LED:
                return "LED";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE:
                return "DRY_CONTACT_CLOSURE";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_AUDIBLE:
                return "AUDIBLE";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY:
                return "LCD_DISPLAY";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_MESSAGE:
                return "MESSAGE";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_COMPOSITE:
                return "COMPOSITE";
            case HpiConst.SAHPI_ANNUNCIATOR_TYPE_OEM:
                return "OEM";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiAnnunciatorTypeT( string s )
    {
        if ( s == "LED" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_LED;
        }
        if ( s == "DRY_CONTACT_CLOSURE" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE;
        }
        if ( s == "AUDIBLE" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_AUDIBLE;
        }
        if ( s == "LCD_DISPLAY" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY;
        }
        if ( s == "MESSAGE" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_MESSAGE;
        }
        if ( s == "COMPOSITE" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_COMPOSITE;
        }
        if ( s == "OEM" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_TYPE_OEM;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiRdrTypeT
     */
    public static string FromSaHpiRdrTypeT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_NO_RECORD:
                return "NO_RECORD";
            case HpiConst.SAHPI_CTRL_RDR:
                return "CTRL_RDR";
            case HpiConst.SAHPI_SENSOR_RDR:
                return "SENSOR_RDR";
            case HpiConst.SAHPI_INVENTORY_RDR:
                return "INVENTORY_RDR";
            case HpiConst.SAHPI_WATCHDOG_RDR:
                return "WATCHDOG_RDR";
            case HpiConst.SAHPI_ANNUNCIATOR_RDR:
                return "ANNUNCIATOR_RDR";
            case HpiConst.SAHPI_DIMI_RDR:
                return "DIMI_RDR";
            case HpiConst.SAHPI_FUMI_RDR:
                return "FUMI_RDR";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiRdrTypeT( string s )
    {
        if ( s == "NO_RECORD" ) {
            return HpiConst.SAHPI_NO_RECORD;
        }
        if ( s == "CTRL_RDR" ) {
            return HpiConst.SAHPI_CTRL_RDR;
        }
        if ( s == "SENSOR_RDR" ) {
            return HpiConst.SAHPI_SENSOR_RDR;
        }
        if ( s == "INVENTORY_RDR" ) {
            return HpiConst.SAHPI_INVENTORY_RDR;
        }
        if ( s == "WATCHDOG_RDR" ) {
            return HpiConst.SAHPI_WATCHDOG_RDR;
        }
        if ( s == "ANNUNCIATOR_RDR" ) {
            return HpiConst.SAHPI_ANNUNCIATOR_RDR;
        }
        if ( s == "DIMI_RDR" ) {
            return HpiConst.SAHPI_DIMI_RDR;
        }
        if ( s == "FUMI_RDR" ) {
            return HpiConst.SAHPI_FUMI_RDR;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiParmActionT
     */
    public static string FromSaHpiParmActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_DEFAULT_PARM:
                return "DEFAULT_PARM";
            case HpiConst.SAHPI_SAVE_PARM:
                return "SAVE_PARM";
            case HpiConst.SAHPI_RESTORE_PARM:
                return "RESTORE_PARM";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiParmActionT( string s )
    {
        if ( s == "DEFAULT_PARM" ) {
            return HpiConst.SAHPI_DEFAULT_PARM;
        }
        if ( s == "SAVE_PARM" ) {
            return HpiConst.SAHPI_SAVE_PARM;
        }
        if ( s == "RESTORE_PARM" ) {
            return HpiConst.SAHPI_RESTORE_PARM;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiResetActionT
     */
    public static string FromSaHpiResetActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_COLD_RESET:
                return "COLD_RESET";
            case HpiConst.SAHPI_WARM_RESET:
                return "WARM_RESET";
            case HpiConst.SAHPI_RESET_ASSERT:
                return "RESET_ASSERT";
            case HpiConst.SAHPI_RESET_DEASSERT:
                return "RESET_DEASSERT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiResetActionT( string s )
    {
        if ( s == "COLD_RESET" ) {
            return HpiConst.SAHPI_COLD_RESET;
        }
        if ( s == "WARM_RESET" ) {
            return HpiConst.SAHPI_WARM_RESET;
        }
        if ( s == "RESET_ASSERT" ) {
            return HpiConst.SAHPI_RESET_ASSERT;
        }
        if ( s == "RESET_DEASSERT" ) {
            return HpiConst.SAHPI_RESET_DEASSERT;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiPowerStateT
     */
    public static string FromSaHpiPowerStateT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_POWER_OFF:
                return "OFF";
            case HpiConst.SAHPI_POWER_ON:
                return "ON";
            case HpiConst.SAHPI_POWER_CYCLE:
                return "CYCLE";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiPowerStateT( string s )
    {
        if ( s == "OFF" ) {
            return HpiConst.SAHPI_POWER_OFF;
        }
        if ( s == "ON" ) {
            return HpiConst.SAHPI_POWER_ON;
        }
        if ( s == "CYCLE" ) {
            return HpiConst.SAHPI_POWER_CYCLE;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiEventLogOverflowActionT
     */
    public static string FromSaHpiEventLogOverflowActionT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_EL_OVERFLOW_DROP:
                return "DROP";
            case HpiConst.SAHPI_EL_OVERFLOW_OVERWRITE:
                return "OVERWRITE";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiEventLogOverflowActionT( string s )
    {
        if ( s == "DROP" ) {
            return HpiConst.SAHPI_EL_OVERFLOW_DROP;
        }
        if ( s == "OVERWRITE" ) {
            return HpiConst.SAHPI_EL_OVERFLOW_OVERWRITE;
        }

        throw new FormatException();
    }

    /**
     * For AtcaHpiLedColorT
     */
    public static string FromAtcaHpiLedColorT( long x )
    {
        switch( x ) {
            case HpiConst.ATCAHPI_LED_COLOR_RESERVED:
                return "RESERVED";
            case HpiConst.ATCAHPI_LED_COLOR_BLUE:
                return "BLUE";
            case HpiConst.ATCAHPI_LED_COLOR_RED:
                return "RED";
            case HpiConst.ATCAHPI_LED_COLOR_GREEN:
                return "GREEN";
            case HpiConst.ATCAHPI_LED_COLOR_AMBER:
                return "AMBER";
            case HpiConst.ATCAHPI_LED_COLOR_ORANGE:
                return "ORANGE";
            case HpiConst.ATCAHPI_LED_COLOR_WHITE:
                return "WHITE";
            case HpiConst.ATCAHPI_LED_COLOR_NO_CHANGE:
                return "NO_CHANGE";
            case HpiConst.ATCAHPI_LED_COLOR_USE_DEFAULT:
                return "USE_DEFAULT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToAtcaHpiLedColorT( string s )
    {
        if ( s == "RESERVED" ) {
            return HpiConst.ATCAHPI_LED_COLOR_RESERVED;
        }
        if ( s == "BLUE" ) {
            return HpiConst.ATCAHPI_LED_COLOR_BLUE;
        }
        if ( s == "RED" ) {
            return HpiConst.ATCAHPI_LED_COLOR_RED;
        }
        if ( s == "GREEN" ) {
            return HpiConst.ATCAHPI_LED_COLOR_GREEN;
        }
        if ( s == "AMBER" ) {
            return HpiConst.ATCAHPI_LED_COLOR_AMBER;
        }
        if ( s == "ORANGE" ) {
            return HpiConst.ATCAHPI_LED_COLOR_ORANGE;
        }
        if ( s == "WHITE" ) {
            return HpiConst.ATCAHPI_LED_COLOR_WHITE;
        }
        if ( s == "NO_CHANGE" ) {
            return HpiConst.ATCAHPI_LED_COLOR_NO_CHANGE;
        }
        if ( s == "USE_DEFAULT" ) {
            return HpiConst.ATCAHPI_LED_COLOR_USE_DEFAULT;
        }

        throw new FormatException();
    }

    /**
     * For AtcaHpiResourceLedModeT
     */
    public static string FromAtcaHpiResourceLedModeT( long x )
    {
        switch( x ) {
            case HpiConst.ATCAHPI_LED_AUTO:
                return "AUTO";
            case HpiConst.ATCAHPI_LED_MANUAL:
                return "MANUAL";
            case HpiConst.ATCAHPI_LED_LAMP_TEST:
                return "LAMP_TEST";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToAtcaHpiResourceLedModeT( string s )
    {
        if ( s == "AUTO" ) {
            return HpiConst.ATCAHPI_LED_AUTO;
        }
        if ( s == "MANUAL" ) {
            return HpiConst.ATCAHPI_LED_MANUAL;
        }
        if ( s == "LAMP_TEST" ) {
            return HpiConst.ATCAHPI_LED_LAMP_TEST;
        }

        throw new FormatException();
    }

    /**
     * For AtcaHpiLedBrSupportT
     */
    public static string FromAtcaHpiLedBrSupportT( long x )
    {
        switch( x ) {
            case HpiConst.ATCAHPI_LED_BR_SUPPORTED:
                return "SUPPORTED";
            case HpiConst.ATCAHPI_LED_BR_NOT_SUPPORTED:
                return "NOT_SUPPORTED";
            case HpiConst.ATCAHPI_LED_BR_UNKNOWN:
                return "UNKNOWN";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToAtcaHpiLedBrSupportT( string s )
    {
        if ( s == "SUPPORTED" ) {
            return HpiConst.ATCAHPI_LED_BR_SUPPORTED;
        }
        if ( s == "NOT_SUPPORTED" ) {
            return HpiConst.ATCAHPI_LED_BR_NOT_SUPPORTED;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.ATCAHPI_LED_BR_UNKNOWN;
        }

        throw new FormatException();
    }

    /**
     * For XtcaHpiLedColorT
     */
    public static string FromXtcaHpiLedColorT( long x )
    {
        switch( x ) {
            case HpiConst.XTCAHPI_LED_COLOR_RESERVED:
                return "RESERVED";
            case HpiConst.XTCAHPI_LED_COLOR_BLUE:
                return "BLUE";
            case HpiConst.XTCAHPI_LED_COLOR_RED:
                return "RED";
            case HpiConst.XTCAHPI_LED_COLOR_GREEN:
                return "GREEN";
            case HpiConst.XTCAHPI_LED_COLOR_AMBER:
                return "AMBER";
            case HpiConst.XTCAHPI_LED_COLOR_ORANGE:
                return "ORANGE";
            case HpiConst.XTCAHPI_LED_COLOR_WHITE:
                return "WHITE";
            case HpiConst.XTCAHPI_LED_COLOR_NO_CHANGE:
                return "NO_CHANGE";
            case HpiConst.XTCAHPI_LED_COLOR_USE_DEFAULT:
                return "USE_DEFAULT";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToXtcaHpiLedColorT( string s )
    {
        if ( s == "RESERVED" ) {
            return HpiConst.XTCAHPI_LED_COLOR_RESERVED;
        }
        if ( s == "BLUE" ) {
            return HpiConst.XTCAHPI_LED_COLOR_BLUE;
        }
        if ( s == "RED" ) {
            return HpiConst.XTCAHPI_LED_COLOR_RED;
        }
        if ( s == "GREEN" ) {
            return HpiConst.XTCAHPI_LED_COLOR_GREEN;
        }
        if ( s == "AMBER" ) {
            return HpiConst.XTCAHPI_LED_COLOR_AMBER;
        }
        if ( s == "ORANGE" ) {
            return HpiConst.XTCAHPI_LED_COLOR_ORANGE;
        }
        if ( s == "WHITE" ) {
            return HpiConst.XTCAHPI_LED_COLOR_WHITE;
        }
        if ( s == "NO_CHANGE" ) {
            return HpiConst.XTCAHPI_LED_COLOR_NO_CHANGE;
        }
        if ( s == "USE_DEFAULT" ) {
            return HpiConst.XTCAHPI_LED_COLOR_USE_DEFAULT;
        }

        throw new FormatException();
    }

    /**
     * For XtcaHpiResourceLedModeT
     */
    public static string FromXtcaHpiResourceLedModeT( long x )
    {
        switch( x ) {
            case HpiConst.XTCAHPI_LED_AUTO:
                return "AUTO";
            case HpiConst.XTCAHPI_LED_MANUAL:
                return "MANUAL";
            case HpiConst.XTCAHPI_LED_LAMP_TEST:
                return "LAMP_TEST";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToXtcaHpiResourceLedModeT( string s )
    {
        if ( s == "AUTO" ) {
            return HpiConst.XTCAHPI_LED_AUTO;
        }
        if ( s == "MANUAL" ) {
            return HpiConst.XTCAHPI_LED_MANUAL;
        }
        if ( s == "LAMP_TEST" ) {
            return HpiConst.XTCAHPI_LED_LAMP_TEST;
        }

        throw new FormatException();
    }

    /**
     * For XtcaHpiLedBrSupportT
     */
    public static string FromXtcaHpiLedBrSupportT( long x )
    {
        switch( x ) {
            case HpiConst.XTCAHPI_LED_BR_SUPPORTED:
                return "SUPPORTED";
            case HpiConst.XTCAHPI_LED_BR_NOT_SUPPORTED:
                return "NOT_SUPPORTED";
            case HpiConst.XTCAHPI_LED_BR_UNKNOWN:
                return "UNKNOWN";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToXtcaHpiLedBrSupportT( string s )
    {
        if ( s == "SUPPORTED" ) {
            return HpiConst.XTCAHPI_LED_BR_SUPPORTED;
        }
        if ( s == "NOT_SUPPORTED" ) {
            return HpiConst.XTCAHPI_LED_BR_NOT_SUPPORTED;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.XTCAHPI_LED_BR_UNKNOWN;
        }

        throw new FormatException();
    }

    /**
     * For SaErrorT
     */
    public static string FromSaErrorT( long x )
    {
        switch( x ) {
            case HpiConst.SA_ERR_HPI_OK:
                return "OK";
            case HpiConst.SA_ERR_HPI_ERROR:
                return "ERROR";
            case HpiConst.SA_ERR_HPI_UNSUPPORTED_API:
                return "UNSUPPORTED_API";
            case HpiConst.SA_ERR_HPI_BUSY:
                return "BUSY";
            case HpiConst.SA_ERR_HPI_INTERNAL_ERROR:
                return "INTERNAL_ERROR";
            case HpiConst.SA_ERR_HPI_INVALID_CMD:
                return "INVALID_CMD";
            case HpiConst.SA_ERR_HPI_TIMEOUT:
                return "TIMEOUT";
            case HpiConst.SA_ERR_HPI_OUT_OF_SPACE:
                return "OUT_OF_SPACE";
            case HpiConst.SA_ERR_HPI_OUT_OF_MEMORY:
                return "OUT_OF_MEMORY";
            case HpiConst.SA_ERR_HPI_INVALID_PARAMS:
                return "INVALID_PARAMS";
            case HpiConst.SA_ERR_HPI_INVALID_DATA:
                return "INVALID_DATA";
            case HpiConst.SA_ERR_HPI_NOT_PRESENT:
                return "NOT_PRESENT";
            case HpiConst.SA_ERR_HPI_NO_RESPONSE:
                return "NO_RESPONSE";
            case HpiConst.SA_ERR_HPI_DUPLICATE:
                return "DUPLICATE";
            case HpiConst.SA_ERR_HPI_INVALID_SESSION:
                return "INVALID_SESSION";
            case HpiConst.SA_ERR_HPI_INVALID_DOMAIN:
                return "INVALID_DOMAIN";
            case HpiConst.SA_ERR_HPI_INVALID_RESOURCE:
                return "INVALID_RESOURCE";
            case HpiConst.SA_ERR_HPI_INVALID_REQUEST:
                return "INVALID_REQUEST";
            case HpiConst.SA_ERR_HPI_ENTITY_NOT_PRESENT:
                return "ENTITY_NOT_PRESENT";
            case HpiConst.SA_ERR_HPI_READ_ONLY:
                return "READ_ONLY";
            case HpiConst.SA_ERR_HPI_CAPABILITY:
                return "CAPABILITY";
            case HpiConst.SA_ERR_HPI_UNKNOWN:
                return "UNKNOWN";
            case HpiConst.SA_ERR_HPI_INVALID_STATE:
                return "INVALID_STATE";
            case HpiConst.SA_ERR_HPI_UNSUPPORTED_PARAMS:
                return "UNSUPPORTED_PARAMS";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaErrorT( string s )
    {
        if ( s == "OK" ) {
            return HpiConst.SA_ERR_HPI_OK;
        }
        if ( s == "ERROR" ) {
            return HpiConst.SA_ERR_HPI_ERROR;
        }
        if ( s == "UNSUPPORTED_API" ) {
            return HpiConst.SA_ERR_HPI_UNSUPPORTED_API;
        }
        if ( s == "BUSY" ) {
            return HpiConst.SA_ERR_HPI_BUSY;
        }
        if ( s == "INTERNAL_ERROR" ) {
            return HpiConst.SA_ERR_HPI_INTERNAL_ERROR;
        }
        if ( s == "INVALID_CMD" ) {
            return HpiConst.SA_ERR_HPI_INVALID_CMD;
        }
        if ( s == "TIMEOUT" ) {
            return HpiConst.SA_ERR_HPI_TIMEOUT;
        }
        if ( s == "OUT_OF_SPACE" ) {
            return HpiConst.SA_ERR_HPI_OUT_OF_SPACE;
        }
        if ( s == "OUT_OF_MEMORY" ) {
            return HpiConst.SA_ERR_HPI_OUT_OF_MEMORY;
        }
        if ( s == "INVALID_PARAMS" ) {
            return HpiConst.SA_ERR_HPI_INVALID_PARAMS;
        }
        if ( s == "INVALID_DATA" ) {
            return HpiConst.SA_ERR_HPI_INVALID_DATA;
        }
        if ( s == "NOT_PRESENT" ) {
            return HpiConst.SA_ERR_HPI_NOT_PRESENT;
        }
        if ( s == "NO_RESPONSE" ) {
            return HpiConst.SA_ERR_HPI_NO_RESPONSE;
        }
        if ( s == "DUPLICATE" ) {
            return HpiConst.SA_ERR_HPI_DUPLICATE;
        }
        if ( s == "INVALID_SESSION" ) {
            return HpiConst.SA_ERR_HPI_INVALID_SESSION;
        }
        if ( s == "INVALID_DOMAIN" ) {
            return HpiConst.SA_ERR_HPI_INVALID_DOMAIN;
        }
        if ( s == "INVALID_RESOURCE" ) {
            return HpiConst.SA_ERR_HPI_INVALID_RESOURCE;
        }
        if ( s == "INVALID_REQUEST" ) {
            return HpiConst.SA_ERR_HPI_INVALID_REQUEST;
        }
        if ( s == "ENTITY_NOT_PRESENT" ) {
            return HpiConst.SA_ERR_HPI_ENTITY_NOT_PRESENT;
        }
        if ( s == "READ_ONLY" ) {
            return HpiConst.SA_ERR_HPI_READ_ONLY;
        }
        if ( s == "CAPABILITY" ) {
            return HpiConst.SA_ERR_HPI_CAPABILITY;
        }
        if ( s == "UNKNOWN" ) {
            return HpiConst.SA_ERR_HPI_UNKNOWN;
        }
        if ( s == "INVALID_STATE" ) {
            return HpiConst.SA_ERR_HPI_INVALID_STATE;
        }
        if ( s == "UNSUPPORTED_PARAMS" ) {
            return HpiConst.SA_ERR_HPI_UNSUPPORTED_PARAMS;
        }

        throw new FormatException();
    }

    /**
     * For SaHpiEventCategoryT
     */
    public static string FromSaHpiEventCategoryT( long x )
    {
        switch( x ) {
            case HpiConst.SAHPI_EC_UNSPECIFIED:
                return "UNSPECIFIED";
            case HpiConst.SAHPI_EC_THRESHOLD:
                return "THRESHOLD";
            case HpiConst.SAHPI_EC_USAGE:
                return "USAGE";
            case HpiConst.SAHPI_EC_STATE:
                return "STATE";
            case HpiConst.SAHPI_EC_PRED_FAIL:
                return "PRED_FAIL";
            case HpiConst.SAHPI_EC_LIMIT:
                return "LIMIT";
            case HpiConst.SAHPI_EC_PERFORMANCE:
                return "PERFORMANCE";
            case HpiConst.SAHPI_EC_SEVERITY:
                return "SEVERITY";
            case HpiConst.SAHPI_EC_PRESENCE:
                return "PRESENCE";
            case HpiConst.SAHPI_EC_ENABLE:
                return "ENABLE";
            case HpiConst.SAHPI_EC_AVAILABILITY:
                return "AVAILABILITY";
            case HpiConst.SAHPI_EC_REDUNDANCY:
                return "REDUNDANCY";
            case HpiConst.SAHPI_EC_SENSOR_SPECIFIC:
                return "SENSOR_SPECIFIC";
            case HpiConst.SAHPI_EC_GENERIC:
                return "GENERIC";
            default:
                return Convert.ToString( x );
        }
    }

    public static long ToSaHpiEventCategoryT( string s )
    {
        if ( s == "UNSPECIFIED" ) {
            return HpiConst.SAHPI_EC_UNSPECIFIED;
        }
        if ( s == "THRESHOLD" ) {
            return HpiConst.SAHPI_EC_THRESHOLD;
        }
        if ( s == "USAGE" ) {
            return HpiConst.SAHPI_EC_USAGE;
        }
        if ( s == "STATE" ) {
            return HpiConst.SAHPI_EC_STATE;
        }
        if ( s == "PRED_FAIL" ) {
            return HpiConst.SAHPI_EC_PRED_FAIL;
        }
        if ( s == "LIMIT" ) {
            return HpiConst.SAHPI_EC_LIMIT;
        }
        if ( s == "PERFORMANCE" ) {
            return HpiConst.SAHPI_EC_PERFORMANCE;
        }
        if ( s == "SEVERITY" ) {
            return HpiConst.SAHPI_EC_SEVERITY;
        }
        if ( s == "PRESENCE" ) {
            return HpiConst.SAHPI_EC_PRESENCE;
        }
        if ( s == "ENABLE" ) {
            return HpiConst.SAHPI_EC_ENABLE;
        }
        if ( s == "AVAILABILITY" ) {
            return HpiConst.SAHPI_EC_AVAILABILITY;
        }
        if ( s == "REDUNDANCY" ) {
            return HpiConst.SAHPI_EC_REDUNDANCY;
        }
        if ( s == "SENSOR_SPECIFIC" ) {
            return HpiConst.SAHPI_EC_SENSOR_SPECIFIC;
        }
        if ( s == "GENERIC" ) {
            return HpiConst.SAHPI_EC_GENERIC;
        }

        throw new FormatException();
    }

};


}; // namespace openhpi
}; // namespace org

