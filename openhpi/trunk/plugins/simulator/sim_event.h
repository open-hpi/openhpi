/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authours:
 *      Racing Guo <racing.guo@intel.com>
 */

#ifndef _INC_SIM_EVENT_
#define _INC_SIM_EVENT_

struct fe_handler{
        volatile int closing;
        struct oh_handler_state *ohh;
        pthread_t tid;
};

struct fe_handler *fhs_event_init(struct oh_handler_state *hnd);
void fhs_event_finish(struct fe_handler *feh);

#endif
