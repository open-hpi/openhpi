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
 *      Sean Dague <http://dague.net/sean>
 *      W. David Ashley <dashley@us.ibm.com>
 */

#ifndef SNMP_RSA_SEL_H
#define SNMP_RSA_SEL_H

#define clearEventLogExecute 1
#define RSA_SEL_ID_STRING 20
#define RSA_SEL_ENTRY_STRING 256
#define RSA_SEL_INDEX_OID ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.1"
#define RSA_SEL_ENTRY_OID ".1.3.6.1.4.1.2.3.51.1.3.4.2.1.2"
#define RSA_CLEAR_SEL_OID ".1.3.6.1.4.1.2.3.51.1.3.4.3.0"

typedef struct {
        struct tm time;
        SaHpiSeverityT sev;
        char source[RSA_SEL_ID_STRING];
        char sname[RSA_SEL_ID_STRING];
        char text[RSA_SEL_ENTRY_STRING];
} rsa_sel_entry;


/* 
 * Function Prototyping
 */
int snmp_rsa_parse_sel_entry(struct oh_handler_state *,char * text, rsa_sel_entry * sel);
int snmp_rsa_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info);
int snmp_rsa_get_sel_entry(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT current,
                          SaHpiEventLogEntryIdT *prev, SaHpiEventLogEntryIdT *next,
                          SaHpiEventLogEntryT *entry);
int snmp_rsa_set_sel_time(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time);
int snmp_rsa_add_sel_entry(void *hnd, SaHpiResourceIdT id, const SaHpiEventLogEntryT *Event);
int snmp_rsa_del_sel_entry(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT sid);
int snmp_rsa_check_selcache(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT entryId);
int snmp_rsa_build_selcache(void *hnd, SaHpiResourceIdT id);
int snmp_rsa_sel_read_add (void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT sid);
int snmp_rsa_selcache_sync(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT entryId);
SaErrorT snmp_rsa_clear_sel(void *hnd, SaHpiResourceIdT id);

#endif
