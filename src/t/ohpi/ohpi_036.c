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
 * Set a paramter, get it, and compare values.
 * Tests without opening a session to see if the global
 * parameter table is initialized correctly anyway.
 * Pass on success, otherwise a failure.
 **/
 
int main(int argc, char **argv)
{
        char *config_file = NULL;
        char buffer[128];
        
        /* Save config file env variable and unset it */
        config_file = getenv("OPENHPI_CONF");
        setenv("OPENHPI_CONF","./noconfig", 1);
                
        if (oHpiGlobalParamSet("OPENHPI_ON_EP", "{SYSTEM_CHASSIS,102}"))
                return -1;
                
        if (oHpiGlobalParamGet("OPENHPI_ON_EP", buffer, 128))
                return -1;
                
        if (strcmp("{SYSTEM_CHASSIS,102}", buffer))
                return -1;
                
        
        return 0;
}
