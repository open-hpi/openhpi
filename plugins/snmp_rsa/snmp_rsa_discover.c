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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include <glib.h>
#include <SaHpi.h>

#include <snmp_util.h>
#include <uid_utils.h>
#include <epath_utils.h>

#include <openhpi.h>
#include <rsa_resources.h>
#include <snmp_rsa.h>
// #include <snmp_rsa_utils.h>
#include <snmp_rsa_discover.h>

static inline struct oh_event *eventdup(const struct oh_event *event) 
{
	struct oh_event *e;
	e = g_malloc0(sizeof(struct oh_event));
	if (!e) {
		dbg("Out of memory!");
		return NULL;
	}
	memcpy(e, event, sizeof(struct oh_event));
	return e;
}


struct oh_event * snmp_rsa_discover_chassis(SaHpiEntityPathT *ep) 
{
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));
         
        working.type = OH_ET_RESOURCE;
        working.u.res_event.entry = snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].rpt;

	working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	len = strlen(snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
		strcpy(working.u.res_event.entry.ResourceTag.Data,
		       snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	} else {
		dbg("Comment string too long - %s\n",snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	}

        working.u.res_event.entry.ResourceId = oh_uid_from_entity_path(ep);
        working.u.res_event.entry.ResourceEntity = *ep;

        e = eventdup(&working);
 
	return e;
}
