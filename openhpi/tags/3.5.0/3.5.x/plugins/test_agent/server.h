/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTSERVERLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef SERVER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define SERVER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <stdint.h>
#include <stddef.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>

#include <glib.h>

#include <SaHpi.h>

#include <oh_utils.h>


namespace TA {


/**************************************************************
 * Sockets
 *************************************************************/
#ifdef _WIN32
typedef SOCKET SockFdT;
const SockFdT InvalidSockFd = INVALID_SOCKET;
#else
typedef int SockFdT;
const SockFdT InvalidSockFd = -1;
#endif


/**************************************************************
 * class cServer
 *************************************************************/
class cServer
{
public:

    explicit cServer( unsigned short port );

    virtual ~cServer();

    bool Init();
    void Send( const char * data, size_t len ) const;

private:

    cServer( const cServer& );
    cServer& operator =( const cServer& );

private:

    virtual void WelcomeUser() const = 0;
    virtual void ProcessUserLine( const std::vector<char>& line,
                                  bool& quit ) = 0;

    void SetClientSocket( SockFdT csock );
    static gpointer ThreadProcAdapter( gpointer data );
    void ThreadProc();

private: // data

    uint16_t               m_port;
    bool                   m_initialized;
    volatile bool          m_stop;
    GThread *              m_thread;
    SockFdT                m_csock;
    mutable GStaticMutex   m_csock_lock;
};


}; // namespace TA


#endif // SERVER_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

