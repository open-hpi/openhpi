/*
 * ipmi_control_fan.h
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */


#ifndef dIpmiControlFan_h
#define dIpmiControlFan_h

#ifndef dIpmiControl_h
#include "ipmi_control.h"
#endif


class cIpmiControlFan : public cIpmiControl
{
  unsigned int m_minimum_speed_level;
  unsigned int m_maximum_speed_level;
  unsigned int m_default_speed_level;

  // support for automatic speed adjustment
  bool m_local_control_mode;

public:
  cIpmiControlFan( cIpmiMc *mc,
                   unsigned int num,
                   unsigned int minium_speed_level,
                   unsigned int maximum_speed_level,
                   unsigned int default_speed_level,
                   bool local_control_mode );
  virtual ~cIpmiControlFan();

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // virtual void Log();

  virtual SaErrorT SetState( const SaHpiCtrlStateT &state );
  virtual SaErrorT GetState( SaHpiCtrlStateT &state );  
};


#endif
