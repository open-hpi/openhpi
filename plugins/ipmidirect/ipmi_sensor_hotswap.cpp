/*
 * ipmi_sensor_hotswap.cpp
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

#include "ipmi_sensor_hotswap.h"
#include "ipmi_domain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>
#include <math.h>


cIpmiSensorHotswap::cIpmiSensorHotswap( cIpmiMc *mc )
  : cIpmiSensorDiscrete( mc )
{
}


cIpmiSensorHotswap::~cIpmiSensorHotswap()
{
}


bool
cIpmiSensorHotswap::GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr )
{
  if ( !cIpmiSensorDiscrete::GetDataFromSdr( mc, sdr ) )
       return false;

  return true;
}


bool
cIpmiSensorHotswap::CreateRdr( SaHpiRptEntryT &resource,
                               SaHpiRdrT &rdr )
{
  if ( cIpmiSensorDiscrete::CreateRdr( resource, rdr ) == false )
       return false;

  if ( !(resource.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) )
     {
       // update resource capabilities
       resource.ResourceCapabilities |= SAHPI_CAPABILITY_MANAGED_HOTSWAP;

       struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

       if ( !e )
          {
            stdlog << "out of space !\n";
            return false;
          }

       memset( e, 0, sizeof( struct oh_event ) );
       e->type               = oh_event::OH_ET_RESOURCE;
       e->u.res_event.entry = resource;

       m_mc->Domain()->AddHpiEvent( e );
     }

  return true;
}


SaHpiHsStateT
cIpmiSensorHotswap::ConvertIpmiToHpiHotswapState( tIpmiFruState h )
{
  switch( h )
     {
       case eIpmiFruStateNotInstalled:
            return SAHPI_HS_STATE_NOT_PRESENT;

       case eIpmiFruStateInactive:
            return SAHPI_HS_STATE_INACTIVE;

       case eIpmiFruStateActivationRequest:
            return SAHPI_HS_STATE_INSERTION_PENDING;

       case eIpmiFruStateActivationInProgress:
            return SAHPI_HS_STATE_INSERTION_PENDING;

       case eIpmiFruStateActive:
            return SAHPI_HS_STATE_ACTIVE_HEALTHY;

       case eIpmiFruStateDeactivationRequest:
            return SAHPI_HS_STATE_EXTRACTION_PENDING;

       case eIpmiFruStateDeactivationInProgress:
            return SAHPI_HS_STATE_EXTRACTION_PENDING;

       case eIpmiFruStateCommunicationLost:
            return SAHPI_HS_STATE_NOT_PRESENT;
     }

  assert( 0 );

  return SAHPI_HS_STATE_NOT_PRESENT;
}


SaErrorT
cIpmiSensorHotswap::CreateEvent( cIpmiEvent *event, SaHpiEventT &h )
{
  memset( &h, 0, sizeof( SaHpiEventT ) );

  cIpmiResource *res = Resource();
  assert( res );

  h.Source    = res->m_resource_id;
  h.EventType = SAHPI_ET_HOTSWAP;

/*
  h.Timestamp = (SaHpiTimeT)IpmiGetUint32( event->m_data );

  if ( h.Timestamp == 0 )
       h.Timestamp = SAHPI_TIME_UNSPECIFIED;
  else
       h.Timestamp *= 1000000000;
*/
  // hotswap event needs exact time, because
  // of auto extraction/insertion
  gettimeofday1( &h.Timestamp );

  // Do not find the severity of hotswap event
  h.Severity = SAHPI_MAJOR;

  SaHpiHotSwapEventT &he = h.EventDataUnion.HotSwapEvent;

  tIpmiFruState state = (tIpmiFruState)(event->m_data[10] & 0x0f);
  he.HotSwapState = ConvertIpmiToHpiHotswapState( state );

  state    = (tIpmiFruState)(event->m_data[11] & 0x0f);
  he.PreviousHotSwapState = ConvertIpmiToHpiHotswapState( state );

  return SA_OK;
}


SaErrorT
cIpmiSensorHotswap::GetState( tIpmiFruState &state )
{
  // read hotswap state
  cIpmiMsg rsp;

  SaErrorT rv = GetSensorReading( rsp );

  if ( rv != SA_OK )
     {
       stdlog << "cannot get hotswap state !\n";
       return rv;
     }

  if ( rsp.m_data[1] != 0 )
     {
       stdlog << "cannot read hotswap sensor " << rsp.m_data[1] << " !\n";
       return SA_ERR_HPI_INVALID_SENSOR_CMD;
     }

  unsigned int value = rsp.m_data[3];

  for( int i = 0; i < 8; i++ )
       if ( value & ( 1 << i ) )
          {
            state = (tIpmiFruState)i;
            return SA_OK;
          }

  assert( 0 );
  return SA_ERR_HPI_INVALID_CMD;
}


SaErrorT
cIpmiSensorHotswap::GetState( SaHpiHsStateT &state )
{
  tIpmiFruState fs;
  SaErrorT rv = GetState( fs );

  if ( rv != SA_OK )
       return rv;

  state = ConvertIpmiToHpiHotswapState( fs );

  return SA_OK;
}
