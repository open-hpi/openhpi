/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static void init_res(struct oh_resource *res)
{
	memset(res, 0, sizeof(res));
	list_init(&res->node);
	list_init(&res->rdr_list);
	return;
}

static struct oh_resource *add_res(struct oh_zone *z, struct oh_resource_id oid)
{
	struct oh_resource *r;
	
	r = malloc(sizeof(*r));
	if (!r) {
		dbg("Cannot get memory!");
		return NULL;
	}
	init_res(r);
	
	memcpy(&r->oid, &oid, sizeof(oid));

	z->domain->update_counter++;
	gettimeofday(&z->domain->update_time, NULL);
	list_add(&r->node, &z->res_list);
	
	return r;
}

static struct oh_resource *get_zone_res(struct oh_zone *z, struct oh_resource_id oid)
{
	struct list_head *i;
	
	list_for_each(i, &z->res_list) {
		struct oh_resource *r;
		r = list_container(i, struct oh_resource, node);
		if (memcmp(&r->oid, &oid, sizeof(oid))==0)
			return r;
	}
	return NULL;
}

struct oh_resource *get_res_by_oid(struct oh_domain *d, struct oh_resource_id oid)
{
	struct list_head *i;
	
	list_for_each(i, &d->zone_list) {
		struct oh_resource *r;
		struct oh_zone *z
			= list_container(i, struct oh_zone, node);
		
		r = get_zone_res(z, oid);
		if (r) return r;
	}
	return NULL;
}

struct oh_resource *insert_resource(struct oh_zone *z, struct oh_resource_id oid)
{
	struct oh_resource *res;

	res = get_zone_res(z, oid);
	if (!res) {
		dbg("New entity, add it");
		res = add_res(z, oid);
	}
	if (!res) {
		dbg("Cannot add new entity");
	}
	return res;
}

struct oh_resource *get_resource(struct oh_domain *d, SaHpiResourceIdT rid)
{
	struct list_head *i;
	list_for_each(i, &d->zone_list) {
		struct oh_zone *z = list_container(i, struct oh_zone, node);
		struct list_head *j;
		list_for_each(j, &z->res_list) {
			struct oh_resource *res;
			res = list_container(j, struct oh_resource, node);
			if (res->entry.ResourceId == rid) return res;
		}
	}

	return NULL;
}

static void init_rdr(struct oh_rdr *rdr)
{
	memset(rdr, 0, sizeof(*rdr));
	list_init(&rdr->node);
	return;
}

static struct oh_rdr *add_rdr(struct oh_resource *res, struct oh_rdr_id oid)
{
	struct oh_rdr *rdr;
	
	rdr = malloc(sizeof(*rdr));
	if (!rdr) {
		dbg("Cannot get memory!");
		return NULL;
	}
	init_rdr(rdr);
	
	memcpy(&rdr->oid, &oid, sizeof(oid));
	
	list_add(&rdr->node, &res->rdr_list);
	return rdr;
}

static struct oh_rdr *get_rdr_by_oid(struct oh_resource *res, struct oh_rdr_id oid)
{
	struct list_head *i;
	list_for_each(i, &res->rdr_list) {
		struct oh_rdr *rdr;
		rdr = list_container(i, struct oh_rdr, node);
		if (memcmp(&rdr->oid, &oid, sizeof(oid))==0)
			return rdr;
	}
	return NULL;
}

struct oh_rdr *insert_rdr(struct oh_resource *res, struct oh_rdr_id oid)
{
	struct oh_rdr *rdr;
	
	rdr = get_rdr_by_oid(res, oid);
	if (!rdr) {
		dbg("New rdr, add it!");
		rdr = add_rdr(res, oid);
	}
	if (!rdr) {
		dbg("Cannot add new RDR");
	}
	return rdr;
}
