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

static SaHpiSessionIdT scounter = 0;

struct oh_session *session_get(SaHpiSessionIdT sid)
{
        struct oh_session *temp = NULL;
        int i;
        
        for (i = 0; i < g_slist_length(global_session_list); i++) {
                temp = (struct oh_session*) 
                        g_slist_nth_data(global_session_list, i);
                if(temp->session_id == sid) {
                        return temp;
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
        
        global_session_list = g_slist_append(global_session_list, (gpointer *) s);
        
	*session = s;
	return 0;
}

int session_del(struct oh_session *session)
{
	/*FIXME: should be more cleanup */
        
	return 0;
}

/*
 * session_push_event pushs and event into a session.  I don't think
 * that we need to do any memory allocation here, as we should
 * already have a valid session and event
*/

int session_push_event(struct oh_session *s, struct oh_event *e)
{
        s->eventq = g_slist_append(s->eventq, (gpointer *) e);
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
        struct oh_event *temp;
        GSList *tail;
        
        if (g_slist_length(s->eventq) == 0) {
                return 0;
	}
        
        tail = g_slist_last(s->eventq);
        if(tail == NULL) {
                dbg("Tail is NULL, your list is corrupt");
                return 0;
        }
        
        s->eventq = g_slist_remove_link(s->eventq, tail);
        
        temp = (struct oh_event *) g_slist_nth_data(tail, 0);
        
	memcpy(e, &s->eventq, sizeof(*e));
	free(temp);
	return 1;
}

/*
 * Leaving session_get_events for Louis or Rusty to convert
 */

#if 0
int session_get_events(struct oh_session *s)
{
	struct oh_domain 	*d 	= s->domain_id;
	struct oh_event	 	event;
	int 		 	rv;
	int			has_event;
	do {
		has_event = 0 ;
		struct list_head *i;

		list_for_each(i, &d->zone_list) { 
			struct timeval to = {0, 0};
			struct oh_zone *z 
				= list_container(i, struct oh_zone, node); 
			rv = z->abi->get_event(z->hnd, &event, &to);
			if (rv>0) {
				if (event.type == OH_ET_HPI) {
					/* add the event to session event list */
					session_push_event(s, &event);
				} else {
					domain_process_event(z, &event);
				}
				has_event = 1;
			} else if (rv <0)
				return -1;
		}
	} while (has_event);

	return 0;
}
#endif
