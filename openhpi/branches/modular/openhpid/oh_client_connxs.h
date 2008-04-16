/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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
 *
 */

#ifndef __OH_CLIENT_CONNXS_H
#define __OH_CLIENT_CONNXS_H

#include <pthread.h>
#include <glib.h>
#include <config.h>
#include "strmsock.h"

extern "C"
{
#include <SaHpi.h>
#include <oh_error.h>
}

struct oh_client_session {
        SaHpiSessionIdT csid; /* Client Session Id */
        SaHpiSessionIdT dsid; /* Domain Session Id */
        SaHpiDomainIdT did; /* Domain Id */
        GHashTable *connxs; /* Connections for this session (per thread) */
};

extern GHashTable *domains;
extern GHashTable *sessions;
extern GStaticRecMutex sessions_sem;

pcstrmsock oh_create_connx(SaHpiDomainIdT);
void oh_delete_connx(pcstrmsock);
SaHpiSessionIdT oh_add_connx(SaHpiSessionIdT, pcstrmsock);
SaHpiBoolT oh_remove_connxs(SaHpiSessionIdT);
SaHpiBoolT oh_remove_connx(SaHpiSessionIdT);
pcstrmsock oh_get_connx(SaHpiSessionIdT);

#endif /* __OH_CLIENT_CONNXS_H */
