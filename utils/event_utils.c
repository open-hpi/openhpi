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

/*
 * right now much of event utils is implemented as macros in
 * event_utils.h.  That can change in the future, but the interface
 * will stay the same.
 */

#include <event_utils.h>

struct oh_event* oh_new_oh_event(oh_event_type t) 
{
        struct oh_event * new = NULL;
        new->type = t
        new = g_new0(sizeof(*new));
        if(new == NULL) {
                dbg("Couldn't allocate new oh_event!");
        }
        return new;
}

