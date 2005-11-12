/*
 * ipmi_resource.cpp
 *
 * Copyright (c) 2004 by FORCE Computers.
 * Copyright (c) 2005 by ESO Technologies.
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
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 */

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include "ipmi_domain.h"


cIpmiResource::cIpmiResource( cIpmiMc *mc, unsigned int fru_id )
  : m_sel( false ), m_mc( mc ), m_fru_id( fru_id ),
    m_is_fru( false ),
    m_hotswap_sensor( 0 ),
    m_fru_state( eIpmiFruStateNotInstalled ),
    m_oem( 0 ), m_current_control_id( 0 ),
    m_populate( false )
{
  for( int i = 0; i < 256; i++ )
       m_sensor_num[i] = -1;
}


cIpmiResource::~cIpmiResource()
{
}


cIpmiDomain *
cIpmiResource::Domain() const
{
  return m_mc->Domain();
}


SaErrorT
cIpmiResource::SendCommand( const cIpmiMsg &msg, cIpmiMsg &rsp,
			    unsigned int lun, int retries )
{
  return m_mc->SendCommand( msg, rsp, lun, retries );
}


SaErrorT
cIpmiResource::SendCommandReadLock( const cIpmiMsg &msg, cIpmiMsg &rsp,
                                    unsigned int lun, int retries )
{
  cIpmiResource *resource = this;
  cIpmiDomain *domain = Domain();
  domain->ReadUnlock();

  SaErrorT rv = SendCommand( msg, rsp, lun, retries );

  domain->ReadLock();

  if ( domain->VerifyResource( resource ) == false )
       return SA_ERR_HPI_NOT_PRESENT;

  return rv;
}


SaErrorT
cIpmiResource::SendCommandReadLock( cIpmiRdr *rdr, const cIpmiMsg &msg, cIpmiMsg &rsp,
                                    unsigned int lun, int retries )
{
  cIpmiDomain *domain = Domain();
  domain->ReadUnlock();
  
  SaErrorT rv = SendCommand( msg, rsp, lun, retries );
  
  domain->ReadLock();
  
  if ( domain->VerifyRdr( rdr ) == false )
       return SA_ERR_HPI_NOT_PRESENT;

  return rv;
}


int
cIpmiResource::CreateSensorNum( SaHpiSensorNumT num )
{
  int v = num;

  if ( m_sensor_num[v] != -1 )
     {
       for( int i = 0xff; i >= 0; i-- )
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

  memset( &info, 0, sizeof( SaHpiResourceInfoT ) );

  entry.ResourceEntity = m_entity_path;
  entry.ResourceId     = oh_uid_from_entity_path( &entry.ResourceEntity );

  entry.ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE;
  if ( m_is_fru == true )
    {
        entry.ResourceCapabilities |= SAHPI_CAPABILITY_FRU;

        if ( m_fru_id == 0 )
            {
                info.ResourceRev      = (SaHpiUint8T)m_mc->DeviceRevision();
                info.DeviceSupport    = (SaHpiUint8T)m_mc->DeviceSupport();
                info.ManufacturerId   = (SaHpiManufacturerIdT)m_mc->ManufacturerId();
                info.ProductId        = (SaHpiUint16T)m_mc->ProductId();
                info.FirmwareMajorRev = (SaHpiUint8T)m_mc->MajorFwRevision();
                info.FirmwareMinorRev = (SaHpiUint8T)m_mc->MinorFwRevision();
                info.AuxFirmwareRev   = (SaHpiUint8T)m_mc->AuxFwRevision( 0 );
            }

        // Reset supported on ATCA FRUs - Don't allow it on the active ShMC
        if ( Domain()->IsAtca()
            && ( m_mc->GetAddress() != dIpmiBmcSlaveAddr) )
            {
                entry.ResourceCapabilities |= SAHPI_CAPABILITY_RESET;
            }
    }

  entry.HotSwapCapabilities = 0;
  entry.ResourceSeverity = SAHPI_OK;
  entry.ResourceFailed = SAHPI_FALSE;
  entry.ResourceTag = ResourceTag();

  return true;
}


bool
cIpmiResource::Destroy()
{
  SaHpiRptEntryT *rptentry;
  stdlog << "removing resource: " << m_entity_path << ").\n";

  // remove sensors
  while( Num() )
     {
       cIpmiRdr *rdr = GetRdr( 0 );
       RemRdr( rdr );
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
  e->type = OH_ET_RESOURCE_DEL;
  rptentry = oh_get_resource_by_id( Domain()->GetHandler()->rptcache, m_resource_id );
  if ( !rptentry )
  {
      stdlog << "Can't find resource in plugin cache !\n";
      g_free( e );
      return false;
  }

  e->u.res_event.entry = *rptentry;
  stdlog << "cIpmiResource::Destroy OH_ET_RESOURCE_DEL Event resource " << m_resource_id << "\n";
  Domain()->AddHpiEvent( e );

  // remove resource from local cache
  int rv = oh_remove_resource( Domain()->GetHandler()->rptcache, m_resource_id );

  if ( rv != 0 )
  {
      stdlog << "Can't remove resource from plugin cache !\n";
      return false;
  }

  m_mc->RemResource( this );

  delete this;

  return true;
}


cIpmiRdr *
cIpmiResource::FindRdr( cIpmiMc *mc, SaHpiRdrTypeT type,
			unsigned int num, unsigned int lun )
{
  for( int i = 0; i < NumRdr(); i++ )
     {
       cIpmiRdr *r = GetRdr( i );

       if (    r->Mc()   == mc 
            && r->Type() == type
            && r->Num()  == num
            && r->Lun()  == lun )
	    return r;
     }

  return 0;
}


bool
cIpmiResource::AddRdr( cIpmiRdr *rdr )
{
  stdlog << "adding rdr: " << rdr->EntityPath();
  stdlog << " " << rdr->Num();
  stdlog << " " << rdr->IdString() << "\n";

  // set resource
  rdr->Resource() = this;

  // add rdr to resource
  Add( rdr );

  // check for hotswap sensor
  cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( rdr );

  if ( hs )
     {
       if ( m_hotswap_sensor )
           stdlog << "WARNING: found a second hotswap sensor, discard it !\n";
       else
           m_hotswap_sensor = hs;
     }

  return true;
}


bool
cIpmiResource::RemRdr( cIpmiRdr *rdr )
{
  int idx = Find( rdr );
  
  if ( idx == -1 )
     {
       stdlog << "user requested removal of a control"
                " from a resource, but the control was not there !\n";
       return false;
     }

  if ( rdr == m_hotswap_sensor )
       m_hotswap_sensor = 0;

  Rem( idx );

  return true;
}


bool
cIpmiResource::PopulateSel()
{
   // find resource
  SaHpiRptEntryT *resource = Domain()->FindResource( m_resource_id );

  if ( !resource )
     {
       stdlog << "Can't find resource !\n";
       return false;
     }

  if ( resource->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG )
     {
       stdlog << "EventLog capabilities already set !\n";
       return false;
     }

  // update resource
  resource->ResourceCapabilities |= SAHPI_CAPABILITY_EVENT_LOG;

  struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return true;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type               = OH_ET_RESOURCE;
  e->u.res_event.entry = *resource;

  stdlog << "cIpmiInventory::CreateRdr OH_ET_RESOURCE Event resource " << resource->ResourceId << "\n";
  Domain()->AddHpiEvent( e );

  return true;
}


bool
cIpmiResource::Populate()
{
  if ( m_populate == false )
     {
       // create rpt entry
       stdlog << "populate resource: " << EntityPath() << ".\n";

       struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

       if ( !e )
          {
            stdlog << "out of space !\n";
            return false;
          }

       memset( e, 0, sizeof( struct oh_event ) );
       e->type = OH_ET_RESOURCE;

       if ( Create( e->u.res_event.entry ) == false )
          {
            g_free( e );
            return false;
          }

       // assign the hpi resource id to ent, so we can find
       // the resource for a given entity
       m_resource_id = e->u.res_event.entry.ResourceId;

       // add the resource to the resource cache
       int rv = oh_add_resource( Domain()->GetHandler()->rptcache,
                                 &(e->u.res_event.entry), this, 1 );

       if ( rv != 0 )
       {
            stdlog << "Can't add resource to plugin cache !\n";
            g_free( e );
            return false;
       }

       stdlog << "cIpmiResource::Populate OH_ET_RESOURCE Event resource " << m_resource_id << "\n";
       Domain()->AddHpiEvent( e );
  
       if ( m_sel )
            PopulateSel();

       m_populate = true;
     }

  for( int i = 0; i < NumRdr(); i++ )
     {
       cIpmiRdr *rdr = GetRdr( i );

       if ( rdr->Populate() == false )
	    return false;
     }

  return true;
}
