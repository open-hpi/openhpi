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

int ohoi_get_control_state(ipmi_control_id_t	control_id,
			   SaHpiCtrlStateT	*state)
{
	int rv;

	rv = ipmi_control_pointer_cb(control_id, get_control_state, state);

	if (rv) {
		dbg("Unable to retrieve control state");
		return SA_ERR_HPI_ERROR;
	}
	return 0;
}
#endif

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
        struct ohoi_resource_id *ohoi_res_id;
	struct oh_handler_state *handler;
        int rv;
        
        if (act != SAHPI_COLD_RESET) {
                dbg("Only support cold reset");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler     = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support reset in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ipmi_control_pointer_cb(ohoi_res_id->reset_ctrl, 
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
        struct ohoi_resource_id *ohoi_res_id;
	struct oh_handler_state *handler;
        int rv;
        
        if (state != SAHPI_HS_POWER_OFF) {
                dbg("Only support power off");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler     = (struct oh_handler_state *)hnd;
        ohoi_res_id = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_id->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support power in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ipmi_control_pointer_cb(ohoi_res_id->power_ctrl, 
                                     set_power_state, NULL);
        if (rv) {
                dbg("Not support power in the entity");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        return SA_OK;
}
