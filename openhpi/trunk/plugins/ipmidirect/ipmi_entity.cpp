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


cIpmiEntity::cIpmiEntity( cIpmiDomain *domain, tIpmiDeviceNum device_num,
                          tIpmiEntityId entity_id, unsigned int entity_instance,
                          bool came_from_sdr )
  : m_domain( domain ),
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
    m_presence_possibly_changed( true ),
    m_hotswap_sensor( 0 ),
    m_current_control_id( 0 ),
    m_sel( 0 )
{
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
  stdlog << "removing entity: " << EntityId() << "." << EntityInstance() 
	 << " (" << IpmiEntityIdToString( EntityId() ) << ").\n";

  // remove resource from local cache
  int rv = oh_remove_resource( Domain()->GetHandler()->rptcache, m_resource_id );
  assert( rv == 0 );

  // create remove event
  oh_event *e = (oh_event *)g_malloc0( sizeof( oh_event ) );

  if ( !e )
     {
       stdlog << "Out of space !\n";
       return false;
     }

  memset( e, 0, sizeof( struct oh_event ));
  e->type = oh_event::OH_ET_RESOURCE_DEL;
  e->u.res_event.entry.ResourceId = m_resource_id;
  Domain()->AddHpiEvent( e );

  // Remove it from the entities list.
  Domain()->RemEntity( this );

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


cIpmiEntityPath::cIpmiEntityPath()
{
  memset( &m_entity_path, 0, sizeof( SaHpiEntityPathT ) );
}


cIpmiEntityPath::cIpmiEntityPath( const SaHpiEntityPathT &entity_path )
{
  m_entity_path = entity_path;
}


cIpmiLog &
operator<<( cIpmiLog &log, const cIpmiEntityPath &epath )
{
  char str[1024] = "{Invalid}";

  entitypath2string( &epath.m_entity_path, str, 1024 );

  log << str;

  return log;
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
       IpmiEntityIdToString( EntityId() ), rdr->Num(), id );

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


bool
cIpmiEntity::CreateResource( SaHpiRptEntryT &entry )
{
  cIpmiAddr addr( eIpmiAddrTypeIpmb, Channel(), 
                  Lun(), AccessAddress() );

  cIpmiMc *mc = Domain()->FindMcByAddr( addr );

  if ( !mc )
     {
       addr.Si();
       mc = Domain()->FindMcByAddr( addr );
     }

  assert( mc );

  entry.EntryId = 0;

  // resource info
  SaHpiResourceInfoT &info = entry.ResourceInfo;

  info.ResourceRev      = 0;
  info.SpecificVer      = 0;
  info.DeviceSupport    = 0;
  info.ManufacturerId   = (SaHpiManufacturerIdT)mc->ManufacturerId();
  info.ProductId        = (SaHpiUint16T)mc->ProductId();
  info.FirmwareMajorRev = (SaHpiUint8T)mc->MajorFwRevision();
  info.FirmwareMinorRev = (SaHpiUint8T)mc->MinorFwRevision();
  info.AuxFirmwareRev   = (SaHpiUint8T)mc->AuxFwRevision( 0 );

  entry.ResourceEntity.Entry[0].EntityType     = (SaHpiEntityTypeT)EntityId();
  entry.ResourceEntity.Entry[0].EntityInstance = (SaHpiEntityInstanceT)EntityInstance();
  entry.ResourceEntity.Entry[1].EntityType     = SAHPI_ENT_UNSPECIFIED;
  entry.ResourceEntity.Entry[1].EntityInstance = 0;

  // let's append entity_root from config
  const char *entity_root = Domain()->EntityRoot();
  dbg( "entity_root: %s", entity_root );
  SaHpiEntityPathT entity_ep;
  string2entitypath( entity_root, &entity_ep );
  append_root( &entity_ep );

  ep_concat( &entry.ResourceEntity, &entity_ep );

  entry.ResourceId = oh_uid_from_entity_path( &entry.ResourceEntity );

  // TODO: set SAHPI_CAPABILITY_FRU only if FRU
  entry.ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE|SAHPI_CAPABILITY_FRU;
  entry.ResourceSeverity = SAHPI_OK;
  entry.DomainId = 0;
  entry.ResourceTag = IdString();

  return true;
}
