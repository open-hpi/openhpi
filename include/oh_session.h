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

#ifndef OH_SESSION_H
#define OH_SESSION_H

#include <SaHpi.h>
#include <oh_event.h>
#include <glib.h>

/*
 *  Global table of all active sessions (oh_session).  This table is
 *  populated and depopulated by calls to saHpiSessionOpen() and
 *  saHpiSessionClose(). The table has been encapsulated to have a lock
 *  alongside of it.
 */
struct sessions {
        GHashTable *table;
        GMutex *lock;                
};

extern struct sessions sessions;

/*
 * Representation of an HPI session
 */
struct oh_session {
        /*
          Session ID as returned by saHpiSessionOpen()
        */
        SaHpiSessionIdT id;

        /*
          A session is always associated with exactly one domain
        */
        SaHpiDomainIdT did;

        enum {
                OH_UNSUBSCRIBED=0,
                OH_SUBSCRIBED,
        } state;

        /*
          Even if multiple sessions are opened for the same domain,
          each session could receive different events depending on what
          events the caller signs up for.

          This is the session specific event queue          
        */
        GSList *eventq;        
        GAsyncQueue *eventq2;
        
};

SaHpiSessionIdT oh_create_session(SaHpiDomainIdT did);
SaHpiDomainIdT oh_get_session_domain(SaHpiSessionIdT sid);
GArray *oh_list_sessions(SaHpiDomainIdT did);
SaErrorT oh_get_session_state(SaHpiDomainIdT sid, SaHpiBoolT *state);
SaErrorT oh_set_session_state(SaHpiDomainIdT sid, SaHpiBoolT state);
SaErrorT oh_queue_session_event(SaHpiSessionIdT sid, struct oh_event *event);
SaErrorT oh_dequeue_session_event(SaHpiSessionIdT sid,
                                  SaHpiTimeoutT timeout,
                                  struct oh_event *event);
SaErrorT oh_destroy_session(SaHpiDomainIdT sid);

#endif /* OH_SESSION_H */

