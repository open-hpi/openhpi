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

static unsigned char oem_alarm_state = 0xff;

struct ohoi_control_info {
        int done;
        SaHpiCtrlStateT *state;
};

/* struct for getting power state */
struct ohoi_power_info {
	int done;
	SaHpiPowerStateT *state;
};


static void __get_control_state(ipmi_control_t *control,
                                int            err,
                                int            *val,
                                void           *cb_data)
{
        struct ohoi_control_info *info = cb_data;

        if (info->state->Type != SAHPI_CTRL_TYPE_OEM) {
                dbg("IPMI plug-in only support OEM control now");
                return;
        }
        
        info->state->StateUnion.Oem.BodyLength 
                = ipmi_control_get_num_vals(control);
        memcpy(&info->state->StateUnion.Oem.Body[0],
               val,
               info->state->StateUnion.Oem.BodyLength);
        info->done = 1;
}

static void _get_control_state(ipmi_control_t *control,
                                void           *cb_data)
{
        ipmi_control_get_val(control, __get_control_state, cb_data);
}

SaErrorT ohoi_get_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlModeT *mode,
                                SaHpiCtrlStateT *state)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_control_info info;
	SaErrorT         rv;
	ipmi_control_id_t *ctrl;
	SaHpiRdrT * rdr;
	SaHpiUint8T val, mask, idx, i;

	rdr = oh_get_rdr_by_type(handler->rptcache, id, SAHPI_CTRL_RDR, num);
	if (!rdr) return SA_ERR_HPI_INVALID_RESOURCE;
        rv = ohoi_get_rdr_data(hnd, id, SAHPI_CTRL_RDR, num, (void *)&ctrl);
        if (rv!=SA_OK) return rv;

	memset(state, 0, sizeof(*state));
	
        info.done  = 0;
        info.state = state;
        info.state->Type = SAHPI_CTRL_TYPE_OEM;
        
        rv = ipmi_control_pointer_cb(*ctrl, _get_control_state, &info);
	if (rv) {
		dbg("Unable to retrieve control state");
		return SA_ERR_HPI_ERROR;
	}

	rv = ohoi_loop(&info.done, ipmi_handler);
	val = info.state->StateUnion.Oem.Body[0];

	if ((rdr->RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL) &&
	    (rdr->RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_LED) &&
	    (rdr->RdrTypeUnion.CtrlRec.Oem >= OEM_ALARM_BASE)) {
		oem_alarm_state = val;
		/* This is a front panel alarm LED. */
		state->Type = SAHPI_CTRL_TYPE_DIGITAL;	
		mask = 0x01;
		idx = rdr->RdrTypeUnion.CtrlRec.Oem - OEM_ALARM_BASE;
		/* bits 0 - 3 = Pwr, Crit, Maj, Min */
		for (i = 0; i < idx; i++) mask = mask << 1;
		if ((val & mask) == 0) 
		  state->StateUnion.Digital = SAHPI_CTRL_STATE_ON;
		else 
		  state->StateUnion.Digital = SAHPI_CTRL_STATE_OFF;
	} 
	return(rv);
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
                             (int *)&info->state->StateUnion.Oem.Body[0],
                             __set_control_state, info);
}

SaErrorT ohoi_set_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlModeT mode,
                                SaHpiCtrlStateT *state)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_control_info info;
	SaErrorT         rv;
	ipmi_control_id_t *ctrl;
	SaHpiRdrT * rdr;
	SaHpiUint8T val, mask, idx, i;

	rdr = oh_get_rdr_by_type(handler->rptcache, id, SAHPI_CTRL_RDR, num);
	if (!rdr) return SA_ERR_HPI_INVALID_RESOURCE;
        rv = ohoi_get_rdr_data(hnd, id, SAHPI_CTRL_RDR, num, (void *)&ctrl);
        if (rv!=SA_OK) return rv;
	
	if ((rdr->RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL) &&
            (rdr->RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_LED) &&
            (rdr->RdrTypeUnion.CtrlRec.Oem >= OEM_ALARM_BASE)) {
                /* This is a front panel alarm LED. */
                val = oem_alarm_state;
                val |= 0xf0;  /* h.o. nibble always write 1 */
                mask = 0x01;
                idx = rdr->RdrTypeUnion.CtrlRec.Oem - OEM_ALARM_BASE;
                /* bits 0 - 3 = Pwr, Crit, Maj, Min */
                for (i = 0; i < idx; i++) mask = mask << 1;
                if (state->StateUnion.Digital == SAHPI_CTRL_STATE_ON)
                        val &= (0xff - mask);  /*turn it on */
                else   /* turn it off */
                        val |= mask;
                state->Type = SAHPI_CTRL_TYPE_OEM;
                state->StateUnion.Oem.BodyLength = 1;
                state->StateUnion.Oem.Body[0] = val;
        }
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

        ohoi_loop(&info.done, ipmi_handler);

	return SA_OK;
}

static void reset_done (ipmi_control_t *ipmi_control,
			int err,
			void *cb_data)
{
	int *reset_flag = cb_data;
	*reset_flag = 1;
}

static void set_reset_state(ipmi_control_t *control,
                            void           *cb_data)
{
        int val=1;

        /* Just cold reset the entity*/
        ipmi_control_set_val(control, &val, reset_done, cb_data);
}

SaErrorT ohoi_set_reset_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiResetActionT act)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	
        struct ohoi_resource_info *ohoi_res_info;
        
	int rv;
	int reset_flag = 0;	/* reset_flag = 1 means reset is done */
        
        if (act != SAHPI_COLD_RESET) {
                dbg("Only support cold reset");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support reset in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ipmi_control_pointer_cb(ohoi_res_info->reset_ctrl, 
                                     set_reset_state, &reset_flag);
        if (rv) {
                dbg("Not support reset in the entity");
                return SA_ERR_HPI_INVALID_CMD;
        }

	/* wait until reset_done is called to exit this function */
	ohoi_loop(&reset_flag, ipmi_handler);
        
        return SA_OK;
}

static void power_done (ipmi_control_t *ipmi_control,
                        int err,
                        void *cb_data)
{
	
	struct ohoi_power_info *power_info = cb_data;

	power_info->done = 1;
}

static void set_power_state_on(ipmi_control_t *control,
                            void           *cb_data)
{
	struct ohoi_power_info *power_info = cb_data;

        ipmi_control_set_val(control, (int *)power_info->state, power_done, cb_data);
}

static void set_power_state_off(ipmi_control_t *control,
                            void           *cb_data)
{
	struct ohoi_power_info *power_info = cb_data;

        ipmi_control_set_val(control, (int *)power_info->state, power_done, cb_data);
}

SaErrorT ohoi_set_power_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiPowerStateT state)
{
        struct ohoi_resource_info *ohoi_res_info;

	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_power_info power_info;

        int rv;
	power_info.done = 0;
	power_info.state = &state;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support power in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }
	
	switch (state) {
		case SAHPI_POWER_ON:
			rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
						    set_power_state_on, &power_info);
			if (rv) {
				dbg("set_power_state_on failed");
				return SA_ERR_HPI_INVALID_CMD;
			}
			break;
		case SAHPI_POWER_OFF:
			rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
                                                    set_power_state_off, &power_info);
	                if (rv) {
        	            dbg("set_power_state_off failed");
                	    return SA_ERR_HPI_INVALID_CMD;
                	}
			break;
		default:
			dbg("Invalid power state requested");
			return SA_ERR_HPI_INVALID_PARAMS;
	}

        ohoi_loop(&power_info.done, ipmi_handler);
        
        return SA_OK;
}

static void get_power_control_val (ipmi_control_t *control,
			     int err,
			     int *val,
			     void *cb_data)
{
	struct ohoi_power_info *info = cb_data;

	if (*val == 0) {
		dbg("Power Control Value: %d", *val);
		*info->state = SAHPI_POWER_OFF;
	} else if (*val == 1) {
		dbg("Power Control Value: %d", *val);
		*info->state = SAHPI_POWER_ON;
	} else {
		dbg("invalid power state");
	}
	info->done = 1;

}

static void get_power_state (ipmi_control_t *ipmi_control,
			     void *cb_data)
{
	int rv;

	rv = ipmi_control_get_val(ipmi_control, get_power_control_val, cb_data);

	if(rv) {
		dbg("reading ipmi control value failed");
	}
	

}

SaErrorT ohoi_get_power_state (void *hnd,
                               SaHpiResourceIdT	id,
                               SaHpiPowerStateT *state)
{
        struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_resource_info *ohoi_res_info;
	struct ohoi_power_info power_state;

	int rv;

	power_state.done = 0;
	power_state.state = state;
	
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support power in MC");
                return SA_ERR_HPI_INVALID_CMD;
        }

	rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl,
					get_power_state, &power_state);
	if (rv) {
		dbg("get_power_state failed");
		return SA_ERR_HPI_INVALID_CMD;
	}

	dbg("waiting for OIPMI to return");
	ohoi_loop(&power_state.done, ipmi_handler);
				     
        return SA_OK;
}
