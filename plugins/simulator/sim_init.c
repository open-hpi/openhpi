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
 *
 */

#include <sim_init.h>

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
                printf("g_async_queue_new failed\n");
                g_free(state);
                return NULL;
        }
        
        return ( (void *) state);
}

SaErrorT sim_discover(void *hnd) 
{
        return SA_OK;
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
