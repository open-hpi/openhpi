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

#ifndef dOpenhpiDaemon_h
#define dOpenhpiDaemon_h


#include <poll.h>


#ifndef dMarshalHpi_h
#include "marshal_hpi.h"
#endif

#ifndef dConnection_h
#include "connection.h"
#endif

#include <glib.h>

// check the connection in 2 minute interval
#define dPingTimeout 120000000000LL // saHpiTimeT


class cConnection;

class cSession
{
  cConnection    *m_connection;
  SaHpiSessionIdT m_session_id;
  bool            m_event_get; // saHpiEventGet is currently running
  SaHpiTimeT      m_timeout;
  unsigned int    m_seq;

public:
  cSession( cConnection *con, SaHpiSessionIdT sid )
    : m_connection( con ), m_session_id( sid ), m_event_get( false )
  {
  }

  cConnection *Connection() { return m_connection; }
  SaHpiSessionIdT SessionId() { return m_session_id; }
  bool IsEventGet()        { return m_event_get; }
  void EventGet( bool v = true ) { m_event_get = v; }
  SaHpiTimeT &Timeout() { return m_timeout; }
  unsigned int &Seq() { return m_seq; }
};


class cConnection
{
  cServerConnection *m_con;
  GList             *m_sessions; // sessions running over this connection
  SaHpiTimeT         m_ping_timer;
  int                m_outstanding_pings;

public:
  cConnection( cServerConnection *con );
  ~cConnection();

  cSession *FindSession( SaHpiSessionIdT sid );
  bool AddSession( SaHpiSessionIdT sid );
  bool RemSession( SaHpiSessionIdT sid );
  GList *Sessions() { return m_sessions; }
  int Fd() { return m_con->m_fd; }

  SaHpiTimeT &PingTimer() { return m_ping_timer; }
  int        &OutstandingPings() { return m_outstanding_pings; }
};


// debug level
#define dDebugInit       1
#define dDebugConnection 2
#define dDebugFunction   4
#define dDebugEvent      8
#define dDebugPing      16


class cOpenHpiDaemon
{
public:
  cOpenHpiDaemon();
  ~cOpenHpiDaemon();

  const char *m_progname;
  bool        m_daemon;
  int         m_debug;
  const char *m_config;

  // connection
  int         m_daemon_port;

  cServerConnectionMain *m_main_socket;

  cConnection **m_connections; // list of all connections
  int           m_num_connections;
  pollfd       *m_pollfd;

  void NewConnection( cServerConnection *c );
  void CloseConnection( int id );

  bool HandleData( cConnection *c );
  void HandlePong( cConnection *c, const cMessageHeader &header );

  enum tResult
  {
    eResultOk,
    eResultError,
    eResultReply
  };

  tResult HandleMsg( cConnection *c, const cMessageHeader &header, const void *data,
		     cMessageHeader &rh, void *&rd );

  void Idle();

  // openhpi
  SaHpiVersionT   m_version;
  SaHpiSessionIdT m_session;

  void Usage();
  bool ParseArgs( int argc, char *argv[] );
  bool Initialize();
  void Finalize();
  int MainLoop();

  void DbgInit( const char *fmt, ... );
  void DbgCon ( const char *fmt, ... );
  void DbgFunc( const char *fmt, ... );
  void DbgEvent( const char *fmt, ... );
  void DbgPing( const char *fmt, ... );
};


#endif
