/*
 *
 * Copyright (c) 2003 by FORCE Computers
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

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include "ipmi_mc.h"
#include "ipmi_entity.h"
#include "ipmi_event.h"
#include "ipmi_log.h"
#include "ipmi_domain.h"


cIpmiEntityInfo::cIpmiEntityInfo( cIpmiDomain *domain )
  : m_domain( domain ), m_entities( 0 )
{
}


cIpmiEntityInfo::~cIpmiEntityInfo()
{
  while( m_entities )
     {
       cIpmiEntity *ent = (cIpmiEntity *)m_entities->data;
     
       m_entities = g_list_remove( m_entities, ent );
       delete ent;
     }  
}


cIpmiEntity *
cIpmiEntityInfo::VerifyEntify( cIpmiEntity *ent )
{
  if ( g_list_find( m_entities, ent ) )
       return ent;

  return 0;
}


cIpmiSensor *
cIpmiEntityInfo::VerifySensor( cIpmiSensor *s )
{
  for( GList *list = m_entities; list; list = g_list_next( list ) )
     {
       cIpmiEntity *ent = (cIpmiEntity *)list->data;
       
       if ( ent->VerifySensor( s ) )
            return s;
     }

  return 0;
}


cIpmiControl *
cIpmiEntityInfo::VerifyControl( cIpmiControl *c )
{
  for( GList *list = m_entities; list; list = g_list_next( list ) )
     {
       cIpmiEntity *ent = (cIpmiEntity *)list->data;

       if ( ent->VerifyControl( c ) )
            return c;
     }

  return 0;
}


cIpmiFru *
cIpmiEntityInfo::VerifyFru( cIpmiFru *f )
{
  for( GList *list = m_entities; list; list = g_list_next( list ) )
     {
       cIpmiEntity *ent = (cIpmiEntity *)list->data;
       
       if ( ent->VerifyFru( f ) )
            return f;
     }

  return 0;
}


cIpmiEntity *
cIpmiEntityInfo::Find( tIpmiDeviceNum device_num,
                       tIpmiEntityId entity_id, int entity_instance )
{
  GList *list = m_entities;

  while( list )
     {
       cIpmiEntity *ent = (cIpmiEntity *)list->data;

       if (    ( ent->DeviceNum().channel == device_num.channel )
	    && ( ent->DeviceNum().address == device_num.address )
	    && ( ent->EntityId()          == entity_id )
            && ( ent->EntityInstance()    == entity_instance ) )
            return ent;

       list  = g_list_next( list );
     }
  
  return 0;
}


cIpmiEntity *
cIpmiEntityInfo::Find( cIpmiMc *mc,
                       tIpmiEntityId entity_id, int entity_instance )
{
  tIpmiDeviceNum device_num;

  if ( mc && entity_instance >= 0x60 )
     {
       device_num.channel = mc->GetChannel();
       device_num.address = mc->GetAddress();
       entity_instance   -= 0x60;
     }
  else
     {
       device_num.channel = 0;
       device_num.address = 0;
     }

  return Find( device_num, entity_id, entity_instance );
}


cIpmiEntity *
cIpmiEntityInfo::Add( tIpmiDeviceNum device_num,
                      tIpmiEntityId  entity_id, int entity_instance,
                      bool came_from_sdr, const char *id,
                      cIpmiMc *mc, int lun )
{
  cIpmiEntity *ent = Find( device_num, entity_id, entity_instance );

  if ( ent )
     {
       // If it came from an SDR, it always will have come from an SDR.
       if ( !ent->CameFromSdr() )
	    ent->CameFromSdr() = came_from_sdr;

       ent->SetId( id );
       ent->AccessAddress() = mc->GetAddress();
       ent->Channel()       = mc->GetChannel();
       ent->Lun()           = lun;

       return ent;
    }

  ent = new cIpmiEntity( this, device_num, entity_id, entity_instance, came_from_sdr );
  assert( ent );

  m_entities = g_list_append( m_entities, ent );

  ent->SetId( id );
  ent->AccessAddress() = mc->GetAddress();
  ent->Channel()       = mc->GetChannel();
  ent->Lun()           = lun;

  // XXXX add entity
  m_domain->IfEntityAdd( ent );

  return ent;
}


cIpmiEntity *
cIpmiEntityInfo::Add( cIpmiMc *mc, int lun, 
                      tIpmiEntityId entity_id, int entity_instance,
                      const char *id )
{
  tIpmiDeviceNum device_num;
  cIpmiEntity *ent;

  if ( mc && entity_instance >= 0x60 )
     {
       device_num.channel = mc->GetChannel();
       device_num.address = mc->GetAddress();
       entity_instance -= 0x60;
     }
  else
     {
       device_num.channel = 0;
       device_num.address = 0;
     }

  ent = Add( device_num, entity_id, entity_instance, false, id, mc, lun );

  if ( !ent )
       return 0;

  return ent;
}


void
cIpmiEntityInfo::Rem( cIpmiEntity *ent )
{
  m_entities = g_list_remove( m_entities, ent );
}


cIpmiEntity::cIpmiEntity( cIpmiEntityInfo *ents, tIpmiDeviceNum device_num,
                          tIpmiEntityId entity_id, int entity_instance,
                          bool came_from_sdr )
  : m_domain( ents->Domain() ),
    m_access_address( 0 ),
    m_slave_address( 0 ), 
    m_channel( 0 ),
    m_lun( 0 ),
    m_private_bus_id( 0 ),
    m_is_fru( false ), m_is_mc( false ),
    m_came_from_sdr( came_from_sdr ),
    m_is_logical_fru( false ), m_fru_id( 0 ),
    m_acpi_system_power_notify_required( false ),
    m_acpi_device_power_notify_required( false ),
    m_controller_logs_init_agent_errors( false ),
    m_log_init_agent_errors_accessing( false ),
    m_chassis_device( false ),
    m_bridge( false ),
    m_ipmb_event_generator( false ),
    m_ipmb_event_receiver( false ),
    m_fru_inventory_device( false ),
    m_sel_device( false ),
    m_sdr_repository_device( false ),
    m_sensor_device( false ),
    m_address_span( 0 ), m_device_num( device_num ),
    m_entity_id( entity_id ),
    m_entity_instance( entity_instance ),
    m_device_type( 0 ), m_device_modifier( 0 ),
    m_oem( 0 ), m_presence_sensor_always_there( false ),
    m_entity_id_string( 0 ),
    m_presence_possibly_changed( true ),
    m_ents( ents ),
    m_sensors( 0 ), m_hotswap_sensor( 0 ),
    m_current_control_id( 0 ),
    m_controls( 0 ),
    m_frus( 0 ), m_sel( 0 )
{
  m_id[0] = 0;

  m_entity_id_string = IpmiEntityIdToString( entity_id );
}


cIpmiEntity::~cIpmiEntity()
{
  while( m_sensors )
       m_sensors = g_list_remove( m_sensors, m_sensors->data );

  while( m_controls )
     {
       delete (cIpmiControl *)m_controls->data;
       m_controls = g_list_remove( m_controls, m_controls->data );
     }

  while( m_frus )
     {
       delete (cIpmiFru *)m_frus->data;
       m_frus = g_list_remove( m_sensors, m_frus->data );
     }

  if ( m_sel )
     {
       // remove connection
       m_sel->Entity() = 0;
       m_sel = 0;
     }
}


cIpmiSensor *
cIpmiEntity::VerifySensor( cIpmiSensor *s )
{
  for( GList *list = m_sensors; list; list = g_list_next( list ) )
     {
       cIpmiSensor *sensor = (cIpmiSensor *)list->data;

       if ( s == sensor )
            return s;
     }

  return 0;
}


cIpmiControl *
cIpmiEntity::VerifyControl( cIpmiControl *c )
{
  for( GList *list = m_controls; list; list = g_list_next( list ) )
     {
       cIpmiControl *control = (cIpmiControl *)list->data;

       if ( c == control )
            return c;
     }

  return 0;
}


cIpmiFru *
cIpmiEntity::VerifyFru( cIpmiFru *f )
{
  for( GList *list = m_frus; list; list = g_list_next( list ) )
     {
       cIpmiFru *fru = (cIpmiFru *)list->data;

       if ( f == fru )
            return f;
     }

  return 0;
}


bool
cIpmiEntity::Destroy()
{
  // XXXX remove ent
  m_domain->IfEntityRem( this );

  // Remove it from the entities list.
  m_ents->Rem( this );

  delete this;

  return true;
}


static const char *entity_id_types[] =
{
  "Unspecified",
  "Other",
  "Unkown",
  "Processor",
  "Disk",
  "Peripheral",
  "SystemManagementModule",
  "SystemBoard",
  "MemoryModule",
  "ProcessorModule",
  "PowerSupply",
  "AddInCard",
  "FrontPanelBoard",
  "BackPanelBoard",
  "PowerSystemBoard",
  "DriveBackplane",
  "SystemInternalExpansionBoard",
  "OtherSystemBoard",
  "ProcessorBoard",
  "PowerUnit",
  "PowerModule",
  "PowerManagementBoard",
  "ChassisBackPanelBoard",
  "SystemChassis",
  "SubChassis",
  "OtherChassisBoard",
  "DiskDriveBay",
  "PeripheralBay",
  "DeviceBay",
  "FanCooling",
  "CoolingUnit",
  "CableInterconnect",
  "MemoryDevice",
  "SystemManagementSoftware",
  "Bios",
  "OperatingSystem",
  "SystemBus",
  "Group",
  "RemoteMgmtCommDevice",
  "ExternalEnvironment",
  "Battery",
  "ProcessingBlade",
  "ConnectivitySwitch",
  "ProcessorMemoryModule",
  "IoModule",
  "ProcessorIoModule",
  "ManagementControllerFirmware",
};

#define dNumEntityIdTypes (sizeof(entity_id_types)/sizeof(char *))


const char *
IpmiEntityIdToString( tIpmiEntityId val )
{
  if ( (unsigned int)val < dNumEntityIdTypes )
       return entity_id_types[val];

  switch( val )
     {
       case eIpmiEntityIdPigMgFrontBoard:
            return "PigmigFrontBoard";

       case eIpmiEntityIdPigMgRearTransitionModule:
            return "PigmigRearTransitionModule";

       case eIpmiEntityIdAtcaShelfManager:
            return "AtcaShelfManager";
            
       case eIpmiEntityIdAtcaFiltrationUnit:
            return "AtcaFiltrationUnit";

       default:
            return "Invalid";
     }

  // not reached
  return "Invalid";
}


void
cIpmiEntity::AddSensor( cIpmiSensor *sensor )
{
  cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( sensor );
  
  if ( hs )
     {
       if ( m_hotswap_sensor )
            stdlog << "ups: found a second hotswap sensor !\n";

       m_hotswap_sensor = hs;
     }

  m_sensors = g_list_append( m_sensors, sensor );

  // XXXX add a rdr
  m_domain->IfSensorAdd( this, sensor );
}


void
cIpmiEntity::RemoveSensor( cIpmiSensor *sensor )
{
  GList *item = g_list_find( m_sensors, sensor );

  if ( item == 0 )
     {
       stdlog << "User requested removal of a sensor"
                " from an entity, but the sensor was not there !\n";
       return;
     }

  cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( sensor );

  if ( hs )
     {
       if ( hs != m_hotswap_sensor )
            stdlog << "ups: remove second hotswap sensor !\n";

       m_hotswap_sensor = 0;
     }
  
  m_sensors = g_list_remove( m_sensors, sensor );

  // XXXX remove rdr
  m_domain->IfSensorRem( this, sensor );

  if (    (!m_came_from_sdr )
       && m_sensors == 0 )
       Destroy();
}


void 
cIpmiEntity::AddControl( cIpmiControl *control )
{
  m_controls = g_list_append( m_controls, control );

  stdlog << "adding control ";
  control->Log();
  stdlog << "\n";

  dbg( "adding control %d.%d (%s) %02x: %s",
       EntityId(), EntityInstance(),
       EntityIdString(), control->Num(), control->Id() );

  // find resource
  SaHpiRptEntryT *resource = m_domain->FindResource( m_resource_id );

  if ( !resource )
     {
       assert( 0 );
       return;
     }

  // create event
  struct oh_event *e;

  e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "Out of space !\n";   
       return;
     }

  memset( e, 0, sizeof( struct oh_event ) );

  e->type = oh_event::OH_ET_RDR;

  // create rdr
  control->CreateRdr( *resource, e->u.rdr_event.rdr );

  int rv = oh_add_rdr( m_domain->GetHandler()->rptcache,
                       resource->ResourceId,
                       &e->u.rdr_event.rdr, control, 1 );

  assert( rv == 0 );

  // assign the hpi record id to sensor, so we can find
  // the rdr for a given sensor.
  // the id comes from oh_add_rdr.
  control->m_record_id = e->u.rdr_event.rdr.RecordId;

  m_domain->AddHpiEvent( e );
}


void
cIpmiEntity::RemoveControl( cIpmiControl *control )
{
  GList *item = g_list_find( m_controls, control );

  if ( item == 0 )
     {
       stdlog << "User requested removal of a control"
                " from an entity, but the control was not there";
       return;
     }

  m_controls = g_list_remove( m_controls, control );

  if (    (!m_came_from_sdr )
       && m_sensors == 0 )
       Destroy();
}


cIpmiFru *
cIpmiEntity::FindFru( unsigned int fru_id )
{
  GList *item = g_list_first( m_frus );

  while( item )
     {
       cIpmiFru *fru = (cIpmiFru *)item->data;

       if ( fru->FruId() == fru_id )
            return fru;

       item = g_list_next( item );
     }

  return 0;
}


void
cIpmiEntity::AddFru( cIpmiFru *fru )
{
  m_frus = g_list_append( m_frus, fru );
}
