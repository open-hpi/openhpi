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

#ifndef __COM_XML2EVENT_H
#define __COM_XML2EVENT_H

#define HPIDUP_STRING  "_HPIDUP"

#define NO_OVR  0x0000  /* No overrides */
#define OVR_SEV 0x0001  /* Override Error Log's severity */
#define OVR_RID 0x0010  /* Override Error Log's source */
#define OVR_EXP 0x0100  /* Override Error Log's source for expansion cards */

typedef struct {
        gchar *event;
	SaHpiSeverityT event_sev;
	unsigned short event_ovr;
        short          event_dup;
} Xml2EventInfoT;

/* Global String to Event Hash Table */
extern GHashTable *bc_xml2event_hash;
extern GHashTable *rsa_xml2event_hash;
extern unsigned int bc_xml2event_hash_use_count;
extern unsigned int rsa_xml2event_hash_use_count;

/* xml code for rsa and bc events */
extern char *bc_eventxml;
extern char *rsa_eventxml;

int xml2event_hash_init(GHashTable **hashtable, const char *xmlstr);
int xml2event_hash_free(GHashTable ** hashtable);

#endif // __COM_XML2EVENT_H
