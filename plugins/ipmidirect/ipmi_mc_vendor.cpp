/*
 * ipmi_mc_vendor.cpp
 *
 * Copyright (c) 2004 by FORCE Computers
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


#include "ipmi_mc_vendor.h"
#include "ipmi_mc_vendor_force.h"
#include "ipmi_domain.h"
#include "ipmi_control_fan.h"


cIpmiMcVendorFactory *cIpmiMcVendorFactory::m_factory = 0;
static int use_count = 0;
static cThreadLock lock;


cIpmiMcVendorFactory::cIpmiMcVendorFactory()
  : m_mc_vendors( 0 ), m_default( new cIpmiMcVendor( 0, 0, "default MC vendor" ) )
{
}


cIpmiMcVendorFactory::~cIpmiMcVendorFactory()
{
  while( m_mc_vendors )
     {
       cIpmiMcVendor *mv = (cIpmiMcVendor *)m_mc_vendors->data;
       m_mc_vendors = g_list_remove( m_mc_vendors, mv );
 
       delete mv;
     }

  if ( m_default )
       delete m_default;
}


void
cIpmiMcVendorFactory::InitFactory()
{
  lock.Lock();

  if ( m_factory == 0 )
     {
       m_factory = new cIpmiMcVendorFactory;

       // register vendor mc here.
       m_factory->Register( new cIpmiMcVendorForceShMc );
     }

  use_count++;

  lock.Unlock();
}


void
cIpmiMcVendorFactory::CleanupFactory()
{
  lock.Lock();

  use_count--;
  assert( use_count >= 0 );

  if ( use_count == 0 )
     {
       delete m_factory;
       m_factory = 0;
     }
  
  lock.Unlock();
}


bool
cIpmiMcVendorFactory::Register( cIpmiMcVendor *mv )
{
  if ( Find( mv->m_manufacturer_id, mv->m_product_id ) )
     {
       assert( 0 );
       return false;
     }
  
  m_mc_vendors = g_list_append( m_mc_vendors, mv );

  return true;
}


bool
cIpmiMcVendorFactory::Unregister( unsigned int manufacturer_id,
                                  unsigned int product_id )
{
  cIpmiMcVendor *mv = Find( mv->m_manufacturer_id, mv->m_product_id );

  if ( !mv )
       return false;

  m_mc_vendors = g_list_remove( m_mc_vendors, mv );

  return true;
}


cIpmiMcVendor *
cIpmiMcVendorFactory::Find( unsigned int manufacturer_id,
                            unsigned int product_id )
{
  GList *list = m_mc_vendors;
  
  while( list )
     {
       cIpmiMcVendor *mv = (cIpmiMcVendor *)list->data;
       
       if (    mv->m_manufacturer_id == manufacturer_id
            && mv->m_product_id == product_id )
            return mv;

       list = g_list_next( list );
     }

  return 0;
}


cIpmiMcVendor *
cIpmiMcVendorFactory::Get( unsigned int manufacturer_id,
                           unsigned int product_id )
{
  cIpmiMcVendor *mv = Find( manufacturer_id, product_id );

  if ( mv )
       return mv;

  return m_default;
}


cIpmiMcVendor::cIpmiMcVendor( unsigned int manufacturer_id,
                              unsigned int product_id,
                              const char *desc )
  : m_manufacturer_id( manufacturer_id ),
    m_product_id( product_id )
{
  strncpy( m_description, desc, 79 );
  m_description[79] = 0;
}


cIpmiMcVendor::~cIpmiMcVendor()
{
}


bool
cIpmiMcVendor::InitMc( cIpmiMc * /*mc*/, const cIpmiMsg & /*devid*/ )
{
  return true;
}


void
cIpmiMcVendor::CleanupMc( cIpmiMc * /*mc*/ )
{
}


bool
cIpmiMcVendor::CreateRdrs( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  if ( CreateSensors( domain, source_mc, sdrs ) == false )
       return false;

  if ( CreateControls( domain, source_mc, sdrs ) == false )
       return false;

  if ( CreateSels( domain, source_mc, sdrs ) == false )
       return false;

  if ( CreateFrus( domain, source_mc, sdrs ) == false )
       return false;

  return true;
}


cIpmiEntity *
cIpmiMcVendor::FindOrCreateEntity( cIpmiDomain *domain, cIpmiMc *mc, int lun,
                                   tIpmiEntityId entity_id, unsigned int entity_instance,
                                   bool came_from_sdr, cIpmiSdr *sdr )
{
  assert( mc );

  tIpmiDeviceNum device_num;

  if ( mc && entity_instance >= 0x60 )
     {
       device_num.channel = mc->GetChannel();
       device_num.address = mc->GetAddress();
       entity_instance   -= 0x60;
     }
  else
     {
       device_num.channel = 0;
       device_num.address = 0;
     }

  cIpmiEntity *ent = domain->FindEntity( device_num, entity_id, entity_instance );

  if ( ent )
     {
       // If it came from an SDR, it always will have come from an SDR.
       if ( !ent->CameFromSdr() )
	    ent->CameFromSdr() = came_from_sdr;

       ent->AccessAddress() = mc->GetAddress();
       ent->Channel()       = mc->GetChannel();
       ent->Lun()           = lun;

       return ent;
    }

  return CreateEntity( domain, mc, device_num, lun, entity_id, entity_instance,
                       came_from_sdr, sdr );
}


cIpmiEntity *
cIpmiMcVendor::CreateEntity( cIpmiDomain *domain, cIpmiMc *mc,
                             tIpmiDeviceNum device_num, int lun,
                             tIpmiEntityId entity_id, unsigned int entity_instance,
                             bool came_from_sdr, cIpmiSdr *sdr )
{
  // create a new entity
  cIpmiEntity *ent = new cIpmiEntity( domain, device_num, entity_id, entity_instance, came_from_sdr );
  assert( ent );

  domain->AddEntity( ent );

  ent->AccessAddress() = mc->GetAddress();
  ent->Channel()       = mc->GetChannel();
  ent->Lun()           = lun;

  if ( sdr && sdr->m_type == eSdrTypeMcDeviceLocatorRecord )
     {
       // set mc id string
       ent->IdString().SetIpmi( sdr->m_data + 15 );
       ent->Oem() = sdr->m_data[14];

       // TODO: additional fields like m_acpi_system_power_notify_required ...
     }

  // create rpt entry
  stdlog << "adding entity: " << ent->EntityId() << "." << ent->EntityInstance()
	 << " (" << IpmiEntityIdToString( entity_id ) << ").\n";

  struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return 0;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type = oh_event::OH_ET_RESOURCE;

  if ( ent->CreateResource( e->u.res_event.entry ) == false )
     {
       g_free( e );
       return 0;
     }

  // assign the hpi resource id to ent, so we can find
  // the resource for a given entity
  ent->m_resource_id = e->u.res_event.entry.ResourceId;

  // add the entity to the resource cache
  int rv = oh_add_resource( domain->GetHandler()->rptcache,
			    &(e->u.res_event.entry), ent, 1 );
  assert( rv == 0 );

  domain->AddHpiEvent( e );

  return ent;
}


static cIpmiSensor *
FindSensor( GList *list, unsigned int num, unsigned char lun )
{
  for( ; list; list = g_list_next( list ) )
     {
       cIpmiSensor *sensor = (cIpmiSensor *)list->data;
       
       if (    sensor->Num() == num
            && sensor->Lun() == lun )
            return sensor;
     }

  return 0;
}


bool
cIpmiMcVendor::CreateSensors( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  GList *old_sensors = domain->GetSdrSensors( source_mc );
  GList *new_sensors = 0;
  GList *sensors     = GetSensorsFromSdrs( domain, source_mc, sdrs );

  while( sensors )
     {
       cIpmiSensor *sensor = (cIpmiSensor *)sensors->data;
       sensors = g_list_remove( sensors, sensor );

       cIpmiSensor *old_sensor = FindSensor( old_sensors, sensor->Num(), sensor->Lun() );

       if ( old_sensor && sensor->Cmp( *old_sensor ) )
          {
            // sensor already there, use old one
            delete sensor;
            old_sensor->HandleNew( domain );
            old_sensors = g_list_remove( old_sensors, old_sensor );
            new_sensors = g_list_append( new_sensors, old_sensor );
            continue;
          }

       if ( old_sensor )
          {
            // remove the old sensor
            old_sensors = g_list_remove( old_sensors, old_sensor );
            old_sensor->Entity()->Rem( old_sensor );
            delete old_sensor;
          }

       // check if the sensor is defined twice
       if ( FindSensor( new_sensors, sensor->Num(), sensor->Lun() ) )
          {
            stdlog << "sensor " << sensor->IdString() << " define twice in SDR !\n";
            delete sensor;
            continue;
          }

       cIpmiSdr *mcdlr = sdrs->FindSdr( sensor->Mc() );

       // create entity
       cIpmiEntity *ent = FindOrCreateEntity( domain, sensor->Mc(), sensor->Lun(),
                                              sensor->EntityId(), sensor->EntityInstance(),
                                              true, mcdlr );

       assert( ent );
       new_sensors = g_list_append( new_sensors, sensor );
       sensor->HandleNew( domain );
       ent->Add( sensor );
     }

  // destry old sensors
  while( old_sensors )
     {
       cIpmiSensor *sensor = (cIpmiSensor *)old_sensors->data;
       old_sensors = g_list_remove( old_sensors, sensor );
       sensor->Entity()->Rem( sensor );
       delete sensor;
     }

  // set new sdr sensors
  domain->SetSdrSensors( source_mc, new_sensors );

  return true;
}


GList *
cIpmiMcVendor::GetSensorsFromSdrs( cIpmiDomain *domain, cIpmiMc *source_mc,
                                   cIpmiSdrs *sdrs )
{
  GList *sensors = 0;

  // create a list of full sensor records
  for( unsigned int i = 0; i < sdrs->NumSdrs(); i++ )
     {
       GList *records = 0;

       cIpmiSdr *sdr = sdrs->Sdr( i );

       if ( sdr->m_type == eSdrTypeFullSensorRecord )
          {
            cIpmiSdr *s = new cIpmiSdr;
            *s = *sdr;
            records = g_list_append( records, s );
          }
       else if ( sdr->m_type == eSdrTypeCompactSensorRecord )
          {
            GList *r = ConvertToFullSensorRecords( domain, source_mc, sdr );

            if ( r )
                 records = g_list_concat( records, r );
          }

       // create sensors for each full sensor record
       while( records )
          {
            cIpmiSdr *s = (cIpmiSdr *)records->data;

            GList *l = CreateSensorFromFullSensorRecord( domain, source_mc, s, sdrs );

            if ( l )
                 sensors = g_list_concat( sensors, l );

            records = g_list_remove( records, s );
            delete s;
          }
     }

  return sensors;
}


// this routine is completly untested !!!
GList *
cIpmiMcVendor::ConvertToFullSensorRecords( cIpmiDomain *domain, cIpmiMc * /*_source_mc*/, cIpmiSdr *sdr )
{
  int n = 1;

  if ( sdr->m_data[23] & 0x0f )
       n = sdr->m_data[23] & 0x0f;

  GList *list = 0;

  for( int i = 0; i < n; i++ )
     {
       cIpmiSdr *s = new cIpmiSdr;
       *s = *sdr;

       memset( s->m_data + 23, 0, dMaxSdrData - 23 );

       // sensor num
       s->m_data[7] = sdr->m_data[7] + i;

       // entity instance
       if ( sdr->m_data[24] & 0x80 )
            s->m_data[9] = sdr->m_data[9] + i;

       // positive-going threshold hysteresis value
       s->m_data[42] = sdr->m_data[25];
       // negativ-going threshold hysteresis value
       s->m_data[43] = sdr->m_data[26];

       // oem
       s->m_data[46] = sdr->m_data[30];

       // id
       int len = sdr->m_data[31] & 0x3f;
       int val = (sdr->m_data[24] & 0x7f) + i;

       memcpy( s->m_data + 47, sdr->m_data + 31, len + 1 );

       int base  = 0;
       int start = 0;

       if ( sdr->m_data[23] & 0x30 == 0 )
          {
            // numeric
            base  = 10;
            start = '0';
          }
       else if ( sdr->m_data[23] & 0x30 == 0x10 )
          {
            // alpha
            base  = 26;
            start = 'A';
          }

       if ( base )
          {
            // add id string postfix
            if ( val / base > 0 )
               {
                 s->m_data[48+len] = (val / base) + start;
                 len++;
               }

            s->m_data[48+len] = (val % base) + start;
            len++;
            s->m_data[48+len] = 0;
            s->m_data[47] = (sdr->m_data[31] & 0xc0) | len;
          }

       list = g_list_append( list, s );
     }

  return list;
}


GList *
cIpmiMcVendor::CreateSensorFromFullSensorRecord( cIpmiDomain *domain, cIpmiMc *source_mc,
                                                 cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  GList *list = 0;

  tIpmiSensorType sensor_type = (tIpmiSensorType)sdr->m_data[12];

  if ( sensor_type == eIpmiSensorTypeAtcaHotSwap )
       list = CreateSensorHotswap( domain, source_mc, sdr, sdrs );
  else
     {
       tIpmiEventReadingType reading_type = (tIpmiEventReadingType)sdr->m_data[13];

       if ( reading_type == eIpmiEventReadingTypeThreshold )
            list = CreateSensorThreshold( domain, source_mc, sdr, sdrs );
       else
            list = CreateSensorDefault( domain, source_mc, sdr, sdrs );
     }

  return list;
}


GList *
cIpmiMcVendor::CreateSensorHotswap( cIpmiDomain *domain, cIpmiMc *source_mc,
                                    cIpmiSdr *sdr, cIpmiSdrs * /*sdrs*/ )
{
  cIpmiMc *mc = FindMcBySdr( domain, sdr );
  assert( mc );

  cIpmiSensorHotswap *hs = new cIpmiSensorHotswap( mc );
  hs->SourceMc() = source_mc;

  if ( !hs->GetDataFromSdr( mc, sdr ) )
     {
       delete hs;
       return 0;
     }

  return g_list_append( 0, hs );
}


GList *
cIpmiMcVendor::CreateSensorThreshold( cIpmiDomain *domain, cIpmiMc *source_mc,
                                      cIpmiSdr *sdr, cIpmiSdrs * /*sdrs*/ )
{
  cIpmiMc *mc = FindMcBySdr( domain, sdr );
  assert( mc );
  
  cIpmiSensorThreshold *ts = new cIpmiSensorThreshold( mc );
  ts->SourceMc() = source_mc;

  if ( !ts->GetDataFromSdr( mc, sdr ) )
     {
       delete ts;

       return 0;
     }

  return g_list_append( 0, ts );
}


GList *
cIpmiMcVendor::CreateSensorDiscrete( cIpmiDomain *domain, cIpmiMc *source_mc,
                                     cIpmiSdr *sdr, cIpmiSdrs * /*sdrs*/ )
{
  cIpmiMc *mc = FindMcBySdr( domain, sdr );
  assert( mc );

  cIpmiSensorDiscrete *ds = new cIpmiSensorDiscrete( mc );
  ds->SourceMc() = source_mc;

  if ( !ds->GetDataFromSdr( mc, sdr ) )
     {
       delete ds;

       return 0;
     }

  return g_list_append( 0, ds );
}


GList *
cIpmiMcVendor::CreateSensorDefault( cIpmiDomain *domain, cIpmiMc *source_mc,
                                    cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  return CreateSensorDiscrete( domain, source_mc, sdr, sdrs );
}


cIpmiMc *
cIpmiMcVendor::FindMcBySdr( cIpmiDomain *domain, cIpmiSdr *sdr )
{
  switch( sdr->m_type )
     {
       case eSdrTypeFullSensorRecord:
       case eSdrTypeMcDeviceLocatorRecord:
       case eSdrTypeFruDeviceLocatorRecord:
            return domain->FindOrCreateMcBySlaveAddr( sdr->m_data[5] );

       default:
            break;
     }

  assert( 0 );

  return 0;
}


bool
cIpmiMcVendor::CreateControls( cIpmiDomain *domain, cIpmiMc *source_mc, 
                               cIpmiSdrs *sdrs )
{
  // controls only for device SDR
  if ( source_mc == 0 )
       return true;

  if ( domain->IsAtca() )
     {
       unsigned int mc_type = domain->GetMcType( source_mc->GetAddress() );
       return CreateControlsAtca( domain, source_mc, sdrs, mc_type );
     }

  return true;
}


bool
cIpmiMcVendor::CreateControlsAtca( cIpmiDomain *domain, cIpmiMc *mc, cIpmiSdrs *sdrs,
                                   unsigned int mc_type )
{
  // TODO: check if there is already a fan control

  // can only create fan controls
  if ( (mc_type & dIpmiMcTypeBitFan) == 0 )
       return true;

  // find the mcdlr
  cIpmiSdr *mcdlr = sdrs->FindSdr( mc );

  if ( mcdlr == 0 )
     {
       stdlog << "cannot find MC device locator record for ATCA MC !\n";
       return true;
     }

  // find entity
  tIpmiEntityId ent_id       = (tIpmiEntityId)mcdlr->m_data[12];
  unsigned int  ent_instance =  mcdlr->m_data[13];

  cIpmiEntity *ent = FindOrCreateEntity( domain, mc, 0, ent_id, 
                                         ent_instance, true, mcdlr );

  // cannot find or create ent
  if ( ent == 0 )
       return true;

  // create ATCA fan
  if ( mc_type & dIpmiMcTypeBitFan )
     {
       if ( CreateControlAtcaFan( domain, mc, sdrs, ent ) == false )
            return false;
     }

  return true;
}


bool
cIpmiMcVendor::CreateControlAtcaFan( cIpmiDomain *domain, cIpmiMc *mc, cIpmiSdrs *sdrs,
                                     cIpmiEntity *ent )
{
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetFanSpeedProperties );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = 0;
  msg.m_data_len = 2;

  cIpmiMsg rsp;

  int rv = mc->SendCommand( msg, rsp );

  if (    rv
       || rsp.m_data_len < 6
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "cannot send get fan speed properties !\n";
       return true;
     }

  unsigned int min      = rsp.m_data[2];
  unsigned int max      = rsp.m_data[3];
  unsigned int def      = rsp.m_data[4];
  bool         auto_adj = rsp.m_data[5] & 0x80;

  cIpmiControlFan *f = new cIpmiControlFan( mc, ent->GetControlNum(),
                                            min, max, def,
                                            auto_adj );

  f->IdString().SetAscii( "ATCA-Fan", SAHPI_TL_TYPE_LANGUAGE, SAHPI_LANG_ENGLISH );

  ent->Add( f );

  return true;
}


bool
cIpmiMcVendor::CreateFrus( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  for( unsigned int i = 0; i < sdrs->NumSdrs(); i++ )
     {
       cIpmiSdr *sdr = sdrs->Sdr( i );

       if ( sdr->m_type == eSdrTypeMcDeviceLocatorRecord )
          {
            // fru inventory device ?
            if ( sdr->m_data[2] == 0x01 )
               {
                 // old IPMI 1.0 mcdlr
                 if ( (sdr->m_data[7] & 8) == 0 )
                      continue;

                 stdlog << "found old IPMI 1.0 MC device locator record.\n";
               }
            else
               {
                 if ( (sdr->m_data[8] & 8) == 0 )
                      continue;
               }
          }
       else if ( sdr->m_type != eSdrTypeFruDeviceLocatorRecord )
            continue;

       if ( CreateFru( domain, source_mc, sdr, sdrs ) == false )
            return false;
     }

  return true;
}


bool
cIpmiMcVendor::CreateFru( cIpmiDomain *domain, cIpmiMc *mc, cIpmiSdr *sdr, cIpmiSdrs * /*sdrs*/ )
{
  unsigned int fru_id;
  unsigned int lun;

  tIpmiEntityId id       = (tIpmiEntityId)sdr->m_data[12];
  unsigned int  instance = sdr->m_data[13];

  if ( sdr->m_type == eSdrTypeMcDeviceLocatorRecord )
     {
       fru_id = 0;
       lun    = 0;
     }
  else
     {
       fru_id = sdr->m_data[6];
       lun    = (sdr->m_data[7] >> 3) & 3;
     }

  // create mc/ domain if nessesary
  cIpmiMc *m = FindMcBySdr( domain, sdr );
  assert( m );

  cIpmiEntity *ent = FindOrCreateEntity( domain, m, lun, id, instance, true, sdr );
  assert( ent );

  cIpmiFru *fru = (cIpmiFru *)ent->Find( m, SAHPI_INVENTORY_RDR, fru_id );
  bool need_add = false;

  if ( fru == 0 )
     {
       fru = new cIpmiFru( m, fru_id );

       fru->IdString().SetIpmi( sdr->m_data + 15 );
       fru->Oem() = sdr->m_data[14];

       fru->Entity() = ent;
       need_add = true;
     }

  int rv = fru->Fetch();

  if ( rv )
     {
       if ( need_add )
            delete fru;

       return false;
     }

  fru->CalcSize();

  if ( !need_add )
       return true;

  ent->Add( fru );

  return true;
}


bool
cIpmiMcVendor::CreateSels( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  if ( source_mc == 0 )
       return false;

  cIpmiSdr *mcdlr = sdrs->FindSdr( source_mc );

  if ( mcdlr == 0 )
       return true;

  tIpmiEntityId id       = (tIpmiEntityId)mcdlr->m_data[12];
  unsigned int  instance = mcdlr->m_data[13];

  cIpmiEntity *ent = FindOrCreateEntity( domain, source_mc, 0, id, instance, true, mcdlr );
  assert( ent );

  cIpmiSel *sel = source_mc->Sel();

  // check this
  if ( ent->Sel() != 0 )
     {
       stdlog << "ups: only one SEL per entity allowd !\n";
       return true;
     }
  
  assert( ent->Sel() == 0 );
  ent->Sel() = sel;

  assert( sel->Entity() == 0 );
  sel->Entity() = ent;

  // create hpi sel
  stdlog << "adding SEL\n";

  dbg( "adding SEL %d.%d (%s)",
       ent->EntityId(), ent->EntityInstance(),
       IpmiEntityIdToString( ent->EntityId() ) );

   // find resource
  SaHpiRptEntryT *resource = ent->Domain()->FindResource( ent->m_resource_id );

  if ( !resource )
     {
       assert( 0 );
       return true;
     }

  assert( (resource->ResourceCapabilities & SAHPI_CAPABILITY_SEL ) == 0 );

  // update resource
  resource->ResourceCapabilities |= SAHPI_CAPABILITY_SEL;

  struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       return true;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type               = oh_event::OH_ET_RESOURCE;
  e->u.res_event.entry = *resource;

  ent->Domain()->AddHpiEvent( e );

  return true;
}
