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

#include <stdlib.h>
#include <string.h>
#include <rpt_utils.h>
#include <openhpi.h>

/* declare Rptable object */		     
RPTable default_rpt; 

static inline RPTEntry *get_rptentry_by_rid( RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        if (rid == RPT_ENTRY_BEGIN) {
                rptentry = (RPTEntry *) (table->rptable->data);
        } else {
                for (node = table->rptable; node != NULL; node = node->next) {
                        rptentry = (RPTEntry *) node->data;
                        if (rptentry->rpt_entry.ResourceId == rid) break;
                        else rptentry = NULL;
                }        
        }

        return rptentry;
}

static inline RDRecord *get_rdrecord_by_id(GSList *records, SaHpiEntryIdT id)
{
        RDRecord *rdrecord = NULL;
        GSList *node;

        if (id == RDR_BEGIN) {
                rdrecord = (RDRecord *) records->data;
        } else {
                for (node = records; node != NULL; node = node->next) {
                        rdrecord = (RDRecord *) node->data;
                        if (rdrecord->rdr.RecordId == id) break;
                        else rdrecord = NULL;
                }                
        }        

        return rdrecord;
}

static inline SaHpiUint32T get_rdr_uid(SaHpiRdrTypeT type, SaHpiUint32T num)
{
        SaHpiUint32T uid;

        uid = ((SaHpiUint32T)type) << 16;
        uid = uid + (SaHpiUint32T)num;

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

static inline int check_ep(SaHpiEntityPathT ep)
{
        int check = -1; 
	int i;

        for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
                if (ep.Entry[i].EntityType == SAHPI_ENT_ROOT) {
                        check = 0;
                        break;
                }
        }

        return check;
}

/**
 * Resource interface functions
 */

/**
 * oh_add_resource: Add a RPT entry to the RPT along with some private data.
 * If an RPT entry with the same resource id exists int the RPT, it will be
 * overlayed with the new RPT entry. Also, this function will assign the
 * resource id as its entry id since it is expected to be unique for the table.
 * Note - If the RPT entry has a resource id of RPT_ENTRY_BEGIN(0),
 * the first RPT entry in the table will be overlayed.
 * @table: Pointer to the RPT to which the RPT entry will be added.
 * @entry: The RPT entry (resource) to be added to the RPT.
 * @data: Pointer to private data for storing along with the RPT entry.
 *
 * Return value:
 * 0 - successful addition to the RPT.
 * -1 - entry does not have an id assigned.
 * -2 - entity path does not contain root element.
 * -3 - failure and not enough memory could be allocated
 * for a new position in the RPT.
 **/
int oh_add_resource(RPTable *table, SaHpiRptEntryT entry, void *data)
{
        RPTEntry *rptentry;

        /* Check to see if the entry is in the RPTable already */
        if (entry.ResourceId == 0) return -1; /* Entry needs an id before being added */
        if (check_ep(entry.ResourceEntity)) return -2; /* Entity path does not contain root element */
        rptentry = get_rptentry_by_rid(table, entry.ResourceId);
        
        /* If not, create new RPTEntry */
        if (!rptentry) {
                rptentry = (RPTEntry *)malloc(sizeof(RPTEntry));
                if (!rptentry) return -3; /* Not enough memory to add resource. */
                /* Put new RPTEntry in RPTable */
                table->rptable = g_slist_append(table->rptable, (gpointer)rptentry);                
        }
        /* Else, modify existing RPTEntry */
        rptentry->data = data;
        rptentry->rpt_entry = entry;
        rptentry->rpt_entry.EntryId = entry.ResourceId; /** Is this ok? */
                       
        return 0; 
}

/**
 * oh_remove_resource: Remove a resource from the RPT. If the rid is
 * RPT_ENTRY_BEGIN (0), the first RPT entry in the table will be removed.
 * @table: Pointer to the RPT from which an RPT entry will be removed.
 * @rid: Resource id of the RPT entry to be removed.
 *
 * Return value:
 * 0 - Successful removal from the RPT.
 * -1 - Failure. No resource found by that id.
 **/
int oh_remove_resource(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;
        
        rptentry = get_rptentry_by_rid(table, rid);

        if (!rptentry) return -1; /* No Resource found by that id */
        else table->rptable = g_slist_remove(table->rptable, (gpointer)rptentry);

        return 0;
}

/**
 * oh_get_resource_data: Get the private data for a RPT entry.  If the rid is
 * RPT_ENTRY_BEGIN (0), the first RPT entry's data in the table will be returned.
 * @table: Pointer to the RPT for looking up the RPT entry's private data.
 * @rid: Resource id of the RPT entry that holds the private data.
 *
 * Return value:
 * A void pointer to the private data for the RPT entry requested, or NULL
 * if the RPT entry was not found.
 **/
void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid)
{
        
        RPTEntry *rptentry;

        rptentry = get_rptentry_by_rid(table, rid);

        return (rptentry) ? rptentry->data : NULL;
}

/**
 * oh_get_resource_by_id: Get a RPT entry from the RPT by using the resource id.
 * If rid is RPT_ENTRY_BEGIN (0), the first RPT entry in the table will be returned.
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @rid: Resource id of the RPT entry to be looked up.
 *
 * Return value:
 * Pointer to the RPT entry found or NULL if an RPT entry by that
 * id was not found.
 **/
SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;

        rptentry = get_rptentry_by_rid(table, rid);

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

/**
 * oh_get_resource_by_ep: Get a RPT entry from the RPT by using the entity path.
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @ep: Entity path of the RPT entry to be looked up.
 *
 * Return value:
 * Pointer to the RPT entry found or NULL if an RPT entry by that
 * entity path was not found.
 **/
SaHpiRptEntryT *oh_get_resource_by_ep(RPTable *table, SaHpiEntityPathT *ep)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        for (node = table->rptable; node != NULL; node = node->next) {
                rptentry = (RPTEntry *) node->data;
                if (!memcmp(&(rptentry->rpt_entry.ResourceEntity), ep, sizeof(SaHpiEntityPathT)))
                        break;
                else rptentry = NULL;
        }

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

/**
 * oh_get_resource_next: Get the RPT entry next to the specified RPT entry
 * from the RPT. If rid_prev is RPT_ENTRY_BEGIN (0), the first RPT entry
 * in the table will be returned.
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @rid_prev: Resource id of the RPT entry previous to the one being looked up.
 *
 * Return value:
 * Pointer to the RPT entry found or NULL if the previous RPT entry by that
 * id was not found.
 **/
SaHpiRptEntryT *oh_get_resource_next(RPTable *table, SaHpiResourceIdT rid_prev)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        if (rid_prev == RPT_ENTRY_BEGIN) {
                rptentry = (RPTEntry *) (table->rptable->data);
        } else {
                for (node = table->rptable; node != NULL; node = node->next) {
                        rptentry = (RPTEntry *) node->data;
                        if (rptentry->rpt_entry.ResourceId == rid_prev) {
                                rptentry = (RPTEntry *)(node->next->data);
                                break;
                        } else rptentry = NULL;
                }                
        }                

        return (rptentry) ? &(rptentry->rpt_entry) : NULL;
}

/**
 * RDR interface functions
 */

/**
 * oh_add_rdr: Add an RDR to a RPT entry's RDR repository.
 * If an RDR is found with the same record id as the one being added, the RDR being
 * added will overlay the existing one. Also, a unique record id will be assigned
 * to it based on the RDR type and its type's numeric id.
 * @table: Pointer to RPT table containig the RPT entry to which the RDR will belong.
 * @rid: Id of the RPT entry that will own the RDR to be added.
 * @rdr: RDR to be added to an RPT entry's RDR repository.
 * @data: Pointer to private data belonging to the RDR that is being added.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * 0 - Successful addition of RDR.
 * -1 - Failure. RPT entry for that rid was not found.
 * -2 - Failure. RDR entity path is different from parent RPT entry.
 * -3 - Failure. RDR type number has not been assigned.
 * -4 - Failure. Could not allocate enough memory to position the new RDR in the RDR
 * repository.
 **/ 
int oh_add_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiRdrT rdr, void *data)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);

        if (!rptentry) return -1; /* Resource was not fount in table */
dbg("TODO: RENIER check line below");
//        if (memcmp( &rptentry->rpt_entry.ResourceEntity, &rdrecord->rdr.Entity, sizeof(SaHpiEntityPathT)))
                return -2; /* RDR entity path is different from parent RPT entry. */
        if (!get_rdr_type_num(rdr)) return -3; /* RDR type number has not been assigned. */

        /* Check if record exists */
        rdr.RecordId = get_rdr_uid(rdr.RdrType, get_rdr_type_num(rdr));
        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdr.RecordId);
                
        /* If not, create new rdr */
        if (!rdrecord) {
                rdrecord = (RDRecord *)malloc(sizeof(RDRecord));
                if (!rdrecord) return -4; /* Could not create rdrecord */
                /* Put new rdrecord in rdr repository */
                rptentry->rdrtable = g_slist_append(rptentry->rdrtable, (gpointer)rdrecord);                        
        }
        /* Else, modify existing rdrecord */        
        rdrecord->rdr = rdr;
        rdrecord->data = data;
        
        return 0;
}

/**
 * oh_remove_rdr: Remove an RDR from a RPT entry's RDR repository.
 * If rdrid is RDR_BEGIN (0), the first RDR in the repository will be removed.
 * @table: Pointer to RPT table containig the RPT entry from which the RDR will
 * be removed.
 * @rid: Id of the RPT entry from which the RDR will be removed.
 * @rdrid: Record id of the RDR to remove.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * 0 - Successful removal of RDR.
 * -1 - Failure. RPT entry for that rid was not found.
 * -2 - Failure. No RDR by that rdrid was found.
 **/
int oh_remove_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return -1; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);
        if (!rdrecord) return -2; /* No rdr found by that id */
        else rptentry->rdrtable = g_slist_remove(rptentry->rdrtable, (gpointer)rdrecord);

        return 0;
}

/**
 * oh_get_rdr_data: Get the private data associated to an RDR.
 * If rdrid is RDR_BEGIN (0), the first RDR's data in the repository will be returned.
 * @table: Pointer to RPT table containig the RPT entry from which the RDR's data
 * will be read.
 * @rid: Id of the RPT entry from which the RDR's data will be read.
 * @rdrid: Record id of the RDR to read data from.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * A void pointer to the RDR data, or NULL if no data for that RDR was found.
 **/
void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);

        return (rdrecord) ? rdrecord->data : NULL;
}

/**
 * oh_get_rdr_by_id: Get a reference to an RDR by its record id.
 * If rdrid is RDR_BEGIN (0), the first RDR in the repository will be returned.
 * @table: Pointer to RPT table containig the RPT entry tha has the RDR
 * being looked up.
 * @rid: Id of the RPT entry containing the RDR being looked up.
 * @rdrid: Record id of the RDR being looked up.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * Reference to the RDR looked up or NULL if no RDR was found.
 **/
SaHpiRdrT *oh_get_rdr_by_id(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, rdrid);

        return (rdrecord) ? &(rdrecord->rdr) : NULL;
}

/**
 * oh_get_rdr_by_type: Get a reference to an RDR by its type and number.
 * @table: Pointer to RPT table containig the RPT entry tha has the RDR
 * being looked up.
 * @rid: Id of the RPT entry containing the RDR being looked up.
 * @type: RDR Type of the RDR being looked up.
 * @num: RDR id within the RDR type for the specified RPT entry.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * Reference to the RDR looked up or NULL if no RDR was found.
 **/
SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;
        SaHpiUint32T rdr_uid;
                        
        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        /* Get rdr_uid from type/num combination */
        rdr_uid = get_rdr_uid(type, num);
        rdrecord = get_rdrecord_by_id(rptentry->rdrtable, (SaHpiEntryIdT)rdr_uid);       

        return (rdrecord) ? &(rdrecord->rdr) : NULL;
}

/**
 * oh_get_rdr_next: Get the RDR next to the specified RDR in the specified
 * RPT entry's repository. If rdrid_prev is RDR_BEGIN (0), the first RDR
 * in the repository will be returned.
 * @table: Pointer to the RPT containing the RPT entry to look up the RDR in.
 * @rid_prev: Record id of the RDR previous to the one being looked up, relative
 * to the specified RPT entry.
 *
 * All rdr interface funtions, except oh_add_rdr will act in the context of
 * the first RPT entry in the table, if rid is RPT_ENTRY_BEGIN (0).
 *
 * Return value:
 * Pointer to the RDR found or NULL if the previous RDR by that
 * id was not found. If the rdrid_prev was 0 (RDR_BEGIN), the first RDR in the list
 * will be returned.
 **/
SaHpiRdrT *oh_get_rdr_next(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid_prev)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord = NULL;
        GSList *node;

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) return NULL; /* No resource found by that id */

        if (rdrid_prev == RDR_BEGIN) {
                rdrecord = (RDRecord *)(rptentry->rdrtable->data);
        } else {
                for (node = rptentry->rdrtable; node != NULL; node = node->next) {
                        rdrecord = (RDRecord *)node->data;
                        if (rdrecord->rdr.RecordId == rdrid_prev) {
                                rdrecord = (RDRecord *)(node->next->data);
                                break;
                        }
                        else rdrecord = NULL;
                }                
        }        

        return (rdrecord) ? &(rdrecord->rdr) : NULL;
}
