/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2005, 2006
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

#define SNMP_BC_MAX_SNMP_RETRY_ATTEMPTED  5
#define SNMP_BC_MAX_RESOURCES_MASK        16 /* 15-char long plus NULL terminated */

#include <stdlib.h>
#include <glib.h>

typedef struct {
        GStaticRecMutex lock;
        guint32 count;
} ohpi_bc_lock;

/* This handle is unique per instance of this plugin. 
 * SNMP session data is stored in the handle along with config file data. */
struct snmp_bc_hnd {
	void   *sessp;			/* Opaque pointer, not a pointer to struct snmp_session */
        struct snmp_session session;
        struct snmp_session *ss; 	/* SNMP Session pointer */
	int    count_per_getbulk;       /* For performance, GETBULK is used with snmpV3. */
					/* This value indicates max OIDs per GETBULK request */
	GHashTable *event2hpi_hash_ptr; /* Global "Event Number to HPI Event" hash table */
	int   platform;
	int   active_mm;                /* Used for duplicate event RID override */
	SaHpiBoolT  first_discovery_done;
	char  handler_timezone[10];
        int   handler_retries;          /* Number of retries attempted on SNMP target (agent) */
	RPTable *tmpcache;
	GSList *tmpqueue;
	ohpi_bc_lock snmp_bc_hlock;
	char blade_mask[SNMP_BC_MAX_RESOURCES_MASK];
	char fan_mask[SNMP_BC_MAX_RESOURCES_MASK];
	char powermodule_mask[SNMP_BC_MAX_RESOURCES_MASK];
	char switch_mask[SNMP_BC_MAX_RESOURCES_MASK];
	char mm_mask[SNMP_BC_MAX_RESOURCES_MASK];		
        long mediatray_mask;

};

SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          const char *objid,
                          struct snmp_value *value, 
			  SaHpiBoolT retry);

SaErrorT snmp_bc_oid_snmp_get(struct snmp_bc_hnd *custom_handle,
			      SaHpiEntityPathT *ep,
			      const gchar *oidstr,
			      struct snmp_value *value,
			      SaHpiBoolT retry);

SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          char *objid,
                          struct snmp_value value);
			  
SaErrorT snmp_bc_oid_snmp_set(struct snmp_bc_hnd *custom_handle,
			      SaHpiEntityPathT *ep,
			      const gchar *oidstr,
			      struct snmp_value value);
			  			  
SaErrorT snmp_bc_get_event(void *hnd,
			   struct oh_event *event);
			   
SaErrorT snmp_bc_set_resource_tag(void *hnd,
				  SaHpiResourceIdT rid,
				  SaHpiTextBufferT *tag);
				  
SaErrorT snmp_bc_set_resource_severity(void *hnd,
					SaHpiResourceIdT rid,
					SaHpiSeverityT sev);
					
SaErrorT snmp_bc_control_parm(void *hnd,
				SaHpiResourceIdT rid,
				SaHpiParmActionT act);

			  
#endif
