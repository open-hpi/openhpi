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
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */

#ifndef dClient_h
#define dClient_h

#include <SaHpi.h>
#include "strmsock.h"


// default prefix for function names
#ifndef dOpenHpiClientFunction
#define dOpenHpiClientFunction(name) saHpi##name
#define dOpenHpiClientParam(...) (__VA_ARGS__)
#endif

#endif

