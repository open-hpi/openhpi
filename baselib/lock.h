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

#ifndef __BASELIB_LOCK_H
#define __BASELIB_LOCK_H

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

extern GStaticRecMutex ohc_lock;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __BASELIB_LOCK_H */

