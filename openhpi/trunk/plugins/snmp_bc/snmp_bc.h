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
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef __SNMP_BC_H
#define __SNMP_BC_H

#define MAX_RETRY_ATTEMPTED  5

/**
 * This handle is unique per instance of this plugin. 
 * SNMP session data is stored in the handle along with config file data.
 **/
struct snmp_bc_hnd {
        struct snmp_session session;
        struct snmp_session *ss; 	/* SNMP Session pointer */
	GHashTable *event2hpi_hash_ptr; /* Global "Event Number to HPI Event" hash table */
	int   platform;
	char  handler_timezone[10];
        int   handler_retries;          /* Number of retries attempted on SNMP target (client) */
	RPTable *tmpcache;
	GSList *tmpqueue;
};

SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          const char *objid,
                          struct snmp_value *value);

SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          char *objid,
                          struct snmp_value value);


#endif
