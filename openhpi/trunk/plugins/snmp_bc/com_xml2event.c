/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include <glib.h>
#include <string.h>

#include <snmp_bc_plugin.h>

GHashTable *bc_xml2event_hash = NULL;
unsigned int bc_xml2event_hash_use_count = 0;

GHashTable *rsa_xml2event_hash = NULL;
unsigned int rsa_xml2event_hash_use_count = 0;

/* local prototypes */
static void free_hash_data(gpointer key, gpointer value, gpointer user_data);
static void event_start_element(GMarkupParseContext *context,
                                const gchar         *element_name,
                                const gchar        **attribute_names,
                                const gchar        **attribute_values,
                                gpointer             user_data,
                                GError             **error);


/**********************************************************************
 * xml2event_hash_init: Read in the XML with the GLib markup APIs
 * and create the BC or RSA hash table.
 *
 * @hashtable: The hash table to use to store events
 * @xmlstr: The string containing the complete xml to parse
 *
 * return value: 0 = hash table created successfully
 *               -1 = An error occured, see the debug messages. In this case
 *                    the caller should attempt to free the hash table
 **********************************************************************/

int xml2event_hash_init(GHashTable **hashtable, const char *xmlstr) {
        GMarkupParser parser;
        GMarkupParseContext *pcontext;
        gboolean rc;
        GError *err = NULL;

        /* initialize the parser */
        memset(&parser, 0, sizeof(parser));
        parser.start_element = event_start_element;
        pcontext = g_markup_parse_context_new(&parser, 0, hashtable, NULL);
        if (pcontext == NULL) {
		dbg("Cannot create parser context for event XML file");
		return -1;
        }

        /* create the hash table */
	*hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	if (*hashtable == NULL) {
                g_markup_parse_context_free(pcontext);
		dbg("Cannot allocate xml2event_hash table");
		return -1;
	}

        /* parse the xml */
        rc = g_markup_parse_context_parse(pcontext,
                                          (const gchar *)xmlstr,
                                          (gssize)strlen(xmlstr), &err);
        if (rc == FALSE || err != NULL) {
                if (err != NULL) {
                        dbg("%s", err->message);
                        g_error_free(err);
                }
                else {
                        dbg("Unknown XML parse error");
                }
                g_markup_parse_context_free(pcontext);
                return -1;
        }
        g_markup_parse_context_end_parse(pcontext, &err);
        g_markup_parse_context_free(pcontext);

        /* make sure we have some elements in the hash table */
        if (g_hash_table_size(*hashtable) == 0) {
                dbg("Unknown XML parse error, the hash table is empty");
                return -1;
        }

	return 0;
}


/**********************************************************************
 * xml2event_hash_free: free the hash table and the internal
 * memory used by the hash value
 *
 * return value: 0 = hash table freed successfully
 *               -1 = An error occured, see the debug messages
 **********************************************************************/

int xml2event_hash_free(GHashTable **hashtable)
{
        if (*hashtable == NULL) {
                return 0;
        }
        g_hash_table_foreach(*hashtable, free_hash_data, NULL);
	g_hash_table_destroy(*hashtable);
        *hashtable = NULL;

	return 0;
}


static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
	Xml2EventInfoT *xmlinfo;

        g_free(key);
        xmlinfo = (Xml2EventInfoT *)value;
        g_free(xmlinfo->event);
        g_free(value);
}


static void event_start_element(GMarkupParseContext *context,
                                const gchar         *element_name,
                                const gchar        **attribute_names,
                                const gchar        **attribute_values,
                                gpointer             user_data,
                                GError             **error)
{
	gchar *key = NULL;
	Xml2EventInfoT *xmlinfo;
        GHashTable **hashtable = (GHashTable **)user_data;
        int i = 0;
        gint line, pos;

        /* Note: Error messages are passed back to the caller via the GError
         * mechanism. There is no need for dbg calls in this function.
         */

        /* ignore all xml elements except the event tag */
        if (strcmp(element_name, "event") != 0) {
                /* this is not an error condition! */
                return;
        }

        /* malloc memory for hash value */
        xmlinfo = g_malloc0(sizeof(Xml2EventInfoT));
        if (!xmlinfo) {
                g_set_error(error, G_MARKUP_ERROR,G_MARKUP_ERROR_PARSE,
                            "Cannot allocate memory for hash value");
                return;
        }

        /* fetch element attributes and set the hash key and value */
        while (attribute_names[i] != NULL) {
                if (strcmp(attribute_names[i], "name") == 0) {
                        /* we don't use this attribute so ignore it */
                }
                else if (strcmp(attribute_names[i], "type") == 0) {
                        /* we don't use this attribute so ignore it */
                }
                else if (strcmp(attribute_names[i], "msg") == 0) {
                        key = g_strdup(attribute_values[i]);
                        if (key == NULL) {
                                g_set_error(error, G_MARKUP_ERROR,
                                            G_MARKUP_ERROR_PARSE,
                                            "Cannot allocate memory for hash key - %s",
                                            attribute_values[i]);
                                return;
                        }
                }
                else if (strcmp(attribute_names[i], "hex") == 0) {
                        xmlinfo->event = g_strdup(attribute_values[i]);
                        if (xmlinfo->event == NULL) {
                                g_set_error(error, G_MARKUP_ERROR,
                                            G_MARKUP_ERROR_PARSE,
                                            "Cannot allocate memory for hash value");
                                return;
                        }
                }
                else if (strcmp(attribute_names[i], "severity") == 0) {
                        if (strcmp(attribute_values[i], "SAHPI_CRITICAL") == 0) {
                                xmlinfo->event_sev = SAHPI_CRITICAL;
                        }
                        else if (strcmp(attribute_values[i], "SAHPI_MAJOR") == 0) {
                                xmlinfo->event_sev = SAHPI_MAJOR;
                        }
                        else if (strcmp(attribute_values[i], "SAHPI_MINOR") == 0) {
                                xmlinfo->event_sev = SAHPI_MINOR;
                        }
                        else if (strcmp(attribute_values[i], "SAHPI_INFORMATIONAL") == 0) {
                                xmlinfo->event_sev = SAHPI_INFORMATIONAL;
                        }
                        else {
                                g_markup_parse_context_get_position(context,
                                                                    &line, &pos);
                                g_set_error(error, G_MARKUP_ERROR,
                                            G_MARKUP_ERROR_INVALID_CONTENT,
                                            "Bad severity attribute value on XML event element line %d",
                                            line);
                                return;
                        }
                }
                else if (strcmp(attribute_names[i], "override") == 0) {
                        xmlinfo->event_ovr |= NO_OVR;
                        /* the following are NOT mutually exclusive! */
                        if (strstr(attribute_values[i], "OVR_SEV") != NULL) {
                                xmlinfo->event_ovr |= OVR_SEV;
                        }
                        if (strstr(attribute_values[i], "OVR_RID") != NULL) {
                                xmlinfo->event_ovr |= OVR_RID;
                        }
                        if (strstr(attribute_values[i], "OVR_EXP") != NULL) {
                                xmlinfo->event_ovr |= OVR_EXP;
                        }
                        /* ignore any other values */
                }
                else if (strcmp(attribute_names[i], "dup") == 0) {
                        xmlinfo->event_dup = (short)atoi(attribute_values[i]);
                }
                else {
                        g_markup_parse_context_get_position(context,
                                                            &line, &pos);
                        g_set_error(error, G_MARKUP_ERROR,
                                    G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                                    "Bad attribute name on XML event element line %d",
                                    line);
                        return;
                }
                i++;
        }
        if (key == NULL) {
                g_set_error(error, G_MARKUP_ERROR,
                            G_MARKUP_ERROR_INVALID_CONTENT,
                            "No key attribute set from XML event element");
                return;
        }

        g_hash_table_insert(*hashtable, key, xmlinfo);
        return;
}

