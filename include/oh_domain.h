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

#ifndef OH_DOMAIN_H
#define OH_DOMAIN_H

#include <SaHpi.h>
#include <rpt_utils.h>
#include <sel_utils.h>

/*
 *  Global table of all active domains (oh_domain).
 */
extern GHashTable *domain_table;

/*
 * Representation of an domain
 */
struct oh_domain {
        /* This id is used to app layer
         * to identy domain
         */
        SaHpiDomainIdT domain_id;

        /* Domain's Resource Presence Table */
        RPTable rptable;

        /* Domain Alarm Table */
        void * alarm_table;

        /* Domain Reference Table */
        void * reference_table;

        /* Domain Information */
        SaHpiDomainInfoT domain_info;

        /* Domain Event Log */
        oh_sel *sel;
};

SaErrorT oh_create_domain(SaHpiDomainCapabilitiesT capabilities,
                          SaHpiBoolT isPeer,
                          SaHpiTextBufferT tag,
                          SaHpiDomainIdT *did);
SaErrorT oh_get_domain_rpt(SaHpiDomainIdT did, RPTable *rptable);
SaErrorT oh_get_domain_dat(SaHpiDomainIdT did, void *datable);
SaErrorT oh_get_domain_drt(SaHpiDomainIdT did, void *drtable);
SaErrorT oh_get_domain_del(SaHpiDomainIdT did, oh_sel *delog);
SaErrorT oh_get_domain_info(SaHpiDomainIdT did, SaHpiDomainInfoT *info);

#endif /* OH_DOMAIN_H */
