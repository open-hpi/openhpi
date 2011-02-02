/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#include <glib.h>

#include <oh_error.h>

#include <marshal_hpi.h>
#include <strmsock.h>

#include "conf.h"
#include "init.h"
#include "lock.h"
#include "session.h"


/***************************************************************
 * Session Layer: class cSession
 **************************************************************/
class cSession
{
public:

    explicit cSession();
    ~cSession();

    SaHpiDomainIdT GetDomainId() const
    {
        return m_did;
    }

    SaErrorT RpcOpen( SaHpiDomainIdT did );
    SaErrorT RpcClose();
    SaErrorT Rpc( uint32_t id,
                  ClientRpcParams& iparams,
                  ClientRpcParams& oparams );

private:

    cSession( const cSession& );
    cSession& operator =( cSession& );

    SaErrorT DoRpc( uint32_t id,
                    ClientRpcParams& iparams,
                    ClientRpcParams& oparams );

    SaErrorT GetSock( cClientStreamSock * & sock );
    static void DeleteSock( gpointer ptr );

private:

    static const size_t RPC_ATTEMPTS = 2;
    static const gulong NEXT_RPC_ATTEMPT_TIMEOUT = 2 * G_USEC_PER_SEC;

    // data
    SaHpiDomainIdT  m_did;
    SaHpiSessionIdT m_remote_sid;
    GStaticPrivate  m_sockets;
};


cSession::cSession()
    : m_did( SAHPI_UNSPECIFIED_DOMAIN_ID ),
      m_remote_sid( 0 )
{
    g_static_private_init( &m_sockets );
}

cSession::~cSession()
{
    g_static_private_free( &m_sockets );
}

SaErrorT cSession::RpcOpen( SaHpiDomainIdT did )
{
    m_did = did;
    SaHpiDomainIdT remote_did = SAHPI_UNSPECIFIED_DOMAIN_ID;

    ClientRpcParams iparams, oparams( &m_remote_sid );
    iparams.SetFirst( &remote_did );
    return DoRpc( eFsaHpiSessionOpen, iparams, oparams );
}

SaErrorT cSession::RpcClose()
{
    ClientRpcParams iparams, oparams;
    iparams.SetFirst( &m_remote_sid );
    return DoRpc( eFsaHpiSessionClose, iparams, oparams );
}

SaErrorT cSession::Rpc( uint32_t id,
                        ClientRpcParams& iparams,
                        ClientRpcParams& oparams )
{
    iparams.SetFirst( &m_remote_sid );
    return DoRpc( id, iparams, oparams );
}

SaErrorT cSession::DoRpc( uint32_t id,
                          ClientRpcParams& iparams,
                          ClientRpcParams& oparams )
{
    SaErrorT rv;

    cHpiMarshal * hm = HpiMarshalFind( id );
    if ( !hm ) {
        return SA_ERR_HPI_UNSUPPORTED_API;
    }

    char data[dMaxPayloadLength];
    uint32_t data_len;
    uint8_t  rp_type;
    uint32_t rp_id;
    int      rp_byte_order;

    data_len = HpiMarshalRequest( hm, data, iparams.const_array );

    bool rc = false;
    for ( size_t attempt = 0; attempt < RPC_ATTEMPTS; ++attempt ) {
        if ( attempt > 0 ) {
            DBG( "Session: RPC request %u, Attempt %u\n", id, (unsigned int)attempt );
        }
        cClientStreamSock * sock;
        rv = GetSock( sock );
        if ( rv != SA_OK ) {
            return rv;
        }

        rc = sock->WriteMsg( eMhMsg, id, data, data_len );
        if ( rc ) {
            rc = sock->ReadMsg( rp_type, rp_id, data, data_len, rp_byte_order );
            if ( rc ) {
                break;
            }
        }

        g_static_private_set( &m_sockets, 0, 0 ); // close socket
        g_usleep( NEXT_RPC_ATTEMPT_TIMEOUT );
    }
    if ( !rc ) {
        return SA_ERR_HPI_NO_RESPONSE;
    }

    oparams.SetFirst( &rv );
    int mr = HpiDemarshalReply( rp_byte_order, hm, data, oparams.array );

    if ( ( mr <= 0 ) || ( rp_type != eMhMsg ) || ( id != rp_id ) ) {
        g_static_private_set( &m_sockets, 0, 0 ); // close socket
        return SA_ERR_HPI_NO_RESPONSE;
    }

    return rv;
}

SaErrorT cSession::GetSock( cClientStreamSock * & sock )
{
    gpointer ptr = g_static_private_get( &m_sockets );
    if ( ptr ) {
        sock = reinterpret_cast<cClientStreamSock *>(ptr);
    } else {
        ohc_lock();
        const struct ohc_domain_conf * dc = ohc_get_domain_conf( m_did );
        ohc_unlock();

        if (!dc) {
            return SA_ERR_HPI_INVALID_DOMAIN;
        }

        sock = new cClientStreamSock;

        bool rc = sock->Create( dc->host, dc->port );
        if ( !rc ) {
            delete sock;
            CRIT("Session: cannot open connection to domain %u.", m_did );
            return SA_ERR_HPI_NO_RESPONSE;
        }

        // TODO configuration file, env vars?
        sock->EnableKeepAliveProbes( /* keepalive_time*/    1,
                                     /* keepalive_intvl */  1,
                                     /* keepalive_probes */ 3 );

        g_static_private_set( &m_sockets, sock, DeleteSock );
    }

    return SA_OK;
}

void cSession::DeleteSock( gpointer ptr )
{
    cClientStreamSock * sock = reinterpret_cast<cClientStreamSock *>(ptr);
    delete sock;
}


/***************************************************************
 * Session Layer: Session Table
 **************************************************************/
static GHashTable * sessions = 0;

gpointer sid_key( SaHpiSessionIdT sid )
{
    char * key = 0;
    key += sid;
    return key;
}

static void sessions_init()
{
    ohc_lock();
    if ( !sessions ) {
        sessions = g_hash_table_new( g_direct_hash, g_direct_equal );
    }
    ohc_unlock();
}

static cSession * sessions_get( SaHpiSessionIdT sid )
{
    ohc_lock();
    gpointer value = g_hash_table_lookup( sessions, sid_key( sid ) );
    ohc_unlock();
    return reinterpret_cast<cSession*>(value);
}

gboolean dehash_sessions( gpointer /* key */, gpointer value, gpointer user_data )
{
    GList ** pvalues = reinterpret_cast<GList **>(user_data);
    *pvalues = g_list_append( *pvalues, value );
    return TRUE;
}

static GList * sessions_take_all()
{
    ohc_lock();
    GList * sessions_list = 0;
    g_hash_table_foreach_remove( sessions, dehash_sessions, &sessions_list );
    ohc_unlock();

    return sessions_list;
}

static SaHpiSessionIdT sessions_add( cSession * session )
{
    static SaHpiSessionIdT next_sid = 1;

    ohc_lock();
    SaHpiSessionIdT sid = next_sid;
    ++next_sid;
    g_hash_table_insert( sessions, sid_key( sid ), session );
    ohc_unlock();

    return sid;
}

static void sessions_remove( SaHpiSessionIdT sid )
{
    ohc_lock();
    g_hash_table_remove( sessions, sid_key( sid ) );
    ohc_unlock();
}


/***************************************************************
 * Session Layer:  Interface
 **************************************************************/

void ohc_sess_init()
{
    sessions_init();
}

SaErrorT ohc_sess_open( SaHpiDomainIdT did, SaHpiSessionIdT& sid )
{
    ohc_init(); // TODO investigate

    cSession * session = new cSession;
    SaErrorT rv = session->RpcOpen( did );
    if ( rv == SA_OK ) {
        sid = sessions_add( session );
    } else {
        delete session;
    }

    return rv;
}

SaErrorT ohc_sess_close( SaHpiSessionIdT sid )
{
    cSession * session = sessions_get( sid );
    if ( !session ) {
        return SA_ERR_HPI_INVALID_SESSION;
    }

    SaErrorT rv = session->RpcClose();
    if ( rv == SA_OK ) {
        sessions_remove( sid );
        delete session;
    }

    return rv;
}

SaErrorT ohc_sess_close_all()
{
    SaErrorT rv = SA_OK;

    GList * sessions_list = sessions_take_all();
    if ( g_list_length( sessions_list ) == 0 ) {
        //rv = SA_ERR_HPI_INVALID_REQUEST;
        rv = SA_OK;
    } else {
        GList * item = sessions_list;
        while ( item ) {
            cSession * session = reinterpret_cast<cSession*>(item->data);
            session->RpcClose();
            delete session;
            item = item->next;
        }
    }

    g_list_free( sessions_list );

    return rv;
}


SaErrorT ohc_sess_rpc( uint32_t id,
                       SaHpiSessionIdT sid,
                       ClientRpcParams& iparams,
                       ClientRpcParams& oparams )
{
    cSession * session = sessions_get( sid );
    if ( !session ) {
        return SA_ERR_HPI_INVALID_SESSION;
    }

    return session->Rpc( id, iparams, oparams );
}

SaErrorT ohc_sess_get_did( SaHpiSessionIdT sid, SaHpiDomainIdT& did )
{
    cSession * session = sessions_get( sid );
    if ( !session ) {
        return SA_ERR_HPI_INVALID_SESSION;
    }

    did = session->GetDomainId();

    return SA_OK;
}

