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
 */

#include <string.h>
#include <sys/time.h>

#include <oh_utils.h>

static RPTEntry *get_rptentry_by_rid(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry = NULL;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return NULL;
        }

        if (!(table->rptlist)) {
                /*dbg("Info: RPT is empty.");*/
                return NULL;
        }

        if (rid == SAHPI_FIRST_ENTRY) {
                rptentry = (RPTEntry *) (table->rptlist->data);
        } else {
                rptentry = (RPTEntry *)g_hash_table_lookup(table->rptable, &rid);
        }

        return rptentry;
}

static RDRecord *get_rdrecord_by_id(RPTEntry *rptentry, SaHpiEntryIdT id)
{
        RDRecord *rdrecord = NULL;

        if (!rptentry) {
                dbg("ERROR: Cannot lookup rdr inside null resource.");
                return NULL;
        }

        if (!rptentry->rdrlist) {
                /*dbg("Info: RDR repository is empty.");*/
                return NULL;
        }

        if (id == SAHPI_FIRST_ENTRY) {
                rdrecord = (RDRecord *) (rptentry->rdrlist->data);
        } else {
                rdrecord = (RDRecord *)g_hash_table_lookup(rptentry->rdrtable, &id);
        }

        return rdrecord;
}

static SaHpiUint8T get_rdr_type_num(SaHpiRdrT *rdr)
{
        SaHpiUint8T num = 0;

        switch (rdr->RdrType) {
                case SAHPI_CTRL_RDR:
                        num = rdr->RdrTypeUnion.CtrlRec.Num;
                        break;
                case SAHPI_SENSOR_RDR:
                        num = rdr->RdrTypeUnion.SensorRec.Num;
                        break;
                case SAHPI_INVENTORY_RDR:
                        num = rdr->RdrTypeUnion.InventoryRec.IdrId;
                        break;
                case SAHPI_WATCHDOG_RDR:
                        num = rdr->RdrTypeUnion.WatchdogRec.WatchdogNum;
                        break;
                default:
                        num = 0;
        }

        return num;
}

static void update_rptable(RPTable *table) {
        struct timeval tv;
        SaHpiTimeT time;

        if (!table) {dbg("ERROR: Cannot work on a null table pointer."); return;}
        if (!table->info) {
                dbg("ERROR: This RPT has not been initialized! At least, call oh_init_rpt(NULL)");
                return;
        }                

        gettimeofday(&tv, NULL);
        time = (SaHpiTimeT) tv.tv_sec * 1000000000 + tv.tv_usec * 1000;

        table->info->RptUpdateTimestamp = time;

        table->info->RptUpdateCount = table->info->RptUpdateCount + 1;
}

/**
 * get_rdr_uid
 * @type: type of rdr
 * @num: id number of the RDR unique withing the RDR type for that resource
 *
 * Helper function to derive the Record id of an rdr from its @type and @num
 *
 * Returns: a derived Record Id used to identify RDRs within Resources
 */
SaHpiUint32T get_rdr_uid(SaHpiRdrTypeT type, SaHpiUint32T num)
{
        SaHpiUint32T uid;

        uid = ((SaHpiUint32T)type) << 16;
        uid = uid + (SaHpiUint32T)num;

        return uid;
}

/**
 * General RPT calls
 **/

/**
 * oh_init_rpt
 * @info: Pointer to the domain info structure that will be updated
 * with the rpt update count and update timestamp.
 *
 * If @info is passed in as NULL, the rpt will be initialized to use its own
 * internal domain info.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_init_rpt(RPTable *table, SaHpiDomainInfoT *info)
{
        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!info)
                table->info = &(table->local_info);
        else
                table->info = info;

        return SA_OK;
}
 
/**
 * oh_flush_rpt
 * @table: Pointer to the RPT to flush.
 *
 * Cleans RPT from all entries and RDRs and frees the memory
 * associated with them.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_flush_rpt(RPTable *table)
{
        SaHpiRptEntryT *tmp_entry;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        while ((tmp_entry = oh_get_resource_by_id(table, SAHPI_FIRST_ENTRY)) != NULL) {
                oh_remove_resource(table, SAHPI_FIRST_ENTRY);
        }

        return SA_OK;
}

/**
 * rpt_diff
 * @cur_rpt: IN. Pointer to RPTable that represents the current state of resources
 * and rdrs.
 * @new_rpt: IN. Pointer to RPTable that contains rpt entries and rdrs just recently
 * discovered.
 * @res_new: OUT. List of new or changed rpt entries
 * @rdr_new: OUT. List of new or changed rdrs
 * @res_gone: OUT. List of old and not present resources.
 * @rdr_gone: OUT. List of old and not present rdrs.
 *
 * Extracts from current the resources and rdrs that are not found
 * in new and puts them in res_gone and rdr_gone. Also, puts in res_new and rdr_new
 * the resources and rdrs that are not already in current Or that are not identical
 * to the ones in current.
 *
 * Returns: void.
 **/
void rpt_diff(RPTable *cur_rpt, RPTable *new_rpt,
              GSList **res_new, GSList **rdr_new,
              GSList **res_gone, GSList **rdr_gone) {

        SaHpiRptEntryT *res = NULL;

        /* Look for absent resources and rdrs */
        for (res = oh_get_resource_by_id(cur_rpt, SAHPI_FIRST_ENTRY);
             res != NULL;
             res = oh_get_resource_next(cur_rpt, res->ResourceId)) {

                SaHpiRptEntryT *tmp_res = oh_get_resource_by_id(new_rpt, res->ResourceId);

                if (tmp_res == NULL) *res_gone = g_slist_append(*res_gone, (gpointer)res);
                else {
                        SaHpiRdrT *rdr = NULL;

                        for (rdr = oh_get_rdr_by_id(cur_rpt, res->ResourceId, SAHPI_FIRST_ENTRY);
                             rdr != NULL;
                             rdr = oh_get_rdr_next(cur_rpt, res->ResourceId, rdr->RecordId)) {

                                SaHpiRdrT *tmp_rdr =
                                        oh_get_rdr_by_id(new_rpt, res->ResourceId, rdr->RecordId);

                                if (tmp_rdr == NULL)
                                        *rdr_gone = g_slist_append(*rdr_gone, (gpointer)rdr);
                        }
                }
        }

        /* Look for new resources and rdrs*/
        for (res = oh_get_resource_by_id(new_rpt, SAHPI_FIRST_ENTRY);
             res != NULL;
             res = oh_get_resource_next(new_rpt, res->ResourceId)) {

                SaHpiRptEntryT *tmp_res = oh_get_resource_by_id(cur_rpt, res->ResourceId);
                SaHpiRdrT *rdr = NULL;
                if (!tmp_res || memcmp(res, tmp_res, sizeof(SaHpiRptEntryT))) {
                        *res_new = g_slist_append(*res_new, (gpointer)res);
                }



                for (rdr = oh_get_rdr_by_id(new_rpt, res->ResourceId, SAHPI_FIRST_ENTRY);
                     rdr != NULL;
                     rdr = oh_get_rdr_next(new_rpt, res->ResourceId, rdr->RecordId)) {

                        SaHpiRdrT *tmp_rdr = NULL;

                        if (tmp_res != NULL)
                                tmp_rdr = oh_get_rdr_by_id(cur_rpt, res->ResourceId, rdr->RecordId);

                        if (tmp_rdr == NULL || memcmp(rdr, tmp_rdr, sizeof(SaHpiRdrT)))
                                *rdr_new = g_slist_append(*rdr_new, (gpointer)rdr);

                }
        }
}

/**
 * oh_get_rpt_info
 * @table: pointer to RPT
 * @update_count: pointer of where to place the rpt's update count
 * @update_timestamp: pointer of where to place the rpt's update timestamp
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_get_rpt_info(RPTable *table,
                         SaHpiUint32T *update_count,
                         SaHpiTimeT *update_timestamp)
{
        if (!table || update_count || update_timestamp) {
                dbg("ERROR: Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!table->info) {
                dbg("ERROR: This RPT has not been initialized! At least, call oh_init_rpt(NULL)");
                return SA_ERR_HPI_ERROR;
        }

        *update_count = table->info->RptUpdateCount;
        *update_timestamp = table->info->RptUpdateTimestamp;

        return SA_OK;
}

/**
 * Resource interface functions
 */

/**
 * oh_add_resource
 * @table: Pointer to the RPT to which the RPT entry will be added.
 * @entry: The RPT entry (resource) to be added to the RPT.
 * @data: Pointer to private data for storing along with the RPT entry.
 * @owndata: boolean flag. true (%KEEP_RPT_DATA) to tell the interface *not*
 * to free the data when the resource is removed. false (%FREE_RPT_DATA) to tell
 * the interface to free the data when the resource is removed.
 *
 * Add a RPT entry to the RPT along with some private data.
 * If an RPT entry with the same resource id exists int the RPT, it will be
 * overlayed with the new RPT entry. Also, this function will assign the
 * resource id as its entry id since it is expected to be unique for the table.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_add_resource(RPTable *table, SaHpiRptEntryT *entry, void *data, int owndata)
{
        RPTEntry *rptentry;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!entry) {
                dbg("Failed to add. RPT entry is NULL.");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (entry->ResourceId == SAHPI_FIRST_ENTRY) {
                dbg("Failed to add. RPT entry needs a resource id before being added");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (entry->ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID) {
                dbg("Failed to add. RPT entry has an invalid/reserved id assigned. (SAHPI_UNSPECIFIED_RESOURCE_ID)");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (validate_ep(&(entry->ResourceEntity))) {
                dbg("Failed to add RPT entry. Entity path does not contain root element.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entry->EntryId = entry->ResourceId;
        /* Check to see if the entry is in the RPTable already */
        rptentry = get_rptentry_by_rid(table, entry->ResourceId);
        /* If not, create new RPTEntry */
        if (!rptentry) {
                rptentry = (RPTEntry *)g_malloc0(sizeof(RPTEntry));
                if (!rptentry) {
                        dbg("Not enough memory to add RPT entry.");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
                /* Put new RPTEntry in RPTable */
                table->rptlist = g_slist_append(table->rptlist, (gpointer)rptentry);

                /* Add to rpt hash table */
                if (!table->rptable) /* Create hash table if it doesn't exist */
                        table->rptable = g_hash_table_new(g_int_hash, g_int_equal);

                rptentry->rpt_entry.EntryId = entry->ResourceId;
                g_hash_table_insert(table->rptable,
                                    &(rptentry->rpt_entry.EntryId),
                                    rptentry);
        }
        /* Else, modify existing RPTEntry */
        if (rptentry->data && rptentry->data != data && !rptentry->owndata)
                g_free(rptentry->data);
        rptentry->data = data;
        rptentry->owndata = owndata;        
        rptentry->rpt_entry = *entry;

        update_rptable(table);

        return SA_OK;
}

/**
 * oh_remove_resource
 * @table: Pointer to the RPT from which an RPT entry will be removed.
 * @rid: Resource id of the RPT entry to be removed.
 *
 * Remove a resource from the RPT. If the @rid is
 * %SAHPI_FIRST_ENTRY, the first RPT entry in the table will be removed.
 * The void data will be freed if @owndata was false (%FREE_RPT_DATA) when adding
 * the resource, otherwise if @owndata was true (%KEEP_RPT_DATA) it will not be freed.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_remove_resource(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Failed to remove RPT entry. No Resource found by that id");
                return SA_ERR_HPI_NOT_PRESENT;
        } else {
                SaHpiRdrT *tmp_rdr;
                /* Remove all RDRs for the resource first */
                while ((tmp_rdr = oh_get_rdr_by_id(table, rid, SAHPI_FIRST_ENTRY)) != NULL) {
                        oh_remove_rdr(table, rid, SAHPI_FIRST_ENTRY);
                }
                /* then remove the resource itself. */
                table->rptlist = g_slist_remove(table->rptlist, (gpointer)rptentry);
                if (!rptentry->owndata) g_free(rptentry->data);
                g_hash_table_remove(table->rptable, &(rptentry->rpt_entry.EntryId));
                g_free((gpointer)rptentry);
                if (!table->rptlist) {
                        g_hash_table_destroy(table->rptable);
                        table->rptable = NULL;
                }
        }

        update_rptable(table);

        return SA_OK;
}

/**
 * oh_get_resource_data
 * @table: Pointer to the RPT for looking up the RPT entry's private data.
 * @rid: Resource id of the RPT entry that holds the private data.
 *
 * Get the private data for a RPT entry.  If the @rid is
 * %SAHPI_FIRST_ENTRY, the first RPT entry's data in the table will be returned.
 *
 * Returns: A void pointer to the private data for the RPT entry requested, or NULL
 * if the RPT entry was not found or the table was a NULL pointer.
 **/
void *oh_get_resource_data(RPTable *table, SaHpiResourceIdT rid)
{

        RPTEntry *rptentry;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                /*dbg("Warning: RPT entry not found. Returning NULL.");*/
                return NULL;
        }

        return rptentry->data;
}

/**
 * oh_get_resource_by_id
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @rid: Resource id of the RPT entry to be looked up.
 *
 * Get a RPT entry from the RPT by using the resource id.
 * If @rid is %SAHPI_FIRST_ENTRY, the first RPT entry in the table will be returned.
 *
 * Returns:
 * Pointer to the RPT entry found or NULL if an RPT entry by that
 * id was not found or the table was a NULL pointer.
 **/
SaHpiRptEntryT *oh_get_resource_by_id(RPTable *table, SaHpiResourceIdT rid)
{
        RPTEntry *rptentry;

        if (!table) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                /*dbg("Warning: RPT entry not found. Returning NULL.");*/
                return NULL;
        }

        return &(rptentry->rpt_entry);
}

/**
 * oh_get_resource_by_ep
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @ep: Entity path of the RPT entry to be looked up.
 *
 * Get a RPT entry from the RPT by using the entity path.
 *
 * Returns:
 * Pointer to the RPT entry found or NULL if an RPT entry by that
 * entity path was not found or the table was a NULL pointer.
 **/
SaHpiRptEntryT *oh_get_resource_by_ep(RPTable *table, SaHpiEntityPathT *ep)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        if (!(table)) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return NULL;
        }

        for (node = table->rptlist; node != NULL; node = node->next) {
                rptentry = (RPTEntry *) node->data;
                if (!ep_cmp(&(rptentry->rpt_entry.ResourceEntity), ep))
                        break;
                else rptentry = NULL;
        }

        if (!rptentry) {
                /*dbg("Warning: RPT entry not found. Returning NULL.");*/
                return NULL;
        }

        return &(rptentry->rpt_entry);
}

/**
 * oh_get_resource_next
 * @table: Pointer to the RPT for looking up the RPT entry.
 * @rid_prev: Resource id of the RPT entry previous to the one being looked up.
 *
 * Get the RPT entry next to the specified RPT entry
 * from the RPT. If @rid_prev is %SAHPI_FIRST_ENTRY, the first RPT entry
 * in the table will be returned.
 *
 * Returns:
 * Pointer to the RPT entry found or NULL if the previous RPT entry by that
 * id was not found or the table was a NULL pointer.
 **/
SaHpiRptEntryT *oh_get_resource_next(RPTable *table, SaHpiResourceIdT rid_prev)
{
        RPTEntry *rptentry = NULL;
        GSList *node;

        if (!(table)) {
                dbg("ERROR: Cannot work on a null table pointer.");
                return NULL;
        }

        if (rid_prev == SAHPI_FIRST_ENTRY) {
                if (table->rptlist) {
                        rptentry = (RPTEntry *)(table->rptlist->data);
                } else {
                        /*dbg("Info: RPT is empty. Returning NULL.");*/
                        return NULL;
                }
        } else {
                for (node = table->rptlist; node != NULL; node = node->next) {
                        rptentry = (RPTEntry *) node->data;
                        if (rptentry->rpt_entry.ResourceId == rid_prev) {
                                if (node->next) rptentry = (RPTEntry *)(node->next->data);
                                else {
                                        /*dbg("Info: Reached end of RPT.");*/
                                        return NULL;
                                }
                                break;
                        } else rptentry = NULL;
                }
        }

        if (!rptentry) {
                /*dbg("Warning: RPT entry not found. Returning NULL.");*/
                return NULL;
        }

        return &(rptentry->rpt_entry);
}

/**
 * RDR interface functions
 */

/**
 * oh_add_rdr
 * @table: Pointer to RPT table containig the RPT entry to which the RDR will belong.
 * @rid: Id of the RPT entry that will own the RDR to be added.
 * @rdr: RDR to be added to an RPT entry's RDR repository.
 * @data: Pointer to private data belonging to the RDR that is being added.
 * @owndata: boolean flag. true (%KEEP_RPT_DATA) to tell the interface *not*
 * to free the data when the rdr is removed. false (%FREE_RPT_DATA) to tell
 * the interface to free the data when the rdr is removed.
 *
 * Add an RDR to a RPT entry's RDR repository.
 * If an RDR is found with the same record id as the one being added, the RDR being
 * added will overlay the existing one. Also, a unique record id will be assigned
 * to it based on the RDR type and its type's numeric id.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_add_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiRdrT *rdr, void *data, int owndata)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;
        SaHpiUint8T type_num;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (!rdr) {
                dbg("Failed to add. RDR is NULL.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry){
                dbg("Failed to add RDR. Parent RPT entry was not found in table.");
                return SA_ERR_HPI_NOT_PRESENT;
        }

        type_num = get_rdr_type_num(rdr);

        /* Form correct RecordId. */
        rdr->RecordId = get_rdr_uid(rdr->RdrType, type_num);
        /* Check if record exists */
        rdrecord = get_rdrecord_by_id(rptentry, rdr->RecordId);
        /* If not, create new rdr */
        if (!rdrecord) {
                rdrecord = (RDRecord *)g_malloc0(sizeof(RDRecord));
                if (!rdrecord) {
                        dbg("Not enough memory to add RDR.");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
                /* Put new rdrecord in rdr repository */
                rptentry->rdrlist = g_slist_append(rptentry->rdrlist, (gpointer)rdrecord);
                /* Create rdr hash table if first rdr here */
                if (!rptentry->rdrtable)
                        rptentry->rdrtable = g_hash_table_new(g_int_hash, g_int_equal);

                rdrecord->rdr.RecordId = rdr->RecordId;
                g_hash_table_insert(rptentry->rdrtable,
                                    &(rdrecord->rdr.RecordId),
                                    rdrecord);
        }
        /* Else, modify existing rdrecord */
        if (rdrecord->data && rdrecord->data != data && !rdrecord->owndata)
                g_free(rdrecord->data);
        rdrecord->data = data;
        rdrecord->owndata = owndata;
        rdrecord->rdr = *rdr;        

        return SA_OK;
}

/**
 * oh_remove_rdr
 * @table: Pointer to RPT table containig the RPT entry from which the RDR will
 * be removed.
 * @rid: Id of the RPT entry from which the RDR will be removed.
 * @rdrid: Record id of the RDR to remove.
 *
 *
 * Remove an RDR from a RPT entry's RDR repository.
 * If @rdrid is %SAHPI_FIRST_ENTRY, the first RDR in the repository will be removed.
 * If @owndata was set to false (%FREE_RPT_DATA) on the rdr when it was added,
 * the data will be freed, otherwise if it was set to true (%KEEP_RPT_DATA),
 * it will not be freed.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns: SA_OK on success Or minus SA_OK on error.
 **/
SaErrorT oh_remove_rdr(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Failed to remove RDR. Parent RPT entry was not found.");
                return SA_ERR_HPI_NOT_PRESENT;
        }

        rdrecord = get_rdrecord_by_id(rptentry, rdrid);
        if (!rdrecord) {
                dbg("Failed to remove RDR. Could not be found.");
                return SA_ERR_HPI_NOT_PRESENT;
        } else {
                rptentry->rdrlist = g_slist_remove(rptentry->rdrlist, (gpointer)rdrecord);
                if (!rdrecord->owndata) g_free(rdrecord->data);
                g_hash_table_remove(rptentry->rdrtable, &(rdrecord->rdr.RecordId));
                g_free((gpointer)rdrecord);
                if (!rptentry->rdrlist) {
                        g_hash_table_destroy(rptentry->rdrtable);
                        rptentry->rdrtable = NULL;
                }
        }

        return SA_OK;
}

/**
 * oh_get_rdr_data
 * @table: Pointer to RPT table containig the RPT entry from which the RDR's data
 * will be read.
 * @rid: Id of the RPT entry from which the RDR's data will be read.
 * @rdrid: Record id of the RDR to read data from.
 *
 *
 * Get the private data associated to an RDR.
 * If @rdrid is %SAHPI_FIRST_ENTRY, the first RDR's data in the repository will be returned.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns: A void pointer to the RDR data, or NULL if no data for that RDR was found or
 * the table pointer is NULL.
 **/
void *oh_get_rdr_data(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Warning: RPT entry not found. Cannot find RDR.");
                return NULL; /* No resource found by that id */
        }

        rdrecord = get_rdrecord_by_id(rptentry, rdrid);
        if (!rdrecord) {
                /*dbg("Warning: RDR not found. Returning NULL.");*/
                return NULL;
        }

        return rdrecord->data;
}

/**
 * oh_get_rdr_by_id
 * @table: Pointer to RPT table containig the RPT entry tha has the RDR
 * being looked up.
 * @rid: Id of the RPT entry containing the RDR being looked up.
 * @rdrid: Record id of the RDR being looked up.
 *
 * Get a reference to an RDR by its record id.
 * If @rdrid is %SAHPI_FIRST_ENTRY, the first RDR in the repository will be returned.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns:
 * Reference to the RDR looked up or NULL if no RDR was found.
 **/
SaHpiRdrT *oh_get_rdr_by_id(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Warning: RPT entry not found. Cannot find RDR.");
                return NULL; /* No resource found by that id */
        }

        rdrecord = get_rdrecord_by_id(rptentry, rdrid);
        if (!rdrecord) {
                /*dbg("Warning: RDR not found. Returning NULL.");*/
                return NULL;
        }

        return &(rdrecord->rdr);
}

/**
 * oh_get_rdr_by_type
 * @table: Pointer to RPT table containig the RPT entry tha has the RDR
 * being looked up.
 * @rid: Id of the RPT entry containing the RDR being looked up.
 * @type: RDR Type of the RDR being looked up.
 * @num: RDR id within the RDR type for the specified RPT entry.
 *
 * Get a reference to an RDR by its type and number.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns:
 * Reference to the RDR looked up or NULL if no RDR was found.
 **/
SaHpiRdrT *oh_get_rdr_by_type(RPTable *table, SaHpiResourceIdT rid,
                              SaHpiRdrTypeT type, SaHpiUint8T num)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord;
        SaHpiUint32T rdr_uid;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Warning: RPT entry not found. Cannot find RDR.");
                return NULL; /* No resource found by that id */
        }

        /* Get rdr_uid from type/num combination */
        rdr_uid = get_rdr_uid(type, num);
        rdrecord = get_rdrecord_by_id(rptentry, (SaHpiEntryIdT)rdr_uid);
        if (!rdrecord) {
                /*dbg("Warning: RDR not found. Returning NULL.");*/
                return NULL;
        }

        return &(rdrecord->rdr);
}

/**
 * oh_get_rdr_next
 * @table: Pointer to the RPT containing the RPT entry to look up the RDR in.
 * @rid: Id of the RPT entry containing the RDR being looked up.
 * @rdrid_prev: Record id of the RDR previous to the one being looked up, relative
 * to the specified RPT entry.
 *
 * Get the RDR next to the specified RDR in the specified
 * RPT entry's repository. If @rdrid_prev is %SAHPI_FIRST_ENTRY, the first RDR
 * in the repository will be returned.
 * All rdr interface funtions, except oh_add_rdr() will act in the context of
 * the first RPT entry in the table, if @rid is %SAHPI_FIRST_ENTRY.
 *
 * Returns:
 * Pointer to the RDR found or NULL if the previous RDR by that
 * id was not found. If the @rdrid_prev was %SAHPI_FIRST_ENTRY, the first RDR in the list
 * will be returned.
 **/
SaHpiRdrT *oh_get_rdr_next(RPTable *table, SaHpiResourceIdT rid, SaHpiEntryIdT rdrid_prev)
{
        RPTEntry *rptentry;
        RDRecord *rdrecord = NULL;
        GSList *node;

        if (!table) {
                dbg("Error: Cannot work on a null table pointer.");
                return NULL;
        }

        rptentry = get_rptentry_by_rid(table, rid);
        if (!rptentry) {
                dbg("Warning: RPT entry not found. Cannot find RDR.");
                return NULL; /* No resource found by that id */
        }

        if (rdrid_prev == SAHPI_FIRST_ENTRY) {
                if (rptentry->rdrlist) {
                        rdrecord = (RDRecord *)(rptentry->rdrlist->data);
                } else {
                        /*dbg("Info: RDR repository is empty. Returning NULL.");*/
                        return NULL;
                }
        } else {
                for (node = rptentry->rdrlist; node != NULL; node = node->next) {
                        rdrecord = (RDRecord *)node->data;
                        if (rdrecord->rdr.RecordId == rdrid_prev) {
                                if (node->next) rdrecord = (RDRecord *)(node->next->data);
                                else {
                                        /*dbg("Info: Reached end of RDR repository.")*/
                                        return NULL;
                                }
                                break;
                        }
                        else rdrecord = NULL;
                }
        }

        if (!rdrecord) {
                /*dbg("Warning: RDR not found. Returning NULL.");*/
                return NULL;
        }

        return &(rdrecord->rdr);
}
