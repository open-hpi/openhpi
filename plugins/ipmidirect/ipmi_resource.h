/*
 * ipmi_resource.h
 *
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

#ifndef dIpmiResource_h
#define dIpmiResource_h


#ifndef dIpmiRdr_h
#include "ipmi_rdr.h"
#endif

#ifndef dIpmiSensor_h
#include "ipmi_sensor.h"
#endif

#ifndef dIpmiControl_h
#include "ipmi_control.h"
#endif


class cIpmiResource : public cIpmiRdrContainer
{
protected:
  cIpmiMc            *m_mc;
  unsigned int        m_fru_id;
  cIpmiEntityPath     m_entity_path;

  cIpmiSensorHotswap *m_hotswap_sensor;

  // state only to create state change Mx -> M0
  // where Mx is m_fru_state
  tIpmiFruState       m_fru_state;

  unsigned int        m_oem;

public:
  cIpmiMc *Mc() const { return m_mc; }
  unsigned int FruId() const { return m_fru_id; }
  tIpmiFruState &FruState() { return m_fru_state; }
  cIpmiDomain *Domain() const { return m_mc->Domain(); }
  unsigned int &Oem() { return m_oem; }
  cIpmiEntityPath &EntityPath() { return m_entity_path; }

protected:
  cIpmiTextBuffer m_resource_tag;

public:
  cIpmiTextBuffer &ResourceTag() { return m_resource_tag; }

public:
  cIpmiResource( cIpmiMc *mc, unsigned int fru_id );
  virtual ~cIpmiResource();

public:
  // return hotswap sensor if there is one
  cIpmiSensorHotswap *GetHotswapSensor() { return m_hotswap_sensor; }

  // add cIpmiRdr and create an Hpi Rdr.
  virtual bool Add( cIpmiRdr *rdr );

  // remove cIpmiRdr
  virtual bool Rem( cIpmiRdr *rdr );

protected:
  unsigned int m_current_control_id;

public:
  // get a unique control num for this resource
  unsigned int GetControlNum()
  {
    return ++m_current_control_id;
  }

  // HPI resource id
  SaHpiResourceIdT m_resource_id;

  virtual bool Create( SaHpiRptEntryT &entry );
  virtual bool Destroy();

  int SendCommand( const cIpmiMsg &msg, cIpmiMsg &rsp,
		   unsigned int lun = 0, int retries = dIpmiDefaultRetries )
  {
    return m_mc->SendCommand( msg, rsp, lun, retries );
  }
};


#endif
