/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2007
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
 *      Renier Morales <renier@openhpi.org>
 *
 */

#ifndef __OH_CLIENT_H
#define __OH_CLIENT_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <glib.h>
#include <config.h>
#include "strmsock.h"

extern "C"
{
#include <SaHpi.h>
#include <oHpi.h>
#include <oh_error.h>
}

#include "marshal_hpi.h"

/*----------------------------------------------------------------------------*/
/* Global Vraiables                                                           */
/*----------------------------------------------------------------------------*/

static GHashTable *sessions = NULL;
static GStaticRecMutex sessions_sem = G_STATIC_REC_MUTEX_INIT;

/*----------------------------------------------------------------------------*/
/* Macros                                                                     */
/*----------------------------------------------------------------------------*/

#define client_dbg(cmd, str) dbg("%s: %s\n", cmd, str)
#define client_err(cmd, str) err("%s: %s\n", cmd, str)

#define SendRecv(sid, cmd) \
	if (pinst->WriteMsg(request)) { \
		client_err(cmd, "WriteMsg failed\n"); \
		if(request) \
			free(request); \
                if (sid) \
                        RemoveOneConnx(sid); \
                else \
                        DeleteConnx(pinst); \
		return SA_ERR_HPI_NO_RESPONSE; \
	} \
	if (pinst->ReadMsg(reply)) { \
		client_err(cmd, "Read failed\n"); \
		if(request) \
			free(request); \
                if (sid) \
                        RemoveOneConnx(sid); \
                else \
                        DeleteConnx(pinst); \
		return SA_ERR_HPI_NO_RESPONSE; \
	}

#define SendRecvNoReturn(cmd) \
	if (pinst->WriteMsg(request)) { \
		client_err(cmd, "WriteMsg failed\n"); \
		if(request) \
			free(request); \
                DeleteConnx(pinst); \
		return; \
	} \
	if (pinst->ReadMsg(reply)) { \
		client_err(cmd, "Read failed\n"); \
		if(request) \
			free(request); \
                DeleteConnx(pinst); \
		return; \
	}

#endif /* __OH_CLIENT_H */
