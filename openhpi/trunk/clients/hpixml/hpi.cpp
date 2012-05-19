/*      -*- c++ -*-
 *
 * Copyright (c) 2011 by Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *
 */

#include <list>

#include <oh_error.h>
#include <oh_utils.h>
#include <oh_clients.h>

#include "hpi.h"
#include "hpi_xml_writer.h"


/***************************************************
 * Data Types
 ***************************************************/
struct LogEntry
{
    SaHpiEventLogEntryT entry;
    SaHpiRdrT rdr;
    SaHpiRptEntryT rpte;
};

struct Log
{
    SaHpiEventLogInfoT info;
    SaHpiEventLogCapabilitiesT caps;
    std::list<LogEntry> entries;
};

struct Resource
{
    SaHpiRptEntryT rpte;
    SaHpiUint32T rdr_update_count;
    std::list<SaHpiRdrT> instruments;
    Log log;
};


/***************************************************
 * Helper Functions
 ***************************************************/
static bool GetDomainInfo( SaHpiSessionIdT sid, SaHpiDomainInfoT& di )
{
    SaErrorT rv;
    rv = saHpiDomainInfoGet( sid, &di );
    if ( rv != SA_OK ) {
        CRIT( "saHpiDomainInfoGet returned %s", oh_lookup_error( rv ) );
    }
    return ( rv == SA_OK );
}

static bool FetchDrt( SaHpiSessionIdT sid,
                      SaHpiDomainInfoT& di,
                      std::list<SaHpiDrtEntryT>& drt )
{
    SaHpiDomainInfoT di2;

    do {
        drt.clear();

        bool rc;

        rc = GetDomainInfo( sid, di );
        if ( !rc ) {
            return false;
        }

        SaHpiEntryIdT id, next_id;
        SaHpiDrtEntryT drte;
        id = SAHPI_FIRST_ENTRY;
        while ( id != SAHPI_LAST_ENTRY ) {
            SaErrorT rv;
            rv = saHpiDrtEntryGet( sid, id, &next_id, &drte );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                drt.clear();
                CRIT( "saHpiDrtEntryGet returned %s", oh_lookup_error( rv ) );
                return false;
            }
            drt.push_back( drte );
            id = next_id;
        }

        rc = GetDomainInfo( sid, di2 );
        if ( !rc ) {
            drt.clear();
            return false;
        }

    } while ( di.DrtUpdateCount != di2.DrtUpdateCount );

    return true;
}

static bool FetchLog( SaHpiSessionIdT sid,
                      SaHpiResourceIdT rid,
                      Log& log )
{
    SaErrorT rv;

    log.entries.clear();

    rv = saHpiEventLogInfoGet( sid, rid, &log.info );
    if ( rv != SA_OK ) {
        CRIT( "saHpiEventLogInfoGet returned %s", oh_lookup_error( rv ) );
        return false;
    }
    rv = saHpiEventLogCapabilitiesGet( sid, rid, &log.caps );
    if ( rv != SA_OK ) {
        CRIT( "saHpiEventLogCapabilitiesGet returned %s", oh_lookup_error( rv ) );
        return false;
    }

    SaHpiEventLogEntryIdT prev_id, id, next_id;
    LogEntry le;
    id = SAHPI_OLDEST_ENTRY;
    while ( id != SAHPI_NO_MORE_ENTRIES ) {
        rv = saHpiEventLogEntryGet( sid,
                                    rid,
                                    id,
                                    &prev_id,
                                    &next_id,
                                    &le.entry,
                                    &le.rdr,
                                    &le.rpte );
        if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
            break;
        }
        if ( rv != SA_OK ) {
            log.entries.clear();
            CRIT( "saHpiRptEntryGet returned %s", oh_lookup_error( rv ) );
            return false;
        }
        log.entries.push_back( le );
        id = next_id;
    }

    return true;
}

static SaHpiUint32T GetRdrUpdateCounter( SaHpiSessionIdT sid, SaHpiResourceIdT rid )
{
    SaHpiUint32T cnt;
    SaErrorT rv = saHpiRdrUpdateCountGet( sid, rid, &cnt );
    if ( rv != SA_OK ) {
        CRIT( "saHpiRdrUpdateCountGet %s", oh_lookup_error( rv ) );
        return 0;
    }
    return cnt;
}

static bool FetchInstruments( SaHpiSessionIdT sid,
                              Resource& resource )

{
    SaErrorT rv;
    SaHpiResourceIdT rid = resource.rpte.ResourceId;
    SaHpiUint32T& cnt = resource.rdr_update_count;
    SaHpiUint32T cnt2;

    do {
        resource.instruments.clear();
        cnt = GetRdrUpdateCounter( sid, rid );

        SaHpiEntryIdT id, next_id;
        SaHpiRdrT rdr;
        id = SAHPI_FIRST_ENTRY;
        while ( id != SAHPI_LAST_ENTRY ) {
            rv = saHpiRdrGet( sid, rid, id, &next_id, &rdr );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                resource.instruments.clear();
                CRIT( "saHpiRdrGet returned %s", oh_lookup_error( rv ) );
                return false;
            }
            resource.instruments.push_back( rdr );
            id = next_id;
        }

        cnt2 = GetRdrUpdateCounter( sid, rid );

    } while ( cnt != cnt2 );

    return true;
}

static bool FetchResources( SaHpiSessionIdT sid,
                            SaHpiDomainInfoT& di,
                            std::list<Resource>& rpt )
{
    SaHpiDomainInfoT di2;

    do {
        rpt.clear();

        bool rc;

        rc = GetDomainInfo( sid, di );
        if ( !rc ) {
            return false;
        }

        SaHpiEntryIdT id, next_id;
        Resource resource;
        id = SAHPI_FIRST_ENTRY;
        while ( id != SAHPI_LAST_ENTRY ) {
            SaErrorT rv;
            rv = saHpiRptEntryGet( sid, id, &next_id, &resource.rpte );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                rpt.clear();
                CRIT( "saHpiRptEntryGet returned %s", oh_lookup_error( rv ) );
                return false;
            }

            resource.instruments.clear();
            resource.log.entries.clear();

            if ( resource.rpte.ResourceFailed == SAHPI_FALSE ) {
                if ( resource.rpte.ResourceCapabilities & SAHPI_CAPABILITY_RDR ) {
                    rc = FetchInstruments( sid, resource );
                    if ( !rc ) {
                        break;
                    }
                }
                if ( resource.rpte.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG ) {
                    rc = FetchLog( sid, resource.rpte.ResourceId, resource.log );
                    if ( !rc ) {
                        break;
                    }
                }
            }

            rpt.push_back( resource );
            id = next_id;
        }

        rc = GetDomainInfo( sid, di2 );
        if ( !rc ) {
            rpt.clear();
            return false;
        }

    } while ( di.RptUpdateCount != di2.RptUpdateCount );

    return true;
}

static bool FetchDat( SaHpiSessionIdT sid,
                      SaHpiDomainInfoT& di,
                      std::list<SaHpiAlarmT>& dat )
{
    SaHpiDomainInfoT di2;

    do {
        dat.clear();

        bool rc;

        rc = GetDomainInfo( sid, di );
        if ( !rc ) {
            return false;
        }

        SaHpiAlarmT a;
        a.AlarmId = SAHPI_FIRST_ENTRY;
        while ( true ) {
            SaErrorT rv;
            rv = saHpiAlarmGetNext( sid, SAHPI_ALL_SEVERITIES, SAHPI_FALSE, &a );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                dat.clear();
                CRIT( "saHpiDatEntryGet returned %s", oh_lookup_error( rv ) );
                return false;
            }
            dat.push_back( a );
        }

        rc = GetDomainInfo( sid, di2 );
        if ( !rc ) {
            dat.clear();
            return false;
        }

    } while ( di.DatUpdateCount != di2.DatUpdateCount );

    return true;
}

static void DumpDrt( cHpiXmlWriter& writer,
                     SaHpiDomainInfoT& di,
                     std::list<SaHpiDrtEntryT>& drt )
{
    writer.BeginDrtNode( di );
    while ( !drt.empty() ) {
        writer.DrtEntryNode( drt.front() );
        drt.pop_front();
    }
    writer.EndDrtNode();
}

static void DumpLog( cHpiXmlWriter& writer,
                     SaHpiResourceIdT rid,
                     Log& log )
{
    writer.BeginEventLogNode( rid, log.info, log.caps );

    while ( !log.entries.empty() ) {
        const LogEntry& le( log.entries.front() );
        writer.LogEntryNode( le.entry, le.rdr, le.rpte );
        log.entries.pop_front();
    }

    writer.EndEventLogNode( rid );
}

static void DumpInstruments( cHpiXmlWriter& writer,
                             std::list<SaHpiRdrT>& instruments )
{
    while ( !instruments.empty() ) {
        writer.BeginInstrumentNode( instruments.front() );
        writer.EndInstrumentNode();
        instruments.pop_front();
    }
}

static void DumpResources( cHpiXmlWriter& writer,
                           SaHpiDomainInfoT& di,
                           std::list<Resource>& rpt )
{
    writer.BeginRptNode( di );
    while ( !rpt.empty() ) {
        Resource& resource = rpt.front();
        writer.BeginResourceNode( resource.rpte, resource.rdr_update_count );

        if ( resource.rpte.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG ) {
            DumpLog( writer, resource.rpte.ResourceId, resource.log );
        }

        DumpInstruments( writer, resource.instruments );

        writer.EndResourceNode();
        rpt.pop_front();
    }
    writer.EndRptNode();
}

static void DumpDat( cHpiXmlWriter& writer,
                     SaHpiDomainInfoT& di,
                     std::list<SaHpiAlarmT> dat )
{
    writer.BeginDatNode( di );
    while ( !dat.empty() ) {
        writer.AlarmNode( dat.front() );
        dat.pop_front();
    }
    writer.EndDatNode();
}


/***************************************************
 * class cHpi
 ***************************************************/
cHpi::cHpi( oHpiCommonOptionsT copt )
    : m_initialized( false ),
      m_opened( false ),
      m_copt( copt ),
      m_sid( 0 )
{
    // empty
}

cHpi::~cHpi()
{
    Close();
}

bool cHpi::Open()
{
    if ( m_opened ) {
        return true;
    }

    SaErrorT rv;

    if ( !m_initialized ) {
        rv = saHpiInitialize( SAHPI_INTERFACE_VERSION, 0, 0, 0, 0 );
        if ( rv != SA_OK ) {
            CRIT( "saHpiInitialize returned %s", oh_lookup_error( rv ) );
            return false;
        }
        m_initialized = true;
    }

    rv = ohc_session_open_by_option ( &m_copt, &m_sid);
    if ( rv != SA_OK ) {
        CRIT( "saHpiSessionOpen returned %s", oh_lookup_error( rv ) ); 
        return false;
    }
    m_opened = true;
    rv = saHpiDiscover( m_sid );
    if ( rv != SA_OK ) {
        CRIT( "saHpiDiscover returned %s", oh_lookup_error( rv ) );
        return false;
    }

    return true;
}

void cHpi::Close()
{
    if ( m_opened ) {
        saHpiSessionClose( m_sid );
        m_sid = 0;
        m_opened = false;
    }
    if ( m_initialized ) {
        saHpiFinalize();
        m_initialized = false;
    }
}

bool cHpi::Dump( cHpiXmlWriter& writer )
{
    bool rc;

    writer.Begin();

    writer.VersionNode( saHpiVersionGet() );

    SaHpiDomainInfoT di;
    rc = GetDomainInfo( m_sid, di );
    if ( !rc ) {
        return false;
    }

    writer.BeginDomainNode( di );

    SaHpiDomainInfoT drt_di;
    std::list<SaHpiDrtEntryT> drt;
    rc = FetchDrt( m_sid, drt_di, drt );
    if ( !rc ) {
        return false;
    }
    DumpDrt( writer, drt_di, drt );

    SaHpiDomainInfoT rpt_di;
    std::list<Resource> rpt;
    rc = FetchResources( m_sid, rpt_di, rpt );
    if ( !rc ) {
        return false;
    }
    DumpResources( writer, rpt_di, rpt );

    Log del;
    rc = FetchLog( m_sid, SAHPI_UNSPECIFIED_RESOURCE_ID, del );
    if ( !rc ) {
        return false;
    }
    DumpLog( writer, SAHPI_UNSPECIFIED_RESOURCE_ID, del );

    SaHpiDomainInfoT dat_di;
    std::list<SaHpiAlarmT> dat;
    rc = FetchDat( m_sid, dat_di, dat );
    if ( !rc ) {
        return false;
    }
    DumpDat( writer, dat_di, dat );

    writer.EndDomainNode();

    writer.End();

    return true;
}

