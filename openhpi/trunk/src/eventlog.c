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

static inline int sel_add(GSList **sel_list, 
		const SaHpiSelEntryT *entry, int counter)
{
	struct oh_sel *sel;
	
	sel = malloc(sizeof(*sel));
	if (!sel) {
		dbg("Out of memory");
		return -1;
	}
	memset(sel, 0, sizeof(*sel));

	memcpy(&sel->entry, entry, sizeof(*entry));
	sel->entry.EntryId = counter;
	*sel_list = g_slist_append(*sel_list, sel);
	return 0;
}

static inline void sel_del(GSList **sel_list, SaHpiSelEntryIdT id)
{
	GSList *i;

	g_slist_for_each(i, *sel_list) {
		struct oh_sel *sel;
		sel = i->data;
		if (sel->entry.EntryId == id) {
			*sel_list = g_slist_remove_link(*sel_list, i);
			free(sel);
			break;
		}
	}
}

static inline void sel_clr(GSList **sel_list) 
{
	GSList *i, *i2;

	g_slist_for_each_safe(i, i2, *sel_list) {
		struct oh_sel *sel;
		sel = i->data;
		*sel_list = g_slist_remove_link(*sel_list, i);
		free(sel);			
	}
}

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

int dsel_set_state(SaHpiDomainIdT domain_id, int enable)
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}
	
	d->sel_state = enable ? OH_SEL_ENABLED : OH_SEL_DISABLED;
	return 0;
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

int dsel_add(SaHpiDomainIdT domain_id, SaHpiSelEntryT *entry)
{
	struct oh_domain *d;
	
	dbg("DSEL from application!");
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}

	d->sel_counter++;
	sel_add(&d->sel_list, entry, d->sel_counter);
	return 0;
}

int dsel_add2(struct oh_domain *d, struct oh_hpi_event *e)
{
	struct oh_sel *sel;
	
	dbg("DSEL from plugin!");
	sel = malloc(sizeof(*sel));
	if (!sel) {
		dbg("Out of memory");
		return -1;
	}
	memset(sel, 0, sizeof(*sel));

	sel->res_id		= e->parent;
	sel->rdr_id		= e->id;
	sel->entry.EntryId	= d->sel_counter++;
	gettimeofday1(&sel->entry.Timestamp);
	
	memcpy(&sel->entry.Event, &e->event, sizeof(sel->entry.Event));
	
	d->sel_list = g_slist_append(d->sel_list, sel);
	return 0;
}

int dsel_del(SaHpiDomainIdT domain_id, SaHpiSelEntryIdT id)
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}

	sel_del(&d->sel_list, id);
	return 0;
}

int dsel_clr(SaHpiDomainIdT domain_id) 
{
	struct oh_domain *d;
	
	d = get_domain_by_id(domain_id);
	if (!d) {
		dbg("Invalid domain");
		return -1;
	}

	sel_clr(&d->sel_list);
	return 0;
}

/* here is a little tricky. rsel_add doesn't add the event into
 * sel_list in resource. Instead, it is plug-in's repository to
 * construct a RSEL event and send the event into infrastruture
 * so that infrastructure can get the entry into resource's 
 * sel_list
 */
int rsel_add(SaHpiResourceIdT res_id, SaHpiSelEntryT *entry)
{
	struct oh_resource *res;
	
	res = get_resource(res_id);
	if (!res) {
		dbg("Invalid resource id");
		return -1;
	}
	
	if (!res->handler->abi->add_sel_entry) {
		dbg("No such function");
		return -1;
	}
	if (res->handler->abi->add_sel_entry(res->handler->hnd, res->oid, entry)<0) {
		dbg("Error when calling");
		return -1;
	}
	
	return 0;
}

int rsel_add2(struct oh_resource *res, struct oh_rsel_event *e)
{
	struct oh_rsel *rsel;
	
	rsel = malloc(sizeof(*rsel));
	if (!rsel) {
		dbg("Out of memory");
		return -1;
	}
	memset(rsel, 0, sizeof(*rsel));

	*rsel = e->rsel;
        rsel->entry_id = res->sel_counter++;	
	res->sel_list = g_slist_append(res->sel_list, rsel);
	return 0;
}

int rsel_del(SaHpiResourceIdT res_id, SaHpiSelEntryIdT id)
{
	struct oh_resource *res;
	
	res = get_resource(res_id);
	if (!res) {
		dbg("Invalid resource id");
		return -1;
	}

	sel_del(&res->sel_list, id);
	return 0;
}

void rsel_clr(SaHpiResourceIdT res_id) 
{
	struct oh_resource *res;
	
	res = get_resource(res_id);
	if (!res) {
		dbg("Invalid resource id");
		return;
	}

	sel_clr(&res->sel_list);
}
