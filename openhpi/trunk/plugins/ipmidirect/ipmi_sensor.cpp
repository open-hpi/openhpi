/*
 * ipmi_sensor.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>
#include <math.h>

#include "ipmi_domain.h"
#include "ipmi_mc.h"
#include "ipmi_sensor.h"
#include "ipmi_entity.h"
#include "ipmi_utils.h"
#include "ipmi_type_code.h"


static const char *sensor_types[] =
{
  "Unspecified",
  "Temperature",
  "Voltage",
  "Current",
  "Fan",
  "PhysicalSecurity",
  "PlatformSecurity",
  "Processor",
  "PowerSupply",
  "PowerUnit",
  "CoolingDevice",
  "OtherUnitsBasedSensor",
  "Memory",
  "DriveSlot",
  "PowerMemoryResize",
  "SystemFirmwareProgress",
  "EventLoggingDisabled",
  "Watchdog1",
  "SystemEvent",
  "CriticalInterrupt",
  "Button",
  "ModuleBoard",
  "MicrocontrollerCoprocessor",
  "AddInCard",
  "Chassis",
  "ChipSet",
  "OtherFru",
  "CableInterconnect",
  "Terminator",
  "SystemBootInitiated",
  "BootError",
  "OsBoot",
  "OsCriticalStop",
  "SlotConnector",
  "SystemAcpiPowerState",
  "Watchdog2",
  "PlatformAlert",
  "EntityPresense",
  "MonitorAsicIc",
  "Lan",
  "ManagementSubsystemHealth",
  "Battery"
};


const char *
IpmiSensorTypeToString( tIpmiSensorType val )
{
  if ( val > eIpmiSensorTypeBattery )
     {
       if ( val == eIpmiSensorTypeAtcaHotSwap )
            return "AtcaHotswap";

       return "Invalid";
     }

  return sensor_types[val];
}


static const char *event_support_types[] =
{
    "PerState",
    "EntireSensor",
    "GlobalDisable",
    "None",
};


const char *
IpmiEventSupportToString( tIpmiEventSupport val )
{
  if ( val > eIpmiEventSupportNone )
       return "Invalid";

  return event_support_types[val];
}


static const char *event_reading_types[] =
{
  "Unspecified",
  "Threshold",
  "DiscreteUsage",
  "DiscreteState",
  "DiscretePredictiveFailure",
  "DiscreteLimitExceeded",
  "DiscretePerformanceMet",
  "DiscreteSeverity",
  "DiscreteDevicePresense",
  "DiscreteDevice_enable",
  "DiscreteAvailability",
  "DiscreteRedundancy",
  "DiscreteAcpiPower",
};


const char *
IpmiEventReadingTypeToString( tIpmiEventReadingType val )
{
  if ( val == eIpmiEventReadingTypeSensorSpecific )
       return "SensorSpecific";
  
  if ( val > eIpmiEventReadingTypeDiscreteAcpiPower )
       return "Invalid";

  return event_reading_types[val];
}


cIpmiSensorInfo::cIpmiSensorInfo( cIpmiMc *mc )
  : m_mc( mc ), m_destroyed( false ), 
    m_sensor_count( 0 )
{
  for( int i = 0; i < 5; i++ )
     {
       m_sensors_by_idx[i] = NULL;
       m_idx_size[i] = 0;
     }
}


cIpmiSensorInfo::~cIpmiSensorInfo()
{
  for( int i = 0; i <= 4; i++ )
     {
       for( int j = 0; j < m_idx_size[i]; j++ )
	    if ( m_sensors_by_idx[i][j] )
                 m_sensors_by_idx[i][j]->Destroy();

       if ( m_sensors_by_idx[i] )
	    delete [] m_sensors_by_idx[i];
     }
}


cIpmiSensor *
cIpmiSensorInfo::FindSensor( unsigned int lun, unsigned int sensor_num )
{
  if ( lun > 4 )
     {
       assert( 0 );
       return 0;
     }

  if ( sensor_num > (unsigned int)m_idx_size[lun] )
       return 0;

  return m_sensors_by_idx[lun][sensor_num];
}


cIpmiSensor::cIpmiSensor( cIpmiMc *mc )
  : m_mc( mc ), m_source_mc( 0 ),
    m_source_idx( 0 ), m_source_array( 0 ),
    m_event_state( 0 ), m_destroyed( false ),
    m_use_count( 0 ),  
    m_owner( 0 ), m_channel( 0 ),
    m_lun( 0 ), m_num( 0 ),
    m_entity_id( eIpmiEntityInvalid ), m_entity_instance( 0 ),
    m_entity_instance_logical( false ),
    m_sensor_init_scanning( false ),
    m_sensor_init_events( false ),
    m_sensor_init_type( false ),
    m_sensor_init_pu_events( false ),
    m_sensor_init_pu_scanning( false ),
    m_ignore_if_no_entity( false ),
    m_supports_auto_rearm( false ),
    m_event_support( eIpmiEventSupportPerState ),
    m_sensor_type( eIpmiSensorTypeInvalid ),
    m_event_reading_type( eIpmiEventReadingTypeInvalid ),
    m_oem( 0 ),
    m_sensor_type_string( 0 ),
    m_event_reading_type_string( 0 ),
    m_rate_unit_string( 0 ),
    m_base_unit_string( 0 ),
    m_modifier_unit_string( 0 )
{
  m_id[0] = 0;
}


cIpmiSensor::~cIpmiSensor()
{
}


bool
cIpmiSensor::GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr )
{
  m_source_mc = mc;

  m_use_count = 1;  
  m_destroyed = false;

  m_mc = mc->Domain()->FindOrCreateMcBySlaveAddr( sdr->m_data[5] );

  if ( !m_mc )
     {
       stdlog << "cannot create MC for sensor !\n";

       return false;
     }

  m_source_mc = mc;
  m_owner                   = sdr->m_data[5];
  m_channel                 = sdr->m_data[6] >> 4;
  m_lun                     = sdr->m_data[6] & 0x03;
  m_num                     = sdr->m_data[7];
  m_entity_id               = (tIpmiEntityId)sdr->m_data[8];
  m_entity_instance_logical = sdr->m_data[9] >> 7;
  m_entity_instance         = sdr->m_data[9] & 0x7f;
  m_sensor_init_scanning    = (sdr->m_data[10] >> 6) & 1;
  m_sensor_init_events      = (sdr->m_data[10] >> 5) & 1;
  m_sensor_init_type        = (sdr->m_data[10] >> 2) & 1;
  m_sensor_init_pu_events   = (sdr->m_data[10] >> 1) & 1;
  m_sensor_init_pu_scanning = (sdr->m_data[10] >> 0) & 1;
  m_ignore_if_no_entity     = (sdr->m_data[11] >> 7) & 1;
  m_supports_auto_rearm     = (sdr->m_data[11] >> 6) & 1;
  m_event_support           = (tIpmiEventSupport)(sdr->m_data[11] & 3);
  m_sensor_type             = (tIpmiSensorType)sdr->m_data[12];
  m_event_reading_type      = (tIpmiEventReadingType)sdr->m_data[13];

  m_oem                     = sdr->m_data[46];

  IpmiGetDeviceString( sdr->m_data+47, sdr->m_length-47, m_id,
                       dSensorIdLen );

  return true;  
}


void
cIpmiSensor::HandleNew( cIpmiDomain *domain )
{
  m_sensor_type_string        = IpmiSensorTypeToString( m_sensor_type );
  m_event_reading_type_string = IpmiEventReadingTypeToString( m_event_reading_type );

  cIpmiEntityInfo &ents = domain->Entities();
  cIpmiEntity *ent = ents.Find( m_mc, m_entity_id, m_entity_instance );

  ent->AddSensor( this );
}


bool
cIpmiSensor::Cmp( const cIpmiSensor &s2 ) const
{
  if ( m_entity_instance_logical != s2.m_entity_instance_logical )
       return false;

  if ( m_sensor_init_scanning    != s2.m_sensor_init_scanning )
       return false;

  if ( m_sensor_init_events      != s2.m_sensor_init_events )
       return false;

  if ( m_sensor_init_type        != s2.m_sensor_init_type )
       return false;

  if ( m_sensor_init_pu_events   != s2.m_sensor_init_pu_events )
       return false;

  if ( m_sensor_init_pu_scanning != s2.m_sensor_init_pu_scanning )
       return false;

  if ( m_ignore_if_no_entity     != s2.m_ignore_if_no_entity )
       return false;

  if ( m_supports_auto_rearm     != s2.m_supports_auto_rearm )
       return false;

  if ( m_event_support           != s2.m_event_support )
       return false;

  if ( m_sensor_type             != s2.m_sensor_type )
       return false;

  if ( m_event_reading_type      != s2.m_event_reading_type )
       return false;

  if ( m_oem != s2.m_oem )
       return false;

  if ( strcmp( m_id, s2.m_id ) != 0 )
       return false;

  return true;
}


void
cIpmiSensor::FinalDestroy()
{
  cIpmiDomain     *domain = m_mc->Domain();
  cIpmiEntityInfo &ents   = domain->Entities();
  cIpmiSensorInfo *sensors = m_mc->Sensors();

  assert( sensors->m_sensors_by_idx[m_lun][m_num] == this );

  if ( m_source_array )
       m_source_array[m_source_idx] = 0;

  sensors->m_sensor_count--;
  sensors->m_sensors_by_idx[m_lun][m_num] = 0;

  cIpmiEntity *ent = ents.Find( m_mc, m_entity_id, m_entity_instance );

  if ( ent )
       ent->RemoveSensor( this );

  delete this;
}


bool
cIpmiSensor::Destroy()
{
  cIpmiSensorInfo *sensors = m_mc->Sensors();

  if ( sensors->m_sensors_by_idx[m_lun][m_num] != this )
     {
       assert( 0 );
       return false;
     }

  FinalDestroy();

  return true;
}


void
cIpmiSensor::GetId( char *id, int length )
{
  strncpy( id, m_id, length );
  id[length] = '\0';
}


cIpmiEntity *
cIpmiSensor::GetEntity()
{
  cIpmiDomain *domain = m_mc->Domain();

  return domain->Entities().Find( m_mc, m_entity_id,
                                  m_entity_instance );
}


bool
cIpmiSensor::Ignore()
{
  // not ipmlemented 
  return false;
}


void 
cIpmiSensor::Log()
{
/*
  cIpmiEntity *ent = GetEntity();

  IpmiLog( "sensor: mc = 0x%02x, num 0x%02x, %d.%d (%s), %s\n",
           m_mc->GetAddress(), m_num,
           ent->EntityId(), ent->EntityInstance(),
           ent->EntityIdString(), m_id );

  IpmiLog( "\tevent_support %s\n",
           IpmiEventSupportToString( m_event_support ) );
*/
}


bool
cIpmiSensor::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  if (    !(resource.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
       || !(resource.ResourceCapabilities & SAHPI_CAPABILITY_SENSOR ) )
     {
       // update resource
       resource.ResourceCapabilities |= SAHPI_CAPABILITY_RDR|SAHPI_CAPABILITY_SENSOR;

       struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

       if ( !e )
          {
            stdlog << "out of space !\n";
            return false;
          }

       memset( e, 0, sizeof( struct oh_event ) );
       e->type              = oh_event::OH_ET_RESOURCE;
       e->u.res_event.entry = resource;

       m_mc->Domain()->AddHpiEvent( e );       
     }

  rdr.RdrType  = SAHPI_SENSOR_RDR;
  rdr.Entity   = resource.ResourceEntity;

  // sensor record
  SaHpiSensorRecT &rec = rdr.RdrTypeUnion.SensorRec;

  rec.Num       = Num();
  rec.Type      = (SaHpiSensorTypeT)SensorType();
  rec.Category  = (SaHpiEventCategoryT)EventReadingType();
  rec.EventCtrl = (SaHpiSensorEventCtrlT)EventSupport();
  rec.Ignore    = (SaHpiBoolT)IgnoreIfNoEntity();
  rec.Oem       = GetOem();

  // id string
  char	name[32];
  memset( name,'\0',32 );
  GetId( name, 31 );
  rdr.IdString.DataType = SAHPI_TL_TYPE_ASCII6;
  rdr.IdString.Language = SAHPI_LANG_ENGLISH;
  rdr.IdString.DataLength = strlen( name );

  memcpy( rdr.IdString.Data,name, strlen( name ) + 1 );

  return true;
}


SaErrorT
cIpmiSensor::GetSensorReading( cIpmiMsg &rsp )
{
  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdGetSensorReading );
  int      rv;

  msg.m_data_len = 1;
  msg.m_data[0]  = m_num;

  rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       stdlog << "IPMI error getting states: " << rv << ", " 
              << strerror( rv ) << " \n";

       return SA_ERR_HPI_ERROR;
     }

  if ( rsp.m_data[0] != 0 )
     {
       stdlog << "IPMI error getting reading: " << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_INVALID_SENSOR_CMD;
     }

  if ( rsp.m_data_len < 4 )
     {
       stdlog << "IPMI error getting reading: data to small "
              << rsp.m_data_len << " !\n";

       return SA_ERR_HPI_INVALID_DATA_FIELD;
     }

  return SA_OK;
}


SaErrorT
cIpmiSensor::GetEventEnables( SaHpiSensorEvtEnablesT &enables, cIpmiMsg &rsp )
{
  cIpmiMsg  msg( eIpmiNetfnSensorEvent, eIpmiCmdGetSensorEventEnable );
  msg.m_data_len = 1;
  msg.m_data[0]  = m_num;

  int rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       stdlog << "Error sending get event enables command: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if ( rsp.m_data[0] )
     {
       stdlog << "IPMI error getting sensor enables: " << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_INVALID_CMD;
    }

  memset( &enables, 0, sizeof( SaHpiSensorEvtEnablesT ) );

  enables.SensorStatus = rsp.m_data[1];

  return SA_OK;
}


SaErrorT
cIpmiSensor::SetEventEnables( const SaHpiSensorEvtEnablesT &enables,
                              cIpmiMsg &msg )
{
  msg.m_netfn = eIpmiNetfnSensorEvent;
  msg.m_cmd   = eIpmiCmdSetSensorEventEnable;

  msg.m_data[0] = m_num;
  msg.m_data[1] = enables.SensorStatus & SAHPI_SENSTAT_EVENTS_ENABLED;

  if ( m_event_support == eIpmiEventSupportEntireSensor )
     {
       // We can only turn on/off the entire sensor, just pass the
       // status to the sensor.
       msg.m_data_len = 2;
     }
  else
     {
       msg.m_data[1] |= (1<<4); // enable selected event messages
       msg.m_data_len = 6;
     }

  cIpmiMsg rsp;

  int r = m_mc->SendCommand( msg, rsp, m_lun );

  if ( r )
     {
       stdlog << "Disable sensor events fail: " << r << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if ( rsp.m_data[0] )
     {
       stdlog << "IPMI Disable sensor events fail: " << rsp.m_data[0] << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmiSensor::CreateEvent( cIpmiEvent *event, SaHpiEventT &h )
{
  memset( &h, 0, sizeof( SaHpiEventT ) );

  cIpmiEntity *ent = GetEntity();
  assert( ent );

  h.Source    = ent->m_resource_id;
  h.EventType = SAHPI_ET_SENSOR;
  h.Timestamp = (SaHpiTimeT)IpmiGetUint32( event->m_data );

  if ( h.Timestamp == 0 )
       h.Timestamp = SAHPI_TIME_UNSPECIFIED;
  else
       h.Timestamp *= 1000000000;

  // sensor event
  SaHpiSensorEventT &s = h.EventDataUnion.SensorEvent;
  s.SensorNum     = m_num;
  s.SensorType    = (SaHpiSensorTypeT)event->m_data[7];
  s.EventCategory = (SaHpiEventCategoryT)event->m_data[9] & 0x7f;

  return SA_OK;
}


void
cIpmiSensor::HandleEvent( cIpmiEvent *event )
{
  cIpmiEntity *ent = GetEntity();
  assert( ent );

  stdlog << "reading event.\n";

  oh_event *e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );
  if ( !e )
     {
       stdlog << "Out of space !\n";
       return;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type = oh_event::OH_ET_HPI;

  oh_hpi_event &hpi_event = e->u.hpi_event;

  hpi_event.parent = ent->m_resource_id;
  hpi_event.id     = m_record_id;

  // hpi event
  SaHpiEventT &he = hpi_event.event;

  int rv = CreateEvent( event, he );

  if ( rv != SA_OK )
       return;

  m_mc->Domain()->AddHpiEvent( e );
}
