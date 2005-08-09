/*
 * ipmi_rdr.cpp
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

  e->type               = OH_ET_RDR;
  e->u.rdr_event.parent = resource->ResourceId;

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

  stdlog << "cIpmiRdr::Populate OH_ET_RDR Event resource " << resource->ResourceId << " RDR " << RecordId() << "\n";
  Domain()->AddHpiEvent( e );

  m_populate = true;

  return true;
}

