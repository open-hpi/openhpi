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

#define SendRecv() \
	if (pinst->WriteMsg(request, NULL)) { \
		cdebug_err(cmd, "WriteMsg failed"); \
		if(request) \
			free(request); \
		CleanupClient(); \
		return SA_ERR_HPI_NO_RESPONSE; \
	} \
	 \
        if (pinst->ReadMsg() == NULL) { \
		cdebug_err(cmd, "ReadMsg failed"); \
		if (request) \
			free(request); \
		CleanupClient(); \
		return SA_ERR_HPI_NO_RESPONSE; \
	} \
	reply = (char *)request + (sizeof(cMessageHeader) * 2) + \
	pinst->reqheader.m_len;


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
	const char		*host, *portstr;
	int			port;

        host = getenv("OPENHPI_DAEMON_HOST");
        if (host == NULL) {
                host = "localhost";
        }
        portstr = getenv("OPENHPI_DAEMON_HOST");
        if (portstr == NULL) {
                port =  4743;
        }
        else {
                port =  atoi(portstr);
        }

	if (!pinst)
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
			    SAHPI_IN  void *SecurityParams)
{
	void				*request;
	void				*reply;  
	SaErrorT			err; 

	char				cmd[] = "saHpiSessionOpen";

	cdebug_out(cmd, "start");

	if (SessionId == 0 || SecurityParams != 0) {
		cdebug_err(cmd, "invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	};
	if (InitClient()) {
		cdebug_err(cmd, "client initialization failed");
		return SA_ERR_HPI_NO_RESPONSE;
	}

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionOpen);

	pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionOpen,
				    hm->m_request_len);
	pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionOpen, 0);
	request = malloc(hm->m_request_len);

	pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &DomainId);

	SendRecv();

	int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit,
				    hm, reply, &err, SessionId);

	cdebug_out(cmd, "end");

	if (err != SA_OK)
		CleanupClient();
	if (request)
		free(request);
	if (mr < 0)
		return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSessionClose                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SessionClose)
	dOpenHpiClientParam(SAHPI_IN SaHpiSessionIdT SessionId)
{
	void				*request;
	void				*reply;  
	SaErrorT			err; 

	char				cmd[] = "saHpiSessionClose";

	cdebug_out(cmd, "start");

	if (SessionId < 0 || pinst == NULL ) {
		cdebug_err(cmd, "invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionClose);
	pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionClose,
				    hm->m_request_len);
	pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSessionClose, 0);
	request = malloc(hm->m_request_len);

	pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv();

	int mr = HpiDemarshalReply0(pinst->repheader.m_flags & dMhEndianBit,
				    hm, reply, &err);

	cdebug_out(cmd, "end");

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
	void				*request;
	void				*reply;  
	SaErrorT			err; 

	char				cmd[] = "sHpiDiscover";

	cdebug_out(cmd, "start");

	if (SessionId < 0 || pinst == NULL) {
		cdebug_err(cmd, "invalid parameters");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDiscover);
	pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiDiscover,
				    hm->m_request_len);
	pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiDiscover, 0);
	request = malloc(hm->m_request_len);

	pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv();

	int mr = HpiDemarshalReply0(pinst->repheader.m_flags & dMhEndianBit,
				    hm, reply, &err);

	cdebug_out(cmd, "end");

	if (pinst->repheader.m_type == eMhError)
		return SA_ERR_HPI_INVALID_PARAMS;
	if (request)
		free(request);
	if (mr < 0)
		return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiDomainInfoGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(DomainInfoGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_OUT SaHpiDomainInfoT *DomainInfo)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDomainInfoGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiDomainInfoGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiDomainInfoGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &DomainInfo);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiDrtEntryGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(DrtEntryGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiEntryIdT   EntryId,
                             SAHPI_OUT SaHpiEntryIdT  *NextEntryId,
                             SAHPI_OUT SaHpiDrtEntryT *DrtEntry)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDrtEntryGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiDrtEntryGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiDrtEntryGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &EntryId);

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


/*----------------------------------------------------------------------------*/
/* saHpiDomainTagSet                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(DomainTagSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiTextBufferT *DomainTag)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDomainTagSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiDomainTagSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiDomainTagSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &DomainTag);

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


/*----------------------------------------------------------------------------*/
/* saHpiRptEntryGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(RptEntryGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiEntryIdT    EntryId,
                             SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
                             SAHPI_OUT SaHpiRptEntryT  *RptEntry)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRptEntryGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &EntryId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &NextEntryId, &RptEntry);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiRptEntryGetByResourceId                                               */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(RptEntryGetByResourceId)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_OUT SaHpiRptEntryT  *RptEntry)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRptEntryGetByResourceId);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGetByResourceId, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGetByResourceId, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &RptEntry);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceSeveritySet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceSeveritySet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSeverityT   Severity)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceSeveritySet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceSeveritySet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceSeveritySet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Severity);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourceTagSet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceTagSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceTagSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceTagSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceTagSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &ResourceTag);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourceIdGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceIdGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceIdGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceIdGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceIdGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &ResourceId);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogInfoGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogInfoGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT     SessionId,
                             SAHPI_IN SaHpiResourceIdT    ResourceId,
                             SAHPI_OUT SaHpiEventLogInfoT *Info)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogInfoGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogInfoGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogInfoGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Info);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogEntryGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogEntryGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT        SessionId,
                             SAHPI_IN SaHpiResourceIdT       ResourceId,
                             SAHPI_IN SaHpiEntryIdT          EntryId,
                             SAHPI_OUT SaHpiEventLogEntryIdT *PrevEntryId,
                             SAHPI_OUT SaHpiEventLogEntryIdT *NextEntryId,
                             SAHPI_OUT SaHpiEventLogEntryT   *EventLogEntry,
                             SAHPI_INOUT SaHpiRdrT           *Rdr,
                             SAHPI_INOUT SaHpiRptEntryT      *RptEntry)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogEntryGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &EntryId, &Rdr, &RptEntry);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply5(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &PrevEntryId, &NextEntryId, EventLogEntry, &Rdr, &RptEntry);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogEntryAdd                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogEntryAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiRptEntryT   *RptEntry)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogEntryAdd);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryAdd, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryAdd, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &RptEntry);

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


/*----------------------------------------------------------------------------*/
/* saHpiEventLogClear                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogClear)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogClear);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogClear, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogClear, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

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


/*----------------------------------------------------------------------------*/
/* saHpiEventLogTimeGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogTimeGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_OUT SaHpiTimeT      *Time)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogTimeGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Time);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogTimeSet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogTimeSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiTimeT       Time)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogTimeSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Time);

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


/*----------------------------------------------------------------------------*/
/* saHpiEventLogStateGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogStateGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_OUT SaHpiBoolT      *EnableState)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogStateGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &EnableState);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventLogStateSet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogStateSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiBoolT       EnableState)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogStateSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &EnableState);

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


/*----------------------------------------------------------------------------*/
/* saHpiEventLogOverflowReset                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogOverflowReset)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogOverflowReset);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogOverflowReset, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogOverflowReset, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

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


/*----------------------------------------------------------------------------*/
/* saHpiSubscribe                                                             */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(Subscribe)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSubscribe);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSubscribe, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSubscribe, 0);
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


/*----------------------------------------------------------------------------*/
/* saHpiUnsSubscribe                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(Unsubscribe)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiUnsubscribe);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiUnsubscribe, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiUnsubscribe, 0);
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


/*----------------------------------------------------------------------------*/
/* saHpiEventGet                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT         SessionId,
                             SAHPI_IN SaHpiTimeoutT           Timeout,
                             SAHPI_OUT SaHpiEventT            *Event,
                             SAHPI_INOUT SaHpiRdrT            *Rdr,
                             SAHPI_INOUT SaHpiRptEntryT       *RptEntry,
                             SAHPI_INOUT SaHpiEvtQueueStatusT *EventQueueStatus)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &Timeout, &Rdr, &RptEntry, &EventQueueStatus);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply4(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Event, &Rdr, &RptEntry, &EventQueueStatus);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiEventAdd                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(EventAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiEventT     *Event)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventAdd);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiEventAdd, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiEventAdd, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &Event);

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


/*----------------------------------------------------------------------------*/
/* saHpiAlarmGetNext                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmGetNext)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiSeverityT  Severity,
                             SAHPI_IN SaHpiBoolT      Unack,
                             SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmGetNext);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGetNext, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGetNext, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &Severity, &Unack, &Alarm);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Alarm);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmGet                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiAlarmIdT   AlarmId,
                             SAHPI_OUT SaHpiAlarmT    *Alarm)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &AlarmId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Alarm);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmAcknowledge                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmAcknowledge)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiAlarmIdT   AlarmId,
                             SAHPI_IN SaHpiSeverityT  Severity)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmAcknowledge);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAcknowledge, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAcknowledge, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &AlarmId, &Severity);

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


/*----------------------------------------------------------------------------*/
/* saHpiAlarmAdd                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_OUT SaHpiAlarmT    *Alarm)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmAdd);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAdd, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAdd, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &Alarm);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Alarm);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmDelete                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmDelete)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiAlarmIdT   AlarmId,
                             SAHPI_IN SaHpiSeverityT  Severity)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmDelete);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmDelete, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmDelete, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &AlarmId, &Severity);

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


/*----------------------------------------------------------------------------*/
/* saHpiRdrGet                                                                */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(RdrGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiEntryIdT    EntryId,
                             SAHPI_OUT SaHpiEntryIdT   *NextEntryId,
                             SAHPI_OUT SaHpiRdrT       *Rdr)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRdrGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &EntryId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &NextEntryId, &Rdr);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiRdrGetByInstrumentId                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(RdrGetByInstrumentId)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_IN SaHpiRdrTypeT      RdrType,
                             SAHPI_IN SaHpiInstrumentIdT InstrumentId,
                             SAHPI_OUT SaHpiRdrT         *Rdr)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRdrGetByInstrumentId);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGetByInstrumentId, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGetByInstrumentId, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &RdrType, &InstrumentId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Rdr);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorReadingGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorReadingGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT        SessionId,
                             SAHPI_IN SaHpiResourceIdT       ResourceId,
                             SAHPI_IN SaHpiSensorNumT        SensorNum,
                             SAHPI_INOUT SaHpiSensorReadingT *Reading,
                             SAHPI_INOUT SaHpiEventStateT    *EventState)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorReadingGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorReadingGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorReadingGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &SensorNum, &Reading, &EventState);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Reading, &EventState);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorThresholdsGet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorThresholdsGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT         SessionId,
                             SAHPI_IN SaHpiResourceIdT        ResourceId,
                             SAHPI_IN SaHpiSensorNumT         SensorNum,
                             SAHPI_OUT SaHpiSensorThresholdsT *Thresholds)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorThresholdsGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Thresholds);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorThresholdsSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorThresholdsSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT        SessionId,
                             SAHPI_IN SaHpiResourceIdT       ResourceId,
                             SAHPI_IN SaHpiSensorNumT        SensorNum,
                             SAHPI_IN SaHpiSensorThresholdsT Thresholds)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorThresholdsSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, &Thresholds);

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


/*----------------------------------------------------------------------------*/
/* saHpiSensorTypeGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorTypeGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiSensorNumT      SensorNum,
                             SAHPI_OUT SaHpiSensorTypeT    *Type,
                             SAHPI_OUT SaHpiEventCategoryT *Category)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorTypeGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorTypeGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorTypeGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Type, &Category);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEnableGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEnableGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSensorNumT  SensorNum,
                             SAHPI_OUT SaHpiBoolT      *Enabled)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEnableGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Enabled);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEnableSet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEnableSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSensorNumT  SensorNum,
                             SAHPI_IN SaHpiBoolT       Enabled)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEnableSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, &Enabled);

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


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventEnableGet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventEnableGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSensorNumT  SensorNum,
                             SAHPI_OUT SaHpiBoolT      *Enabled)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventEnableGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Enabled);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventEnableSet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventEnableSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSensorNumT  SensorNum,
                             SAHPI_IN SaHpiBoolT       Enabled)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventEnableSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, &Enabled);

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


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventMasksGet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventMasksGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiSensorNumT  SensorNum,
                             SAHPI_INOUT SaHpiEventStateT *Assert,
                             SAHPI_INOUT SaHpiEventStateT *Deassert)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventMasksGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &SensorNum, &Assert, &Deassert);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Assert, &Deassert);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSensorEventMasksSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventMasksSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT             SessionId,
                             SAHPI_IN SaHpiResourceIdT            ResourceId,
                             SAHPI_IN SaHpiSensorNumT             SensorNum,
                             SAHPI_IN SaHpiSensorEventMaskActionT Action,
                             SAHPI_IN SaHpiEventStateT            Assert,
                             SAHPI_IN SaHpiEventStateT            Deassert)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventMasksSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest6(hm, request, &SessionId, &ResourceId, &SensorNum, &Action, &Assert, &Deassert);

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


/*----------------------------------------------------------------------------*/
/* saHpiControlTypeGet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ControlTypeGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiCtrlNumT    CtrlNum,
                             SAHPI_OUT SaHpiCtrlTypeT  *Type)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlTypeGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiControlTypeGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiControlTypeGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &CtrlNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Type);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlGet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ControlGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_IN SaHpiCtrlNumT      CtrlNum,
                             SAHPI_OUT SaHpiCtrlModeT    *Mode,
                             SAHPI_INOUT SaHpiCtrlStateT *State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiControlGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiControlGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &CtrlNum, &State);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Mode, &State);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlSet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ControlSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_IN SaHpiCtrlNumT      CtrlNum,
                             SAHPI_OUT SaHpiCtrlModeT    Mode,
                             SAHPI_INOUT SaHpiCtrlStateT *State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiControlSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiControlSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &CtrlNum, &Mode, &State);

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


/*----------------------------------------------------------------------------*/
/* saHpiIdrInfoGet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrInfoGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiIdrIdT      Idrid,
                             SAHPI_OUT SaHpiIdrInfoT   *Info)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrInfoGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrInfoGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrInfoGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Idrid);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Info);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaHeaderGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrAreaHeaderGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiIdrIdT       Idrid,
                             SAHPI_IN SaHpiIdrAreaTypeT AreaType,
                             SAHPI_OUT SaHpiEntryIdT    *AreaId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrAreaHeaderGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaHeaderGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaHeaderGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &AreaType);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &AreaId);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaDelete                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrAreaDelete)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiIdrIdT       Idrid,
                             SAHPI_IN SaHpiEntryIdT     AreaId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrAreaDelete);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaDelete, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaDelete, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId);

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


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldGet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrFieldGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_IN SaHpiIdrIdT        Idrid,
                             SAHPI_IN SaHpiEntryIdT      AreaId,
                             SAHPI_IN SaHpiIdrFieldTypeT FieldType,
                             SAHPI_IN SaHpiEntryIdT      FieldId,
                             SAHPI_OUT SaHpiEntryIdT     *NextId,
                             SAHPI_OUT SaHpiIdrFieldT    *Field)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest6(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId, &FieldType, &FieldId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply2(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &NextId, &Field);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldAdd                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrFieldAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_IN SaHpiIdrIdT        Idrid,
                             SAHPI_INOUT SaHpiIdrFieldT  *Field)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldAdd);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldAdd, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldAdd, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &Field);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Field);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldSet                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrFieldSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiIdrIdT      Idrid,
                             SAHPI_IN SaHpiIdrFieldT   *Field)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &Field);

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


/*----------------------------------------------------------------------------*/
/* saHpiIdrFieldDelete                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrFieldDelete)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiIdrIdT      Idrid,
                             SAHPI_IN SaHpiEntryIdT    AreaId,
                             SAHPI_IN SaHpiEntryIdT    FieldId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldDelete);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldDelete, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldDelete, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId, &FieldId);

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


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                             SAHPI_OUT SaHpiWatchdogT   *Watchdog)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &WatchdogNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Watchdog);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerSet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
                             SAHPI_IN SaHpiWatchdogT    *Watchdog)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &WatchdogNum, &Watchdog);

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


/*----------------------------------------------------------------------------*/
/* saHpiWatchdogTimerReset                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerReset)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerReset);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerReset, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerReset, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &WatchdogNum);

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


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorGetNext                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorGetNext)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
                             SAHPI_IN SaHpiSeverityT       Severity,
                             SAHPI_IN SaHpiBoolT           Unack,
                             SAHPI_OUT SaHpiAnnouncementT  *Announcement)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorGetNext);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGetNext, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGetNext, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &AnnNum, &Severity, &Unack);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Announcement);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorGet                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
                             SAHPI_IN SaHpiEntryIdT        EntryId,
                             SAHPI_OUT SaHpiAnnouncementT  *Announcement)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Announcement);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorAcknowledge                                                */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorAcknowledge)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
                             SAHPI_IN SaHpiEntryIdT        EntryId,
                             SAHPI_IN SaHpiSeverityT       Severity)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorAcknowledge);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAcknowledge, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAcknowledge, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId, &Severity);

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


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorAdd                                                        */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
                             SAHPI_INOUT SaHpiAnnouncementT *Announcement)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorAdd);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAdd, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAdd, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, &Announcement);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Announcement);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorDelete                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorDelete)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT AnnNum,
                             SAHPI_IN SaHpiEntryIdT        EntryId,
                             SAHPI_IN SaHpiSeverityT       Severity)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorDelete);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorDelete, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorDelete, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId, &Severity);

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


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorModeGet                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorModeGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT        SessionId,
                             SAHPI_IN SaHpiResourceIdT       ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT   AnnNum,
                             SAHPI_OUT SaHpiAnnunciatorModeT *Mode)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorModeGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &AnnNum);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Mode);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorModeSet                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorModeSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
                             SAHPI_IN SaHpiAnnunciatorModeT Mode)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorModeSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, &Mode);

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


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapPolicyCancel                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapPolicyCancel)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapPolicyCancel);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapPolicyCancel, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapPolicyCancel, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourceActiveSet                                                     */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceActiveSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceActiveSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceActiveSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceActiveSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourceInactiveSet                                                   */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceInactiveSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceInactiveSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceInactiveSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceInactiveSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

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


/*----------------------------------------------------------------------------*/
/* saHpiAutoInsertTimeoutGet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AutoInsertTimeoutGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_OUT SaHpiTimeoutT  *Timeout)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoInsertTimeoutGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest1(hm, request, &SessionId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Timeout);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoInsertTimeoutSet                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AutoInsertTimeoutSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_IN SaHpiTimeoutT   Timeout)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoInsertTimeoutSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &Timeout);

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


/*----------------------------------------------------------------------------*/
/* saHpiAutoExtractTimeoutGet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AutoExtractTimeoutGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_OUT SaHpiTimeoutT   *Timeout)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoExtractTimeoutGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Timeout);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAutoExtractTimeoutSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AutoExtractTimeoutSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiTimeoutT    Timeout)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoExtractTimeoutSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Timeout);

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


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapStateGet                                                       */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapStateGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_OUT SaHpiHsStateT   *State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapStateGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapStateGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapStateGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &State);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapActionRequest                                                  */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapActionRequest)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiHsActionT   Action)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapActionRequest);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapActionRequest, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapActionRequest, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

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


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapIndicatorStateGet                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapIndicatorStateGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT         SessionId,
                             SAHPI_IN SaHpiResourceIdT        ResourceId,
                             SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapIndicatorStateGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &State);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiHotSwapIndicatorStateSet                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapIndicatorStateSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT        SessionId,
                             SAHPI_IN SaHpiResourceIdT       ResourceId,
                             SAHPI_IN SaHpiHsIndicatorStateT State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapIndicatorStateSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &State);

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


/*----------------------------------------------------------------------------*/
/* saHpiParmControl                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ParmControl)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiParmActionT Action)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiParmControl);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiParmControl, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiParmControl, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourceResetStateGet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceResetStateGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT    SessionId,
                             SAHPI_IN SaHpiResourceIdT   ResourceId,
                             SAHPI_OUT SaHpiResetActionT *Action)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceResetStateGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &Action);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourceResetStateSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceResetStateSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiResetActionT Action)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceResetStateSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

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


/*----------------------------------------------------------------------------*/
/* saHpiResourcePowerStateGet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourcePowerStateGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_OUT SaHpiPowerStateT *State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourcePowerStateGet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateGet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateGet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

        pinst->WriteMsg(request, NULL);
        pinst->ReadMsg();
        reply = (char *)request + (sizeof(cMessageHeader) * 2) + pinst->reqheader.m_len;

        int mr = HpiDemarshalReply1(pinst->repheader.m_flags & dMhEndianBit, hm, reply, &err, &State);

        if (pinst->repheader.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (request)
             free(request);
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiResourcePowerStateSet                                                 */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ResourcePowerStateSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiPowerStateT State)
{
        void *request;
        void *reply;  
        SaErrorT err; 

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourcePowerStateSet);
        pinst->ReqMessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateSet, hm->m_request_len);
        pinst->RepMessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateSet, 0);
        request = malloc(hm->m_request_len);

        pinst->reqheader.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &State);

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
