/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Sean Dague <http://dague.net/sean>
 */
#ifndef __SNMP_BC_LOCK_H
#define __SNMP_BC_LOCK_H

#include <stdlib.h>
#include <glib.h>
/*
 *  Define our own lock type, this allows for debuging
 *
 */

/* FIXME: turn all this off if there isn't thread safe enabled */

typedef struct {
        GStaticRecMutex lock;
        guint32 count;
} ohpi_bc_lock;

#define dbg_snmp_lock(format, ...) \
        do { \
                if (getenv("OPENHPI_DEBUG_BCLOCK") && !strcmp("YES",getenv("OPENHPI_DEBUG_BCLOCK"))) { \
                        fprintf(stderr, "    BC_LOCK Thread: %p - %s:%d:%s: ", g_thread_self(), __FILE__, __LINE__, __func__); \
                        fprintf(stderr, format "\n", ## __VA_ARGS__); \
                } \
        } while(0)

#define snmp_bc_lock(bclock)                                            \
        do {                                                            \
                if (!g_static_rec_mutex_trylock(&bclock.lock)) {        \
                        dbg_snmp_lock("Lockcount: %d", bclock.count);        \
                        dbg_snmp_lock("Going to block for a lock now");      \
                        g_static_rec_mutex_lock(&bclock.lock);          \
                        dbg_snmp_lock("Got the lock after blocking");        \
                        bclock.count++;                                 \
                } else {                                                \
                        dbg_snmp_lock("Got the lock because no one had it"); \
                        bclock.count++;                                 \
                        dbg_snmp_lock("Lockcount: %d", bclock.count++);      \
                }                                                    \
        } while(0)

#define snmp_bc_unlock(bclock)                                          \
        do {                                                            \
                bclock.count--;                                         \
                g_static_rec_mutex_unlock(&bclock.lock);               \
                dbg_snmp_lock("Released the lock");   \
        } while(0)

#endif
