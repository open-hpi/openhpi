/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

/**
 * snmp_bc_get_control_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @cid: Control ID.
 * @mode: Location to store control's operational mode.
 * @state: Location to store control's state.
 *
 * Retrieves a control's operational mode and/or state. Both @mode and @state
 * may be NULL (e.g. check for presence).
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_CONTROL.
 * SA_ERR_HPI_INVALID_CMD - Control is write-only.
 * SA_ERR_HPI_INVALID_DATA - @state contain invalid text line number.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_NOT_PRESENT - Control doesn't exist.
 **/
SaErrorT snmp_bc_get_control_state(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiCtrlNumT cid,
				   SaHpiCtrlModeT *mode,
				   SaHpiCtrlStateT *state)
{
	SaErrorT err;
	SaHpiCtrlStateT working_state;
        struct ControlInfo *cinfo;
	struct snmp_value get_value;
	
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	memset(&working_state, 0, sizeof(SaHpiCtrlStateT));

	/* Check if resource exists and has control capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Find control and its mapping data - see if it accessable */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_CTRL_RDR, cid);
	if (rdr == NULL) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	cinfo = (struct ControlInfo *)oh_get_rdr_data(handle->rptcache, cid, rdr->RecordId);
 	if (cinfo == NULL) {
		dbg("No control data. Control=%s", rdr->IdString.Data);
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	if (rdr->RdrTypeUnion.CtrlRec.WriteOnly) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_CMD);
	}
	if (!mode && !state) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_OK);
	}
	
	if (state) {
		if (state->Type == SAHPI_CTRL_TYPE_TEXT) {
			if (state->StateUnion.Text.Line != SAHPI_TLN_ALL_LINES ||
			    state->StateUnion.Text.Line > rdr->RdrTypeUnion.CtrlRec.TypeUnion.Text.MaxLines) {
			    	g_static_rec_mutex_unlock(&handle->handler_lock);
				return(SA_ERR_HPI_INVALID_DATA);
			}
		}

		/* Find control's state */
		working_state.Type = rdr->RdrTypeUnion.CtrlRec.Type;
		
		err = snmp_bc_oid_snmp_get(custom_handle, &(rdr->Entity), cinfo->mib.oid, &get_value, SAHPI_TRUE);
		if (err  || get_value.type != ASN_INTEGER) {
			dbg("Cannot read SNMP OID=%s; Type=%d.", cinfo->mib.oid, get_value.type);
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(err);
		}
		
		switch (rdr->RdrTypeUnion.CtrlRec.Type) {
		case SAHPI_CTRL_TYPE_DIGITAL:
			g_static_rec_mutex_unlock(&handle->handler_lock);
			dbg("Digital controls not supported.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_DISCRETE:
			working_state.StateUnion.Discrete = get_value.integer;
			break;
		case SAHPI_CTRL_TYPE_ANALOG:
			g_static_rec_mutex_unlock(&handle->handler_lock);
			dbg("Analog controls not supported.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_STREAM:
			g_static_rec_mutex_unlock(&handle->handler_lock);
			dbg("Stream controls not supported.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_TEXT:
			g_static_rec_mutex_unlock(&handle->handler_lock);
			dbg("Text controls not supported.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_OEM:
			g_static_rec_mutex_unlock(&handle->handler_lock);	
			dbg("Oem controls not supported.");
			return(SA_ERR_HPI_INTERNAL_ERROR);
		default:
			dbg("%s has invalid control state=%d.", cinfo->mib.oid, working_state.Type);
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

	if (state) memcpy(state, &working_state, sizeof(SaHpiCtrlStateT));
	if (mode) *mode = cinfo->cur_mode;
	
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_OK);
}

/**
 * snmp_bc_set_control_state:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @cid: Control ID.
 * @mode: Control's operational mode to set.
 * @state: Pointer to control's state to set.
 *
 * Sets a control's operational mode and/or state. Both @mode and @state
 * may be NULL (e.g. check for presence).
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_CONTROL.
 * SA_ERR_HPI_INVALID_DATA - @state contains bad text control data.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_NOT_PRESENT - Control doesn't exist.
 **/
SaErrorT snmp_bc_set_control_state(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiCtrlNumT cid,
				   SaHpiCtrlModeT mode,
				   SaHpiCtrlStateT *state)
{
	SaErrorT err;
        struct ControlInfo *cinfo;
	struct snmp_value set_value;

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}	
	
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has control capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	/* Find control and its mapping data - see if it accessable */
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, rid, SAHPI_CTRL_RDR, cid);
	if (rdr == NULL) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_NOT_PRESENT);
	}
	
	cinfo = (struct ControlInfo *)oh_get_rdr_data(handle->rptcache, cid, rdr->RecordId);
 	if (cinfo == NULL) {
		dbg("No control data. Control=%s", rdr->IdString.Data);
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}       

	/* Validate static control state and mode data */
	err = oh_valid_ctrl_state_mode(&(rdr->RdrTypeUnion.CtrlRec), mode, state);
	if (err) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(err);
	}

	/* Write control state */
	if (mode != SAHPI_CTRL_MODE_AUTO && state) {
		switch (state->Type) {
		case SAHPI_CTRL_TYPE_DIGITAL:
			dbg("Digital controls not supported.");
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_DISCRETE:
			set_value.type = ASN_INTEGER;
			set_value.str_len = 1;
			set_value.integer = state->StateUnion.Discrete;
			
			err = snmp_bc_oid_snmp_set(custom_handle, &(rdr->Entity), cinfo->mib.oid, set_value);
			if (err) {
				dbg("Cannot set SNMP OID=%s; Value=%d.", 
				    cinfo->mib.oid, (int)set_value.integer);
				g_static_rec_mutex_unlock(&handle->handler_lock);
				return(err);
			}
			break;
		case SAHPI_CTRL_TYPE_ANALOG:
			dbg("Analog controls not supported.");
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_STREAM:
			dbg("Stream controls not supported.");
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_TEXT:
			dbg("Text controls not supported.");
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		case SAHPI_CTRL_TYPE_OEM:	
			dbg("OEM controls not supported.");
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		default:
			dbg("Invalid control state=%d", state->Type);
			g_static_rec_mutex_unlock(&handle->handler_lock);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
	}

	/* Write control mode, if changed */
	if (mode != cinfo->cur_mode) {
		cinfo->cur_mode = mode;
	}
	
	g_static_rec_mutex_unlock(&handle->handler_lock);
        return(SA_OK);
}
