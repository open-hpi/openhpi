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
#include <config.h>

#ifdef HAVE_THREAD_SAFE
#include <openhpi.h>

/* multi-threading support, use Posix mutex for data access */
/* initialize mutex used for data locking */
static pthread_mutex_t data_access_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

void data_access_lock(void) 
{
        pthread_mutex_lock(&data_access_mutex);
}

void data_access_unlock(void)
{
        pthread_mutex_unlock(&data_access_mutex);
}
#endif/*HAVE_THREAD_SAFE*/
