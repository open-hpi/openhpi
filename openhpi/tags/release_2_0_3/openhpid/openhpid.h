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
#include <stdlib.h>


#ifndef dMarshalHpi_h
#include "marshal_hpi.h"
#endif

#ifndef dConnection_h
#include "connection.h"
#endif

#ifndef dArray_h
#include "array.h"
#endif


// check the connection in 10s interval
#define dPingTimeout 10000000000LL // saHpiTimeT
#define dMaxLostPings 10


class cConnection;

class cSession
{
  cConnection    *m_connection;
  SaHpiSessionIdT m_session_id;
  bool            m_event_get; // saHpiEventGet is currently running
  SaHpiTimeT      m_timeout;
  unsigned int    m_event_get_seq;

public:
  cSession( cConnection *con, SaHpiSessionIdT sid )
    : m_connection( con ), m_session_id( sid ), m_event_get( false )
  {
  }

  cConnection *Connection() const { return m_connection; }
  SaHpiSessionIdT SessionId() const { return m_session_id; }
  bool IsEventGet()  const      { return m_event_get; }
  void EventGet( bool v = true ) { m_event_get = v; }
  SaHpiTimeT &Timeout() { return m_timeout; }
  unsigned int &EventGetSeq() { return m_event_get_seq; }
};


class cConnection : public cArray<cSession>
{
  cServerConnection *m_con;
  SaHpiTimeT         m_ping_timer;
  int                m_outstanding_pings;

public:
  cConnection( cServerConnection *con );
  ~cConnection();

  void Reinit();

  cSession *FindSession( SaHpiSessionIdT sid );
  bool AddSession( SaHpiSessionIdT sid );
  bool RemSession( SaHpiSessionIdT sid );

  SaHpiTimeT &PingTimer() { return m_ping_timer; }
  int        &OutstandingPings() { return m_outstanding_pings; }

  int WriteMsg( cMessageHeader &header, void *data );

  void           GetIp( char *str );
  unsigned short GetPort();
};


// debug level
#define dDebugInit       1
#define dDebugConnection 2
#define dDebugFunction   4
#define dDebugEvent      8
#define dDebugPing      16


class cOpenHpiDaemon : public cArray<cConnection>
{
public:
  cOpenHpiDaemon();
  ~cOpenHpiDaemon();

  const char *m_progname;
  bool        m_daemon;
  int         m_debug;
  char        m_config[PATH_MAX];
  bool        m_interactive;

  // connection
  int         m_daemon_port;

  cServerSocket *m_server_socket;
  pollfd        *m_pollfd;

  void NewConnection( cServerConnection *c );
  void CloseConnection( cConnection *con );

  bool HandleInteractive();
  bool HandleData( cConnection *c, const cMessageHeader &header, const void *data );
  bool HandlePong( cConnection *c, const cMessageHeader &header );
  bool HandlePing( cConnection *c, const cMessageHeader &header );
  bool HandleOpen( cConnection *c, const cMessageHeader &header );
  bool HandleClose( cConnection *c, const cMessageHeader &header );

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
