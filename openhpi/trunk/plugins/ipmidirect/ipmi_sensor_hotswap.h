/*
 * ipmi_sensor_hotswap.h
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

#ifndef dIpmiSensorHotswap_h
#define dIpmiSensorHotswap_h


#ifndef dIpmiSensorDiscrete_h
#include "ipmi_sensor_discrete.h"
#endif

#ifndef dIpmiFru_h
#include "ipmi_fru.h"
#endif


class cIpmiSensorHotswap : public cIpmiSensorDiscrete
{
public:
  static SaHpiHsStateT ConvertIpmiToHpiHotswapState( tIpmiFruState h );

public:
  cIpmiSensorHotswap( cIpmiMc *mc );
  virtual ~cIpmiSensorHotswap();

  // create hpi event from ipmi event
  SaErrorT CreateEvent( cIpmiEvent *event, SaHpiEventT &h );

  // read sensor parameter from Full Sensor Record
  virtual bool GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr );

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // read hotswap state
  SaErrorT GetState( tIpmiFruState &state );
  SaErrorT GetState( SaHpiHsStateT &state );
};


#endif
