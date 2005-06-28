/*
 * ipmi_con_file.c
 *
 * IPMI file interface
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

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ipmi_con_file.h"


static unsigned char *
Whitespace( unsigned char *p )
{
  while( *p && ( *p == ' ' || *p == '\t' ) )
       p++;
  
  return p;
}


#define dXDigit(x) ( (x) <= '9' ? (x) - '0' : tolower( x ) - 'a' + 10 )


int
cIpmiConFile::ReadHex( unsigned char **pp, unsigned char *v )
{
  unsigned char *p = *pp;

  p = Whitespace( p );
  
  if ( !isxdigit( *p ) || !isxdigit( p[1] ) )
     {
       m_type = eDtError;
       return 0;
     }

  *v = dXDigit( *p ) << 4 | dXDigit( p[1] );

  *pp = p += 2;

  return 1;
}


unsigned char *
cIpmiConFile::ReadLine( unsigned char *line, int size )
{
  unsigned char *p;
  int s;

  while( 1 )
     {
       int eof = 0;
       p = line;
       s = size - 1;

       while( 1 )
	  {
	    int ch = fgetc( m_fd );

	    if ( ch == EOF )
	       {
		 eof = 1;
		 break;
	       }

	    if ( ch == '\r' )
		 continue;

	    if ( ch == '\n' )
		 break;

            if ( s > 0 )
               {
                 *p++ = ch;
                 s--;
               }
            else
                 IpmiLog( "line to long !\n" );
	  }

       *p = 0;

       p = Whitespace( line );

       if ( *p == 0 || *p == '#' )
	  {
	    if ( eof )
		 return 0;
	  }
       else
            break;
     }

  return p;
}


void
cIpmiConFile::ReadLine()
{
  unsigned char line[1024];
  unsigned char *p;
  unsigned char *q;
  unsigned char v;
  unsigned int i;

  p = ReadLine( line, 1024 );

  if ( p == 0 )
     {
       m_type = eDtEOF;
       return;
     }

  if ( strlen( (char *)p ) < 12 )
     {
       m_type = eDtError;
       return;
     }
  
  p += 12;

  // read type
  if ( !strncmp( (char *)p, "cmd", 3 ) )
       m_type = eDtCommand;
  else if ( !strncmp( (char *)p, "rsp", 3 ) )
       m_type = eDtResponse;
  else if ( !strncmp( (char *)p, "evt", 3 ) )
       m_type = eDtEvent;
  else if ( !strncmp( (char *)p, "tim", 3 ) )
     {
       m_type = eDtTimeout;
       return;
     }
  else if ( !strncmp( (char *)p, "err", 3 ) )
     {
       m_type = eDtError;
       return;
     }
  else
     {
       m_type = eDtError;
       return;
     }

  p = Whitespace( p + 3 );

  // read addr type
  if ( !ReadHex( &p, &v ) )
       return;

  m_addr.m_type = (tIpmiAddrType)v;

  // read addr channel
  if ( !ReadHex( &p, &v ) )
       return;

  m_addr.m_channel = v;

  // read addr lun
  if ( !ReadHex( &p, &v ) )
       return;

  m_addr.m_lun = v;

  switch( v )
     {
       case eIpmiAddrTypeSystemInterface:
            break;

       case eIpmiAddrTypeIpmb:
       case eIpmiAddrTypeIpmbBroadcast:
            // read slave addr
            if ( !ReadHex( &p, &v ) )
                 return;

            m_addr.m_slave_addr = v;
            break;

       default:
            // wrong addr type
            m_type = eDtError;
            return;
     }
 
  // read message
  if ( !ReadHex( &p, &v ) )
       return;

  m_msg.m_netfn = (tIpmiNetfn)v;

  if ( !ReadHex( &p, &v ) )
       return;

  m_msg.m_cmd = (tIpmiCmd)v;

  if ( !ReadHex( &p, &v ) )
       return;

  m_msg.m_data_len = v;

  q = m_msg.m_data;

  for( i = 0; i < m_msg.m_data_len; i++ )
       if ( !ReadHex( &p, q++ ) )
	    return;

  p = Whitespace( p );

  if ( *p != 0 && *p != '#' )
       m_type = eDtError;
}


int
cIpmiConFile::IfOpen()
{
  m_fd = fopen( m_filename, "r" );
  
  if ( m_fd == 0 )
       return errno;

  ReadLine();

  if ( m_type == eDtError )
       return ENOSYS;

  return 0;
}


void
cIpmiConFile::IfClose()
{
  if ( m_fd )
     {
       fclose( m_fd );
       m_fd = 0;
     }
}


int
cIpmiConFile::IfCmd( const cIpmiAddr &addr, const cIpmiMsg &msg,
                     cIpmiAddr &rsp_addr, cIpmiMsg &rsp_msg )
{
  while( m_type == eDtEvent )
       Event();

  if ( m_type != eDtCommand )
       return EINVAL;

  if (    addr.m_type == eIpmiAddrTypeIpmbBroadcast
       && m_addr.m_type == eIpmiAddrTypeIpmb )
     {
       if (    addr.m_channel != m_addr.m_channel 
            || addr.m_lun != m_addr.m_lun
            || addr.m_slave_addr != m_addr.m_slave_addr )
            return EINVAL;
     }
  else
       if ( addr != m_addr )
            return EINVAL;

  if ( !msg.Equal( m_msg ) )
       return EINVAL;

  // read the response
  ReadLine();

  while( m_type == eDtEvent )
       Event();

  if ( m_type == eDtTimeout )
     {
       ReadLine();
       return ETIMEDOUT;
     }

  if ( m_type != eDtResponse )
       return EINVAL;

  rsp_addr = m_addr;
  rsp_msg  = m_msg;

  // read ahead
  do
       ReadLine();
  while( m_type == eDtEvent );

  return 0;
}


void
cIpmiConFile::IfEvent()
{
  if ( m_type != eDtEvent )
       return;

  HandleEvent( m_addr, m_msg );

  ReadLine();
}


cIpmiConFile::cIpmiConFile( unsigned int timeout, unsigned int atca_timeout,
                            const char *filename )
  : cIpmiCon( timeout, atca_timeout )
{
  if ( filename == 0 )
       filename = dIpmiConFileDefault;

  assert( filename );

  strcpy( m_filename, filename );
}


cIpmiConFile::~cIpmiConFile()
{
  if ( IsOpen() )
       Close();
}
