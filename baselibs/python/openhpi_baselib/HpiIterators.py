
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#

from openhpi_baselib.HpiDataTypes import *
from openhpi_baselib.Hpi import *
from openhpi_baselib import HpiUtil


#**********************************************************
# Helper functions
# TODO - move to HpiUtils?
#**********************************************************
def new_cond():
    c             = SaHpiConditionT()
    c.Type        = SAHPI_STATUS_COND_TYPE_USER
    c.Entity      = HpiUtil.makeUnspecifiedSaHpiEntityPathT()
    c.DomainId    = SAHPI_UNSPECIFIED_DOMAIN_ID
    c.ResourceId  = SAHPI_UNSPECIFIED_RESOURCE_ID
    c.SensorNum   = SAHPI_ENTRY_UNSPECIFIED
    c.EventState  = SAHPI_ES_UNSPECIFIED
    c.Name        = SaHpiNameT()
    c.Name.Length = 0
    c.Name.Value  = "".ljust( SA_HPI_MAX_NAME_LENGTH, chr(0) )
    c.Mid         = SAHPI_MANUFACTURER_ID_UNSPECIFIED
    c.Data        = HpiUtil.toSaHpiTextBufferT( "" )
    return c

def new_alarm( id, timestamp ):
    a              = SaHpiAlarmT()
    a.AlarmId      = id
    a.Timestamp    = timestamp
    a.Severity     = SAHPI_ALL_SEVERITIES
    a.Acknowledged = SAHPI_FALSE
    a.AlarmCond    = new_cond()
    return a

def new_announcement( id, timestamp ):
    a              = SaHpiAnnouncementT()
    a.EntryId      = id
    a.Timestamp    = timestamp
    a.AddedByUser  = SAHPI_TRUE
    a.Severity     = SAHPI_ALL_SEVERITIES
    a.Acknowledged = SAHPI_FALSE
    a.StatusCond   = new_cond()
    return a


#**********************************************************
# HPI Utility Functions: Iterators
#**********************************************************

#**********************************************************
# Iterate over DRT
# Provides SaHpiDrtEntryT
#**********************************************************
def Drt( sid ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, drte ) = saHpiDrtEntryGet( sid, id )
        if rv != SA_OK:
            break
        yield drte

#**********************************************************
# Iterate over DAT
# Provides SaHpiAlarmT
#**********************************************************
def Dat( sid, severity = SAHPI_ALL_SEVERITIES, unacknowledged_only = SAHPI_FALSE ):
    id = SAHPI_FIRST_ENTRY
    timestamp = 0L
    while id != SAHPI_LAST_ENTRY:
        a = new_alarm( id, timestamp )
        ( rv, a ) = saHpiAlarmGetNext( sid, severity, unacknowledged_only, a )
        if rv != SA_OK:
            break
        id        = a.AlarmId
        timestamp = a.Timestamp
        yield a

#**********************************************************
# Iterate over Event Log
# Provides ( SaHpiEventLogEntryT, SaHpiRdrT, SaHpiRptEntryT ) tuple
#**********************************************************
def EventLogEntries( sid, rid, readforward = True ):
    id = SAHPI_OLDEST_ENTRY if readforward else SAHPI_NEWEST_ENTRY
    while id != SAHPI_NO_MORE_ENTRIES:
        ( rv, previd, nextid, entry, rdr, rpte ) = saHpiEventLogEntryGet( sid, rid, id )
        if rv != SA_OK:
            break
        id = nextid if readforward else previd
        yield ( entry, rdr, rpte )

#**********************************************************
# Iterate over entity resource ids
# Provides SaHpiResourceIdT
#**********************************************************
def EntityResourceIds( sid, ep ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, rid, instrid, rptcnt ) = saHpiGetIdByEntityPath( sid, ep, SAHPI_NO_RECORD, id )
        if rv != SA_OK:
            break
        yield rid

#**********************************************************
# Iterate over entity instrument ids
# Provides ( SaHpiResourceIdT, SaHpiInstrumentIdT ) tuple
#**********************************************************
def EntityInstrumentIds( sid, ep, rdrtype ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, rid, instrid, rptcnt ) = saHpiGetIdByEntityPath( sid, ep, rdrtype, id )
        if rv != SA_OK:
            break
        yield ( rid, instrid )

#**********************************************************
# Iterate over child entities
# Provides SaHpiEntityPathT
#**********************************************************
def ChildEntities( sid, parent_ep ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, child_ep, rptcnt ) = saHpiGetChildEntityPath( sid, parent_ep, id )
        if rv != SA_OK:
            break
        yield child_ep

#**********************************************************
# Iterate over RPT
# Provides SaHpiRptEntryT
#**********************************************************
def Rpt( sid ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, rpte ) = saHpiRptEntryGet( sid, id )
        if rv != SA_OK:
            break
        yield rpte

#**********************************************************
# Iterate over RDRs
# Provides SaHpiRdrT
#**********************************************************
def Rdrs( sid, rid ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, rdr ) = saHpiRdrGet( sid, rid, id )
        if rv != SA_OK:
            break
        yield rdr

#**********************************************************
# Iterate over Idr Areas
# Provides SaHpiIdrAreaHeaderT
#**********************************************************
def IdrAreaHeaders( sid, rid, iid, atype = SAHPI_IDR_AREATYPE_UNSPECIFIED ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, ahdr ) = saHpiIdrAreaHeaderGet( sid, rid, iid, atype, id )
        if rv != SA_OK:
            break
        yield ahdr

#**********************************************************
# Iterate over Idr Area Fields
# Provides SaHpiIdrFieldT
#**********************************************************
def IdrAreaFields( sid, rid, iid, aid, ftype = SAHPI_IDR_FIELDTYPE_UNSPECIFIED ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, field ) = saHpiIdrFieldGet( sid, rid, iid, aid, ftype, id )
        if rv != SA_OK:
            break
        yield field

#**********************************************************
# Iterate over annuncuator announcements
# Provides SaHpiAnnouncementT
#**********************************************************
def Announcements( sid, rid, annnum, severity = SAHPI_ALL_SEVERITIES, unacknowledged_only = SAHPI_FALSE ):
    id = SAHPI_FIRST_ENTRY
    timestamp = 0L
    while id != SAHPI_LAST_ENTRY:
        a = new_announcement( id, timestamp )
        ( rv, a ) = saHpiAnnunciatorGetNext( sid, rid, annnum, severity, unacknowledged_only, a )
        if rv != SA_OK:
            break
        id        = a.EntryId
        timestamp = a.Timestamp
        yield a

#**********************************************************
# Iterate over FUMI Source Components
# Provides SaHpiFumiComponentInfoT
#**********************************************************
def FumiSourceComponents( sid, rid, fuminum, banknum ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, info ) = saHpiFumiSourceComponentInfoGet( sid, rid, fuminum, banknum, id )
        if rv != SA_OK:
            break
        yield info

#**********************************************************
# Iterate over FUMI Target Components
# Provides SaHpiFumiComponentInfoT
#**********************************************************
def FumiTargetComponents( sid, rid, fuminum, banknum ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, info ) = saHpiFumiTargetComponentInfoGet( sid, rid, fuminum, banknum, id )
        if rv != SA_OK:
            break
        yield info

#**********************************************************
# Iterate over FUMI Logical Target Components
# Provides SaHpiFumiLogicalComponentInfoT
#**********************************************************
def FumiLogicalTargetComponents( sid, rid, fuminum ):
    id = SAHPI_FIRST_ENTRY
    while id != SAHPI_LAST_ENTRY:
        ( rv, id, info ) = saHpiFumiLogicalTargetComponentInfoGet( sid, rid, fuminum, id )
        if rv != SA_OK:
            break
        yield info

