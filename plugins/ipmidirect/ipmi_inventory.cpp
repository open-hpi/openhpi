/*
 * ipmi_inventory.h
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


#include <string.h>
#include <assert.h>
#include <errno.h>

#include "ipmi_domain.h"
#include "ipmi_inventory.h"


//////////////////////////////////////////////////
//                  cIpmiInventory
//////////////////////////////////////////////////

cIpmiInventory::cIpmiInventory( cIpmiMc *mc, unsigned int fru_device_id )
  : cIpmiRdr( mc, SAHPI_INVENTORY_RDR ), m_fru_device_id( fru_device_id ),
    m_access( eInventoryAccessModeByte ), m_size( 0 ),
    m_oem( 0 )
{
  char str[80];
  sprintf( str, "FRU%d", fru_device_id );

  m_id_string.SetAscii( str, SAHPI_TL_TYPE_LANGUAGE, SAHPI_LANG_ENGLISH );
}


cIpmiInventory::~cIpmiInventory()
{
}


SaErrorT
cIpmiInventory::GetFruInventoryAreaInfo( unsigned int &size,
					 tInventoryAccessMode &byte_access )
{
  cIpmiMsg msg( eIpmiNetfnStorage, eIpmiCmdGetFruInventoryAreaInfo );
  msg.m_data[0] = m_fru_device_id;
  msg.m_data_len = 1;

  cIpmiMsg rsp;

  SaErrorT rv = SendCommand( msg, rsp );

  if ( rv != SA_OK )
     {
       stdlog << "cannot GetFruInventoryAreaInfo: " << rv << " !\n";
       return rv;
     }

  if ( rsp.m_data[0] != eIpmiCcOk )
     {
       stdlog << "cannot GetFruInventoryAreaInfo: "
              << IpmiCompletionCodeToString( (tIpmiCompletionCode)rsp.m_data[0] ) << " !\n";

       return SA_ERR_HPI_INVALID_PARAMS;
     }

  byte_access = (rsp.m_data[3] & 1) ? eInventoryAccessModeWord : eInventoryAccessModeByte;
  size = IpmiGetUint16( rsp.m_data + 1 ) >> byte_access;

  return SA_OK;
}


SaErrorT
cIpmiInventory::ReadFruData( unsigned short offset, unsigned int num, unsigned int &n, unsigned char *data )
{
  cIpmiMsg msg( eIpmiNetfnStorage, eIpmiCmdReadFruData );
  msg.m_data[0] = m_fru_device_id;
  IpmiSetUint16( msg.m_data + 1, offset >> m_access );
  msg.m_data[3] = num >> m_access;
  msg.m_data_len = 4;

  cIpmiMsg rsp;

  SaErrorT rv = SendCommand( msg, rsp );

  if ( rv != SA_OK )
     {
       stdlog << "cannot ReadFruData: " << rv << " !\n";
       return rv;
     }

  if ( rsp.m_data[0] != eIpmiCcOk )
     {
       stdlog << "cannot ReadFruData: "
              << IpmiCompletionCodeToString( (tIpmiCompletionCode)rsp.m_data[0] ) << " !\n";

       return SA_ERR_HPI_INVALID_PARAMS;
     }

  n = rsp.m_data[1] << m_access;

  if ( n < 1 )
     {
       stdlog << "ReadFruData: read 0 bytes !\n";

       return SA_ERR_HPI_INVALID_PARAMS;
     }

  memcpy( data, rsp.m_data + 2, n );

  return SA_OK;
}


SaErrorT
cIpmiInventory::Fetch()
{
  m_fetched = false;
  Clear();

  SaErrorT rv = GetFruInventoryAreaInfo( m_size,
                                         m_access );

  if ( rv != SA_OK || m_size == 0 )
       return  rv != SA_OK ? rv : SA_ERR_HPI_DATA_LEN_INVALID;

  unsigned short offset = 0;
  unsigned char *data = new unsigned char[m_size];

  while( offset < m_size )
     {
       unsigned int num = m_size - offset;

       if ( num > dMaxFruFetchBytes )
            num = dMaxFruFetchBytes;

       unsigned int n;

       rv = ReadFruData( offset, num, n, data + offset );

       if ( rv != SA_OK )
          {
            delete [] data;
            return rv;
          }

       offset += n;
     }

  rv = IpmiRead( data, m_size );

  delete [] data;

  m_fetched = ((rv != SA_OK) ? false : true);

  return rv;
}


bool
cIpmiInventory::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
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


void
cIpmiInventory::Dump( cIpmiLog &dump, const char *name ) const
{
  int i;
  char str[80];
  sprintf( str, "FruRecord%02x_%d_", Mc()->GetAddress(), m_fru_device_id );

  int nr = 0;
  int r  = 0;

  for( i = 0; i < eIpmiInventoryRecordTypeLast; i++ )
     {
       const cIpmiInventoryRecord *ir = GetRecord( (tIpmiInventoryRecordType)i );

       if ( ir == 0 )
            continue;

       nr += ir->IpmiNumRecords();

       for( int j = 0; j < ir->IpmiNumRecords(); j++ )
          {
            char s[80];
            sprintf( s, "%s%d", str, r++ );

            ir->Dump( dump, j, s );
          }
     }

  dump.Begin( "FruDevice", name );

  dump.Entry( "DeviceId" ) << (int)m_fru_device_id << ";\n";
  dump.Entry( "Access" ) << ( ( m_access == eInventoryAccessModeWord )
                              ? "dFruAccessWord" : "dFruAccessByte" ) << ";\n";

  if ( nr )
       dump.Entry( "FruRecords" );

  for( i = 0; i < nr; i++ )
     {
       if ( i != 0 )
            dump << ", ";

       dump << str << i;
     }

  if ( nr )
       dump << ";\n";

  dump.End();
}
