/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *      David M Jukdkovics <dmjudkov@us.ibm.com>
 *
 */

#ifndef __OH_INIT_H
#define __OH_INIT_H

#include <SaHpi.h>
#include <config.h>

#ifdef OH_DAEMON_ENABLED
int initialize(void);
int finalize(void);
#else
int _init(void) __attribute__((constructor));
int _fini(void) __attribute__((destructor));
#endif

#endif /* __OH_INIT_H */
