/*      -*- linux-c -*-
 *
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      David Judkovics <djudkovi@us.ibm.com>
 *
 */
 
#ifndef _SNMP_CLIENT_UTILS_
#define _SNMP_CLIENT_UTILS_

/* SNMP data doesn't allow for enumeration starting at '0' */
/* therefore it was decided all enumerations would be sent */
/* +1, this requires -1 when data is received		   */			     
#define SNMP_ENUM_ADJUST(x) x - 1


SaErrorT snmp_get2(struct snmp_session *ss, 
		   oid *objid, 
		   size_t objid_len,
		   struct snmp_value *value);

int snmp_getn_bulk( struct snmp_session *ss, 
		    oid *bulk_objid, 
		    size_t bulk_objid_len,
		    struct snmp_pdu *bulk_pdu, 
		    struct snmp_pdu **bulk_response,
		    int num_repetitions );

void build_res_oid(oid *d, oid *s, int s_len, oid *indices, int num_indices); 

SaErrorT net_snmp_failure(struct snmp_client_hnd *custom_handle, 
			  int snmp_status, 
			  struct snmp_pdu *response);

void display_vars( struct snmp_pdu *response);

int build_state_value (char *str,
		       size_t len,
		       SaHpiEventStateT *state);

int build_flag_value (char *str,
		      size_t len,
		      SaHpiSensorRangeFlagsT *flags);


#endif /* _SNMP_CLIENT_UTILS_ */
