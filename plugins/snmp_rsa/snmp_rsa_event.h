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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#ifndef __SNMP_RSA_EVENT_H
#define __SNMP_RSA_EVENT_H

#define MAX_THRESHOLD_VALUE_STRINGSIZE 10

typedef struct {
	SaHpiResourceIdT       rid;
	RSARptEntryT            rpt;
	struct snmp_rsa_sensor *sensor_array_ptr;
	SaHpiEntityPathT       ep;
} LogSource2ResourceT;

int event2hpi_hash_init(struct oh_handler_state *handle);
int event2hpi_hash_free(struct oh_handler_state *handle);

int find_res_events(struct oh_handler_state *handle, SaHpiEntityPathT *ep, const struct RSA_ResourceInfo *rsa_res_info);
int find_sensor_events(struct oh_handler_state *handle, SaHpiEntityPathT *ep, SaHpiSensorNumT sid, const struct snmp_rsa_sensor *rpt_sensor);

int log2event(void *hnd, gchar *logstr, SaHpiEventT *event, int isdst, int *event_enabled);
int rsasrc2rid(void *hnd, gchar *src, LogSource2ResourceT *resinfo);

int snmp_rsa_add_to_eventq(void *hnd, SaHpiEventT *thisEvent);
#endif
