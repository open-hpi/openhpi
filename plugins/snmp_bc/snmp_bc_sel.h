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
 */

#ifndef SNMP_BC_SEL_H
#define SNMP_BC_SEL_H

#define clearEventLogExecute 1
#define BC_SEL_ID_STRING 20
#define BC_SEL_ENTRY_STRING 256

typedef struct {
        struct tm time;
        SaHpiSeverityT sev;
        char source[BC_SEL_ID_STRING];
        char sname[BC_SEL_ID_STRING];
        char text[BC_SEL_ENTRY_STRING];
} bc_sel_entry;


/* 
 * Function Prototyping
 */
int snmp_bc_parse_sel_entry(struct oh_handler_state *,char * text, bc_sel_entry * sel);
int snmp_bc_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info);
int snmp_bc_get_sel_entry(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT current,
                          SaHpiEventLogEntryIdT *prev, SaHpiEventLogEntryIdT *next,
                          SaHpiEventLogEntryT *entry);
int snmp_bc_set_sel_time(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time);
int snmp_bc_add_sel_entry(void *hnd, SaHpiResourceIdT id, const SaHpiEventT *Event);
int snmp_bc_check_selcache(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT entryId);
int snmp_bc_build_selcache(void *hnd, SaHpiResourceIdT id);
int snmp_bc_sel_read_add (void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT sid);
int snmp_bc_selcache_sync(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT entryId);
SaErrorT snmp_bc_clear_sel(void *hnd, SaHpiResourceIdT id);

#endif
