/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003-2004
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
 *     Sean Dague <http://dague.net/sean>
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     David Judkovics <djudkovi@us.ibm.com>
 *     Thomas Kangieser <Thomas.Kanngieser@fci.com>
 *     Renier Morales <renierm@users.sf.net>
 */

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <oh_config.h>
#include <oh_plugin.h>
#include <oh_error.h>
#include <oh_lock.h>

/*
 * Global Parameters Table
 */
static GHashTable *global_params = NULL;

static const char *known_globals[] = {
        "OPENHPI_LOG_SEV",
        "OPENHPI_ON_EP"
};

/*
 * List of plugin names specified in configuration file.
 */
static GSList *plugin_names = NULL;

/*
 *  List of handler configs (parameter tables).  This list is
 *  populated during config file parse, and used to build the handler_table
 */
static GSList *handler_configs = NULL;

/*******************************************************************************
 *  In order to use the glib lexical parser we need to define token
 *  types which we want to switch on
 ******************************************************************************/

enum {
        HPI_CONF_TOKEN_GLOBAL = G_TOKEN_LAST,
        HPI_CONF_TOKEN_HANDLER,
        HPI_CONF_TOKEN_PLUGIN
} hpiConfType;

struct tokens {
        gchar *name;
        guint token;
};

static struct tokens oh_conf_tokens[] = {
        {
                .name = "global",
                .token = HPI_CONF_TOKEN_GLOBAL
        },
        {
                .name = "handler",
                .token = HPI_CONF_TOKEN_HANDLER
        },
        {
                .name = "plugin",
                .token = HPI_CONF_TOKEN_PLUGIN
        }

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
                        )                       /* cset_skip_characters */,
                (
                        G_CSET_a_2_z
                        "_/."
                        G_CSET_A_2_Z
                        )                       /* cset_identifier_first */,
                (
                        G_CSET_a_2_z
                        "_-0123456789/."
                        G_CSET_A_2_Z
                        )                       /* cset_identifier_nth */,
                ( "#\n" )               /* cpair_comment_single */,
                FALSE                   /* case_sensitive */,
                TRUE                    /* skip_comment_multi */,
                TRUE                    /* skip_comment_single */,
                TRUE                    /* scan_comment_multi */,
                TRUE                    /* scan_identifier */,
                TRUE                    /* scan_identifier_1char */,
                TRUE                    /* scan_identifier_NULL */,
                TRUE                    /* scan_symbols */,
                TRUE                    /* scan_binary */,
                TRUE                    /* scan_octal */,
                TRUE                    /* scan_float */,
                TRUE                    /* scan_hex */,
                TRUE                    /* scan_hex_dollar */,
                TRUE                    /* scan_string_sq */,
                TRUE                    /* scan_string_dq */,
                TRUE                    /* numbers_2_int */,
                FALSE                   /* int_2_float */,
                TRUE                    /* identifier_2_string */,
                TRUE                    /* char_2_token */,
                TRUE                    /* symbol_2_token */,
                FALSE                   /* scope_0_fallback */,
        };

static int seen_plugin(char *plugin_name)
{
        int seen = 0;
        GSList *node = NULL;

        for (node = plugin_names; node; node = node->next) {
                char *p_name = (char *)node->data;
                if (strcmp(plugin_name, p_name) == 0) {
                        seen = 1;
                        break;
                }
        }

        return seen;
}

/**
 * process_plugin_token:
 * @oh_scanner:
 *
 *
 *
 * Return value:
 **/
static int process_plugin_token (GScanner *oh_scanner)
{
        guint my_token;
        int seen = 0;

        data_access_lock();

        my_token = g_scanner_get_next_token(oh_scanner);
        if (my_token != HPI_CONF_TOKEN_PLUGIN) {
                dbg("Token is not what I was promissed");
                data_access_unlock();
                return -1;
        }
        my_token = g_scanner_get_next_token(oh_scanner);
        if(my_token != G_TOKEN_STRING) {
                dbg("Where the heck is my string!");
                data_access_unlock();
                return -1;
        }

        seen = seen_plugin(oh_scanner->value.v_string);

        if(!seen) {
                plugin_names = g_slist_append(
                        plugin_names,
                        (gpointer) g_strdup(oh_scanner->value.v_string)
                        );
        } else {
                dbg("Plugin name %s was found more than once. Ignoring.",
                    oh_scanner->value.v_string);
        }

        data_access_unlock();

        return 0;
}

/**
 * process_handler_token
 * @oh_scanner
 * @is_global: says if the handler is global (true) or not.
 *
 * handles parsing of handler tokens into a hash table.
 * If the the handler is global, it converts the name/values to
 * environment variables.
 *
 * Return value: 0 on sucess, < 0 on fail
 **/
static int process_handler_token (GScanner* oh_scanner, int is_global)
{
        GHashTable *handler_stanza = NULL;
        char *tablekey, *tablevalue;
        int found_right_curly = 0;
        int THIS_TOKEN;

        data_access_lock();

        if (is_global) {
                THIS_TOKEN = HPI_CONF_TOKEN_GLOBAL;
        } else THIS_TOKEN = HPI_CONF_TOKEN_HANDLER;

        if (g_scanner_get_next_token(oh_scanner) != THIS_TOKEN) {
                dbg("Processing handler: Unexpected token.");
                data_access_unlock();
                return -1;
        }

        /* Get the plugin type and store in Hash Table */
        if (!is_global && g_scanner_get_next_token(oh_scanner) != G_TOKEN_STRING) {
                dbg("Processing handler: Expected string token.");
                data_access_unlock();
                return -1;
        } else if (!is_global) {
                handler_stanza = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                       g_free, g_free);
                tablekey = g_strdup("plugin");
                if (!tablekey) {
                        dbg("Processing handler: Unable to allocate memory");
                        goto free_table;
                }
                tablevalue = g_strdup(oh_scanner->value.v_string);
                if (!tablevalue) {
                        dbg("Processing handler: Unable to allocate memory");
                        goto free_table_and_key;
                }
                g_hash_table_insert(handler_stanza,
                                    (gpointer) tablekey,
                                    (gpointer) tablevalue);
        }

        /* Check for Left Brace token type. If we have it, then continue parsing. */
        if (g_scanner_get_next_token(oh_scanner) != G_TOKEN_LEFT_CURLY) {
                dbg("Processing handler: Expected left curly token.");
                goto free_table;
        }

        while(!found_right_curly) {
                /* get key token in key\value pair set (e.g. key = value) */
                if (g_scanner_get_next_token(oh_scanner) != G_TOKEN_STRING) {
                        dbg("Processing handler: Expected string token.");
                        goto free_table;
                } else {
                        tablekey = g_strdup(oh_scanner->value.v_string);
                        if (!tablekey) {
                                dbg("Processing handler: Unable to allocate memory");
                                goto free_table;
                        }
                }

                /* Check for the equal sign next. If we have it, continue parsing */
                if (g_scanner_get_next_token(oh_scanner) != G_TOKEN_EQUAL_SIGN) {
                        dbg("Processing handler: Expected equal sign token.");
                        goto free_table_and_key;
                }

                /**
                Now check for the value token in the key\value set. Store the key\value value pair
                in the hash table and continue on.
                */
                if (!is_global &&
                    g_scanner_peek_next_token(oh_scanner) != G_TOKEN_INT &&
                    g_scanner_peek_next_token(oh_scanner) != G_TOKEN_FLOAT &&
                    g_scanner_peek_next_token(oh_scanner) != G_TOKEN_STRING) {
                        dbg("Processing handler: Expected string, integer, or float token.");
                        goto free_table_and_key;
                } else if (is_global &&
                           g_scanner_peek_next_token(oh_scanner) != G_TOKEN_STRING) {
                        dbg("Processing handler: Expected string, integer, or float token.");
                        goto free_table_and_key;
                } else { /* The type of token tells us how to fetch the value from oh_scanner */
                        gulong *value_int;
                        gdouble *value_float;
                        gchar *value_string;
                        gpointer value;
                        int current_token = g_scanner_get_next_token(oh_scanner);

                        if (current_token == G_TOKEN_INT) {
                                value_int = (gulong *)g_malloc(sizeof(gulong));
                                if (value_int != NULL) *value_int = oh_scanner->value.v_int;
                                value = (gpointer)value_int;
                        } else if (current_token == G_TOKEN_FLOAT) {
                                value_float = (gdouble *)g_malloc(sizeof(gdouble));
                                if (value_float != NULL) *value_float = oh_scanner->value.v_float;
                                value = (gpointer)value_float;
                        } else {
                                value_string = g_strdup(oh_scanner->value.v_string);
                                value = (gpointer)value_string;
                        }

                        if (value == NULL) {
                                dbg("Processing handler: Unable to allocate memory for value. Token Type: %d",
                                    current_token);
                                goto free_table_and_key;
                        }

                        if (is_global) {
                                g_hash_table_insert(global_params,
                                                    (gpointer) tablekey,
                                                    value);
                        } else {
                                g_hash_table_insert(handler_stanza,
                                                    (gpointer) tablekey,
                                                    value);
                        }
                }

                if (g_scanner_peek_next_token(oh_scanner) == G_TOKEN_RIGHT_CURLY) {
                        g_scanner_get_next_token(oh_scanner);
                        found_right_curly = 1;
                }
        } /* end of while(!found_right_curly) */

        /* Attach table describing handler stanza to the global linked list of handlers */
        if(!is_global && handler_stanza != NULL) {
                handler_configs = g_slist_append(
                        handler_configs,
                        (gpointer) handler_stanza);
        }

        data_access_unlock();

        return 0;

free_table_and_key:
        g_free(tablekey);
free_table:
        /**
        There was an error reading a token so we need to error out,
        but not before cleaning up. Destroy the table.
        */
        g_hash_table_destroy(handler_stanza);

        data_access_unlock();

        return -1;
}

/**
 * scanner_msg_handler: a reference of this function is passed into the GScanner.
 * Used by the GScanner object to output messages that come up during parsing.
 *
 * @scanner: Object used to parse the config file.
 * @message: Message string.
 * @is_error: Bit to say the message is an error.
 *
 * Return value: None (void).
 **/
static void scanner_msg_handler (GScanner *scanner, gchar *message, gboolean is_error)
{
  g_return_if_fail (scanner != NULL);

  dbg("%s:%d: %s%s\n",
      scanner->input_name ? scanner->input_name : "<memory>",
      scanner->line, is_error ? "error: " : "", message );
}

/* Read in globals from environment and replace in table for globals */
static void read_globals_from_env(void)
{
        int known_globals_total = sizeof(known_globals)/sizeof(known_globals[0]);
        int i;

        for (i = 0; i < known_globals_total; i++) {
                char *g = getenv(known_globals[i]);
                if (g) {
                        if (!oh_set_global_param(known_globals[i], g))
                                trace("Set global param %s from environment.",
                                      known_globals[i]);
                }
        }
}

/**
 * oh_load_config:
 * @filename:
 * @config
 *
 *
 * Return value:
 **/
int oh_load_config (char *filename, struct oh_parsed_config *config)
{
        int oh_conf_file, i;
        GScanner *oh_scanner;
        int done = 0;
        int num_tokens = sizeof(oh_conf_tokens) / sizeof(oh_conf_tokens[0]);

        if (!filename || !config) {
                dbg("Error. Invalid parameters");
                return -1;
        }

        oh_scanner = g_scanner_new(&oh_scanner_config);
        if(!oh_scanner) {
                dbg("Couldn't allocate g_scanner for file parsing");
                return -2;
        }

        global_params = g_hash_table_new_full(g_str_hash, g_str_equal,
                                              g_free, g_free);
        if (!global_params) {
                dbg("Could not allocate for global_params hash table.");
                return -3;
        }

        oh_scanner->msg_handler = scanner_msg_handler;
        oh_scanner->input_name = filename;

        oh_conf_file = open(filename, O_RDONLY);
        if(oh_conf_file < 0) {
                dbg("Configuration file '%s' could not be opened", filename);
                g_scanner_destroy(oh_scanner);
                read_globals_from_env();
                return -4;
        }

        g_scanner_input_file(oh_scanner, oh_conf_file);

        for (i = 0; i < num_tokens; i++) {
                g_scanner_scope_add_symbol(oh_scanner, 0, oh_conf_tokens[i].name,
                                           (void *)((unsigned long)oh_conf_tokens[i].token));
        }

        while(!done) {
                guint my_token;
                my_token = g_scanner_peek_next_token (oh_scanner);
                /*dbg("token: %d", my_token);*/
                switch (my_token)
                {
                case G_TOKEN_EOF:
                        done = 1;
                        break;
                case HPI_CONF_TOKEN_GLOBAL:
                        process_handler_token(oh_scanner, 1);
                        break;
                case HPI_CONF_TOKEN_HANDLER:
                        process_handler_token(oh_scanner, 0);
                        break;
                case HPI_CONF_TOKEN_PLUGIN:
                        process_plugin_token(oh_scanner);
                        break;
                default:
                        /* need to advance it */
                        my_token = g_scanner_get_next_token(oh_scanner);
                        g_scanner_unexp_token(oh_scanner, G_TOKEN_SYMBOL,
                                              NULL, "\"handle\" or \"plugin\"", NULL, NULL, 1);
                        break;
                }
        }

        if(close(oh_conf_file) != 0) {
                dbg("Couldn't close file '%s'.", filename);
                g_scanner_destroy(oh_scanner);
                return -5;
        }

        done = oh_scanner->parse_errors;

        g_scanner_destroy(oh_scanner);

        trace("Done processing conf file.\nNumber of parse errors:%d", done);

        read_globals_from_env();
        config->plugin_names = plugin_names;
        config->handler_configs = handler_configs;

        return 0;
}

void oh_clean_config()
{
        /* Free list of plugin names read from configuration file. */
        g_slist_free(plugin_names);

        /* Free list of handler configuration blocks */
        g_slist_free(handler_configs);
}

void oh_unload_config()
{
        g_hash_table_destroy(global_params);
}

/**
 * oh_lookup_global_param
 * @param
 * @value
 * @size
 *
 * Returns: 0 on Success.
 **/
int oh_lookup_global_param(char *param, char *value, int size)
{
        char *v = NULL;

        if (!param || !value) {
                dbg("ERROR. Invalid parameters.");
                return -1;
        }

        data_access_lock();
        v = (char *)g_hash_table_lookup(global_params, param);
        if (v) {
                strncpy(value, v, size);
        }
        data_access_unlock();

        return (v) ? 0 : -1;
}

int oh_set_global_param(const char *param, char *value)
{
        char c;

        if (!oh_lookup_next_plugin(NULL, &c, sizeof(c))) {
                dbg("ERROR. Cannot set global params. Plugins already loaded.");
                return -1;
        }

        if (!param || !value) {
                dbg("ERROR. Invalid parameters.");
                return -2;
        }

        data_access_lock();
        g_hash_table_replace(global_params, g_strdup(param), g_strdup(value));
        data_access_unlock();

        return 0;
}
