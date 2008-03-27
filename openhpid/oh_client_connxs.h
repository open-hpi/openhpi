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

extern GHashTable *sessions;
extern GStaticRecMutex sessions_sem;

pcstrmsock oh_create_connx(void);
void oh_delete_connx(pcstrmsock);
bool oh_add_connx(SaHpiSessionIdT, pcstrmsock);
bool oh_remove_connxs(SaHpiSessionIdT);
bool oh_remove_connx(SaHpiSessionIdT);
pcstrmsock oh_get_connx(SaHpiSessionIdT);

#endif /* __OH_CLIENT_CONNXS_H */
