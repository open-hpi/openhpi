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

#include <rpt_utils.h>

static inline RPTEntry *get_rptentry_by_rid( RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        for (node = table->rptable; node != NULL; node = node->next) {
                rptentry = (RPTEntry *) node->data;
                if (rptentry->rpt_entry.ResourceId == rid) break;
                else rptentry = NULL;
        }

        return rptentry;
}

static inline RDRecord *get_rdrecord_by_id(GSList *records, SaHpiEntryIdT id)
{
        RDRecord *rdrecord = NULL;
        GSList *node;

        for (node = records; node != NULL; node = node->next) {
                rdrecord = (RDRecord *) node->data;
                if (rdrecord->rdr.RecordId == id) break;
                else rdrecord = NULL;
        }

        return rdrecord;
}

static inline SaHpiUint32T get_rdr_uid(SaHpiRdrTypeT type, SaHpiUint32T num)
{
        SaHpiUint32T uid;

        uid = (SaHpiUint32T)rdr.RdrType;
        uid = uid << 28;
        uid = uid | (SaHpiUint32T)num;

        return uid;
}

static inline SaHpiUint8T get_rdr_type_num(SaHpiRdrT rdr)
{
        SaHpiUint8T num;
        
        switch (rdr.RdrType) {
                case SAHPI_CTRL_RDR:
                        num = rdr.RdrTypeUnion.CtrlRec.Num;
                case SAHPI_SENSOR_RDR:
                        num = rdr.RdrTypeUnion.SensorRec.Num;
                case SAHPI_INVENTORY_RDR:
                        num = rdr.RdrTypeUnion.InventoryRec.EirId;
                case SAHPI_WATCHDOG_RDR:
                        num = rdr.RdrTypeUnion.WatchdogRec.WatchdogNum;
                default:
                        num = 0;
        }

        return num;
}

/**
 * Resource interface functions
 */

int oh_add_resource(RPTable *table, SaHpiRptEntryT entry, void *data)
{
        RPTEntry *rptentry;

        /* Check to see if the entry is in the RPTable already */
        rptentry = get_rptentry_by_rid(table, entry.ResourceId);
        
        /* If not, create new RPTEntry */
        if (!rptentry) {
                rptentry = (RPTEntry *)malloc(sizeof(RPTEntry));
                if (!rptentry) return -1; /* Not enough memory to add resource. */
                /* Put new RPTEntry in RPTable */
                table->rptable = g_slist_append(table->rptable, (gpointer)rptentry);
                
        }
        /* Else, modify existing RPTEntry */
        rptentry->data = data;
        rptentry->rpt_entry = entry;
        rptentry->rpt_entry.EntryId = entry.ResourceId; /** Is this ok? */
               
        return 0; 
}

int oh_remove_resource(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;
        
        rptentry = get_rptentry_by_rid(table, rid);

        if (!rptentry) return -1; /* No Resource found by that id */
        else g_slist_remove(table->rptable, (gpointer)rptentry);

        return 0;
}

void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid)
{
        
        RPTEntry *rptentry;

        rptentry = get_rptentry_by_rid(table, rid);

        return (rptentry) ? rptentry->data : NULL;
}

SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;

        rptentry = get_rptentry_by_rid(table, rid);

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

SaHpiRptEntryT *oh_get_resource_by_ep(RPTable *table, SaHpiEntityPathT *ep)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        for (node = table->rptable; node != NULL; node = node->next) {
                rptentry = (RPTEntry *) node->data;
                if (!memcmp(&(rptentry->rpt_entry.ResourceEntity), ep, sizeof(SaHpiEntityPathT)))
                else rptentry = NULL;
        }

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

SaHpiRptEntryT *oh_get_resource_next(RPTable *table, SaHpiResourceIdT rid_prev)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        for (node = table->rptable; node != NULL; node = node->next) {
                rptentry = (RPTEntry *) node->data;
                if (rptentry->rpt_entry.ResourceId == rid_prev) {
                        rptentry = (RPTEntry *)(node->next->data);
                        break;
                } else rptentry = NULL;
        }        

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

/**
 * RDR interface functions
 */
 
int oh_add_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiRdrT rdr, void *data)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);

        if (!rptentry) return -1; /* Resource was not fount in table */

        /* Check if resource exists */
        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdr.RecordId);
        /* If not, create new rdr */
        if (!rdrecord) {
                rdrecord = (RDRecord *)malloc(sizeof(RDRecord));
                if (!rdrecord) return -2; /* Could not create rdrecord */
                /* Put new rdrecord in rdr repository */
                rptentry->rdrtable = g_slist_append(rptentry->rdrtable, (gpointer)rdrecord);
        }
        /* Else, modify existing rdrecord */
        rdrecord->rdr = rdr;
        rdrecord->data = data;
        rdrecord->rdr.RecordId = get_rdr_uid(rdr.RdrType, get_rdr_type_num(rdr));

        return 0;
}

int oh_remove_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return -1; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);
        if (!rdrecord) return -2; /* No rdr found by that id */
        else g_slist_remove(rptentry->rdrtable, (gpointer)rdrecord);

        return 0;
}

void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);
        if (!rdrecord) return NULL; /* No rdr found by that id */

        return rdrecord->data;
}

SaHpiRdrT *oh_get_rdr_by_id(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);
        if (!rdrecord) return NULL; /* No rdr found by that id */

        return &(rdrecord->rdr);
}

SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;
        SaHpiEntryIdT rdrid;
        
        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        /* Get rdrid from type/num combination */
        rdrid = get_rdr_uid(type, num);
        
        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);
        if (!rdrecord) return NULL; /* No rdr found by that id */

        return &(rdrecord->rdr);
}

SaHpiRdrT *oh_get_rdr_next(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid_prev)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord = NULL;
        GSList *node;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        for (node = rptentry->rdrtable; node != NULL; node = node->next) {
                rdrecord = (RDRecord *)node->data;
                if (rdrecord->rdr.RecordId == rdrid_prev) {
                        rdrecord = (RDRecord *)node->next->data;
                        break;
                }
                else rdrecord = NULL;
        }

        return (rdrecord) ? &(rdrecord->rdr) : NULL;
}
