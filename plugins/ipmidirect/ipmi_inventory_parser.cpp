/*
 * ipmi_inventory_parser.h
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

#include "ipmi_inventory_parser.h"
#include "ipmi_utils.h"
#include <string.h>


unsigned char
IpmiChecksum( const unsigned char *data, int size )
{
  unsigned char c = 0;

  while( size-- )
      c += *data++;

  return c;
}


static const char *inventory_record_type_map[] =
{
  "Internal",
  "Chassis",
  "Board",
  "Product",
  "MultiRecord"
};


const char *
IpmiInventoryRecordTypeToString( tIpmiInventoryRecordType type )
{
  if ( type >= eIpmiInventoryRecordTypeLast )
       return "Invalid";

  return inventory_record_type_map[type];
}


static SaErrorT
ReadTextBuffer( const unsigned char *&data, unsigned &size, 
		cIpmiTextBuffer &tb )
{
  if ( size < 1 )
       return SA_ERR_HPI_DATA_LEN_INVALID;

  const unsigned char *d = tb.SetIpmi( data );

  if ( d == 0 )
       return SA_ERR_HPI_DATA_LEN_INVALID;

  size = d - data;
  data = d;

  return SA_OK;
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryRecordInternal
////////////////////////////////////////////////////////////

cIpmiInventoryRecordInternal::cIpmiInventoryRecordInternal()
  : cIpmiInventoryRecordCommon( eIpmiInventoryRecordTypeInternal ),
    m_data( 0 ), m_size( 0 )
{
}


cIpmiInventoryRecordInternal::~cIpmiInventoryRecordInternal()
{
  if ( m_data )
       delete [] m_data;
}


void
cIpmiInventoryRecordInternal::SetData( const unsigned char *data, unsigned int size )
{
  if ( m_data )
       delete [] m_data;

  if ( size == 0 )
     {
       m_data = 0;
       m_size = 0;

       return;
     }

  m_data = new unsigned char[size];
  memcpy( m_data, data, size );
  m_size = size;
}


const unsigned char *
cIpmiInventoryRecordInternal::GetData( unsigned int &size ) const
{
  size = m_size;
  
  return m_data;
}


SaErrorT
cIpmiInventoryRecordInternal::IpmiRead( const unsigned char *data, unsigned int size )
{
  SaErrorT rv = cIpmiInventoryRecordCommon::IpmiRead( data, size );

  if ( rv != SA_OK )
       return rv;

  SetData( data+1, size - 1 );

  return SA_OK;
}


unsigned int
cIpmiInventoryRecordInternal::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordInternal::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordInternal::HpiSize( int record_id ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return 0;
     }

  unsigned int size =   sizeof( SaHpiInventDataRecordT )
                      - sizeof( SaHpiInventDataUnionT );

  size += m_size;

  return size;
}


void
cIpmiInventoryRecordInternal::HpiRead( int record_id, SaHpiInventDataRecordT *r ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  r->RecordType = SAHPI_INVENT_RECTYPE_INTERNAL_USE;
  r->DataLength = m_size;

  if ( m_size )
       memcpy( r->RecordData.InternalUse.Data, m_data, m_size );
}


void
cIpmiInventoryRecordInternal::HpiWrite( int record_id, SaHpiInventDataRecordT * /*r*/ )
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  assert( 0 );
}


void
cIpmiInventoryRecordInternal::Dump( cIpmiLog &dump,
                                    int record_id, 
                                    const char *name ) const
{
  if ( record_id )
     {
       assert( 0 );
       return;
     }
  
  dump.Begin( "FruInternalUse", name );

  dump.Entry( "Version" ) << m_version << ";\n";
  dump.Entry( "Data" );

  for( unsigned int i = 0; i < m_size; i++ )
     {
       if ( i != 0 )
            dump << ", ";
  
       dump << m_data[i];
     }

  dump << ";\n";

  dump.End();
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryRecordChassis
////////////////////////////////////////////////////////////


cIpmiInventoryRecordChassis::cIpmiInventoryRecordChassis()
  : cIpmiInventoryRecordCommon( eIpmiInventoryRecordTypeChassis ),
    m_chassis_type( SAHPI_INVENT_CTYP_UNKNOWN )
{
}


cIpmiInventoryRecordChassis::~cIpmiInventoryRecordChassis()
{
}


SaErrorT
cIpmiInventoryRecordChassis::IpmiRead( const unsigned char *data, unsigned int size )
{
  SaErrorT rv = cIpmiInventoryRecordCommon::IpmiRead( data, size );

  if ( rv != SA_OK )
       return rv;

  data += 2;
  size -= 2;

  if ( size < 1 )
       return SA_ERR_HPI_DATA_LEN_INVALID;

  m_chassis_type = (SaHpiInventChassisTypeT)data[0];
  data++;
  size--;

  rv = ReadTextBuffer( data, size, m_chassis_part_number );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_chassis_serial_number );

  if ( rv != SA_OK )
       return rv;

  while( true )
     {
       if ( size < 1 )
	    return SA_ERR_HPI_DATA_LEN_INVALID;

       if ( *data == 0xc1 )
	    break;

       cIpmiTextBuffer *tb = new cIpmiTextBuffer;
       rv = ReadTextBuffer( data, size, *tb );

       if ( rv != SA_OK )
	  {
	    delete tb;
	    return rv;
	  }

       Add( tb );
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryRecordChassis::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordChassis::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordChassis::HpiSize( int record_id ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return 0;
     }

  unsigned int size =   sizeof( SaHpiInventDataRecordT )
                      - sizeof( SaHpiInventDataUnionT );

  size += sizeof( SaHpiInventChassisDataT );

  if ( m_chassis_part_number.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_chassis_serial_number.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  size += Num() * ( sizeof( SaHpiTextBufferT * ) + sizeof( SaHpiTextBufferT ) );

  return size;
}


void
cIpmiInventoryRecordChassis::HpiRead( int record_id, SaHpiInventDataRecordT *r ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  unsigned char *data = (unsigned char *)r;
  data +=   sizeof( SaHpiInventDataRecordT )
          - sizeof( SaHpiInventDataUnionT );
  data += sizeof( SaHpiInventChassisDataT );
  data += Num() * sizeof( SaHpiTextBufferT * );

  r->RecordType = SAHPI_INVENT_RECTYPE_CHASSIS_INFO;

  r->DataLength =   sizeof( SaHpiInventChassisDataT )
                  + Num() * ( sizeof( SaHpiTextBufferT * ) + sizeof( SaHpiTextBufferT ) );

  SaHpiInventChassisDataT &cd = r->RecordData.ChassisInfo;
  cd.Type        = m_chassis_type;
  cd.GeneralData.MfgDateTime = SAHPI_TIME_UNSPECIFIED;

  if ( m_chassis_part_number.DataLength() )
     {
       cd.GeneralData.PartNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_chassis_part_number;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_chassis_serial_number.DataLength() )
     {
       cd.GeneralData.SerialNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_chassis_serial_number;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  for( int i = 0; i < Num(); i++ )
     {
       cd.GeneralData.CustomField[i] = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)*operator[]( i );

       data += sizeof( SaHpiTextBufferT );
     }

  cd.GeneralData.CustomField[Num()] = 0;
}


void
cIpmiInventoryRecordChassis::HpiWrite( int record_id, SaHpiInventDataRecordT * /*r*/ )
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  assert( 0 );
}


void
cIpmiInventoryRecordChassis::Dump( cIpmiLog &dump,
                                   int record_id, 
                                   const char *name ) const
{
  if ( record_id )
     {
       assert( 0 );
       return;
     }

  dump.Begin( "FruChassisInfo", name );

  dump.Entry( "Version" ) << m_version << ";\n";
  dump.Entry( "ChassisType" ) << (unsigned char)m_chassis_type << ";\n";
  dump.Entry( "PartNumber" ) << "\"" << m_chassis_part_number << "\";\n";
  dump.Entry( "SerialNumber" ) << "\"" << m_chassis_serial_number << "\";\n";

  if ( Num() )
     {
       dump.Entry( "CustomField" );

       for( int i = 0; i < Num(); i++ )
          {
            if ( i != 0 )
                 dump << ", ";

            dump << "\"" << *operator[]( i ) << "\"";
          }

       dump << ";\n";
     }

  dump.End();
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryRecordBoard
////////////////////////////////////////////////////////////


cIpmiInventoryRecordBoard::cIpmiInventoryRecordBoard()
  : cIpmiInventoryRecordCommon( eIpmiInventoryRecordTypeBoard ),
    m_language( SAHPI_LANG_UNDEF ), m_mfg_time( 0 )
{
}


cIpmiInventoryRecordBoard::~cIpmiInventoryRecordBoard()
{
}


SaErrorT
cIpmiInventoryRecordBoard::IpmiRead( const unsigned char *data, unsigned int size )
{
  SaErrorT rv = cIpmiInventoryRecordCommon::IpmiRead( data, size );

  if ( rv != SA_OK )
       return rv;

  data += 2;
  size -= 2;

  if ( size < 4 )
       return SA_ERR_HPI_DATA_LEN_INVALID;

  m_language = (SaHpiLanguageT)data[0];
  data++;
  size--;

  m_mfg_time = (SaHpiTimeT)data[0] + (SaHpiTimeT)data[1] * 256 + (SaHpiTimeT)data[1] * 65536;

  size -= 3;
  data += 3;

  m_mfg_time *= 60;

  // create date offset
  struct tm tmt;

  tmt.tm_sec  = 0;
  tmt.tm_min  = 0;
  tmt.tm_hour = 0;
  tmt.tm_mday = 1;
  tmt.tm_mon  = 0;
  tmt.tm_year = 96;
  tmt.tm_isdst = 0;

  m_mfg_time += mktime( &tmt );

  m_mfg_time *= 1000000000;

  rv = ReadTextBuffer( data, size, m_board_manufacturer );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_board_product_name );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_board_serial_number );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_board_part_number );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_fru_file_id );

  if ( rv != SA_OK )
       return rv;

  while( true )
     {
       if ( size < 1 )
	    return SA_ERR_HPI_DATA_LEN_INVALID;

       if ( *data == 0xc1 )
	    break;

       cIpmiTextBuffer *tb = new cIpmiTextBuffer;
       rv = ReadTextBuffer( data, size, *tb );

       if ( rv != SA_OK )
	  {
	    delete tb;
	    return rv;
	  }

       Add( tb );
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryRecordBoard::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordBoard::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordBoard::HpiSize( int record_id ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return 0;
     }

  unsigned int size =   sizeof( SaHpiInventDataRecordT )
                      - sizeof( SaHpiInventDataUnionT );

  size += sizeof( SaHpiInventGeneralDataT );

  if ( m_board_manufacturer.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_board_product_name.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_board_serial_number.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_board_part_number.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_fru_file_id.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  size += Num() * ( sizeof( SaHpiTextBufferT * ) + sizeof( SaHpiTextBufferT ) );

  return size;
}


void
cIpmiInventoryRecordBoard::HpiRead( int record_id, SaHpiInventDataRecordT *r ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  unsigned char *data = (unsigned char *)r;
  data +=   sizeof( SaHpiInventDataRecordT )
          - sizeof( SaHpiInventDataUnionT );
  data += sizeof( SaHpiInventGeneralDataT );
  data += Num() * sizeof( SaHpiTextBufferT * );

  r->RecordType = SAHPI_INVENT_RECTYPE_BOARD_INFO;
  r->DataLength =   sizeof( SaHpiInventGeneralDataT )
                  + Num() * ( sizeof( SaHpiTextBufferT * ) + sizeof( SaHpiTextBufferT ) );

  SaHpiInventGeneralDataT &gd = r->RecordData.BoardInfo;
  gd.MfgDateTime = m_mfg_time;

  if ( m_board_manufacturer.DataLength() )
     {
       gd.Manufacturer = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_board_manufacturer;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_board_product_name.DataLength() )
     {
       gd.ProductName = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_board_product_name;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_board_serial_number.DataLength() )
     {
       gd.SerialNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_board_serial_number;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_board_part_number.DataLength() )
     {
       gd.PartNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_board_part_number;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_fru_file_id.DataLength() )
     {
       gd.FileId = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_fru_file_id;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  for( int i = 0; i < Num(); i++ )
     {
       gd.CustomField[i] = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)*operator[]( i );

       data += sizeof( SaHpiTextBufferT );
     }

  gd.CustomField[Num()] = 0;
}


void
cIpmiInventoryRecordBoard::HpiWrite( int record_id, SaHpiInventDataRecordT * /*r*/ )
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  assert( 0 );
}


void
cIpmiInventoryRecordBoard::Dump( cIpmiLog &dump,
                                 int record_id, 
                                 const char *name ) const
{
  if ( record_id )
     {
       assert( 0 );
       return;
     }

  dump.Begin( "FruBoardInfo", name );

  dump.Entry( "Version" ) << m_version << ";\n";
  dump.Entry( "Language" ) << (unsigned char)m_language << ";\n";

  SaHpiTimeT mt = m_mfg_time / 1000000000;
  struct tm tmt;
  time_t dummy = mt;
  localtime_r( &dummy, &tmt );

  // 2003,10,30,11,11,11
  char str[80];
  strftime( str, 80, "%Y,%m,%d,%H,%M,%S", &tmt );

  dump.Entry( "ManufacturerDate" ) << str << ";\n";
  dump.Entry( "Manufacturer" ) << "\"" << m_board_manufacturer << "\";\n";
  dump.Entry( "Name" ) << "\"" << m_board_product_name << "\";\n";
  dump.Entry( "SerialNumber" ) << "\"" << m_board_serial_number << "\";\n";
  dump.Entry( "PartNumber" ) << "\"" << m_board_part_number << "\";\n";
  dump.Entry( "FruFileId" ) << "\"" << m_fru_file_id << "\";\n";

  if ( Num() )
     {
       dump.Entry( "CustomField" );

       for( int i = 0; i < Num(); i++ )
          {
            if ( i != 0 )
                 dump << ", ";

            dump << "\"" << *operator[]( i ) << "\"";
          }

       dump << ";\n";
     }

  dump.End();
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryRecordProduct
////////////////////////////////////////////////////////////


cIpmiInventoryRecordProduct::cIpmiInventoryRecordProduct()
  : cIpmiInventoryRecordCommon( eIpmiInventoryRecordTypeProduct ),
    m_language( SAHPI_LANG_UNDEF )
{
}


cIpmiInventoryRecordProduct::~cIpmiInventoryRecordProduct()
{
}


SaErrorT
cIpmiInventoryRecordProduct::IpmiRead( const unsigned char *data, unsigned int size )
{
  SaErrorT rv = cIpmiInventoryRecordCommon::IpmiRead( data, size );

  if ( rv != SA_OK )
       return rv;

  data += 2;
  size -= 2;

  if ( size < 1 )
       return SA_ERR_HPI_DATA_LEN_INVALID;

  m_language = (SaHpiLanguageT)data[0];
  data++;
  size--;

  rv = ReadTextBuffer( data, size, m_manufacturer_name );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_product_name );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_product_part );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_product_version );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_product_serial_number );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_asset_tag );

  if ( rv != SA_OK )
       return rv;

  rv = ReadTextBuffer( data, size, m_fru_file_id );

  if ( rv != SA_OK )
       return rv;

  while( true )
     {
       if ( size < 1 )
	    return SA_ERR_HPI_DATA_LEN_INVALID;

       if ( *data == 0xc1 )
	    break;

       cIpmiTextBuffer *tb = new cIpmiTextBuffer;
       rv = ReadTextBuffer( data, size, *tb );

       if ( rv != SA_OK )
	  {
	    delete tb;
	    return rv;
	  }

       Add( tb );
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryRecordProduct::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordProduct::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordProduct::HpiSize( int record_id ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return 0;
     }

  unsigned int size =   sizeof( SaHpiInventDataRecordT )
                      - sizeof( SaHpiInventDataUnionT );

  size += sizeof( SaHpiInventGeneralDataT );

  if ( m_manufacturer_name.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_product_name.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_product_part.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_product_version.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_product_serial_number.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_asset_tag.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  if ( m_fru_file_id.DataLength() )
       size += sizeof( SaHpiTextBufferT );

  size += Num() * (   sizeof( SaHpiTextBufferT * )
                    + sizeof( SaHpiTextBufferT ) );

  return size;
}


void
cIpmiInventoryRecordProduct::HpiRead( int record_id, SaHpiInventDataRecordT *r ) const
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  unsigned char *data = (unsigned char *)r;
  data +=   sizeof( SaHpiInventDataRecordT )
          - sizeof( SaHpiInventDataUnionT );
  data += sizeof( SaHpiInventGeneralDataT );
  data += Num() * sizeof( SaHpiTextBufferT * );

  r->RecordType = SAHPI_INVENT_RECTYPE_PRODUCT_INFO;

  r->DataLength =   sizeof( SaHpiInventGeneralDataT )
                  + Num() * (   sizeof( SaHpiTextBufferT * )
                              + sizeof( SaHpiTextBufferT ) );

  SaHpiInventGeneralDataT &gd = r->RecordData.BoardInfo;
  gd.MfgDateTime = SAHPI_TIME_UNSPECIFIED;

  if ( m_manufacturer_name.DataLength() )
     {
       gd.Manufacturer = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_manufacturer_name;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_product_name.DataLength() )
     {
       gd.ProductName = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_product_name;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_product_part.DataLength() )
     {
       gd.PartNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_product_part;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_product_version.DataLength() )
     {
       gd.ProductVersion = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_product_version;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_product_serial_number.DataLength() )
     {
       gd.SerialNumber = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_product_serial_number;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_asset_tag.DataLength() )
     {
       gd.AssetTag = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_asset_tag;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  if ( m_fru_file_id.DataLength() )
     {
       gd.FileId = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)m_fru_file_id;
       data += sizeof( SaHpiTextBufferT );

       r->DataLength += sizeof( SaHpiTextBufferT );
     }

  for( int i = 0; i < Num(); i++ )
     {
       gd.CustomField[i] = (SaHpiTextBufferT *)(void *)data;
       *(SaHpiTextBufferT *)(void *)data = (SaHpiTextBufferT)*operator[]( i );

       data += sizeof( SaHpiTextBufferT );
     }

  gd.CustomField[Num()] = 0;
}


void
cIpmiInventoryRecordProduct::HpiWrite( int record_id, SaHpiInventDataRecordT *r )
{
  if ( record_id != 0 )
     {
       assert( 0 );
       return;
     }

  assert( 0 );
}


void
cIpmiInventoryRecordProduct::Dump( cIpmiLog &dump,
                                   int record_id, 
                                   const char *name ) const
{
  if ( record_id )
     {
       assert( 0 );
       return;
     }

  dump.Begin( "FruProductInfo", name );

  dump.Entry( "Version" ) << m_version << ";\n";
  dump.Entry( "Language" ) << (unsigned char)m_language << ";\n";
  dump.Entry( "Manufacturer" ) << "\"" << m_manufacturer_name << "\";\n";
  dump.Entry( "Name" ) << "\"" << m_product_name << "\";\n";
  dump.Entry( "PartNumber" ) << "\"" << m_product_part << "\";\n";
  dump.Entry( "ProductVersion" ) << "\"" << m_product_version << "\";\n";
  dump.Entry( "SerialNumber" ) << "\"" << m_product_serial_number << "\";\n";
  dump.Entry( "AssetTag" ) << "\"" << m_asset_tag << "\";\n";
  dump.Entry( "FruFileId" ) << "\"" << m_fru_file_id << "\";\n";

  if ( Num() )
     {
       dump.Entry( "CustomField" );

       for( int i = 0; i < Num(); i++ )
          {
            if ( i != 0 )
                 dump << ", ";

            dump << "\"" << *operator[]( i ) << "\"";
          }

       dump << ";\n";
     }

  dump.End();
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryBackplanePointToPointConnectivity
////////////////////////////////////////////////////////////

cIpmiInventoryBackplanePointToPointConnectivity::cIpmiInventoryBackplanePointToPointConnectivity()
  : cIpmiInventoryMultiRecord( tIpmiMultiRecordTypeBackplanePointToPointConnectivity )
{
}


cIpmiInventoryBackplanePointToPointConnectivity::~cIpmiInventoryBackplanePointToPointConnectivity()
{
}


SaErrorT
cIpmiInventoryBackplanePointToPointConnectivity::IpmiRead( const unsigned char *data, unsigned int size )
{
  assert( 0 );

  return SA_OK;
}


unsigned int
cIpmiInventoryBackplanePointToPointConnectivity::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryBackplanePointToPointConnectivity::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryBackplanePointToPointConnectivity::HpiSize() const
{
  assert( 0 );

  return 0;
}


void
cIpmiInventoryBackplanePointToPointConnectivity::HpiRead( SaHpiInventDataRecordT *r ) const
{
  assert( 0 );
}


void
cIpmiInventoryBackplanePointToPointConnectivity::HpiWrite( SaHpiInventDataRecordT *r )
{
  assert( 0 );
}


void
cIpmiInventoryBackplanePointToPointConnectivity::Dump( cIpmiLog &dump, const char *name ) const
{
  assert( 0 );
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryShelfPowerDistribution
////////////////////////////////////////////////////////////

cIpmiInventoryShelfPowerDistribution::cIpmiInventoryShelfPowerDistribution()
  : cIpmiInventoryMultiRecord( tIpmiMultiRecordTypeShelfPowerDistribution )
{
}


cIpmiInventoryShelfPowerDistribution::~cIpmiInventoryShelfPowerDistribution()
{
}


SaErrorT
cIpmiInventoryShelfPowerDistribution::IpmiRead( const unsigned char *data, unsigned int size )
{
  unsigned char n = data[10];
  data += 11;
  size -= 11;

  for( unsigned char i = 0; i < n; i++ )
     {
       cIpmiInventoryPowerMap *p = new cIpmiInventoryPowerMap;
       Add( p );

       p->m_maximum_external_current = IpmiGetUint16( data );
       p->m_maximum_internal_current = IpmiGetUint16( data+2 );
       p->m_minimum_expected_voltage = data[4];

       unsigned char m = data[5];

       data += 6;
       size -= 6;

       for( unsigned char j = 0; j < m; j++ )
	  {
	    cIpmiInventoryFeedToFru *f = new cIpmiInventoryFeedToFru;
	    p->Add( f );

	    f->m_hardware_address = data[0];
	    f->m_fru_device_id = data[1];

	    data += 2;
	    size -= 2;
	  }
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryShelfPowerDistribution::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryShelfPowerDistribution::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int 
cIpmiInventoryShelfPowerDistribution::HpiSize() const
{
  assert( 0 );

  return 0;
}


void
cIpmiInventoryShelfPowerDistribution::HpiRead( SaHpiInventDataRecordT *r ) const
{
  assert( 0 );
}


void
cIpmiInventoryShelfPowerDistribution::HpiWrite( SaHpiInventDataRecordT *r )
{
  assert( 0 );
}


void
cIpmiInventoryShelfPowerDistribution::Dump( cIpmiLog &dump, const char *name ) const
{
  assert( 0 );
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryShelfActivationAndPower
////////////////////////////////////////////////////////////

cIpmiInventoryShelfActivationAndPower::cIpmiInventoryShelfActivationAndPower()
  : cIpmiInventoryMultiRecord( tIpmiMultiRecordTypeShelfActivationAndPowerManagement )
{
}


cIpmiInventoryShelfActivationAndPower::~cIpmiInventoryShelfActivationAndPower()
{
}


SaErrorT
cIpmiInventoryShelfActivationAndPower::IpmiRead( const unsigned char *data, unsigned int size )
{
  m_allowance = data[10];

  unsigned char n = data[11];

  data += 12;
  size -= 12;

  for( int i = 0; i < n; i++ )
     {
       cIpmiInventoryActivationAndPower *a = new cIpmiInventoryActivationAndPower;
       Add( a );

       a->m_hardware_address = data[0];
       a->m_fru_device_id = data[1];
       a->m_maximum_fru_power = IpmiGetUint16( data + 2 );
       a->m_activation = data[4] & 0x40; 
       a->m_delay = data[4] & 0x3f;
       data += 5;
       size -= 5;
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryShelfActivationAndPower::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryShelfActivationAndPower::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int 
cIpmiInventoryShelfActivationAndPower::HpiSize() const
{
  assert( 0 );

  return 0;
}


void
cIpmiInventoryShelfActivationAndPower::HpiRead( SaHpiInventDataRecordT *r ) const
{
  assert( 0 );
}


void
cIpmiInventoryShelfActivationAndPower::HpiWrite( SaHpiInventDataRecordT *r )
{
  assert( 0 );
}


void
cIpmiInventoryShelfActivationAndPower::Dump( cIpmiLog &dump, const char *name ) const
{
  assert( 0 );
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryRecordMultiRecord
////////////////////////////////////////////////////////////

cIpmiInventoryRecordMultiRecord::cIpmiInventoryRecordMultiRecord()
  : cIpmiInventoryRecord( eIpmiInventoryRecordTypeMultiRecord )
{
}


cIpmiInventoryRecordMultiRecord::~cIpmiInventoryRecordMultiRecord()
{
}


cIpmiInventoryMultiRecord *
cIpmiInventoryRecordMultiRecord::GetRecord( tIpmiMultiRecordType type ) const  
{
  for( int i = 0; i < Num(); i++ )
     {
       cIpmiInventoryMultiRecord *mr = operator[]( i );

       if ( mr->Type() == type )
	    return mr;
     }

  return 0;
}


void
cIpmiInventoryRecordMultiRecord::SetRecord( tIpmiMultiRecordType type, cIpmiInventoryMultiRecord *record )
{
  for( int i = 0; i < Num(); i++ )
     {
       cIpmiInventoryMultiRecord *mr = operator[]( i );

       if ( mr->Type() == type )
	  {
	    delete mr;

	    if ( record )
		 operator[]( i ) = record;
	    else
		 Rem( i );

	    return;
	  }
     }

  if ( record )
       Add( record );
}


cIpmiInventoryMultiRecord *
cIpmiInventoryRecordMultiRecord::AllocRecord( tIpmiMultiRecordType type )
{
  switch( type )
     {
       case tIpmiMultiRecordTypeBackplanePointToPointConnectivity:
	    return new cIpmiInventoryBackplanePointToPointConnectivity;

       case tIpmiMultiRecordTypeShelfPowerDistribution:
	    return new cIpmiInventoryShelfPowerDistribution;

       case tIpmiMultiRecordTypeShelfActivationAndPowerManagement:
	    return new cIpmiInventoryShelfActivationAndPower;
     }

  assert( 0 );

  return 0;
}


SaErrorT
cIpmiInventoryRecordMultiRecord::IpmiRead( const unsigned char *data, unsigned int size )  
{
  return SA_OK;

  while( size >= 5 )
     {
       // check header checksum
       unsigned char cs = IpmiChecksum( data, 5 );

       if ( cs != 0 )
	  {
	    stdlog << "wrong header checksum multi record !\n";
	    return SA_ERR_HPI_INVALID_DATA_FIELD;
	  }

       // data checksum
       cs = IpmiChecksum( data + 5, data[2] );

       if ( cs != data[4] )
	  {
	    stdlog << "wrong data checksum in multirecord !\n";
	    return SA_ERR_HPI_INVALID_DATA_FIELD;
	  }

       if ( data[0] == 0xc0 )
	  {
	    cIpmiInventoryMultiRecord *mr = AllocRecord( (tIpmiMultiRecordType)data[8] );

	    if ( mr )
	       {
		 mr->Version() = data[1] & 0x0f;
		 SaErrorT rv = mr->IpmiRead( data, data[2] );

		 if ( rv != SA_OK )
		    {
		      delete mr;
		      return rv;
		    }

		 SetRecord( mr->Type(), mr );
	       }
	    else
		 stdlog << "unknown PICMG multirecord " << data[8] << ".\n";
	  }
       else
	    stdlog << "unknown multirecord type " << data[0] << ".\n";

       size -= data[2] + 5;

       if ( data[1] & 0x80 )
	    break;

       // next record
       data += data[2] + 5;
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryRecordMultiRecord::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordMultiRecord::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryRecordMultiRecord::HpiSize( int record_id ) const
{
  if ( record_id < 0 || record_id >= Num() )
     {
       assert( 0 );
       return 0;
     }

  return operator[]( record_id )->HpiSize();
}


void
cIpmiInventoryRecordMultiRecord::HpiRead( int record_id,
					  SaHpiInventDataRecordT *r ) const
{
  if ( record_id < 0 || record_id >= Num() )
     {
       assert( 0 );
       return;
     }

  return operator[]( record_id )->HpiRead( r );
}


void
cIpmiInventoryRecordMultiRecord::HpiWrite( int record_id,
					   SaHpiInventDataRecordT *r )
{
  if ( record_id < 0 || record_id >= Num() )
     {
       assert( 0 );
       return;
     }

  operator[]( record_id )->HpiWrite( r );
}


void
cIpmiInventoryRecordMultiRecord::Dump( cIpmiLog &dump,
                                       int record_id, 
                                       const char *name ) const
{
  if ( record_id < 0 || record_id >= Num() )
     {
       assert( 0 );
       return;
     }

  operator[]( record_id )->Dump( dump, name );
}


////////////////////////////////////////////////////////////
//                  cIpmiInventoryParser
////////////////////////////////////////////////////////////

cIpmiInventoryParser::cIpmiInventoryParser()
{
  for( int i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
       m_records[i] = 0;
}


cIpmiInventoryParser::~cIpmiInventoryParser()
{
  Clear();
}


cIpmiInventoryRecord *
cIpmiInventoryParser::GetRecord( tIpmiInventoryRecordType type ) const
{
  return m_records[type];
}


void
cIpmiInventoryParser::SetRecord( tIpmiInventoryRecordType type, cIpmiInventoryRecord *record )
{
  if ( m_records[type] )
     {
       delete m_records[type];
       m_records[type] = 0;
     }

  m_records[type] = record;
}


void
cIpmiInventoryParser::Clear()
{
  for( int i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
       if ( m_records[i] )
	  {
	    delete m_records[i];
	    m_records[i] = 0;
	  }
}


cIpmiInventoryRecord *
cIpmiInventoryParser::AllocRecord( tIpmiInventoryRecordType type )
{
  switch( type )
     {
       case eIpmiInventoryRecordTypeInternal:
	    return new cIpmiInventoryRecordInternal;

       case eIpmiInventoryRecordTypeChassis:
	    return new cIpmiInventoryRecordChassis;

       case eIpmiInventoryRecordTypeBoard:
	    return new cIpmiInventoryRecordBoard;

       case eIpmiInventoryRecordTypeProduct:
	    return new cIpmiInventoryRecordProduct;

       case eIpmiInventoryRecordTypeMultiRecord:
	    return new cIpmiInventoryRecordMultiRecord;

       default:
	    assert( 0 );
	    break;
     }

  return 0;
}


SaErrorT
cIpmiInventoryParser::IpmiRead( const unsigned char *data, unsigned int size )
{
  if ( size < 8 )
     {
       stdlog << "Inventory data too short (" << size << " < 8) !\n";
       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  // checksum of common header
  if ( IpmiChecksum( data, 8 ) )
     {
       stdlog << "wrong common header checksum !\n";
       stdlog.Hex( data, 8 );
       stdlog << "\n";

       return SA_ERR_HPI_INVALID_PARAMS;
     }

  m_version = data[0] & 0x0f;

  // clear old
  Clear();

  unsigned int pos = size;

  for( int i = 5; i >= 1; i-- )
     {
       if ( data[i] == 0 )
	    continue;

       tIpmiInventoryRecordType type = (tIpmiInventoryRecordType)(i - 1);
       unsigned int offset = data[i] * 8;
       unsigned int len = pos - offset;

       stdlog << IpmiInventoryRecordTypeToString( type ) << ": offset " << offset << ", len " << len << "\n";
       m_records[type] = AllocRecord( type );
       SaErrorT rv = m_records[type]->IpmiRead( data + offset, len );

       if ( rv )
	  {
	    Clear();
	    return rv;
	  }

       pos -= len;
     }

  return SA_OK;
}


unsigned int
cIpmiInventoryParser::IpmiSize() const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryParser::IpmiWrite( unsigned char *data, unsigned int size ) const
{
  assert( 0 );

  return 0;
}


unsigned int
cIpmiInventoryParser::HpiSize() const
{
  unsigned int size = sizeof( SaHpiInventoryDataT );

  for( int i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
     {
       if ( m_records[i] == 0 )
	    continue;

       int nr = m_records[i]->HpiNumRecords();

       for( int j = 0; j < nr; j++ )
	  {
	    size += m_records[i]->HpiSize( j );
	    size += sizeof( SaHpiInventDataRecordT * );
	  }
     }

#ifndef NDEBUG
  // overwrite check
  size += 256;
#endif

  return size;
}


SaErrorT
cIpmiInventoryParser::HpiRead( SaHpiInventoryDataT &data ) const
{
  unsigned char *d = (unsigned char *)&data;
  unsigned s = HpiSize();

#ifndef NDEBUG
  int l;

  memset( d, 0, s+256 );
  for( l = 0; l < 256; l++ )
       d[s+l] = l;
#else
  memset( d, 0, s );
#endif

  d += sizeof( SaHpiInventoryDataT );

  // how many records
  for( int i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
     {
       if ( m_records[i] == 0 )
	    continue;

       int nr = m_records[i]->HpiNumRecords();

       d += nr * sizeof( SaHpiInventDataRecordT * );
     }

  int pos = 0;

  for( int i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
     {
       if ( m_records[i] == 0 )
	    continue;

       int nr = m_records[i]->HpiNumRecords();

       for( int j = 0; j < nr; j++ )
	  {
	    data.DataRecords[pos++] = (SaHpiInventDataRecordT *)(void *)d;
	    m_records[i]->HpiRead( j, (SaHpiInventDataRecordT *)(void *)d );
	    d += m_records[i]->HpiSize( j );
	  }
     }

#ifndef NDEBUG
  // check for overwrite
  d = (unsigned char *)&data;

  for( l = 0; l < 256; l++ )
       assert( d[s+l] == l );
#endif

  return SA_OK;
}


SaErrorT
cIpmiInventoryParser::HpiWrite( const SaHpiInventoryDataT &data )
{
  assert( 0 );

  return SA_OK;
}
