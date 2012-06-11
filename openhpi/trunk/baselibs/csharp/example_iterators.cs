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
using System.Linq;

using org.openhpi;


class Example
{
    public static void Main()
    {
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

        rv = Api.saHpiSessionOpen( did, out sid, null );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionOpen: {0}", rv );
            return;
        }

        // DRT
        Console.WriteLine( "" );
        Console.WriteLine( "DRT:" );
        foreach ( var drte in HpiIterators.Drt( sid ) ) {
            Console.WriteLine( "  HPI Domain {0}: Is Peer {1}",
                               drte.DomainId,
                               drte.IsPeer );
        }

        // DAT
        Console.WriteLine( "" );
        Console.WriteLine( "DAT:" );
        foreach( var a in HpiIterators.Dat( sid ) )
        {
            Console.WriteLine( "  Alarm {0}: {1}: {2}",
                               a.AlarmId,
                               HpiUtil.FromSaHpiSeverityT( a.Severity ),
                               HpiUtil.FromSaHpiStatusCondTypeT( a.AlarmCond.Type ) );
        }

        // DEL: Read backward
        Console.WriteLine( "" );
        Console.WriteLine( "DEL: Newest entries first" );
        foreach ( var ex in HpiIterators.EventLogEntries( sid, HpiConst.SAHPI_UNSPECIFIED_RESOURCE_ID, false ) )
        {
            Console.WriteLine( "  Entry {0}: {1}",
                               ex.EventLogEntry.EntryId,
                               HpiUtil.FromSaHpiEventTypeT( ex.EventLogEntry.Event.EventType ) );
        }

        // Iterate over top-level entities
        Console.WriteLine( "" );
        Console.WriteLine( "Top-level Entities:" );
        SaHpiEntityPathT root = HpiUtil.MakeRootSaHpiEntityPathT();
        foreach ( var child in HpiIterators.ChildEntities( sid, root ) ) {
            Console.WriteLine( "  {0}", HpiUtil.FromSaHpiEntityPathT( child ) );
            // Resources for the entity
            Console.WriteLine( "    Resources:" );
            foreach ( var rid in HpiIterators.EntityResourceIds( sid, child ) ) {
                Console.WriteLine( "      Resource {0}", rid );
            }
            // Sensors for the entity
            Console.WriteLine( "    Sensors:" );
            foreach ( var ri in HpiIterators.EntityInstrumentIds( sid, child, HpiConst.SAHPI_SENSOR_RDR ) )
            {
                Console.WriteLine( "      Resource {0} Sensor {1}", ri.ResourceId, ri.InstrumentId );
            }
        }

        // RPT
        Console.WriteLine( "" );
        Console.WriteLine( "RPT:" );
        foreach ( var rpte in HpiIterators.Rpt( sid ) ) {
            Console.WriteLine( "  Resource {0}: {1}: {2}",
                               rpte.ResourceId,
                               HpiUtil.FromSaHpiTextBufferT( rpte.ResourceTag ),
                               HpiUtil.FromSaHpiEntityPathT( rpte.ResourceEntity ) );
        }

        // RDRs
        Console.WriteLine( "" );
        Console.WriteLine( "RDRs:" );
        foreach ( var rpte in HpiIterators.Rpt( sid ) ) {
            Console.WriteLine( "  Resource {0}:", rpte.ResourceId );
            foreach ( var rdr in HpiIterators.Rdrs( sid, rpte.ResourceId ) ) {
                Console.WriteLine( "    {0}: {1}",
                                   HpiUtil.FromSaHpiRdrTypeT( rdr.RdrType ),
                                   HpiUtil.FromSaHpiTextBufferT( rdr.IdString ) );
            }
        }

        // IDR Areas (LINQ is used to select IDRs as an example)
        Console.WriteLine( "" );
        Console.WriteLine( "IDRs:" );
        var idrs = from rpte in HpiIterators.Rpt( sid )
                   from rdr in HpiIterators.Rdrs( sid, rpte.ResourceId )
                   where rdr.RdrType == HpiConst.SAHPI_INVENTORY_RDR
                   select new { rid = rpte.ResourceId, idrid = rdr.RdrTypeUnion.InventoryRec.IdrId };
        foreach ( var ri in idrs ) {
            Console.WriteLine( "  Resource {0}: IDR {1}:", ri.rid, ri.idrid );
            // IDR Areas
            foreach ( var ahdr in HpiIterators.IdrAreaHeaders( sid, ri.rid, ri.idrid ) ) {
                Console.WriteLine( "    Area {0}: {1}",
                                   ahdr.AreaId,
                                   HpiUtil.FromSaHpiIdrAreaTypeT( ahdr.Type ) );
                // IDR Fields
                foreach ( var f in HpiIterators.IdrAreaFields( sid, ri.rid, ri.idrid, ahdr.AreaId ) )
                {
                    Console.WriteLine( "      Field {0}: {1}",
                                       f.FieldId,
                                       HpiUtil.FromSaHpiIdrFieldTypeT( f.Type ) );
                }
            }
        }

        // Announcements in Annunciators (LINQ is used to select annunciators as an example)
        Console.WriteLine( "" );
        Console.WriteLine( "Annunciators:" );
        var anns = from rpte in HpiIterators.Rpt( sid )
                   from rdr in HpiIterators.Rdrs( sid, rpte.ResourceId )
                   where rdr.RdrType == HpiConst.SAHPI_ANNUNCIATOR_RDR
                   select new { rid = rpte.ResourceId, annnum = rdr.RdrTypeUnion.AnnunciatorRec.AnnunciatorNum };
        foreach ( var ri in anns ) {
            Console.WriteLine( "  Resource {0}: Annunciator {1}:", ri.rid, ri.annnum );
            // Announcements
            foreach ( var a in HpiIterators.Announcements( sid, ri.rid, ri.annnum ) ) {
                Console.WriteLine( "    Announcement {0}: {1}: {2}",
                                   a.EntryId,
                                   HpiUtil.FromSaHpiSeverityT( a.Severity ),
                                   HpiUtil.FromSaHpiStatusCondTypeT( a.StatusCond.Type ) );
            }
        }

        // FUMI (LINQ is used to select FUMIs with Logical Bank only as an example)
        Console.WriteLine( "" );
        Console.WriteLine( "FUMIs:" );
        var fumis = from rpte in HpiIterators.Rpt( sid )
                    from rdr in HpiIterators.Rdrs( sid, rpte.ResourceId )
                    where rdr.RdrType == HpiConst.SAHPI_FUMI_RDR
                    where rdr.RdrTypeUnion.FumiRec.NumBanks == 0
                    select new { rid = rpte.ResourceId, fuminum = rdr.RdrTypeUnion.FumiRec.Num };
        foreach ( var ri in fumis ) {
            Console.WriteLine( "  Resource {0}: FUMI {1}:", ri.rid, ri.fuminum );
            // Source components
            Console.WriteLine( "    Source Components:" );
            foreach ( var info in HpiIterators.FumiSourceComponents( sid, ri.rid, ri.fuminum, 0 ) ) {
                Console.WriteLine( "      Component {0}: {1}: FW {2}.{3}.{4}",
                                   info.ComponentId,
                                   HpiUtil.FromSaHpiTextBufferT( info.MainFwInstance.Description ),
                                   info.MainFwInstance.MajorVersion,
                                   info.MainFwInstance.MinorVersion,
                                   info.MainFwInstance.AuxVersion );
            }
            // Target components
            Console.WriteLine( "    Target Components:" );
            foreach ( var info in HpiIterators.FumiTargetComponents( sid, ri.rid, ri.fuminum, 0 ) ) {
                Console.WriteLine( "      Component {0}: {1}: FW {2}.{3}.{4}",
                                   info.ComponentId,
                                   HpiUtil.FromSaHpiTextBufferT( info.MainFwInstance.Description ),
                                   info.MainFwInstance.MajorVersion,
                                   info.MainFwInstance.MinorVersion,
                                   info.MainFwInstance.AuxVersion );
            }
            // Logical Target components
            Console.WriteLine( "    Logical Target Components:" );
            foreach ( var info in HpiIterators.FumiLogicalTargetComponents( sid, ri.rid, ri.fuminum ) )
            {
                Console.WriteLine( "      Component {0}:", info.ComponentId );
                if ( info.PendingFwInstance.InstancePresent != HpiConst.SAHPI_FALSE ) {
                    Console.WriteLine( "        Pending FW {0}.{1}.{2}:",
                                       info.PendingFwInstance.MajorVersion,
                                       info.PendingFwInstance.MinorVersion,
                                       info.PendingFwInstance.AuxVersion );
                }
                if ( info.RollbackFwInstance.InstancePresent != HpiConst.SAHPI_FALSE ) {
                    Console.WriteLine( "        Rollback FW {0}.{1}.{2}:",
                                       info.RollbackFwInstance.MajorVersion,
                                       info.RollbackFwInstance.MinorVersion,
                                       info.RollbackFwInstance.AuxVersion );
                }
            }
        }

        rv = Api.saHpiSessionClose( sid );
        if ( rv != HpiConst.SA_OK ) {
            Console.WriteLine( "Error: saHpiSessionClose: {0}", rv );
            return;
        }
    }
};

