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
#include "SaHpi.h"


/*--------------------------------------------------------------------*/
/* Local definitions                                                  */
/*--------------------------------------------------------------------*/

extern "C"
{

enum tResult
{
   eResultOk,
   eResultError,
   eResultReply
};

static bool morph2daemon(bool runasdaemon);
static void service_thread(gpointer data, gpointer user_data);
static void HandleOpen(psstrmsock thrdinst);
static void HandlePing(psstrmsock thrdinst);
static void HandleInvalidRequest(psstrmsock thrdinst, unsigned char et);
static tResult HandleMsg(psstrmsock thrdinst);

}

static bool stop_server = FALSE;



/*--------------------------------------------------------------------*/
/* Function: main                                                     */
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
// m_interactive = false;
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
	const void *buf;
//        void *rd = NULL;
//        int rv = 0;

	printf("Servicing connection.\n");
	while (stop == false) {
                buf = thrdinst->ServerReadMsg();
                switch( thrdinst->header.m_type ) {
                case eMhOpen:
                        HandleOpen(thrdinst);
                        break;
                case eMhClose:
                        stop = true;
                        break;
                case eMhPing:
                        HandlePing(thrdinst);
                        break;
                case eMhMsg:
                        HandleMsg(thrdinst);
                        // marshal error ?
//                        if ( rh.m_len < 0 )
//                             rv = 1;
//                        if ( r == eResultReply )
//                           {
//                             assert( rh.m_len >= 0 && rh.m_len <= dMaxMessageLength );
//                             rv = c->WriteMsg( rh, rd );
//                           }
//                        else if ( r == eResultError )
//                             rv = 1;
//
//                        if ( rd )
//                             free( rd );
//
//                        return !rv;
                        break;
                default:
                        HandleInvalidRequest(thrdinst, thrdinst->header.m_type);
                        break;
                }
	}

        delete thrdinst; // cleanup thread instance data

	return; // do NOT use g_thread_exit here!
}


/*--------------------------------------------------------------------*/
/* Function: HandleOpen                                               */
/*--------------------------------------------------------------------*/

void HandleOpen(psstrmsock thrdinst) {

  /* create and deliver a pong message */
  thrdinst->MessageHeaderInit(eMhOpen, 0, dMhReply, 0 );
  thrdinst->ServerWriteMsg(NULL);

  return;
}


/*--------------------------------------------------------------------*/
/* Function: HandlePing                                               */
/*--------------------------------------------------------------------*/

void HandlePing(psstrmsock thrdinst) {

  /* create and deliver a pong message */
  thrdinst->MessageHeaderInit(eMhPing, 0, dMhReply, 0 );
  thrdinst->ServerWriteMsg(NULL);

  return;
}


/*--------------------------------------------------------------------*/
/* Function: HandleInvalidRequest                                     */
/*--------------------------------------------------------------------*/

void HandleInvalidRequest(psstrmsock thrdinst, unsigned char et) {

  /* create and deliver a pong message */
  thrdinst->MessageHeaderInit((tMessageType)et, 0, dMhError, 0 );
  thrdinst->ServerWriteMsg(NULL);

  return;
}


/*--------------------------------------------------------------------*/
/* Function: HandleMsg                                                */
/*--------------------------------------------------------------------*/

static tResult HandleMsg(psstrmsock thrdinst)
{
  cHpiMarshal *hm = HpiMarshalFind( thrdinst->header.m_id );
  void *rd;
  SaErrorT ret;

  // check for function and data length
  if ( !hm || hm->m_request_len < thrdinst->header.m_len )
     {
       fprintf( stderr, "wrong message length: id %d !\n", thrdinst->header.m_id );

       return eResultError;
     }

//  assert( hm->m_reply_len );

  // init reply header
  thrdinst->MessageHeaderInit((tMessageType) thrdinst->header.m_type, 0, 
                              dMhReply, hm->m_reply_len );

  // alloc reply buffer
  rd = malloc( hm->m_reply_len );
  memset( rd, 0, hm->m_reply_len );

  switch( thrdinst->header.m_id ) {
       case eFsaHpiSessionOpen: {
	      SaHpiDomainIdT domain_id;
	      SaHpiSessionIdT session_id = 0;

	      if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit, hm, thrdinst->pBuf, (void *)&domain_id ) < 0 )
		   return eResultError;

	      ret = saHpiSessionOpen( domain_id, &session_id, 0 );

//	      DbgFunc( "saHpiSessionOpen( %x, %x ) = %d\n",
//                       domain_id, session_id, ret );

//     if ( ret == SA_OK )
//   c->AddSession( session_id );

	      thrdinst->header.m_len = HpiMarshalReply1( hm, rd, &ret, &session_id );
 
       }
       break;

       case eFsaHpiSessionClose: {
	      SaHpiSessionIdT session_id;

	      if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit, hm, thrdinst->pBuf, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiSessionClose( session_id );

//	      DbgFunc( "saHpiSessionClose( %x ) = %d\n", session_id, ret );

//     if ( ret == SA_OK )
//   c->RemSession( session_id );

	      thrdinst->header.m_len = HpiMarshalReply0( hm, rd, &ret );

       }
       break;

       case eFsaHpiDiscover: {
	      SaHpiSessionIdT session_id;

	      if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit, hm, thrdinst->pBuf, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiDiscover( session_id );

//	      DbgFunc( "saHpiResourcesDiscover( %x ) = %d\n", session_id, ret );

	      thrdinst->header.m_len = HpiMarshalReply0( hm, rd, &ret );

       }
       break;

       default:
//            assert( 0 );
            break;
       }

//assert( rh.m_len <= hm->m_reply_len );

return eResultReply;
}




