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
 * This header file contains prototypes to stub out SNMP calls.
 * This file is used in "unit testing" the various plugin interface functions
 */

#ifndef TSTUBS_SNMP_H
#define TSTUBS_SNMP_H

SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          struct snmp_session *ss,
                          const char *objid,
                          struct snmp_value *value);

int snmp_get(struct snmp_session *ss,
	     const char *objid,
	     struct snmp_value *value);

int snmp_set(struct snmp_session *ss,
	     char *objid,
	     struct snmp_value value);

#endif
