/*
 *
 * Copyright (c) 2003,2004 by FORCE Computers
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>

#include "ipmi_domain.h"
#include "ipmi_mc.h"
#include "ipmi_fru.h"
#include "ipmi_sensor.h"
#include "ipmi_utils.h"


cIpmiMc::cIpmiMc( cIpmiDomain *domain, const cIpmiAddr &addr )
  : m_addr( addr ), m_active( true ),
    m_fru_state( eIpmiFruStateNotInstalled ),
    m_domain( domain ), m_sdrs( 0 ),
    m_sensors_in_my_sdr( 0 ), m_sensors_in_my_sdr_count( 0 ),
    m_sensors( 0 ), m_sel( 0 ),
    m_device_id( 0 ), m_device_revision( 0 ), 
    m_provides_device_sdrs( false ), m_device_available( false ),
    m_chassis_support( false ), m_bridge_support( false ), 
    m_ipmb_event_generator_support( false ), m_ipmb_event_receiver_support( false ),
    m_fru_inventory_support( false ), m_sel_device_support( false ),
    m_sdr_repository_support( false ), m_sensor_device_support( false ),
    m_major_fw_revision( 0 ), m_minor_fw_revision( 0 ),
    m_major_version( 0 ), m_minor_version( 0 ),
    m_manufacturer_id( 0 ), m_product_id( 0 ),
    m_real_device_id( 0 ), m_real_device_revision( 0 ),
    m_real_provides_device_sdrs( false ), m_real_device_available( false ),
    m_real_chassis_support( false ), m_real_bridge_support( false ),
    m_real_ipmb_event_generator_support( false ),
    m_real_ipmb_event_receiver_support( false ),
    m_real_fru_inventory_support( false ), m_real_sel_device_support( false ),
    m_real_sdr_repository_support( false ), m_real_sensor_device_support( false ),
    m_real_major_fw_revision( 0 ), m_real_minor_fw_revision( 0 ),
    m_real_major_version( 0 ), m_real_minor_version( 0 ),
    m_real_manufacturer_id( 0 ), m_real_product_id( 0 )
{
  IpmiLog( "adding MC: 0x%02x 0x%02x\n",
           addr.m_channel, addr.m_slave_addr );

  // use default as long the manufactorer
  // and product id not known
  m_vendor = cIpmiMcVendorFactory::GetFactory()->Default();

  m_aux_fw_revision[0] = 0;
  m_aux_fw_revision[1] = 0;
  m_aux_fw_revision[2] = 0;
  m_aux_fw_revision[3] = 0;
  m_real_aux_fw_revision[0] = 0;
  m_real_aux_fw_revision[1] = 0;
  m_real_aux_fw_revision[2] = 0;
  m_real_aux_fw_revision[3] = 0;

  m_sdrs = new cIpmiSdrs( this, 0, 1 );
  assert( m_sdrs );

  m_sensors = new cIpmiSensorInfo( this );
  assert( m_sensors );

  m_sel = new cIpmiSel( this, 0 );
  assert( m_sel );
}


cIpmiMc::~cIpmiMc()
{
  assert( m_domain );
  assert( !m_active );

  if ( m_sensors )
     {
       delete m_sensors;
       m_sensors = 0;
     }

  if ( m_sdrs )
     {
       delete m_sdrs;
       m_sdrs = 0;
     }

  if ( m_sel )
     {
       delete m_sel;
       m_sel = 0;
     }
}


bool
cIpmiMc::Cleanup()
{
  assert( m_domain );

  // First the device SDR sensors, since they can be there for any MC.

  if ( m_sensors_in_my_sdr )
     {
       unsigned int i;

       for( i = 0; i < m_sensors_in_my_sdr_count; i++ )
	    if ( m_sensors_in_my_sdr[i] )
                 m_sensors_in_my_sdr[i]->Destroy();

       delete [] m_sensors_in_my_sdr;

       m_sensors_in_my_sdr       = 0;
       m_sensors_in_my_sdr_count = 0;
     }

  m_active = false;

  if ( m_sensors == 0 || m_sensors->m_sensor_count == 0 )
       return true;

  IpmiLog( "removing MC: 0x%02x 0x%02x\n",
           m_addr.m_channel, m_addr.m_slave_addr );

  return false;
}


int
cIpmiMc::SendSetEventRcvr( unsigned int addr )
{
  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdSetEventReceiver );
  cIpmiMsg rsp;
  int rv;

  IpmiLog( "Send set event receiver: 0x%02x.\n", addr );

  msg.m_data_len = 2;
  msg.m_data[0]  = addr;
  msg.m_data[1]  = 0; // LUN is 0 per the spec (section 7.2 of 1.5 spec).

  IpmiLog( "SendSetEventRcvr: %x %x -> %x %x\n",
           GetChannel(), GetAddress(), 0, addr  );

  rv = SendCommand( msg, rsp );

  if ( rv )
       return rv;

  if ( rsp.m_data[0] != 0 )
     {
       // Error setting the event receiver, report it.
       IpmiLog( "Could not set event receiver for MC at 0x%x !\n",
                m_addr.m_slave_addr );

       return EINVAL;
     }

  return 0;
}


int
cIpmiMc::HandleNew()
{
  int rv;

  m_active = true;

  if ( m_provides_device_sdrs )
     {
       // read sdr
       rv = m_sdrs->Fetch();

       if ( rv )
            return EINVAL;

       if ( m_vendor->CreateSensors( this, m_sdrs ) == false )
            return EINVAL;

       if ( m_vendor->CreateControls( this, m_sdrs ) == false )
            return EINVAL;

       if ( m_sel_device_support )
            IpmiSelHandleSdr( m_domain, this, m_sdrs );

       rv = IpmiFruHandleSdr( m_domain, this, m_sdrs );

       if ( rv )
            return rv;
     }

  // read the sel first
  if ( m_sel_device_support )
     {
       // this is for testing
       // clear sel
       rv = m_sel->GetInfo();

       if ( rv )
            return rv;

       m_sel->m_fetched = false;

       rv = m_sel->ClearSel();

       if ( rv )
            return rv;

       // read old events
       GList *list = m_sel->GetEvents();
       m_sel->ClearList( list );
     }

  // We set the event receiver here, so that we know all the SDRs
  // are installed.  That way any incoming events from the device
  // will have the proper sensor set.
  unsigned int event_rcvr = 0;

  if ( m_ipmb_event_generator_support )
     {
       cIpmiMc *er = m_domain->GetEventRcvr();

       if ( er )
            event_rcvr = er->GetAddress();
     }
  else if ( m_sel_device_support )
       // If it is an SEL device and not an event receiver, then
       // set it's event receiver to itself.
       event_rcvr = GetAddress();

  if ( event_rcvr )
     {
       // This is a re-arm of all sensors of the MC
       // => each sensor sends the pending events again.
       // because we now which events are old,
       // we can get the current state.
       rv = SendSetEventRcvr( event_rcvr );

       if ( rv )
            return rv;
     }

  return 0;
}


bool
cIpmiMc::DeviceDataCompares( const cIpmiMsg &rsp ) const
{
  const unsigned char *rsp_data = rsp.m_data;

  if ( rsp.m_data_len < 12 )
       return false;

  if ( m_real_device_id != rsp_data[1] )
       return false;

  if ( m_real_device_revision != (rsp_data[2] & 0xf) )
       return false;
    
  if ( m_real_provides_device_sdrs != ((rsp_data[2] & 0x80) == 0x80) )
       return false;

  if ( m_real_device_available != ((rsp_data[3] & 0x80) == 0x80) )
       return false;

  if ( m_real_major_fw_revision != (rsp_data[3] & 0x7f) )
       return false;

  if ( m_real_minor_fw_revision != (rsp_data[4]) )
       return false;

  if ( m_real_major_version != (rsp_data[5] & 0xf) )
       return false;

  if ( m_real_minor_version != ((rsp_data[5] >> 4) & 0xf) )
       return false;

  if ( m_real_chassis_support != ((rsp_data[6] & 0x80) == 0x80) )
       return false;

  if ( m_real_bridge_support != ((rsp_data[6] & 0x40) == 0x40) )
       return false;

  if ( m_real_ipmb_event_generator_support != ((rsp_data[6] & 0x20)==0x20) )
       return false;

  if ( m_real_ipmb_event_receiver_support != ((rsp_data[6] & 0x10) == 0x10) )
       return false;

  if ( m_real_fru_inventory_support != ((rsp_data[6] & 0x08) == 0x08) )
       return false;

  if ( m_real_sel_device_support != ((rsp_data[6] & 0x04) == 0x04) )
       return false;

  if ( m_real_sdr_repository_support != ((rsp_data[6] & 0x02) == 0x02) )
       return false;

  if ( m_real_sensor_device_support != ((rsp_data[6] & 0x01) == 0x01) )
       return false;

  if ( m_real_manufacturer_id != (unsigned int)(   (rsp_data[7]
						 | (rsp_data[8] << 8)
						 | (rsp_data[9] << 16))) )
       return false;

  if ( m_real_product_id != (rsp_data[10] | (rsp_data[11] << 8)) )
       return false;

  if ( rsp.m_data_len < 16 )
     {
       // no aux revision, it should be all zeros.
       if (    ( m_real_aux_fw_revision[0] != 0 )
            || ( m_real_aux_fw_revision[1] != 0 )
            || ( m_real_aux_fw_revision[2] != 0 )
            || ( m_real_aux_fw_revision[3] != 0 ) )
            return false;
     }
  else 
       if ( memcmp( m_real_aux_fw_revision, rsp_data + 12, 4 ) != 0 )
            return false;

  // Everything's the same.
  return true;
}


int
cIpmiMc::GetDeviceIdDataFromRsp( const cIpmiMsg &rsp )
{
  const unsigned char *rsp_data = rsp.m_data;

  if ( rsp_data[0] != 0 )
       return EINVAL;

  if ( rsp.m_data_len < 12 )
       return EINVAL;

  m_device_id                    = rsp_data[1];
  m_device_revision              = rsp_data[2] & 0xf;
  m_provides_device_sdrs         = (rsp_data[2] & 0x80) == 0x80;
  m_device_available             = (rsp_data[3] & 0x80) == 0x80;
  m_major_fw_revision            = rsp_data[3] & 0x7f;
  m_minor_fw_revision            = rsp_data[4];
  m_major_version                = rsp_data[5] & 0xf;
  m_minor_version                = (rsp_data[5] >> 4) & 0xf;
  m_chassis_support              = (rsp_data[6] & 0x80) == 0x80;
  m_bridge_support               = (rsp_data[6] & 0x40) == 0x40;
  m_ipmb_event_generator_support = (rsp_data[6] & 0x20) == 0x20;
  m_ipmb_event_receiver_support  = (rsp_data[6] & 0x10) == 0x10;
  m_fru_inventory_support        = (rsp_data[6] & 0x08) == 0x08;
  m_sel_device_support           = (rsp_data[6] & 0x04) == 0x04;
  m_sdr_repository_support       = (rsp_data[6] & 0x02) == 0x02;
  m_sensor_device_support        = (rsp_data[6] & 0x01) == 0x01;
  m_manufacturer_id              =    (rsp_data[7]
                                    | (rsp_data[8] << 8)
                                    | (rsp_data[9] << 16));
  m_product_id                   = rsp_data[10] | (rsp_data[11] << 8);

  if ( rsp.m_data_len < 16 )
       // no aux revision.
       memset( m_aux_fw_revision, 0, 4 );
  else
       memcpy( m_aux_fw_revision, rsp_data + 12, 4 );

  // Copy these to the version we use for comparison.
  m_real_device_id                    = m_device_id;
  m_real_device_revision              = m_device_revision;
  m_real_provides_device_sdrs         = m_provides_device_sdrs;
  m_real_device_available             = m_device_available;
  m_real_chassis_support              = m_chassis_support;
  m_real_bridge_support               = m_bridge_support;
  m_real_ipmb_event_generator_support = m_ipmb_event_generator_support;
  m_real_ipmb_event_receiver_support  = m_ipmb_event_receiver_support;
  m_real_fru_inventory_support        = m_fru_inventory_support;
  m_real_sel_device_support           = m_sel_device_support;
  m_real_sdr_repository_support       = m_sdr_repository_support;
  m_real_sensor_device_support        = m_sensor_device_support;
  m_real_major_fw_revision            = m_major_fw_revision;
  m_real_minor_fw_revision            = m_minor_fw_revision;
  m_real_major_version                = m_major_version;
  m_real_minor_version                = m_minor_version;
  m_real_manufacturer_id              = m_manufacturer_id;
  m_real_product_id                   = m_product_id;
  memcpy( m_real_aux_fw_revision, m_aux_fw_revision,
          sizeof(m_real_aux_fw_revision ) );

  return 0;
}


void
cIpmiMc::CheckEventRcvr()
{
  int rv;

  if ( m_ipmb_event_generator_support )
       return;

  // We have an MC that is live (or still live) and generates
  // events, make sure the event receiver is set properly.
  cIpmiMc *er = m_domain->GetEventRcvr();
  
  if ( !er )
       return;

  unsigned int event_rcvr = er->GetAddress();

  // Don't bother if we have no possible event receivers.
  if ( !event_rcvr )
       return;

  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdGetEventReceiver );
  cIpmiMsg rsp;

  rv = SendCommand( msg, rsp );

  if ( rv )
       // No care about return values, if this fails it will be done
       //   again later.
       return;

  if ( rsp.m_data[0] != 0 )
     {
       // Error getting the event receiver, report it.
       IpmiLog( "Could not get event receiver for MC at 0x%x !\n",
                m_addr.m_slave_addr );
       return;
     } 

  if ( rsp.m_data_len < 2 )
     {
       IpmiLog( "Get event receiver length invalid for MC at 0x%x !\n",
                m_addr.m_slave_addr );

       return;
     }

  cIpmiDomain  *domain = m_domain;
  cIpmiMc      *destmc;
  cIpmiAddr    ipmb( eIpmiAddrTypeIpmb, GetChannel(), 0, rsp.m_data[1] );

  destmc = domain->FindMcByAddr( ipmb );

  if ( !destmc || destmc->m_ipmb_event_receiver_support == 0 )
     {
       // The current event receiver doesn't exist or cannot
       // receive events, change it.
       er = m_domain->GetEventRcvr();

       if ( er )
            SendSetEventRcvr( er->GetAddress() );
     }
}


int
cIpmiMc::SendCommand( const cIpmiMsg &msg, cIpmiMsg &rsp_msg,
                      unsigned int lun, int retries )
{
  cIpmiAddr addr = m_addr;
  int rv;

  assert( m_domain );

  rv = addr.m_lun = lun;

  if ( rv )
       return rv;

  return m_domain->SendCommand( addr, msg, rsp_msg, retries );
}


unsigned int
cIpmiMc::GetChannel()
{
  if ( m_addr.m_type == eIpmiAddrTypeSystemInterface )
       return dIpmiBmcChannel;

  return m_addr.m_channel;
}


unsigned int
cIpmiMc::GetAddress()
{
  if ( m_addr.m_type == eIpmiAddrTypeIpmb )
       return m_addr.m_slave_addr;
   
  if ( m_addr.m_type == eIpmiAddrTypeSystemInterface )
       return m_addr.m_channel;

  // Address is ignore for other types.
  return 0;
}


cIpmiSensor *
cIpmiMc::FindSensor( unsigned int lun, unsigned int sensor_id )
{
  return m_sensors->FindSensor( lun, sensor_id );
}


cIpmiSensorHotswap *
cIpmiMc::FindHotswapSensor()
{
  for( unsigned int i = 0; i < m_sensors_in_my_sdr_count; i++ )
     {
       if ( m_sensors_in_my_sdr[i] == 0 )
            continue;

       cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( m_sensors_in_my_sdr[i] );
       
       if ( hs )
            return hs;
     }

  return 0;
}


int
cIpmiMc::AtcaPowerFru( int fru_id )
{
  // get power level
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetPowerLevel );
  cIpmiMsg rsp;

  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = fru_id; // FRU id
  msg.m_data[2] = 0x01; // desired steady power
  msg.m_data_len = 3;

  int rv = SendCommand( msg, rsp );

  if ( rv )
     {
       IpmiLog( "cannot send get power level: %d\n", rv );
       return EINVAL;
     }

  if (    rsp.m_data_len < 3
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "cannot get power level: 0x%02x !\n", rsp.m_data[0] );
       return EINVAL;
     }

  unsigned char power_level = rsp.m_data[2] & 0x1f;

  // set power level
  msg.m_netfn = eIpmiNetfnPicmg;
  msg.m_cmd   = eIpmiCmdSetPowerLevel;
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = 0; // FRU id
  msg.m_data[2] = power_level;
  msg.m_data[3] = 0x01; // copy desierd level to present level
  msg.m_data_len = 4;

  rv = SendCommand( msg, rsp );

  if ( rv )
     {
       IpmiLog( "cannot send set power level: %d\n", rv );
       return EINVAL;
     }

  if (    rsp.m_data_len != 2
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[1] != dIpmiPigMgId )
       IpmiLog( "cannot set power level: 0x%02x !\n", rsp.m_data[0] );

  return 0;
}
