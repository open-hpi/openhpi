/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      David Judkovics <djudkovi@us.ibm.com>
 *      Renier Morales <renierm@users.sf.net>
 */

#ifndef __UID_UTILS_H
#define __UID_UTILS_H

#ifndef __OH_UTILS_H
#warning *** Include oh_utils.h instead of individual utility header files ***
#endif

#define dbg_uid_lock(format, ...) \
        do { \
                if (getenv("OPENHPI_DBG_UID_LOCK") && !strcmp("YES",getenv("OPENHPI_DBG_UID_LOCK"))){ \
                        fprintf(stderr, "        UID_LOCK: %s:%d:%s: ", __FILE__, __LINE__, __func__); \
                        fprintf(stderr, format "\n", ## __VA_ARGS__); \
                } \
        } while(0)

#define uid_lock(uidmutex) \
        do { \
                dbg_uid_lock("Locking UID mutex..."); \
                g_static_mutex_lock(uidmutex); \
                dbg_uid_lock("OK. UID mutex locked."); \
        } while (0)

#define uid_unlock(uidmutex) \
        do { \
                dbg_uid_lock("Unlocking UID mutex..."); \
                g_static_mutex_unlock(uidmutex); \
                dbg_uid_lock("OK. UID mutex unlocked."); \
        } while (0)

#ifdef __cplusplus
extern "C" {
#endif

/* hpi internal apis */
SaErrorT oh_uid_initialize(void);
SaHpiBoolT oh_uid_is_initialized(void);
guint oh_uid_from_entity_path(SaHpiEntityPathT *ep);
gint oh_uid_remove(guint uid);
guint oh_uid_lookup(SaHpiEntityPathT *ep);
gint oh_entity_path_lookup(guint *id, SaHpiEntityPathT *ep);
gint oh_uid_map_to_file(void);
#ifdef __cplusplus
}
#endif

/* uid to entity path cross reference (xref) data structure */
typedef struct {
        SaHpiResourceIdT resource_id;
        SaHpiEntityPathT entity_path;
} EP_XREF;

#endif
