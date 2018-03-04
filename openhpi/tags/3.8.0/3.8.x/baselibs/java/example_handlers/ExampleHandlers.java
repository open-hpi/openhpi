/*      -*- java -*-
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

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import static org.openhpi.Hpi.*;
import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.Ohpi.*;
import static org.openhpi.OhpiDataTypes.*;
import org.openhpi.HpiUtil;
import org.openhpi.OhpiUtil;
import org.openhpi.OhpiCollections;


public class ExampleHandlers
{
    public static void main( String[] args )
    {
        try {
            long rv;

            saHpiSessionOpenOutputParamsT oo = new saHpiSessionOpenOutputParamsT();
            rv = saHpiSessionOpen( SAHPI_UNSPECIFIED_DOMAIN_ID, null, oo );
            if ( rv != SA_OK ) {
                System.out.printf( "Error: saHpiSessionOpen: %d\n", rv );
                return;
            }

            long sid = oo.SessionId;

            long hid;
            long last_hid = SAHPI_LAST_ENTRY;

            // List all handlers
            oHpiHandlerGetNextOutputParamsT ogn = new oHpiHandlerGetNextOutputParamsT();
            hid = SAHPI_FIRST_ENTRY;
            do {
                rv = oHpiHandlerGetNext( sid, hid, ogn );
                if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                    break;
                }
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: oHpiHandlerGetNext: %d\n", rv );
                    break;
                }
                hid = ogn.NextHandlerId;
                last_hid = hid;

                System.out.printf( "Handler %d\n", hid );

                oHpiHandlerInfoOutputParamsT oi = new oHpiHandlerInfoOutputParamsT();
                rv = oHpiHandlerInfo( sid, hid, oi );
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: oHpiHandlerInfo: %d\n", rv );
                    continue;
                }

                oHpiHandlerInfoT hinfo = oi.HandlerInfo;
                oHpiHandlerConfigT hconf = oi.HandlerConfig;

                System.out.printf( " Info\n" );
                System.out.printf( "  id %d\n", hinfo.id );
                System.out.printf( "  name %s\n", new String( hinfo.plugin_name, "US-ASCII" ) );
                System.out.printf( "  entity_root %s\n", HpiUtil.fromSaHpiEntityPathT( hinfo.entity_root ) );
                System.out.printf( "  load_failed %d\n", hinfo.load_failed );
                System.out.printf( " Config\n" );
                Map<String, String> m = OhpiUtil.fromoHpiHandlerConfigT( hconf );
                for ( Entry<String, String> kv : m.entrySet() ) {
                    System.out.printf( "  %s = %s\n", kv.getKey(), kv.getValue() );
                }

            } while ( hid != SAHPI_LAST_ENTRY );

            // List all handlers with OhpiCollections
            System.out.printf( "List all handlers with OhpiCollections:\n" );
            for ( long hid2 : OhpiCollections.HandlerIds( sid ) ) {
                System.out.printf( " Handler %d\n", hid2 );
            }

            // Retry last handler
            if ( last_hid != SAHPI_LAST_ENTRY ) {
                System.out.printf( "Re-trying last handler: %d\n", last_hid );
                rv = oHpiHandlerRetry( sid, last_hid );
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: oHpiHandlerRetry: %d\n", rv );
                }
            }

            // Destroy last handler
            if ( last_hid != SAHPI_LAST_ENTRY ) {
                System.out.printf( "Destroying last handler: %d\n", last_hid );
                rv = oHpiHandlerDestroy( sid, last_hid );
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: oHpiHandlerDestroy: %d\n", rv );
                }
            }

            // Look for handler providing specified resource
            {
                long rid = 5;
                oHpiHandlerFindOutputParamsT of = new oHpiHandlerFindOutputParamsT();
                rv = oHpiHandlerFind( sid, rid, of );
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: oHpiHandlerFind: %d\n", rv );
                }
                hid = of.HandlerId;
                System.out.printf( "Resource %d is provided by handler %d\n", rid, hid );
            }

            // Create new instance of test_agent plugin
            {
                System.out.printf( "Creating new handler\n" );
                HashMap<String, String> m = new HashMap<String, String>();
                m.put( "plugin", "libtest_agent" );
                m.put( "port", "12345" );
                oHpiHandlerConfigT hconf = OhpiUtil.tooHpiHandlerConfigT( m );
                oHpiHandlerCreateOutputParamsT oc = new oHpiHandlerCreateOutputParamsT();
                rv = oHpiHandlerCreate( sid, hconf, oc );
                if ( rv == SA_OK ) {
                    hid = oc.HandlerId;
                    System.out.printf( "Created handler %d\n", hid );
                } else {
                    System.out.printf( "Error: oHpiHandlerCreate: %d\n", rv );
                }
            }

            rv = saHpiSessionClose( sid );
            if ( rv != SA_OK ) {
                System.out.printf( "Error: saHpiSessionClose: %d\n", rv );
                return;
            }
        } catch ( Exception e ) {
            System.out.printf( "Got exception: %s!\n", e.getMessage() );
        } finally {
            System.out.println( "End" );
        }
    }
};

