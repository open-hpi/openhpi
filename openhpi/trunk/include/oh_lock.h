/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#ifndef OH_LOCK_H
#define OH_LOCK_H

#include <config.h>
#include <errno.h>
#include <unistd.h>

extern int oh_will_block;

int data_access_block_times(void);
		 
#ifdef HAVE_THREAD_SAFE
/* multi-threading support, use Posix mutex for data access */
/* initialize mutex used for data locking */
#include <glib/gthread.h>
extern GStaticRecMutex oh_main_lock;

#define data_access_lock_init()

#define data_access_lock()                                          \
        do {                                                        \
                if (!g_static_rec_mutex_trylock(&oh_main_lock)) {    \
                        dbg("Going to block for a lock now");       \
                        oh_will_block++;                            \
                        g_static_rec_mutex_lock(&oh_main_lock);      \
                } else {                                            \
                        dbg("Got the lock because no one had it");  \
                }                                                   \
        } while(0)

#define data_access_unlock()                             \
        do {                                             \
                dbg("trying to release the lock");       \
                g_static_rec_mutex_unlock(&oh_main_lock); \
                dbg("released the lock");                \
        } while(0)

#else 

#define data_access_lock_init()
#define data_access_lock()
#define data_access_unlock()

#endif/*HAVE_THREAD_SAFE*/

#endif
