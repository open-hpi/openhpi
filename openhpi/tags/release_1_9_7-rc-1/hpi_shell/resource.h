/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Racing Guo <racing.guo@intel.com>
 *     Aaron  Chen <yukun.chen@intel.com>
 *     Nick   Yin <hu.yin@intel.com>
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SaHpi.h>

/*  Transition from string to SAHPI type begin */

int sa_discover(void);
int sa_list_res(void);
int sa_show_rpt(SaHpiResourceIdT resourceid);
int sa_show_rdr(SaHpiResourceIdT resourceid);
