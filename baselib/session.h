/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
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
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __BASELIB_SESSION_H
#define __BASELIB_SESSION_H

#include <stdint.h>

#include <SaHpi.h>

#include <oh_rpc_params.h>


void ohc_sess_init();
SaErrorT ohc_sess_open( SaHpiDomainIdT did, SaHpiSessionIdT& sid );
SaErrorT ohc_sess_close( SaHpiSessionIdT sid );
SaErrorT ohc_sess_close_all();
SaErrorT ohc_sess_rpc( uint32_t id,
                       SaHpiSessionIdT sid,
                       ClientRpcParams& iparams,
                       ClientRpcParams& oparams );
SaErrorT ohc_sess_get_did( SaHpiSessionIdT sid, SaHpiDomainIdT& did );
SaErrorT ohc_sess_get_entity_root( SaHpiSessionIdT sid, SaHpiEntityPathT& ep );

#endif /* __BASELIB_SESSION_H */

