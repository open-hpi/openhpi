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


SaHpiEntityTypeT
MapAtcaSiteTypeToEntity( tIpmiAtcaSiteType type )
{
  static SaHpiEntityTypeT et[] = 
  {
    SAHPI_ENT_SYSTEM_BOARD,
    SAHPI_ENT_POWER_UNIT,
    SAHPI_ENT_EXTERNAL_ENVIRONMENT,
    (SaHpiEntityTypeT)0xf0, // ATCA ShMc
    SAHPI_ENT_COOLING_UNIT,
    SAHPI_ENT_CHASSIS_SPECIFIC,
    (SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC+1),
    (SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC+2),
    (SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC+3),
    (SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC+4)
  };

  if ( type > eIpmiAtcaSiteTypeRearTransitionModule )
     {
       assert( 0 );
       return SAHPI_ENT_UNKNOWN;
     }
 
  return et[type];
}


cIpmiFruInfo::cIpmiFruInfo( unsigned int addr, unsigned int fru_id,
			    SaHpiEntityTypeT entity, unsigned int slot,
			    tIpmiAtcaSiteType site, unsigned int properties )
  : m_addr( addr ), m_fru_id( fru_id ),
    m_slot( slot ), m_entity( entity ),
    m_site( site ), m_properties( properties )
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
cIpmiFruInfoContainer::FindFruInfo( unsigned int addr, unsigned int fru_id ) const
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
				   SaHpiEntityTypeT entity, unsigned int slot,
                                   tIpmiAtcaSiteType site, unsigned int properties )
{
  cIpmiFruInfo *fi = FindFruInfo( addr, fru_id );

  if ( fi )
       return fi;

  fi = new cIpmiFruInfo( addr, fru_id, entity, slot, site, properties );

  if ( !AddFruInfo( fi ) )
     {
       assert( 0 );
       delete fi;

       return 0;
     }

  return fi;
}


unsigned int
cIpmiFruInfoContainer::GetFreeSlotForOther( unsigned int addr )
{
  unsigned int slot = 0;

  for( GList *list = m_fru_info; list; list = g_list_next( list ) )
     {
       cIpmiFruInfo *fi = (cIpmiFruInfo *)m_fru_info->data;

       if ( fi->Address() != addr || fi->Entity() )
            continue;

       if ( slot < fi->Slot() )
            slot = fi->Slot();
     }

  return slot + 1;
}
