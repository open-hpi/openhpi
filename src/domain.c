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
#include <oh_error.h>
#include <oh_utils.h>
#include <string.h>

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
                                SaHpiBoolT is_peer,
                                SaHpiTextBufferT *tag)
{
        struct oh_domain *domain = NULL;
        static SaHpiDomainIdT id = OH_FIRST_DOMAIN; /* domain ids will start at 1 */

        domain = g_new0(struct oh_domain,1);
        if (!domain) return 0;

        domain->capabilities = capabilities;
        domain->is_peer = is_peer;
        oh_init_rpt(&(domain->rpt));

        if (tag)
                memcpy(&(domain->tag), tag, sizeof(SaHpiTextBufferT));
        
        domain->del = oh_el_create(OH_EL_MAX_SIZE);
        domain->sessions = g_array_sized_new(FALSE, TRUE,
                                             sizeof(SaHpiSessionIdT),
                                             OH_SESSION_PREALLOC);

        g_static_rec_mutex_init(&(domain->lock));

        if (!domain->del || !domain->sessions) {
                g_free(domain->del);
                g_array_free(domain->sessions, TRUE);
                g_static_rec_mutex_free(&(domain->lock));
                g_free(domain);
                return 0;
        }
        g_static_rec_mutex_lock(&(oh_domains.lock)); /* Locked domain table */
        domain->id = id++;
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

static gboolean __dec_domain_refcount(struct oh_domain *d)
{
        return g_atomic_int_dec_and_test(&(d->refcount));
}

static void __delete_domain(struct oh_domain *d)
{
        oh_flush_rpt(&(d->rpt));
        oh_el_close(d->del);
        g_array_free(d->sessions, TRUE);
        g_static_rec_mutex_free(&(d->lock));
        g_free(d);
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

        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_static_rec_mutex_lock(&(oh_domains.lock)); /* Locked domain table */
        domain = g_hash_table_lookup(oh_domains.table, &did);
        if (!domain) {
                g_static_rec_mutex_unlock(&(oh_domains.lock));
                return SA_ERR_HPI_NOT_PRESENT;
        }

        g_hash_table_remove(oh_domains.table, &(domain->id));
        g_static_rec_mutex_unlock(&(oh_domains.lock)); /* Unlocked domain table */
        __dec_domain_refcount(domain);

        
        if (domain->refcount < 0)
                __delete_domain(domain);

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
        g_atomic_int_inc(&(domain->refcount));
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
