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

static GSList *hs_eq=NULL;

/*
 * session_push_event pushs and event into a session.
 * We store a copy of event so that caller of the function
 * needn't care about ref counter of the event.
*/

int hotswap_push_event(struct oh_event *e)
{
	struct oh_event *e1;

	e1 = malloc(sizeof(*e1));
	if (!e1) {
		dbg("Out of memory!");
		return -1;
	}
	memcpy(e1, e, sizeof(*e));

        hs_eq = g_slist_append(hs_eq, (gpointer *) e1);
        return 0;
}

/*
 * session_pop_event - pops events off the session.  
 *
 * return codes are left as was, but it seems that return 1 for success
 * here doesn't jive with the rest of the exit codes
 */

int hotswap_pop_event(struct oh_event *e) 
{
        GSList *head;
        
        if (g_slist_length(hs_eq) == 0) {
                return 0;
	}
       
	head = hs_eq;
        hs_eq = g_slist_remove_link(hs_eq, head);
        
	memcpy(e, head->data, sizeof(*e));
	
	free(head->data);
	g_slist_free_1(head);
	
	return 1;
}

/*
 * session_has_event - query if the session has events
 */
int hotswap_has_event()
{
	return (hs_eq == NULL) ? 0 : 1;
}
