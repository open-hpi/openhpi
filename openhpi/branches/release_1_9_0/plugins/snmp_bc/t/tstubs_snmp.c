/* -*- linux-c -*-
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
 *     Steve Sherman <stevees@us.ibm.com>
 */

/*
 * These functions stub out SNMP calls. They are used to "unit test" the 
 * various plugin interface calls.
 */

SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          struct snmp_session *ss,
                          const char *objid,
                          struct snmp_value *value) {
	int err;
	err = snmp_get(ss, objid, value);
	
	if (err) { return SA_ERR_HPI_ERROR; }
	
	return SA_OK;
}

SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          struct snmp_session *ss,
                          char *objid,
                          struct snmp_value value) {
	int err;
	err = snmp_set(ss, objid, value);
	
	if (err) { return SA_ERR_HPI_ERROR; }
	
	return SA_OK;
}
 
int snmp_get(struct snmp_session *ss, const char *objid, struct snmp_value *value)
{

	if (snmp_force_error) { return -1; }
	
	if (snmp_get_string_type) {
		value->type = ASN_OCTET_STR;
		strcpy(value->string, snmp_value_string);
	}
	else {
		value->type = ASN_INTEGER;
		value->integer = snmp_value_integer;
	}
	
	return 0;
}

int snmp_set(struct snmp_session *ss, char *objid, struct snmp_value value) 
{
	if (snmp_force_error) { return -1; }
	else { return 0; }
}
