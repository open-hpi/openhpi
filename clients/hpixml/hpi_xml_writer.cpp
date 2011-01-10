/*      -*- c++ -*-
 *
 * Copyright (c) 2010 by Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *
 */

#include <string>

#include <glib.h>

#include <oh_utils.h>

#include "hpi_xml_writer.h"


/***************************************************
 * class cHpiXmlWriter
 ***************************************************/
cHpiXmlWriter::cHpiXmlWriter( int indent_step, bool use_names )
    : cXmlWriter( "http://openhpi.org/SAI-HPI-B.03.02",
                  "http://openhpi.org hpixml.xsd",
                  indent_step,
                  use_names )
{
    // empty
}

void cHpiXmlWriter::Begin( void )
{
    cXmlWriter::Begin( "HPI" );
}

void cHpiXmlWriter::End( void )
{
    cXmlWriter::End( "HPI" );
}

void cHpiXmlWriter::VersionNode( const SaHpiVersionT& ver )
{
    NodeSaHpiVersionT( "Version", ver );
}

void cHpiXmlWriter::BeginDomainNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "Domain" );
    NodeSaHpiDomainIdT( "DomainId", di.DomainId );
    NodeSaHpiDomainCapabilitiesT( "DomainCapabilities", di.DomainCapabilities );
    NodeSaHpiBoolT( "IsPeer", di.IsPeer );
    NodeSaHpiTextBufferT( "DomainTag", di.DomainTag );
    NodeSaHpiGuidT( "Guid", di.Guid );
}

void cHpiXmlWriter::EndDomainNode()
{
    cXmlWriter::EndNode( "Domain" );
}

void cHpiXmlWriter::BeginDrtNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "DomainReferenceTable" );
    NodeSaHpiUint32T( "DrtUpdateCount", di.DrtUpdateCount );
    NodeSaHpiTimeT( "DrtUpdateTimestamp", di.DrtUpdateTimestamp );
}

void cHpiXmlWriter::EndDrtNode()
{
    cXmlWriter::EndNode( "DomainReferenceTable" );
}

void cHpiXmlWriter::DrtEntryNode( const SaHpiDrtEntryT& drte )
{
    cXmlWriter::BeginNode( "Reference" );
    NodeSaHpiDomainIdT( "DomainId", drte.DomainId );
    NodeSaHpiBoolT( "IsPeer", drte.IsPeer );
    cXmlWriter::EndNode( "Reference" );
}

void cHpiXmlWriter::BeginRptNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "ResourcePresenceTable" );
    NodeSaHpiUint32T( "RptUpdateCount", di.RptUpdateCount );
    NodeSaHpiTimeT( "RptUpdateTimestamp", di.RptUpdateTimestamp );
}

void cHpiXmlWriter::EndRptNode()
{
    cXmlWriter::EndNode( "ResourcePresenceTable" );
}

void cHpiXmlWriter::BeginResourceNode( const SaHpiRptEntryT& rpte )
{
    const SaHpiResourceInfoT info = rpte.ResourceInfo;

    cXmlWriter::BeginNode( "Resource" );
    NodeSaHpiResourceIdT( "ResourceId", rpte.ResourceId );
    cXmlWriter::BeginNode( "ResourceInfo" );
    NodeSaHpiUint8T( "ResourceRev", info.ResourceRev );
    NodeSaHpiUint8T( "SpecificVer", info.SpecificVer );
    NodeSaHpiUint8T( "DeviceSupport", info.DeviceSupport );
    NodeSaHpiManufacturerIdT( "ManufacturerId", info.ManufacturerId );
    NodeSaHpiUint16T( "ProductId", info.ProductId );
    NodeSaHpiUint8T( "FirmwareMajorRev", info.FirmwareMajorRev );
    NodeSaHpiUint8T( "FirmwareMinorRev", info.FirmwareMinorRev );
    NodeSaHpiUint8T( "AuxFirmwareRev", info.AuxFirmwareRev );
    NodeSaHpiGuidT( "Guid", info.Guid );
    cXmlWriter::EndNode( "ResourceInfo" );
    NodeSaHpiEntityPathT( "ResourceEntity", rpte.ResourceEntity );
    NodeSaHpiCapabilitiesT( "ResourceCapabilities", rpte.ResourceCapabilities );
    NodeSaHpiHsCapabilitiesT( "HotSwapCapabilities", rpte.HotSwapCapabilities );
    NodeSaHpiSeverityT( "ResourceSeverity", rpte.ResourceSeverity );
    NodeSaHpiBoolT( "ResourceFailed", rpte.ResourceFailed );
    NodeSaHpiTextBufferT( "ResourceTag", rpte.ResourceTag );
}

void cHpiXmlWriter::EndResourceNode()
{
    cXmlWriter::EndNode( "Resource" );
}


void cHpiXmlWriter::BeginDatNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "DomainAlarmTable" );
    NodeSaHpiUint32T( "DatUpdateCount", di.DatUpdateCount );
    NodeSaHpiTimeT( "DatUpdateTimestamp", di.DatUpdateTimestamp );
    NodeSaHpiUint32T( "ActiveAlarms", di.ActiveAlarms );
    NodeSaHpiUint32T( "CriticalAlarms", di.CriticalAlarms );
    NodeSaHpiUint32T( "MajorAlarms", di.MajorAlarms );
    NodeSaHpiUint32T( "MinorAlarms", di.MinorAlarms );
    NodeSaHpiUint32T( "DatUserAlarmLimit", di.DatUserAlarmLimit );
    NodeSaHpiBoolT( "DatOverflow", di.DatOverflow );
}

void cHpiXmlWriter::EndDatNode()
{
    cXmlWriter::EndNode( "DomainAlarmTable" );
}

void cHpiXmlWriter::AlarmNode( const SaHpiAlarmT& a )
{
    cXmlWriter::BeginNode( "Alarm" );
    NodeSaHpiAlarmIdT( "AlarmId", a.AlarmId );
    NodeSaHpiTimeT( "Timestamp", a.Timestamp );
    NodeSaHpiSeverityT( "Severity", a.Severity );
    NodeSaHpiBoolT( "Acknowledged", a.Acknowledged );
    NodeSaHpiConditionT( "AlarmCond", a.AlarmCond );
    cXmlWriter::EndNode( "Alarm" );
}


/****************************
 * Basic Types
 ***************************/
void cHpiXmlWriter::NodeSaHpiUint8T(
    const char * name,
    const SaHpiUint8T& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiUint16T(
    const char * name,
    const SaHpiUint16T& x )

{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiUint32T(
    const char * name,
    const SaHpiUint32T& x )
{
    cXmlWriter::Node( name, "%u", x );
}

void cHpiXmlWriter::NodeSaHpiUint64T(
    const char * name,
    const SaHpiUint64T& x )
{
    cXmlWriter::Node( name, "%llu", x );
}

void cHpiXmlWriter::NodeSaHpiInt8T(
    const char * name,
    const SaHpiInt8T& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInt16T(
    const char * name,
    const SaHpiInt16T& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInt32T(
    const char * name,
    const SaHpiInt32T& x )
{
    cXmlWriter::Node( name, "%d", x );
}

void cHpiXmlWriter::NodeSaHpiInt64T(
    const char * name,
    const SaHpiInt64T& x )
{
    cXmlWriter::Node( name, "%lld", x );
}

void cHpiXmlWriter::NodeSaHpiFloat64T(
    const char * name,
    const SaHpiFloat64T& x )
{
    cXmlWriter::Node( name, "%f", x );
}


/****************************
 * Typedefs
 ***************************/
void cHpiXmlWriter::NodeSaHpiBoolT(
    const char * name,
    const SaHpiBoolT& x )
{
    cXmlWriter::Node( name, "%s", ( x == SAHPI_FALSE ) ? "false" : "true" );
}

void cHpiXmlWriter::NodeSaHpiManufacturerIdT(
    const char * name,
    const SaHpiManufacturerIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiVersionT(
    const char * name,
    const SaHpiVersionT& x )
{
    SaHpiUint8T comp  = ( x >> 16 ) & 0xFF;
    SaHpiUint8T major = ( x >> 8 ) & 0xFF;
    SaHpiUint8T minor = x & 0xFF;
    cXmlWriter::Node( name, "%c.%02u.%02u", 'A' - 1 + comp, major, minor );
}

void cHpiXmlWriter::NodeSaErrorT(
    const char * name,
    const SaErrorT& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDomainIdT(
    const char * name,
    const SaHpiDomainIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSessionIdT(
    const char * name,
    const SaHpiSessionIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiResourceIdT(
    const char * name,
    const SaHpiResourceIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEntryIdT(
    const char * name,
    const SaHpiEntryIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTimeT(
    const char * name,
    const SaHpiTimeT& x )
{
    NodeSaHpiInt64T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTimeoutT(
    const char * name,
    const SaHpiTimeoutT& x )
{
    NodeSaHpiInt64T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInstrumentIdT(
    const char * name,
    const SaHpiInstrumentIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEntityLocationT(
    const char * name,
    const SaHpiEntityLocationT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventCategoryT(
    const char * name,
    const SaHpiEventCategoryT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventStateT(
    const char * name,
    const SaHpiEventStateT& x )
{
    NodeSaHpiUint16T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorNumT(
    const char * name,
    const SaHpiSensorNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorRangeFlagsT(
    const char * name,
    const SaHpiSensorRangeFlagsT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorThdMaskT(
    const char * name,
    const SaHpiSensorThdMaskT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiCtrlNumT(
    const char * name,
    const SaHpiCtrlNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateDiscreteT(
    const char * name,
    const SaHpiCtrlStateDiscreteT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateAnalogT(
    const char * name,
    const SaHpiCtrlStateAnalogT& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTxtLineNumT(
    const char * name,
    const SaHpiTxtLineNumT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiIdrIdT(
    const char * name,
    const SaHpiIdrIdT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiWatchdogNumT(
    const char * name,
    const SaHpiWatchdogNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiWatchdogExpFlagsT(
    const char * name,
    const SaHpiWatchdogExpFlagsT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiNumT(
    const char * name,
    const SaHpiDimiNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiTestCapabilityT(
    const char * name,
    const SaHpiDimiTestCapabilityT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiTestNumT(
    const char * name,
    const SaHpiDimiTestNumT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiTestPercentCompletedT(
    const char * name,
    const SaHpiDimiTestPercentCompletedT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiNumT(
    const char * name,
    const SaHpiFumiNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiBankNumT(
    const char * name,
    const SaHpiBankNumT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiLogicalBankStateFlagsT(
    const char * name,
    const SaHpiFumiLogicalBankStateFlagsT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiProtocolT(
    const char * name,
    const SaHpiFumiProtocolT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiCapabilityT(
    const char * name,
    const SaHpiFumiCapabilityT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorOptionalDataT(
    const char * name,
    const SaHpiSensorOptionalDataT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorEnableOptDataT(
    const char * name,
    const SaHpiSensorEnableOptDataT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEvtQueueStatusT(
    const char * name,
    const SaHpiEvtQueueStatusT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorNumT(
    const char * name,
    const SaHpiAnnunciatorNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiLoadNumberT(
    const char * name,
    const SaHpiLoadNumberT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiCapabilitiesT(
    const char * name,
    const SaHpiCapabilitiesT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiHsCapabilitiesT(
    const char * name,
    const SaHpiHsCapabilitiesT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDomainCapabilitiesT(
    const char * name,
    const SaHpiDomainCapabilitiesT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiAlarmIdT(
    const char * name,
    const SaHpiAlarmIdT& x )
{
    NodeSaHpiEntryIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventLogCapabilitiesT(
    const char * name,
    const SaHpiEventLogCapabilitiesT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventLogEntryIdT(
    const char * name,
    const SaHpiEventLogEntryIdT& x )
{
    NodeSaHpiUint32T( name, x );
}


/****************************
 * Enums
 ***************************/
void cHpiXmlWriter::NodeSaHpiLanguageT(
    const char * name,
    const SaHpiLanguageT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_language );
}

void cHpiXmlWriter::NodeSaHpiTextTypeT(
    const char * name,
    const SaHpiTextTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_texttype );
}

void cHpiXmlWriter::NodeSaHpiEntityTypeT(
    const char * name,
    const SaHpiEntityTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_entitytype );
}

void cHpiXmlWriter::NodeSaHpiSensorTypeT(
    const char * name,
    const SaHpiSensorTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensortype );
}

void cHpiXmlWriter::NodeSaHpiSensorReadingTypeT(
    const char * name,
    const SaHpiSensorReadingTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensorreadingtype );
}

void cHpiXmlWriter::NodeSaHpiSensorEventMaskActionT(
    const char * name,
    const SaHpiSensorEventMaskActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensoreventmaskaction );
}

void cHpiXmlWriter::NodeSaHpiSensorUnitsT(
    const char * name,
    const SaHpiSensorUnitsT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensorunits );
}

void cHpiXmlWriter::NodeSaHpiSensorModUnitUseT(
    const char * name,
    const SaHpiSensorModUnitUseT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensormodunituse );
}

void cHpiXmlWriter::NodeSaHpiSensorEventCtrlT(
    const char * name,
    const SaHpiSensorEventCtrlT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensoreventctrl );
}

void cHpiXmlWriter::NodeSaHpiCtrlTypeT(
    const char * name,
    const SaHpiCtrlTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrltype );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateDigitalT(
    const char * name,
    const SaHpiCtrlStateDigitalT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrlstatedigital );
}

void cHpiXmlWriter::NodeSaHpiCtrlModeT(
    const char * name,
    const SaHpiCtrlModeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrlmode );
}

void cHpiXmlWriter::NodeSaHpiCtrlOutputTypeT(
    const char * name,
    const SaHpiCtrlOutputTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrloutputtype );
}

void cHpiXmlWriter::NodeSaHpiIdrAreaTypeT(
    const char * name,
    const SaHpiIdrAreaTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_idrareatype );
}

void cHpiXmlWriter::NodeSaHpiIdrFieldTypeT(
    const char * name,
    const SaHpiIdrFieldTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_idrfieldtype );
}

void cHpiXmlWriter::NodeSaHpiWatchdogActionT(
    const char * name,
    const SaHpiWatchdogActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogaction );
}

void cHpiXmlWriter::NodeSaHpiWatchdogActionEventT(
    const char * name,
    const SaHpiWatchdogActionEventT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogactionevent );
}

void cHpiXmlWriter::NodeSaHpiWatchdogPretimerInterruptT(
    const char * name,
    const SaHpiWatchdogPretimerInterruptT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogpretimerinterrupt );
}

void cHpiXmlWriter::NodeSaHpiWatchdogTimerUseT(
    const char * name,
    const SaHpiWatchdogTimerUseT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogtimeruse );
}

void cHpiXmlWriter::NodeSaHpiDimiTestServiceImpactT(
    const char * name,
    const SaHpiDimiTestServiceImpactT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestserviceimpact );
}

void cHpiXmlWriter::NodeSaHpiDimiTestRunStatusT(
    const char * name,
    const SaHpiDimiTestRunStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestrunstatus );
}

void cHpiXmlWriter::NodeSaHpiDimiTestErrCodeT(
    const char * name,
    const SaHpiDimiTestErrCodeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitesterrcode );
}

void cHpiXmlWriter::NodeSaHpiDimiTestParamTypeT(
    const char * name,
    const SaHpiDimiTestParamTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestparamtype );
}

void cHpiXmlWriter::NodeSaHpiDimiReadyT(
    const char * name,
    const SaHpiDimiReadyT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimiready );
}

void cHpiXmlWriter::NodeSaHpiFumiSpecInfoTypeT(
    const char * name,
    const SaHpiFumiSpecInfoTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumispecinfotype );
}

void cHpiXmlWriter::NodeSaHpiFumiSafDefinedSpecIdT(
    const char * name,
    const SaHpiFumiSafDefinedSpecIdT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumisafdefinedspecid );
}

void cHpiXmlWriter::NodeSaHpiFumiServiceImpactT(
    const char * name,
    const SaHpiFumiServiceImpactT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumiserviceimpact );
}

void cHpiXmlWriter::NodeSaHpiFumiSourceStatusT(
    const char * name,
    const SaHpiFumiSourceStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumisourcestatus );
}

void cHpiXmlWriter::NodeSaHpiFumiBankStateT(
    const char * name,
    const SaHpiFumiBankStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumibankstate );
}

void cHpiXmlWriter::NodeSaHpiFumiUpgradeStatusT(
    const char * name,
    const SaHpiFumiUpgradeStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumiupgradestatus );
}

void cHpiXmlWriter::NodeSaHpiHsIndicatorStateT(
    const char * name,
    const SaHpiHsIndicatorStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsindicatorstate );
}

void cHpiXmlWriter::NodeSaHpiHsActionT(
    const char * name,
    const SaHpiHsActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsaction );
}

void cHpiXmlWriter::NodeSaHpiHsStateT(
    const char * name,
    const SaHpiHsStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsstate );
}

void cHpiXmlWriter::NodeSaHpiHsCauseOfStateChangeT(
    const char * name,
    const SaHpiHsCauseOfStateChangeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hscauseofstatechange );
}

void cHpiXmlWriter::NodeSaHpiSeverityT(
    const char * name,
    const SaHpiSeverityT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_severity );
}

void cHpiXmlWriter::NodeSaHpiResourceEventTypeT(
    const char * name,
    const SaHpiResourceEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_resourceeventtype );
}

void cHpiXmlWriter::NodeSaHpiDomainEventTypeT(
    const char * name,
    const SaHpiDomainEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_domaineventtype );
}

void cHpiXmlWriter::NodeSaHpiSwEventTypeT(
    const char * name,
    const SaHpiSwEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sweventtype );
}

void cHpiXmlWriter::NodeSaHpiEventTypeT(
    const char * name,
    const SaHpiEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_eventtype );
}

void cHpiXmlWriter::NodeSaHpiStatusCondTypeT(
    const char * name,
    const SaHpiStatusCondTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_statuscondtype );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorModeT(
    const char * name,
    const SaHpiAnnunciatorModeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_annunciatormode );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorTypeT(
    const char * name,
    const SaHpiAnnunciatorTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_annunciatortype );
}

void cHpiXmlWriter::NodeSaHpiRdrTypeT(
    const char * name,
    const SaHpiRdrTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_rdrtype );
}

void cHpiXmlWriter::NodeSaHpiParmActionT(
    const char * name,
    const SaHpiParmActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_parmaction );
}

void cHpiXmlWriter::NodeSaHpiResetActionT(
    const char * name,
    const SaHpiResetActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_resetaction );
}

void cHpiXmlWriter::NodeSaHpiPowerStateT(
    const char * name,
    const SaHpiPowerStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_powerstate );
}

void cHpiXmlWriter::NodeSaHpiEventLogOverflowActionT(
    const char * name,
    const SaHpiEventLogOverflowActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_eventlogoverflowaction );
}


/****************************
 * Complex Types
 ***************************/
void cHpiXmlWriter::NodeSaHpiTextBufferT(
    const char * name,
    const SaHpiTextBufferT& x )
{
    std::string s;

    cXmlWriter::BeginNode( name );
    NodeSaHpiTextTypeT( "DataType", x.DataType );
    NodeSaHpiLanguageT( "Language", x.Language );
    NodeSaHpiUint8T( "DataLength", x.DataLength );
    switch ( x.DataType ) {
        case SAHPI_TL_TYPE_UNICODE: // UTF-16, Little-Endian
            for ( size_t i = 0; i < x.DataLength; i += 2 ) {
                gunichar2 utf16;
                utf16 = ( ( (gunichar2)x.Data[i + 1] ) << 8 ) | x.Data[i];
                gchar * utf8 = g_utf16_to_utf8( &utf16, 1, 0, 0, 0 );
                if ( utf8 ) {
                    s.append( utf8 );
                    g_free( utf8 );
                } else {
                    s.push_back( '?' );
                }
            }
            cXmlWriter::Node( "Data", "%s", s.c_str() );
            break;
        case SAHPI_TL_TYPE_BCDPLUS:
        case SAHPI_TL_TYPE_ASCII6:
        case SAHPI_TL_TYPE_TEXT:
            s.assign( reinterpret_cast<const char *>(&x.Data[0]), x.DataLength );
            cXmlWriter::Node( "Data", "%s", s.c_str() );
            break;
        case SAHPI_TL_TYPE_BINARY:
        default:
            cXmlWriter::NodeHex( "Data", &x.Data[0], x.DataLength );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiEntityPathT(
    const char * name,
    const SaHpiEntityPathT& x )
{
    SaErrorT rv;
    oh_big_textbuffer buf;
    rv = oh_init_bigtext( &buf );
    if ( rv == SA_OK ) {
        rv = oh_decode_entitypath( &x, &buf );
    }
    if ( rv == SA_OK ) {
        cXmlWriter::Node( name,
                          "%s",
                          reinterpret_cast<const char *>(&buf.Data[0]) );
    } else {
        cXmlWriter::Node( name, "%s", "{UNKNOWN, 0}" );
    }
}

void cHpiXmlWriter::NodeSaHpiConditionT(
    const char * name,
    const SaHpiConditionT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiStatusCondTypeT( "Type", x.Type );

typedef struct {

    SaHpiStatusCondTypeT Type;         /* Status Condition Type */
    SaHpiEntityPathT     Entity;       /* Entity assoc. with status condition */
    SaHpiDomainIdT       DomainId;     /* Domain associated with status.
                                          May be SAHPI_UNSPECIFIED_DOMAIN_ID
                                          meaning current domain, or domain
                                          not meaningful for status condition*/
    SaHpiResourceIdT     ResourceId;   /* Resource associated with status.
                                          May be SAHPI_UNSPECIFIED_RESOURCE_ID
                                          if Type is SAHPI_STATUS_COND_USER.
                                          Must be set to valid ResourceId in
                                          domain specified by DomainId,
                                          or in current domain, if DomainId
                                          is SAHPI_UNSPECIFIED_DOMAIN_ID */
    SaHpiSensorNumT      SensorNum;    /* Sensor associated with status
                                          Only valid if Type is
                                          SAHPI_STATUS_COND_TYPE_SENSOR */
    SaHpiEventStateT     EventState;   /* Sensor event state.
                                          Only valid if Type is
                                          SAHPI_STATUS_COND_TYPE_SENSOR. */
    SaHpiNameT           Name;         /* AIS compatible identifier associated
                                          with Status condition */
    SaHpiManufacturerIdT Mid;          /* Manufacturer Id associated with
                                          status condition, required when type
                                          is SAHPI_STATUS_COND_TYPE_OEM. */
    SaHpiTextBufferT     Data;         /* Optional Data associated with
                                          Status condition */
} SaHpiConditionT;


    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiGuidT(
    const char * name,
    const SaHpiGuidT& x )
{
    cXmlWriter::NodeHex( name, &x[0], sizeof(x) );
}

