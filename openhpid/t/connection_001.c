/*
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

#include "connection.h"
#include "marshal.h"
#include <unistd.h>
#include <string.h>


#define dTestPort 4718


int
main( int argc, char *argv[] )
{
  int rt = 1;

  cServerConnectionMain *mc = ServerConnectionMainOpen( dTestPort );

  if ( mc == 0 )
       goto fail1;

  cClientConnection *cc = ClientConnectionOpen( "localhost", dTestPort );

  if ( cc == 0 )
       goto fail2;

  int rv = ConnectionWait( mc->m_fd, 1000 );

  if ( rv )
       goto fail2;

  cServerConnection *sc = ServerConnectionMainAccept( mc );

  if ( sc == 0 )
       goto fail3;

  cMessageHeader h;
  MessageHeaderInit( &h, eMhRequest, 47, 1234, 98 );
  rv = ConnectionWriteHeader( cc->m_fd, &h );

  if ( rv )
       goto fail4;

  cMessageHeader header;
  rv = ConnectionReadHeader( sc->m_fd, &header );

  if ( rv )
       goto fail4;

  if (    header.m_version != dMhVersion
       || header.m_type    != eMhRequest
       || header.m_flags   != MarshalByteOrder()
       || header.m_seq     != 47
       || header.m_id      != 1234
       || header.m_len     != 98 )
       goto fail4;

  rt = 0;

fail4:
  ServerConnectionClose( sc );

fail3:
  ClientConnectionClose( cc );

fail2:
  ServerConnectionMainClose( mc );

fail1:
  return rt;
}
