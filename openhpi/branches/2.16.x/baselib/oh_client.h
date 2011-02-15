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

#ifndef __OH_CLIENT_H
#define __OH_CLIENT_H

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GStaticRecMutex ohc_lock;

void oh_client_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __OH_CLIENT_H */

