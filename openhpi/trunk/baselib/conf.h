/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#ifndef __BASELIB_CONFIG_H
#define __BASELIB_CONFIG_H

#include <glib.h>

#include <SaHpi.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ohc_domain_conf {
        SaHpiDomainIdT did;
        char host[SAHPI_MAX_TEXT_BUFFER_LENGTH];
        unsigned short port;
        SaHpiEntityPathT entity_root;
};


void ohc_conf_init(void);
const SaHpiEntityPathT * ohc_get_my_entity(void);
const struct ohc_domain_conf * ohc_get_domain_conf(SaHpiDomainIdT did);
const struct ohc_domain_conf * ohc_get_next_domain_conf(SaHpiEntryIdT entry_id,
                                                        SaHpiEntryIdT *next_entry_id);

SaErrorT ohc_add_domain_conf(const char *host,
                             unsigned short port,
                             const SaHpiEntityPathT *entity_root,
                             SaHpiDomainIdT *did);
SaErrorT ohc_add_domain_conf_by_id(SaHpiDomainIdT did,
                                   const char *host,
                                   unsigned short port,
                                   const SaHpiEntityPathT *entity_root);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __BASELIB_CONFIG_H */

