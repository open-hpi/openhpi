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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static void process_session_event(struct oh_event *e)
{
	struct oh_resource *res;
	GSList *i;
	
	res = get_res_by_oid(e->u.res_event.id);
	if (!res) {
		dbg("No the resource");
	}

	g_slist_for_each(i, res->domain_list) {
		SaHpiDomainIdT domain_id;
		GSList *j;

		domain_id = GPOINTER_TO_UINT(i->data);
		g_slist_for_each(j, global_session_list) {
			struct oh_session *s = j->data;
			if (domain_id == s->domain_id
			    && s->event_state==OH_EVENT_SUBSCRIBE) {
				dbg("push a new event, type=%d", e->type);
				session_push_event(s, e);
			}
		}
	}
}

static void process_internal_event(struct oh_handler *h, struct oh_event *e)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	switch (e->type) {
	case OH_ET_RESOURCE:
		res = insert_resource(h, e->u.res_event.id);
		memcpy(&res->entry, &e->u.res_event.entry, sizeof(res->entry));
		res->entry.ResourceId = global_rpt_counter++;
		
		/*Assume all resources blongs to DEFAULT_DOMAIN*/
		res->domain_list = g_slist_append(res->domain_list, 
				GUINT_TO_POINTER(SAHPI_DEFAULT_DOMAIN_ID));
		/* resource with domain cap needs openhpi to call abi->open_domain
		 * to extend the domain into global domain table.
		 */
		if (res->entry.ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN) {
			dbg("FIXME: can not process domain capability now");
		}
		break;

	case OH_ET_RDR:
		res = get_res_by_oid(e->u.rdr_event.parent);
		if (!res) {
			dbg("Cannot find corresponding resource");
			break;
		}
		rdr = insert_rdr(res, e->u.rdr_event.id);
		memcpy(&rdr->rdr, &e->u.rdr_event.rdr, sizeof(rdr->rdr));
		switch (rdr->rdr.RdrType) {
		case SAHPI_SENSOR_RDR: 
			rdr->rdr.RdrTypeUnion.SensorRec.Num = res->sensor_counter++;
			break;
		default:
			dbg("FIXME: Cannot process such RDR type ");
			break;
		}
		break;
	default:
		dbg("Error! Should not touch here!");
		break;
	}
}

static int get_event(void)
{
	int has_event;
	struct oh_event	event;
	GSList *i;

	has_event = 0;
	g_slist_for_each(i, global_handler_list) {
		struct timeval to = {0, 0};
		struct oh_handler *h = i->data; 
		int rv;
		
		rv = h->abi->get_event(h->hnd, &event, &to);
		if (rv>0) {
			if (event.type == OH_ET_HPI) {
				/* add the event to session event list */
				process_session_event(&event);
			} else {
				process_internal_event(h, &event);
			}
			has_event = 1;
		} else if (rv <0) {
			has_event = -1;
			break;
		}
	}

	return has_event;
}

int get_events(void)
{
	int has_event;
	do {
		has_event = get_event();
	} while (has_event>0);

	return 0;
}
