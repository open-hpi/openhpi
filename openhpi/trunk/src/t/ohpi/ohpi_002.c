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
 * Load the dummy plugin, unload it, and load it again.
 * Test without opening a session. Opening a handler should
 * initialize the library.
 * Pass on success, otherwise a failure.
 **/
 
int main(int argc, char **argv)
{
        
        setenv("OPENHPI_CONF","./noconfig", 1);        
                    
        if (oHpiPluginLoad("libdummy"))
                return -1;
        
        if (oHpiPluginUnload("libdummy"))
                return -1;
        
        
        return oHpiPluginLoad("libdummy");
}
