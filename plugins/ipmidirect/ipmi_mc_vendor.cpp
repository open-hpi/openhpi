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


SaHpiEntityInstanceT cIpmiMcVendor::m_unique_instance = 256;


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
  if ( CreateResources( domain, source_mc, sdrs ) == false )
       return false;

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


bool
cIpmiMcVendor::CreateResources( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  bool found = false;

  // create one resource per mcdlr and fdlr
  for( unsigned int i = 0; i < sdrs->NumSdrs(); i++ )
     {
       cIpmiSdr *sdr = sdrs->Sdr( i );

       cIpmiAddr addr;
       unsigned int fru_id = 0;

       if ( sdr->m_type == eSdrTypeMcDeviceLocatorRecord )
	  {
	    addr.m_slave_addr = sdr->m_data[5];
	    addr.m_channel    = sdr->m_data[6] & 0xf;

	    found = true;
	  }
/*
       else if ( sdr->m_type == eSdrTypeFruDeviceLocatorRecord )
	  {
	    if ( (sdr->m_data[7] & 0x80) == 0 )
		 // only logical frus are supported
		 continue;

	    addr = source_mc->Addr();
	    fru_id = sdr->m_data[6];
	  }
*/
       else
	    continue;

       cIpmiMc *mc = domain->FindMcByAddr( addr );

       // TODO: create mc !!!
       assert( mc );

       if ( !mc )
	    // read mcdlr for other mc than source_mc
	    return false;

       if ( FindOrCreateResource( domain, mc, fru_id, sdr, sdrs ) == false )
	    return false;
     }

  // there must be a mcdlr !
  if ( !found )
     {
       assert( found );

       if ( FindOrCreateResource( domain, source_mc, 0, 0, sdrs ) == false )
	    return false;
     }

  return true;
}


cIpmiResource *
cIpmiMcVendor::FindOrCreateResource( cIpmiDomain *domain, cIpmiMc *mc,
				     unsigned int fru_id,
				     cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  assert( mc );

  cIpmiResource *res = mc->FindResource( fru_id );

  if ( res )
       return res;

  return CreateResource( domain, mc, fru_id, sdr, sdrs );
}


cIpmiResource *
cIpmiMcVendor::CreateResource( cIpmiDomain *domain, cIpmiMc *mc,
			       unsigned int fru_id, cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  // create a new resource
  cIpmiResource *res = new cIpmiResource( mc, fru_id );
  assert( res );

  SaHpiEntityTypeT type;
  SaHpiEntityInstanceT instance;

  if ( sdr )
     {
       type     = (SaHpiEntityTypeT)sdr->m_data[12];
       instance = (SaHpiEntityInstanceT)sdr->m_data[13];
     }
  else
     {
       type = SAHPI_ENT_UNKNOWN;
       instance = GetUniqueInstance();
     }

  res->EntityPath() = CreateEntityPath( domain, mc->GetAddress(), fru_id,
                                        type, instance, sdrs );

  if (    sdr
       && (    sdr->m_type == eSdrTypeMcDeviceLocatorRecord 
            || sdr->m_type == eSdrTypeFruDeviceLocatorRecord ) )
     {
       // set mc id string
       res->ResourceTag().SetIpmi( sdr->m_data + 15 );
       res->Oem() = sdr->m_data[14];

       // TODO: additional fields like m_acpi_system_power_notify_required ...
     }

  // create rpt entry
  stdlog << "adding resource: " << res->EntityPath() << ".\n";

  struct oh_event *e = (struct oh_event *)g_malloc0( sizeof( struct oh_event ) );

  if ( !e )
     {
       stdlog << "out of space !\n";
       delete res;
       return 0;
     }

  memset( e, 0, sizeof( struct oh_event ) );
  e->type = oh_event::OH_ET_RESOURCE;

  if ( res->Create( e->u.res_event.entry ) == false )
     {
       g_free( e );
       delete res;
       return 0;
     }

  // add res to mc
  mc->AddResource( res );

  // assign the hpi resource id to ent, so we can find
  // the resource for a given entity
  res->m_resource_id = e->u.res_event.entry.ResourceId;

  // add the entity to the resource cache
  int rv = oh_add_resource( domain->GetHandler()->rptcache,
			    &(e->u.res_event.entry), res, 1 );
  assert( rv == 0 );

  domain->AddHpiEvent( e );

  return res;
}


// TODO:
//    Handling of entity association records.
//    This is the reason for the parameter sdrs.
cIpmiEntityPath 
cIpmiMcVendor::CreateEntityPath( cIpmiDomain *domain, unsigned int mc_addr, unsigned int fru_id, 
                                 SaHpiEntityTypeT type,
                                 SaHpiEntityInstanceT instance, cIpmiSdrs * /*sdrs*/ )
{
  // find fru info
  cIpmiFruInfo *fi = domain->FindFruInfo( mc_addr, fru_id );

  cIpmiEntityPath bottom;

  if ( instance < 0x60 )
     {
       if ( fi )
            // if there is fru info => slot id already in the entity path
            instance = 0;
     }
  else if ( instance <= 0x7f )
       instance -= 0x60;

  bottom.SetEntry( 0, type, instance );

  bottom.AppendRoot();

  cIpmiEntityPath top = domain->EntityRoot();

  if ( fi )
       return fi->CreateEntityPath( top, bottom );

  // fru info not found => use default entity path
  cIpmiEntityPath ep = bottom;
  ep += top;
  ep.AppendRoot();

  return ep;
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
            old_sensor->Resource()->Rem( old_sensor );
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

       // create resource
       cIpmiResource *res = FindOrCreateResource( domain, sensor->Mc(), 0, mcdlr, sdrs );

       assert( res );
       new_sensors = g_list_append( new_sensors, sensor );
       sensor->HandleNew( domain );
       res->Add( sensor );
     }

  // destry old sensors
  while( old_sensors )
     {
       cIpmiSensor *sensor = (cIpmiSensor *)old_sensors->data;
       old_sensors = g_list_remove( old_sensors, sensor );
       sensor->Resource()->Rem( sensor );
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
                                    cIpmiSdr *sdr, cIpmiSdrs *sdrs )
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

  CreateSensorEntityPath( domain, hs, source_mc, sdr, sdrs );

  return g_list_append( 0, hs );
}


GList *
cIpmiMcVendor::CreateSensorThreshold( cIpmiDomain *domain, cIpmiMc *source_mc,
                                      cIpmiSdr *sdr, cIpmiSdrs *sdrs )
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

  CreateSensorEntityPath( domain, ts, source_mc, sdr, sdrs );

  return g_list_append( 0, ts );
}


GList *
cIpmiMcVendor::CreateSensorDiscrete( cIpmiDomain *domain, cIpmiMc *source_mc,
                                     cIpmiSdr *sdr, cIpmiSdrs *sdrs )
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

  CreateSensorEntityPath( domain, ds, source_mc, sdr, sdrs );

  return g_list_append( 0, ds );
}


GList *
cIpmiMcVendor::CreateSensorDefault( cIpmiDomain *domain, cIpmiMc *source_mc,
                                    cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  return CreateSensorDiscrete( domain, source_mc, sdr, sdrs );
}


void 
cIpmiMcVendor::CreateSensorEntityPath( cIpmiDomain *domain, cIpmiSensor *s, 
                                       cIpmiMc *source_mc,
                                       cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  SaHpiEntityTypeT     type;
  SaHpiEntityInstanceT instance;

  if ( sdr )
     {
       type     = (SaHpiEntityTypeT)sdr->m_data[8];
       instance = (SaHpiEntityInstanceT)sdr->m_data[9];
     }
  else
     {
       type     = SAHPI_ENT_UNKNOWN;
       instance = GetUniqueInstance();
     }

  s->EntityPath() = CreateEntityPath( domain, source_mc->GetAddress(), 0,
                                      type, instance, sdrs );
}


cIpmiMc *
cIpmiMcVendor::FindMcBySdr( cIpmiDomain *domain, cIpmiSdr *sdr )
{
  switch( sdr->m_type )
     {
       case eSdrTypeCompactSensorRecord:
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
       cIpmiFruInfo *fi= domain->FindFruInfo( source_mc->GetAddress(), 0 );
       
       if ( fi == 0 )
          {
            assert( 0 );
            return true;
          }

       return CreateControlsAtca( domain, source_mc, sdrs, fi->Site() );
     }

  return true;
}


bool
cIpmiMcVendor::CreateControlsAtca( cIpmiDomain *domain, cIpmiMc *mc, cIpmiSdrs *sdrs,
                                   tIpmiAtcaSiteType type )
{
  // TODO: check if there is already a fan control

  // can only create fan controls
  if ( type != eIpmiAtcaSiteTypeFanTray )
       return true;

  // find the mcdlr
  cIpmiSdr *mcdlr = sdrs->FindSdr( mc );

  if ( mcdlr == 0 )
     {
       stdlog << "cannot find MC device locator record for ATCA MC !\n";
       return true;
     }

  // find resource
  cIpmiResource *res = FindOrCreateResource( domain, mc, 0, 
					     mcdlr, sdrs );

  // cannot find or create ent
  if ( res == 0 )
       return true;

  // create ATCA fan
  if ( CreateControlAtcaFan( domain, res, sdrs ) == false )
       return false;

  return true;
}


bool
cIpmiMcVendor::CreateControlAtcaFan( cIpmiDomain *domain, cIpmiResource *res,
				     cIpmiSdrs *sdrs )
{
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetFanSpeedProperties );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = 0;
  msg.m_data_len = 2;

  cIpmiMsg rsp;

  int rv = res->SendCommand( msg, rsp );

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

  cIpmiControlFan *f = new cIpmiControlFan( res->Mc(), res->GetControlNum(),
                                            min, max, def,
                                            auto_adj );

  f->IdString().SetAscii( "ATCA-Fan", SAHPI_TL_TYPE_LANGUAGE, SAHPI_LANG_ENGLISH );

  res->Add( f );

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
cIpmiMcVendor::CreateFru( cIpmiDomain *domain, cIpmiMc *mc, cIpmiSdr *sdr, cIpmiSdrs *sdrs )
{
  unsigned int fru_id;
  unsigned int lun;

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

  cIpmiResource *res = FindOrCreateResource( domain, m, 0 /*fru_id*/, sdr, sdrs );
  assert( res );

  cIpmiFru *fru = (cIpmiFru *)res->Find( m, SAHPI_INVENTORY_RDR, fru_id );
  bool need_add = false;

  if ( fru == 0 )
     {
       fru = new cIpmiFru( m, fru_id );

       fru->IdString().SetIpmi( sdr->m_data + 15 );
       fru->Oem() = sdr->m_data[14];

       fru->Resource() = res;
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

  res->Add( fru );

  return true;
}


bool
cIpmiMcVendor::CreateSels( cIpmiDomain *domain, cIpmiMc *source_mc, cIpmiSdrs *sdrs )
{
  if ( source_mc == 0 )
       return false;

  if ( !source_mc->SelDeviceSupport() )
       return true;

  cIpmiSdr *mcdlr = sdrs->FindSdr( source_mc );

  if ( mcdlr == 0 )
       return true;

  cIpmiResource *res = FindOrCreateResource( domain, source_mc, 0, mcdlr, sdrs );
  assert( res );

  // create hpi sel
  stdlog << "adding SEL " << res->EntityPath() << "\n";

   // find resource
  SaHpiRptEntryT *resource = domain->FindResource( res->m_resource_id );

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

  domain->AddHpiEvent( e );

  return true;
}
