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


#include <assert.h>


__BEGIN_DECLS
#include "SaHpi.h"
__END_DECLS

class cIpmiMc;
class cIpmiEntity;


#define dControlIdLen 32


class cIpmiControl
{
protected:
  cIpmiMc     *m_mc;
  cIpmiEntity *m_entity;

  unsigned int m_num;
  bool         m_ignore;
  unsigned int m_oem;

  SaHpiCtrlOutputTypeT m_output_type;
  SaHpiCtrlTypeT       m_type;

  char         m_id[dControlIdLen+1];

public:
  // HPI record id to find the rdr
  SaHpiEntryIdT m_record_id;

public:
  cIpmiControl( cIpmiMc *mc, cIpmiEntity *ent,
                unsigned int num, const char *id,
                SaHpiCtrlOutputTypeT output_type, SaHpiCtrlTypeT type );
  virtual ~cIpmiControl();

  unsigned char Num() const { return m_num; }
  const char *  Id() const { return m_id; }

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  virtual void Log();

  // hpi
  virtual SaErrorT SetState( const SaHpiCtrlStateT &state ) = 0;
  virtual SaErrorT GetState( SaHpiCtrlStateT &state ) = 0;
};


#endif
