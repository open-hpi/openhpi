/*
 * ipmi_sdr.cpp
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

#include "ipmi_mc.h"
#include "ipmi_cmd.h"
#include "ipmi_log.h"
#include "ipmi_utils.h"
#include "ipmi_text_buffer.h"
#include "ipmi_sensor_threshold.h"


static const char *repository_sdr_update_map[] =
{
  "Unspecified",
  "NonModal",
  "Modal",
  "Both"
};

static int repository_sdr_update_num =   sizeof( repository_sdr_update_map )
                                       / sizeof( char * );

const char *
IpmiRepositorySdrUpdateToString( tIpmiRepositorySdrUpdate val )
{
  if ( (int)val >= repository_sdr_update_num )
       return "Invalid";
  
  return repository_sdr_update_map[val];
}


struct cIpmiSdrTypeToName
{
  tIpmiSdrType m_type;
  const char  *m_name;
};


static cIpmiSdrTypeToName type_to_name[] =
{
  { eSdrTypeFullSensorRecord                     , "FullSensor" },
  { eSdrTypeCompactSensorRecord                  , "CompactSensor" },
  { eSdrTypeEntityAssociationRecord              , "EntityAssociation" },
  { eSdrTypeDeviceRelativeEntityAssociationRecord, "DeviceRelativeEntityAssociation" },
  { eSdrTypeGenericDeviceLocatorRecord           , "GenericDeviceLocator" },
  { eSdrTypeFruDeviceLocatorRecord               , "FruDeviceLocator" },
  { eSdrTypeMcDeviceLocatorRecord                , "McDeviceLocator" },
  { eSdrTypeMcConfirmationRecord                 , "McConfirmation" },
  { eSdrTypeBmcMessageChannelInfoRecord          , "BmcMessageChannelInfo" },
  { eSdrTypeOemRecord                            , "Oem" },
  { eSdrTypeUnknown, 0 }
};


const char *
IpmiSdrTypeToName( tIpmiSdrType type )
{
  for( cIpmiSdrTypeToName *t = type_to_name; t->m_name; t++ )
       if ( t->m_type == type )
            return t->m_name;

  return "Invalid";
}


static bool
Bit( unsigned char v, int bit )
{
  return v & (1<<bit);
}


void 
cIpmiSdr::DumpFullSensor( cIpmiLog &dump ) const
{
  char str[80];

  dump.Entry( "SlaveAddress" ) << m_data[5] << ";\n";    
  dump.Entry( "Channel" ) << (int)(m_data[6] >> 4) << ";\n";
  dump.Entry( "Lun" ) << (int)(m_data[6] & 0x3) << ";\n";
  dump.Entry( "SensorNum" ) << m_data[7] << ";\n";

  tIpmiEntityId id = (tIpmiEntityId)m_data[8];

  if ( !strcmp( IpmiEntityIdToString( id ), "Invalid" ) )
       sprintf( str, "0x%02x", id );
  else
       sprintf( str, "dIpmiEntityId%s", IpmiEntityIdToString( id ) );

  dump.Entry( "EntityId" ) << str << ";\n";
  dump.Entry( "EntityInstance" ) << (int)m_data[9] << ";\n";

  dump.Entry( "InitScanning" ) << Bit( m_data[10], 6 ) << ";\n";
  dump.Entry( "InitEvents" ) << Bit( m_data[10], 5 ) << ";\n";
  dump.Entry( "InitThresholds" ) << Bit( m_data[10], 4 ) << ";\n";
  dump.Entry( "InitHysteresis" ) << Bit( m_data[10], 3 ) << ";\n";
  dump.Entry( "InitSensorType" ) << Bit( m_data[10], 2 ) << ";\n";
  dump.Entry( "SensorInitPuEvents" ) << Bit( m_data[10], 1 ) << ";\n";
  dump.Entry( "SensorInitPuScanning" ) << Bit( m_data[10], 0 ) << ";\n";
  dump.Entry( "IgnoreIfNoEntity" ) << Bit( m_data[11], 7 ) << ";\n";
  dump.Entry( "SupportsAutoRearm" ) << Bit( m_data[11], 6 ) << ";\n";

  tIpmiHysteresisSupport hs = (tIpmiHysteresisSupport)((m_data[11] >> 4) & 3);
  dump.Entry( "HysteresisSupport" ) << "dIpmiHysteresisSupport" 
       << IpmiHysteresisSupportToString( hs ) << ";\n";

  tIpmiThresholdAccessSuport ts = (tIpmiThresholdAccessSuport)((m_data[11] >> 2) & 3);
  dump.Entry( "ThresholdAccess" ) << "dIpmiThresholdAccessSupport" 
       << IpmiThresholdAccessSupportToString( ts ) << ";\n";

  tIpmiEventSupport es = (tIpmiEventSupport)(m_data[11] & 3);
  dump.Entry( "EventSupport" ) << "dIpmiEventSupport"
       << IpmiEventSupportToString( es ) <<  ";\n";

  tIpmiSensorType sensor_type = (tIpmiSensorType)m_data[12];

  if ( !strcmp( IpmiSensorTypeToString( sensor_type ), "Invalid" ) )
       sprintf( str, "0x%02x", sensor_type );
  else
       sprintf( str, "dIpmiSensorType%s", IpmiSensorTypeToString( sensor_type ) );

  dump.Entry( "SensorType" ) << str << ";\n";

  tIpmiEventReadingType reading_type = (tIpmiEventReadingType)m_data[13];

  if ( !strcmp( IpmiEventReadingTypeToString( reading_type ), "Invalid" ) )
       sprintf( str, "0x%02x", reading_type );
  else
       sprintf( str, "dIpmiEventReadingType%s", IpmiEventReadingTypeToString( reading_type ) );

  dump.Entry( "EventReadingType" ) << str << ";\n";

  if ( reading_type == eIpmiEventReadingTypeThreshold )
     {
       // assertion
       unsigned short em = IpmiGetUint16( m_data + 14 );
       IpmiThresholdEventMaskToString( em, str );

       if ( str[0] == 0 )
            strcat( str, "0" );

       dump.Entry( "AssertionEventMask" ) << str << ";\n";

       sprintf( str, "0x%04x", em >> 12 );
       dump.Entry( "LowerThresholdReadingMask" ) << str << ";\n";

       // deassertion
       em = IpmiGetUint16( m_data + 16 );
       IpmiThresholdEventMaskToString( em, str );

       if ( str[0] == 0 )
            strcat( str, "0" );

       dump.Entry( "DeassertionEventMask" ) << str << ";\n";

       sprintf( str, "0x%04x", em >> 12 );
       dump.Entry( "UpperThresholdReadingMask" ) << str << ";\n";

       // settable threshold
       em = IpmiGetUint16( m_data + 18 );

       IpmiThresholdMaskToString( em >> 8, str );

       if ( str[0] == 0 )
            strcat( str, "0" );

       dump.Entry( "SettableThresholdsMask" ) << str <<  ";\n";

       IpmiThresholdMaskToString( em & 0xff, str );

       if ( str[0] == 0 )
            strcat( str, "0" );

       dump.Entry( "ReadableThresholdsMask" ) << str << ";\n";

       tIpmiRateUnit ru = (tIpmiRateUnit)((m_data[20] >> 3) & 7);
       dump.Entry( "RateUnit" ) << "dIpmRateUnit" << IpmiRateUnitToString( ru ) << ";\n";

       tIpmiModifierUnit mu = (tIpmiModifierUnit)( (m_data[20] >> 1) & 3);
       dump.Entry( "ModifierUnit" ) << "dIpmiModifierUnit"
            << IpmiModifierUnitToString( mu ) << ";\n";
       dump.Entry( "Percentage" ) << ((m_data[20] & 1) == 1) << ";\n";

       dump.Entry( "BaseUnit" ) << "dIpmiSensorUnit"
            << IpmiUnitTypeToString( (tIpmiUnitType)m_data[21] ) << ";\n";
       dump.Entry( "ModifierUnit2" ) << "dIpmiSensorUnit"
            << IpmiUnitTypeToString( (tIpmiUnitType)m_data[22] ) << ";\n";

       cIpmiSensorFactors sf;
       sf.GetDataFromSdr( this );

       dump.Entry( "AnalogDataFormat" ) << "dIpmiAnalogDataFormat"
            << IpmiAnalogeDataFormatToString( sf.AnalogDataFormat() ) << ";\n";

       dump.Entry( "Linearization" ) << "dIpmiLinearization"
            << IpmiLinearizationToString( sf.Linearization() ) << ";\n";

       dump.Entry( "M" ) << sf.M() << ";\n";
       dump.Entry( "Tolerance" ) << sf.Tolerance() << ";\n";
       dump.Entry( "B" ) << sf.B() << ";\n";
       dump.Entry( "Accuracy" ) << sf.Accuracy() << ";\n";
       dump.Entry( "AccuracyExp" ) << sf.AccuracyExp() << ";\n";
       dump.Entry( "RExp" ) << sf.RExp() << ";\n";
       dump.Entry( "BExp" ) << sf.BExp() << ";\n";

       bool v = m_data[30] & 1;
       dump.Entry( "NominalReadingSpecified" ) << v << ";\n";

       if ( v )
            dump.Entry( "NominalReading" ) << m_data[31] << ";\n";

       v = m_data[30] & 2;
       dump.Entry( "NormalMaxSpecified" ) << v << ";\n";

       if ( v )
            dump.Entry( "NormalMax" ) << m_data[32] << ";\n";

       v = m_data[30] & 4;
       dump.Entry( "NormalMinSpecified" ) << v << ";\n";

       if ( v )
            dump.Entry( "NormalMin" ) << m_data[33] << ";\n";

       dump.Entry( "SensorMax" ) << m_data[34] << ";\n";
       dump.Entry( "SensorMin" ) << m_data[35] << ";\n";
       
       dump.Entry( "UpperNonRecoverableThreshold" ) << m_data[36] << ";\n";
       dump.Entry( "UpperCriticalThreshold" ) << m_data[37] << ";\n";
       dump.Entry( "UpperNonCriticalThreshold" ) << m_data[38] << ";\n";

       dump.Entry( "LowerNonRecoverableThreshold" ) << m_data[39] << ";\n";
       dump.Entry( "LowerCriticalThreshold" ) << m_data[40] << ";\n";
       dump.Entry( "LowerNonCriticalThreshold" ) << m_data[41] << ";\n";
       
       dump.Entry( "PositiveGoingThresholdHysteresis" ) << m_data[42] << ";\n";
       dump.Entry( "NegativeGoingThresholdHysteresis" ) << m_data[43] << ";\n";
     }
  else
     {
       // assertion
       unsigned short em = IpmiGetUint16( m_data + 14 );
       dump.Hex( true );
       dump.Entry( "AssertionEventMask" ) << em << ";\n";

       // deassertion
       em = IpmiGetUint16( m_data + 16 );
       dump.Entry( "DeassertionEventMask" ) << em << ";\n";

       // event mask
       em = IpmiGetUint16( m_data + 18 );
       dump.Entry( "DiscreteReadingMask" ) << em << ";\n";

       dump.Hex( false );
     }

  dump.Entry( "Oem" ) << m_data[46] << ";\n";

  cIpmiTextBuffer tb;
  tb.SetIpmi( m_data + 47 );
  tb.GetAscii( str, 80 );
  dump.Entry( "Id" ) << "\"" << str << "\";\n";
}


void
cIpmiSdr::DumpFruDeviceLocator( cIpmiLog &dump ) const
{
  dump.Entry( "DeviceAccessAddress" ) << m_data[5] << ";\n";

  if ( m_data[7] & 0x80 )
     {
       // logical FRU device
       dump.Entry( "FruDeviceId" ) << (int)m_data[6] << ";\n";
     }
  else
     {
       // device is directly on IPMB
       dump.Entry( "SlaveAddress" ) << m_data[6] << ";\n";
       dump.Entry( "Lun" ) << (int)((m_data[7] >> 3) & 3) << ";\n";
     }

  dump.Entry( "LogicalDevice" ) << Bit(m_data[7], 7 ) << ";\n";
  dump.Entry( "Channel" ) << (int)(m_data[8] >> 4) << ";\n";
  dump.Entry( "DeviceType" ) << m_data[10] << ";\n";
  dump.Entry( "DeviceTypeModifier" ) << m_data[11] << ";\n";

  tIpmiEntityId id = (tIpmiEntityId)m_data[12];
  char str[80];

  if ( !strcmp( IpmiEntityIdToString( id ), "Invalid" ) )
       sprintf( str, "0x%02x", id );
  else
       sprintf( str, "dIpmiEntityId%s", IpmiEntityIdToString( id ) );

  dump.Entry( "EntityId" ) << str << ";\n";
  dump.Entry( "EntityInstance" ) << (int)m_data[13] << ";\n";

  dump.Entry( "Oem" ) << m_data[14] << ";\n";

  cIpmiTextBuffer tb;
  tb.SetIpmi( m_data + 15 );
  tb.GetAscii( str, 80 );
  dump.Entry( "Id" ) << "\"" << str << "\";\n";
}


void 
cIpmiSdr::DumpMcDeviceLocator( cIpmiLog &dump ) const
{
  dump.Entry( "SlaveAddress" ) << m_data[5] << ";\n";
  dump.Entry( "Channel" ) << (int)(m_data[6] & 0x0f) << ";\n";

  dump.Entry( "AcpiSystemPower" ) << Bit( m_data[7], 7 ) << ";\n";
  dump.Entry( "AcpiDevicePower" ) << Bit( m_data[7], 6 ) << ";\n";

  dump.Entry( "ControllerLogInitAgentErrors" ) << Bit( m_data[7], 3 ) << ";\n";
  dump.Entry( "LogInitializationAgentError" ) << Bit( m_data[7], 2 ) << ";\n";
  dump.Entry( "EventMessageGeneration" ) << ( m_data[7] & 3 ) << ";\n";
  dump.Entry( "ChassisSupport" ) << Bit( m_data[8], 7 ) << ";\n";
  dump.Entry( "BridgeSupport" ) << Bit( m_data[8], 6 ) << ";\n";
  dump.Entry( "IpmbEventGeneratorSupport" ) << Bit( m_data[8], 5 ) << ";\n";
  dump.Entry( "IpmbEventReceiverSupport" ) << Bit( m_data[8], 4 ) << ";\n";
  dump.Entry( "FruInventorySupport" ) << Bit( m_data[8], 3 ) << ";\n";
  dump.Entry( "SelDeviceSupport" ) << Bit( m_data[8], 2 ) << ";\n";
  dump.Entry( "SdrRepositorySupport" ) << Bit( m_data[8], 1 ) << ";\n";
  dump.Entry( "SensorDeviceSupport" ) << Bit( m_data[8], 0 ) << ";\n";

  tIpmiEntityId id = (tIpmiEntityId)m_data[12];
  char str[80];

  if ( !strcmp( IpmiEntityIdToString( id ), "Invalid" ) )
       sprintf( str, "0x%02x", id );
  else
       sprintf( str, "dIpmiEntityId%s", IpmiEntityIdToString( id ) );

  dump.Entry( "EntityId" ) << str << ";\n";
  dump.Entry( "EntityInstance" ) << (int)m_data[13] << ";\n";

  dump.Entry( "Oem" ) << m_data[14] << ";\n";

  cIpmiTextBuffer tb;
  tb.SetIpmi( m_data + 15 );
  tb.GetAscii( str, 80 );
  dump.Entry( "Id" ) << "\"" << str << "\";\n";
}


void
cIpmiSdr::Dump( cIpmiLog &dump, const char *name ) const
{
  char str[80];
  sprintf( str, "%sRecord", IpmiSdrTypeToName( m_type ) );
  dump.Begin( str, name );
  dump.Entry( "RecordId" ) << m_record_id << ";\n";
  dump.Entry( "Version" ) << (int)m_major_version << ", "
			  << (int)m_minor_version << ";\n";

  switch( m_type )
     {
       case eSdrTypeFullSensorRecord:
            DumpFullSensor( dump );
            break;

       case eSdrTypeFruDeviceLocatorRecord:
            DumpFruDeviceLocator( dump );
            break;

       case eSdrTypeMcDeviceLocatorRecord:
            DumpMcDeviceLocator( dump );

       default:
            break;
     }

  dump.End();
}


static void
IpmiSdrDestroyRecords( cIpmiSdr **&sdr, unsigned int &n )
{
  if ( sdr == 0 )
       return;

  for( unsigned int i = 0; i < n; i++ )
     {
       assert( sdr[i] );
       delete sdr[i];
     }

  delete [] sdr;

  n = 0;
  sdr = 0;
}



cIpmiSdrs::cIpmiSdrs( cIpmiMc *mc, unsigned int lun, bool device_sdr )
  : m_mc( mc ), m_lun( lun ), m_device_sdr( device_sdr ),
    m_fetched( false ), m_major_version( 0 ), m_minor_version( 0 ),
    m_last_addition_timestamp( 0 ), m_last_erase_timestamp( 0 ),
    m_overflow( 0 ), m_update_mode( eIpmiRepositorySdrUpdateUnspecified ),
    m_supports_delete_sdr( false ), m_supports_partial_add_sdr( false ),
    m_supports_reserve_sdr( false ), m_supports_get_sdr_repository_allocation( false ),
    m_dynamic_population( false ),
    m_reservation( 0 ), m_sdr_changed( false ),
    m_num_sdrs( 0 ), m_sdrs( 0 )
{
  m_lun_has_sensors[0] = false;
  m_lun_has_sensors[1] = false;
  m_lun_has_sensors[2] = false;
  m_lun_has_sensors[3] = false;
}


cIpmiSdrs::~cIpmiSdrs()
{
  if ( m_sdrs )
       IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );
}


cIpmiSdr *
cIpmiSdrs::ReadRecord( unsigned short record_id,
                       unsigned short &next_record_id,
                       tReadRecord &err )
{
  cIpmiMsg       msg;
  cIpmiMsg       rsp;
  int            rv;
  int            offset = 0;
  unsigned char  data[dMaxSdrData];
  int            record_size = 0;
  int            read_len = 0;
  cIpmiSdr      *sdr;

  memset( data, 0xaa, dMaxSdrData );

  do
     {
       if ( m_device_sdr )
          { 
            msg.m_netfn = eIpmiNetfnSensorEvent;
            msg.m_cmd   = eIpmiCmdGetDeviceSdr;
          }
       else
          {
            msg.m_netfn = eIpmiNetfnStorage;
            msg.m_cmd   = eIpmiCmdGetSdr;
          }

       msg.m_data_len = 6;
       IpmiSetUint16( msg.m_data, m_reservation );
       IpmiSetUint16( msg.m_data + 2, record_id );

       msg.m_data[4] = offset;

       if ( offset == 0 )
            read_len = dSdrHeaderSize;
       else
          {
            read_len = record_size - offset;

            if ( read_len > dMaxSdrFetch )
                 read_len = dMaxSdrFetch;
          }

       msg.m_data[5] = read_len;

       rv = m_mc->SendCommand( msg, rsp, m_lun );

       if ( rv )
          {
            stdlog << "initial_sdr_fetch: Couldn't send GetSdr or GetDeviveSdr fetch: " << rv << " !\n";
            err = eReadError;

            return 0;
          }

       if ( rsp.m_data[0] == 0x80 )
          {
            // Data changed during fetch, retry.  Only do this so many
            // times before giving up.
            stdlog << "SRD reservation lost.\n";

            err = eReadReservationLost;

            return 0;
          }

       if ( rsp.m_data[0] == eIpmiCcInvalidReservation )
          {
            stdlog << "SRD reservation lost.\n";

            err = eReadReservationLost;
            return 0;
          }

       if (    record_id == 0 
            && (    (rsp.m_data[0] == eIpmiCcUnknownErr )
	         || (rsp.m_data[0] == eIpmiCcNotPresent ) ) )
          {
            // We got an error fetching the first SDR, so the repository is
            // probably empty.  Just go on.
            stdlog << "SRD reservation lost.\n";

            err = eReadEndOfSdr;
            return 0;
          }
 
       if ( rsp.m_data[0] != eIpmiCcOk )
          {
            stdlog << "SDR fetch error getting sdr " << record_id << ": "
                   << rsp.m_data[0] << " !\n";

            err = eReadError;
            return 0;
          }

       if ( rsp.m_data_len != read_len + 3 )
          {
            stdlog << "Got an invalid amount of SDR data: " << rsp.m_data_len 
                   << ", expected " << read_len + 3 << " !\n";

            err = eReadError;
            return 0;
          }

       // copy the data
       memcpy( data + offset, rsp.m_data + 3, read_len );

       // header => get record size
       if ( offset == 0 )
          {
            record_size = rsp.m_data[7] + dSdrHeaderSize;
            next_record_id = IpmiGetUint16( rsp.m_data + 1 );
          }

       offset += read_len;
     }
  while( offset < record_size );

  // create sdr
  sdr = new cIpmiSdr;
  memset( sdr, 0, sizeof( cIpmiSdr ));

  sdr->m_record_id     = IpmiGetUint16( data );
  sdr->m_major_version = data[2] & 0xf;
  sdr->m_minor_version = (data[2] >> 4) & 0xf;
  sdr->m_type          = (tIpmiSdrType)data[3];  

  // Hack to support 1.0 MCs
  if ((sdr->m_major_version == 1)
      && (sdr->m_minor_version == 0)
      && (sdr->m_type == eSdrTypeMcDeviceLocatorRecord))
  {
      data[8] = data[7];
      data[7] = data[6];
      data[6] = 0;
  }

  sdr->m_length        = record_size;
  memcpy( sdr->m_data, data, record_size );

  err = eReadOk;

  return sdr;
}


int
cIpmiSdrs::Reserve()
{
  cIpmiMsg msg;
  cIpmiMsg rsp;
  int rv;

  assert( m_supports_reserve_sdr );

  // Now get the reservation.
  if ( m_device_sdr )
     {
       msg.m_netfn = eIpmiNetfnSensorEvent;
       msg.m_cmd   = eIpmiCmdReserveDeviceSdrRepository;
     }
  else
     {
       msg.m_netfn = eIpmiNetfnStorage;
       msg.m_cmd   = eIpmiCmdReserveSdrRepository;
     }

  msg.m_data_len = 0;
  rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       stdlog << "Couldn't send SDR reservation: " << rv << " !\n";
       return EINVAL;
     }

  if ( rsp.m_data[0] != 0) 
     {
       if (    m_device_sdr
            && rsp.m_data[0] == eIpmiCcInvalidCmd )
          {
	    // This is a special case.  We always attempt a
            // reservation with a device SDR (since there is nothing
            // telling us if this is supported), if it fails then we
            // just go on without the reservation.
	    m_supports_reserve_sdr = false;
	    m_reservation = 0;

            return 0;
	}

       stdlog << "Error getting SDR fetch reservation: " << rsp.m_data[0] << " !\n";

       return EINVAL;
     }

  if ( rsp.m_data_len < 3 )
     {
       stdlog << "SDR Reservation data not long enough: " << rsp.m_data_len << " bytes!\n";
       return EINVAL;
     }

  m_reservation = IpmiGetUint16( rsp.m_data + 1 );

  return 0;
}


int
cIpmiSdrs::GetInfo( unsigned short &working_num_sdrs )
{
  int      rv;
  cIpmiMsg msg;
  cIpmiMsg rsp;
  unsigned int add_timestamp;
  unsigned int erase_timestamp;

  // sdr info
  if ( m_device_sdr )
     {
       msg.m_netfn = eIpmiNetfnSensorEvent;
       msg.m_cmd   = eIpmiCmdGetDeviceSdrInfo;
     }
  else
     {
       msg.m_netfn = eIpmiNetfnStorage;
       msg.m_cmd   = eIpmiCmdGetSdrRepositoryInfo;
     }

  msg.m_data_len = 0;

  rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       stdlog << "IpmiSdrsFetch: GetDeviceSdrInfoCmd or GetSdrRepositoryInfoCmd "
              << rv << ", " << strerror( rv ) << " !\n";

       m_sdr_changed = true;
       IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

       return rv;
     }

  if ( rsp.m_data[0] != 0 )
     {
  	if ( m_device_sdr == false )
           {
             // The device doesn't support the get device SDR info
             // command, so just assume some defaults.
             working_num_sdrs     = 0xfffe;
             m_dynamic_population = false;

             // Assume it uses reservations, if the reservation returns
             // an error, then say that it doesn't.
             m_supports_reserve_sdr = true;

             m_lun_has_sensors[0] = true;
             m_lun_has_sensors[1] = false;
             m_lun_has_sensors[2] = false;
             m_lun_has_sensors[3] = false;

             add_timestamp   = 0;
             erase_timestamp = 0;
           }
        else
           {
             stdlog << "IPMI Error getting SDR info: " << rsp.m_data[0] << " !\n";

             m_sdr_changed = true;
             IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

             return EINVAL;
           }
     }
  else if ( m_device_sdr )
     {
       // device SDR

       if ( rsp.m_data_len < 3 ) 
          {
	    stdlog << "SDR info is not long enough !\n";

            m_sdr_changed = true;
            IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

	    return EINVAL;
	}

       working_num_sdrs = rsp.m_data[1];
       m_dynamic_population = (rsp.m_data[2] & 0x80) == 0x80;

       // Assume it uses reservations, if the reservation returns
       // an error, then say that it doesn't.
       m_supports_reserve_sdr = true;

       m_lun_has_sensors[0] = (rsp.m_data[2] & 0x01) == 0x01;
       m_lun_has_sensors[1] = (rsp.m_data[2] & 0x01) == 0x02;
       m_lun_has_sensors[2] = (rsp.m_data[2] & 0x01) == 0x04;
       m_lun_has_sensors[3] = (rsp.m_data[2] & 0x01) == 0x08;

       if ( m_dynamic_population )
          {
	    if ( rsp.m_data_len < 7 )
               {
                 stdlog << "SDR info is not long enough !\n";

                 m_sdr_changed = 1;
                 IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

                 return EINVAL;
               }

	    add_timestamp = IpmiGetUint32( rsp.m_data + 3 );
          }
       else
	    add_timestamp = 0;

       erase_timestamp = 0;
     }
  else
     {
       // repository SDR

       if ( rsp.m_data_len < 15 )
          {
	    stdlog << "SDR info is not long enough\n";

            m_sdr_changed = true;
            IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

	    return EINVAL;
          }

       /* Pull pertinant info from the response. */
       m_major_version = rsp.m_data[1] & 0xf;
       m_minor_version = (rsp.m_data[1] >> 4) & 0xf;
       working_num_sdrs = IpmiGetUint16( rsp.m_data + 2 );

       m_overflow                 = (rsp.m_data[14] & 0x80) == 0x80;
       m_update_mode              = (tIpmiRepositorySdrUpdate)((rsp.m_data[14] >> 5) & 0x3);
       m_supports_delete_sdr      = (rsp.m_data[14] & 0x08) == 0x08;
       m_supports_partial_add_sdr = (rsp.m_data[14] & 0x04) == 0x04;
       m_supports_reserve_sdr     = (rsp.m_data[14] & 0x02) == 0x02;
       m_supports_get_sdr_repository_allocation
         = (rsp.m_data[14] & 0x01) == 0x01;

       add_timestamp   = IpmiGetUint32( rsp.m_data + 6 );
       erase_timestamp = IpmiGetUint32( rsp.m_data + 10 );
     }

  // If the timestamps still match, no need to re-fetch the repository
  if (      m_fetched
       && ( add_timestamp   == m_last_addition_timestamp )
       && ( erase_timestamp == m_last_erase_timestamp ) )
      return -1; 

  m_last_addition_timestamp = add_timestamp;
  m_last_erase_timestamp    = erase_timestamp;

  return 0;
}


int
cIpmiSdrs::Fetch()
{
  int      rv;
  int      retry_count;
  cIpmiSdr **records;
  unsigned int num;
  int      finish;
  unsigned short working_num_sdrs;

  m_sdr_changed = false;

  assert( m_mc );

  if ( m_device_sdr )
     {
       if ( !m_mc->ProvidesDeviceSdrs() )
            return ENOSYS;
     }
  else if ( !m_mc->SdrRepositorySupport() )
       return ENOSYS;

  // working num sdrs is just an estimation
  rv = GetInfo( working_num_sdrs );

  // sdr records are up to date
  if ( rv < 0 )
       return 0;

  if ( rv )
       return rv;

  m_sdr_changed = true;
  IpmiSdrDestroyRecords( m_sdrs, m_num_sdrs );

  // because working_num_sdrs is an estimation
  // read the sdr to get the real number
  if ( working_num_sdrs == 0 )
       working_num_sdrs = 1;

  // read sdr
  records = new cIpmiSdr *[working_num_sdrs];
  retry_count = 0;
  finish      = 0;

  while( !finish )
     {
       unsigned short next_record_id = 0;

       if ( retry_count++ == dMaxSdrFetchRetries )
          {
            stdlog << "To many retries trying to fetch SDRs\n";
            IpmiSdrDestroyRecords( records, num );

            return EBUSY;
          }

       num = 0;

       rv = Reserve();

       if ( rv )
          {
            IpmiSdrDestroyRecords( records, num );
            return rv;
          }

       // read sdr records
       while( 1 )
          {
            tReadRecord err;
            cIpmiSdr *sdr;
            unsigned short record_id = next_record_id;

            sdr = ReadRecord( record_id, next_record_id, err );

            if ( sdr == 0 )
               {
                 if ( err == eReadReservationLost )
                      break;

                 if ( err != eReadEndOfSdr )
                    {
                      IpmiSdrDestroyRecords( records, num );
                      return EINVAL;
                    }

                 // SDR is empty
                 finish = 1;

                 break;
               }

            sdr->Dump( stdlog, "sdr" );

            if ( num >= working_num_sdrs )
               {
                 // resize records
                 cIpmiSdr **rec = new cIpmiSdr*[ working_num_sdrs + 10 ];
                 memcpy( rec, records, sizeof( cIpmiSdr * ) * working_num_sdrs );

                 delete [] records;
                 records = rec;
                 working_num_sdrs += 10;
               }

            records[num++] = sdr;

            if ( next_record_id == 0xffff )
               {
                 // finish
                 finish = 1;

                 break;
               }
          }
     }

  if ( num == 0 )
     {
       delete [] records;
       m_sdrs = 0;
       m_num_sdrs = 0;
  
       return 0;
     }

  if ( num == working_num_sdrs )
     {
       m_sdrs = records;
       m_num_sdrs = working_num_sdrs;

       return 0;
     }

  m_sdrs = new cIpmiSdr *[num];
  memcpy( m_sdrs, records, num * sizeof( cIpmiSdr * ) );
  m_num_sdrs = num;

  delete [] records;

  return 0;
}


void
cIpmiSdrs::Dump( cIpmiLog &dump, const char *name ) const
{
  unsigned int i;
  char str[80];

  if ( dump.IsRecursive() )
     {
       for( i = 0; i < m_num_sdrs; i++ )
	  {
	    sprintf( str, "Sdr%02x_%d", m_mc->GetAddress(), i );
	    m_sdrs[i]->Dump( dump, str );
	  }
     }

  dump.Begin( "Sdr", name );

  if ( m_device_sdr )
     {
       dump.Entry( "DynamicPopulation" ) << m_dynamic_population << ";\n";
       dump.Entry( "LunHasSensors" )
            << m_lun_has_sensors[0] << ", "
            << m_lun_has_sensors[1] << ", "
            << m_lun_has_sensors[2] << ", "
            << m_lun_has_sensors[3] << ";\n";
     }
  else
     {
       dump.Entry( "Version" ) << m_major_version << ", "
                                                 << m_minor_version << ";\n";
       dump.Entry( "Overflow" ) << m_overflow << ";\n";
       dump.Entry( "UpdateMode" ) << "dMainSdrUpdate"
            << IpmiRepositorySdrUpdateToString( m_update_mode ) << ";\n";
       dump.Entry( "SupportsDeleteSdr" ) << m_supports_delete_sdr << ";\n";
       dump.Entry( "SupportsPartialAddSdr" ) << m_supports_partial_add_sdr << ";\n";
       dump.Entry( "SupportsReserveSdr" ) << m_supports_reserve_sdr << ";\n";
       dump.Entry( "SupportsGetSdrRepositoryAllocation" ) << m_supports_get_sdr_repository_allocation << ";\n";
     }

  if ( dump.IsRecursive() && m_num_sdrs )
     {
       dump.Entry( "Sdr" );

       for( i = 0; i < m_num_sdrs; i++ )
          {
            if ( i != 0 )
                 dump << ", ";

            sprintf( str, "Sdr%02x_%d", m_mc->GetAddress(), i );
            dump << str;
          }

       dump << ";\n";
     }

  dump.End();
}


cIpmiSdr *
cIpmiSdrs::FindSdr( cIpmiMc *mc )
{
  for( unsigned int i = 0; i < NumSdrs(); i++ )
     {
       cIpmiSdr *sdr = Sdr( i );

       if ( sdr->m_type != eSdrTypeMcDeviceLocatorRecord )
            continue;

       if (    mc->GetAddress() == (unsigned int)sdr->m_data[5] 
            && mc->GetChannel() == (unsigned int)(sdr->m_data[6] & 0x0f) )
            return sdr;
     }

  return 0;
}
