/*
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
 */

#include "marshal_hpi_types.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static int
cmp_text_buffer( SaHpiTextBufferT *d1, SaHpiTextBufferT *d2 )
{
  int b1 = d1 ? 1 : 0;
  int b2 = d2 ? 1 : 0;

  if ( b1 != b2 )
       return 0;

  if ( d1 == 0 )
       return 1;

  if ( d1->DataType != d2->DataType )
       return 0;

  if ( d1->Language != d2->Language )
       return 0;

  if ( d1->DataLength != d2->DataLength )
       return 0;

  return memcmp( d1->Data, d2->Data, d1->DataLength ) ? 0 : 1;
}

static unsigned char *
text_buffer( unsigned char *b, SaHpiTextBufferT **tt, SaHpiTextTypeT type, SaHpiLanguageT lang, SaHpiUint8T l,
	     const char *data )
{
  *tt = (SaHpiTextBufferT *)b;
  SaHpiTextBufferT *t = *tt;
  t->DataType = type;
  t->Language = lang;
  t->DataLength = l;
  strcpy( t->Data, data );

  return b + sizeof( SaHpiTextBufferT );
}


int
main( int argc, char *argv[] )
{
  unsigned char buffer1[10240];

// TODO  SaHpiInventoryDataT *d = (SaHpiInventoryDataT *)buffer1;
// TODO d->Validity = SAHPI_INVENT_DATA_OVERFLOW;

  // use 5 records
// TODO  unsigned char *b = buffer1 + sizeof( SaHpiInventoryDataT ) + 5 * sizeof( SaHpiInventDataRecordT * );

  // 0 record: InternalUse
// TODO  d->DataRecords[0] = (SaHpiInventDataRecordT *)b;
// TODO  SaHpiInventDataRecordT *r = (SaHpiInventDataRecordT *)b;
// TODO  r->RecordType = SAHPI_INVENT_RECTYPE_INTERNAL_USE;
// TODO  r->DataLength = 7;
// TODO  r->RecordData.InternalUse.Data[0] = 0x42;
// TODO  r->RecordData.InternalUse.Data[1] = 0x43;
// TODO  r->RecordData.InternalUse.Data[2] = 0x44;
// TODO  r->RecordData.InternalUse.Data[3] = 0x45;
// TODO  r->RecordData.InternalUse.Data[4] = 0x46;
// TODO  r->RecordData.InternalUse.Data[5] = 0x47;
// TODO  r->RecordData.InternalUse.Data[6] = 0x48;

// TODO  b += sizeof( SaHpiInventDataRecordT ) + 6;

  // 1 record: ChassisInfo
// TODO  d->DataRecords[1] = (SaHpiInventDataRecordT *)b;
// TODO  r = (SaHpiInventDataRecordT *)b;
// TODO  r->RecordType = SAHPI_INVENT_RECTYPE_CHASSIS_INFO;
// TODO  r->DataLength = 8;

// TODO  b += sizeof( SaHpiInventDataRecordT );
// TODO  SaHpiInventChassisDataT *c = &r->RecordData.ChassisInfo;
// TODO  c->Type = SAHPI_INVENT_CTYP_SPACE_SAVING;
// TODO  c->GeneralData.MfgDateTime = 0x4713;

// TODO  b = text_buffer( b, &c->GeneralData.Manufacturer, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_SERBIAN, 7, "1234567" );
// TODO  c->GeneralData.ProductName = 0;
// TODO  b = text_buffer( b, &c->GeneralData.ProductVersion, SAHPI_TL_TYPE_ASCII6, SAHPI_LANG_LATIN, 7, "1234567" );
// TODO  b = text_buffer( b, &c->GeneralData.ModelNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_MALAY, 7, "1234567" );
// TODO  b = text_buffer( b, &c->GeneralData.SerialNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_NORWEGIAN, 7, "1234567" );
// TODO  b = text_buffer( b, &c->GeneralData.PartNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_FRENCH, 7, "1234567" );
// TODO  b = text_buffer( b, &c->GeneralData.FileId, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_HAUSA, 7, "1234567" );
// TODO  b = text_buffer( b, &c->GeneralData.AssetTag, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_AYMARA, 7, "1234567" );
// TODO  c->GeneralData.CustomField[0] = 0;

  // 2 record: BoardInfo
// TODO  d->DataRecords[2] = (SaHpiInventDataRecordT *)b;
// TODO  r = (SaHpiInventDataRecordT *)b;
// TODO  r->RecordType = SAHPI_INVENT_RECTYPE_BOARD_INFO;
// TODO  r->DataLength = 9;

// TODO  b += sizeof( SaHpiInventDataRecordT );
// TODO  SaHpiInventGeneralDataT *g = &r->RecordData.BoardInfo;

// TODO  b = text_buffer( b, &g->Manufacturer, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_SERBIAN, 7, "1234567" );
// TODO  g->ProductName = 0;
// TODO  b = text_buffer( b, &g->ProductVersion, SAHPI_TL_TYPE_ASCII6, SAHPI_LANG_LATIN, 7, "abc" );
// TODO  b = text_buffer( b, &g->ModelNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_MALAY, 7, "tttt" );
// TODO  b = text_buffer( b, &g->SerialNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_NORWEGIAN, 7, "durban" );
// TODO  b = text_buffer( b, &g->PartNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_FRENCH, 7, "luxor" );
// TODO  b = text_buffer( b, &g->FileId, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_HAUSA, 7, "maun" );
// TODO  b = text_buffer( b, &g->AssetTag, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_AYMARA, 7, "sydney" );
// TODO  g->CustomField[0] = 0;

  // 3 record: ProductInfo
// TODO  d->DataRecords[3] = (SaHpiInventDataRecordT *)b;
// TODO  r = (SaHpiInventDataRecordT *)b;
// TODO  r->RecordType = SAHPI_INVENT_RECTYPE_PRODUCT_INFO;
// TODO  r->DataLength = 11;

// TODO  b += sizeof( SaHpiInventDataRecordT );
// TODO  g = &r->RecordData.ProductInfo;

// TODO  b = text_buffer( b, &g->Manufacturer, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_SERBIAN, 7, "belgrad" );
// TODO  g->ProductName = 0;
// TODO  b = text_buffer( b, &g->ProductVersion, SAHPI_TL_TYPE_ASCII6, SAHPI_LANG_LATIN, 7, "rom" );
// TODO  b = text_buffer( b, &g->ModelNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_MALAY, 7, "1" );
// TODO  b = text_buffer( b, &g->SerialNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_NORWEGIAN, 7, "aaaa" );
// TODO  b = text_buffer( b, &g->PartNumber, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_FRENCH, 7, "1234567" );
// TODO  b = text_buffer( b, &g->FileId, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_HAUSA, 7, "1234567" );
// TODO  b = text_buffer( b, &g->AssetTag, SAHPI_TL_TYPE_BINARY, SAHPI_LANG_AYMARA, 7, "1234567" );
// TODO  g->CustomField[0] = 0;

  // 4 record: Oem
// TODO  d->DataRecords[4] = (SaHpiInventDataRecordT *)b;
// TODO  r = (SaHpiInventDataRecordT *)b;
// TODO  r->RecordType = SAHPI_INVENT_RECTYPE_OEM;
// TODO  r->DataLength = 12;
// TODO  SaHpiInventOemDataT *o = &r->RecordData.OemData;
// TODO  o->MId = 0x12345;
// TODO  o->Data[0] = 0x01;
// TODO  o->Data[1] = 0x02;
// TODO  o->Data[2] = 0x03;
// TODO  o->Data[3] = 0x04;
// TODO  o->Data[4] = 0x05;
// TODO  o->Data[5] = 0x06;
// TODO  o->Data[6] = 0x07;
// TODO  o->Data[7] = 0x08;

// TODO  d->DataRecords[5] = 0;

  unsigned char data[10240];

// TODO  int s1 = Marshal( &SaHpiInventoryDataType, buffer1, data );

  unsigned char buffer2[10240];

// TODO  int s2 = Demarshal( MarshalByteOrder(), &SaHpiInventoryDataType, buffer2, data );

// TODO  if ( s1 != s2 )
// TODO       return 1;

// TODO  if ( !cmp_inventory_data( (SaHpiInventoryDataT *)buffer1,
// TODO			    (SaHpiInventoryDataT *)buffer2 ) )
// TODO       return;

// TODO
  printf("completely non -functional\n");

  return 0;
}
