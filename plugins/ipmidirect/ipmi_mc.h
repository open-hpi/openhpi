/*
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

#ifndef dIpmiMc_h
#define dIpmiMc_h


#include <glib.h>
#include <sys/time.h>


#ifndef dIpmiSdr_h
#include "ipmi_sdr.h"
#endif

#ifndef dIpmiAddr_h
#include "ipmi_addr.h"
#endif

#ifndef dIpmiMsg_h
#include "ipmi_msg.h"
#endif

#ifndef dIpmiSensorHotswap_h
#include "ipmi_sensor_hotswap.h"
#endif

#ifndef dIpmiSel_h
#include "ipmi_sel.h"
#endif

#ifndef dIpmiCon_h
#include "ipmi_con.h"
#endif

#ifndef dIpmiMcVendor_h
#include "ipmi_mc_vendor.h"
#endif

#ifndef dIpmiFru_h
#include "ipmi_fru.h"
#endif

class cIpmiDomain;
class cIpmiMcThread;


class cIpmiMc : public cIpmiRdrContainer
{
protected:
  cIpmiMcVendor *m_vendor;

  cIpmiAddr      m_addr;

  // If the MC is known to be good in the system, then active is
  // true.  If active is false, that means that there are sensors
  // that refer to this MC, but the MC is not currently in the
  // system.
  bool           m_active;

  // state only to create state change Mx -> M0
  // where Mx is m_fru_state
  tIpmiFruState  m_fru_state;

  cIpmiDomain   *m_domain;

  cIpmiSdrs     *m_sdrs;

  // The sensors that came from the device SDR on this MC.
  GList         *m_sensors_in_my_sdr;

  // The system event log, for querying and storing events.
  cIpmiSel      *m_sel;

  // The rest is the actual data from the get device id and SDRs.
  unsigned char  m_device_id;

  unsigned char  m_device_revision;

  bool           m_provides_device_sdrs;
  bool           m_device_available;

  bool           m_chassis_support;
  bool           m_bridge_support;
  bool           m_ipmb_event_generator_support;
  bool           m_ipmb_event_receiver_support;
  bool           m_fru_inventory_support;
  bool           m_sel_device_support;
  bool           m_sdr_repository_support;
  bool           m_sensor_device_support;

  unsigned char  m_major_fw_revision;
  unsigned char  m_minor_fw_revision;

  unsigned char  m_major_version;
  unsigned char  m_minor_version;

  unsigned int   m_manufacturer_id;
  unsigned short m_product_id;

  unsigned char  m_aux_fw_revision[4];

  int SendSetEventRcvr( unsigned int addr );

  GList *m_resources;

public:
  void AddResource( cIpmiResource *res );
  void RemResource( cIpmiResource *res );
  cIpmiResource *FindResource( unsigned int fru_id );
  cIpmiResource *FindResource( cIpmiResource *res );

public:
  cIpmiMc( cIpmiDomain *domain, const cIpmiAddr &addr );
  ~cIpmiMc();

  cIpmiDomain *Domain() const { return m_domain; }

  unsigned char MajorFwRevision() const { return m_major_fw_revision; }
  unsigned char MinorFwRevision() const { return m_minor_fw_revision; }

  unsigned char MajorVersion()    const { return m_major_version; }
  unsigned char MinorVersion()    const { return m_minor_version; }

  unsigned int  ManufacturerId()  const { return m_manufacturer_id; }
  unsigned short ProductId()      const { return m_product_id; }

  unsigned char AuxFwRevision( int v ) const
  {
    assert( v >= 0 && v < 4 );
    return m_aux_fw_revision[v];
  }

  tIpmiFruState &FruState() { return m_fru_state; }

  const cIpmiAddr &Addr() { return m_addr; }

  bool IsActive() const { return m_active; }
  void SetActive( bool active ) { m_active = active; }

  cIpmiSensorHotswap *FindHotswapSensor();

  GList *GetSdrSensors() const
  {
    return m_sensors_in_my_sdr;
  }

  void SetSdrSensors( GList *sensors )
  {
    m_sensors_in_my_sdr = sensors;
  }

  bool ProvidesDeviceSdrs() const { return m_provides_device_sdrs; }

  bool &SdrRepositorySupport() { return m_sdr_repository_support; }
  bool SelDeviceSupport() const { return m_sel_device_support; }

  cIpmiSel *Sel() const { return m_sel; }

  bool Cleanup(); // true => it is safe to destroy mc

  int      HandleNew();
  bool     DeviceDataCompares( const cIpmiMsg &msg ) const;
  int      GetDeviceIdDataFromRsp( const cIpmiMsg &msg );
  void     CheckEventRcvr();
  int      SendCommand( const cIpmiMsg &msg, cIpmiMsg &rsp_msg,
                        unsigned int lun = 0, int retries = dIpmiDefaultRetries );

  unsigned int GetChannel() const;
  unsigned int GetAddress() const;
  void         SetActive();
  cIpmiSensor *FindSensor( unsigned int lun, unsigned int sensor_id );

  int AtcaPowerFru( int fru_id );

  void SetVendor( cIpmiMcVendor *mv )
  {
    assert( mv );
    m_vendor = mv;
  }

  cIpmiMcVendor *GetVendor()  { return m_vendor; }
protected:
  bool DumpFrus( cIpmiLog &dump, const char *name ) const;
  bool DumpControls( cIpmiLog &dump, const char *name ) const;

public:
  void Dump( cIpmiLog &dump, const char *name ) const;
};


#endif
