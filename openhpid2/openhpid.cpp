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
#include "marshal_hpi.h"



/*--------------------------------------------------------------------*/
/* Local definitions                                                  */
/*--------------------------------------------------------------------*/

extern "C"
{

enum tResult
{
   eResultOk,
   eResultError,
   eResultReply,
   eResultClose
};

static bool morph2daemon(bool runasdaemon);
static void service_thread(gpointer data, gpointer user_data);
static void HandleInvalidRequest(psstrmsock thrdinst, void *buf);
static tResult HandleMsg(psstrmsock thrdinst, void *buf);

}

static bool stop_server = FALSE;



/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
	GThreadPool *thrdpool;

        // use config file given by the command line
        if (argc > 1) {
                printf("Using configuration file: %s.\n", argv[1]);
                setenv("OPENHPI_CONF", argv[1], 1);
        }

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
/* Function: morph2daemon                                             */
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
	}

	return true;
}


/*--------------------------------------------------------------------*/
/* Function: service_thread                                           */
/*--------------------------------------------------------------------*/

static void service_thread(gpointer data, gpointer user_data)
{
	psstrmsock thrdinst = (psstrmsock) data;
        bool stop = false;
	void *buf;
        tResult result;

	printf("Servicing connection.\n");
	while (stop == false) {
                buf = thrdinst->ReadMsg();
                if (buf == NULL) {
                        printf("Error reading socket.\n");
                        return;
                }
                else {
                        switch( thrdinst->reqheader.m_type ) {
                        case eMhMsg:
                                printf("Calling HandleMsg.\n");
                                result = HandleMsg(thrdinst, buf);
                                // marshal error ?
                                if (result == eResultError) {
                                        HandleInvalidRequest(thrdinst, buf);
                                }
                                // done ?
                                if (result == eResultClose) {
                                        stop = true;
                                }
                                break;
                        default:
                                HandleInvalidRequest(thrdinst, buf);
                                break;
                        }
                        free(buf); // this was malloced in ServerReadMsg
                }
	}

        delete thrdinst; // cleanup thread instance data

	printf("Connection ended.\n");
	return; // do NOT use g_thread_exit here!
}


/*--------------------------------------------------------------------*/
/* Function: HandleInvalidRequest                                     */
/*--------------------------------------------------------------------*/

void HandleInvalidRequest(psstrmsock thrdinst, void *data) {
        char *pReq = (char *)data + sizeof(cMessageHeader);

  /* create and deliver a pong message */
  printf("Invalid request.\n");
  thrdinst->RepMessageHeaderInit(eMhError, 0, thrdinst->reqheader.m_id, 0 );
  thrdinst->WriteMsg(pReq, NULL);

  return;
}


/*--------------------------------------------------------------------*/
/* Function: HandleMsg                                                */
/*--------------------------------------------------------------------*/

static tResult HandleMsg(psstrmsock thrdinst, void *data)
{
  cHpiMarshal *hm;
  void *rd;
  SaErrorT ret;
  tResult result = eResultReply;
  char *pReq = (char *)data + sizeof(cMessageHeader);

  printf("Processing message.\n");


  hm = HpiMarshalFind(thrdinst->reqheader.m_id);
  // check for function and data length
  if ( !hm || hm->m_request_len < thrdinst->reqheader.m_len )
     {
//       fprintf( stderr, "Wrong message length: id %d !\n", thrdinst->reqheader.m_id );
       return eResultError;
     }

  // init reply header
  thrdinst->RepMessageHeaderInit((tMessageType) thrdinst->reqheader.m_type, 0, 
                                 thrdinst->reqheader.m_id, hm->m_reply_len );

  // alloc reply buffer
  rd = malloc( hm->m_reply_len );
  memset( rd, 0, hm->m_reply_len );

  switch( thrdinst->reqheader.m_id ) {
       case eFsaHpiSessionOpen: {
	      SaHpiDomainIdT domain_id;
	      SaHpiSessionIdT session_id = 0;

              printf("Processing saHpiSessionOpen.\n");
	      if ( HpiDemarshalRequest1( thrdinst->repheader.m_flags & dMhEndianBit, hm, pReq, (void *)&domain_id ) < 0 )
		   return eResultError;

	      ret = saHpiSessionOpen( domain_id, &session_id, 0 );

	      thrdinst->repheader.m_len = HpiMarshalReply1( hm, rd, &ret, &session_id );
 
       }
       break;

       case eFsaHpiSessionClose: {
	      SaHpiSessionIdT session_id;

              printf("Processing saHpiSessionClose.\n");
	      if ( HpiDemarshalRequest1( thrdinst->repheader.m_flags & dMhEndianBit, hm, pReq, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiSessionClose( session_id );

	      thrdinst->repheader.m_len = HpiMarshalReply0( hm, rd, &ret );
              result = eResultClose;

       }
       break;

       case eFsaHpiDiscover: {
	      SaHpiSessionIdT session_id;

              printf("Processing saHpiDiscover.\n");
	      if ( HpiDemarshalRequest1( thrdinst->repheader.m_flags & dMhEndianBit, hm, pReq, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiDiscover( session_id );

	      thrdinst->repheader.m_len = HpiMarshalReply0( hm, rd, &ret );

       }
       break;

       default:
            break;
       }

       // send the reply
       thrdinst->WriteMsg(pReq, rd);

       if (rd != NULL) {
               free(rd);
       }
       return result;
}

