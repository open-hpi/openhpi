/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *       Peter D Phan <pdphan@users.sf.net>
 *
 */

#ifndef ECODE_UTILS_H
#define ECODE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SaHpi.h"

#define NECODES  27

#ifdef __cplusplus
extern "C" {
#endif 

char *decode_error(SaErrorT code);

#ifdef __cplusplus
}
#endif

#endif
