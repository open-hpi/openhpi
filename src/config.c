/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by International Business Machines.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Sean Dague <sean@dague.net>
 */

#include <openhpi.h>
#include <oh_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

struct oh_handler_config *new_handler_config(char *, char *, char *);

/*
  load_config will actually get out to a config file soon enough based
  on DomainId mapping.  For now this just defines the interface.
*/

int load_domain_config (struct oh_domain_config **config, SaHpiDomainIdT DomainId) 
{
        char dn[] = "default_domain";
        char p1[] = "libdummy";
        char p2[] = "libwatchdog";
	
	if (DomainId != DEFAULT_DOMAIN_ID)
		return -1;

        (*config)->domain_name = calloc(1,sizeof(dn));
        strcpy((*config)->domain_name,dn);
        (*config)->plugins = NULL;
        
        (*config)->plugins = g_slist_append(
                (*config)->plugins, 
                (gpointer *) new_handler_config(p1, NULL, NULL)
                );
        
        (*config)->plugins = g_slist_append(
                (*config)->plugins, 
                (gpointer *) new_handler_config(p2, NULL, NULL)
                );
        return 0;
}

void free_domain_config(struct oh_domain_config *dconf) 
{
        int i;
        struct oh_handler_config *temp;

        for(i = 0; i < g_slist_length(dconf->plugins); i++) {
                temp = (struct oh_handler_config *) g_slist_nth_data(
                        dconf->plugins, i);
                free(temp->plugin);
                free(temp->name);
                free(temp->address);
        }
        g_slist_free(dconf->plugins);
        free(dconf->domain_name);
        free(dconf);
        return;
}

struct oh_handler_config *new_handler_config (char *plugin, char *name, char *address) 
{
        struct oh_handler_config *hc;
        
        hc = calloc(1,sizeof(*hc));
        if(hc == NULL) {
                dbg("Couldn't allocate memory for handler_config");
                return hc;
        }

        if(plugin != NULL) {
                (*hc).plugin = calloc(strlen(plugin)+1, sizeof(char));
                strcpy(hc->plugin,plugin);
        }
        if(name != NULL) {
                (*hc).name = calloc(strlen(name)+1, sizeof(char));
                strcpy(hc->name,name);
        }
        if(name != NULL) {
                (*hc).address = calloc(strlen(address)+1, sizeof(char));
                strcpy(hc->address,address);
        }
        return hc;
}
