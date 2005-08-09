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
 * oh_get_default_domain_id
 *
 * Returns:
 **/
SaHpiDomainIdT oh_get_default_domain_id()
{
        return (SaHpiDomainIdT)OH_FIRST_DOMAIN;
}

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
        oh_flush_rpt(&(d->rpt));
        oh_el_close(d->del);
        oh_close_alarmtable(d);
        g_array_free(d->sessions, TRUE);
        g_static_rec_mutex_free(&(d->lock));
        g_static_rec_mutex_free(&(d->refcount_lock));
        g_free(d);
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

static void collect_domain_ids(gpointer key, gpointer value, gpointer user_data)
{
        struct oh_domain *domain = (struct oh_domain *)value;
        GArray *data = (GArray *)user_data;

        g_array_append_val(data, domain->id);
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
        dbg("Entering list_domains");
        GArray *domain_ids = NULL;

        domain_ids = g_array_new(FALSE, TRUE, sizeof(SaHpiDomainIdT));
        if (!domain_ids) return NULL;
        dbg("setup domain ids");
        g_static_rec_mutex_lock(&(oh_domains.lock));
        g_hash_table_foreach(oh_domains.table, collect_domain_ids, domain_ids);
        dbg("Looping through table");
        g_static_rec_mutex_unlock(&(oh_domains.lock));

        return domain_ids;
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
 * disconnect_domains
 * 
 *
 *
 *
 * Returns:
 **/
static int disconnect_domains(struct oh_domain *parent,
                              struct oh_domain *child)
{
	GSList *node = NULL;
	SaHpiDrtEntryT *curdrt;
	int removed = 0;	
	struct timeval tv1;
	
	for (node = parent->drt.list; node; node = node->next) {
		curdrt = node->data;
		if (curdrt->DomainId == child->id) {
			parent->drt.list = g_slist_remove(parent->drt.list, curdrt);
			child->p_id = 0;
        		gettimeofday(&tv1, NULL);
        		parent->drt.update_timestamp = (SaHpiTimeT) tv1.tv_sec *
					1000000000 + tv1.tv_usec * 1000;
			parent->drt.update_count++;
			removed = 1;
			break;
        	}
	}
	if (removed) {
		return SA_OK;
	}
	
	dbg("BUG. parent %d doesn't refer to child %d",
			parent->id, child->id);
	// Scan all domains and remove all parent links to this domain?
	return SA_ERR_HPI_INTERNAL_ERROR;
}

/**
 * add_drt
 * 
 *
 *
 *
 * Returns:
 **/
static int add_drt(struct oh_domain *domain,
                   SaHpiDomainIdT id,
		   SaHpiBoolT is_peer)
{
	SaHpiDrtEntryT *drt;
	struct timeval tv1;
	
	drt = g_new0(SaHpiDrtEntryT, 1);
        if (!drt) {
		dbg("could not alloc drt");
		return -1;
	}

	drt->DomainId = id;
	drt->EntryId = ++(domain->drt.next_id);
	drt->IsPeer = is_peer;


        gettimeofday(&tv1, NULL);
        domain->drt.update_timestamp = (SaHpiTimeT) tv1.tv_sec *
					1000000000 + tv1.tv_usec * 1000;
	domain->drt.update_count++;
	domain->drt.list = g_slist_append(domain->drt.list, drt);
	return 0;
}	




/**
 * connect_domains
 * 
 *
 *
 *
 * Returns:
 **/
static int connect_domains(struct oh_domain *parent,
                              SaHpiDomainIdT id)
{
	struct oh_domain *child;
	
	child = g_hash_table_lookup(oh_domains.table, &id);
	
	if (child == NULL) {
		dbg("couldn't get child domain. id = %d", id);
		return -1;
	}

        if (add_drt(parent, id, SAHPI_FALSE) != 0) {
		dbg("could not add drt %d to domain %d",
			id, parent->id);
		return -1;
	}
	child->p_id = parent->id;

	return 0;
}

static GArray *disconnect_from_peers(struct oh_domain *my_domain)
{
	struct oh_domain *domain;
	GSList *node = NULL;
	GSList *nd = NULL;
	SaHpiDrtEntryT *curdrt;
	SaHpiBoolT is_peer = SAHPI_FALSE;
	GArray *peer_ids = NULL;
	

	for (node = my_domain->drt.list; node; node = node->next) {
		curdrt = node->data;
		if (!curdrt->IsPeer) {
			continue;
		}
		domain = g_hash_table_lookup(oh_domains.table,
				&curdrt->DomainId);
		if (domain == NULL) {
			dbg("BUG. can't find out peer %d for domain %d",
					curdrt->DomainId, my_domain->id);
			continue;
		}
		is_peer = SAHPI_FALSE;
		for (nd = domain->drt.list; nd; nd = nd->next) {
			curdrt = nd->data;
			if (curdrt->IsPeer) {
				if (curdrt->DomainId != my_domain->id) {
					is_peer = SAHPI_TRUE;
					continue;
				}
				domain->drt.list =
				    g_slist_remove(domain->drt.list, curdrt);
				if (peer_ids == NULL) {
					peer_ids = g_array_new(TRUE, FALSE,
						sizeof (SaHpiDomainIdT));
				}
				g_array_append_val(peer_ids, domain->id);
				if (is_peer) {
					break;
				}
			}
		}
		domain->is_peer = is_peer;
	}
	for (node = my_domain->drt.list; node; node = node->next) {
		curdrt = node->data;
		if (!curdrt->IsPeer) {
			continue;
		}
		my_domain->drt.list =
				g_slist_remove(my_domain->drt.list, curdrt);
	}
	my_domain->is_peer = SAHPI_FALSE;
	return peer_ids;
}


static SaErrorT make_peer_domains(struct oh_domain *peer,
                                 SaHpiDomainIdT id)
{
	struct oh_domain *domain;
	struct oh_domain *my_domain;
	GSList *node = NULL;
	SaHpiDrtEntryT *curdrt;

	my_domain = g_hash_table_lookup(oh_domains.table, &id);
	
	if (my_domain == NULL) {
		dbg("couldn't get child domain. id = %d", id);
		return -1;
	}
	
	for (node = peer->drt.list; node; node = node->next) {
		curdrt = node->data;
		if (curdrt->IsPeer) {
			domain = g_hash_table_lookup(oh_domains.table,
				&curdrt->DomainId);
			if (domain == NULL) {
				dbg("BUG. can't find out peer %d for domain %d",
					curdrt->DomainId, peer->id);
				goto restore_drts_on_failure;
			}
			if (add_drt(my_domain, domain->id, SAHPI_TRUE) != 0) {
				goto restore_drts_on_failure;
			}
			if (add_drt(domain, id, SAHPI_TRUE) != 0) {
				goto restore_drts_on_failure;
			}
        	}
	}

	if (add_drt(peer, id, SAHPI_TRUE) != 0) {
		goto restore_drts_on_failure;
	}
	if (add_drt(my_domain, peer->id, SAHPI_TRUE) != 0) {
		goto restore_drts_on_failure;
	}
	my_domain->is_peer = SAHPI_TRUE;
	peer->is_peer = SAHPI_TRUE;

	return 0;
restore_drts_on_failure:
	g_array_free(disconnect_from_peers(my_domain), TRUE);
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
 * oh_request_domain_id
 * 
 *
 *
 *
 * Returns:
 **/
SaHpiDomainIdT oh_request_domain_id(unsigned int handler_id,
                                    SaHpiTextBufferT *tag,
				    SaHpiDomainCapabilitiesT capabilities,
				    SaHpiDomainIdT parent_id,
				    SaHpiDomainIdT peer_id)
{
	struct oh_handler *handler;
	struct oh_domain *parent;
	struct oh_domain *peer = NULL;
	SaHpiDomainIdT my_id;

	
	if (parent_id == 0) {
		parent_id = oh_get_default_domain_id();
	}
	handler = oh_lookup_handler(handler_id);
	if (handler == NULL) {
		dbg("couldn't lookup handler id %d", handler_id);
		return 0;
	}
	
	g_static_rec_mutex_lock(&oh_domains.lock);
	parent = g_hash_table_lookup(oh_domains.table, &parent_id);
	if (parent == NULL) {
		dbg("couldn't lookup parent domain. domain id = %d", parent_id);
		g_static_rec_mutex_unlock(&oh_domains.lock);
		return 0;
	}
	if (peer_id != 0) {
		peer = oh_get_domain(peer_id);
		if (peer == NULL) {
			dbg("couldn't lookup parent domain. domain id = %d",
					 peer_id);
			g_static_rec_mutex_unlock(&oh_domains.lock);
			return 0;
		}
	}

	my_id = oh_create_domain(capabilities, tag);
	if (my_id == 0) {
		dbg("couldn't create child domain");
		g_static_rec_mutex_unlock(&oh_domains.lock);
		return 0;
	}
	if (connect_domains(parent, my_id) != SA_OK) {
		dbg("couldn't create child domain");
		oh_destroy_domain(my_id);
		g_static_rec_mutex_unlock(&oh_domains.lock);
		return 0;
	}
	

	if ((peer != NULL) && (make_peer_domains(peer, my_id) != SA_OK)) {
		dbg("couldn't make peer domains %d & %d", peer_id, my_id);
		oh_destroy_domain(my_id);
		g_static_rec_mutex_unlock(&oh_domains.lock);
		return 0;
	}

	g_static_rec_mutex_unlock(&oh_domains.lock);
	
	oh_add_domain_to_handler(handler_id, my_id);
	
	generate_domain_event(parent_id, my_id, SAHPI_DOMAIN_REF_ADDED);
	// FIXME. send domain event to parent and peers
	
	return my_id;
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
        struct oh_domain *parent = NULL;
	GSList *node = NULL;
	SaHpiDrtEntryT *curdrt;
	GArray *peers;
	SaHpiDomainIdT pid = 0;

        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;
        if (did == oh_get_default_domain_id()) return SA_ERR_HPI_INVALID_PARAMS;
        g_static_rec_mutex_lock(&(oh_domains.lock)); /* Locked domain table */
        domain = g_hash_table_lookup(oh_domains.table, &did);
        if (!domain) {
                g_static_rec_mutex_unlock(&(oh_domains.lock));
                return SA_ERR_HPI_NOT_PRESENT;
        }
	for (node = domain->drt.list; node; node = node->next) {
		curdrt = node->data;
		if (!curdrt->IsPeer) {
                	g_static_rec_mutex_unlock(&(oh_domains.lock));
                	return SA_ERR_HPI_BUSY;
        	}
	}
	if (domain->p_id != 0) {
		parent = g_hash_table_lookup(oh_domains.table, &domain->p_id);
		pid = parent->id;
		if (disconnect_domains(parent, domain) != 0) {
                	g_static_rec_mutex_unlock(&(oh_domains.lock));
			return SA_ERR_HPI_INTERNAL_ERROR;
		}
		domain->p_id = 0;
	}

	if (domain->is_peer) {
		peers = disconnect_from_peers(domain);
	}
        g_hash_table_remove(oh_domains.table, &(domain->id));
        g_static_rec_mutex_unlock(&(oh_domains.lock)); /* Unlocked domain table */


	generate_domain_event(pid, did, SAHPI_DOMAIN_REF_REMOVED);
	// FIXME. send domain event to peers
	
        return SA_OK;
}

SaErrorT oh_release_domain_id(unsigned int handler_id,
			      SaHpiDomainIdT     did)
{
	SaErrorT rv;

        if (did == oh_get_default_domain_id()) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	if (!oh_domain_served_by_handler(handler_id, did)) {
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	rv = oh_destroy_domain(did);
	if (rv != SA_OK) {
		return rv;
	}
	oh_remove_domain_from_handler(handler_id, did);
	return SA_OK;
}


SaErrorT oh_drt_entry_get(SaHpiDomainIdT     did,
			  SaHpiEntryIdT      entryid,
			  SaHpiEntryIdT      *nextentryid,
			  SaHpiDrtEntryT     *drt)
{
	struct oh_domain *domain;	
	GSList *node = NULL;
	SaHpiDrtEntryT *curdrt;
	SaHpiDrtEntryT *nextdrt;


	g_static_rec_mutex_lock(&(oh_domains.lock));
        domain = g_hash_table_lookup(oh_domains.table, &did);
	if (domain == NULL) {
		dbg("no domain for id %d", did);
		g_static_rec_mutex_unlock(&(oh_domains.lock));
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	for (node = domain->drt.list; node; node = node->next) {
		curdrt = node->data;
		if ((curdrt->EntryId == entryid) ||
				(entryid == SAHPI_FIRST_ENTRY)) {
			if (node->next == NULL) { // last entry
				*nextentryid = SAHPI_LAST_ENTRY;
			} else {
				nextdrt = node->next->data;
				*nextentryid = nextdrt->EntryId;
			}
			memcpy(drt, curdrt, sizeof(SaHpiDrtEntryT));
			g_static_rec_mutex_unlock(&(oh_domains.lock));
			return SA_OK;
		}
	}
	g_static_rec_mutex_unlock(&(oh_domains.lock));

	return SA_ERR_HPI_NOT_PRESENT;
}
