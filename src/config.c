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
#include <fcntl.h>
#include <unistd.h>

/*
 * The global handler and plugin lists are kept here, where they are initialized
 */

GSList *global_plugin_list = NULL;
GSList *global_handler_list = NULL;

/* 
 * the following stanzas are needed for glib's lexical parser 
 */

enum {
        HPI_CONF_TOKEN_HANDLER = G_TOKEN_LAST,
        HPI_CONF_TOKEN_PLUGIN
} hpiConfType;

static struct {
        gchar *name;
        guint token;
} oh_conf_tokens[] = {
        { "handler", HPI_CONF_TOKEN_HANDLER },
        { "plugin", HPI_CONF_TOKEN_PLUGIN }
};

/*
 * note: many of the switches in the config aren't well explained
 * some trial and error was used to figure out the appropriate values
 */

static GScannerConfig oh_scanner_config =
        {
                (
                        " \t\n"
                        )			/* cset_skip_characters */,
                (
                        G_CSET_a_2_z
                        "_/."
                        G_CSET_A_2_Z
                        )			/* cset_identifier_first */,
                (
                        G_CSET_a_2_z
                        "_-0123456789/."
                        G_CSET_A_2_Z
                        )			/* cset_identifier_nth */,
                ( "#\n" )		/* cpair_comment_single */,
                
                FALSE			/* case_sensitive */,
                
                TRUE			/* skip_comment_multi */,
                TRUE			/* skip_comment_single */,
                TRUE			/* scan_comment_multi */,
                TRUE			/* scan_identifier */,
                TRUE			/* scan_identifier_1char */,
                TRUE			/* scan_identifier_NULL */,
                TRUE			/* scan_symbols */,
                TRUE			/* scan_binary */,
                TRUE			/* scan_octal */,
                TRUE			/* scan_float */,
                TRUE			/* scan_hex */,
                TRUE			/* scan_hex_dollar */,
                TRUE			/* scan_string_sq */,
                TRUE			/* scan_string_dq */,
                TRUE			/* numbers_2_int */,
                FALSE			/* int_2_float */,
                TRUE			/* identifier_2_string */,
                FALSE			/* char_2_token */,
                TRUE			/* symbol_2_token */,
                FALSE			/* scope_0_fallback */,
        };

static struct oh_plugin_config *new_plugin_config(char *plugin) 
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

GTokenType get_next_token_if (GScanner* oh_scanner, GTokenType expected) 
{
        GTokenType my_token;
        
        my_token = g_scanner_peek_next_token (oh_scanner);
        if(my_token == expected) {
                my_token = g_scanner_get_next_token (oh_scanner);
                if(my_token == expected) {
                        return my_token;
                }
        }

        return G_TOKEN_ERROR;
}


int process_handler_token(GScanner* oh_scanner) 
{
        int i;
        GSList *templist = NULL;
        struct oh_handler *temphandler;
        char *temps;
        guint my_token;
        
        my_token = get_next_token_if(oh_scanner, 
                                     HPI_CONF_TOKEN_HANDLER);
        
        if (my_token != HPI_CONF_TOKEN_HANDLER) {
                dbg("Token is not what I was promissed");
                return -1;
        }
        
        for(i = 0; i < 3; i++) {
                my_token = get_next_token_if(oh_scanner, G_TOKEN_STRING);
                if(my_token != G_TOKEN_STRING) {
                        dbg("String expected");
                        goto free_temp;
                } else {
                        dbg("Field %i, %s: ", i, oh_scanner->value.v_string);
                        templist = g_slist_append(
                                templist, 
                                (gpointer *) g_strdup(oh_scanner->value.v_string)
                                );
                }
        }

        temphandler = new_handler(
                g_strdup((char *) g_slist_nth_data(templist, 0)),
                g_strdup((char *) g_slist_nth_data(templist, 1)),
                g_strdup((char *) g_slist_nth_data(templist, 2))
                );
        
        if(temphandler != NULL) {
                global_handler_list = g_slist_append(
                        global_handler_list,
                        (gpointer *) temphandler
                        );
        }
        
        
 free_temp:
        for(i = 0; i < g_slist_length(templist); i++) {
                temps = (char *) g_slist_nth_data(templist, i);
                free(temps);
        }
        g_slist_free(templist);
        
        return 0;
}

/*
  load_config will actually get out to a config file soon enough based
  on DomainId mapping.  For now this just defines the interface.
*/

int oh_load_config (struct oh_config *config) 
{
        int oh_conf_file, i;
        GScanner* oh_scanner;
        int done = 0;
        int num_tokens = (int) (sizeof(oh_conf_tokens) / sizeof(GTokenType));

        init_plugin();
        add_domain(SAHPI_DEFAULT_DOMAIN_ID);	
        
        oh_scanner = g_scanner_new(&oh_scanner_config);
        if(!oh_scanner)
                return -1;

        oh_conf_file = open(OH_DEFAULT_CONF, O_RDONLY);
        if(!oh_conf_file)
                return -2;

        g_scanner_input_file(oh_scanner, oh_conf_file);
        
        for (i = 0; i < num_tokens; i++) {
                g_scanner_add_symbol (oh_scanner, oh_conf_tokens[i].name, 
                                      GINT_TO_POINTER (oh_conf_tokens[i].token));
        }

        while(!done) {
                guint my_token;
                my_token = g_scanner_peek_next_token (oh_scanner);
                
                switch (my_token) 
                {
                case G_TOKEN_EOF:
                        done = 1;
                        break;
                case HPI_CONF_TOKEN_HANDLER:
                        process_handler_token(oh_scanner);
                        
                        break;
                        
                case HPI_CONF_TOKEN_PLUGIN:
                        my_token = g_scanner_get_next_token(oh_scanner); 
                        if (my_token != HPI_CONF_TOKEN_PLUGIN) {
                                dbg("Token is not what I was promissed");
                                break;
                        }
                        my_token = g_scanner_get_next_token(oh_scanner);
                        if(my_token != G_TOKEN_STRING) {
                                dbg("Where the heck is my string!");
                                break;
                        }
                        global_plugin_list = g_slist_append(
                                global_plugin_list,
                                (gpointer *) new_plugin_config(oh_scanner->value.v_string)
                                );
                        break;

                default:
                        /* need to advance it */
                        my_token = g_scanner_get_next_token(oh_scanner);
                        break;
                }
        }
        
        close(oh_conf_file);
        g_scanner_destroy(oh_scanner);
        dbg("Done processing conf file");
        
#if 0        
        /* This is just a quick hacking before real config functio
         * Currently, it assume all handlers are in default domain */	
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



