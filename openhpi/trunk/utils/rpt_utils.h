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

/* oh_add_resource/rdr free-data flag */
#define FREE_RPT_DATA SAHPI_FALSE
#define KEEP_RPT_DATA SAHPI_TRUE

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct {
        SaHpiUint32T UpdateCount;     
        SaHpiTimeT   UpdateTimestamp; 
} SaHpiRptInfoT;

typedef struct {
        SaHpiRptInfoT rpt_info;
        /* The structure to hold this is subject to change. */
        /* No one should touch this. */
        GSList *rptlist; /* Contains RPTEntrys for sequence lookups */
        GHashTable *rptable; /* Contains RPTEntrys for fast EntryId lookups */
} RPTable;

typedef struct {
        SaHpiRptEntryT rpt_entry;
        int owndata;
        void *data; /* private data for the owner of the RPTable */
        GSList *rdrlist; /* Contains RDRecords for sequence lookups */
        GHashTable *rdrtable; /* Contains RDRecords for fast RecordId lookups */
} RPTEntry;

typedef struct {
       SaHpiRdrT rdr;
       int owndata;
       void *data; /* private data for the owner of the rpt entry. */
} RDRecord;


/* General RPT calls */
void oh_flush_rpt(RPTable *table);
void rpt_diff(RPTable *cur_rpt, RPTable *new_rpt,
              GSList **res_new, GSList **rdr_new,
              GSList **res_gone, GSList **rdr_gone);

/* Resource calls */
int oh_add_resource(RPTable *table, SaHpiRptEntryT *entry, void *data, int owndata);

int oh_remove_resource(RPTable *table, SaHpiResourceIdT rid);

void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid);
SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid);
SaHpiRptEntryT *oh_get_resource_by_ep(RPTable *table, SaHpiEntityPathT *ep);
SaHpiRptEntryT *oh_get_resource_next(RPTable *table, SaHpiResourceIdT rid_prev);

/* RDR calls */
int oh_add_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiRdrT *rdr, void *data, int owndata);

int oh_remove_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);

void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);
SaHpiRdrT *oh_get_rdr_by_id(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid);
SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num);
SaHpiRdrT *oh_get_rdr_next(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid_prev);
SaHpiUint32T get_rdr_uid(SaHpiRdrTypeT type, SaHpiUint32T num);


#ifdef __cplusplus
}
#endif

#endif
