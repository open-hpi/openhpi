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
        guint *session_key = NULL;

        if (!did || !sid) return SA_ERR_HPI_INVALID_PARAMS;
                
        session = g_new0(struct oh_session, 1);
        if (!session) return SA_ERR_HPI_OUT_OF_MEMORY;

        session_key = g_new0(guint, 1);
        if (!session_key) return SA_ERR_HPI_OUT_OF_MEMORY;

        *session_key = ++session_id; /* Should this line(s) be locked? -- RM */
        g_hash_table_insert(session_table, session_key, session);

        session->domain_id = did;
        session->session_id = session_id; 
                        
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

        if (!sid || !did) return SA_ERR_HPI_INVALID_PARAMS;

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
SaErrorT oh_lookup_session(SaHpiSessionIdT sid, struct oh_session *session)
{
        struct oh_session *sess = NULL;
        
        if (!sid) return SA_ERR_HPI_INVALID_PARAMS;

        sess = g_hash_table_lookup(session_table, &sid);
        if (!sess) return SA_ERR_HPI_NOT_PRESENT;

        memcpy(session, sess, sizeof(struct oh_session));

        return SA_OK;
}

/**
 * oh_list_sessions
 * @sid:
 * @session:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_list_sessions(SaHpiDomainIdT did, GSList **session_ids)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

/**
 * oh_is_session_subscribed
 * @sid:
 * @session:
 *
 *
 *
 * Returns:
 **/
SaHpiBoolT oh_is_session_subscribed(SaHpiDomainIdT sid)
{
        struct oh_session *session = NULL;

        if (!sid) return SAHPI_FALSE;

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
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}
