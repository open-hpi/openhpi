/*      -*- linux-c -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include <oHpi.h>

#include <oh_error.h>
#include <strmsock.h>

#include "init.h"
#include "server.h"


/*--------------------------------------------------------------------*/
/* Globals                                                            */
/*--------------------------------------------------------------------*/
static gchar    *cfgfile        = NULL;
static gboolean verbose_flag    = FALSE;
static gchar    *bindaddr       = NULL;
static gint     port            = OPENHPI_DEFAULT_DAEMON_PORT;
static gchar    *portstr        = NULL;
static gint     sock_timeout    = 0;  // unlimited -- TODO: unlimited or 30 minutes default? was unsigned int
static gint     max_threads     = -1; // unlimited
//static gboolean runasforeground = FALSE;     // TODO daemonization
static bool daemonized   = false;            // TODO daemonization
static gboolean enableIPv4      = FALSE;
static gboolean enableIPv6      = FALSE;

static GOptionEntry daemon_options[] =
{
  { "cfg",       'c', 0, G_OPTION_ARG_FILENAME, &cfgfile,       "Sets path/name of the configuration file.\n"
                                    "                            This option is required unless the environment\n"
                                    "                            variable OPENHPI_CONF has been set to a valid\n"
                                    "                            configuration file.",                              "conf_file" },
  { "verbose",   'v', 0, G_OPTION_ARG_NONE,     &verbose_flag,  "This option causes the daemon to display verbose\n"
                                    "                            messages. This option is optional.",                NULL },
  { "bind",      'b', 0, G_OPTION_ARG_STRING,   &bindaddr,       "Bind address for the daemon socket.\n"
                                    "                            Also bind address can be specified with\n"
                                    "                            OPENHPI_DAEMON_BIND_ADDRESS environment variable.\n"
                                    "                            No bind address is used by default.",              "bind_address" },
  { "port",      'p', 0, G_OPTION_ARG_STRING,   &portstr,       "Overrides the default listening port (4743) of\n"
                                    "                            the daemon. The option is optional.",              "port" },
  { "timeout",   's', 0, G_OPTION_ARG_INT,      &sock_timeout,  "Overrides the default socket read timeout of 30\n"
                                    "                            minutes. The option is optional.",                 "seconds" },
  { "threads",   't', 0, G_OPTION_ARG_INT,      &max_threads,   "Sets the maximum number of connection threads.\n"
                                    "                            The default is umlimited. The option is optional.","threads" },
//  { "nondaemon", 'n', 0, G_OPTION_ARG_NONE,   &runasforeground, "Forces the code to run as a foreground process\n"
//                                    "                            and NOT as a daemon. The default is to run as\n"
//                                    "                            a daemon. The option is optional.",                 NULL },
  { "ipv6",      '6', 0, G_OPTION_ARG_NONE,   &enableIPv6,      "The daemon will try to bind IPv6 socket.",          NULL },
  { "ipv4",      '4', 0, G_OPTION_ARG_NONE,   &enableIPv4,      "The daemon will try to bind IPv4 socket (default).\n"
                                    "                            IPv6 option takes precedence over IPv4 option.",    NULL },

  { NULL }
};



/*--------------------------------------------------------------------*/
/* Function: display_help                                             */
/*--------------------------------------------------------------------*/

void display_help(void)
{
    printf("Usage:\n");
    printf("  openhpid [OPTION...] - HPI instance to which multiple clients can connect.\n\n");

    printf("A typical invocation might be\n");
    printf("   openhpid.exe -c C:\\openhpi.conf\n\n");

    printf("Help Options:\n");
    printf("  -h, --help                Show help options\n\n");

    printf("Application Options:\n");
    printf("  -c, --cfg=conf_file       Sets path/name of the configuration file.\n");
    printf("                            This option is required unless the environment\n");
    printf("                            variable OPENHPI_CONF has been set to a valid\n");
    printf("                            configuration file.\n");
    printf("  -v, --verbose             This option causes the daemon to display verbose\n");
    printf("                            messages. This option is optional.\n");
    printf("  -b, --bind                Sets bind address for the daemon socket.\n");
    printf("                            Also bind address can be specified with\n");
    printf("                            OPENHPI_DAEMON_BIND_ADDRESS environment variable.\n");
    printf("                            No bind address is used by default.\n");
    printf("  -p, --port=port           Overrides the default listening port (4743) of\n");
    printf("                            the daemon. The option is optional.\n");
    printf("  -s, --timeout=seconds     Overrides the default socket read timeout of 30\n");
    printf("                            minutes. The option is optional.\n");
    printf("  -t, --threads=threads     Sets the maximum number of connection threads.\n");
    printf("                            The default is umlimited. The option is optional.\n");
//    printf("  -n, --nondaemon           Forces the code to run as a foreground process\n");
//    printf("                            and NOT as a daemon. The default is to run as\n");
//    printf("                            a daemon. The option is optional.\n");
    printf("  -6, --ipv6                The daemon will try to bind IPv6 socket.\n");
    printf("  -4, --ipv4                The daemon will try to bind IPv4 socket (default).\n");
    printf("                            IPv6 option takes precedence over IPv4 option.\n\n");
}


/*--------------------------------------------------------------------*/
/* Function: setenv                                                   */
/*--------------------------------------------------------------------*/

static int setenv(const char * var, const char * val)
{
    static const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    snprintf(buf, BUFSIZE, "%s=%s", var, val);
    return _putenv(buf);
}


/*--------------------------------------------------------------------*/
/* Logging Utility Functions                                          */
/*--------------------------------------------------------------------*/

static const char * get_log_level_name(GLogLevelFlags log_level)
{
    if (log_level & G_LOG_LEVEL_ERROR) {
        return "ERR";
    } else if (log_level & G_LOG_LEVEL_CRITICAL) {
        return "CRIT";
    } else if (log_level & G_LOG_LEVEL_WARNING) {
        return "WARN";
    } else if (log_level & G_LOG_LEVEL_MESSAGE) {
        return "MSG";
    } else if (log_level & G_LOG_LEVEL_INFO) {
        return "INFO";
    } else if (log_level & G_LOG_LEVEL_DEBUG) {
        return "DBG";
    }
    return "???";
}

void log_handler(const gchar *log_domain,
                 GLogLevelFlags log_level,
                 const gchar *message,
                 gpointer /*user_data */)
{
    if ((!verbose_flag) && ((log_level & G_LOG_LEVEL_CRITICAL) == 0) ) {
        return;
    }
    if (!daemonized) {
        printf("%s: %s: %s\n",
               log_domain,
               get_log_level_name(log_level),
               message);
    } else {
        // TODO implement
    }
}


/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int ipvflags;
    GError *error = NULL;
    GOptionContext *context;

// TODO daemonization
    g_log_set_default_handler(log_handler, 0);

    /* get the command line options */
    context = g_option_context_new ("- HPI instance to which multiple clients can connect.\n\n"
                      "A typical invocation might be\n"
                      "  openhpid.exe -c C:\\openhpi.conf\n");
    g_option_context_add_main_entries (context, daemon_options, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
           CRIT ("option parsing failed: %s",error->message);
           display_help();
           exit(-1);
    }

    if (cfgfile) {
        setenv("OPENHPI_CONF", cfgfile);
    } else {
        cfgfile = getenv("OPENHPI_CONF");
    }
    if (bindaddr) {
        setenv("OPENHPI_DAEMON_BIND_ADDRESS", bindaddr);
    } else {
        bindaddr = getenv("OPENHPI_DAEMON_BIND_ADDRESS");
    }
    if (portstr) {
        setenv("OPENHPI_DAEMON_PORT", portstr);
    } else {
        portstr = getenv("OPENHPI_DAEMON_PORT");
    }
    if (portstr) {
        port = atoi(portstr);
    }
    ipvflags = ( enableIPv6 == TRUE ) ? FlagIPv6 : FlagIPv4;

    // see if any invalid parameters are given
    if (sock_timeout<0) {
        CRIT("Socket timeout value must be positive. Exiting.");
        display_help();
    }

    // announce ourselves
    INFO("Starting OpenHPI %s.", VERSION);
    if (cfgfile) {
        INFO("OPENHPI_CONF = %s.", cfgfile);
    }
    if (bindaddr) {
        INFO("OPENHPI_DAEMON_BIND_ADDRESS = %s.", bindaddr);
    }
    INFO("OPENHPI_DAEMON_PORT = %u.", port);
    INFO("Enabled IP versions:%s%s.",
         (ipvflags & FlagIPv4) ? " IPv4" : "",
         (ipvflags & FlagIPv6) ? " IPv6" : "");
    INFO("Max threads: %d.", max_threads);
    INFO("Socket timeout(sec): %d.", sock_timeout);

    if (oh_init()) { // Initialize OpenHPI
        CRIT("There was an error initializing OpenHPI. Exiting.");
        return 8;
    }

    bool rc = oh_server_run(ipvflags, bindaddr, port, sock_timeout, max_threads);
    if (!rc) {
        return 9;
    }

    if (oh_finit()) {
        CRIT("There was an error finalizing OpenHPI. Exiting.");
        return 8;
    }

    return 0;
}

