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


class cConnection
{
  cServerConnection *m_con;

public:
  cConnection( cServerConnection *con )
    : m_con( con )
  {}

  ~cConnection()
  {
    if ( m_con )
	 ServerConnectionClose( m_con );
  }

  int Fd() { return m_con->m_fd; }
};


// debug level
#define dDebugInit       1
#define dDebugConnection 2
#define dDebugFunction   4


class cOpenHpiDaemon
{
public:
  cOpenHpiDaemon();
  ~cOpenHpiDaemon();

  const char *m_progname;
  bool        m_daemon;
  int         m_debug;

  // connection
  int         m_daemon_port;

  cServerConnectionMain *m_main_socket;

  cConnection **m_connections; // list of all connections
  int           m_num_connections;
  pollfd       *m_pollfd;

  void NewConnection( cServerConnection *c );
  void CloseConnection( int id );

  bool HandleData( cConnection *c );
  bool HandleMsg( const cMessageHeader &header, const void *data,
		  cMessageHeader &rh, void *&rd );

  void Idle();

  // openhpi
  SaHpiVersionT m_version;

  void Usage();
  bool ParseArgs( int argc, char *argv[] );
  bool Initialize();
  void Finalize();
  int MainLoop();

  void DbgInit( const char *fmt, ... );
  void DbgCon ( const char *fmt, ... );
  void DbgFunc( const char *fmt, ... );
};


#endif
