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

#ifndef __SNMP_BC_EVENT_H
#define __SNMP_BC_EVENT_H

#define MAX_THRESHOLD_VALUE_STRINGSIZE 10

/* Global BC Event Number to HPI Event Hash Table */
GHashTable * event2hpi_hash;

typedef struct {
	SaHpiResourceIdT       rid;
	BCRptEntryT            rpt;
	struct snmp_bc_sensor  *sensor_array_ptr;
	SaHpiEntityPathT       ep;
} LogSource2ResourceT;

int event2hpi_hash_init(void);
int event2hpi_hash_free(void);

int find_res_events(SaHpiEntityPathT *ep, const struct BC_ResourceInfo *bc_res_info);
int find_sensor_events(SaHpiEntityPathT *ep, SaHpiSensorNumT sid, const struct snmp_bc_sensor *rpt_sensor);

int log2event(void *hnd, gchar *logstr, SaHpiEventT *event, int isdst, int *event_enabled);
int bcsrc2rid(void *hnd, gchar *src, LogSource2ResourceT *resinfo);

int snmp_bc_add_to_eventq(void *hnd, SaHpiEventT *thisEvent);
#endif
