/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 */

#ifndef SNMP_BC_H
#define SNMP_BC_H

#define MAX_RETRY_ATTEMPTED  5

/**
 * This handle will be unique per instance of this plugin. 
 * SNMP session data is stored in the handle along with config file data.
 **/

struct snmp_bc_hnd {
        struct snmp_session session;
        struct snmp_session *ss; 	/* SNMP Session pointer */
	GHashTable *event2hpi_hash_ptr; /* Global BC Event Number to HPI Event Hash Table */
	char    bc_type[4];
	char handler_timezone[10];
        int     handler_retries;        /* number of retries attempted on snmp target (client) */
};

/**
 * 
 *
 **/
SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          struct snmp_session *ss,
                          const char *objid,
                          struct snmp_value *value);



#endif
