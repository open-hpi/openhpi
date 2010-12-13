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

#ifndef __OH_CLIENT_SESSION_H
#define __OH_CLIENT_SESSION_H

#include <stdint.h>

#include <SaHpi.h>


struct Params
{
    explicit Params( void * p2 = 0, void * p3 = 0, void * p4 = 0,
                     void * p5 = 0, void * p6 = 0 )
    {
        array[0] = 0;
        array[1] = p2;
        array[2] = p3;
        array[3] = p4;
        array[4] = p5;
        array[5] = p6;
    }

    void SetFirst( void * p1 )
    {
        array[0] = p1;
    }

    union {
        void * array[6];
        const void * const_array[6];
    };
};


void ohc_sess_init();
SaErrorT ohc_sess_open( SaHpiDomainIdT did, SaHpiSessionIdT& sid );
SaErrorT ohc_sess_close( SaHpiSessionIdT sid );
SaErrorT ohc_sess_rpc( uint32_t id,
                       SaHpiSessionIdT sid,
                       Params& iparams,
                       Params& oparams );
SaErrorT ohc_sess_get_did( SaHpiSessionIdT sid, SaHpiDomainIdT& did );


#endif /* __OH_CLIENT_SESSION_H */

