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
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>

#include "strmsock.h"
#include "openhpi.h"


/*--------------------------------------------------------------------*/
/* Local definitions                                                  */
/*--------------------------------------------------------------------*/

extern "C"
{

static bool morph2daemon(bool runasdaemon);
static void service_thread(gpointer data, gpointer user_data);

}

static bool stop_server = FALSE;



/*--------------------------------------------------------------------*/
/* Function: main                                                         */
/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
	GThreadPool *thrdpool;

// become a daemon
	if (!morph2daemon(FALSE)) {	// this is an error condition
		exit(8);
	}

// create the thread pool
	g_thread_init(NULL);
	thrdpool = g_thread_pool_new(service_thread, NULL, -1, FALSE, NULL);

// create the server socket
	psstrmsock servinst = new sstrmsock;
	if (servinst->Create(55566)) {
		printf("Error creating server socket.\n");
		g_thread_pool_free(thrdpool, FALSE, TRUE);
                	delete servinst;
		return 8;
	}

// wait for a connection and then service the connection
	while (TRUE) {
		if (stop_server) {
			break;
		}
		printf("Waiting on connection.\n");
		if (servinst->Accept()) {
			printf("Error accepting server socket.\n");
			break;
		}
		printf("Spawning thread to handle connection.\n");
		psstrmsock thrdinst = new sstrmsock(*servinst);
		g_thread_pool_push(thrdpool, (gpointer)thrdinst, NULL);
	}

	servinst->CloseSrv();
	printf("Server socket closed.\n");

// ensure all threads are complete
	g_thread_pool_free(thrdpool, FALSE, TRUE);

	delete servinst;
	return 0;
}


/*--------------------------------------------------------------------*/
/* Function: morph2daemon                                       */
/*--------------------------------------------------------------------*/

static bool morph2daemon(bool runasdaemon)
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		return false;
	}

	if (runasdaemon) {
		pid_t pid = fork();
		if (pid < 0) {
			return false;
		}
// parent process
		if (pid != 0) {
			exit( 0 );
		}

// become the session leader
		setsid();
// second fork to become a real daemon
		pid = fork();
		if (pid < 0) {
			return false;
		}
// parent process
		if (pid != 0) {
			exit(0);
		}

		chdir("/");
		umask(0);
		for(int i = 0; i < 1024; i++) {
			close(i);
		}
// m_interactive = false;
	}

	return true;
}


/*--------------------------------------------------------------------*/
/* Function: service_thread                                        */
/*--------------------------------------------------------------------*/

static void service_thread(gpointer data, gpointer user_data)
{
	psstrmsock thrdinst = (psstrmsock) data;

	char *buf;

	printf("Servicing connection.\n");

	while (TRUE) {
		(void *)buf = thrdinst->ServerReadMsg();
		if (buf == NULL) {
			if (thrdinst->GetErrcode() == 0) {
				printf("Conection has been aborted!\n");
				delete thrdinst;
				break;
			} else {
				printf("Error reading message from client.\n");
				thrdinst->Close();
				delete thrdinst;
				g_thread_exit(NULL);
			}
		}

	cMessageHeader *header =  thrdinst->GetHeader();
	printf("Message from client read. The message (len=%d) is \"%s\" --- type=%d\n",
						header->m_len, buf, header->m_id);
	}

// stop_server = TRUE;
	return; // do NOT use g_thread_exit here!
}

