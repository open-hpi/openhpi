/*
 * ipmi_sensor.h
 *
 * Copyright (c) 2003,2004 by FORCE Computers
 *
 * Note that this file is based on parts of OpenIPMI
 * written by Corey Minyard <minyard@mvista.com>
 * of MontaVista Software. Corey's code was helpful
 * and many thanks go to him. He gave the permission
 * to use this code in OpenHPI under BSD license.
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


#ifndef dIpmiSensor_h
#define dIpmiSensor_h


#include <assert.h>


__BEGIN_DECLS
#include "SaHpi.h"
__END_DECLS

#ifndef dIpmiMsg_h
#include "ipmi_msg.h"
#endif

#ifndef dIpmiEvent_h
#include "ipmi_event.h"
#endif

#ifndef dIpmiSdr_h
#include "ipmi_sdr.h"
#endif

#ifndef dIpmiRdr_h
#include "ipmi_rdr.h"
#endif


struct tIpmiEntity;
class  cIpmiDomain;


enum tIpmiSensorType
{
  eIpmiSensorTypeInvalid                      = 0x00,
  eIpmiSensorTypeTemperature                  = 0x01,
  eIpmiSensorTypeVoltage		      = 0x02,
  eIpmiSensorTypeCurrent		      = 0x03,
  eIpmiSensorTypeFan			      = 0x04,
  eIpmiSensorTypePhysicalSecurity	      = 0x05,
  eIpmiSensorTypePlatformSecurity	      = 0x06,
  eIpmiSensorTypeProcessor		      =	0x07,
  eIpmiSensorTypePowerSupply		      =	0x08,
  eIpmiSensorTypePowerUnit		      =	0x09,
  eIpmiSensorTypeCoolingDevice		      =	0x0a,
  eIpmiSensorTypeOtherUnitsBasedSensor	      = 0x0b,
  eIpmiSensorTypeMemory			      =	0x0c,
  eIpmiSensorTypeDriveSlot		      =	0x0d,
  eIpmiSensorTypePowerMemoryResize	      =	0x0e,
  eIpmiSensorTypeSystemFirmwareProgress	      = 0x0f,
  eIpmiSensorTypeEventLoggingDisabled	      =	0x10,
  eIpmiSensorTypeWatchdog1		      =	0x11,
  eIpmiSensorTypeSystemEvent		      =	0x12,
  eIpmiSensorTypeCriticalInterrupt	      =	0x13,
  eIpmiSensorTypeButton			      =	0x14,
  eIpmiSensorTypeModuleBoard		      =	0x15,
  eIpmiSensorTypeMicrocontrollerCoprocessor   =	0x16,
  eIpmiSensorTypeAddInCard		      =	0x17,
  eIpmiSensorTypeChassis		      =	0x18,
  eIpmiSensorTypeChipSet		      =	0x19,
  eIpmiSensorTypeOtherFru		      =	0x1a,
  eIpmiSensorTypeCableInterconnect	      =	0x1b,
  eIpmiSensorTypeTerminator		      =	0x1c,
  eIpmiSensorTypeSystemBootInitiated	      =	0x1d,
  eIpmiSensorTypeBootError		      =	0x1e,
  eIpmiSensorTypeOsBoot			      = 0x1f,
  eIpmiSensorTypeOsCriticalStop		      = 0x20,
  eIpmiSensorTypeSlotConnector		      = 0x21,
  eIpmiSensorTypeSystemAcpiPowerState	      = 0x22,
  eIpmiSensorTypeWatchdog2		      = 0x23,
  eIpmiSensorTypePlatformAlert		      = 0x24,
  eIpmiSensorTypeEntityPresence		      = 0x25,
  eIpmiSensorTypeMonitorAsicIc		      = 0x26,
  eIpmiSensorTypeLan			      = 0x27,
  eIpmiSensorTypeManagementSubsystemHealth    = 0x28,
  eIpmiSensorTypeBattery		      = 0x29,
  eIpmiSensorTypeAtcaHotSwap                  = 0xf0
};

const char *IpmiSensorTypeToString( tIpmiSensorType type );


enum tIpmiEventReadingType
{
  eIpmiEventReadingTypeInvalid                   = 0x00,
  eIpmiEventReadingTypeThreshold                 = 0x01,
  eIpmiEventReadingTypeDiscreteUsage             = 0x02,
  eIpmiEventReadingTypeDiscreteState             = 0x03,
  eIpmiEventReadingTypeDiscretePredictiveFailure = 0x04,
  eIpmiEventReadingTypeDiscreteLimitExceeded     = 0x05,
  eIpmiEventReadingTypeDiscretePerformanceMet    = 0x06,
  eIpmiEventReadingTypeDiscreteSeverity          = 0x07,
  eIpmiEventReadingTypeDiscreteDevicePresence    = 0x08,
  eIpmiEventReadingTypeDiscreteDeviceEnable      = 0x09,
  eIpmiEventReadingTypeDiscreteAvailability      = 0x0a,
  eIpmiEventReadingTypeDiscreteRedundancy        = 0x0b,
  eIpmiEventReadingTypeDiscreteAcpiPower         = 0x0c,
  eIpmiEventReadingTypeSensorSpecific            = 0x6f
};

const char *IpmiEventReadingTypeToString( tIpmiEventReadingType type );


enum tIpmiEventSupport
{
  eIpmiEventSupportPerState     = 0,
  eIpmiEventSupportEntireSensor = 1,
  eIpmiEventSupportGlobalEnable = 2,
  eIpmiEventSupportNone         = 3
};

const char *IpmiEventSupportToString( tIpmiEventSupport val );


enum tIpmiValuePresent
{
  eIpmiNoValuesPresent,
  eIpmiRawValuePresent,
  eIpmiBothValuesPresent
};


#define dSensorIdLen 32


class cIpmiSensor : public cIpmiRdr
{
protected:
  cIpmiMc      *m_source_mc; // If the sensor came from the main SDR,
                             // this will be NULL.  Otherwise, it
                             // will be the MC that owned the device
                             // SDR this came from.

  SaHpiEventStateT m_event_state; // HPI current event state

  bool          m_destroyed;
  int           m_use_count;

  unsigned char m_owner;
  unsigned char m_channel;
  unsigned char m_num;

  /*
  tIpmiEntityId m_entity_id;
  unsigned int  m_entity_instance;
  bool          m_entity_instance_logical;
  */

  bool          m_sensor_init_scanning;
  bool          m_sensor_init_events;
  bool          m_sensor_init_type;
  bool          m_sensor_init_pu_events;
  bool          m_sensor_init_pu_scanning;
  bool          m_ignore_if_no_entity;
  bool          m_supports_auto_rearm;

  tIpmiEventSupport m_event_support;

  tIpmiSensorType       m_sensor_type;
  tIpmiEventReadingType m_event_reading_type;

  unsigned int m_oem;

  const char *m_sensor_type_string;
  const char *m_event_reading_type_string;
  const char *m_rate_unit_string;
  const char *m_base_unit_string;
  const char *m_modifier_unit_string;

public:
  cIpmiSensor( cIpmiMc *mc );
  virtual ~cIpmiSensor();

  cIpmiMc *&SourceMc() { return m_source_mc; }

  virtual unsigned int Num() const { return m_num; }

  SaHpiEventStateT &EventState() { return m_event_state; }

  tIpmiSensorType SensorType() const { return m_sensor_type; }
  tIpmiEventReadingType EventReadingType() const { return m_event_reading_type; }

  bool IgnoreIfNoEntity() const { return m_ignore_if_no_entity; }

  tIpmiEventSupport EventSupport() const { return m_event_support; }

  virtual void HandleNew( cIpmiDomain *domain );

  virtual bool Cmp( const cIpmiSensor &s2 ) const;

  unsigned int GetOem() { return m_oem; }

  // create an HPI event from ipmi event
  virtual SaErrorT CreateEvent( cIpmiEvent *event, SaHpiEventT &h );

  // handle all incomming sensor events
  virtual void HandleEvent( cIpmiEvent *event );

  bool Ignore();

  virtual void Dump( cIpmiLog &dump ) const;

  // read sensor parameter from Full Sensor Record
  virtual bool GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr );

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // read sensor
  virtual SaErrorT GetSensorReading( cIpmiMsg &rsp );

  // get sensor data
  virtual SaErrorT GetData( SaHpiSensorReadingT &data ) = 0;

  virtual SaErrorT GetEventEnables( SaHpiSensorEvtEnablesT &enables ) = 0;

protected:
  virtual SaErrorT GetEventEnables( SaHpiSensorEvtEnablesT &enables,
                                    cIpmiMsg &rsp );

public:
 virtual SaErrorT SetEventEnables( const SaHpiSensorEvtEnablesT &enables ) = 0;

protected:
  virtual SaErrorT SetEventEnables( const SaHpiSensorEvtEnablesT &enables,
                                    cIpmiMsg &msg );
};


#endif
