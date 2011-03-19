/*      -*- c++ -*-
 *
 * Copyright (c) 2011 by Pigeon Point Systems.
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *     09/02/2011  ulikleber  Use glib for option parsing and common
 *                            options for all clients
 *
 */

#define OH_SVN_REV "$Revision: 7281 $"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <oh_error.h>

#include "hpi.h"
#include "hpi_xml_writer.h"
#include "schema.h"
#include <oh_clients.h>

static gboolean f_indent    = FALSE;
static gboolean f_xsd       = FALSE;
static gboolean f_use_names = FALSE;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "indent", 'i', 0, G_OPTION_ARG_NONE, &f_indent,    "Use indentation",                                    NULL },
  { "text",   't', 0, G_OPTION_ARG_NONE, &f_use_names, "Use enum and flag text names instead of raw values", NULL },
  { "xsd",    's', 0, G_OPTION_ARG_NONE, &f_xsd,       "Show XML schema",                                    NULL },
 { NULL }
};


/***************************************************
 * Main
 ***************************************************/
int main( int argc, char * argv[] )
{
    int  indent_step = 0;

    GOptionContext *context;
	    
    context = g_option_context_new ("- Display system view in XML");
    /* Parsing options */
    static char usetext[]="- Display system view in XML.\n  "
                          OH_SVN_REV; 
    OHC_PREPARE_REVISION(usetext);

    context = g_option_context_new (usetext);
    g_option_context_add_main_entries (context, my_options, NULL);

    if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127?
                    - OHC_VERBOSE_OPTION     // no verbose mode implemented
                    - OHC_DEBUG_OPTION )) {      // no debug option implemented
                g_option_context_free (context);
		return 1;
	}
        g_option_context_free (context);
    if (f_indent) indent_step = 1;

    if ( f_xsd ) {
        for ( char * p = &schema_begin; p < &schema_end; ++p ) {
            fwrite( p, 1, 1, stdout );
        }
        return 0;
    }

    bool rc;

    cHpi hpi( copt );
    rc = hpi.Open();
    if ( !rc ) {
        return 1;
    }
    cHpiXmlWriter writer( indent_step, f_use_names );
    rc = hpi.Dump( writer );
    if ( !rc ) {
        CRIT( "Failed to produce XML" );
        return 1;
    }

    return 0;
}

