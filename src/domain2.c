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
#include <sel_utils.h>
#include <string.h>

GHashTable *domain_table = NULL;

/**
 * oh_create_domain
 * @capabilities:
 * @isPeer:
 * @tag:
 * @did:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_create_domain(SaHpiDomainCapabilitiesT capabilities,
                          SaHpiBoolT isPeer,
                          SaHpiTextBufferT *tag,
                          SaHpiDomainIdT *did)
{
        static guint domain_id = 0;
        struct oh_domain *domain = NULL;

        if (!did || !tag) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_new0(struct oh_domain,1);
        if (!domain) return SA_ERR_HPI_OUT_OF_MEMORY;

        /* Need to lock these lines -- RM */
        domain->domain_id = ++domain_id;
        domain->domain_info.DomainCapabilities = capabilities;
        domain->domain_info.IsPeer = isPeer;
        memcpy(&(domain->domain_info.DomainTag),tag,sizeof(SaHpiTextBufferT));
        domain->sel = oh_sel_create(0);
        /* Will I need to create DRT and DAT? -- RM */
        g_hash_table_insert(domain_table, &(domain->domain_id), domain);

        *did = domain->domain_id;

        return SA_OK;
}

/**
 * oh_get_domain_rpt
 * @did:
 * @rptable:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_domain_rpt(SaHpiDomainIdT did, RPTable **rptable)
{
        struct oh_domain *domain = NULL;

        if (did < 1 || !rptable) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_hash_table_lookup(domain_table, &did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        *rptable = &(domain->rptable);

        return SA_OK;
}

/**
 * oh_get_domain_dat
 * @did:
 * @datable:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_domain_dat(SaHpiDomainIdT did, void **datable)
{
        struct oh_domain *domain = NULL;

        if (did < 1 || !datable) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_hash_table_lookup(domain_table, &did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        *datable = domain->alarm_table;

        return SA_OK;
}

/**
 * oh_get_domain_drt
 * @did:
 * @drtable:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_domain_drt(SaHpiDomainIdT did, void **drtable)
{
        struct oh_domain *domain = NULL;

        if (did < 1 || !drtable) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_hash_table_lookup(domain_table, &did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        *drtable = domain->reference_table;

        return SA_OK;
}

/**
 * oh_get_domain_del
 * @did:
 * @delog:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_domain_del(SaHpiDomainIdT did, oh_sel **delog)
{
        struct oh_domain *domain = NULL;

        if (did < 1 || !delog) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_hash_table_lookup(domain_table, &did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        *delog = domain->sel;

        return SA_OK;
}

/**
 * oh_get_domain_info
 * @did:
 * @info:
 *
 *
 *
 * Returns:
 **/
SaErrorT oh_get_domain_info(SaHpiDomainIdT did, SaHpiDomainInfoT **info)
{
        struct oh_domain *domain = NULL;

        if (did < 1 || !info) return SA_ERR_HPI_INVALID_PARAMS;

        domain = g_hash_table_lookup(domain_table, &did);
        if (!domain) return SA_ERR_HPI_NOT_PRESENT;

        *info = &(domain->domain_info);

        return SA_OK;
}
