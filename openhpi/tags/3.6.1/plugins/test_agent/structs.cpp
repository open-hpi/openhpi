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

#include <stddef.h>
#include <stdio.h>

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

void GetVars( SaHpiEventLogInfoT& info, cVars& vars )
{
    vars << "Info.Entries"
         << dtSaHpiUint32T
         << DATA( info.Entries )
         << READONLY()
         << VAR_END();
    vars << "Info.Size"
         << dtSaHpiUint32T
         << DATA( info.Size )
         << VAR_END();
    vars << "Info.UserEventMaxSize"
         << dtSaHpiUint32T
         << DATA( info.UserEventMaxSize )
         << VAR_END();
    vars << "Info.UpdateTimestamp"
         << dtSaHpiTimeT
         << DATA( info.UpdateTimestamp )
         << READONLY()
         << VAR_END();
    vars << "Info.CurrentTime"
         << dtSaHpiTimeT
         << DATA( info.CurrentTime )
         << VAR_END();
    vars << "Info.Enabled"
         << dtSaHpiBoolT
         << DATA( info.Enabled )
         << VAR_END();
    vars << "Info.OverflowFlag"
         << dtSaHpiBoolT
         << DATA( info.OverflowFlag )
         << VAR_END();
    vars << "Info.OverflowResetable"
         << dtSaHpiBoolT
         << DATA( info.OverflowResetable )
         << VAR_END();
    vars << "Info.OverflowAction"
         << dtSaHpiEventLogOverflowActionT
         << DATA( info.OverflowAction )
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

static void GetVars( SaHpiInventoryRecT& rec, cVars& vars )
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

static void GetVars( SaHpiWatchdogRecT& rec, cVars& vars )
{
    vars << "Rdr.WatchdogRec.WatchdogNum"
         << dtSaHpiWatchdogNumT
         << DATA( rec.WatchdogNum )
         << READONLY()
         << VAR_END();
    vars << "Rdr.WatchdogRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

static void GetVars( SaHpiAnnunciatorRecT& rec, cVars& vars )
{
    vars << "Rdr.AnnunciatorRec.AnnunciatorNum"
         << dtSaHpiAnnunciatorNumT
         << DATA( rec.AnnunciatorNum )
         << READONLY()
         << VAR_END();
    vars << "Rdr.AnnunciatorRec.AnnunciatorType"
         << dtSaHpiAnnunciatorTypeT
         << DATA( rec.AnnunciatorType )
         << VAR_END();
    vars << "Rdr.AnnunciatorRec.ModeReadOnly"
         << dtSaHpiBoolT
         << DATA( rec.ModeReadOnly )
         << VAR_END();
    vars << "Rdr.AnnunciatorRec.MaxConditions"
         << dtSaHpiUint32T
         << DATA( rec.MaxConditions )
         << VAR_END();
    vars << "Rdr.AnnunciatorRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

static void GetVars( SaHpiDimiRecT& rec, cVars& vars )
{
    vars << "Rdr.DimiRec.DimiNum"
         << dtSaHpiDimiNumT
         << DATA( rec.DimiNum )
         << READONLY()
         << VAR_END();
    vars << "Rdr.DimiRec.Oem"
         << dtSaHpiUint32T
         << DATA( rec.Oem )
         << VAR_END();
}

static void GetVars( SaHpiFumiRecT& rec, cVars& vars )
{
    vars << "Rdr.FumiRec.Num"
         << dtSaHpiFumiNumT
         << DATA( rec.Num )
         << READONLY()
         << VAR_END();
    vars << "Rdr.FumiRec.AccessProt"
         << dtSaHpiFumiProtocolT
         << DATA( rec.AccessProt )
         << VAR_END();
    vars << "Rdr.FumiRec.Capability"
         << dtSaHpiFumiCapabilityT
         << DATA( rec.Capability )
         << VAR_END();
    vars << "Rdr.FumiRec.NumBanks"
         << dtSaHpiUint8T
         << DATA( rec.NumBanks )
         << READONLY()
         << VAR_END();
    vars << "Rdr.FumiRec.Oem"
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
    } else if ( rdr.RdrType == SAHPI_WATCHDOG_RDR ) {
        GetVars( rdr.RdrTypeUnion.WatchdogRec, vars );
    } else if ( rdr.RdrType == SAHPI_ANNUNCIATOR_RDR ) {
        GetVars( rdr.RdrTypeUnion.AnnunciatorRec, vars );
    } else if ( rdr.RdrType == SAHPI_DIMI_RDR ) {
        GetVars( rdr.RdrTypeUnion.DimiRec, vars );
    } else if ( rdr.RdrType == SAHPI_FUMI_RDR ) {
        GetVars( rdr.RdrTypeUnion.FumiRec, vars );
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

void GetVars( SaHpiWatchdogT& wd, cVars& vars )
{
    vars << "Watchdog.Log"
         << dtSaHpiBoolT
         << DATA( wd.Log )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.Running"
         << dtSaHpiBoolT
         << DATA( wd.Running )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.TimerUse"
         << dtSaHpiWatchdogTimerUseT
         << DATA( wd.TimerUse )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.TimerAction"
         << dtSaHpiWatchdogActionT
         << DATA( wd.TimerAction )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.PretimerInterrupt"
         << dtSaHpiWatchdogPretimerInterruptT
         << DATA( wd.PretimerInterrupt )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.PreTimeoutInterval"
         << dtSaHpiUint32T
         << DATA( wd.PreTimeoutInterval )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.TimerUseExpFlags"
         << dtSaHpiWatchdogExpFlagsT
         << DATA( wd.TimerUseExpFlags )
         << VAR_END();
    vars << "Watchdog.InitialCount"
         << dtSaHpiUint32T
         << DATA( wd.InitialCount )
         << READONLY()
         << VAR_END();
    vars << "Watchdog.PresentCount"
         << dtSaHpiUint32T
         << DATA( wd.PresentCount )
         << READONLY()
         << VAR_END();
}

void GetVars( SaHpiAnnouncementT& a, cVars& vars )
{
    vars << "EntryId"
         << dtSaHpiEntryIdT
         << DATA( a.EntryId )
         << READONLY()
         << VAR_END();
    vars << "Timestamp"
         << dtSaHpiTimeT
         << DATA( a.Timestamp )
         << VAR_END();
    vars << "AddedByUser"
         << dtSaHpiBoolT
         << DATA( a.AddedByUser )
         << VAR_END();
    vars << "Severity"
         << dtSaHpiSeverityT
         << DATA( a.Severity )
         << VAR_END();
    vars << "Acknowledged"
         << dtSaHpiBoolT
         << DATA( a.Acknowledged )
         << VAR_END();

    SaHpiConditionT& c = a.StatusCond;

    vars << "StatusCond.Type"
         << dtSaHpiStatusCondTypeT
         << DATA( c.Type )
         << VAR_END();
    vars << "StatusCond.Entity"
         << dtSaHpiEntityPathT
         << DATA( c.Entity )
         << VAR_END();
    vars << "StatusCond.DomainId"
         << dtSaHpiDomainIdT
         << DATA( c.DomainId )
         << VAR_END();
    vars << "StatusCond.ResourceId"
         << dtSaHpiResourceIdT
         << DATA( c.ResourceId )
         << VAR_END();
    vars << IF( c.Type == SAHPI_STATUS_COND_TYPE_SENSOR )
         << "StatusCond.SensorNum"
         << dtSaHpiSensorNumT
         << DATA( c.SensorNum )
         << VAR_END();
    vars << IF( c.Type == SAHPI_STATUS_COND_TYPE_SENSOR )
         << "StatusCond.EventState"
         << dtSaHpiEventStateT
         << DATA( c.EventState )
         << VAR_END();
    vars << "StatusCond.Name"
         << dtSaHpiNameT
         << DATA( c.Name )
         << VAR_END();
    vars << IF( c.Type == SAHPI_STATUS_COND_TYPE_OEM )
         << "StatusCond.Mid"
         << dtSaHpiManufacturerIdT
         << DATA( c.Mid )
         << VAR_END();
    vars << IF( c.Type == SAHPI_STATUS_COND_TYPE_OEM )
         << "StatusCond.Data"
         << dtSaHpiTextBufferT
         << DATA( c.Data )
         << VAR_END();
}

static void GetVars( const std::string& name,
                     SaHpiDimiTestParamsDefinitionT& pd,
                     cVars& vars )
{
    vars << name + ".ParamName"
         << dtDimiTestParamName
         << DATA( pd.ParamName )
         << VAR_END();
    vars << name + ".ParamInfo"
         << dtSaHpiTextBufferT
         << DATA( pd.ParamInfo )
         << VAR_END();
    vars << name + ".ParamType"
         << dtSaHpiDimiTestParamTypeT
         << DATA( pd.ParamType )
         << VAR_END();
    if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
        vars << name + ".MinValue.IntValue"
             << dtSaHpiInt32T
             << DATA( pd.MinValue.IntValue )
             << VAR_END();
        vars << name + ".MaxValue.IntValue"
             << dtSaHpiInt32T
             << DATA( pd.MaxValue.IntValue )
             << VAR_END();
    } else if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
        vars << name + ".MinValue.FloatValue"
             << dtSaHpiFloat64T
             << DATA( pd.MinValue.FloatValue )
             << VAR_END();
        vars << name + ".MaxValue.FloatValue"
             << dtSaHpiFloat64T
             << DATA( pd.MaxValue.FloatValue )
             << VAR_END();
    }
    if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN ) {
        vars << name + ".DefaultParam.parambool"
             << dtSaHpiBoolT
             << DATA( pd.DefaultParam.parambool )
             << VAR_END();
    } else if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_INT32 ) {
        vars << name + ".DefaultParam.paramint"
             << dtSaHpiInt32T
             << DATA( pd.DefaultParam.paramint )
             << VAR_END();
    } else if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 ) {
        vars << name + ".DefaultParam.paramfloat"
             << dtSaHpiFloat64T
             << DATA( pd.DefaultParam.paramfloat )
             << VAR_END();
    } else if ( pd.ParamType == SAHPI_DIMITEST_PARAM_TYPE_TEXT ) {
        vars << name + ".DefaultParam.paramtext"
             << dtSaHpiTextBufferT
             << DATA( pd.DefaultParam.paramtext )
             << VAR_END();
    }
}

void GetVars( SaHpiDimiTestT& info, cVars& vars )
{
    char prefix[256];

    vars << "TestInfo.TestName"
         << dtSaHpiTextBufferT
         << DATA( info.TestName )
         << VAR_END();
    vars << "TestInfo.ServiceImpact"
         << dtSaHpiDimiTestServiceImpactT
         << DATA( info.ServiceImpact )
         << VAR_END();
    for ( size_t i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
        snprintf( &prefix[0], sizeof(prefix), "TestInfo.EntitiesImpacted[%u]", (unsigned int)i );
        vars << std::string( prefix ) + ".EntityImpacted"
             << dtSaHpiEntityPathT
             << DATA( info.EntitiesImpacted[i].EntityImpacted )
             << VAR_END();
        vars << std::string( prefix ) + ".ServiceImpact"
             << dtSaHpiDimiTestServiceImpactT
             << DATA( info.EntitiesImpacted[i].ServiceImpact )
             << VAR_END();
    }
    vars << "TestInfo.NeedServiceOS"
         << dtSaHpiBoolT
         << DATA( info.NeedServiceOS )
         << VAR_END();
    vars << "TestInfo.ServiceOS"
         << dtSaHpiTextBufferT
         << DATA( info.ServiceOS )
         << VAR_END();
    vars << "TestInfo.ExpectedRunDuration"
         << dtSaHpiTimeT
         << DATA( info.ExpectedRunDuration )
         << VAR_END();
    vars << "TestInfo.TestCapabilities"
         << dtSaHpiDimiTestCapabilityT
         << DATA( info.TestCapabilities )
         << VAR_END();
    for ( size_t i = 0; i < SAHPI_DIMITEST_MAX_PARAMETERS; ++i ) {
        snprintf( &prefix[0], sizeof(prefix), "TestInfo.TestParameters[%u]", (unsigned int)i );
        GetVars( prefix, info.TestParameters[i], vars );
    }
}

void GetVars( SaHpiFumiSpecInfoT& info, cVars& vars )
{
    vars << "SpecInfo.SpecInfoType"
         << dtSaHpiFumiSpecInfoTypeT
         << DATA( info.SpecInfoType )
         << VAR_END();

    vars << IF( info.SpecInfoType == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED )
         << "SpecInfo.SafDefined.SpecID"
         << dtSaHpiFumiSafDefinedSpecIdT
         << DATA( info.SpecInfoTypeUnion.SafDefined.SpecID )
         << VAR_END();
    vars << IF( info.SpecInfoType == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED )
         << "SpecInfo.SafDefined.RevisionID"
         << dtSaHpiUint32T
         << DATA( info.SpecInfoTypeUnion.SafDefined.RevisionID )
         << VAR_END();

    vars << IF( info.SpecInfoType == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED )
         << "SpecInfo.OemDefined.Mid"
         << dtSaHpiManufacturerIdT
         << DATA( info.SpecInfoTypeUnion.OemDefined.Mid )
         << VAR_END();
    vars << IF( info.SpecInfoType == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED )
         << "SpecInfo.OemDefined.Body"
         << dtSaHpiFumiOemDefinedSpecInfoTWithoutMid
         << DATA( info.SpecInfoTypeUnion.OemDefined )
         << VAR_END();
}

void GetVars( SaHpiFumiServiceImpactDataT& data, cVars& vars )
{
    char prefix[256];

    vars << "ServiceImpact.NumEntities"
         << dtSaHpiUint32T
         << DATA( data.NumEntities )
         << VAR_END();

    for ( size_t i = 0; i < data.NumEntities; ++i ) {
        snprintf( &prefix[0], sizeof(prefix), "ServiceImpact.ImpactedEntities[%u]", (unsigned int)i );
        vars << std::string( prefix ) + ".ImpactedEntity"
             << dtSaHpiEntityPathT
             << DATA( data.ImpactedEntities[i].ImpactedEntity )
             << VAR_END();
        vars << std::string( prefix ) + ".ServiceImpact"
             << dtSaHpiFumiServiceImpactT
             << DATA( data.ImpactedEntities[i].ServiceImpact )
             << VAR_END();
    }
}

void GetVars( SaHpiFumiBankInfoT& info, cVars& vars )
{
    vars << "BankInfo.BankId"
         << dtSaHpiUint8T
         << DATA( info.BankId )
         << READONLY()
         << VAR_END();
    vars << "BankInfo.BankSize"
         << dtSaHpiUint32T
         << DATA( info.BankSize )
         << VAR_END();
    vars << "BankInfo.Position"
         << dtSaHpiUint32T
         << DATA( info.Position )
         << READONLY()
         << VAR_END();
    vars << "BankInfo.BankState"
         << dtSaHpiFumiBankStateT
         << DATA( info.BankState )
         << VAR_END();
    vars << "BankInfo.Identifier"
         << dtSaHpiTextBufferT
         << DATA( info.Identifier )
         << VAR_END();
    vars << "BankInfo.Description"
         << dtSaHpiTextBufferT
         << DATA( info.Description )
         << VAR_END();
    vars << "BankInfo.DateTime"
         << dtSaHpiTextBufferT
         << DATA( info.DateTime )
         << VAR_END();
    vars << "BankInfo.MajorVersion"
         << dtSaHpiUint32T
         << DATA( info.MajorVersion )
         << VAR_END();
    vars << "BankInfo.MinorVersion"
         << dtSaHpiUint32T
         << DATA( info.MinorVersion )
         << VAR_END();
    vars << "BankInfo.AuxVersion"
         << dtSaHpiUint32T
         << DATA( info.AuxVersion )
         << VAR_END();
}

void GetVars( const std::string& name, SaHpiFumiSourceInfoT& info, bool uri_and_status, cVars& vars )
{
    // This function does provide Source URI and Status.
    vars << IF( uri_and_status )
         << name + ".SourceUri"
         << dtSaHpiTextBufferT
         << DATA( info.SourceUri )
         << VAR_END();
    vars << IF( uri_and_status )
         << name + ".SourceStatus"
         << dtSaHpiFumiSourceStatusT
         << DATA( info.SourceStatus )
         << VAR_END();
    vars << name + ".Identifier"
         << dtSaHpiTextBufferT
         << DATA( info.Identifier )
         << VAR_END();
    vars << name + ".Description"
         << dtSaHpiTextBufferT
         << DATA( info.Description )
         << VAR_END();
    vars << name + ".DateTime"
         << dtSaHpiTextBufferT
         << DATA( info.DateTime )
         << VAR_END();
    vars << name + ".MajorVersion"
         << dtSaHpiUint32T
         << DATA( info.MajorVersion )
         << VAR_END();
    vars << name + ".MinorVersion"
         << dtSaHpiUint32T
         << DATA( info.MinorVersion )
         << VAR_END();
    vars << name + ".AuxVersion"
         << dtSaHpiUint32T
         << DATA( info.AuxVersion )
         << VAR_END();
}

static void GetVars( const std::string& name, SaHpiFumiFirmwareInstanceInfoT& info, cVars& vars )
{
    vars << name + ".InstancePresent"
         << dtSaHpiBoolT
         << DATA( info.InstancePresent )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".Identifier"
         << dtSaHpiTextBufferT
         << DATA( info.Identifier )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".Description"
         << dtSaHpiTextBufferT
         << DATA( info.Description )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".DateTime"
         << dtSaHpiTextBufferT
         << DATA( info.DateTime )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".MajorVersion"
         << dtSaHpiUint32T
         << DATA( info.MajorVersion )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".MinorVersion"
         << dtSaHpiUint32T
         << DATA( info.MinorVersion )
         << VAR_END();
    vars << IF( info.InstancePresent != SAHPI_FALSE )
         << name + ".AuxVersion"
         << dtSaHpiUint32T
         << DATA( info.AuxVersion )
         << VAR_END();
}

void GetVars( const std::string& name, SaHpiFumiComponentInfoT& info, cVars& vars )
{
    GetVars( name + ".MainFwInstance", info.MainFwInstance, vars );

    vars << name + ".ComponentFlags"
         << dtSaHpiUint32T
         << DATA( info.ComponentFlags )
         << VAR_END();
}

void GetVars( SaHpiFumiLogicalBankInfoT& info, cVars& vars )
{
    vars << "LogicalBankInfo.FirmwarePersistentLocationCount"
         << dtSaHpiUint8T
         << DATA( info.FirmwarePersistentLocationCount )
         << VAR_END();
    vars << "LogicalBankInfo.BankStateFlags"
         << dtSaHpiFumiLogicalBankStateFlagsT
         << DATA( info.BankStateFlags )
         << VAR_END();

    // Do not show pending/rollback fw info
/*
    GetVars( "LogicalBankInfo.PendingFwInstance", info.PendingFwInstance, vars );
    GetVars( "LogicalBankInfo.RollbackFwInstance", info.RollbackFwInstance, vars );
*/
}

void GetVars( const std::string& name, SaHpiFumiLogicalComponentInfoT& info, cVars& vars )
{
    // Do not show pending/rollback fw info
/*
    GetVars( name + ".PendingFwInstance", info.PendingFwInstance, vars );
    GetVars( name + ".RollbackFwInstance", info.RollbackFwInstance, vars );
*/

    vars << name + ".ComponentFlags"
         << dtSaHpiUint32T
         << DATA( info.ComponentFlags )
         << VAR_END();
}


}; // namespace Structs


}; // namespace TA

