/*
 * ipmi_resource.cpp
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

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include "ipmi_event.h"
#include "ipmi_log.h"
#include "ipmi_domain.h"
#include "ipmi_resource.h"


cIpmiResource::cIpmiResource( cIpmiMc *mc, unsigned int fru_id )
  : m_mc( mc ), m_fru_id( fru_id ),
    m_hotswap_sensor( 0 ),
    m_fru_state( eIpmiFruStateNotInstalled ),
    m_oem( 0 )
{
  for( int i = 0; i < 256; i++ )
       m_sensor_num[i] = -1;
}


cIpmiResource::~cIpmiResource()
{
}


int
cIpmiResource::CreateSensorNum( SaHpiSensorNumT num )
{
  int v = num;

  if ( m_sensor_num[v] != -1 )
     {
       for( int i = 255; i >= 0; i-- )
            if ( m_sensor_num[i] == -1 )
               {
                 v = i;
                 break;
               }

       if ( m_sensor_num[v] != -1 )
          {
            assert( 0 );
            return -1;
          }
     }

  m_sensor_num[v] = num;

  return v;
}


bool
cIpmiResource::Create( SaHpiRptEntryT &entry )
{
  stdlog << "add resource: " << m_entity_path << ".\n";

  entry.EntryId = 0;

  // resource info
  SaHpiResourceInfoT &info = entry.ResourceInfo;

  info.ResourceRev      = 0;
  info.SpecificVer      = 0;
  info.DeviceSupport    = 0;
  info.ManufacturerId   = (SaHpiManufacturerIdT)m_mc->ManufacturerId();
  info.ProductId        = (SaHpiUint16T)m_mc->ProductId();
  info.FirmwareMajorRev = (SaHpiUint8T)m_mc->MajorFwRevision();
  info.FirmwareMinorRev = (SaHpiUint8T)m_mc->MinorFwRevision();
  info.AuxFirmwareRev   = (SaHpiUint8T)m_mc->AuxFwRevision( 0 );

  entry.ResourceEntity = m_entity_path;
  entry.ResourceId     = oh_uid_from_entity_path( &entry.ResourceEntity );

  // TODO: set SAHPI_CAPABILITY_FRU only if FRU
  entry.ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE|SAHPI_CAPABILITY_FRU;
  entry.ResourceSeverity = SAHPI_OK;
  entry.DomainId = 0;
  entry.ResourceTag = ResourceTag();

  return true;
}


bool
cIpmiResource::Destroy()
{
  stdlog << "removing resource: " << m_entity_path << ").\n";

  // remove sensors
  while( m_rdrs )
     {
       cIpmiRdr *rdr = (cIpmiRdr *)m_rdrs->data;
       Rem( rdr );
       delete rdr;
     }

  // create remove event
  oh_event *e = (oh_event *)g_malloc0( sizeof( oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return false;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type = oh_event::OH_ET_RESOURCE_DEL;
  e->u.res_del_event.resource_id = m_resource_id;
  Domain()->AddHpiEvent( e );

  // remove resource from local cache
  int rv = oh_remove_resource( Domain()->GetHandler()->rptcache, m_resource_id );
  assert( rv == 0 );

  m_mc->RemResource( this );

  delete this;

  return true;
}


bool
cIpmiResource::Add( cIpmiRdr *rdr )
{
  stdlog << "adding rdr: " << rdr->EntityPath() << " " << rdr->Num() << " " << rdr->IdString() << "\n";

  // set entity
  rdr->Resource() = this;

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
  SaHpiRptEntryT *resource = Domain()->FindResource( m_resource_id );

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
       stdlog << "out of space !\n";
       return false;
     }

  memset( e, 0, sizeof( struct oh_event ) );

  e->type = oh_event::OH_ET_RDR;

  // create rdr
  rdr->CreateRdr( *resource, e->u.rdr_event.rdr );

  int rv = oh_add_rdr( Domain()->GetHandler()->rptcache,
                       resource->ResourceId,
                       &e->u.rdr_event.rdr, rdr, 1 );

  assert( rv == 0 );

  // assign the hpi record id to sensor, so we can find
  // the rdr for a given sensor.
  // the id comes from oh_add_rdr.
  rdr->RecordId() = e->u.rdr_event.rdr.RecordId;

  Domain()->AddHpiEvent( e );

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
cIpmiResource::Rem( cIpmiRdr *rdr )
{
  if ( Find( rdr ) == false )
     {
       stdlog << "user requested removal of a control"
                " from a resource, but the control was not there !\n";
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

  return true;
}
