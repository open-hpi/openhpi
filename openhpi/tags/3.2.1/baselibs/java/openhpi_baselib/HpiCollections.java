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

package org.openhpi;

import java.util.Collection;
import java.util.LinkedList;

import static org.openhpi.HpiDataTypes.*;
import static org.openhpi.Hpi.*;


/**********************************************************
 * HPI Utility Functions: Collections
 *********************************************************/
public class HpiCollections
{
    // Just to ensure nobody creates it
    private HpiCollections()
    {
        // empty
    }


    /***********************************************************
     * Helper functions
     * TODO - move to HpiUtils?
     **********************************************************/
    private static SaHpiConditionT NewCondition()
    {
        SaHpiConditionT c = new SaHpiConditionT();
        c.Type        = SAHPI_STATUS_COND_TYPE_USER;
        c.Entity      = HpiUtil.makeUnspecifiedSaHpiEntityPathT();
        c.DomainId    = SAHPI_UNSPECIFIED_DOMAIN_ID;
        c.ResourceId  = SAHPI_UNSPECIFIED_RESOURCE_ID;
        c.SensorNum   = SAHPI_ENTRY_UNSPECIFIED;
        c.EventState  = SAHPI_ES_UNSPECIFIED;
        c.Name        = new SaHpiNameT();
        c.Name.Length = 0;
        c.Name.Value  = new byte[SA_HPI_MAX_NAME_LENGTH];
        c.Mid         = SAHPI_MANUFACTURER_ID_UNSPECIFIED;
        c.Data        = HpiUtil.toSaHpiTextBufferT( "" );
        return c;
    }

    public static SaHpiAlarmT NewAlarm( long id, long timestamp )
    {
        SaHpiAlarmT a = new SaHpiAlarmT();
        a.AlarmId      = id;
        a.Timestamp    = timestamp;
        a.Severity     = SAHPI_ALL_SEVERITIES;
        a.Acknowledged = SAHPI_FALSE;
        a.AlarmCond    = NewCondition();
        return a;
    }

    public static SaHpiAnnouncementT NewAnnouncement( long id, long timestamp )
    {
        SaHpiAnnouncementT a = new SaHpiAnnouncementT();
        a.EntryId      = id;
        a.Timestamp    = timestamp;
        a.AddedByUser  = SAHPI_TRUE;
        a.Severity     = SAHPI_ALL_SEVERITIES;
        a.Acknowledged = SAHPI_FALSE;
        a.StatusCond   = NewCondition();
        return a;
    }


    /***********************************************************
     * DRT
     **********************************************************/
    public static Collection<SaHpiDrtEntryT> Drt(
        long sid
    ) throws HpiException
    {
        LinkedList<SaHpiDrtEntryT> all = new LinkedList<SaHpiDrtEntryT>();

        long eid = SAHPI_FIRST_ENTRY;
        SaHpiDrtEntryT drte;
        do {
            saHpiDrtEntryGetOutputParamsT out = new saHpiDrtEntryGetOutputParamsT();
            long rv = saHpiDrtEntryGet( sid, eid, out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextEntryId;
            all.add( out.DrtEntry );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }

    /***********************************************************
     * DAT
     **********************************************************/
    public static Collection<SaHpiAlarmT> Dat(
        long sid,
        long severity,
        long unacknowledged_only
    ) throws HpiException
    {
        LinkedList<SaHpiAlarmT> all = new LinkedList<SaHpiAlarmT>();

        long id = SAHPI_FIRST_ENTRY;
        long timestamp = 0;
        do {
            SaHpiAlarmT a = NewAlarm( id, timestamp );
            saHpiAlarmGetNextOutputParamsT out = new saHpiAlarmGetNextOutputParamsT();
            long rv = saHpiAlarmGetNext( sid, severity, unacknowledged_only, a, out );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            id = out.Alarm.AlarmId;
            timestamp = out.Alarm.Timestamp;
            all.add( out.Alarm );
        } while ( id != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * Event Log
     **********************************************************/
    public static class EventLogEntryEx
    {
        public SaHpiEventLogEntryT EventLogEntry;
        public SaHpiRdrT           Rdr;
        public SaHpiRptEntryT      RptEntry;
    };

    public static Collection<EventLogEntryEx> EventLogEntries(
        long sid,
        long rid,
        boolean readforward
    ) throws HpiException
    {
        LinkedList<EventLogEntryEx> all = new LinkedList<EventLogEntryEx>();

        long eid = readforward ? SAHPI_OLDEST_ENTRY : SAHPI_NEWEST_ENTRY;
        do {
            saHpiEventLogEntryGetOutputParamsT out = new saHpiEventLogEntryGetOutputParamsT();
            long rv = saHpiEventLogEntryGet( sid, rid, eid, out );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = readforward ? out.NextEntryId : out.PrevEntryId;
            EventLogEntryEx ex = new EventLogEntryEx();
            ex.EventLogEntry = out.EventLogEntry;
            ex.Rdr           = out.Rdr;
            ex.RptEntry      = out.RptEntry;
            all.add( ex );
        } while ( eid != SAHPI_NO_MORE_ENTRIES );

        return all;
    }


    /***********************************************************
     * entity resource ids
     **********************************************************/
    public static Collection<Long> EntityResourceIds(
        long sid,
        SaHpiEntityPathT ep
    ) throws HpiException
    {
        LinkedList<Long> all = new LinkedList<Long>();

        long id = SAHPI_FIRST_ENTRY;
        do {
            saHpiGetIdByEntityPathOutputParamsT out = new saHpiGetIdByEntityPathOutputParamsT();
            long rv = saHpiGetIdByEntityPath( sid, ep, SAHPI_NO_RECORD, id, out );
            if ( ( id == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            id = out.InstanceId;
            all.add( out.ResourceId );
        } while ( id != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * entity instrument ids
     **********************************************************/
    public static class ResourceIdInstrumentId
    {
        public long ResourceId;
        public long InstrumentId;
    };

    public static Collection<ResourceIdInstrumentId> EntityInstrumentIds(
        long sid,
        SaHpiEntityPathT ep,
        long rdrtype
    ) throws HpiException
    {
        LinkedList<ResourceIdInstrumentId> all = new LinkedList<ResourceIdInstrumentId>();

        long id = SAHPI_FIRST_ENTRY;
        do {
            saHpiGetIdByEntityPathOutputParamsT out = new saHpiGetIdByEntityPathOutputParamsT();
            long rv = saHpiGetIdByEntityPath( sid, ep, rdrtype, id, out );
            if ( ( id == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            id = out.InstanceId;
            ResourceIdInstrumentId ri = new ResourceIdInstrumentId();
            ri.ResourceId    = out.ResourceId;
            ri.InstrumentId = out.InstrumentId;
            all.add( ri );
        } while ( id != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * child entities
     **********************************************************/
    public static Collection<SaHpiEntityPathT> ChildEntities(
        long sid,
        SaHpiEntityPathT parent_ep
    ) throws HpiException
    {
        LinkedList<SaHpiEntityPathT> all = new LinkedList<SaHpiEntityPathT>();

        long id = SAHPI_FIRST_ENTRY;
        do {
            saHpiGetChildEntityPathOutputParamsT out = new saHpiGetChildEntityPathOutputParamsT();
            long rv = saHpiGetChildEntityPath( sid, parent_ep, id, out );
            if ( ( id == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            id = out.InstanceId;
            all.add( out.ChildEntityPath );
        } while ( id != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * RPT
     **********************************************************/
    public static Collection<SaHpiRptEntryT> Rpt(
        long sid
    ) throws HpiException
    {
        LinkedList<SaHpiRptEntryT> all = new LinkedList<SaHpiRptEntryT>();

        long eid = SAHPI_FIRST_ENTRY;
        do {
            saHpiRptEntryGetOutputParamsT out = new saHpiRptEntryGetOutputParamsT();
            long rv = saHpiRptEntryGet( sid, eid, out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextEntryId;
            all.add( out.RptEntry );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * RDRs
     **********************************************************/
    public static Collection<SaHpiRdrT> Rdrs(
        long sid,
        long rid
    ) throws HpiException
    {
        LinkedList<SaHpiRdrT> all = new LinkedList<SaHpiRdrT>();

        long eid = SAHPI_FIRST_ENTRY;
        do {
            saHpiRdrGetOutputParamsT out = new saHpiRdrGetOutputParamsT();
            long rv = saHpiRdrGet( sid, rid, eid, out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextEntryId;
            all.add( out.Rdr );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * Idr Areas
     **********************************************************/
    public static Collection<SaHpiIdrAreaHeaderT> IdrAreaHeaders(
        long sid,
        long rid,
        long idrid,
        long atype
    ) throws HpiException
    {
        LinkedList<SaHpiIdrAreaHeaderT> all = new LinkedList<SaHpiIdrAreaHeaderT>();

        long aid = SAHPI_FIRST_ENTRY;
        do {
            saHpiIdrAreaHeaderGetOutputParamsT out = new saHpiIdrAreaHeaderGetOutputParamsT();
            long rv = saHpiIdrAreaHeaderGet( sid, rid, idrid, atype, aid, out );
            if ( ( aid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            aid = out.NextAreaId;
            all.add( out.Header );
        } while ( aid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * Area Fields
     **********************************************************/
    public static Collection<SaHpiIdrFieldT> IdrAreaFields(
        long sid,
        long rid,
        long idrid,
        long aid,
        long ftype
    ) throws HpiException
    {
        LinkedList<SaHpiIdrFieldT> all = new LinkedList<SaHpiIdrFieldT>();

        long fid = SAHPI_FIRST_ENTRY;
        do {
            saHpiIdrFieldGetOutputParamsT out = new saHpiIdrFieldGetOutputParamsT();
            long rv = saHpiIdrFieldGet( sid, rid, idrid, aid, ftype, fid, out );
            if ( ( aid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            fid = out.NextFieldId;
            all.add( out.Field );
        } while ( aid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * annuncuator announcements
     **********************************************************/
    public static Collection<SaHpiAnnouncementT> Announcements(
        long sid,
        long rid,
        long annnum,
        long severity,
        long unacknowledged_only
    ) throws HpiException
    {
        LinkedList<SaHpiAnnouncementT> all = new LinkedList<SaHpiAnnouncementT>();

        long id = SAHPI_FIRST_ENTRY;
        long timestamp = 0;
        do {
            SaHpiAnnouncementT a = NewAnnouncement( id, timestamp );
            saHpiAnnunciatorGetNextOutputParamsT out = new saHpiAnnunciatorGetNextOutputParamsT();
            long rv = saHpiAnnunciatorGetNext( sid,
                                               rid,
                                               annnum,
                                               severity,
                                               unacknowledged_only,
                                               a,
                                               out );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            id        = out.Announcement.EntryId;
            timestamp = out.Announcement.Timestamp;
            all.add( out.Announcement );
        } while ( id != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * FUMI Source Components
     **********************************************************/
    public static Collection<SaHpiFumiComponentInfoT> FumiSourceComponents(
        long sid,
        long rid,
        long fuminum,
        long banknum
    ) throws HpiException
    {
        LinkedList<SaHpiFumiComponentInfoT> all = new LinkedList<SaHpiFumiComponentInfoT>();

        long eid = SAHPI_FIRST_ENTRY;
        do {
            saHpiFumiSourceComponentInfoGetOutputParamsT out = new saHpiFumiSourceComponentInfoGetOutputParamsT();
            long rv = saHpiFumiSourceComponentInfoGet( sid,
                                                       rid,
                                                       fuminum,
                                                       banknum,
                                                       eid,
                                                       out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextComponentEntryId;
            all.add( out.ComponentInfo );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * FUMI Target Components
     **********************************************************/
    public static Collection<SaHpiFumiComponentInfoT> FumiTargetComponents(
        long sid,
        long rid,
        long fuminum,
        long banknum
    ) throws HpiException
    {
        LinkedList<SaHpiFumiComponentInfoT> all = new LinkedList<SaHpiFumiComponentInfoT>();

        long eid = SAHPI_FIRST_ENTRY;
        do {
            saHpiFumiTargetComponentInfoGetOutputParamsT out = new saHpiFumiTargetComponentInfoGetOutputParamsT();
            long rv = saHpiFumiTargetComponentInfoGet( sid,
                                                       rid,
                                                       fuminum,
                                                       banknum,
                                                       eid,
                                                       out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextComponentEntryId;
            all.add( out.ComponentInfo );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }


    /***********************************************************
     * FUMI Logical Target Components
     **********************************************************/
    public static Collection<SaHpiFumiLogicalComponentInfoT> FumiLogicalTargetComponents(
        long sid,
        long rid,
        long fuminum
    ) throws HpiException
    {
        LinkedList<SaHpiFumiLogicalComponentInfoT> all = new LinkedList<SaHpiFumiLogicalComponentInfoT>();

        long eid = SAHPI_FIRST_ENTRY;
        do {
            saHpiFumiLogicalTargetComponentInfoGetOutputParamsT out = new saHpiFumiLogicalTargetComponentInfoGetOutputParamsT();
            long rv = saHpiFumiLogicalTargetComponentInfoGet( sid,
                                                              rid,
                                                              fuminum,
                                                              eid,
                                                              out );
            if ( ( eid == SAHPI_FIRST_ENTRY ) && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
                break;
            }
            if ( rv != SA_OK ) {
                break;
            }
            eid = out.NextComponentEntryId;
            all.add( out.ComponentInfo );
        } while ( eid != SAHPI_LAST_ENTRY );

        return all;
    }


}; // class HpiCollections

