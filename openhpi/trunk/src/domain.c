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

/* sd: I think we need global mappings here, we'll need to 
   address this issue later */
#if 0
static SaHpiDomainIdT dcounter = SAHPI_DEFAULT_DOMAIN_ID;
#endif

#if 0
int reset_domain_list(void) 
{
        SaHpiDomainIdT *temp;
        int i;

        for (i = 0; i < g_slist_length(global_domain_list); i++) {
                temp = (SaHpiDomainIdT*) g_slist_nth_data(global_domain_list, i);
                free(temp);
        }
        g_slist_free(global_domain_list);
        global_domain_list = NULL;

        return 0;
}

#endif

GSList *global_domain_list = NULL;

int domain_exists(SaHpiDomainIdT did) 
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

int domain_add(SaHpiDomainIdT did)
{
        if(domain_exists(did) > 0) {
                dbg("Domain %d exists already, something is fishy", did);
                return -1;
        }
        global_domain_list 
		= g_slist_append(global_domain_list, 
				GUINT_TO_POINTER(did));
        
        return 0;
}

#if 0
int domain_del(SaHpiDomainIdT did)
{
	/* FIXME cleaup resources in domain */
        return 0;
}

#endif
