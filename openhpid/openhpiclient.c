/*
 * hpi library code
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

#include "client_config.h"
#include "openhpiclient.h"
#include "marshal_hpi.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/poll.h>
#include <errno.h>
#include <stdio.h>


static void CleanupClient( cOpenHpiClientConf *c );
static SaErrorT SendMsg( cOpenHpiClientConf *c, cMessageHeader *request_header,
                         void *request, cMessageHeader *reply_header,
                         void **reply, int retries );

static void *ReaderThread( void *param );
static void  Reader( cOpenHpiClientConf *c );
static void  ReadResponse( cOpenHpiClientConf *c );

static pthread_mutex_t lock_tmpl = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;


#ifdef dOpenHpiClientWithConfig
static cOpenHpiClientConf config_data = 
{
  .m_client_connection = 0,
  .m_initialize        = 0,

  .m_lock              = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP,
  .m_queue             = 0,
  .m_num_outstanding   = 0,
  .m_session           = 0,
  .m_num_sessions      = 0
};

static cOpenHpiClientConf *config = &config_data;

static cConfigEntry config_table[] =
{
  {
    .m_name = "daemon",
    .m_type = eConfigTypeString,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_server ),
    .m_default.m_string = "localhost"
  },
  {
    .m_name = "port",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_port ),
    .m_default.m_int = dDefaultDaemonPort
  },
  {
    .m_name = "debug",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_debug ),
    .m_default.m_int = 0xffff
  },
  {
    // max outstanding messages
    .m_name = "max_outstanding",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_max_outstanding ),
    .m_default.m_int = dDefaultMaxOutstanding
  },
  {
    // retries
    .m_name = "retries",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_retries ),
    .m_default.m_int = dDefaultMessageRetries
  },
  {
    // timeout in ms
    .m_name = "timeout",
    .m_type = eConfigTypeInt,
    .m_offset = dConfigOffset( cOpenHpiClientConf, m_timeout ),
    .m_default.m_int = dDefaultMessageTimeout
  },
  {
    .m_type = eConfigTypeUnknown
  }
};
#endif


static int
InitClient( cOpenHpiClientConf *c )
{
  assert( c->m_client_connection == 0 );

  c->m_client_connection = 0;
  c->m_lock              = lock_tmpl;
  c->m_queue             = 0;

  int i;
  for( i = 0; i < 256; i++ )
       c->m_outstanding[i] = 0;

  c->m_num_outstanding   = 0;
  c->m_thread_state      = eOpenHpiClientThreadStateSuspend;
  c->m_thread_exit       = 0;
  c->m_session           = 0;
  c->m_num_sessions      = 0;

#ifdef dOpenHpiClientWithConfig
  char *openhpi_client_conf = getenv( "OPENHPICLIENT_CONF" );

  if ( openhpi_client_conf == 0 )
       openhpi_client_conf = OH_CLIENT_DEFAULT_CONF;

  ConfigRead( openhpi_client_conf, config_table, c );
#endif

  c->m_client_connection = ClientOpen( c->m_server, c->m_port );

  if ( !c->m_client_connection )
       return 0;

  // start reader thread
  int rv = pthread_create( &c->m_thread, 0, ReaderThread, c );

  if ( rv )
       return 0;

  // wait till real thread start
  while( c->m_thread_state == eOpenHpiClientThreadStateSuspend )
       // wait 100 ms
       usleep( 10000 );

  // send reset message
  cMessageHeader header;
  MessageHeaderInit( &header, eMhReset, dMhRequest, 0, 0, 0 );
  cMessageHeader rh;
  void *data;

  SaErrorT r = SendMsg( c, &header, 0, &rh, &data, c->m_retries );

  if ( r != SA_OK )
     {
       //LogUtilsError( eLfRemote, "cannot send startup message to daemon !\n" );
       CleanupClient( c );

       return 0;
     }

  return 1;
}


static void
CleanupClient( cOpenHpiClientConf *c )
{
  c->m_thread_exit = 1;

  while( c->m_thread_state == eOpenHpiClientThreadStateRun )
       usleep( 100000 );

  // send a close
  cMessageHeader header;
  MessageHeaderInit( &header, eMhMsg, dMhRequest, 0, eFClose, 0 );
  ClientWriteMsg( c->m_client_connection, &header, 0 );

  assert( c->m_client_connection );
  ClientClose( c->m_client_connection );
  c->m_client_connection = 0;

  assert( c->m_num_outstanding == 0 );

  if ( c->m_session )
     {
       free( c->m_session );
       c->m_session = 0;
     }
 
  c->m_num_sessions = 0;
}


static void
Lock( cOpenHpiClientConf *c )
{
  pthread_mutex_lock( &c->m_lock );
}


static void 
Unlock( cOpenHpiClientConf *c )
{
  pthread_mutex_unlock( &c->m_lock );
}


static int 
FindSessionId( cOpenHpiClientConf *c, SaHpiSessionIdT sid )
{
  int i;

  for( i = 0; i < c->m_num_sessions; i++ )
       if ( c->m_session[i] == sid )
            return i;
  
  return -1;
}


static void
AddSessionId( cOpenHpiClientConf *c, SaHpiSessionIdT sid )
{
  SaHpiSessionIdT *array = (SaHpiSessionIdT *)malloc( sizeof( SaHpiSessionIdT ) * ( c->m_num_sessions + 1 ) );

  if ( c->m_session )
     {
       memcpy( array, c->m_session, sizeof( SaHpiSessionIdT ) * c->m_num_sessions );
       free( c->m_session );
     }

  c->m_session = array;
  c->m_session[c->m_num_sessions] = sid;
  c->m_num_sessions += 1;
}


static int
RemSessionId( cOpenHpiClientConf *c, SaHpiSessionIdT sid )
{
  int idx = FindSessionId( c, sid );

  if ( idx < 0 )
       return -1;

  if ( c->m_num_sessions == 1 )
     {
       free( c->m_session );
       c->m_session = 0;
       c->m_num_sessions -= 1;

       return 0;
     }

  int s = sizeof( SaHpiSessionIdT ) * c->m_num_sessions;

  SaHpiSessionIdT *array = (SaHpiSessionIdT *)malloc( s - sizeof( SaHpiSessionIdT ) );

  if ( idx > 0 )
       memcpy( array, c->m_session, s );

  if ( idx != c->m_num_sessions - 1 )
       memcpy( array + s, c->m_session + s + sizeof( SaHpiSessionIdT ),
               sizeof( SaHpiSessionIdT ) * ( c->m_num_sessions - idx - 1 ) );

  free( c->m_session );
  c->m_session = array;

  c->m_num_sessions -= 1;

  return 0;
}


static void
AddOutstanding( cOpenHpiClientConf *c, cOpenHpiClientRequest *r )
{
  assert( c->m_num_outstanding < c->m_max_outstanding );

  int seq = r->m_request_header->m_seq;

  assert( c->m_outstanding[seq] == 0 );
  c->m_outstanding[seq] = r;
  c->m_num_outstanding += 1;
}


static void
RemOutstanding( cOpenHpiClientConf *c, int seq )
{
  assert( seq >= 0 && seq < 256 );

  if ( c->m_outstanding[seq] == 0 )
     {
       assert( 0 );
       return;
     }

  c->m_outstanding[seq] = 0;
  c->m_num_outstanding -= 1;

  assert( c->m_num_outstanding >= 0 );
}


static void
HandleMsgError( cOpenHpiClientConf *c, cOpenHpiClientRequest *r, SaErrorT err )
{
  r->m_error = err;
  pthread_mutex_lock( r->m_lock );
  r->m_cond_var = 1;
  pthread_cond_signal( r->m_cond );
  pthread_mutex_unlock( r->m_lock );
}


static void
SetMsgTimeout( cOpenHpiClientConf *c, cOpenHpiClientRequest *r )
{
  // message timeout
  gettimeofday( &r->m_timeout, 0 );

  r->m_timeout.tv_sec += c->m_timeout / 1000;
  r->m_timeout.tv_usec += (c->m_timeout % 1000) * 1000;

  while( r->m_timeout.tv_usec > 1000000 )
     {
       r->m_timeout.tv_sec += 1;
       r->m_timeout.tv_usec -= 1000000;
     }
}


static SaErrorT
ResendMsg( cOpenHpiClientConf *c, cOpenHpiClientRequest *r )
{
  //  LogUtilsError( eLfRemote, "resend msg.\n" );

  r->m_retries_left -= 1;

  int rv = ConnectionWriteMsg( c->m_client_connection->m_fd,
                               &c->m_client_connection->m_resend.m_ip_addr,
                               r->m_request_header, r->m_request );

  if ( rv )
       return SA_ERR_HPI_BUSY;

  SetMsgTimeout( c, r );

  return SA_OK;
}


static SaErrorT
SendCmd( cOpenHpiClientConf *c, cOpenHpiClientRequest *r )
{
  assert( c->m_num_outstanding < c->m_max_outstanding );
  r->m_retries_left -= 1;

  int rv = ClientWriteMsg( c->m_client_connection, r->m_request_header, r->m_request );

  if ( rv )
       return SA_ERR_HPI_BUSY;

  AddOutstanding( c, r );

  SetMsgTimeout( c, r );

  return SA_OK;
}


static void
SendCmds( cOpenHpiClientConf *c )
{
  while( c->m_queue && c->m_num_outstanding < c->m_max_outstanding )
     {
       cOpenHpiClientRequest *r = c->m_queue;
       c->m_queue = c->m_queue->m_next;

       SaErrorT rv = SendCmd( c, r );

       if ( rv )
            HandleMsgError( c, r, rv );
     }
}


static SaErrorT
SendMsg( cOpenHpiClientConf *c, cMessageHeader *request_header,
	 void *request, cMessageHeader *reply_header, void **reply, int retries )
{
  *reply = 0;

  // check for marshal error
  if ( request_header->m_len < 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t lock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

  cOpenHpiClientRequest *r = (cOpenHpiClientRequest *)malloc( sizeof( cOpenHpiClientRequest ) );
  memset( r, 0, sizeof( cOpenHpiClientRequest ) );

  r->m_request_header = request_header;
  r->m_request        = request;
  r->m_reply_header   = reply_header;
  r->m_reply          = reply;
  r->m_retries_left   = retries;
  r->m_lock           = &lock;
  r->m_cond           = &cond;
  r->m_cond_var       = 0;
  r->m_error          = SA_ERR_HPI_TIMEOUT;

  assert( c->m_thread_state == eOpenHpiClientThreadStateRun );

  pthread_mutex_lock( r->m_lock );
  Lock( c );

  SaErrorT rv;

  if ( c->m_num_outstanding < c->m_max_outstanding )
     {
       // send the command within this thread context.
       rv = SendCmd( c, r );

       if ( rv )
	  {
	    // error
	    free( r );

	    Unlock( c );
	    pthread_mutex_unlock( r->m_lock );
	    return rv;
	  }
     }
  else
     {
       // send queue full
       r->m_next = c->m_queue;
       c->m_queue = r;
     }

  Unlock( c );

  // wait for response
  do
     {
       pthread_cond_wait( &cond, &lock );
     }
  while( r->m_cond_var == 0 );

  pthread_mutex_unlock( &lock );

  rv = r->m_error;

  free( r );

  return rv;
}


static void *
ReaderThread( void *param )
{
  cOpenHpiClientConf *c = (cOpenHpiClientConf *)param;

  c->m_thread_state = eOpenHpiClientThreadStateRun;

  Reader( c );

  c->m_thread_state = eOpenHpiClientThreadStateExit;

  return 0;
}


static void
Reader( cOpenHpiClientConf *c )
{
  // create pollfd
  struct pollfd pfd;

  pfd.events = POLLIN;

  // reader loop
  while( !c->m_thread_exit )
     {
       assert( c->m_client_connection->m_fd >= 0 );
       pfd.fd = c->m_client_connection->m_fd;

       int rv = poll( &pfd, 1, 100 );

       if ( rv == 1 )
            // read response
            ReadResponse( c );
       else if ( rv != 0 )
          {
            if ( errno != EINTR )
               {
                 // error
                 assert( 0 );
                 abort();
               }
          }

       // check for expiered ipmi commands
       struct timeval now;
       gettimeofday( &now, 0 );

       Lock( c );

       int i;
       for( i = 0; i < 256; i++ )
          {
            if ( c->m_outstanding[i] == 0 )
                 continue;

            cOpenHpiClientRequest *r = c->m_outstanding[i];

            if (    r->m_timeout.tv_sec > now.tv_sec 
		 || (    r->m_timeout.tv_sec == now.tv_sec
		      && r->m_timeout.tv_usec > now.tv_usec ) )
                 continue;

            if ( r->m_retries_left > 0 )
               {
                 if ( ResendMsg( c, r ) == SA_OK )
                      continue;
               }

            // timeout expired
            RemOutstanding( c, r->m_request_header->m_seq );

            HandleMsgError( c, r, SA_ERR_HPI_NO_RESPONSE );
          }

       // send new comands
       SendCmds( c );

       Unlock( c );
     }
}


static void
HandlePing( cOpenHpiClientConf *c, cMessageHeader *request_header )
{
  // send ping back
  cMessageHeader header;
  MessageHeaderInit( &header, (tMessageType)request_header->m_type, dMhReply, 
                     request_header->m_seq, request_header->m_id, 0 );

  ClientWriteMsg( c->m_client_connection, &header, 0 );
}


static void
HandleReset( cOpenHpiClientConf *c, cMessageHeader *request_header )
{
  // send reset back
  cMessageHeader header;
  MessageHeaderInit( &header, (tMessageType)request_header->m_type, dMhReply, 
                     request_header->m_seq, request_header->m_id, 0 );

  ClientWriteMsg( c->m_client_connection, &header, 0 );

  Lock( c );

  int i;

  for( i = 0; i < 256; i++ )
     {
       cOpenHpiClientRequest *r = c->m_outstanding[i];

       if ( r == 0 )
            continue;

       RemOutstanding( c, i );

       HandleMsgError( c, r, SA_ERR_HPI_NO_RESPONSE );
     }

  Unlock( c );
}


static void
ReadResponse( cOpenHpiClientConf *c )
{
  cMessageHeader header;
  unsigned char reply[dMaxMessageLength];

  tConnectionError rv = ClientReadMsg( c->m_client_connection, &header, reply );

  if ( rv != eConnectionOk )
     {
       // error
       return;
     }

  if ( header.m_len > dMaxMessageLength )
     {
       assert( 0 );
       return;
     }

  if ( IsRequestMsg( &header ) )
     {
       if ( header.m_type == eMhPing )
          {
            HandlePing( c, &header );
            return;
          }

       if ( header.m_type == eMhReset )
          {
            HandleReset( c, &header );
            return;
          }

       // ignore other request messages
       assert( 0 );

       return;
     }

  Lock( c );

  if ( c->m_outstanding[header.m_seq_in] == 0 )
     {
       // reply without response
       //assert( 0 );
       Unlock( c );

       return;
     }

  cOpenHpiClientRequest *r = c->m_outstanding[header.m_seq_in];
  assert( r->m_request_header->m_seq == header.m_seq_in );

  if ( r->m_request_header->m_id != header.m_id )
     {
       // wrong id
       //assert( 0 );

       Unlock( c );
       return;
     }

  RemOutstanding( c, header.m_seq_in );

  // reply
  unsigned char *rep = 0;

  if ( header.m_len )
     {
       rep = (unsigned char *)malloc( header.m_len );
       memcpy( rep, reply, header.m_len );
     }

  *r->m_reply_header = header;
  *r->m_reply        = rep;
  r->m_error         = SA_OK;

  pthread_mutex_lock( r->m_lock );
  r->m_cond_var = 1;
  pthread_cond_signal( r->m_cond );
  pthread_mutex_unlock( r->m_lock );

  Unlock( c );
}


#define CheckSession() \
  do                                            \
     {                                          \
       if ( !config->m_initialize )             \
            return SA_ERR_HPI_INTERNAL_ERROR;   \
                                                \
       if ( !config->m_client_connection )      \
            return SA_ERR_HPI_INVALID_SESSION;  \
     }                                          \
  while( 0 )


#define PreMarshal(id)   \
  cMessageHeader request_header; \
  cMessageHeader reply_header; \
  void *request;         \
  void *reply;           \
  SaErrorT err;          \
                         \
  cHpiMarshal *hm = hm = HpiMarshalFind( id ); \
  assert( hm );          \
                         \
  MessageHeaderInit( &request_header, eMhMsg, dMhRequest, 0, id, hm->m_request_len ); \
                         \
  request = malloc( hm->m_request_len )


#define Send()       \
  do                 \
     {               \
       SaErrorT r = SendMsg( config, &request_header, \
                             request, &reply_header, &reply, \
                             config->m_retries );            \
                     \
       free( request ); \
                     \
       if ( r )      \
	    return r; \
     }               \
  while( 0 )


#define PostMarshal() \
  if ( reply )        \
       free( reply ); \
                      \
  if ( mr < 0 )       \
       return SA_ERR_HPI_INVALID_PARAMS


SaErrorT SAHPI_API dOpenHpiClientFunction(
SessionOpen)dOpenHpiClientParam( SAHPI_IN  SaHpiDomainIdT   DomainId,
				 SAHPI_OUT SaHpiSessionIdT *SessionId,
				 SAHPI_IN  void            *SecurityParams )
{
  if ( config->m_initialize == 0 )
       config->m_initialize = 1;

  if ( SessionId == 0 || SecurityParams != 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  if ( config->m_num_sessions == 0 )
     {
       if ( !InitClient( config ) )
	    return SA_ERR_HPI_NO_RESPONSE;
     }

  PreMarshal( eFsaHpiSessionOpen );

  request_header.m_len = HpiMarshalRequest1( hm, request, &DomainId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, SessionId );

  PostMarshal();

  if ( err == SA_OK )
       AddSessionId( config, *SessionId );
  else if ( config->m_num_sessions == 0 )
       CleanupClient( config );

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SessionClose)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  if ( RemSessionId( config, SessionId ) < 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  PreMarshal( eFsaHpiSessionClose );
  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );

  SaErrorT r = SendMsg( config, &request_header,
                        request, &reply_header, &reply,
                        config->m_retries );

  free( request );

  if ( config->m_num_sessions == 0 )
       CleanupClient( config );

  if ( r )
       return r;

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
Discover)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiDiscover );
  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
RptInfoGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
				SAHPI_OUT SaHpiRptInfoT *RptInfo )
{
  if ( RptInfo == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiRptInfoGet );
  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, RptInfo );

  PostMarshal();

  return err;  
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
RptEntryGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT SessionId,
				 SAHPI_IN  SaHpiEntryIdT EntryId,
				 SAHPI_OUT SaHpiEntryIdT *NextEntryId,
				 SAHPI_OUT SaHpiRptEntryT *RptEntry )
{
  if ( NextEntryId == 0 || RptEntry == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiRptEntryGet );
  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &EntryId );

  Send();

  int mr = HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply,
                      &err, NextEntryId, RptEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
RptEntryGetByResourceId)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
					     SAHPI_IN  SaHpiResourceIdT ResourceId,
					     SAHPI_OUT SaHpiRptEntryT  *RptEntry )
{
  if ( RptEntry == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiRptEntryGetByResourceId );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, RptEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceSeveritySet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
					 SAHPI_IN SaHpiResourceIdT ResourceId,
					 SAHPI_IN SaHpiSeverityT   Severity )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceSeveritySet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Severity);
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceTagSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
				    SAHPI_IN SaHpiResourceIdT  ResourceId,
				    SAHPI_IN SaHpiTextBufferT *ResourceTag )
{
  if ( ResourceTag == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiResourceTagSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, ResourceTag );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceIdGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
				   SAHPI_OUT SaHpiResourceIdT *ResourceId )
{
  if ( ResourceId == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();
  
  if ( FindSessionId( config, SessionId ) == -1 )
       return SA_ERR_HPI_INVALID_SESSION;
  
  //return GetLocalResourceId();
  
         /*
  PreMarshal( eFsaHpiResourceIdGet );

  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, ResourceId );

  PostMarshal();
         */
         
  return SA_ERR_HPI_INVALID_PARAMS;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EntitySchemaGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT SessionId,
				     SAHPI_OUT SaHpiUint32T   *SchemaId )
{
  if ( SchemaId == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEntitySchemaGet );

  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, SchemaId );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogInfoGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
				     SAHPI_IN  SaHpiResourceIdT ResourceId,
				     SAHPI_OUT SaHpiEventLogInfoT    *Info )
{
  if ( Info == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventLogInfoGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Info );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogEntryGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT   SessionId,
				      SAHPI_IN    SaHpiResourceIdT  ResourceId,
				      SAHPI_IN    SaHpiEventLogEntryIdT  EntryId,
				      SAHPI_OUT   SaHpiEventLogEntryIdT *PrevEntryId,
				      SAHPI_OUT   SaHpiEventLogEntryIdT *NextEntryId,
				      SAHPI_OUT   SaHpiEventLogEntryT   *EventLogEntry,
				      SAHPI_INOUT SaHpiRdrT        *Rdr,
				      SAHPI_INOUT SaHpiRptEntryT   *RptEntry )
{
  if (    PrevEntryId == 0 
       || NextEntryId == 0 
       || EventLogEntry == 0
       || Rdr == 0
       || RptEntry == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );

  Send();

  SaHpiRdrT rdr;
  SaHpiRptEntryT rpt_entry;

  int mr = HpiDemarshalReply5( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       PrevEntryId, NextEntryId, EventLogEntry, 
			       &rdr, &rpt_entry );

  if ( Rdr )
       *Rdr = rdr;

  if ( RptEntry )
       *RptEntry = rpt_entry;

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogEntryAdd)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				      SAHPI_IN SaHpiResourceIdT ResourceId,
				      SAHPI_IN SaHpiEventLogEntryT  *EvtEntry )
{
  if ( EvtEntry == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryAdd );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, EvtEntry );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, EvtEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogEntryDelete)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
					 SAHPI_IN SaHpiResourceIdT ResourceId,
					 SAHPI_IN SaHpiEventLogEntryIdT EntryId )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogEntryDelete );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogClear)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				   SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogClear );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogTimeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
				     SAHPI_IN  SaHpiResourceIdT ResourceId,
				     SAHPI_OUT SaHpiTimeT       *Time )
{
  if ( Time == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventLogTimeGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Time );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogTimeSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				     SAHPI_IN SaHpiResourceIdT ResourceId,
				     SAHPI_IN SaHpiTimeT       Time )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogTimeSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Time );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
				      SAHPI_IN  SaHpiResourceIdT ResourceId,
				      SAHPI_OUT SaHpiBoolT       *Enable )
{
  if ( Enable == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventLogStateGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Enable );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventLogStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				      SAHPI_IN SaHpiResourceIdT ResourceId,
				      SAHPI_IN SaHpiBoolT       Enable )
{
  CheckSession();

  PreMarshal( eFsaHpiEventLogStateSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Enable );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
Subscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
			       SAHPI_IN SaHpiBoolT      ProvideActiveAlarms )
{
  CheckSession();

  PreMarshal( eFsaHpiSubscribe );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ProvideActiveAlarms );

  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
Unsubscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId )
{
  CheckSession();

  PreMarshal( eFsaHpiUnsubscribe );

  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EventGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT SessionId,
			      SAHPI_IN    SaHpiTimeoutT   Timeout,
			      SAHPI_OUT   SaHpiEventT    *Event,
			      SAHPI_INOUT SaHpiRdrT      *Rdr,
			      SAHPI_INOUT SaHpiRptEntryT *RptEntry )
{
  if ( Event == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEventGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &Timeout );

  int retries = config->m_retries;

  if ( Timeout == SAHPI_TIMEOUT_IMMEDIATE )
       retries = config->m_retries;
  else if ( Timeout == SAHPI_TIMEOUT_BLOCK )
       retries = 0x8fffffff;
  else
     {
       SaHpiTimeoutT t = Timeout / (SaHpiTimeoutT)config->m_timeout;
       t /= (SaHpiTimeoutT)1000000;
       t += (SaHpiTimeoutT)config->m_retries;

       if ( t > (SaHpiTimeoutT)0x8fffffff )
            retries = 0x8fffffff;
       else
            retries = (int)t;
     }

  //LogUtilsError( eLfRemote, "EventGet: retries %d, timeout %d, use %d retries\n",
  //               config->m_retries, config->m_timeout, retries );

  SaErrorT r = SendMsg( config, &request_header,
                        request, &reply_header, &reply,
                        retries );

  free( request );

  //LogUtilsError( eLfRemote, "EventGet: return: %d\n", r );

  if ( r )
       return r;

  SaHpiRdrT      rdr;
  SaHpiRptEntryT rpt_entry;

  if ( Rdr == 0 )
       Rdr = &rdr;

  if ( RptEntry == 0 )
       RptEntry = &rpt_entry;

  int mr = HpiDemarshalReply3( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Event, Rdr, RptEntry );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
RdrGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
			    SAHPI_IN  SaHpiResourceIdT ResourceId,
			    SAHPI_IN  SaHpiEntryIdT    EntryId,
			    SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
			    SAHPI_OUT SaHpiRdrT       *Rdr )
{
  if ( NextEntryId == 0 || Rdr == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiRdrGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &EntryId );
  
  Send();

  int mr = HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       NextEntryId, Rdr );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorReadingGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
				      SAHPI_IN  SaHpiResourceIdT     ResourceId,
				      SAHPI_IN  SaHpiSensorNumT      SensorNum,
				      SAHPI_OUT SaHpiSensorReadingT *Reading )
{
  if ( Reading == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorReadingGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Reading);

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorReadingConvert)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
					  SAHPI_IN  SaHpiResourceIdT     ResourceId,
					  SAHPI_IN  SaHpiSensorNumT      SensorNum,
					  SAHPI_IN  SaHpiSensorReadingT *ReadingInput,
					  SAHPI_OUT SaHpiSensorReadingT *ConvertedReading )
{
  if ( ReadingInput == 0 || ConvertedReading == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorReadingConvert );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId,
		     &SensorNum, ReadingInput );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       ConvertedReading );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorThresholdsGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
					 SAHPI_IN  SaHpiResourceIdT        ResourceId,
					 SAHPI_IN  SaHpiSensorNumT         SensorNum,
					 SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds )
{
  if ( SensorThresholds == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorThresholdsGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );

  Send();

  // this is to pass test sf_202.test of hpitest
  memset( SensorThresholds, 0, sizeof( SaHpiSensorThresholdsT ) );
  
  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       SensorThresholds );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorThresholdsSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
					 SAHPI_IN  SaHpiResourceIdT        ResourceId,
					 SAHPI_IN  SaHpiSensorNumT         SensorNum,
					 SAHPI_IN  SaHpiSensorThresholdsT *SensorThresholds )
{
  if ( SensorThresholds == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorThresholdsSet );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &SensorNum, SensorThresholds );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
				   SAHPI_IN  SaHpiResourceIdT     ResourceId,
				   SAHPI_IN  SaHpiSensorNumT      SensorNum,
				   SAHPI_OUT SaHpiSensorTypeT    *Type,
				   SAHPI_OUT SaHpiEventCategoryT *Category )
{
  if ( Type == 0 || Category == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorTypeGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );
  
  Send();

  int mr = HpiDemarshalReply2( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Type, Category );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorEventEnablesGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
					   SAHPI_IN  SaHpiResourceIdT        ResourceId,
					   SAHPI_IN  SaHpiSensorNumT         SensorNum,
					   SAHPI_OUT SaHpiSensorEvtEnablesT *Enables )
{
  if ( Enables == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorEventEnablesGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &SensorNum );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, 
			       Enables );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
SensorEventEnablesSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT         SessionId,
					   SAHPI_IN SaHpiResourceIdT        ResourceId,
					   SAHPI_IN SaHpiSensorNumT         SensorNum,
					   SAHPI_IN SaHpiSensorEvtEnablesT *Enables )
{
  if ( Enables == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiSensorEventEnablesSet );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &SensorNum, Enables );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ControlTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
				    SAHPI_IN  SaHpiResourceIdT ResourceId,
				    SAHPI_IN  SaHpiCtrlNumT    CtrlNum,
				    SAHPI_OUT SaHpiCtrlTypeT  *Type )
{
  if ( Type == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiControlTypeGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &CtrlNum );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Type );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ControlStateGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT  SessionId,
				     SAHPI_IN    SaHpiResourceIdT ResourceId,
				     SAHPI_IN    SaHpiCtrlNumT    CtrlNum,
				     SAHPI_INOUT SaHpiCtrlStateT *CtrlState )
{
  if ( CtrlState == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiControlStateGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &CtrlNum );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       CtrlState );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ControlStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				     SAHPI_IN SaHpiResourceIdT ResourceId,
				     SAHPI_IN SaHpiCtrlNumT    CtrlNum,
				     SAHPI_IN SaHpiCtrlStateT *CtrlState )
{
  if ( CtrlState == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiControlStateSet );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &CtrlNum, CtrlState );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EntityInventoryDataRead)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
					     SAHPI_IN  SaHpiResourceIdT     ResourceId,
					     SAHPI_IN  SaHpiEirIdT          EirId,
					     SAHPI_IN  SaHpiUint32T         BufferSize,
					     SAHPI_OUT SaHpiInventoryDataT *InventData,
					     SAHPI_OUT SaHpiUint32T        *ActualSize )
{
  if ( ActualSize == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEntityInventoryDataRead );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &EirId, &BufferSize );

  Send();

  const cMarshalType *types[4];
  types[0] = &SaErrorType; // SA_OK
  types[1] = &SaHpiUint32Type;  // actual size
  types[2] = 0;

  void *params[3];
  params[0] = &err;
  params[1] = ActualSize;

  int mr = DemarshalArray( reply_header.m_flags & dMhEndianBit, types, params, reply );

  if ( mr < 0 )
     {
       if ( reply )
	    free( reply );

       return SA_ERR_HPI_INVALID_PARAMS;
     }

  if ( err == SA_OK && InventData )
     {
       types[2] = &SaHpiInventoryDataType;
       types[3] = 0;
       params[2] = InventData;

       mr = DemarshalArray( reply_header.m_flags & dMhEndianBit, types, params, reply );
     }

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
EntityInventoryDataWrite)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT      SessionId,
					      SAHPI_IN SaHpiResourceIdT     ResourceId,
					      SAHPI_IN SaHpiEirIdT          EirId,
					      SAHPI_IN SaHpiInventoryDataT *InventData )
{
  if ( InventData == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiEntityInventoryDataWrite );

  request_header.m_len = HpiMarshalRequest4( hm, request, &SessionId, &ResourceId, &EirId, InventData );

  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
WatchdogTimerGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT    SessionId,
				      SAHPI_IN  SaHpiResourceIdT   ResourceId,
				      SAHPI_IN  SaHpiWatchdogNumT  WatchdogNum,
				      SAHPI_OUT SaHpiWatchdogT    *Watchdog )
{
  if ( Watchdog == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerGet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Watchdog );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
WatchdogTimerSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
				      SAHPI_IN SaHpiResourceIdT  ResourceId,
				      SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
				      SAHPI_IN SaHpiWatchdogT   *Watchdog )
{
  if ( Watchdog == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       Watchdog );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
WatchdogTimerReset)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
					SAHPI_IN SaHpiResourceIdT  ResourceId,
					SAHPI_IN SaHpiWatchdogNumT WatchdogNum )
{
  CheckSession();

  PreMarshal( eFsaHpiWatchdogTimerReset );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &WatchdogNum );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
HotSwapControlRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
					   SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapControlRequest );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceActiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				       SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceActiveSet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceInactiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
					 SAHPI_IN SaHpiResourceIdT ResourceId )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceInactiveSet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
AutoInsertTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT SessionId,
					  SAHPI_OUT SaHpiTimeoutT    *Timeout )
{
  if ( Timeout == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiAutoInsertTimeoutGet );

  request_header.m_len = HpiMarshalRequest1( hm, request, &SessionId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, 
			       Timeout );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
AutoInsertTimeoutSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
					  SAHPI_IN SaHpiTimeoutT   Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoInsertTimeoutSet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &Timeout );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
AutoExtractTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
					   SAHPI_IN  SaHpiResourceIdT ResourceId,
					   SAHPI_OUT SaHpiTimeoutT   *Timeout )
{
  if ( Timeout == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiAutoExtractTimeoutGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err, Timeout );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
AutoExtractTimeoutSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
					   SAHPI_IN  SaHpiResourceIdT ResourceId,
					   SAHPI_IN  SaHpiTimeoutT    Timeout )
{
  CheckSession();

  PreMarshal( eFsaHpiAutoExtractTimeoutSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Timeout );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
HotSwapStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
				     SAHPI_IN  SaHpiResourceIdT ResourceId,
				     SAHPI_OUT SaHpiHsStateT   *State )
{
  if ( State == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiHotSwapStateGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
HotSwapActionRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
					  SAHPI_IN SaHpiResourceIdT ResourceId,
					  SAHPI_IN SaHpiHsActionT   Action )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapActionRequest );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Action );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourcePowerStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
					   SAHPI_IN  SaHpiResourceIdT    ResourceId,
					   SAHPI_OUT SaHpiHsPowerStateT *State )
{
  if ( State == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiResourcePowerStateGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourcePowerStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
					   SAHPI_IN SaHpiResourceIdT   ResourceId,
					   SAHPI_IN SaHpiHsPowerStateT State )
{
  CheckSession();

  PreMarshal( eFsaHpiResourcePowerStateSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &State );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
HotSwapIndicatorStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
					      SAHPI_IN  SaHpiResourceIdT        ResourceId,
					      SAHPI_OUT SaHpiHsIndicatorStateT *State )
{
  if ( State == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiHotSwapIndicatorStateGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );
  
  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       State );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
HotSwapIndicatorStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT        SessionId,
					      SAHPI_IN SaHpiResourceIdT       ResourceId,
					      SAHPI_IN SaHpiHsIndicatorStateT State )
{
  CheckSession();

  PreMarshal( eFsaHpiHotSwapIndicatorStateSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &State );

  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ParmControl)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
				 SAHPI_IN SaHpiResourceIdT ResourceId,
				 SAHPI_IN SaHpiParmActionT Action )
{
  CheckSession();

  PreMarshal( eFsaHpiParmControl );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &Action );

  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceResetStateGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT     SessionId,
					   SAHPI_IN SaHpiResourceIdT    ResourceId,
					   SAHPI_OUT SaHpiResetActionT *ResetAction )
{
  if ( ResetAction == 0 )
       return SA_ERR_HPI_INVALID_PARAMS;

  CheckSession();

  PreMarshal( eFsaHpiResourceResetStateGet );

  request_header.m_len = HpiMarshalRequest2( hm, request, &SessionId, &ResourceId );

  Send();

  int mr = HpiDemarshalReply1( reply_header.m_flags & dMhEndianBit, hm, reply, &err,
			       ResetAction );

  PostMarshal();

  return err;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(
ResourceResetStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
					   SAHPI_IN SaHpiResourceIdT  ResourceId,
					   SAHPI_IN SaHpiResetActionT ResetAction )
{
  CheckSession();

  PreMarshal( eFsaHpiResourceResetStateSet );

  request_header.m_len = HpiMarshalRequest3( hm, request, &SessionId, &ResourceId, &ResetAction );
  
  Send();

  int mr = HpiDemarshalReply0( reply_header.m_flags & dMhEndianBit, hm, reply, &err );

  PostMarshal();

  return err;
}
