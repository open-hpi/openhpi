/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#ifndef RPT_UTILS_H
#define RPT_UTILS_H
#include <SaHpi.h>
#include <glib.h>

#define RPT_ENTRY_BEGIN 0
#define RDR_BEGIN 0

typedef struct {
        SaHpiRptInfoT rpt_info;
        /* The structure to hold this is subject to change. */
        /* No one should touch this. */
        GSList *rptable; /* Contains RPTEntrys */
} RPTable;

typedef struct {
        SaHpiRptEntryT rpt_entry;
        void *data; /* private data for the owner of the RPTable */
        GSList *rdrtable;  /* Contains RDRecords */
} RPTEntry;

typedef struct {
       SaHpiRdrT rdr;
       void *data; /* private data for the owner of the rpt entry. */
} RDRecord;

/* Resource */
int oh_add_resource(RPTable *table, SaHpiRptEntryT entry, void *data);

int oh_remove_resource(RPTable *table, SaHpiResourceIdT rid);

void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid);
SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid);
SaHpiRptEntryT *oh_get_resource_by_ep(RPTable *table, SaHpiEntityPathT *ep);
SaHpiRptEntryT *oh_get_resource_next(RPTable *table, SaHpiResourceIdT rid_prev);

/* RDR */
int oh_add_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiRdrT rdr, void *data);

int oh_remove_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);

void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);
SaHpiRdrT *oh_get_rdr_by_id(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);
SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num);
SaHpiRdrT *oh_get_rdr_next(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid_prev);

#endif
