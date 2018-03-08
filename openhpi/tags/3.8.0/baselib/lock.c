/*      -*- c -*-
 *
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#include <glib.h>

#include "lock.h"
#include "sahpi_wrappers.h"

#if GLIB_CHECK_VERSION (2, 32, 0)
static GRecMutex ohc_main_lock;
#else
static GStaticRecMutex ohc_main_lock = G_STATIC_REC_MUTEX_INIT;
#endif

void ohc_lock( void )
{
    wrap_g_static_rec_mutex_lock(&ohc_main_lock);
}

void ohc_unlock( void )
{
    wrap_g_static_rec_mutex_unlock(&ohc_main_lock);
}

