/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003, 2006
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
 *     Renier Morales <renier@openhpi.org>
 */

#ifndef __OH_LOCK_H
#define __OH_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

void data_access_lock(void);
void data_access_unlock(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __OH_LOCK_H */

