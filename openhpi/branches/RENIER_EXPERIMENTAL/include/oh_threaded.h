/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
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

#ifndef __OH_THREADED_H
#define __OH_THREADED_H

#include <SaHpi.h>
#include <glib.h>
#include <oh_event.h>

#ifdef __cplusplus
extern "C" {
#endif

gboolean oh_threaded_mode(void);
int oh_threaded_init(void);
int oh_threaded_start(void);
int oh_threaded_final(void);

#ifdef __cplusplus
}
#endif

#endif /* __OH_THREADED_H */