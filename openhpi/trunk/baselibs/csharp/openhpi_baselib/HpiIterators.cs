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


namespace org {
namespace openhpi {


/**********************************************************
 * HPI Utility Functions: Iterators
 *********************************************************/
public static class HpiIterators
{


    /***********************************************************
     * Helper functions
     * TODO - move to HpiUtils?
     **********************************************************/
    private static SaHpiConditionT NewCondition()
    {
        return new SaHpiConditionT
        {
            Type        = HpiConst.SAHPI_STATUS_COND_TYPE_USER,
            Entity      = HpiUtil.MakeUnspecifiedSaHpiEntityPathT(),
            DomainId    = HpiConst.SAHPI_UNSPECIFIED_DOMAIN_ID,
            ResourceId  = HpiConst.SAHPI_UNSPECIFIED_RESOURCE_ID,
            SensorNum   = HpiConst.SAHPI_ENTRY_UNSPECIFIED,
            EventState  = HpiConst.SAHPI_ES_UNSPECIFIED,
            Name        = new SaHpiNameT
            {
                Length = 0,
                Value  = new byte[HpiConst.SA_HPI_MAX_NAME_LENGTH],
            },
            Mid         = HpiConst.SAHPI_MANUFACTURER_ID_UNSPECIFIED,
            Data        = HpiUtil.ToSaHpiTextBufferT( "" ),
        };
    }

    public static SaHpiAlarmT NewAlarm( long id, long timestamp )
    {
        return new SaHpiAlarmT
        {
            AlarmId      = id,
            Timestamp    = timestamp,
            Severity     = HpiConst.SAHPI_ALL_SEVERITIES,
            Acknowledged = HpiConst.SAHPI_FALSE,
            AlarmCond    = NewCondition(),
        };
    }

    public static SaHpiAnnouncementT NewAnnouncement( long id, long timestamp )
    {
        return new SaHpiAnnouncementT
        {
            EntryId      = id,
            Timestamp    = timestamp,
            AddedByUser  = HpiConst.SAHPI_TRUE,
            Severity     = HpiConst.SAHPI_ALL_SEVERITIES,
            Acknowledged = HpiConst.SAHPI_FALSE,
            StatusCond   = NewCondition(),
        };
    }


    /***********************************************************
     * Iterate over DRT
     **********************************************************/
    public static IEnumerable<SaHpiDrtEntryT> Drt( long sid )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        SaHpiDrtEntryT drte;
        do {
            long rv = Api.saHpiDrtEntryGet( sid, eid, out next_eid, out drte );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return drte;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }

    /***********************************************************
     * Iterate over DAT
     **********************************************************/
    public static IEnumerable<SaHpiAlarmT> Dat( long sid,
                                                long severity = HpiConst.SAHPI_ALL_SEVERITIES,
                                                long unacknowledged_only = HpiConst.SAHPI_FALSE )
    {
        long id = HpiConst.SAHPI_FIRST_ENTRY;
        long timestamp = 0;
        do {
            SaHpiAlarmT a = NewAlarm( id, timestamp );
            long rv = Api.saHpiAlarmGetNext( sid, severity, unacknowledged_only, ref a );
            if ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            id = a.AlarmId;
            timestamp = a.Timestamp;
            yield return a;
        } while ( id != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over Event Log
     **********************************************************/
    public class EventLogEntryEx
    {
        public SaHpiEventLogEntryT EventLogEntry;
        public SaHpiRdrT           Rdr;
        public SaHpiRptEntryT      RptEntry;
    };

    public static IEnumerable<EventLogEntryEx> EventLogEntries( long sid,
                                                                long rid,
                                                                bool readforward = true )
    {
        long eid = readforward ? HpiConst.SAHPI_OLDEST_ENTRY : HpiConst.SAHPI_NEWEST_ENTRY;
        long prev_eid, next_eid;
        do {
            EventLogEntryEx ex = new EventLogEntryEx();
            long rv = Api.saHpiEventLogEntryGet( sid,
                                                 rid,
                                                 eid,
                                                 out prev_eid,
                                                 out next_eid,
                                                 out ex.EventLogEntry,
                                                 out ex.Rdr,
                                                 out ex.RptEntry );
            if ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = readforward ? next_eid : prev_eid;
            yield return ex;
        } while ( eid != HpiConst.SAHPI_NO_MORE_ENTRIES );
    }


    /***********************************************************
     * Iterate over entity resource ids
     **********************************************************/
    public static IEnumerable<long> EntityResourceIds( long sid, SaHpiEntityPathT ep )
    {
        long id = HpiConst.SAHPI_FIRST_ENTRY;
        do {
            long rid;
            long instrid;
            long rptcnt;
            long rv = Api.saHpiGetIdByEntityPath( sid,
                                                  ep,
                                                  HpiConst.SAHPI_NO_RECORD,
                                                  ref id,
                                                  out rid,
                                                  out instrid,
                                                  out rptcnt );
            if ( ( id == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            yield return rid;
        } while ( id != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over entity instrument ids
     **********************************************************/
    public class ResourceIdInstrumentId
    {
        public long ResourceId;
        public long InstrumentId;
    };

    public static IEnumerable<ResourceIdInstrumentId> EntityInstrumentIds( long sid,
                                                                           SaHpiEntityPathT ep,
                                                                           long rdrtype )
    {
        long id = HpiConst.SAHPI_FIRST_ENTRY;
        do {
            ResourceIdInstrumentId ri = new ResourceIdInstrumentId();
            long rptcnt;
            long rv = Api.saHpiGetIdByEntityPath( sid,
                                                  ep,
                                                  rdrtype,
                                                  ref id,
                                                  out ri.ResourceId,
                                                  out ri.InstrumentId,
                                                  out rptcnt );
            if ( ( id == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            yield return ri;
        } while ( id != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over child entities
     **********************************************************/
    public static IEnumerable<SaHpiEntityPathT> ChildEntities( long sid, SaHpiEntityPathT parent_ep )
    {
        long id = HpiConst.SAHPI_FIRST_ENTRY;
        do {
            long rptcnt;
            SaHpiEntityPathT child_ep;
            long rv = Api.saHpiGetChildEntityPath( sid, parent_ep, ref id, out child_ep, out rptcnt );
            if ( ( id == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            yield return child_ep;
        } while ( id != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over RPT
     **********************************************************/
    public static IEnumerable<SaHpiRptEntryT> Rpt( long sid )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        do {
            SaHpiRptEntryT rpte;
            long rv = Api.saHpiRptEntryGet( sid, eid, out next_eid, out rpte );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return rpte;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over RDRs
     **********************************************************/
    public static IEnumerable<SaHpiRdrT> Rdrs( long sid, long rid )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        do {
            SaHpiRdrT rdr;
            long rv = Api.saHpiRdrGet( sid, rid, eid, out next_eid, out rdr );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return rdr;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over Idr Areas
     **********************************************************/
    public static IEnumerable<SaHpiIdrAreaHeaderT> IdrAreaHeaders( long sid,
                                                                   long rid,
                                                                   long idrid,
                                                                   long atype = HpiConst.SAHPI_IDR_AREATYPE_UNSPECIFIED )
    {
        long aid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_aid;
        do {
            SaHpiIdrAreaHeaderT ahdr;
            long rv = Api.saHpiIdrAreaHeaderGet( sid, rid, idrid, atype, aid, out next_aid, out ahdr );
            if ( ( aid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            aid = next_aid;
            yield return ahdr;
        } while ( aid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over Area Fields
     **********************************************************/
    public static IEnumerable<SaHpiIdrFieldT> IdrAreaFields( long sid,
                                                             long rid,
                                                             long idrid,
                                                             long aid,
                                                             long ftype = HpiConst.SAHPI_IDR_FIELDTYPE_UNSPECIFIED )
    {
        long fid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_fid;
        do {
            SaHpiIdrFieldT f;
            long rv = Api.saHpiIdrFieldGet( sid, rid, idrid, aid, ftype, fid, out next_fid, out f );
            if ( ( aid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            fid = next_fid;
            yield return f;
        } while ( aid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over annuncuator announcements
     **********************************************************/
    public static IEnumerable<SaHpiAnnouncementT> Announcements( long sid,
                                                                 long rid,
                                                                 long annnum,
                                                                 long severity = HpiConst.SAHPI_ALL_SEVERITIES,
                                                                 long unacknowledged_only = HpiConst.SAHPI_FALSE )
    {
        long id = HpiConst.SAHPI_FIRST_ENTRY;
        long timestamp = 0;
        do {
            SaHpiAnnouncementT a = NewAnnouncement( id, timestamp );
            long rv = Api.saHpiAnnunciatorGetNext( sid,
                                                   rid,
                                                   annnum,
                                                   severity,
                                                   unacknowledged_only,
                                                   ref a );
            if ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            id = a.EntryId;
            timestamp = a.Timestamp;
            yield return a;
        } while ( id != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over FUMI Source Components
     **********************************************************/
    public static IEnumerable<SaHpiFumiComponentInfoT> FumiSourceComponents( long sid,
                                                                             long rid,
                                                                             long fuminum,
                                                                             long banknum )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        do {
            SaHpiFumiComponentInfoT info;
            long rv = Api.saHpiFumiSourceComponentInfoGet( sid,
                                                           rid,
                                                           fuminum,
                                                           banknum,
                                                           eid,
                                                           out next_eid,
                                                           out info );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return info;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over FUMI Target Components
     **********************************************************/
    public static IEnumerable<SaHpiFumiComponentInfoT> FumiTargetComponents( long sid,
                                                                             long rid,
                                                                             long fuminum,
                                                                             long banknum )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        do {
            SaHpiFumiComponentInfoT info;
            long rv = Api.saHpiFumiTargetComponentInfoGet( sid,
                                                           rid,
                                                           fuminum,
                                                           banknum,
                                                           eid,
                                                           out next_eid,
                                                           out info );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return info;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }


    /***********************************************************
     * Iterate over FUMI Logical Target Components
     **********************************************************/
    public static IEnumerable<SaHpiFumiLogicalComponentInfoT> FumiLogicalTargetComponents( long sid,
                                                                                           long rid,
                                                                                           long fuminum )
    {
        long eid = HpiConst.SAHPI_FIRST_ENTRY;
        long next_eid;
        do {
            SaHpiFumiLogicalComponentInfoT info;
            long rv = Api.saHpiFumiLogicalTargetComponentInfoGet( sid,
                                                                  rid,
                                                                  fuminum,
                                                                  eid,
                                                                  out next_eid,
                                                                  out info );
            if ( ( eid == HpiConst.SAHPI_FIRST_ENTRY ) && ( rv == HpiConst.SA_ERR_HPI_NOT_PRESENT ) ) {
                yield break;
            }
            if ( rv != HpiConst.SA_OK ) {
                yield break;
            }
            eid = next_eid;
            yield return info;
        } while ( eid != HpiConst.SAHPI_LAST_ENTRY );
    }


}; // class HpiIterators


}; // namespace openhpi
}; // namespace org

