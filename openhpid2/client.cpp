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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>

#include "marshal_hpi.h"
#include "client.h"

#define dClientDebugErr
#define dClientDebug

#ifndef dClientDebug
#define cdebug_out(cmd, str)
#else
#define cdebug_out(cmd, str)	fprintf(stdout, "%s: %s\n", cmd, str);
#endif

#ifndef dClientDebugErr
#define cdebug_err(cmd, str)
#else
#define cdebug_err(cmd, str)	fprintf(stderr, "%s: %s\n", cmd, str);
#endif

// note: doing it this way means the client is NOT thread safe!
static pcstrmsock pinst = NULL;

static bool
InitClient(void);

static void
CleanupClient(void);


/*----------------------------------------------------------------------------*/
/* InitClient                                                                 */
/*----------------------------------------------------------------------------*/

static bool
InitClient(void)
{
	const char		*host;
	int			port;

#ifdef dClientWithConfig

#else
	host = "localhost";
	port =  55566;
#endif

	pinst = new cstrmsock;
	if (pinst->Open(host, port)) {
		cdebug_err("InitClient", "Could not open client socket");
		CleanupClient();
		return true;
	}
	cdebug_out("InitClient", "Client instance created");
	return false;
}


/*----------------------------------------------------------------------------*/
/* CleanupClient                                                              */
/*----------------------------------------------------------------------------*/

static void
CleanupClient(void)
{
	if (pinst == NULL)
		return;
	pinst->Close();
	cdebug_out("CleanupClient", "Client socket closed");
	delete pinst;
        pinst = NULL;
}


/*----------------------------------------------------------------------------*/
/* saHpiSessionOpen                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SessionOpen)
	dOpenHpiClientParam(SAHPI_IN SaHpiDomainIdT DomainId,
		SAHPI_OUT SaHpiSessionIdT *SessionId,
		SAHPI_IN  void *SecurityParams) {
        void *request;
        void *reply;  
        SaErrorT err; 

	cdebug_out("saHpiSessionOpen", "start");

	if (SessionId == 0 || SecurityParams != 0)
		return SA_ERR_HPI_INVALID_PARAMS;
	if (InitClient())
		return SA_ERR_HPI_NO_RESPONSE;

	cdebug_out("saHpiSessionOpen", "calling HpiMarshalFind");
        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionOpen);
	cdebug_out("saHpiSessionOpen", "initializing message headers");
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionOpen, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionOpen, 0);
        request = malloc(hm->m_request_len);

	cdebug_out("saHpiSessionOpen", "marshaling request");
        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &DomainId);

	cdebug_out("saHpiSessionOpen", "write request");
        pinst->WriteMsg(request, NULL);
	cdebug_out("saHpiSessionOpen", "read reply");
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

	cdebug_out("saHpiSessionOpen", "demarshaling reply");
        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, SessionId);

	cdebug_out("saHpiSessionOpen", "cleanup");
	if (err != SA_OK)
		CleanupClient();
        if (request)
             free(request);
        if (mr < 0)
             return SA_ERR_HPI_INVALID_PARAMS;

	cdebug_out("saHpiSessionOpen", "end");

        return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSessionClose                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SessionClose)
	dOpenHpiClientParam(SAHPI_IN SaHpiSessionIdT SessionId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionClose);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionClose, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionClose, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply0(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err);

	if (err == SA_OK)
		CleanupClient();
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiDiscover                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(Discover)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDiscover);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiDiscover, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiDiscover, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply0(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}
