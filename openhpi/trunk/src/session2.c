/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#include <oh_session.h>
#include <string.h>

struct session_net {
        SaHpiDomainIdT did;
        GSList **sessions_ptr;
};

static void fetch_domain_sessions(gpointer key, gpointer value, gpointer data)
{
        struct oh_session *session = value;
        struct session_net *net = data;        

        if (session->domain_id == net->did) {
                *(net->sessions_ptr) =
                        g_slist_append(*(net->sessions_ptr), session);
        }        
}

/**
 * oh_create_session
 * @did:
 * @sid:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_create_session(SaHpiDomainIdT did, SaHpiSessionIdT *sid)
{
        static guint session_id = 0; /* Session ids will be > 0 */
        struct oh_session *session = NULL;
        
        if (did < 1 || !sid) return SA_ERR_HPI_INVALID_PARAMS;
                
        session = g_new0(struct oh_session, 1);
        if (!session) return SA_ERR_HPI_OUT_OF_MEMORY;

        /* Need to lock these lines -- RM */
        session->session_id = ++session_id;
        session->domain_id = did;
        g_hash_table_insert(session_table, &(session->session_id), session);        
                        
        *sid = session_id;

        return SA_OK;
}

/**
 * oh_get_session_domain
 * @sid:
 * @did:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_session_domain(SaHpiSessionIdT sid, SaHpiDomainIdT *did)
{
        struct oh_session *session = NULL;

        if (sid < 1 || !did) return SA_ERR_HPI_INVALID_PARAMS;

        session = g_hash_table_lookup(session_table, &sid);
        if (!session) return SA_ERR_HPI_NOT_PRESENT;

        *did = session->domain_id;

        return SA_OK;
}

/**
 * oh_lookup_session
 * @sid:
 * @session:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_lookup_session(SaHpiSessionIdT sid, struct oh_session **session)
{
        struct oh_session *sess = NULL;

        if (sid < 1 || !session) return SA_ERR_HPI_INVALID_PARAMS;

        sess = g_hash_table_lookup(session_table, &sid);
        if (!sess) return SA_ERR_HPI_NOT_PRESENT;

        *session = sess;

        return SA_OK;
}

/**
 * oh_list_sessions
 * @did:
 * @session_ids:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_list_sessions(SaHpiDomainIdT did, GSList **session_ids)
{
        struct session_net net;
        
        if (did < 1 || !session_ids) return SA_ERR_HPI_INVALID_PARAMS;

        net.did = did;
        net.sessions_ptr = session_ids;

        /* Need to lock session_table here -- RM */
        g_hash_table_foreach(session_table,
                             fetch_domain_sessions,
                             &net);
        /* Need to unlock session_table here -- RM */

        if (g_slist_length(*session_ids) < 1)
                return SA_ERR_HPI_NOT_PRESENT;
        else
                return SA_OK;
}

/**
 * oh_is_session_subscribed
 * @sid:
 *
 *
 *
 * Returns:
 **/
SaHpiBoolT oh_is_session_subscribed(SaHpiDomainIdT sid)
{
        struct oh_session *session = NULL;

        if (sid < 1) return SAHPI_FALSE;

        session = g_hash_table_lookup(session_table, &sid);
        if (!session) return SAHPI_FALSE;

        if (session->event_state == OH_EVENT_SUBSCRIBE)
                return SAHPI_TRUE;
        else
                return SAHPI_FALSE;        
}

/**
 * oh_destroy_session
 * @sid:
 * @session:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_destroy_session(SaHpiDomainIdT sid)
{       /* Will need to adjust this when the eventq's type changes -- RM */
        struct oh_session *session = NULL;
        GSList *node = NULL;

        if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

        session = g_hash_table_lookup(session_table, &sid);
        if (!session) return SA_ERR_HPI_NOT_PRESENT;
        
        /* Need locking starting here -- RM */
        for (node = session->eventq; node != NULL; node = node->next) {
                g_free(node->data);
        }
        g_slist_free(session->eventq);
        g_hash_table_remove(session_table, &(session->session_id));
        g_free(session);

        return SA_OK;
}
