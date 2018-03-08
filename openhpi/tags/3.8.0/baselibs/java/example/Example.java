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

import static org.openhpi.Hpi.*;
import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.Ohpi.*;
import static org.openhpi.OhpiDataTypes.*;
import org.openhpi.HpiUtil;


public class Example
{
    public static void main( String[] args )
    {
        try {
            long version = oHpiVersionGet();
            System.out.printf( "OpenHPI baselib package version: 0x%x\n", version );

            long rv;
            long did;
            long sid;

            oHpiDomainAddOutputParamsT oa = new oHpiDomainAddOutputParamsT();
            rv = oHpiDomainAdd( HpiUtil.toSaHpiTextBufferT( "localhost" ),
                                OPENHPI_DEFAULT_DAEMON_PORT,
                                HpiUtil.makeRootSaHpiEntityPathT(),
                                oa );
            if ( rv != SA_OK ) {
                System.out.printf( "Error: oHpiDomainAdd: %d\n", rv );
                return;
            }

            did = oa.DomainId;
            System.out.printf( "DID = %d\n", did );

            saHpiSessionOpenOutputParamsT oo = new saHpiSessionOpenOutputParamsT();
            rv = saHpiSessionOpen( did, null, oo );
            if ( rv != SA_OK ) {
                System.out.printf( "Error: saHpiSessionOpen: %d\n", rv );
                return;
            }

            sid = oo.SessionId;
            System.out.printf( "SID = %d\n", sid );

            saHpiMyEntityPathGetOutputParamsT om = new saHpiMyEntityPathGetOutputParamsT();
            rv = saHpiMyEntityPathGet( sid, om );
            if ( rv == SA_OK ) {
                System.out.printf( "My entity: %s\n",
                                   HpiUtil.fromSaHpiEntityPathT( om.EntityPath ) );
            }

            System.out.printf( "Resource List:\n" );

            saHpiRptEntryGetOutputParamsT or = new saHpiRptEntryGetOutputParamsT();
            long eid = SAHPI_FIRST_ENTRY;
            do {
                rv = saHpiRptEntryGet( sid, eid, or );
                if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                    break;
                }
                if ( rv != SA_OK ) {
                    System.out.printf( "Error: saHpiRptEntryGet: %d\n", rv );
                    return;
                }
                System.out.printf( "  HPI Resource %d: %s: %s\n",
                                   or.RptEntry.ResourceId,
                                   HpiUtil.fromSaHpiTextBufferT( or.RptEntry.ResourceTag ),
                                   HpiUtil.fromSaHpiEntityPathT( or.RptEntry.ResourceEntity ) );
                eid = or.NextEntryId;
            } while ( eid != SAHPI_LAST_ENTRY );

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

