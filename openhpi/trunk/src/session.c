/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static struct list_head slist;
static SaHpiSessionIdT scounter = 0;


int init_session(void) 
{
	list_init(&slist);
	return 0;
}

int uninit_session(void) 
{
	return 0;
}

struct oh_session *session_get(SaHpiSessionIdT sid)
{
	struct list_head *i;
	list_for_each(i, &slist) {
		struct oh_session *s;
		s = list_container(i, struct oh_session, node);
		if (s->sid == sid) return s;
	}

	return NULL;
}

int session_add(struct oh_domain *domain,
		 struct oh_session **session)
{
	struct oh_session *s;
	
	s = malloc(sizeof(*s));
	if (!s) {
		dbg("Cannot get memory!");
		return -1;
	}
	memset(s, 0, sizeof(*s));
		
	s->sid 	  = scounter++;
	s->domain = domain;

	list_add(&s->node, &slist);
	list_add(&s->dnode, &domain->session_list);

	list_init(&s->event_list);

	*session =s;
	return 0;
}

int session_del(struct oh_session *session)
{
	/*FIXME: should be more cleanup */
	list_del(&session->node);
	list_del(&session->dnode);
	free(session);
	return 0;
}

struct session_event {
	struct list_head node; /* link-node in the session's event list */
	struct oh_event event;
};

int session_push_event(struct oh_session *s, struct oh_event *e)
{
	struct session_event *se;
	
	se = malloc(sizeof(*se));
	if (se) {
		dbg("Cannot alloc memory");
		return -1;
	};
	
	memcpy(&se->event, e, sizeof(*e));
	list_add_tail(&se->node, &s->event_list);
	return 0;
}

int session_pop_event(struct oh_session *s, struct oh_event *e) 
{
	struct session_event *se;
	
	if (list_empty(&s->event_list)) {
		return 0;
	}
	
	se = list_container(s->event_list.next, struct session_event, node);
	list_del(&se->node);

	memcpy(e, &se->event, sizeof(*e));
	free(se);
	return 1;
}

int session_get_events(struct oh_session *s)
{
	struct oh_domain 	*d 	= s->domain;
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

int session_discover_resources(struct oh_session *s)
{
	struct oh_domain *d = s->domain;
	struct list_head *i;
	
	list_for_each(i, &d->zone_list) {
		struct oh_zone *z
			= list_container(i, struct oh_zone, node);
		z->abi->discover_resources(z->hnd);
	}
	return session_get_events(s);
}
