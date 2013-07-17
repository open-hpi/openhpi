/*      -*- c# -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

using System;
using System.Collections.Generic;
using System.Text;

using org.openhpi;


class Example
{
    public static void Main()
    {
        long rv;
        long sid;

        rv = Api.saHpiSessionOpen( HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID, out sid, null );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionOpen: {0}", rv );
            return;
        }

        long last_hid = HpiConst.SAHPI_LAST_ENTRY;

        // List all handlers
        ASCIIEncoding ascii = new ASCIIEncoding();
        foreach ( long hid in OhpiIterators.HandlerIds( sid ) ) {
            last_hid = hid;
            Console.WriteLine( "Handler {0}", hid );
            oHpiHandlerInfoT hinfo;
            oHpiHandlerConfigT hconf;
            rv = Api.oHpiHandlerInfo( sid, hid, out hinfo, out hconf );
            if ( rv != HpiConst.SA_OK ) {
                Console.WriteLine( "Error: oHpiHandlerInfo: {0}", rv );
                continue;
            }
            Console.WriteLine( " Info" );
            Console.WriteLine( "  id {0}", hinfo.id );
            Console.WriteLine( "  name {0}", ascii.GetString( hinfo.plugin_name ) );
            Console.WriteLine( "  entity_root {0}", HpiUtil.FromSaHpiEntityPathT( hinfo.entity_root ) );
            Console.WriteLine( "  load_failed {0}", hinfo.load_failed );
            Console.WriteLine( " Config" );
            foreach ( var kv in OhpiUtil.FromoHpiHandlerConfigT( hconf ) ) {
                Console.WriteLine( "  {0} = {1}", kv.Key, kv.Value );
            }
        }

        // Retry last handler
        if ( last_hid != HpiConst.SAHPI_LAST_ENTRY ) {
            Console.WriteLine( "Re-trying last handler: {0}", last_hid );
            rv = Api.oHpiHandlerRetry( sid, last_hid );
            if ( rv != HpiConst.SA_OK ) {
                Console.WriteLine( "Error: oHpiHandlerRetry: {0}", rv );
            }
        }

        // Destroy last handler
        if ( last_hid != HpiConst.SAHPI_LAST_ENTRY ) {
            Console.WriteLine( "Destroying last handler: {0}", last_hid );
            rv = Api.oHpiHandlerDestroy( sid, last_hid );
            if ( rv != HpiConst.SA_OK ) {
                Console.WriteLine( "Error: oHpiHandlerDestroy: {0}", rv );
            }
        }

        // Look for handler providing specified resource
        {
            long hid = HpiConst.SAHPI_LAST_ENTRY;
            long rid = 5;
            rv = Api.oHpiHandlerFind( sid, rid, out hid );
            if ( rv != HpiConst.SA_OK ) {
                Console.WriteLine( "Error: oHpiHandlerFind: {0}", rv );
            }
            if ( hid != HpiConst.SAHPI_LAST_ENTRY ) {
                Console.WriteLine( "Resource {0} is provided by handler {1}", rid, hid );
            }
        }

        // Create new instance of test_agent plugin
        {
            Console.WriteLine( "Creating new handler" );
            var d = new Dictionary<String, String>
            {
                { "plugin", "libtest_agent" },
                { "port" , "9999" }
            };
            var hconf = OhpiUtil.TooHpiHandlerConfigT( d );
            long hid;
            rv = Api.oHpiHandlerCreate( sid, hconf, out hid );
            if ( rv == HpiConst.SA_OK ) {
                Console.WriteLine( "Created handler {0}", hid );
            } else {
                Console.WriteLine( "Error: oHpiHandlerCreate: {0}", rv );
            }
        }

        rv = Api.saHpiSessionClose( sid );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionClose: {0}", rv );
            return;
        }
    }
};

