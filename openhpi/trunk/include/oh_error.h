/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
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
 *
 */

#ifndef OH_ERROR_H
#define OH_ERROR_H

#include <stdio.h>

/* Only print debug info when --enable-debug */
#ifdef DEBUG
#define dbg(format, ...) \
        do { \
                fprintf(stderr, "%s:%d:%s: ", __FILE__, __LINE__, __func__); \
                fprintf(stderr, format "\n", ## __VA_ARGS__); \
        } while(0)
#else
#define dbg(format, ...)
#endif

#define info(f, ...) printf(__FILE__": " f "\n", ## __VA_ARGS__)
#define error(f, ...) fprintf(stderr, "ERROR: " f "\n", ## __VA_ARGS__)
#define trace(f, ...) printf(__FILE__":%s(" f ")\n", __FUNCTION__, ## __VA_ARGS__)
 
#endif /* OH_ERROR_H */

