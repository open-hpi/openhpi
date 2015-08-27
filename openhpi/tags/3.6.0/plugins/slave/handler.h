/*      -*- linux-c++ -*-
 *
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
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef __HANDLER_H__
#define __HANDLER_H__

#include <queue>
#include <string>

#include <glib.h>

#include <SaHpi.h>

#include <oh_utils.h>

#include "baselib.h"
#include "lock.h"
#include "resourcemap.h"


namespace Slave {


/**************************************************************
 * class cHandler
 *************************************************************/
class cHandler : public cBaseLib, public cResourceMap
{
public:

    explicit cHandler( unsigned int id,
                       const SaHpiEntityPathT& root,
                       const std::string& host,
                       unsigned short port,
                       oh_evt_queue& eventq );

    ~cHandler();

    bool Init();

    bool WaitForDiscovery();

    const SaHpiEntityPathT& GetRoot() const
    {
        return m_root;
    }

    SaHpiSessionIdT GetSessionId() const
    {
        return m_sid;
    }

private:

    cHandler( const cHandler& );
    cHandler& operator =( const cHandler& );

    SaHpiResourceIdT GetOrCreateMaster( const SaHpiRptEntryT& slave_rpte );

    bool StartThread();
    static gpointer ThreadProcAdapter( gpointer data );
    void ThreadProc();

    bool OpenSession();

    bool CloseSession();

    bool Discover();

    void RemoveAllResources();

    bool ReceiveEvent( struct oh_event *& e );

    void HandleEvent( struct oh_event * e );

    SaHpiUint32T GetRptUpdateCounter() const;
    SaHpiUint32T GetRdrUpdateCounter( SaHpiResourceIdT slave_rid ) const;

    bool FetchRptAndRdrs( std::queue<struct oh_event *>& events ) const;
    bool FetchRdrs( struct oh_event * e ) const;

    void CompleteAndPostEvent( struct oh_event * e,
                               SaHpiResourceIdT master_rid,
                               bool set_timestamp );

    void CompleteAndPostResourceUpdateEvent( struct oh_event * e,
                                             SaHpiResourceIdT master_rid );


    // constants
    static const SaHpiSessionIdT InvalidSessionId             = 0xFFFFFFFF;
    static const SaHpiTimeoutT   DiscoveryStatusCheckInterval = 500000000ULL;
    static const SaHpiTimeoutT   OpenSessionRetryInterval     = 5000000000ULL;
    static const unsigned int    MaxFetchAttempts             = 42;
    static const SaHpiTimeoutT   GetEventTimeout              = 5000000000ULL;


    enum eStartupDiscoveryStatus
    {
        StartupDiscoveryUncompleted,
        StartupDiscoveryFailed,
        StartupDiscoveryDone,
    };


    // data
    unsigned int                     m_id;
    SaHpiEntityPathT                 m_root;
    SaHpiTextBufferT                 m_host;
    unsigned short                   m_port;
    SaHpiDomainIdT                   m_did;
    volatile SaHpiSessionIdT         m_sid;
    oh_evt_queue&                    m_eventq;
    volatile bool                    m_stop;
    GThread *                        m_thread;
    volatile eStartupDiscoveryStatus m_startup_discovery_status;
};


}; // namespace Slave


#endif // __HANDLER_H__

