/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
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
 *     Racing Guo <racing.guo@intel.com>
 */

#include "ipmi.h"
#include <oh_utils.h>
#include <string.h>


static 
SaHpiHsStateT _ipmi_to_hpi_state_conv(enum ipmi_hot_swap_states ipmi_state)
{
        SaHpiHsStateT state = 0;
        switch (ipmi_state) {
                case IPMI_HOT_SWAP_NOT_PRESENT:
                        state = SAHPI_HS_STATE_NOT_PRESENT;
                        break;
                case IPMI_HOT_SWAP_INACTIVE:
                        state = SAHPI_HS_STATE_INACTIVE;
                        break;
                case IPMI_HOT_SWAP_ACTIVATION_REQUESTED:
                case IPMI_HOT_SWAP_ACTIVATION_IN_PROGRESS:
                        state = SAHPI_HS_STATE_INSERTION_PENDING;
                        break;
                case IPMI_HOT_SWAP_ACTIVE:
                        state = SAHPI_HS_STATE_ACTIVE;
                        break;
                case IPMI_HOT_SWAP_DEACTIVATION_REQUESTED:
                case IPMI_HOT_SWAP_DEACTIVATION_IN_PROGRESS:
                        state = SAHPI_HS_STATE_EXTRACTION_PENDING;
                        break;
#if 0
                case IPMI_HOT_SWAP_OUT_OF_CON:
                        state = SAHPI_HS_STATE_ACTIVE_UNHEALTHY;
                        break;
#endif
                default:
                        dbg("Unknown state: %d", ipmi_state);
        }        
        return state;
}

static 
enum ipmi_hot_swap_states _hpi_to_ipmi_state_conv(SaHpiHsStateT hpi_state)
{
        enum ipmi_hot_swap_states state = 0;
        switch (hpi_state) {
                case SAHPI_HS_STATE_NOT_PRESENT:
                        state = IPMI_HOT_SWAP_NOT_PRESENT;
                        break;
                case SAHPI_HS_STATE_INACTIVE:
                        state = IPMI_HOT_SWAP_INACTIVE;
                        break;
                case SAHPI_HS_STATE_INSERTION_PENDING: 
                        state = IPMI_HOT_SWAP_ACTIVATION_IN_PROGRESS;
                        break;
                case SAHPI_HS_STATE_ACTIVE:
                        state = IPMI_HOT_SWAP_ACTIVE;
                        break;
                case SAHPI_HS_STATE_EXTRACTION_PENDING:
                        state = IPMI_HOT_SWAP_DEACTIVATION_IN_PROGRESS;
                        break;
                default:
                        dbg("Unknown state: %d", hpi_state);
        }        
        return state;
}

int ohoi_hot_swap_cb(ipmi_entity_t  *ent,
		     enum ipmi_hot_swap_states last_state,
                     enum ipmi_hot_swap_states curr_state,
		     void                      *cb_data,
		     ipmi_event_t              *event)
{
	struct oh_handler_state *handler =  (struct oh_handler_state*)cb_data;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)ipmi_handler;
	ipmi_entity_id_t entity_id;
	SaHpiRptEntryT  *rpt_entry;
	struct oh_event  *e;
	
	trace_ipmi("HotSwap Handler called");

	entity_id = ipmi_entity_convert_to_id(ent);
	
	rpt_entry = ohoi_get_resource_by_entityid(handler->rptcache, &entity_id);

	if (!rpt_entry) {
		dbg(" No rpt\n");
		return IPMI_EVENT_NOT_HANDLED;
	}
	e = malloc(sizeof(*e));
	if (!e) {
		dbg("Out of space");
		return IPMI_EVENT_NOT_HANDLED;
	}
	
	memset(e, 0, sizeof(*e));
	e->type = OH_ET_HPI;
	e->u.hpi_event.event.Source = rpt_entry->ResourceId;
	e->u.hpi_event.event.EventType = SAHPI_ET_HOTSWAP;
	/* Real severity is in the sensor event and not
	 * in the hotswap (presence) event itself
	 * We should ignore this but will set to SAHPI_INFORMATIONAL
	 */
	e->u.hpi_event.event.Severity = SAHPI_INFORMATIONAL;

	/* FIXME: Possible bug with OpenIPMI, event is always NULL so we don't get 
	 * timestamp?? 
	 */
	if (event != NULL) {
	      	e->u.hpi_event.event.Timestamp =
		    (SaHpiTimeT)ipmi_event_get_timestamp(event);
	} else
	      	e->u.hpi_event.event.Timestamp = SAHPI_TIME_UNSPECIFIED;
	      	
	
	e->u.hpi_event.event.EventDataUnion.HotSwapEvent.HotSwapState
			= _ipmi_to_hpi_state_conv(curr_state);
	e->u.hpi_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState 
			= _ipmi_to_hpi_state_conv(last_state);

	if (e->u.hpi_event.event.EventDataUnion.HotSwapEvent.HotSwapState ==
	    					SAHPI_HS_STATE_NOT_PRESENT) {
		trace_ipmi("HS_STATE NOT PRESENT, removing RPT");
	  	handler->eventq = g_slist_append(handler->eventq,e);
	}else if (e->u.hpi_event.event.EventDataUnion.HotSwapEvent
		  	.HotSwapState == SAHPI_HS_STATE_ACTIVE) {
		trace_ipmi("HS_STATE ACTIVE");
		handler->eventq = g_slist_append(handler->eventq, e);
	}else {
	  	handler->eventq = g_slist_append(handler->eventq, e);
	}

	return IPMI_EVENT_HANDLED;
}

static
void _get_hotswap_state(ipmi_entity_t             *ent,
                       int                       err,
                       enum ipmi_hot_swap_states state,
                       void                      *cb_data)
{
        *(enum ipmi_hot_swap_states*)cb_data = state;
}

SaErrorT ohoi_get_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT *state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_info   *ohoi_res_info;
        enum ipmi_hot_swap_states       ipmi_state;

        int rv;

        handler = (struct oh_handler_state *)hnd;
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        rv = ipmi_entity_id_get_hot_swap_state(ohoi_res_info->u.entity_id,
                                               _get_hotswap_state,
                                               &ipmi_state);
        
        if (rv) {
                dbg("Unable to get hotswap state: %d", rv);
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        *state = _ipmi_to_hpi_state_conv(ipmi_state);        
        return SA_OK;
}

static void _hotswap_done(ipmi_entity_t *ent,
                          int           err,
                          void          *cb_data)
{
        *(int *)cb_data = 1;
}

SaErrorT ohoi_set_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_info   *ohoi_res_info;
        int done = 0;

        handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = handler->data;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);

        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        switch (_hpi_to_ipmi_state_conv(state)) {
                case IPMI_HOT_SWAP_ACTIVE:
                         ipmi_entity_id_activate(ohoi_res_info->u.entity_id,
                                                 _hotswap_done,
                                                 &done);
                         break;
                case IPMI_HOT_SWAP_INACTIVE:
                         ipmi_entity_id_deactivate(ohoi_res_info->u.entity_id,
                                                   _hotswap_done,
                                                   &done);
                         break;
                default:
                         dbg("Unable set hotswap state: %d", state);
                         return SA_ERR_HPI_INVALID_CMD;
        }
                
        return ohoi_loop(&done, ipmi_handler);
}

SaErrorT ohoi_request_hotswap_action(void *hnd, SaHpiResourceIdT id, 
                                     SaHpiHsActionT act)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

struct ohoi_indicator_state {
        int done;
        int val;
};

static 
void _get_indicator_state(ipmi_entity_t *ent,
                          int           err,
                          int           val,
                          void          *cb_data)
{
        struct ohoi_indicator_state *state;
        state = cb_data;
        
        state->done = 1;
        state->val  = val;
}

SaErrorT ohoi_get_indicator_state(void *hnd, SaHpiResourceIdT id, 
                                  SaHpiHsIndicatorStateT *state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_info   *ohoi_res_info;
        struct ohoi_indicator_state     ipmi_state;
	
        SaErrorT rv;

        handler = (struct oh_handler_state *)hnd;
		struct ohoi_handler *ipmi_handler = handler->data;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get HS in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        ipmi_state.done = 0;
        rv = ipmi_entity_id_get_hot_swap_indicator(ohoi_res_info->u.entity_id,
							_get_indicator_state,
							&ipmi_state);
	if(rv) {
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

        rv = ohoi_loop(&ipmi_state.done, ipmi_handler);
        if (rv != SA_OK) {
                return rv;
	}

        *state = ipmi_state.val;
        return SA_OK;;
}

SaErrorT ohoi_set_indicator_state(void *hnd, SaHpiResourceIdT id, 
				  SaHpiHsIndicatorStateT state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_info   *ohoi_res_info;
        int done;
        
        handler = (struct oh_handler_state *)hnd;
		struct ohoi_handler *ipmi_handler = handler->data;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        done = 0;
        ipmi_entity_id_set_hot_swap_indicator(ohoi_res_info->u.entity_id,
                                              state,
                                              _hotswap_done,                                                                
                                              &done);
        return ohoi_loop(&done,ipmi_handler);
}



void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("ohoi_get_hotswap_state")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("ohoi_set_hotswap_state")));

void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("ohoi_request_hotswap_action")));

void * oh_get_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT *)
                __attribute__ ((weak, alias("ohoi_get_indicator_state")));

void * oh_set_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("ohoi_set_indicator_state")));

