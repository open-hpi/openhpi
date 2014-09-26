/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003, 2005
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

#include <glib.h>

#include "lock.h"
#include "sahpi_wrappers.h"
#if GLIB_CHECK_VERSION (2, 32, 0)
static GRecMutex oh_main_lock;
#else
static GStaticRecMutex oh_main_lock = G_STATIC_REC_MUTEX_INIT;
#endif
void data_access_lock(void)
{
    wrap_g_static_rec_mutex_lock(&oh_main_lock);
}

void data_access_unlock(void)
{
    wrap_g_static_rec_mutex_unlock(&oh_main_lock);
}

