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
 * Authors:
 *     Renier Morales <renier@openhpi.org>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SaHpi.h>
#include <oHpi.h>

/**
 * Set OPENHPI_CONF to valid file. Open session. Get parameter
 * that was set through config file comparing with known value.
 * Pass on success, otherwise a failure.
 **/
 
int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        oHpiGlobalParamT param = { .Type = OHPI_LOG_ON_SEV };
                
	if (saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL)) {
		printf("Could not open session\n");
                return -1;
	}
                
        if (oHpiGlobalParamGet(sid, &param)) {
		printf("Could not get parameter\n");
                return -1;
	}
                
        if (param.u.LogOnSev != SAHPI_MAJOR)
                return -1;
                
        return 0;
}
