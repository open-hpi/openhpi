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
#define cdebug_out(cmd, str)	fprintf(stdout, "\"%s\": %s\n", cmd, str);
#endif

#ifndef dClientDebugErr
#define cdebug_err(cmd, str)
#else
#define cdebug_err(cmd, str)	fprintf(stderr, "\"%s\": %s\n", cmd, str);
#endif

pcstrmsock	clients = NULL;

//temporarily------------
SaHpiSessionIdT	sid = 0;
//-----------------------

static pcstrmsock
InitClient(void);

static void
CleanupClient(pcstrmsock inst);

static pcstrmsock
FindInst(SaHpiSessionIdT sid);

static pcstrmsock
InitClient(void)
{
	const char		*host;
	int			port;
	pcstrmsock		pprevinst = NULL, pinst = clients;

#ifdef dClientWithConfig

#else
	host = "localhost";
	port =  55566;
#endif

	while (pinst != NULL) {
		pprevinst = pinst;
		pinst = pinst->next;
	}
	pinst = new cstrmsock;
	if (clients == NULL)
		clients = pinst;
	if (pprevinst != NULL) {
		pprevinst->next = pinst;
	}
	if (pinst->Open(host, port)) {
		cdebug_err("InitClient", "Could not open client socket");
		CleanupClient(pinst);
		return NULL;
	}
	cdebug_out("InitClient", "Client instance created");
	return pinst;
}

static void
CleanupClient(pcstrmsock inst)
{
	pcstrmsock	pprevinst = NULL, pinst = clients;

	if (inst == NULL)
		return;
	while (pinst != inst) {
		if (pinst == NULL)
			return;
		pprevinst = pinst;
		pinst = pinst->next;
	}
	pinst->Close();
	cdebug_out("CleanupClient", "Client socket closed");
	if (pprevinst != NULL)
		pprevinst->next = pinst->next;
	delete pinst;
}

static pcstrmsock
FindInst(SaHpiSessionIdT s_id)
{
	pcstrmsock		pinst = clients;
	while (pinst != NULL) {
		if (pinst->sid == s_id)
			break;
		pinst = pinst->next;
	}
	return pinst;
}

static SaErrorT
SendCmd(pcstrmsock inst, int cmd)
{
	int rv;

	if (inst == NULL)
		return SA_ERR_HPI_INVALID_PARAMS;
	inst->MessageHeaderInit(eMhMsg, 0, cmd, 20);
	inst->ClientWriteMsg("Clients's command");
	rv = inst->GetErrcode();
	if (rv) {
		cdebug_err("SendCmd", "Sending message failed");
		return SA_ERR_HPI_BUSY;
	}
	return SA_OK;
}


SaErrorT SAHPI_API dOpenHpiClientFunction(SessionOpen)
	dOpenHpiClientParam(SAHPI_IN SaHpiDomainIdT DomainId,
		SAHPI_OUT SaHpiSessionIdT *SessionId,
		SAHPI_IN  void *SecurityParams)
{
	pcstrmsock	pinst;

	if ( SessionId == 0 || SecurityParams != 0 )
		return SA_ERR_HPI_INVALID_PARAMS;

	if ( (pinst = InitClient()) == NULL)
		return SA_ERR_HPI_NO_RESPONSE;

	int err = SendCmd(pinst, eFsaHpiSessionOpen);

	if ( err != SA_OK )
		CleanupClient(pinst);

	sid++;
	pinst->sid = sid;
	*SessionId = sid;

	return err;
}

SaErrorT SAHPI_API dOpenHpiClientFunction(SessionClose)
	dOpenHpiClientParam(SAHPI_IN SaHpiSessionIdT SessionId)
{
	pcstrmsock		pinst = NULL;
	char			str[100], cmd[] = "SaHpiSessionClose";

	pinst = FindInst(SessionId);
	if (pinst == NULL) {
		sprintf(str, "bad session id %d", SessionId);
		cdebug_err(cmd, str);
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	int err = SendCmd(pinst, eFsaHpiSessionClose);
	CleanupClient(pinst);

	return err;
}

SaErrorT SAHPI_API dOpenHpiClientFunction(Discover)
	dOpenHpiClientParam (SAHPI_IN SaHpiSessionIdT SessionId)
{
	pcstrmsock		pinst = NULL;
	char			str[100], cmd[] = "SaHpiDiscover";
	pinst = FindInst(SessionId);
	if (pinst == NULL) {
		sprintf(str, "bad session id %d", SessionId);
		cdebug_err(cmd, str);
		return SA_ERR_HPI_INVALID_PARAMS;
	}
	int err = SendCmd(pinst, eFsaHpiDiscover);
	if ( err != SA_OK ) {
		CleanupClient(pinst);
	}
	return err;
}
