/*
 * ipmi_discover.cpp
 *
 * discover MCs
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
 *
 * This file contains:
 * 1. discover of MCs
 * 2. periodic polling of MCs
 * 3. periodic reading SEL
 * 4. handling of events
 */


#include "ipmi_domain.h"
#include <errno.h>


class cIpmiMcTask
{
public:
  cIpmiMcTask *m_next;
  tIpmiMcTask  m_task;
  cTime        m_timeout;
  void        *m_userdata;

  cIpmiMcTask( tIpmiMcTask task, const cTime &timeout, 
               void *userdata )
    : m_next( 0 ), m_task( task ), 
      m_timeout( timeout ), m_userdata( userdata )
  {}
};


cIpmiMcThread::cIpmiMcThread( cIpmiDomain *domain,
                              unsigned char addr,
                              unsigned int properties,
                              unsigned int mc_type )
  : m_domain( domain ), m_addr( addr ), m_type( mc_type ),
    m_mc( 0 ), m_properties( properties ),
    m_exit( false ), m_tasks( 0 ),
    m_sel( 0 ), m_events( 0 )
{
}


cIpmiMcThread::~cIpmiMcThread()
{
  ClearMcTaskList();
}


void
cIpmiMcThread::AddMcTask( tIpmiMcTask task, const cTime &timeout, 
                          void *userdata )
{
  cIpmiMcTask *dt = new cIpmiMcTask( task, timeout, userdata );

  cIpmiMcTask *prev = 0;
  cIpmiMcTask *current = m_tasks;

  // loop to right position
  while( current && current->m_timeout <= dt->m_timeout )
     {
       prev = current;
       current = current->m_next;
     }

  if ( prev == 0 )
     {
       // insert dt at first position
       m_tasks = dt;
       dt->m_next = current;
     }
  else
     {
       dt->m_next = current;
       prev->m_next = dt;
     }
}


void
cIpmiMcThread::AddMcTask( tIpmiMcTask task, unsigned int ms,
                          void *userdata )
{
  cTime timeout = cTime::Now();
  timeout += ms;

  AddMcTask( task, timeout, userdata );
}


bool
cIpmiMcThread::RemMcTask( void *userdata )
{
  bool rv = false;

  cIpmiMcTask *prev = 0;
  cIpmiMcTask *current = m_tasks;

  // loop to right position
  while( current && current->m_userdata != userdata )
     {
       prev = current;
       current = current->m_next;
     }

  if ( current && current->m_userdata )
     {
       if ( prev == 0 )
	    m_tasks = current->m_next;
       else
	    prev->m_next = current->m_next;

       delete current;

       rv = true;
     }
  else
       assert( 0 );

  return rv;
}


void
cIpmiMcThread::ClearMcTaskList()
{
  while( m_tasks )
     {
       cIpmiMcTask *dt = m_tasks;
       m_tasks = m_tasks->m_next;
       delete dt;
     }
}


void *
cIpmiMcThread::Run()
{
  IpmiLog( "starting MC thread 0x%02x.\n", m_addr );

  m_domain->m_mc_thread_lock.Lock();
  m_domain->m_num_mc_threads++;
  m_domain->m_mc_thread_lock.Unlock();

  if ( m_properties & dIpmiMcThreadInitialDiscover )
     {
       Discover();
       m_domain->m_initial_discover_lock.Lock();
       m_domain->m_initial_discover--;
       m_domain->m_initial_discover_lock.Unlock();
     }

  if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
       || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
       PollAddr( m_mc );

  // this is a hack, because
  // of the usleep calls and polling
  // of event queue and task list.
  while( !m_exit )
     {
       // handling all events in the event 
       // in the event queue
       HandleEvents();
       usleep( 100000 );

       // check for tasks to do
       while( m_tasks )
          {
            cTime now = cTime::Now();

	    if ( now < m_tasks->m_timeout )
		 break;

	    // timeout
            cIpmiMcTask *dt = m_tasks;
            m_tasks = m_tasks->m_next;

            (this->*dt->m_task)( dt->m_userdata );
            delete dt;
          }
     }

  IpmiLog( "stop MC thread 0x%02x.\n", m_addr );

  m_domain->m_mc_thread_lock.Lock();
  assert( m_domain->m_num_mc_threads > 0 );
  m_domain->m_num_mc_threads--;
  m_domain->m_mc_thread_lock.Unlock();  

  return 0;
}


void
cIpmiMcThread::Discover( cIpmiMsg *get_device_id_rsp )
{
  cIpmiAddr addr( eIpmiAddrTypeIpmb, 0, 0, m_addr );
  cIpmiMsg gdi_rsp;

  if ( !get_device_id_rsp )
     {
       // send a get device id
       cIpmiMsg msg( eIpmiNetfnApp, eIpmiCmdGetDeviceId );

       // try sending the command only one time
       int rv = m_domain->SendCommand( addr, msg, gdi_rsp, 1 );

       if ( rv || gdi_rsp.m_data[0] != 0 )
            return;

       get_device_id_rsp = &gdi_rsp;
     }

  IpmiLog( "MC at 0x%02x found:\n", m_addr );
  IpmiLog( "\tdevice id             : 0x%02x\n", get_device_id_rsp->m_data[1] );
  IpmiLog( "\tdevice SDR            : %s\n", (get_device_id_rsp->m_data[2] & 0x80) ? "yes" : "no" );
  IpmiLog( "\tdevice revision       : 0x%02x\n", get_device_id_rsp->m_data[2] & 0x0f );
  IpmiLog( "\tdevice available      : %s\n", (get_device_id_rsp->m_data[3] & 0x80) ? "update" : "normal operation" );
  IpmiLog( "\tmajor firmware revsion: 0x%02x\n", get_device_id_rsp->m_data[3] & 0x7f );
  IpmiLog( "\tfirmware              : %d.%d\n", (get_device_id_rsp->m_data[4] >>4) & 0xf,
           get_device_id_rsp->m_data[4] & 0xf );
  IpmiLog( "\tIPMI version          : %d.%d\n", get_device_id_rsp->m_data[5] & 0xf,
           (get_device_id_rsp->m_data[5] >> 4) & 0xf );
  IpmiLog( "\tchassis device        : %s\n", (get_device_id_rsp->m_data[6] & 0x80) ? "yes" : "no" );
  IpmiLog( "\tbridge                : %s\n", (get_device_id_rsp->m_data[6] & 0x40) ? "yes" : "no" );
  IpmiLog( "\tIPMB event generator  : %s\n", (get_device_id_rsp->m_data[6] & 0x20) ? "yes" : "no" );
  IpmiLog( "\tIPMB event receiver   : %s\n", (get_device_id_rsp->m_data[6] & 0x10) ? "yes" : "no" );
  IpmiLog( "\tFRU inventory data    : %s\n", (get_device_id_rsp->m_data[6] & 0x08) ? "yes" : "no" );
  IpmiLog( "\tSEL device            : %s\n", (get_device_id_rsp->m_data[6] & 0x04) ? "yes" : "no" );
  IpmiLog( "\tSDR repository device : %s\n", (get_device_id_rsp->m_data[6] & 0x02) ? "yes" : "no" );
  IpmiLog( "\tsensor device         : %s\n", (get_device_id_rsp->m_data[6] & 0x01) ? "yes" : "no" );

  unsigned int mid =    get_device_id_rsp->m_data[7]
                     | (get_device_id_rsp->m_data[8] << 8)
                     | (get_device_id_rsp->m_data[9] << 16);
  IpmiLog( "\tmanufacturer id       : 0x%03x\n", mid );

  unsigned int pid = IpmiGetUint16( get_device_id_rsp->m_data + 10 );
  IpmiLog( "\tproduct id            : 0x%02x\n", pid );

  m_mc = m_domain->FindMcByAddr( addr );

  if (    m_mc && m_mc->IsActive()
       && !m_mc->DeviceDataCompares( *get_device_id_rsp ) )
     {
       // The MC was replaced with a new one, so clear the old
       // one and add a new one.
       m_domain->CleanupMc( m_mc );

       m_mc = 0;
     }

  if ( !m_mc || !m_mc->IsActive() )
     {
       // It doesn't already exist, or it's inactive, so add
       // it.
       if ( !m_mc )
          {
            // If it's not there, then add it.  If it's just not
            // active, reuse the same data.
            m_mc = m_domain->NewMc( addr );
          }

       int rv = m_mc->GetDeviceIdDataFromRsp( *get_device_id_rsp );

       if ( rv )
          {
            // If we couldn't handle the device data, just clean
            // it up
            IpmiLog( "couldn't handle the device data !\n" );

            m_domain->CleanupMc( m_mc );
            m_mc = 0;

            return;
          }

       cIpmiMcVendor *mv = cIpmiMcVendorFactory::GetFactory()->Get( mid, pid );
       m_mc->SetVendor( mv );

       if ( mv->Init( m_mc, *get_device_id_rsp ) == false )
          {
            IpmiLog( "cannot initialize MC: %02x !\n", m_mc->GetAddress() );

            m_domain->CleanupMc( m_mc );
            m_mc = 0;

            return;
          }

       rv = m_mc->HandleNew();

       if ( rv )
          {
            IpmiLog( "error while discover MC 0x%02x !\n", m_addr );

            m_domain->CleanupMc( m_mc );
            m_mc = 0;

            return;
          }
       
       if ( m_mc->SelDeviceSupport() )
          {
            GList *new_events = m_mc->Sel()->GetEvents();

            if ( new_events )
                 m_domain->HandleEvents( new_events );
          }
     }

  if ( m_mc->SelDeviceSupport() )
     {
       assert( m_sel == 0 );

       IpmiLog( "addr 0x%02x: add read sel. cIpmiMcThread::Discover\n", m_addr );

       m_sel = m_mc->Sel();

       AddMcTask( &cIpmiMcThread::ReadSel,
                  m_domain->m_sel_rescan_interval,
                  m_sel );
     }
}


void
cIpmiMcThread::HandleEvents()
{
  bool loop = true;

  while( loop )
     {
       cIpmiEvent *event = 0;

       m_events_lock.Lock();

       if ( m_events )
          {
            event = (cIpmiEvent *)m_events->data;
            m_events = g_list_remove( m_events, event );
          }

       loop = m_events ? true : false;

       m_events_lock.Unlock(); 

       if ( event )
          {
            cIpmiSensor *sensor = 0;

            if ( m_mc )
                 sensor = m_mc->FindSensor( (event->m_data[5] & 0x3), event->m_data[8] );

            HandleEvent( sensor, event );
            delete event;
          }
     }
}


void
cIpmiMcThread::HandleEvent( cIpmiSensor *sensor,
                            cIpmiEvent *event )
{
  IpmiLog( "event: " );
  event->Log();

  if ( event->m_type != 0x02 )
     {
       IpmiLog( "remove event: unknown event type %02x !\n", event->m_type );
       return;
     }

  if ( (event->m_data[4] & 0x01) != 0 )
     {
       IpmiLog( "remove event: system software event.\n" );
       return;
     }

  cIpmiMc     *mc = 0;
  cIpmiAddr    addr;

  addr.m_type = eIpmiAddrTypeIpmb;

  if ( event->m_data[6] == 0x03 )
       addr.m_channel = 0;
  else
       addr.m_channel = event->m_data[5] >> 4;

  addr.m_slave_addr = event->m_data[4];
  addr.m_lun = 0;

  mc = m_domain->FindMcByAddr( addr );

  if ( mc )
       sensor = mc->FindSensor( (event->m_data[5] & 0x3), event->m_data[8] );

  // hotswap event
  if ( event->m_data[7] == eIpmiSensorTypeAtcaHotSwap )
     {
       cIpmiSensorHotswap *hs = dynamic_cast<cIpmiSensorHotswap *>( sensor );
       
       if ( !hs )
          {
            IpmiLog( "Not a hotswap sensor !\n" );
            return;
          }

       HandleHotswapEvent( hs, event );
       return;
     }

  // reveive an event from an unknown MC
  // => discover
  if ( !m_mc )
     {
       assert( m_sel == 0 );

       // remove old task
       if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
            || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
          {
            IpmiLog( "addr 0x%02x: rem poll. cIpmiMcThread::HandleEvent\n", m_addr );
            RemMcTask( m_mc );
          }

       Discover();

       // add new poll task
       if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
            || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
          {
            IpmiLog( "addr 0x%02x: add poll. cIpmiMcThread::HandleEvent\n", m_addr );
            AddMcTask( &cIpmiMcThread::PollAddr, m_domain->m_mc_poll_interval,
                       m_mc );
          }

       // find sensor again
       if ( m_mc )
            sensor = m_mc->FindSensor( (event->m_data[5] & 0x3),
                                       event->m_data[8] );
     }

  // sensor event
  if ( sensor )
     {
       sensor->HandleEvent( event );
       return;
     }

  // unknown event
  IpmiLog( "unknown event.\n" );
}


void
cIpmiMcThread::HandleHotswapEvent( cIpmiSensorHotswap *sensor,
                                   cIpmiEvent *event )
{
  // remove old task
  if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
       || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
     {
       IpmiLog( "addr 0x%02x: rem poll. cIpmiMcThread::HandleHotswapEvent\n", m_addr );
       RemMcTask( m_mc );
     }

  tIpmiFruState current_state = (tIpmiFruState)(event->m_data[10] & 0x0f);
  tIpmiFruState prev_state    = (tIpmiFruState)(event->m_data[11] & 0x0f);

  IpmiLog( "hot swap event M%d -> M%d.\n", prev_state, current_state );

  // check for mc
  if ( !m_mc )
     {
       IpmiLog( "scan for MC 0x%02x 0x%02x.\n",
                event->m_data[5] >> 4, event->m_data[4] );

       if (    m_mc && m_mc->IsActive()
            && m_mc->GetChannel() == (unsigned int)(event->m_data[5] >> 4)
            && m_mc->GetAddress() == (unsigned int)event->m_data[4] )
            IpmiLog( "MC exists and is active !\n" );
       else
          {
            assert( m_sel == 0 );

            Discover();

            // find sensor
            if ( m_mc )
               {
                 cIpmiSensor *s = m_mc->FindSensor( (event->m_data[5] & 0x3), event->m_data[8] );
                 sensor = dynamic_cast<cIpmiSensorHotswap *>( s );
               }
          }
     }

  if ( current_state == eIpmiFruStateActivationInProgress )
     {
       assert( m_mc );

       int rv = m_mc->AtcaPowerFru( 0 );

       if ( rv )
          { 
            // deactivate fru, because M3 has no representation in 
            // HPI we try to restart the FRU
            cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdSetFruActivation );

            msg.m_data[0] = dIpmiPigMgId;
            msg.m_data[1] = 0; // FRU id
            msg.m_data[2] = 0; // deactivate fru
            msg.m_data_len = 3;

            cIpmiMsg rsp;

            rv = m_mc->SendCommand( msg, rsp );

            if ( rv )
                 IpmiLog( "cannot send set fru activation: %d\n", rv );
            else if (    rsp.m_data_len != 2
                      || rsp.m_data[0] != eIpmiCcOk 
                      || rsp.m_data[1] != dIpmiPigMgId )
                 IpmiLog( "cannot set fru activation: 0x%02x !\n", rsp.m_data[0] );
          }
       else
            IpmiLog( "power fru.\n" );
     }

  sensor->HandleEvent( event );

  if ( m_mc )
       m_mc->FruState() = current_state;

  if ( current_state == eIpmiFruStateNotInstalled )
     {
       // remove mc
       if ( m_mc )
            m_domain->CleanupMc( m_mc );

       m_mc = 0;
     }

  if ( m_mc == 0 && m_sel )
     {
       RemMcTask( m_sel );
       m_sel = 0;
     }

  // add new poll task
  if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
       || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
     {
       IpmiLog( "addr 0x%02x: add poll. cIpmiMcThread::HandleHotswapEvent\n", m_addr );
       AddMcTask( &cIpmiMcThread::PollAddr, m_domain->m_mc_poll_interval, m_mc );
     }
}


void
cIpmiMcThread::AddEvent( cIpmiEvent *event )
{
  m_events_lock.Lock();
  m_events = g_list_append( m_events, event );
  m_events_lock.Unlock();
}


void
cIpmiMcThread::PollAddr( void *userdata )
{
  cIpmiMc *mc = (cIpmiMc *)userdata;

  IpmiLog( "poll MC at 0x%02x.\n", m_addr );

  // send a get device id
  cIpmiAddr addr( eIpmiAddrTypeIpmb, 0, 0, m_addr );
  cIpmiMsg msg( eIpmiNetfnApp, eIpmiCmdGetDeviceId );
  cIpmiMsg rsp;

  // because this command is send periodical 
  // retries is set to 1.
  int rv = m_domain->SendCommand( addr, msg, rsp, 1 );

  if ( rv )
     {
       if ( m_mc )
          {
            IpmiLog( "communication lost: 0x%02x !\n", m_addr );

            if ( m_properties & dIpmiMcThreadCreateM0 )
               {
                 cIpmiSensorHotswap *hs = m_mc->FindHotswapSensor();

                 if ( hs )
                    {
                      // generate an event hotswap event M0
                      cIpmiEvent *event = new cIpmiEvent;
                      event->m_mc = m_mc;

                      event->m_data[0]  = 0; // timestamp
                      event->m_data[1]  = 0;
                      event->m_data[2]  = 0;
                      event->m_data[3]  = 0;
                      event->m_data[4]  = m_mc->GetAddress();
                      event->m_data[5]  = 0;
                      event->m_data[6]  = 0x04; // message format
                      event->m_data[7]  = hs->SensorType();
                      event->m_data[8]  = hs->Num();
                      event->m_data[9]  = 0; // assertion
                      event->m_data[10] = 0; // M0
                      event->m_data[11] = m_mc->FruState() | (7 << 4); // communication lost
                      event->m_data[12] = 0;
                      
                      // this is because HandleHotswapEvent first removes the PollAddr task
                      if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
                              || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
                         {
                           IpmiLog( "addr 0x%02x: add poll. cIpmiMcThread::PollAddr\n", m_addr );
                           AddMcTask( &cIpmiMcThread::PollAddr, m_domain->m_mc_poll_interval, m_mc );
                         }

                      HandleHotswapEvent( hs, event );
                      delete event;
                      
                      return;
                    }
               }

            m_domain->CleanupMc( mc );
            m_mc = 0;
          }
     }
  else
     {
       if ( !mc )
            // MC found.
            Discover( &rsp );
       //       else if ( m_mc )
            // Periodically check the event receiver for existing MCs.
       //     m_mc->CheckEventRcvr();
     }

  if ( m_mc == 0 && m_sel )
     {
       RemMcTask( m_sel );
       m_sel = 0;
     }

  if (    ( m_mc  && (m_properties & dIpmiMcThreadPollAliveMc ) )
       || ( !m_mc && (m_properties & dIpmiMcThreadPollDeadMc ) ) )
     {
       IpmiLog( "addr 0x%02x: add poll. cIpmiMcThread::PollAddr\n", m_addr );
       AddMcTask( &cIpmiMcThread::PollAddr, m_domain->m_mc_poll_interval, m_mc );
     }
}


void 
cIpmiMcThread::ReadSel( void *userdata )
{
  cIpmiSel *sel = (cIpmiSel *)userdata;
  GList *new_events = sel->GetEvents();

  IpmiLog( "addr 0x%02x: add sel reading. cIpmiMcThread::ReadSel\n", m_addr );

  // add myself to task list
  AddMcTask( &cIpmiMcThread::ReadSel, m_domain->m_sel_rescan_interval,
                   userdata );

  if ( new_events )
       m_domain->HandleEvents( new_events );
}
