/*
 * daemon code
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

#include "openhpid.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>


cOpenHpiDaemon::cOpenHpiDaemon()
  : m_progname( 0 ), m_daemon( false ), m_debug( 0 ),
    m_daemon_port( dDefaultDaemonPort ),
    m_main_socket( 0 ), m_connections( 0 ),
    m_num_connections( 0 ), m_pollfd( 0 )
{
}


cOpenHpiDaemon::~cOpenHpiDaemon()
{
}


void
cOpenHpiDaemon::DbgInit( const char *fmt, ... )
{
  va_list args;
  va_start( args, fmt );

  if ( m_debug & dDebugInit )
       vprintf( fmt, args );

  va_end( args );
}


void
cOpenHpiDaemon::DbgCon( const char *fmt, ... )
{
  va_list args;
  va_start( args, fmt );

  if ( m_debug & dDebugConnection )
       vprintf( fmt, args );

  va_end( args );
}


void
cOpenHpiDaemon::DbgFunc( const char *fmt, ... )
{
  va_list args;
  va_start( args, fmt );

  if ( m_debug & dDebugFunction )
       vprintf( fmt, args );

  va_end( args );
}


void
cOpenHpiDaemon::Usage()
{
  fprintf( stderr, "usage: %s [OPTION] ...\n", m_progname );
  fprintf( stderr, "\t-h --help         help\n" );
  fprintf( stderr, "\t-d --debug=level  debug level\n" );
  fprintf( stderr, "\t-n --nodaemon     do not became a daemon\n" );
  fprintf( stderr, "\t-p --port=port    daemon port\n" );
}


bool
cOpenHpiDaemon::ParseArgs( int argc, char *argv[] )
{
  m_progname = "openhpid";

  if ( argc > 0 )
       m_progname = argv[0];

  static struct option long_options[] = 
  {
    { "help"    , 0, 0, 'h' },
    { "debug"   , 1, 0, 'd' },
    { "nodaemon", 0, 0, 'n' },
    { 0, 0, 0, 0 }
  };

  bool help = false;

  while( true )
     {
       int option_index = 0;
       int c;

       c = getopt_long( argc, argv, "hd:np:",
                        long_options, &option_index );

       if ( c == -1 )
	    break;

       switch( c )
	  {
	    case 'n':
		 m_daemon = false;
		 break;

	    case 'd':
		 m_debug = atoi( optarg );
		 break;

	    case 'p':
		 m_daemon_port = atoi( optarg );
		 break;

	    case 'h':
		 help = true;
		 break;

	    default:
		 fprintf( stderr, "unknown option %s\n", 
			  argv[optind ? optind : 1] );

		 help = true;
		 break;
	  }
     }

  // check for additional arguments
  for( int i = optind; i < argc; i++ )
     {
       help = true;

       fprintf( stderr, "unknown option %s\n",
		argv[i] );
     }

  if ( help )
     {
       Usage();
       return false;
     }

  return true;
}


bool
cOpenHpiDaemon::Initialize()
{
  if ( m_daemon )
     {
       DbgInit( "become a daemon.\n" );

       pid_t pid = fork();

       if ( pid < 0 )
	  {
	    fprintf( stderr, "cannot fork: %s !\n",
		     strerror( errno ) );
	    return false;
	  }

       // parent process
       if ( pid != 0 )
	    exit( 0 );

       // become the session leader
       setsid();

       // second fork to become a real daemon
       pid = fork();

       if ( pid < 0 )
	  {
	    fprintf( stderr, "cannot fork: %s !\n",
		     strerror( errno ) );
	    return false;
	  }

       // parent process
       if ( pid != 0 )
	    exit( 0 );

       chdir( "/" );

       umask( 0 );
     }

  // initialize openhpi
  DbgInit( "initialize openhpi.\n" );
  SaErrorT rv = saHpiInitialize( &m_version );

  if ( rv != SA_OK )
     {
       fprintf( stderr, "cannot initialize openhpi: %d !\n", rv );
       return false;
     }

  // open daemon socket
  DbgInit( "create daemon connection port %d.\n", m_daemon_port );

  m_main_socket = ServerConnectionMainOpen( m_daemon_port );

  if ( !m_main_socket )
     {
       fprintf( stderr, "cannot create daemon socket: %d, %s !\n",
                errno, strerror( errno ) );

       saHpiFinalize();
       return false;
     }

  DbgInit( "daemon is up.\n" );

  return true;
}


void
cOpenHpiDaemon::Finalize()
{
  for( int i = 0; i < m_num_connections; i++ )
       delete m_connections[i];

  if ( m_connections )
       delete [] m_connections;

  if ( m_pollfd )
       delete [] m_pollfd;

  if ( m_main_socket )
     {
       ServerConnectionMainClose(  m_main_socket );
       m_main_socket = 0;
     }

  saHpiFinalize();
}


int
cOpenHpiDaemon::MainLoop()
{
  DbgCon( "ready for incomming connections.\n" );

  m_pollfd = new pollfd[1];
  m_pollfd[0].fd     = m_main_socket->m_fd;
  m_pollfd[0].events = POLLIN;
  m_pollfd[0].revents = 0;

  while( true )
     {
       int rv = poll( m_pollfd, m_num_connections + 1, 100000 );

       if ( rv < 0 )
	  {
	    usleep( 100000 );

	    if ( errno != EINTR )
		 fprintf( stderr, "can't poll: %s %d !\n",
			  strerror( errno ), errno );

	    // sleep a while
	    usleep( 100 );
	    continue;
	  }

       if ( rv == 0 )
	  {
	    Idle();
	    continue;
	  }

       for( int i = 0; i < m_num_connections; i++ )
	    if ( m_pollfd[i].revents )
		 if ( !HandleData( m_connections[i] ) )
		      CloseConnection( i );

       if ( m_pollfd[m_num_connections].revents )
	  {
	    // create new connection
	    cServerConnection *c = ServerConnectionMainAccept( m_main_socket );

	    if ( c )
		 NewConnection( c );
	  }
     }

  return 0;
}


void 
cOpenHpiDaemon::NewConnection( cServerConnection *c )
{
  DbgCon( "creating new connection.\n" );

  pollfd *pfd = new pollfd[m_num_connections + 2];
  cConnection **ca = new cConnection *[m_num_connections + 1];

  memcpy( pfd, m_pollfd, sizeof( pollfd ) * m_num_connections );
  memcpy( ca, m_connections, sizeof( cConnection * ) * m_num_connections );

  delete [] m_pollfd;
  delete [] m_connections;

  pfd[m_num_connections].fd       = c->m_fd;
  pfd[m_num_connections].events   = POLLIN;
  pfd[m_num_connections+1].fd     = m_main_socket->m_fd;
  pfd[m_num_connections+1].events = POLLIN;

  ca[m_num_connections] = new cConnection( c );

  m_pollfd = pfd;
  m_connections = ca;

  m_num_connections++;
}


void 
cOpenHpiDaemon::CloseConnection( int id )
{
  DbgCon( "closing connection.\n" );

  pollfd *pfd = new pollfd[m_num_connections];
  cConnection **ca = 0;

  if ( m_num_connections > 1 )
       ca = new cConnection *[m_num_connections - 1];

  int i;

  for( i = 0; i < id; i++ )
     {
       pfd[i] = m_pollfd[i];
       ca[i] = m_connections[i];
     }

  for( i = id + 1; i < m_num_connections; i++ )
     {
       pfd[i-1] = m_pollfd[i];
       ca[i-1] = m_connections[i];
     }

  pfd[m_num_connections-1] = m_pollfd[m_num_connections];

  delete [] m_pollfd;
  m_pollfd = pfd;

  delete [] m_connections;
  m_connections = ca;
  m_num_connections--;
}


void
cOpenHpiDaemon::Idle()
{
}


bool
cOpenHpiDaemon::HandleData( cConnection *c )
{
  cMessageHeader header;
  void *data = 0;

  int rv = ConnectionReadMsg( c->Fd(), &header, &data, 1000 );

  if ( rv )
     {
       if ( data )
	    free( data );

       fprintf( stderr, "cannot read message !\n" );

       return false;
     }

  cMessageHeader rh;
  void *rd = 0;

  bool r = HandleMsg( header, data, rh, rd );

  if ( data )
       free( data );

  if ( !r )
       return false;

  rv = ConnectionWriteMsg( c->Fd(), &rh, rd );

  if ( rd )
       free( rd );

  return !rv;
}


bool
cOpenHpiDaemon::HandleMsg( const cMessageHeader &header, const void *data,
		    cMessageHeader &rh, void *&rd )
{
  cHpiMarshal *hm = HpiMarshalFind( header.m_id );

  // check for function and data length
  if ( !hm || hm->m_request_len != header.m_len )
     {
       MessageHeaderInit( &rh, eMhError, header.m_seq, 0, 0 );
       rd = 0;

       fprintf( stderr, "wrong message length: id %d !\n", header.m_id );

       return true;
     }

  assert( hm->m_reply_len );

  // init reply header
  MessageHeaderInit( &rh, eMhReply, header.m_seq, header.m_id, hm->m_reply_len );

  // alloc reply buffer
  rd = calloc( 1, hm->m_reply_len );

  SaErrorT ret;

  switch( header.m_id )
     {
       case eFsaHpiSessionOpen:
	    {
	      SaHpiDomainIdT domain_id;
	      SaHpiSessionIdT session_id = 0;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data, (void *)&domain_id );

	      ret = saHpiSessionOpen( domain_id, &session_id, 0 );

	      DbgFunc( "saHpiSessionOpen( %x, %x ) = %d\n",
                       domain_id, session_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &session_id );
	    }
 
	    break;

       case eFsaHpiSessionClose:
	    {
	      SaHpiSessionIdT session_id;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data, &session_id );

	      ret = saHpiSessionClose( session_id );

	      DbgFunc( "saHpiSessionClose( %x ) = %d\n", session_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiResourcesDiscover:
	    {
	      SaHpiSessionIdT session_id;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data, &session_id );

	      ret = saHpiResourcesDiscover( session_id );

	      DbgFunc( "saHpiResourcesDiscover( %x ) = %d\n", session_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;
    
       case eFsaHpiRptInfoGet:
	    {
	      SaHpiSessionIdT session_id;
              SaHpiRptInfoT rpt_info;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data, &session_id );

	      ret = saHpiRptInfoGet( session_id, &rpt_info );

	      DbgFunc( "saHpiRptInfoGet( %x ) = %d\n", session_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &rpt_info );
	    }

	    break;

       case eFsaHpiRptEntryGet:
	    {
	      SaHpiSessionIdT session_id;
              SaHpiEntryIdT   entry_id;
              SaHpiEntryIdT   next_entry_id;
              SaHpiRptEntryT  rpt_entry;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
                                    &session_id, &entry_id );

	      ret = saHpiRptEntryGet( session_id, entry_id, &next_entry_id, &rpt_entry );

	      DbgFunc( "saHpiRptEntryGet( %x, %x, %x ) = %d\n",
                       session_id, entry_id, next_entry_id, ret );

	      rh.m_len = HpiMarshalReply2( hm, rd, &ret, &next_entry_id, &rpt_entry );
	    }

	    break;

       case eFsaHpiRptEntryGetByResourceId:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiRptEntryT   rpt_entry;
	      
	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiRptEntryGetByResourceId( session_id, resource_id, &rpt_entry );

	      DbgFunc( "saHpiRptEntryGetByResourceId( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &rpt_entry );
	    }

	    break;

       case eFsaHpiResourceSeveritySet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSeverityT   severity;
	      
	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &severity );

	      ret = saHpiResourceSeveritySet( session_id,
					      resource_id, severity );
	      
	      DbgFunc( "saHpiResourceSeveritySet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiResourceTagSet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiTextBufferT resource_tag;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &resource_tag );

	      ret = saHpiResourceTagSet( session_id, resource_id,
					 &resource_tag );

	      DbgFunc( "saHpiResourceTagSet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiResourceIdGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id = 0;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data,
				    &session_id );

	      ret = saHpiResourceIdGet( session_id, &resource_id );

	      DbgFunc( "saHpiResourceIdGet( %x ) = %d, %x\n",
                       session_id, ret, resource_id );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &resource_id );
	    }

	    break;

       case eFsaHpiEntitySchemaGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiUint32T    schema_id = 0;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data,
				   &session_id );

	      ret = saHpiEntitySchemaGet( session_id, &schema_id );
	      
	      DbgFunc( "saHpiEntitySchemaGet( %x ) = %d, %x\n",
                       session_id, ret, schema_id );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &schema_id );
	    }
	    
	    break;

       case eFsaHpiEventLogInfoGet:
 	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSelInfoT    info;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id );

	      ret = saHpiEventLogInfoGet( session_id, resource_id, &info );

	      DbgFunc( "saHpiEventLogInfoGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &info );
	    }

	    break;

       case eFsaHpiEventLogEntryGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSelEntryIdT entry_id;
	      SaHpiSelEntryIdT prev_entry_id = 0;
	      SaHpiSelEntryIdT next_entry_id = 0;
	      SaHpiSelEntryT   event_log_entry;
	      SaHpiRdrT        rdr;
	      SaHpiRptEntryT   rpt_entry;

	      memset( &rdr, 0, sizeof( SaHpiRdrT ) );
	      memset( &rpt_entry, 0, sizeof( SaHpiRptEntryT ) );

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &entry_id );

	      ret = saHpiEventLogEntryGet( session_id, resource_id, entry_id,
					   &prev_entry_id, &next_entry_id,
					   &event_log_entry, &rdr, &rpt_entry );

	      DbgFunc( "saHpiEventLogEntryGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, entry_id, ret );

	      rh.m_len = HpiMarshalReply5( hm, rd, &ret, &prev_entry_id, &next_entry_id,
				&event_log_entry, &rdr, &rpt_entry );
	    }

	    break;

       case eFsaHpiEventLogEntryAdd:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSelEntryT   evt_entry;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &evt_entry );

	      ret = saHpiEventLogEntryAdd( session_id, resource_id,
					   &evt_entry );
	      
	      DbgFunc( "saHpiEventLogEntryAdd( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiEventLogEntryDelete:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSelEntryIdT entry_id;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &entry_id );

	      ret = saHpiEventLogEntryDelete( session_id, resource_id, entry_id );

	      DbgFunc( "saHpiEventLogEntryDelete( %x, %x, %x ) = %d\n",
                       session_id, resource_id, entry_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiEventLogClear:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiEventLogClear( session_id, resource_id );

	      DbgFunc( "saHpiEventLogClear( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiEventLogTimeGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiTimeT       ti;
	      
	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id );

	      ret = saHpiEventLogTimeGet( session_id, resource_id, &ti );

	      DbgFunc( "saHpiEventLogTimeGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &ti );
	    }

	    break;

       case eFsaHpiEventLogTimeSet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiTimeT       ti;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id , &ti );

	      ret = saHpiEventLogTimeSet( session_id, resource_id, ti );
 
	      DbgFunc( "saHpiEventLogTimeSet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiEventLogStateGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiBoolT       enable;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiEventLogStateGet( session_id, resource_id, &enable );

	      DbgFunc( "saHpiEventLogStateGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &enable );
	    }

	    break;

       case eFsaHpiEventLogStateSet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiBoolT       enable;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &enable );

	      ret = saHpiEventLogStateSet( session_id, resource_id, enable );
	      
	      DbgFunc( "saHpiEventLogStateSet( %x, %x, %s ) = %d\n",
                       session_id, resource_id, enable ? "true" : "false", ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiSubscribe:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiBoolT      provide_active_alarms;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &provide_active_alarms );

	      ret = saHpiSubscribe( session_id, provide_active_alarms );
	      
	      DbgFunc( "saHpiSubscribe( %x, %s ) = %d\n",
                       session_id,  provide_active_alarms ? "true" : "false",
		       ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiUnsubscribe:
	    {
	      SaHpiSessionIdT session_id;

	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data,
				    &session_id );

	      ret = saHpiUnsubscribe( session_id );
	      
	      DbgFunc( "saHpiUnsubscribe( %x ) = %d\n",
                       session_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiEventGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiTimeoutT   timeout;
	      SaHpiEventT     event;
	      SaHpiRdrT       rdr;
	      SaHpiRptEntryT  rpt_entry;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &timeout );

	      ret = saHpiEventGet( session_id, timeout, &event, &rdr, &rpt_entry );

	      DbgFunc( "saHpiEventGet( %x ) = %d\n",
                       session_id, ret );

	      rh.m_len = HpiMarshalReply3( hm, rd, &ret, &event, &rdr, &rpt_entry );
	    }

	    break;

       case eFsaHpiRdrGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiEntryIdT    entry_id;
	      SaHpiEntryIdT    next_entry_id;
	      SaHpiRdrT        rdr;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &entry_id );

	      ret = saHpiRdrGet( session_id, resource_id, entry_id,
				 &next_entry_id, &rdr );

	      DbgFunc( "saHpiRdrGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, entry_id, ret );

	      rh.m_len = HpiMarshalReply2( hm, rd, &ret, &next_entry_id, &rdr );
	    }

	    break;

       case eFsaHpiSensorReadingGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorReadingT reading;
	      
	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &sensor_num );

	      ret = saHpiSensorReadingGet( session_id, resource_id,
					   sensor_num, &reading );
	      
	      DbgFunc( "saHpiSensorReadingGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &reading );
	    }

	    break;

       case eFsaHpiSensorReadingConvert:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorReadingT reading_input;
	      SaHpiSensorReadingT converted_reading;

	      HpiDemarshalRequest4( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &sensor_num,
				    &reading_input );

	      ret = saHpiSensorReadingConvert( session_id,
					       resource_id, sensor_num,
					       &reading_input,
					       &converted_reading );

	      DbgFunc( "saHpiSensorReadingConvert( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &converted_reading );
	    }

	    break;

       case eFsaHpiSensorThresholdsGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorThresholdsT sensor_thresholds;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &sensor_num );

	      ret = saHpiSensorThresholdsGet( session_id,
					      resource_id, sensor_num,
					      &sensor_thresholds);

	      DbgFunc( "saHpiSensorThresholdsGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num,  ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &sensor_thresholds );
	    }

	    break;

       case eFsaHpiSensorThresholdsSet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorThresholdsT sensor_thresholds;

	      HpiDemarshalRequest4( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &sensor_num,
				   &sensor_thresholds );

	      ret = saHpiSensorThresholdsSet( session_id, resource_id,
					      sensor_num,
					      &sensor_thresholds );

	      DbgFunc( "saHpiSensorThresholdsSet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiSensorTypeGet:
	    {
	      SaHpiResourceIdT resource_id;
	      SaHpiSessionIdT  session_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorTypeT type;
	      SaHpiEventCategoryT category;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &sensor_num );

	      ret = saHpiSensorTypeGet( session_id, resource_id,
					sensor_num, &type, &category );

	      DbgFunc( "saHpiSensorTypeGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply2( hm, rd, &ret, &type, &category );
	    }

	    break;

       case eFsaHpiSensorEventEnablesGet:
	    {
	      SaHpiSessionIdT        session_id;
	      SaHpiResourceIdT       resource_id;
	      SaHpiSensorNumT        sensor_num;
	      SaHpiSensorEvtEnablesT enables;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &sensor_num );

	      ret = saHpiSensorEventEnablesGet( session_id, resource_id,
						sensor_num, &enables );

	      DbgFunc( "saHpiSensorEventEnablesGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &enables );
	    }

	    break;

       case eFsaHpiSensorEventEnablesSet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiSensorNumT  sensor_num;
	      SaHpiSensorEvtEnablesT enables;

	      HpiDemarshalRequest4( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &sensor_num,
				    &enables );

	      ret = saHpiSensorEventEnablesSet( session_id, resource_id,
						sensor_num, &enables );
	      
	      DbgFunc( "saHpiSensorEventEnablesSet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, sensor_num, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiControlTypeGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiCtrlNumT    ctrl_num;
	      SaHpiCtrlTypeT   type;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &ctrl_num );

	      ret = saHpiControlTypeGet( session_id, resource_id, ctrl_num,
					 &type );

	      DbgFunc( "saHpiControlTypeGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, ctrl_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &type );
	    }

	    break;

       case eFsaHpiControlStateGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiCtrlNumT    ctrl_num;
	      SaHpiCtrlStateT  ctrl_state;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &ctrl_num );

	      ret = saHpiControlStateGet( session_id, resource_id,
					  ctrl_num, &ctrl_state );

	      DbgFunc( "saHpiControlStateGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, ctrl_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &ctrl_state );
	    }

	    break;

       case eFsaHpiControlStateSet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiCtrlNumT    ctrl_num;
	      SaHpiCtrlStateT  ctrl_state;

	      HpiDemarshalRequest4( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &ctrl_num,
				    &ctrl_state );

	      ret = saHpiControlStateSet( session_id, resource_id,
					  ctrl_num, &ctrl_state );

	      DbgFunc( "saHpiControlStateSet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, ctrl_num, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiWatchdogTimerGet:
	    {
	      SaHpiSessionIdT   session_id;
	      SaHpiResourceIdT  resource_id;
	      SaHpiWatchdogNumT watchdog_num;
	      SaHpiWatchdogT    watchdog;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &watchdog_num );

	      ret = saHpiWatchdogTimerGet( session_id, resource_id,
					   watchdog_num, &watchdog );

	      DbgFunc( "saHpiWatchdogTimerGet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, watchdog_num, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &watchdog );
	    }

	    break;

       case eFsaHpiWatchdogTimerSet:
	    {
	      SaHpiSessionIdT   session_id;
	      SaHpiResourceIdT  resource_id;
	      SaHpiWatchdogNumT watchdog_num;
	      SaHpiWatchdogT    watchdog;

	      HpiDemarshalRequest4( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &watchdog_num,
				    &watchdog );

	      ret = saHpiWatchdogTimerSet( session_id, resource_id,
					   watchdog_num, &watchdog );
	      
	      DbgFunc( "saHpiWatchdogTimerSet( %x, %x, %x ) = %d\n",
                       session_id, resource_id, watchdog_num, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiWatchdogTimerReset:
	    {
	      SaHpiSessionIdT   session_id;
	      SaHpiResourceIdT  resource_id;
	      SaHpiWatchdogNumT watchdog_num;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &watchdog_num );

	      ret = saHpiWatchdogTimerReset( session_id, resource_id,
					     watchdog_num );

	      DbgFunc( "eFsaHpiWatchdogTimerReset( %x, %x, %x ) = %d\n",
                       session_id, resource_id, watchdog_num, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiHotSwapControlRequest:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiHotSwapControlRequest( session_id, resource_id );

	      DbgFunc( "saHpiHotSwapControlRequest( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiResourceActiveSet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiResourceActiveSet( session_id, resource_id );
	      
	      DbgFunc( "saHpiResourceActiveSet( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiResourceInactiveSet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id );

	      ret = saHpiResourceInactiveSet( session_id, resource_id );

	      DbgFunc( "saHpiResourceInactiveSet( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiAutoInsertTimeoutGet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiTimeoutT   timeout;
	      
	      HpiDemarshalRequest1( header.m_flags & dMhEndianBit, hm, data,
				   &session_id );

	      ret = saHpiAutoInsertTimeoutGet( session_id, &timeout );
	      
	      DbgFunc( "saHpiAutoInsertTimeoutGet( %x ) = %d\n",
                       session_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &timeout );
	    }

	    break;

       case eFsaHpiAutoInsertTimeoutSet:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiTimeoutT   timeout;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &timeout );

	      ret = saHpiAutoInsertTimeoutSet( session_id, timeout );
	      
	      DbgFunc( "saHpiAutoInsertTimeoutSet( %x ) = %d\n",
                       session_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiAutoExtractTimeoutGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiTimeoutT    timeout;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id );

	      ret = saHpiAutoExtractTimeoutGet( session_id, resource_id, &timeout );

	      DbgFunc( "saHpiAutoExtractTimeoutGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &timeout );
	    }

	    break;

       case eFsaHpiAutoExtractTimeoutSet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiTimeoutT    timeout;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &timeout );

	      ret = saHpiAutoExtractTimeoutSet( session_id, resource_id, timeout );

	      DbgFunc( "saHpiAutoExtractTimeoutSet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiHotSwapStateGet:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiHsStateT    state;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiHotSwapStateGet( session_id, resource_id, &state );

	      DbgFunc( "saHpiHotSwapStateGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &state );
	    }
	    
	    break;

       case eFsaHpiHotSwapActionRequest:
	    {
	      SaHpiSessionIdT session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiHsActionT   action;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &action );

	      ret = saHpiHotSwapActionRequest( session_id, resource_id, action );

	      DbgFunc( "saHpiHotSwapActionRequest( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiResourcePowerStateGet:
	    {
	      SaHpiSessionIdT    session_id;
	      SaHpiResourceIdT   resource_id;
	      SaHpiHsPowerStateT state;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiResourcePowerStateGet( session_id, resource_id, &state );
	      
	      DbgFunc( "saHpiResourcePowerStateGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &state );
	    }
	    
	    break;

       case eFsaHpiResourcePowerStateSet:
	    {
	      SaHpiSessionIdT    session_id;
	      SaHpiResourceIdT   resource_id;
	      SaHpiHsPowerStateT state;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id, &state  );

	      ret = saHpiResourcePowerStateSet( session_id, resource_id, state );

	      DbgFunc( "(saHpiResourcePowerStateSet %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       case eFsaHpiHotSwapIndicatorStateGet:
	    {
	      SaHpiSessionIdT        session_id;
	      SaHpiResourceIdT       resource_id;
	      SaHpiHsIndicatorStateT state;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				   &session_id, &resource_id );

	      ret = saHpiHotSwapIndicatorStateGet( session_id, resource_id, &state );
	      
	      DbgFunc( "saHpiHotSwapIndicatorStateGet( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &state );
	    }
	    
	    break;

       case eFsaHpiHotSwapIndicatorStateSet:
	    {
	      SaHpiSessionIdT        session_id;
	      SaHpiResourceIdT       resource_id;
	      SaHpiHsIndicatorStateT state;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &state );

	      ret = saHpiHotSwapIndicatorStateSet( session_id, resource_id, state );

	      DbgFunc( "saHpiHotSwapIndicatorStateSet( %x, %x ) = %d\n",
                       session_id, resource_id , ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }
	    
	    break;

       case eFsaHpiParmControl:
	    {
	      SaHpiSessionIdT  session_id;
	      SaHpiResourceIdT resource_id;
	      SaHpiParmActionT action;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &action );

	      ret = saHpiParmControl( session_id, resource_id, action );

	      DbgFunc( "saHpiParmControl( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret  );
	    }

	    break;

       case eFsaHpiResourceResetStateGet:
	    {
	      SaHpiSessionIdT   session_id;
	      SaHpiResourceIdT  resource_id;
	      SaHpiResetActionT reset_action;

	      HpiDemarshalRequest2( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id );

	      ret = saHpiResourceResetStateGet( session_id, resource_id,
						&reset_action );

	      DbgFunc( "saHpiResourceResetStateGet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply1( hm, rd, &ret, &reset_action );
	    }

	    break;

       case eFsaHpiResourceResetStateSet:
	    {
	      SaHpiSessionIdT   session_id;
	      SaHpiResourceIdT  resource_id;
	      SaHpiResetActionT reset_action;

	      HpiDemarshalRequest3( header.m_flags & dMhEndianBit, hm, data,
				    &session_id, &resource_id, &reset_action );

	      ret = saHpiResourceResetStateSet( session_id, resource_id, reset_action );

	      DbgFunc( "saHpiResourceResetStateSet( %x, %x ) = %d\n",
                       session_id, resource_id, ret );

	      rh.m_len = HpiMarshalReply0( hm, rd, &ret );
	    }

	    break;

       default:
	    assert( 0 );
	    break;
     }

  assert( rh.m_len <= hm->m_reply_len );

  return true;
}


int
main( int argc, char *argv[] )
{
  int rv = 1;

  cOpenHpiDaemon *s = new cOpenHpiDaemon;

  if ( s->ParseArgs( argc, argv ) )
     {
       if ( s->Initialize() )
	  {
	    rv = s->MainLoop();
	    s->Finalize();
	  }
     }

  delete s;

  return rv;
}
