/*
 * ipmi_control.cpp
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

#include "ipmi_control.h"
#include "ipmi_domain.h"


cIpmiControl::cIpmiControl( cIpmiMc *mc, cIpmiEntity *ent,
			    unsigned int num, const char *id,
                            SaHpiCtrlOutputTypeT output_type,
                            SaHpiCtrlTypeT type )
  : m_mc( mc ), m_entity( ent ),
    m_num( num ), m_ignore( false ),
    m_output_type( output_type ),
    m_type( type )
{
  memset( m_id, 0, dControlIdLen + 1 );
  strncpy( m_id, id, dControlIdLen );
}


cIpmiControl::~cIpmiControl()
{
}


void
cIpmiControl::Log()
{
  IpmiLog( "control: mc = 0x%02x, num 0x%02x, %d.%d (%s), %s\n",
           m_mc->GetAddress(), m_num,
           m_entity->EntityId(), m_entity->EntityInstance(),
           m_entity->EntityIdString(), m_id );
}


bool
cIpmiControl::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  if (    !(resource.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
       || !(resource.ResourceCapabilities & SAHPI_CAPABILITY_CONTROL ) )
     {
       // update resource
       resource.ResourceCapabilities |= SAHPI_CAPABILITY_RDR|SAHPI_CAPABILITY_CONTROL;

       struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

       if ( !e )
          {
            IpmiLog( "Out of space !\n" );
            return false;
          }

       memset( e, 0, sizeof( struct oh_event ) );
       e->type              = oh_event::OH_ET_RESOURCE;
       e->u.res_event.entry = resource;

       m_mc->Domain()->AddHpiEvent( e );       
     }

  rdr.RdrType  = SAHPI_CTRL_RDR;
  rdr.Entity   = resource.ResourceEntity;

  // control record
  SaHpiCtrlRecT &rec = rdr.RdrTypeUnion.CtrlRec;

  rec.Num        = (SaHpiCtrlNumT)m_num;
  rec.Ignore     = m_ignore ? SAHPI_TRUE : SAHPI_FALSE;
  rec.OutputType = m_output_type;
  rec.Type       = m_type;
  rec.Oem        = m_oem;

  // id string
  char	name[32];
  memset( name,'\0', 32 );
  strncpy( name, m_id, 31 );
  rdr.IdString.DataType = SAHPI_TL_TYPE_ASCII6;
  rdr.IdString.Language = SAHPI_LANG_ENGLISH;
  rdr.IdString.DataLength = strlen( name );

  memcpy( rdr.IdString.Data, name, 32 );

  return true;
}
