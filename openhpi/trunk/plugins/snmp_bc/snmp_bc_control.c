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

SaErrorT snmp_bc_get_control_state(void *hnd, 
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlModeT *mode,
				   SaHpiCtrlStateT *state)
{
        gchar *oid;
	int i, found;
	SaHpiCtrlStateT working;
        struct snmp_value get_value;
	SaErrorT status;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_CTRL_RDR, num);
	if(rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
        struct BC_ControlInfo *s =
                (struct BC_ControlInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
	if(s == NULL) {
		return -1;
	}	

	if (rdr->RdrTypeUnion.CtrlRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	memset(&working, 0, sizeof(SaHpiCtrlStateT));
	working.Type = rdr->RdrTypeUnion.CtrlRec.Type;

	oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
	if(oid == NULL) {
		dbg("NULL SNMP OID returned for %s\n",s->mib.oid);
		return -1;
	}
	
	status = snmp_bc_snmp_get(custom_handle, oid, &get_value);
	if(( status != SA_OK) | (get_value.type != ASN_INTEGER)) {
		dbg("SNMP could not read %s; Type=%d.\n", oid, get_value.type);
		g_free(oid);
		if ( status == SA_ERR_HPI_BUSY) 
			return status;
		else 
			return SA_ERR_HPI_NO_RESPONSE;
	}
	g_free(oid);
	
	switch (working.Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:
		
		found = 0;
		for(i=0; i<OH_MAX_CTRLSTATEDIGITAL; i++) {
			if(s->mib.digitalmap[i] == get_value.integer) { 
				found++;
				break; 
			}
		}

		if(found) {
			switch (i) {
			case 0:
				working.StateUnion.Digital = SAHPI_CTRL_STATE_OFF;
				break;
			case 1:
				working.StateUnion.Digital = SAHPI_CTRL_STATE_ON;
				break;
			case 2:
				working.StateUnion.Digital = SAHPI_CTRL_STATE_PULSE_OFF;
				break;
			case 3:
				working.StateUnion.Digital = SAHPI_CTRL_STATE_PULSE_ON;
				break;
			default:
				dbg("Invalid Case=%d", i);
				return -1;
			}
		} else {
			dbg("Control's value not defined\n");
			return -1;
		}
		break;
	case SAHPI_CTRL_TYPE_DISCRETE:
		working.StateUnion.Discrete = get_value.integer;
		break;
	case SAHPI_CTRL_TYPE_ANALOG:
		dbg("Analog controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_STREAM:
		dbg("Stream controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_TEXT:
		dbg("Text controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_OEM:	
		dbg("Oem controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
        default:
		dbg("%s has invalid control state=%d\n", s->mib.oid,working.Type);
                return -1;
        }

	memcpy(state,&working,sizeof(SaHpiCtrlStateT));
	return SA_OK;
}

SaErrorT snmp_bc_set_control_state(void *hnd, 
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlModeT *mode,
				   SaHpiCtrlStateT *state)
{
        gchar *oid;
	int value;
	SaErrorT status;

        struct snmp_value set_value;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, id, SAHPI_CTRL_RDR, num);
	if(rdr == NULL) {
		return SA_ERR_HPI_NOT_PRESENT;
	}
        struct BC_ControlInfo *s =
                (struct BC_ControlInfo *)oh_get_rdr_data(handle->rptcache, id, rdr->RecordId);
	if(s == NULL) {
		return -1;
	}

	if (rdr->RdrTypeUnion.CtrlRec.Ignore == SAHPI_TRUE) {
		return SA_ERR_HPI_INVALID_CMD;
	}

	if(state->Type != rdr->RdrTypeUnion.CtrlRec.Type) {
		dbg("Control %s type %d cannot be changed\n",s->mib.oid,state->Type);
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	switch (state->Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:
		switch (state->StateUnion.Digital) {
		case SAHPI_CTRL_STATE_OFF:
			value = s->mib.digitalwmap[SAHPI_CTRL_STATE_OFF];
			break;
		case SAHPI_CTRL_STATE_ON:
			value = s->mib.digitalwmap[SAHPI_CTRL_STATE_ON];
			break;	
		case SAHPI_CTRL_STATE_PULSE_OFF:
			value = s->mib.digitalwmap[SAHPI_CTRL_STATE_PULSE_OFF];
			break;
		case SAHPI_CTRL_STATE_PULSE_ON:
			value = s->mib.digitalwmap[OH_MAX_CTRLSTATEDIGITAL - 1];
			break;
		default:
			dbg("Invalid Case=%d", state->StateUnion.Digital);
			return -1;
		}

		if(value < 0) {
			dbg("Control state %d not allowed to be set\n",state->StateUnion.Digital);
			return SA_ERR_HPI_INVALID_CMD;
		}

		oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
		if(oid == NULL) {
			dbg("NULL SNMP OID returned for %s\n",s->mib.oid);
			return -1;
		}

		set_value.type = ASN_INTEGER;
		set_value.str_len = 1;
		set_value.integer = value;

		status = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (status != SA_OK) {
			dbg("SNMP could not set %s; Value=%d.\n", oid, value);
			g_free(oid);
			if (status == SA_ERR_HPI_BUSY) return status;
			else return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);
		break;

	case SAHPI_CTRL_TYPE_DISCRETE:
		oid = snmp_derive_objid(rdr->Entity, s->mib.oid);
		if(oid == NULL) {
			dbg("NULL SNMP OID returned for %s\n",s->mib.oid);
			return -1;
		}

		set_value.type = ASN_INTEGER;
		set_value.str_len = 1;
		set_value.integer = state->StateUnion.Discrete;

		status = snmp_bc_snmp_set(custom_handle, oid, set_value);
		if (status != SA_OK) {
			dbg("SNMP could not set %s; Value=%d.\n", oid, (int)set_value.integer);
			g_free(oid);
			if (status == SA_ERR_HPI_BUSY) return status;
			else return SA_ERR_HPI_NO_RESPONSE;
		}
		g_free(oid);
		break;
	case SAHPI_CTRL_TYPE_ANALOG:
		dbg("Analog controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_STREAM:
		dbg("Stream controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_TEXT:
		dbg("Text controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
	case SAHPI_CTRL_TYPE_OEM:	
		dbg("Oem controls not supported\n");
		return SA_ERR_HPI_INVALID_CMD;
        default:
		dbg("Request has invalid control state=%d\n", state->Type);
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	
        return SA_OK;
}
