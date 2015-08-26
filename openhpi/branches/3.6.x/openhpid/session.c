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
 *      Renier Morales <renier@openhpi.org>
 *
 */

#include <string.h>

#include <oHpi.h>

#include <oh_domain.h>
#include <oh_error.h>
#include <oh_session.h>
#include <oh_utils.h>

#include "conf.h"
#include "event.h"
#include "lock.h"
#include <sahpi_wrappers.h>

struct oh_session_table oh_sessions = {
        .table = NULL,
#if !GLIB_CHECK_VERSION (2, 32, 0)
        .lock = G_STATIC_REC_MUTEX_INIT
#endif
};


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
        static SaHpiSessionIdT id = 1;        /* Session ids will start at 1 */

        if (did == SAHPI_UNSPECIFIED_DOMAIN_ID)
                did = OH_DEFAULT_DOMAIN_ID;

        session = g_new0(struct oh_session, 1);
        if (!session)
                return 0;

        session->did = did;
        session->eventq = g_async_queue_new();
        session->subscribed = SAHPI_FALSE;

        domain = oh_get_domain(did);
        if (!domain) {
                g_async_queue_unref(session->eventq);
                g_free(session);
                return 0;
        }
        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session->id = id++;
        g_hash_table_insert(oh_sessions.table, &(session->id), session);
        oh_sessions.list = g_slist_append(oh_sessions.list, session);
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
        oh_release_domain(domain);

        return session->id;
}

/**
 * oh_get_session_domain
 * @sid:
 *
 *
 *
 * Returns: SAHPI_UNSPECIFIED_DOMAIN_ID if domain id was not found.
 **/
SaHpiDomainIdT oh_get_session_domain(SaHpiSessionIdT sid)
{
        struct oh_session *session = NULL;
        SaHpiDomainIdT did;

        if (sid < 1)
                return SAHPI_UNSPECIFIED_DOMAIN_ID;

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SAHPI_UNSPECIFIED_DOMAIN_ID;
        }

        did = session->did;
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */


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
        GSList *node = NULL;

        if (did == SAHPI_UNSPECIFIED_DOMAIN_ID)
                did = OH_DEFAULT_DOMAIN_ID;

        domain = oh_get_domain(did);
        if (!domain)
                return NULL;

        session_ids = g_array_new(FALSE, TRUE, sizeof(SaHpiSessionIdT));

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        for (node = oh_sessions.list; node; node = node->next) {
                struct oh_session *s = node->data;
                if (s->did != did) continue;
                g_array_append_val(session_ids, s->id);
        }
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
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
SaErrorT oh_get_session_subscription(SaHpiSessionIdT sid,
                                     SaHpiBoolT * state)
{
        struct oh_session *session = NULL;        
                
        if (sid < 1)
        	return SA_ERR_HPI_INVALID_SESSION;
        	
	if (state == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;       
                
        if (oh_sessions.table == NULL)
        	return SA_ERR_HPI_INTERNAL_ERROR;

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }
        *state = session->subscribed;
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */

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

        if (sid < 1)
                return SA_ERR_HPI_INVALID_PARAMS;

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
               wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }
        session->subscribed = state;

        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */
        /* Flush session's event queue
         */
        if (state == SAHPI_FALSE) {
                while (oh_dequeue_session_event(sid,
                                                SAHPI_TIMEOUT_IMMEDIATE,
                                                &e, NULL) == SA_OK) {
			oh_event_free(&e, TRUE);
		}
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
SaErrorT oh_queue_session_event(SaHpiSessionIdT sid,
                                struct oh_event * event)
{
        struct oh_session *session = NULL;
        struct oh_event *qevent = NULL;
        struct oh_global_param param = {.type = OPENHPI_EVT_QUEUE_LIMIT };
        SaHpiBoolT nolimit = SAHPI_FALSE;

        if (sid < 1 || !event)
                return SA_ERR_HPI_INVALID_PARAMS;

        qevent = oh_dup_event(event);
        if (!qevent)
                return SA_ERR_HPI_OUT_OF_MEMORY;

        if (oh_get_global_param(&param)) {
                nolimit = SAHPI_TRUE;
        }

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                oh_event_free(qevent, FALSE);
                return SA_ERR_HPI_INVALID_SESSION;
        }

        if (nolimit == SAHPI_FALSE) {
                SaHpiSessionIdT tmp_sid;
                tmp_sid = session->id;
                gint qlength = g_async_queue_length(session->eventq);
                if (qlength > 0 && qlength >= param.u.evt_queue_limit) {
                        /* Don't proceed with event push if queue is overflowed */
                        session->eventq_status = SAHPI_EVT_QUEUE_OVERFLOW;
                        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                        oh_event_free(qevent, FALSE);
                        CRIT("Session %d's queue is out of space; "
                            "# of events is %d; Max is %d",
                            tmp_sid, qlength, param.u.evt_queue_limit);
                        return SA_ERR_HPI_OUT_OF_SPACE;
                }
        }

        g_async_queue_push(session->eventq, qevent);
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */

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
                                  struct oh_event * event,
                                  SaHpiEvtQueueStatusT * eventq_status)
{
        struct oh_session *session = NULL;
        struct oh_event *devent = NULL;
        GAsyncQueue *eventq = NULL;
        SaHpiBoolT subscribed;
        SaErrorT invalid;

        if (sid < 1 || (event == NULL))
                return SA_ERR_HPI_INVALID_PARAMS;

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }

        if (eventq_status) {
                *eventq_status = session->eventq_status;
        }
        session->eventq_status = 0;
        eventq = session->eventq;
        g_async_queue_ref(eventq);
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);

        if (timeout == SAHPI_TIMEOUT_IMMEDIATE) {
                devent = g_async_queue_try_pop(eventq);
        } else if (timeout == SAHPI_TIMEOUT_BLOCK) {
                while (devent == NULL) {
                        #if GLIB_CHECK_VERSION (2, 32, 0)
                        guint64 gfinaltime;
                        gfinaltime = (guint64) 5000000L;
                        devent =
                            wrap_g_async_queue_timed_pop(eventq, gfinaltime);
                        #else
                        GTimeVal gfinaltime;
                        g_get_current_time(&gfinaltime);
                        g_time_val_add(&gfinaltime, 5000000L);
                        devent =
                            wrap_g_async_queue_timed_pop(eventq, &gfinaltime);
                        #endif
                        /* compliance with spec page 63 */
                        invalid =
                            oh_get_session_subscription(sid, &subscribed);
                        /* Is the session still open? or still subscribed? */
                        if (invalid || !subscribed) {
                                g_async_queue_unref(eventq);
                                oh_event_free(devent, FALSE);
                                return invalid ? SA_ERR_HPI_INVALID_SESSION
                                    : SA_ERR_HPI_INVALID_REQUEST;
                        }
                }
        } else {
                #if GLIB_CHECK_VERSION (2, 32, 0)
                guint64 gfinaltime;
                gfinaltime = (guint64) (timeout / 1000);
                devent = wrap_g_async_queue_timed_pop(eventq, gfinaltime);
                #else
                GTimeVal gfinaltime;
                g_get_current_time(&gfinaltime);
                g_time_val_add(&gfinaltime, (glong) (timeout / 1000));
                devent = wrap_g_async_queue_timed_pop(eventq, &gfinaltime);
                #endif
                invalid = oh_get_session_subscription(sid, &subscribed);
                if (invalid || !subscribed) {
                        g_async_queue_unref(eventq);
                        oh_event_free(devent, FALSE);
                        return invalid ? SA_ERR_HPI_INVALID_SESSION :
                            SA_ERR_HPI_INVALID_REQUEST;
                }
        }
        g_async_queue_unref(eventq);

        if (devent) {
                int cc;
                cc = oh_detect_quit_event(devent);
                if (cc == 0) {
                        // OpenHPI is about to quit
                        oh_event_free(devent, FALSE);
                        return SA_ERR_HPI_NO_RESPONSE;
                }
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
        gpointer event = NULL;
        int i, len;

        if (sid < 1)
                return SA_ERR_HPI_INVALID_PARAMS;

        wrap_g_static_rec_mutex_lock(&oh_sessions.lock); /* Locked session table */
        session = g_hash_table_lookup(oh_sessions.table, &sid);
        if (!session) {
                wrap_g_static_rec_mutex_unlock(&oh_sessions.lock);
                return SA_ERR_HPI_INVALID_SESSION;
        }
        oh_sessions.list = g_slist_remove(oh_sessions.list, session);
        g_hash_table_remove(oh_sessions.table, &(session->id));
        wrap_g_static_rec_mutex_unlock(&oh_sessions.lock); /* Unlocked session table */

        /* Finalize session */
        len = g_async_queue_length(session->eventq);
        if (len > 0) {
                for (i = 0; i < len; i++) {
                        event = g_async_queue_try_pop(session->eventq);
                        if (event)
                                oh_event_free(event, FALSE);
                        event = NULL;
                }
        }
        g_async_queue_unref(session->eventq);
        g_free(session);

        return SA_OK;
}

