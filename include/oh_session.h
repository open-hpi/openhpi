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
#include <glib.h>

/*
 *  Global table of all active sessions (oh_session).  This table is
 *  populated and depopulated by calls to saHpiSessionOpen() and
 *  saHpiSessionClose()
 */
extern GHashTable *session_table;

/*
 * Representation of an HPI session
 */
struct oh_session {
        /*
          Session ID as returned by saHpiSessionOpen()
        */
        SaHpiSessionIdT session_id;

        /*
          A session is always associated with exactly one domain
        */
        SaHpiDomainIdT domain_id;

        enum {
                OH_EVENT_UNSUBSCRIBE=0,
                OH_EVENT_SUBSCRIBE,
        } event_state;

        /*
          Even if multiple sessions are opened for the same domain,
          each session could receive different events depending on what
          events the caller signs up for.

          This is the session specific event queue          
        */
        GSList *eventq;
};

SaErrorT oh_create_session(SaHpiDomainIdT did, SaHpiSessionIdT *sid);
SaErrorT oh_get_session_domain(SaHpiSessionIdT sid, SaHpiDomainIdT *did);
SaErrorT oh_lookup_session(SaHpiSessionIdT sid, struct oh_session **session);
SaErrorT oh_list_sessions(SaHpiDomainIdT did, GSList **session_ids);
SaHpiBoolT oh_is_session_subscribed(SaHpiDomainIdT sid);
SaErrorT oh_destroy_session(SaHpiDomainIdT sid);

#endif /* OH_SESSION_H */

