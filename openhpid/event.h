/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
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
 *      Sean Dague <sdague@users.sf.net>
 *	    Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __OH_EVENT_H
#define __OH_EVENT_H

#include <oh_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

extern oh_evt_queue * oh_process_q;

/* function definitions */
int oh_event_init(void);
int oh_event_finit(void);
void oh_post_quit_event(void);
int oh_detect_quit_event(struct oh_event * e);
SaErrorT oh_harvest_events(void);
SaErrorT oh_process_events(void);

#ifdef __cplusplus
}
#endif

#endif /* __OH_EVENT_H */

