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

  cServerConnection *sc = 0;

  // send messages
  int i;

  for( i = 0; i < 1000; i++ )
     {
       cMessageHeader request;
       MessageHeaderInit( &request, eMhMsg, dMhRequest, 0, i, 0 );

       if ( ClientWriteMsg( cc, &request, 0 ) )
            goto fail3;

       // read message
       cMessageHeader req;
       unsigned char  req_data[256];
       tConnectionError mr = ServerReadMsg( mc, &sc, &req, req_data );

       if ( i == 0 )
          {
            if ( mr != eConnectionNew )
                 goto fail3;
          }
       else if ( mr != eConnectionOk )
            goto fail3;

       if ( cmp_header( &request, &req ) )
            goto fail3;

       // send reply
       cMessageHeader reply;
       MessageHeaderInit( &reply, eMhMsg, dMhReply, request.m_seq, i, 0 );

       if ( ServerWriteMsg( sc, &reply, 0 ) )
            goto fail3;

       // read reply
       cMessageHeader rep;
       unsigned char  rep_data[256];
       mr = ClientReadMsg( cc, &rep, rep_data );

       if ( mr != eConnectionOk )
            goto fail3;

       if ( cmp_header( &reply, &rep ) )
            goto fail3;

       // send the same request again
       if ( ConnectionWriteMsg( cc->m_fd, &cc->m_resend.m_ip_addr, &request, 0 ) )
            goto fail3;

       // read response
       mr = ServerReadMsg( mc, &sc, &req, req_data );

       if ( mr != eConnectionDuplicate )
            goto fail3;

       // read the duplicated response
       mr = ClientReadMsg( cc, &rep, rep_data );

       if ( mr != eConnectionDuplicate )
            goto fail3;

       if ( cmp_header( &reply, &rep ) )
            goto fail3;
     }

  rt = 0;

fail3:
  ClientClose( cc );

fail2:
  ServerClose( mc );

fail1:
  return rt;
}
