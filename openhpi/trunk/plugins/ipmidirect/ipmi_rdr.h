/*
 * ipmi_rdr.h basecalss for cIpmiSensor, cIpmiControl
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

#ifndef dIpmiRdr_h
#define dIpmiRdr_h


#ifndef dIpmiTextBuffer_h
#include "ipmi_text_buffer.h"
#endif

#include <assert.h>
#include <glib.h>

__BEGIN_DECLS
#include "SaHpi.h"
__END_DECLS


class cIpmiMc;
class cIpmiEntity;


class cIpmiRdr
{
protected:
  cIpmiMc        *m_mc;
  cIpmiEntity    *m_entity;
  SaHpiEntryIdT   m_record_id;
  SaHpiRdrTypeT   m_type;
  cIpmiTextBuffer m_id_string;
  unsigned int    m_lun;

public:
  cIpmiRdr( cIpmiMc *mc, SaHpiRdrTypeT type );
  virtual ~cIpmiRdr();

  cIpmiMc         *Mc() const { return m_mc; }
  cIpmiEntity    *&Entity()   { return m_entity; }
  SaHpiEntryIdT   &RecordId() { return m_record_id; }
  SaHpiRdrTypeT    Type() const { return m_type; }
  cIpmiTextBuffer &IdString() { return m_id_string; }
  const cIpmiTextBuffer &IdString() const { return m_id_string; }

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // sensor num, control num, fru device id
  virtual unsigned int Num() const = 0;
  virtual unsigned int Lun() const { return m_lun; }

  //virtual void Dump( cIpmiLog &dump ) = 0;
};


class cIpmiRdrContainer
{
protected:
  GList *m_rdrs;

public:
  cIpmiRdrContainer();
  virtual ~cIpmiRdrContainer();

  // find a specific rdr
  virtual cIpmiRdr *Find( cIpmiMc *mc, SaHpiRdrTypeT type, unsigned int num, unsigned int lun = 0 );
  virtual GList *GetRdrList( cIpmiMc *mc, SaHpiRdrTypeT type );

  virtual bool Find( cIpmiRdr *rdr );
  virtual bool Add( cIpmiRdr *rdr );
  virtual bool Rem( cIpmiRdr *rdr );
};


#endif
