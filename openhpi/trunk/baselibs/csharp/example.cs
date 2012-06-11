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

using org.openhpi;


class Example
{
    public static void Main()
    {
        long version = Api.oHpiVersionGet();
        Console.WriteLine( "OpenHPI baselib package version: 0x{0:X16}", version );

        long rv;
        long did;
        long sid;

        rv = Api.oHpiDomainAdd( HpiUtil.ToSaHpiTextBufferT( "localhost" ),
                                OhpiConst.DEFAULT_PORT,
                                HpiUtil.MakeRootSaHpiEntityPathT(),
                                out did );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: oHpiDomainAdd: {0}", rv );
            return;
        }

        Console.WriteLine( "DID = {0}", did );

        rv = Api.saHpiSessionOpen( did, out sid, null );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionOpen: {0}", rv );
            return;
        }

        Console.WriteLine( "SID = {0}", sid );

        SaHpiEntityPathT my_ep;
        rv = Api.saHpiMyEntityPathGet( sid, out my_ep );
        if ( rv == HpiConst.SA_OK ) {
            Console.WriteLine( "My entity: {0}", HpiUtil.FromSaHpiEntityPathT( my_ep ) );
        }

        Console.WriteLine( "Resource List:" );

        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        SaHpiRptEntryT rpte;
        do {
            rv = Api.saHpiRptEntryGet( sid, eid, out next_eid, out rpte );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != HpiConst.SA_OK ) {
                Console.WriteLine( "Error: saHpiRptEntryGet: {0}", rv );
                return;
            }
            Console.WriteLine( "  HPI Resource {0}: {1}: {2}",
                               rpte.ResourceId,
                               HpiUtil.FromSaHpiTextBufferT( rpte.ResourceTag ),
                               HpiUtil.FromSaHpiEntityPathT( rpte.ResourceEntity ) );
            eid = next_eid;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );

        rv = Api.saHpiSessionClose( sid );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionClose: {0}", rv );
            return;
        }

        Console.WriteLine( "End" );
    }
};

