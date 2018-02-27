/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003-2006
 * (C) Copyright Pigeon Point Systems. 2010
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
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Sean Dague <http://dague.net/sean>
 *     Renier Morales <renier@openhpi.org>
 *     Racing Guo <racing.guo@intel.com>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <glib.h>

#include <SaHpi.h>

#include <oh_utils.h>


void oh_event_free(struct oh_event *e, int only_rdrs)
{
	if (e) {
		if (e->rdrs) {
			GSList *node = NULL;
			for (node = e->rdrs; node; node = node->next) {
				g_free(node->data);
			}
			g_slist_free(e->rdrs);
		}
		if (e->rdrs_to_remove) {
			GSList *node = NULL;
			for (node = e->rdrs_to_remove; node; node = node->next) {
				g_free(node->data);
			}
			g_slist_free(e->rdrs_to_remove);
		}
		if (!only_rdrs) g_free(e);
	}
}

struct oh_event *oh_dup_event(struct oh_event *old_event)
{
	GSList *node = NULL;
	struct oh_event *e = NULL;

	if (!old_event) return NULL;

	e = g_new0(struct oh_event, 1);
	*e = *old_event;
	e->rdrs = NULL;
	for (node = old_event->rdrs; node; node = node->next) {
		e->rdrs = g_slist_append(e->rdrs, g_memdup(node->data,
							   sizeof(SaHpiRdrT)));
	}
    e->rdrs_to_remove = NULL;
	for (node = old_event->rdrs_to_remove; node; node = node->next) {
		e->rdrs_to_remove = g_slist_append(e->rdrs_to_remove, g_memdup(node->data,
							   sizeof(SaHpiRdrT)));
	}

	return e;
}

void oh_evt_queue_push(oh_evt_queue *equeue, gpointer data)
{
        g_async_queue_push(equeue, data);
}

