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
 *     Renier Morales <renierm@users.sf.net>
 */
 
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
        char buffer[128];
        
        if (saHpiSessionOpen(1, &sid, NULL))
                return -1;
                
        if (oHpiGlobalParamGet("OPENHPI_ON_EP", buffer, 128))
                return -1;
                
        if (strcmp("{SYSTEM_CHASSIS,1}", buffer))
                return -1;
                
        return 0;
}
