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

#include "init.h"
#include "server.h"


static bool daemonized   = false;
static bool verbose_flag = false;


/*--------------------------------------------------------------------*/
/* Function: display_help                                             */
/*--------------------------------------------------------------------*/

void display_help(void)
{
    printf("Help for openhpid:\n\n");
    printf("   openhpid -c conf_file [-v] [-p port]\n\n");
    printf("   -c conf_file  Sets path/name of the configuration file.\n");
    printf("                 This option is required unless the environment\n");
    printf("                 variable OPENHPI_CONF has been set to a valid\n");
    printf("                 configuration file.\n");
    printf("   -v            This option causes the daemon to display verbose\n");
    printf("                 messages. This option is optional.\n");
    printf("   -p port       Overrides the default listening port (%d) of\n",
           OPENHPI_DEFAULT_DAEMON_PORT);
    printf("                 the daemon. The option is optional.\n");
    printf("   -s seconds    Overrides the default socket read timeout of 30\n");
    printf("                 minutes. The option is optional.\n");
    printf("   -t threads    Sets the maximum number of connection threads.\n");
    printf("                 The default is umlimited. The option is optional.\n");
    printf("   -n            Forces the code to run as a foreground process\n");
    printf("                 and NOT as a daemon. The default is to run as\n");
    printf("                 a daemon. The option is optional.\n\n");
    printf("A typical invocation might be\n\n");
    printf("   openhpid.exe -c C:\\openhpi.conf\n\n");
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
// TODO deamonization
    g_log_set_default_handler(log_handler, 0);

    struct option options[] = {
        {"verbose",   no_argument,       0, 'v'},
        {"nondaemon", no_argument,       0, 'n'},
        {"cfg",       required_argument, 0, 'c'},
        {"port",      required_argument, 0, 'p'},
        {"timeout",   required_argument, 0, 's'},
        {"threads",   required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    char *cfgfile             = 0;
    int  port                 = OPENHPI_DEFAULT_DAEMON_PORT;
    unsigned int sock_timeout = 0;  // unlimited
    int  max_threads          = -1; // unlimited
    //bool runasdaemon          = true;

    /* get the command line options */
    int c;
    while (1) {
        c = getopt_long(argc, argv, "nvc:p:f:s:t:", options, 0);
        /* detect when done scanning options */
        if (c == -1) {
            break;
        }
        switch (c) {
            case 0:
                /* no need to do anything here */
                break;
            case 'c':
                setenv("OPENHPI_CONF", optarg);
                cfgfile = g_strdup(optarg);
                break;
            case 'p':
                setenv("OPENHPI_DAEMON_PORT", optarg);
                port = atoi(optarg);
                break;
            case 'v':
                verbose_flag = true;
                break;
            case 's':
                sock_timeout = atoi(optarg);
                break;
            case 't':
                max_threads = atoi(optarg);
                if (max_threads <= 0) {
                    max_threads = -1;
                }
                break;
            case 'n':
                //runasdaemon = false;
                break;
            case '?':
            default:
                /* they obviously need it */
                display_help();
                exit(-1);
        }
    }
    if (optind < argc) {
        CRIT("Unknown command line option specified. Exiting.");
        display_help();
        exit(-1);
    }

    // see if we have a valid configuration file
    if ((!cfgfile) || (!g_file_test(cfgfile, G_FILE_TEST_EXISTS))) {
        CRIT("Cannot find configuration file. Exiting.");
        display_help();
        exit(-1);
    }

    if (oh_init()) { // Initialize OpenHPI
        CRIT("There was an error initializing OpenHPI. Exiting.");
        return 8;
    }

    // announce ourselves
    INFO("%s version %s started.", argv[0], VERSION);
    INFO("OPENHPI_CONF = %s.", cfgfile);
    INFO("OPENHPI_DAEMON_PORT = %u.", port);

    bool rc = oh_server_run(port, sock_timeout, max_threads);
    if (!rc) {
        return 9;
    }

    return 0;
}

