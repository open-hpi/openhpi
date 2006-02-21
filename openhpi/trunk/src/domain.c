/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
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
 *
 */

#include <oh_domain.h>
#include <oh_session.h>
#include <oh_alarm.h>
#include <oh_config.h>
#include <oh_error.h>
#include <oh_plugin.h>
#include <oh_event.h>
#include <oh_utils.h>
#include <string.h>
#include <sys/time.h>

#define OH_FIRST_DOMAIN 1

struct oh_domain_table oh_domains = {
        .table = NULL,
        .lock = G_STATIC_REC_MUTEX_INIT,
};


static void __inc_domain_refcount(struct oh_domain *d)
{
        g_static_rec_mutex_lock(&d->refcount_lock);
        d->refcount++;
        g_static_rec_mutex_unlock(&d->refcount_lock);

        return;
}

static void __dec_domain_refcount(struct oh_domain *d)
{
        g_static_rec_mutex_lock(&d->refcount_lock);
        d->refcount--;
        g_static_rec_mutex_unlock(&d->refcount_lock);

        return;
}

static void __free_drt_list(GSList *drt_list)
{
        GSList *node = NULL;

        for (node = drt_list; node; node = node->next) {
                g_free(node->data);
        }
        g_slist_free(drt_list);

        return;
}

static void generate_domain_event(SaHpiDomainIdT pid,
                                  SaHpiDomainIdT did,
                                  SaHpiDomainEventTypeT type)
{
        struct oh_event *event = NULL;
        struct timeval tv1;

        event = g_new0(struct oh_event, 1);
        event->type = OH_ET_HPI;
        event->did = pid;
        event->hid = 0;
        event->u.hpi_event.res.ResourceId= SAHPI_UNSPECIFIED_RESOURCE_ID;
        event->u.hpi_event.rdr.RecordId = SAHPI_ENTRY_UNSPECIFIED;
        event->u.hpi_event.event.Source = SAHPI_UNSPECIFIED_RESOURCE_ID;
        event->u.hpi_event.event.EventType = SAHPI_ET_DOMAIN;
        event->u.hpi_event.event.Severity = SAHPI_INFORMATIONAL;
        event->u.hpi_event.event.EventDataUnion.DomainEvent.Type = type;
        event->u.hpi_event.event.EventDataUnion.DomainEvent.DomainId = did;
        gettimeofday(&tv1, NULL);
        event->u.hpi_event.event.Timestamp = (SaHpiTimeT) tv1.tv_sec *
                                1000000000 + tv1.tv_usec * 1000;
        trace("domain %d %s domain %d", did,
                type == SAHPI_DOMAIN_REF_ADDED ?
                "added to" : "removed from", pid);
        g_async_queue_push(oh_process_q, event);
}

static int disconnect_parent(struct oh_domain *child)
{
        GSList *node = NULL;
        struct oh_domain *parent = NULL;
        struct timeval tv;

        parent = oh_get_domain(child->pdid);
        if (!parent) return -1;

        for (node = parent->drt.list; node; node = node->next) {
                SaHpiDrtEntryT *curdrt = node->data;
                if (curdrt->DomainId == child->id) {
                        g_free(node->data);
                        parent->drt.list =
                                g_slist_delete_link(parent->drt.list, node);
                        gettimeofday(&tv, NULL);
                        parent->drt.update_timestamp =
                                (SaHpiTimeT)tv.tv_sec * 1000000000 +
                                        tv.tv_usec * 1000;
                        parent->drt.update_count++;
                        oh_release_domain(parent);
                        generate_domain_event(child->pdid, child->id,
                                              SAHPI_DOMAIN_REF_REMOVED);
                        child->pdid = 0;
                        return 0;
                }
        }
        oh_release_domain(parent);

        dbg("BUG. Domain %d doesn't have child domain %d",
            parent->id, child->id);
        /* FIXME: Scan all domains and remove all parent links to this domain? */
        return -2;
}

static int disconnect_peers(struct oh_domain *domain)
{
        GSList *node = NULL;

        for (node = domain->drt.list; node; node = node->next) {
                struct oh_domain *peer_domain = NULL;
                SaHpiDrtEntryT *curdrt = (SaHpiDrtEntryT *)node->data;
                GSList *peer_node = NULL;

                if (!curdrt->IsPeer) {
                        continue;
                }

                peer_domain = oh_get_domain(curdrt->DomainId);
                if (!peer_domain) {
                        dbg("BUG. Can't find out peer %d for domain %d",
                            curdrt->DomainId, domain->id);
                        continue;
                }

                for (peer_node = peer_domain->drt.list;
                     peer_node;
                     peer_node = peer_node->next) {
                        curdrt = (SaHpiDrtEntryT *)peer_node->data;
                        if (curdrt->IsPeer && curdrt->DomainId == domain->id) {
                                g_free(peer_node->data);
                                peer_domain->drt.list =
                                    g_slist_delete_link(peer_domain->drt.list,
                                                        peer_node);
                                peer_domain->is_peer = FALSE;
                                generate_domain_event(peer_domain->id,
                                                      domain->id,
                                                      SAHPI_DOMAIN_REF_REMOVED);
                                break;
                        }
                }
                oh_release_domain(peer_domain);
                g_free(node->data);
                node->data = NULL;
        }

        for (node = domain->drt.list; node;) {
                if (node->data == NULL) {
                        GSList *nodenext = node->next;
                        domain->drt.list = g_slist_delete_link(domain->drt.list, node);
                        node = nodenext;
                }
                node = node->next;
        }

        domain->is_peer = FALSE;

        return 0;
}

static void __delete_domain(struct oh_domain *d)
{
        disconnect_parent(d); /* Remove drt entry from parent */
        disconnect_peers(d); /* Remove drt entry from brothers */

        oh_flush_rpt(&d->rpt);
        oh_el_close(d->del);
        oh_close_alarmtable(d);
        __free_drt_list(d->drt.list);
        g_array_free(d->sessions, TRUE);
        g_static_rec_mutex_free(&d->lock);
        g_static_rec_mutex_free(&d->refcount_lock);
        /*oh_destroy_domain_sessions(d->id);*/ /* Not needed after all */
        g_free(d);
}

static void collect_domain_ids(gpointer key, gpointer value, gpointer user_data)
{
        struct oh_domain *domain = (struct oh_domain *)value;
        GArray *data = (GArray *)user_data;

        g_array_append_val(data, domain->id);
}

static int add_drtentry(struct oh_domain *domain,
                        SaHpiDomainIdT did,
                        SaHpiBoolT is_peer)
{
        SaHpiDrtEntryT *drtentry;
        struct timeval tv1;

        drtentry = g_new0(SaHpiDrtEntryT, 1);
        if (!drtentry) {
                dbg("Could not allocate a drt entry");
                return -1;
        }

        drtentry->DomainId = did;
        drtentry->EntryId = ++(domain->drt.next_id);
        drtentry->IsPeer = is_peer;


        gettimeofday(&tv1, NULL);
        domain->drt.update_timestamp = (SaHpiTimeT) tv1.tv_sec *
                                       1000000000 + tv1.tv_usec * 1000;
        domain->drt.update_count++;
        domain->drt.list = g_slist_append(domain->drt.list, drtentry);
        return 0;
}

static int connect_parent(SaHpiDomainIdT did, SaHpiDomainIdT pdid)
{
        struct oh_domain *parent = NULL;
        struct oh_domain *child = NULL;

        if (did < 1 || pdid < 1) return 0; /* Don't return error here */

        parent = oh_get_domain(pdid);
        if (!parent) return -1;

        if (add_drtentry(parent, did, SAHPI_FALSE) != 0) {
                dbg("Could not add drtentry for domain %d to domain %d",
                    did, pdid);
                oh_release_domain(parent);
                return -2;
        }
        oh_release_domain(parent);
        generate_domain_event(pdid, did, SAHPI_DOMAIN_REF_ADDED);

        child = oh_get_domain(did);
        if (!child) return -3;

        child->pdid = parent->id;
        oh_release_domain(child);

        return 0;
}

static SaErrorT connect_peers(SaHpiDomainIdT did, SaHpiDomainIdT bdid)
{
        struct oh_domain *d = NULL, *peer = NULL;
        GSList *node = NULL, *drtlist = NULL, *node2 = NULL;

        if (did < 1 || bdid < 1) return 0; /* Don't return error here */

        /* Create a new drt that is the sum of the drts of both peers */
        d = oh_get_domain(did);
        if (d == NULL) {
                dbg("Couldn't get domain %d", did);
                return -1;
        }
        drtlist = g_slist_copy(d->drt.list);
        for (node = drtlist; node; node = node->next) {
                node->data = g_memdup(node->data, sizeof(SaHpiDrtEntryT));
        }
        oh_release_domain(d);

        peer = oh_get_domain(bdid);
        if (peer == NULL) {
                dbg("Couldn't get peer domain %d", bdid);
                return -2;
        }
        for (node = peer->drt.list; node; node = node->next) {
                drtlist = g_slist_append(drtlist,
                                         g_memdup(node->data,
                                                  sizeof(SaHpiDrtEntryT)));
        }
        oh_release_domain(peer);

        /* Now add new drt list to both peers */
        d = oh_get_domain(did);
        if (!d) {
                dbg("Could not add new drt to peer domain %d", did);
                return -3;
        }
        d->is_peer = SAHPI_TRUE;
        for (node = drtlist; node; node = node->next) { // generate events
                int found = 0;
                SaHpiDrtEntryT *drtentry = node->data;
                for (node2 = d->drt.list; node2; node2 = node2->next) {
                        SaHpiDrtEntryT *drtentry2 = node->data;
                        if (drtentry->DomainId == drtentry2->DomainId) {
                                found = 1;
                                break;
                        }
                }
                if (found) continue;
                generate_domain_event(d->id, drtentry->DomainId, SAHPI_DOMAIN_REF_ADDED);
        }
        __free_drt_list(d->drt.list);
        for (node = drtlist; node; node = node->next) { // Add new drt
                SaHpiDrtEntryT *drtentry = node->data;
                if (drtentry->DomainId == d->id) continue; // Don't add self ref
                d->drt.list = g_slist_append(d->drt.list,
                                             g_memdup(node->data,
                                                      sizeof(SaHpiDrtEntryT)));
        }
        oh_release_domain(d);

        peer = oh_get_domain(bdid);
        if (!peer) {
                dbg("Could not add new drt to peer domain %d", bdid);
                return -4;
        }
        peer->is_peer = SAHPI_TRUE;
        for (node = drtlist; node; node = node->next) { // generate events
                int found = 0;
                SaHpiDrtEntryT *drtentry = node->data;
                for (node2 = peer->drt.list; node2; node2 = node2->next) {
                        SaHpiDrtEntryT *drtentry2 = node->data;
                        if (drtentry->DomainId == drtentry2->DomainId) {
                                found = 1;
                                break;
                        }
                }
                if (found) continue;
                generate_domain_event(peer->id, drtentry->DomainId, SAHPI_DOMAIN_REF_ADDED);
        }
        __free_drt_list(peer->drt.list);
        for (node = drtlist; node; node = node->next) { // Add new drt
                SaHpiDrtEntryT *drtentry = node->data;
                if (drtentry->DomainId == peer->id) continue; // Don't add self ref
                peer->drt.list = g_slist_append(peer->drt.list,
                                                g_memdup(node->data,
                                                         sizeof(SaHpiDrtEntryT)));
        }
        oh_release_domain(peer);

        __free_drt_list(drtlist);
        return 0;
}

/**
 * oh_get_default_domain_id
 *
 * Returns:
 **/
SaHpiDomainIdT oh_get_default_domain_id()
{
        return (SaHpiDomainIdT)OH_FIRST_DOMAIN;
}

/**
 * oh_create_domain
 * @capabilities:
 * @isPeer:
 * @tag:
 *
 *
 *
 * Returns: Domain id of newly created domain, or 0 if failed to create.
 **/
SaHpiDomainIdT oh_create_domain(SaHpiDomainCapabilitiesT capabilities,
                                SaHpiTimeoutT   aitimeout,
                                SaHpiTextBufferT *tag)
{
        struct oh_domain *domain = NULL;
        static SaHpiDomainIdT id = OH_FIRST_DOMAIN; /* domain ids will start at 1 */
        struct oh_global_param param = { .type = OPENHPI_DEL_SIZE_LIMIT };
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];

        domain = g_new0(struct oh_domain,1);
        if (!domain) return 0;

        domain->capabilities = capabilities;
        domain->ai_timeout = aitimeout;
        domain->is_peer = SAHPI_FALSE;
        oh_init_rpt(&(domain->rpt));

        if (tag)
                memcpy(&(domain->tag), tag, sizeof(SaHpiTextBufferT));

        oh_get_global_param(&param);

        domain->del = oh_el_create(param.u.del_size_limit);
        domain->sessions = g_array_sized_new(FALSE, TRUE,
                                             sizeof(SaHpiSessionIdT),
                                             OH_SESSION_PREALLOC);

        g_static_rec_mutex_init(&(domain->lock));
        g_static_rec_mutex_init(&(domain->refcount_lock));

        if (!domain->del || !domain->sessions) {
                g_free(domain->del);
                g_array_free(domain->sessions, TRUE);
                g_static_rec_mutex_free(&(domain->lock));
                g_static_rec_mutex_free(&(domain->refcount_lock));
                g_free(domain);
                return 0;
        }

        param.type = OPENHPI_DEL_SAVE;
        oh_get_global_param(&param);

        g_static_rec_mutex_lock(&(oh_domains.lock)); /* Locked domain table */
        domain->id = id++;
        if (param.u.del_save) {
                param.type = OPENHPI_VARPATH;
                oh_get_global_param(&param);
                snprintf(del_filepath,
                         SAHPI_MAX_TEXT_BUFFER_LENGTH*2,
                         "%s/del.%u", param.u.varpath, domain->id);
                oh_el_map_from_file(domain->del, del_filepath);
        }
        g_hash_table_insert(oh_domains.table, &(domain->id), domain);
        g_static_rec_mutex_unlock(&(oh_domains.lock));  /* Unlocked domain table */

        return domain->id;
}

/**
 * oh_destroy_domain
 * @did:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_destroy_domain(SaHpiDomainIdT did)
{
        struct oh_domain *domain = NULL;
        GSList *node = NULL;

        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;
        if (did == oh_get_default_domain_id())
                return SA_ERR_HPI_INVALID_PARAMS;

        domain = oh_get_domain(did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        /* If the domain is a parent of other domains, then cancel the destroy. */
        for (node = domain->drt.list; node; node = node->next) {
                SaHpiDrtEntryT *curdrt = node->data;
                if (!curdrt->IsPeer) {
                        oh_release_domain(domain);
                        return SA_ERR_HPI_BUSY;
                }
        }

        g_static_rec_mutex_lock(&oh_domains.lock);
        g_hash_table_remove(oh_domains.table, &(domain->id));
        g_static_rec_mutex_unlock(&oh_domains.lock);

        __dec_domain_refcount(domain);
        if (domain->refcount < 1)
                __delete_domain(domain);
        else
                oh_release_domain(domain);

        return SA_OK;
}

/**
 * oh_get_domain
 * @did:
 *
 *
 *
 * Returns:
 **/
struct oh_domain *oh_get_domain(SaHpiDomainIdT did)
{
        struct oh_domain *domain = NULL;

        if (did < 1) return NULL;

        if (did == SAHPI_UNSPECIFIED_DOMAIN_ID) {
                did = oh_get_default_domain_id();
        }

        g_static_rec_mutex_lock(&(oh_domains.lock)); /* Locked domain table */
        domain = g_hash_table_lookup(oh_domains.table, &did);
        if (!domain) {
                g_static_rec_mutex_unlock(&(oh_domains.lock));
                return NULL;
        }

        /* Punch in */
        __inc_domain_refcount(domain);
        /* Unlock domain table */
        g_static_rec_mutex_unlock(&(oh_domains.lock));
        /* Wait to get domain lock */
        g_static_rec_mutex_lock(&(domain->lock));

        return domain;
}

/**
 * oh_release_domain
 * @domain:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_release_domain(struct oh_domain *domain)
{
        if (!domain) return SA_ERR_HPI_INVALID_PARAMS;

        __dec_domain_refcount(domain); /* Punch out */
        /*
         * If domain was scheduled for destruction before, and
         * no other threads are referring to it, then delete domain.
         */
        if (domain->refcount < 0)
                __delete_domain(domain);
        else
                g_static_rec_mutex_unlock(&domain->lock);

        return SA_OK;
}

/**
 * oh_list_domains
 *
 *
 *
 * Returns:
 **/
GArray *oh_list_domains()
{
        trace("Entering list_domains");
        GArray *domain_ids = NULL;

        domain_ids = g_array_new(FALSE, TRUE, sizeof(SaHpiDomainIdT));
        if (!domain_ids) return NULL;
        trace("setup domain ids");
        g_static_rec_mutex_lock(&(oh_domains.lock));
        g_hash_table_foreach(oh_domains.table, collect_domain_ids, domain_ids);
        trace("Looping through table");
        g_static_rec_mutex_unlock(&(oh_domains.lock));

        return domain_ids;
}


SaHpiDomainIdT oh_request_new_domain(unsigned int hid,
                                     SaHpiTextBufferT *tag,
                                     SaHpiDomainCapabilitiesT capabilities,
                                     SaHpiDomainIdT pdid,
                                     SaHpiDomainIdT bdid)
{
         return oh_request_new_domain_aitimeout(hid, tag, capabilities,
                           SAHPI_TIMEOUT_IMMEDIATE, pdid, bdid);
}

/**
 * oh_request_new_domain
 * @hid: a handler id that is requesting the domain
 * @tag: a tag to put in the domain being requested
 * @capabilities: capabilities of the requested domain
 * @pdid: make new domain with this parent? if so, has to be > 0.
 * @bdid: make new domain with this peer/brother? if so, has to be > 0.
 *
 * Creates a new domain and adds it to the list of domains
 * that the handler (@handler_id) can report events/resources on.
 *
 * Returns: domain id of new domain requested, or 0 if an error ocurred.
 **/
SaHpiDomainIdT oh_request_new_domain_aitimeout(
                                    unsigned int hid,
                                    SaHpiTextBufferT *tag,
                                    SaHpiDomainCapabilitiesT capabilities,
                                    SaHpiTimeoutT aitimeout,
                                    SaHpiDomainIdT pdid,
                                    SaHpiDomainIdT bdid)

{
        SaHpiDomainIdT did = 0;

        if (hid < 1) {
                dbg("Warning - invalid handler id parameter passed.");
                return 0;
        }

        if (pdid == 0) {
                pdid = oh_get_default_domain_id();
        }

        did = oh_create_domain(capabilities, aitimeout, tag);
        if (did == 0) {
                dbg("New domain request failed.");
                return 0;
        }
        oh_add_domain_to_handler(hid, did);

        /* Connect new domain as a child of pdid domain */
        if (connect_parent(did, pdid)) {
                oh_destroy_domain(did);
                dbg("Operation failed."
                    " Could not connect new domain to parent %d.", pdid);
                return 0;
        }

        /* Connect new domain as a brother of bdid domain */
        if (connect_peers(did, bdid)) {
                oh_destroy_domain(did);
                dbg("Operation failed."
                    " Could not make new domain peer of domain %d.", bdid);
                return 0;
        }

        return did;
}

/**
 * oh_request_domain_delete
 * @handler_id: a handler id
 * @did: a domain id
 *
 * This is an extended version of oh_destroy_domain which
 * deletes the domain from the list of domains that @handler_id
 * has registered in.
 *
 * Returns: SA_OK on success, otherwise and error.
 **/
SaErrorT oh_request_domain_delete(unsigned int hid,
                                  SaHpiDomainIdT did)
{
        SaErrorT error;

        if (did == oh_get_default_domain_id() || hid < 1 || did < 1) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (oh_remove_domain_from_handler(hid, did))
                return SA_ERR_HPI_INTERNAL_ERROR;

        error = oh_destroy_domain(did);
        if (error != SA_OK) return error;

        return SA_OK;
}

/**
 * oh_drt_entry_get
 * @did: a domain id
 * @entryid: id of drt entry
 * @nextentryid: id next to @entryid in the drt will be put here.
 * @drtentry: drt entry corresponding to @entryid will be placed here.
 *
 * Fetches a drt entry from the domain identified by @did
 *
 * Returns: SA_OK on success, otherwise an error.
 **/
SaErrorT oh_drt_entry_get(SaHpiDomainIdT     did,
                          SaHpiEntryIdT      entryid,
                          SaHpiEntryIdT      *nextentryid,
                          SaHpiDrtEntryT     *drtentry)
{
        struct oh_domain *domain = NULL;
        GSList *node = NULL;

        if (did < 0 || !nextentryid || !drtentry) {
                dbg("Error - Invalid parameters passed.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        domain = oh_get_domain(did);
        if (domain == NULL) {
                dbg("no domain for id %d", did);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        for (node = domain->drt.list; node; node = node->next) {
                SaHpiDrtEntryT *curdrt = (SaHpiDrtEntryT *)node->data;
                if (curdrt->EntryId == entryid || entryid == SAHPI_FIRST_ENTRY) {
                        if (node->next == NULL) { /* last entry */
                                *nextentryid = SAHPI_LAST_ENTRY;
                        } else {
                                SaHpiDrtEntryT *nextdrt =
                                        (SaHpiDrtEntryT *)node->next->data;
                                *nextentryid = nextdrt->EntryId;
                        }
                        memcpy(drtentry, curdrt, sizeof(SaHpiDrtEntryT));
                        oh_release_domain(domain);
                        return SA_OK;
                }
        }
        oh_release_domain(domain);

        return SA_ERR_HPI_NOT_PRESENT;
}
