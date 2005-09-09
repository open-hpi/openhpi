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

static void __delete_domain(struct oh_domain *d)
{
        GSList *node = NULL;

        oh_flush_rpt(&(d->rpt));
        oh_el_close(d->del);
        oh_close_alarmtable(d);
        for (node = d->drt.list; node; node = node->next) {
                g_free(node->data);
        }
        g_slist_free(d->drt.list);
        g_array_free(d->sessions, TRUE);
        g_static_rec_mutex_free(&(d->lock));
        g_static_rec_mutex_free(&(d->refcount_lock));
        oh_destroy_domain_sessions(d->id);
        g_free(d);
}

static void collect_domain_ids(gpointer key, gpointer value, gpointer user_data)
{
        struct oh_domain *domain = (struct oh_domain *)value;
        GArray *data = (GArray *)user_data;

        g_array_append_val(data, domain->id);
}

static SaErrorT disconnect_domains(struct oh_domain *parent,
                                   struct oh_domain *child)
{
        GSList *node = NULL;
        SaHpiDrtEntryT *curdrt = NULL;
        struct timeval tv1;

        for (node = parent->drt.list; node; node = node->next) {
                curdrt = node->data;
                if (curdrt->DomainId == child->id) {
                        g_free(node->data);
                        parent->drt.list =
                                g_slist_delete_link(parent->drt.list, node);
                        child->pdid = 0;
                        gettimeofday(&tv1, NULL);
                        parent->drt.update_timestamp =
                                (SaHpiTimeT)tv1.tv_sec * 1000000000 + tv1.tv_usec * 1000;
                        parent->drt.update_count++;
                        return SA_OK;
                }
        }

        dbg("BUG. Domain %d doesn't have child domain %d",
            parent->id, child->id);
        /* FIXME: Scan all domains and remove all parent links to this domain? */
        return SA_ERR_HPI_INTERNAL_ERROR;
}

static int add_drt(struct oh_domain *domain,
                   SaHpiDomainIdT did,
                   SaHpiBoolT is_peer)
{
        SaHpiDrtEntryT *drt_entry;
        struct timeval tv1;

        drt_entry = g_new0(SaHpiDrtEntryT, 1);
        if (!drt_entry) {
                dbg("Could not allocate a drt entry");
                return -1;
        }

        drt_entry->DomainId = did;
        drt_entry->EntryId = ++(domain->drt.next_id);
        drt_entry->IsPeer = is_peer;


        gettimeofday(&tv1, NULL);
        domain->drt.update_timestamp = (SaHpiTimeT) tv1.tv_sec *
                                       1000000000 + tv1.tv_usec * 1000;
        domain->drt.update_count++;
        domain->drt.list = g_slist_append(domain->drt.list, drt_entry);
        return 0;
}

static int connect_domains(struct oh_domain *parent, SaHpiDomainIdT did)
{
        struct oh_domain *child = NULL;

        child = g_hash_table_lookup(oh_domains.table, &did);
        if (child == NULL) {
                dbg("Couldn't get child domain %d", did);
                return -1;
        }

        if (add_drt(parent, did, SAHPI_FALSE) != 0) {
                dbg("Could not add drt for domain %d to domain %d",
                    did, parent->id);
                return -1;
        }
        child->pdid = parent->id;

        return 0;
}

static GArray *disconnect_from_peers(struct oh_domain *domain)
{
        GSList *node = NULL, *next_node = NULL;
        GArray *peer_ids = NULL;

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
                                if (peer_ids == NULL) {
                                        peer_ids =
                                                g_array_new(TRUE, FALSE,
                                                            sizeof (SaHpiDomainIdT));
                                }
                                g_array_append_val(peer_ids, peer_domain->id);
                                peer_domain->is_peer = FALSE;
                                break;
                        }
                }
                oh_release_domain(peer_domain);
        }

        for (node = domain->drt.list; node;) {
                SaHpiDrtEntryT *curdrt = (SaHpiDrtEntryT *)node->data;
                if (!curdrt->IsPeer) {
                        node = node->next;
                        continue;
                }
                next_node = node->next;
                g_free(node->data);
                domain->drt.list =
                        g_slist_delete_link(domain->drt.list, node);
                node = next_node;
        }
        domain->is_peer = FALSE;

        return peer_ids;
}

static SaErrorT make_peer_domains(struct oh_domain *peer,
                                  SaHpiDomainIdT did)
{
        struct oh_domain *new_peer = NULL;
        GSList *node = NULL;

        /* FIXME: This needs locking! */
        new_peer = g_hash_table_lookup(oh_domains.table, &did);
        if (new_peer == NULL) {
                dbg("Couldn't get new peer domain %d", did);
                return -1;
        }

        for (node = peer->drt.list; node; node = node->next) {
                SaHpiDrtEntryT *curdrt = (SaHpiDrtEntryT *)node->data;
                if (curdrt->IsPeer) {
                        struct oh_domain *d =
                                g_hash_table_lookup(oh_domains.table,
                                                    &curdrt->DomainId);
                        if (d == NULL) {
                                dbg("BUG. Can't find out peer %d for domain %d",
                                    curdrt->DomainId, peer->id);
                                goto restore_drts_on_failure;
                        }

                        if (add_drt(new_peer, d->id, SAHPI_TRUE) != 0) {
                                goto restore_drts_on_failure;
                        }

                        if (add_drt(d, new_peer->id, SAHPI_TRUE) != 0) {
                                goto restore_drts_on_failure;
                        }
                }
        }

        if (add_drt(peer, new_peer->id, SAHPI_TRUE) != 0) {
                goto restore_drts_on_failure;
        }

        if (add_drt(new_peer, peer->id, SAHPI_TRUE) != 0) {
                goto restore_drts_on_failure;
        }
        new_peer->is_peer = SAHPI_TRUE;
        peer->is_peer = SAHPI_TRUE;

        return 0;
restore_drts_on_failure:
        g_array_free(disconnect_from_peers(new_peer), TRUE);
        return -1;
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
                                SaHpiTextBufferT *tag)
{
        struct oh_domain *domain = NULL;
        static SaHpiDomainIdT id = OH_FIRST_DOMAIN; /* domain ids will start at 1 */
        struct oh_global_param param = { .type = OPENHPI_DEL_SIZE_LIMIT };
        char del_filepath[SAHPI_MAX_TEXT_BUFFER_LENGTH*2];

        domain = g_new0(struct oh_domain,1);
        if (!domain) return 0;

        domain->capabilities = capabilities;
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
        struct oh_domain *domain = NULL, *parent = NULL;
        GSList *node = NULL;
        SaHpiDrtEntryT *curdrt;
        GArray *peers;
        SaHpiDomainIdT pid = 0;

        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;
        if (did == oh_get_default_domain_id())
                return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&oh_domains.lock);
        domain = oh_get_domain(did);
        if (!domain) {
                g_static_rec_mutex_unlock(&oh_domains.lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        for (node = domain->drt.list; node; node = node->next) {
                curdrt = node->data;
                if (!curdrt->IsPeer) {
                        oh_release_domain(domain);
                        g_static_rec_mutex_unlock(&oh_domains.lock);
                        return SA_ERR_HPI_BUSY;
                }
        }

        if (domain->pdid != 0) {
                parent = oh_get_domain(domain->pdid);
                if (!parent) {
                        oh_release_domain(domain);
                        g_static_rec_mutex_unlock(&oh_domains.lock);
                        return SA_ERR_HPI_NOT_PRESENT;
                }
                
                pid = parent->id;
                if (disconnect_domains(parent, domain) != 0) {
                        oh_release_domain(parent);
                        oh_release_domain(domain);
                        g_static_rec_mutex_unlock(&oh_domains.lock);
                        return SA_ERR_HPI_INTERNAL_ERROR;
                }
                oh_release_domain(parent);
                domain->pdid = 0;
        }

        if (domain->is_peer) {
                peers = disconnect_from_peers(domain);
                g_array_free(peers, TRUE);
        }

        g_hash_table_remove(oh_domains.table, &(domain->id));
        g_static_rec_mutex_unlock(&oh_domains.lock);

        generate_domain_event(pid, did, SAHPI_DOMAIN_REF_REMOVED);
        /* FIXME. send domain event to peers */

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

        __dec_domain_refcount(domain);
        if (domain->refcount < 0)
                __delete_domain(domain);
        else
                g_static_rec_mutex_unlock(&(domain->lock));

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

/**
 * oh_request_new_domain
 * @hid: a handler id that is requesting the domain
 * @tag: a tag to put in the domain being requested
 * @capabilities: capabilities of the requested domain
 * @parent_id: make new domain with this parent? if so, has to be > 0.
 * @peer_id: make new domain with this peer? if so, has to be > 0.
 *
 * Creates a new domain and adds it to the list of domains
 * that the handler (@handler_id) can report events/resources on.
 *
 * Returns: domain id of new domain requested, or 0 if an error ocurred.
 **/
SaHpiDomainIdT oh_request_new_domain(unsigned int handler_id,
                                     SaHpiTextBufferT *tag,
                                     SaHpiDomainCapabilitiesT capabilities,
                                     SaHpiDomainIdT parent_id,
                                     SaHpiDomainIdT peer_id)
{
        struct oh_domain *parent = NULL;
        struct oh_domain *peer = NULL;
        SaHpiDomainIdT did;

        if (handler_id < 1) {
                dbg("Warning - invalid handler id parameter passed.");
                return 0;
        }

        if (parent_id == 0) {
                parent_id = oh_get_default_domain_id();
        }

        g_static_rec_mutex_lock(&oh_domains.lock);
        parent = oh_get_domain(parent_id);
        if (parent == NULL) {
                g_static_rec_mutex_unlock(&oh_domains.lock);
                dbg("Couldn't lookup parent domain %d", parent_id);
                return 0;
        }

        if (peer_id != 0) {
                peer = oh_get_domain(peer_id);
                if (peer == NULL) {
                        oh_release_domain(parent);
                        g_static_rec_mutex_unlock(&oh_domains.lock);
                        dbg("Couldn't lookup parent domain %d", peer_id);
                        return 0;
                }
        }

        did = oh_create_domain(capabilities, tag);
        if (did == 0) {
                if (peer) oh_release_domain(peer);
                oh_release_domain(parent);
                g_static_rec_mutex_unlock(&oh_domains.lock);
                dbg("Couldn't create child domain");
                return 0;
        }

        if (connect_domains(parent, did) != SA_OK) {
                oh_destroy_domain(did);
                if (peer) oh_release_domain(peer);
                oh_release_domain(parent);
                g_static_rec_mutex_unlock(&oh_domains.lock);
                dbg("Couldn't make child domain");
                return 0;
        }
        oh_release_domain(parent);


        if (peer && (make_peer_domains(peer, did) != SA_OK)) {
                oh_destroy_domain(did);
                oh_release_domain(peer);
                g_static_rec_mutex_unlock(&oh_domains.lock);
                dbg("Couldn't make peer domains %d & %d", peer_id, did);
                return 0;
        }

        if (peer) oh_release_domain(peer);
        g_static_rec_mutex_unlock(&oh_domains.lock);

        oh_add_domain_to_handler(handler_id, did);

        generate_domain_event(parent_id, did, SAHPI_DOMAIN_REF_ADDED);
        /* FIXME: send domain event to parent and peers */

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
SaErrorT oh_request_domain_delete(unsigned int handler_id,
                                  SaHpiDomainIdT     did)
{
        SaErrorT error;

        if (did == oh_get_default_domain_id() || handler_id < 1 || did < 1) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (!oh_domain_served_by_handler(handler_id, did)) {
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        error = oh_destroy_domain(did);
        if (error != SA_OK) return error;

        oh_remove_domain_from_handler(handler_id, did);

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
