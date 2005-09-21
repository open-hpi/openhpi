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


/* This list maintains a list of all handler state structs. It is used by
   to determine the name of a handler so that the pointer to the handler state
   can be returned to an injector API.
 */
GSList *sim_handler_states = NULL;


void *sim_open(GHashTable *handler_config)
{
        struct oh_handler_state *state = NULL;
        char *tok = NULL;

        if (!handler_config) {
                dbg("GHashTable *handler_config is NULL!");
                return NULL;
        }
        /* check for required hash table entries */
        tok = g_hash_table_lookup(handler_config, "entity_root");
        if (!tok) {
                dbg("entity_root is needed and not present in conf");
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

        /* save the handler config hash table, it holds  */
        /* the openhpi.conf file config info             */
        state->config = handler_config;

        /* save the handler state to our list */
        sim_handler_states = g_slist_append(sim_handler_states, state);

        return (void *)state;
}


SaErrorT sim_discover(void *hnd)
{
        /* NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!
           Since the simulator uses the full managed hot swap model and we
           do not have any latency issues, discovery only needs to be performed
           one time for each handler instance. Subsequent calls should just
           return SA_OK for that instance.
         */
	struct oh_handler_state *inst = (struct oh_handler_state *) hnd;
        int i;
        SaHpiRptEntryT res;
        GThread *service_thrd;

        /* We use the inst->data variable to store the initial discovery state
           for an instance of the handler.
         */
        if (inst->data) {
                return SA_OK;
        }

        /* ---------------------------------------------------------------
           The following assumes that the resource array is in a specific
           order. Changing this order means changing some of this code.
           ------------------------------------------------------------ */

        /* discover chassis resources and RDRs */
        i = SIM_RPT_ENTRY_CHASSIS - 1;
        memcpy(&res, &sim_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
        sim_inject_resource(inst, &res, NULL,
                            sim_rpt_array[i].comment);
	sim_discover_chassis_sensors(inst, res.ResourceId);
	sim_discover_chassis_controls(inst, res.ResourceId);
	sim_discover_chassis_annunciators(inst, res.ResourceId);
	sim_discover_chassis_watchdogs(inst, res.ResourceId);
	sim_discover_chassis_inventory(inst, res.ResourceId);

        /* discover cpu resources and RDRs */
        i = SIM_RPT_ENTRY_CPU - 1;
        memcpy(&res, &sim_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
        sim_inject_resource(inst, &res, NULL,
                            sim_rpt_array[i].comment);
	sim_discover_cpu_sensors(inst, res.ResourceId);
	sim_discover_cpu_controls(inst, res.ResourceId);
	sim_discover_cpu_annunciators(inst, res.ResourceId);
	sim_discover_cpu_watchdogs(inst, res.ResourceId);
	sim_discover_cpu_inventory(inst, res.ResourceId);

        /* discover dasd resources and RDRs */
        i = SIM_RPT_ENTRY_DASD - 1;
        memcpy(&res, &sim_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
        sim_inject_resource(inst, &res, NULL,
                            sim_rpt_array[i].comment);
	sim_discover_dasd_sensors(inst, res.ResourceId);
	sim_discover_dasd_controls(inst, res.ResourceId);
	sim_discover_dasd_annunciators(inst, res.ResourceId);
	sim_discover_dasd_watchdogs(inst, res.ResourceId);
	sim_discover_dasd_inventory(inst, res.ResourceId);

        /* discover hot swap dasd resources and RDRs */
        i = SIM_RPT_ENTRY_HS_DASD - 1;
        memcpy(&res, &sim_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
        sim_inject_resource(inst, &res, NULL,
                            sim_rpt_array[i].comment);
        sim_discover_hs_dasd_sensors(inst, res.ResourceId);
        sim_discover_hs_dasd_controls(inst, res.ResourceId);
        sim_discover_hs_dasd_annunciators(inst, res.ResourceId);
        sim_discover_hs_dasd_watchdogs(inst, res.ResourceId);
        sim_discover_hs_dasd_inventory(inst, res.ResourceId);

        /* discover fan resources and RDRs */
        i = SIM_RPT_ENTRY_FAN - 1;
        memcpy(&res, &sim_rpt_array[i].rpt, sizeof(SaHpiRptEntryT));
        sim_inject_resource(inst, &res, NULL,
                            sim_rpt_array[i].comment);
	sim_discover_fan_sensors(inst, res.ResourceId);
	sim_discover_fan_controls(inst, res.ResourceId);
	sim_discover_fan_annunciators(inst, res.ResourceId);
	sim_discover_fan_watchdogs(inst, res.ResourceId);
	sim_discover_fan_inventory(inst, res.ResourceId);

        /* now start the message queue thread for reading events */
        service_thrd = start_injector_service_thread(NULL);
        if (service_thrd == NULL) {
                trace("injector service thread not started");
        }

        /* Let subsequent discovery invocations know that discovery has already
           been performed.
         */
        inst->data = (void *)1;
	return SA_OK;
}


/*
 * Return values:
 * 1 - events to be processed.
 * SA_OK - No events to be processed.
 * SA_ERR_HPI_INVALID_PARAMS - @event is NULL.
 */
SaErrorT sim_get_event(void *hnd, struct oh_event *event)
{
	struct oh_handler_state *state = hnd;
	struct oh_event *e = NULL;

	if ((e = g_async_queue_try_pop(state->eventq_async))) {
		trace("retrieving sim event from async q");
		memcpy(event, e, sizeof(*event));
		event->did = oh_get_default_domain_id();

//		g_free(e);

		return 1;
	} else {
		trace("no more events for sim instance");
        }
        return 0;
}


SaErrorT sim_close(void *hnd)
{
	struct oh_handler_state *state = hnd;

        /* TODO: we may need to do more here than just this! */
//      g_free(state->rptcache);
        g_free(state);
        return 0;
}

SaErrorT sim_set_resource_tag(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRptEntryT *resource = NULL;

        if (!tag)
                return SA_ERR_HPI_INVALID_PARAMS;

        resource = oh_get_resource_by_id(inst->rptcache, id);
        if (!resource) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(&resource->ResourceTag, tag, sizeof(SaHpiTextBufferT));

        return SA_OK;
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

void * oh_set_resource_tag (void *, SaHpiResourceIdT, SaHpiTextBufferT *)
                __attribute__ ((weak, alias("sim_set_resource_tag")));

