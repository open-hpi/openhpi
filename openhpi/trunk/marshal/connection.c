/*
 * socket and message  handling 
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

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <byteswap.h>

#include "connection.h"
#include "marshal.h"


void
MessageHeaderInit( cMessageHeader *header, tMessageType type,
		   unsigned char seq, unsigned int id,
		   unsigned int len )
{
  memset( header, 0, sizeof( cMessageHeader ) );

  header->m_version = dMhVersion;
  header->m_type    = type;
  header->m_flags   = MarshalByteOrder();
  header->m_seq     = seq;
  header->m_id      = id;
  header->m_len     = len;
}


int
ConnectionRead( int fd, void *d, unsigned int len )
{
  assert( len < 0xffff );
  
  unsigned char *data = (unsigned char *)d;

  while( len > 0 )
     {
       int rv = read( fd, data, len );

       if ( rv == 0 )
	    // socket closed
	    return EPIPE;

       if ( rv < 0 )
	    return errno;

       data += rv;
       len -= rv;
     }

  return 0;
}


int
ConnectionWrite( int fd, const void *data, unsigned int len )
{
  assert( len < 0xffff );

  int rv = write( fd, data, len );

  return rv == len ? 0 : errno;
}


int 
ConnectionWait( int fd, unsigned int timeout_ms )
{
  struct timeval tv;
  struct timeval timeout;
  struct timeval t0;

  // create absolute timeout
  gettimeofday( &timeout, 0 );

  timeout.tv_sec += timeout_ms / 1000;
  timeout.tv_usec += (timeout_ms % 1000) * 1000;

  while( timeout.tv_usec > 10000000 )
     {
       timeout.tv_sec++;
       timeout.tv_usec -= 10000000;
     }

  struct pollfd pfd;

  pfd.fd = fd;
  pfd.events = POLLIN;

  // loop
  while( 1 )
     {
       // relative timeout
       gettimeofday( &t0, 0 );

       timersub( &timeout, &t0, &tv );

       if ( tv.tv_sec < 0 || tv.tv_usec < 0 )
	  {
	    tv.tv_sec  = 0;
	    tv.tv_usec = 0;
	  }

       timeout_ms =  tv.tv_sec * 1000 + tv.tv_usec / 1000;

       int rv = poll( &pfd, 1, timeout_ms );

       // timeout
       if ( !rv )
	    return ETIME;

       if ( rv == -1 )
	  {
            if ( errno == EINTR )
                 continue;

	    assert( 0 );
	    return errno;
	  }

       assert( rv == 1 );
       break;
     }

  return 0;
}


int
ConnectionReadHeader( int fd, cMessageHeader *header )
{
  int rv = ConnectionRead( fd, header, sizeof( cMessageHeader ) );

  if ( rv )
       return rv;
  
  if ( (header->m_flags & dMhEndianBit) != MarshalByteOrder() )
     {
       header->m_id  = bswap_32( header->m_id );
       header->m_len = bswap_32( header->m_len );
     }

  return 0;
}


int
ConnectionWriteHeader( int fd, cMessageHeader *header )
{
  int rv = ConnectionWrite( fd, header, sizeof( cMessageHeader ) );

  return rv;
}


int
ConnectionReadMsg( int fd, cMessageHeader *header, 
		   void **response,
		   unsigned int timeout_ms )
{
  int rv;

  if ( timeout_ms )
     {
       rv = ConnectionWait( fd, timeout_ms );

       if ( rv )
	    return rv;
     }

  rv = ConnectionReadHeader( fd, header );

  if ( rv )
       return rv;

  if ( header->m_len )
     {
       assert( header->m_len < 0xffff );

       void *data = malloc( header->m_len );

       if ( !data )
	    return ENOMEM;

       rv = ConnectionRead( fd, data, header->m_len );

       if ( rv )
	    return rv;

       *response = data;
     }

  return 0;
}


int
ConnectionWriteMsg( int fd, cMessageHeader *header, 
		    const void *msg )
{
  int rv = ConnectionWriteHeader( fd, header );

  if ( rv )
       return rv;

  if ( header->m_len )
     {
       rv = ConnectionWrite( fd, msg, header->m_len );

       if ( rv )
            return rv;
     }

  return 0;
}


cServerConnectionMain *
ServerConnectionMainOpen( int port )
{
  int listen_socket = socket( AF_INET, SOCK_STREAM, 0 );

  if ( listen_socket == -1 )
     {
       fprintf( stderr, "can't create socket: %s, %d\n",
                strerror( errno ), errno );
       return 0;
     }

  // Set socket options.  We try to make the port
  // reusable and have it close as fast as possible
  // without waiting in unnecessary wait states on
  // close.
  int on = 1;
  setsockopt( listen_socket, SOL_SOCKET, SO_REUSEADDR,
	      (void *)&on, sizeof(on) );

  // Bind our address and the port to the socket
  struct sockaddr_in myaddr;
  memset( &myaddr, 0, sizeof( struct sockaddr_in ) );
  myaddr.sin_family      = AF_INET;
  myaddr.sin_port        = htons( port );
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if ( bind( listen_socket, (struct sockaddr *)&myaddr,
	     sizeof( struct sockaddr ) ) < 0 )
     {
       fprintf( stderr, "can't bind: %s, %d\n",
                strerror( errno ), errno );

       close( listen_socket );
       return 0;
     }

  // Listen for an incomming connections...
  if ( listen( listen_socket, 256 ) < 0 )
     {
       fprintf( stderr, "can't listen: %s, %d\n",
                strerror( errno ), errno );
       close( listen_socket );
       return 0;
     }

  cServerConnectionMain *s = malloc( sizeof( cServerConnectionMain ) );
  s->m_fd = listen_socket;
  s->m_num_connections = 0;

  return s;
}


void
ServerConnectionMainClose( cServerConnectionMain *c )
{
  assert( c->m_num_connections == 0 );

  if ( c->m_fd )
       close( c->m_fd );

  free( c );
}


cServerConnection *
ServerConnectionMainAccept( cServerConnectionMain *c )
{
  struct sockaddr_in from;
  socklen_t fromlen = sizeof( struct sockaddr_in );

  assert( c->m_fd );
  int newsock = accept( c->m_fd, (struct sockaddr *)&from, &fromlen );

  if ( newsock < 0 )
     {
       if ( errno == EINVAL )
          {
            fprintf( stderr, "I think this is not a main socket (error %d, %s) !\n",
                     errno, strerror(errno) );
            fprintf( stderr, "use the 'wait' option in your inetd.conf !\n" );

            return 0;
          }

       if ( errno != EINTR && errno != EWOULDBLOCK )
            fprintf( stderr, "accept fail: %d, %d, %s !\n",
                     c->m_fd, errno, strerror(errno) );

       return 0;
     }

  cServerConnection *s = malloc( sizeof( cServerConnection ) );
  s->m_main = c;
  s->m_fd = newsock;

  c->m_num_connections++;

  return s;
}


void
ServerConnectionClose( cServerConnection *c )
{
  if ( c->m_fd )
       close( c->m_fd );

  c->m_main->m_num_connections--;
  assert( c->m_main->m_num_connections >= 0 );

  free( c );
}


cClientConnection *
ClientConnectionOpen( const char *host, int host_port )
{
  struct addrinfo hints, *ai, *aitop;
  char ntop[NI_MAXHOST], strport[NI_MAXSERV];
  int gaierr;
  int sock = -1;

  memset( &hints, 0, sizeof( hints ) );
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  snprintf( strport, sizeof strport, "%d", host_port );

  gaierr = getaddrinfo( host, strport, &hints, &aitop );

  if ( gaierr != 0 )
     {
       fprintf( stderr, "unknown host %s: %s\n", host, gai_strerror( gaierr ) );
       return 0;
     }

  for( ai = aitop; ai; ai = ai->ai_next)
     {
       if ( ai->ai_family != AF_INET && ai->ai_family != AF_INET6 )
	    continue;

       if ( getnameinfo( ai->ai_addr, ai->ai_addrlen, ntop, sizeof( ntop ), 
			 strport, sizeof( strport ), NI_NUMERICHOST|NI_NUMERICSERV ) != 0 )
	  {
	    fprintf( stderr, "getnameinfo failed\n" );
	    continue;
	  }

       // Create the socket.
       sock = socket( ai->ai_family, SOCK_STREAM, 0 );
       if ( sock < 0 )
	  {
	    fprintf( stderr, "can't create socket: %s, %d\n",
                     strerror( errno ), errno );
	    continue;
	  }

       // Connect to the host/port.
       if ( connect( sock, ai->ai_addr, ai->ai_addrlen ) < 0 )
	  {
	    fprintf( stderr, "can't connect %s port %s: %s, %d\n", ntop, strport,
                     strerror( errno ), errno );

	    close( sock );
	    continue;       // fail -- try next
	  }

       break; // success
     }

  freeaddrinfo( aitop );

  if ( !ai )
     {
       fprintf( stderr, "connect %s port %d: failed\n", host, host_port );

       return 0;
     }

  cClientConnection *c = malloc( sizeof( cClientConnection ) );
  c->m_fd      = sock;
  c->m_timeout = dDefaultTimeout;
  c->m_seq     = 0;

  return c;
}


void
ClientConnectionClose( cClientConnection *c )
{
  if ( c->m_fd )
       close( c->m_fd );

  free( c );
}


int
ClientConnectionWriteMsg( cClientConnection *c,
			  cMessageHeader *header, const void *request,
                          cMessageHeader *h, void **response )
{
  c->m_seq += 1;

  header->m_seq     = c->m_seq;
  
  int rv = ConnectionWriteMsg( c->m_fd, header, request );

  if ( rv )
       return rv;

  return ConnectionReadMsg( c->m_fd, h, response, c->m_timeout );
}
