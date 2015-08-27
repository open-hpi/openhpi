/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005-2006
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
 *
 */

#ifndef __OH_THREADED_H
#define __OH_THREADED_H

#ifdef __cplusplus
extern "C" {
#endif

int oh_threaded_start(void);
void oh_signal_service(void);
int oh_threaded_stop(void);

void oh_wake_discovery_thread(void);

#ifdef __cplusplus
}
#endif

#endif /* __OH_THREADED_H */

