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
 */

#ifndef EL_UTILS_H
#define EL_UTILS_H

#ifndef OH_UTILS_H
#warning *** Include oh_utils.h instead of individual utility header files ***
#endif

#include <SaHpi.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OH_EL_MAX_SIZE 0

/* this struct encapsulates all the data for a system event log */
/* the log records themselves are stored in the el GList */
typedef struct {
        SaHpiBoolT       enabled; // log enabled?
        SaHpiBoolT       overflow; // log overflowed?
        SaHpiBoolT       gentimestamp; // generate timestamp?
        SaHpiTimeT       lastUpdate; // last entry's timestamp
        SaHpiTimeT       offset; // offset to be added when generating a timestamp
        SaHpiUint32T     maxsize; //max number of entries supported
        SaHpiEventLogEntryIdT nextId; // next generated Id i.e. number of entries
        GList            *elentries; // list of SaHpiEventLogEntryT structs
} oh_el;

/* General EL utility calls */
oh_el *oh_el_create(SaHpiUint32T size);
SaErrorT oh_el_close(oh_el *el);
SaErrorT oh_el_add(oh_el *el, SaHpiEventT *event); /* to be removed in the future */
SaErrorT oh_el_append(oh_el *el, SaHpiEventT *event);
SaErrorT oh_el_prepend(oh_el *el, SaHpiEventT *event);
SaErrorT oh_el_delete(oh_el *el, SaHpiEntryIdT *entryid);
SaErrorT oh_el_clear(oh_el *el);
SaErrorT oh_el_get(oh_el *el, SaHpiEventLogEntryIdT entryid, SaHpiEventLogEntryIdT *prev,
                    SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT **entry);
SaErrorT oh_el_info(oh_el *el, SaHpiEventLogInfoT *info);
SaErrorT oh_el_map_to_file(oh_el *el, char *filename);
SaErrorT oh_el_map_from_file(oh_el *el, char *filename);
SaErrorT oh_el_timeset(oh_el *el, SaHpiTimeT timestamp);
SaErrorT oh_el_setgentimestampflag(oh_el *el, SaHpiBoolT flag);


#ifdef __cplusplus
}
#endif

#endif
