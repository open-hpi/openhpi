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
 * Authors:
 *      David Ashley <dashley@us.ibm.com>
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#ifndef SEL_UTILS_H
#define SEL_UTILS_H
#include <SaHpi.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define OH_SEL_MAX_SIZE 0

/* this struct encapsulates all the data for a system event log */
/* the log records themselves are stored in the sel GList */
typedef struct {
        SaHpiBoolT       enabled; // log enabled?
        SaHpiBoolT       overflow; // log overflowed?
        SaHpiBoolT       gentimestamp; // generate timestamp?
        SaHpiTimeT       lastUpdate; // last entry's timestamp
        SaHpiTimeT       offset; // offset to be added when generating a timestamp
        SaHpiUint32T     maxsize; //max number of entries supported
        SaHpiEventLogEntryIdT nextId; // next generated Id i.e. number of entries
        GList            *selentries; // list of SaHpiEventLogEntryT structs
} oh_sel;

/* General SEL utility calls */
oh_sel *oh_sel_create(SaHpiUint32T size);
SaErrorT oh_sel_close(oh_sel *sel);
SaErrorT oh_sel_add(oh_sel *sel, SaHpiEventT *event); /* to be removed in the future */
SaErrorT oh_sel_append(oh_sel *sel, SaHpiEventT *event);
SaErrorT oh_sel_prepend(oh_sel *sel, SaHpiEventT *event);
SaErrorT oh_sel_delete(oh_sel *sel, SaHpiEntryIdT *entryid);
SaErrorT oh_sel_clear(oh_sel *sel);
SaErrorT oh_sel_get(oh_sel *sel, SaHpiEventLogEntryIdT entryid, SaHpiEventLogEntryIdT *prev,
                    SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT **entry);
SaErrorT oh_sel_info(oh_sel *sel, SaHpiEventLogInfoT *info);
SaErrorT oh_sel_map_to_file(oh_sel *sel, char *filename);
SaErrorT oh_sel_map_from_file(oh_sel *sel, char *filename);
SaErrorT oh_sel_timeset(oh_sel *sel, SaHpiTimeT timestamp);
SaErrorT oh_sel_setgentimestampflag(oh_sel *sel, SaHpiBoolT flag);


#ifdef __cplusplus
}
#endif

#endif

