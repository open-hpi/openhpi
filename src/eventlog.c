/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * Copyright (c) 2003 by International Business Machines
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>


int dsel_get_info(SaHpiDomainIdT domain_id, SaHpiSelInfoT *info)
{
	return 0;
}

int dsel_get_state(SaHpiDomainIdT domain_id)
{
	return 0;
}

void dsel_set_state(SaHpiDomainIdT domain_id, int enable)
{
}

SaHpiTimeT dsel_get_time(SaHpiDomainIdT domain_id)
{
	return 0;
}

void dsel_set_time(SaHpiDomainIdT domain_id, SaHpiTimeT time)
{
}
