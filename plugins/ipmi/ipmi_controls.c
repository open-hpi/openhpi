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
	SaErrorT err;
        SaHpiCtrlStateT *state;
};

/* struct for getting power state */
struct ohoi_power_info {
	int done;
	SaErrorT err;
	SaHpiPowerStateT *state;
};

/* struct for getting reset state */
struct ohoi_reset_info {
	int done;
	SaErrorT err;
	SaHpiResetActionT *state;
};

static void __get_control_state(ipmi_control_t *control,
                                int            err,
                                int            *val,
                                void           *cb_data)
{
        struct ohoi_control_info *info = cb_data;

	if (err || !val) {
		dbg("__get_control_state: err = %d; val = %p", err, val);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
		return;
	}
       if (info->state->Type != SAHPI_CTRL_TYPE_OEM) {
                dbg("IPMI plug-in only support OEM control now");
                return;
        }
        
	if (err || !val) {
		dbg("__get_control_state: err = %d; val = %p", err, val);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
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
	SaHpiCtrlStateT localstate;
	SaHpiCtrlModeT  localmode;

	rdr = oh_get_rdr_by_type(handler->rptcache, id, SAHPI_CTRL_RDR, num);
	if (!rdr) return SA_ERR_HPI_INVALID_RESOURCE;
        rv = ohoi_get_rdr_data(hnd, id, SAHPI_CTRL_RDR, num, (void *)&ctrl);
        if (rv!=SA_OK) return rv;

	if (state == NULL) {
		state = &localstate;
	}
	if (mode == NULL) {
		mode = &localmode;
	}
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
	if (err) {
		dbg("__set_control_state: err = %d", err);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
	}
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

static void reset_resource_done (ipmi_control_t *ipmi_control,
				 int err,
				 void *cb_data)
{
	struct ohoi_reset_info *info = cb_data;
	info->done = 1;
	if (err) {
		dbg("reset_resource_done: err = %d", err);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
	}
}

static void set_resource_reset_state(ipmi_control_t *control,
                            void           *cb_data)
{
        struct ohoi_reset_info *info = cb_data;
	int val = 1;
	int rv;

        /* Just cold reset the entity*/
        rv = ipmi_control_set_val(control, &val, reset_resource_done, cb_data);
	if (rv) {
		dbg("ipmi_control_set_val returned err = %d", rv);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
	}
}


static void reset_mc_done (ipmi_mc_t *mc,
			int err,
			void *cb_data)
{
	struct ohoi_reset_info *info = cb_data;
	info->done = 1;
	if (err) {
		dbg("reset_mc_done err = %d", err);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
	}
}

static void set_mc_reset_state(ipmi_mc_t *mc,
                            void           *cb_data)
{
        struct ohoi_reset_info *info = cb_data;
	int rv;
	int act;

	if (*info->state == SAHPI_COLD_RESET) {
		act = IPMI_MC_RESET_COLD;
	} else if (*info->state == SAHPI_WARM_RESET) {
		act = IPMI_MC_RESET_WARM;
	} else {
		info->err = SA_ERR_HPI_INVALID_CMD;
		info->done = 1;
		return;
	}		
        rv = ipmi_mc_reset(mc, act, reset_mc_done, cb_data);
	if (rv) {
		dbg("ipmi_mc_reset returned err = %d", rv);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
	}
}



SaErrorT ohoi_set_reset_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiResetActionT act)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_reset_info info;	
        struct ohoi_resource_info *ohoi_res_info;
        
	int rv;
        
	info.done = 0;
	info.err = 0;
	if ((act == SAHPI_COLD_RESET) || (act == SAHPI_WARM_RESET)) {
	      	dbg("ResetAction requested: %d", act);
	      	info.state = &act;
	} else {
	      	dbg("Currently we only support cold and warm reset");
		return SA_ERR_HPI_INVALID_CMD;
	}

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type == OHOI_RESOURCE_ENTITY) {
                rv = ipmi_control_pointer_cb(ohoi_res_info->reset_ctrl, 
                                     set_resource_reset_state, &info);
	} else {
		rv = ipmi_mc_pointer_cb(ohoi_res_info->u.mc_id, 
			set_mc_reset_state, &info);
	}

        if (rv) {
                dbg("Not support reset in the entity or mc");
                return SA_ERR_HPI_CAPABILITY;
        }

	/* wait until reset_done is called to exit this function */
	rv = ohoi_loop(&info.done, ipmi_handler);
	if ((rv == SA_OK) && (info.err == 0)) {
		return SA_OK;
	} else if (info.err) {
		return info.err;
	} else {
		return rv;
	}
}

static void power_done (ipmi_control_t *ipmi_control,
                        int err,
                        void *cb_data)
{
	
	struct ohoi_power_info *power_info = cb_data;

	power_info->done = 1;
	if (err) {
		dbg("power_done: err = %d", err);
		power_info->err = SA_ERR_HPI_INTERNAL_ERROR;
	}
}

static void set_power_state_on(ipmi_control_t *control,
                            void           *cb_data)
{
	struct ohoi_power_info *power_info = cb_data;
	int rv;

        rv = ipmi_control_set_val(control, (int *)power_info->state, power_done, cb_data);

	if (rv) {
	  	dbg("Failed to set control val (power off)");
		power_info->err = SA_ERR_HPI_INTERNAL_ERROR;
		power_info->done = 1;
	} else
	  	power_info->err = SA_OK;
}

static void set_power_state_off(ipmi_control_t *control,
                            void           *cb_data)
{
	struct ohoi_power_info *power_info = cb_data;
	int rv;

        rv = ipmi_control_set_val(control, (int *)power_info->state,
				  power_done, cb_data);
	if (rv) {
	  	dbg("Failed to set control val (power off)");
		power_info->err = SA_ERR_HPI_INTERNAL_ERROR;
		power_info->done = 1;
	} else
	  	power_info->err = SA_OK;
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
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			break;
		case SAHPI_POWER_OFF:
			rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
                                                    set_power_state_off, &power_info);
	                if (rv) {
        	            	dbg("set_power_state_off failed");
                	    	return SA_ERR_HPI_INTERNAL_ERROR;
                	}
			break;
		case SAHPI_POWER_CYCLE:
			dbg("CYCLE power");
			SaHpiPowerStateT	cy_state = 0;
			cy_state = SAHPI_POWER_OFF;
			power_info.state = &cy_state;
			rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
						    set_power_state_off, &power_info);
			if (rv) {
				dbg("set_power_state_off failed");
				return SA_ERR_HPI_INTERNAL_ERROR;
			}
			ohoi_loop(&power_info.done, ipmi_handler);
			dbg("CYCLE Stage 1: OK");

			if ((power_info.done) && (power_info.err == SA_OK)) {
			      dbg("CYCLE: done = %d , err = %d",
				  				power_info.done,
								power_info.err);
			      	cy_state = SAHPI_POWER_ON;
				power_info.state = &cy_state;
			      	power_info.done = 0;
			      	rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl, 
							    set_power_state_on, &power_info);
				if (rv) {
				      	dbg("set_power_state_on failed");
					return SA_ERR_HPI_INTERNAL_ERROR;
				}
			}
			break;
		default:
			dbg("Invalid power state requested");
			return SA_ERR_HPI_INVALID_PARAMS;
	}

        ohoi_loop(&power_info.done, ipmi_handler);
        
        return power_info.err;
}

static void get_power_control_val (ipmi_control_t *control,
			     int err,
			     int *val,
			     void *cb_data)
{
	struct ohoi_power_info *info = cb_data;

	if (err || !val) {
		dbg("get_power_control_val: err = %d; val = %p", err, val);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
		return;
	}
	if (*val == 0) {
		dbg("Power Control Value: %d", *val);
		*info->state = SAHPI_POWER_OFF;
		info->err = SA_OK;
	} else if (*val == 1) {
		dbg("Power Control Value: %d", *val);
		*info->state = SAHPI_POWER_ON;
		info->err = SA_OK;
	} else {
		dbg("invalid power state");
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
	}
	info->done = 1;

}

static void get_power_state (ipmi_control_t *ipmi_control,
			     void *cb_data)
{
	struct ohoi_power_info *power_state = cb_data;
	int rv;

	rv = ipmi_control_get_val(ipmi_control, get_power_control_val, cb_data);

	if(rv) {
		dbg("[power]control_get_val failed. rv = %d", rv);
		power_state->err = SA_ERR_HPI_INTERNAL_ERROR;
		power_state->done = 1;
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
	power_state.err = 0;
	power_state.state = state;
	
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("MC does not support power!");
                return SA_ERR_HPI_CAPABILITY;
        }

	rv = ipmi_control_pointer_cb(ohoi_res_info->power_ctrl,
					get_power_state, &power_state);
	if (rv) {
		dbg("get_power_state failed");
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	dbg("waiting for OIPMI to return");
	ohoi_loop(&power_state.done, ipmi_handler);
				     
        return power_state.err;
}

static void get_reset_control_val (ipmi_control_t *control,
			     int err,
			     int *val,
			     void *cb_data)
{
	struct ohoi_reset_info *info = cb_data;

	if (err || !val) {
		dbg("get_reset_control_val: err = %d; val = %p", err, val);
		info->err = SA_ERR_HPI_INTERNAL_ERROR;
		info->done = 1;
		return;
	}
	if (*val == 0) {
		dbg("Reset Control Value: %d", *val);
		*info->state = SAHPI_RESET_DEASSERT;
		info->err = SA_OK;
	} else if (*val == 1) {
		dbg("Power Control Value: %d", *val);
		*info->state = SAHPI_RESET_ASSERT;
		info->err = SA_OK;
	} else {
	  	/* Note: IPMI platfroms don't hold reset state
		 * so we'll always return SAHPI_RESET_DEASSER
		 * this code is here just in case ;-)
		 */
		dbg("System does not support holding ResetState");
		*info->state = SAHPI_RESET_DEASSERT;
		info->err = SA_OK;
	}
	info->done = 1;
}

static
void get_reset_state(ipmi_control_t *control,
		     void *cb_data)
{
	struct ohoi_reset_info *reset_info = cb_data;

	int rv;

	rv = ipmi_control_get_val(control, get_reset_control_val, cb_data);
	if (rv) {
		//dbg("[reset] control_get_val failed. IPMI error = %i", rv);
		dbg("This IPMI system has a pulse reset, state is always DEASSERT");
		/* OpenIPMI will return an error for this call
		   since pulse resets do not support get_state
		   we will return UNSUPPORTED_API
		 */
		*reset_info->state = SAHPI_RESET_DEASSERT;
		reset_info->err = SA_OK;
		reset_info->done = 1;
	}
}
		    
SaErrorT ohoi_get_reset_state(void *hnd,
			      SaHpiResourceIdT id,
			      SaHpiResetActionT *act)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
        struct ohoi_resource_info *ohoi_res_info;
        
	struct ohoi_reset_info reset_state;

	int rv;

	reset_state.done = 0;
	reset_state.err = 0;
	reset_state.state = act;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_ENTITY) {
                dbg("Not support power in MC");
                return SA_ERR_HPI_CAPABILITY;
        }

	rv = ipmi_control_pointer_cb(ohoi_res_info->reset_ctrl,
				     get_reset_state, &reset_state);
	if(rv) {
		dbg("[reset_state] control pointer callback failed. rv = %d", rv);
		return SA_ERR_HPI_INVALID_CMD;
	}

	ohoi_loop(&reset_state.done, ipmi_handler);

	return reset_state.err;
}
