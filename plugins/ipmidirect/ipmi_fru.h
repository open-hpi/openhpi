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

#ifndef dIpmiFru_h
#define dIpmiFru_h


#ifndef dIpmiRdr_h
#include "ipmi_rdr.h"
#endif

#ifndef dIpmiTextBuffer_h
#include "ipmi_text_buffer.h"
#endif


enum tIpmiFruState
{
  eIpmiFruStateNotInstalled           = 0,
  eIpmiFruStateInactive               = 1,
  eIpmiFruStateActivationRequest      = 2,
  eIpmiFruStateActivationInProgress   = 3,
  eIpmiFruStateActive                 = 4,
  eIpmiFruStateDeactivationRequest    = 5,
  eIpmiFruStateDeactivationInProgress = 6,
  eIpmiFruStateCommunicationLost      = 7
};

const char *IpmiFruStateToString( tIpmiFruState state );


class cIpmiEntity;

enum tIpmiChassisType
{
  eIpmiChassisTypeOther = 1
};


enum tIpmiFruItemType
{
  eIpmiFruItemTypeUnknown,
  eIpmiFruItemTypeData,
  eIpmiFruItemTypeTextBuffer,
  eIpmiFruItemTypeInt,
  eIpmiFruItemTypeRecord,

  // cmds
  eIpmiFruItemTypeDate3,
  eIpmiFruItemTypeCustom,
  eIpmiFruItemTypePad,
};


// item names
#define dIpmiFruItemAssetTag       "AssetTag"
#define dIpmiFruItemChassisType    "ChassisType"
#define dIpmiFruItemCustom         "Custom"
#define dIpmiFruItemData           "Data"
#define dIpmiFruItemFruFileId      "FruFileId"
#define dIpmiFruItemLanguage       "Language"
#define dIpmiFruItemManufacturer   "Manufacturer"
#define dIpmiFruItemMfgDate        "MfgDate"
#define dIpmiFruItemPartNumber     "PartNumber"
#define dIpmiFruItemProductName    "ProductName"
#define dIpmiFruItemProductVersion "ProductVersion"
#define dIpmiFruItemSerialNumber   "SerialNumber"
#define dIpmiFruItemVersion        "Version"


struct tIpmiFruItemDesc
{
  const char       *m_name;
  tIpmiFruItemType  m_type;
};


#define dIpmiFruItemNameLen 32

class cIpmiFruRecord;


class cIpmiFruItem
{
public:
  char             m_name[dIpmiFruItemNameLen];
  tIpmiFruItemType m_type;

  cIpmiFruItem( const char *name, tIpmiFruItemType type );
  ~cIpmiFruItem();

  union
  {
    struct
    {
      unsigned char *m_data;
      unsigned int   m_size;
    } m_data;

    unsigned int     m_int;
    SaHpiTextBufferT m_text_buffer;
    cIpmiFruRecord  *m_record;
  } m_u;

  void Dump( cIpmiLog &dump ) const;
};


// record names
#define dIpmiFruRecordInternalUseArea "InternalUseArea"
#define dIpmiFruRecordChassisInfoArea "ChassisInfoArea"
#define dIpmiFruRecordBoardInfoArea   "BoardInfoArea"
#define dIpmiFruRecordProductInfoArea "ProductInfoArea"
#define dIpmiFruRecordMultiRecord     "MultiRecord"


#define dIpmiFruRecordNameLen 64

class cIpmiFruRecord
{
public:
  char           m_name[dIpmiFruRecordNameLen];
  cIpmiFruItem **m_array;
  int            m_num;

  cIpmiFruRecord( const char *name );
  ~cIpmiFruRecord();

  void Add( cIpmiFruItem *item );
  void Clear();

  cIpmiFruItem *Find( const char *name );

  // creating of HPI inventory data
  SaHpiTextBufferT *SetItem( unsigned char *&p, int &s, const char *name );
  int GeneralDataRecord( SaHpiInventGeneralDataT *r );
  int InternalUseRecord( SaHpiInventInternalUseDataT *r );
  int ChassisInfoAreaRecord( SaHpiInventChassisDataT *r );
  int BoradInfoAreaRecord( SaHpiInventGeneralDataT *r );
  int ProductInfoAreaRecord( SaHpiInventGeneralDataT *r );

  void Dump( cIpmiLog &dump, const char *name ) const;
};


enum tFruAccessMode
{
  eFruAccessModeByte = 0,
  eFruAccessModeWord = 1
};


#define dMaxFruFetchBytes 20


class cIpmiFru : public cIpmiRdr
{
protected:
  unsigned char  m_fru_device_id; // fru device id
  tFruAccessMode m_access;
  unsigned int   m_size;
  bool           m_fetched;

  int GetFruInventoryAreaInfo( unsigned int &size, tFruAccessMode &byte_access );
  int ReadFruData( unsigned short offset, unsigned int num, unsigned int &n, unsigned char *data );

  int CreateRecord( const char *name, tIpmiFruItemDesc *desc, 
                    const unsigned char *data, unsigned int len );

  int CreateInventory( const unsigned char *data );
  int CreateInternalUse( const unsigned char *data, unsigned int len );
  int CreateChassis( const unsigned char *data, unsigned int len );
  int CreateBoard( const unsigned char *data, unsigned int len );
  int CreateProduct( const unsigned char *data, unsigned int len );
  int CreateMultiRecord( const unsigned char *data, unsigned int len );

  cIpmiFruRecord **m_array;
  int              m_num;

  unsigned int     m_oem;

  void Add( cIpmiFruRecord *record );
  void Clear();

public:
  cIpmiFru( cIpmiMc *mc, unsigned int fru_device_id );
  ~cIpmiFru();

  int Fetch();

  virtual unsigned int Num() const { return m_fru_device_id; }
  unsigned int &Oem() { return m_oem; }

  cIpmiFruRecord *Find( const char *name );

  int NumRecords() const { return m_num; }
  cIpmiFruRecord *GetRecord( int idx ) const
  {
    assert( idx >= 0 && idx < m_num );
    return m_array[idx];
  }

  // create an RDR inventory record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

protected:
  unsigned int m_inventory_size;

public:
  // calulate the size of inventory data
  unsigned int CalcSize();
  unsigned int GetInventorySize() const { return m_inventory_size; }

  unsigned int GetInventoryInfo( SaHpiInventoryDataT &data ) const;

  void Dump( cIpmiLog &dump, const char *name ) const;
};


#endif
