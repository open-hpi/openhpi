/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
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
 *     W. David Ashley <dashley@us.ibm.com>
 *     David Judkoivcs <djudkovi@us.ibm.com>
 *     Renier Morales <renier@openhpi.org>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <glib.h>

#include <oHpi.h>

#include <oh_error.h>
#include <strmsock.h>

#include "event.h"
#include "init.h"
#include "server.h"
#include "threaded.h"


/*--------------------------------------------------------------------*/
/* Globals                                                            */
/*--------------------------------------------------------------------*/

static gchar    *cfgfile        = NULL;
static gboolean verbose_flag    = FALSE;
static gchar    *bindaddr       = NULL;
static gint     port            = OPENHPI_DEFAULT_DAEMON_PORT;
static gchar    *portstr        = NULL;
static gchar    *optpidfile     = NULL;
static gint     sock_timeout    = 0;  // unlimited -- TODO: unlimited or 30 minutes default? was unsigned int
static gint     max_threads     = -1; // unlimited
static gboolean runasforeground = FALSE;
static bool daemonized   = false;
static gboolean enableIPv4      = FALSE;
static gboolean enableIPv6      = FALSE;

#ifdef HAVE_ENCRYPT
static gboolean g_decrypt       = FALSE;
static gchar * gcrypt_str         = NULL;
#endif

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

  { "pidfile",   'f', 0, G_OPTION_ARG_FILENAME, &optpidfile,    "Overrides the default path/name for the daemon.\n"
                                    "                            pid file. The option is optional.",                "pidfile" },
  { "timeout",   's', 0, G_OPTION_ARG_INT,      &sock_timeout,  "Overrides the default socket read timeout of 30\n"
                                    "                            minutes. The option is optional.",                 "seconds" },
  { "threads",   't', 0, G_OPTION_ARG_INT,      &max_threads,   "Sets the maximum number of connection threads.\n"
                                    "                            The default is umlimited. The option is optional.","threads" },
  { "nondaemon", 'n', 0, G_OPTION_ARG_NONE,   &runasforeground, "Forces the code to run as a foreground process\n"
                                    "                            and NOT as a daemon. The default is to run as\n"
                                    "                            a daemon. The option is optional.",                 NULL },
#ifdef HAVE_ENCRYPT
  { "decrypt",   'd', 0, G_OPTION_ARG_NONE,   &g_decrypt,       "Config file encrypted with hpicrypt. Decrypt and read",  NULL },
#endif
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
    printf("  ./openhpid -c /etc/openhpi/openhpi.conf\n\n");

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
    printf("  -f, --pidfile=pidfile     Overrides the default path/name for the daemon.\n");
    printf("                            pid file. The option is optional.\n");
    printf("  -s, --timeout=seconds     Overrides the default socket read timeout of 30\n");
    printf("                            minutes. The option is optional.\n");
    printf("  -t, --threads=threads     Sets the maximum number of connection threads.\n");
    printf("                            The default is umlimited. The option is optional.\n");
    printf("  -n, --nondaemon           Forces the code to run as a foreground process\n");
    printf("                            and NOT as a daemon. The default is to run as\n");
    printf("                            a daemon. The option is optional.\n");
#ifdef HAVE_ENCRYPT
    printf("  -d, --decrypt             Configuration file is encrypted. Decrypt and read.\n");
#endif
    printf("  -6, --ipv6                The daemon will try to bind IPv6 socket.\n");
    printf("  -4, --ipv4                The daemon will try to bind IPv4 socket (default).\n");
    printf("                            IPv6 option takes precedence over IPv4 option.\n\n");
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

static int get_syslog_level(GLogLevelFlags log_level)
{
    if (log_level & G_LOG_LEVEL_ERROR) {
        return LOG_ERR;
    } else if (log_level & G_LOG_LEVEL_CRITICAL) {
        return LOG_CRIT;
    } else if (log_level & G_LOG_LEVEL_WARNING) {
        return LOG_WARNING;
    } else if (log_level & G_LOG_LEVEL_MESSAGE) {
        return LOG_NOTICE;
    } else if (log_level & G_LOG_LEVEL_INFO) {
        return LOG_INFO;
    } else if (log_level & G_LOG_LEVEL_DEBUG) {
        return LOG_DEBUG;
    }
    return LOG_INFO;
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
        syslog(LOG_DAEMON | get_syslog_level(log_level),
               "%s: %s\n",
               log_domain,
               message);
    }
}


/*--------------------------------------------------------------------*/
/* PID File Unility Functions                                         */
/*--------------------------------------------------------------------*/
bool check_pidfile(const char *pidfile)
{
    // TODO add more checks here
    if (!pidfile) {
        return false;
    }

    int fd = open(pidfile, O_RDONLY);
    if (fd >= 0) {
        char buf[32];
        memset(buf, 0, sizeof(buf));
        ssize_t len = read(fd, buf, sizeof(buf) - 1);
        if (len < 0) {
            CRIT("Cannot read from PID file.");
            return false;
        }
        close(fd);
        int pid = atoi(buf);
        if ((pid > 0) && (pid == getpid() || (kill(pid, 0) < 0))) {
            unlink(pidfile);
        } else {
            CRIT("There is another active OpenHPI daemon.");
            return false;
        }
    }

    return true;
}

bool update_pidfile(const char *pidfile)
{
    // TODO add more checks here
    if (!pidfile) {
        return false;
    }

    int fd = open(pidfile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP);
    if (fd < 0) {
        CRIT("Cannot open PID file.");
        return false;
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", (int)getpid());
    write(fd, buf, strlen(buf));
    close(fd);

    return true;
}

/*--------------------------------------------------------------------*/
/* Function: daemonize                                                */
/*--------------------------------------------------------------------*/

static bool daemonize(const char *pidfile)
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        return false;
    }

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid > 0) {
        exit(0);
    }
    // Become the session leader
    setsid();
    // Second fork to make sure we are detached
    // from any controlling terminal.
    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid > 0) {
        exit(0);
    }

    daemonized = true;

    update_pidfile(pidfile);

    //chdir("/");

#ifndef _WIN32
    mode_t prev_umask = umask(022); // Reset default file permissions
    if ( prev_umask != 022 ) {
        WARN("Using umask 0%o instead of 022(default)",prev_umask);
        umask(prev_umask);
    }
#endif

    // Close unneeded inherited file descriptors
    // Keep stdout and stderr open if they already are.
#ifdef NR_OPEN
    for (int i = 3; i < NR_OPEN; i++) {
#else
    for (int i = 3; i < 1024; i++) {
#endif
        close(i);
    }

    return true;
}

static void sig_handler( int signum )
{
    // Handles SIGTERM and SIGINT
    oh_post_quit_event();
    oh_server_request_stop();
    oh_signal_service();
}


/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int ipvflags;
    const char *pidfile = "/var/run/openhpid.pid";
    GError *error = NULL;
    GOptionContext *context;

    g_log_set_default_handler(log_handler, 0);


    /* get the command line options */
    context = g_option_context_new ("- HPI instance to which multiple clients can connect.\n\n"
                      "A typical invocation might be\n"
                      "  ./openhpid -c /etc/openhpi/openhpi.conf\n");
    g_option_context_add_main_entries (context, daemon_options, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
           CRIT ("option parsing failed: %s",error->message);
           display_help();
           exit(-1);
    }

    if (cfgfile) {
        setenv("OPENHPI_CONF", cfgfile, 1);
    } else {
        cfgfile = getenv("OPENHPI_CONF");
    }
    if (bindaddr) {
        setenv("OPENHPI_DAEMON_BIND_ADDRESS", bindaddr, 1);
    } else {
        bindaddr = getenv("OPENHPI_DAEMON_BIND_ADDRESS");
    }
    if (portstr) {
        setenv("OPENHPI_DAEMON_PORT", portstr, 1);
    } else {
        portstr = getenv("OPENHPI_DAEMON_PORT");
    }
    if (portstr) {
        port = atoi(portstr);
    }

#ifdef HAVE_ENCRYPT
    if (g_decrypt) {
        setenv("OPENHPI_GCRYPT", "1", 1);
    } else {
         gcrypt_str = getenv("OPENHPI_GCRYPT");
    }
#endif

    ipvflags = ( enableIPv6 == TRUE ) ? FlagIPv6 : FlagIPv4;
    if (optpidfile) {
        pidfile = g_strdup(optpidfile);
    }

    // see if any invalid parameters are given
    if (sock_timeout<0) {
        CRIT("Socket timeout value must be positive. Exiting.");
        display_help();
    }

    // see if we have a valid configuration file
    if ((!cfgfile) || (!g_file_test(cfgfile, G_FILE_TEST_EXISTS))) {
        CRIT("Cannot find configuration file %s. Exiting.",cfgfile);
        display_help();
        exit(-1);
    }

    // see if we are already running
    if (!check_pidfile(pidfile)) {
        CRIT("PID file check failed. Exiting.");
        display_help();
        exit(1);
    }
    if (!update_pidfile(pidfile)) {
        CRIT("Cannot update PID file. Exiting.");
        display_help();
        exit(1);
    }

    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        CRIT("Cannot set SIGTERM handler. Exiting.");
        exit(1);
    }
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        CRIT("Cannot set SIGINT handler. Exiting.");
        exit(1);
    }

    if (!runasforeground) {
        if (!daemonize(pidfile)) {
            exit(8);
        }
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

