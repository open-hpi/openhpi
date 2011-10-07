/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
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
 *     Racing Guo <racing.guo@intel.com>
 *
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *  28.10.2010 - Anton Pak: fixed -c command line argument
 *  28.10.2010 - Anton Pak: added -D command line argument
 *  10.02.2011 - Ulrich Kleber: Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hpi_cmd.h"
#include <oh_clients.h>

#define OH_SVN_REV "$Revision$"

int	debug_flag = 0;
static gchar *f_cmdfile = NULL;
static gboolean f_events = FALSE;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "cmdfile", 'f', 0, G_OPTION_ARG_FILENAME, &f_cmdfile, "Execute command file",                        "filename"   },
  { "events",  'e', 0, G_OPTION_ARG_NONE,     &f_events,  "Show short events, discover after subscribe", NULL },
  { NULL }
};


int main(int argc, char **argv)
{
        GOptionContext *context;

        /* Print version strings */
        oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Allows a user to interactively "
                              "perform a number of HPI operations\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION // not applicable
                    - OHC_VERBOSE_OPTION )) {    // no verbose mode implemented
                g_option_context_free (context);
		exit(1);
	}
        if (copt.debug) debug_flag = 1;
        g_option_context_free (context);
        if (f_cmdfile) {
                open_file ( f_cmdfile );
                g_free ( f_cmdfile );
        }

	domainlist = (GSList *)NULL;
	if (open_session(copt.domainid, f_events) == -1) 
                    //TODO For complete implementation of -N option, need to call 
                    //TODO ohAddDomain to get the domain Id.
                                                 
		return(1);
	cmd_shell();
	close_session();
	return 0;
}

ret_code_t ask_entity(SaHpiEntityPathT *ret)
{
	term_def_t	*term;
	int	res;
    SaErrorT rv;
    char buf[256];
    const char * epstr;

	term = get_next_term();
	if (term == NULL) {
        SaHpiEntityPathT root;
        root.Entry[0].EntityType = SAHPI_ENT_ROOT;
        root.Entry[0].EntityLocation = 0;

		if (read_file) return(HPI_SHELL_PARM_ERROR);
		rv = show_entity_tree(Domain, &root, 0, ui_print);
		if (rv != SA_OK) {
			printf("NO entities!\n");
			return(HPI_SHELL_CMD_ERROR);
		};
		res = get_string_param("Entity Path ==> ", buf, sizeof(buf));
		if (res != 0) {
            printf("Invalid entity path");
            return(HPI_SHELL_PARM_ERROR);
        }
        epstr = buf;
	} else {
        epstr = term->term;
	};

    rv = oh_encode_entitypath(epstr, ret); 
    if ( rv != SA_OK ) {
        printf("Invalid entity path");
        return(HPI_SHELL_PARM_ERROR);
    }

	return(HPI_SHELL_OK);
}

ret_code_t ask_rpt(SaHpiResourceIdT *ret)
{
	term_def_t	*term;
	int		i, res;

	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_PARM_ERROR);
		i = show_rpt_list(Domain, SHOW_ALL_RPT, 0, SHORT_LSRES, ui_print);
		if (i == 0) {
			printf("NO rpts!\n");
			return(HPI_SHELL_CMD_ERROR);
		};
		i = get_int_param("RPT ID ==> ", &res);
		if (i == 1) *ret = (SaHpiResourceIdT)res;
		else return(HPI_SHELL_PARM_ERROR);
	} else {
		*ret = (SaHpiResourceIdT)atoi(term->term);
	};
	return(HPI_SHELL_OK);
}

ret_code_t ask_rdr(SaHpiResourceIdT rptid, SaHpiRdrTypeT type, SaHpiInstrumentIdT *ret)
{
	term_def_t	*term;
	int		i, res;
	char            buf[64];

	strncpy(buf, oh_lookup_rdrtype(type), 64);
	buf[strlen(buf)-4] = '\0';
	strncat(buf, " NUM ==> ", 64-strlen(buf));
	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_CMD_ERROR);
		i = show_rdr_list(Domain, rptid, type, ui_print);
		if (i == 0) {
			printf("No rdrs for rpt: %d\n", rptid);
			return(HPI_SHELL_CMD_ERROR);
		};
		i = get_int_param(buf, &res);
		if (i != 1) return(HPI_SHELL_PARM_ERROR);
		*ret = (SaHpiInstrumentIdT)res;
	} else {
		*ret = (SaHpiInstrumentIdT)atoi(term->term);
	};
	return(HPI_SHELL_OK);
}

ret_code_t open_file(char *path)
{
	if (add_input_file(path) != 0) {
		printf("Can not run file: %s\n", path);
		return(HPI_SHELL_PARM_ERROR);
	};
	read_file = 1;
	read_stdin = 0;
	return(HPI_SHELL_OK);
}
