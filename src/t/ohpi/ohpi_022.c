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
 * Load 'libdummy', load 'libwatchdog', 
 * and call oHpiPluginGetNext passing a bogus plugin.
 * Unload the plugins.
 * Pass on error, otherwise test failed.
 **/
 
#define PLUGIN_NAME_SIZE 32
 
int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        char *config_file = NULL;
        char next_plugin[PLUGIN_NAME_SIZE];
        
        /* Save config file env variable and unset it */
        config_file = getenv("OPENHPI_CONF");
        setenv("OPENHPI_CONF","./noconfig", 1);
        
        if (saHpiSessionOpen(1, &sid, NULL))
                return -1;
                    
        /* Load plugins */
        if (oHpiPluginLoad("libdummy"))
                return -1;
                
        if (oHpiPluginLoad("libwatchdog"))
                return -1;
                
        next_plugin[0] = '\0';
        if (!oHpiPluginGetNext("bogusplugin", next_plugin, PLUGIN_NAME_SIZE))
                return -1;
                
        if (oHpiPluginUnload("libwatchdog"))
                return -1;

        /* Restore config file env variable */
        setenv("OPENHPI_CONF",config_file,1);                           
        
        return oHpiPluginUnload("libdummy");
}
