/*
 * ipmi_rdr.cpp
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

#include "ipmi_rdr.h"
#include "ipmi_mc.h"
#include "ipmi_entity.h"
#include "ipmi_resource.h"
#include "ipmi_domain.h"


cIpmiRdr::cIpmiRdr( cIpmiMc *mc, SaHpiRdrTypeT type )
  : m_mc( mc ), m_resource( 0 ), m_type( type ),
    m_lun( 0 ), m_populate( false )
{
}


cIpmiRdr::~cIpmiRdr()
{
}


cIpmiDomain *
cIpmiRdr::Domain()
{
  return m_mc->Domain();
}


bool
cIpmiRdr::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  rdr.RecordId = m_record_id;
  rdr.RdrType  = m_type;
  rdr.Entity   = m_entity_path;
  rdr.IdString = m_id_string;
  //rdr.RecordId = oh_uid_from_entity_path( &rdr.Entity );

  return true;
}


SaErrorT
cIpmiRdr::SendCommand( const cIpmiMsg &msg, cIpmiMsg &rsp,
		       unsigned int lun, int retries )
{
  return m_mc->SendCommand( msg, rsp, lun, retries );
}


bool
cIpmiRdr::Populate()
{
  if ( m_populate )
       return true;

  // find resource
  SaHpiRptEntryT *resource = Domain()->FindResource( Resource()->m_resource_id );

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
  CreateRdr( *resource, e->u.rdr_event.rdr );

  int rv = oh_add_rdr( Domain()->GetHandler()->rptcache,
                       resource->ResourceId,
                       &e->u.rdr_event.rdr, this, 1 );

  assert( rv == 0 );

  // assign the hpi record id to sensor, so we can find
  // the rdr for a given sensor.
  // the id comes from oh_add_rdr.
  RecordId() = e->u.rdr_event.rdr.RecordId;

  Domain()->AddHpiEvent( e );

  m_populate = true;

  return true;
}


/*
cIpmiRdrContainer::cIpmiRdrContainer()
  : m_rdrs( 0 )
{
}


cIpmiRdrContainer::~cIpmiRdrContainer()
{
  assert( m_rdrs == 0 );
}


GList *
cIpmiRdrContainer::GetRdrList( cIpmiMc *mc, SaHpiRdrTypeT type )
{
  GList *list = 0;

  for( GList *l = m_rdrs; l; l = g_list_next( l ) )
     {
       cIpmiRdr *r = (cIpmiRdr *)l->data;

       if (    r->Mc()   == mc 
            && r->Type() == type )
	    list = g_list_append( list, r );
     }

  return list;
}


cIpmiRdr *
cIpmiRdrContainer::Find( cIpmiMc *mc, SaHpiRdrTypeT type,
                         unsigned int num, unsigned int lun )
{
  for( GList *list = m_rdrs; list; list = g_list_next( list ) )
     {
       cIpmiRdr *r = (cIpmiRdr *)list->data;

       if (    r->Mc()   == mc 
            && r->Type() == type
            && r->Num()  == num
            && r->Lun()  == lun )
	    return r;
     }

  return 0;
}


bool
cIpmiRdrContainer::Find( cIpmiRdr *rdr )
{
  for( GList *list = m_rdrs; list; list = g_list_next( list ) )
     {
       cIpmiRdr *r = (cIpmiRdr *)list->data;
       
       if ( r == rdr )
	    return true;
     }
  
  return false;
}


bool
cIpmiRdrContainer::Add( cIpmiRdr *rdr )
{
  assert( Find( rdr ) == false );

  m_rdrs = g_list_append( m_rdrs, rdr );

  return true;
}


bool
cIpmiRdrContainer::Rem( cIpmiRdr *rdr )
{
  assert( Find( rdr ) );

  m_rdrs = g_list_remove( m_rdrs, rdr );

  return true;
}
*/
