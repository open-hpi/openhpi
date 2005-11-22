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
 * Load 'libsimulator', load 'libwatchdog', iterate through the names
 * and make sure they match with the expected values.
 * Unload the plugins.
 * Pass on success, otherwise failure.
 **/
 
#define PLUGIN_NAME_SIZE 32
 
int main(int argc, char **argv)
{
        SaHpiSessionIdT sid = 0;
        char next_plugin[PLUGIN_NAME_SIZE];
        
        setenv("OPENHPI_CONF","./noconfig", 1);
        
        if (saHpiSessionOpen(1, &sid, NULL))
                return -1;
                    
        /* Load plugins */
        if (oHpiPluginLoad("libsimulator"))
                return -1;
                
        if (oHpiPluginLoad("libwatchdog"))
                return -1;
                
        /* Iterate through all loaded plugins and check them */
        next_plugin[0] = '\0';
        oHpiPluginGetNext(NULL, next_plugin, PLUGIN_NAME_SIZE);
        if (strcmp("libsimulator", next_plugin))
                return -1;
                        
        next_plugin[0] = '\0';
        oHpiPluginGetNext("libsimulator", next_plugin, PLUGIN_NAME_SIZE);
        if (strcmp("libwatchdog", next_plugin))
                return -1;
                
        /* Cannot be anymore plugins after libwatchdog */
        if (!oHpiPluginGetNext("libwatchdog", next_plugin, PLUGIN_NAME_SIZE))
                return -1;
        
        if (oHpiPluginUnload("libwatchdog"))
                return -1;

        
        return oHpiPluginUnload("libsimulator");
}
