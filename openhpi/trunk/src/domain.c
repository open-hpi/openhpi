/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2003
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
 *     Sean Dague <http://dague.net/sean>
 *     David Judkovics <djudkovi@us.ibm.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>

/* multi-threading support, use Posix mutex for data access */
/* initialize mutex used for data locking */
extern pthread_mutex_t data_access_mutex; 
/*
 *  Global list of all available domain id's (SaHpiDomainIdT).
 *  The intent is that this list is maintained as new RPT entries
 *  are added and removed from the global RPT table, and used by
 *  saHpiSessionOpen() to determine if the requested domain exist
 *  without doing a full search of the RPT. 
 */
static GSList *global_domain_list = NULL;

int is_in_domain_list(SaHpiDomainIdT did) 
{
        GSList *i;
        
        pthread_mutex_lock(&data_access_mutex);

        g_slist_for_each(i, global_domain_list) {
                struct oh_domain *d = i->data;
                if(d->domain_id == did) {
                        pthread_mutex_unlock(&data_access_mutex);
                        return 1;
                }
        }

        pthread_mutex_unlock(&data_access_mutex);
        
        return 0;
}

struct oh_domain *get_domain_by_id(SaHpiDomainIdT did)
{
        GSList *i;
        
        pthread_mutex_lock(&data_access_mutex);

        g_slist_for_each(i, global_domain_list) {
                struct oh_domain *d = i->data;
                if(d->domain_id == did) {
                        pthread_mutex_unlock(&data_access_mutex);
                        return d;
                }
        }

        pthread_mutex_unlock(&data_access_mutex);

        return NULL;
}

struct oh_domain *get_domain_by_oid(struct oh_domain_id oid)
{
        GSList *i;
        
        pthread_mutex_lock(&data_access_mutex);

        g_slist_for_each(i, global_domain_list) {
                struct oh_domain *d = i->data;
                if(memcmp(&oid, &d->domain_oid, sizeof(oid)) == 0) {
                        pthread_mutex_unlock(&data_access_mutex);
                        return d;
                }
        }

        pthread_mutex_unlock(&data_access_mutex);

        return NULL;
}

#define MAX_GLOBAL_DOMAIN 10000
#define MIN_DYNAMIC_DOMAIN (MAX_GLOBAL_DOMAIN+1)

int add_domain(SaHpiDomainIdT did)
{
        struct oh_domain *d;
        
        if (did>MAX_GLOBAL_DOMAIN) {
                dbg("Could not add so large domain, the region is kept for dymanic domain");
                return -1;
        }
        if(is_in_domain_list(did) > 0) {
                dbg("Domain %d exists already, something is fishy", did);
                return -1;
        }
        
        pthread_mutex_lock(&data_access_mutex);
        
        d = malloc(sizeof(*d));
        if (!d) {
                dbg("Out of memory");
                return -1;
        }
        
        d->domain_id = did;
 
        global_domain_list = g_slist_append(global_domain_list, d);

        pthread_mutex_unlock(&data_access_mutex);
        
        return 0;
}

SaHpiDomainIdT new_domain(struct oh_domain_id domain_oid) 
{
        static SaHpiDomainIdT dcounter = MIN_DYNAMIC_DOMAIN;
        
        struct oh_domain *d;        

        pthread_mutex_lock(&data_access_mutex);  

        if(is_in_domain_list(dcounter) > 0) {
                pthread_mutex_unlock(&data_access_mutex);
                dbg("Domain %d exists already, something is fishy", dcounter);
                return -1;
        }   

        d = malloc(sizeof(*d));
        if (!d) {
                dbg("Out of memory");
                pthread_mutex_unlock(&data_access_mutex);
                return -1;
        }
        memset(d, 0, sizeof(*d));
        
        d->domain_id = dcounter;
        d->domain_oid = domain_oid;
        global_domain_list = g_slist_append(global_domain_list, d);

        pthread_mutex_unlock(&data_access_mutex);

        return dcounter++;
}
