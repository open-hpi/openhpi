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
                          SaHpiTextBufferT tag,
                          SaHpiDomainIdT *did)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
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
SaErrorT oh_get_domain_rpt(SaHpiDomainIdT did, RPTable *rptable)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
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
SaErrorT oh_get_domain_dat(SaHpiDomainIdT did, void *datable)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
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
SaErrorT oh_get_domain_drt(SaHpiDomainIdT did, void *drtable)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
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
SaErrorT oh_get_domain_del(SaHpiDomainIdT did, oh_sel *delog)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
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
SaErrorT oh_get_domain_info(SaHpiDomainIdT did, SaHpiDomainInfoT *info)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}
