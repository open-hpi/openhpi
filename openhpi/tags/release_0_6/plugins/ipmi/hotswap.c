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
 */

#include "ipmi.h"
#include <epath_utils.h>
#include <uid_utils.h>
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
                        state = SAHPI_HS_STATE_ACTIVE_HEALTHY;
                        break;
                case IPMI_HOT_SWAP_DEACTIVATION_REQUESTED:
                case IPMI_HOT_SWAP_DEACTIVATION_IN_PROGRESS:
                        state = SAHPI_HS_STATE_EXTRACTION_PENDING;
                        break;
                case IPMI_HOT_SWAP_OUT_OF_CON:
                        state = SAHPI_HS_STATE_ACTIVE_UNHEALTHY;
                        break;
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
                case SAHPI_HS_STATE_ACTIVE_HEALTHY:
                        state = IPMI_HOT_SWAP_ACTIVE;
                        break;
                case SAHPI_HS_STATE_EXTRACTION_PENDING:
                        state = IPMI_HOT_SWAP_DEACTIVATION_IN_PROGRESS;
                        break;
                case SAHPI_HS_STATE_ACTIVE_UNHEALTHY:
                        state = IPMI_HOT_SWAP_OUT_OF_CON;
                        break;
                default:
                        dbg("Unknown state: %d", hpi_state);
        }        
        return state;
}

static
int _get_hotswap_state(ipmi_entity_t    *ent,
                       enum ipmi_hot_swap_states last_state,
                       enum ipmi_hot_swap_states curr_state,
                       void                      *cb_data,
                       ipmi_event_t              *event)
{
        *(enum ipmi_hot_swap_states*)cb_data = curr_state;
        return IPMI_EVENT_NOT_HANDLED;
}

SaErrorT ohoi_get_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT *state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_id   *ohoi_res_id;
        enum ipmi_hot_swap_states       ipmi_state;

        int rv;

        handler = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        rv = ipmi_entity_id_get_hot_swap_state(ohoi_res_id->u.entity_id,
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
        const struct ohoi_resource_id   *ohoi_res_id;
        int done = 0;

        handler = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        switch (_hpi_to_ipmi_state_conv(state)) {
                case IPMI_HOT_SWAP_ACTIVE:
                         ipmi_entity_id_activate(ohoi_res_id->u.entity_id,
                                                 _hotswap_done,
                                                 &done);
                         break;
                case IPMI_HOT_SWAP_INACTIVE:
                         ipmi_entity_id_deactivate(ohoi_res_id->u.entity_id,
                                                   _hotswap_done,
                                                   &done);
                         break;
                default:
                         dbg("Unable set hotswap state: %d", state);
                         return SA_ERR_HPI_INVALID;
        }
                
        return ohoi_loop(&done);
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
        const struct ohoi_resource_id   *ohoi_res_id;
        struct ohoi_indicator_state     ipmi_state;

        SaErrorT rv;

        handler = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        ipmi_state.done = 0;
        ipmi_entity_id_get_hot_swap_indicator(ohoi_res_id->u.entity_id,
                                              _get_indicator_state,
                                              &ipmi_state);

        rv = ohoi_loop(&ipmi_state.done);
        if (rv != SA_OK)
                return rv;

        *state = ipmi_state.val;
        return SA_OK;;
}

SaErrorT ohoi_set_indicator_state(void *hnd, SaHpiResourceIdT id, 
				  SaHpiHsIndicatorStateT state)
{
        struct oh_handler_state         *handler;
        const struct ohoi_resource_id   *ohoi_res_id;
        int done;
        
        handler = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

        done = 0;
        ipmi_entity_id_set_hot_swap_indicator(ohoi_res_id->u.entity_id,
                                              state,
                                              _hotswap_done,                                                                
                                              &done);
        return ohoi_loop(&done);
}
