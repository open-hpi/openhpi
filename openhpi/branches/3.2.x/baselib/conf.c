/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renier@openhpi.org>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#include <glib.h>

#include <oHpi.h>

#include <config.h>
#include <oh_error.h>
#include <oh_utils.h>

#include "conf.h"
#include "init.h"
#include "lock.h"


static GHashTable *ohc_domains = NULL;

static SaHpiEntityPathT my_entity
    = { .Entry[0] = { .EntityType = SAHPI_ENT_UNSPECIFIED, .EntityLocation = 0 } };


static int load_client_config(const char *filename);
static void add_domain_conf(SaHpiDomainIdT did,
                            const char *host,
                            unsigned short port,
                            const SaHpiEntityPathT *entity_root);
static void extract_keys(gpointer key, gpointer val, gpointer user_data);
static gint compare_keys(const gint *a, const gint *b);


void ohc_conf_init(void)
{

    ohc_lock();

    // Create domain table
    if (!ohc_domains) { // Create domain table
        char * config_file;
        const struct ohc_domain_conf *default_conf;

        ohc_domains = g_hash_table_new_full(g_int_hash,
                                            g_int_equal,
                                            NULL,
                                            g_free);

        /* TODO: Have a default openhpiclient.conf file in /etc */
        config_file = getenv("OPENHPICLIENT_CONF");
        if (config_file != NULL) {
            load_client_config(config_file);
        } else {
#ifdef _WIN32
            char buf[MAX_PATH];
            config_file = &buf[0];
            DWORD cc = ExpandEnvironmentStrings(OH_CLIENT_DEFAULT_CONF, config_file, MAX_PATH);
            if ((cc != 0) && (cc < MAX_PATH)) {
                load_client_config(config_file);
            }
#else
            load_client_config(OH_CLIENT_DEFAULT_CONF);
#endif /* _WIN32 */
        }

        /* Check to see if a default domain exists, if not, add it */
        default_conf = ohc_get_domain_conf(OH_DEFAULT_DOMAIN_ID);
        if (default_conf == NULL) {
            const char *host, *portstr;
            unsigned short port;
            SaHpiEntityPathT entity_root;

            /* TODO: change these envs to have DEFAULT in the name*/
            host = getenv("OPENHPI_DAEMON_HOST");
            if (host == NULL) {
                host = "localhost";
            }
            portstr = getenv("OPENHPI_DAEMON_PORT");
            if (portstr == NULL) {
                port = OPENHPI_DEFAULT_DAEMON_PORT;
            } else {
                port = atoi(portstr);
            }
            oh_init_ep(&entity_root);

            add_domain_conf(OH_DEFAULT_DOMAIN_ID, host, port, &entity_root);
        }
    }

    ohc_unlock();
}

const SaHpiEntityPathT * ohc_get_my_entity(void)
{
    // NB: Since my_entity is assigned on initialization
    // we don't need to aquire lock
    if ( my_entity.Entry[0].EntityType == SAHPI_ENT_UNSPECIFIED ) {
        if ( my_entity.Entry[0].EntityLocation == 0 ) {
            return 0;
        }
    }

    return &my_entity;
}

const struct ohc_domain_conf * ohc_get_domain_conf(SaHpiDomainIdT did)
{
    struct ohc_domain_conf *dc;
    ohc_lock();
    dc = (struct ohc_domain_conf *)g_hash_table_lookup(ohc_domains, &did);
    ohc_unlock();

    return dc;
}

SaErrorT ohc_add_domain_conf(const char *host,
                             unsigned short port,
                             const SaHpiEntityPathT *entity_root,
                             SaHpiDomainIdT *did)
{
    ohc_lock();

    // get all known domain ids and sort them
    GList *keys = 0;
    g_hash_table_foreach(ohc_domains, extract_keys, &keys);
    keys = g_list_sort(keys, (GCompareFunc)compare_keys);

    // found prev = a gap in domain ids list or max domain id
    // so that new did will be prev + 1
    SaHpiDomainIdT prev_did = 0;
    GList *item;
    for (item = keys; item != NULL; item = item->next) {
        SaHpiDomainIdT item_did = *(const SaHpiDomainIdT *)(item->data);
        if ((prev_did + 1) < item_did) {
            break;
        }
        prev_did = item_did;
    }

    g_list_free(keys);

    if (prev_did == SAHPI_UNSPECIFIED_DOMAIN_ID) {
        ohc_unlock();
        return SA_ERR_HPI_OUT_OF_SPACE;
    }
    if ((prev_did + 1) == SAHPI_UNSPECIFIED_DOMAIN_ID) {
        ohc_unlock();
        return SA_ERR_HPI_OUT_OF_SPACE;
    }

    *did = prev_did + 1;
    add_domain_conf(*did, host, port, entity_root);

    ohc_unlock();

    return SA_OK;
}

SaErrorT ohc_add_domain_conf_by_id(SaHpiDomainIdT did,
                                   const char *host,
                                   unsigned short port,
                                   const SaHpiEntityPathT *entity_root)
{
    if (did==SAHPI_UNSPECIFIED_DOMAIN_ID || 
        did==OH_DEFAULT_DOMAIN_ID)
       return SA_ERR_HPI_INVALID_PARAMS;

    ohc_lock();

    // check new did against all known domain ids 
    if (ohc_get_domain_conf(did) != NULL) {
        ohc_unlock();
        return SA_ERR_HPI_DUPLICATE;
    }
    
    add_domain_conf(did, host, port, entity_root);
    ohc_unlock();
    return SA_OK;
}

const struct ohc_domain_conf * ohc_get_next_domain_conf(SaHpiEntryIdT entry_id,
                                                        SaHpiEntryIdT *next_entry_id)
{
    struct ohc_domain_conf *dc;
    int did, nextdid = SAHPI_UNSPECIFIED_DOMAIN_ID;

    ohc_lock();

    // get all known domain ids and sort them
    GList *keys = 0;
    g_hash_table_foreach(ohc_domains, extract_keys, &keys);
    keys = g_list_sort(keys, (GCompareFunc)compare_keys);

    // DomainId is used for EntryId
    if (entry_id == SAHPI_FIRST_ENTRY) // get first valid domain id
       did = *(const SaHpiDomainIdT *)(keys->data);
    else // EntryId already must be a valid domain id
       did = (SaHpiDomainIdT) entry_id;

    dc = (struct ohc_domain_conf *)g_hash_table_lookup(ohc_domains, &did);

    if (dc != NULL) { 
       // search first domain id > did
       GList *item;
       item = keys;
       while (item != NULL && nextdid <= did) {
          nextdid = *(const SaHpiDomainIdT *)(item->data);
          item = item->next;
       }
       if (nextdid == did) // no next domain id found
          *next_entry_id = SAHPI_LAST_ENTRY;
       else *next_entry_id = (SaHpiEntryIdT) nextdid;
    }
    else *next_entry_id = SAHPI_LAST_ENTRY;

    g_list_free(keys);
    ohc_unlock();

    return dc;
}

static void extract_keys(gpointer key, gpointer val, gpointer user_data)
{
    GList ** key_list = (GList **)(user_data);
    *key_list = g_list_append(*key_list, key);
}

static gint compare_keys(const gint *a, const gint *b)
{
    if ( *a < *b ) {
        return -1;
    } else if ( *a > *b ) {
        return 1;
    } else {
        return 0;
    }
}


/*******************************************************************************
 *  In order to use the glib lexical parser we need to define token
 *  types which we want to switch on
 ******************************************************************************/

enum {
        HPI_CLIENT_CONF_TOKEN_DOMAIN = G_TOKEN_LAST,
        HPI_CLIENT_CONF_TOKEN_DEFAULT,	
        HPI_CLIENT_CONF_TOKEN_HOST,
        HPI_CLIENT_CONF_TOKEN_PORT,
        HPI_CLIENT_CONF_TOKEN_ROOT,
        HPI_CLIENT_CONF_TOKEN_MY_EP,
} hpiClientConfType;

struct tokens {
        gchar *name;
        guint token;
};

static struct tokens ohc_conf_tokens[] = {
        {
                .name = "domain",
                .token = HPI_CLIENT_CONF_TOKEN_DOMAIN
        },
        {
                .name = "default",
                .token = HPI_CLIENT_CONF_TOKEN_DEFAULT
        },
	
        {
                .name = "host",
                .token = HPI_CLIENT_CONF_TOKEN_HOST
        },
        {
                .name = "port",
                .token = HPI_CLIENT_CONF_TOKEN_PORT
        },
        {
                .name = "entity_root",
                .token = HPI_CLIENT_CONF_TOKEN_ROOT
        },
        {
                .name = "my_entity",
                .token = HPI_CLIENT_CONF_TOKEN_MY_EP
        },
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

static GScannerConfig oh_scanner_conf = {
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

static int get_next_good_token_in_domain_stanza(GScanner *oh_scanner) {
        int next_token;

        next_token = g_scanner_get_next_token(oh_scanner);
        while (next_token != G_TOKEN_RIGHT_CURLY &&
               next_token != HPI_CLIENT_CONF_TOKEN_HOST &&
               next_token != HPI_CLIENT_CONF_TOKEN_PORT &&
               next_token != HPI_CLIENT_CONF_TOKEN_ROOT) {
                if (next_token == G_TOKEN_EOF) break;
                next_token = g_scanner_get_next_token(oh_scanner);
        }

        return next_token;
}

static void add_domain_conf(SaHpiDomainIdT did,
                            const char *host,
                            unsigned short port,
                            const SaHpiEntityPathT * entity_root)
{
    struct ohc_domain_conf *domain_conf;

    domain_conf = g_new0(struct ohc_domain_conf, 1);
    domain_conf->did = did;
    strncpy(domain_conf->host, host, SAHPI_MAX_TEXT_BUFFER_LENGTH);
    domain_conf->port = port;
    memcpy(&domain_conf->entity_root, entity_root, sizeof(SaHpiEntityPathT));
    g_hash_table_insert(ohc_domains, &domain_conf->did, domain_conf);
}

static int process_domain_token (GScanner *oh_scanner)
{
        SaHpiDomainIdT did;
        char host[SAHPI_MAX_TEXT_BUFFER_LENGTH];
        unsigned int port;
        SaHpiEntityPathT entity_root;

        int next_token;

        host[0] = '\0';
        port = OPENHPI_DEFAULT_DAEMON_PORT;
        oh_init_ep(&entity_root);

        next_token = g_scanner_get_next_token(oh_scanner);
        if (next_token != HPI_CLIENT_CONF_TOKEN_DOMAIN) {
                CRIT("Processing domain: Expected a domain token");
                return -1;
        }

        /* Get the domain id and store in Hash Table */
        next_token = g_scanner_get_next_token(oh_scanner);
        if (next_token == HPI_CLIENT_CONF_TOKEN_DEFAULT) {
                did = OH_DEFAULT_DOMAIN_ID;
        } else if (next_token == G_TOKEN_INT) {
                if (oh_scanner->value.v_int == 0) { // Domain Id of 0 is invalid
                        CRIT("Processing domain: A domain id of 0 is invalid");
                        return -2;
                }
                did = (SaHpiDomainIdT)oh_scanner->value.v_int;
        } else {
                CRIT("Processing domain: Expected int or string ('default') token");
                return -3;
        }

        /* Check for Left Brace token type. If we have it, then continue parsing. */
        if (g_scanner_get_next_token(oh_scanner) != G_TOKEN_LEFT_CURLY) {
                CRIT("Processing domain: Expected left curly token.");
                return -10;
        }

        next_token = get_next_good_token_in_domain_stanza(oh_scanner);
        while (next_token != G_TOKEN_EOF && next_token != G_TOKEN_RIGHT_CURLY) {
                if (next_token == HPI_CLIENT_CONF_TOKEN_HOST) {
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_EQUAL_SIGN) {
                                CRIT("Processing domain: Expected equal sign");
                                return -10;
                        }
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_STRING) {
                                CRIT("Processing domain: Expected a string");
                                return -10;
                        }
                        if (host[0] == '\0') {
                                strncpy(host, oh_scanner->value.v_string, SAHPI_MAX_TEXT_BUFFER_LENGTH);
                        }
                } else if (next_token == HPI_CLIENT_CONF_TOKEN_PORT) {
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_EQUAL_SIGN) {
                                CRIT("Processing domain: Expected equal sign");
                                return -10;
                        }
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_INT) {
                                CRIT("Processing domain: Expected an integer");
                                return -10;
                        }
                        port = oh_scanner->value.v_int;
                } else if (next_token == HPI_CLIENT_CONF_TOKEN_ROOT) {
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_EQUAL_SIGN) {
                                CRIT("Processing entity_root: Expected equal sign");
                                return -10;
                        }
                        next_token = g_scanner_get_next_token(oh_scanner);
                        if (next_token != G_TOKEN_STRING) {
                                CRIT("Processing entity_root: Expected a string");
                                return -10;
                        }
                        if ( oh_encode_entitypath(oh_scanner->value.v_string, &entity_root) != SA_OK ) {
                                CRIT("Processing entity_root: Invalid entity path");
                                return -10;
                        }
                } else {
                        CRIT("Processing domain: Should not get here!");
                        return -10;
                }
                next_token = g_scanner_get_next_token(oh_scanner);
        }

        if (next_token == G_TOKEN_EOF) {
                CRIT("Processing domain: Expected a right curly");
                return -10;
        } else if (host[0] == '\0') {
                CRIT("Processing domain: Did not find the host parameter");
                return -10;
        }

        add_domain_conf(did, host, port, &entity_root);

        return 0;
}

static int process_my_entity_token(GScanner *oh_scanner)
{
        int next_token;
        SaHpiEntityPathT ep;

        next_token = g_scanner_get_next_token(oh_scanner);
        if (next_token != HPI_CLIENT_CONF_TOKEN_MY_EP) {
                CRIT("Processing my_entity: Expected a my_entity token");
                return -1;
        }
        next_token = g_scanner_get_next_token(oh_scanner);
        if (next_token != G_TOKEN_EQUAL_SIGN) {
                CRIT("Processing my_entity: Expected equal sign");
                return -2;
        }
        next_token = g_scanner_get_next_token(oh_scanner);
        if (next_token != G_TOKEN_STRING) {
                CRIT("Processing my_entity: Expected a string");
                return -3;
        }
        if ( oh_encode_entitypath(oh_scanner->value.v_string, &ep) != SA_OK ) {
                CRIT("Processing my_entity: Invalid entity path");
                return -4;
        }

        // NB: Since this code works only on initialization
        // we don't need to acquire lock
        memcpy( &my_entity, &ep, sizeof(SaHpiEntityPathT) );

        return 0;
}


static void scanner_msg_handler (GScanner *scanner, gchar *message, gboolean is_error)
{
        g_return_if_fail (scanner != NULL);

        CRIT("%s:%d: %s%s\n",
            scanner->input_name ? scanner->input_name : "<memory>",
            scanner->line, is_error ? "error: " : "", message );
}

static int load_client_config(const char *filename)
{
        int i, done = 0;
        GScanner *oh_scanner;
        int num_tokens = sizeof(ohc_conf_tokens) / sizeof(ohc_conf_tokens[0]);

        if (!filename) {
                CRIT("Error. Invalid parameters");
                return -1;
        }

        oh_scanner = g_scanner_new(&oh_scanner_conf);
        if (!oh_scanner) {
                CRIT("Couldn't allocate g_scanner for file parsing");
                return -2;
        }

        oh_scanner->msg_handler = scanner_msg_handler;
        oh_scanner->input_name = filename;

        FILE * fp = fopen(filename, "r");
        if (!fp) {
                CRIT("Client configuration file '%s' could not be opened", filename);
                g_scanner_destroy(oh_scanner);
                return -3;
        }

#ifdef _WIN32
        g_scanner_input_file(oh_scanner, _fileno(fp));
#else
        g_scanner_input_file(oh_scanner, fileno(fp));
#endif

        for (i = 0; i < num_tokens; i++) {
                g_scanner_scope_add_symbol(
                        oh_scanner, 0,
                        ohc_conf_tokens[i].name,
                        GUINT_TO_POINTER(ohc_conf_tokens[i].token));
        }

        while (!done) {
                guint my_token;
                my_token = g_scanner_peek_next_token(oh_scanner);
                switch (my_token)
                {
                case G_TOKEN_EOF:
                        done = 1;
                        break;
                case HPI_CLIENT_CONF_TOKEN_DOMAIN:
                        process_domain_token(oh_scanner);
                        break;
                case HPI_CLIENT_CONF_TOKEN_MY_EP:
                        process_my_entity_token(oh_scanner);
                        break;
                default:
                        /* need to advance it */
                        my_token = g_scanner_get_next_token(oh_scanner);
                        g_scanner_unexp_token(oh_scanner, G_TOKEN_SYMBOL,
                                              NULL, "\"domain\" or \"my_entity\"", NULL, NULL, 1);
                        break;
                }
        }

        fclose(fp);

        done = oh_scanner->parse_errors;

        g_scanner_destroy(oh_scanner);

        return 0;
}
