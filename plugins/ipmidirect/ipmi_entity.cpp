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
       
       if ( ent->Find( s ) )
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

       if ( ent->Find( c ) )
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

       if ( ent->Find( f ) )
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
    m_hotswap_sensor( 0 ),
    m_current_control_id( 0 ),
    m_sel( 0 )
{
  m_id[0] = 0;

  m_entity_id_string = IpmiEntityIdToString( entity_id );
}


cIpmiEntity::~cIpmiEntity()
{
  if ( m_sel )
     {
       // remove connection
       m_sel->Entity() = 0;
       m_sel = 0;
     }
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


bool
cIpmiEntity::Add( cIpmiRdr *rdr )
{
  stdlog << "adding rdr ";
  //control->Log();
  stdlog << "\n";

  char id[80] = "";
  rdr->IdString().GetAscii( id, 80 );

  dbg( "adding rdr %d.%d (%s) %02x: %s",
       EntityId(), EntityInstance(),
       EntityIdString(), rdr->Num(), id );

  // set entity
  rdr->Entity() = this;

  // add rdr to entity
  if ( cIpmiRdrContainer::Add( rdr ) == false )
     {
       assert( 0 );
       return false;
     }

  // this is for testing, because at the moment
  // rdrs cannot exits without an mc
  assert( rdr->Mc() );

  if ( rdr->Mc() )
     {
       // add rdr to mc
       if ( rdr->Mc()->Add( rdr ) == false )
            return false;
     }

  // find resource
  SaHpiRptEntryT *resource = m_domain->FindResource( m_resource_id );

  if ( !resource )
     {
       assert( 0 );
       return false;
     }

  // create event
  struct oh_event *e;

  e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "Out of space !\n";
       return false;
     }

  memset( e, 0, sizeof( struct oh_event ) );

  e->type = oh_event::OH_ET_RDR;

  // create rdr
  rdr->CreateRdr( *resource, e->u.rdr_event.rdr );

  int rv = oh_add_rdr( m_domain->GetHandler()->rptcache,
                       resource->ResourceId,
                       &e->u.rdr_event.rdr, rdr, 1 );

  assert( rv == 0 );

  // assign the hpi record id to sensor, so we can find
  // the rdr for a given sensor.
  // the id comes from oh_add_rdr.
  rdr->RecordId() = e->u.rdr_event.rdr.RecordId;

  m_domain->AddHpiEvent( e );

  // check for hotswap sensor
  cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( rdr );

  if ( hs )
     {
       if ( m_hotswap_sensor )
            stdlog << "ups: found a second hotswap sensor !\n";

       m_hotswap_sensor = hs;
     }

  return true;
}


bool
cIpmiEntity::Rem( cIpmiRdr *rdr )
{
  if ( Find( rdr ) == false )
     {
       stdlog << "user requested removal of a control"
                " from an entity, but the control was not there !\n";
       return false;
     }

  if ( rdr == m_hotswap_sensor )
       m_hotswap_sensor = 0;

  // this is for testing, because at the moment
  // rdrs cannot exits without an mc
  assert( rdr->Mc() );

  if ( rdr->Mc() )
       rdr->Mc()->Rem( rdr );

  cIpmiRdrContainer::Rem( rdr );

  if ( m_rdrs == 0 )
       Destroy();

  return true;
}
