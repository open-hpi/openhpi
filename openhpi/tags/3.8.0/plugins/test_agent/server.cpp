/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <vector>

#include <glib.h>

#include <SaHpi.h>

#include <oHpi.h>
#include <oh_error.h>
#include <oh_utils.h>

#include "server.h"

#include "sahpi_wrappers.h"


namespace TA {


/**************************************************************
 * Helpers
 *************************************************************/
static void CloseSocket( SockFdT sock )
{
    if ( sock != InvalidSockFd ) {
        int cc;
#ifdef _WIN32
        cc = shutdown( sock, SD_BOTH );
        cc = closesocket( sock );
#else
        cc = shutdown( sock, SHUT_RDWR );
        cc = close( sock );
#endif
        if ( cc != 0 ) {
            CRIT( "cannot close socket." );
        }
    }
}

static SockFdT CreateServerSocket( uint16_t port )
{
    SockFdT s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( s == InvalidSockFd ) {
        CRIT( "cannot create server ocket." );
        return InvalidSockFd;
    }

    int cc;

    int v = 1;
#ifdef _WIN32
    cc = setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)&v, sizeof(v) );
#else
    cc = setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v) );
#endif
    if ( cc != 0 ) {
        CRIT( "failed to set SO_REUSEADDR option." );
        CloseSocket( s );
        return InvalidSockFd;
    }

    struct sockaddr_in sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );
    sa.sin_addr.s_addr = htonl( INADDR_ANY );
    cc = bind( s, reinterpret_cast<const struct sockaddr*>(&sa), sizeof(sa) );
    if ( cc != 0 ) {
        CRIT( "bind failed." );
        CloseSocket( s );
        return InvalidSockFd;
    }

    cc = listen( s, 1 /* TODO */ );
    if ( cc != 0 ) {
        CRIT( "listen failed." );
        CloseSocket( s );
        return InvalidSockFd;
    }

    return s;
}

enum eWaitCc
{
    eWaitSuccess,
    eWaitTimeout,
    eWaitError,
};

static eWaitCc WaitOnSocket( SockFdT s )
{
    fd_set fds;
    struct timeval tv;

    FD_ZERO( &fds );
    FD_SET( s, &fds );
    tv.tv_sec  = 3; // TODO
    tv.tv_usec = 0;

#ifdef _WIN32
    int cc = select( 0, &fds, 0, 0, &tv );
#else
    int cc = select( s + 1, &fds, 0, 0, &tv );
#endif
    if ( cc == 0 ) { // timeout
        return eWaitTimeout;
    } else if ( cc != 1 ) {
        CRIT( "select failed" );
        return eWaitError;
    } else if ( FD_ISSET( s, &fds ) == 0 ) {
        CRIT( "unexpected select behaviour" );
        return eWaitError;
    }

    return eWaitSuccess;
}


/**************************************************************
 * class cServer
 *************************************************************/
cServer::cServer( unsigned short port )

    : m_port( port ),
      m_initialized( false ),
      m_stop( false ),
      m_thread( 0 ),
      m_csock( InvalidSockFd )
{
    wrap_g_static_mutex_init( &m_csock_lock );
}

cServer::~cServer()
{
    if ( m_thread ) {
        m_stop = true;
        g_thread_join( m_thread );
    }

    wrap_g_static_mutex_free_clear( &m_csock_lock );
}

bool cServer::Init()
{
    if ( m_initialized ) {
        return true;
    }

    if ( g_thread_supported() == FALSE ) {
        wrap_g_thread_init( 0 );
    }

    m_thread = wrap_g_thread_create_new( "Init", cServer::ThreadProcAdapter, this, TRUE, 0 );
    if ( m_thread == 0  ) {
        CRIT( "cannot start thread" );
        return false;
    }

    m_initialized = true;

    return true;
}

void cServer::Send( const char * data, size_t len ) const
{
    wrap_g_static_mutex_lock( &m_csock_lock );
    if ( ( data != 0 ) && ( m_csock != InvalidSockFd ) ) {
        send( m_csock, data, len, 0 );
    }
    wrap_g_static_mutex_unlock( &m_csock_lock );
}

gpointer cServer::ThreadProcAdapter( gpointer data )
{
    cServer * me = reinterpret_cast<cServer*>(data);
    me->ThreadProc();

    return 0;
}

void cServer::SetClientSocket( SockFdT csock )
{
    wrap_g_static_mutex_lock( &m_csock_lock );
    m_csock = csock;
    wrap_g_static_mutex_unlock( &m_csock_lock );
}

void cServer::ThreadProc()
{
    SockFdT ssock = CreateServerSocket( m_port );
    if ( ssock == InvalidSockFd ) {
        CRIT( "cannot create server socket." );
        return;
    }

    while ( !m_stop ) {
        eWaitCc wcc;
        wcc = WaitOnSocket( ssock );
        if ( wcc == eWaitTimeout ) {
            continue;
        } else if ( wcc == eWaitError ) {
            break;
        }
        SockFdT csock = accept( ssock, 0, 0 );
        if ( csock == InvalidSockFd ) {
            CRIT( "accept failed." );
            break;
        }

        SetClientSocket( csock );
        WelcomeUser();

        std::vector<char> line;

        while ( !m_stop ) {
            wcc = WaitOnSocket( csock );
            if ( wcc == eWaitTimeout ) {
                continue;
            } else if ( wcc == eWaitError ) {
                break;
            }
            char buf[4096];
            int cc = recv( csock, &buf[0], sizeof(buf), 0 );
            if ( cc <= 0 ) {
                break;
            }
            size_t got = cc;
            bool quit = false;
            for ( size_t i = 0; ( i < got ) && ( !quit ); ++i ) {
                if ( buf[i] == '\n' ) {
                    ProcessUserLine( line, quit );
                    line.clear();
                } else {
                    line.push_back( buf[i] );
                }
            }
            if ( quit ) {
                break;
            }
        }

        SetClientSocket( InvalidSockFd );
        CloseSocket( csock );
    }

    CloseSocket( ssock );
}


}; // namespace TA

