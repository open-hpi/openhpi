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
#include <el_utils.h>
#include <string.h>

struct oh_domains domains = {        
        .table = NULL,
        .lock = NULL
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
                                SaHpiBoolT isPeer,
                                SaHpiTextBufferT *tag)
{
        struct oh_domain *domain = NULL;
        static SaHpiDomainIdT id = 1; /* domain ids will start at 1 */
        
        domain = g_new0(struct oh_domain,1);
        if (!domain) return 0;
        
        domain->info.DomainCapabilities = capabilities;
        domain->info.IsPeer = isPeer;
        if (tag)
                memcpy(&(domain->info.DomainTag),tag,sizeof(SaHpiTextBufferT));
        domain->del = oh_el_create(OH_EL_MAX_SIZE);
        domain->sessions = g_array_sized_new(FALSE, TRUE,
                                             sizeof(SaHpiSessionIdT),
                                             OH_SESSION_PREALLOC);
        domain->lock = g_mutex_new();
        if (!domain->del || !domain->sessions || !domain->lock) {
                g_free(domain->del);
                g_array_free(domain->sessions, TRUE);
                g_mutex_free(domain->lock);
                g_free(domain);
                g_mutex_unlock(domains.lock);
                return 0;
        }
        g_mutex_lock(domains.lock); /* Locked domain table */
        domain->id = id++;
        g_hash_table_insert(domains.table, &(domain->id), domain);
        g_mutex_unlock(domains.lock);  /* Unlocked domain table */

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

        if (did < 1) return SA_ERR_HPI_INVALID_PARAMS;

        g_mutex_lock(domains.lock); /* Locked domain table */
        domain = g_hash_table_lookup(domains.table, &did);
        if (!domain) {
                g_mutex_unlock(domains.lock);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        g_mutex_lock(domain->lock);
        oh_flush_rpt(&(domain->rpt));
        oh_el_close(domain->del);
        g_array_free(domain->sessions, TRUE);

        g_hash_table_remove(domains.table, &(domain->id));
        g_mutex_unlock(domains.lock); /* Unlocked domain table */

        g_mutex_free(domain->lock);
        g_free(domain);

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

        g_mutex_lock(domains.lock); /* Locked domain table */
        domain = g_hash_table_lookup(domains.table, &did);
        if (!domain) {
                g_mutex_unlock(domains.lock);
                return NULL;
        }

        g_mutex_lock(domain->lock);
        g_mutex_unlock(domains.lock); /* Unlocked domain table */

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
        GArray *domain_ids = NULL;

        domain_ids = g_array_new(FALSE, TRUE, sizeof(SaHpiDomainIdT));
        if (!domain_ids) return NULL;

        g_mutex_lock(domains.lock);
        g_hash_table_foreach(domains.table, collect_domain_ids, domain_ids);
        g_mutex_unlock(domains.lock);

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

        g_mutex_unlock(domain->lock);

        return SA_OK;
}
