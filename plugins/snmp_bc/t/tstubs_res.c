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
 * These functions stub out infra-structure Resource calls. They are used to 
 * "unit test" the various plugin interface calls.
 */ 

void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid) 
{
	if (ifobj_data_force_error || table == NULL) { return NULL; }
	else { return((void *)(&(((struct snmp_rpt *)table)->bc_res_info))); }
}

SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid)
{
	if (ifobj_force_error || table == NULL) { return NULL; }
	else { return (&(((struct snmp_rpt *)table)->rpt)); }
}
