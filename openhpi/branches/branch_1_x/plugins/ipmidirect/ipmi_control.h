/*
 * ipmi_control.h
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

#ifndef dIpmiControl_h
#define dIpmiControl_h


#ifndef dIpmiRdr_h
#include "ipmi_rdr.h"
#endif

#include <assert.h>

extern "C" {
#include "SaHpi.h"
}

class cIpmiControl : public cIpmiRdr
{
protected:
  unsigned int         m_num; // control num
  bool                 m_ignore;
  unsigned int         m_oem;

  SaHpiCtrlOutputTypeT m_output_type;
  SaHpiCtrlTypeT       m_type;

public:
  cIpmiControl( cIpmiMc *mc,
                unsigned int num,
                SaHpiCtrlOutputTypeT output_type,
		SaHpiCtrlTypeT type );
  virtual ~cIpmiControl();

  virtual unsigned int Num() const { return m_num; }

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // hpi
  virtual SaErrorT SetState( const SaHpiCtrlStateT &state ) = 0;
  virtual SaErrorT GetState( SaHpiCtrlStateT &state ) = 0;

  virtual void Dump( cIpmiLog &dump, const char *name ) const = 0;
};


#endif
