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


static int
cmp_header( cMessageHeader *h1, cMessageHeader *h2 )
{
  if ( h1->m_type != h2->m_type )
       return 1;

  if ( h1->m_flags != h2->m_flags )
       return 1;

  if ( h1->m_seq != h2->m_seq )
       return 1;

  if ( h1->m_seq_in != h2->m_seq_in )
       return 1;

  if ( h1-> m_id != h2-> m_id )
       return 1;

  if ( h1->m_len != h2->m_len )
       return 1;

  return 0;
}


int
main( int argc, char *argv[] )
{
  int rt = 1;

  cServerSocket *mc = ServerOpen( dTestPort );

  if ( mc == 0 )
       goto fail1;

  cClientConnection *cc = ClientOpen( "localhost", dTestPort );

  if ( cc == 0 )
       goto fail2;

  // send a ping
  cMessageHeader request;
  MessageHeaderInit( &request, eMhPing, dMhRequest, 0, 0, 0 );

  if ( ClientWriteMsg( cc, &request, 0 ) )
       goto fail3;

  // read ping
  cMessageHeader req;
  unsigned char  req_data[256];
  cServerConnection *sc = 0;
  tConnectionError mr = ServerReadMsg( mc, &sc, &req, req_data );

  if ( mr != eConnectionNew )
       goto fail3;

  if ( cmp_header( &request, &req ) )
       goto fail3;

  // send pong back
  MessageHeaderInit( &request, eMhPing, dMhReply, request.m_seq, 0, 0 );

  if ( ServerWriteMsg( sc, &request, 0 ) )
       goto fail3;

  // read pong
  if ( ClientReadMsg( cc, &req, req_data ) != eConnectionOk )
       goto fail3;

  if ( cmp_header( &request, &req ) )
       goto fail3;

  rt = 0;

fail3:
  ClientClose( cc );

fail2:
  ServerClose( mc );

fail1:
  return rt;
}
