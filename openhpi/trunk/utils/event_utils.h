/* -*- linux-c -*-
 * 
 * $Id$ 
 *
 * (C) Copyright IBM Corp 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *    
 * Authors:  
 *     Sean Dague <sdague@users.sf.net>
 * 
 */

#include <oh_event.h>
#include <glib.h>

#define oh_new_event() g_new0(SaHpiEventT, 1)
#define oh_dup_event(old) g_memdup(old, sizeof(*old))

#define oh_copy_event(new, old)                 \
        do {                                    \
                memset(new, 0, sizeof(*new));     \
                memcpy(new, old, sizeof(*new));  \
        } while(0)

struct oh_event* oh_new_oh_event(oh_event_type);
#define oh_dup_oh_event(old) g_memdup(old, sizeof(*old))

#define oh_copy_oh_event(new, old)                 \
        do {                                    \
                memset(new, 0, sizeof(*new));     \
                memcpy(new, old, sizeof(*new));  \
        } while(0)

