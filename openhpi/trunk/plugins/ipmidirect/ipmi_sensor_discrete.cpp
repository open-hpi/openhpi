/*
 * ipmi_sensor_discrete.cpp
 *
 * Copyright (c) 2004 by FORCE Computers
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

#include "ipmi_sensor_discrete.h"
#include "ipmi_domain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>
#include <math.h>


cIpmiSensorDiscrete::cIpmiSensorDiscrete( cIpmiMc *mc )
  : cIpmiSensor( mc )
{
}


cIpmiSensorDiscrete::~cIpmiSensorDiscrete()
{
}

  
bool
cIpmiSensorDiscrete::GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr )
{
  if ( !cIpmiSensor::GetDataFromSdr( mc, sdr ) )
       return false;

  m_assertion_event_mask = IpmiGetUint16( sdr->m_data + 14 );
  m_assertion_event_mask &= 0x8fff;

  m_deassertion_event_mask = IpmiGetUint16( sdr->m_data + 16 );
  m_deassertion_event_mask &= 0x8fff;

  m_event_mask = IpmiGetUint16( sdr->m_data + 18 );
  m_event_mask &= 0x8fff;

  return true;
}


bool
cIpmiSensorDiscrete::CreateRdr( SaHpiRptEntryT &resource,
                                SaHpiRdrT &rdr )
{
  if ( cIpmiSensor::CreateRdr( resource, rdr ) == false )
       return false;

  SaHpiSensorRecT &rec = rdr.RdrTypeUnion.SensorRec;

  rec.Events = m_event_mask;
  rec.DataFormat.ReadingFormats = SAHPI_SRF_EVENT_STATE;
  rec.DataFormat.IsNumeric      = SAHPI_FALSE;
  rec.ThresholdDefn.IsThreshold = SAHPI_FALSE;

  return true;
}


SaErrorT
cIpmiSensorDiscrete::GetData( SaHpiSensorReadingT &data )
{
  if ( Ignore() )
     {
       dbg("sensor is ignored");
       return SA_ERR_HPI_NOT_PRESENT;
     }

  cIpmiMsg rsp;
  SaErrorT rv = GetSensorReading( rsp );

  if ( rv != SA_OK )
       return rv;

  memset( &data, 0, sizeof( SaHpiSensorReadingT ) );

  data.ValuesPresent = SAHPI_SRF_EVENT_STATE;
  data.EventStatus.SensorStatus = 0;

  if ( (rsp.m_data[2] & 0x80) != 0 )
       data.EventStatus.SensorStatus |= SAHPI_SENSTAT_EVENTS_ENABLED;

  if ( (rsp.m_data[2] & 0x40) != 0 )
       data.EventStatus.SensorStatus |= SAHPI_SENSTAT_SCAN_ENABLED;

  if ( rsp.m_data[2] & 0x20 )
       data.EventStatus.SensorStatus |= SAHPI_SENSTAT_BUSY;

  if ( rsp.m_data_len < 5 )
       rsp.m_data[4] = 0;

  // only 15 states
  rsp.m_data[4] &= 0x7f;

  data.EventStatus.EventStatus = IpmiGetUint16( rsp.m_data + 3 );

  return SA_OK;
}


SaErrorT
cIpmiSensorDiscrete::GetEventEnables( SaHpiSensorEvtEnablesT &enables )
{
  cIpmiMsg rsp;
  SaErrorT rv = cIpmiSensor::GetEventEnables( enables, rsp );

  if ( rv != SA_OK )
       return rv;

  enables.AssertEvents   = IpmiGetUint16( rsp.m_data + 2 );
  enables.DeassertEvents = IpmiGetUint16( rsp.m_data + 4 );

  return SA_OK;
}


SaErrorT
cIpmiSensorDiscrete::SetEventEnables( const SaHpiSensorEvtEnablesT &enables )
{
  cIpmiMsg msg;
  IpmiSetUint16( msg.m_data + 2, enables.AssertEvents );
  IpmiSetUint16( msg.m_data + 4, enables.DeassertEvents );

  SaErrorT rv = cIpmiSensor::SetEventEnables( enables, msg );

  return rv;
}


enum tIpmiDiscrete
{
  eIpmiTransIdle = 0,
  eIpmiTransActive,
  eIpmiTransBusy
};


SaErrorT
cIpmiSensorDiscrete::CreateEvent( cIpmiEvent *event, SaHpiEventT &h )
{
  SaErrorT rv = cIpmiSensor::CreateEvent( event, h );

  if ( rv != SA_OK )
       return rv;

  // sensor event
  SaHpiSensorEventT &se = h.EventDataUnion.SensorEvent;

  se.Assertion = (SaHpiBoolT)!(event->m_data[9] & 0x80);
  se.PreviousState = EventState();

  tIpmiDiscrete e = (tIpmiDiscrete )(event->m_data[10] & 0x7);

  switch( e )
     {
       case eIpmiTransIdle:
            se.EventState = SAHPI_ES_IDLE;
            break;

       case eIpmiTransActive:
            se.EventState = SAHPI_ES_ACTIVE;
            break;

       case eIpmiTransBusy:
            se.EventState = SAHPI_ES_BUSY;
            break;
     }

  EventState() = se.EventState;

  tIpmiEventType type;

  type = (tIpmiEventType)(event->m_data[10] >> 6);

  if ( type == eIpmiEventData2 )
       se.Oem = (SaHpiUint32T)event->m_data[11];
  else if ( type == eIpmiEventData3 )
       se.SensorSpecific = (SaHpiUint32T)event->m_data[11]; 

  type = (tIpmiEventType)((event->m_data[10] & 0x30) >> 4);

  if ( type == eIpmiEventData2 )
       se.Oem = (SaHpiUint32T)event->m_data[12];
  else if (type == eIpmiEventData3 )
       se.SensorSpecific = (SaHpiUint32T)event->m_data[12];

  return SA_OK;
}
