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
	dbg("Not implemented!");
}

static void process_internal_event(struct oh_handler *h, struct oh_event *e)
{
	dbg("Not implemented!");
}

int get_events(struct oh_session *s)
{
	int has_event;
	do {
		struct oh_event	event;
		int rv;
		int i;

		has_event = 0;
		for (i=0; i< g_slist_length(global_handler_list); i++) {
			struct timeval to = {0, 0};
			struct oh_handler *h 
				= g_slist_nth_data(global_handler_list, i); 
			rv = h->abi->get_event(h->hnd, &event, &to);
			if (rv>0) {
				if (event.type == OH_ET_HPI) {
					/* add the event to session event list */
					process_session_event(&event);
				} else {
					process_internal_event(h, &event);
				}
				has_event = 1;
			} else if (rv <0)
				return -1;
		}
	} while (has_event);

	return 0;
}
