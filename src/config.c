/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by International Business Machines.
 * Copyright (c) 2003 by Intel Corp.
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
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include <openhpi.h>
#include <oh_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

GSList *global_handler_list=NULL;
#if  0
static struct oh_plugin_config *new_plugin_config(const char *plugin) 
{
        struct oh_plugin_config *pc;

        pc = calloc(1,sizeof(*pc));
        if(pc == NULL) {
                dbg("Couldn't allocate memory for handler_config");
                return pc;
        }
        
        if(plugin != NULL) {
                pc->name = calloc(strlen(plugin)+1, sizeof(char));
                strcpy(pc->name,plugin);
        }
        
        return pc;
}


static struct oh_handler_config *new_handler_config(const char *plugin, char *name, char *address) 
{
        struct oh_handler_config *hc;
        
        hc = calloc(1,sizeof(*hc));
        if(hc == NULL) {
                dbg("Couldn't allocate memory for handler_config");
                return hc;
        }

        if(plugin != NULL) {
                hc->plugin = calloc(strlen(plugin)+1, sizeof(char));
                strcpy(hc->plugin,plugin);
        }
        if(name != NULL) {
                hc->name = calloc(strlen(name)+1, sizeof(char));
                strcpy(hc->name,name);
        }
        if(name != NULL) {
                hc->address = calloc(strlen(address)+1, sizeof(char));
                strcpy(hc->address,address);
        }
        return hc;
}
#endif

/*
  load_config will actually get out to a config file soon enough based
  on DomainId mapping.  For now this just defines the interface.
*/

int oh_load_config (struct oh_config *config) 
{
        const char *name[] = {
		"libdummy",
		NULL,
	};
	int i;
#if 0        
	config->plugins = NULL;
        config->handlers = NULL;
        
        for (i=0; name[i]!=NULL; i++) {
		config->plugins 
			= g_slist_append(
					config->plugins, 
					(gpointer *) new_plugin_config(name[i])
					);
	}
	
        for (i=0; name[i]!=NULL; i++) {
		config->handlers 
			= g_slist_append(
					config->handlers, 
					(gpointer *) new_handler_config(name[i], NULL, NULL)
					);
	}
#else
	/* This is just a quick hacking before real config functio
	 * Currently, it assume all handlers are in default domain */	
	init_plugin();
	add_domain(SAHPI_DEFAULT_DOMAIN_ID);	
	for (i=0; name[i]!=NULL; i++) {
		struct oh_handler *h 
			= new_handler(name[i], NULL, NULL);
		if (!h) {
			dbg("Error on new handler");
			return -1;
		}
		global_handler_list = g_slist_append(global_handler_list, h);
	}
#endif
        return 0;
}
#if 0
void oh_free_config(struct oh_config *config) 
{
        int i;
        struct oh_handler_config *temp;
        struct oh_plugin_config *temp2;

        for(i = 0; i < g_slist_length(config->plugins); i++) {
                temp2 = (struct oh_plugin_config *) g_slist_nth_data(
                        config->plugins, i);
                free(temp2->name);
        }
        for(i = 0; i < g_slist_length(config->handlers); i++) {
                temp = (struct oh_handler_config *) g_slist_nth_data(
                        config->handlers, i);
                free(temp->plugin);
                free(temp->name);
                free(temp->address);
        }
        g_slist_free(config->handlers);
        g_slist_free(config->plugins);
        
        return;
}
#endif
