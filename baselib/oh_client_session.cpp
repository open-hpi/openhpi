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

#include <pthread.h>

#include <oHpi.h>
#include <config.h>
#include <oh_domain.h>
#include <oh_error.h>

#include "oh_client.h"
#include "oh_client_conf.h"
#include "oh_client_session.h"


struct oh_client_session {
        SaHpiDomainIdT did; /* Domain Id */
        SaHpiSessionIdT csid; /* Client Session Id */
        SaHpiSessionIdT dsid; /* Domain Session Id */
        GHashTable *connxs; /* Connections for this session (per thread) */
};

static GHashTable *ohc_sessions = NULL;
static SaHpiSessionIdT next_client_sid = 1;


static void __destroy_client_connx(gpointer data)
{
        struct oh_client_session *client_session = (struct oh_client_session *)data;

        g_hash_table_destroy(client_session->connxs);
        g_free(client_session);
}

void oh_client_session_init(void)
{
    g_static_rec_mutex_lock(&ohc_lock);

    // Create session table.
    if (!ohc_sessions) {
        ohc_sessions = g_hash_table_new_full( g_int_hash, 
                                              g_int_equal,
                                              NULL, 
                                              __destroy_client_connx);
    }

    g_static_rec_mutex_unlock(&ohc_lock);
}

SaErrorT oh_create_connx(SaHpiDomainIdT did, pcstrmsock *pinst)
{
        const struct oh_domain_conf *domain_conf = NULL;
        pcstrmsock connx = NULL;
        
        if (!pinst) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        oh_client_init(); /* Initialize library - Will run only once */

        g_static_rec_mutex_lock(&ohc_lock);
	connx = new cstrmsock;
        if (!connx) {
                g_static_rec_mutex_unlock(&ohc_lock);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        domain_conf = oh_get_domain_conf(did);
        if (!domain_conf) {
                delete connx;
                g_static_rec_mutex_unlock(&ohc_lock);
                err("Client configuration for domain %u was not found.", did);
                return SA_ERR_HPI_INVALID_DOMAIN;
        }
        g_static_rec_mutex_unlock(&ohc_lock);
        
	if (connx->Open(domain_conf->host, domain_conf->port)) {
		err("Could not open client socket"
		    "\nPossibly, the OpenHPI daemon has not been started.");
                delete connx;
		return SA_ERR_HPI_NO_RESPONSE;
	}
        
        *pinst = connx;
	dbg("Client instance created");
	return SA_OK;
}

void oh_delete_connx(pcstrmsock pinst)
{
	if (pinst) {
                pinst->Close();
                dbg("Connection closed and deleted");
                delete pinst;
        }
}

SaErrorT oh_close_connx(SaHpiSessionIdT SessionId)
{
        pthread_t thread_id = pthread_self();
        struct oh_client_session *client_session = NULL;

        if (SessionId == 0)
                return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&ohc_lock);
        client_session = (struct oh_client_session *)g_hash_table_lookup(ohc_sessions, &SessionId);
        if (!client_session) {
                g_static_rec_mutex_unlock(&ohc_lock);
                err("Did not find connx for sid %d", SessionId);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }
        
        g_hash_table_remove(client_session->connxs, &thread_id);

        g_static_rec_mutex_unlock(&ohc_lock);

        return SA_OK;
}

SaErrorT oh_get_connx(SaHpiSessionIdT csid, SaHpiSessionIdT *dsid, pcstrmsock *pinst, SaHpiDomainIdT *did)
{
        pthread_t thread_id = pthread_self();
        struct oh_client_session *client_session = NULL;
        pcstrmsock connx = NULL;
        SaErrorT ret = SA_OK;

	if (!csid || !dsid || !pinst || !did)
		return SA_ERR_HPI_INVALID_PARAMS;
		
	oh_client_init(); /* Initialize library - Will run only once */

        // Look up connection table. If it exists, look up connection.
        // if there is not connection, create one on-the-fly.
        g_static_rec_mutex_lock(&ohc_lock);
        client_session =
          (struct oh_client_session *)g_hash_table_lookup(ohc_sessions, &csid);
        
        if (client_session) {
                connx = (pcstrmsock)g_hash_table_lookup(client_session->connxs,
                                                        &thread_id);

                if (!connx) {
                        ret = oh_create_connx(client_session->did, &connx);
                        if (connx) {
                        	g_hash_table_insert(client_session->connxs, 
                                		    g_memdup(&thread_id,
                                		     sizeof(pthread_t)),
                                		    connx);
				dbg("We are inserting a new connection"
				    " in conns table");
                        }
                }
                *dsid = client_session->dsid;
				*did  = client_session->did;
                *pinst = connx;
        }
        g_static_rec_mutex_unlock(&ohc_lock);        

	if (client_session) {
                if (connx)
                        return SA_OK;
                else 
                        return ret;
	}
	else
		return SA_ERR_HPI_INVALID_SESSION;
}

static void __delete_connx(gpointer data)
{
        pcstrmsock pinst = (pcstrmsock)data;

        oh_delete_connx(pinst);
}

SaHpiSessionIdT oh_open_session(SaHpiDomainIdT did,
                                SaHpiSessionIdT sid,
                                pcstrmsock pinst)
{
        GHashTable *connxs = NULL;
        pthread_t thread_id;
        struct oh_client_session *client_session;
        
        if (!sid || !pinst)
		return 0;
	
        client_session = g_new0(struct oh_client_session, 1);
        
        g_static_rec_mutex_lock(&ohc_lock);
        // Create connections table for new session.
        connxs = g_hash_table_new_full(g_int_hash, g_int_equal,
                                       g_free, __delete_connx);
        // Map connection to thread id
        thread_id = pthread_self();
        g_hash_table_insert(connxs, g_memdup(&thread_id, sizeof(pthread_t)),
                            pinst);
        // Map connecitons table to session id
        client_session->did = did;
        client_session->dsid = sid;
        client_session->csid = next_client_sid++;
        client_session->connxs = connxs;
        g_hash_table_insert(ohc_sessions, &client_session->csid, client_session);
        g_static_rec_mutex_unlock(&ohc_lock);

        return client_session->csid;
}

SaErrorT oh_close_session(SaHpiSessionIdT SessionId)
{        
        if (SessionId == 0)
		return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&ohc_lock);
        // Since we used g_hash_table_new_full to create the tables,
        // this will remove the connection hash table and all of its entries also.
        g_hash_table_remove(ohc_sessions, &SessionId);
        
        g_static_rec_mutex_unlock(&ohc_lock);
        return SA_OK;
}
