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
import org.openhpi.HpiUtil;
import org.openhpi.HpiCollections;


public class ExampleCollections
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

            // DRT
            System.out.printf( "\n" );
            System.out.printf( "DRT:\n" );
            for ( SaHpiDrtEntryT drte : HpiCollections.Drt( sid ) ) {
                System.out.printf( "  HPI Domain %d: Is Peer %d\n",
                                   drte.DomainId,
                                   drte.IsPeer );
            }

            // DAT
            System.out.printf( "\n" );
            System.out.printf( "DAT:\n" );
            for( SaHpiAlarmT a : HpiCollections.Dat( sid, SAHPI_ALL_SEVERITIES, SAHPI_FALSE ) )
            {
                System.out.printf( "  Alarm %d: %s: %s\n",
                                   a.AlarmId,
                                   HpiUtil.fromSaHpiSeverityT( a.Severity ),
                                   HpiUtil.fromSaHpiStatusCondTypeT( a.AlarmCond.Type ) );
            }

            // DEL: Read backward
            System.out.printf( "\n" );
            System.out.printf( "DEL: Newest entries first\n" );
            for ( HpiCollections.EventLogEntryEx ex : HpiCollections.EventLogEntries( sid, SAHPI_UNSPECIFIED_RESOURCE_ID, false ) )
            {
                System.out.printf( "  Entry %d: %s\n",
                                   ex.EventLogEntry.EntryId,
                                   HpiUtil.fromSaHpiEventTypeT( ex.EventLogEntry.Event.EventType ) );
            }

            // Iterate over top-level entities
            System.out.printf( "\n" );
            System.out.printf( "Top-level Entities:\n" );
            SaHpiEntityPathT root = HpiUtil.makeRootSaHpiEntityPathT();
            for ( SaHpiEntityPathT child : HpiCollections.ChildEntities( sid, root ) ) {
                System.out.printf( "  %s\n", HpiUtil.fromSaHpiEntityPathT( child ) );
                // Resources for the entity
                System.out.printf( "    Resources:\n" );
                for ( long rid : HpiCollections.EntityResourceIds( sid, child ) ) {
                    System.out.printf( "      Resource %d\n", rid );
                }
                // Sensors for the entity
                System.out.printf( "    Sensors:\n" );
                for ( HpiCollections.ResourceIdInstrumentId ri : HpiCollections.EntityInstrumentIds( sid, child, SAHPI_SENSOR_RDR ) )
                {
                    System.out.printf( "      Resource %d Sensor %d\n", ri.ResourceId, ri.InstrumentId );
                }
            }

            // RPT
            System.out.printf( "\n" );
            System.out.printf( "RPT:\n" );
            for ( SaHpiRptEntryT rpte : HpiCollections.Rpt( sid ) ) {
                System.out.printf( "  Resource %d: %s: %s\n",
                                   rpte.ResourceId,
                                   HpiUtil.fromSaHpiTextBufferT( rpte.ResourceTag ),
                                   HpiUtil.fromSaHpiEntityPathT( rpte.ResourceEntity ) );
            }

            // RDRs
            System.out.printf( "\n" );
            System.out.printf( "RDRs:\n" );
            for ( SaHpiRptEntryT rpte : HpiCollections.Rpt( sid ) ) {
                System.out.printf( "  Resource %d:\n", rpte.ResourceId );
                for ( SaHpiRdrT rdr : HpiCollections.Rdrs( sid, rpte.ResourceId ) ) {
                    System.out.printf( "    %s: %s\n",
                                       HpiUtil.fromSaHpiRdrTypeT( rdr.RdrType ),
                                       HpiUtil.fromSaHpiTextBufferT( rdr.IdString ) );
                }
            }

            // IDR Areas
            System.out.printf( "\n" );
            System.out.printf( "IDRs:\n" );
            for ( SaHpiRptEntryT rpte : HpiCollections.Rpt( sid ) ) {
                for ( SaHpiRdrT rdr : HpiCollections.Rdrs( sid, rpte.ResourceId ) ) {
                    if ( rdr.RdrType != SAHPI_INVENTORY_RDR ) {
                        continue;
                    }
                    long rid = rpte.ResourceId;
                    long idrid = rdr.RdrTypeUnion.InventoryRec.IdrId;
                    System.out.printf( "  Resource %d: IDR %d:\n", rid, idrid );
                    // IDR Areas
                    for ( SaHpiIdrAreaHeaderT ahdr : HpiCollections.IdrAreaHeaders( sid, rid, idrid, SAHPI_IDR_AREATYPE_UNSPECIFIED ) ) {
                        System.out.printf( "    Area %d: %s\n",
                                           ahdr.AreaId,
                                           HpiUtil.fromSaHpiIdrAreaTypeT( ahdr.Type ) );
                        // IDR Fields
                        for ( SaHpiIdrFieldT f : HpiCollections.IdrAreaFields( sid, rid, idrid, ahdr.AreaId, SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) )
                        {
                            System.out.printf( "      Field %d: %s\n",
                                               f.FieldId,
                                               HpiUtil.fromSaHpiIdrFieldTypeT( f.Type ) );
                        }
                    }
                }
            }

            // Announcements in Annunciators
            System.out.printf( "\n" );
            System.out.printf( "Annunciators:\n" );
            for ( SaHpiRptEntryT rpte : HpiCollections.Rpt( sid ) ) {
                for ( SaHpiRdrT rdr : HpiCollections.Rdrs( sid, rpte.ResourceId ) ) {
                    if ( rdr.RdrType != SAHPI_ANNUNCIATOR_RDR ) {
                        continue;
                    }
                    long rid = rpte.ResourceId;
                    long annnum = rdr.RdrTypeUnion.AnnunciatorRec.AnnunciatorNum;
                    System.out.printf( "  Resource %d: Annunciator %d:\n", rid, annnum );
                    // Announcements
                    for ( SaHpiAnnouncementT a : HpiCollections.Announcements( sid, rid, annnum, SAHPI_ALL_SEVERITIES, SAHPI_FALSE ) ) {
                        System.out.printf( "    Announcement %d: %s: %s\n",
                                           a.EntryId,
                                           HpiUtil.fromSaHpiSeverityT( a.Severity ),
                                           HpiUtil.fromSaHpiStatusCondTypeT( a.StatusCond.Type ) );
                    }
                }
            }

            // FUMI
            System.out.printf( "\n" );
            System.out.printf( "FUMIs:\n" );
            for ( SaHpiRptEntryT rpte : HpiCollections.Rpt( sid ) ) {
                for ( SaHpiRdrT rdr : HpiCollections.Rdrs( sid, rpte.ResourceId ) ) {
                    if ( rdr.RdrType != SAHPI_FUMI_RDR ) {
                        continue;
                    }
                    if ( rdr.RdrTypeUnion.FumiRec.NumBanks != 0 ) {
                        continue;
                    }
                    long rid = rpte.ResourceId;
                    long fuminum = rdr.RdrTypeUnion.FumiRec.Num;
                    System.out.printf( "  Resource %d: FUMI %d:\n", rid, fuminum );
                    // Source components
                    System.out.printf( "    Source Components:" );
                    for ( SaHpiFumiComponentInfoT info : HpiCollections.FumiSourceComponents( sid, rid, fuminum, 0 ) ) {
                        System.out.printf( "      Component %d: %s: FW %d.%d.%d\n",
                                           info.ComponentId,
                                           HpiUtil.fromSaHpiTextBufferT( info.MainFwInstance.Description ),
                                           info.MainFwInstance.MajorVersion,
                                           info.MainFwInstance.MinorVersion,
                                           info.MainFwInstance.AuxVersion );
                    }
                    // Target components
                    System.out.printf( "    Target Components:\n" );
                    for ( SaHpiFumiComponentInfoT info : HpiCollections.FumiTargetComponents( sid, rid, fuminum, 0 ) ) {
                        System.out.printf( "      Component %d: %s: FW %d.%d.%d\n",
                                           info.ComponentId,
                                           HpiUtil.fromSaHpiTextBufferT( info.MainFwInstance.Description ),
                                           info.MainFwInstance.MajorVersion,
                                           info.MainFwInstance.MinorVersion,
                                           info.MainFwInstance.AuxVersion );
                    }
                    // Logical Target components
                    System.out.printf( "    Logical Target Components:\n" );
                    for ( SaHpiFumiLogicalComponentInfoT info : HpiCollections.FumiLogicalTargetComponents( sid, rid, fuminum ) )
                    {
                        System.out.printf( "      Component %d:\n", info.ComponentId );
                        if ( info.PendingFwInstance.InstancePresent != SAHPI_FALSE ) {
                            System.out.printf( "        Pending FW %d.%d.%d:\n",
                                               info.PendingFwInstance.MajorVersion,
                                               info.PendingFwInstance.MinorVersion,
                                               info.PendingFwInstance.AuxVersion );
                        }
                        if ( info.RollbackFwInstance.InstancePresent != SAHPI_FALSE ) {
                            System.out.printf( "        Rollback FW %d.%d.%d:\n",
                                               info.RollbackFwInstance.MajorVersion,
                                               info.RollbackFwInstance.MinorVersion,
                                               info.RollbackFwInstance.AuxVersion );
                        }
                    }
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

