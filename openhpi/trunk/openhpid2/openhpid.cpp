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
//#include "openhpi.h"
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
static void HandleInvalidRequest(psstrmsock thrdinst);
static tResult HandleMsg(psstrmsock thrdinst, char *buf);

}

static bool stop_server = FALSE;



/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
	GThreadPool *thrdpool;
        int port;
        char *portstr;

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

        // get our listening port
        portstr = getenv("OPENHPI_DAEMON_PORT");
        if (portstr == NULL) {
                port =  4743;
        }
        else {
                port =  atoi(portstr);
        }

        // create the server socket
	psstrmsock servinst = new sstrmsock;
	if (servinst->Create(port)) {
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
	char buf[dMaxMessageLength];  
        tResult result;

	printf("Servicing connection.\n");
	while (stop == false) {
                if (thrdinst->ReadMsg(buf)) {
                        printf("Error reading socket.\n");
                        goto thrd_cleanup;
                }
                else {
                        switch( thrdinst->header.m_type ) {
                        case eMhMsg:
                                result = HandleMsg(thrdinst, buf);
                                // marshal error ?
                                if (result == eResultError) {
                                        printf("Invalid API found.\n");
                                        HandleInvalidRequest(thrdinst);
                                }
                                // done ?
                                if (result == eResultClose) {
                                        stop = true;
                                }
                                break;
                        default:
                                printf("Error in socket read buffer data.\n");
                                HandleInvalidRequest(thrdinst);
                                break;
                        }
                }
	}

        thrd_cleanup:
        delete thrdinst; // cleanup thread instance data

	printf("Connection ended.\n");
	return; // do NOT use g_thread_exit here!
}


/*--------------------------------------------------------------------*/
/* Function: HandleInvalidRequest                                     */
/*--------------------------------------------------------------------*/

void HandleInvalidRequest(psstrmsock thrdinst) {

  /* create and deliver a pong message */
  printf("Invalid request.\n");
  thrdinst->MessageHeaderInit(eMhError, 0, thrdinst->header.m_id, 0 );
  thrdinst->WriteMsg(NULL);

  return;
}


/*--------------------------------------------------------------------*/
/* Function: HandleMsg                                                */
/*--------------------------------------------------------------------*/

static tResult HandleMsg(psstrmsock thrdinst, char *data)
{
  cHpiMarshal *hm;
  SaErrorT ret;
  tResult result = eResultReply;
  char *pReq = data + sizeof(cMessageHeader);

  hm = HpiMarshalFind(thrdinst->header.m_id);
  // check for function and data length
  if ( !hm || hm->m_request_len != thrdinst->header.m_len )
     {
       return eResultError;
     }

  // init reply header
  thrdinst->MessageHeaderInit((tMessageType) thrdinst->header.m_type, 0, 
                                 thrdinst->header.m_id, hm->m_reply_len );

  switch( thrdinst->header.m_id ) {
       case eFsaHpiVersionGet: {
	      SaHpiVersionT ver;

              printf("Processing saHpiVersionGet.\n");

	      ver = saHpiVersionGet( );

	      thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ver );
       }
       break;

       case eFsaHpiSessionOpen: {
              SaHpiDomainIdT  domain_id;
              SaHpiSessionIdT session_id = 0;
              void            *securityparams = NULL;

              printf("Processing saHpiSessionOpen.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &domain_id ) < 0 )
                   return eResultError;

              ret = saHpiSessionOpen( domain_id, &session_id, securityparams );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &session_id );
       }
       break;

       case eFsaHpiSessionClose: {
	      SaHpiSessionIdT session_id;

              printf("Processing saHpiSessionClose.\n");
	      
              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiSessionClose( session_id );

	      thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
              result = eResultClose;
       }
       break;

       case eFsaHpiDiscover: {
	      SaHpiSessionIdT session_id;

              printf("Processing saHpiDiscover.\n");
	      
              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
		   return eResultError;

	      ret = saHpiDiscover( session_id );

	      thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiDomainInfoGet: {
              SaHpiSessionIdT  session_id;
              SaHpiDomainInfoT domain_info;

              printf("Processing saHpiDomainInfoGet.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
                   return eResultError;

              ret = saHpiDomainInfoGet( session_id, &domain_info );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &domain_info );
       }
       break;

       case eFsaHpiDrtEntryGet: {
              SaHpiSessionIdT session_id;
              SaHpiEntryIdT   entry_id;
              SaHpiEntryIdT   next_entry_id = 0;
              SaHpiDrtEntryT  drt_entry;

              printf("Processing saHpiDrtEntryGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &entry_id ) < 0 )
                   return eResultError;

              ret = saHpiDrtEntryGet( session_id, entry_id, &next_entry_id,
                                      &drt_entry );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &next_entry_id, &drt_entry );
       }
       break;

       case eFsaHpiDomainTagSet: {
              SaHpiSessionIdT  session_id;
              SaHpiTextBufferT *domain_tag;

              printf("Processing saHpiDomainTagSet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &domain_tag ) < 0 )
                   return eResultError;

              ret = saHpiDomainTagSet( session_id, domain_tag );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiRptEntryGet: {
              SaHpiSessionIdT session_id;
              SaHpiEntryIdT   entry_id;
              SaHpiEntryIdT   next_entry_id = 0; // for valgring
              SaHpiRptEntryT  rpt_entry;

              printf("Processing saHpiRptEntryGet.\n,");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &entry_id ) < 0 )
                   return eResultError;

              printf("Session = %d, entryid = %d\n", session_id, entry_id);
              ret = saHpiRptEntryGet( session_id, entry_id, &next_entry_id, &rpt_entry );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &next_entry_id, &rpt_entry );
       }
       break;

       case eFsaHpiRptEntryGetByResourceId: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiRptEntryT   rpt_entry;

              printf("Processing saHpiRptEntryGetByResourceId.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiRptEntryGetByResourceId( session_id, resource_id, &rpt_entry );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &rpt_entry );
       }
       break;

       case eFsaHpiResourceSeveritySet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSeverityT   severity;

              printf("Processing saHpiResourceSeveritySet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &severity ) < 0 )
                   return eResultError;

              ret = saHpiResourceSeveritySet( session_id,
                                              resource_id, severity );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiResourceTagSet:
            {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiTextBufferT resource_tag;

              printf("Processing saHpiResourceTagSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &resource_tag ) < 0 )
                   return eResultError;

              ret = saHpiResourceTagSet( session_id, resource_id, &resource_tag );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }

       break;

       case eFsaHpiResourceIdGet: {
              SaHpiSessionIdT session_id;
              SaHpiResourceIdT resource_id = 0;

              printf("Processing saHpiResourceIdGet.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
                   return eResultError;

              ret = saHpiResourceIdGet( session_id, &resource_id );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &resource_id );
       }

       break;

       case eFsaHpiEventLogInfoGet: {
              SaHpiSessionIdT    session_id;
              SaHpiResourceIdT   resource_id;
              SaHpiEventLogInfoT info;

              printf("Processing saHpiEventLogInfoGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiEventLogInfoGet( session_id, resource_id, &info );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &info );
       }
       break;

       case eFsaHpiEventLogEntryGet: {
              SaHpiSessionIdT       session_id;
              SaHpiResourceIdT      resource_id;
              SaHpiEventLogEntryIdT entry_id;
              SaHpiEventLogEntryIdT prev_entry_id = 0;
              SaHpiEventLogEntryIdT next_entry_id = 0;
              SaHpiEventLogEntryT   event_log_entry;
              SaHpiRdrT             rdr;
              SaHpiRptEntryT        rpt_entry;

              printf("Processing saHpiEventLogEntryGet.\n");

              memset( &rdr, 0, sizeof( SaHpiRdrT ) );
              memset( &rpt_entry, 0, sizeof( SaHpiRptEntryT ) );

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id, 
                                         &entry_id, &rdr, &rpt_entry ) < 0 )
                   return eResultError;

              ret = saHpiEventLogEntryGet( session_id, resource_id, entry_id,
                                           &prev_entry_id, &next_entry_id,
                                           &event_log_entry, &rdr, &rpt_entry );

              thrdinst->header.m_len = HpiMarshalReply5( hm, pReq, &ret, &prev_entry_id, &next_entry_id,
                                                         &event_log_entry, &rdr, &rpt_entry );
       }
       break;

       case eFsaHpiEventLogEntryAdd: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiEventT      evt_entry;

              printf("Processing saHpiEventLogEntryAdd.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &evt_entry ) < 0 )
                   return eResultError;

              ret = saHpiEventLogEntryAdd( session_id, resource_id,
                                           &evt_entry );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiEventLogClear: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;

              printf("Processing saHpiEventLogClear.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiEventLogClear( session_id, resource_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiEventLogTimeGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiTimeT       ti;

              printf("Processing saHpiEventLogTimeGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiEventLogTimeGet( session_id, resource_id, &ti );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &ti );
       }
       break;

       case eFsaHpiEventLogTimeSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiTimeT       ti;

              printf("Processing saHpiEventLogTimeSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &ti ) < 0 )
                   return eResultError;

              ret = saHpiEventLogTimeSet( session_id, resource_id, ti );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiEventLogStateGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiBoolT       enable;

              printf("Processing saHpiEventLogStateGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiEventLogStateGet( session_id, resource_id, &enable );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &enable );
       }
       break;

       case eFsaHpiEventLogStateSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiBoolT       enable;

              printf("Processing saHpiEventLogStateSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &enable ) < 0 )
                   return eResultError;

              ret = saHpiEventLogStateSet( session_id, resource_id, enable );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiEventLogOverflowReset: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;

              printf("Processing saHpiEventLogOverflowReset.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiEventLogOverflowReset( session_id, resource_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiSubscribe: {
              SaHpiSessionIdT session_id;

              printf("Processing saHpiSubscribe.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
                   return eResultError;

              ret = saHpiSubscribe( session_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiUnsubscribe: {
              SaHpiSessionIdT session_id;

              printf("Processing saHpiUnsubscribe.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
                   return eResultError;

              ret = saHpiUnsubscribe( session_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiEventGet: {
              SaHpiSessionIdT      session_id;
              SaHpiTimeoutT        timeout;
              SaHpiEventT          event;
              SaHpiRdrT            rdr;
              SaHpiRptEntryT       rpt_entry;
              SaHpiEvtQueueStatusT status;

              printf("Processing saHpiEventGet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &timeout, &rdr, &rpt_entry, &status ) < 0 )
                   return eResultError;

              ret = saHpiEventGet( session_id, timeout, &event, &rdr,
                                   &rpt_entry, &status );

              thrdinst->header.m_len = HpiMarshalReply4( hm, pReq, &ret, &event, &rdr, &rpt_entry, &status );
       }
       break;

       case eFsaHpiEventAdd: {
              SaHpiSessionIdT session_id;
              SaHpiEventT     event;

              printf("Processing saHpiEventAdd.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &event ) < 0 )
                   return eResultError;

              ret = saHpiEventAdd( session_id, &event );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAlarmGetNext: {
              SaHpiSessionIdT session_id;
              SaHpiSeverityT  severity;
              SaHpiBoolT      unack;
              SaHpiAlarmT     alarm;

              printf("Processing saHpiAlarmGetNext.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &severity,
                                         &unack, &alarm ) < 0 )
                   return eResultError;

              ret = saHpiAlarmGetNext( session_id, severity, unack, &alarm );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &alarm );
       }
       break;

       case eFsaHpiAlarmGet: {
              SaHpiSessionIdT session_id;
              SaHpiAlarmIdT   alarm_id;
              SaHpiAlarmT     alarm;

              printf("Processing saHpiAlarmGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &alarm_id ) < 0 )
                   return eResultError;

              ret = saHpiAlarmGet( session_id, alarm_id, &alarm );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &alarm );
       }
       break;

       case eFsaHpiAlarmAcknowledge: {
              SaHpiSessionIdT session_id;
              SaHpiAlarmIdT   alarm_id;
              SaHpiSeverityT  severity;

              printf("Processing saHpiAlarmAcknowledge.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &alarm_id,
                                         &severity ) < 0 )
                   return eResultError;

              ret = saHpiAlarmAcknowledge( session_id, alarm_id, severity );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAlarmAdd: {
              SaHpiSessionIdT session_id;
              SaHpiAlarmT     alarm;

              printf("Processing saHpiAlarmAdd.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &alarm ) < 0 )
                   return eResultError;

              ret = saHpiAlarmAdd( session_id, &alarm );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &alarm );
       }
       break;

       case eFsaHpiAlarmDelete: {
              SaHpiSessionIdT session_id;
              SaHpiAlarmIdT   alarm_id;
              SaHpiSeverityT  severity;

              printf("Processing saHpiAlarmDelete.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &alarm_id,
                                         &severity ) < 0 )
                   return eResultError;

              ret = saHpiAlarmDelete( session_id, alarm_id, severity );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiRdrGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiEntryIdT    entry_id;
              SaHpiEntryIdT    next_entry_id;
              SaHpiRdrT        rdr;

              printf("Processing saHpiRdrGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &entry_id ) < 0 )
                   return eResultError;

              ret = saHpiRdrGet( session_id, resource_id, entry_id,
                                 &next_entry_id, &rdr );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &next_entry_id, &rdr );
       }
       break;

       case eFsaHpiRdrGetByInstrumentId: {
              SaHpiSessionIdT    session_id;
              SaHpiResourceIdT   resource_id;
              SaHpiRdrTypeT      rdr_type;
              SaHpiInstrumentIdT inst_id;
              SaHpiRdrT          rdr;

              printf("Processing saHpiRdrGetByInstrumentId.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &rdr_type, &inst_id ) < 0 )
                   return eResultError;

              ret = saHpiRdrGetByInstrumentId( session_id, resource_id, rdr_type,
                                               inst_id, &rdr );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &rdr );
       }
       break;

       case eFsaHpiSensorReadingGet: {
              SaHpiSessionIdT     session_id;
              SaHpiResourceIdT    resource_id;
              SaHpiSensorNumT     sensor_num;
              SaHpiSensorReadingT reading;
              SaHpiEventStateT    state;

              printf("Processing saHpiSensorReadingGet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &reading, &state ) < 0 )
                   return eResultError;

              ret = saHpiSensorReadingGet( session_id, resource_id,
                                           sensor_num, &reading, &state );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &reading, &state );
       }
       break;

       case eFsaHpiSensorThresholdsGet: {
              SaHpiSessionIdT        session_id;
              SaHpiResourceIdT       resource_id;
              SaHpiSensorNumT        sensor_num;
              SaHpiSensorThresholdsT sensor_thresholds;

              printf("Processing saHpiSensorThresholdsGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num ) < 0 )
                   return eResultError;

              ret = saHpiSensorThresholdsGet( session_id,
                                              resource_id, sensor_num,
                                              &sensor_thresholds);

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &sensor_thresholds );
       }
       break;

       case eFsaHpiSensorThresholdsSet: {
              SaHpiSessionIdT session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiSensorThresholdsT sensor_thresholds;

              printf("Processing saHpiSensorThresholdsSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &sensor_thresholds ) < 0 )
                   return eResultError;

              ret = saHpiSensorThresholdsSet( session_id, resource_id,
                                              sensor_num,
                                              &sensor_thresholds );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiSensorTypeGet: {
              SaHpiSessionIdT     session_id;
              SaHpiResourceIdT    resource_id;
              SaHpiSensorNumT     sensor_num;
              SaHpiSensorTypeT    type;
              SaHpiEventCategoryT category;

              printf("Processing saHpiSensorTypeGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num ) < 0 )
                   return eResultError;

              ret = saHpiSensorTypeGet( session_id, resource_id,
                                        sensor_num, &type, &category );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &type, &category );
       }
       break;

       case eFsaHpiSensorEnableGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiBoolT       enables;

              printf("Processing saHpiSensorEnableGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num ) < 0 )
                   return eResultError;

              ret = saHpiSensorEnableGet( session_id, resource_id,
                                          sensor_num, &enables );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &enables );
       }
       break;

       case eFsaHpiSensorEnableSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiBoolT       enables;

              printf("Processing saHpiSensorEnableSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &enables ) < 0 )
                   return eResultError;

              ret = saHpiSensorEnableSet( session_id, resource_id,
                                          sensor_num, enables );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiSensorEventEnableGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiBoolT       enables;

              printf("Processing saHpiSensorEventEnableGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num ) < 0 )
                   return eResultError;

              ret = saHpiSensorEventEnableGet( session_id, resource_id,
                                               sensor_num, &enables );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &enables );
       }
       break;

       case eFsaHpiSensorEventEnableSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiBoolT       enables;

              printf("Processing saHpiSensorEventEnableSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &enables ) < 0 )
                   return eResultError;

              ret = saHpiSensorEventEnableSet( session_id, resource_id,
                                               sensor_num, enables );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiSensorEventMasksGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiSensorNumT  sensor_num;
              SaHpiEventStateT assert_mask;
              SaHpiEventStateT deassert_mask;

              printf("Processing saHpiSensorEventMasksGet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &assert_mask,
                                         &deassert_mask ) < 0 )
                   return eResultError;

              ret = saHpiSensorEventMasksGet( session_id, resource_id, sensor_num,
                                              &assert_mask, &deassert_mask );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &assert_mask, &deassert_mask );
       }
       break;

       case eFsaHpiSensorEventMasksSet: {
              SaHpiSessionIdT             session_id;
              SaHpiResourceIdT            resource_id;
              SaHpiSensorNumT             sensor_num;
              SaHpiSensorEventMaskActionT action;
              SaHpiEventStateT            assert_mask;
              SaHpiEventStateT            deassert_mask;

              printf("Processing saHpiSensorEventMasksSet.\n");

              if ( HpiDemarshalRequest6( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &sensor_num, &action, &assert_mask,
                                         &deassert_mask ) < 0 )
                   return eResultError;

              ret = saHpiSensorEventMasksSet( session_id, resource_id, sensor_num,
                                              action, assert_mask, deassert_mask );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiControlTypeGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiCtrlNumT    ctrl_num;
              SaHpiCtrlTypeT   type;

              printf("Processing saHpiControlTypeGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &ctrl_num ) < 0 )
                   return eResultError;

              ret = saHpiControlTypeGet( session_id, resource_id, ctrl_num,
                                         &type );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &type );
       }
       break;

       case eFsaHpiControlGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiCtrlNumT    ctrl_num;
              SaHpiCtrlModeT   ctrl_mode;
              SaHpiCtrlStateT  ctrl_state;

              printf("Processing saHpiControlGet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &ctrl_num, &ctrl_state ) < 0 )
                   return eResultError;

              ret = saHpiControlGet( session_id, resource_id, ctrl_num,
                                     &ctrl_mode, &ctrl_state );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &ctrl_mode, &ctrl_state );
       }
       break;

       case eFsaHpiControlSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiCtrlNumT    ctrl_num;
              SaHpiCtrlModeT   ctrl_mode;
              SaHpiCtrlStateT  ctrl_state;

              printf("Processing saHpiControlSet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &ctrl_num, &ctrl_mode, &ctrl_state ) < 0 )
                   return eResultError;

              ret = saHpiControlSet( session_id, resource_id,
                                     ctrl_num, ctrl_mode, &ctrl_state );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiIdrInfoGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiIdrIdT      idr_id;
              SaHpiIdrInfoT    info;

              printf("Processing saHpiIdrInfoGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id ) < 0 )
                   return eResultError;

              ret = saHpiIdrInfoGet( session_id, resource_id,
                                     idr_id, &info );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &info );
       }
       break;

       case eFsaHpiIdrAreaHeaderGet: {
              SaHpiSessionIdT     session_id;
              SaHpiResourceIdT    resource_id;
              SaHpiIdrIdT         idr_id;
              SaHpiIdrAreaTypeT   area;
              SaHpiEntryIdT       area_id;
              SaHpiEntryIdT       next;
              SaHpiIdrAreaHeaderT header;

              printf("Processing saHpiIdrAreaHeaderGet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &area, &area_id ) < 0 )
                   return eResultError;

              ret = saHpiIdrAreaHeaderGet( session_id, resource_id, idr_id,
                                           area, area_id, &next, &header );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &next, &header );
       }
       break;

       case eFsaHpiIdrAreaAdd: {
              SaHpiSessionIdT     session_id;
              SaHpiResourceIdT    resource_id;
              SaHpiIdrIdT         idr_id;
              SaHpiIdrAreaTypeT   area;
              SaHpiEntryIdT       area_id;

              printf("Processing saHpiIdrAreaAdd.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &area ) < 0 )
                   return eResultError;

              ret = saHpiIdrAreaAdd( session_id, resource_id, idr_id,
                                     area, &area_id  );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &area_id );
       }
       break;

       case eFsaHpiIdrAreaDelete: {
              SaHpiSessionIdT     session_id;
              SaHpiResourceIdT    resource_id;
              SaHpiIdrIdT         idr_id;
              SaHpiEntryIdT       area_id;

              printf("Processing saHpiIdrAreaAdd.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &area_id ) < 0 )
                   return eResultError;

              ret = saHpiIdrAreaDelete( session_id, resource_id, idr_id,
                                        area_id  );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiIdrFieldGet: {
              SaHpiSessionIdT    session_id;
              SaHpiResourceIdT   resource_id;
              SaHpiIdrIdT        idr_id;
              SaHpiEntryIdT      area_id;
              SaHpiIdrFieldTypeT type;
              SaHpiEntryIdT      field_id;
              SaHpiEntryIdT      next;
              SaHpiIdrFieldT     field;

              printf("Processing saHpiIdrFieldGet.\n");

              if ( HpiDemarshalRequest6( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &area_id, &type, &field_id ) < 0 )
                   return eResultError;

              ret = saHpiIdrFieldGet( session_id, resource_id, idr_id, area_id,
                                      type, field_id, &next, &field );

              thrdinst->header.m_len = HpiMarshalReply2( hm, pReq, &ret, &next, &field );
       }
       break;

       case eFsaHpiIdrFieldAdd: {
              SaHpiSessionIdT    session_id;
              SaHpiResourceIdT   resource_id;
              SaHpiIdrIdT        idr_id;
              SaHpiIdrFieldT     field;

              printf("Processing saHpiIdrFieldAdd.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &field ) < 0 )
                   return eResultError;

              ret = saHpiIdrFieldAdd( session_id, resource_id, idr_id,
                                      &field );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &field );
       }
       break;

       case eFsaHpiIdrFieldSet: {
              SaHpiSessionIdT    session_id;
              SaHpiResourceIdT   resource_id;
              SaHpiIdrIdT        idr_id;
              SaHpiIdrFieldT     field;

              printf("Processing saHpiIdrFieldSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &field ) < 0 )
                   return eResultError;

              ret = saHpiIdrFieldSet( session_id, resource_id, idr_id,
                                      &field );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiIdrFieldDelete: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiIdrIdT      idr_id;
              SaHpiEntryIdT    area_id;
              SaHpiEntryIdT    field_id;

              printf("Processing saHpiIdrFieldSet.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &idr_id, &area_id, &field_id ) < 0 )
                   return eResultError;

              ret = saHpiIdrFieldDelete( session_id, resource_id, idr_id,
                                         area_id, field_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiWatchdogTimerGet: {
              SaHpiSessionIdT   session_id;
              SaHpiResourceIdT  resource_id;
              SaHpiWatchdogNumT watchdog_num;
              SaHpiWatchdogT    watchdog;

              printf("Processing saHpiWatchdogTimerGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &watchdog_num ) < 0 )
                   return eResultError;

              ret = saHpiWatchdogTimerGet( session_id, resource_id,
                                           watchdog_num, &watchdog );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &watchdog );
       }
       break;

       case eFsaHpiWatchdogTimerSet: {
              SaHpiSessionIdT   session_id;
              SaHpiResourceIdT  resource_id;
              SaHpiWatchdogNumT watchdog_num;
              SaHpiWatchdogT    watchdog;

              printf("Processing saHpiWatchdogTimerSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &watchdog_num, &watchdog ) < 0 )
                   return eResultError;

              ret = saHpiWatchdogTimerSet( session_id, resource_id,
                                           watchdog_num, &watchdog );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiWatchdogTimerReset: {
              SaHpiSessionIdT   session_id;
              SaHpiResourceIdT  resource_id;
              SaHpiWatchdogNumT watchdog_num;

              printf("Processing saHpiWatchdogTimerReset.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &watchdog_num ) < 0 )
                   return eResultError;

              ret = saHpiWatchdogTimerReset( session_id, resource_id,
                                             watchdog_num );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAnnunciatorGetNext: {
              SaHpiSessionIdT      session_id;
              SaHpiResourceIdT     resource_id;
              SaHpiAnnunciatorNumT annun_num;
              SaHpiSeverityT       severity;
              SaHpiBoolT           unack;
              SaHpiAnnouncementT   announcement;

              printf("Processing saHpiAnnunciatorGetNext.\n");

              if ( HpiDemarshalRequest6( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &severity, &unack,
                                         &announcement ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorGetNext( session_id, resource_id, annun_num,
                                             severity, unack, &announcement );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &announcement );
       }
       break;

       case eFsaHpiAnnunciatorGet: {
              SaHpiSessionIdT      session_id;
              SaHpiResourceIdT     resource_id;
              SaHpiAnnunciatorNumT annun_num;
              SaHpiEntryIdT        entry_id;
              SaHpiAnnouncementT   announcement;

              printf("Processing saHpiAnnunciatorGet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &entry_id ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorGet( session_id, resource_id, annun_num,
                                         entry_id, &announcement );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &announcement );
       }
       break;

       case eFsaHpiAnnunciatorAcknowledge: {
              SaHpiSessionIdT      session_id;
              SaHpiResourceIdT     resource_id;
              SaHpiAnnunciatorNumT annun_num;
              SaHpiEntryIdT        entry_id;
              SaHpiSeverityT       severity;

              printf("Processing saHpiAnnunciatorAcknowledge.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &entry_id, &severity ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorAcknowledge( session_id, resource_id, annun_num,
                                                 entry_id, severity );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAnnunciatorAdd: {
              SaHpiSessionIdT      session_id;
              SaHpiResourceIdT     resource_id;
              SaHpiAnnunciatorNumT annun_num;
              SaHpiAnnouncementT   announcement;

              printf("Processing saHpiAnnunciatorAdd.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &announcement ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorAdd( session_id, resource_id, annun_num,
                                         &announcement );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &announcement );
       }
       break;

       case eFsaHpiAnnunciatorDelete: {
              SaHpiSessionIdT      session_id;
              SaHpiResourceIdT     resource_id;
              SaHpiAnnunciatorNumT annun_num;
              SaHpiEntryIdT        entry_id;
              SaHpiSeverityT       severity;

              printf("Processing saHpiAnnunciatorAdd.\n");

              if ( HpiDemarshalRequest5( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &entry_id, &severity ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorDelete( session_id, resource_id, annun_num,
                                            entry_id, severity );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAnnunciatorModeGet: {
              SaHpiSessionIdT       session_id;
              SaHpiResourceIdT      resource_id;
              SaHpiAnnunciatorNumT  annun_num;
              SaHpiAnnunciatorModeT mode;

              printf("Processing saHpiAnnunciatorModeGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorModeGet( session_id, resource_id, annun_num,
                                             &mode );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &mode );
       }
       break;

       case eFsaHpiAnnunciatorModeSet: {
              SaHpiSessionIdT       session_id;
              SaHpiResourceIdT      resource_id;
              SaHpiAnnunciatorNumT  annun_num;
              SaHpiAnnunciatorModeT mode;

              printf("Processing saHpiAnnunciatorModeSet.\n");

              if ( HpiDemarshalRequest4( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &annun_num, &mode ) < 0 )
                   return eResultError;

              ret = saHpiAnnunciatorModeSet( session_id, resource_id, annun_num,
                                             mode );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiHotSwapPolicyCancel: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;

              printf("Processing saHpiHotSwapPolicyCancel.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiHotSwapPolicyCancel( session_id, resource_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiResourceActiveSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;

              printf("Processing saHpiResourceActiveSet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiResourceActiveSet( session_id, resource_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiResourceInactiveSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;

              printf("Processing saHpiResourceInactiveSet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiResourceInactiveSet( session_id, resource_id );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAutoInsertTimeoutGet: {
              SaHpiSessionIdT session_id;
              SaHpiTimeoutT   timeout;

              printf("Processing saHpiAutoInsertTimeoutGet.\n");

              if ( HpiDemarshalRequest1( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id ) < 0 )
                   return eResultError;

              ret = saHpiAutoInsertTimeoutGet( session_id, &timeout );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &timeout );
       }
       break;

       case eFsaHpiAutoInsertTimeoutSet: {
              SaHpiSessionIdT session_id;
              SaHpiTimeoutT   timeout;

              printf("Processing saHpiAutoInsertTimeoutSet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &timeout ) < 0 )
                   return eResultError;

              ret = saHpiAutoInsertTimeoutSet( session_id, timeout );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiAutoExtractTimeoutGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiTimeoutT    timeout;

              printf("Processing saHpiAutoExtractTimeoutGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiAutoExtractTimeoutGet( session_id, resource_id, &timeout );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &timeout );
       }
       break;

       case eFsaHpiAutoExtractTimeoutSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiTimeoutT    timeout;

              printf("Processing saHpiAutoExtractTimeoutSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &timeout ) < 0 )
                   return eResultError;

              ret = saHpiAutoExtractTimeoutSet( session_id, resource_id, timeout );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiHotSwapStateGet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiHsStateT    state;

              printf("Processing saHpiHotSwapStateGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiHotSwapStateGet( session_id, resource_id, &state );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &state );
       }
       break;

       case eFsaHpiHotSwapActionRequest: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiHsActionT   action;

              printf("Processing saHpiHotSwapActionRequest.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &action ) < 0 )
                   return eResultError;

              ret = saHpiHotSwapActionRequest( session_id, resource_id, action );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiHotSwapIndicatorStateGet: {
              SaHpiSessionIdT        session_id;
              SaHpiResourceIdT       resource_id;
              SaHpiHsIndicatorStateT state;

              printf("Processing saHpiHotSwapIndicatorStateGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiHotSwapIndicatorStateGet( session_id, resource_id, &state );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &state );
       }
       break;

       case eFsaHpiHotSwapIndicatorStateSet: {
              SaHpiSessionIdT        session_id;
              SaHpiResourceIdT       resource_id;
              SaHpiHsIndicatorStateT state;

              printf("Processing saHpiHotSwapIndicatorStateSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &state ) < 0 )
                   return eResultError;

              ret = saHpiHotSwapIndicatorStateSet( session_id, resource_id, state );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiParmControl: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiParmActionT action;

              printf("Processing saHpiParmControl.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &action ) < 0 )
                   return eResultError;

              ret = saHpiParmControl( session_id, resource_id, action );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiResourceResetStateGet: {
              SaHpiSessionIdT   session_id;
              SaHpiResourceIdT  resource_id;
              SaHpiResetActionT action;

              printf("Processing saHpiResourceResetStateGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiResourceResetStateGet( session_id, resource_id, &action );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &action );
       }
       break;

       case eFsaHpiResourceResetStateSet: {
              SaHpiSessionIdT   session_id;
              SaHpiResourceIdT  resource_id;
              SaHpiResetActionT action;

              printf("Processing saHpiResourceResetStateSet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &action ) < 0 )
                   return eResultError;

              ret = saHpiResourceResetStateSet( session_id, resource_id, action );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       case eFsaHpiResourcePowerStateGet:
            {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiPowerStateT state;

              printf("Processing saHpiResourcePowerStateGet.\n");

              if ( HpiDemarshalRequest2( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id ) < 0 )
                   return eResultError;

              ret = saHpiResourcePowerStateGet( session_id, resource_id, &state );

              thrdinst->header.m_len = HpiMarshalReply1( hm, pReq, &ret, &state );
       }
       break;

       case eFsaHpiResourcePowerStateSet: {
              SaHpiSessionIdT  session_id;
              SaHpiResourceIdT resource_id;
              SaHpiPowerStateT state;

              printf("Processing saHpiResourcePowerStateGet.\n");

              if ( HpiDemarshalRequest3( thrdinst->header.m_flags & dMhEndianBit,
                                         hm, pReq, &session_id, &resource_id,
                                         &state  ) < 0 )
                   return eResultError;

              ret = saHpiResourcePowerStateSet( session_id, resource_id, state );

              thrdinst->header.m_len = HpiMarshalReply0( hm, pReq, &ret );
       }
       break;

       default:
              return eResultError;
       }

       // send the reply
       bool wrt_result = thrdinst->WriteMsg(pReq);
       if (wrt_result) {
               return eResultError;
       }

       printf("Return code = %d\n", ret);

       return result;
}

