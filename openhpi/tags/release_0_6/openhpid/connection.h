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

#ifndef dConnection_h
#define dConnection_h


#ifdef __cplusplus
extern "C" {
#endif


// current version
#define dMhVersion 1

#define dMhEndianBit 1


// cMessageHeader::m_type
typedef enum
{
  eMhPing    = 1,
  eMhPong    = 2,
  eMhRequest = 3,
  eMhReply   = 4,
  eMhClose   = 5, // connection close
  eMhError   = 6, // error
} tMessageType;


typedef struct
{
  unsigned char m_version;
  unsigned char m_type;
  unsigned char m_flags;
  unsigned char m_seq;
  unsigned int  m_id;
  unsigned int  m_len;
} cMessageHeader;


void MessageHeaderInit( cMessageHeader *header, tMessageType type,
		   unsigned char seq, unsigned int id,
		   unsigned int len );


#define dDefaultTimeout 500000

typedef struct
{
  int           m_fd;
  unsigned int  m_timeout;
  unsigned char m_seq;
} cClientConnection;


cClientConnection *ClientConnectionOpen( const char *host, int host_port );
void               ClientConnectionClose( cClientConnection *c );

int ClientConnectionWriteMsg( cClientConnection *c,
			      cMessageHeader *header, const void *request,
			      cMessageHeader *h, void **response );

typedef struct
{
  int m_fd;
  int m_num_connections;
} cServerConnectionMain;


typedef struct
{
  cServerConnectionMain *m_main;
  int                    m_fd;
} cServerConnection;


cServerConnectionMain *ServerConnectionMainOpen  ( int port );
void                   ServerConnectionMainClose ( cServerConnectionMain *c );
cServerConnection     *ServerConnectionMainAccept( cServerConnectionMain *c );
void                   ServerConnectionClose     ( cServerConnection *c );


int ConnectionRead ( int fd, void *data, unsigned int len );
int ConnectionWrite( int fd, const void *data, unsigned int len );
int ConnectionWait ( int fd, unsigned int timeout_ms );

int ConnectionReadHeader( int fd, cMessageHeader *header );
int ConnectionWriteHeader( int fd, cMessageHeader *header );

int ConnectionReadMsg( int fd, cMessageHeader *header, 
                       void **response,
                       unsigned int timeout_ms );
int ConnectionWriteMsg( int fd, cMessageHeader *header, 
                        const void *msg );


#ifdef __cplusplus
}
#endif


#endif
