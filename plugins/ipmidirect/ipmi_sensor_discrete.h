/*
 * ipmi_sensor_discrete.h
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

#ifndef dIpmiSensorDiscrete_h
#define dIpmiSensorDiscrete_h


#ifndef dIpmiSensor_h
#include "ipmi_sensor.h"
#endif


class cIpmiSensorDiscrete : public cIpmiSensor
{
protected:
  unsigned int m_assertion_event_mask;
  unsigned int m_deassertion_event_mask;
  unsigned int m_event_mask;

public:
  cIpmiSensorDiscrete( cIpmiMc *mc );
  virtual ~cIpmiSensorDiscrete();

  // create an hpi event from ipmi event
  virtual SaErrorT CreateEvent( cIpmiEvent *event, SaHpiEventT &h );

  // read sensor parameter from Full Sensor Record
  virtual bool GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr );

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // get sensor data
  virtual SaErrorT GetData( SaHpiSensorReadingT &data );

  virtual SaErrorT GetEventEnables( SaHpiSensorEvtEnablesT &enables );
  virtual SaErrorT SetEventEnables( const SaHpiSensorEvtEnablesT &enables );
};


#endif
