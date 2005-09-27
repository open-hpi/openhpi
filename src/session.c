/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
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

#include <string.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <oh_lock.h>
#include <oh_session.h>
#include <oh_domain.h>
#include <oh_config.h>

struct oh_session_table oh_sessions = {
        .table = NULL,
        .lock = G_STATIC_REC_MUTEX_INIT
};

static struct oh_event *oh_generate_hpi_event(void)
{
        struct oh_event *event = NULL;

        event = g_new0(struct oh_event, 1);
        event->type = OH_ET_HPI;
        event->u.hpi_event.res.ResourceId= SAHPI_UNSPECIFIED_RESOURCE_ID;
        event->u.hpi_event.rdr.RecordId = SAHPI_ENTRY_UNSPECIFIED;
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
        strcpy((char *)(event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Data),
                "This session is being destroyed now!");
        event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.DataLength =
                strlen((char *)(event->u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Data));

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
        session->eventq = g_async_queue_new();

        domain = oh_get_domain(did);
        if (!domain) {
                g_async_queue_unref(session->eventq);
                g_free(session);
                return 0;
        }
        g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session->id = id++;
        g_hash_table_insert(oh_sessions.table, &(session->id), session);
        g_array_append_val(domain->sessions, session->id);
        g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
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

        g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_static_rec_mutex_unlock(&oh_sessions.lock);
                return 0;
        }

        did = session->did;
        g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */


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

	/* compile error */
#undef g_array_index
#define g_array_index(a,t,i)      (((t*)(void *) ((a)->data)) [(i)])
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
SaErrorT oh_get_session_subscription(SaHpiSessionIdT sid, SaHpiBoolT *state)
{
        struct oh_session *session = NULL;

        if (sid < 1 || (state == NULL)) return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }
        *state = session->state;
        g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */

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
SaErrorT oh_set_session_subscription(SaHpiSessionIdT sid, SaHpiBoolT state)
{
       struct oh_session *session = NULL;
       struct oh_event e;

       if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

       g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {
               g_static_rec_mutex_unlock(&oh_sessions.lock);
               return SA_ERR_HPI_INVALID_SESSION;
       }
       session->state = state;

       g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
       /* Flush session's event queue
        */
       if (state == SAHPI_FALSE) {
               while (oh_dequeue_session_event(sid,
                                               SAHPI_TIMEOUT_IMMEDIATE,
                                               &e, NULL) == SA_OK);
       }
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
       struct oh_global_param param = { .type = OPENHPI_EVT_QUEUE_LIMIT };

       if (sid < 1 || !event) return SA_ERR_HPI_INVALID_PARAMS;

       qevent = g_memdup(event, sizeof(struct oh_event));
       if (!qevent) return SA_ERR_HPI_OUT_OF_MEMORY;

       if (oh_get_global_param(&param))
               param.u.evt_queue_limit = OH_MAX_EVT_QUEUE_LIMIT;

       g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {
               g_static_rec_mutex_unlock(&oh_sessions.lock);
               g_free(qevent);
               return SA_ERR_HPI_INVALID_SESSION;
       }

       if (param.u.evt_queue_limit != OH_MAX_EVT_QUEUE_LIMIT) {
               SaHpiSessionIdT tmp_sid = session->id;
               gint qlength = g_async_queue_length(session->eventq);
               if (qlength > 0 && qlength >= param.u.evt_queue_limit) {
                       /* Don't proceed with event push if queue is overflowed */
                       session->eventq_status = SAHPI_EVT_QUEUE_OVERFLOW;
                       g_static_rec_mutex_unlock(&oh_sessions.lock);
                       g_free(qevent);
                       dbg("Session %d's queue is out of space; "
                           "# of events is %d; Max is %d",
                           tmp_sid, qlength, param.u.evt_queue_limit);
                       return SA_ERR_HPI_OUT_OF_SPACE;
               }
       }

       g_async_queue_push(session->eventq, qevent);
       g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */

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
                                  struct oh_event *event,
                                  SaHpiEvtQueueStatusT *eventq_status)
{
       struct oh_session *session = NULL;
       struct oh_event *devent = NULL;
       GTimeVal gfinaltime;
       GAsyncQueue *eventq = NULL;

       if (sid < 1 || (event == NULL)) return SA_ERR_HPI_INVALID_PARAMS;

       g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
       session = g_hash_table_lookup(oh_sessions.table, &sid);
       if (!session) {
               g_static_rec_mutex_unlock(&oh_sessions.lock);
               return SA_ERR_HPI_INVALID_SESSION;
       }

       if (eventq_status) {
               *eventq_status = session->eventq_status;
               session->eventq_status = 0;
       }
       eventq = session->eventq;
       g_async_queue_ref(eventq);
       g_static_rec_mutex_unlock(&oh_sessions.lock);

       if (timeout == SAHPI_TIMEOUT_IMMEDIATE) {
               devent = g_async_queue_try_pop(eventq);
       } else if (timeout == SAHPI_TIMEOUT_BLOCK) {
               devent = g_async_queue_pop(eventq);
       } else {
               g_get_current_time(&gfinaltime);
               g_time_val_add(&gfinaltime, (glong) (timeout / 1000));
               devent = g_async_queue_timed_pop(eventq, &gfinaltime);
       }
       g_async_queue_unref(eventq);

       if (devent) {
               memcpy(event, devent, sizeof(struct oh_event));
               g_free(devent);
               return SA_OK;
       } else {
               memset(event, 0, sizeof(struct oh_event));
               return SA_ERR_HPI_TIMEOUT;
       }
}

/**
 * oh_destroy_session
 * @sid:
 * @update_domain:
 *
 *
 * Returns:
 **/
SaErrorT oh_destroy_session(SaHpiSessionIdT sid)
{
        struct oh_session *session = NULL;
        struct oh_domain *domain = NULL;
        SaHpiDomainIdT did;
        gpointer event = NULL;
        int i, len;

        if (sid < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }

        g_hash_table_remove(oh_sessions.table, &(session->id));
        g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
        did = session->did;

        /* Finalize session */
        len = g_async_queue_length(session->eventq);
        if (len < 0) {
                for (i = 0; i > len; i--) {
                        g_async_queue_push(session->eventq, oh_generate_hpi_event());
                }
        } else if (len > 0) {
                for (i = 0; i < len; i++) {
                        event = g_async_queue_try_pop(session->eventq);
                        if (event) g_free(event);
                        event = NULL;
                }
        }
        g_async_queue_unref(session->eventq);
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

static void __delete_by_did(gpointer key, gpointer value, gpointer user_data)
{
        SaHpiDomainIdT did = *((SaHpiDomainIdT *)user_data);
        struct oh_session *session = (struct oh_session *)value;
        int i, len;
        gpointer event = NULL;

        if (!session) {
                dbg("Session does not exist!");
                return;
        }

        if (session->did != did) {
                return;
        }

        g_hash_table_remove(oh_sessions.table, &(session->id));

        /* Neutralize event queue */
        len = g_async_queue_length(session->eventq);
        if (len < 0) {
                for (i = 0; i > len; i--) {
                        g_async_queue_push(session->eventq, oh_generate_hpi_event());
                }
        } else if (len > 0) {
                for (i = 0; i < len; i++) {
                        event = g_async_queue_try_pop(session->eventq);
                        if (event) g_free(event);
                        event = NULL;
                }
        }
        g_async_queue_unref(session->eventq);
        g_free(session);
}

SaErrorT oh_destroy_domain_sessions(SaHpiDomainIdT did)
{
        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&oh_sessions.lock);
        g_hash_table_foreach(oh_sessions.table, __delete_by_did, &did);
        g_static_rec_mutex_unlock(&oh_sessions.lock);

        return SA_OK;
}
