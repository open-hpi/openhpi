/*
 * ipmi_fru.h
 *
 * Copyright (c) 2003,2004 by FORCE Computers
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
#include <errno.h>

#include "ipmi_domain.h"
#include "ipmi_utils.h"
#include "ipmi_fru.h"


static const char *fru_state[] =
{
  "not installed",
  "inactive",
  "activation request",
  "activation in progress",
  "active",
  "deactivation request",
  "deactivation in progress",
  "communication lost"
};


const char *
IpmiFruStateToString( tIpmiFruState val )
{
  if ( val > eIpmiFruStateCommunicationLost )
       return "invalid";

  return fru_state[val];
}


static tIpmiFruItemDesc chassis_desc[] =
{
  { dIpmiFruItemVersion     , eIpmiFruItemTypeInt },
  { ""                      , eIpmiFruItemTypePad }, // ignore length
  { dIpmiFruItemChassisType , eIpmiFruItemTypeInt },
  { dIpmiFruItemPartNumber  , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemSerialNumber, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemCustom      , eIpmiFruItemTypeCustom },
  { 0, eIpmiFruItemTypeUnknown }
};


static tIpmiFruItemDesc board_desc[] =
{
  { dIpmiFruItemVersion     , eIpmiFruItemTypeInt },
  { ""                      , eIpmiFruItemTypePad }, // ignore length
  { dIpmiFruItemLanguage    , eIpmiFruItemTypeInt },
  { dIpmiFruItemMfgDate     , eIpmiFruItemTypeDate3 },
  { dIpmiFruItemManufacturer, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemProductName , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemSerialNumber, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemPartNumber  , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemFruFileId   , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemCustom      , eIpmiFruItemTypeCustom },
  { 0, eIpmiFruItemTypeUnknown }
};

static tIpmiFruItemDesc product_desc[] =
{
  { dIpmiFruItemVersion     , eIpmiFruItemTypeInt },
  { ""                      , eIpmiFruItemTypePad }, // ignore length
  { dIpmiFruItemLanguage    , eIpmiFruItemTypeInt },
  { dIpmiFruItemManufacturer, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemProductName , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemPartNumber  , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemProductVersion, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemSerialNumber, eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemAssetTag    , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemFruFileId   , eIpmiFruItemTypeTextBuffer },
  { dIpmiFruItemCustom      , eIpmiFruItemTypeCustom },
  { 0, eIpmiFruItemTypeUnknown }
};


//////////////////////////////////////////////////
//                  cIpmiFruItem
//////////////////////////////////////////////////

cIpmiFruItem::cIpmiFruItem( const char *name, tIpmiFruItemType type )
  : m_type( type )
{
  assert( name );
  strcpy( m_name, name );

  m_u.m_data.m_data = 0;
}


cIpmiFruItem::~cIpmiFruItem()
{
  if ( m_type == eIpmiFruItemTypeData && m_u.m_data.m_data )
       delete [] m_u.m_data.m_data;
  else if ( m_type == eIpmiFruItemTypeRecord && m_u.m_record )
       delete m_u.m_record;
}


/*
void
cIpmiFruItem::Log()
{
  switch( m_type )
     {
       case eIpmiFruItemTypeTextBuffer:
            {
              char str[256] = "";
              m_u.m_text_buffer.GetAscii( str, 256 );
              str[255] = 0;

              stdlog << "\t" << m_name << "\t\t" << str << "\n";
            }
            break;

       case eIpmiFruItemTypeInt:
            stdlog << "\t" << m_name << "\t\t" << m_u.m_int << " ";
            stdlog.Hex( true );
            stdlog << m_u.m_int << "\n";
            stdlog.Hex( false );
            break;

       default:
            stdlog << "\t" << m_name << "\t\tunknown\n";
            break;
     }
}
*/


//////////////////////////////////////////////////
//                  cIpmiFruRecord
//////////////////////////////////////////////////

cIpmiFruRecord::cIpmiFruRecord( const char *name )
  : m_array( 0 ), m_num( 0 )
{
  strcpy( m_name, name );
}


cIpmiFruRecord::~cIpmiFruRecord()
{
  Clear();
}


void
cIpmiFruRecord::Add( cIpmiFruItem *item )
{
  cIpmiFruItem **array = new cIpmiFruItem *[m_num+1];

  if ( m_array )
     {
       memcpy( array, m_array, sizeof( cIpmiFruItem * ) * m_num );
       delete [] m_array;
     }

  m_array = array;
  m_array[m_num++] = item;
}


void
cIpmiFruRecord::Clear()
{
  if ( m_array )
     {
       for( int i = 0; i < m_num; i++ )
            delete m_array[i];

       delete [] m_array;
       m_array = 0;
       m_num = 0;
     }
}


cIpmiFruItem *
cIpmiFruRecord::Find( const char *name )
{
  for( int i = 0; i < m_num; i++ )
       if ( !strcmp( m_array[i]->m_name, name ) )
            return m_array[i];

  return 0;
}


SaHpiTextBufferT *
cIpmiFruRecord::SetItem( unsigned char *&p, int &s, const char *name )
{
  SaHpiTextBufferT *t = (SaHpiTextBufferT *)p;
  memset( t->Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH );

  cIpmiFruItem *i = Find( name );

  if ( i )
     {
       assert( i->m_type == eIpmiFruItemTypeTextBuffer );
       *t = i->m_u.m_text_buffer;
     }
  else
     {
       t->DataType = SAHPI_TL_TYPE_BINARY;
       t->Language = SAHPI_LANG_UNDEF;
       t->DataLength = 0;
     }

  s += sizeof( SaHpiTextBufferT );
  p += sizeof( SaHpiTextBufferT );

  return t;
}


int
cIpmiFruRecord::GeneralDataRecord( SaHpiInventGeneralDataT *r )
{
  int size = 0;
  unsigned char *p = (unsigned char *)r + sizeof( SaHpiInventGeneralDataT );

  // calculate number of custom fields
  unsigned int idx = 0;

  while( true )
     {
       char str[dIpmiFruItemNameLen];
       sprintf( str, "%s%d", dIpmiFruItemCustom, idx + 1 );

       if ( Find( str ) == 0 )
            break;

       idx++;
     }

  size = idx * sizeof( SaHpiTextBufferT * );
  p += size;

  // mfg date
  r->MfgDateTime = SAHPI_TIME_UNSPECIFIED;

  cIpmiFruItem *i = Find( dIpmiFruItemMfgDate );

  if ( i )
     {
       r->MfgDateTime = (SaHpiTimeT)i->m_u.m_int;
       r->MfgDateTime *= 1000000000;
     }

  r->Manufacturer   = SetItem( p, size, dIpmiFruItemManufacturer );
  r->ProductName    = SetItem( p, size, dIpmiFruItemProductName );
  r->ProductVersion = SetItem( p, size, dIpmiFruItemProductVersion );
  r->ModelNumber    = SetItem( p, size, "ModelNumber" ); // just to create an empty text buffer
  r->SerialNumber   = SetItem( p, size, dIpmiFruItemSerialNumber );
  r->PartNumber	    = SetItem( p, size, dIpmiFruItemPartNumber );
  r->FileId 	    = SetItem( p, size, dIpmiFruItemFruFileId );
  r->AssetTag       = SetItem( p, size,  dIpmiFruItemAssetTag);

  // custom fields
  idx = 0;

  while( true )
     {
       char str[dIpmiFruItemNameLen];
       sprintf( str, "%s%d", dIpmiFruItemCustom, idx + 1 );

       if ( !Find( str ) )
            break;

       r->CustomField[idx] = SetItem( p, size, str ); 
       idx++;
     }

  r->CustomField[idx] = 0;

  return size;
}


int
cIpmiFruRecord::InternalUseRecord( SaHpiInventInternalUseDataT *r )
{
  int s = 0;

  cIpmiFruItem *i = Find( dIpmiFruItemData );

  if ( i )
     {
       memcpy( r->Data, i->m_u.m_data.m_data, i->m_u.m_data.m_size );
       s += i->m_u.m_data.m_size;
     }

  return s;
}


int
cIpmiFruRecord::ChassisInfoAreaRecord( SaHpiInventChassisDataT *r )
{
  int s = sizeof( SaHpiInventChassisDataT );

  r->Type = SAHPI_INVENT_CTYP_UNKNOWN;

  cIpmiFruItem *i = Find( dIpmiFruItemChassisType );

  if ( i )
       r->Type = (SaHpiInventChassisTypeT)i->m_u.m_int;

  return s + GeneralDataRecord( &r->GeneralData );
}


int
cIpmiFruRecord::BoradInfoAreaRecord( SaHpiInventGeneralDataT *r )
{
  int s = sizeof( SaHpiInventGeneralDataT );

  return s + GeneralDataRecord( r );
}


int
cIpmiFruRecord::ProductInfoAreaRecord( SaHpiInventGeneralDataT *r )
{
  int s = sizeof( SaHpiInventGeneralDataT );

  return s + GeneralDataRecord( r );
}


/*
void
cIpmiFruRecord::Log()
{
  for( int i = 0; i < m_num; i++ )
       m_array[i]->Log();
}
*/


//////////////////////////////////////////////////
//                  cIpmiFru
//////////////////////////////////////////////////

cIpmiFru::cIpmiFru( cIpmiMc *mc, unsigned int fru_device_id )
  : cIpmiRdr( mc, SAHPI_INVENTORY_RDR ), m_fru_device_id( fru_device_id ),
    m_size( 0 ), m_access( eFruAccessModeByte ),
    m_array( 0 ), m_num( 0 ),
    m_oem( 0 ), m_inventory_size( 0 )
{
  char str[80];
  sprintf( str, "FRU%d", fru_device_id );

  m_id_string.SetAscii( str, SAHPI_TL_TYPE_LANGUAGE, SAHPI_LANG_ENGLISH );
}


cIpmiFru::~cIpmiFru()
{
  Clear();
}


void
cIpmiFru::Add( cIpmiFruRecord *record )
{
  cIpmiFruRecord **array = new cIpmiFruRecord *[m_num+1];

  if ( m_array )
     {
       memcpy( array, m_array, sizeof( cIpmiFruRecord * ) * m_num );
       delete [] m_array;
     }

  m_array = array;
  m_array[m_num++] = record;
}


void
cIpmiFru::Clear()
{
  if ( m_array )
     {
       for( int i = 0; i < m_num; i++ )
            delete m_array[i];

       delete [] m_array;
       m_array = 0;
       m_num = 0;
     }

  m_inventory_size = 0;
}


cIpmiFruRecord *
cIpmiFru::Find( const char *name )
{
  for( int i = 0; i < m_num; i++ )
       if ( !strcmp( m_array[i]->m_name, name ) )
            return m_array[i];

  return 0;
}


int
cIpmiFru::GetFruInventoryAreaInfo( unsigned int &size,
                                   tFruAccessMode &byte_access )
{
  cIpmiAddr addr( eIpmiAddrTypeIpmb, m_entity->Channel(),
                  m_entity->Lun(), m_entity->AccessAddress() );
  cIpmiMsg  msg( eIpmiNetfnStorage, eIpmiCmdGetFruInventoryAreaInfo );
  cIpmiMsg  rsp;

  msg.m_data[0] = m_fru_device_id;
  msg.m_data_len = 1;

  int rv = m_entity->Domain()->SendCommand( addr, msg, rsp );

  if ( rv )
     {
       stdlog << "cannot GetFruInventoryAreaInfo: " << rv << " !\n";
       return rv;
     }

  if ( rsp.m_data[0] != eIpmiCcOk )
     {
       stdlog << "cannot GetFruInventoryAreaInfo: "
              << IpmiCompletionCodeToString( (tIpmiCompletionCode)rsp.m_data[0] ) << " !\n";

       return EINVAL;
     }

  byte_access = (rsp.m_data[3] & 1) ? eFruAccessModeWord : eFruAccessModeByte;
  size = IpmiGetUint16( rsp.m_data + 1 ) >> byte_access;

  return 0;
}


int
cIpmiFru::ReadFruData( unsigned short offset, unsigned int num, unsigned int &n, unsigned char *data )
{
  cIpmiAddr addr( eIpmiAddrTypeIpmb, m_entity->Channel(),
                  m_entity->Lun(), m_entity->AccessAddress() );
  cIpmiMsg  msg( eIpmiNetfnStorage, eIpmiCmdReadFruData );
  cIpmiMsg  rsp;

  msg.m_data[0] = m_fru_device_id;
  IpmiSetUint16( msg.m_data + 1, offset >> m_access );
  msg.m_data[3] = num >> m_access;
  msg.m_data_len = 4;

  int rv = m_entity->Domain()->SendCommand( addr, msg, rsp );

  if ( rv )
     {
       stdlog << "cannot ReadFruData: " << rv << " !\n";
       return rv;
     }

  if ( rsp.m_data[0] != eIpmiCcOk )
     {
       stdlog << "cannot ReadFruData: "
              << IpmiCompletionCodeToString( (tIpmiCompletionCode)rsp.m_data[0] ) << " !\n";

       return EINVAL;
     }

  n = rsp.m_data[1] << m_access;

  if ( n < 1 )
     {
       stdlog << "ReadFruData: read 0 bytes !\n";

       return EINVAL;
     }

  memcpy( data, rsp.m_data + 2, n );

  return 0;
}


int
cIpmiFru::Fetch()
{
  m_fetched = false;
  Clear();

  int rv = GetFruInventoryAreaInfo( m_size,
                                    m_access );

  if ( rv || m_size == 0 )
       return rv;

  unsigned short offset = 0;
  unsigned char *data = new unsigned char[m_size];

  while( offset < m_size )
     {
       unsigned int num = m_size - offset;

       if ( num > dMaxFruFetchBytes )
            num = dMaxFruFetchBytes;

       unsigned int n;

       rv = ReadFruData( offset, num, n, data + offset );

       if ( rv )
          {
            delete [] data;
            return rv;
          }

       offset += n;
     }

  rv = CreateInventory( data );

  delete [] data;

  m_fetched = rv ? false : true;

  return rv;
}


static unsigned char
checksum( const unsigned char *data, int size )
{
  unsigned char c = 0;

  while( size-- )
      c += *data++;

  return c;
}


int
cIpmiFru::CreateInventory( const unsigned char *data )
{
  stdlog << "MC " << m_entity->AccessAddress() << " FRU Inventory " << m_fru_device_id << "\n";

  if ( m_size < 8 )
     {
       stdlog << "FRU data too short (" << m_size << " < 8) !\n";
       return EINVAL;
     }

  if ( checksum( data, 8 ) )
     {
       stdlog << "wrong FRU header checksum !\n";
       stdlog.Hex( data, 8 );
       stdlog << "\n";

       return EINVAL;
     }

  unsigned int pos = m_size;
  unsigned int offset;
  unsigned int len;

  if ( data[5] )
     {
       offset = data[5] * 8;
       len = pos - offset;

       stdlog << "MultiRecord: offset " << offset << ", len " << len << "\n";

       CreateMultiRecord( data + offset, len );
       pos -= len;
     }

  if ( data[4] )
     {
       offset = data[4] * 8;
       len = pos - offset;

       stdlog << "Product: offset " << offset << ", len " << len << "\n";

       CreateProduct( data + offset, len );

       pos -= len;
     }

  if ( data[3] )
     {
       offset = data[3] * 8;
       len = pos - offset;

       stdlog << "Board: offset " << offset << ", len " << len << "\n";

       CreateBoard( data + offset, len );
       pos -= len;
     }

  if ( data[2] )
     {
       offset = data[2] * 8;
       len = pos - offset;

       stdlog << "Chassis: offset " << offset << ", len " << len << "\n";

       CreateChassis( data + offset, len );
       pos -= len;
     }

  if ( data[1] )
     {
       offset = data[1] * 8;
       len = pos - offset;

       stdlog << "Internal: offset " << offset << ", len " << len << "\n";

       CreateInternalUse( data + offset, len );
       pos -= len;
     }

  return 0;
}


int
cIpmiFru::CreateRecord( const char *name, tIpmiFruItemDesc *desc, 
                        const unsigned char *data, unsigned int /*len*/ )
{
  cIpmiFruRecord *r = new cIpmiFruRecord( name );
  cIpmiFruItem *item;
  int i;

  while( desc->m_name )
     {
       item = 0;

       switch( desc->m_type )
          {
            case eIpmiFruItemTypeTextBuffer:
		 {
		   cIpmiTextBuffer tb;
		   data = tb.SetIpmi( data );
		   assert( data );

		   item = new cIpmiFruItem( desc->m_name, eIpmiFruItemTypeTextBuffer );
		   item->m_u.m_text_buffer = tb;
		 }

                 break;

            case eIpmiFruItemTypeInt:
                 item = new cIpmiFruItem( desc->m_name, eIpmiFruItemTypeInt );
                 item->m_u.m_int = *data++;

                 break;
  
            case eIpmiFruItemTypeDate3:
                 {
                   item = new cIpmiFruItem( desc->m_name, eIpmiFruItemTypeInt );

                   time_t t;
                   struct tm tmt;

                   // minutes since 1996.01.01 00:00:00
                   t = *data++ << 16;
                   t += *data++ << 8;
                   t += *data++;
                   t *= 60;

                   // create date offset
                   tmt.tm_sec  = 0;
                   tmt.tm_min  = 0;
                   tmt.tm_hour = 0;
                   tmt.tm_mday = 1;
                   tmt.tm_mon  = 0;
                   tmt.tm_year = 96;
                   tmt.tm_isdst = 0;

                   item->m_u.m_int = t;
                   item->m_u.m_int += mktime( &tmt );
                 }

                 break;

            case eIpmiFruItemTypeCustom:
                 for( i = 0; i < 20; i++ )
                    {
                      char str[dIpmiFruItemNameLen];
                      sprintf( str, "%s%d", desc->m_name, i + 1 );

		      cIpmiTextBuffer tb;
		      data = tb.SetIpmi( data );

                      if ( data )
			 {
			   item = new cIpmiFruItem( str, eIpmiFruItemTypeTextBuffer );
			   item->m_u.m_text_buffer = tb;
                           r->Add( item );
			 }
                      else
                         {
                           Add( r );
                           // r->Log();

                           return 0;
                         }
                    }

                 // something goes wrong
                 assert( 0 );
                 return EINVAL;

            case eIpmiFruItemTypePad:
                 data++;
                 break;

            default:
                 assert( 0 );
                 break;
          }

       if ( item )
            r->Add( item );

       desc++;
     }

  Add( r );
  // r->Log();

  return 0;
}


int
cIpmiFru::CreateInternalUse( const unsigned char *data, unsigned int len )
{
  cIpmiFruRecord *r = new cIpmiFruRecord( dIpmiFruRecordInternalUseArea );
  
  // create version item
  cIpmiFruItem *i = new cIpmiFruItem( dIpmiFruItemVersion, eIpmiFruItemTypeInt );
  i->m_u.m_int = *data++;
  len--;

  r->Add( i );

  // create data item
  unsigned char *d = new unsigned char[len];
  memcpy( d, data, len );

  i = new cIpmiFruItem( dIpmiFruItemData, eIpmiFruItemTypeData );
  i->m_u.m_data.m_data = d;
  i->m_u.m_data.m_size = len;

  Add( r );

  //  r->Log();

  return 0;
}


int
cIpmiFru::CreateChassis( const unsigned char *data, unsigned int len )
{
  return CreateRecord( dIpmiFruRecordChassisInfoArea, 
                       chassis_desc, data, len );
}


int
cIpmiFru::CreateBoard( const unsigned char *data, unsigned int len )
{
  return CreateRecord( dIpmiFruRecordBoardInfoArea,
                       board_desc, data, len );
}


int
cIpmiFru::CreateProduct( const unsigned char *data, unsigned int len )
{
  return CreateRecord( dIpmiFruRecordProductInfoArea, 
                       product_desc, data, len );
}


int
cIpmiFru::CreateMultiRecord( const unsigned char * /*data*/, unsigned int /*len*/ )
{
  return 0;
}


bool
cIpmiFru::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  if ( cIpmiRdr::CreateRdr( resource, rdr ) == false )
       return false;

  if ( !(resource.ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA ) )
     {
       // update resource
       resource.ResourceCapabilities |= SAHPI_CAPABILITY_RDR|SAHPI_CAPABILITY_INVENTORY_DATA;

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

  // control record
  SaHpiInventoryRecT &rec = rdr.RdrTypeUnion.InventoryRec;
  rec.EirId = Num();
  rec.Oem = m_oem;

  return true;
}


unsigned int
cIpmiFru::GetInventoryInfo( SaHpiInventoryDataT &data )
{
  int size = sizeof( SaHpiInventoryDataT )
    + NumRecords() * sizeof( SaHpiInventDataRecordT * );

  data.Validity = SAHPI_INVENT_DATA_VALID;

  // position of the first record
  unsigned char *p = (unsigned char *)&data + size;

  int idx = 0;

  for( int i = NumRecords() - 1; i >= 0; i-- )
     {
       cIpmiFruRecord *fr = GetRecord( i );
       SaHpiInventDataRecordT *r = (SaHpiInventDataRecordT *)p;
       int s;

       if ( !strcmp( fr->m_name, dIpmiFruRecordInternalUseArea ) )
	  {
	    s = fr->InternalUseRecord( &r->RecordData.InternalUse );
	    r->RecordType = SAHPI_INVENT_RECTYPE_INTERNAL_USE;
	  }
       else if ( !strcmp( fr->m_name, dIpmiFruRecordChassisInfoArea ) )
	  {
	    s = fr->ChassisInfoAreaRecord( &r->RecordData.ChassisInfo );
	    r->RecordType = SAHPI_INVENT_RECTYPE_CHASSIS_INFO;
	  }
       else if ( !strcmp( fr->m_name, dIpmiFruRecordBoardInfoArea ) )
	  {
            s = fr->BoradInfoAreaRecord( &r->RecordData.BoardInfo );
	    r->RecordType = SAHPI_INVENT_RECTYPE_BOARD_INFO;
	  }
       else if ( !strcmp( fr->m_name, dIpmiFruRecordProductInfoArea ) )
	  {
	    s = fr->ProductInfoAreaRecord( &r->RecordData.ProductInfo );
	    r->RecordType = SAHPI_INVENT_RECTYPE_PRODUCT_INFO;
	  }
       else if ( !strcmp( fr->m_name, dIpmiFruRecordMultiRecord ) )
	    continue;
       else
	  {
	    assert( 0 );
	    continue;
          }

       data.DataRecords[idx++] = (SaHpiInventDataRecordT *)p;
       r->DataLength = s;
       s += sizeof( SaHpiInventDataRecordT ) - sizeof( SaHpiInventDataUnionT );
       p += s;
       size += s;
     }

  // end mark
  data.DataRecords[NumRecords()] = 0;

  return size;
}


unsigned int
cIpmiFru::CalcSize()
{
  // calulate fru inventory size
  unsigned char *buffer = new unsigned char[1024*128];
  SaHpiInventoryDataT *d = (SaHpiInventoryDataT *)buffer;
  m_inventory_size = GetInventoryInfo( *d );
  delete [] buffer;

#if 1
  // check for overflow
  unsigned int n = 256;

  buffer = new unsigned char[m_inventory_size+n];

  unsigned char *b = buffer + m_inventory_size;
  unsigned int i;

  for( i = 0; i < n; i++ )
       *b++ = (unsigned char)i;

  d = (SaHpiInventoryDataT *)buffer;
  unsigned int s = GetInventoryInfo( *d );

  assert( s == m_inventory_size );
  
  b = buffer + m_inventory_size;

  for( i = 0; i < n; i++, b++ )
     {
       //printf( "%d = 0x%02x\n", i, *b );
       assert( *b == i );
     }

  delete [] buffer;
#endif

  return m_inventory_size;
}
