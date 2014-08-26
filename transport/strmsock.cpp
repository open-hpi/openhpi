/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <list>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <glib.h>

#include <oh_error.h>

#include "strmsock.h"


/***************************************************************
 * Initialization/Finalization
 **************************************************************/
static void Initialize( void )__attribute__ ((constructor));

static void Initialize( void )
{
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup( MAKEWORD( 2, 2), &wsa_data );
#endif
}

static void Finalize( void )__attribute__ ((destructor));

static void Finalize( void )
{
#ifdef _WIN32
    WSACleanup();
#endif
}

/***************************************************************
 * Helper functions
 **************************************************************/
static uint32_t DecodeUint32( const uint8_t * bytes, int byte_order )
{
    uint32_t x;
    memcpy( &x, bytes, sizeof( x ) );
    return ( byte_order == G_BYTE_ORDER ) ? x : GUINT32_SWAP_LE_BE( x );
}

static void EncodeUint32( uint8_t * bytes, uint32_t x, int byte_order )
{
    uint32_t x2 = ( byte_order == G_BYTE_ORDER ) ? x : GUINT32_SWAP_LE_BE( x );
    memcpy( bytes, &x2, sizeof( x ) );
}
 
static void SelectAddresses( int ipvflags,
                             int hintflags,
                             const char * node,
                             uint16_t port,
                             std::list<struct addrinfo *>& selected )
{
    struct addrinfo hints;
    memset( &hints, 0, sizeof(hints) );
    hints.ai_flags    = hintflags;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char service[32];
    snprintf( service, sizeof(service), "%u", port );

    struct addrinfo * items;
    int cc = getaddrinfo( node, service, &hints, &items );
    if ( cc != 0 ) {
        CRIT( "getaddrinfo failed." );
        return;
    }

    for ( struct addrinfo * item = items; item != 0; ) {
        struct addrinfo * next = item->ai_next;
        item->ai_next = 0;
        if ( ( ipvflags & FlagIPv4 ) && ( item->ai_family == AF_INET ) ) {
            selected.push_back( item );
        } else if ( ( ipvflags & FlagIPv6 ) && ( item->ai_family == AF_INET6 ) ) {
            selected.push_back( item );
        } else {
            freeaddrinfo( item );
        }

        item = next;
    }
}


/***************************************************************
 * Base Stream Socket class
 **************************************************************/
cStreamSock::cStreamSock( SockFdT sockfd )
    : m_sockfd( sockfd )
{
    // empty
}

cStreamSock::~cStreamSock()
{
    Close();
}

bool cStreamSock::GetPeerAddress( SockAddrStorageT& storage ) const
{
    SockAddrLenT len = sizeof(storage);
    int cc = getpeername( m_sockfd,
                          reinterpret_cast<struct sockaddr *>( &storage ),
                          &len );
    return ( cc == 0 );
}

bool cStreamSock::Close()
{
    if ( m_sockfd == InvalidSockFd ) {
        return true;
    }

    int cc;

#ifdef _WIN32
    cc = shutdown( m_sockfd, SD_BOTH );
    cc = closesocket( m_sockfd );
#else
    cc = shutdown( m_sockfd, SHUT_RDWR );
    cc = close( m_sockfd );
#endif
    if ( cc != 0 ) {
        CRIT( "cannot close stream socket." );
        return false;
    }

    m_sockfd = InvalidSockFd;

    return true;
}

bool cStreamSock::ReadMsg( uint8_t& type,
                           uint32_t& id,
                           void * payload,
                           uint32_t& payload_len,
                           int& payload_byte_order )
{
    // Windows recv() takes char * so we need the workaround below.
    union {
        MessageHeader hdr;
        char rawhdr[sizeof(MessageHeader)];
    };

    char * dst = rawhdr;
    size_t got  = 0;
    size_t need = dMhSize;
    while ( got < need ) {
        ssize_t len = recv( m_sockfd, dst + got, need - got, 0 );
        if ( len < 0 ) {
            CRIT( "error while reading message in thread %p.", 
            g_thread_self() );
            return false;
        } else if ( len == 0 ) {
            //CRIT( "peer closed connection." );
            return false;
        }
        got += len;

        if ( ( got == need ) && ( dst == rawhdr ) ) {
            // we got header
            uint8_t ver = hdr[dMhOffFlags] >> 4;
            if ( ver != dMhRpcVersion ) {
                CRIT( "unsupported version 0x%x != 0x%x.",
                     ver,
                     dMhRpcVersion );
                return false;
            }
            type = hdr[dMhOffType];
            payload_byte_order = ( ( hdr[dMhOffFlags] & dMhEndianBit ) != 0 ) ?
                                 G_LITTLE_ENDIAN : G_BIG_ENDIAN;
            id = DecodeUint32( &hdr[dMhOffId], payload_byte_order );
            payload_len = DecodeUint32( &hdr[dMhOffLen], payload_byte_order );

            // now prepare to get payload
            dst  = reinterpret_cast<char *>(payload);
            got  = 0;
            need = payload_len;
        }
    }

    payload_len = got;

/*
    printf( "Transport: got message of %u bytes in buffer %p:\n",
            sizeof(MessageHeader) + got,
            payload );
    for ( size_t i = 0; i < dMhSize; ++i ) {
        union {
            char c;
            unsigned char uc;
        };
        c = rawhdr[i];
        printf( "%02x ", uc );
    }
    for ( size_t i = 0; i < got; ++i ) {
        union {
            char c;
            unsigned char uc;
        };
        c = (reinterpret_cast<char *>(payload))[i];
        printf( "%02x ", uc );
    }
    printf( "\n" );
*/

    return true;
}

bool cStreamSock::WriteMsg( uint8_t type,
                            uint32_t id,
                            const void * payload,
                            uint32_t payload_len )
{
    if ( ( payload_len > 0 ) && ( payload == 0 ) ) {
        return false;
    }
    if ( payload_len > dMaxPayloadLength ) {
        CRIT("message payload too large.");
        return false;
    }

    // Windows send() takes char * so we need the workaround below.
    union {
        MessageHeader hdr;
        char msg[dMaxMessageLength];
    };

    hdr[dMhOffType] = type;
    hdr[dMhOffFlags] = dMhRpcVersion << 4;
    if ( G_BYTE_ORDER == G_LITTLE_ENDIAN ) {
        hdr[dMhOffFlags] |= dMhEndianBit;
    }
    hdr[dMhOffReserved1] = 0;
    hdr[dMhOffReserved2] = 0;
    EncodeUint32( &hdr[dMhOffId], id, G_BYTE_ORDER );
    EncodeUint32( &hdr[dMhOffLen], payload_len, G_BYTE_ORDER );

    if ( payload ) {
        memcpy( &msg[dMhSize], payload, payload_len );
    }
    size_t msg_len = dMhSize + payload_len;

/*
    printf("Transport: sending message of %d bytes:\n", msg_len );
    for ( size_t i = 0; i < msg_len; ++i ) {
        union {
            char c;
            unsigned char uc;
        };
        c = msg[i];
        printf( "%02x ", uc );
    }
    printf( "\n" );
*/

    ssize_t cc = send( m_sockfd, &msg[0], msg_len, 0 );
    if ( cc != (ssize_t)msg_len ) {
        CRIT( "error while sending message." );
        return false;
    }

    return true;
}

cStreamSock::eWaitCc cStreamSock::Wait()
{
    fd_set fds;
    struct timeval tv;

    FD_ZERO( &fds );
    FD_SET( m_sockfd, &fds );
    tv.tv_sec  = 5; // TODO
    tv.tv_usec = 0;

#ifdef _WIN32
    int cc = select( 0, &fds, 0, 0, &tv );
#else
    int cc = select( m_sockfd + 1, &fds, 0, 0, &tv );
#endif
    if ( cc == 0 ) { // timeout
        return eWaitTimeout;
    } else if ( cc != 1 ) {
        // CRIT( "select failed" );
        return eWaitError;
    } else if ( FD_ISSET( m_sockfd, &fds ) == 0 ) {
        CRIT( "unexpected select behaviour" );
        return eWaitError;
    }

    return eWaitSuccess;
}

bool cStreamSock::CreateAttempt( const struct addrinfo * info, bool last_attempt )
{
    bool rc = Close();
    if ( !rc ) {
        return false;
    }

    SockFdT new_sock;
    new_sock = socket( info->ai_family, info->ai_socktype, info->ai_protocol );
    if ( new_sock == InvalidSockFd ) {
        if ( last_attempt ) {
#ifdef _WIN32
            bool noaif = ( WSAGetLastError() == WSAEAFNOSUPPORT );
#else
            bool noaif = ( errno == EAFNOSUPPORT );
#endif
            if ( noaif ) {
                const char * aif; 
                switch ( info->ai_family ) {
                    case AF_INET:
                        aif = "IPv4";
                        break;
                    case AF_INET6:
                        aif = "IPv6";
                        break;
                    default:
                        aif = "";
                }
                CRIT( "cannot create %s stream socket, address family is not supported on this platform", aif );
            } else {
                CRIT( "cannot create stream socket." );
            }
        }
        return false;
    }

    m_sockfd = new_sock;

    return true;
}


/***************************************************************
 * Client Stream Socket class
 **************************************************************/
cClientStreamSock::cClientStreamSock()
    : cStreamSock()
{
    // empty
}

cClientStreamSock::~cClientStreamSock()
{
    // empty
}

bool cClientStreamSock::Create( const char * host, uint16_t port )
{
    bool connected = false;
    struct addrinfo * info;
    std::list<struct addrinfo *> infos;

    SelectAddresses( FlagIPv4 | FlagIPv6, 0, host, port, infos );

    while ( !infos.empty() ) {
        info = *infos.begin();
        if ( !connected ) {
            connected = CreateAttempt( info, infos.size() == 1 );
        }
        freeaddrinfo( info );
        infos.pop_front();
    }

    return connected;
}

bool cClientStreamSock::EnableKeepAliveProbes( int keepalive_time,
                                               int keepalive_intvl,
                                               int keepalive_probes )
{
#ifdef __linux__
    int rc;
    int val;

    val = 1;
    rc = setsockopt( SockFd(), SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val) );
    if ( rc != 0 ) {
        CRIT( "failed to set SO_KEEPALIVE option." );
        return false;
    }
    val = keepalive_time;
    rc = setsockopt( SockFd(), SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val) );
    if ( rc != 0 ) {
        CRIT( "failed to set TCP_KEEPIDLE option." );
        return false;
    }
    val = keepalive_intvl;
    rc = setsockopt( SockFd(), SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val) );
    if ( rc != 0 ) {
        CRIT( "failed to set TCP_KEEPINTVL option." );
        return false;
    }
    val = keepalive_probes;
    rc = setsockopt( SockFd(), SOL_TCP, TCP_KEEPCNT, &val, sizeof(val) );
    if ( rc != 0 ) {
        CRIT( "failed to set TCP_KEEPCNT option." );
        return false;
    }

    return true;

#else

    WARN( "TCP Keep-Alive Probes are not supported." );

    return false;

#endif /* __linux__ */
}

bool cClientStreamSock::CreateAttempt( const struct addrinfo * info, bool last_attempt )
{
    bool rc = cStreamSock::CreateAttempt( info, last_attempt );
    if ( !rc ) {
        return false;
    }

    int cc = connect( SockFd(), info->ai_addr, info->ai_addrlen );
    if ( cc != 0 ) {
        Close();
        if ( last_attempt ) {
            CRIT( "connect failed." );
        }
        return false;
    }

    return true;
}


/***************************************************************
 * Server Stream Socket class
 **************************************************************/
cServerStreamSock::cServerStreamSock()
    : cStreamSock()
{
    // empty
}

cServerStreamSock::~cServerStreamSock()
{
    // empty
}

bool cServerStreamSock::Create( int ipvflags, const char * bindaddr, uint16_t port )
{
    bool bound = false;
    struct addrinfo * info;
    std::list<struct addrinfo *> infos;

    SelectAddresses( ipvflags, AI_PASSIVE, bindaddr, port, infos );

    while ( !infos.empty() ) {
        info = *infos.begin();
        if ( !bound ) {
            bound = CreateAttempt( info, infos.size() == 1 );
        }
        freeaddrinfo( info );
        infos.pop_front();
    }

    return bound;
}

bool cServerStreamSock::CreateAttempt( const struct addrinfo * info, bool last_attempt )
{
    bool rc = cStreamSock::CreateAttempt( info, last_attempt );
    if ( !rc ) {
        return false;
    }

    int cc;

    int val = 1;
#ifdef _WIN32
    cc = setsockopt( SockFd(),
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     (const char *)&val, sizeof(val) );
#else
    cc = setsockopt( SockFd(),
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     &val,
                     sizeof(val) );
#endif
    if ( cc != 0 ) {
        Close();
        if ( last_attempt ) {
            CRIT( "failed to set SO_REUSEADDR option." );
        }
        return false;
    }
    cc = bind( SockFd(), info->ai_addr, info->ai_addrlen );
    if ( cc != 0 ) {
        Close();
        if ( last_attempt ) {
            CRIT( "bind failed." );
        }
        return false;
    }
    cc = listen( SockFd(), 5 /* TODO */ );
    if ( cc != 0 ) {
        Close();
        if ( last_attempt ) {
            CRIT( "listen failed." );
        }
        return false;
    }

    return true;
}

cStreamSock * cServerStreamSock::Accept()
{
    SockFdT sock = accept( SockFd(), 0, 0 );
    if ( sock == InvalidSockFd ) {
        CRIT( "accept failed." );
        return 0;
    }

    return new cStreamSock( sock );
}

