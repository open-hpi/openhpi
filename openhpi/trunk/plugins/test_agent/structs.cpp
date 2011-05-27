/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTSTRUCTSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include "structs.h"
#include "vars.h"


namespace TA {


namespace Structs {


/**************************************************************
 * Helpers to get vars from common HPI structures
 *************************************************************/
void GetVars( SaHpiRptEntryT& rpte, cVars& vars )
{
    SaHpiResourceInfoT& ri = rpte.ResourceInfo;

    vars << "RptEntry.ResourceId"
         << dtSaHpiResourceIdT
         << DATA( rpte.ResourceId )
         << READONLY()
         << VAR_END();
    vars << "RptEntry.ResourceInfo.ResourceRev"
         << dtSaHpiUint8T
         << DATA( ri.ResourceRev )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.SpecificVer"
         << dtSaHpiUint8T
         << DATA( ri.SpecificVer )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.DeviceSupport"
         << dtSaHpiUint8T
         << DATA( ri.DeviceSupport )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.ManufacturerId"
         << dtSaHpiManufacturerIdT
         << DATA( ri.ManufacturerId )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.ProductId"
         << dtSaHpiUint16T
         << DATA( ri.ProductId )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.FirmwareMajorRev"
         << dtSaHpiUint8T
         << DATA( ri.FirmwareMajorRev )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.FirmwareMinorRev"
         << dtSaHpiUint8T
         << DATA( ri.FirmwareMinorRev )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.AuxFirmwareRev"
         << dtSaHpiUint8T
         << DATA( ri.AuxFirmwareRev )
         << VAR_END();
    vars << "RptEntry.ResourceInfo.Guid"
         << dtSaHpiGuidT
         << DATA( ri.Guid )
         << VAR_END();
    vars << "RptEntry.ResourceEntity"
         << dtSaHpiEntityPathT
         << DATA( rpte.ResourceEntity )
         << READONLY()
         << VAR_END();
    vars << "RptEntry.ResourceCapabilities"
         << dtSaHpiCapabilitiesT
         << DATA( rpte.ResourceCapabilities )
         << VAR_END();
    vars << "RptEntry.HotSwapCapabilities"
         << dtSaHpiHsCapabilitiesT
         << DATA( rpte.HotSwapCapabilities )
         << VAR_END();
    vars << "RptEntry.ResourceSeverity"
         << dtSaHpiSeverityT
         << DATA( rpte.ResourceSeverity )
         << VAR_END();
    vars << "RptEntry.ResourceFailed"
         << dtSaHpiBoolT
         << DATA( rpte.ResourceFailed )
         << READONLY()
         << VAR_END();
    vars << "RptEntry.ResourceTag"
         << dtSaHpiTextBufferT
         << DATA( rpte.ResourceTag )
         << VAR_END();
}

void GetVars( SaHpiLoadIdT& load_id, cVars& vars )
{
    vars << "LoadId.LoadNumber"
         << dtSaHpiLoadNumberT
         << DATA( load_id.LoadNumber )
         << VAR_END();
    vars << IF( load_id.LoadNumber == SAHPI_LOAD_ID_BYNAME )
         << "LoadId.LoadName"
         << dtSaHpiTextBufferT
         << DATA( load_id.LoadName )
         << VAR_END();
}

static void GetVars( SaHpiCtrlRecT& rec, cVars& vars )
{
    vars << "Rdr.CtrlRec.Num"
         << dtSaHpiCtrlNumT
         << DATA( rec.Num )
         << READONLY()
         << VAR_END();
    vars << "Rdr.CtrlRec.OutputType"
         << dtSaHpiCtrlOutputTypeT
         << DATA( rec.OutputType )
         << VAR_END();
    vars << "Rdr.CtrlRec.Type"
         << dtSaHpiCtrlTypeT
         << DATA( rec.Type )
         << VAR_END();

    vars << IF( rec.Type == SAHPI_CTRL_TYPE_DIGITAL )
         << "Rdr.CtrlRec.Digital.Default"
         << dtSaHpiCtrlStateDigitalT
         << DATA( rec.TypeUnion.Digital.Default )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_DISCRETE )
         << "Rdr.CtrlRec.Discrete.Default"
         << dtSaHpiCtrlStateDiscreteT
         << DATA( rec.TypeUnion.Discrete.Default )
         << VAR_END();

    vars << IF( rec.Type == SAHPI_CTRL_TYPE_ANALOG )
         << "Rdr.CtrlRec.Analog.Min"
         << dtSaHpiCtrlStateAnalogT
         << DATA( rec.TypeUnion.Analog.Min )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_ANALOG )
         << "Rdr.CtrlRec.Analog.Max"
         << dtSaHpiCtrlStateAnalogT
         << DATA( rec.TypeUnion.Analog.Max )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_ANALOG )
         << "Rdr.CtrlRec.Analog.Default"
         << dtSaHpiCtrlStateAnalogT
         << DATA( rec.TypeUnion.Analog.Default )
         << VAR_END();

    vars << IF( rec.Type == SAHPI_CTRL_TYPE_STREAM )
         << "Rdr.CtrlRec.Stream.Default.Repeat"
         << dtSaHpiBoolT
         << DATA( rec.TypeUnion.Stream.Default.Repeat )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_STREAM )
         << "Rdr.CtrlRec.Stream.Default.Stream"
         << dtSaHpiCtrlStateStreamTWithoutRepeat
         << DATA( rec.TypeUnion.Stream.Default )
         << VAR_END();

    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.MaxChars"
         << dtSaHpiUint8T
         << DATA( rec.TypeUnion.Text.MaxChars )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.MaxLines"
         << dtSaHpiUint8T
         << DATA( rec.TypeUnion.Text.MaxLines )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.Language"
         << dtSaHpiLanguageT
         << DATA( rec.TypeUnion.Text.Language )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.DataType"
         << dtSaHpiTextTypeT
         << DATA( rec.TypeUnion.Text.DataType )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.Default.Line"
         << dtSaHpiTxtLineNumT
         << DATA( rec.TypeUnion.Text.Default.Line )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_TEXT )
         << "Rdr.CtrlRec.Text.Default.Text"
         << dtSaHpiTextBufferT
         << DATA( rec.TypeUnion.Text.Default.Text )
         << VAR_END();

    vars << IF( rec.Type == SAHPI_CTRL_TYPE_OEM )
         << "Rdr.CtrlRec.Oem.MId"
         << dtSaHpiManufacturerIdT
         << DATA( rec.TypeUnion.Oem.MId )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_OEM )
         << "Rdr.CtrlRec.Oem.ConfigData"
         << dtControlOemConfigData
         << DATA( rec.TypeUnion.Oem.ConfigData )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_OEM )
         << "Rdr.CtrlRec.Oem.Default.MId"
         << dtSaHpiManufacturerIdT
         << DATA( rec.TypeUnion.Oem.Default.MId )
         << VAR_END();
    vars << IF( rec.Type == SAHPI_CTRL_TYPE_OEM )
         << "Rdr.CtrlRec.Oem.Default.Body"
         << dtSaHpiCtrlStateOemTWithoutMId
         << DATA( rec.TypeUnion.Oem.Default )
         << VAR_END();

    vars << "Rdr.CtrlRec.DefaultModeMode"
         << dtSaHpiCtrlModeT
         << DATA( rec.DefaultMode.Mode )
         << VAR_END();
    vars << "Rdr.CtrlRec.DefaultMode.ReadOnly"
         << dtSaHpiBoolT
         << DATA( rec.DefaultMode.ReadOnly )
         << VAR_END();
    vars << "Rdr.CtrlRec.WriteOnly"
         << dtSaHpiBoolT
         << DATA( rec.WriteOnly )
         << VAR_END();
    vars << "Rdr.CtrlRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

static void GetVars( SaHpiSensorRecT& rec, cVars& vars )
{
    SaHpiSensorDataFormatT& df = rec.DataFormat;
    SaHpiSensorRangeT& r       = df.Range;
    SaHpiSensorThdDefnT& thd   = rec.ThresholdDefn;

    bool has_df  = ( df.IsSupported != SAHPI_FALSE );
    bool is_th   = ( rec.Category == SAHPI_EC_THRESHOLD );
    bool has_thd = is_th && ( thd.IsAccessible != SAHPI_FALSE );

    vars << "Rdr.SensorRec.Num"
         << dtSaHpiSensorNumT
         << DATA( rec.Num )
         << READONLY()
         << VAR_END();
    vars << "Rdr.SensorRec.Type"
         << dtSaHpiSensorTypeT
         << DATA( rec.Type )
         << VAR_END();
    vars << "Rdr.SensorRec.Category"
         << dtSaHpiEventCategoryT
         << DATA( rec.Category )
         << VAR_END();
    vars << "Rdr.SensorRec.EnableCtrl"
         << dtSaHpiBoolT
         << DATA( rec.EnableCtrl )
         << VAR_END();
    vars << "Rdr.SensorRec.EventCtrl"
         << dtSaHpiSensorEventCtrlT
         << DATA( rec.EventCtrl )
         << VAR_END();
    vars << "Rdr.SensorRec.Events"
         << dtSaHpiEventStateT
         << DATA( rec.Events )
         << VAR_END();

    vars << "Rdr.SensorRec.DataFormat.IsSupported"
         << dtSaHpiBoolT
         << DATA( df.IsSupported )
         << VAR_END();
    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.ReadingType"
         << dtSaHpiSensorReadingTypeT
         << DATA( df.ReadingType )
         << VAR_END();
    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.BaseUnits"
         << dtSaHpiSensorUnitsT
         << DATA( df.BaseUnits )
         << VAR_END();
    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.ModifierUnits"
         << dtSaHpiSensorUnitsT
         << DATA( df.ModifierUnits )
         << VAR_END();
    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.ModifierUse"
         << dtSaHpiSensorModUnitUseT
         << DATA( df.ModifierUse )
         << VAR_END();
    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.Percentage"
         << dtSaHpiBoolT
         << DATA( df.Percentage )
         << VAR_END();

    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.Range.Flags"
         << dtSaHpiSensorRangeFlagsT
         << DATA( r.Flags )
         << VAR_END();
    if ( has_df && ( r.Flags & SAHPI_SRF_MAX ) ) {
        GetVars( "Rdr.SensorRec.DataFormat.Range.Max", r.Max, vars );
    }
    if ( has_df && ( r.Flags & SAHPI_SRF_MIN ) ) {
        GetVars( "Rdr.SensorRec.DataFormat.Range.Min", r.Min, vars );
    }
    if ( has_df && ( r.Flags & SAHPI_SRF_NOMINAL ) ) {
        GetVars( "Rdr.SensorRec.DataFormat.Range.Nominal", r.Nominal, vars );
    }
    if ( has_df && ( r.Flags & SAHPI_SRF_NORMAL_MAX ) ) {
        GetVars( "Rdr.SensorRec.DataFormat.Range.NormalMax", r.NormalMax, vars );
    }
    if ( has_df && ( r.Flags & SAHPI_SRF_NORMAL_MIN ) ) {
        GetVars( "Rdr.SensorRec.DataFormat.Range.NormalMin", r.NormalMin, vars );
    }

    vars << IF( has_df )
         << "Rdr.SensorRec.DataFormat.AccuracyFactor"
         << dtSaHpiFloat64T
         << DATA( df.AccuracyFactor )
         << VAR_END();

    vars << IF( is_th )
         << "Rdr.SensorRec.ThresholdDefn.IsAccessible"
         << dtSaHpiBoolT
         << DATA( thd.IsAccessible )
         << VAR_END();
    vars << IF( has_thd )
         << "Rdr.SensorRec.ThresholdDefn.ReadThold"
         << dtSaHpiSensorThdMaskT
         << DATA( thd.ReadThold )
         << VAR_END();
    vars << IF( has_thd )
         << "Rdr.SensorRec.ThresholdDefn.WriteThold"
         << dtSaHpiSensorThdMaskT
         << DATA( thd.WriteThold )
         << VAR_END();
    vars << IF( has_thd )
         << "Rdr.SensorRec.ThresholdDefn.Nonlinear"
         << dtSaHpiBoolT
         << DATA( thd.Nonlinear )
         << VAR_END();

    vars << "Rdr.SensorRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

void GetVars( SaHpiInventoryRecT& rec, cVars& vars )
{
    vars << "Rdr.InventoryRec.IdrId"
         << dtSaHpiIdrIdT
         << DATA( rec.IdrId )
         << READONLY()
         << VAR_END();
    vars << "Rdr.InventoryRec.Persistent"
         << dtSaHpiBoolT
         << DATA( rec.Persistent )
         << VAR_END();
    vars << "Rdr.InventoryRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

void GetVars( SaHpiRdrT& rdr, cVars& vars )
{
    vars << "Rdr.RdrType"
         << dtSaHpiRdrTypeT
         << DATA( rdr.RdrType )
         << READONLY()
         << VAR_END();
    vars << "Rdr.Entity"
         << dtSaHpiEntityPathT
         << DATA( rdr.Entity )
         << VAR_END();
    vars << "Rdr.IsFru"
         << dtSaHpiBoolT
         << DATA( rdr.IsFru )
         << VAR_END();

    if ( rdr.RdrType == SAHPI_CTRL_RDR ) {
        GetVars( rdr.RdrTypeUnion.CtrlRec, vars );
    } else if ( rdr.RdrType == SAHPI_SENSOR_RDR ) {
        GetVars( rdr.RdrTypeUnion.SensorRec, vars );
    } else if ( rdr.RdrType == SAHPI_INVENTORY_RDR ) {
        GetVars( rdr.RdrTypeUnion.InventoryRec, vars );
    }

    vars << "Rdr.IdString"
         << dtSaHpiTextBufferT
         << DATA( rdr.IdString )
         << VAR_END();
}

void GetVars( SaHpiCtrlStateT& state, cVars& vars )
{
    vars << "State.Type"
         << dtSaHpiCtrlTypeT
         << DATA( state.Type )
         << VAR_END();

    vars << IF( state.Type == SAHPI_CTRL_TYPE_DIGITAL )
         << "State.Digital"
         << dtSaHpiCtrlStateDigitalT
         << DATA( state.StateUnion.Digital )
         << VAR_END();

    vars << IF( state.Type == SAHPI_CTRL_TYPE_DISCRETE )
         << "State.Discrete"
         << dtSaHpiCtrlStateDiscreteT
         << DATA( state.StateUnion.Discrete )
         << VAR_END();

    vars << IF( state.Type == SAHPI_CTRL_TYPE_ANALOG )
         << "State.Analog"
         << dtSaHpiCtrlStateAnalogT
         << DATA( state.StateUnion.Analog )
         << VAR_END();

    vars << IF ( state.Type == SAHPI_CTRL_TYPE_STREAM )
         << "State.Stream.Repeat"
         << dtSaHpiBoolT
         << DATA( state.StateUnion.Stream.Repeat )
         << VAR_END();
    vars << IF ( state.Type == SAHPI_CTRL_TYPE_STREAM )
         << "State.Stream.Stream"
         << dtSaHpiCtrlStateStreamTWithoutRepeat
         << DATA( state.StateUnion.Stream )
         << VAR_END();

    vars << IF( state.Type == SAHPI_CTRL_TYPE_TEXT )
         << "State.Text.Line"
         << dtSaHpiTxtLineNumT
         << DATA( state.StateUnion.Text.Line )
         << VAR_END();
    vars << IF( state.Type == SAHPI_CTRL_TYPE_TEXT )
         << "State.Text.Text"
         << dtSaHpiTextBufferT
         << DATA( state.StateUnion.Text.Text )
         << VAR_END();

    vars << IF( state.Type == SAHPI_CTRL_TYPE_OEM )
         << "State.Oem.MId"
         << dtSaHpiManufacturerIdT
         << DATA( state.StateUnion.Oem.MId )
         << VAR_END();
    vars << IF( state.Type == SAHPI_CTRL_TYPE_OEM )
         << "State.Oem.Body"
         << dtSaHpiCtrlStateOemTWithoutMId
         << DATA( state.StateUnion.Oem )
         << VAR_END();
}

void GetVars( const std::string& name, SaHpiSensorReadingT& r, cVars& vars )
{
    vars << name + ".IsSupported"
         << dtSaHpiBoolT
         << DATA( r.IsSupported )
         << VAR_END();

    if ( r.IsSupported == SAHPI_FALSE ) {
        return;
    }

/*
    vars << name + ".Type"
         << dtSaHpiSensorReadingTypeT
         << DATA( r.Type )
         << VAR_END();
*/

    vars << IF( r.Type == SAHPI_SENSOR_READING_TYPE_INT64 )
         << name + ".Value"
         << dtSaHpiInt64T
         << DATA( r.Value.SensorInt64 )
         << VAR_END();

    vars << IF( r.Type == SAHPI_SENSOR_READING_TYPE_UINT64 )
         << name + ".Value"
         << dtSaHpiUint64T
         << DATA( r.Value.SensorUint64 )
         << VAR_END();

    vars << IF( r.Type == SAHPI_SENSOR_READING_TYPE_FLOAT64 )
         << name + ".Value"
         << dtSaHpiFloat64T
         << DATA( r.Value.SensorFloat64 )
         << VAR_END();

    vars << IF( r.Type == SAHPI_SENSOR_READING_TYPE_BUFFER )
         << name + ".Value"
         << dtSensorReadingBuffer
         << DATA( r.Value.SensorBuffer )
         << VAR_END();
}

void GetVars( SaHpiSensorThresholdsT& ths, cVars& vars )
{
    GetVars( "Thresholds.LowCritical", ths.LowCritical, vars );
    GetVars( "Thresholds.LowMajor", ths.LowMajor, vars );
    GetVars( "Thresholds.LowMinor", ths.LowMinor, vars );
    GetVars( "Thresholds.UpMinor", ths.UpMinor, vars );
    GetVars( "Thresholds.UpMajor", ths.UpMajor, vars );
    GetVars( "Thresholds.UpCritical", ths.UpCritical, vars );
    GetVars( "Thresholds.PosThdHysteresis", ths.PosThdHysteresis, vars );
    GetVars( "Thresholds.NegThdHysteresis", ths.NegThdHysteresis, vars );
}


}; // namespace Structs


}; // namespace TA

