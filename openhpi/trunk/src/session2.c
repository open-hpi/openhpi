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

struct oh_session_table oh_sessions = {        
        .table = NULL,
        .lock = NULL
};

static struct oh_event *oh_generate_hpi_event(void)
{
        struct oh_event *event = NULL;

        event = g_new0(struct oh_event, 1);        
        event->type = OH_ET_HPI;
        event->u.hpi_event.parent = SAHPI_UNSPECIFIED_RESOURCE_ID;
        event->u.hpi_event.id = SAHPI_ENTRY_UNSPECIFIED;
        event->u.hpi_event.event.Source = SAHPI_UNSPECIFIED_RESOURCE_ID;
        event->u.hpi_event.event.EventType = SAHPI_ET_HPI_SW;
        event->u.hpi_event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
        event->u.hpi_event.event.Severity = SAHPI_CRITICAL;
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.MId =
                SAHPI_MANUFACTURER_ID_UNSPECIFIED;
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.Type =
                SAHPI_HPIE_OTHER;
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.DataType =
                SAHPI_TL_TYPE_TEXT;
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Language =
                SAHPI_TL_TYPE_TEXT;        
        strcpy(event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Data,
                "This session is being destroyed now!");
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.DataLength =
                strlen(event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Data);

        return event;
}

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
        struct oh_session *session = NULL;
        struct oh_domain *domain = NULL;
        static SaHpiSessionIdT id = 1; /* Session ids will start at 1 */

        if (did < 1) return 0;

        session = g_new0(struct oh_session, 1);
        if (!session) return 0;

        session->did = did;
        session->eventq2 = g_async_queue_new();

        domain = oh_get_domain(did);
        if (!domain) {
                g_async_queue_unref(session->eventq2);
                g_free(session);
                return 0;
        }
        g_mutex_lock(oh_sessions.lock); /* Locked session table */
        session->id = id++;
        g_hash_table_insert(oh_sessions.table, &(session->id), session);
        g_array_append_val(domain->sessions, session->id);
        g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */        
        oh_release_domain(domain);
                
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
        SaHpiDomainIdT did;
        
        if (sid < 1) return 0;

        g_mutex_lock(oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_mutex_unlock(oh_sessions.lock);
                return 0;
        }
        
        did = session->did;
        g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */
        

        return did;
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

        length = domain->sessions->len;
        session_ids = g_array_sized_new(FALSE, TRUE,
                                        sizeof(SaHpiSessionIdT),
                                        length);
        
        for (i = 0; i < length; i++) {
                g_array_append_val(session_ids,
                                   g_array_index(domain->sessions,
                                                 SaHpiSessionIdT,
                                                 i)
                                   );
        }
        oh_release_domain(domain);

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

        g_mutex_lock(oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_mutex_unlock(oh_sessions.lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }
        *state = session->state;
        g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */

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

       g_mutex_lock(oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {
               g_mutex_unlock(oh_sessions.lock);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       session->state = state;
       g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */

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
{      
       struct oh_session *session = NULL;
       struct oh_event *qevent = NULL;

       if (sid < 1 || !event) return SA_ERR_HPI_INVALID_PARAMS;

       qevent = g_memdup(event, sizeof(struct oh_event));
       g_mutex_lock(oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {               
               g_mutex_unlock(oh_sessions.lock);
               g_free(qevent);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       g_async_queue_push(session->eventq2, qevent);
       g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */

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
SaErrorT oh_dequeue_session_event(SaHpiSessionIdT sid,
                                  SaHpiTimeoutT timeout,
                                  struct oh_event *event)
{
       struct oh_session *session = NULL;
       struct oh_event *devent = NULL;
       GTimeVal gtimeval;
       GAsyncQueue *eventq = NULL;

       if (sid < 1 || !event) return SA_ERR_HPI_INVALID_PARAMS;

       g_mutex_lock(oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {
               g_mutex_unlock(oh_sessions.lock);
               return SA_ERR_HPI_NOT_PRESENT;
       }
       eventq = session->eventq2;
       g_async_queue_ref(eventq);
       g_mutex_unlock(oh_sessions.lock);

       if (timeout == SAHPI_TIMEOUT_IMMEDIATE) {
               devent = g_async_queue_try_pop(eventq);
       } else if (timeout == SAHPI_TIMEOUT_BLOCK) {
               devent = g_async_queue_pop(eventq); /* FIXME: Need to time this. */
       } else {
               gtimeval.tv_sec = timeout / 1000000000;
               gtimeval.tv_usec = timeout % 1000000000 / 1000;
               devent = g_async_queue_timed_pop(eventq, &gtimeval); /* FIXME: Put final time. */
       }
       g_async_queue_unref(eventq);

       if (devent) {
               memcpy(event, devent, sizeof(struct oh_event));
               g_free(devent);
               return SA_OK;
       } else {
               return SA_ERR_HPI_TIMEOUT;
       }
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
{
        struct oh_session *session = NULL;
        struct oh_domain *domain = NULL;
        SaHpiDomainIdT did;
        gpointer event = NULL;
        int i, len;

        if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_mutex_lock(oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_mutex_unlock(oh_sessions.lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        g_hash_table_remove(oh_sessions.table, &(session->id));
        g_mutex_unlock(oh_sessions.lock); /* Unlocked session table */
        did = session->did;

        /* Finalize session */
        len = g_async_queue_length(session->eventq2);
        if (len < 0) {
                for (i = 0; i > len; i--) {
                        g_async_queue_push(session->eventq2, oh_generate_hpi_event());
                }
        } else if (len > 0) {
                for (i = 0; i < len; i++) {
                        event = g_async_queue_try_pop(session->eventq2);
                        if (event) g_free(event);
                        event = NULL;
                }
        }
        g_async_queue_unref(session->eventq2);
        g_free(session);

        /* Update domain about session deletion. */
        domain = oh_get_domain(did);
        if (domain) {
                len = domain->sessions->len;
                for (i = 0; i < len; i++) {
                        if (g_array_index(domain->sessions,SaHpiSessionIdT,i) == sid) {
                                g_array_remove_index(domain->sessions,i);
                                break;
                        }
                }
                oh_release_domain(domain);
        }

        return SA_OK;
}
