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
#include <SaHpi.h>
#include <oHpi.h>


#include "marshal_hpi.h"
#include "openhpiclient.h"

#define dClientDebugErr
// #define dClientDebug

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

#define SendRecv(cmd) \
	if (pinst->WriteMsg(request)) { \
		cdebug_err(cmd, "WriteMsg failed\n"); \
		if(request) \
			free(request); \
		CleanupClient(); \
		return SA_ERR_HPI_NO_RESPONSE; \
	} \
	if (pinst->ReadMsg(reply)) { \
		cdebug_err(cmd, "Read failed\n"); \
		if(request) \
			free(request); \
		CleanupClient(); \
		return SA_ERR_HPI_NO_RESPONSE; \
	}


// note: doing it this way means the client is NOT thread safe!
static pcstrmsock pinst = NULL;

static bool
InitClient(void);

static void
CleanupClient(void);


/*----------------------------------------------------------------------------*/
/* Utility routines                                                           */
/*----------------------------------------------------------------------------*/

static const char * oh_lookup_severity(SaHpiSeverityT value)
{
        switch (value) {
        case SAHPI_CRITICAL:
                return "CRITICAL";
        case SAHPI_MAJOR:
                return "MAJOR";
        case SAHPI_MINOR:
                return "MINOR";
        case SAHPI_INFORMATIONAL:
                return "INFORMATIONAL";
        case SAHPI_OK:
                return "OK";
        case SAHPI_DEBUG:
                return "DEBUG";
        case SAHPI_ALL_SEVERITIES:
                return "ALL_SEVERITIES";
        default:
                return NULL;
        }
}


static const char * oh_lookup_rdrtype(SaHpiRdrTypeT value)
{
        switch (value) {
        case SAHPI_NO_RECORD:
                return "NO_RECORD";
        case SAHPI_CTRL_RDR:
                return "CTRL_RDR";
        case SAHPI_SENSOR_RDR:
                return "SENSOR_RDR";
        case SAHPI_INVENTORY_RDR:
                return "INVENTORY_RDR";
        case SAHPI_WATCHDOG_RDR:
                return "WATCHDOG_RDR";
        case SAHPI_ANNUNCIATOR_RDR:
                return "ANNUNCIATOR_RDR";
        default:
                return NULL;
        }
}


static const char * oh_lookup_ctrlmode(SaHpiCtrlModeT value)
{
        switch (value) {
        case SAHPI_CTRL_MODE_AUTO:
                return "AUTO";
        case SAHPI_CTRL_MODE_MANUAL:
                return "MANUAL";
        default:
                return NULL;
        }
}


static const char * oh_lookup_ctrlstatedigital(SaHpiCtrlStateDigitalT value)
{
        switch (value) {
        case SAHPI_CTRL_STATE_OFF:
                return "OFF";
        case SAHPI_CTRL_STATE_ON:
                return "ON";
        case SAHPI_CTRL_STATE_PULSE_OFF:
                return "PULSE_OFF";
        case SAHPI_CTRL_STATE_PULSE_ON:
                return "PULSE_ON";
        default:
                return NULL;
        }
}


static const char * oh_lookup_watchdogaction(SaHpiWatchdogActionT value)
{
        switch (value) {
        case SAHPI_WA_NO_ACTION:
                return "NO_ACTION";
        case SAHPI_WA_RESET:
                return "RESET";
        case SAHPI_WA_POWER_DOWN:
                return "POWER_DOWN";
        case SAHPI_WA_POWER_CYCLE:
                return "POWER_CYCLE";
        default:
                return NULL;
        }
}


static const char * oh_lookup_watchdogpretimerinterrupt(SaHpiWatchdogPretimerInterruptT value)
{
        switch (value) {
        case SAHPI_WPI_NONE:
                return "NONE";
        case SAHPI_WPI_SMI:
                return "SMI";
        case SAHPI_WPI_NMI:
                return "NMI";
        case SAHPI_WPI_MESSAGE_INTERRUPT:
                return "MESSAGE_INTERRUPT";
        case SAHPI_WPI_OEM:
                return "OEM";
        default:
                return NULL;
        }
}


static const char * oh_lookup_watchdogtimeruse(SaHpiWatchdogTimerUseT value)
{
        switch (value) {
        case SAHPI_WTU_NONE:
                return "NONE";
        case SAHPI_WTU_BIOS_FRB2:
                return "BIOS_FRB2";
        case SAHPI_WTU_BIOS_POST:
                return "BIOS_POST";
        case SAHPI_WTU_OS_LOAD:
                return "OS_LOAD";
        case SAHPI_WTU_SMS_OS:
                return "SMS_OS";
        case SAHPI_WTU_OEM:
                return "OEM";
        case SAHPI_WTU_UNSPECIFIED:
                return "UNSPECIFIED";
        default:
                return NULL;
        }
}


static const char * oh_lookup_annunciatormode(SaHpiAnnunciatorModeT value)
{
        switch (value) {
        case SAHPI_ANNUNCIATOR_MODE_AUTO:
                return "AUTO";
        case SAHPI_ANNUNCIATOR_MODE_USER:
                return "USER";
        case SAHPI_ANNUNCIATOR_MODE_SHARED:
                return "SHARED";
        default:
                return NULL;
        }
}

static const char * oh_lookup_hsaction(SaHpiHsActionT value)
{
        switch (value) {
        case SAHPI_HS_ACTION_INSERTION:
                return "INSERTION";
        case SAHPI_HS_ACTION_EXTRACTION:
                return "EXTRACTION";
        default:
                return NULL;
        }
}


static const char * oh_lookup_hsindicatorstate(SaHpiHsIndicatorStateT value)
{
        switch (value) {
        case SAHPI_HS_INDICATOR_OFF:
                return "OFF";
        case SAHPI_HS_INDICATOR_ON:
                return "ON";
        default:
                return NULL;
        }
}


static const char * oh_lookup_parmaction(SaHpiParmActionT value)
{
        switch (value) {
        case SAHPI_DEFAULT_PARM:
                return "DEFAULT_PARM";
        case SAHPI_SAVE_PARM:
                return "SAVE_PARM";
        case SAHPI_RESTORE_PARM:
                return "RESTORE_PARM";
        default:
                return NULL;
        }
}


static const char * oh_lookup_resetaction(SaHpiResetActionT value)
{
        switch (value) {
        case SAHPI_COLD_RESET:
                return "COLD_RESET";
        case SAHPI_WARM_RESET:
                return "WARM_RESET";
        case SAHPI_RESET_ASSERT:
                return "RESET_ASSERT";
        case SAHPI_RESET_DEASSERT:
                return "RESET_DEASSERT";
        default:
                return NULL;
        }
}


static const char * oh_lookup_powerstate(SaHpiPowerStateT value)
{
        switch (value) {
        case SAHPI_POWER_OFF:
                return "OFF";
        case SAHPI_POWER_ON:
                return "ON";
        case SAHPI_POWER_CYCLE:
                return "CYCLE";
        default:
                return NULL;
        }
}


static const char * oh_lookup_texttype(SaHpiTextTypeT value)
{
        switch (value) {
        case SAHPI_TL_TYPE_UNICODE:
                return "UNICODE";
        case SAHPI_TL_TYPE_BCDPLUS:
                return "BCDPLUS";
        case SAHPI_TL_TYPE_ASCII6:
                return "ASCII6";
        case SAHPI_TL_TYPE_TEXT:
                return "TEXT";
        case SAHPI_TL_TYPE_BINARY:
                return "BINARY";
        default:
                return NULL;
        }
}


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
/* saHpiVersionGet                                                            */
/*----------------------------------------------------------------------------*/

SaHpiVersionT SAHPI_API dOpenHpiClientFunction(VersionGet)
	dOpenHpiClientParam(void)
{
	void *request = NULL;
	char reply[dMaxMessageLength];
	SaHpiVersionT err;
	char cmd[] = "saHpiVersionget";

        if (pinst == NULL) {
                if (InitClient()) {
                        cdebug_err(cmd, "Client initialization failed");
                        return SA_ERR_HPI_NO_RESPONSE;
                }
        }

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiVersionGet);

	pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiVersionGet, 0);

	SendRecv(cmd);

	int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit,
				    hm, reply + sizeof(cMessageHeader), &err);

	if (mr < 0)
		return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiSessionOpen                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(SessionOpen)
	dOpenHpiClientParam(SAHPI_IN SaHpiDomainIdT   DomainId,
			    SAHPI_OUT SaHpiSessionIdT *SessionId,
			    SAHPI_IN  void            *SecurityParams)
{
	void *request;
	char reply[dMaxMessageLength];
	SaErrorT err;
	char cmd[] = "saHpiSessionOpen";

        if (SessionId == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (SecurityParams != NULL)
		return SA_ERR_HPI_INVALID_PARAMS;
        if (pinst == NULL) {
                if (InitClient()) {
                        return SA_ERR_HPI_NO_RESPONSE;
                }
        }

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionOpen);

	pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSessionOpen,
				 hm->m_request_len);
	request = malloc(hm->m_request_len);

	pinst->header.m_len = HpiMarshalRequest1(hm, request, &DomainId);

	SendRecv(cmd);

	int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit,
				    hm, reply + sizeof(cMessageHeader), &err, SessionId);

	if (request)
		free(request);
	if (err != SA_OK)
		CleanupClient();
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
	void *request;
	char reply[dMaxMessageLength];
	SaErrorT err;
	char cmd[] = "saHpiSessionClose";

	if (SessionId < 0 || pinst == NULL )
		return SA_ERR_HPI_INVALID_PARAMS;

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSessionClose);
	pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSessionClose,
				 hm->m_request_len);
	request = malloc(hm->m_request_len);

	pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

	int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit,
				    hm, reply + sizeof(cMessageHeader), &err);

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
	char reply[dMaxMessageLength];
	SaErrorT err;
	char cmd[] = "saHpiDiscover";

	if (SessionId < 0 || pinst == NULL)
		return SA_ERR_HPI_INVALID_PARAMS;

	cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDiscover);
	pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiDiscover,
				 hm->m_request_len);
	request = malloc(hm->m_request_len);

	pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

	int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit,
				    hm, reply + sizeof(cMessageHeader), &err);

	if (request)
		free(request);
	if (pinst->header.m_type == eMhError)
		return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiDomainInfoGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!DomainInfo)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDomainInfoGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiDomainInfoGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, DomainInfo);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiDrtEntryGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if((DrtEntry == NULL) ||
           (NextEntryId == NULL) ||
           (EntryId == SAHPI_LAST_ENTRY)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDrtEntryGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiDrtEntryGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &EntryId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, NextEntryId, DrtEntry);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiDomainTagSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!DomainTag)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (!oh_lookup_texttype(DomainTag->DataType))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiDomainTagSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiDomainTagSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, DomainTag);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiRptEntryGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if ((NextEntryId == NULL) || (RptEntry == NULL)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (EntryId == SAHPI_LAST_ENTRY) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRptEntryGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &EntryId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, NextEntryId, RptEntry);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiRptEntryGetByResourceId";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID ||
            RptEntry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRptEntryGetByResourceId);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiRptEntryGetByResourceId, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, RptEntry);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceSeveritySet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceSeveritySet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceSeveritySet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Severity);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceTagSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (ResourceTag == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceTagSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceTagSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, ResourceTag);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceIdGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (ResourceId == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceIdGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceIdGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, ResourceId);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogInfoGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Info == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogInfoGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogInfoGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Info);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogEntryGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!PrevEntryId || !EventLogEntry || !NextEntryId ||
            EntryId == SAHPI_NO_MORE_ENTRIES) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogEntryGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &EntryId, Rdr, RptEntry);

	SendRecv(cmd);

        int mr = HpiDemarshalReply5(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, PrevEntryId, NextEntryId, EventLogEntry, Rdr, RptEntry);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
                             SAHPI_IN SaHpiEventT      *EvtEntry)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogEntryAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (EvtEntry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (EvtEntry->EventType != SAHPI_ET_USER ||
            EvtEntry->Source != SAHPI_UNSPECIFIED_RESOURCE_ID) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (!oh_lookup_severity(EvtEntry->Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogEntryAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogEntryAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, EvtEntry);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogClear";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogClear);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogClear, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogTimeGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Time == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogTimeGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Time);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogTimeSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogTimeSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogTimeSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Time);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogStateGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (EnableState == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogStateGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, EnableState);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogStateSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogStateSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogStateSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &EnableState);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventLogOverflowReset";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventLogOverflowReset);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventLogOverflowReset, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSubscribe";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSubscribe);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSubscribe, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiUnsubscribe";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiUnsubscribe);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiUnsubscribe, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Timeout < SAHPI_TIMEOUT_BLOCK || !Event)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &Timeout, Rdr, RptEntry, EventQueueStatus);

	SendRecv(cmd);

        int mr = HpiDemarshalReply4(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Event, Rdr, RptEntry, EventQueueStatus);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiEventAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Event)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiEventAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiEventAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, Event);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAlarmGetNext";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Alarm)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Alarm->AlarmId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_NOT_PRESENT;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmGetNext);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGetNext, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &Severity, &Unack, &Alarm);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Alarm);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAlarmGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Alarm)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &AlarmId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Alarm);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAlarmAcknowledge";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (AlarmId == SAHPI_ENTRY_UNSPECIFIED &&
            !oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmAcknowledge);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAcknowledge, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &AlarmId, &Severity);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAlarmAdd                                                              */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AlarmAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId,
                             SAHPI_INOUT SaHpiAlarmT  *Alarm)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAlarmAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Alarm ||
            !oh_lookup_severity(Alarm->Severity) ||
            Alarm->AlarmCond.Type != SAHPI_STATUS_COND_TYPE_USER)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, Alarm);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Alarm);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAlarmDelete";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (AlarmId == SAHPI_ENTRY_UNSPECIFIED &&
            !oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAlarmDelete);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAlarmDelete, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &AlarmId, &Severity);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiRdrGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (EntryId == SAHPI_LAST_ENTRY || !Rdr || !NextEntryId)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRdrGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &EntryId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, NextEntryId, Rdr);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiRdrGetByInstrumentId";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_rdrtype(RdrType) ||
            RdrType == SAHPI_NO_RECORD || !Rdr)
        {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiRdrGetByInstrumentId);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiRdrGetByInstrumentId, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &RdrType, &InstrumentId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Rdr);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorReadingGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorReadingGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorReadingGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &SensorNum, Reading, EventState);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Reading, EventState);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorThresholdsGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Thresholds)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorThresholdsGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Thresholds);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
                             SAHPI_IN SaHpiSensorThresholdsT *Thresholds)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorThresholdsSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Thresholds)
                return SA_ERR_HPI_INVALID_DATA;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorThresholdsSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorThresholdsSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, Thresholds);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorTypeGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Type || !Category)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorTypeGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorTypeGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Type, Category);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEnableGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Enabled)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEnableGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Enabled);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEnableSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEnableSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEnableSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, &Enabled);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEventEnableGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Enabled)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventEnableGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &SensorNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Enabled);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEventEnableSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventEnableSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventEnableSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &SensorNum, &Enabled);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEventMasksGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Assert)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Deassert)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventMasksGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &SensorNum, Assert, Deassert);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Assert, Deassert);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiSensorEventMasksSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiSensorEventMasksSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiSensorEventMasksSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest6(hm, request, &SessionId, &ResourceId, &SensorNum, &Action, &Assert, &Deassert);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiControlTypeGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Type)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlTypeGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiControlTypeGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &CtrlNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Type);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiControlGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiControlGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &CtrlNum, State);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Mode, State);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiControlSet                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(ControlSet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT  SessionId,
                             SAHPI_IN SaHpiResourceIdT ResourceId,
                             SAHPI_IN SaHpiCtrlNumT    CtrlNum,
                             SAHPI_IN SaHpiCtrlModeT   Mode,
                             SAHPI_IN SaHpiCtrlStateT  *State)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiControlSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_ctrlmode(Mode) ||
            (Mode != SAHPI_CTRL_MODE_AUTO && !State) ||
            (State && State->Type == SAHPI_CTRL_TYPE_DIGITAL &&
             !oh_lookup_ctrlstatedigital(State->StateUnion.Digital)) ||
            (State && State->Type == SAHPI_CTRL_TYPE_STREAM &&
             State->StateUnion.Stream.StreamLength > SAHPI_CTRL_MAX_STREAM_LENGTH))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiControlSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiControlSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &CtrlNum, &Mode, State);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrInfoGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Info == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrInfoGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrInfoGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Idrid);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Info);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaHeaderGet                                                      */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrAreaHeaderGet)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT      SessionId,
                             SAHPI_IN SaHpiResourceIdT     ResourceId,
                             SAHPI_IN SaHpiIdrIdT          Idrid,
                             SAHPI_IN SaHpiIdrAreaTypeT    AreaType,
                             SAHPI_IN SaHpiEntryIdT        AreaId,
                             SAHPI_OUT SaHpiEntryIdT       *NextAreaId,
                             SAHPI_OUT SaHpiIdrAreaHeaderT *Header)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrAreaHeaderGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if ( ((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
             ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
             (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
             (AreaType != SAHPI_IDR_AREATYPE_OEM)) ||
             (AreaId == SAHPI_LAST_ENTRY)||
             (NextAreaId == NULL) ||
             (Header == NULL)))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrAreaHeaderGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaHeaderGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &Idrid, &AreaType, &AreaId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, NextAreaId, Header);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiIdrAreaAdd                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(IdrAreaAdd)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT   SessionId,
                             SAHPI_IN SaHpiResourceIdT  ResourceId,
                             SAHPI_IN SaHpiIdrIdT       Idrid,
                             SAHPI_IN SaHpiIdrAreaTypeT AreaType,
                             SAHPI_OUT SaHpiEntryIdT    *AreaId)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrAreaAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if ( ((AreaType < SAHPI_IDR_AREATYPE_INTERNAL_USE) ||
             ((AreaType > SAHPI_IDR_AREATYPE_PRODUCT_INFO) &&
             (AreaType != SAHPI_IDR_AREATYPE_UNSPECIFIED)  &&
             (AreaType != SAHPI_IDR_AREATYPE_OEM)) ||
             (AreaId == NULL)))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrAreaAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &AreaType);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, AreaId);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrAreaDelete";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (AreaId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrAreaDelete);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrAreaDelete, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrFieldGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if ((((FieldType > SAHPI_IDR_FIELDTYPE_CUSTOM) &&
             (FieldType != SAHPI_IDR_FIELDTYPE_UNSPECIFIED)) ||
             (AreaId == SAHPI_LAST_ENTRY) ||
             (FieldId == SAHPI_LAST_ENTRY) ||
             (NextId == NULL) ||
             (Field == NULL)))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest6(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId, &FieldType, &FieldId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply2(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, NextId, Field);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrFieldAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Field)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, Field);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Field);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrFieldSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Field)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Field->Type > SAHPI_IDR_FIELDTYPE_CUSTOM)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &Idrid, Field);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiIdrFieldDelete";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (FieldId == SAHPI_LAST_ENTRY || AreaId == SAHPI_LAST_ENTRY)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiIdrFieldDelete);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiIdrFieldDelete, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &Idrid, &AreaId, &FieldId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiWatchdogTimerGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Watchdog)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &WatchdogNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Watchdog);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiWatchdogTimerSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Watchdog ||
            (Watchdog && (!oh_lookup_watchdogtimeruse(Watchdog->TimerUse) ||
                          !oh_lookup_watchdogaction(Watchdog->TimerAction) ||
                          !oh_lookup_watchdogpretimerinterrupt(Watchdog->PretimerInterrupt)))) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &WatchdogNum, Watchdog);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiWatchdogTimerReset";

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiWatchdogTimerReset);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiWatchdogTimerReset, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &WatchdogNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* saHpiAnnunciatorGetNext                                                    */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API dOpenHpiClientFunction(AnnunciatorGetNext)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT       SessionId,
                             SAHPI_IN SaHpiResourceIdT      ResourceId,
                             SAHPI_IN SaHpiAnnunciatorNumT  AnnNum,
                             SAHPI_IN SaHpiSeverityT        Severity,
                             SAHPI_IN SaHpiBoolT            Unack,
                             SAHPI_INOUT SaHpiAnnouncementT *Announcement)
{
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorGetNext";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Announcement == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorGetNext);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGetNext, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest6(hm, request, &SessionId, &ResourceId, &AnnNum, &Severity, &Unack, Announcement);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Announcement);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Announcement == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Announcement);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorAcknowledge";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (EntryId == SAHPI_ENTRY_UNSPECIFIED)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorAcknowledge);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAcknowledge, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId, &Severity);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorAdd";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if(Announcement == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorAdd);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorAdd, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, Announcement);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Announcement);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorDelete";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (EntryId == SAHPI_ENTRY_UNSPECIFIED)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_severity(Severity))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorDelete);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorDelete, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest5(hm, request, &SessionId, &ResourceId, &AnnNum, &EntryId, &Severity);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorModeGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Mode == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorModeGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &AnnNum);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Mode);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAnnunciatorModeSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_annunciatormode(Mode))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAnnunciatorModeSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAnnunciatorModeSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest4(hm, request, &SessionId, &ResourceId, &AnnNum, &Mode);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiHotSwapPolicyCancel";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapPolicyCancel);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapPolicyCancel, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceActiveSet";

        if (SessionId < 0 )
             return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceActiveSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceActiveSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceInactiveSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceInactiveSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceInactiveSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAutoInsertTimeoutGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoInsertTimeoutGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &SessionId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Timeout);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAutoInsertTimeoutSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
            Timeout != SAHPI_TIMEOUT_BLOCK &&
            Timeout < 0)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoInsertTimeoutSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAutoInsertTimeoutSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &Timeout);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAutoExtractTimeoutGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Timeout) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoExtractTimeoutGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Timeout);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiAutoExtractTimeoutSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (Timeout != SAHPI_TIMEOUT_IMMEDIATE &&
            Timeout != SAHPI_TIMEOUT_BLOCK &&
            Timeout < 0)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiAutoExtractTimeoutSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiAutoExtractTimeoutSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Timeout);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiHotSwapStateGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!State)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapStateGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapStateGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, State);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiHotSwapActionRequest";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_hsaction(Action))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapActionRequest);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapActionRequest, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiHotSwapIndicatorStateGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!State)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapIndicatorStateGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, State);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiHotSwapIndicatorStateSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_hsindicatorstate(State))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiHotSwapIndicatorStateSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiHotSwapIndicatorStateSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &State);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiParmControl";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_parmaction(Action))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiParmControl);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiParmControl, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceResetStateGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!Action)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceResetStateGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, Action);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourceResetStateSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_resetaction(Action))
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourceResetStateSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourceResetStateSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &Action);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourcePowerStateGet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!State)
                return SA_ERR_HPI_INVALID_PARAMS;
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourcePowerStateGet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateGet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest2(hm, request, &SessionId, &ResourceId);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, State);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
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
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "saHpiResourcePowerStateSet";

        if (SessionId < 0 )
                return SA_ERR_HPI_INVALID_PARAMS;
        if (!oh_lookup_powerstate(State)) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	if (pinst == NULL) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        cHpiMarshal *hm = hm = HpiMarshalFind(eFsaHpiResourcePowerStateSet);
        pinst->MessageHeaderInit(eMhMsg, 0, eFsaHpiResourcePowerStateSet, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest3(hm, request, &SessionId, &ResourceId, &State);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* oHpiPluginLoad                                                             */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiPluginLoad(char *name)
{
        SaHpiTextBufferT buf;
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "oHpiPluginLoad";

        if (strlen(name) + 1 > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        // the following is bogus and not used by the deamon
        buf.DataType = SAHPI_TL_TYPE_TEXT;
        buf.Language = SAHPI_LANG_ENGLISH;
        // the real data
        buf.DataLength = strlen(name);
        strcpy((char *)buf.Data, name);

        cHpiMarshal *hm = hm = HpiMarshalFind(eFoHpiPluginLoad);
        pinst->MessageHeaderInit(eMhMsg, 0, eFoHpiPluginLoad, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &buf);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* oHpiPluginUnload                                                           */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiPluginUnload(char *name)
{
        SaHpiTextBufferT buf;
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "oHpiPluginUnload";

        if (strlen(name) + 1 > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        // the following is bogus and not used by the deamon
        buf.DataType = SAHPI_TL_TYPE_TEXT;
        buf.Language = SAHPI_LANG_ENGLISH;
        // the real data
        buf.DataLength = strlen(name);
        strcpy((char *)buf.Data, name);

        cHpiMarshal *hm = hm = HpiMarshalFind(eFoHpiPluginUnload);
        pinst->MessageHeaderInit(eMhMsg, 0, eFoHpiPluginUnload, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &buf);

	SendRecv(cmd);

        int mr = HpiDemarshalReply0(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* oHpiPluginInfo                                                             */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiPluginInfo(char *name, oHpiPluginInfoT *info)
{
        SaHpiTextBufferT buf;
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "oHpiPluginInfo";

        if (strlen(name) + 1 > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        // the following is bogus and not used by the deamon
        buf.DataType = SAHPI_TL_TYPE_TEXT;
        buf.Language = SAHPI_LANG_ENGLISH;
        // the real data
        buf.DataLength = strlen(name);
        strcpy((char *)buf.Data, name);

        cHpiMarshal *hm = hm = HpiMarshalFind(eFoHpiPluginInfo);
        pinst->MessageHeaderInit(eMhMsg, 0, eFoHpiPluginInfo, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &buf);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, info);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

	return err;
}


/*----------------------------------------------------------------------------*/
/* oHpiPluginGetNext                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiPluginGetNext(char *name, char *next_name, int size)
{
        SaHpiTextBufferT buf, retbuf;
        void *request;
	char reply[dMaxMessageLength];
        SaErrorT err;
	char cmd[] = "oHpiPluginUnload";

        // The following code assumes that SAHPI_MAX_TEXT_BUFFER_LENGTH is
        // always > MAX_PLUGIN_NAME_LENGTH

        if (strlen(name) + 1 > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        // the following is bogus and not used by the deamon
        buf.DataType = SAHPI_TL_TYPE_TEXT;
        buf.Language = SAHPI_LANG_ENGLISH;
        // the real data
        buf.DataLength = strlen(name);
        strcpy((char *)buf.Data, name);

        cHpiMarshal *hm = hm = HpiMarshalFind(eFoHpiPluginGetNext);
        pinst->MessageHeaderInit(eMhMsg, 0, eFoHpiPluginGetNext, hm->m_request_len);
        request = malloc(hm->m_request_len);

        pinst->header.m_len = HpiMarshalRequest1(hm, request, &buf);

	SendRecv(cmd);

        int mr = HpiDemarshalReply1(pinst->header.m_flags & dMhEndianBit, hm, reply + sizeof(cMessageHeader), &err, &retbuf);

        if (request)
                free(request);
        if (pinst->header.m_type == eMhError)
                return SA_ERR_HPI_INVALID_PARAMS;
        if (mr < 0)
                return SA_ERR_HPI_INVALID_PARAMS;

        if (size - 1 > retbuf.DataLength) {
                strncpy(next_name, (char *)retbuf.Data, size - 1);
                next_name[size - 1] = '\0';
        } else {
                strncpy(next_name, (char *)retbuf.Data, retbuf.DataLength);
                next_name[retbuf.DataLength] = '\0';
        }

	return err;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerCreate                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiHandlerCreate(GHashTable *config,
                           oHpiHandlerIdT *id)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerDestroy                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiHandlerDestroy(oHpiHandlerIdT id)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerInfo                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiHandlerInfo(oHpiHandlerIdT id, oHpiHandlerInfoT *info)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerGetNext                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiHandlerGetNext(oHpiHandlerIdT id, oHpiHandlerIdT *next_id)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/*----------------------------------------------------------------------------*/
/* oHpiGlobalParamGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiGlobalParamGet(oHpiGlobalParamT *param)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/*----------------------------------------------------------------------------*/
/* oHpiGlobalParamSet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT oHpiGlobalParamSet(oHpiGlobalParamT *param)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}
