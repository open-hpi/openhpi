/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague
 *	Christina Hernandez
 *
 */

#include <sim_init.h>
//#include <math.h>

void *sim_open(GHashTable *handler_config)
{
        struct oh_handler_state *state = NULL;
        char *tok = NULL;
        if (!handler_config) {
                dbg("GHashTable *handler_config is NULL!");
                return NULL;
        }
        tok = g_hash_table_lookup(handler_config, "entity_root");
        
        if (!tok) {
                dbg("entity_root is needed and not present");
                return NULL;
        }

        trace("%s, %s, %s",
              (char *)g_hash_table_lookup(handler_config, "plugin"),
              (char *)g_hash_table_lookup(handler_config, "name"),
              tok);
        
        
                                           
        state = g_malloc0(sizeof(*state));
        if (!state) {
                dbg("out of memory");
                return NULL;
        }
        
        /* save the handler config has table it holds   */
        /* the openhpi.conf file config info            */
        state->config = handler_config;

        /* initialize hashtable pointer */
        state->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
        oh_init_rpt(state->rptcache);

        if ( !(state->eventq_async = g_async_queue_new()) ) {
                dbg("g_async_queue_new failed\n");
                g_free(state);
                return NULL;
        }
       

        return ( (void *) state);
}





SaErrorT sim_discover(void *hnd)
{

	struct oh_handler_state *inst = (struct oh_handler_state *) hnd;
	struct oh_event event;
	SaHpiRptEntryT *rpt_entry;
	SaHpiEntityPathT root_ep;
	char *entity_root;
	SaHpiRdrT *rdr_entry;

	

	struct oh_handler_state *oh_hnd = hnd;
	entity_root=(char *)g_hash_table_lookup(oh_hnd->config,"entity_root");
	oh_encode_entitypath (entity_root, &root_ep);

        build_rptcache(inst->rptcache, &root_ep);
	sim_discover_sensors(inst->rptcache);
		
	rpt_entry = oh_get_resource_next(inst->rptcache, SAHPI_FIRST_ENTRY);

	while (rpt_entry) {
		dbg("here resource event id %d", rpt_entry->ResourceId);
		memset(&event, 0, sizeof(event));
		event.type = OH_ET_RESOURCE;
		event.did = oh_get_default_domain_id();
		memcpy(&event.u.res_event.entry, rpt_entry, sizeof(SaHpiRptEntryT));
		g_async_queue_push(inst->eventq_async, eventdup(&event));

		rdr_entry = oh_get_rdr_next(inst->rptcache, rpt_entry->ResourceId, SAHPI_FIRST_ENTRY);
		if(!rdr_entry){printf("I don't work\n");}
		while (rdr_entry) {
			printf("i am making an event out of an rdr \n");
			/*dbg("here rdr event id %d", rdr_entry->RecordId);*/
			memset(&event, 0, sizeof(event));
			event.type = OH_ET_RDR;
			event.u.rdr_event.parent = rpt_entry->ResourceId;
			memcpy(&event.u.rdr_event.rdr, rdr_entry, sizeof(SaHpiRdrT));
			g_async_queue_push(inst->eventq_async, eventdup(&event));
	                
			rdr_entry = oh_get_rdr_next(inst->rptcache, rpt_entry->ResourceId, rdr_entry->RecordId);
		}
								
		rpt_entry = oh_get_resource_next(inst->rptcache, rpt_entry->ResourceId);	
        }
	
	return 0;
}



int sim_get_event(void *hnd, struct oh_event *event)
{
	struct oh_handler_state *handle = hnd;
	struct oh_event *e = NULL;

	if ( (e = g_async_queue_try_pop(handle->eventq_async)) ) {
		trace("retrieving sim event from async q");
		memcpy(event, e, sizeof(*event));		
		event->did = oh_get_default_domain_id(); 		
		g_free(e);
		return 1;
	} else {
		trace("no more events for sim instance");
                return 0;
        }
}

void sim_close(void *hnd) 
{
        /* TODO: clean up state */
        return;
}


SaErrorT build_rptcache(RPTable *rptcache, SaHpiEntityPathT *root_ep) 
{
	int i;
	SaHpiRptEntryT res;
	int x = 0;
	SaHpiTextBufferT build_name;
	struct oh_event *e;
		
        e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));

	//e->u.res_event.entry.ResourceEntity = root_ep;
	//e->u.res_event.entry.ResourceId = oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
			
	while (dummy_rpt_array[x].rpt.ResourceInfo.ManufacturerId != 0){
		x++;
	}

	for(i=0; i<x; i++){

        	memcpy(&res, &dummy_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
		oh_concat_ep(&res.ResourceEntity, root_ep);
		res.ResourceId = oh_uid_from_entity_path(&res.ResourceEntity);
		printf("I am res.ResourceId %d\ni", res.ResourceId);	
		dbg("Adding resource number %d",i);

		oh_append_textbuffer(&build_name, dummy_rpt_array[i].comment);
//		printf(&res.ResourceTag, "%s I am resourcetag\n");
		dummy_create_resourcetag(&res.ResourceTag, (char*)build_name.Data, root_ep->Entry[i].EntityLocation);
		printf("I actually run dummy_create_resourcetag\n");
		oh_add_resource(rptcache, &res, NULL, FREE_RPT_DATA);
								
	}
        return i;
}
	


struct oh_event *eventdup(const struct oh_event *event)
{
        struct oh_event *e;
        e = g_malloc0(sizeof(*e));
        if (!e) {
                dbg("Out of memory!");
                return NULL;
        }
        memcpy(e, event, sizeof(*e));
        return e;
}


/*
 * Simulator function table for plugin interface 
 *
 */

static struct oh_abi_v2 oh_sim_plugin = {
        .open                   = sim_open,
        .close                  = sim_close,
        .get_event              = sim_get_event,
        .discover_resources     = sim_discover
};

/* removes the warning about no previous declaration */
int sim_get_interface(void **pp, const uuid_t uuid);

int sim_get_interface(void **pp, const uuid_t uuid)
{
        if (uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
                *(struct oh_abi_v2 **)pp = &oh_sim_plugin;
                return 0;
        }

        *pp = NULL;
        return -1;
}

int get_interface(void **pp, const uuid_t uuid) __attribute__ ((weak, alias("sim_get_interface")));

