/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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

#include <string.h>
#include <glib.h>
#include <config.h>

extern "C"
{
#include <SaHpi.h>
#include <oHpi.h>
#include <oh_error.h>
}

#include "marshal_hpi.h"

/*----------------------------------------------------------------------------*/
/* Macros                                                                     */
/*----------------------------------------------------------------------------*/

#define SendRecv(sid, cmd) \
	if (pinst->WriteMsg(request)) { \
		client_err(cmd, "WriteMsg failed\n"); \
		if(request) \
			free(request); \
                if (sid) \
                        oh_remove_connx(sid); \
                else \
                        oh_delete_connx(pinst); \
		return SA_ERR_HPI_NO_RESPONSE; \
	} \
	if (pinst->ReadMsg(reply)) { \
		client_err(cmd, "Read failed\n"); \
		if(request) \
			free(request); \
                if (sid) \
                	oh_remove_connx(sid); \
                else \
                	oh_delete_connx(pinst); \
		return SA_ERR_HPI_NO_RESPONSE; \
	}

#define SendRecvNoReturn(cmd) \
	if (pinst->WriteMsg(request)) { \
		client_err(cmd, "WriteMsg failed\n"); \
		if(request) \
			free(request); \
		oh_delete_connx(pinst); \
		return; \
	} \
	if (pinst->ReadMsg(reply)) { \
		client_err(cmd, "Read failed\n"); \
		if(request) \
			free(request); \
		oh_delete_connx(pinst); \
		return; \
	}

#endif /* __OH_CLIENT_H */
