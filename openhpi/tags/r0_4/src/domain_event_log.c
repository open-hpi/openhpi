#if 0


struct list_entry_ops {
        void                    (*get_entry)(void *, SaHpiSelEntryT *);
        struct oh_resource      *(*get_res)(void *);
        struct oh_rdr           *(*get_rdr)(void *);
};

static SaErrorT process_sel_entry(
                GSList *sel_list, 
                SaHpiSelEntryIdT EntryId,
                SaHpiSelEntryIdT *PrevEntryId,
                SaHpiSelEntryIdT *NextEntryId,
                SaHpiSelEntryT *EventLogEntry,
                SaHpiRdrT *Rdr,
                SaHpiRptEntryT *RptEntry,
                struct list_entry_ops *ops)
{
        struct oh_resource *res;
        struct oh_rdr *rdr;
        GSList *pi, *i, *ni;
        SaHpiSelEntryT entry;

        data_access_lock();

        if (sel_list==NULL) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        } else if (EntryId==SAHPI_OLDEST_ENTRY) {
                pi = NULL;
                i  = g_slist_nth(sel_list, 0);
                ni = g_slist_nth(sel_list, 1);
        } else if (EntryId==SAHPI_NEWEST_ENTRY) {
                int num = g_slist_length(sel_list);
                pi  = g_slist_nth(sel_list, num-2);
                i   = g_slist_nth(sel_list, num-1);
                ni  = NULL;
        } else {
                int num = EntryId;
                pi = g_slist_nth(sel_list, num-1);
                i  = g_slist_nth(sel_list, num);
                ni = g_slist_nth(sel_list, num+1);
        }
        
        if (!i) {
                data_access_unlock();
                return SA_ERR_HPI_INVALID;
        }
        
        ops->get_entry(i->data, EventLogEntry);
        EventLogEntry->EntryId =  EntryId;
        res = ops->get_res(i->data);
        
        if (RptEntry) {
                if (res) 
                        memcpy(RptEntry, &res->entry, sizeof(*RptEntry));
                else
                        RptEntry->ResourceCapabilities = 0;
        }

        if (Rdr) {
                if (res) 
                        rdr = ops->get_rdr(i->data);
                else 
                        rdr = NULL;
                if (rdr)
                        memcpy(Rdr, &rdr->rdr, sizeof(*Rdr));
                else 
                        Rdr->RdrType = SAHPI_NO_RECORD;
        }
        
        if (pi) {
                ops->get_entry(pi->data, &entry);
                *PrevEntryId = entry.EntryId;
        } else 
                *PrevEntryId = SAHPI_NO_MORE_ENTRIES;

        if (ni) {
                ops->get_entry(ni->data, &entry);
                *NextEntryId = entry.EntryId;
        } else
                *NextEntryId = SAHPI_NO_MORE_ENTRIES;

        data_access_unlock();
        
        return SA_OK;
}

static void dsel_get_entry(void *ptr, SaHpiSelEntryT *entry)
{
        struct oh_dsel *dsel = ptr;
        memcpy(entry, &dsel->entry, sizeof(*entry));
}

static struct oh_resource *dsel_get_res(void *ptr)
{
        struct oh_dsel *dsel = ptr;
        return get_res_by_oid(dsel->res_id);
}

static struct oh_rdr *dsel_get_rdr(void *ptr)
{
        struct oh_dsel *dsel = ptr;
        struct oh_resource *res;

        res = get_res_by_oid(dsel->res_id);
        if (!res) {
                dbg("Cannot find resource");
                return NULL;
        }

        return get_rdr_by_oid(res, dsel->rdr_id);
}

static struct list_entry_ops dsel_ops = {
        .get_entry = dsel_get_entry,
        .get_res   = dsel_get_res,
        .get_rdr   = dsel_get_rdr
};

static void rsel_get_entry(void *ptr, 
                SaHpiSelEntryT *entry)
{
        struct oh_rsel *rsel = ptr;
        struct oh_resource *res;
        res = get_res_by_oid(rsel->parent);
        if (!res) {
                dbg("Cannot find resource");
                return;
        }

        res->handler->abi->get_sel_entry(
                        res->handler->hnd,
                        rsel->oid, entry);
        entry->EntryId = rsel->entry_id;
}

static struct oh_resource *rsel_get_res(void *ptr)
{
        struct oh_rsel *rsel = ptr;
        return get_res_by_oid(rsel->res_id);
}

static struct oh_rdr *rsel_get_rdr(void *ptr)
{
        struct oh_rsel *rsel = ptr;
        struct oh_resource *res;

        res = get_res_by_oid(rsel->res_id);
        if (!res) {
                dbg("Cannot find resource");
                return NULL;
        }

        return get_rdr_by_oid(res, rsel->rdr_id);
}

static struct list_entry_ops rsel_ops = {
        .get_entry = rsel_get_entry,
        .get_res   = rsel_get_res,
        .get_rdr   = rsel_get_rdr
};

#endif
