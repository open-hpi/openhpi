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

#ifndef dIpmiInventoryParser_h
#define dIpmiInventoryParser_h


#ifndef dArray_h
#include "array.h"
#endif

#ifndef dIpmiTextBuffer
#include "ipmi_text_buffer.h"
#endif


unsigned char IpmiChecksum( const unsigned char *data, int size );


enum tIpmiInventoryRecordType
{
  eIpmiInventoryRecordTypeInternal,
  eIpmiInventoryRecordTypeChassis,
  eIpmiInventoryRecordTypeBoard,
  eIpmiInventoryRecordTypeProduct,
  eIpmiInventoryRecordTypeMultiRecord,
  eIpmiInventoryRecordTypeLast
};

const char *IpmiInventoryRecordTypeToString( tIpmiInventoryRecordType );


class cIpmiInventoryRecord
{
protected:
  tIpmiInventoryRecordType m_type;

public:
  cIpmiInventoryRecord( tIpmiInventoryRecordType type )
    : m_type( type ) {}
  virtual ~cIpmiInventoryRecord() {}

  tIpmiInventoryRecordType Type() const
  {
    return m_type;
  }

  virtual int          IpmiNumRecords() const = 0;
  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size ) = 0;
  virtual unsigned int IpmiSize() const = 0;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const = 0;

  virtual int          HpiNumRecords() const = 0;
  virtual unsigned int HpiSize( int record_id ) const = 0;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const = 0;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r ) = 0;

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const = 0;
};


class cIpmiInventoryRecordCommon : public cIpmiInventoryRecord
{
public:
  unsigned char m_version;

public:
  cIpmiInventoryRecordCommon( tIpmiInventoryRecordType type )
    : cIpmiInventoryRecord( type ), m_version( 0 ) {}
  virtual ~cIpmiInventoryRecordCommon() {}

  unsigned char Version() const 
  {
    return m_version;
  }

  virtual int IpmiNumRecords() const { return 1; }

  virtual SaErrorT IpmiRead( const unsigned char *data, unsigned int size )
  {
    if ( size < 1 )
	 return SA_ERR_HPI_DATA_LEN_INVALID;

    m_version = data[0] & 0x0f;

    return SA_OK;
  }

  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const
  {
    assert( 0 );
    return 0;
  }
  
  virtual int HpiNumRecords() const { return 1; }
};


class cIpmiInventoryRecordInternal : public cIpmiInventoryRecordCommon
{
protected:
  unsigned char *m_data;
  unsigned int   m_size;

public:
  cIpmiInventoryRecordInternal();
  virtual ~cIpmiInventoryRecordInternal();

  void                 SetData( const unsigned char *data, unsigned int size );
  const unsigned char *GetData( unsigned int &size ) const;

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize( int record_id ) const;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const;
};


class cIpmiInventoryRecordChassis : public cIpmiInventoryRecordCommon,
				           cArray<cIpmiTextBuffer>
{
public:
  SaHpiInventChassisTypeT m_chassis_type;
  cIpmiTextBuffer  m_chassis_part_number;
  cIpmiTextBuffer  m_chassis_serial_number;

public:
  cIpmiInventoryRecordChassis();
  virtual ~cIpmiInventoryRecordChassis();

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize( int record_id ) const;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const;
};


class cIpmiInventoryRecordBoard : public cIpmiInventoryRecordCommon,
				         cArray<cIpmiTextBuffer>
{
public:
  SaHpiLanguageT  m_language;
  SaHpiTimeT      m_mfg_time;
  cIpmiTextBuffer m_board_manufacturer;
  cIpmiTextBuffer m_board_product_name;
  cIpmiTextBuffer m_board_serial_number;
  cIpmiTextBuffer m_board_part_number;
  cIpmiTextBuffer m_fru_file_id;

public:
  cIpmiInventoryRecordBoard();
  virtual ~cIpmiInventoryRecordBoard();

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize( int record_id ) const;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const;
};


class cIpmiInventoryRecordProduct : public cIpmiInventoryRecordCommon,
				           cArray<cIpmiTextBuffer>
{
public:
  SaHpiLanguageT  m_language;
  cIpmiTextBuffer m_manufacturer_name;
  cIpmiTextBuffer m_product_name;
  cIpmiTextBuffer m_product_part;
  cIpmiTextBuffer m_product_version;
  cIpmiTextBuffer m_product_serial_number;
  cIpmiTextBuffer m_asset_tag;
  cIpmiTextBuffer m_fru_file_id;

public:
  cIpmiInventoryRecordProduct();
  virtual ~cIpmiInventoryRecordProduct();

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize( int record_id ) const;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const;
};


class cIpmiInventoryRecordMultiRecord : public cIpmiInventoryRecord
{
public:
  cIpmiInventoryRecordMultiRecord();
  virtual ~cIpmiInventoryRecordMultiRecord();

  virtual int          IpmiNumRecords() const { return 0; }
  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual int          HpiNumRecords() const { return 0; }
  virtual unsigned int HpiSize( int record_id ) const { return 0; }
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT * /*r*/ ) const { assert( 0 ); }
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT * /*r*/ ) { assert( 0 ); }

  virtual void Dump( cIpmiLog &dump, int record_id, const char *name ) const;
};


class cIpmiInventoryParser
{
  unsigned char         m_version;
  cIpmiInventoryRecord *m_records[eIpmiInventoryRecordTypeLast];

public:
  cIpmiInventoryParser();
  virtual ~cIpmiInventoryParser();

  unsigned char         Version() const { return m_version; }

  cIpmiInventoryRecord *GetRecord( tIpmiInventoryRecordType type ) const;
  void                  SetRecord( tIpmiInventoryRecordType type, cIpmiInventoryRecord *record );

  void Clear();
  static cIpmiInventoryRecord *AllocRecord( tIpmiInventoryRecordType type );


  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize() const;
  virtual SaErrorT     HpiRead( SaHpiInventoryDataT &data ) const;
  virtual SaErrorT     HpiWrite( const SaHpiInventoryDataT &data );
};


#endif
