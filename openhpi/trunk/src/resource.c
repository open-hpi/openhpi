/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static int init_res(struct oh_resource *res, enum oh_id_type type)
{
	list_init(&res->node);
	res->oid.type = type;	
	
	switch (type) {
		case OH_ID_DOMAIN:
			res->u.domain = NULL;
			break;
		case OH_ID_SEL:
			res->u.sel = NULL;
			break;
		case OH_ID_ENTITY:
			list_init(&res->u.rdr_list);
			break;
		default:
			dbg("Error resource type");
			return -1;
	}

	return 0;
}

struct oh_resource *get_res(struct oh_domain *domain, SaHpiResourceIdT rid)
{
	struct list_head *i;
	list_for_each(i, &domain->res_list) {
		struct oh_resource *res;
		res = list_container(i, struct oh_resource, node);
		if (res->rid == rid) return res;
	}

	return NULL;
}

static int add_res(struct oh_domain *d, struct oh_resource **res, enum oh_id_type type)
{
	struct oh_resource *r;
	
	r = malloc(sizeof(*r));
	if (!r) {
		dbg("Cannot get memory!");
		return -1;
	}
	memset(r, 0, sizeof(*r));
	init_res(r, type);
	
	r->rid = d->res_counter++;

	d->update_counter++;
	gettimeofday(&d->update_time, NULL);
	list_add(&r->node, &d->res_list);
	
	*res = r;
	return 0;
}

#if 0
static int del_res(struct oh_resource *res)
{
	list_del(&res->node);
	free(res);
	return 0;
}
#endif

static struct oh_resource *get_res_by_oid(struct oh_domain *d, struct oh_id *oid)
{
	struct list_head *tmp;
	
	list_for_each(tmp, &d->res_list) {
		struct oh_resource *r;
		r = list_container(tmp, struct oh_resource, node);
		if (memcmp(&r->oid, oid, sizeof(*oid))==0)
			return r;
	}
	return NULL;
}

static struct oh_resource *get_res_by_oid2(struct oh_domain *d, struct oh_id *oid)
{
	struct oh_resource *r;

	r = get_res_by_oid(d, oid);
	if (!r) {
		int rv;
		dbg("New entity, add it");
		rv = add_res(d, &r, oid->type);
		if (rv<0) {
			return NULL;
		}
	}
	return r;
}

int absent_entity(struct oh_domain *d, struct oh_id *oid)
{
	struct oh_resource *res;
	
	res = get_res_by_oid2(d, oid);
	if (!res) {
		dbg("Cannot add new entity");
		return -1;
	}
	
	res->present = 0;
	return 0;
}

int present_entity(struct oh_domain *d, struct oh_id *oid) 
{
	struct oh_resource *res;
	
	res = get_res_by_oid2(d, oid);
	if (!res) {
		dbg("Cannot add new entity");
		return -1;
	}
	
	res->present = 1;
	return 0;
}

#if 0
static int init_rdr(struct oh_rdr *rdr, enum oh_item type)
{
	list_init(&rdr->node);
	rdr->type =type;

	switch (type) {
		case OH_SENSOR:
			rdr->sensor = NULL;
			break;
		case OH_CONTROL:
			rdr->control = NULL;
			break;
		case OH_INVENTORY:
			rdr->inventory = NULL;
			break;
		case OH_WATCHDOG:
			rdr->watchdog = NULL;
			break;
		default:
			dbg("Error RDR type");
			return -1;
	}

	return 0;
}

static int add_rdr(struct oh_resource *res, struct oh_rdr **rdr, enum oh_item type)
{
	struct oh_rdr *r;
	
	r = malloc(sizeof(*r));
	if (!r) {
		dbg("Cannot get memory!");
		return -1;
	}
	memset(r, 0, sizeof(*r));
	init_rdr(r, type);
	
	list_add(&r->node, &res->rdr_list);

	*rdr = r;
	return 0;
}

static int del_rdr(struct oh_rdr *rdr)
{
	list_del(&rdr->node);
	free(rdr);
	return 0;
}
#endif
