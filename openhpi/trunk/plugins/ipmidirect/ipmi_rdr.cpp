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


cIpmiRdr::cIpmiRdr( cIpmiMc *mc, SaHpiRdrTypeT type )
  : m_mc( mc ), m_entity( 0 ), m_type( type )
{
}


cIpmiRdr::~cIpmiRdr()
{
}


bool
cIpmiRdr::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  rdr.RecordId = m_record_id;
  rdr.RdrType  = m_type;
  rdr.Entity   = resource.ResourceEntity;
  rdr.IdString = m_id_string;

  return true;
}


cIpmiRdrContainer::cIpmiRdrContainer()
  : m_rdrs( 0 )
{
}


cIpmiRdrContainer::~cIpmiRdrContainer()
{
  assert( m_rdrs == 0 );
}

  
cIpmiRdr *
cIpmiRdrContainer::Find( cIpmiMc *mc, SaHpiRdrTypeT type, unsigned int num )
{
  for( GList *list = m_rdrs; list; list = g_list_next( list ) )
     {
       cIpmiRdr *r = (cIpmiRdr *)list->data;
       
       if (    r->Mc()   == mc 
            && r->Type() == type
            && r->Num()  == num )
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
