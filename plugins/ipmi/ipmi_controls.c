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
 *     Tariq Shureih    <tariq.shureih@intel.com>
 *     Louis Zhuang     <louis.zhuang@intel.com>
 */

#include "ipmi.h"

#if 0
static void get_control_state(ipmi_control_t	*control,
				void		*cb_data)
{
	//SaHpiCtrlStateT ctrl_state;
	//int rv;


	dbg("Under construction");


}
#endif

struct ohoi_control_info {
        int done;
        SaHpiCtrlStateT *state;
};

static void __get_control_state(ipmi_control_t *control,
                                int            err,
                                int            *val,
                                void           *cb_data)
{
        struct ohoi_control_info *info = cb_data;

        info->done = 1;
        if (info->state->Type != SAHPI_CTRL_TYPE_OEM) {
                dbg("IPMI plug-in only support OEM control now");
                return;
        }
        
        info->state->StateUnion.Oem.BodyLength 
                = ipmi_control_get_num_vals(control);
        memcpy(&info->state->StateUnion.Oem.Body[0],
               val,
               info->state->StateUnion.Oem.BodyLength);
}

static void _get_control_state(ipmi_control_t *control,
                                void           *cb_data)
{
        ipmi_control_get_val(control, __get_control_state, cb_data);
}

SaErrorT ohoi_get_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlStateT *state)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_control_info info;
	SaErrorT         rv;
	ipmi_control_id_t *ctrl;

        rv = ohoi_get_rdr_data(hnd, id, SAHPI_CTRL_RDR, num, (void *)&ctrl);
        if (rv!=SA_OK)
                return rv;

	memset(state, 0, sizeof(*state));
	
        info.done  = 0;
        info.state = state;
        info.state->Type = SAHPI_CTRL_TYPE_OEM;
        
        rv = ipmi_control_pointer_cb(*ctrl, _get_control_state, &info);
	if (rv) {
		dbg("Unable to retrieve control state");
		return SA_ERR_HPI_ERROR;
	}

        return ohoi_loop(&info.done, ipmi_handler);
}

static void __set_control_state(ipmi_control_t *control,
                                int            err,
                                void           *cb_data)
{
       struct ohoi_control_info *info = cb_data;
       info->done = 1;
}

static void _set_control_state(ipmi_control_t *control,
                                void           *cb_data)
{
        struct ohoi_control_info *info = cb_data;
        
        if (info->state->StateUnion.Oem.BodyLength 
                        != ipmi_control_get_num_vals(control)) {
                dbg("control number is not equal to supplied data");
                info->done = -1;
                return;
        }
                        
        ipmi_control_set_val(control, 
                             (void *)&info->state->StateUnion.Oem.Body[0],
                             __set_control_state, info);
}

SaErrorT ohoi_set_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlStateT *state)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_control_info info;
	SaErrorT         rv;
	ipmi_control_id_t *ctrl;

        rv = ohoi_get_rdr_data(hnd, id, SAHPI_CTRL_RDR, num, (void *)&ctrl);
        if (rv!=SA_OK)
                return rv;
	
        info.done  = 0;
        info.state = state;
        if (info.state->Type != SAHPI_CTRL_TYPE_OEM) {
                dbg("IPMI only support OEM control");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        rv = ipmi_control_pointer_cb(*ctrl, _set_control_state, &info);
	if (rv) {
		dbg("Unable to retrieve control state");
		return SA_ERR_HPI_ERROR;
	}

        if (info.done<0) {
                dbg("Invalid control state data");
                return SA_ERR_HPI_INVALID;
        }
        
        return ohoi_loop(&info.done, ipmi_handler);
}

static void set_reset_state(ipmi_control_t *control,
                            void           *cb_data)
{
        int val=1;

        /* Just cold reset the entity*/
        ipmi_control_set_val(control, &val, NULL, NULL);
}

SaErrorT ohoi_set_reset_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiResetActionT act)
{
        struct ohoi_resource_info *ohoi_res_info;
	struct oh_handler_state *handler;
        int rv;
        
        if (act != SAHPI_COLD_RESET) {
                dbg("Only support cold reset");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler     = (struct oh_handler_state *)hnd;
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support reset in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ipmi_control_pointer_cb(ohoi_res_info->reset_ctrl, 
                                     set_reset_state, NULL);
        if (rv) {
                dbg("Not support reset in the entity");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        return SA_OK;
}

static void set_power_state(ipmi_control_t *control,
                            void           *cb_data)
{
        int val=0;

        /* Just cold reset the entity*/
        ipmi_control_set_val(control, &val, NULL, NULL);
}

SaErrorT ohoi_set_power_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiHsPowerStateT state)
{
        struct ohoi_resource_info *ohoi_res_info;
	struct oh_handler_state *handler;
        int rv;
        
        if (state != SAHPI_HS_POWER_OFF) {
                dbg("Only support power off");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler     = (struct oh_handler_state *)hnd;
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support power in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
                                     set_power_state, NULL);
        if (rv) {
                dbg("Not support power in the entity");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        return SA_OK;
}
