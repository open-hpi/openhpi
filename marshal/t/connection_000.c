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

  char data[256] = "bla";
  int len = strlen( data ) + 1;

  rv = write( cc->m_fd, data, len );

  if ( rv != len )
       goto fail4;

  char buffer[256];

  rv = read( sc->m_fd, buffer, len );

  if ( rv != len )
       goto fail4;

  if ( memcmp( data, buffer, len ) )
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
