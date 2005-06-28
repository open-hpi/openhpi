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
 *	  Christina Hernandez <hernanc@us.ibm.com>
 *        W. David Ashley <dashley@us.ibm.com>
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

        state = g_malloc0(sizeof(struct oh_handler_state));
        if (!state) {
                dbg("out of memory");
                return NULL;
        }

        /* initialize rpt hashtable pointer */
        state->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
        oh_init_rpt(state->rptcache);

        /* initialize the event log */
        state->elcache = oh_el_create(256);
        if (!state->elcache) {
                dbg("Event log creation failed");
                g_free(state->rptcache);
                g_free(state);
                return NULL;
        }

        /* initialize the async event queue */
        if (!(state->eventq_async = g_async_queue_new())) {
                dbg("Async event log creation failed");
                g_free(state->rptcache);
                oh_el_close(state->elcache);
                g_free(state);
                return NULL;
        }

        /* save the handler config hash table it holds  */
        /* the openhpi.conf file config info            */
        state->config = handler_config;

        return (void *)state;
}


SaErrorT sim_discover(void *hnd)
{
	struct oh_handler_state *inst = (struct oh_handler_state *) hnd;
        int i = 0;

        /* discover resources (build the base rptcache) */
	while (sim_rpt_array[i].rpt.ResourceInfo.ManufacturerId != 0) {
                sim_inject_resource(inst, &sim_rpt_array[i].rpt, NULL,
                                    sim_rpt_array[i].comment);
                i++;
        }

	sim_discover_sensors(inst);
	sim_discover_controls(inst);
	sim_discover_annunciators(inst);

	return SA_OK;
}


int sim_get_event(void *hnd, struct oh_event *event)
{
	struct oh_handler_state *state = hnd;
	struct oh_event *e = NULL;

	if ((e = g_async_queue_try_pop(state->eventq_async))) {
		trace("retrieving sim event from async q");
		memcpy(event, e, sizeof(*event));
		event->did = oh_get_default_domain_id();
		g_free(e);
		return 1;
	} else {
		trace("no more events for sim instance");
        }
        return 0;
}


void sim_close(void *hnd)
{
	struct oh_handler_state *state = hnd;

        /* TODO: we may need to do more here than just this! */
//      g_free(state->rptcache);
        g_free(state);
        return;
}


/*
 * Simulator plugin interface
 *
 */

void * oh_open (GHashTable *) __attribute__ ((weak, alias("sim_open")));

void * oh_close (void *) __attribute__ ((weak, alias("sim_close")));

void * oh_get_event (void *, struct oh_event *)
                __attribute__ ((weak, alias("sim_get_event")));

void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("sim_discover")));

