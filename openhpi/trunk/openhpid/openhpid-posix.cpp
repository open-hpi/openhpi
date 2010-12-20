/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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
#include <unistd.h>

#include <glib.h>

#include <oHpi.h>

#include <oh_error.h>

#include "init.h"
#include "server.h"


static int verbose_flag = 0;


/*--------------------------------------------------------------------*/
/* Function: display_help                                             */
/*--------------------------------------------------------------------*/

void display_help(void)
{
    printf("Help for openhpid:\n\n");
    printf("   openhpid -c conf_file [-v] [-p port] [-f pidfile]\n\n");
    printf("   -c conf_file  Sets path/name of the configuration file.\n");
    printf("                 This option is required unless the environment\n");
    printf("                 variable OPENHPI_CONF has been set to a valid\n");
    printf("                 configuration file.\n");
    printf("   -v            This option causes the daemon to display verbose\n");
    printf("                 messages. This option is optional.\n");
    printf("   -p port       Overrides the default listening port (%d) of\n",
           OPENHPI_DEFAULT_DAEMON_PORT);
    printf("                 the daemon. The option is optional.\n");
    printf("   -f pidfile    Overrides the default path/name for the daemon.\n");
    printf("                 pid file. The option is optional.\n");
    printf("   -s seconds    Overrides the default socket read timeout of 30\n");
    printf("                 minutes. The option is optional.\n");
    printf("   -t threads    Sets the maximum number of connection threads.\n");
    printf("                 The default is umlimited. The option is optional.\n");
    printf("   -n            Forces the code to run as a foreground process\n");
    printf("                 and NOT as a daemon. The default is to run as\n");
    printf("                 a daemon. The option is optional.\n\n");
    printf("A typical invocation might be\n\n");
    printf("   ./openhpid -c /etc/openhpi/openhpi.conf\n\n");
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
            err("Error: Cannot read from PID file.\n");
            return false;
        }
        close(fd);
        int pid = atoi(buf);
        if ((pid > 0) && (pid == getpid() || (kill(pid, 0) < 0))) {
            unlink(pidfile);
        } else {
            err("Error: There is another active OpenHPI daemon.\n");
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
        err("Error: Cannot open PID file.\n");
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

    update_pidfile(pidfile);

    //chdir("/");
    umask(0); // Reset default file permissions

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

/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    struct option options[] = {
        {"verbose",   no_argument,       0, 'v'},
        {"nondaemon", no_argument,       0, 'n'},
        {"cfg",       required_argument, 0, 'c'},
        {"port",      required_argument, 0, 'p'},
        {"pidfile",   required_argument, 0, 'f'},
        {"timeout",   required_argument, 0, 's'},
        {"threads",   required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    char *cfgfile             = 0;
    int  port                 = OPENHPI_DEFAULT_DAEMON_PORT;
    const char *pidfile       = "/var/run/openhpid.pid";
    unsigned int sock_timeout = 0;  // unlimited
    int  max_threads          = -1; // unlimited
    bool runasdaemon          = true;

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
                setenv("OPENHPI_CONF", optarg, 1);
                cfgfile = g_strdup(optarg);
                break;
            case 'p':
                setenv("OPENHPI_DAEMON_PORT", optarg, 1);
                port = atoi(optarg);
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case 'f':
                pidfile = g_strdup(optarg);
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
                runasdaemon = false;
                break;
            case '?':
            default:
                /* they obviously need it */
                display_help();
                exit(-1);
        }
    }
    if (optind < argc) {
        fprintf(stderr, "Error: Unknown command line option specified. Exiting.\n");
        display_help();
        exit(-1);
    }

    // see if we have a valid configuration file
    if ((!cfgfile) || (!g_file_test(cfgfile, G_FILE_TEST_EXISTS))) {
        fprintf(stderr, "Error: Cannot find configuration file. Exiting.\n");
        display_help();
        exit(-1);
    }

    // see if we are already running
    if (!check_pidfile(pidfile)) {
        fprintf(stderr, "Error: PID file check failed. Exiting.\n");
        display_help();
        exit(1);
    }
    if (!update_pidfile(pidfile)) {
        fprintf(stderr, "Error: Cannot update PID file. Exiting.\n");
        display_help();
        exit(1);
    }

    if (runasdaemon) {
        if (!daemonize(pidfile)) {
            exit(8);
        }
    }

    if (oh_init()) { // Initialize OpenHPI
        err("There was an error initializing OpenHPI. Exiting.\n");
        return 8;
    }

    // announce ourselves
    dbg("%s started.\n", argv[0]);
    dbg("OPENHPI_CONF = %s.\n", cfgfile);
    dbg("OPENHPI_DAEMON_PORT = %u.\n\n", port);

    bool rc = oh_server_run(port, sock_timeout, max_threads);
    if (!rc) {
        return 9;
    }

    return 0;
}

