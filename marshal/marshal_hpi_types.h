/*
 * marshaling/demarshaling of hpi data types
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#ifndef dMarshalHpiTypes_h
#define dMarshalHpiTypes_h


#ifdef __cplusplus
extern "C" {
#endif

#include <SaHpi.h>
#ifdef __cplusplus
}
#endif


#ifndef dMarshal_h
#include "marshal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define SaHpiUint8Type   Uint8Type
#define SaHpiUint16Type  Uint16Type
#define SaHpiUint32Type  Uint32Type
#define SaHpiInt8Type    Int8Type
#define SaHpiInt16Type   Int16Type
#define SaHpiInt32Type   Int32Type
#define SaHpiInt64Type   Int64Type
#define SaHpiUint64Type  Uint64Type
#define SaHpiFloat32Type Float32Type
#define SaHpiFloat64Type Float64Type

#define SaHpiBoolType    SaHpiUint8Type
#define SaHpiManufacturerIdType SaHpiUint32Type
#define SaHpiVersionType SaHpiUint32Type
#define SaErrorType SaHpiUint32Type

#define SaHpiDomainIdType   SaHpiUint32Type
#define SaHpiSessionIdType  SaHpiUint32Type
#define SaHpiResourceIdType SaHpiUint32Type
#define SaHpiEntryIdType    SaHpiUint32Type
#define SaHpiTimeType       SaHpiInt64Type
#define SaHpiTimeoutType    SaHpiInt64Type

// text buffer
#define SaHpiTextTypeType   SaHpiUint32Type
#define SaHpiLanguageType   SaHpiUint32Type
extern cMarshalType SaHpiTextBufferType;

// entity
#define SaHpiEntityTypeType SaHpiUint32Type
#define SaHpiEntityLocationType SaHpiUint32Type
extern cMarshalType SaHpiEntityType;
extern cMarshalType SaHpiEntityPathType;

// events
#define SaHpiEventCategoryType SaHpiUint8Type
#define SaHpiEventStateType SaHpiUint16Type

// sensors
#define SaHpiSensorNumType SaHpiUint32Type
#define SaHpiSensorTypeType SaHpiUint32Type
#define SaHpiSensorReadingTypeType SaHpiUint32Type 
//#define SaHpiSensorInterpretedTypeType SaHpiUint32Type
//extern cMarshalType SaHpiSensorInterpretedType;

// sensor status
#define SaHpiSensorStatusType SaHpiUint8Type
extern cMarshalType SaHpiSensorEvtStatusType;
extern cMarshalType SaHpiSensorEvtEnablesType;

// sensor reading
#define SaHpiSensorReadingFormatsType SaHpiUint8Type
extern cMarshalType SaHpiSensorReadingType;

extern cMarshalType SaHpiSensorThresholdsType;

// sensor rdr
#define SaHpiSensorLinearizationType SaHpiUint32Type
extern cMarshalType SaHpiSensorFactorsType;

#define SaHpiSensorRangeFlagsType SaHpiUint8Type
extern cMarshalType SaHpiSensorRangeType;  

#define SaHpiSensorUnitsType SaHpiUint32Type
#define SaHpiSensorModUnitUseType SaHpiUint32Type
#define SaHpiReadingTypeType SaHpiUint32Type
//#define SaHpiSensorSignFormatType SaHpiUint32Type
extern cMarshalType SaHpiSensorDataFormatType;

// threshold support
#define SaHpiSensorThdMaskType SaHpiUint8Type
//#define SaHpiSensorThdCapType SaHpiUint8Type
extern cMarshalType SaHpiSensorThdDefnType;

// event control
#define SaHpiSensorEventCtrlType SaHpiUint32Type

// record
extern cMarshalType SaHpiSensorRecType;

// controls
#define SaHpiCtrlNumType SaHpiUint8Type
#define SaHpiCtrlTypeType SaHpiUint32Type
#define SaHpiCtrlStateDigitalType SaHpiUint32Type
#define SaHpiCtrlStateDiscreteType SaHpiUint32Type
#define SaHpiCtrlStateAnalogType SaHpiInt32Type
extern cMarshalType SaHpiCtrlStateStreamType;

#define SaHpiTxtLineNumType SaHpiUint8Type
extern cMarshalType SaHpiCtrlStateTextType;
extern cMarshalType SaHpiCtrlStateOemType;
extern cMarshalType SaHpiCtrlStateType;
  
// control resource data records
#define SaHpiCtrlOutputTypeType SaHpiUint32Type
extern cMarshalType SaHpiCtrlRecDigitalType;
extern cMarshalType SaHpiCtrlRecDiscreteType;
extern cMarshalType SaHpiCtrlRecAnalogType;
extern cMarshalType SaHpiCtrlRecStreamType;
extern cMarshalType SaHpiCtrlRecTextType;
extern cMarshalType SaHpiCtrlRecOemType;
extern cMarshalType SaHpiCtrlRecType;

// inventory data
#define SaHpiEirIdType SaHpiUint8Type
#define SaHpiInventDataValidityType SaHpiUint32Type
#define SaHpiInventDataRecordTypeType SaHpiUint32Type
#define SaHpiInventChassisTypeType SaHpiUint32Type
extern cMarshalType SaHpiInventoryDataType;

// inventory resource data records
extern cMarshalType SaHpiInventoryRecType;

// watchdogs
#define SaHpiWatchdogNumType SaHpiUint8Type
#define SaHpiWatchdogActionType SaHpiUint32Type
#define SaHpiWatchdogActionEventType SaHpiUint32Type
#define SaHpiWatchdogPretimerInterruptType SaHpiUint32Type
#define SaHpiWatchdogTimerUseType SaHpiUint32Type
#define SaHpiWatchdogExpFlagsType SaHpiUint8Type
extern cMarshalType SaHpiWatchdogType;

// watchdog resource data records
extern cMarshalType SaHpiWatchdogRecType;
  
// resource data record
#define SaHpiRdrTypeType SaHpiUint32Type
extern cMarshalType SaHpiRdrType;

// hot swap
#define SaHpiHsPowerStateType SaHpiUint32Type
#define SaHpiHsIndicatorStateType SaHpiUint32Type
#define SaHpiHsActionType SaHpiUint32Type
#define SaHpiHsStateType SaHpiUint32Type

// events
#define SaHpiSeverityType SaHpiUint32Type
#define SaHpiSensorOptionalDataType SaHpiUint8Type
extern cMarshalType SaHpiSensorEventType;
extern cMarshalType SaHpiHotSwapEventType;
extern cMarshalType SaHpiWatchdogEventType;
extern cMarshalType SaHpiOemEventType;
extern cMarshalType SaHpiUserEventType;
#define SaHpiEventTypeType SaHpiUint32Type
extern cMarshalType SaHpiEventType;

// param control
#define SaHpiParmActionType SaHpiUint32Type

// reset
#define SaHpiResetActionType SaHpiUint32Type

// resource presence table
extern cMarshalType SaHpiRptInfoType;

// resource info type defs
extern cMarshalType SaHpiResourceInfoType;

// resource capabilities
#define SaHpiCapabilitiesType SaHpiUint32Type

// rpt entry
extern cMarshalType SaHpiRptEntryType;

// system event log
#define SaHpiSelOverflowActionType SaHpiUint32Type
extern cMarshalType SaHpiEventLogInfoType;
#define SaHpiEventLogEntryIdType SaHpiUint32Type
extern cMarshalType SaHpiEventLogEntryType;


#ifdef __cplusplus
}
#endif

#endif
