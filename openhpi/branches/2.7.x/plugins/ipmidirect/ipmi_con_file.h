/*
 * ipmi_con_file.h
 *
 * Interface for IPMI file connection
 *
 * Copyright (c) 2003 by FORCE Computers.
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

#ifndef dIpmiConFile_h
#define dIpmiConFile_h


#include <stdio.h>

#ifndef dIpmiCon_h
#include "ipmi_con.h"
#endif


#define dIpmiConFileDefault "ipmi.con"


class cIpmiConFileMsg
{
public:
  int                  m_seq;

  tIpmiConFileDataType m_type;
  cIpmiAddr            m_addr;
  cIpmiMsg             m_msg;
};


class cIpmiConFile : public cIpmiCon
{
protected:
  char  m_filename[1024];
  FILE *m_fd;

  enum tIpmiConFileDataType
  {
    eDtError,
    eDtEOF,
    eDtCommand,
    eDtResponse,
    eDtTimeout,
    eDtEvent
  };

  // read ahead
  tIpmiConFileDataType m_type;
  cIpmiAddr            m_addr;
  cIpmiMsg             m_msg;

  int ReadHex( unsigned char **pp, unsigned char *v );
  unsigned char *ReadLine( unsigned char *line, int size );
  void ReadLine();

public:
  cIpmiConFile( unsigned int timeout, unsigned int atca_timeout, const char *filename );
  virtual ~cIpmiConFile();

  virtual int  IfOpen();
  virtual void IfClose();
  virtual int  IfCmd( const cIpmiAddr &addr, const cIpmiMsg &msg,
                      cIpmiAddr &rsp_addr, cIpmiMsg &rsp_msg );
  virtual void IfEvent();
};


#endif
