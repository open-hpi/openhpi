/*
 * ipmi_event.c
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
#include <assert.h>

#include "ipmi_event.h"
#include "ipmi_utils.h"
#include "ipmi_log.h"
#include "ipmi_sensor.h"
#include "ipmi_mc.h"


static const char *thres_map[] =
{
  "IpmiLowerNonCritical",
  "IpmiLowerCritical",
  "IpmiLowerNonRecoverable",
  "IpmiUpperNonCritical",
  "IpmiUpperCritical",
  "IpmiUpperNonRecoverable"
};

static int thres_map_num = sizeof( thres_map ) / sizeof( char * );


const char *
IpmiThresToString( tIpmiThresh val )
{
  if ( val >= thres_map_num )
       return "invalid";

  return thres_map[val];
}


cIpmiEvent::cIpmiEvent()
  : m_mc( 0 ), m_record_id( 0 ), m_type( 0 )
{
  memset( m_data, 0, dIpmiMaxSelData );
}


int
cIpmiEvent::Cmp( const cIpmiEvent &event2 ) const
{
  //  if ( event1->mc != event2->mc )
  //       return 1;

  if ( m_record_id > event2.m_record_id )
       return 1;

  if ( m_record_id < event2.m_record_id )
       return -1;

  if ( m_type > event2.m_type )
	return 1;

  if ( m_type < event2.m_type )
       return -1;

  return memcmp( m_data, event2.m_data, 13 );
}


void
cIpmiEvent::Log() const
{
  assert( m_mc );

  unsigned int t = IpmiGetUint32( m_data );
  char ts[dDateTimeStringSize];

  IpmiDateTimeToString( t, ts );

  IpmiLog( "mc 0x%02x, id %d, type %s, time %s, slave addr 0x%02x, rev 0x%02x, sensor type %s, sensor id 0x%02x, dir %s, event type %s, 0x%02x, 0x%02x, 0x%02x\n",
           m_mc->GetAddress(), m_record_id,
           m_type == 0x02 ? "system event" : "OEM", ts, m_data[4], m_data[6],
           IpmiSensorTypeToString( (tIpmiSensorType)( m_data[7]) ),
           m_data[8], (m_data[9] >> 7) ? "deassertion" : "assertion",
           IpmiEventReadingTypeToString( (tIpmiEventReadingType)( m_data[9] & 0x7f) ),
           m_data[10], m_data[11], m_data[12] );
}
