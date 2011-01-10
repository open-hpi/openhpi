/*      -*- c++ -*-
 *
 * Copyright (c) 2010 by Pigeon Point Systems.
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

#include "hpi.h"
#include "hpi_xml_writer.h"


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

/*
static SaHpiUint32T GetRdrUpdateCounter( SaHpiSessionIdT rid )
{
}
*/

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

static bool FetchResources( SaHpiSessionIdT sid,
                            SaHpiDomainInfoT& di,
                            std::list<SaHpiRptEntryT>& rpt )
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
        SaHpiRptEntryT rpte;
        id = SAHPI_FIRST_ENTRY;
        while ( id != SAHPI_LAST_ENTRY ) {
            SaErrorT rv;
            rv = saHpiRptEntryGet( sid, id, &next_id, &rpte );
            if ( rv == SA_ERR_HPI_NOT_PRESENT ) {
                break;
            }
            if ( rv != SA_OK ) {
                rpt.clear();
                CRIT( "saHpiRptEntryGet returned %s", oh_lookup_error( rv ) );
                return false;
            }
            rpt.push_back( rpte );
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

/*
void cHpiSubProviderDRT::GetEntries( std::deque<HpiEntry>& entries ) const
{
    entries.clear();

    SaErrorT rv;
    SaHpiEntryIdT id, next_id;
    SaHpiDrtEntryT drte;

    bool first = true;
    id = SAHPI_FIRST_ENTRY;
    while ( id != SAHPI_LAST_ENTRY ) {
        rv = saHpiDrtEntryGet( m_ctx.session_id, id, &next_id, &drte );
        if ( first && ( rv == SA_ERR_HPI_NOT_PRESENT ) ) {
            break;
        }

        if ( rv != SA_OK ) {
            return;
        }

        std::wstring name;
        MakeNameForDomain( drte.DomainId, name );
        entries.push_back( HpiEntry( eHpiEntryResource, drte.EntryId, name ) );

        first = false;
        id = next_id;
    }
}

SaHpiUint32T cHandler::GetRptUpdateCounter() const
{
    SaErrorT rv;
    SaHpiDomainInfoT di;
    rv = Abi()->saHpiDomainInfoGet( m_sid, &di );
    if ( rv == SA_OK ) {
        return di.RptUpdateCount;
    } else {
        CRIT( "saHpiDomainInfoGet failed with rv = %d", rv );
        return 0;
    }
}

SaHpiUint32T cHandler::GetRdrUpdateCounter( SaHpiResourceIdT slave_rid ) const
{
    SaErrorT rv;
    SaHpiUint32T cnt;
    rv = Abi()->saHpiRdrUpdateCountGet( m_sid, slave_rid, &cnt );
    if ( rv == SA_OK ) {
        return cnt;
    } else {
        CRIT( "saHpiRdrUpdateCountGet failed with rv = %d", rv );
        return 0;
    }
}

bool cHandler::FetchRptAndRdrs( std::queue<struct oh_event *>& events ) const
{
    for ( unsigned int attempt = 0; attempt < MaxFetchAttempts; ++attempt ) {
        while( !events.empty() ) {
            oh_event_free( events.front(), 0 );
            events.pop();
        }

        SaHpiUint32T cnt = GetRptUpdateCounter();
        SaHpiEntryIdT id, next_id;
        for ( id = SAHPI_FIRST_ENTRY; id != SAHPI_LAST_ENTRY; id = next_id ) {
            struct oh_event * e = g_new0( struct oh_event, 1 );
            SaErrorT rv = Abi()->saHpiRptEntryGet( m_sid,
                                                   id,
                                                   &next_id,
                                                   &e->resource );
            if ( rv != SA_OK ) {
                CRIT( "saHpiRptEntryGet failed with rv = %d", rv );
                break;
            }
            e->event.Source = e->resource.ResourceId;
            bool rc = FetchRdrs( e );
            if ( !rc ) {
                break;
            }
            events.push( e );
        }
        if ( cnt == GetRptUpdateCounter() ) {
            return true;
        }
    }

    while( !events.empty() ) {
        oh_event_free( events.front(), 0 );
        events.pop();

    while( !events.empty() ) {
        oh_event_free( events.front(), 0 );
        events.pop();
    }

    return false;
}

bool cHandler::FetchRdrs( struct oh_event * e ) const
{
    SaHpiResourceIdT slave_rid = e->event.Source;

    for ( unsigned int attempt = 0; attempt < MaxFetchAttempts; ++attempt ) {
        oh_event_free( e, 1 );
        e->rdrs = 0;

        SaHpiUint32T cnt = GetRdrUpdateCounter( slave_rid );
        SaHpiEntryIdT id, next_id;
        for ( id = SAHPI_FIRST_ENTRY; id != SAHPI_LAST_ENTRY; id = next_id ) {
            SaHpiRdrT * rdr = g_new0( SaHpiRdrT, 1 );
            SaErrorT rv = Abi()->saHpiRdrGet( m_sid,
                                              slave_rid,
                                              id,
                                              &next_id,
                                              rdr );
            if ( rv != SA_OK ) {
                g_free( rdr );
                CRIT( "saHpiRdrGet failed with rv = %d", rv );
                break;
            }
            e->rdrs = g_slist_append( e->rdrs, rdr );
        }

        if ( cnt == GetRdrUpdateCounter( slave_rid ) ) {
            return true;
        }
    }

    oh_event_free( e, 1 );
    e->rdrs = 0;

    return false;
}

*/


/***************************************************
 * class cHpi
 ***************************************************/
cHpi::cHpi( SaHpiDomainIdT did )
    : m_initialized( false ),
      m_opened( false ),
      m_did( did ),
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
    rv = saHpiSessionOpen( m_did, &m_sid, 0 );
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
    writer.BeginDrtNode( drt_di );
    while ( !drt.empty() ) {
        writer.DrtEntryNode( drt.front() );
        drt.pop_front();
    }
    writer.EndDrtNode();

    SaHpiDomainInfoT rpt_di;
    std::list<SaHpiRptEntryT> rpt;
    rc = FetchResources( m_sid, rpt_di, rpt );
    if ( !rc ) {
        return false;
    }
    writer.BeginRptNode( rpt_di );
    while ( !rpt.empty() ) {
        writer.BeginResourceNode( rpt.front() );
        writer.EndResourceNode();
        rpt.pop_front();
    }
    writer.EndRptNode();

    SaHpiDomainInfoT dat_di;
    std::list<SaHpiAlarmT> dat;
    rc = FetchDat( m_sid, dat_di, dat );
    if ( !rc ) {
        return false;
    }
    writer.BeginDatNode( dat_di );
    while ( !dat.empty() ) {
        writer.AlarmNode( dat.front() );
        dat.pop_front();
    }
    writer.EndDatNode();

    writer.EndDomainNode();

    writer.End();

    return true;
}

