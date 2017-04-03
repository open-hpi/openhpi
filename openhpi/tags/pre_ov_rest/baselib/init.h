/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2008
 * (C) Copyright Pigeon Point Systems. 2010
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
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#ifndef __BASELIB_INIT_H
#define __BASELIB_INIT_H

#include <SaHpi.h>

#ifdef __cplusplus
extern "C" {
#endif

SaErrorT ohc_init(void);

SaErrorT ohc_finit(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __BASELIB_INIT_H */

