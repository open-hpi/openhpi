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

static GOptionEntry host_option[] =
{
  { "host",         'N', 0, G_OPTION_ARG_STRING, &optdaemon,  "Open session to the domain served by the daemon\n"
"                               at the specified URL (host:port)\n"
"                               This option overrides the OPENHPI_DAEMON_HOST and\n"
"                               OPENHPI_DAEMON_PORT environment variables.",                 "\"host<:port>\"" },
  { NULL }
};

static GOptionEntry clientconf_option[] =
{
  { "cfgfile",      'C', 0, G_OPTION_ARG_FILENAME, &optcfgfile, "Use passed file as client configuration file\n"
"                               This option overrides the OPENHPICLIENT_CONF\n"
"                               environment variable.",                                      "\"file\"" },
  { NULL }
};


#ifdef _WIN32
/*--------------------------------------------------------------------*/
/* Function: setenv                                                   */
/*--------------------------------------------------------------------*/
static int setenv(const char * var, const char * val, int overwrite)
{
    static const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    snprintf(buf, BUFSIZE, "%s=%s", var, val);
    return _putenv(buf);
}
#endif /* _WIN32 */

/*--------------------------------------------------------------------*/
/* ohc_domain_add_by_options                                          */
/*--------------------------------------------------------------------*/
static SaErrorT ohc_domain_add_by_options(oHpiCommonOptionsT *opt)
{
   SaErrorT rv = SA_OK;
   SaHpiDomainIdT did_N = SAHPI_UNSPECIFIED_DOMAIN_ID;
   SaHpiEntityPathT entity_root;
   const char *envhoststr, *envportstr;
   unsigned short envport;
   SaHpiTextBufferT envhost;
   SaHpiDomainIdT did_env = SAHPI_UNSPECIFIED_DOMAIN_ID;

   oh_init_ep(&entity_root);

   // Add a domain for the -N option
   if (opt->withdaemonhost) {// add a domain for that host
      if (opt->domainid == SAHPI_UNSPECIFIED_DOMAIN_ID) { // add with generated did  
         rv = oHpiDomainAdd ( &opt->daemonhost, opt->daemonport, &entity_root, &did_N );
         if (rv != SA_OK) {
            CRIT("Domain could not be created for given daemonhost");
            return rv;
         }
         if (opt->debug) DBG("Added domain %u for daemonhost", did_N);
         // use this did (thus -N option will override environment variables)
         opt->domainid = did_N;
      }
      else { // add with given domain id
         rv = oHpiDomainAddById ( opt->domainid, &opt->daemonhost, opt->daemonport, &entity_root );
         if (rv != SA_OK) {
            CRIT("Domain %u could not be created for given daemonhost", opt->domainid);
            return rv;
         }
         if (opt->debug) DBG("Added domain %u for daemonhost", opt->domainid);
      }
   }

   // check always for the environment variables.
   envhoststr = getenv("OPENHPI_DAEMON_HOST");
   if (envhoststr != NULL) { // add a domain for this host
      oh_init_textbuffer(&envhost);
      rv = oh_append_textbuffer(&envhost, envhoststr);
      envportstr = getenv("OPENHPI_DAEMON_PORT");
      if (envportstr == NULL) envport = OPENHPI_DEFAULT_DAEMON_PORT;
      else envport = atoi(envportstr);

      if (opt->withdaemonhost) { // we will add the domain with a new did, but
                                 // we will not use it now.
         rv = oHpiDomainAdd ( &envhost, envport, &entity_root, &did_env );
         if (rv != SA_OK) {
            CRIT("Domain could not be created for OPENHPI_DAEMON_HOST %s:%u",
                 envhoststr, envport);
            return rv;
         }
         if (opt->debug) DBG("Added domain %u for OPENHPI_DAEMON_HOST %s:%u", 
                             did_env, envhoststr, envport);
      } 
      else if (opt->domainid == SAHPI_UNSPECIFIED_DOMAIN_ID) { 
         // add with generated did  (here we don't have -N or -D options)
         rv = oHpiDomainAdd ( &envhost, envport, &entity_root, &did_env );
         if (rv != SA_OK) {
            CRIT("Domain could not be created for OPENHPI_DAEMON_HOST %s:%u",
                 envhoststr, envport);
            return rv;
         }
         if (opt->debug) DBG("Added domain %u for OPENHPI_DAEMON_HOST %s:%u", 
                             did_env, envhoststr, envport);
         opt->domainid = did_env;
      }

      else { // use the given did for OPENHPI_DAEMON_HOST
         rv = oHpiDomainAddById ( opt->domainid, &envhost, envport, &entity_root );
         if (rv != SA_OK) {
            CRIT("Domain %u could not be created for OPENHPI_DAEMON_HOST %s:%u", 
                 opt->domainid, envhoststr, envport);
            return rv;
         }
         if (opt->debug) DBG("Added domain %u for OPENHPI_DAEMON_HOST %s:%u", 
                 opt->domainid, envhoststr, envport);
      }
   }
   return SA_OK;
}

/*--------------------------------------------------------------------*/
/* oh_prog_version                                                    */
/*--------------------------------------------------------------------*/
void oh_prog_version(const char *prog_name)
{
	SaHpiUint32T ohpi_major = oHpiVersionGet() >> 48;
        SaHpiUint32T ohpi_minor = (oHpiVersionGet() << 16) >> 48;
        SaHpiUint32T ohpi_patch = (oHpiVersionGet() << 32) >> 48;
        SaHpiVersionT hpiver;

        printf("%s - This program came with OpenHPI %u.%u.%u\n",
               prog_name, ohpi_major, ohpi_minor, ohpi_patch);
        hpiver = saHpiVersionGet();
        printf("SAF HPI Version %c.0%d.0%d\n\n",
               (hpiver >> 16) + ('A' - 1),
               (hpiver & 0x0000FF00) >> 8,
               hpiver & 0x000000FF);
}

/*--------------------------------------------------------------------*/
/* ohc_option_parse                                                   */
/*--------------------------------------------------------------------*/
gboolean ohc_option_parse(int *argc, char *argv[],  
                         GOptionContext     *context,
                         oHpiCommonOptionsT *common_options,
                         SaHpiUint8T        optionmask )
{
   GError *error = NULL;
   SaErrorT rv = SA_OK;

   if (!argc || !argv || !context || !common_options) {
      CRIT ("Internal error. Terminating.");
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
   }

   /* prepare output */
   common_options->debug      = optdebug;
   common_options->verbose    = optverbose;
   common_options->domainid   = optdid;

   oh_init_ep(&common_options->entitypath); 
   if (common_options->withentitypath) {
      rv = oh_encode_entitypath(optep, &common_options->entitypath);
      if (optdebug) DBG("oh_encode_entitypath returned %s", oh_lookup_error(rv));
      if (rv) {
         CRIT ("oh_encode_entitypath() returned %s", oh_lookup_error(rv));
         CRIT ("Invalid entity path: %s", optep);
         return FALSE;
      }
      if (optdebug && optverbose) {
         DBG("Entity Path encoded successfully: ");
         oh_print_ep(&common_options->entitypath, 0);
      }
   }

   oh_init_textbuffer(&common_options->daemonhost);
   if (common_options->withdaemonhost) {
      char * hostbegin = NULL;
      char * hostend = NULL;
      char * portbegin = NULL;
      if ( *optdaemon == '[' ) {
         hostbegin = optdaemon + 1;
         hostend = strchr(hostbegin, ']');
         if (hostend==NULL) {
            CRIT("Ill-formed host: %s", optdaemon);
            return FALSE;
         }
      } else {
         hostbegin = optdaemon;
         hostend = strchr(hostbegin, ':');
      }
      if (hostend) {
         portbegin = strchr(hostend, ':' );
         if (portbegin) {
            ++portbegin;
         }
         *hostend = '\0';
      }
      rv = oh_append_textbuffer(&common_options->daemonhost, hostbegin);
      if (portbegin) {
         common_options->daemonport = atoi(portbegin);
      } else {
         common_options->daemonport = OPENHPI_DEFAULT_DAEMON_PORT;
      }
      if (optdebug && optverbose) {
         DBG("Daemon host:port scanned successfully: host=%s", hostbegin);
         DBG("Daemon host:port scanned successfully: port=%u", common_options->daemonport);
      }
   }

   oh_init_textbuffer(&common_options->clientconf);
   if (optcfgfile) {
      rv = oh_append_textbuffer(&common_options->clientconf, optcfgfile);
      setenv("OPENHPICLIENT_CONF", optcfgfile, 1);  // -C option overrides environment var
   }

   g_free (optep);
   g_free (optdaemon);
   g_free (optcfgfile);

   // add domains for -N option and environment variables
   ohc_domain_add_by_options ( common_options );

   return TRUE;
}

/*--------------------------------------------------------------------*/
/* ohc_session_open_by_option                                         */
/*--------------------------------------------------------------------*/
SaErrorT ohc_session_open_by_option (
                      oHpiCommonOptionsT *opt,
                      SaHpiSessionIdT    *sessionid)
{
   SaErrorT rv = SA_OK;

   if (opt->debug) {
      if (opt->domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) DBG("saHpiSessionOpen");
      else DBG("saHpiSessionOpen to domain %u",opt->domainid);
   }
   
   rv = saHpiSessionOpen(opt->domainid, sessionid, NULL);

   if (rv != SA_OK) {
      CRIT("saHpiSessionOpen returns %s",oh_lookup_error(rv));
      return rv;
   }

   if (opt->debug)
      DBG("saHpiSessionOpen returns with SessionId %u", *sessionid);

   return SA_OK;
}

 /* end clients.c */
