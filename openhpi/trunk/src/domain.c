/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

static struct list_head dlist;
static SaHpiDomainIdT dcounter = SAHPI_DEFAULT_DOMAIN_ID;


int init_domain(void) 
{
	list_init(&dlist);
	return 0;
}

int uninit_domain(void) 
{
	return 0;
}

struct oh_domain *domain_get(SaHpiDomainIdT did)
{
	struct list_head *i;
	list_for_each(i, &dlist) {
		struct oh_domain *d;
		d = list_container(i, struct oh_domain, node);
		if (d->did == did) return d;
	}

	return NULL;
}

struct oh_domain *domain_add()
{
	struct oh_domain  *d;
	
	d = malloc(sizeof(*d));
	if (!d) {
		dbg("Cannot get memory!");
		return NULL;
	}
	memset(d, 0, sizeof(*d));
	
	d->did = dcounter++;

	list_add(&d->node, &dlist);
	list_init(&d->session_list);
	list_init(&d->zone_list);

	d->res_counter = 0;

	return d;
}

#if 0
void domain_del(struct oh_domain *domain)
{
	/* FIXME cleaup resources in domain */
	list_del(&domain->node);
	free(domain);
}
#endif

void domain_process_event(struct oh_zone *z, struct oh_event *e)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	switch (e->type) {
	case OH_ET_RESOURCE:
		res = insert_resource(z, e->u.res_event.id);
		memcpy(&res->entry, &e->u.res_event.entry, sizeof(res->entry));
		res->entry.ResourceId = z->domain->res_counter++;

		/* resource with domain cap needs openhpi to call abi->open_domain
		 * to extend the domain into global domain table.
		 */
		if (res->entry.ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN) {
			struct oh_domain *nd;
			struct oh_zone *nz;
			void *hnd;
			nd 	= domain_add();
			hnd	= z->abi->open_domain(z->hnd, &e->u.res_event.id);
			nz	= domain_add_zone(nd, z->abi, hnd);
			res->entry.DomainId = nd->did;
			dbg("Find a new domain!");
		}

		break;

	case OH_ET_RDR:
		res = get_res_by_oid(z->domain, e->u.rdr_event.parent);
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

struct oh_zone *domain_add_zone(struct oh_domain *domain,
		struct oh_abi_v1 *abi, void *hnd)
{
	struct oh_zone  *z;

	z = malloc(sizeof(*z));
	if (!z) {
		dbg("Cannot get memory!");
		return NULL;
	}
	
	memset(z, 0, sizeof(*z));
	
	list_add(&z->node, &domain->zone_list);
	list_init(&z->res_list);
	
	z->domain = domain;
	z->abi = abi;
	z->hnd = hnd;
	
	return z;
}
