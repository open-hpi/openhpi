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


int dsel_get_info(SaHpiDomainIdT domain_id, SaHpiSelInfoT *info)
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}
	
	info->Entries			= g_slist_length(d->sel_list);
	info->Size			= 0xFFFFFFFF;
	info->UpdateTimestamp		= 0;
	gettimeofday1(&info->CurrentTime);
	info->Enabled	= (d->sel_state==OH_SEL_ENABLED) ? 1 : 0;
	info->OverflowFlag		= 0;
	info->OverflowAction		= SAHPI_SEL_OVERFLOW_DROP;
	info->DeleteEntrySupported 	= 1;

	return 0;
}

int dsel_get_state(SaHpiDomainIdT domain_id)
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}
	
	return (d->sel_state==OH_SEL_ENABLED) ? 1 : 0;
}

void dsel_set_state(SaHpiDomainIdT domain_id, int enable)
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return;
	}
	
	d->sel_state = enable ? OH_SEL_ENABLED : OH_SEL_DISABLED;
}

SaHpiTimeT dsel_get_time(SaHpiDomainIdT domain_id)
{
	SaHpiTimeT cur;
	gettimeofday1(&cur);
	return cur;
}

void dsel_set_time(SaHpiDomainIdT domain_id, SaHpiTimeT time)
{
	/* just escape */
	return; 
}

void dsel_add(SaHpiDomainIdT domain_id, SaHpiSelEntryT *entry)
{
	struct oh_domain *d;
	struct oh_sel *sel;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return;
	}
	
	sel = malloc(sizeof(*sel));
	if (!sel) {
		dbg("Out of memory");
		return;
	}
	memset(sel, 0, sizeof(*sel));

	memcpy(&sel->entry, entry, sizeof(*entry));
	sel->entry.EntryId = d->sel_counter++;
	d->sel_list = g_slist_append(d->sel_list, sel);
}

void dsel_add2(struct oh_domain *d, struct oh_event *e)
{
	struct oh_sel *sel;
	
	sel = malloc(sizeof(*sel));
	if (!sel) {
		dbg("Out of memory");
		return;
	}
	memset(sel, 0, sizeof(*sel));

	sel->res_id		= e->u.hpi_event.parent;
	sel->rdr_id		= e->u.hpi_event.id;
	sel->entry.EntryId	= d->sel_counter++;
	gettimeofday1(&sel->entry.Timestamp);
	
	memcpy(&sel->entry.Event, &e->u.hpi_event.event, sizeof(sel->entry.Event));
	
	d->sel_list = g_slist_append(d->sel_list, sel);
}

void dsel_del(SaHpiDomainIdT domain_id, SaHpiSelEntryIdT id)
{
	struct oh_domain *d;
	GSList *i;
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return;
	}

	g_slist_for_each(i, d->sel_list) {
		struct oh_sel *sel;
		sel = i->data;
		if (sel->entry.EntryId == id) {
			d->sel_list = g_slist_remove_link(d->sel_list, i);
			free(sel);
			break;
		}
	}
}

void dsel_clr(SaHpiDomainIdT domain_id) 
{
	
	struct oh_domain *d;
	GSList *i, *i2;
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return;
	}

	g_slist_for_each_safe(i, i2, d->sel_list) {
		struct oh_sel *sel;
		sel = i->data;
		d->sel_list = g_slist_remove_link(d->sel_list, i);
		free(sel);			
	}
}
