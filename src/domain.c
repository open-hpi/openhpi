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
 *     Sean Dague <sean@dague.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

GSList *global_domain_list = NULL;

int is_in_domain_list(SaHpiDomainIdT did) 
{
        int i;
        
        for (i = 0; i < g_slist_length(global_domain_list); i++) {
		SaHpiDomainIdT temp;
                temp = GPOINTER_TO_UINT(g_slist_nth_data(global_domain_list, i));
                if(temp == did) {
                        return 1;
                }
        }
        return 0;
}

#define MAX_GLOBAL_DOMAIN 10000
#define MIN_DYNAMIC_DOMAIN (MAX_GLOBAL_DOMAIN+1)

int add_domain(SaHpiDomainIdT did)
{
	if (did>MAX_GLOBAL_DOMAIN) {
		dbg("Could not add so large domain, it is kept for dymanic domain");
		return -1;
	}
        if(is_in_domain_list(did) > 0) {
                dbg("Domain %d exists already, something is fishy", did);
                return -1;
        }
        global_domain_list 
		= g_slist_append(global_domain_list, 
				GUINT_TO_POINTER(did));
        
        return 0;
}

SaHpiDomainIdT new_domain() 
{
	static SaHpiDomainIdT dcounter = MIN_DYNAMIC_DOMAIN;
        
	if(is_in_domain_list(dcounter) > 0) {
                dbg("Domain %d exists already, something is fishy", dcounter);
                return -1;
        }
	global_domain_list
		= g_slist_append(global_domain_list,
				GUINT_TO_POINTER(dcounter));
	return dcounter++;
}
