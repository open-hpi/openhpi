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

#include "oh_client.h"
#include "oh_client_conf.h"
#include "oh_client_session.h"


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

    cClientStreamSock * sock;
    rv = GetSock( sock );
    if ( rv != SA_OK ) {
        return rv;
    }

    char data[dMaxPayloadLength];
    uint32_t data_len;
    uint8_t  rp_type;
    uint32_t rp_id;
    int      rp_byte_order;

    data_len = HpiMarshalRequest( hm, data, iparams.const_array );
    bool rc = sock->WriteMsg( eMhMsg, id, data, data_len );
    if ( rc ) {
        rc = sock->ReadMsg( rp_type, rp_id, data, data_len, rp_byte_order );
    }
    if ( !rc ) {
        g_static_private_set( &m_sockets, 0, 0 ); // close socket
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
        g_static_rec_mutex_lock(&ohc_lock);
        const struct oh_domain_conf * dc = oh_get_domain_conf( m_did );
        g_static_rec_mutex_unlock( &ohc_lock );

        if (!dc) {
            err( "Session: cannot find domain %u config.\n", m_did );
            return SA_ERR_HPI_INVALID_DOMAIN;
        }

        sock = new cClientStreamSock;

        bool rc = sock->Create( dc->host, dc->port );
        if ( !rc ) {
            delete sock;
            err("Session: cannot open connection to domain %u.\n", m_did );
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
    g_static_rec_mutex_lock(&ohc_lock);
    if ( !sessions ) {
        sessions = g_hash_table_new( g_direct_hash, g_direct_equal );
    }
    g_static_rec_mutex_unlock(&ohc_lock);
}

static cSession * sessions_get( SaHpiSessionIdT sid )
{
    g_static_rec_mutex_lock(&ohc_lock);
    gpointer value = g_hash_table_lookup( sessions, sid_key( sid ) );
    g_static_rec_mutex_unlock(&ohc_lock);
    return reinterpret_cast<cSession*>(value);
}

static SaHpiSessionIdT sessions_add( cSession * session )
{
    static SaHpiSessionIdT next_sid = 1;

    g_static_rec_mutex_lock( &ohc_lock );
    SaHpiSessionIdT sid = next_sid;
    ++next_sid;
    g_hash_table_insert( sessions, sid_key( sid ), session );
    g_static_rec_mutex_unlock(&ohc_lock);

    return sid;
}

static void sessions_remove( SaHpiSessionIdT sid )
{
    g_static_rec_mutex_lock( &ohc_lock );
    g_hash_table_remove( sessions, sid_key( sid ) );
    g_static_rec_mutex_unlock(&ohc_lock);
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
    oh_client_init(); // TODO investigate

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

