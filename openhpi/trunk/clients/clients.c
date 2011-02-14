/* -*- linux-c -*-
 *
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 * (C) Copyright IBM Corp. 2007
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Shuah Khan <shuah.khan@hp.com>
 *	Renier Morales <renier@openhpi.org>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *    20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 *
 */

#include "oh_clients.h"

static gint     optdid                           = SAHPI_UNSPECIFIED_DOMAIN_ID;
static gboolean optdebug                         = FALSE;
static gboolean optverbose                       = FALSE;
static gchar    *optep        = NULL;
static gchar    *optdaemon    = NULL;    
static gchar    *optcfgfile   = NULL;    

static GOptionEntry domain_option[] =
{
  { "domain",       'D', 0, G_OPTION_ARG_INT,    &optdid,     "Select domain id nn",         "nn" },
  { NULL }
};

static GOptionEntry debug_option[] =
{
  { "debug",        'X', 0, G_OPTION_ARG_NONE,   &optdebug,   "Display debug messages",      NULL },
  { NULL }
};

static GOptionEntry verbose_option[] =
{
  { "verbose",      'V', 0, G_OPTION_ARG_NONE,   &optverbose, "Verbose mode",                NULL },
  { NULL }
};

static GOptionEntry entity_path_option[] =
{
  { "entity-path",  'E', 0, G_OPTION_ARG_STRING, &optep,      "Use entity path epath",       "\"epath\"" },
  { NULL }
};

// Preparation for option -N / --host (Features 2726603 & 3129972 & 3129967)
static GOptionEntry host_option[] =
{
  { "host",         'N', 0, G_OPTION_ARG_STRING, &optdaemon,  "Connect to daemon host:port", "\"host<:port>\"" },
  { NULL }
};

static GOptionEntry clientconf_option[] =
{
  { "cfgfile",      'C', 0, G_OPTION_ARG_FILENAME, &optcfgfile, "Use passed file as client configuration file", "\"file\"" },
  { NULL }
};


void oh_prog_version(const char *prog_name, const char *svn_rev_str)
{
	SaHpiUint32T ohpi_major = oHpiVersionGet() >> 48;
        SaHpiUint32T ohpi_minor = (oHpiVersionGet() << 16) >> 48;
        SaHpiUint32T ohpi_patch = (oHpiVersionGet() << 32) >> 48;
        SaHpiVersionT hpiver;
        char svn_rev[SAHPI_MAX_TEXT_BUFFER_LENGTH];

        strncpy(svn_rev, svn_rev_str, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        svn_rev[strlen(svn_rev_str)-2] = '\0';

        // Revision not printed here any more
        //printf("%s (rev %s) - This program came with OpenHPI %u.%u.%u\n",
        //       prog_name, svn_rev+11, ohpi_major, ohpi_minor, ohpi_patch);
        printf("%s - This program came with OpenHPI %u.%u.%u\n",
               prog_name, ohpi_major, ohpi_minor, ohpi_patch);
        hpiver = saHpiVersionGet();
        printf("SAF HPI Version %c.0%d.0%d\n\n",
               (hpiver >> 16) + ('A' - 1),
               (hpiver & 0x0000FF00) >> 8,
               hpiver & 0x000000FF);
}

gboolean ohc_option_parse(int *argc, char *argv[],  
                         GOptionContext     *context,
                         oHpiCommonOptionsT *common_options,
                         SaHpiUint8T        optionmask,
                         GError             *error)
{
   SaErrorT rv = SA_OK;

   if (!argc || !argv || !context || !common_options) {
      CRIT ("Internal error. Terminating.\n");
      return FALSE;
   }

   if (optionmask & OHC_DOMAIN_OPTION)
      g_option_context_add_main_entries (context, domain_option, NULL);
   if (optionmask & OHC_DEBUG_OPTION)
      g_option_context_add_main_entries (context, debug_option, NULL);
   if (optionmask & OHC_VERBOSE_OPTION)
      g_option_context_add_main_entries (context, verbose_option, NULL);
   if (optionmask & OHC_ENTITY_PATH_OPTION)
      g_option_context_add_main_entries (context, entity_path_option, NULL);
// Preparation for option -N / --host (Features 2726603 & 3129972 & 3129967)
// Preliminary implementation like it was in hpi_shell
   if (optionmask & OHC_HOST_OPTION)
      g_option_context_add_main_entries (context, host_option, NULL);
   if (optionmask & OHC_CLIENTCONF_OPTION)
      g_option_context_add_main_entries (context, clientconf_option, NULL);

   if (!g_option_context_parse (context, argc, &argv, &error)) {
           CRIT ("option parsing failed: %s\n",error->message);
           // CRIT ("\n%s", g_option_context_get_help (context, FALSE, NULL)); 
           // Needs glib-2.14 
           return FALSE;
   }

   common_options->withentitypath = (optep != NULL);
   common_options->withdaemonhost = (optdaemon != NULL);

   if (optdebug && optverbose) {
      DBG("Parsing of options completed. Common options:\n --debug --verbose");
      if (optdid >= 0)                    DBG(" --domain=%u ",optdid);
      if (common_options->withentitypath) DBG(" --entity-path=%s ",optep);
      if (common_options->withdaemonhost) DBG(" --daemon=%s ",optdaemon);
      DBG("\n");
   }

   /* prepare output */
   common_options->debug      = optdebug;
   common_options->verbose    = optverbose;
   common_options->domainid   = optdid;

   oh_init_ep(&common_options->entitypath); 
   if (common_options->withentitypath) {
      rv = oh_encode_entitypath(optep, &common_options->entitypath);
      if (error) {
         CRIT ("Invalid entity path: %s\n"
              "oh_encode_entitypath() returned %s \n",
              optep, oh_lookup_error(rv));
              return FALSE;
      }
      if (optdebug && optverbose) {
         DBG("Entity Path encoded successfully: ");
         oh_print_ep(&common_options->entitypath, 0);
         DBG("\n");
      }
   }

   oh_init_textbuffer(&common_options->daemonhost);
   if (common_options->withdaemonhost) {
      char *colon=strchr(optdaemon, ':');
      if (colon!=NULL) {
         *colon = '\0';
         common_options->daemonport = atoi(++colon);
         setenv("OPENHPI_DAEMON_PORT", colon, 1);  
      } else common_options->daemonport = OPENHPI_DEFAULT_DAEMON_PORT;
      setenv("OPENHPI_DAEMON_HOST", optdaemon, 1);  // copied from hpi_shell
      rv = oh_append_textbuffer(&common_options->daemonhost, optdaemon);
      if (optdebug && optverbose) {
         DBG("Daemon host:port scanned successfully: host=");
         oh_print_text(&common_options->daemonhost);
         DBG(" port=%u\n",common_options->daemonport);
      }
   }

   oh_init_textbuffer(&common_options->clientconf);
   if (optcfgfile) {
      rv = oh_append_textbuffer(&common_options->daemonhost, optcfgfile);
      setenv("OPENHPICLIENT_CONF", optcfgfile, 1);  // copied from hpi_shell
   }

   g_free (optep);
   g_free (optdaemon);
   g_free (optcfgfile);
   return TRUE;
}

SaErrorT ohc_session_open_by_option (
                      oHpiCommonOptionsT *opt,
                      SaHpiSessionIdT    *sessionid)
{
   // TODO correctly implement -N / -C option
   SaErrorT rv = SA_OK;

   if (opt->debug) {
      if (opt->domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) printf("saHpiSessionOpen\n");
      else printf("saHpiSessionOpen to domain %u\n",opt->domainid);
   }
   
   rv = saHpiSessionOpen(opt->domainid, sessionid, NULL);

   if (rv != SA_OK || opt->debug) {
      printf("saHpiSessionOpen returns %s\n",oh_lookup_error(rv));
      return rv;
   }

   if (opt->debug)
      printf("saHpiSessionOpen returns with SessionId %u\n", *sessionid);

   return SA_OK;
}

 /* end clients.c */
