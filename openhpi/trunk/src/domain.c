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
 *     Sean Dague <sean@dague.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

/* sd: I think we need global mappings here, we'll need to 
   address this issue later */
#if 0
static SaHpiDomainIdT dcounter = SAHPI_DEFAULT_DOMAIN_ID;
#endif

#if 0
int reset_domain_list(void) 
{
        SaHpiDomainIdT *temp;
        int i;

        for (i = 0; i < g_slist_length(global_domain_list); i++) {
                temp = (SaHpiDomainIdT*) g_slist_nth_data(global_domain_list, i);
                free(temp);
        }
        g_slist_free(global_domain_list);
        global_domain_list = NULL;

        return 0;
}

#endif

GSList *global_domain_list = NULL;

int domain_exists(SaHpiDomainIdT did) 
{
        SaHpiDomainIdT *temp;
        int i;
        
        for (i = 0; i < g_slist_length(global_domain_list); i++) {
                temp = (SaHpiDomainIdT*) g_slist_nth_data(global_domain_list, i);
                if(*temp == did) {
                        return 1;
                }
        }
        return 0;
}

int domain_add(SaHpiDomainIdT did)
{
        SaHpiDomainIdT *temp;
        temp = calloc(1,sizeof(*temp));
        memcpy(temp, &did, sizeof(did));
        
        if(domain_exists(did) > 0) {
                dbg("Domain %d exists already, something is fishy", did);
                return -1;
        }
        global_domain_list = g_slist_append(global_domain_list, (gpointer *) temp);
        
        return 0;
}

int domain_del(SaHpiDomainIdT did)
{
	/* FIXME cleaup resources in domain */
        return 0;
}

#if 0
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
#endif
