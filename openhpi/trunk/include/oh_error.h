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
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define dbg(format, ...)                                                \
        do {                                                            \
                if (getenv("OHDEBUG") != NULL) {                        \
                        if (strcmp((char *)getenv("OHDEBUG"),"YES") == 0) { \
                                fprintf(stderr, "%s:%d:%s: ", __FILE__, __LINE__, __func__); \
                                fprintf(stderr, format "\n", ## __VA_ARGS__); \
                        }                                               \
                }                                                       \
        } while(0)

#define info(f, ...) printf(__FILE__": " f "\n", ## __VA_ARGS__)
#define error(f, ...) fprintf(stderr, "ERROR: " f "\n", ## __VA_ARGS__)
#define trace(f, ...) printf(__FILE__":%s(" f ")\n", __FUNCTION__, ## __VA_ARGS__)

#ifdef __cplusplus
}
#endif
        
#endif /* OH_ERROR_H */

