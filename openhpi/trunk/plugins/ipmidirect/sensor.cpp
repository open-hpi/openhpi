/*
 *
 * Copyright (c) 2003,2004 by FORCE Computers.
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


#include "ipmi.h"
#include "ipmi_sensor_discrete.h"
#include "ipmi_sensor_threshold.h"
#include "ipmi_utils.h"

/*
void
cIpmi::IfSensorAdd( cIpmiEntity *ent, cIpmiSensor *sensor )
{
  stdlog << "adding sensor ";
  sensor->Log();
  stdlog << "\n";

  char str[512];
  sensor->IdString().GetAscii( str, 512 );

  dbg( "adding sensor %d.%d (%s) %02x: %s",
       ent->EntityId(), ent->EntityInstance(),
       ent->EntityIdString(), sensor->Num(), str );

  // find resource
  SaHpiRptEntryT *resource = ent->Domain()->FindResource( ent->m_resource_id );

  if ( !resource )
     {
       assert( 0 );
       return;
     }

  // create event
  struct oh_event *e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return;
     }

  memset( e, 0, sizeof( struct oh_event ) );

  e->type = oh_event::OH_ET_RDR;

  // create rdr
  sensor->CreateRdr( *resource, e->u.rdr_event.rdr );

  int rv = oh_add_rdr( ent->Domain()->GetHandler()->rptcache,
                       resource->ResourceId,
                       &e->u.rdr_event.rdr, sensor, 1 );

  assert( rv == 0 );

  // assign the hpi record id to sensor, so we can find
  // the rdr for a given sensor.
  // the id comes from oh_add_rdr.
  sensor->m_record_id = e->u.rdr_event.rdr.RecordId;

  ent->Domain()->AddHpiEvent( e );

  sensor->EventState() = SAHPI_ES_UNSPECIFIED;
}
*/


/*
void
cIpmi::IfSensorRem( cIpmiEntity *ent, cIpmiSensor *sensor )
{
  stdlog << "removing sensor " << ent->EntityId() << "." 
         << ent->EntityInstance() << " (" << ent->EntityIdString()
         << "): " << sensor->IdString() << "\n";

  // find resource
  SaHpiRptEntryT *resource = ent->Domain()->FindResource( ent->m_resource_id );

  if ( !resource )
     {
       assert( 0 );
       return;
     }

  int rv = oh_remove_rdr( ent->Domain()->GetHandler()->rptcache,
                          resource->ResourceId,
                          sensor->m_record_id );

  assert( rv == 0 );

  sensor->m_record_id = 0xfffffff4;
}
*/
