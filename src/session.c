/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * Copyright (c) 2003 by International Business Machines
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Sean Dague <sean@dague.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

GSList *global_session_list=NULL;

static SaHpiSessionIdT scounter = 0;

struct oh_session *session_get(SaHpiSessionIdT sid)
{
        GSList *i;
        
        g_slist_for_each(i, global_session_list) {
		struct oh_session *s = i->data;
                if(s->session_id == sid) {
                        return s;
                }
        }
        return NULL;
}

int session_add(SaHpiDomainIdT did, 
                struct oh_session **session)
{
        struct oh_session *s;
        
        s = malloc(sizeof(*s));
        if (!s) {
                dbg("Cannot get memory!");
                return -1;
        }
        memset(s, 0, sizeof(*s));
        
        // (sd: session id should be set more intellegently, no?
        s->session_id = scounter++;
        s->domain_id = did;
        s->eventq = NULL;
        
        global_session_list = g_slist_append(global_session_list, s);
        
	*session = s;
	return 0;
}

int session_del(struct oh_session *session)
{
	/*FIXME: should be more cleanup */
        
	return 0;
}

/*
 * session_push_event pushs and event into a session.
 * We store a copy of event so that caller of the function
 * needn't care about ref counter of the event.
*/

int session_push_event(struct oh_session *s, struct oh_event *e)
{
	struct oh_event *e1;

	e1 = malloc(sizeof(*e1));
	if (!e1) {
		dbg("Out of memory!");
		return -1;
	}
	memcpy(e1, e, sizeof(*e));

        s->eventq = g_slist_append(s->eventq, (gpointer *) e1);
        return 0;
}

/*
 * session_pop_event - pops events off the session.  
 *
 * return codes are left as was, but it seems that return 1 for success
 * here doesn't jive with the rest of the exit codes
 */

int session_pop_event(struct oh_session *s, struct oh_event *e) 
{
        GSList *head;
        
        if (g_slist_length(s->eventq) == 0) {
                return 0;
	}
       
	head = s->eventq;
        s->eventq = g_slist_remove_link(s->eventq, head);
        
	memcpy(e, head->data, sizeof(*e));
	
	free(head->data);
	g_slist_free_1(head);
	
	return 1;
}

/*
 * session_has_event - query if the session has events
 */
int session_has_event(struct oh_session *s)
{
	return (s->eventq == NULL) ? 0 : 1;
}
