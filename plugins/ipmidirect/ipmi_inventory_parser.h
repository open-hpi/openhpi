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


enum tIpmiMultiRecordType
{
  tIpmiMultiRecordTypeBackplanePointToPointConnectivity = 0x04,
  //  tIpmiMultiRecordTypeAddressTable                      = 0x10,
  tIpmiMultiRecordTypeShelfPowerDistribution            = 0x11,
  tIpmiMultiRecordTypeShelfActivationAndPowerManagement = 0x12,
  //tIpmiMultiRecordTypeShelfManagerIpConnection          = 0x13,
  //tIpmiMultiRecordTypeBoardPointToPointConnectivity     = 0x14,
  //tIpmiMultiRecordTypeRadialIpmb0LinkMap                = 0x15,
};


class cIpmiInventoryMultiRecord 
{
  tIpmiMultiRecordType m_type;
  unsigned char        m_version;

public:
  cIpmiInventoryMultiRecord( tIpmiMultiRecordType type )
    : m_type( type ) {}
  virtual ~cIpmiInventoryMultiRecord() {}

  tIpmiMultiRecordType Type() { return m_type; }
  unsigned char &Version() { return m_version; }

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size ) = 0;
  virtual unsigned int IpmiSize() const = 0;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const = 0;

  virtual unsigned int HpiSize() const = 0;
  virtual void         HpiRead( SaHpiInventDataRecordT *r ) const = 0;
  virtual void         HpiWrite( SaHpiInventDataRecordT *r ) = 0;

  virtual void Dump( cIpmiLog &dump, const char *name ) const = 0;
};


class cIpmiInventoryPointToPointChannel
{
public:
  unsigned char m_local_channel;
  unsigned char m_remote_channel;
  unsigned char m_remote_slot;

  cIpmiInventoryPointToPointChannel()
    : m_local_channel( 0 ),
      m_remote_channel( 0 ),
      m_remote_slot( 0 )
  {}
};


enum tIpmiPointToPointChannelType
{
  eIpmiPointToPointChannelTypeSinglePortFabric  = 0x08,
  eIpmiPointToPointChannelTypeDoublePortFabric  = 0x09,
  eIpmiPointToPointChannelTypeFullChannelFabric = 0x0a,
  eIpmiPointToPointChannelTypeBase              = 0x0b,
  eIpmiPointToPointChannelTypeUpdateChannel     = 0x0c,
};


class cIpmiInventoryPointToPointSlot : public cArray<cIpmiInventoryPointToPointChannel>
{
public:
  tIpmiPointToPointChannelType m_type;
  unsigned char                m_slot_address;

  cIpmiInventoryPointToPointSlot()
    : m_type( eIpmiPointToPointChannelTypeSinglePortFabric ),
      m_slot_address( 0 )
  {}
};


class cIpmiInventoryBackplanePointToPointConnectivity : public cIpmiInventoryMultiRecord,
							public cArray<cIpmiInventoryPointToPointSlot>
{
public:
  cIpmiInventoryBackplanePointToPointConnectivity();
  virtual ~cIpmiInventoryBackplanePointToPointConnectivity();
  
  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize() const;
  virtual void         HpiRead( SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, const char *name ) const;
};


class cIpmiInventoryFeedToFru
{
public:
  unsigned char m_hardware_address;
  unsigned char m_fru_device_id;

  cIpmiInventoryFeedToFru()
    : m_hardware_address( 0 ),
      m_fru_device_id( 0 )
  {}
};


class cIpmiInventoryPowerMap : public cArray<cIpmiInventoryFeedToFru>
{
public:
  unsigned short m_maximum_external_current;
  unsigned short m_maximum_internal_current;
  unsigned char  m_minimum_expected_voltage;

  cIpmiInventoryPowerMap()
    : m_maximum_external_current( 0 ),
      m_maximum_internal_current( 0 ),
      m_minimum_expected_voltage( 0 )
  {}
};


class cIpmiInventoryShelfPowerDistribution : public cIpmiInventoryMultiRecord,
					     public cArray<cIpmiInventoryPowerMap>
{
public:
  cIpmiInventoryShelfPowerDistribution();
  virtual ~cIpmiInventoryShelfPowerDistribution();

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize() const;
  virtual void         HpiRead( SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, const char *name ) const;
};


class cIpmiInventoryActivationAndPower
{
public:
  unsigned char  m_hardware_address;
  unsigned char  m_fru_device_id;
  unsigned short m_maximum_fru_power;
  bool           m_activation; 
  unsigned char  m_delay;

  cIpmiInventoryActivationAndPower()
    : m_hardware_address( 0 ),
      m_fru_device_id( 0 ),
      m_maximum_fru_power( 0 ),
      m_activation( false ),
      m_delay( 0 )
    {}
};


class cIpmiInventoryShelfActivationAndPower : public cIpmiInventoryMultiRecord,
					     public cArray<cIpmiInventoryActivationAndPower>
{
public:
  cIpmiInventoryShelfActivationAndPower();
  virtual ~cIpmiInventoryShelfActivationAndPower();

  unsigned char m_allowance;

  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual unsigned int HpiSize() const;
  virtual void         HpiRead( SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( SaHpiInventDataRecordT *r );

  virtual void Dump( cIpmiLog &dump, const char *name ) const;
};


class cIpmiInventoryRecordMultiRecord : public cIpmiInventoryRecord,
					public cArray<cIpmiInventoryMultiRecord>
{
public:
  cIpmiInventoryRecordMultiRecord();
  virtual ~cIpmiInventoryRecordMultiRecord();

  cIpmiInventoryMultiRecord *GetRecord( tIpmiMultiRecordType type ) const;
  void                  SetRecord( tIpmiMultiRecordType type, cIpmiInventoryMultiRecord *record );

  static cIpmiInventoryMultiRecord *AllocRecord( tIpmiMultiRecordType type );

  virtual int          IpmiNumRecords() const { return Num(); }
  virtual SaErrorT     IpmiRead( const unsigned char *data, unsigned int size );
  virtual unsigned int IpmiSize() const;
  virtual unsigned int IpmiWrite( unsigned char *data, unsigned int size ) const;

  virtual int          HpiNumRecords() const { return 0; /*Num();*/ }
  virtual unsigned int HpiSize( int record_id ) const;
  virtual void         HpiRead( int record_id, SaHpiInventDataRecordT *r ) const;
  virtual void         HpiWrite( int record_id, SaHpiInventDataRecordT *r );

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
