/*
 * ipmi_fru_info.cpp
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

#include "ipmi_fru_info.h"


cIpmiFruInfo::cIpmiFruInfo( unsigned int addr, unsigned int fru_id,
			    unsigned int slot, SaHpiEntityTypeT entity,
			    tIpmiAtcaSiteType site )
  : m_addr( addr ), m_fru_id( fru_id ),
    m_slot( slot ), m_entity( entity ),
    m_site( site )
{
  assert( fru_id == 0 );
}


cIpmiFruInfo::~cIpmiFruInfo()
{
}


cIpmiEntityPath 
cIpmiFruInfo::CreateEntityPath( const cIpmiEntityPath &top,
				const cIpmiEntityPath &bottom )
{
  cIpmiEntityPath middle;
  middle.m_entity_path.Entry[0].EntityType     = m_entity;
  middle.m_entity_path.Entry[0].EntityInstance = m_slot;

  cIpmiEntityPath ep = bottom;
  ep += middle;
  ep += top;
  ep.AppendRoot();

  return ep;
}


cIpmiFruInfoContainer::cIpmiFruInfoContainer()
  : m_fru_info( 0 )
{
}


cIpmiFruInfoContainer::~cIpmiFruInfoContainer()
{
  while( m_fru_info )
     {
       cIpmiFruInfo *fi = (cIpmiFruInfo *)m_fru_info->data;
       m_fru_info = g_list_remove( m_fru_info, fi );
       delete fi;
     }
}


cIpmiFruInfo *
cIpmiFruInfoContainer::FindFruInfo( unsigned int addr, unsigned int fru_id )
{
  GList *list = m_fru_info;

  while( list )
     {
       cIpmiFruInfo *fi = (cIpmiFruInfo *)m_fru_info->data;
       
       if ( fi->Address() == addr 
	    && fi->FruId() == fru_id )
	    return fi;

       list = g_list_next( list );
     }

  return 0;
}


bool
cIpmiFruInfoContainer::AddFruInfo( cIpmiFruInfo *fru_info )
{
  if ( FindFruInfo( fru_info->Address(), fru_info->FruId() ) )
     {
       assert( 0 );
       return false;
     }

  m_fru_info = g_list_append( m_fru_info, fru_info );

  return true;
}


bool
cIpmiFruInfoContainer::RemFruInfo( cIpmiFruInfo *fru_info )
{
  GList *list = m_fru_info;

  while( list )
     {
       cIpmiFruInfo *fi = (cIpmiFruInfo *)m_fru_info->data;

       if ( fi == fru_info )
	  {
	    m_fru_info = g_list_remove( m_fru_info, fru_info );
	    delete fru_info;

	    return true;
	  }

       list = g_list_next( list );
     }

  assert( 0 );

  return false;
}


cIpmiFruInfo *
cIpmiFruInfoContainer::NewFruInfo( unsigned int addr, unsigned int fru_id,
				   unsigned int slot, SaHpiEntityTypeT entity )
{
  cIpmiFruInfo *fi = new cIpmiFruInfo( addr, fru_id, slot, entity );
  
  if ( AddFruInfo( fi ) )
     {
       assert( 0 );
       delete fi;

       return 0;
     }

  return fi;
}
