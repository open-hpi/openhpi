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
#include <oh_domain.h>
#include <string.h>

struct sessions sessions = {NULL, NULL};

/**
 * oh_create_session
 * @did:
 *
 *
 *
 * Returns:
 **/
SaHpiSessionIdT oh_create_session(SaHpiDomainIdT did)
{
        static guint id = 0; /* Session ids will start at 1 */
        struct oh_session *session = NULL;
        struct oh_domain *domain = NULL;

        if (did < 1) return 0;

        domain = oh_get_domain(did);
        if (!domain) return 0;

        session = g_new0(struct oh_session, 1);
        if (!session) return 0;

        g_mutex_lock(sessions.lock); /* Locked session table */
        session->id = ++id;
        session->did = did;
        /* TODO - Initialize GAsyncQueue here -- RM */
        g_hash_table_insert(sessions.table, &(session->id), session);
        g_mutex_unlock(sessions.lock); /* Unlocked session table */
        g_array_append_val(domain->sessions, session->id);
        oh_release_domain(domain->id);
                
        return session->id;
}

/**
 * oh_get_session_domain
 * @sid:
 *
 *
 *
 * Returns:
 **/
SaHpiDomainIdT oh_get_session_domain(SaHpiSessionIdT sid)
{
        struct oh_session *session = NULL;

        if (sid < 1) return 0;

        g_mutex_lock(sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(sessions.table, &sid);
        g_mutex_unlock(sessions.lock); /* Unlocked session table */
        if (!session) return 0;

        return session->did;
}

/**
 * oh_list_sessions
 * @did:
 *
 *
 *
 * Returns: A dynamically allocated array of session ids.
 * The caller needs to free this array when he is done with it.
 **/
GArray *oh_list_sessions(SaHpiDomainIdT did)
{
        struct oh_domain *domain = NULL;
        GArray *session_ids = NULL;
        guint i, length;

        if (did < 1) return NULL;

        domain = oh_get_domain(did);
        if (!domain) return NULL;

        session_ids = g_array_sized_new(FALSE, TRUE,
                                        sizeof(SaHpiSessionIdT),
                                        domain->sessions->len);
        length = domain->sessions->len;
        for (i = 0; i < length; i++) {
                g_array_append_val(session_ids,
                                   g_array_index(domain->sessions,
                                                 SaHpiSessionIdT,
                                                 i)
                                   );
        }
        oh_release_domain(did);

        return session_ids;
}

/**
 * oh_get_session_state
 * @sid:
 * @state:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_session_state(SaHpiDomainIdT sid, SaHpiBoolT *state)
{
        struct oh_session *session = NULL;

        if (sid < 1 || !state) return SA_ERR_HPI_INVALID_PARAMS;

        g_mutex_lock(sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(sessions.table, &sid);
        if (!session) {
                g_mutex_unlock(sessions.lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        *state = session->state;
        g_mutex_unlock(sessions.lock); /* Unlocked session table */

        return SA_OK;
}

/**
 * oh_set_session_state
 * @sid:
 * @state:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_set_session_state(SaHpiDomainIdT sid, SaHpiBoolT state)
{
       struct oh_session *session = NULL;

       if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

       g_mutex_lock(sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(sessions.table, &sid);
       if (!session) {
               g_mutex_unlock(sessions.lock);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       session->state = state;
       g_mutex_unlock(sessions.lock); /* Unlocked session table */

       return SA_OK;
}

/**
 * oh_queue_session_event
 * @sid:
 * @event:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_queue_session_event(SaHpiSessionIdT sid, struct oh_event *event)
{      /* TODO - Will need to adjust this when the eventq's type changes -- RM */
       struct oh_session *session = NULL;

       if (sid < 1 || !event) return SA_ERR_HPI_INVALID_PARAMS;

       g_mutex_lock(sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(sessions.table, &sid);
       if (!session) {
               g_mutex_unlock(sessions.lock);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       g_slist_append(session->eventq, event);
       g_mutex_unlock(sessions.lock); /* Unlocked session table */

       return SA_OK;
}

/**
 * oh_dequeue_session_event
 * @sid:
 * @event:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_dequeue_session_event(SaHpiSessionIdT sid, struct oh_event *event)
{      /* TODO - Will need to adjust this when the eventq's type changes -- RM */
       struct oh_session *session = NULL;
       struct oh_event *devent = NULL;

       if (sid < 1 || !event) return SA_ERR_HPI_INVALID_PARAMS;

       g_mutex_lock(sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(sessions.table, &sid);
       if (!session) {
               g_mutex_unlock(sessions.lock);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       devent = (struct oh_event *)session->eventq->data;
       memcpy(event, devent, sizeof(struct oh_event));
       session->eventq = g_slist_remove(session->eventq, devent);
       g_free(devent);
       g_mutex_unlock(sessions.lock); /* Unlocked session table */

       return SA_OK;       
}

/**
 * oh_destroy_session
 * @sid:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_destroy_session(SaHpiDomainIdT sid)
{       /* TODO - Will need to adjust this when the eventq's type changes -- RM */
        struct oh_session *session = NULL;
        GSList *node = NULL;

        if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_mutex_lock(sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(sessions.table, &sid);
        if (!session) {
                g_mutex_unlock(sessions.lock);
                return SA_ERR_HPI_NOT_PRESENT;                
        }
        
        for (node = session->eventq; node != NULL; node = node->next) {
                g_free(node->data);
        }
        g_slist_free(session->eventq);
        
        g_hash_table_remove(sessions.table, &(session->id));
        g_free(session);
        g_mutex_unlock(sessions.lock); /* Unlocked session table */

        return SA_OK;
}
