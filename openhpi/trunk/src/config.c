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

/*******************************************************************************
 *  global_plugin_list - list of all the plugins that should be loaded
 *
 *  global_handler_list - list of handlers that have been loaded
 ******************************************************************************/

GSList *global_plugin_list = NULL;
GSList *global_handler_list = NULL;
GSList *global_handler_configs = NULL;

/*******************************************************************************
 *  In order to use the glib lexical parser we need to define token 
 *  types which we want to switch on, and also 
 ******************************************************************************/

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

/*******************************************************************************
 * In order to initialize the lexical scanner, you need the following config.
 * This config was figured out by reading the glib sources, and lots of
 * trial and error (documentation for this isn't very good).
 *
 * G_TOKEN_STRING will be created when anything starts with a-zA-z_/.
 * due to cset_identifier_first and identifier2string values below.
 * Therefor, if you want 0 to be scanned as a string, you need to quote 
 * it (e.g. "0")
 *
 *******************************************************************************/

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

/*******************************************************************************
 *  prototypes for functions internal to this file
 ******************************************************************************/
GTokenType get_next_token_if (GScanner *, GTokenType);

int process_plugin_token (GScanner *);

int process_handler_token (GScanner *);

struct oh_plugin_config * new_plugin_config (char *);
struct oh_handler_config * new_handler_config (char *, char *, char *);


/**
 * new_plugin_config:
 * @plugin: 
 * 
 * 
 * 
 * Return value: 
 **/

struct oh_plugin_config *new_plugin_config (char *plugin) 
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
        pc->refcount = 0;

        return pc;
}

/**
 * new_plugin_config:
 * @plugin: 
 * 
 * 
 * 
 * Return value: 
 **/
struct oh_handler_config *new_handler_config (char *plugin, char *name, char *addr) 
{
        struct oh_handler_config *hc;
        
        hc = calloc(1,sizeof(*hc));
        if(hc == NULL) {
                dbg("Couldn't allocate memory for handler_config");
                return hc;
        }
        
        if(plugin != NULL) {
                hc->plugin = g_strdup(plugin);
        }
        if(name != NULL) {
                hc->name = g_strdup(name);
        }
        if(addr != NULL) {
                hc->addr = g_strdup(addr);
        }
        
        return hc;
}

/**
 * get_next_token_if:  returns the next token if it matches an expected type
 * @oh_scanner:  a scanner object
 * @expected:  the expected token type
 * 
 * 
 * 
 * Return value: expected on success, G_TOKEN_ERROR on fail
 **/

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

/**
 * process_plugin_token:
 * @oh_scanner: 
 * 
 * 
 * 
 * Return value: 
 **/
int process_plugin_token (GScanner *oh_scanner) 
{
        guint my_token;
        int refcount;

        my_token = g_scanner_get_next_token(oh_scanner); 
        if (my_token != HPI_CONF_TOKEN_PLUGIN) {
                dbg("Token is not what I was promissed");
                return -1;
        }
        my_token = g_scanner_get_next_token(oh_scanner);
        if(my_token != G_TOKEN_STRING) {
                dbg("Where the heck is my string!");
                return -1;
        }
        
        refcount = plugin_refcount(oh_scanner->value.v_string);
        if(refcount < 0) {
                global_plugin_list = g_slist_append(
                        global_plugin_list,
                        (gpointer *) new_plugin_config(oh_scanner->value.v_string)
                        );
        } else {
                dbg("WARNING: Attempt to load a plugin more than once");
                return -1;
        }
        return 0;
}

/**
 * plugin_refcount:
 * @name: 
 * 
 * 
 * 
 * Return value: 
 **/
int plugin_refcount (char *name) 
{
        struct oh_plugin_config *temp_config;
        int refcount = -1;
        
        temp_config = plugin_config(name); 

        if(temp_config != NULL) {
                refcount = temp_config->refcount;
        }
        return refcount;
}

struct oh_plugin_config * plugin_config (char *name) 
{
        GSList *node;
        struct oh_plugin_config *temp_config = NULL;
        struct oh_plugin_config *return_config = NULL;
        
        node = global_plugin_list;
        while(g_slist_next(node) != NULL) {
                temp_config = (struct oh_plugin_config *) node->data;
                if(strcmp(temp_config->name, name) == 0) {
                        return_config = (struct oh_plugin_config *) node->data;
                        break;
                }
        }
        
        return return_config;
}

/**
 * process_handler_token:  handles parsing of handler tokens into 
 * @oh_scanner: 
 * 
 * 
 * 
 * Return value: 0 on sucess, < 0 on fail
 **/
int process_handler_token (GScanner* oh_scanner) 
{
        int i;
        int numstanza = 3;
        GSList *templist = NULL;
        struct oh_handler_config *temp_config;
        char *temps;
        guint my_token;
        
        my_token = get_next_token_if(oh_scanner, 
                                     HPI_CONF_TOKEN_HANDLER);
        
        if (my_token != HPI_CONF_TOKEN_HANDLER) {
                dbg("Token is not what I was promissed");
                return -1;
        }
        
        for(i = 0; i < numstanza; i++) {
                my_token = get_next_token_if(oh_scanner, G_TOKEN_STRING);
                if(my_token != G_TOKEN_STRING) {
                        dbg("String expected");
                        goto free_temp;
                } else {
                        templist = g_slist_append(
                                templist, 
                                (gpointer *) g_strdup(oh_scanner->value.v_string)
                                );
                }
        }

        temp_config = new_handler_config(
                g_slist_nth_data(templist, 0),
                g_slist_nth_data(templist, 1),
                g_slist_nth_data(templist, 2)
                );
        
        if(temp_config != NULL) {
                global_handler_configs = g_slist_append(
                        global_handler_configs,
                        (gpointer *) temp_config
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

/**
 * oh_load_config:
 * @filename: 
 * 
 * 
 * 
 * Return value: 
 **/
int oh_load_config (char *filename) 
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

        oh_conf_file = open(filename, O_RDONLY);
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
                        process_plugin_token(oh_scanner);
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
        
        return 0;
}



