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
#include <SaHpi.h>
#include <oHpi.h>

/**
 * Load 'libdummy', get plugin info, compare it with the known
 * value (0) , and unload the plugins.
 * Pass on success, otherwise failure.
 **/
 
int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        oHpiPluginInfoT pinfo;
        
        setenv("OPENHPI_CONF","./noconfig", 1);
        
        if (saHpiSessionOpen(1, &sid, NULL))
                return -1;
                    
        if (oHpiPluginLoad("libdummy"))
                return -1;
                
        if (oHpiPluginInfo("libdummy",&pinfo))
                return -1;
                
        if (pinfo.refcount != 0)
                return -1;                
                
        
        return oHpiPluginUnload("libdummy");
}
