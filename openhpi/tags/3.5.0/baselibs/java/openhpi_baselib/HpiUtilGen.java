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

import java.lang.IllegalArgumentException;

import static org.openhpi.HpiDataTypes.*;


/**********************************************************
 * HPI Utility Functions (auto-generated)
 *********************************************************/
public class HpiUtilGen
{
    // Just to ensure nobody creates it
    protected HpiUtilGen()
    {
        // empty
    }

    /**********************************************************
     * Check Functions for HPI Complex Data Types
     *********************************************************/

    /**
     * check function for struct SaHpiTextBufferT
     */
    public static boolean check( SaHpiTextBufferT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Data == null ) {
            return false;
        }
        if ( x.Data.length != SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiEntityT
     */
    public static boolean check( SaHpiEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiEntityPathT
     */
    public static boolean check( SaHpiEntityPathT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Entry == null ) {
            return false;
        }
        if ( x.Entry.length != SAHPI_MAX_ENTITY_PATH ) {
            return false;
        }
        for ( int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
            if ( !check( x.Entry[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for union SaHpiSensorReadingUnionT
     */
    public static boolean check( SaHpiSensorReadingUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            if ( x.SensorBuffer == null ) {
                return false;
            }
        }
        if ( mod == SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            if ( x.SensorBuffer.length != SAHPI_SENSOR_BUFFER_LENGTH ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorReadingT
     */
    public static boolean check( SaHpiSensorReadingT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Value, x.Type ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorThresholdsT
     */
    public static boolean check( SaHpiSensorThresholdsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.LowCritical ) ) {
            return false;
        }
        if ( !check( x.LowMajor ) ) {
            return false;
        }
        if ( !check( x.LowMinor ) ) {
            return false;
        }
        if ( !check( x.UpCritical ) ) {
            return false;
        }
        if ( !check( x.UpMajor ) ) {
            return false;
        }
        if ( !check( x.UpMinor ) ) {
            return false;
        }
        if ( !check( x.PosThdHysteresis ) ) {
            return false;
        }
        if ( !check( x.NegThdHysteresis ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorRangeT
     */
    public static boolean check( SaHpiSensorRangeT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Max ) ) {
            return false;
        }
        if ( !check( x.Min ) ) {
            return false;
        }
        if ( !check( x.Nominal ) ) {
            return false;
        }
        if ( !check( x.NormalMax ) ) {
            return false;
        }
        if ( !check( x.NormalMin ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorDataFormatT
     */
    public static boolean check( SaHpiSensorDataFormatT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Range ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorThdDefnT
     */
    public static boolean check( SaHpiSensorThdDefnT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorRecT
     */
    public static boolean check( SaHpiSensorRecT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.DataFormat ) ) {
            return false;
        }
        if ( !check( x.ThresholdDefn ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlStateStreamT
     */
    public static boolean check( SaHpiCtrlStateStreamT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Stream == null ) {
            return false;
        }
        if ( x.Stream.length != SAHPI_CTRL_MAX_STREAM_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlStateTextT
     */
    public static boolean check( SaHpiCtrlStateTextT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Text ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlStateOemT
     */
    public static boolean check( SaHpiCtrlStateOemT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Body == null ) {
            return false;
        }
        if ( x.Body.length != SAHPI_CTRL_MAX_OEM_BODY_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiCtrlStateUnionT
     */
    public static boolean check( SaHpiCtrlStateUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            if ( !check( x.Stream ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            if ( !check( x.Text ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            if ( !check( x.Oem ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlStateT
     */
    public static boolean check( SaHpiCtrlStateT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.StateUnion, x.Type ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecDigitalT
     */
    public static boolean check( SaHpiCtrlRecDigitalT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecDiscreteT
     */
    public static boolean check( SaHpiCtrlRecDiscreteT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecAnalogT
     */
    public static boolean check( SaHpiCtrlRecAnalogT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecStreamT
     */
    public static boolean check( SaHpiCtrlRecStreamT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecTextT
     */
    public static boolean check( SaHpiCtrlRecTextT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecOemT
     */
    public static boolean check( SaHpiCtrlRecOemT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ConfigData == null ) {
            return false;
        }
        if ( x.ConfigData.length != SAHPI_CTRL_OEM_CONFIG_LENGTH ) {
            return false;
        }
        if ( !check( x.Default ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiCtrlRecUnionT
     */
    public static boolean check( SaHpiCtrlRecUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_CTRL_TYPE_DIGITAL ) {
            if ( !check( x.Digital ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_DISCRETE ) {
            if ( !check( x.Discrete ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_ANALOG ) {
            if ( !check( x.Analog ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_STREAM ) {
            if ( !check( x.Stream ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_TEXT ) {
            if ( !check( x.Text ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_CTRL_TYPE_OEM ) {
            if ( !check( x.Oem ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlDefaultModeT
     */
    public static boolean check( SaHpiCtrlDefaultModeT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiCtrlRecT
     */
    public static boolean check( SaHpiCtrlRecT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.TypeUnion, x.Type ) ) {
            return false;
        }
        if ( !check( x.DefaultMode ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiIdrFieldT
     */
    public static boolean check( SaHpiIdrFieldT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Field ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiIdrAreaHeaderT
     */
    public static boolean check( SaHpiIdrAreaHeaderT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiIdrInfoT
     */
    public static boolean check( SaHpiIdrInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiInventoryRecT
     */
    public static boolean check( SaHpiInventoryRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiWatchdogT
     */
    public static boolean check( SaHpiWatchdogT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiWatchdogRecT
     */
    public static boolean check( SaHpiWatchdogRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiTestAffectedEntityT
     */
    public static boolean check( SaHpiDimiTestAffectedEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.EntityImpacted ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiTestResultsT
     */
    public static boolean check( SaHpiDimiTestResultsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.TestResultString ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiDimiTestParamValueT
     */
    public static boolean check( SaHpiDimiTestParamValueT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            if ( !check( x.paramtext ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for union SaHpiDimiTestParameterValueUnionT
     */
    public static boolean check( SaHpiDimiTestParameterValueUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiTestParamsDefinitionT
     */
    public static boolean check( SaHpiDimiTestParamsDefinitionT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ParamName == null ) {
            return false;
        }
        if ( x.ParamName.length != SAHPI_DIMITEST_PARAM_NAME_LEN ) {
            return false;
        }
        if ( !check( x.ParamInfo ) ) {
            return false;
        }
        if ( !check( x.MinValue, x.ParamType ) ) {
            return false;
        }
        if ( !check( x.MaxValue, x.ParamType ) ) {
            return false;
        }
        if ( !check( x.DefaultParam, x.ParamType ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiTestT
     */
    public static boolean check( SaHpiDimiTestT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.TestName ) ) {
            return false;
        }
        if ( x.EntitiesImpacted == null ) {
            return false;
        }
        if ( x.EntitiesImpacted.length != SAHPI_DIMITEST_MAX_ENTITIESIMPACTED ) {
            return false;
        }
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
            if ( !check( x.EntitiesImpacted[i] ) ) {
                return false;
            }
        }
        if ( !check( x.ServiceOS ) ) {
            return false;
        }
        if ( x.TestParameters == null ) {
            return false;
        }
        if ( x.TestParameters.length != SAHPI_DIMITEST_MAX_PARAMETERS ) {
            return false;
        }
        for ( int i = 0; i < SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
            if ( !check( x.TestParameters[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiTestVariableParamsT
     */
    public static boolean check( SaHpiDimiTestVariableParamsT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ParamName == null ) {
            return false;
        }
        if ( x.ParamName.length != SAHPI_DIMITEST_PARAM_NAME_LEN ) {
            return false;
        }
        if ( !check( x.Value, x.ParamType ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiInfoT
     */
    public static boolean check( SaHpiDimiInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiRecT
     */
    public static boolean check( SaHpiDimiRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiSafDefinedSpecInfoT
     */
    public static boolean check( SaHpiFumiSafDefinedSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiOemDefinedSpecInfoT
     */
    public static boolean check( SaHpiFumiOemDefinedSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Body == null ) {
            return false;
        }
        if ( x.Body.length != SAHPI_FUMI_MAX_OEM_BODY_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiFumiSpecInfoTypeUnionT
     */
    public static boolean check( SaHpiFumiSpecInfoTypeUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            if ( !check( x.SafDefined ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            if ( !check( x.OemDefined ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiSpecInfoT
     */
    public static boolean check( SaHpiFumiSpecInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.SpecInfoTypeUnion, x.SpecInfoType ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiFirmwareInstanceInfoT
     */
    public static boolean check( SaHpiFumiFirmwareInstanceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Identifier ) ) {
            return false;
        }
        if ( !check( x.Description ) ) {
            return false;
        }
        if ( !check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiImpactedEntityT
     */
    public static boolean check( SaHpiFumiImpactedEntityT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.ImpactedEntity ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiServiceImpactDataT
     */
    public static boolean check( SaHpiFumiServiceImpactDataT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.ImpactedEntities == null ) {
            return false;
        }
        if ( x.ImpactedEntities.length != SAHPI_FUMI_MAX_ENTITIES_IMPACTED ) {
            return false;
        }
        for ( int i = 0; i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
            if ( !check( x.ImpactedEntities[i] ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiSourceInfoT
     */
    public static boolean check( SaHpiFumiSourceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.SourceUri ) ) {
            return false;
        }
        if ( !check( x.Identifier ) ) {
            return false;
        }
        if ( !check( x.Description ) ) {
            return false;
        }
        if ( !check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiComponentInfoT
     */
    public static boolean check( SaHpiFumiComponentInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.MainFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiBankInfoT
     */
    public static boolean check( SaHpiFumiBankInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Identifier ) ) {
            return false;
        }
        if ( !check( x.Description ) ) {
            return false;
        }
        if ( !check( x.DateTime ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiLogicalBankInfoT
     */
    public static boolean check( SaHpiFumiLogicalBankInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.PendingFwInstance ) ) {
            return false;
        }
        if ( !check( x.RollbackFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiLogicalComponentInfoT
     */
    public static boolean check( SaHpiFumiLogicalComponentInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.PendingFwInstance ) ) {
            return false;
        }
        if ( !check( x.RollbackFwInstance ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiRecT
     */
    public static boolean check( SaHpiFumiRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiResourceEventT
     */
    public static boolean check( SaHpiResourceEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDomainEventT
     */
    public static boolean check( SaHpiDomainEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorEventT
     */
    public static boolean check( SaHpiSensorEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.TriggerReading ) ) {
            return false;
        }
        if ( !check( x.TriggerThreshold ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiSensorEnableChangeEventT
     */
    public static boolean check( SaHpiSensorEnableChangeEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiHotSwapEventT
     */
    public static boolean check( SaHpiHotSwapEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiWatchdogEventT
     */
    public static boolean check( SaHpiWatchdogEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiHpiSwEventT
     */
    public static boolean check( SaHpiHpiSwEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.EventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiOemEventT
     */
    public static boolean check( SaHpiOemEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.OemEventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiUserEventT
     */
    public static boolean check( SaHpiUserEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.UserEventData ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiEventT
     */
    public static boolean check( SaHpiDimiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDimiUpdateEventT
     */
    public static boolean check( SaHpiDimiUpdateEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiFumiEventT
     */
    public static boolean check( SaHpiFumiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiEventUnionT
     */
    public static boolean check( SaHpiEventUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_ET_RESOURCE ) {
            if ( !check( x.ResourceEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_DOMAIN ) {
            if ( !check( x.DomainEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_SENSOR ) {
            if ( !check( x.SensorEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            if ( !check( x.SensorEnableChangeEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_HOTSWAP ) {
            if ( !check( x.HotSwapEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_WATCHDOG ) {
            if ( !check( x.WatchdogEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_HPI_SW ) {
            if ( !check( x.HpiSwEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_OEM ) {
            if ( !check( x.OemEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_USER ) {
            if ( !check( x.UserEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_DIMI ) {
            if ( !check( x.DimiEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_DIMI_UPDATE ) {
            if ( !check( x.DimiUpdateEvent ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ET_FUMI ) {
            if ( !check( x.FumiEvent ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiEventT
     */
    public static boolean check( SaHpiEventT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.EventDataUnion, x.EventType ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiNameT
     */
    public static boolean check( SaHpiNameT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Value == null ) {
            return false;
        }
        if ( x.Value.length != SA_HPI_MAX_NAME_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiConditionT
     */
    public static boolean check( SaHpiConditionT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Entity ) ) {
            return false;
        }
        if ( !check( x.Name ) ) {
            return false;
        }
        if ( !check( x.Data ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiAnnouncementT
     */
    public static boolean check( SaHpiAnnouncementT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.StatusCond ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiAnnunciatorRecT
     */
    public static boolean check( SaHpiAnnunciatorRecT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for union SaHpiRdrTypeUnionT
     */
    public static boolean check( SaHpiRdrTypeUnionT x, long mod )
    {
        if ( x == null ) {
            return false;
        }
        if ( mod == SAHPI_CTRL_RDR ) {
            if ( !check( x.CtrlRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_SENSOR_RDR ) {
            if ( !check( x.SensorRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_INVENTORY_RDR ) {
            if ( !check( x.InventoryRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_WATCHDOG_RDR ) {
            if ( !check( x.WatchdogRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_ANNUNCIATOR_RDR ) {
            if ( !check( x.AnnunciatorRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_DIMI_RDR ) {
            if ( !check( x.DimiRec ) ) {
                return false;
            }
        }
        if ( mod == SAHPI_FUMI_RDR ) {
            if ( !check( x.FumiRec ) ) {
                return false;
            }
        }
        return true;
    }

    /**
     * check function for struct SaHpiRdrT
     */
    public static boolean check( SaHpiRdrT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Entity ) ) {
            return false;
        }
        if ( !check( x.RdrTypeUnion, x.RdrType ) ) {
            return false;
        }
        if ( !check( x.IdString ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiLoadIdT
     */
    public static boolean check( SaHpiLoadIdT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.LoadName ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiResourceInfoT
     */
    public static boolean check( SaHpiResourceInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( x.Guid == null ) {
            return false;
        }
        if ( x.Guid.length != SAHPI_GUID_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiRptEntryT
     */
    public static boolean check( SaHpiRptEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.ResourceInfo ) ) {
            return false;
        }
        if ( !check( x.ResourceEntity ) ) {
            return false;
        }
        if ( !check( x.ResourceTag ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDomainInfoT
     */
    public static boolean check( SaHpiDomainInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.DomainTag ) ) {
            return false;
        }
        if ( x.Guid == null ) {
            return false;
        }
        if ( x.Guid.length != SAHPI_GUID_LENGTH ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiDrtEntryT
     */
    public static boolean check( SaHpiDrtEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiAlarmT
     */
    public static boolean check( SaHpiAlarmT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.AlarmCond ) ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiEventLogInfoT
     */
    public static boolean check( SaHpiEventLogInfoT x )
    {
        if ( x == null ) {
            return false;
        }
        return true;
    }

    /**
     * check function for struct SaHpiEventLogEntryT
     */
    public static boolean check( SaHpiEventLogEntryT x )
    {
        if ( x == null ) {
            return false;
        }
        if ( !check( x.Event ) ) {
            return false;
        }
        return true;
    }


    /**********************************************************
     * Lookups for enums, errors, event categories...
     * Value -> Name (FromXXX)
     * Name -> Value (ToXXX)
     * NB: ToXXX throws IllegalArgumentException if lookup fails
     *********************************************************/
    /**
     * For SaHpiLanguageT
     */
    public static String fromSaHpiLanguageT( long x )
    {
        if ( x == SAHPI_LANG_UNDEF ) {
            return "UNDEF";
        }
        if ( x == SAHPI_LANG_AFAR ) {
            return "AFAR";
        }
        if ( x == SAHPI_LANG_ABKHAZIAN ) {
            return "ABKHAZIAN";
        }
        if ( x == SAHPI_LANG_AFRIKAANS ) {
            return "AFRIKAANS";
        }
        if ( x == SAHPI_LANG_AMHARIC ) {
            return "AMHARIC";
        }
        if ( x == SAHPI_LANG_ARABIC ) {
            return "ARABIC";
        }
        if ( x == SAHPI_LANG_ASSAMESE ) {
            return "ASSAMESE";
        }
        if ( x == SAHPI_LANG_AYMARA ) {
            return "AYMARA";
        }
        if ( x == SAHPI_LANG_AZERBAIJANI ) {
            return "AZERBAIJANI";
        }
        if ( x == SAHPI_LANG_BASHKIR ) {
            return "BASHKIR";
        }
        if ( x == SAHPI_LANG_BYELORUSSIAN ) {
            return "BYELORUSSIAN";
        }
        if ( x == SAHPI_LANG_BULGARIAN ) {
            return "BULGARIAN";
        }
        if ( x == SAHPI_LANG_BIHARI ) {
            return "BIHARI";
        }
        if ( x == SAHPI_LANG_BISLAMA ) {
            return "BISLAMA";
        }
        if ( x == SAHPI_LANG_BENGALI ) {
            return "BENGALI";
        }
        if ( x == SAHPI_LANG_TIBETAN ) {
            return "TIBETAN";
        }
        if ( x == SAHPI_LANG_BRETON ) {
            return "BRETON";
        }
        if ( x == SAHPI_LANG_CATALAN ) {
            return "CATALAN";
        }
        if ( x == SAHPI_LANG_CORSICAN ) {
            return "CORSICAN";
        }
        if ( x == SAHPI_LANG_CZECH ) {
            return "CZECH";
        }
        if ( x == SAHPI_LANG_WELSH ) {
            return "WELSH";
        }
        if ( x == SAHPI_LANG_DANISH ) {
            return "DANISH";
        }
        if ( x == SAHPI_LANG_GERMAN ) {
            return "GERMAN";
        }
        if ( x == SAHPI_LANG_BHUTANI ) {
            return "BHUTANI";
        }
        if ( x == SAHPI_LANG_GREEK ) {
            return "GREEK";
        }
        if ( x == SAHPI_LANG_ENGLISH ) {
            return "ENGLISH";
        }
        if ( x == SAHPI_LANG_ESPERANTO ) {
            return "ESPERANTO";
        }
        if ( x == SAHPI_LANG_SPANISH ) {
            return "SPANISH";
        }
        if ( x == SAHPI_LANG_ESTONIAN ) {
            return "ESTONIAN";
        }
        if ( x == SAHPI_LANG_BASQUE ) {
            return "BASQUE";
        }
        if ( x == SAHPI_LANG_PERSIAN ) {
            return "PERSIAN";
        }
        if ( x == SAHPI_LANG_FINNISH ) {
            return "FINNISH";
        }
        if ( x == SAHPI_LANG_FIJI ) {
            return "FIJI";
        }
        if ( x == SAHPI_LANG_FAEROESE ) {
            return "FAEROESE";
        }
        if ( x == SAHPI_LANG_FRENCH ) {
            return "FRENCH";
        }
        if ( x == SAHPI_LANG_FRISIAN ) {
            return "FRISIAN";
        }
        if ( x == SAHPI_LANG_IRISH ) {
            return "IRISH";
        }
        if ( x == SAHPI_LANG_SCOTSGAELIC ) {
            return "SCOTSGAELIC";
        }
        if ( x == SAHPI_LANG_GALICIAN ) {
            return "GALICIAN";
        }
        if ( x == SAHPI_LANG_GUARANI ) {
            return "GUARANI";
        }
        if ( x == SAHPI_LANG_GUJARATI ) {
            return "GUJARATI";
        }
        if ( x == SAHPI_LANG_HAUSA ) {
            return "HAUSA";
        }
        if ( x == SAHPI_LANG_HINDI ) {
            return "HINDI";
        }
        if ( x == SAHPI_LANG_CROATIAN ) {
            return "CROATIAN";
        }
        if ( x == SAHPI_LANG_HUNGARIAN ) {
            return "HUNGARIAN";
        }
        if ( x == SAHPI_LANG_ARMENIAN ) {
            return "ARMENIAN";
        }
        if ( x == SAHPI_LANG_INTERLINGUA ) {
            return "INTERLINGUA";
        }
        if ( x == SAHPI_LANG_INTERLINGUE ) {
            return "INTERLINGUE";
        }
        if ( x == SAHPI_LANG_INUPIAK ) {
            return "INUPIAK";
        }
        if ( x == SAHPI_LANG_INDONESIAN ) {
            return "INDONESIAN";
        }
        if ( x == SAHPI_LANG_ICELANDIC ) {
            return "ICELANDIC";
        }
        if ( x == SAHPI_LANG_ITALIAN ) {
            return "ITALIAN";
        }
        if ( x == SAHPI_LANG_HEBREW ) {
            return "HEBREW";
        }
        if ( x == SAHPI_LANG_JAPANESE ) {
            return "JAPANESE";
        }
        if ( x == SAHPI_LANG_YIDDISH ) {
            return "YIDDISH";
        }
        if ( x == SAHPI_LANG_JAVANESE ) {
            return "JAVANESE";
        }
        if ( x == SAHPI_LANG_GEORGIAN ) {
            return "GEORGIAN";
        }
        if ( x == SAHPI_LANG_KAZAKH ) {
            return "KAZAKH";
        }
        if ( x == SAHPI_LANG_GREENLANDIC ) {
            return "GREENLANDIC";
        }
        if ( x == SAHPI_LANG_CAMBODIAN ) {
            return "CAMBODIAN";
        }
        if ( x == SAHPI_LANG_KANNADA ) {
            return "KANNADA";
        }
        if ( x == SAHPI_LANG_KOREAN ) {
            return "KOREAN";
        }
        if ( x == SAHPI_LANG_KASHMIRI ) {
            return "KASHMIRI";
        }
        if ( x == SAHPI_LANG_KURDISH ) {
            return "KURDISH";
        }
        if ( x == SAHPI_LANG_KIRGHIZ ) {
            return "KIRGHIZ";
        }
        if ( x == SAHPI_LANG_LATIN ) {
            return "LATIN";
        }
        if ( x == SAHPI_LANG_LINGALA ) {
            return "LINGALA";
        }
        if ( x == SAHPI_LANG_LAOTHIAN ) {
            return "LAOTHIAN";
        }
        if ( x == SAHPI_LANG_LITHUANIAN ) {
            return "LITHUANIAN";
        }
        if ( x == SAHPI_LANG_LATVIANLETTISH ) {
            return "LATVIANLETTISH";
        }
        if ( x == SAHPI_LANG_MALAGASY ) {
            return "MALAGASY";
        }
        if ( x == SAHPI_LANG_MAORI ) {
            return "MAORI";
        }
        if ( x == SAHPI_LANG_MACEDONIAN ) {
            return "MACEDONIAN";
        }
        if ( x == SAHPI_LANG_MALAYALAM ) {
            return "MALAYALAM";
        }
        if ( x == SAHPI_LANG_MONGOLIAN ) {
            return "MONGOLIAN";
        }
        if ( x == SAHPI_LANG_MOLDAVIAN ) {
            return "MOLDAVIAN";
        }
        if ( x == SAHPI_LANG_MARATHI ) {
            return "MARATHI";
        }
        if ( x == SAHPI_LANG_MALAY ) {
            return "MALAY";
        }
        if ( x == SAHPI_LANG_MALTESE ) {
            return "MALTESE";
        }
        if ( x == SAHPI_LANG_BURMESE ) {
            return "BURMESE";
        }
        if ( x == SAHPI_LANG_NAURU ) {
            return "NAURU";
        }
        if ( x == SAHPI_LANG_NEPALI ) {
            return "NEPALI";
        }
        if ( x == SAHPI_LANG_DUTCH ) {
            return "DUTCH";
        }
        if ( x == SAHPI_LANG_NORWEGIAN ) {
            return "NORWEGIAN";
        }
        if ( x == SAHPI_LANG_OCCITAN ) {
            return "OCCITAN";
        }
        if ( x == SAHPI_LANG_AFANOROMO ) {
            return "AFANOROMO";
        }
        if ( x == SAHPI_LANG_ORIYA ) {
            return "ORIYA";
        }
        if ( x == SAHPI_LANG_PUNJABI ) {
            return "PUNJABI";
        }
        if ( x == SAHPI_LANG_POLISH ) {
            return "POLISH";
        }
        if ( x == SAHPI_LANG_PASHTOPUSHTO ) {
            return "PASHTOPUSHTO";
        }
        if ( x == SAHPI_LANG_PORTUGUESE ) {
            return "PORTUGUESE";
        }
        if ( x == SAHPI_LANG_QUECHUA ) {
            return "QUECHUA";
        }
        if ( x == SAHPI_LANG_RHAETOROMANCE ) {
            return "RHAETOROMANCE";
        }
        if ( x == SAHPI_LANG_KIRUNDI ) {
            return "KIRUNDI";
        }
        if ( x == SAHPI_LANG_ROMANIAN ) {
            return "ROMANIAN";
        }
        if ( x == SAHPI_LANG_RUSSIAN ) {
            return "RUSSIAN";
        }
        if ( x == SAHPI_LANG_KINYARWANDA ) {
            return "KINYARWANDA";
        }
        if ( x == SAHPI_LANG_SANSKRIT ) {
            return "SANSKRIT";
        }
        if ( x == SAHPI_LANG_SINDHI ) {
            return "SINDHI";
        }
        if ( x == SAHPI_LANG_SANGRO ) {
            return "SANGRO";
        }
        if ( x == SAHPI_LANG_SERBOCROATIAN ) {
            return "SERBOCROATIAN";
        }
        if ( x == SAHPI_LANG_SINGHALESE ) {
            return "SINGHALESE";
        }
        if ( x == SAHPI_LANG_SLOVAK ) {
            return "SLOVAK";
        }
        if ( x == SAHPI_LANG_SLOVENIAN ) {
            return "SLOVENIAN";
        }
        if ( x == SAHPI_LANG_SAMOAN ) {
            return "SAMOAN";
        }
        if ( x == SAHPI_LANG_SHONA ) {
            return "SHONA";
        }
        if ( x == SAHPI_LANG_SOMALI ) {
            return "SOMALI";
        }
        if ( x == SAHPI_LANG_ALBANIAN ) {
            return "ALBANIAN";
        }
        if ( x == SAHPI_LANG_SERBIAN ) {
            return "SERBIAN";
        }
        if ( x == SAHPI_LANG_SISWATI ) {
            return "SISWATI";
        }
        if ( x == SAHPI_LANG_SESOTHO ) {
            return "SESOTHO";
        }
        if ( x == SAHPI_LANG_SUDANESE ) {
            return "SUDANESE";
        }
        if ( x == SAHPI_LANG_SWEDISH ) {
            return "SWEDISH";
        }
        if ( x == SAHPI_LANG_SWAHILI ) {
            return "SWAHILI";
        }
        if ( x == SAHPI_LANG_TAMIL ) {
            return "TAMIL";
        }
        if ( x == SAHPI_LANG_TELUGU ) {
            return "TELUGU";
        }
        if ( x == SAHPI_LANG_TAJIK ) {
            return "TAJIK";
        }
        if ( x == SAHPI_LANG_THAI ) {
            return "THAI";
        }
        if ( x == SAHPI_LANG_TIGRINYA ) {
            return "TIGRINYA";
        }
        if ( x == SAHPI_LANG_TURKMEN ) {
            return "TURKMEN";
        }
        if ( x == SAHPI_LANG_TAGALOG ) {
            return "TAGALOG";
        }
        if ( x == SAHPI_LANG_SETSWANA ) {
            return "SETSWANA";
        }
        if ( x == SAHPI_LANG_TONGA ) {
            return "TONGA";
        }
        if ( x == SAHPI_LANG_TURKISH ) {
            return "TURKISH";
        }
        if ( x == SAHPI_LANG_TSONGA ) {
            return "TSONGA";
        }
        if ( x == SAHPI_LANG_TATAR ) {
            return "TATAR";
        }
        if ( x == SAHPI_LANG_TWI ) {
            return "TWI";
        }
        if ( x == SAHPI_LANG_UKRAINIAN ) {
            return "UKRAINIAN";
        }
        if ( x == SAHPI_LANG_URDU ) {
            return "URDU";
        }
        if ( x == SAHPI_LANG_UZBEK ) {
            return "UZBEK";
        }
        if ( x == SAHPI_LANG_VIETNAMESE ) {
            return "VIETNAMESE";
        }
        if ( x == SAHPI_LANG_VOLAPUK ) {
            return "VOLAPUK";
        }
        if ( x == SAHPI_LANG_WOLOF ) {
            return "WOLOF";
        }
        if ( x == SAHPI_LANG_XHOSA ) {
            return "XHOSA";
        }
        if ( x == SAHPI_LANG_YORUBA ) {
            return "YORUBA";
        }
        if ( x == SAHPI_LANG_CHINESE ) {
            return "CHINESE";
        }
        if ( x == SAHPI_LANG_ZULU ) {
            return "ZULU";
        }

        return Long.toString( x );
    }

    public static long toSaHpiLanguageT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "UNDEF" ) ) {
            return SAHPI_LANG_UNDEF;
        }
        if ( s.equals( "AFAR" ) ) {
            return SAHPI_LANG_AFAR;
        }
        if ( s.equals( "ABKHAZIAN" ) ) {
            return SAHPI_LANG_ABKHAZIAN;
        }
        if ( s.equals( "AFRIKAANS" ) ) {
            return SAHPI_LANG_AFRIKAANS;
        }
        if ( s.equals( "AMHARIC" ) ) {
            return SAHPI_LANG_AMHARIC;
        }
        if ( s.equals( "ARABIC" ) ) {
            return SAHPI_LANG_ARABIC;
        }
        if ( s.equals( "ASSAMESE" ) ) {
            return SAHPI_LANG_ASSAMESE;
        }
        if ( s.equals( "AYMARA" ) ) {
            return SAHPI_LANG_AYMARA;
        }
        if ( s.equals( "AZERBAIJANI" ) ) {
            return SAHPI_LANG_AZERBAIJANI;
        }
        if ( s.equals( "BASHKIR" ) ) {
            return SAHPI_LANG_BASHKIR;
        }
        if ( s.equals( "BYELORUSSIAN" ) ) {
            return SAHPI_LANG_BYELORUSSIAN;
        }
        if ( s.equals( "BULGARIAN" ) ) {
            return SAHPI_LANG_BULGARIAN;
        }
        if ( s.equals( "BIHARI" ) ) {
            return SAHPI_LANG_BIHARI;
        }
        if ( s.equals( "BISLAMA" ) ) {
            return SAHPI_LANG_BISLAMA;
        }
        if ( s.equals( "BENGALI" ) ) {
            return SAHPI_LANG_BENGALI;
        }
        if ( s.equals( "TIBETAN" ) ) {
            return SAHPI_LANG_TIBETAN;
        }
        if ( s.equals( "BRETON" ) ) {
            return SAHPI_LANG_BRETON;
        }
        if ( s.equals( "CATALAN" ) ) {
            return SAHPI_LANG_CATALAN;
        }
        if ( s.equals( "CORSICAN" ) ) {
            return SAHPI_LANG_CORSICAN;
        }
        if ( s.equals( "CZECH" ) ) {
            return SAHPI_LANG_CZECH;
        }
        if ( s.equals( "WELSH" ) ) {
            return SAHPI_LANG_WELSH;
        }
        if ( s.equals( "DANISH" ) ) {
            return SAHPI_LANG_DANISH;
        }
        if ( s.equals( "GERMAN" ) ) {
            return SAHPI_LANG_GERMAN;
        }
        if ( s.equals( "BHUTANI" ) ) {
            return SAHPI_LANG_BHUTANI;
        }
        if ( s.equals( "GREEK" ) ) {
            return SAHPI_LANG_GREEK;
        }
        if ( s.equals( "ENGLISH" ) ) {
            return SAHPI_LANG_ENGLISH;
        }
        if ( s.equals( "ESPERANTO" ) ) {
            return SAHPI_LANG_ESPERANTO;
        }
        if ( s.equals( "SPANISH" ) ) {
            return SAHPI_LANG_SPANISH;
        }
        if ( s.equals( "ESTONIAN" ) ) {
            return SAHPI_LANG_ESTONIAN;
        }
        if ( s.equals( "BASQUE" ) ) {
            return SAHPI_LANG_BASQUE;
        }
        if ( s.equals( "PERSIAN" ) ) {
            return SAHPI_LANG_PERSIAN;
        }
        if ( s.equals( "FINNISH" ) ) {
            return SAHPI_LANG_FINNISH;
        }
        if ( s.equals( "FIJI" ) ) {
            return SAHPI_LANG_FIJI;
        }
        if ( s.equals( "FAEROESE" ) ) {
            return SAHPI_LANG_FAEROESE;
        }
        if ( s.equals( "FRENCH" ) ) {
            return SAHPI_LANG_FRENCH;
        }
        if ( s.equals( "FRISIAN" ) ) {
            return SAHPI_LANG_FRISIAN;
        }
        if ( s.equals( "IRISH" ) ) {
            return SAHPI_LANG_IRISH;
        }
        if ( s.equals( "SCOTSGAELIC" ) ) {
            return SAHPI_LANG_SCOTSGAELIC;
        }
        if ( s.equals( "GALICIAN" ) ) {
            return SAHPI_LANG_GALICIAN;
        }
        if ( s.equals( "GUARANI" ) ) {
            return SAHPI_LANG_GUARANI;
        }
        if ( s.equals( "GUJARATI" ) ) {
            return SAHPI_LANG_GUJARATI;
        }
        if ( s.equals( "HAUSA" ) ) {
            return SAHPI_LANG_HAUSA;
        }
        if ( s.equals( "HINDI" ) ) {
            return SAHPI_LANG_HINDI;
        }
        if ( s.equals( "CROATIAN" ) ) {
            return SAHPI_LANG_CROATIAN;
        }
        if ( s.equals( "HUNGARIAN" ) ) {
            return SAHPI_LANG_HUNGARIAN;
        }
        if ( s.equals( "ARMENIAN" ) ) {
            return SAHPI_LANG_ARMENIAN;
        }
        if ( s.equals( "INTERLINGUA" ) ) {
            return SAHPI_LANG_INTERLINGUA;
        }
        if ( s.equals( "INTERLINGUE" ) ) {
            return SAHPI_LANG_INTERLINGUE;
        }
        if ( s.equals( "INUPIAK" ) ) {
            return SAHPI_LANG_INUPIAK;
        }
        if ( s.equals( "INDONESIAN" ) ) {
            return SAHPI_LANG_INDONESIAN;
        }
        if ( s.equals( "ICELANDIC" ) ) {
            return SAHPI_LANG_ICELANDIC;
        }
        if ( s.equals( "ITALIAN" ) ) {
            return SAHPI_LANG_ITALIAN;
        }
        if ( s.equals( "HEBREW" ) ) {
            return SAHPI_LANG_HEBREW;
        }
        if ( s.equals( "JAPANESE" ) ) {
            return SAHPI_LANG_JAPANESE;
        }
        if ( s.equals( "YIDDISH" ) ) {
            return SAHPI_LANG_YIDDISH;
        }
        if ( s.equals( "JAVANESE" ) ) {
            return SAHPI_LANG_JAVANESE;
        }
        if ( s.equals( "GEORGIAN" ) ) {
            return SAHPI_LANG_GEORGIAN;
        }
        if ( s.equals( "KAZAKH" ) ) {
            return SAHPI_LANG_KAZAKH;
        }
        if ( s.equals( "GREENLANDIC" ) ) {
            return SAHPI_LANG_GREENLANDIC;
        }
        if ( s.equals( "CAMBODIAN" ) ) {
            return SAHPI_LANG_CAMBODIAN;
        }
        if ( s.equals( "KANNADA" ) ) {
            return SAHPI_LANG_KANNADA;
        }
        if ( s.equals( "KOREAN" ) ) {
            return SAHPI_LANG_KOREAN;
        }
        if ( s.equals( "KASHMIRI" ) ) {
            return SAHPI_LANG_KASHMIRI;
        }
        if ( s.equals( "KURDISH" ) ) {
            return SAHPI_LANG_KURDISH;
        }
        if ( s.equals( "KIRGHIZ" ) ) {
            return SAHPI_LANG_KIRGHIZ;
        }
        if ( s.equals( "LATIN" ) ) {
            return SAHPI_LANG_LATIN;
        }
        if ( s.equals( "LINGALA" ) ) {
            return SAHPI_LANG_LINGALA;
        }
        if ( s.equals( "LAOTHIAN" ) ) {
            return SAHPI_LANG_LAOTHIAN;
        }
        if ( s.equals( "LITHUANIAN" ) ) {
            return SAHPI_LANG_LITHUANIAN;
        }
        if ( s.equals( "LATVIANLETTISH" ) ) {
            return SAHPI_LANG_LATVIANLETTISH;
        }
        if ( s.equals( "MALAGASY" ) ) {
            return SAHPI_LANG_MALAGASY;
        }
        if ( s.equals( "MAORI" ) ) {
            return SAHPI_LANG_MAORI;
        }
        if ( s.equals( "MACEDONIAN" ) ) {
            return SAHPI_LANG_MACEDONIAN;
        }
        if ( s.equals( "MALAYALAM" ) ) {
            return SAHPI_LANG_MALAYALAM;
        }
        if ( s.equals( "MONGOLIAN" ) ) {
            return SAHPI_LANG_MONGOLIAN;
        }
        if ( s.equals( "MOLDAVIAN" ) ) {
            return SAHPI_LANG_MOLDAVIAN;
        }
        if ( s.equals( "MARATHI" ) ) {
            return SAHPI_LANG_MARATHI;
        }
        if ( s.equals( "MALAY" ) ) {
            return SAHPI_LANG_MALAY;
        }
        if ( s.equals( "MALTESE" ) ) {
            return SAHPI_LANG_MALTESE;
        }
        if ( s.equals( "BURMESE" ) ) {
            return SAHPI_LANG_BURMESE;
        }
        if ( s.equals( "NAURU" ) ) {
            return SAHPI_LANG_NAURU;
        }
        if ( s.equals( "NEPALI" ) ) {
            return SAHPI_LANG_NEPALI;
        }
        if ( s.equals( "DUTCH" ) ) {
            return SAHPI_LANG_DUTCH;
        }
        if ( s.equals( "NORWEGIAN" ) ) {
            return SAHPI_LANG_NORWEGIAN;
        }
        if ( s.equals( "OCCITAN" ) ) {
            return SAHPI_LANG_OCCITAN;
        }
        if ( s.equals( "AFANOROMO" ) ) {
            return SAHPI_LANG_AFANOROMO;
        }
        if ( s.equals( "ORIYA" ) ) {
            return SAHPI_LANG_ORIYA;
        }
        if ( s.equals( "PUNJABI" ) ) {
            return SAHPI_LANG_PUNJABI;
        }
        if ( s.equals( "POLISH" ) ) {
            return SAHPI_LANG_POLISH;
        }
        if ( s.equals( "PASHTOPUSHTO" ) ) {
            return SAHPI_LANG_PASHTOPUSHTO;
        }
        if ( s.equals( "PORTUGUESE" ) ) {
            return SAHPI_LANG_PORTUGUESE;
        }
        if ( s.equals( "QUECHUA" ) ) {
            return SAHPI_LANG_QUECHUA;
        }
        if ( s.equals( "RHAETOROMANCE" ) ) {
            return SAHPI_LANG_RHAETOROMANCE;
        }
        if ( s.equals( "KIRUNDI" ) ) {
            return SAHPI_LANG_KIRUNDI;
        }
        if ( s.equals( "ROMANIAN" ) ) {
            return SAHPI_LANG_ROMANIAN;
        }
        if ( s.equals( "RUSSIAN" ) ) {
            return SAHPI_LANG_RUSSIAN;
        }
        if ( s.equals( "KINYARWANDA" ) ) {
            return SAHPI_LANG_KINYARWANDA;
        }
        if ( s.equals( "SANSKRIT" ) ) {
            return SAHPI_LANG_SANSKRIT;
        }
        if ( s.equals( "SINDHI" ) ) {
            return SAHPI_LANG_SINDHI;
        }
        if ( s.equals( "SANGRO" ) ) {
            return SAHPI_LANG_SANGRO;
        }
        if ( s.equals( "SERBOCROATIAN" ) ) {
            return SAHPI_LANG_SERBOCROATIAN;
        }
        if ( s.equals( "SINGHALESE" ) ) {
            return SAHPI_LANG_SINGHALESE;
        }
        if ( s.equals( "SLOVAK" ) ) {
            return SAHPI_LANG_SLOVAK;
        }
        if ( s.equals( "SLOVENIAN" ) ) {
            return SAHPI_LANG_SLOVENIAN;
        }
        if ( s.equals( "SAMOAN" ) ) {
            return SAHPI_LANG_SAMOAN;
        }
        if ( s.equals( "SHONA" ) ) {
            return SAHPI_LANG_SHONA;
        }
        if ( s.equals( "SOMALI" ) ) {
            return SAHPI_LANG_SOMALI;
        }
        if ( s.equals( "ALBANIAN" ) ) {
            return SAHPI_LANG_ALBANIAN;
        }
        if ( s.equals( "SERBIAN" ) ) {
            return SAHPI_LANG_SERBIAN;
        }
        if ( s.equals( "SISWATI" ) ) {
            return SAHPI_LANG_SISWATI;
        }
        if ( s.equals( "SESOTHO" ) ) {
            return SAHPI_LANG_SESOTHO;
        }
        if ( s.equals( "SUDANESE" ) ) {
            return SAHPI_LANG_SUDANESE;
        }
        if ( s.equals( "SWEDISH" ) ) {
            return SAHPI_LANG_SWEDISH;
        }
        if ( s.equals( "SWAHILI" ) ) {
            return SAHPI_LANG_SWAHILI;
        }
        if ( s.equals( "TAMIL" ) ) {
            return SAHPI_LANG_TAMIL;
        }
        if ( s.equals( "TELUGU" ) ) {
            return SAHPI_LANG_TELUGU;
        }
        if ( s.equals( "TAJIK" ) ) {
            return SAHPI_LANG_TAJIK;
        }
        if ( s.equals( "THAI" ) ) {
            return SAHPI_LANG_THAI;
        }
        if ( s.equals( "TIGRINYA" ) ) {
            return SAHPI_LANG_TIGRINYA;
        }
        if ( s.equals( "TURKMEN" ) ) {
            return SAHPI_LANG_TURKMEN;
        }
        if ( s.equals( "TAGALOG" ) ) {
            return SAHPI_LANG_TAGALOG;
        }
        if ( s.equals( "SETSWANA" ) ) {
            return SAHPI_LANG_SETSWANA;
        }
        if ( s.equals( "TONGA" ) ) {
            return SAHPI_LANG_TONGA;
        }
        if ( s.equals( "TURKISH" ) ) {
            return SAHPI_LANG_TURKISH;
        }
        if ( s.equals( "TSONGA" ) ) {
            return SAHPI_LANG_TSONGA;
        }
        if ( s.equals( "TATAR" ) ) {
            return SAHPI_LANG_TATAR;
        }
        if ( s.equals( "TWI" ) ) {
            return SAHPI_LANG_TWI;
        }
        if ( s.equals( "UKRAINIAN" ) ) {
            return SAHPI_LANG_UKRAINIAN;
        }
        if ( s.equals( "URDU" ) ) {
            return SAHPI_LANG_URDU;
        }
        if ( s.equals( "UZBEK" ) ) {
            return SAHPI_LANG_UZBEK;
        }
        if ( s.equals( "VIETNAMESE" ) ) {
            return SAHPI_LANG_VIETNAMESE;
        }
        if ( s.equals( "VOLAPUK" ) ) {
            return SAHPI_LANG_VOLAPUK;
        }
        if ( s.equals( "WOLOF" ) ) {
            return SAHPI_LANG_WOLOF;
        }
        if ( s.equals( "XHOSA" ) ) {
            return SAHPI_LANG_XHOSA;
        }
        if ( s.equals( "YORUBA" ) ) {
            return SAHPI_LANG_YORUBA;
        }
        if ( s.equals( "CHINESE" ) ) {
            return SAHPI_LANG_CHINESE;
        }
        if ( s.equals( "ZULU" ) ) {
            return SAHPI_LANG_ZULU;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiTextTypeT
     */
    public static String fromSaHpiTextTypeT( long x )
    {
        if ( x == SAHPI_TL_TYPE_UNICODE ) {
            return "UNICODE";
        }
        if ( x == SAHPI_TL_TYPE_BCDPLUS ) {
            return "BCDPLUS";
        }
        if ( x == SAHPI_TL_TYPE_ASCII6 ) {
            return "ASCII6";
        }
        if ( x == SAHPI_TL_TYPE_TEXT ) {
            return "TEXT";
        }
        if ( x == SAHPI_TL_TYPE_BINARY ) {
            return "BINARY";
        }

        return Long.toString( x );
    }

    public static long toSaHpiTextTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "UNICODE" ) ) {
            return SAHPI_TL_TYPE_UNICODE;
        }
        if ( s.equals( "BCDPLUS" ) ) {
            return SAHPI_TL_TYPE_BCDPLUS;
        }
        if ( s.equals( "ASCII6" ) ) {
            return SAHPI_TL_TYPE_ASCII6;
        }
        if ( s.equals( "TEXT" ) ) {
            return SAHPI_TL_TYPE_TEXT;
        }
        if ( s.equals( "BINARY" ) ) {
            return SAHPI_TL_TYPE_BINARY;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiEntityTypeT
     */
    public static String fromSaHpiEntityTypeT( long x )
    {
        if ( x == SAHPI_ENT_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }
        if ( x == SAHPI_ENT_OTHER ) {
            return "OTHER";
        }
        if ( x == SAHPI_ENT_UNKNOWN ) {
            return "UNKNOWN";
        }
        if ( x == SAHPI_ENT_PROCESSOR ) {
            return "PROCESSOR";
        }
        if ( x == SAHPI_ENT_DISK_BAY ) {
            return "DISK_BAY";
        }
        if ( x == SAHPI_ENT_PERIPHERAL_BAY ) {
            return "PERIPHERAL_BAY";
        }
        if ( x == SAHPI_ENT_SYS_MGMNT_MODULE ) {
            return "SYS_MGMNT_MODULE";
        }
        if ( x == SAHPI_ENT_SYSTEM_BOARD ) {
            return "SYSTEM_BOARD";
        }
        if ( x == SAHPI_ENT_MEMORY_MODULE ) {
            return "MEMORY_MODULE";
        }
        if ( x == SAHPI_ENT_PROCESSOR_MODULE ) {
            return "PROCESSOR_MODULE";
        }
        if ( x == SAHPI_ENT_POWER_SUPPLY ) {
            return "POWER_SUPPLY";
        }
        if ( x == SAHPI_ENT_ADD_IN_CARD ) {
            return "ADD_IN_CARD";
        }
        if ( x == SAHPI_ENT_FRONT_PANEL_BOARD ) {
            return "FRONT_PANEL_BOARD";
        }
        if ( x == SAHPI_ENT_BACK_PANEL_BOARD ) {
            return "BACK_PANEL_BOARD";
        }
        if ( x == SAHPI_ENT_POWER_SYSTEM_BOARD ) {
            return "POWER_SYSTEM_BOARD";
        }
        if ( x == SAHPI_ENT_DRIVE_BACKPLANE ) {
            return "DRIVE_BACKPLANE";
        }
        if ( x == SAHPI_ENT_SYS_EXPANSION_BOARD ) {
            return "SYS_EXPANSION_BOARD";
        }
        if ( x == SAHPI_ENT_OTHER_SYSTEM_BOARD ) {
            return "OTHER_SYSTEM_BOARD";
        }
        if ( x == SAHPI_ENT_PROCESSOR_BOARD ) {
            return "PROCESSOR_BOARD";
        }
        if ( x == SAHPI_ENT_POWER_UNIT ) {
            return "POWER_UNIT";
        }
        if ( x == SAHPI_ENT_POWER_MODULE ) {
            return "POWER_MODULE";
        }
        if ( x == SAHPI_ENT_POWER_MGMNT ) {
            return "POWER_MGMNT";
        }
        if ( x == SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD ) {
            return "CHASSIS_BACK_PANEL_BOARD";
        }
        if ( x == SAHPI_ENT_SYSTEM_CHASSIS ) {
            return "SYSTEM_CHASSIS";
        }
        if ( x == SAHPI_ENT_SUB_CHASSIS ) {
            return "SUB_CHASSIS";
        }
        if ( x == SAHPI_ENT_OTHER_CHASSIS_BOARD ) {
            return "OTHER_CHASSIS_BOARD";
        }
        if ( x == SAHPI_ENT_DISK_DRIVE_BAY ) {
            return "DISK_DRIVE_BAY";
        }
        if ( x == SAHPI_ENT_PERIPHERAL_BAY_2 ) {
            return "PERIPHERAL_BAY_2";
        }
        if ( x == SAHPI_ENT_DEVICE_BAY ) {
            return "DEVICE_BAY";
        }
        if ( x == SAHPI_ENT_COOLING_DEVICE ) {
            return "COOLING_DEVICE";
        }
        if ( x == SAHPI_ENT_COOLING_UNIT ) {
            return "COOLING_UNIT";
        }
        if ( x == SAHPI_ENT_INTERCONNECT ) {
            return "INTERCONNECT";
        }
        if ( x == SAHPI_ENT_MEMORY_DEVICE ) {
            return "MEMORY_DEVICE";
        }
        if ( x == SAHPI_ENT_SYS_MGMNT_SOFTWARE ) {
            return "SYS_MGMNT_SOFTWARE";
        }
        if ( x == SAHPI_ENT_BIOS ) {
            return "BIOS";
        }
        if ( x == SAHPI_ENT_OPERATING_SYSTEM ) {
            return "OPERATING_SYSTEM";
        }
        if ( x == SAHPI_ENT_SYSTEM_BUS ) {
            return "SYSTEM_BUS";
        }
        if ( x == SAHPI_ENT_GROUP ) {
            return "GROUP";
        }
        if ( x == SAHPI_ENT_REMOTE ) {
            return "REMOTE";
        }
        if ( x == SAHPI_ENT_EXTERNAL_ENVIRONMENT ) {
            return "EXTERNAL_ENVIRONMENT";
        }
        if ( x == SAHPI_ENT_BATTERY ) {
            return "BATTERY";
        }
        if ( x == SAHPI_ENT_PROCESSING_BLADE ) {
            return "PROCESSING_BLADE";
        }
        if ( x == SAHPI_ENT_CONNECTIVITY_SWITCH ) {
            return "CONNECTIVITY_SWITCH";
        }
        if ( x == SAHPI_ENT_PROCESSOR_MEMORY_MODULE ) {
            return "PROCESSOR_MEMORY_MODULE";
        }
        if ( x == SAHPI_ENT_IO_MODULE ) {
            return "IO_MODULE";
        }
        if ( x == SAHPI_ENT_PROCESSOR_IO_MODULE ) {
            return "PROCESSOR_IO_MODULE";
        }
        if ( x == SAHPI_ENT_MC_FIRMWARE ) {
            return "MC_FIRMWARE";
        }
        if ( x == SAHPI_ENT_IPMI_CHANNEL ) {
            return "IPMI_CHANNEL";
        }
        if ( x == SAHPI_ENT_PCI_BUS ) {
            return "PCI_BUS";
        }
        if ( x == SAHPI_ENT_PCI_EXPRESS_BUS ) {
            return "PCI_EXPRESS_BUS";
        }
        if ( x == SAHPI_ENT_SCSI_BUS ) {
            return "SCSI_BUS";
        }
        if ( x == SAHPI_ENT_SATA_BUS ) {
            return "SATA_BUS";
        }
        if ( x == SAHPI_ENT_PROC_FSB ) {
            return "PROC_FSB";
        }
        if ( x == SAHPI_ENT_CLOCK ) {
            return "CLOCK";
        }
        if ( x == SAHPI_ENT_SYSTEM_FIRMWARE ) {
            return "SYSTEM_FIRMWARE";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC ) {
            return "CHASSIS_SPECIFIC";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC01 ) {
            return "CHASSIS_SPECIFIC01";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC02 ) {
            return "CHASSIS_SPECIFIC02";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC03 ) {
            return "CHASSIS_SPECIFIC03";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC04 ) {
            return "CHASSIS_SPECIFIC04";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC05 ) {
            return "CHASSIS_SPECIFIC05";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC06 ) {
            return "CHASSIS_SPECIFIC06";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC07 ) {
            return "CHASSIS_SPECIFIC07";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC08 ) {
            return "CHASSIS_SPECIFIC08";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC09 ) {
            return "CHASSIS_SPECIFIC09";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC10 ) {
            return "CHASSIS_SPECIFIC10";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC11 ) {
            return "CHASSIS_SPECIFIC11";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC12 ) {
            return "CHASSIS_SPECIFIC12";
        }
        if ( x == SAHPI_ENT_CHASSIS_SPECIFIC13 ) {
            return "CHASSIS_SPECIFIC13";
        }
        if ( x == SAHPI_ENT_BOARD_SET_SPECIFIC ) {
            return "BOARD_SET_SPECIFIC";
        }
        if ( x == SAHPI_ENT_OEM_SYSINT_SPECIFIC ) {
            return "OEM_SYSINT_SPECIFIC";
        }
        if ( x == SAHPI_ENT_ROOT ) {
            return "ROOT";
        }
        if ( x == SAHPI_ENT_RACK ) {
            return "RACK";
        }
        if ( x == SAHPI_ENT_SUBRACK ) {
            return "SUBRACK";
        }
        if ( x == SAHPI_ENT_COMPACTPCI_CHASSIS ) {
            return "COMPACTPCI_CHASSIS";
        }
        if ( x == SAHPI_ENT_ADVANCEDTCA_CHASSIS ) {
            return "ADVANCEDTCA_CHASSIS";
        }
        if ( x == SAHPI_ENT_RACK_MOUNTED_SERVER ) {
            return "RACK_MOUNTED_SERVER";
        }
        if ( x == SAHPI_ENT_SYSTEM_BLADE ) {
            return "SYSTEM_BLADE";
        }
        if ( x == SAHPI_ENT_SWITCH ) {
            return "SWITCH";
        }
        if ( x == SAHPI_ENT_SWITCH_BLADE ) {
            return "SWITCH_BLADE";
        }
        if ( x == SAHPI_ENT_SBC_BLADE ) {
            return "SBC_BLADE";
        }
        if ( x == SAHPI_ENT_IO_BLADE ) {
            return "IO_BLADE";
        }
        if ( x == SAHPI_ENT_DISK_BLADE ) {
            return "DISK_BLADE";
        }
        if ( x == SAHPI_ENT_DISK_DRIVE ) {
            return "DISK_DRIVE";
        }
        if ( x == SAHPI_ENT_FAN ) {
            return "FAN";
        }
        if ( x == SAHPI_ENT_POWER_DISTRIBUTION_UNIT ) {
            return "POWER_DISTRIBUTION_UNIT";
        }
        if ( x == SAHPI_ENT_SPEC_PROC_BLADE ) {
            return "SPEC_PROC_BLADE";
        }
        if ( x == SAHPI_ENT_IO_SUBBOARD ) {
            return "IO_SUBBOARD";
        }
        if ( x == SAHPI_ENT_SBC_SUBBOARD ) {
            return "SBC_SUBBOARD";
        }
        if ( x == SAHPI_ENT_ALARM_MANAGER ) {
            return "ALARM_MANAGER";
        }
        if ( x == SAHPI_ENT_SHELF_MANAGER ) {
            return "SHELF_MANAGER";
        }
        if ( x == SAHPI_ENT_DISPLAY_PANEL ) {
            return "DISPLAY_PANEL";
        }
        if ( x == SAHPI_ENT_SUBBOARD_CARRIER_BLADE ) {
            return "SUBBOARD_CARRIER_BLADE";
        }
        if ( x == SAHPI_ENT_PHYSICAL_SLOT ) {
            return "PHYSICAL_SLOT";
        }
        if ( x == SAHPI_ENT_PICMG_FRONT_BLADE ) {
            return "PICMG_FRONT_BLADE";
        }
        if ( x == SAHPI_ENT_SYSTEM_INVENTORY_DEVICE ) {
            return "SYSTEM_INVENTORY_DEVICE";
        }
        if ( x == SAHPI_ENT_FILTRATION_UNIT ) {
            return "FILTRATION_UNIT";
        }
        if ( x == SAHPI_ENT_AMC ) {
            return "AMC";
        }
        if ( x == SAHPI_ENT_BMC ) {
            return "BMC";
        }
        if ( x == SAHPI_ENT_IPMC ) {
            return "IPMC";
        }
        if ( x == SAHPI_ENT_MMC ) {
            return "MMC";
        }
        if ( x == SAHPI_ENT_SHMC ) {
            return "SHMC";
        }
        if ( x == SAHPI_ENT_CPLD ) {
            return "CPLD";
        }
        if ( x == SAHPI_ENT_EPLD ) {
            return "EPLD";
        }
        if ( x == SAHPI_ENT_FPGA ) {
            return "FPGA";
        }
        if ( x == SAHPI_ENT_DASD ) {
            return "DASD";
        }
        if ( x == SAHPI_ENT_NIC ) {
            return "NIC";
        }
        if ( x == SAHPI_ENT_DSP ) {
            return "DSP";
        }
        if ( x == SAHPI_ENT_UCODE ) {
            return "UCODE";
        }
        if ( x == SAHPI_ENT_NPU ) {
            return "NPU";
        }
        if ( x == SAHPI_ENT_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_ENT_INTERFACE ) {
            return "INTERFACE";
        }
        if ( x == SAHPI_ENT_MICROTCA_CHASSIS ) {
            return "MICROTCA_CHASSIS";
        }
        if ( x == SAHPI_ENT_CARRIER ) {
            return "CARRIER";
        }
        if ( x == SAHPI_ENT_CARRIER_MANAGER ) {
            return "CARRIER_MANAGER";
        }
        if ( x == SAHPI_ENT_CONFIG_DATA ) {
            return "CONFIG_DATA";
        }
        if ( x == SAHPI_ENT_INDICATOR ) {
            return "INDICATOR";
        }

        return Long.toString( x );
    }

    public static long toSaHpiEntityTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_ENT_UNSPECIFIED;
        }
        if ( s.equals( "OTHER" ) ) {
            return SAHPI_ENT_OTHER;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return SAHPI_ENT_UNKNOWN;
        }
        if ( s.equals( "PROCESSOR" ) ) {
            return SAHPI_ENT_PROCESSOR;
        }
        if ( s.equals( "DISK_BAY" ) ) {
            return SAHPI_ENT_DISK_BAY;
        }
        if ( s.equals( "PERIPHERAL_BAY" ) ) {
            return SAHPI_ENT_PERIPHERAL_BAY;
        }
        if ( s.equals( "SYS_MGMNT_MODULE" ) ) {
            return SAHPI_ENT_SYS_MGMNT_MODULE;
        }
        if ( s.equals( "SYSTEM_BOARD" ) ) {
            return SAHPI_ENT_SYSTEM_BOARD;
        }
        if ( s.equals( "MEMORY_MODULE" ) ) {
            return SAHPI_ENT_MEMORY_MODULE;
        }
        if ( s.equals( "PROCESSOR_MODULE" ) ) {
            return SAHPI_ENT_PROCESSOR_MODULE;
        }
        if ( s.equals( "POWER_SUPPLY" ) ) {
            return SAHPI_ENT_POWER_SUPPLY;
        }
        if ( s.equals( "ADD_IN_CARD" ) ) {
            return SAHPI_ENT_ADD_IN_CARD;
        }
        if ( s.equals( "FRONT_PANEL_BOARD" ) ) {
            return SAHPI_ENT_FRONT_PANEL_BOARD;
        }
        if ( s.equals( "BACK_PANEL_BOARD" ) ) {
            return SAHPI_ENT_BACK_PANEL_BOARD;
        }
        if ( s.equals( "POWER_SYSTEM_BOARD" ) ) {
            return SAHPI_ENT_POWER_SYSTEM_BOARD;
        }
        if ( s.equals( "DRIVE_BACKPLANE" ) ) {
            return SAHPI_ENT_DRIVE_BACKPLANE;
        }
        if ( s.equals( "SYS_EXPANSION_BOARD" ) ) {
            return SAHPI_ENT_SYS_EXPANSION_BOARD;
        }
        if ( s.equals( "OTHER_SYSTEM_BOARD" ) ) {
            return SAHPI_ENT_OTHER_SYSTEM_BOARD;
        }
        if ( s.equals( "PROCESSOR_BOARD" ) ) {
            return SAHPI_ENT_PROCESSOR_BOARD;
        }
        if ( s.equals( "POWER_UNIT" ) ) {
            return SAHPI_ENT_POWER_UNIT;
        }
        if ( s.equals( "POWER_MODULE" ) ) {
            return SAHPI_ENT_POWER_MODULE;
        }
        if ( s.equals( "POWER_MGMNT" ) ) {
            return SAHPI_ENT_POWER_MGMNT;
        }
        if ( s.equals( "CHASSIS_BACK_PANEL_BOARD" ) ) {
            return SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD;
        }
        if ( s.equals( "SYSTEM_CHASSIS" ) ) {
            return SAHPI_ENT_SYSTEM_CHASSIS;
        }
        if ( s.equals( "SUB_CHASSIS" ) ) {
            return SAHPI_ENT_SUB_CHASSIS;
        }
        if ( s.equals( "OTHER_CHASSIS_BOARD" ) ) {
            return SAHPI_ENT_OTHER_CHASSIS_BOARD;
        }
        if ( s.equals( "DISK_DRIVE_BAY" ) ) {
            return SAHPI_ENT_DISK_DRIVE_BAY;
        }
        if ( s.equals( "PERIPHERAL_BAY_2" ) ) {
            return SAHPI_ENT_PERIPHERAL_BAY_2;
        }
        if ( s.equals( "DEVICE_BAY" ) ) {
            return SAHPI_ENT_DEVICE_BAY;
        }
        if ( s.equals( "COOLING_DEVICE" ) ) {
            return SAHPI_ENT_COOLING_DEVICE;
        }
        if ( s.equals( "COOLING_UNIT" ) ) {
            return SAHPI_ENT_COOLING_UNIT;
        }
        if ( s.equals( "INTERCONNECT" ) ) {
            return SAHPI_ENT_INTERCONNECT;
        }
        if ( s.equals( "MEMORY_DEVICE" ) ) {
            return SAHPI_ENT_MEMORY_DEVICE;
        }
        if ( s.equals( "SYS_MGMNT_SOFTWARE" ) ) {
            return SAHPI_ENT_SYS_MGMNT_SOFTWARE;
        }
        if ( s.equals( "BIOS" ) ) {
            return SAHPI_ENT_BIOS;
        }
        if ( s.equals( "OPERATING_SYSTEM" ) ) {
            return SAHPI_ENT_OPERATING_SYSTEM;
        }
        if ( s.equals( "SYSTEM_BUS" ) ) {
            return SAHPI_ENT_SYSTEM_BUS;
        }
        if ( s.equals( "GROUP" ) ) {
            return SAHPI_ENT_GROUP;
        }
        if ( s.equals( "REMOTE" ) ) {
            return SAHPI_ENT_REMOTE;
        }
        if ( s.equals( "EXTERNAL_ENVIRONMENT" ) ) {
            return SAHPI_ENT_EXTERNAL_ENVIRONMENT;
        }
        if ( s.equals( "BATTERY" ) ) {
            return SAHPI_ENT_BATTERY;
        }
        if ( s.equals( "PROCESSING_BLADE" ) ) {
            return SAHPI_ENT_PROCESSING_BLADE;
        }
        if ( s.equals( "CONNECTIVITY_SWITCH" ) ) {
            return SAHPI_ENT_CONNECTIVITY_SWITCH;
        }
        if ( s.equals( "PROCESSOR_MEMORY_MODULE" ) ) {
            return SAHPI_ENT_PROCESSOR_MEMORY_MODULE;
        }
        if ( s.equals( "IO_MODULE" ) ) {
            return SAHPI_ENT_IO_MODULE;
        }
        if ( s.equals( "PROCESSOR_IO_MODULE" ) ) {
            return SAHPI_ENT_PROCESSOR_IO_MODULE;
        }
        if ( s.equals( "MC_FIRMWARE" ) ) {
            return SAHPI_ENT_MC_FIRMWARE;
        }
        if ( s.equals( "IPMI_CHANNEL" ) ) {
            return SAHPI_ENT_IPMI_CHANNEL;
        }
        if ( s.equals( "PCI_BUS" ) ) {
            return SAHPI_ENT_PCI_BUS;
        }
        if ( s.equals( "PCI_EXPRESS_BUS" ) ) {
            return SAHPI_ENT_PCI_EXPRESS_BUS;
        }
        if ( s.equals( "SCSI_BUS" ) ) {
            return SAHPI_ENT_SCSI_BUS;
        }
        if ( s.equals( "SATA_BUS" ) ) {
            return SAHPI_ENT_SATA_BUS;
        }
        if ( s.equals( "PROC_FSB" ) ) {
            return SAHPI_ENT_PROC_FSB;
        }
        if ( s.equals( "CLOCK" ) ) {
            return SAHPI_ENT_CLOCK;
        }
        if ( s.equals( "SYSTEM_FIRMWARE" ) ) {
            return SAHPI_ENT_SYSTEM_FIRMWARE;
        }
        if ( s.equals( "CHASSIS_SPECIFIC" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC;
        }
        if ( s.equals( "CHASSIS_SPECIFIC01" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC01;
        }
        if ( s.equals( "CHASSIS_SPECIFIC02" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC02;
        }
        if ( s.equals( "CHASSIS_SPECIFIC03" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC03;
        }
        if ( s.equals( "CHASSIS_SPECIFIC04" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC04;
        }
        if ( s.equals( "CHASSIS_SPECIFIC05" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC05;
        }
        if ( s.equals( "CHASSIS_SPECIFIC06" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC06;
        }
        if ( s.equals( "CHASSIS_SPECIFIC07" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC07;
        }
        if ( s.equals( "CHASSIS_SPECIFIC08" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC08;
        }
        if ( s.equals( "CHASSIS_SPECIFIC09" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC09;
        }
        if ( s.equals( "CHASSIS_SPECIFIC10" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC10;
        }
        if ( s.equals( "CHASSIS_SPECIFIC11" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC11;
        }
        if ( s.equals( "CHASSIS_SPECIFIC12" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC12;
        }
        if ( s.equals( "CHASSIS_SPECIFIC13" ) ) {
            return SAHPI_ENT_CHASSIS_SPECIFIC13;
        }
        if ( s.equals( "BOARD_SET_SPECIFIC" ) ) {
            return SAHPI_ENT_BOARD_SET_SPECIFIC;
        }
        if ( s.equals( "OEM_SYSINT_SPECIFIC" ) ) {
            return SAHPI_ENT_OEM_SYSINT_SPECIFIC;
        }
        if ( s.equals( "ROOT" ) ) {
            return SAHPI_ENT_ROOT;
        }
        if ( s.equals( "RACK" ) ) {
            return SAHPI_ENT_RACK;
        }
        if ( s.equals( "SUBRACK" ) ) {
            return SAHPI_ENT_SUBRACK;
        }
        if ( s.equals( "COMPACTPCI_CHASSIS" ) ) {
            return SAHPI_ENT_COMPACTPCI_CHASSIS;
        }
        if ( s.equals( "ADVANCEDTCA_CHASSIS" ) ) {
            return SAHPI_ENT_ADVANCEDTCA_CHASSIS;
        }
        if ( s.equals( "RACK_MOUNTED_SERVER" ) ) {
            return SAHPI_ENT_RACK_MOUNTED_SERVER;
        }
        if ( s.equals( "SYSTEM_BLADE" ) ) {
            return SAHPI_ENT_SYSTEM_BLADE;
        }
        if ( s.equals( "SWITCH" ) ) {
            return SAHPI_ENT_SWITCH;
        }
        if ( s.equals( "SWITCH_BLADE" ) ) {
            return SAHPI_ENT_SWITCH_BLADE;
        }
        if ( s.equals( "SBC_BLADE" ) ) {
            return SAHPI_ENT_SBC_BLADE;
        }
        if ( s.equals( "IO_BLADE" ) ) {
            return SAHPI_ENT_IO_BLADE;
        }
        if ( s.equals( "DISK_BLADE" ) ) {
            return SAHPI_ENT_DISK_BLADE;
        }
        if ( s.equals( "DISK_DRIVE" ) ) {
            return SAHPI_ENT_DISK_DRIVE;
        }
        if ( s.equals( "FAN" ) ) {
            return SAHPI_ENT_FAN;
        }
        if ( s.equals( "POWER_DISTRIBUTION_UNIT" ) ) {
            return SAHPI_ENT_POWER_DISTRIBUTION_UNIT;
        }
        if ( s.equals( "SPEC_PROC_BLADE" ) ) {
            return SAHPI_ENT_SPEC_PROC_BLADE;
        }
        if ( s.equals( "IO_SUBBOARD" ) ) {
            return SAHPI_ENT_IO_SUBBOARD;
        }
        if ( s.equals( "SBC_SUBBOARD" ) ) {
            return SAHPI_ENT_SBC_SUBBOARD;
        }
        if ( s.equals( "ALARM_MANAGER" ) ) {
            return SAHPI_ENT_ALARM_MANAGER;
        }
        if ( s.equals( "SHELF_MANAGER" ) ) {
            return SAHPI_ENT_SHELF_MANAGER;
        }
        if ( s.equals( "DISPLAY_PANEL" ) ) {
            return SAHPI_ENT_DISPLAY_PANEL;
        }
        if ( s.equals( "SUBBOARD_CARRIER_BLADE" ) ) {
            return SAHPI_ENT_SUBBOARD_CARRIER_BLADE;
        }
        if ( s.equals( "PHYSICAL_SLOT" ) ) {
            return SAHPI_ENT_PHYSICAL_SLOT;
        }
        if ( s.equals( "PICMG_FRONT_BLADE" ) ) {
            return SAHPI_ENT_PICMG_FRONT_BLADE;
        }
        if ( s.equals( "SYSTEM_INVENTORY_DEVICE" ) ) {
            return SAHPI_ENT_SYSTEM_INVENTORY_DEVICE;
        }
        if ( s.equals( "FILTRATION_UNIT" ) ) {
            return SAHPI_ENT_FILTRATION_UNIT;
        }
        if ( s.equals( "AMC" ) ) {
            return SAHPI_ENT_AMC;
        }
        if ( s.equals( "BMC" ) ) {
            return SAHPI_ENT_BMC;
        }
        if ( s.equals( "IPMC" ) ) {
            return SAHPI_ENT_IPMC;
        }
        if ( s.equals( "MMC" ) ) {
            return SAHPI_ENT_MMC;
        }
        if ( s.equals( "SHMC" ) ) {
            return SAHPI_ENT_SHMC;
        }
        if ( s.equals( "CPLD" ) ) {
            return SAHPI_ENT_CPLD;
        }
        if ( s.equals( "EPLD" ) ) {
            return SAHPI_ENT_EPLD;
        }
        if ( s.equals( "FPGA" ) ) {
            return SAHPI_ENT_FPGA;
        }
        if ( s.equals( "DASD" ) ) {
            return SAHPI_ENT_DASD;
        }
        if ( s.equals( "NIC" ) ) {
            return SAHPI_ENT_NIC;
        }
        if ( s.equals( "DSP" ) ) {
            return SAHPI_ENT_DSP;
        }
        if ( s.equals( "UCODE" ) ) {
            return SAHPI_ENT_UCODE;
        }
        if ( s.equals( "NPU" ) ) {
            return SAHPI_ENT_NPU;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_ENT_OEM;
        }
        if ( s.equals( "INTERFACE" ) ) {
            return SAHPI_ENT_INTERFACE;
        }
        if ( s.equals( "MICROTCA_CHASSIS" ) ) {
            return SAHPI_ENT_MICROTCA_CHASSIS;
        }
        if ( s.equals( "CARRIER" ) ) {
            return SAHPI_ENT_CARRIER;
        }
        if ( s.equals( "CARRIER_MANAGER" ) ) {
            return SAHPI_ENT_CARRIER_MANAGER;
        }
        if ( s.equals( "CONFIG_DATA" ) ) {
            return SAHPI_ENT_CONFIG_DATA;
        }
        if ( s.equals( "INDICATOR" ) ) {
            return SAHPI_ENT_INDICATOR;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorTypeT
     */
    public static String fromSaHpiSensorTypeT( long x )
    {
        if ( x == SAHPI_TEMPERATURE ) {
            return "TEMPERATURE";
        }
        if ( x == SAHPI_VOLTAGE ) {
            return "VOLTAGE";
        }
        if ( x == SAHPI_CURRENT ) {
            return "CURRENT";
        }
        if ( x == SAHPI_FAN ) {
            return "FAN";
        }
        if ( x == SAHPI_PHYSICAL_SECURITY ) {
            return "PHYSICAL_SECURITY";
        }
        if ( x == SAHPI_PLATFORM_VIOLATION ) {
            return "PLATFORM_VIOLATION";
        }
        if ( x == SAHPI_PROCESSOR ) {
            return "PROCESSOR";
        }
        if ( x == SAHPI_POWER_SUPPLY ) {
            return "POWER_SUPPLY";
        }
        if ( x == SAHPI_POWER_UNIT ) {
            return "POWER_UNIT";
        }
        if ( x == SAHPI_COOLING_DEVICE ) {
            return "COOLING_DEVICE";
        }
        if ( x == SAHPI_OTHER_UNITS_BASED_SENSOR ) {
            return "OTHER_UNITS_BASED_SENSOR";
        }
        if ( x == SAHPI_MEMORY ) {
            return "MEMORY";
        }
        if ( x == SAHPI_DRIVE_SLOT ) {
            return "DRIVE_SLOT";
        }
        if ( x == SAHPI_POST_MEMORY_RESIZE ) {
            return "POST_MEMORY_RESIZE";
        }
        if ( x == SAHPI_SYSTEM_FW_PROGRESS ) {
            return "SYSTEM_FW_PROGRESS";
        }
        if ( x == SAHPI_EVENT_LOGGING_DISABLED ) {
            return "EVENT_LOGGING_DISABLED";
        }
        if ( x == SAHPI_RESERVED1 ) {
            return "RESERVED1";
        }
        if ( x == SAHPI_SYSTEM_EVENT ) {
            return "SYSTEM_EVENT";
        }
        if ( x == SAHPI_CRITICAL_INTERRUPT ) {
            return "CRITICAL_INTERRUPT";
        }
        if ( x == SAHPI_BUTTON ) {
            return "BUTTON";
        }
        if ( x == SAHPI_MODULE_BOARD ) {
            return "MODULE_BOARD";
        }
        if ( x == SAHPI_MICROCONTROLLER_COPROCESSOR ) {
            return "MICROCONTROLLER_COPROCESSOR";
        }
        if ( x == SAHPI_ADDIN_CARD ) {
            return "ADDIN_CARD";
        }
        if ( x == SAHPI_CHASSIS ) {
            return "CHASSIS";
        }
        if ( x == SAHPI_CHIP_SET ) {
            return "CHIP_SET";
        }
        if ( x == SAHPI_OTHER_FRU ) {
            return "OTHER_FRU";
        }
        if ( x == SAHPI_CABLE_INTERCONNECT ) {
            return "CABLE_INTERCONNECT";
        }
        if ( x == SAHPI_TERMINATOR ) {
            return "TERMINATOR";
        }
        if ( x == SAHPI_SYSTEM_BOOT_INITIATED ) {
            return "SYSTEM_BOOT_INITIATED";
        }
        if ( x == SAHPI_BOOT_ERROR ) {
            return "BOOT_ERROR";
        }
        if ( x == SAHPI_OS_BOOT ) {
            return "OS_BOOT";
        }
        if ( x == SAHPI_OS_CRITICAL_STOP ) {
            return "OS_CRITICAL_STOP";
        }
        if ( x == SAHPI_SLOT_CONNECTOR ) {
            return "SLOT_CONNECTOR";
        }
        if ( x == SAHPI_SYSTEM_ACPI_POWER_STATE ) {
            return "SYSTEM_ACPI_POWER_STATE";
        }
        if ( x == SAHPI_RESERVED2 ) {
            return "RESERVED2";
        }
        if ( x == SAHPI_PLATFORM_ALERT ) {
            return "PLATFORM_ALERT";
        }
        if ( x == SAHPI_ENTITY_PRESENCE ) {
            return "ENTITY_PRESENCE";
        }
        if ( x == SAHPI_MONITOR_ASIC_IC ) {
            return "MONITOR_ASIC_IC";
        }
        if ( x == SAHPI_LAN ) {
            return "LAN";
        }
        if ( x == SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH ) {
            return "MANAGEMENT_SUBSYSTEM_HEALTH";
        }
        if ( x == SAHPI_BATTERY ) {
            return "BATTERY";
        }
        if ( x == SAHPI_SESSION_AUDIT ) {
            return "SESSION_AUDIT";
        }
        if ( x == SAHPI_VERSION_CHANGE ) {
            return "VERSION_CHANGE";
        }
        if ( x == SAHPI_OPERATIONAL ) {
            return "OPERATIONAL";
        }
        if ( x == SAHPI_OEM_SENSOR ) {
            return "OEM_SENSOR";
        }
        if ( x == SAHPI_COMM_CHANNEL_LINK_STATE ) {
            return "COMM_CHANNEL_LINK_STATE";
        }
        if ( x == SAHPI_MANAGEMENT_BUS_STATE ) {
            return "MANAGEMENT_BUS_STATE";
        }
        if ( x == SAHPI_COMM_CHANNEL_BUS_STATE ) {
            return "COMM_CHANNEL_BUS_STATE";
        }
        if ( x == SAHPI_CONFIG_DATA ) {
            return "CONFIG_DATA";
        }
        if ( x == SAHPI_POWER_BUDGET ) {
            return "POWER_BUDGET";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "TEMPERATURE" ) ) {
            return SAHPI_TEMPERATURE;
        }
        if ( s.equals( "VOLTAGE" ) ) {
            return SAHPI_VOLTAGE;
        }
        if ( s.equals( "CURRENT" ) ) {
            return SAHPI_CURRENT;
        }
        if ( s.equals( "FAN" ) ) {
            return SAHPI_FAN;
        }
        if ( s.equals( "PHYSICAL_SECURITY" ) ) {
            return SAHPI_PHYSICAL_SECURITY;
        }
        if ( s.equals( "PLATFORM_VIOLATION" ) ) {
            return SAHPI_PLATFORM_VIOLATION;
        }
        if ( s.equals( "PROCESSOR" ) ) {
            return SAHPI_PROCESSOR;
        }
        if ( s.equals( "POWER_SUPPLY" ) ) {
            return SAHPI_POWER_SUPPLY;
        }
        if ( s.equals( "POWER_UNIT" ) ) {
            return SAHPI_POWER_UNIT;
        }
        if ( s.equals( "COOLING_DEVICE" ) ) {
            return SAHPI_COOLING_DEVICE;
        }
        if ( s.equals( "OTHER_UNITS_BASED_SENSOR" ) ) {
            return SAHPI_OTHER_UNITS_BASED_SENSOR;
        }
        if ( s.equals( "MEMORY" ) ) {
            return SAHPI_MEMORY;
        }
        if ( s.equals( "DRIVE_SLOT" ) ) {
            return SAHPI_DRIVE_SLOT;
        }
        if ( s.equals( "POST_MEMORY_RESIZE" ) ) {
            return SAHPI_POST_MEMORY_RESIZE;
        }
        if ( s.equals( "SYSTEM_FW_PROGRESS" ) ) {
            return SAHPI_SYSTEM_FW_PROGRESS;
        }
        if ( s.equals( "EVENT_LOGGING_DISABLED" ) ) {
            return SAHPI_EVENT_LOGGING_DISABLED;
        }
        if ( s.equals( "RESERVED1" ) ) {
            return SAHPI_RESERVED1;
        }
        if ( s.equals( "SYSTEM_EVENT" ) ) {
            return SAHPI_SYSTEM_EVENT;
        }
        if ( s.equals( "CRITICAL_INTERRUPT" ) ) {
            return SAHPI_CRITICAL_INTERRUPT;
        }
        if ( s.equals( "BUTTON" ) ) {
            return SAHPI_BUTTON;
        }
        if ( s.equals( "MODULE_BOARD" ) ) {
            return SAHPI_MODULE_BOARD;
        }
        if ( s.equals( "MICROCONTROLLER_COPROCESSOR" ) ) {
            return SAHPI_MICROCONTROLLER_COPROCESSOR;
        }
        if ( s.equals( "ADDIN_CARD" ) ) {
            return SAHPI_ADDIN_CARD;
        }
        if ( s.equals( "CHASSIS" ) ) {
            return SAHPI_CHASSIS;
        }
        if ( s.equals( "CHIP_SET" ) ) {
            return SAHPI_CHIP_SET;
        }
        if ( s.equals( "OTHER_FRU" ) ) {
            return SAHPI_OTHER_FRU;
        }
        if ( s.equals( "CABLE_INTERCONNECT" ) ) {
            return SAHPI_CABLE_INTERCONNECT;
        }
        if ( s.equals( "TERMINATOR" ) ) {
            return SAHPI_TERMINATOR;
        }
        if ( s.equals( "SYSTEM_BOOT_INITIATED" ) ) {
            return SAHPI_SYSTEM_BOOT_INITIATED;
        }
        if ( s.equals( "BOOT_ERROR" ) ) {
            return SAHPI_BOOT_ERROR;
        }
        if ( s.equals( "OS_BOOT" ) ) {
            return SAHPI_OS_BOOT;
        }
        if ( s.equals( "OS_CRITICAL_STOP" ) ) {
            return SAHPI_OS_CRITICAL_STOP;
        }
        if ( s.equals( "SLOT_CONNECTOR" ) ) {
            return SAHPI_SLOT_CONNECTOR;
        }
        if ( s.equals( "SYSTEM_ACPI_POWER_STATE" ) ) {
            return SAHPI_SYSTEM_ACPI_POWER_STATE;
        }
        if ( s.equals( "RESERVED2" ) ) {
            return SAHPI_RESERVED2;
        }
        if ( s.equals( "PLATFORM_ALERT" ) ) {
            return SAHPI_PLATFORM_ALERT;
        }
        if ( s.equals( "ENTITY_PRESENCE" ) ) {
            return SAHPI_ENTITY_PRESENCE;
        }
        if ( s.equals( "MONITOR_ASIC_IC" ) ) {
            return SAHPI_MONITOR_ASIC_IC;
        }
        if ( s.equals( "LAN" ) ) {
            return SAHPI_LAN;
        }
        if ( s.equals( "MANAGEMENT_SUBSYSTEM_HEALTH" ) ) {
            return SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH;
        }
        if ( s.equals( "BATTERY" ) ) {
            return SAHPI_BATTERY;
        }
        if ( s.equals( "SESSION_AUDIT" ) ) {
            return SAHPI_SESSION_AUDIT;
        }
        if ( s.equals( "VERSION_CHANGE" ) ) {
            return SAHPI_VERSION_CHANGE;
        }
        if ( s.equals( "OPERATIONAL" ) ) {
            return SAHPI_OPERATIONAL;
        }
        if ( s.equals( "OEM_SENSOR" ) ) {
            return SAHPI_OEM_SENSOR;
        }
        if ( s.equals( "COMM_CHANNEL_LINK_STATE" ) ) {
            return SAHPI_COMM_CHANNEL_LINK_STATE;
        }
        if ( s.equals( "MANAGEMENT_BUS_STATE" ) ) {
            return SAHPI_MANAGEMENT_BUS_STATE;
        }
        if ( s.equals( "COMM_CHANNEL_BUS_STATE" ) ) {
            return SAHPI_COMM_CHANNEL_BUS_STATE;
        }
        if ( s.equals( "CONFIG_DATA" ) ) {
            return SAHPI_CONFIG_DATA;
        }
        if ( s.equals( "POWER_BUDGET" ) ) {
            return SAHPI_POWER_BUDGET;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorReadingTypeT
     */
    public static String fromSaHpiSensorReadingTypeT( long x )
    {
        if ( x == SAHPI_SENSOR_READING_TYPE_INT64 ) {
            return "INT64";
        }
        if ( x == SAHPI_SENSOR_READING_TYPE_UINT64 ) {
            return "UINT64";
        }
        if ( x == SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
            return "FLOAT64";
        }
        if ( x == SAHPI_SENSOR_READING_TYPE_BUFFER ) {
            return "BUFFER";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorReadingTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "INT64" ) ) {
            return SAHPI_SENSOR_READING_TYPE_INT64;
        }
        if ( s.equals( "UINT64" ) ) {
            return SAHPI_SENSOR_READING_TYPE_UINT64;
        }
        if ( s.equals( "FLOAT64" ) ) {
            return SAHPI_SENSOR_READING_TYPE_FLOAT64;
        }
        if ( s.equals( "BUFFER" ) ) {
            return SAHPI_SENSOR_READING_TYPE_BUFFER;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorEventMaskActionT
     */
    public static String fromSaHpiSensorEventMaskActionT( long x )
    {
        if ( x == SAHPI_SENS_ADD_EVENTS_TO_MASKS ) {
            return "ADD_EVENTS_TO_MASKS";
        }
        if ( x == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS ) {
            return "REMOVE_EVENTS_FROM_MASKS";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorEventMaskActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "ADD_EVENTS_TO_MASKS" ) ) {
            return SAHPI_SENS_ADD_EVENTS_TO_MASKS;
        }
        if ( s.equals( "REMOVE_EVENTS_FROM_MASKS" ) ) {
            return SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorUnitsT
     */
    public static String fromSaHpiSensorUnitsT( long x )
    {
        if ( x == SAHPI_SU_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }
        if ( x == SAHPI_SU_DEGREES_C ) {
            return "DEGREES_C";
        }
        if ( x == SAHPI_SU_DEGREES_F ) {
            return "DEGREES_F";
        }
        if ( x == SAHPI_SU_DEGREES_K ) {
            return "DEGREES_K";
        }
        if ( x == SAHPI_SU_VOLTS ) {
            return "VOLTS";
        }
        if ( x == SAHPI_SU_AMPS ) {
            return "AMPS";
        }
        if ( x == SAHPI_SU_WATTS ) {
            return "WATTS";
        }
        if ( x == SAHPI_SU_JOULES ) {
            return "JOULES";
        }
        if ( x == SAHPI_SU_COULOMBS ) {
            return "COULOMBS";
        }
        if ( x == SAHPI_SU_VA ) {
            return "VA";
        }
        if ( x == SAHPI_SU_NITS ) {
            return "NITS";
        }
        if ( x == SAHPI_SU_LUMEN ) {
            return "LUMEN";
        }
        if ( x == SAHPI_SU_LUX ) {
            return "LUX";
        }
        if ( x == SAHPI_SU_CANDELA ) {
            return "CANDELA";
        }
        if ( x == SAHPI_SU_KPA ) {
            return "KPA";
        }
        if ( x == SAHPI_SU_PSI ) {
            return "PSI";
        }
        if ( x == SAHPI_SU_NEWTON ) {
            return "NEWTON";
        }
        if ( x == SAHPI_SU_CFM ) {
            return "CFM";
        }
        if ( x == SAHPI_SU_RPM ) {
            return "RPM";
        }
        if ( x == SAHPI_SU_HZ ) {
            return "HZ";
        }
        if ( x == SAHPI_SU_MICROSECOND ) {
            return "MICROSECOND";
        }
        if ( x == SAHPI_SU_MILLISECOND ) {
            return "MILLISECOND";
        }
        if ( x == SAHPI_SU_SECOND ) {
            return "SECOND";
        }
        if ( x == SAHPI_SU_MINUTE ) {
            return "MINUTE";
        }
        if ( x == SAHPI_SU_HOUR ) {
            return "HOUR";
        }
        if ( x == SAHPI_SU_DAY ) {
            return "DAY";
        }
        if ( x == SAHPI_SU_WEEK ) {
            return "WEEK";
        }
        if ( x == SAHPI_SU_MIL ) {
            return "MIL";
        }
        if ( x == SAHPI_SU_INCHES ) {
            return "INCHES";
        }
        if ( x == SAHPI_SU_FEET ) {
            return "FEET";
        }
        if ( x == SAHPI_SU_CU_IN ) {
            return "CU_IN";
        }
        if ( x == SAHPI_SU_CU_FEET ) {
            return "CU_FEET";
        }
        if ( x == SAHPI_SU_MM ) {
            return "MM";
        }
        if ( x == SAHPI_SU_CM ) {
            return "CM";
        }
        if ( x == SAHPI_SU_M ) {
            return "M";
        }
        if ( x == SAHPI_SU_CU_CM ) {
            return "CU_CM";
        }
        if ( x == SAHPI_SU_CU_M ) {
            return "CU_M";
        }
        if ( x == SAHPI_SU_LITERS ) {
            return "LITERS";
        }
        if ( x == SAHPI_SU_FLUID_OUNCE ) {
            return "FLUID_OUNCE";
        }
        if ( x == SAHPI_SU_RADIANS ) {
            return "RADIANS";
        }
        if ( x == SAHPI_SU_STERADIANS ) {
            return "STERADIANS";
        }
        if ( x == SAHPI_SU_REVOLUTIONS ) {
            return "REVOLUTIONS";
        }
        if ( x == SAHPI_SU_CYCLES ) {
            return "CYCLES";
        }
        if ( x == SAHPI_SU_GRAVITIES ) {
            return "GRAVITIES";
        }
        if ( x == SAHPI_SU_OUNCE ) {
            return "OUNCE";
        }
        if ( x == SAHPI_SU_POUND ) {
            return "POUND";
        }
        if ( x == SAHPI_SU_FT_LB ) {
            return "FT_LB";
        }
        if ( x == SAHPI_SU_OZ_IN ) {
            return "OZ_IN";
        }
        if ( x == SAHPI_SU_GAUSS ) {
            return "GAUSS";
        }
        if ( x == SAHPI_SU_GILBERTS ) {
            return "GILBERTS";
        }
        if ( x == SAHPI_SU_HENRY ) {
            return "HENRY";
        }
        if ( x == SAHPI_SU_MILLIHENRY ) {
            return "MILLIHENRY";
        }
        if ( x == SAHPI_SU_FARAD ) {
            return "FARAD";
        }
        if ( x == SAHPI_SU_MICROFARAD ) {
            return "MICROFARAD";
        }
        if ( x == SAHPI_SU_OHMS ) {
            return "OHMS";
        }
        if ( x == SAHPI_SU_SIEMENS ) {
            return "SIEMENS";
        }
        if ( x == SAHPI_SU_MOLE ) {
            return "MOLE";
        }
        if ( x == SAHPI_SU_BECQUEREL ) {
            return "BECQUEREL";
        }
        if ( x == SAHPI_SU_PPM ) {
            return "PPM";
        }
        if ( x == SAHPI_SU_RESERVED ) {
            return "RESERVED";
        }
        if ( x == SAHPI_SU_DECIBELS ) {
            return "DECIBELS";
        }
        if ( x == SAHPI_SU_DBA ) {
            return "DBA";
        }
        if ( x == SAHPI_SU_DBC ) {
            return "DBC";
        }
        if ( x == SAHPI_SU_GRAY ) {
            return "GRAY";
        }
        if ( x == SAHPI_SU_SIEVERT ) {
            return "SIEVERT";
        }
        if ( x == SAHPI_SU_COLOR_TEMP_DEG_K ) {
            return "COLOR_TEMP_DEG_K";
        }
        if ( x == SAHPI_SU_BIT ) {
            return "BIT";
        }
        if ( x == SAHPI_SU_KILOBIT ) {
            return "KILOBIT";
        }
        if ( x == SAHPI_SU_MEGABIT ) {
            return "MEGABIT";
        }
        if ( x == SAHPI_SU_GIGABIT ) {
            return "GIGABIT";
        }
        if ( x == SAHPI_SU_BYTE ) {
            return "BYTE";
        }
        if ( x == SAHPI_SU_KILOBYTE ) {
            return "KILOBYTE";
        }
        if ( x == SAHPI_SU_MEGABYTE ) {
            return "MEGABYTE";
        }
        if ( x == SAHPI_SU_GIGABYTE ) {
            return "GIGABYTE";
        }
        if ( x == SAHPI_SU_WORD ) {
            return "WORD";
        }
        if ( x == SAHPI_SU_DWORD ) {
            return "DWORD";
        }
        if ( x == SAHPI_SU_QWORD ) {
            return "QWORD";
        }
        if ( x == SAHPI_SU_LINE ) {
            return "LINE";
        }
        if ( x == SAHPI_SU_HIT ) {
            return "HIT";
        }
        if ( x == SAHPI_SU_MISS ) {
            return "MISS";
        }
        if ( x == SAHPI_SU_RETRY ) {
            return "RETRY";
        }
        if ( x == SAHPI_SU_RESET ) {
            return "RESET";
        }
        if ( x == SAHPI_SU_OVERRUN ) {
            return "OVERRUN";
        }
        if ( x == SAHPI_SU_UNDERRUN ) {
            return "UNDERRUN";
        }
        if ( x == SAHPI_SU_COLLISION ) {
            return "COLLISION";
        }
        if ( x == SAHPI_SU_PACKETS ) {
            return "PACKETS";
        }
        if ( x == SAHPI_SU_MESSAGES ) {
            return "MESSAGES";
        }
        if ( x == SAHPI_SU_CHARACTERS ) {
            return "CHARACTERS";
        }
        if ( x == SAHPI_SU_ERRORS ) {
            return "ERRORS";
        }
        if ( x == SAHPI_SU_CORRECTABLE_ERRORS ) {
            return "CORRECTABLE_ERRORS";
        }
        if ( x == SAHPI_SU_UNCORRECTABLE_ERRORS ) {
            return "UNCORRECTABLE_ERRORS";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorUnitsT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_SU_UNSPECIFIED;
        }
        if ( s.equals( "DEGREES_C" ) ) {
            return SAHPI_SU_DEGREES_C;
        }
        if ( s.equals( "DEGREES_F" ) ) {
            return SAHPI_SU_DEGREES_F;
        }
        if ( s.equals( "DEGREES_K" ) ) {
            return SAHPI_SU_DEGREES_K;
        }
        if ( s.equals( "VOLTS" ) ) {
            return SAHPI_SU_VOLTS;
        }
        if ( s.equals( "AMPS" ) ) {
            return SAHPI_SU_AMPS;
        }
        if ( s.equals( "WATTS" ) ) {
            return SAHPI_SU_WATTS;
        }
        if ( s.equals( "JOULES" ) ) {
            return SAHPI_SU_JOULES;
        }
        if ( s.equals( "COULOMBS" ) ) {
            return SAHPI_SU_COULOMBS;
        }
        if ( s.equals( "VA" ) ) {
            return SAHPI_SU_VA;
        }
        if ( s.equals( "NITS" ) ) {
            return SAHPI_SU_NITS;
        }
        if ( s.equals( "LUMEN" ) ) {
            return SAHPI_SU_LUMEN;
        }
        if ( s.equals( "LUX" ) ) {
            return SAHPI_SU_LUX;
        }
        if ( s.equals( "CANDELA" ) ) {
            return SAHPI_SU_CANDELA;
        }
        if ( s.equals( "KPA" ) ) {
            return SAHPI_SU_KPA;
        }
        if ( s.equals( "PSI" ) ) {
            return SAHPI_SU_PSI;
        }
        if ( s.equals( "NEWTON" ) ) {
            return SAHPI_SU_NEWTON;
        }
        if ( s.equals( "CFM" ) ) {
            return SAHPI_SU_CFM;
        }
        if ( s.equals( "RPM" ) ) {
            return SAHPI_SU_RPM;
        }
        if ( s.equals( "HZ" ) ) {
            return SAHPI_SU_HZ;
        }
        if ( s.equals( "MICROSECOND" ) ) {
            return SAHPI_SU_MICROSECOND;
        }
        if ( s.equals( "MILLISECOND" ) ) {
            return SAHPI_SU_MILLISECOND;
        }
        if ( s.equals( "SECOND" ) ) {
            return SAHPI_SU_SECOND;
        }
        if ( s.equals( "MINUTE" ) ) {
            return SAHPI_SU_MINUTE;
        }
        if ( s.equals( "HOUR" ) ) {
            return SAHPI_SU_HOUR;
        }
        if ( s.equals( "DAY" ) ) {
            return SAHPI_SU_DAY;
        }
        if ( s.equals( "WEEK" ) ) {
            return SAHPI_SU_WEEK;
        }
        if ( s.equals( "MIL" ) ) {
            return SAHPI_SU_MIL;
        }
        if ( s.equals( "INCHES" ) ) {
            return SAHPI_SU_INCHES;
        }
        if ( s.equals( "FEET" ) ) {
            return SAHPI_SU_FEET;
        }
        if ( s.equals( "CU_IN" ) ) {
            return SAHPI_SU_CU_IN;
        }
        if ( s.equals( "CU_FEET" ) ) {
            return SAHPI_SU_CU_FEET;
        }
        if ( s.equals( "MM" ) ) {
            return SAHPI_SU_MM;
        }
        if ( s.equals( "CM" ) ) {
            return SAHPI_SU_CM;
        }
        if ( s.equals( "M" ) ) {
            return SAHPI_SU_M;
        }
        if ( s.equals( "CU_CM" ) ) {
            return SAHPI_SU_CU_CM;
        }
        if ( s.equals( "CU_M" ) ) {
            return SAHPI_SU_CU_M;
        }
        if ( s.equals( "LITERS" ) ) {
            return SAHPI_SU_LITERS;
        }
        if ( s.equals( "FLUID_OUNCE" ) ) {
            return SAHPI_SU_FLUID_OUNCE;
        }
        if ( s.equals( "RADIANS" ) ) {
            return SAHPI_SU_RADIANS;
        }
        if ( s.equals( "STERADIANS" ) ) {
            return SAHPI_SU_STERADIANS;
        }
        if ( s.equals( "REVOLUTIONS" ) ) {
            return SAHPI_SU_REVOLUTIONS;
        }
        if ( s.equals( "CYCLES" ) ) {
            return SAHPI_SU_CYCLES;
        }
        if ( s.equals( "GRAVITIES" ) ) {
            return SAHPI_SU_GRAVITIES;
        }
        if ( s.equals( "OUNCE" ) ) {
            return SAHPI_SU_OUNCE;
        }
        if ( s.equals( "POUND" ) ) {
            return SAHPI_SU_POUND;
        }
        if ( s.equals( "FT_LB" ) ) {
            return SAHPI_SU_FT_LB;
        }
        if ( s.equals( "OZ_IN" ) ) {
            return SAHPI_SU_OZ_IN;
        }
        if ( s.equals( "GAUSS" ) ) {
            return SAHPI_SU_GAUSS;
        }
        if ( s.equals( "GILBERTS" ) ) {
            return SAHPI_SU_GILBERTS;
        }
        if ( s.equals( "HENRY" ) ) {
            return SAHPI_SU_HENRY;
        }
        if ( s.equals( "MILLIHENRY" ) ) {
            return SAHPI_SU_MILLIHENRY;
        }
        if ( s.equals( "FARAD" ) ) {
            return SAHPI_SU_FARAD;
        }
        if ( s.equals( "MICROFARAD" ) ) {
            return SAHPI_SU_MICROFARAD;
        }
        if ( s.equals( "OHMS" ) ) {
            return SAHPI_SU_OHMS;
        }
        if ( s.equals( "SIEMENS" ) ) {
            return SAHPI_SU_SIEMENS;
        }
        if ( s.equals( "MOLE" ) ) {
            return SAHPI_SU_MOLE;
        }
        if ( s.equals( "BECQUEREL" ) ) {
            return SAHPI_SU_BECQUEREL;
        }
        if ( s.equals( "PPM" ) ) {
            return SAHPI_SU_PPM;
        }
        if ( s.equals( "RESERVED" ) ) {
            return SAHPI_SU_RESERVED;
        }
        if ( s.equals( "DECIBELS" ) ) {
            return SAHPI_SU_DECIBELS;
        }
        if ( s.equals( "DBA" ) ) {
            return SAHPI_SU_DBA;
        }
        if ( s.equals( "DBC" ) ) {
            return SAHPI_SU_DBC;
        }
        if ( s.equals( "GRAY" ) ) {
            return SAHPI_SU_GRAY;
        }
        if ( s.equals( "SIEVERT" ) ) {
            return SAHPI_SU_SIEVERT;
        }
        if ( s.equals( "COLOR_TEMP_DEG_K" ) ) {
            return SAHPI_SU_COLOR_TEMP_DEG_K;
        }
        if ( s.equals( "BIT" ) ) {
            return SAHPI_SU_BIT;
        }
        if ( s.equals( "KILOBIT" ) ) {
            return SAHPI_SU_KILOBIT;
        }
        if ( s.equals( "MEGABIT" ) ) {
            return SAHPI_SU_MEGABIT;
        }
        if ( s.equals( "GIGABIT" ) ) {
            return SAHPI_SU_GIGABIT;
        }
        if ( s.equals( "BYTE" ) ) {
            return SAHPI_SU_BYTE;
        }
        if ( s.equals( "KILOBYTE" ) ) {
            return SAHPI_SU_KILOBYTE;
        }
        if ( s.equals( "MEGABYTE" ) ) {
            return SAHPI_SU_MEGABYTE;
        }
        if ( s.equals( "GIGABYTE" ) ) {
            return SAHPI_SU_GIGABYTE;
        }
        if ( s.equals( "WORD" ) ) {
            return SAHPI_SU_WORD;
        }
        if ( s.equals( "DWORD" ) ) {
            return SAHPI_SU_DWORD;
        }
        if ( s.equals( "QWORD" ) ) {
            return SAHPI_SU_QWORD;
        }
        if ( s.equals( "LINE" ) ) {
            return SAHPI_SU_LINE;
        }
        if ( s.equals( "HIT" ) ) {
            return SAHPI_SU_HIT;
        }
        if ( s.equals( "MISS" ) ) {
            return SAHPI_SU_MISS;
        }
        if ( s.equals( "RETRY" ) ) {
            return SAHPI_SU_RETRY;
        }
        if ( s.equals( "RESET" ) ) {
            return SAHPI_SU_RESET;
        }
        if ( s.equals( "OVERRUN" ) ) {
            return SAHPI_SU_OVERRUN;
        }
        if ( s.equals( "UNDERRUN" ) ) {
            return SAHPI_SU_UNDERRUN;
        }
        if ( s.equals( "COLLISION" ) ) {
            return SAHPI_SU_COLLISION;
        }
        if ( s.equals( "PACKETS" ) ) {
            return SAHPI_SU_PACKETS;
        }
        if ( s.equals( "MESSAGES" ) ) {
            return SAHPI_SU_MESSAGES;
        }
        if ( s.equals( "CHARACTERS" ) ) {
            return SAHPI_SU_CHARACTERS;
        }
        if ( s.equals( "ERRORS" ) ) {
            return SAHPI_SU_ERRORS;
        }
        if ( s.equals( "CORRECTABLE_ERRORS" ) ) {
            return SAHPI_SU_CORRECTABLE_ERRORS;
        }
        if ( s.equals( "UNCORRECTABLE_ERRORS" ) ) {
            return SAHPI_SU_UNCORRECTABLE_ERRORS;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorModUnitUseT
     */
    public static String fromSaHpiSensorModUnitUseT( long x )
    {
        if ( x == SAHPI_SMUU_NONE ) {
            return "NONE";
        }
        if ( x == SAHPI_SMUU_BASIC_OVER_MODIFIER ) {
            return "BASIC_OVER_MODIFIER";
        }
        if ( x == SAHPI_SMUU_BASIC_TIMES_MODIFIER ) {
            return "BASIC_TIMES_MODIFIER";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorModUnitUseT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONE" ) ) {
            return SAHPI_SMUU_NONE;
        }
        if ( s.equals( "BASIC_OVER_MODIFIER" ) ) {
            return SAHPI_SMUU_BASIC_OVER_MODIFIER;
        }
        if ( s.equals( "BASIC_TIMES_MODIFIER" ) ) {
            return SAHPI_SMUU_BASIC_TIMES_MODIFIER;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSensorEventCtrlT
     */
    public static String fromSaHpiSensorEventCtrlT( long x )
    {
        if ( x == SAHPI_SEC_PER_EVENT ) {
            return "PER_EVENT";
        }
        if ( x == SAHPI_SEC_READ_ONLY_MASKS ) {
            return "READ_ONLY_MASKS";
        }
        if ( x == SAHPI_SEC_READ_ONLY ) {
            return "READ_ONLY";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSensorEventCtrlT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "PER_EVENT" ) ) {
            return SAHPI_SEC_PER_EVENT;
        }
        if ( s.equals( "READ_ONLY_MASKS" ) ) {
            return SAHPI_SEC_READ_ONLY_MASKS;
        }
        if ( s.equals( "READ_ONLY" ) ) {
            return SAHPI_SEC_READ_ONLY;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiCtrlTypeT
     */
    public static String fromSaHpiCtrlTypeT( long x )
    {
        if ( x == SAHPI_CTRL_TYPE_DIGITAL ) {
            return "DIGITAL";
        }
        if ( x == SAHPI_CTRL_TYPE_DISCRETE ) {
            return "DISCRETE";
        }
        if ( x == SAHPI_CTRL_TYPE_ANALOG ) {
            return "ANALOG";
        }
        if ( x == SAHPI_CTRL_TYPE_STREAM ) {
            return "STREAM";
        }
        if ( x == SAHPI_CTRL_TYPE_TEXT ) {
            return "TEXT";
        }
        if ( x == SAHPI_CTRL_TYPE_OEM ) {
            return "OEM";
        }

        return Long.toString( x );
    }

    public static long toSaHpiCtrlTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "DIGITAL" ) ) {
            return SAHPI_CTRL_TYPE_DIGITAL;
        }
        if ( s.equals( "DISCRETE" ) ) {
            return SAHPI_CTRL_TYPE_DISCRETE;
        }
        if ( s.equals( "ANALOG" ) ) {
            return SAHPI_CTRL_TYPE_ANALOG;
        }
        if ( s.equals( "STREAM" ) ) {
            return SAHPI_CTRL_TYPE_STREAM;
        }
        if ( s.equals( "TEXT" ) ) {
            return SAHPI_CTRL_TYPE_TEXT;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_CTRL_TYPE_OEM;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiCtrlStateDigitalT
     */
    public static String fromSaHpiCtrlStateDigitalT( long x )
    {
        if ( x == SAHPI_CTRL_STATE_OFF ) {
            return "OFF";
        }
        if ( x == SAHPI_CTRL_STATE_ON ) {
            return "ON";
        }
        if ( x == SAHPI_CTRL_STATE_PULSE_OFF ) {
            return "PULSE_OFF";
        }
        if ( x == SAHPI_CTRL_STATE_PULSE_ON ) {
            return "PULSE_ON";
        }

        return Long.toString( x );
    }

    public static long toSaHpiCtrlStateDigitalT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "OFF" ) ) {
            return SAHPI_CTRL_STATE_OFF;
        }
        if ( s.equals( "ON" ) ) {
            return SAHPI_CTRL_STATE_ON;
        }
        if ( s.equals( "PULSE_OFF" ) ) {
            return SAHPI_CTRL_STATE_PULSE_OFF;
        }
        if ( s.equals( "PULSE_ON" ) ) {
            return SAHPI_CTRL_STATE_PULSE_ON;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiCtrlModeT
     */
    public static String fromSaHpiCtrlModeT( long x )
    {
        if ( x == SAHPI_CTRL_MODE_AUTO ) {
            return "AUTO";
        }
        if ( x == SAHPI_CTRL_MODE_MANUAL ) {
            return "MANUAL";
        }

        return Long.toString( x );
    }

    public static long toSaHpiCtrlModeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUTO" ) ) {
            return SAHPI_CTRL_MODE_AUTO;
        }
        if ( s.equals( "MANUAL" ) ) {
            return SAHPI_CTRL_MODE_MANUAL;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiCtrlOutputTypeT
     */
    public static String fromSaHpiCtrlOutputTypeT( long x )
    {
        if ( x == SAHPI_CTRL_GENERIC ) {
            return "GENERIC";
        }
        if ( x == SAHPI_CTRL_LED ) {
            return "LED";
        }
        if ( x == SAHPI_CTRL_FAN_SPEED ) {
            return "FAN_SPEED";
        }
        if ( x == SAHPI_CTRL_DRY_CONTACT_CLOSURE ) {
            return "DRY_CONTACT_CLOSURE";
        }
        if ( x == SAHPI_CTRL_POWER_SUPPLY_INHIBIT ) {
            return "POWER_SUPPLY_INHIBIT";
        }
        if ( x == SAHPI_CTRL_AUDIBLE ) {
            return "AUDIBLE";
        }
        if ( x == SAHPI_CTRL_FRONT_PANEL_LOCKOUT ) {
            return "FRONT_PANEL_LOCKOUT";
        }
        if ( x == SAHPI_CTRL_POWER_INTERLOCK ) {
            return "POWER_INTERLOCK";
        }
        if ( x == SAHPI_CTRL_POWER_STATE ) {
            return "POWER_STATE";
        }
        if ( x == SAHPI_CTRL_LCD_DISPLAY ) {
            return "LCD_DISPLAY";
        }
        if ( x == SAHPI_CTRL_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_CTRL_GENERIC_ADDRESS ) {
            return "GENERIC_ADDRESS";
        }
        if ( x == SAHPI_CTRL_IP_ADDRESS ) {
            return "IP_ADDRESS";
        }
        if ( x == SAHPI_CTRL_RESOURCE_ID ) {
            return "RESOURCE_ID";
        }
        if ( x == SAHPI_CTRL_POWER_BUDGET ) {
            return "POWER_BUDGET";
        }
        if ( x == SAHPI_CTRL_ACTIVATE ) {
            return "ACTIVATE";
        }
        if ( x == SAHPI_CTRL_RESET ) {
            return "RESET";
        }

        return Long.toString( x );
    }

    public static long toSaHpiCtrlOutputTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "GENERIC" ) ) {
            return SAHPI_CTRL_GENERIC;
        }
        if ( s.equals( "LED" ) ) {
            return SAHPI_CTRL_LED;
        }
        if ( s.equals( "FAN_SPEED" ) ) {
            return SAHPI_CTRL_FAN_SPEED;
        }
        if ( s.equals( "DRY_CONTACT_CLOSURE" ) ) {
            return SAHPI_CTRL_DRY_CONTACT_CLOSURE;
        }
        if ( s.equals( "POWER_SUPPLY_INHIBIT" ) ) {
            return SAHPI_CTRL_POWER_SUPPLY_INHIBIT;
        }
        if ( s.equals( "AUDIBLE" ) ) {
            return SAHPI_CTRL_AUDIBLE;
        }
        if ( s.equals( "FRONT_PANEL_LOCKOUT" ) ) {
            return SAHPI_CTRL_FRONT_PANEL_LOCKOUT;
        }
        if ( s.equals( "POWER_INTERLOCK" ) ) {
            return SAHPI_CTRL_POWER_INTERLOCK;
        }
        if ( s.equals( "POWER_STATE" ) ) {
            return SAHPI_CTRL_POWER_STATE;
        }
        if ( s.equals( "LCD_DISPLAY" ) ) {
            return SAHPI_CTRL_LCD_DISPLAY;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_CTRL_OEM;
        }
        if ( s.equals( "GENERIC_ADDRESS" ) ) {
            return SAHPI_CTRL_GENERIC_ADDRESS;
        }
        if ( s.equals( "IP_ADDRESS" ) ) {
            return SAHPI_CTRL_IP_ADDRESS;
        }
        if ( s.equals( "RESOURCE_ID" ) ) {
            return SAHPI_CTRL_RESOURCE_ID;
        }
        if ( s.equals( "POWER_BUDGET" ) ) {
            return SAHPI_CTRL_POWER_BUDGET;
        }
        if ( s.equals( "ACTIVATE" ) ) {
            return SAHPI_CTRL_ACTIVATE;
        }
        if ( s.equals( "RESET" ) ) {
            return SAHPI_CTRL_RESET;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiIdrAreaTypeT
     */
    public static String fromSaHpiIdrAreaTypeT( long x )
    {
        if ( x == SAHPI_IDR_AREATYPE_INTERNAL_USE ) {
            return "INTERNAL_USE";
        }
        if ( x == SAHPI_IDR_AREATYPE_CHASSIS_INFO ) {
            return "CHASSIS_INFO";
        }
        if ( x == SAHPI_IDR_AREATYPE_BOARD_INFO ) {
            return "BOARD_INFO";
        }
        if ( x == SAHPI_IDR_AREATYPE_PRODUCT_INFO ) {
            return "PRODUCT_INFO";
        }
        if ( x == SAHPI_IDR_AREATYPE_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_IDR_AREATYPE_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiIdrAreaTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "INTERNAL_USE" ) ) {
            return SAHPI_IDR_AREATYPE_INTERNAL_USE;
        }
        if ( s.equals( "CHASSIS_INFO" ) ) {
            return SAHPI_IDR_AREATYPE_CHASSIS_INFO;
        }
        if ( s.equals( "BOARD_INFO" ) ) {
            return SAHPI_IDR_AREATYPE_BOARD_INFO;
        }
        if ( s.equals( "PRODUCT_INFO" ) ) {
            return SAHPI_IDR_AREATYPE_PRODUCT_INFO;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_IDR_AREATYPE_OEM;
        }
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_IDR_AREATYPE_UNSPECIFIED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiIdrFieldTypeT
     */
    public static String fromSaHpiIdrFieldTypeT( long x )
    {
        if ( x == SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE ) {
            return "CHASSIS_TYPE";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_MFG_DATETIME ) {
            return "MFG_DATETIME";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_MANUFACTURER ) {
            return "MANUFACTURER";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_PRODUCT_NAME ) {
            return "PRODUCT_NAME";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION ) {
            return "PRODUCT_VERSION";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER ) {
            return "SERIAL_NUMBER";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_PART_NUMBER ) {
            return "PART_NUMBER";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_FILE_ID ) {
            return "FILE_ID";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_ASSET_TAG ) {
            return "ASSET_TAG";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_CUSTOM ) {
            return "CUSTOM";
        }
        if ( x == SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiIdrFieldTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "CHASSIS_TYPE" ) ) {
            return SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE;
        }
        if ( s.equals( "MFG_DATETIME" ) ) {
            return SAHPI_IDR_FIELDTYPE_MFG_DATETIME;
        }
        if ( s.equals( "MANUFACTURER" ) ) {
            return SAHPI_IDR_FIELDTYPE_MANUFACTURER;
        }
        if ( s.equals( "PRODUCT_NAME" ) ) {
            return SAHPI_IDR_FIELDTYPE_PRODUCT_NAME;
        }
        if ( s.equals( "PRODUCT_VERSION" ) ) {
            return SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION;
        }
        if ( s.equals( "SERIAL_NUMBER" ) ) {
            return SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER;
        }
        if ( s.equals( "PART_NUMBER" ) ) {
            return SAHPI_IDR_FIELDTYPE_PART_NUMBER;
        }
        if ( s.equals( "FILE_ID" ) ) {
            return SAHPI_IDR_FIELDTYPE_FILE_ID;
        }
        if ( s.equals( "ASSET_TAG" ) ) {
            return SAHPI_IDR_FIELDTYPE_ASSET_TAG;
        }
        if ( s.equals( "CUSTOM" ) ) {
            return SAHPI_IDR_FIELDTYPE_CUSTOM;
        }
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiWatchdogActionT
     */
    public static String fromSaHpiWatchdogActionT( long x )
    {
        if ( x == SAHPI_WA_NO_ACTION ) {
            return "NO_ACTION";
        }
        if ( x == SAHPI_WA_RESET ) {
            return "RESET";
        }
        if ( x == SAHPI_WA_POWER_DOWN ) {
            return "POWER_DOWN";
        }
        if ( x == SAHPI_WA_POWER_CYCLE ) {
            return "POWER_CYCLE";
        }

        return Long.toString( x );
    }

    public static long toSaHpiWatchdogActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NO_ACTION" ) ) {
            return SAHPI_WA_NO_ACTION;
        }
        if ( s.equals( "RESET" ) ) {
            return SAHPI_WA_RESET;
        }
        if ( s.equals( "POWER_DOWN" ) ) {
            return SAHPI_WA_POWER_DOWN;
        }
        if ( s.equals( "POWER_CYCLE" ) ) {
            return SAHPI_WA_POWER_CYCLE;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiWatchdogActionEventT
     */
    public static String fromSaHpiWatchdogActionEventT( long x )
    {
        if ( x == SAHPI_WAE_NO_ACTION ) {
            return "NO_ACTION";
        }
        if ( x == SAHPI_WAE_RESET ) {
            return "RESET";
        }
        if ( x == SAHPI_WAE_POWER_DOWN ) {
            return "POWER_DOWN";
        }
        if ( x == SAHPI_WAE_POWER_CYCLE ) {
            return "POWER_CYCLE";
        }
        if ( x == SAHPI_WAE_TIMER_INT ) {
            return "TIMER_INT";
        }

        return Long.toString( x );
    }

    public static long toSaHpiWatchdogActionEventT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NO_ACTION" ) ) {
            return SAHPI_WAE_NO_ACTION;
        }
        if ( s.equals( "RESET" ) ) {
            return SAHPI_WAE_RESET;
        }
        if ( s.equals( "POWER_DOWN" ) ) {
            return SAHPI_WAE_POWER_DOWN;
        }
        if ( s.equals( "POWER_CYCLE" ) ) {
            return SAHPI_WAE_POWER_CYCLE;
        }
        if ( s.equals( "TIMER_INT" ) ) {
            return SAHPI_WAE_TIMER_INT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiWatchdogPretimerInterruptT
     */
    public static String fromSaHpiWatchdogPretimerInterruptT( long x )
    {
        if ( x == SAHPI_WPI_NONE ) {
            return "NONE";
        }
        if ( x == SAHPI_WPI_SMI ) {
            return "SMI";
        }
        if ( x == SAHPI_WPI_NMI ) {
            return "NMI";
        }
        if ( x == SAHPI_WPI_MESSAGE_INTERRUPT ) {
            return "MESSAGE_INTERRUPT";
        }
        if ( x == SAHPI_WPI_OEM ) {
            return "OEM";
        }

        return Long.toString( x );
    }

    public static long toSaHpiWatchdogPretimerInterruptT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONE" ) ) {
            return SAHPI_WPI_NONE;
        }
        if ( s.equals( "SMI" ) ) {
            return SAHPI_WPI_SMI;
        }
        if ( s.equals( "NMI" ) ) {
            return SAHPI_WPI_NMI;
        }
        if ( s.equals( "MESSAGE_INTERRUPT" ) ) {
            return SAHPI_WPI_MESSAGE_INTERRUPT;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_WPI_OEM;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiWatchdogTimerUseT
     */
    public static String fromSaHpiWatchdogTimerUseT( long x )
    {
        if ( x == SAHPI_WTU_NONE ) {
            return "NONE";
        }
        if ( x == SAHPI_WTU_BIOS_FRB2 ) {
            return "BIOS_FRB2";
        }
        if ( x == SAHPI_WTU_BIOS_POST ) {
            return "BIOS_POST";
        }
        if ( x == SAHPI_WTU_OS_LOAD ) {
            return "OS_LOAD";
        }
        if ( x == SAHPI_WTU_SMS_OS ) {
            return "SMS_OS";
        }
        if ( x == SAHPI_WTU_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_WTU_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiWatchdogTimerUseT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONE" ) ) {
            return SAHPI_WTU_NONE;
        }
        if ( s.equals( "BIOS_FRB2" ) ) {
            return SAHPI_WTU_BIOS_FRB2;
        }
        if ( s.equals( "BIOS_POST" ) ) {
            return SAHPI_WTU_BIOS_POST;
        }
        if ( s.equals( "OS_LOAD" ) ) {
            return SAHPI_WTU_OS_LOAD;
        }
        if ( s.equals( "SMS_OS" ) ) {
            return SAHPI_WTU_SMS_OS;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_WTU_OEM;
        }
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_WTU_UNSPECIFIED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDimiTestServiceImpactT
     */
    public static String fromSaHpiDimiTestServiceImpactT( long x )
    {
        if ( x == SAHPI_DIMITEST_NONDEGRADING ) {
            return "NONDEGRADING";
        }
        if ( x == SAHPI_DIMITEST_DEGRADING ) {
            return "DEGRADING";
        }
        if ( x == SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL ) {
            return "VENDOR_DEFINED_LEVEL";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDimiTestServiceImpactT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONDEGRADING" ) ) {
            return SAHPI_DIMITEST_NONDEGRADING;
        }
        if ( s.equals( "DEGRADING" ) ) {
            return SAHPI_DIMITEST_DEGRADING;
        }
        if ( s.equals( "VENDOR_DEFINED_LEVEL" ) ) {
            return SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDimiTestRunStatusT
     */
    public static String fromSaHpiDimiTestRunStatusT( long x )
    {
        if ( x == SAHPI_DIMITEST_STATUS_NOT_RUN ) {
            return "NOT_RUN";
        }
        if ( x == SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS ) {
            return "FINISHED_NO_ERRORS";
        }
        if ( x == SAHPI_DIMITEST_STATUS_FINISHED_ERRORS ) {
            return "FINISHED_ERRORS";
        }
        if ( x == SAHPI_DIMITEST_STATUS_CANCELED ) {
            return "CANCELED";
        }
        if ( x == SAHPI_DIMITEST_STATUS_RUNNING ) {
            return "RUNNING";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDimiTestRunStatusT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NOT_RUN" ) ) {
            return SAHPI_DIMITEST_STATUS_NOT_RUN;
        }
        if ( s.equals( "FINISHED_NO_ERRORS" ) ) {
            return SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS;
        }
        if ( s.equals( "FINISHED_ERRORS" ) ) {
            return SAHPI_DIMITEST_STATUS_FINISHED_ERRORS;
        }
        if ( s.equals( "CANCELED" ) ) {
            return SAHPI_DIMITEST_STATUS_CANCELED;
        }
        if ( s.equals( "RUNNING" ) ) {
            return SAHPI_DIMITEST_STATUS_RUNNING;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDimiTestErrCodeT
     */
    public static String fromSaHpiDimiTestErrCodeT( long x )
    {
        if ( x == SAHPI_DIMITEST_STATUSERR_NOERR ) {
            return "NOERR";
        }
        if ( x == SAHPI_DIMITEST_STATUSERR_RUNERR ) {
            return "RUNERR";
        }
        if ( x == SAHPI_DIMITEST_STATUSERR_UNDEF ) {
            return "UNDEF";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDimiTestErrCodeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NOERR" ) ) {
            return SAHPI_DIMITEST_STATUSERR_NOERR;
        }
        if ( s.equals( "RUNERR" ) ) {
            return SAHPI_DIMITEST_STATUSERR_RUNERR;
        }
        if ( s.equals( "UNDEF" ) ) {
            return SAHPI_DIMITEST_STATUSERR_UNDEF;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDimiTestParamTypeT
     */
    public static String fromSaHpiDimiTestParamTypeT( long x )
    {
        if ( x == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
            return "BOOLEAN";
        }
        if ( x == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
            return "INT32";
        }
        if ( x == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
            return "FLOAT64";
        }
        if ( x == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
            return "TEXT";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDimiTestParamTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "BOOLEAN" ) ) {
            return SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN;
        }
        if ( s.equals( "INT32" ) ) {
            return SAHPI_DIMITEST_PARAM_TYPE_INT32;
        }
        if ( s.equals( "FLOAT64" ) ) {
            return SAHPI_DIMITEST_PARAM_TYPE_FLOAT64;
        }
        if ( s.equals( "TEXT" ) ) {
            return SAHPI_DIMITEST_PARAM_TYPE_TEXT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDimiReadyT
     */
    public static String fromSaHpiDimiReadyT( long x )
    {
        if ( x == SAHPI_DIMI_READY ) {
            return "READY";
        }
        if ( x == SAHPI_DIMI_WRONG_STATE ) {
            return "WRONG_STATE";
        }
        if ( x == SAHPI_DIMI_BUSY ) {
            return "BUSY";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDimiReadyT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "READY" ) ) {
            return SAHPI_DIMI_READY;
        }
        if ( s.equals( "WRONG_STATE" ) ) {
            return SAHPI_DIMI_WRONG_STATE;
        }
        if ( s.equals( "BUSY" ) ) {
            return SAHPI_DIMI_BUSY;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiSpecInfoTypeT
     */
    public static String fromSaHpiFumiSpecInfoTypeT( long x )
    {
        if ( x == SAHPI_FUMI_SPEC_INFO_NONE ) {
            return "NONE";
        }
        if ( x == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED ) {
            return "SAF_DEFINED";
        }
        if ( x == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED ) {
            return "OEM_DEFINED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiSpecInfoTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONE" ) ) {
            return SAHPI_FUMI_SPEC_INFO_NONE;
        }
        if ( s.equals( "SAF_DEFINED" ) ) {
            return SAHPI_FUMI_SPEC_INFO_SAF_DEFINED;
        }
        if ( s.equals( "OEM_DEFINED" ) ) {
            return SAHPI_FUMI_SPEC_INFO_OEM_DEFINED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiSafDefinedSpecIdT
     */
    public static String fromSaHpiFumiSafDefinedSpecIdT( long x )
    {
        if ( x == SAHPI_FUMI_SPEC_HPM1 ) {
            return "";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiSafDefinedSpecIdT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "" ) ) {
            return SAHPI_FUMI_SPEC_HPM1;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiServiceImpactT
     */
    public static String fromSaHpiFumiServiceImpactT( long x )
    {
        if ( x == SAHPI_FUMI_PROCESS_NONDEGRADING ) {
            return "NONDEGRADING";
        }
        if ( x == SAHPI_FUMI_PROCESS_DEGRADING ) {
            return "DEGRADING";
        }
        if ( x == SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL ) {
            return "VENDOR_DEFINED_IMPACT_LEVEL";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiServiceImpactT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NONDEGRADING" ) ) {
            return SAHPI_FUMI_PROCESS_NONDEGRADING;
        }
        if ( s.equals( "DEGRADING" ) ) {
            return SAHPI_FUMI_PROCESS_DEGRADING;
        }
        if ( s.equals( "VENDOR_DEFINED_IMPACT_LEVEL" ) ) {
            return SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiSourceStatusT
     */
    public static String fromSaHpiFumiSourceStatusT( long x )
    {
        if ( x == SAHPI_FUMI_SRC_VALID ) {
            return "VALID";
        }
        if ( x == SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED ) {
            return "PROTOCOL_NOT_SUPPORTED";
        }
        if ( x == SAHPI_FUMI_SRC_UNREACHABLE ) {
            return "UNREACHABLE";
        }
        if ( x == SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED ) {
            return "VALIDATION_NOT_STARTED";
        }
        if ( x == SAHPI_FUMI_SRC_VALIDATION_INITIATED ) {
            return "VALIDATION_INITIATED";
        }
        if ( x == SAHPI_FUMI_SRC_VALIDATION_FAIL ) {
            return "VALIDATION_FAIL";
        }
        if ( x == SAHPI_FUMI_SRC_TYPE_MISMATCH ) {
            return "TYPE_MISMATCH";
        }
        if ( x == SAHPI_FUMI_SRC_INVALID ) {
            return "INVALID";
        }
        if ( x == SAHPI_FUMI_SRC_VALIDITY_UNKNOWN ) {
            return "VALIDITY_UNKNOWN";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiSourceStatusT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "VALID" ) ) {
            return SAHPI_FUMI_SRC_VALID;
        }
        if ( s.equals( "PROTOCOL_NOT_SUPPORTED" ) ) {
            return SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED;
        }
        if ( s.equals( "UNREACHABLE" ) ) {
            return SAHPI_FUMI_SRC_UNREACHABLE;
        }
        if ( s.equals( "VALIDATION_NOT_STARTED" ) ) {
            return SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED;
        }
        if ( s.equals( "VALIDATION_INITIATED" ) ) {
            return SAHPI_FUMI_SRC_VALIDATION_INITIATED;
        }
        if ( s.equals( "VALIDATION_FAIL" ) ) {
            return SAHPI_FUMI_SRC_VALIDATION_FAIL;
        }
        if ( s.equals( "TYPE_MISMATCH" ) ) {
            return SAHPI_FUMI_SRC_TYPE_MISMATCH;
        }
        if ( s.equals( "INVALID" ) ) {
            return SAHPI_FUMI_SRC_INVALID;
        }
        if ( s.equals( "VALIDITY_UNKNOWN" ) ) {
            return SAHPI_FUMI_SRC_VALIDITY_UNKNOWN;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiBankStateT
     */
    public static String fromSaHpiFumiBankStateT( long x )
    {
        if ( x == SAHPI_FUMI_BANK_VALID ) {
            return "VALID";
        }
        if ( x == SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS ) {
            return "UPGRADE_IN_PROGRESS";
        }
        if ( x == SAHPI_FUMI_BANK_CORRUPTED ) {
            return "CORRUPTED";
        }
        if ( x == SAHPI_FUMI_BANK_ACTIVE ) {
            return "ACTIVE";
        }
        if ( x == SAHPI_FUMI_BANK_BUSY ) {
            return "BUSY";
        }
        if ( x == SAHPI_FUMI_BANK_UNKNOWN ) {
            return "UNKNOWN";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiBankStateT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "VALID" ) ) {
            return SAHPI_FUMI_BANK_VALID;
        }
        if ( s.equals( "UPGRADE_IN_PROGRESS" ) ) {
            return SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS;
        }
        if ( s.equals( "CORRUPTED" ) ) {
            return SAHPI_FUMI_BANK_CORRUPTED;
        }
        if ( s.equals( "ACTIVE" ) ) {
            return SAHPI_FUMI_BANK_ACTIVE;
        }
        if ( s.equals( "BUSY" ) ) {
            return SAHPI_FUMI_BANK_BUSY;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return SAHPI_FUMI_BANK_UNKNOWN;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiFumiUpgradeStatusT
     */
    public static String fromSaHpiFumiUpgradeStatusT( long x )
    {
        if ( x == SAHPI_FUMI_OPERATION_NOTSTARTED ) {
            return "OPERATION_NOTSTARTED";
        }
        if ( x == SAHPI_FUMI_SOURCE_VALIDATION_INITIATED ) {
            return "SOURCE_VALIDATION_INITIATED";
        }
        if ( x == SAHPI_FUMI_SOURCE_VALIDATION_FAILED ) {
            return "SOURCE_VALIDATION_FAILED";
        }
        if ( x == SAHPI_FUMI_SOURCE_VALIDATION_DONE ) {
            return "SOURCE_VALIDATION_DONE";
        }
        if ( x == SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED ) {
            return "SOURCE_VALIDATION_CANCELLED";
        }
        if ( x == SAHPI_FUMI_INSTALL_INITIATED ) {
            return "INSTALL_INITIATED";
        }
        if ( x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED ) {
            return "INSTALL_FAILED_ROLLBACK_NEEDED";
        }
        if ( x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED ) {
            return "INSTALL_FAILED_ROLLBACK_INITIATED";
        }
        if ( x == SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE ) {
            return "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE";
        }
        if ( x == SAHPI_FUMI_INSTALL_DONE ) {
            return "INSTALL_DONE";
        }
        if ( x == SAHPI_FUMI_INSTALL_CANCELLED ) {
            return "INSTALL_CANCELLED";
        }
        if ( x == SAHPI_FUMI_ROLLBACK_INITIATED ) {
            return "ROLLBACK_INITIATED";
        }
        if ( x == SAHPI_FUMI_ROLLBACK_FAILED ) {
            return "ROLLBACK_FAILED";
        }
        if ( x == SAHPI_FUMI_ROLLBACK_DONE ) {
            return "ROLLBACK_DONE";
        }
        if ( x == SAHPI_FUMI_ROLLBACK_CANCELLED ) {
            return "ROLLBACK_CANCELLED";
        }
        if ( x == SAHPI_FUMI_BACKUP_INITIATED ) {
            return "BACKUP_INITIATED";
        }
        if ( x == SAHPI_FUMI_BACKUP_FAILED ) {
            return "BACKUP_FAILED";
        }
        if ( x == SAHPI_FUMI_BACKUP_DONE ) {
            return "BACKUP_DONE";
        }
        if ( x == SAHPI_FUMI_BACKUP_CANCELLED ) {
            return "BACKUP_CANCELLED";
        }
        if ( x == SAHPI_FUMI_BANK_COPY_INITIATED ) {
            return "BANK_COPY_INITIATED";
        }
        if ( x == SAHPI_FUMI_BANK_COPY_FAILED ) {
            return "BANK_COPY_FAILED";
        }
        if ( x == SAHPI_FUMI_BANK_COPY_DONE ) {
            return "BANK_COPY_DONE";
        }
        if ( x == SAHPI_FUMI_BANK_COPY_CANCELLED ) {
            return "BANK_COPY_CANCELLED";
        }
        if ( x == SAHPI_FUMI_TARGET_VERIFY_INITIATED ) {
            return "TARGET_VERIFY_INITIATED";
        }
        if ( x == SAHPI_FUMI_TARGET_VERIFY_FAILED ) {
            return "TARGET_VERIFY_FAILED";
        }
        if ( x == SAHPI_FUMI_TARGET_VERIFY_DONE ) {
            return "TARGET_VERIFY_DONE";
        }
        if ( x == SAHPI_FUMI_TARGET_VERIFY_CANCELLED ) {
            return "TARGET_VERIFY_CANCELLED";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_INITIATED ) {
            return "ACTIVATE_INITIATED";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED ) {
            return "ACTIVATE_FAILED_ROLLBACK_NEEDED";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED ) {
            return "ACTIVATE_FAILED_ROLLBACK_INITIATED";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE ) {
            return "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_DONE ) {
            return "ACTIVATE_DONE";
        }
        if ( x == SAHPI_FUMI_ACTIVATE_CANCELLED ) {
            return "ACTIVATE_CANCELLED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiFumiUpgradeStatusT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "OPERATION_NOTSTARTED" ) ) {
            return SAHPI_FUMI_OPERATION_NOTSTARTED;
        }
        if ( s.equals( "SOURCE_VALIDATION_INITIATED" ) ) {
            return SAHPI_FUMI_SOURCE_VALIDATION_INITIATED;
        }
        if ( s.equals( "SOURCE_VALIDATION_FAILED" ) ) {
            return SAHPI_FUMI_SOURCE_VALIDATION_FAILED;
        }
        if ( s.equals( "SOURCE_VALIDATION_DONE" ) ) {
            return SAHPI_FUMI_SOURCE_VALIDATION_DONE;
        }
        if ( s.equals( "SOURCE_VALIDATION_CANCELLED" ) ) {
            return SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED;
        }
        if ( s.equals( "INSTALL_INITIATED" ) ) {
            return SAHPI_FUMI_INSTALL_INITIATED;
        }
        if ( s.equals( "INSTALL_FAILED_ROLLBACK_NEEDED" ) ) {
            return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED;
        }
        if ( s.equals( "INSTALL_FAILED_ROLLBACK_INITIATED" ) ) {
            return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED;
        }
        if ( s.equals( "INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE" ) ) {
            return SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE;
        }
        if ( s.equals( "INSTALL_DONE" ) ) {
            return SAHPI_FUMI_INSTALL_DONE;
        }
        if ( s.equals( "INSTALL_CANCELLED" ) ) {
            return SAHPI_FUMI_INSTALL_CANCELLED;
        }
        if ( s.equals( "ROLLBACK_INITIATED" ) ) {
            return SAHPI_FUMI_ROLLBACK_INITIATED;
        }
        if ( s.equals( "ROLLBACK_FAILED" ) ) {
            return SAHPI_FUMI_ROLLBACK_FAILED;
        }
        if ( s.equals( "ROLLBACK_DONE" ) ) {
            return SAHPI_FUMI_ROLLBACK_DONE;
        }
        if ( s.equals( "ROLLBACK_CANCELLED" ) ) {
            return SAHPI_FUMI_ROLLBACK_CANCELLED;
        }
        if ( s.equals( "BACKUP_INITIATED" ) ) {
            return SAHPI_FUMI_BACKUP_INITIATED;
        }
        if ( s.equals( "BACKUP_FAILED" ) ) {
            return SAHPI_FUMI_BACKUP_FAILED;
        }
        if ( s.equals( "BACKUP_DONE" ) ) {
            return SAHPI_FUMI_BACKUP_DONE;
        }
        if ( s.equals( "BACKUP_CANCELLED" ) ) {
            return SAHPI_FUMI_BACKUP_CANCELLED;
        }
        if ( s.equals( "BANK_COPY_INITIATED" ) ) {
            return SAHPI_FUMI_BANK_COPY_INITIATED;
        }
        if ( s.equals( "BANK_COPY_FAILED" ) ) {
            return SAHPI_FUMI_BANK_COPY_FAILED;
        }
        if ( s.equals( "BANK_COPY_DONE" ) ) {
            return SAHPI_FUMI_BANK_COPY_DONE;
        }
        if ( s.equals( "BANK_COPY_CANCELLED" ) ) {
            return SAHPI_FUMI_BANK_COPY_CANCELLED;
        }
        if ( s.equals( "TARGET_VERIFY_INITIATED" ) ) {
            return SAHPI_FUMI_TARGET_VERIFY_INITIATED;
        }
        if ( s.equals( "TARGET_VERIFY_FAILED" ) ) {
            return SAHPI_FUMI_TARGET_VERIFY_FAILED;
        }
        if ( s.equals( "TARGET_VERIFY_DONE" ) ) {
            return SAHPI_FUMI_TARGET_VERIFY_DONE;
        }
        if ( s.equals( "TARGET_VERIFY_CANCELLED" ) ) {
            return SAHPI_FUMI_TARGET_VERIFY_CANCELLED;
        }
        if ( s.equals( "ACTIVATE_INITIATED" ) ) {
            return SAHPI_FUMI_ACTIVATE_INITIATED;
        }
        if ( s.equals( "ACTIVATE_FAILED_ROLLBACK_NEEDED" ) ) {
            return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED;
        }
        if ( s.equals( "ACTIVATE_FAILED_ROLLBACK_INITIATED" ) ) {
            return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED;
        }
        if ( s.equals( "ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE" ) ) {
            return SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE;
        }
        if ( s.equals( "ACTIVATE_DONE" ) ) {
            return SAHPI_FUMI_ACTIVATE_DONE;
        }
        if ( s.equals( "ACTIVATE_CANCELLED" ) ) {
            return SAHPI_FUMI_ACTIVATE_CANCELLED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiHsIndicatorStateT
     */
    public static String fromSaHpiHsIndicatorStateT( long x )
    {
        if ( x == SAHPI_HS_INDICATOR_OFF ) {
            return "FF";
        }
        if ( x == SAHPI_HS_INDICATOR_ON ) {
            return "N";
        }

        return Long.toString( x );
    }

    public static long toSaHpiHsIndicatorStateT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "FF" ) ) {
            return SAHPI_HS_INDICATOR_OFF;
        }
        if ( s.equals( "N" ) ) {
            return SAHPI_HS_INDICATOR_ON;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiHsActionT
     */
    public static String fromSaHpiHsActionT( long x )
    {
        if ( x == SAHPI_HS_ACTION_INSERTION ) {
            return "INSERTION";
        }
        if ( x == SAHPI_HS_ACTION_EXTRACTION ) {
            return "EXTRACTION";
        }

        return Long.toString( x );
    }

    public static long toSaHpiHsActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "INSERTION" ) ) {
            return SAHPI_HS_ACTION_INSERTION;
        }
        if ( s.equals( "EXTRACTION" ) ) {
            return SAHPI_HS_ACTION_EXTRACTION;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiHsStateT
     */
    public static String fromSaHpiHsStateT( long x )
    {
        if ( x == SAHPI_HS_STATE_INACTIVE ) {
            return "INACTIVE";
        }
        if ( x == SAHPI_HS_STATE_INSERTION_PENDING ) {
            return "INSERTION_PENDING";
        }
        if ( x == SAHPI_HS_STATE_ACTIVE ) {
            return "ACTIVE";
        }
        if ( x == SAHPI_HS_STATE_EXTRACTION_PENDING ) {
            return "EXTRACTION_PENDING";
        }
        if ( x == SAHPI_HS_STATE_NOT_PRESENT ) {
            return "NOT_PRESENT";
        }

        return Long.toString( x );
    }

    public static long toSaHpiHsStateT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "INACTIVE" ) ) {
            return SAHPI_HS_STATE_INACTIVE;
        }
        if ( s.equals( "INSERTION_PENDING" ) ) {
            return SAHPI_HS_STATE_INSERTION_PENDING;
        }
        if ( s.equals( "ACTIVE" ) ) {
            return SAHPI_HS_STATE_ACTIVE;
        }
        if ( s.equals( "EXTRACTION_PENDING" ) ) {
            return SAHPI_HS_STATE_EXTRACTION_PENDING;
        }
        if ( s.equals( "NOT_PRESENT" ) ) {
            return SAHPI_HS_STATE_NOT_PRESENT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiHsCauseOfStateChangeT
     */
    public static String fromSaHpiHsCauseOfStateChangeT( long x )
    {
        if ( x == SAHPI_HS_CAUSE_AUTO_POLICY ) {
            return "AUTO_POLICY";
        }
        if ( x == SAHPI_HS_CAUSE_EXT_SOFTWARE ) {
            return "EXT_SOFTWARE";
        }
        if ( x == SAHPI_HS_CAUSE_OPERATOR_INIT ) {
            return "OPERATOR_INIT";
        }
        if ( x == SAHPI_HS_CAUSE_USER_UPDATE ) {
            return "USER_UPDATE";
        }
        if ( x == SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION ) {
            return "UNEXPECTED_DEACTIVATION";
        }
        if ( x == SAHPI_HS_CAUSE_SURPRISE_EXTRACTION ) {
            return "SURPRISE_EXTRACTION";
        }
        if ( x == SAHPI_HS_CAUSE_EXTRACTION_UPDATE ) {
            return "EXTRACTION_UPDATE";
        }
        if ( x == SAHPI_HS_CAUSE_HARDWARE_FAULT ) {
            return "HARDWARE_FAULT";
        }
        if ( x == SAHPI_HS_CAUSE_CONTAINING_FRU ) {
            return "CONTAINING_FRU";
        }
        if ( x == SAHPI_HS_CAUSE_UNKNOWN ) {
            return "UNKNOWN";
        }

        return Long.toString( x );
    }

    public static long toSaHpiHsCauseOfStateChangeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUTO_POLICY" ) ) {
            return SAHPI_HS_CAUSE_AUTO_POLICY;
        }
        if ( s.equals( "EXT_SOFTWARE" ) ) {
            return SAHPI_HS_CAUSE_EXT_SOFTWARE;
        }
        if ( s.equals( "OPERATOR_INIT" ) ) {
            return SAHPI_HS_CAUSE_OPERATOR_INIT;
        }
        if ( s.equals( "USER_UPDATE" ) ) {
            return SAHPI_HS_CAUSE_USER_UPDATE;
        }
        if ( s.equals( "UNEXPECTED_DEACTIVATION" ) ) {
            return SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION;
        }
        if ( s.equals( "SURPRISE_EXTRACTION" ) ) {
            return SAHPI_HS_CAUSE_SURPRISE_EXTRACTION;
        }
        if ( s.equals( "EXTRACTION_UPDATE" ) ) {
            return SAHPI_HS_CAUSE_EXTRACTION_UPDATE;
        }
        if ( s.equals( "HARDWARE_FAULT" ) ) {
            return SAHPI_HS_CAUSE_HARDWARE_FAULT;
        }
        if ( s.equals( "CONTAINING_FRU" ) ) {
            return SAHPI_HS_CAUSE_CONTAINING_FRU;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return SAHPI_HS_CAUSE_UNKNOWN;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSeverityT
     */
    public static String fromSaHpiSeverityT( long x )
    {
        if ( x == SAHPI_CRITICAL ) {
            return "CRITICAL";
        }
        if ( x == SAHPI_MAJOR ) {
            return "MAJOR";
        }
        if ( x == SAHPI_MINOR ) {
            return "MINOR";
        }
        if ( x == SAHPI_INFORMATIONAL ) {
            return "INFORMATIONAL";
        }
        if ( x == SAHPI_OK ) {
            return "OK";
        }
        if ( x == SAHPI_DEBUG ) {
            return "DEBUG";
        }
        if ( x == SAHPI_ALL_SEVERITIES ) {
            return "ALL_SEVERITIES";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSeverityT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "CRITICAL" ) ) {
            return SAHPI_CRITICAL;
        }
        if ( s.equals( "MAJOR" ) ) {
            return SAHPI_MAJOR;
        }
        if ( s.equals( "MINOR" ) ) {
            return SAHPI_MINOR;
        }
        if ( s.equals( "INFORMATIONAL" ) ) {
            return SAHPI_INFORMATIONAL;
        }
        if ( s.equals( "OK" ) ) {
            return SAHPI_OK;
        }
        if ( s.equals( "DEBUG" ) ) {
            return SAHPI_DEBUG;
        }
        if ( s.equals( "ALL_SEVERITIES" ) ) {
            return SAHPI_ALL_SEVERITIES;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiResourceEventTypeT
     */
    public static String fromSaHpiResourceEventTypeT( long x )
    {
        if ( x == SAHPI_RESE_RESOURCE_FAILURE ) {
            return "FAILURE";
        }
        if ( x == SAHPI_RESE_RESOURCE_RESTORED ) {
            return "RESTORED";
        }
        if ( x == SAHPI_RESE_RESOURCE_ADDED ) {
            return "ADDED";
        }
        if ( x == SAHPI_RESE_RESOURCE_REMOVED ) {
            return "REMOVED";
        }
        if ( x == SAHPI_RESE_RESOURCE_INACCESSIBLE ) {
            return "INACCESSIBLE";
        }
        if ( x == SAHPI_RESE_RESOURCE_UPDATED ) {
            return "UPDATED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiResourceEventTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "FAILURE" ) ) {
            return SAHPI_RESE_RESOURCE_FAILURE;
        }
        if ( s.equals( "RESTORED" ) ) {
            return SAHPI_RESE_RESOURCE_RESTORED;
        }
        if ( s.equals( "ADDED" ) ) {
            return SAHPI_RESE_RESOURCE_ADDED;
        }
        if ( s.equals( "REMOVED" ) ) {
            return SAHPI_RESE_RESOURCE_REMOVED;
        }
        if ( s.equals( "INACCESSIBLE" ) ) {
            return SAHPI_RESE_RESOURCE_INACCESSIBLE;
        }
        if ( s.equals( "UPDATED" ) ) {
            return SAHPI_RESE_RESOURCE_UPDATED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiDomainEventTypeT
     */
    public static String fromSaHpiDomainEventTypeT( long x )
    {
        if ( x == SAHPI_DOMAIN_REF_ADDED ) {
            return "ADDED";
        }
        if ( x == SAHPI_DOMAIN_REF_REMOVED ) {
            return "REMOVED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiDomainEventTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "ADDED" ) ) {
            return SAHPI_DOMAIN_REF_ADDED;
        }
        if ( s.equals( "REMOVED" ) ) {
            return SAHPI_DOMAIN_REF_REMOVED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiSwEventTypeT
     */
    public static String fromSaHpiSwEventTypeT( long x )
    {
        if ( x == SAHPI_HPIE_AUDIT ) {
            return "AUDIT";
        }
        if ( x == SAHPI_HPIE_STARTUP ) {
            return "STARTUP";
        }
        if ( x == SAHPI_HPIE_OTHER ) {
            return "OTHER";
        }

        return Long.toString( x );
    }

    public static long toSaHpiSwEventTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUDIT" ) ) {
            return SAHPI_HPIE_AUDIT;
        }
        if ( s.equals( "STARTUP" ) ) {
            return SAHPI_HPIE_STARTUP;
        }
        if ( s.equals( "OTHER" ) ) {
            return SAHPI_HPIE_OTHER;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiEventTypeT
     */
    public static String fromSaHpiEventTypeT( long x )
    {
        if ( x == SAHPI_ET_RESOURCE ) {
            return "RESOURCE";
        }
        if ( x == SAHPI_ET_DOMAIN ) {
            return "DOMAIN";
        }
        if ( x == SAHPI_ET_SENSOR ) {
            return "SENSOR";
        }
        if ( x == SAHPI_ET_SENSOR_ENABLE_CHANGE ) {
            return "SENSOR_ENABLE_CHANGE";
        }
        if ( x == SAHPI_ET_HOTSWAP ) {
            return "HOTSWAP";
        }
        if ( x == SAHPI_ET_WATCHDOG ) {
            return "WATCHDOG";
        }
        if ( x == SAHPI_ET_HPI_SW ) {
            return "HPI_SW";
        }
        if ( x == SAHPI_ET_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_ET_USER ) {
            return "USER";
        }
        if ( x == SAHPI_ET_DIMI ) {
            return "DIMI";
        }
        if ( x == SAHPI_ET_DIMI_UPDATE ) {
            return "DIMI_UPDATE";
        }
        if ( x == SAHPI_ET_FUMI ) {
            return "FUMI";
        }

        return Long.toString( x );
    }

    public static long toSaHpiEventTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "RESOURCE" ) ) {
            return SAHPI_ET_RESOURCE;
        }
        if ( s.equals( "DOMAIN" ) ) {
            return SAHPI_ET_DOMAIN;
        }
        if ( s.equals( "SENSOR" ) ) {
            return SAHPI_ET_SENSOR;
        }
        if ( s.equals( "SENSOR_ENABLE_CHANGE" ) ) {
            return SAHPI_ET_SENSOR_ENABLE_CHANGE;
        }
        if ( s.equals( "HOTSWAP" ) ) {
            return SAHPI_ET_HOTSWAP;
        }
        if ( s.equals( "WATCHDOG" ) ) {
            return SAHPI_ET_WATCHDOG;
        }
        if ( s.equals( "HPI_SW" ) ) {
            return SAHPI_ET_HPI_SW;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_ET_OEM;
        }
        if ( s.equals( "USER" ) ) {
            return SAHPI_ET_USER;
        }
        if ( s.equals( "DIMI" ) ) {
            return SAHPI_ET_DIMI;
        }
        if ( s.equals( "DIMI_UPDATE" ) ) {
            return SAHPI_ET_DIMI_UPDATE;
        }
        if ( s.equals( "FUMI" ) ) {
            return SAHPI_ET_FUMI;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiStatusCondTypeT
     */
    public static String fromSaHpiStatusCondTypeT( long x )
    {
        if ( x == SAHPI_STATUS_COND_TYPE_SENSOR ) {
            return "SENSOR";
        }
        if ( x == SAHPI_STATUS_COND_TYPE_RESOURCE ) {
            return "RESOURCE";
        }
        if ( x == SAHPI_STATUS_COND_TYPE_OEM ) {
            return "OEM";
        }
        if ( x == SAHPI_STATUS_COND_TYPE_USER ) {
            return "USER";
        }

        return Long.toString( x );
    }

    public static long toSaHpiStatusCondTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "SENSOR" ) ) {
            return SAHPI_STATUS_COND_TYPE_SENSOR;
        }
        if ( s.equals( "RESOURCE" ) ) {
            return SAHPI_STATUS_COND_TYPE_RESOURCE;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_STATUS_COND_TYPE_OEM;
        }
        if ( s.equals( "USER" ) ) {
            return SAHPI_STATUS_COND_TYPE_USER;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiAnnunciatorModeT
     */
    public static String fromSaHpiAnnunciatorModeT( long x )
    {
        if ( x == SAHPI_ANNUNCIATOR_MODE_AUTO ) {
            return "AUTO";
        }
        if ( x == SAHPI_ANNUNCIATOR_MODE_USER ) {
            return "USER";
        }
        if ( x == SAHPI_ANNUNCIATOR_MODE_SHARED ) {
            return "SHARED";
        }

        return Long.toString( x );
    }

    public static long toSaHpiAnnunciatorModeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUTO" ) ) {
            return SAHPI_ANNUNCIATOR_MODE_AUTO;
        }
        if ( s.equals( "USER" ) ) {
            return SAHPI_ANNUNCIATOR_MODE_USER;
        }
        if ( s.equals( "SHARED" ) ) {
            return SAHPI_ANNUNCIATOR_MODE_SHARED;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiAnnunciatorTypeT
     */
    public static String fromSaHpiAnnunciatorTypeT( long x )
    {
        if ( x == SAHPI_ANNUNCIATOR_TYPE_LED ) {
            return "LED";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE ) {
            return "DRY_CONTACT_CLOSURE";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_AUDIBLE ) {
            return "AUDIBLE";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY ) {
            return "LCD_DISPLAY";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_MESSAGE ) {
            return "MESSAGE";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_COMPOSITE ) {
            return "COMPOSITE";
        }
        if ( x == SAHPI_ANNUNCIATOR_TYPE_OEM ) {
            return "OEM";
        }

        return Long.toString( x );
    }

    public static long toSaHpiAnnunciatorTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "LED" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_LED;
        }
        if ( s.equals( "DRY_CONTACT_CLOSURE" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE;
        }
        if ( s.equals( "AUDIBLE" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_AUDIBLE;
        }
        if ( s.equals( "LCD_DISPLAY" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY;
        }
        if ( s.equals( "MESSAGE" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_MESSAGE;
        }
        if ( s.equals( "COMPOSITE" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_COMPOSITE;
        }
        if ( s.equals( "OEM" ) ) {
            return SAHPI_ANNUNCIATOR_TYPE_OEM;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiRdrTypeT
     */
    public static String fromSaHpiRdrTypeT( long x )
    {
        if ( x == SAHPI_NO_RECORD ) {
            return "NO_RECORD";
        }
        if ( x == SAHPI_CTRL_RDR ) {
            return "CTRL_RDR";
        }
        if ( x == SAHPI_SENSOR_RDR ) {
            return "SENSOR_RDR";
        }
        if ( x == SAHPI_INVENTORY_RDR ) {
            return "INVENTORY_RDR";
        }
        if ( x == SAHPI_WATCHDOG_RDR ) {
            return "WATCHDOG_RDR";
        }
        if ( x == SAHPI_ANNUNCIATOR_RDR ) {
            return "ANNUNCIATOR_RDR";
        }
        if ( x == SAHPI_DIMI_RDR ) {
            return "DIMI_RDR";
        }
        if ( x == SAHPI_FUMI_RDR ) {
            return "FUMI_RDR";
        }

        return Long.toString( x );
    }

    public static long toSaHpiRdrTypeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "NO_RECORD" ) ) {
            return SAHPI_NO_RECORD;
        }
        if ( s.equals( "CTRL_RDR" ) ) {
            return SAHPI_CTRL_RDR;
        }
        if ( s.equals( "SENSOR_RDR" ) ) {
            return SAHPI_SENSOR_RDR;
        }
        if ( s.equals( "INVENTORY_RDR" ) ) {
            return SAHPI_INVENTORY_RDR;
        }
        if ( s.equals( "WATCHDOG_RDR" ) ) {
            return SAHPI_WATCHDOG_RDR;
        }
        if ( s.equals( "ANNUNCIATOR_RDR" ) ) {
            return SAHPI_ANNUNCIATOR_RDR;
        }
        if ( s.equals( "DIMI_RDR" ) ) {
            return SAHPI_DIMI_RDR;
        }
        if ( s.equals( "FUMI_RDR" ) ) {
            return SAHPI_FUMI_RDR;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiParmActionT
     */
    public static String fromSaHpiParmActionT( long x )
    {
        if ( x == SAHPI_DEFAULT_PARM ) {
            return "DEFAULT_PARM";
        }
        if ( x == SAHPI_SAVE_PARM ) {
            return "SAVE_PARM";
        }
        if ( x == SAHPI_RESTORE_PARM ) {
            return "RESTORE_PARM";
        }

        return Long.toString( x );
    }

    public static long toSaHpiParmActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "DEFAULT_PARM" ) ) {
            return SAHPI_DEFAULT_PARM;
        }
        if ( s.equals( "SAVE_PARM" ) ) {
            return SAHPI_SAVE_PARM;
        }
        if ( s.equals( "RESTORE_PARM" ) ) {
            return SAHPI_RESTORE_PARM;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiResetActionT
     */
    public static String fromSaHpiResetActionT( long x )
    {
        if ( x == SAHPI_COLD_RESET ) {
            return "COLD_RESET";
        }
        if ( x == SAHPI_WARM_RESET ) {
            return "WARM_RESET";
        }
        if ( x == SAHPI_RESET_ASSERT ) {
            return "RESET_ASSERT";
        }
        if ( x == SAHPI_RESET_DEASSERT ) {
            return "RESET_DEASSERT";
        }

        return Long.toString( x );
    }

    public static long toSaHpiResetActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "COLD_RESET" ) ) {
            return SAHPI_COLD_RESET;
        }
        if ( s.equals( "WARM_RESET" ) ) {
            return SAHPI_WARM_RESET;
        }
        if ( s.equals( "RESET_ASSERT" ) ) {
            return SAHPI_RESET_ASSERT;
        }
        if ( s.equals( "RESET_DEASSERT" ) ) {
            return SAHPI_RESET_DEASSERT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiPowerStateT
     */
    public static String fromSaHpiPowerStateT( long x )
    {
        if ( x == SAHPI_POWER_OFF ) {
            return "OFF";
        }
        if ( x == SAHPI_POWER_ON ) {
            return "ON";
        }
        if ( x == SAHPI_POWER_CYCLE ) {
            return "CYCLE";
        }

        return Long.toString( x );
    }

    public static long toSaHpiPowerStateT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "OFF" ) ) {
            return SAHPI_POWER_OFF;
        }
        if ( s.equals( "ON" ) ) {
            return SAHPI_POWER_ON;
        }
        if ( s.equals( "CYCLE" ) ) {
            return SAHPI_POWER_CYCLE;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiEventLogOverflowActionT
     */
    public static String fromSaHpiEventLogOverflowActionT( long x )
    {
        if ( x == SAHPI_EL_OVERFLOW_DROP ) {
            return "DROP";
        }
        if ( x == SAHPI_EL_OVERFLOW_OVERWRITE ) {
            return "OVERWRITE";
        }

        return Long.toString( x );
    }

    public static long toSaHpiEventLogOverflowActionT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "DROP" ) ) {
            return SAHPI_EL_OVERFLOW_DROP;
        }
        if ( s.equals( "OVERWRITE" ) ) {
            return SAHPI_EL_OVERFLOW_OVERWRITE;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For AtcaHpiLedColorT
     */
    public static String fromAtcaHpiLedColorT( long x )
    {
        if ( x == ATCAHPI_LED_COLOR_RESERVED ) {
            return "RESERVED";
        }
        if ( x == ATCAHPI_LED_COLOR_BLUE ) {
            return "BLUE";
        }
        if ( x == ATCAHPI_LED_COLOR_RED ) {
            return "RED";
        }
        if ( x == ATCAHPI_LED_COLOR_GREEN ) {
            return "GREEN";
        }
        if ( x == ATCAHPI_LED_COLOR_AMBER ) {
            return "AMBER";
        }
        if ( x == ATCAHPI_LED_COLOR_ORANGE ) {
            return "ORANGE";
        }
        if ( x == ATCAHPI_LED_COLOR_WHITE ) {
            return "WHITE";
        }
        if ( x == ATCAHPI_LED_COLOR_NO_CHANGE ) {
            return "NO_CHANGE";
        }
        if ( x == ATCAHPI_LED_COLOR_USE_DEFAULT ) {
            return "USE_DEFAULT";
        }

        return Long.toString( x );
    }

    public static long toAtcaHpiLedColorT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "RESERVED" ) ) {
            return ATCAHPI_LED_COLOR_RESERVED;
        }
        if ( s.equals( "BLUE" ) ) {
            return ATCAHPI_LED_COLOR_BLUE;
        }
        if ( s.equals( "RED" ) ) {
            return ATCAHPI_LED_COLOR_RED;
        }
        if ( s.equals( "GREEN" ) ) {
            return ATCAHPI_LED_COLOR_GREEN;
        }
        if ( s.equals( "AMBER" ) ) {
            return ATCAHPI_LED_COLOR_AMBER;
        }
        if ( s.equals( "ORANGE" ) ) {
            return ATCAHPI_LED_COLOR_ORANGE;
        }
        if ( s.equals( "WHITE" ) ) {
            return ATCAHPI_LED_COLOR_WHITE;
        }
        if ( s.equals( "NO_CHANGE" ) ) {
            return ATCAHPI_LED_COLOR_NO_CHANGE;
        }
        if ( s.equals( "USE_DEFAULT" ) ) {
            return ATCAHPI_LED_COLOR_USE_DEFAULT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For AtcaHpiResourceLedModeT
     */
    public static String fromAtcaHpiResourceLedModeT( long x )
    {
        if ( x == ATCAHPI_LED_AUTO ) {
            return "AUTO";
        }
        if ( x == ATCAHPI_LED_MANUAL ) {
            return "MANUAL";
        }
        if ( x == ATCAHPI_LED_LAMP_TEST ) {
            return "LAMP_TEST";
        }

        return Long.toString( x );
    }

    public static long toAtcaHpiResourceLedModeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUTO" ) ) {
            return ATCAHPI_LED_AUTO;
        }
        if ( s.equals( "MANUAL" ) ) {
            return ATCAHPI_LED_MANUAL;
        }
        if ( s.equals( "LAMP_TEST" ) ) {
            return ATCAHPI_LED_LAMP_TEST;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For AtcaHpiLedBrSupportT
     */
    public static String fromAtcaHpiLedBrSupportT( long x )
    {
        if ( x == ATCAHPI_LED_BR_SUPPORTED ) {
            return "SUPPORTED";
        }
        if ( x == ATCAHPI_LED_BR_NOT_SUPPORTED ) {
            return "NOT_SUPPORTED";
        }
        if ( x == ATCAHPI_LED_BR_UNKNOWN ) {
            return "UNKNOWN";
        }

        return Long.toString( x );
    }

    public static long toAtcaHpiLedBrSupportT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "SUPPORTED" ) ) {
            return ATCAHPI_LED_BR_SUPPORTED;
        }
        if ( s.equals( "NOT_SUPPORTED" ) ) {
            return ATCAHPI_LED_BR_NOT_SUPPORTED;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return ATCAHPI_LED_BR_UNKNOWN;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For XtcaHpiLedColorT
     */
    public static String fromXtcaHpiLedColorT( long x )
    {
        if ( x == XTCAHPI_LED_COLOR_RESERVED ) {
            return "RESERVED";
        }
        if ( x == XTCAHPI_LED_COLOR_BLUE ) {
            return "BLUE";
        }
        if ( x == XTCAHPI_LED_COLOR_RED ) {
            return "RED";
        }
        if ( x == XTCAHPI_LED_COLOR_GREEN ) {
            return "GREEN";
        }
        if ( x == XTCAHPI_LED_COLOR_AMBER ) {
            return "AMBER";
        }
        if ( x == XTCAHPI_LED_COLOR_ORANGE ) {
            return "ORANGE";
        }
        if ( x == XTCAHPI_LED_COLOR_WHITE ) {
            return "WHITE";
        }
        if ( x == XTCAHPI_LED_COLOR_NO_CHANGE ) {
            return "NO_CHANGE";
        }
        if ( x == XTCAHPI_LED_COLOR_USE_DEFAULT ) {
            return "USE_DEFAULT";
        }

        return Long.toString( x );
    }

    public static long toXtcaHpiLedColorT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "RESERVED" ) ) {
            return XTCAHPI_LED_COLOR_RESERVED;
        }
        if ( s.equals( "BLUE" ) ) {
            return XTCAHPI_LED_COLOR_BLUE;
        }
        if ( s.equals( "RED" ) ) {
            return XTCAHPI_LED_COLOR_RED;
        }
        if ( s.equals( "GREEN" ) ) {
            return XTCAHPI_LED_COLOR_GREEN;
        }
        if ( s.equals( "AMBER" ) ) {
            return XTCAHPI_LED_COLOR_AMBER;
        }
        if ( s.equals( "ORANGE" ) ) {
            return XTCAHPI_LED_COLOR_ORANGE;
        }
        if ( s.equals( "WHITE" ) ) {
            return XTCAHPI_LED_COLOR_WHITE;
        }
        if ( s.equals( "NO_CHANGE" ) ) {
            return XTCAHPI_LED_COLOR_NO_CHANGE;
        }
        if ( s.equals( "USE_DEFAULT" ) ) {
            return XTCAHPI_LED_COLOR_USE_DEFAULT;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For XtcaHpiResourceLedModeT
     */
    public static String fromXtcaHpiResourceLedModeT( long x )
    {
        if ( x == XTCAHPI_LED_AUTO ) {
            return "AUTO";
        }
        if ( x == XTCAHPI_LED_MANUAL ) {
            return "MANUAL";
        }
        if ( x == XTCAHPI_LED_LAMP_TEST ) {
            return "LAMP_TEST";
        }

        return Long.toString( x );
    }

    public static long toXtcaHpiResourceLedModeT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "AUTO" ) ) {
            return XTCAHPI_LED_AUTO;
        }
        if ( s.equals( "MANUAL" ) ) {
            return XTCAHPI_LED_MANUAL;
        }
        if ( s.equals( "LAMP_TEST" ) ) {
            return XTCAHPI_LED_LAMP_TEST;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For XtcaHpiLedBrSupportT
     */
    public static String fromXtcaHpiLedBrSupportT( long x )
    {
        if ( x == XTCAHPI_LED_BR_SUPPORTED ) {
            return "SUPPORTED";
        }
        if ( x == XTCAHPI_LED_BR_NOT_SUPPORTED ) {
            return "NOT_SUPPORTED";
        }
        if ( x == XTCAHPI_LED_BR_UNKNOWN ) {
            return "UNKNOWN";
        }

        return Long.toString( x );
    }

    public static long toXtcaHpiLedBrSupportT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "SUPPORTED" ) ) {
            return XTCAHPI_LED_BR_SUPPORTED;
        }
        if ( s.equals( "NOT_SUPPORTED" ) ) {
            return XTCAHPI_LED_BR_NOT_SUPPORTED;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return XTCAHPI_LED_BR_UNKNOWN;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaErrorT
     */
    public static String fromSaErrorT( long x )
    {
        if ( x == SA_ERR_HPI_OK ) {
            return "OK";
        }
        if ( x == SA_ERR_HPI_ERROR ) {
            return "ERROR";
        }
        if ( x == SA_ERR_HPI_UNSUPPORTED_API ) {
            return "UNSUPPORTED_API";
        }
        if ( x == SA_ERR_HPI_BUSY ) {
            return "BUSY";
        }
        if ( x == SA_ERR_HPI_INTERNAL_ERROR ) {
            return "INTERNAL_ERROR";
        }
        if ( x == SA_ERR_HPI_INVALID_CMD ) {
            return "INVALID_CMD";
        }
        if ( x == SA_ERR_HPI_TIMEOUT ) {
            return "TIMEOUT";
        }
        if ( x == SA_ERR_HPI_OUT_OF_SPACE ) {
            return "OUT_OF_SPACE";
        }
        if ( x == SA_ERR_HPI_OUT_OF_MEMORY ) {
            return "OUT_OF_MEMORY";
        }
        if ( x == SA_ERR_HPI_INVALID_PARAMS ) {
            return "INVALID_PARAMS";
        }
        if ( x == SA_ERR_HPI_INVALID_DATA ) {
            return "INVALID_DATA";
        }
        if ( x == SA_ERR_HPI_NOT_PRESENT ) {
            return "NOT_PRESENT";
        }
        if ( x == SA_ERR_HPI_NO_RESPONSE ) {
            return "NO_RESPONSE";
        }
        if ( x == SA_ERR_HPI_DUPLICATE ) {
            return "DUPLICATE";
        }
        if ( x == SA_ERR_HPI_INVALID_SESSION ) {
            return "INVALID_SESSION";
        }
        if ( x == SA_ERR_HPI_INVALID_DOMAIN ) {
            return "INVALID_DOMAIN";
        }
        if ( x == SA_ERR_HPI_INVALID_RESOURCE ) {
            return "INVALID_RESOURCE";
        }
        if ( x == SA_ERR_HPI_INVALID_REQUEST ) {
            return "INVALID_REQUEST";
        }
        if ( x == SA_ERR_HPI_ENTITY_NOT_PRESENT ) {
            return "ENTITY_NOT_PRESENT";
        }
        if ( x == SA_ERR_HPI_READ_ONLY ) {
            return "READ_ONLY";
        }
        if ( x == SA_ERR_HPI_CAPABILITY ) {
            return "CAPABILITY";
        }
        if ( x == SA_ERR_HPI_UNKNOWN ) {
            return "UNKNOWN";
        }
        if ( x == SA_ERR_HPI_INVALID_STATE ) {
            return "INVALID_STATE";
        }
        if ( x == SA_ERR_HPI_UNSUPPORTED_PARAMS ) {
            return "UNSUPPORTED_PARAMS";
        }

        return Long.toString( x );
    }

    public static long toSaErrorT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "OK" ) ) {
            return SA_ERR_HPI_OK;
        }
        if ( s.equals( "ERROR" ) ) {
            return SA_ERR_HPI_ERROR;
        }
        if ( s.equals( "UNSUPPORTED_API" ) ) {
            return SA_ERR_HPI_UNSUPPORTED_API;
        }
        if ( s.equals( "BUSY" ) ) {
            return SA_ERR_HPI_BUSY;
        }
        if ( s.equals( "INTERNAL_ERROR" ) ) {
            return SA_ERR_HPI_INTERNAL_ERROR;
        }
        if ( s.equals( "INVALID_CMD" ) ) {
            return SA_ERR_HPI_INVALID_CMD;
        }
        if ( s.equals( "TIMEOUT" ) ) {
            return SA_ERR_HPI_TIMEOUT;
        }
        if ( s.equals( "OUT_OF_SPACE" ) ) {
            return SA_ERR_HPI_OUT_OF_SPACE;
        }
        if ( s.equals( "OUT_OF_MEMORY" ) ) {
            return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        if ( s.equals( "INVALID_PARAMS" ) ) {
            return SA_ERR_HPI_INVALID_PARAMS;
        }
        if ( s.equals( "INVALID_DATA" ) ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
        if ( s.equals( "NOT_PRESENT" ) ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        if ( s.equals( "NO_RESPONSE" ) ) {
            return SA_ERR_HPI_NO_RESPONSE;
        }
        if ( s.equals( "DUPLICATE" ) ) {
            return SA_ERR_HPI_DUPLICATE;
        }
        if ( s.equals( "INVALID_SESSION" ) ) {
            return SA_ERR_HPI_INVALID_SESSION;
        }
        if ( s.equals( "INVALID_DOMAIN" ) ) {
            return SA_ERR_HPI_INVALID_DOMAIN;
        }
        if ( s.equals( "INVALID_RESOURCE" ) ) {
            return SA_ERR_HPI_INVALID_RESOURCE;
        }
        if ( s.equals( "INVALID_REQUEST" ) ) {
            return SA_ERR_HPI_INVALID_REQUEST;
        }
        if ( s.equals( "ENTITY_NOT_PRESENT" ) ) {
            return SA_ERR_HPI_ENTITY_NOT_PRESENT;
        }
        if ( s.equals( "READ_ONLY" ) ) {
            return SA_ERR_HPI_READ_ONLY;
        }
        if ( s.equals( "CAPABILITY" ) ) {
            return SA_ERR_HPI_CAPABILITY;
        }
        if ( s.equals( "UNKNOWN" ) ) {
            return SA_ERR_HPI_UNKNOWN;
        }
        if ( s.equals( "INVALID_STATE" ) ) {
            return SA_ERR_HPI_INVALID_STATE;
        }
        if ( s.equals( "UNSUPPORTED_PARAMS" ) ) {
            return SA_ERR_HPI_UNSUPPORTED_PARAMS;
        }

        throw new IllegalArgumentException();
    }

    /**
     * For SaHpiEventCategoryT
     */
    public static String fromSaHpiEventCategoryT( long x )
    {
        if ( x == SAHPI_EC_UNSPECIFIED ) {
            return "UNSPECIFIED";
        }
        if ( x == SAHPI_EC_THRESHOLD ) {
            return "THRESHOLD";
        }
        if ( x == SAHPI_EC_USAGE ) {
            return "USAGE";
        }
        if ( x == SAHPI_EC_STATE ) {
            return "STATE";
        }
        if ( x == SAHPI_EC_PRED_FAIL ) {
            return "PRED_FAIL";
        }
        if ( x == SAHPI_EC_LIMIT ) {
            return "LIMIT";
        }
        if ( x == SAHPI_EC_PERFORMANCE ) {
            return "PERFORMANCE";
        }
        if ( x == SAHPI_EC_SEVERITY ) {
            return "SEVERITY";
        }
        if ( x == SAHPI_EC_PRESENCE ) {
            return "PRESENCE";
        }
        if ( x == SAHPI_EC_ENABLE ) {
            return "ENABLE";
        }
        if ( x == SAHPI_EC_AVAILABILITY ) {
            return "AVAILABILITY";
        }
        if ( x == SAHPI_EC_REDUNDANCY ) {
            return "REDUNDANCY";
        }
        if ( x == SAHPI_EC_SENSOR_SPECIFIC ) {
            return "SENSOR_SPECIFIC";
        }
        if ( x == SAHPI_EC_GENERIC ) {
            return "GENERIC";
        }

        return Long.toString( x );
    }

    public static long toSaHpiEventCategoryT( String s )
        throws IllegalArgumentException
    {
        if ( s.equals( "UNSPECIFIED" ) ) {
            return SAHPI_EC_UNSPECIFIED;
        }
        if ( s.equals( "THRESHOLD" ) ) {
            return SAHPI_EC_THRESHOLD;
        }
        if ( s.equals( "USAGE" ) ) {
            return SAHPI_EC_USAGE;
        }
        if ( s.equals( "STATE" ) ) {
            return SAHPI_EC_STATE;
        }
        if ( s.equals( "PRED_FAIL" ) ) {
            return SAHPI_EC_PRED_FAIL;
        }
        if ( s.equals( "LIMIT" ) ) {
            return SAHPI_EC_LIMIT;
        }
        if ( s.equals( "PERFORMANCE" ) ) {
            return SAHPI_EC_PERFORMANCE;
        }
        if ( s.equals( "SEVERITY" ) ) {
            return SAHPI_EC_SEVERITY;
        }
        if ( s.equals( "PRESENCE" ) ) {
            return SAHPI_EC_PRESENCE;
        }
        if ( s.equals( "ENABLE" ) ) {
            return SAHPI_EC_ENABLE;
        }
        if ( s.equals( "AVAILABILITY" ) ) {
            return SAHPI_EC_AVAILABILITY;
        }
        if ( s.equals( "REDUNDANCY" ) ) {
            return SAHPI_EC_REDUNDANCY;
        }
        if ( s.equals( "SENSOR_SPECIFIC" ) ) {
            return SAHPI_EC_SENSOR_SPECIFIC;
        }
        if ( s.equals( "GENERIC" ) ) {
            return SAHPI_EC_GENERIC;
        }

        throw new IllegalArgumentException();
    }

};

