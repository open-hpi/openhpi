/*
 * ipmi_control.cpp
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

#include "ipmi_control.h"
#include "ipmi_domain.h"


cIpmiControl::cIpmiControl( cIpmiMc *mc,
			    unsigned int num,
                            SaHpiCtrlOutputTypeT output_type,
                            SaHpiCtrlTypeT type )
  : cIpmiRdr( mc, SAHPI_CTRL_RDR ),
    m_num( num ),
    m_output_type( output_type ),
    m_type( type )
{
}


cIpmiControl::~cIpmiControl()
{
}

bool
cIpmiControl::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  if ( cIpmiRdr::CreateRdr( resource, rdr ) == false )
       return false;

  if (    !(resource.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
       || !(resource.ResourceCapabilities & SAHPI_CAPABILITY_CONTROL ) )
     {
       // update resource
       resource.ResourceCapabilities |= SAHPI_CAPABILITY_RDR|SAHPI_CAPABILITY_CONTROL;

       struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

       if (resource.ResourceCapabilities & SAHPI_CAPABILITY_FRU)
       {
           e->event.EventType = SAHPI_ET_RESOURCE;
           e->event.EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
           stdlog << "cIpmiControl::CreateRdr SAHPI_ET_RESOURCE Event resource " << resource.ResourceId << "\n";
       }
       else
       {
           e->event.EventType = SAHPI_ET_HOTSWAP;
	   e->event.EventDataUnion.HotSwapEvent.HotSwapState = SAHPI_HS_STATE_ACTIVE;
	   e->event.EventDataUnion.HotSwapEvent.PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE;
           stdlog << "cIpmiControl::CreateRdr SAHPI_ET_HOTSWAP Event resource " << resource.ResourceId << "\n";
       }
       e->resource = resource;
       e->event.Source = resource.ResourceId;
       oh_gettimeofday(&e->event.Timestamp);
       e->event.Severity = resource.ResourceSeverity;

       m_mc->Domain()->AddHpiEvent( e );       
     }

  // control record
  SaHpiCtrlRecT &rec = rdr.RdrTypeUnion.CtrlRec;

  rec.Num        = (SaHpiCtrlNumT)m_num;
  rec.OutputType = m_output_type;
  rec.Type       = m_type;
  rec.Oem        = m_oem;

  return true;
}
