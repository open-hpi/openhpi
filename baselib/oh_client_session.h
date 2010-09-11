/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __OH_CLIENT_SESSION_H
#define __OH_CLIENT_SESSION_H

#include <strmsock.h>

#include <glib.h>
#include <SaHpi.h>

#ifdef __cplusplus
extern "C" {
#endif

void oh_client_session_init(void);

SaErrorT oh_create_connx(SaHpiDomainIdT, pcstrmsock *);
void oh_delete_connx(pcstrmsock);
SaErrorT oh_close_connx(SaHpiSessionIdT);
SaErrorT oh_get_connx(SaHpiSessionIdT, SaHpiSessionIdT *, pcstrmsock *, SaHpiDomainIdT *);

SaHpiSessionIdT oh_open_session(SaHpiDomainIdT, SaHpiSessionIdT, pcstrmsock);
SaErrorT oh_close_session(SaHpiSessionIdT);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __OH_CLIENT_SESSION_H */
