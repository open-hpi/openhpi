/*
 * ipmi_mc_vendor.h
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

#ifndef dIpmiMcVendor_h
#define dIpmiMcVendor_h


#ifndef dIpmiSensorHotswap_h
#include "ipmi_sensor_hotswap.h"
#endif

#ifndef dIpmiSensorThreshold_h
#include "ipmi_sensor_threshold.h"
#endif

#ifndef dIpmiMsg_h
#include "ipmi_msg.h"
#endif

class cIpmiMc;


class cIpmiMcVendor
{
public:
  unsigned int m_manufacturer_id;
  unsigned int m_product_id;
  char         m_description[80];

  cIpmiMcVendor( unsigned int manufacturer_id,
                 unsigned int product_id,
                 const char *desc );

  virtual ~cIpmiMcVendor();

  // a new MC is found
  virtual bool Init( cIpmiMc *mc, const cIpmiMsg &devid );

  // cleanup code for an MC
  virtual void Cleanup( cIpmiMc *mc );

  // create sensors
  virtual bool CreateSensors( cIpmiMc *mc, cIpmiSdrs *sdrs );

protected:
  virtual GList *CreateSensorFromFullSensorRecord( cIpmiMc *source_mc, cIpmiSdr *sdr );
  virtual GList *ConvertToFullSensorRecords( cIpmiMc *source_mc, cIpmiSdr *sdr );
  virtual cIpmiSensor **GetSensorsFromSdrs( cIpmiMc *source_mc,
                                            cIpmiSdrs    *sdrs,
                                            unsigned int &sensor_count );

  virtual GList *CreateSensorHotswap( cIpmiMc *mc, cIpmiSdr *sdr );
  virtual GList *CreateSensorThreshold( cIpmiMc *mc, cIpmiSdr *sdr );
  virtual GList *CreateSensorDiscrete( cIpmiMc *mc, cIpmiSdr *sdr );
  virtual GList *CreateSensorDefault( cIpmiMc *mc, cIpmiSdr *sdr );

public:
  // create controls
  virtual bool CreateControls( cIpmiMc *mc, cIpmiSdrs *sdrs );

protected:
  virtual bool CreateControlsAtca( cIpmiMc *mc, cIpmiSdrs *sdrs,
                                   unsigned int mc_type );
  virtual bool CreateControlAtcaFan( cIpmiMc *mc, cIpmiSdrs *sdrs,
                                     cIpmiEntity *ent );
  
};


class cIpmiMcVendorFactory
{
  static cIpmiMcVendorFactory *m_factory;

public:
  cIpmiMcVendorFactory();
  ~cIpmiMcVendorFactory();

  static void InitFactory();
  static void CleanupFactory();
  static cIpmiMcVendorFactory *GetFactory() { return m_factory; }

protected:
  // list of all vendor MCs
  GList *m_mc_vendors;

  // default mc vendor
  cIpmiMcVendor *m_default;

public:
  // register a new cIpmiMcVendor
  bool Register( cIpmiMcVendor *mv );

  // unregister cIpmiMcVendor
  bool Unregister( unsigned int manufacturer_id,
                   unsigned int product_id );

  // find a cIpmiMcVendor to a give manufacturer id and product id
  cIpmiMcVendor *Find( unsigned int manufacturer_id,
                       unsigned int product_id );

  // returns the default if not found
  cIpmiMcVendor *Get( unsigned int manufacturer_id,
                      unsigned int product_id );

  cIpmiMcVendor *Default()
  {
    return m_default;
  }
};


#endif
