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

#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <queue>
#include <string>

#include <glib.h>

#include <SaHpi.h>

#include <oHpi.h>
#include <oh_error.h>
#include <oh_utils.h>

#include "handler.h"
#include "util.h"

#include "sahpi_wrappers.h"


namespace Slave {


/**************************************************************
 * class cHandler
 *************************************************************/
cHandler::cHandler( unsigned int id,
                    const SaHpiEntityPathT& root,
                    const std::string& host,
                    unsigned short port,
                    oh_evt_queue& eventq )

    : m_id( id ),
      m_root( root ),
      m_port( port ),
      m_did( SAHPI_UNSPECIFIED_DOMAIN_ID ),
      m_sid( InvalidSessionId ),
      m_eventq( eventq ),
      m_stop( false ),
      m_thread( 0 ),
      m_startup_discovery_status( StartupDiscoveryUncompleted )
{
    m_host.DataType = SAHPI_TL_TYPE_TEXT;
    m_host.Language = SAHPI_LANG_UNDEF;
    m_host.DataLength = std::min<size_t>( host.size(), SAHPI_MAX_TEXT_BUFFER_LENGTH );
    memcpy( &m_host.Data[0], host.c_str(), m_host.DataLength );
}

cHandler::~cHandler()
{
    if ( m_thread ) {
        m_stop = true;
        g_thread_join( m_thread );
    }

    CloseSession();
    RemoveAllResources();
}

bool cHandler::Init()
{
    if ( g_thread_supported() == FALSE ) {
        wrap_g_thread_init( 0 );
    }

    bool rc;

    rc = LoadBaseLib();
    if ( !rc ) {
        return false;
    }

    SaHpiEntityPathT root;
    oh_init_ep( &root );

    SaHpiDomainIdT did;
    SaErrorT rv = Abi()->oHpiDomainAdd( &m_host, m_port, &root, &did );
    if ( rv != SA_OK ) {
        CRIT( "oHpiDomainAdd failed with rv = %d", rv );
        return false;
    }
    m_did = did;
    DBG( "Domain %u is created", m_did );

    rc = StartThread();
    if ( !rc ) {
        CRIT( "cannot start thread" );
        return false;
    }

    return true;
}

bool cHandler::WaitForDiscovery()
{
    while ( m_startup_discovery_status == StartupDiscoveryUncompleted ) {
        g_usleep( (gulong)( DiscoveryStatusCheckInterval / 1000ULL ) );
    }

    return ( m_startup_discovery_status == StartupDiscoveryDone );
}

SaHpiResourceIdT cHandler::GetOrCreateMaster( const SaHpiRptEntryT& slave_rpte )
{
    if ( !IsRptEntryValid( slave_rpte ) ) {
        return SAHPI_UNSPECIFIED_RESOURCE_ID;
    }

    SaHpiResourceIdT slave_rid = slave_rpte.ResourceId;
    SaHpiResourceIdT master_rid = GetMaster( slave_rid );

    // New Entry?
    if ( slave_rid != SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        if ( master_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
            if ( IsEntityPathValid( slave_rpte.ResourceEntity ) ) {
                // Yes, New Entry!
                SaHpiEntityPathT ep = slave_rpte.ResourceEntity;
                TranslateEntityPath( ep, m_root );
                master_rid = oh_uid_from_entity_path( &ep );
                AddEntry( master_rid, slave_rid );
            }
        }
    }

    return master_rid;
}

bool cHandler::StartThread()
{
    m_thread = wrap_g_thread_create_new( "StartThread", cHandler::ThreadProcAdapter, this, TRUE, 0 );

    return ( m_thread != 0 );
}

gpointer cHandler::ThreadProcAdapter( gpointer data )
{
    cHandler * handler = reinterpret_cast<cHandler*>(data);
    handler->ThreadProc();

    return 0;
}

void cHandler::ThreadProc()
{
    bool rc;

    while ( !m_stop ) {
        rc = OpenSession();
        if ( rc ) {
            rc = Discover();
            if ( rc ) {
                if ( m_startup_discovery_status == StartupDiscoveryUncompleted ) {
                    m_startup_discovery_status = StartupDiscoveryDone;
                }
                while ( !m_stop ) {
                    struct oh_event * e = 0;
                    rc = ReceiveEvent( e );
                    if ( rc ) {
                        if ( e != 0 ) {
                            HandleEvent( e );
                        }
                    } else {
                        break;
                    }
                }
            } else {
                if ( m_startup_discovery_status == StartupDiscoveryUncompleted ) {
                    m_startup_discovery_status = StartupDiscoveryFailed;
                }
            }
            CloseSession();
            RemoveAllResources();
        } else {
            if ( m_startup_discovery_status == StartupDiscoveryUncompleted ) {
                m_startup_discovery_status = StartupDiscoveryFailed;
            }
        }
        if ( !m_stop ) {
            g_usleep( (gulong)( OpenSessionRetryInterval / 1000ULL ) );
        }
    }
}

bool cHandler::OpenSession()
{
    if ( m_sid != InvalidSessionId ) {
        CRIT( "Session is already open" );
        return true;
    }

    SaErrorT rv;

    SaHpiSessionIdT sid;
    rv = Abi()->saHpiSessionOpen( m_did, &sid, 0 );
    if ( rv != SA_OK ) {
        CRIT( "saHpiSessionOpen failed with rv = %d", rv );
        return false;
    }

    rv = Abi()->saHpiSubscribe( sid );
    if ( rv != SA_OK ) {
        CRIT( "saHpiSubscribe failed with rv = %d", rv );
        rv = Abi()->saHpiSessionClose( m_sid );
        if ( rv != SA_OK ) {
            CRIT( "saHpiSessionClose failed with rv = %d", rv );
        }
        return false;
    }

    m_sid = sid;

    return true;
}

bool cHandler::CloseSession()
{
    SaErrorT rv;

    if ( m_sid == InvalidSessionId ) {
        return true;
    }

    rv = Abi()->saHpiSessionClose( m_sid );
    if ( rv != SA_OK ) {
        CRIT( "saHpiSessionClose failed with rv = %d", rv );
    }
    m_sid = InvalidSessionId;

    return true;
}

bool cHandler::Discover()
{
    bool rc;
    SaErrorT rv;

    rv = Abi()->saHpiDiscover( m_sid );
    if ( rv != SA_OK ) {
        CRIT( "saHpiDiscover failed with rv = %d", rv );
        return false;
    }

    std::queue<struct oh_event *> events;
    rc = FetchRptAndRdrs( events );
    if ( !rc ) {
        return false;
    }
    while( !events.empty() ) {
        struct oh_event * e = events.front();
        events.pop();

        SaHpiResourceIdT master_rid = GetOrCreateMaster( e->resource );
        // TODO may be e->event.Source is better than e->resource.ResourceId
        CompleteAndPostResourceUpdateEvent( e, master_rid );
    }

    return true;
}

void cHandler::RemoveAllResources()
{
    std::vector<ResourceMapEntry> entries;
    TakeEntriesAway( entries );
    for ( unsigned int i = 0, n = entries.size(); i < n; ++i ) {
        struct oh_event * e = g_new0( struct oh_event, 1 );
        e->event.Source = entries[i].slave_rid;
        e->resource.ResourceCapabilities = 0;
        SaHpiEventT& he = e->event;
        he.EventType = SAHPI_ET_RESOURCE;
        he.Severity = SAHPI_MAJOR;
        SaHpiResourceEventT& re = he.EventDataUnion.ResourceEvent;
        re.ResourceEventType = SAHPI_RESE_RESOURCE_REMOVED;
   
        CompleteAndPostEvent( e, entries[i].master_rid, true );
    }
}

bool cHandler::ReceiveEvent( struct oh_event *& e )
{
    e = g_new0( struct oh_event, 1 );
    SaHpiRdrT * rdr = g_new0( SaHpiRdrT, 1 );
    e->rdrs = g_slist_append( e->rdrs, rdr );

    SaErrorT rv = Abi()->saHpiEventGet( m_sid,
                                        GetEventTimeout,
                                        &e->event,
                                        rdr,
                                        &e->resource,
                                        0 );
    if ( rv != SA_OK ) {
        oh_event_free( e, 0 );
        e = 0;
        if ( rv != SA_ERR_HPI_TIMEOUT ) {
            CRIT( "saHpiEventGet failed with rv = %d", rv );
            return false;
        }
        return true;
    }
    if ( !IsRdrValid( *rdr ) ) {
        oh_event_free( e, 1 );
        e->rdrs = 0;
    }
    // We do not pass Domain Event to the Master OpenHPI daemon
    if ( e->event.EventType == SAHPI_ET_DOMAIN ) {
        oh_event_free( e, 0 );
        e = 0;
    }

    return true;
}

void cHandler::HandleEvent( struct oh_event * e )
{
    SaHpiResourceIdT slave_rid = e->event.Source;

    bool is_known   = IsSlaveKnown( slave_rid );
    bool is_update  = IsUpdateEvent( e->event );
    bool is_leaving = IsLeavingEvent( e->event );

    SaHpiResourceIdT master_rid;
    if ( is_known ) {
        master_rid = GetMaster( slave_rid );
    } else {
        master_rid = GetOrCreateMaster( e->resource );
    }

    struct oh_event * e_main = e;

    struct oh_event * e_update = 0;
    if ( !is_leaving ) {
        if ( ( !is_known ) || is_update ) {
            e_update = g_new0( struct oh_event, 1 );
            e_update->event.Source = slave_rid;
            e_update->resource = e->resource;
        }
    }

    CompleteAndPostEvent( e_main, master_rid, false );

    if ( is_leaving ) {
        RemoveEntry( slave_rid );
    }

    if ( e_update != 0 ) {
        bool rc = FetchRdrs( e_update );
        if ( rc ) {
            CompleteAndPostResourceUpdateEvent( e_update, master_rid );
        } else {
            oh_event_free( e_update, 0 );
        }
    }
}

SaHpiUint32T cHandler::GetRptUpdateCounter() const
{
    SaErrorT rv;
    SaHpiDomainInfoT di;
    rv = Abi()->saHpiDomainInfoGet( m_sid, &di );
    if ( rv == SA_OK ) {
        return di.RptUpdateCount;
    } else {
        CRIT( "saHpiDomainInfoGet failed with rv = %d", rv );
        return 0;
    }
}

SaHpiUint32T cHandler::GetRdrUpdateCounter( SaHpiResourceIdT slave_rid ) const
{
    SaErrorT rv;
    SaHpiUint32T cnt;
    rv = Abi()->saHpiRdrUpdateCountGet( m_sid, slave_rid, &cnt );
    if ( rv == SA_OK ) {
        return cnt;
    } else {
        CRIT( "saHpiRdrUpdateCountGet failed with rv = %d", rv );
        return 0;
    }
}

bool cHandler::FetchRptAndRdrs( std::queue<struct oh_event *>& events ) const
{
    for ( unsigned int attempt = 0; attempt < MaxFetchAttempts; ++attempt ) {
        while( !events.empty() ) {
            oh_event_free( events.front(), 0 );
            events.pop();
        }

        SaHpiUint32T cnt = GetRptUpdateCounter();
        SaHpiEntryIdT id, next_id;
        for ( id = SAHPI_FIRST_ENTRY; id != SAHPI_LAST_ENTRY; id = next_id ) {
            struct oh_event * e = g_new0( struct oh_event, 1 );
            SaErrorT rv = Abi()->saHpiRptEntryGet( m_sid,
                                                   id,
                                                   &next_id,
                                                   &e->resource );
            if ( rv != SA_OK ) {
                CRIT( "saHpiRptEntryGet failed with rv = %d", rv );
                break;
            }
            e->event.Source = e->resource.ResourceId;
            bool rc = FetchRdrs( e );
            if ( !rc ) {
                break;
            }
            events.push( e );
        }
        if ( cnt == GetRptUpdateCounter() ) {
            return true;
        }
    }

    while( !events.empty() ) {
        oh_event_free( events.front(), 0 );
        events.pop();
    }

    return false;
}

bool cHandler::FetchRdrs( struct oh_event * e ) const
{
    SaHpiResourceIdT slave_rid = e->event.Source;

    for ( unsigned int attempt = 0; attempt < MaxFetchAttempts; ++attempt ) {
        oh_event_free( e, 1 );
        e->rdrs = 0;

        SaHpiUint32T cnt = GetRdrUpdateCounter( slave_rid );
        SaHpiEntryIdT id, next_id;
        for ( id = SAHPI_FIRST_ENTRY; id != SAHPI_LAST_ENTRY; id = next_id ) {
            SaHpiRdrT * rdr = g_new0( SaHpiRdrT, 1 );
            SaErrorT rv = Abi()->saHpiRdrGet( m_sid,
                                              slave_rid,
                                              id,
                                              &next_id,
                                              rdr );
            if ( rv != SA_OK ) {
                g_free( rdr );
                CRIT( "saHpiRdrGet failed with rv = %d", rv );
                break;
            }
            e->rdrs = g_slist_append( e->rdrs, rdr );
        }
        if ( cnt == GetRdrUpdateCounter( slave_rid ) ) {
            return true;
        }
    }

    oh_event_free( e, 1 );
    e->rdrs = 0;

    return false;
}

void cHandler::CompleteAndPostEvent( struct oh_event * e,
                                     SaHpiResourceIdT master_rid,
                                     bool set_timestamp )
{
    TranslateEvent( e->event, master_rid );
    TranslateRptEntry( e->resource, master_rid, m_root );
    TranslateRdrs( e->rdrs, m_root );

    e->hid = m_id;
    SaHpiEventT& he = e->event;
    if ( set_timestamp ) {
        oh_gettimeofday( &he.Timestamp );
    }

    oh_evt_queue_push( &m_eventq, e );
}

void cHandler::CompleteAndPostResourceUpdateEvent( struct oh_event * e,
                                                   SaHpiResourceIdT master_rid )
{
    SaHpiEventT& he = e->event;
    he.EventType = SAHPI_ET_RESOURCE;
    he.Severity = SAHPI_MAJOR; // TODO
    SaHpiResourceEventT& re = he.EventDataUnion.ResourceEvent;
    re.ResourceEventType = SAHPI_RESE_RESOURCE_UPDATED;

    CompleteAndPostEvent( e, master_rid, true );
}


}; // namespace Slave

